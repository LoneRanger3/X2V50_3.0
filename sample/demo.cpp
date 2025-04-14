/******************************************************************************
  Copyright (C), 2020, xmsilicon Tech. Co., Ltd.
 ******************************************************************************
  File Name     : demo.c
  Author        : xmsilicon R&D
  Description   : Video output demo 
  History       :
******************************************************************************/
#include <sys/time.h>
#include <sys/times.h>
#include <deque>
#include <string>
#include <mutex>
#include <algorithm>
#include "lvgl.h"
#include "global_data.h"
#include "porting/lv_port_indev_template.h"
#include "lv_port_disp_template.h"
#include "xm_middleware_api.h"
#include "xm_middleware_network.h"
#include "xm_ia_td_interface.h"
#include "Log.h"
#include "TimeUtil.h"
#include "XMThread.h"
#include "global_page.h"
#include "cJson/cJSON.h"
#include "periphery/PeripheryManager.h"
#include "periphery/PeripheryThread.h"
#include "log/LogFileManager.h"
#include "mpp/MppMdl.h"
#include "network/HTTPSerMdl.h"
#include "CommDef.h"
#include "DemoDef.h"
#include "osd_user.h"
#include "gps.h"
extern "C" {
#include "cr_vo.h"
}
extern "C" int camera_set_mirror(int mirror);
extern "C" int camera_set_flip(int flip);
const int kMainPicWidth = MY_DISP_HOR_RES;
const int kMainPicHeight = MY_DISP_VER_RES;
#ifdef BOARD650V200
const int kChannelNum = 2;
#else
const int kChannelNum = 4;
#endif

bool g_bad_sdcard = false;
int g_sd_status = XM_SD_NOEXIST;
bool g_realplay = false;
int g_engineId = 0;
int g_connId = 0;
int g_frame_id = 0;
int g_channel = 0;
int64_t g_first_frame_time = 0;
bool g_app_connect = false;
bool g_wait_IFrame = false;
void OnMWEventCallBack(long handle, XMEventType event_type, const char* msg, int param, int64_t user, char* out_param);

void OnNetworkEvent(int engineId, int connId, XMIPEventType type, char *data, int len)
{
	if (type == XM_EVENT_DISCONNECT) {
		XMLogW("OnNetworkEvent");
		g_realplay = false;
	}
	XMLogW("after OnNetworkEvent, type=%d", (int)type);
}

void OnNetworkMsg(int engineId, int connId, uint8_t type, char* msg, char* connType, int contentLen) {
	XMLogW("respMsg,engineId=%d,connId=%d,msg=%s,connType=%s", engineId, connId, msg, connType);
	if(g_app_connect==false){
      XM_Middleware_Periphery_Notify(XM_EVENT_IN_VIDEO, "", 0);
	 // XM_Middleware_Smart_EnableKeyword(0);
	}
	g_app_connect = true;
	OnMWEventCallBack(0, XM_EVENT_NETWORK_MSG, "", 0, 0, NULL);
	HTTPSerMdl::Instance()->respMsg(engineId, connId, type, msg, connType, contentLen);
}

void closeThread(int connId)
{
	HTTPSerMdl::Instance()->CloseThread(connId);
}

void lv_ignore_signal_handler(int sig)
{
	g_realplay = false;
	XMLogW("lv_ignore_signal_handler");
}

void lv_ignore_signals()
{
	signal(SIGPIPE, lv_ignore_signal_handler);
	signal(SIGTERM, lv_ignore_signal_handler);
	signal(SIGHUP, lv_ignore_signal_handler);
}

void run_project()
{
	XMLogI("====================Start Run UI=====================");
	lv_obj_set_style_bg_opa(lv_scr_act(), LV_OPA_0, 0);
	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(0, 0, 0), 0);
	lv_img_cache_set_size(25);

	GlobalPage::Instance()->page_main()->CfgInit();
	GlobalPage::Instance()->page_main()->OpenPage();
	
	if (GlobalPage::Instance()->page_main()->connect_computer_) {
		XMLogI("Usb connect computer!");
		XM_Middleware_Network_SetDataCallback(OnNetworkMsg);
		//XM_Middleware_Network_SetEventCallback(OnNetworkEvent);	
		GlobalPage::Instance()->page_usb()->OpenPage();
	}
}

//设置编码参数
int SetEncodeParam(int channel, PAYLOAD_TYPE_E video_codec_type, int width, int height, int frame_rate, int bit_rate)
{
	XM_MW_Media_Info media_info;
	media_info.video_codec = video_codec_type == PT_H265 ? PT_H265 : PT_H264;
	media_info.width = width;
	media_info.height = height;
	media_info.bit_rate = bit_rate;
	media_info.frame_rate = frame_rate;		
	media_info.audio_codec = PT_PCMA;
	media_info.audio_channel = 1;
	media_info.bit_per_sample = 16;
	media_info.samples_per_sec = 16000;
	MppMdl::Instance()->SetEncInfo(channel, 0, media_info);
	return 0;
}

struct MWEventInfo {
	long handle;
	XMEventType event_type;
	int param;
	std::string msg;
};

std::deque<MWEventInfo> g_mw_event_infos;
std::mutex g_event_mutex;

void PackParamInfo(const char* name, const char **items_array, int items_len, const int* index_array, cJSON* infoObject)
{
    cJSON* object = cJSON_CreateObject();
    cJSON* items = cJSON_CreateStringArray(items_array, items_len);
    cJSON* index = cJSON_CreateIntArray(index_array, items_len);
    if (nullptr != name) {
        cJSON_AddStringToObject(object, "name", name);
        cJSON_AddItemToObject(object, "items", items);
        cJSON_AddItemToObject(object, "index", index);
        cJSON_AddItemToArray(infoObject, object);
    } else {
        cJSON_AddItemToObject(object, "items", items);
        cJSON_AddItemToObject(object, "index", index);
        cJSON_AddItemToObject(infoObject, "info", object);
    }
}

int ProcessGetParam(char* res_str)
{
	cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON* info = cJSON_CreateArray();

	const char *mic_items_array[2] = {"on", "off"};
	const int mic_index_array[2] = {1, 0};
	PackParamInfo("mic", mic_items_array, 2, mic_index_array, info);
#if defined SUPPORT_4K
	const char* resolution_items_array[3] = { "1080P+1080P", "2K+1080P", "4K+1080P" };
	const int resolution_index_array[3] = { 1080, 1440, k4KHeight };
    PackParamInfo("rec_resolution", resolution_items_array, 3, resolution_index_array, info);
#elif defined SUPPORT_3K	
	const char* resolution_items_array[3] = { "1080P+1080P", "2K+1080P","3K+1080P"};
	const int resolution_index_array[3] = { 1080, 1440 ,k3KHeight}; 
	PackParamInfo("rec_resolution", resolution_items_array, 3, resolution_index_array, info);
#elif defined SUPPORT_2K	
	const char* resolution_items_array[2] = { "1080P+1080P", "2K+1080P"};
	const int resolution_index_array[2] = { 1080, 1440 }; 
	PackParamInfo("rec_resolution", resolution_items_array, 2, resolution_index_array, info);

#endif	
	
	
	const char* rec_dur_items_array[3] = { "1MIN", "2MIN", "3MIN" };
	const int rec_dur_index_array[3] = { 1, 2, 3 };
	PackParamInfo("rec_split_duration", rec_dur_items_array, 3, rec_dur_index_array, info);

	const char *osd_items_array[2] = {"on", "off"};
	const int osd_index_array[2] = {1, 0};
	PackParamInfo("osd", osd_items_array, 2, osd_index_array, info);

	const char* gsr_sensitivity_items_array[4] = { "off", "low", "middle", "high" };
	const int gsr_sensitivity_index_array[4] = { 0, 1, 2, 3 };
	PackParamInfo("gsr_sensitivity", gsr_sensitivity_items_array, 4, gsr_sensitivity_index_array, info);

	const char* screen_standby_items_array[4] = { "off", "1MIN", "3MIN", "5MIN" };
	const int screen_standby_index_array[4] = { 0, 1, 3, 5 };
	PackParamInfo("screen_standby", screen_standby_items_array, 4, screen_standby_index_array, info);

	const char* parking_monitor_items_array[2] = { "off", "on" };
	const int parking_monitor_index_array[2] = { 0, 1 };
	PackParamInfo("parking_monitor", parking_monitor_items_array, 2, parking_monitor_index_array, info);

	const char *rec_items_array[2] = {"on", "off"};
	const int rec_index_array[2] = {1, 0};
	PackParamInfo("rec", rec_items_array, 2, rec_index_array, info);

  #if VOICE_CONTROL_EN
	const char *voice_control_items_array[2] = {"on", "off"};
	const int voice_control_index_array[2] = {1, 0};
	PackParamInfo("voice_control", voice_control_items_array, 2, voice_control_index_array, info);
  #endif
  
	const char* keytone_items_array[2] = { "on", "off" };
	const int keytone_index_array[2] = { 1, 0 };
	PackParamInfo("key_tone", keytone_items_array, 2, keytone_index_array, info);
#if COMPACT_RECORD_EN
   //缩时录影帧率
    const char* timelapse_rate_items_array[4] = { "off", "1fps", "2fps", "5fps" };
	const int timelapse_rate_index_array[4] = { 0, 1, 2, 5 };
	PackParamInfo("timelapse_rate", timelapse_rate_items_array, 4, timelapse_rate_index_array, info);
	//缩时录影时长
	const char* park_record_time_items_array[4] = { "off", "8H", "16H", "24H" };
	const int park_record_time_index_array[4] = { 0, 8, 16, 24 };
	PackParamInfo("park_record_time", park_record_time_items_array, 4, park_record_time_index_array, info);
#endif
	cJSON_AddItemToObject(root_res, "info", info);
    char* http_body = cJSON_Print(root_res);
	cJSON_Delete(root_res);
	//http_body长度不要超过4K
	strcpy(res_str, http_body);
	free(http_body);
	return 0;
}

int ProcessGetDevAttr()
{
	WIFI_MAC_PARAM_S net_parm;
    memset(net_parm.ssid, 0, sizeof(net_parm.ssid));
    memset(net_parm.mac, 0, sizeof(net_parm.mac));
    XM_Middleware_WIFI_GetMacParam(&net_parm);
    std::string ssid = net_parm.ssid;
    std::string mac = net_parm.mac;
    mac.erase(remove_if(mac.begin(), mac.end(), [](char ch) {return ch == ':'; }), mac.end());

    cJSON* info = cJSON_CreateObject();
    cJSON_AddStringToObject(info, "uuid", mac.c_str());
    cJSON_AddStringToObject(info, "softver", GlobalData::Instance()->version().c_str());
    cJSON_AddStringToObject(info, "otaver", "v1.20230312.1");
    cJSON_AddStringToObject(info, "hwver", "v1.2");
    cJSON_AddStringToObject(info, "ssid", ssid.c_str());
    cJSON_AddStringToObject(info, "bssid", mac.c_str());
    if (!MppMdl::Instance()->AdLoss()) {
        cJSON_AddNumberToObject(info, "camnum", 2);
    }
    else {
        cJSON_AddNumberToObject(info, "camnum", 1);
    }
    
    cJSON_AddNumberToObject(info, "curcamid", 0);
    cJSON_AddNumberToObject(info, "wifireboot", 0);

    cJSON* root_res = cJSON_CreateObject();
    cJSON_AddNumberToObject(root_res, "result", 0);
    cJSON_AddItemToObject(root_res, "info", info);
    char* http_body = cJSON_Print(root_res);
    cJSON_Delete(root_res);
    XM_Middleware_Network_SendBuffer(g_engineId, g_connId, 0, http_body, strlen(http_body));
    free(http_body);
	return 0;
}

//中间件sdk事件回调
void OnMWEventCallBack(long handle, XMEventType event_type, const char* msg, int param, int64_t user, char* out_msg)
{
	//app来获取参数能力集，同步返回
	if (XM_EVENT_APP_GET_PARAM == event_type) {
		ProcessGetParam(out_msg);
		return;
	}

	//app来获取设备属性，可以异步处理，直接返回，等设备切换到预览模式再回复
	if (XM_EVENT_APP_GET_DEVATTR == event_type) { 
		ProcessGetDevAttr();
		return;
	}
	
	if (XM_EVENT_CATCH_PIC == event_type) {
		GlobalPage::Instance()->page_main()->CheckSDStatus();
		if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
			MppMdl::Instance()->GetPicJPEG(-1);
		}
		return;
	}

	if (GlobalPage::Instance()->page_usb()->usb_page_ != NULL
		|| GlobalPage::Instance()->page_usb()->user_selected_usb_mode_ != USB_MODE_NONE
		|| GlobalPage::Instance()->page_main()->connect_computer_) {
		if (event_type != XM_EVENT_DEV_ADD && event_type != XM_EVENT_DEV_REMOVE && event_type != XM_EVENT_VOLTAGE_LOW
			&& event_type != XM_EVENT_KEY_SHUTDOWN && event_type != XM_EVENT_USB_DISCONNECT && event_type != XM_EVENT_USB_CONNECT
			&& event_type != XM_EVENT_POWERKEY_SHORTPRESS) {
			return;
		}
	}

	//喇叭使能，要同步执行
	if (XM_EVENT_SPEAKER_ENABLE == event_type) {
		bool enable = param;
		PeripheryManager::Instance()->SpeakerEnable(enable);
		return;
	}
	else if (XM_EVENT_DECODE_ENABLE == event_type) {
		PeripheryManager::Instance()->DecodeEnable(param);
		return;
	}
#if GPS_EN	
    if (XM_EVENT_RECORDFILE_START == event_type) {
		XMLogI("XM_EVENT_RECORD_START, channel=%d", handle);
		if(handle==0){
		write_start=1;
		}else if(handle==1){
		write_startback=1;
		}
		return;
	}
	if (XM_EVENT_RECORDFILE_END == event_type) {
        XMLogI("XM_EVENT_RECORDFILE_END, channel=%d, file name=%s", handle, msg); //msg.c_str()
			if(handle==0){
				write_start=0;
				get_gps_data(msg);
			}else if(handle==1){
				write_startback=0;
				get_gps_back_data(msg);
			}	
		return;
    }
#endif
	MWEventInfo event_info;
	event_info.handle = handle;
	event_info.event_type = event_type;
	event_info.param = param;
	event_info.msg = msg;
	g_event_mutex.lock();
	g_mw_event_infos.push_back(event_info);
	g_event_mutex.unlock();

	if (event_type == XM_EVENT_KEY_SHUTDOWN) {
		XMLogI("XM_EVENT_KEY_SHUTDOWN power = %d", param);
	}

	if (event_type == XM_EVENT_USB_DISCONNECT) {
		XMLogI("XM_EVENT_USB_DISCONNECT, power = %d", param);
	}

	if (event_type >= XM_EVENT_KEYWORD_XIANSHIQIANLU && event_type <= XM_EVENT_KEYWORD_GUANBIREDIAN) {
		
		if (event_type == XM_EVENT_KEYWORD_WOYAOLUXIANG && GlobalData::Instance()->UI_mode_ != UIMode_Videotape) return;
		if (event_type == XM_EVENT_KEYWORD_DAKAIREDIAN
			&& (GlobalData::Instance()->UI_mode_ != UIMode_Videotape || GlobalPage::Instance()->page_main()->wifi_enable_
				|| !GlobalPage::Instance()->page_main()->wifi_prepared_ || g_app_connect==true)) return;

		if (event_type == XM_EVENT_KEYWORD_GUANBIREDIAN
			&& (GlobalData::Instance()->UI_mode_ != UIMode_Videotape || !GlobalPage::Instance()->page_main()->wifi_enable_
				|| !GlobalPage::Instance()->page_main()->wifi_prepared_|| g_app_connect==true)) return;

		if (event_type != XM_EVENT_KEYWORD_GUANBIPINGMU) {
			GlobalPage::Instance()->page_main()->OpenScreen();
		}

		#if 0
		if (event_type != XM_EVENT_KEYWORD_ZHUAPAIZHAOPIAN) {
			std::string sound_file = kAudioPath;
			sound_file += "haode_16k.pcm";
			MppMdl::Instance()->PlaySound(sound_file.c_str());
		}
		#endif
	}
}

void OnMWLogCallback(char* log_buf, int log_len, int64_t user)
{
	// LogFileManager::Instance()->Write(log_buf);
	LogFileManager::Instance()->get_log_data(log_buf,log_len,user);
}

#if OSD_SHOW_ADJUST
extern int osd_time_ofs_y;
#endif
int ProcessEvent(long handle, XMEventType event_type, const std::string& msg, int param) 
{
 	switch (event_type)
    {
	case XM_EVENT_STARTUP_SOUND_OVER:
		XMLogI("startup sound play over");
        break;
    case XM_EVENT_Play_Over:
        XMLogI("play over, handle=%ld", handle);
        break;
    case XM_EVENT_DEV_ADD:
		{
			XMLogI("SD ADD");
			if (g_sd_status == XM_SD_NORMAL) {
				XMLogW("SD card is detected to be inserted! g_sd_status = %d", g_sd_status);
				break;
			}
			
			g_sd_status = XM_Middleware_Storage_CheckSDCard(XM_STORAGE_SDCard_0);
			GlobalPage::Instance()->page_main()->SDCardImg();
			if (GlobalPage::Instance()->page_usb()->user_selected_usb_mode_ == USB_MODE_MASS_STORAGE) {
				GlobalPage::Instance()->page_usb()->SetUsbFunc(USB_MODE_MASS_STORAGE);
			}

			if(GlobalPage::Instance()->page_main()->Handtip_box_){
				
			    lv_obj_del(GlobalPage::Instance()->page_main()->Handtip_box_);
				GlobalPage::Instance()->page_main()->Handtip_box_ = NULL;
			}
			
		#if 1	
			if(g_sd_status == XM_SD_NORMAL){	
				int write_speed = 0;				
				int ret = XM_Middleware_Storage_GetSpeedState(XM_STORAGE_SDCard_0, &write_speed);				
				XMLogI("write_speed = %d", write_speed);				
				GlobalData::Instance()->SDCard_write_speed_ = write_speed;				
				if (ret < 0) {					
					g_sd_status = XM_SD_NEEDCHECKSPEED;				
					}			
				}			
			if(!GlobalPage::Instance()->page_set()->set_page_ 			
				&& (GlobalData::Instance()->UI_mode_ == UIMode_Videotape || GlobalData::Instance()->UI_mode_ == UIMode_Photograph) 			
				/*&& !GlobalPage::Instance()->page_main()->wifi_enable_*/&& !GlobalPage::Instance()->page_main()->connect_computer_			
				&& (g_sd_status == XM_SD_NEEDCHECKSPEED || g_sd_status == XM_SD_NEEDFORMAT || g_sd_status == XM_SD_NOTRW)){				
					if (!GlobalPage::Instance()->page_sys_set()->format_tip_win_) {					
						GlobalPage::Instance()->page_sys_set()->OpenFormatTipWin();				
						}
					}else{				
					GlobalPage::Instance()->page_main()->CheckSDStatus(false);						
			}
        #endif
			if (g_sd_status == XM_SD_NORMAL) {
				// GlobalPage::Instance()->page_main()->SDCardImg();
				if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape
					&& !lv_obj_has_flag(GlobalPage::Instance()->page_main()->main_page_, LV_OBJ_FLAG_HIDDEN)
					&& GlobalData::Instance()->SDCard_write_speed_ >= 1024
					&& GlobalPage::Instance()->page_usb()->usb_page_ == NULL
					&& GlobalPage::Instance()->page_usb()->user_selected_usb_mode_ == USB_MODE_NONE) {
					GlobalPage::Instance()->page_main()->StartRecord();
				}
				LogFileManager::Instance()->Enable(true);
				if (GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
					HTTPSerMdl::Instance()->tcpSdStatus(g_engineId, 0, GlobalData::Instance()->SDCard_write_speed_);
				}
				else {
					HTTPSerMdl::Instance()->tcpSdStatus(g_engineId, 11, GlobalData::Instance()->SDCard_write_speed_);
				}
			}
			else {
				int status = 0;
				if (XM_SD_NOEXIST == g_sd_status) {
					status = 2;
				}
				else if (XM_SD_ABNORMAL == g_sd_status) {
					status = 3;
				}
				else if (XM_SD_NEEDFORMAT == g_sd_status) {
					status = 1;
				}
				else if (XM_SD_NOTRW == g_sd_status) {
					status = 10;
				}
				else if (XM_SD_NEEDCHECKSPEED == g_sd_status) {
					status = 1;
				}
				else if (XM_SD_NOSPACE == g_sd_status) {
					status = 12;
				}
				else {
					status = 99;
				}
				HTTPSerMdl::Instance()->tcpSdStatus(g_engineId, status, GlobalData::Instance()->SDCard_write_speed_);
			}
		}
        break;
    case XM_EVENT_DEV_REMOVE:
		{
			LogFileManager::Instance()->Enable(false);
			XMLogI("SD REMOVE");
			if (GlobalPage::Instance()->page_sys_set()->format_tip_win_) {				
				lv_obj_del(GlobalPage::Instance()->page_sys_set()->format_tip_win_);				
			    GlobalPage::Instance()->page_sys_set()->format_tip_win_ = NULL;		
				}	
			g_sd_status = XM_SD_NOEXIST;
			HTTPSerMdl::Instance()->tcpSdStatus(g_engineId, 2, GlobalData::Instance()->SDCard_write_speed_);
			//缩时录影时拔出SD卡，直接关机
			if (GlobalData::Instance()->UI_mode_ == UIMode_CompactRecord) {
				GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_Acc, true, true);
				break;
			}

			GlobalPage::Instance()->page_main()->CloseRecord();
			GlobalPage::Instance()->page_main()->OpenTipBox("Please insert SD card");
			GlobalPage::Instance()->page_main()->SDCardImg();
			if (GlobalData::Instance()->UI_mode_ == UIMode_Playback)
				GlobalPage::Instance()->page_playback()->UnplugSDCard();
			if (GlobalData::Instance()->UI_mode_ == UIMode_Picture)
				GlobalPage::Instance()->page_play_pic()->UnplugSDCard();
			if (GlobalPage::Instance()->page_usb()->user_selected_usb_mode_ == USB_MODE_MASS_STORAGE)
				GlobalPage::Instance()->page_usb()->SetUsbFunc(USB_MODE_NONE);

			if (GlobalPage::Instance()->page_playback()->select_filetype_page_) {
				lv_obj_del(GlobalPage::Instance()->page_playback()->select_filetype_page_);
				GlobalPage::Instance()->page_playback()->select_filetype_page_ = NULL;
			}
	}
        break;
	case XM_EVENT_SDCARD_DAMAGED:
		XMLogW("insert bad sdcard");
		g_bad_sdcard = true;
		GlobalPage::Instance()->page_main()->OpenTipBox("SD card is damaged");
		break;
	case XM_EVENT_SDCARD_READ_ONLY:
		{
			XMLogW("XM_EVENT_SDCARD_READ_ONLY");
			XM_CONFIG_VALUE cfg_value;
			cfg_value.bool_value = true;
			GlobalData::Instance()->car_config()->SetValue(CFG_Operation_NEED_REPAIR_SDCARD, cfg_value);
		}
		break;
    case XM_EVENT_USB_ADD:
		{
			XMLogI("USB_ADD");
			int ret = XM_Middleware_Playback_CheckUDisk();
		}
		break;
	case XM_EVENT_USB_REMOVE:
		{
			XMLogI("USB_REMOVE");
			XM_Middleware_Playback_UmountUDisk();
		}
		break;
     case XM_EVENT_LOCALALARM_START:
		{
			XMLogI("LocalAlarm Start chn:%d", handle);
		}
     	break;
     case XM_EVENT_LOCALALARM_STOP:
		{
			XMLogI("LocalAlarm Stop chn:%d", handle);
		}
     	break;
	case XM_EVENT_KEYWORD_XIANSHIQIANLU:
		{
			XMLogI("open front channel");
			int ret = GlobalPage::Instance()->page_main()->ChangeSplit(XM_PLAY_SENSOR);
			if (!ret) {
				GlobalPage::Instance()->page_sys_set()->camera_dir_ = XM_PLAY_SENSOR;
				XM_CONFIG_VALUE cfg_value;
				cfg_value.int_value = XM_PLAY_SENSOR;
				GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Camera, cfg_value);
				MppMdl::Instance()->SetUserMode(XM_PLAY_SENSOR);
			}
		}
		break;
	case XM_EVENT_KEYWORD_XIANSHIHOULU:
		{
			XMLogI("open behind channel");
			int ret = GlobalPage::Instance()->page_main()->ChangeSplit(XM_PLAY_AD);
			if (!ret) {
				GlobalPage::Instance()->page_sys_set()->camera_dir_ = XM_PLAY_AD;
				XM_CONFIG_VALUE cfg_value;
				cfg_value.int_value = XM_PLAY_AD;
				GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Camera, cfg_value);
				MppMdl::Instance()->SetUserMode(XM_PLAY_AD);
			}
		}
		break;
	case XM_EVENT_KEYWORD_CHAKANQUANBU:
		{
			XMLogI("open quanjing");
			int ret = GlobalPage::Instance()->page_main()->ChangeSplit(XM_PLAY_BOTH);
			if (!ret) {
				GlobalPage::Instance()->page_sys_set()->camera_dir_ = XM_PLAY_BOTH;
				XM_CONFIG_VALUE cfg_value;
				cfg_value.int_value = XM_PLAY_BOTH;
				GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Camera, cfg_value);
				MppMdl::Instance()->SetUserMode(XM_PLAY_BOTH);
			}
		}
		break;
	case XM_EVENT_KEYWORD_DAKAIPINGMU:
		{
			XMLogI("open screen");
			GlobalPage::Instance()->page_main()->OpenScreen();
		}
		break;
	case XM_EVENT_KEYWORD_GUANBIPINGMU:
		{
			XMLogI("close screen");
			GlobalPage::Instance()->page_main()->CloseScreen();
		}
		break;
	case XM_EVENT_KEYWORD_ZHUAPAIZHAOPIAN:
		{
			XMLogI("catch pic");
			if (GlobalData::Instance()->UI_mode_ == UIMode_Photograph || GlobalData::Instance()->UI_mode_ == UIMode_Videotape) {
				GlobalPage::Instance()->page_main()->CheckSDStatus();
				if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
					GlobalPage::Instance()->page_main()->TakePic();
				}
			}
		}
		break;
	case XM_EVENT_KEYWORD_DAKAILUYIN:
		{
			XMLogI("da kai lu yin");
			GlobalPage::Instance()->page_set()->RecordVoiceEnable(true);
			HTTPSerMdl::Instance()->tcpMictatus(g_engineId, 1);
		}
		break;
	case XM_EVENT_KEYWORD_GUANBILUYIN:
		{
			XMLogI("guan bi lu yin");
			GlobalPage::Instance()->page_set()->RecordVoiceEnable(false);
			HTTPSerMdl::Instance()->tcpMictatus(g_engineId, 0);
		}
		break;
	case XM_EVENT_KEYWORD_DAKAIREDIAN:
		{
			XMLogI("da kai re dian");
			if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape && !GlobalPage::Instance()->page_main()->wifi_enable_
				&& GlobalPage::Instance()->page_main()->wifi_prepared_) {
				lv_event_send(GlobalPage::Instance()->page_main()->wifi_img_, LV_EVENT_CLICKED, NULL);
			}
		}
		break;
	case XM_EVENT_KEYWORD_GUANBIREDIAN:
		{
			XMLogI("guan bi re dian");
			if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape && GlobalPage::Instance()->page_main()->wifi_enable_
				&& GlobalPage::Instance()->page_main()->wifi_prepared_) {
				lv_event_send(GlobalPage::Instance()->page_main()->wifi_img_, LV_EVENT_CLICKED, NULL);
			}
		}
		break;
	case XM_EVENT_KEYWORD_JINJILUXIANG:
	case XM_EVENT_KEYWORD_WOYAOLUXIANG:
		{
			if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape) {
				GlobalPage::Instance()->page_main()->CheckSDStatus();
				if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
					GlobalPage::Instance()->page_main()->StartRecord();
				}
			}

			if (event_type == XM_EVENT_KEYWORD_JINJILUXIANG) {
				XMLogI("jin ji lu xiang");
				if (GlobalPage::Instance()->page_main()->lock_current_recording_file_ == false) 
					GlobalPage::Instance()->page_main()->LockCurrentFile();
			} else {
				XMLogI("wo yao lu xiang");
			}
		}
		break;
	case XM_EVENT_PLAY_First_I_frame:
		{
		}
		break;
	case XM_EVENT_VOLTAGE_LOW:
		{
			XMLogI("XM_EVENT_VOLTAGE_LOW");
			GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_USBDisconnect, true, true);
		}

	case XM_EVENT_KEY_SHUTDOWN:
		{
			XMLogI("XM_EVENT_KEY_SHUTDOWN");
			GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_PressKey, true, true);
		}
		break;
	case XM_EVENT_USB_DISCONNECT:
		{
			XMLogI("XM_EVENT_USB_DISCONNECT");
			//有USB断开则判断为是车充
			GlobalPage::Instance()->page_main()->ChangeCarChargerCfg(true);
			GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_USBDisconnect, true, true);
		}
		break;
	case XM_EVENT_USB_CONNECT:
		{
			XMLogI("XM_EVENT_USB_CONNECT");
			//USB连接则不是电池供电
			GlobalPage::Instance()->page_main()->battery_powered_ = false;
			GlobalPage::Instance()->page_main()->check_battery_ = false;
			// lv_obj_t* battery_img = GlobalPage::Instance()->page_main()->battery_img_;
			// if (battery_img && strcmp((char*)lv_img_get_src(battery_img), image_path"i_char.png"))
			// 	lv_img_set_src(battery_img, image_path"i_char.png");
		}
		break;
	case XM_EVENT_AD_EXIT:
		{
			XMLogI("XM_EVENT_AD_EXIT");
			osd_data_init();
			GlobalPage::Instance()->page_main()->OnADExit();
			HTTPSerMdl::Instance()->tcpCameraStatus(g_engineId, 0, 0, 1);
			//if (GlobalPage::Instance()->page_main()->wifi_enable_) {
			if(g_app_connect){
				MppMdl::Instance()->SubStreamEnable(true);
			}
			//ad被拔出，且当前app正在预览ad画面时，自动切到前摄
			if (g_channel == 1) {
				g_wait_IFrame = true;
				g_channel = 0;	
			}
		}
		break;
	case XM_EVENT_AD_CONNECT:
		{
			XMLogI("XM_EVENT_AD_CONNECT");
			GlobalPage::Instance()->page_main()->OnADInsert(false);
			osd_data_init();
			HTTPSerMdl::Instance()->tcpCameraStatus(g_engineId, 1, g_channel, 2);
			//if (GlobalPage::Instance()->page_main()->wifi_enable_) {
				if(g_app_connect){
				MppMdl::Instance()->SubStreamEnable(true);
			}
		}
		break;
	case XM_EVENT_ACC_DISCONNECT:
		{
			XMLogI("XM_EVENT_ACC_DISCONNECT");
			//有acc连接或断开则判断为不是车充
			GlobalPage::Instance()->page_main()->ChangeCarChargerCfg(false);
			GlobalPage::Instance()->page_main()->acc_disconnect_ = true;
			GlobalPage::Instance()->page_main()->AccDisconnectWork();
		}
		break;
	case XM_EVENT_ACC_CONNECT:
		{
			XMLogI("XM_EVENT_ACC_CONNECT");
			//有acc连接或断开则判断为不是车充
			GlobalPage::Instance()->page_main()->ChangeCarChargerCfg(false);
			GlobalPage::Instance()->page_main()->acc_disconnect_ = false;
			GlobalPage::Instance()->page_main()->AccConnectWork();
		}
		break;
	case XM_EVENT_POWERKEY_SHORTPRESS:
		{
			XMLogI("XM_EVENT_POWERKEY_SHORTPRESS");
			GlobalPage::Instance()->page_main()->PWREvent();
		}
		break;
	case XM_EVENT_COLLISION_DETECTED:
		{
			XMLogI("XM_EVENT_COLLISION_DETECTED, lock_current_recording_file_=%d",
				GlobalPage::Instance()->page_main()->lock_current_recording_file_);
			if (GlobalPage::Instance()->page_main()->lock_current_recording_file_ == false) 
				GlobalPage::Instance()->page_main()->LockCurrentFile();
		}
		break;
	case XM_EVENT_RECORD_END:
		{
			bool collision_startup_record = param;
			//后拉碰撞开机录像，且开机后已接上外电,转普通录像
			if (handle == 1 && collision_startup_record && 
				!GlobalPage::Instance()->page_main()->CollisionStartWithBattery()) {
				int channel = 1;
				XM_MW_Media_Info media_info;
				MppMdl::Instance()->GetEncInfo(channel, 0, &media_info);
				media_info.frame_rate = 25;
				XM_Middleware_Storage_SetMediaInfo(channel, &media_info);
				XM_Middleware_Storage_Start(channel, 0);
				return 0;
			}

			XMLogI("XM_EVENT_RECORD_END, collision_startup_=%d, param=%d",
				GlobalPage::Instance()->page_main()->collision_startup_, collision_startup_record);
			lv_img_set_src(GlobalPage::Instance()->page_main()->lock_img_, image_path"lock_off.png");
			lv_obj_add_flag(GlobalPage::Instance()->page_main()->lock_png_, LV_OBJ_FLAG_HIDDEN);
			if (GlobalPage::Instance()->page_main()->record_label_) {
				lv_img_set_src(GlobalPage::Instance()->page_main()->record_label_, image_path"red_circle.png");
				RED_OFF;
			}
			if (collision_startup_record) {
				GlobalPage::Instance()->page_main()->CollisionStartupWork(CollisionStartup_StopRecord);
			}
			GlobalPage::Instance()->page_main()->collision_startup_ = false;
			GlobalPage::Instance()->page_main()->lock_current_recording_file_ = false;
			//定期保存系统时间
			GlobalData::Instance()->SetCurTime(time(NULL));
		}
		break;
	case XM_EVENT_SDCARD_NOENOUGH_SPACE:
		{
			XMLogI("XM_EVENT_SDCARD_NOENOUGH_SPACE");
			//缩时录影时SD卡没有空间直接关机
			if (GlobalData::Instance()->UI_mode_ == UIMode_CompactRecord) {
				GlobalPage::Instance()->page_main()->OpenTipBox("SD card space is insufficient", 4000);
				lv_timer_create(GlobalPage::Instance()->page_main()->DelayShutDownTimer, 1000, NULL);
				break;
			}
			//碰撞录影时若SD卡空间不足且熄火直接关机
			if (GlobalPage::Instance()->page_main()->collision_startup_) {
				int ret = GlobalPage::Instance()->page_main()->CollisionStartupWork(CollisionStartup_SDCardError);
				if (ret < 0) 
					break;
			}

			GlobalPage::Instance()->page_main()->CloseRecord();
			GlobalPage::Instance()->page_main()->OpenTipBox("SD card space is insufficient");
		}
		break;
	case XM_EVENT_REVERSE_DETECTED:
		{
			XMLogI("XM_EVENT_REVERSE_DETECTED");
			 XM_Middleware_Smart_EnableKeyword(0);
			GlobalPage::Instance()->page_main()->ReverseEvent(true);
		}
		break;
	case XM_EVENT_REVERSE_END:
		{
			XMLogI("XM_EVENT_REVERSE_END");
			XM_CONFIG_VALUE cfg_value;
			cfg_value.bool_value = true;
			GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
			XM_Middleware_Smart_EnableKeyword(cfg_value.bool_value);
			GlobalPage::Instance()->page_main()->ReverseEvent(false);
		}
		break;
	case XM_EVENT_CHANGE_TIME:
		{
			XMLogI("XM_EVENT_CHANGE_TIME");
			if (GlobalPage::Instance()->page_main()->record_time_label_) {
				GlobalPage::Instance()->page_main()->CloseRecord();
				GlobalPage::Instance()->page_main()->StartRecord();
			}

			time_t t1 = time(NULL);
			GlobalPage::Instance()->page_main()->pre_sys_time_ = t1;
			//保存系统时间到配置文件
			GlobalData::Instance()->SetCurTime(time(NULL));
		}
		break;
	case XM_EVENT_RECORD_START:
		{
			XMLogI("XM_EVENT_RECORD_START");
			g_wait_IFrame = true;
			GlobalPage::Instance()->page_main()->StartRecord();
		}
		break;
	case XM_EVENT_RECORD_CLOSE:
		{
			XMLogI("XM_EVENT_RECORD_CLOSE");
			GlobalPage::Instance()->page_main()->CloseRecord();
		}
		break;
	case XM_EVENT_RECORDFILE_START:
		{
			XMLogI("XM_EVENT_RECORD_START, channel=%d", handle);
		}
		break;
	case XM_EVENT_RECORDFILE_END:
		{
			XMLogI("XM_EVENT_RECORDFILE_END, channel=%d, file name=%s", handle, msg.c_str());
		}
		break;
	case XM_EVENT_GPS_TIME:
		XMLogI("XM_EVENT_GPS_TIME, param=%d", param);
		#if GPS_EN
			if(1){
				if(GlobalPage::Instance()->page_main()->record_time_label_){
					GlobalPage::Instance()->page_main()->CloseRecord();
					update_hardware_time(gps.time);
					GlobalPage::Instance()->page_main()->StartRecord();
				}else{
					update_hardware_time(gps.time);
				}
			}
		#endif	
		break;
	case XM_EVENT_RECORD_SOUND_ENABLE:
		{
			XMLogI("XM_EVENT_RECORD_SOUND_ENABLE");
			bool enable = (param == 0) ? false : true;
			GlobalPage::Instance()->page_set()->RecordVoiceEnable(enable);
		}
		break;
	case XM_EVENT_VOICE_CONTROL_ENABLE:
		{
			XMLogI("XM_EVENT_VOICE_CONTROL_ENABLE");
			bool enable = (param == 0) ? false : true;
			XM_Middleware_Smart_EnableKeyword(enable);
			XM_CONFIG_VALUE cfg_value;
			cfg_value.bool_value = enable;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Voice_Control, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Voice_Control");
			}
		}
		break;
	case XM_EVENT_SDCARD_FORMAT:
		{
			XMLogI("XM_EVENT_SDCARD_FORMAT");
			GlobalData::Instance()->SDCard_write_speed_ = param;
		}
		break;
	case XM_EVENT_CAMERA_SWITCH:
		{
			g_wait_IFrame = true;
			XMLogI("XM_EVENT_CAMERA_SWITCH, channel = %d", param);
			g_channel = param;
		}
		break;
	case XM_EVENT_CFG_RESET:
		{
			XMLogI("XM_EVENT_CFG_RESET");
			GlobalPage::Instance()->page_sys_set()->RestoreDefaultSet();
		}
		break;
	case XM_EVENT_CHANGE_RESOLUTION:
		{
			XMLogI("XM_EVENT_CHANGE_RESOLUTION, resolution = %d", param);
			XM_CONFIG_VALUE cfg_value;
			XMUIEventInParam event_inparam;
			XMUIEventOutParam event_outparam;
           if (param == kMaxPicHeight) {
				event_inparam.set_enc.width = kMaxPicWidth;
				event_inparam.set_enc.height = kMaxPicHeight;
				event_inparam.set_enc.bit_rate = 20480;
				cfg_value.int_value = kMaxPicHeight;
				 if(param == k4KHeight)
					lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "4K");
					else
					lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "3K");
			}
			else if (param == 1440) {
				event_inparam.set_enc.width = 2560;
				event_inparam.set_enc.height = 1440;
				event_inparam.set_enc.bit_rate = 20480;
				cfg_value.int_value = 1440;
				lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "2K");
			}
			else if (param == 1080) {
				event_inparam.set_enc.width = 1920;
				event_inparam.set_enc.height = 1080;
				event_inparam.set_enc.bit_rate = 4096;
				cfg_value.int_value = 1080;
				lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "1080P");
			}

			event_inparam.set_enc.video_codec_type = PT_H265;
			event_inparam.set_enc.channel = Direction_Front;
			// if (param == k3KHeight || param == k4KHeight) {
			// 	if (MppMdl::Instance()->AdLoss()) {
			// 	    if(param == k4KHeight)
			// 		lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "4K");
			// 		else
			// 		lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "3K");
			// 		GlobalData::Instance()->ui_event_cb_(XM_UI_SET_ENCODE_PARAM, &event_inparam, &event_outparam);
			// 	}
			// }
			// else {
			 	GlobalData::Instance()->ui_event_cb_(XM_UI_SET_ENCODE_PARAM, &event_inparam, &event_outparam);
			// }

			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Video_Resolution, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Video_Resolution");
			}
			osd_data_init();
			if(g_app_connect){
			  MppMdl::Instance()->SubStreamEnable(true);
			}
		}
		break;
	case XM_EVENT_REC_SPLIT_DUR:
		{
			XMLogI("XM_EVENT_REC_SPLIT_DUR, param = %d", param);
			XMStorageRecordDuration dur = XM_STORAGE_Record_Duration_60;
			if (param == 1) {
				dur = XM_STORAGE_Record_Duration_60;
			}
			else if (param == 2) {
				dur = XM_STORAGE_Record_Duration_120;
			}
			else if (param == 3) {
				dur = XM_STORAGE_Record_Duration_180;
			}
			XM_Middleware_Storage_SetRecordDuration(dur);

			XM_CONFIG_VALUE cfg_value;
			cfg_value.int_value = param;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Circular_Record_Time, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Circular_Record_Time");
			}
		}
		break;
	case XM_EVENT_GSR_SENSITIVITY:
		{
			XMLogI("XM_EVENT_GSR_SENSITIVITY, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			if (param == 0)
				cfg_value.int_value = Sensitivity_Close;
			else if (param == 1)
				cfg_value.int_value = Sensitivity_Low;
			else if (param == 2)
				cfg_value.int_value = Sensitivity_Mid;
			else if (param == 3)
				cfg_value.int_value = Sensitivity_High;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Collision_Sensitivity, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Collision_Sensitivity");
			}

			PeripheryManager::Instance()->SensitivitySet(cfg_value.int_value);
			if (cfg_value.int_value == Sensitivity_Close) {
				PeripheryManager::Instance()->GsensorModeSelect(2);
			}
			else {
				PeripheryManager::Instance()->GsensorModeSelect(0);
			}
		GlobalPage::Instance()->page_main()->UpdateGsensorImg();	
		}
		break;
	case XM_EVENT_SCREEN_STANDBY:
		{
			XMLogI("XM_EVENT_SCREEN_STANDBY, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			cfg_value.int_value = param * 60;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Lcd_OffTime, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Lcd_OffTime");
			}

			lv_timer_t* timer = GlobalPage::Instance()->page_main()->rest_screen_timer_;
			if (timer) {
				if (cfg_value.int_value != 0) {
					lv_timer_set_period(timer, cfg_value.int_value * 1000);
					lv_timer_resume(timer);
					lv_timer_reset(timer);
				}else{
					lv_timer_pause(timer);
				}
			}
		}
		break;
	case XM_EVENT_PARKING_MONITOR:
		{
			XMLogI("XM_EVENT_PARKING_MONITOR, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			cfg_value.bool_value = param;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Collision_Startup, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Collision_Startup");
			}
			GlobalPage::Instance()->page_main()->UpdateCollisionStartupImg();
		}
		break;
	case XM_EVENT_TIMELAPSE_RATE:
		{
			XMLogI("XM_EVENT_TIMELAPSE_RATE, param = %d", param);
			XMTimeLapse time_lapse;
			if (param == 0)
				time_lapse = XM_TIME_LAPSE_Closed;
			else if (param == 1)
				time_lapse = XM_TIME_LAPSE_Frame_1;
			else if (param == 2)
				time_lapse = XM_TIME_LAPSE_Frame_2;
			else if (param == 5)
				time_lapse = XM_TIME_LAPSE_Frame_5;

			XM_CONFIG_VALUE cfg_value;
			cfg_value.int_value = param;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Compact_Record_Fps, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Compact_Record_Fps");
			}
			XM_Middleware_ParkRecord_SetTimeLapse(time_lapse);
		}
		break;
	case XM_EVENT_OSD_ENABLE:
		{
			XMLogI("XM_EVENT_OSD_ENABLE, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			cfg_value.bool_value = param == 1 ? true : false;
			// XM_Middleware_Encode_EnableOsdTime(cfg_value.bool_value, osd_x, osd_y);
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Date_Watermark, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Date_Watermark");
			}
			osd_data_init();
		}
		break;
	case XM_EVENT_CHANGE_WIFI_PARAM:
		{
			XMLogI("XM_EVENT_CHANGE_WIFI_PARAM, param = %d", param);
			if (GlobalPage::Instance()->page_main()->wifi_id_label_) {
				WIFI_MAC_PARAM_S wifi_param;
				memset(wifi_param.ssid, 0, sizeof(wifi_param.ssid));
				memset(wifi_param.mac, 0, sizeof(wifi_param.mac));
				XM_Middleware_WIFI_GetMacParam(&wifi_param);

				//lv_label_set_text(GlobalPage::Instance()->page_main()->wifi_id_label_, wifi_param.ssid);
				//lv_obj_align(GlobalPage::Instance()->page_main()->wifi_id_label_, LV_ALIGN_BOTTOM_RIGHT, -size_w(10), -size_h(10));
			}
		}
		break;
	case XM_EVENT_NETWORK_MSG:
		{
			if (!GlobalPage::Instance()->page_main()->app_disconnect_timer_) {
				GlobalPage::Instance()->page_main()->app_disconnect_timer_ = lv_timer_create(GlobalPage::Instance()->page_main()->AppDisconnectTimer, 12 * 1000, NULL);
				GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(false);
			}
			else {
				lv_timer_reset(GlobalPage::Instance()->page_main()->app_disconnect_timer_);
			}
		}
		break;
	case XM_EVENT_KEYTONE_ENABLE:
		{
			XMLogI("XM_EVENT_KEYTONE_ENABLE, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			cfg_value.bool_value = param == 1 ? true : false;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Key_Voice, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Key_Voice");
			}

			GlobalData::Instance()->key_tone_ = cfg_value.bool_value;
		}
		break;
	case XM_EVENT_PARK_RECORD_TIME:
		{
			XMLogI("XM_EVENT_PARK_RECORD_TIME, param = %d", param);
			XM_CONFIG_VALUE cfg_value;
			cfg_value.int_value = param;
			int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Compact_Record_Duration, cfg_value);
			if (ret < 0) {
				XMLogE("set config error, opr=CFG_Operation_Compact_Record_Duration");
			}

			XMTimeLapseDuration dur = XM_TIME_LAPSE_Duration_Closed;
			if (param == 0) {
				dur = XM_TIME_LAPSE_Duration_Closed;
			}
			else if (param == 8) {
				dur = XM_TIME_LAPSE_Duration_8;
			}
			else if (param == 16) {
				dur = XM_TIME_LAPSE_Duration_16;
			}
			else if (param == 24) {
				dur = XM_TIME_LAPSE_Duration_24;
			}

			XM_Middleware_ParkRecord_SetTimeLapseDuration(dur);
		}
		break;
	case XM_EVENT_APP_PAGE:
		{
			XMLogI("XM_EVENT_APP_PAGE, param = %d", param);
			GlobalData::Instance()->app_page_ = (APP_PAGE)param;
		}
		break;

    case XM_EVENT_IN_VIDEO:
		{
			XMLogI("XM_EVENT_IN_VIDEO, param ");
			GlobalPage::Instance()->page_main()->ChangeMode_Video();
			GlobalPage::Instance()->page_main()->AppMenuValue();
			lv_img_set_src(GlobalPage::Instance()->page_main()->wifi_img_,image_path"wifi.png");
		}
		break;

	default:
        break;
    }
	return 0;
}

//中间件sdk回放流回调
bool OnMWVODStreamCallBack(long handle, XM_MW_Media_Frame* media_frame, int64_t user)
{
    return true;
}

//MPP实时预览数据回调
void OnMWRealTimeStreamCallBack(int channel, int stream, XM_MW_Media_Frame* media_frame, int64_t user)
{
	if (!g_realplay || channel != g_channel)
		return;

	if (g_channel == 0) {
		if (stream != 1)
			return;
	}

	if (g_wait_IFrame) {
		if (media_frame->key_frame) {
			g_wait_IFrame = false;
		}
		else {
			return;
		}
	}

	HTTPSerMdl::Instance()->OnMWRealTimeStream(channel, 0, media_frame, user);
	/* if (!g_realplay || channel != g_channel)
		return ;

	int64_t now = GetTickTime();
	if (g_first_frame_time <= 0) {
		g_first_frame_time = now;
	}

	int pts = (int)(now - g_first_frame_time);
	if (media_frame->video) {
		XM_Middleware_Network_SendFrame(g_engineId, g_connId, (char*)media_frame->frame_buffer,
			media_frame->frame_size, true, media_frame->key_frame, media_frame->timestamp, g_frame_id);
		if (media_frame->key_frame) 
			XMLogI("key frame, len=%d", media_frame->frame_size);
	}
	else {
		XM_Middleware_Network_SendFrame(g_engineId, g_connId, (char*)media_frame->frame_buffer,
			media_frame->frame_size, false, false, media_frame->timestamp, g_frame_id);
	} */
}

//UI界面事件回调
int OnUIEventCallback(XMUIEventType ui_event_type, XMUIEventInParam* in_param, XMUIEventOutParam* out_param)
{
	if (XM_UI_VOD_GET_CURRENT_DURATION != ui_event_type)
		XMLogI("ui_event_type=%d", ui_event_type);

	switch (ui_event_type)
	{
	case XM_UI_START_STORAGE:
		{
			for (int i = 0; i < kChannelNum; i++) {
				int channel = i;
				XM_MW_Media_Info media_info;
				MppMdl::Instance()->GetEncInfo(channel, 0, &media_info);
				if(channel==0){
                 media_info.frame_rate = kFrameRate;
				}
				XM_Middleware_Storage_SetMediaInfo(channel, &media_info);
				XM_Middleware_Storage_Start(channel, 0);
			}
		}
		break;
	case XM_UI_STOP_STORAGE:
		{
			for (int i = 0; i < kChannelNum; i++) {
				int channel = i;
				XM_Middleware_Storage_Stop(channel);
			}
		}
		break;
	case XM_UI_QUERY_RECORD:
		XM_Middleware_Playback_Query(&in_param->query_record.rec_query, &out_param->query_record.rec_list);
		break;
    case XM_UI_VOD_OPEN:
		{
			XMStorageSDCardNum sdcard_num = in_param->vod_start.sdcard_num;
			int channel = in_param->vod_start.channel;
			const char* file_full_name = (const char*)in_param->vod_start.file_full_name;
			XMFileFormat file_format = in_param->vod_start.file_format;
			long handle = XM_Middleware_Playback_Open(sdcard_num, channel, file_format, file_full_name);
			//返回播放句柄
			out_param->vod_start.handle = handle;
		}
		break;
	case XM_UI_VOD_GET_FILE_INFO:
		{
			XM_MW_File_Info file_info;
			long handle = in_param->vod_getfileinfo.handle;
			XM_Middleware_Playback_GetFileInfo(handle, &file_info);
			XMLogI("XM_Middleware_Playback_GetFileInfo, width=%d, height=%d, rate=%d, duration=%d", 
				file_info.media_info.width, file_info.media_info.height, file_info.media_info.frame_rate, file_info.duration);
			//返回文件长度，单位毫秒
			out_param->vod_getfileinfo.duration = file_info.duration;
			out_param->vod_getfileinfo.media_info = file_info.media_info;
		}
		break;
    case XM_UI_VOD_PLAY:
		{
			long handle = in_param->vod_play.handle;
			bool play_one = in_param->vod_play.play_one;
			RECT_S rc = in_param->vod_play.dst_rc; //回放界面位置和大小
			XM_Middleware_Playback_Play(handle, play_one, &rc);
		}
		break;
    case XM_UI_VOD_PAUSE:
		{
			long handle = in_param->vod_pause.handle;
			bool pause = in_param->vod_pause.pause;
			XM_Middleware_Playback_Pause(handle, pause);
		}
		break;
    case XM_UI_VOD_SEEK:
		{
			long handle = in_param->vod_seek.handle;
			int seek_time = in_param->vod_seek.seek_time;
			XM_Middleware_Playback_Seek(handle, seek_time);
		}
		break;
	case XM_UI_VOD_GET_CURRENT_DURATION:
		{
			long handle = in_param->vod_getcurduration.handle;
			int cur_duration = XM_Middleware_Playback_GetCurrentDuration(handle);
			out_param->vod_getcurduration.cur_duration = cur_duration;
		}
		break;
	case XM_UI_VOD_STOP:
		{
			long handle = in_param->vod_stop.handle;
			XM_Middleware_Playback_Close(handle);
		}
		break;
	case XM_UI_VOD_CLOSE:
		{
			long handle = in_param->vod_stop.handle;
			XM_Middleware_Playback_Close(handle);
			//关闭回放后，直接开启预览
			MppMdl::Instance()->StartPreview();
		}
		break;

	case XM_UI_VOD_LOCK_HISTORY_FILE:
		{
			XM_Middleware_Storage_LockHistoryFile(&in_param->vod_lockhistoryfile.file_param);
		}
		break;
	case XM_UI_VOD_UNLOCK_HISTORY_FILE:
		{
			XM_Middleware_Storage_UnLockHistoryFile(&in_param->vod_unlockhistoryfile.file_param);
		}
		break;
	case XM_UI_SET_STORAGE_CONFIG:
		{
			XMStorageRecordDuration record_duration = in_param->set_record_duraion.record_duration;
			XM_Middleware_Storage_SetRecordDuration(record_duration);
		}
		break;
	case XM_UI_SET_ENCODE_PARAM:
		{
			PAYLOAD_TYPE_E video_codec_type = in_param->set_enc.video_codec_type;
			int width = in_param->set_enc.width;
			int height = in_param->set_enc.height;
			int bit_rate = in_param->set_enc.bit_rate;
			int channel = in_param->set_enc.channel;
			SetEncodeParam(channel, video_codec_type, width, height, height == kMaxPicHeight ? 25 : 25, bit_rate); //25

			XMLogI("-------------encode:%d, width:%d, height:%d, bit_rate:%d", video_codec_type, width, height, bit_rate);
		}
		break;	
	case XM_UI_PICTURE_RENDER:
		{
			const char* file_full_name = (const char*)in_param->picture_render.file_full_name;
			int file_len = in_param->picture_render.file_len;
			RECT_S rc = GlobalData::Instance()->dst_rc; //图片渲染的位置和大小
			XM_Middleware_Picture_Play(file_full_name, file_len, &rc);
		}
		break;
	case XM_UI_PICTURE_CLOSE:
		{
			MppMdl::Instance()->StartPreview();
		}
		break;
	case XM_UI_SMART_SET_AI_TYPE:
		{
			XMAIType ai_type = in_param->set_AI_type.ai_type;
			for (int i = 0; i < kChannelNum; i++) {
				int channel = i;
				XM_Middleware_Smart_SetAIType(channel, ai_type);
			}
		}
		break;
	case XM_UI_SMART_SET_BSD_PARAM:
		{
			int channel = in_param->set_BSD_param.channel;
			XM_Middleware_Smart_SetBsdParam(channel, &in_param->set_BSD_param.bsd_param);

		}
		break;
	case XM_UI_SMART_ENABLE_AI:
		{
			for (int i = 0; i < kChannelNum; i++) {
				int channel = i;
				XM_Middleware_Smart_EnableAI(channel, in_param->enable_AI.enable);
			}
		}
		break;
	case XM_UI_SPLIT_SET_GRID_RECT:
		{
			// XM_Middleware_Split_SetGridRect(&in_param->set_grid_rect.split_rect_param, 
			// 	&out_param->set_grid_rect.split_param);
		}
		break;
	case XM_UI_STORAGE_DEL_FILE:
		{
			XM_Middleware_Storage_DelFile(&in_param->storage_del_file.file_param);
		}
		break;
	case XM_UI_SOUND_SET_VOLUME:
		{
			MppMdl::Instance()->SetVolume(in_param->sound_set_volume.sound_volume);
		}
		break;
	case XM_UI_PREVIEW_STOP:
		{
			MppMdl::Instance()->StopPreview();
		}
		break;
	case XM_UI_MIRROR_SET_MIRROR:
		{
			int chn = in_param->set_mirror.chn;
			bool open_mirror = in_param->set_mirror.open_mirror;
			//屏显和录像都做镜像
				MppMdl::Instance()->SetMirror(chn, open_mirror ? XM_MIRROR_VI : XM_MIRROR_NONE);
		}
		break;
	case XM_UI_SMART_ENABLE_KEYWORD:
		{
			bool enable = in_param->smart_enable_keyword.enable;
			XM_Middleware_Smart_EnableKeyword(enable);
		}
		break;
	default:
		return -1;
	}

	if (XM_UI_VOD_GET_CURRENT_DURATION != ui_event_type)
		XMLogI("after process event, ui_event_type=%d", ui_event_type); 
	return 0;
}

// Retrieve year info
#define OS_YEAR     ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 \
                                     + (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))
 
// Retrieve month info
#define OS_MONTH    (__DATE__ [2] == 'n' ? (__DATE__ [1] == 'a' ? 1 : 6) \
                                 : __DATE__ [2] == 'b' ? 2 \
                                 : __DATE__ [2] == 'r' ? (__DATE__ [0] == 'M' ? 3 : 4) \
                                 : __DATE__ [2] == 'y' ? 5 \
                                 : __DATE__ [2] == 'l' ? 7 \
                                 : __DATE__ [2] == 'g' ? 8 \
                                 : __DATE__ [2] == 'p' ? 9 \
                                : __DATE__ [2] == 't' ? 10 \
                                 : __DATE__ [2] == 'v' ? 11 : 12)
 
// Retrieve day info
#define OS_DAY      ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 \
                                 + (__DATE__ [5] - '0'))

#define OS_HOUR     ((__TIME__ [0] - '0') * 10 + (__TIME__ [1] - '0'))
 
// Retrieve minute info
#define OS_MINUTE   ((__TIME__ [3] - '0') * 10 + (__TIME__ [4] - '0'))

// Retrieve second info
#define OS_SECOND   ((__TIME__ [6] - '0') * 10 + (__TIME__ [7] - '0'))

bool sdcard_checked = false;
bool record_start_dealt = false;

void* check_sdcard_thread(void* args)
{
	int sd_status = XM_Middleware_Storage_CheckSDCard(XM_STORAGE_SDCard_0);
	XMLogI("g_sd_status = %d, sd_status=%d", g_sd_status, sd_status);
	//做这个判断是为防止已经回调出XM_EVENT_SDCARD_DAMAGED, XM_Middleware_Storage_CheckSDCard才返回的情况
	if (!g_bad_sdcard) {
		g_sd_status = sd_status;
		sdcard_checked = true;
	}
}

void* timer_hander_thread(void* args)
{
	while (1) {
		lv_timer_handler();
		usleep(4 * 1000);   /*Sleep for 5 millisecond*/
		
		int size = g_mw_event_infos.size();
		for (int i = 0; i < size; i++) {
			g_event_mutex.lock();
			MWEventInfo ev_info = g_mw_event_infos.front();
			g_mw_event_infos.pop_front();
			g_event_mutex.unlock();
			ProcessEvent(ev_info.handle, ev_info.event_type, ev_info.msg, ev_info.param);
		}

		if (sdcard_checked && !record_start_dealt) {		
			XMLogI("g_sd_status = %d", g_sd_status);
			if (g_sd_status == XM_SD_NORMAL) {
				int write_speed = 0;
				int ret;
				if (GlobalData::Instance()->gdb_debug_) {
					ret = 0;
					write_speed = 2048;
				}
				else {
					ret = XM_Middleware_Storage_GetSpeedState(XM_STORAGE_SDCard_0, &write_speed);
				}
				XMLogI("write_speed = %d, ret=%d", write_speed, ret);
				GlobalData::Instance()->SDCard_write_speed_ = write_speed;
				if (ret < 0) {
					g_sd_status = XM_SD_NEEDCHECKSPEED;
				}
				else {
					if (write_speed < 1024)
						GlobalPage::Instance()->page_main()->OpenSDSpeedTipBox(write_speed, true, 1000);
					else {
						LogFileManager::Instance()->Enable(true);
						LogFileManager::Instance()->write_log_sd_set(1,1);
					}
				}
			}
			else if (g_sd_status == XM_SD_NEEDFORMAT) {
			}
			else if (g_sd_status == XM_SD_ABNORMAL) {
				GlobalPage::Instance()->page_main()->OpenTipBox("The memory card is damaged, please replace it",
					3000, true, 1000);
			}
			else if (g_sd_status == XM_SD_NOTRW) {
			}
			else if (g_sd_status == XM_SD_NOSPACE) {
				GlobalPage::Instance()->page_main()->OpenTipBox("SD card space is insufficient",
					3000, true, 1000);
			}
			else if (g_sd_status == XM_SD_NO_C10) {
				GlobalPage::Instance()->page_main()->OpenTipBox("SD card is not class10",
					3000, true, 1000);
			}

			if (!GlobalPage::Instance()->page_main()->connect_computer_
				&& (g_sd_status == XM_SD_NEEDCHECKSPEED || g_sd_status == XM_SD_NEEDFORMAT || g_sd_status == XM_SD_NOTRW)) {
				GlobalPage::Instance()->page_sys_set()->OpenFormatTipWin();
			}
			XMLogI("RecordOnStartup, g_sd_status = %d", g_sd_status);
			record_start_dealt = true;
			GlobalPage::Instance()->page_main()->SDCardImg();
			GlobalPage::Instance()->page_main()->RecordOnStartup();
			
			XMLogW("[HandOpenTipBox] after RecordOnStartup, g_sd_status = %d \r\n", g_sd_status);
			GlobalPage::Instance()->page_main()->audio_flag_ = SD_CARD_PLAY_INTERVAL_TIME -2;
			GlobalPage::Instance()->page_main()->HandOpenTipBox();
		}
	}
}


void Adjust_Screen_Effect()
{
	  lcd_vo_param vo_param = {0};
       unsigned int  gama_value[65]={0};
		XM_CONFIG_VALUE cfg_value;
		cfg_value.int_value = 85;  //对应gamma值
		GlobalData::Instance()->car_config()->SetValue(CFG_Operation_GAMMA_DELTA, cfg_value);
		gama_value[0] = 0;
		gama_value[64] = 0x100;
		int j = 1;
		for (double i = 1; i < 64; i++) {
			double dd = pow(i / 64.0, 100.0 / cfg_value.int_value);
			gama_value[j] = dd * 0x100;
			//XMLogI("%d,", gama_value[j]);
			j++;
		}
        vo_param.ygain = 110;//135;//125
		vo_param.ugain = 115;//120;//105;//130
		vo_param.vgain = 115;//120;//105;//130
		vo_param.rgain = 100;//105;
		vo_param.ggain = 100;//102;
		vo_param.bgain = 98;//105;
		// XMLogI("ygain, %d %d %d %d %d %d", vo_param.ygain, vo_param.ugain, vo_param.vgain,
		// 	vo_param.rgain, vo_param.ggain, vo_param.bgain);
		vo_param.yoffs = -8;//-8;//-5
		vo_param.uoffs = 0;
		vo_param.voffs = 0;
		vo_param.roffs = 0;
		vo_param.goffs = 0;
		vo_param.boffs = 0;
		// XMLogI("yoffs, %d %d %d %d %d %d", vo_param.yoffs, vo_param.uoffs, vo_param.voffs,
		// 	vo_param.roffs, vo_param.goffs, vo_param.boffs);
		vo_param.rcoe[0] = 100;
		vo_param.rcoe[1] = 0;
		vo_param.rcoe[2] = 0;
		vo_param.gcoe[0] = 0;
		vo_param.gcoe[1] = 100;
		vo_param.gcoe[2] = 0;
		vo_param.bcoe[0] = 0;
		vo_param.bcoe[1] = 0;
		vo_param.bcoe[2] = 100;
		
		for (int i = 0; i < 65; i++) {
				vo_param.gamma[i] = gama_value[i];
			//	printf("%d ", vo_param.gamma[i]);
		}
	LIBCR_VO_SetParam(&vo_param);		
}


int main(int argc, char *argv[ ])
{
	//父进程
	XMLogI("============XM_Middleware_SetLogCallback\n");
	XM_Middleware_SetLogCallback(OnMWLogCallback, 0);
	if (argc == 1) {
		XMLogI("car app start");
		//同步系统时间
		std::string a("/sbin/hwclock -s");
		system(a.c_str());

		//时间处理
        time_t t = time(NULL);
        struct tm* current_time = localtime(&t);
        if ((current_time->tm_year + 1900) < 2024) {
			//rtc时钟出现异常
			char buf1[128] = { 0 };
			//读取配置文件中保存的时间
			int64_t pretime = GlobalData::Instance()->GetCurTime();
			if (pretime < 0) {
				sprintf(buf1, "date -s \"%d-%d-%d %d:%d:%d\"", 2024, 1, 1, 0, 0, 0);  
			}	
            else {
				time_t pre = pretime;
				struct tm* pre_time = localtime(&pre);
				sprintf(buf1, "date -s \"%d-%d-%d %d:%d:%d\"", pre_time->tm_year + 1900, pre_time->tm_mon + 1,
				pre_time->tm_mday, pre_time->tm_hour, pre_time->tm_min, pre_time->tm_sec);
			}
            
            system(buf1);
            //同步硬件时间
            std::string a("/sbin/hwclock -w");
            system(a.c_str());
        }

		//开启看门狗
		XM_Middleware_WDT_WatchDogStart(3);

		//检测是否是连接电脑
		/* XMLogI("before connect_computer check");
		GlobalPage::Instance()->page_main()->connect_computer_ = (bool)XM_Middleware_USBFunc_ConnectComputer();
		XMLogI("after connect_computer_ = %d", GlobalPage::Instance()->page_main()->connect_computer_); */

		int i,j;
		MppMdl::Instance()->SetChannelNum(kChannelNum);
		XM_Middleware_SetChannelNum(kChannelNum);
		//初始化中间件sdk
		XM_Middleware_Init();

		//设置后拉参数
		RECT_S rect_info;
		memset(&rect_info, 0 , sizeof(RECT_S));
		rect_info.u32Width = 240;
		rect_info.u32Height = 160;
		rect_info.s32X = 640 - rect_info.u32Width;
		rect_info.s32Y = 0;
		MppMdl::Instance()->SetADRectInfos(rect_info);
		//设置后拉类型
		MppMdl::Instance()->SetADType(VIDEO_FMT_1080);
		
		XM_MW_OSD_INFOS osd_infos;
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 2; j++) {
				osd_infos.osd[i][j].enable=true;
				osd_infos.osd[i][j].x=j*4096;
				osd_infos.osd[i][j].y=4096;
				osd_infos.osd[i][j].width=XM_MAX_OSD_WIDTH;
				osd_infos.osd[i][j].height=XM_MAX_OSD_HEIGHT;
				for (int k = 0; k < XM_MAX_OSD_HEIGHT; k++) {
					memset(osd_infos.osd[i][j].osd_buf+k*XM_MAX_OSD_WIDTH*2, 255-k, XM_MAX_OSD_WIDTH*2);
				}
				osd_infos.osd[i][j].up=false;
			}
		}
		
		//SystemInit 前就要设置好ad信息
		MppMdl::Instance()->SystemInit();
		//osd信息要在SystemInit后立即设置
		//临时调试用
		//MppMdl::Instance()->SetOSDInfos(osd_infos);	
		//XM_Middleware_SetLogLevel(XM_LOG_VERBOSE);
		XM_Middleware_SetEventCallback(OnMWEventCallBack, 0);
		XM_Middleware_SetVodStreamCallback(OnMWVODStreamCallBack, 0);
		XM_Middleware_SetRealStreamCallback(OnMWRealTimeStreamCallBack, 0);
		// XM_Middleware_SetLogCallback(OnMWLogCallback, 0);
		
		
		//设置版本号
		std::string version ="X2V50-";// "CAR.";
	#ifdef BOARD8520DV200   
		version += "8520dv200.";
	#endif
		//version += "00-" ;//VSP，定义00为通用
		version += XM_Middleware_GetVersion(); //sdk版本号
		char date_version[32] = {0};
		#if 0
		sprintf(date_version, "-%04d%02d%02d-%02d:%02d:%02d", 
			OS_YEAR, OS_MONTH, OS_DAY, OS_HOUR, OS_MINUTE, OS_SECOND);
		#else
		 sprintf(date_version, "-20250218");
		#endif	
		version += date_version;
		GlobalData::Instance()->set_version(version);
		XMLogW("program version is [%s]", version.c_str());

		//设置坏卡检测超时时间
		XM_Middleware_Storage_SetBadSdcardCheckTime(8000);
		//设置存储路径，分区路径和mount路径
		XM_MW_Storage_SDCard_Config sdcard_cfg = { 0 };
		sdcard_cfg.sdcard_count = 1;
		strcpy(sdcard_cfg.mount_path[XM_STORAGE_SDCard_0], kTFCardPath);
		//strcpy(sdcard_cfg.mount_path[XM_STORAGE_SDCard_1], "/var/usb2");
		XMLogI("before XM_Middleware_Storage_SetSDCardConfig");
		XM_Middleware_Storage_SetSDCardConfig(&sdcard_cfg);

		XMLogI("before ReadConfigFromFile");
		//读取配置文件
		GlobalData::Instance()->car_config()->ReadConfigFromFile();

			//播放开机音频
        XM_CONFIG_VALUE cfg_value;
		int ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_boot_Voice, cfg_value);
		int PlaySound_flag=cfg_value.bool_value;
        if(ret>=0 && cfg_value.bool_value){
		std::string sound_file = kAudioPath;
		sound_file += "kaiji_16k.pcm";
		MppMdl::Instance()->PlaySound(sound_file.c_str());
		}
		cfg_value.bool_value = false;
		 ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_NEED_REPAIR_SDCARD, cfg_value);
		if (ret >= 0 && cfg_value.bool_value) {
			//play some warning sound
			//MppMdl::Instance()->PlaySound(sound_file.c_str());

			XMLogW("before fsck sdcard");
			char* kDevNode1 = "/dev/mmcblk0p1";
			char* kDevNode2 = "/dev/mmcblk0";
			bool find_dev_node1 = access(kDevNode1, F_OK) == 0 ? true : false;
			const char* sdcard_dev_node = find_dev_node1 ? kDevNode1 : kDevNode2;
			char str_cmd[64] = {0};
			sprintf(str_cmd, "fsck.fat -V -a -w %s", sdcard_dev_node);
			system(str_cmd);
			XMLogW("after fsck sdcard");
			cfg_value.bool_value = false;
			GlobalData::Instance()->car_config()->SetValue(CFG_Operation_NEED_REPAIR_SDCARD, cfg_value);
		}

		//读取并设置录像单文件时长
		XMStorageRecordDuration record_duration = XM_STORAGE_Record_Duration_60;
		 ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Circular_Record_Time, cfg_value);
		if (ret >= 0) {
			if (cfg_value.int_value == 1) {
				record_duration = XM_STORAGE_Record_Duration_60;
			}
			else if (cfg_value.int_value == 2) {
				record_duration = XM_STORAGE_Record_Duration_120;
			}
			else if (cfg_value.int_value == 3) {
				record_duration = XM_STORAGE_Record_Duration_180;
			}
		}
		XM_Middleware_Storage_SetRecordDuration(record_duration);

		//设置编码类型，目前暂只支持h265
		PAYLOAD_TYPE_E video_codec_type = PT_H265;
		ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Encode_Format, cfg_value);
		if (ret >= 0) {
			if (cfg_value.int_value == PT_H264) {
				video_codec_type = PT_H264;
			}
			else {
				video_codec_type = PT_H265;
			}
		}
		
		XMRealPlayMode play_mode = XM_PLAY_BOTH;
		cfg_value.int_value = XM_PLAY_BOTH;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Camera, cfg_value);
		play_mode = (XMRealPlayMode)cfg_value.int_value;
		GlobalPage::Instance()->page_sys_set()->camera_dir_ = cfg_value.int_value;
		MppMdl::Instance()->SetUserMode(play_mode);


		//后拉镜像设置
	   cfg_value.bool_value = false;
	   GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Behind_Mirror, cfg_value);
	   MppMdl::Instance()->SetMirror(1, cfg_value.int_value ? XM_MIRROR_VI : XM_MIRROR_NONE);
		MppMdl::Instance()->Init();
		//设置分辨率
		int width = kMaxPicWidth;
		int height = kMaxPicHeight;
		int bit_rate = 20480;
		ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Resolution, cfg_value);
		if (ret >= 0) {
			if (cfg_value.int_value == 1080) {
				width = 1920;
				height = 1080;
				bit_rate = 4096;
			}
			else if (cfg_value.int_value == 1440) {
				width = 2560;
				height = 1440;
				bit_rate = 20480;
			}
			else if (cfg_value.int_value == kMaxPicHeight) {
				width = kMaxPicWidth;
				height = kMaxPicHeight;
				bit_rate = 20480;
			}
		}
		XM_MW_Encode_Info enc_info;
		enc_info.channel_num = 2;
		enc_info.enc_medias[0][0].video_codec = video_codec_type;
		enc_info.enc_medias[0][0].width = width;
		enc_info.enc_medias[0][0].height = height;
		enc_info.enc_medias[0][0].bit_rate = bit_rate;
		enc_info.enc_medias[0][0].frame_rate = height == kMaxPicHeight ? 25 : 25;//30
		enc_info.enc_medias[0][1].video_codec = video_codec_type;
		enc_info.enc_medias[0][1].width = kSubStreamWidth;
		enc_info.enc_medias[0][1].height = kSubStreamHeight;
		enc_info.enc_medias[0][1].bit_rate = kSubStreambit_rate;
		enc_info.enc_medias[0][1].frame_rate = kSubStreamframe_rate;
		enc_info.enc_medias[1][0].video_codec = video_codec_type;
		enc_info.enc_medias[1][0].width = 1920;
		enc_info.enc_medias[1][0].height = 1080;
		enc_info.enc_medias[1][0].bit_rate = 4096;
		enc_info.enc_medias[1][0].frame_rate = 25;
		for (int i = 0; i < kChannelNum; i++) {
			enc_info.enc_medias[i][0].audio_codec = PT_PCMA;
			enc_info.enc_medias[i][0].audio_channel = 1;
			enc_info.enc_medias[i][0].bit_per_sample = 16;
			enc_info.enc_medias[i][0].samples_per_sec = 16000;
		}
		MppMdl::Instance()->SetEncInfo(enc_info);
		
		lv_init();
		lv_disp_drv_t* disp_drv = lv_port_disp_init();

		lv_port_indev_init();
		//设置视频输出区域

		//相关外设调用
		PeripheryManager::Instance()->Periphery();

		int hor_res = disp_drv->ver_res;
		int ver_res = disp_drv->hor_res;
		int physical_hor_res = disp_drv->physical_ver_res;
		int physical_ver_res = disp_drv->physical_hor_res;
		if(disp_drv->rotated == 0){
			hor_res = disp_drv->hor_res;
			ver_res = disp_drv->ver_res;
			physical_hor_res = disp_drv->physical_hor_res;
			physical_ver_res = disp_drv->physical_ver_res;			
		}
		
		MppMdl::Instance()->SetPreviewRegion(hor_res, ver_res, physical_hor_res, physical_ver_res);
		GlobalData::Instance()->g_pImgVirAttr = MppMdl::Instance()->GetImgVirAttr();

		//SD卡升级;检测可以升级就杀死当前应用程序然后在/var下重新运行应用程序并传入传输参数upgrade
		XMLogI("before XM_Middleware_SDCard_AlreadyUpgrade");		
		ret = XM_Middleware_SDCard_AlreadyUpgrade(kUpdateFileName);		
		if (!ret) {
			XM_Middleware_WDT_WatchDogStop();
			//升级处理
			XM_Middleware_SafeSystem("cp /usr/bin/car_demo /var/");
			char cmd_buf[50];
			sprintf(cmd_buf, "kill -15 %d ; /var/car_demo upgrade", getpid());
			XM_Middleware_SafeSystem(cmd_buf);
			return XM_SUCCESS;
		}
		else {
			osd_data_init();
			//Adjust_Screen_Effect();
			//设置UI界面操作事件回调函数，ui界面中开始存储、停止存储、查询查询、点播等操作，可以通过GlobalData::Instance()->ui_event_cb_回调出来
			GlobalData::Instance()->ui_event_cb_ = OnUIEventCallback;
			XM_Middleware_Storage_SetAlarmRecordDuration(15);
			//camera_set_mirror(1);
           // camera_set_flip(1);
			while(!MppMdl::Instance()->startup_sound_over_ && PlaySound_flag==true){
				static int cnt=0;
				usleep(100);
				//XMLogI("%d",cnt);
				if((cnt++)>100*4){
					break;
				}

			}
         //  usleep(500*1000);
		  
			//开启预览
			MppMdl::Instance()->StartPreview();
			Adjust_Screen_Effect();
			//MppMdl::Instance()->SetRealTimeStreamCallback(OnMppRealTimeStreamCallBack, 0);
		
			//GlobalData::Instance()->gdb_debug_ = true;
			// if (GlobalData::Instance()->gdb_debug_) {	
			// 	//gdb调试时，应用程序放在sd卡上，此时不要再去挂载sd卡
			// 	g_sd_status = XM_SD_NORMAL;
			// 	XM_Middleware_Storage_CheckFileSystem(XM_STORAGE_SDCard_0);
			// }
			// else {
			// 	XMLogI("before XM_Middleware_Storage_CheckSDCard");
				
			// }

			OS_THREAD check_sdcard;
			CreateThreadEx(check_sdcard, (LPTHREAD_START_ROUTINE)check_sdcard_thread, NULL);
			run_project();
		}

		lcd_vo_param vo_param = {0};
		//LIBCR_VO_SetParam(&vo_param);
		#if GPS_EN
		//串口GPS外设调用	
			UartGpsThreadStart();
		#endif 
	}
	//子进程升级
	else if (argc == 2) {
		XMLogI("upgrade start");
		//开启看门狗
		XM_Middleware_WDT_WatchDogStart(60);
		//读取配置文件
		GlobalData::Instance()->car_config()->ReadConfigFromFile();
		XM_CONFIG_VALUE cfg_value;
		//lvgl初始化
		lv_init();
		lv_disp_drv_t* disp_drv = lv_port_disp_init();
		//开机图片及lvgl图层设置
		int hor_res = disp_drv->ver_res;
		int ver_res = disp_drv->hor_res;
		int physical_hor_res = disp_drv->physical_ver_res;
		int physical_ver_res = disp_drv->physical_hor_res;
		if(disp_drv->rotated == 0){
			hor_res = disp_drv->hor_res;
			ver_res = disp_drv->ver_res;
			physical_hor_res = disp_drv->physical_hor_res;
			physical_ver_res = disp_drv->physical_ver_res;			
		}
		MppMdl::Instance()->SetPreviewRegion(hor_res, ver_res, physical_hor_res, physical_ver_res);
		GlobalData::Instance()->g_pImgVirAttr = MppMdl::Instance()->GetImgVirAttr();

		//SD卡升级
		XMLogI("before XM_Middleware_SDCard_Upgrade");
		GlobalData::Instance()->upgrading_ = true;
		g_mw_event_infos.clear();
		cfg_value.int_value = SimpChinese;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Language, cfg_value);
		set_language(cfg_value.int_value);
		GlobalPage::Instance()->page_sys_set()->StartUpgrade();	
	}

	OS_THREAD tick;
	CreateThreadEx(tick, (LPTHREAD_START_ROUTINE)timer_hander_thread, NULL);
	int64_t dur = 0;
	while (1) {
		int64_t t1 = GetTickTime();
		usleep(1 * 1000);
		int64_t t2 = GetTickTime();
		dur = t2 - t1;
		if (dur > 0 && dur < 100) {
			lv_tick_inc(dur);
		}
	}
	return XM_SUCCESS;
}
