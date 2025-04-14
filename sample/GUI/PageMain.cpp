#include "global_data.h"
#include "global_page.h"
#include "PageMain.h"
#include "TimeUtil.h"
#include "periphery/PeripheryManager.h"
#include "periphery/power.h"
#include "xm_middleware_network.h"
#include "network/HTTPSerMdl.h"
#include "network/WifiOpr.h"
#include <time.h>
#include "log/LogFileManager.h"
#include "mpp/MppMdl.h"
#include "CommDef.h"
#include "PageSet.h"
#include "osd_user.h"
#include "periphery/PeripheryThread.h"

LV_FONT_DECLARE(lv_font_montserrat_30);
extern int g_sd_status;
extern int g_engineId;
extern bool g_app_connect;
extern bool g_realplay;
extern void OnNetworkMsg(int engineId, int connId, uint8_t type, char* msg, char* connType, int contentLen);
extern void closeThread(int connId);
extern void lv_ignore_signals();

PageMain::PageMain():pre_date_(0), record_time_label_(NULL), record_timer_(NULL), record_start_time_(0),
	rest_screen_(false), rest_screen_timer_(NULL), SD_img_(NULL),
	close_rest_screen_fun_(false), acc_disconnect_(true), acc_disconnect_timer_(NULL), acc_connect_timer_(NULL),
	pre_time_(0), pre_record_time_(0), update_time_(true), pre_camera_status_(XM_PLAY_SENSOR), collision_startup_(false),
	reverse_line_img_(NULL), lock_current_recording_file_(false), car_charger_(true), battery_powered_(true),
	led_on_(true), battery_img_(NULL), connect_computer_(false), check_battery_(true), battery_level_(221), pre_sys_time_(0),
	wifi_enable_(false), wifi_id_label_(NULL), wifi_prepared_(false), app_disconnect_timer_(NULL),touch_plan(false),wifi_tip_win_(NULL)
{

}

PageMain::~PageMain()
{

}

void PageMain::OpenPage()
{
	CreatePage();
}

void PageMain::CreatePage()
{
	main_page_ = lv_create_page(lv_scr_act(), screen_width, screen_height, lv_color_black(), 0, 0,
		lv_font_all, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(main_page_, 0, 0);
	lv_obj_set_pos(main_page_, 0, start_y);

	mode_label_ = lv_label_create(main_page_);
	lv_label_set_text(mode_label_, "\uF03D");
	lv_obj_align(mode_label_, LV_ALIGN_TOP_LEFT, size_w(10), size_h(10));
	lv_obj_add_flag(mode_label_, LV_OBJ_FLAG_HIDDEN);

	record_resolution_label_ = lv_label_create(main_page_);
	XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value = 1080;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Resolution, cfg_value);
		if (cfg_value.int_value == k4KHeight) {
		// if(!MppMdl::Instance()->AdLoss())
		// 	lv_label_set_text(record_resolution_label_, "2K");
		// else
		 	lv_label_set_text(record_resolution_label_, "4K");
	}
	else if (cfg_value.int_value == k3KHeight) {
		// if(!MppMdl::Instance()->AdLoss())
		// 	lv_label_set_text(record_resolution_label_, "2K");
		// else
			lv_label_set_text(record_resolution_label_, "3K");
	}
	else if (cfg_value.int_value == 1440) {
		lv_label_set_text(record_resolution_label_, "2K");
	}
	else if (cfg_value.int_value == 1080) {
		lv_label_set_text(record_resolution_label_, "1080P");
	}
	lv_obj_set_style_text_font(record_resolution_label_, &lv_font_montserrat_16, 0);
	lv_obj_align(record_resolution_label_, LV_ALIGN_TOP_LEFT, size_w(100), size_h(7));

	time_t t = time(NULL);
	pre_sys_time_ = t;
	struct tm* current_time = localtime(&t);

	pre_date_ = current_time->tm_year + current_time->tm_mon + current_time->tm_mday;
	date_label_ = lv_label_create(main_page_);
	lv_label_set_text_fmt(date_label_, "%04d-%02d-%02d", current_time->tm_year + 1900, current_time->tm_mon + 1, current_time->tm_mday);
    lv_obj_set_style_text_font(date_label_, &lv_font_montserrat_16, 0);
	lv_obj_align(date_label_, LV_ALIGN_BOTTOM_LEFT, size_w(10), -size_h(5));

	pre_time_ = current_time->tm_hour + current_time->tm_min + current_time->tm_sec;
	time_label_ = lv_label_create(main_page_);
	lv_label_set_text_fmt(time_label_, "%02d:%02d:%02d", current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
	 lv_obj_set_style_text_font(time_label_, &lv_font_montserrat_16, 0);
	lv_obj_align_to(time_label_, date_label_, LV_ALIGN_OUT_RIGHT_MID, size_w(20), 0);

	battery_img_ = lv_img_create(main_page_);
    if(usb_status()){
     lv_img_set_src(battery_img_, image_path"i_char.png");
	}else{
	int battery_level = PeripheryManager::Instance()->GetAdcBattery();
	battery_level_ = battery_level;
	XMLogI("battery_level_ = %d", battery_level_);
	if (battery_level >= 180) {
		lv_img_set_src(battery_img_, image_path"i_bat_ful.png");
	}
	else if (battery_level >= 172 && battery_level <= 179) {
		lv_img_set_src(battery_img_, image_path"i_bat_h.png");
	}
	else if (battery_level >= 167 && battery_level <= 171) {
		lv_img_set_src(battery_img_, image_path"i_bat_half.png");
	}
	else if (battery_level <= 166) {
		lv_img_set_src(battery_img_, image_path"i_bat_0.png");
	}
   }
	lv_obj_align(battery_img_, LV_ALIGN_TOP_RIGHT, -size_w(5), size_h(7));

	/*circular_record_time_label_ = lv_label_create(main_page_);
	UpdateCirRecordTimeLabel();*/

	SDCardImg();


	// logo_label_ = lv_label_create(main_page_);
	// lv_label_set_text(logo_label_, "ssontong");
	// lv_obj_align(logo_label_, LV_ALIGN_BOTTOM_RIGHT, -size_w(20), -size_h(20));

	silent_record_img_ = lv_img_create(main_page_);
	mic_record_img_ = lv_img_create(main_page_);
	cfg_value.bool_value = false;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Record_Voice, cfg_value);
	if (cfg_value.bool_value) {
		lv_img_set_src(silent_record_img_, image_path"mic_on.png");
		lv_img_set_src(mic_record_img_,image_path"i_aud_o.png");
	}
	else {
		lv_img_set_src(silent_record_img_, image_path"mic_off.png");
		lv_img_set_src(mic_record_img_,image_path"i_aud.png");
	}
	lv_obj_align(silent_record_img_, LV_ALIGN_BOTTOM_LEFT, size_w(60), -size_h(20));
    lv_obj_align_to(mic_record_img_, SD_img_, LV_ALIGN_OUT_LEFT_MID, size_w(0), size_h(0)); 
	lv_obj_add_flag(silent_record_img_, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_event_cb(silent_record_img_, BtnEvent, LV_EVENT_ALL, (void*)RecordPageBtnFlag_RecordSound);
	lv_obj_set_ext_click_area(silent_record_img_, 20);

	g_sensor_img_ = lv_img_create(main_page_);
	lv_img_set_src(g_sensor_img_, image_path"G-sensor.png");
	lv_obj_align_to(g_sensor_img_,mic_record_img_, LV_ALIGN_OUT_LEFT_MID,size_w(0), size_h(0));
	UpdateGsensorImg();

	collision_startup_img_ = lv_img_create(main_page_);
	lv_img_set_src(collision_startup_img_, image_path"i_park.png");
	lv_obj_align_to(collision_startup_img_, g_sensor_img_, LV_ALIGN_OUT_LEFT_MID, size_w(0), size_h(0));
	UpdateCollisionStartupImg();

	photo_img_ = lv_img_create(main_page_);
	lv_img_set_src(photo_img_, image_path"photo_on.png");
	lv_obj_align_to(photo_img_, silent_record_img_, LV_ALIGN_OUT_RIGHT_MID, size_w(58), 0);
	lv_obj_add_flag(photo_img_, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_event_cb(photo_img_, BtnEvent, LV_EVENT_ALL, (void*)RecordPageBtnFlag_PlayPthoto);
	lv_obj_set_ext_click_area(photo_img_, 20);

	record_img_ = lv_img_create(main_page_);
	lv_img_set_src(record_img_, image_path"rec_off.png");
	lv_obj_align_to(record_img_, photo_img_, LV_ALIGN_OUT_RIGHT_MID, size_w(58), 0);
	lv_obj_add_flag(record_img_, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_event_cb(record_img_, BtnEvent, LV_EVENT_ALL, (void*)RecordPageBtnFlag_Record);
	lv_obj_set_ext_click_area(record_img_, 20);

	set_img_ = lv_img_create(main_page_);
	lv_img_set_src(set_img_, image_path"set_on.png");
	lv_obj_align_to(set_img_, record_img_, LV_ALIGN_OUT_RIGHT_MID, size_w(58), 0);
	lv_obj_add_flag(set_img_, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_event_cb(set_img_, BtnEvent, LV_EVENT_ALL, (void*)RecordPageBtnFlag_Set);
	lv_obj_set_ext_click_area(set_img_, 20);

	playback_img_ = lv_img_create(main_page_);
	lv_img_set_src(playback_img_, image_path"playback_on.png");
	lv_obj_align_to(playback_img_, set_img_, LV_ALIGN_OUT_RIGHT_MID, size_w(58), 0);
	lv_obj_add_flag(playback_img_, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_event_cb(playback_img_, BtnEvent, LV_EVENT_ALL, (void*)RecordPageBtnFlag_Playback);
	lv_obj_set_ext_click_area(playback_img_, 20);

	lock_img_ = lv_img_create(main_page_);
	lv_img_set_src(lock_img_, image_path"lock_off.png");
	lv_obj_align_to(lock_img_, playback_img_, LV_ALIGN_OUT_RIGHT_MID, size_w(58), 0);
	lv_obj_add_flag(lock_img_, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_event_cb(lock_img_, BtnEvent, LV_EVENT_ALL, (void*)RecordPageBtnFlag_Lock);
	lv_obj_set_ext_click_area(lock_img_, 20);
	
    lock_png_ = lv_img_create(main_page_);
	lv_img_set_src(lock_png_, image_path"lock.png");
	lv_obj_align(lock_png_, LV_ALIGN_TOP_MID, size_w(2), size_h(3)); 
	lv_obj_add_flag(lock_png_, LV_OBJ_FLAG_HIDDEN);

  if(!touch_plan){
      lv_obj_add_flag(silent_record_img_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(photo_img_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(record_img_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(set_img_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(playback_img_, LV_OBJ_FLAG_HIDDEN);
      lv_obj_add_flag(lock_img_, LV_OBJ_FLAG_HIDDEN);
 }

     cfg_value.int_value = 0;
	 
	// GlobalData::Instance()->car_config()->SetValue(CFG_Operation_WiFi, cfg_value);
     GlobalData::Instance()->car_config()->GetValue(CFG_Operation_WiFi, cfg_value);
	 wifi_img_ = lv_img_create(main_page_);
     if (cfg_value.int_value==1) {
            lv_img_set_src(wifi_img_, image_path"0wifi.png");
     }
     else {
         lv_img_set_src(wifi_img_, image_path"no_wifi.png");
     }

	lv_obj_align(wifi_img_, LV_ALIGN_BOTTOM_RIGHT, -size_w(10), -size_h(3));
	lv_obj_add_flag(wifi_img_, LV_OBJ_FLAG_CLICKABLE);
	lv_obj_add_event_cb(wifi_img_, BtnEvent, LV_EVENT_ALL, (void*)RecordPageBtnFlag_Wifi);
	lv_obj_set_ext_click_area(wifi_img_, 20);
	//lv_obj_add_flag(wifi_img_, LV_OBJ_FLAG_HIDDEN);

	//录像界面水印
	// x2_logo_img_ = lv_img_create(main_page_);
	// lv_img_set_src(x2_logo_img_, image_path"Safe_cam.png");
	// lv_obj_align(x2_logo_img_, LV_ALIGN_BOTTOM_LEFT, size_w(10), -size_h(0));

	//开机开启录像
	collision_startup_ = PeripheryManager::Instance()->GsensorCheck(); 
	XMLogI("GsensorCheck collision_startup_=%d", collision_startup_);
	//RecordOnStartup();
	//时间定时器
	lv_timer_t* time_timer = lv_timer_create(UpdateTime, 100, NULL);
}

void PageMain::RecordOnStartup()
{
	if (!connect_computer_) {
		if (collision_startup_) {
			if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
				lv_img_set_src(lock_img_, image_path"lock_on.png");
				lv_obj_clear_flag(lock_png_, LV_OBJ_FLAG_HIDDEN);
				XM_Middleware_Storage_CollisionRecord(-1);
				lock_current_recording_file_ = true;
				Voice_prompts("Video locked_16k.pcm");
				StartRecord();
				RED_ON;
				if (record_label_) {
					XMLogI("record_label, yellow");
					lv_img_set_src(record_label_, image_path"yellow_circle.png");
				}
			}
			else {
				int collision_startup = 1;
				lv_timer_create(DelayShutDownTimer, 2000, (void*)collision_startup);
			}
		}
		else {
			StartRecord();
		}
	}
}

void PageMain::BtnEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		PageMain* page_main = GlobalPage::Instance()->page_main();
		int user_data = (int)lv_event_get_user_data(e);

		if (GlobalData::Instance()->UI_mode_ == UIMode_CompactRecord) {
			page_main->OpenTipBox("Retracting video recording in progress");
			return;
		}
		
		XM_CONFIG_VALUE cfg_value;
		if (user_data == RecordPageBtnFlag_RecordSound) {
			GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Record_Voice, cfg_value);
			GlobalPage::Instance()->page_set()->RecordVoiceEnable(!cfg_value.bool_value);
		}
		else if (user_data == RecordPageBtnFlag_PlayPthoto) {
			if (GlobalData::Instance()->UI_mode_ == UIMode_Photograph) {
				page_main->CheckSDStatus();
				if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
					page_main->TakePic();
				}
			}
			else if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape) {
				if (page_main->record_timer_ == NULL) {
					page_main->ChangeMode(UIMode_Photograph);
				}
				else {
					page_main->CheckSDStatus();
					if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
						page_main->TakePic();
					}
				}
			}
		}
		else if (user_data == RecordPageBtnFlag_Record) {
			if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape) {
				page_main->CheckSDStatus();
				if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
					if (page_main->record_timer_ == NULL) {
						page_main->StartRecord();
					}
					else {
						page_main->CloseRecord();
					}
				}
			}
			else if (GlobalData::Instance()->UI_mode_ == UIMode_Photograph) {
				page_main->ChangeMode(UIMode_Videotape);
			}
		}
		else if (user_data == RecordPageBtnFlag_Set) {
			page_main->LeaveMainPageWork(true);
			GlobalPage::Instance()->page_set()->OpenPage();
			page_main->CloseRecord();
		}
		else if (user_data == RecordPageBtnFlag_Playback) {
			page_main->CheckSDStatus(true, false, false, false);
			if (g_sd_status == XM_SD_NORMAL || g_sd_status == XM_SD_NEEDCHECKSPEED || g_sd_status == XM_SD_NOSPACE) {
				page_main->LeaveMainPageWork(true);
				GlobalPage::Instance()->page_playback()->OpenSelectFilePage();
				page_main->CloseRecord();
			}
		}
		else if (user_data == RecordPageBtnFlag_Lock) {
			page_main->LockCurrentFile();
		}
		else if (user_data == RecordPageBtnFlag_Wifi) {
           page_main->WifiEnable(!page_main->wifi_enable_);
		}
	}
}

int PageMain::WifiEnable(bool enable)
{
	XMLogI("WifiEnable enable = %d", enable);
	if (wifi_enable_ == enable) {
		XMLogE("WifiEnable error");
		return -1;
	}

	if(g_app_connect){  //连接APP状态下，wifi功能禁止关闭，且添加提示语
	GlobalPage::Instance()->page_main()->OpenTipBox("Please exit the mobile app first");
	return -1;
	}


	wifi_enable_ = enable;
	if (enable) {
		GlobalData::Instance()->app_page_ = APP_PAGE_PREVIEW;
		lv_img_set_src(wifi_img_, image_path"0wifi.png");

		
		// if (wifi_id_label_ == NULL && wifi_password_label_==NULL) {
		// 	WIFI_MAC_PARAM_S wifi_param;
		// 	memset(wifi_param.ssid, 0, sizeof(wifi_param.ssid));
		// 	memset(wifi_param.ssid, 0, sizeof(wifi_param.password));
		// 	memset(wifi_param.mac, 0, sizeof(wifi_param.mac));
		// 	XM_Middleware_WIFI_GetMacParam(&wifi_param);
			
		// 	wifi_id_label_ = lv_label_create(main_page_);
		// 	lv_label_set_text_fmt(wifi_id_label_, "Wifi ssid: ""%s",wifi_param.ssid);
		// 	 if(!touch_plan){
		// 	  lv_obj_align(wifi_id_label_, LV_ALIGN_BOTTOM_LEFT, size_w(10), -size_h(40));
		// 	 }else{
        //       lv_obj_align(wifi_id_label_, LV_ALIGN_BOTTOM_LEFT, size_w(10), -size_h(80));
		// 	 }
		// 	wifi_password_label_ = lv_label_create(main_page_);
		// 	lv_label_set_text_fmt(wifi_password_label_, "Wifi password: ""%s",wifi_param.password);
		// 	lv_obj_align_to(wifi_password_label_,wifi_id_label_, LV_ALIGN_BOTTOM_LEFT, size_w(0), size_h(25));
		// }
		XM_Middleware_WIFI_WifiEnable(true);
		MppMdl::Instance()->SubStreamEnable(false);
		//开启server
		XM_Middleware_Network_SetDataCallback(OnNetworkMsg);
		XM_Middleware_Network_CloseThreadCallback(closeThread);

		//GlobalPage::Instance()->page_main()->AppMenuValue();
		
		XM_Middleware_Network_StartServer(80, 2222);
		lv_ignore_signals();
	}
	else {
		XM_Middleware_Network_StopServer();
		XM_Middleware_RTSP_Stop();
		g_realplay = false;
		MppMdl::Instance()->SubStreamEnable(false);

		if (app_disconnect_timer_) {
			lv_timer_del(app_disconnect_timer_);
			app_disconnect_timer_ = NULL;
			XM_CONFIG_VALUE cfg_value;
	        cfg_value.bool_value = false;
	        GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
	        XM_Middleware_Smart_EnableKeyword(cfg_value.bool_value);
		}
		g_app_connect = false;
		GlobalData::Instance()->app_page_ = APP_PAGE_PREVIEW;

		if (wifi_id_label_) {
			lv_obj_del(wifi_id_label_);
			wifi_id_label_ = NULL;
		}
		if (wifi_password_label_) {
			lv_obj_del(wifi_password_label_);
			wifi_password_label_ = NULL;
		}

		//if (record_time_label_) {
		//	lv_obj_align(record_time_label_, LV_ALIGN_TOP_LEFT, size_w(150), size_h(10));
		//}

		//for (int i = 0;i < lv_obj_get_child_cnt(main_page_);i++) {
		//	lv_obj_t* child_obj = lv_obj_get_child(main_page_, i);
		//	if (child_obj == mode_label_) {
		//		continue;
		//	}
  //       if(!touch_plan){
  //          if (child_obj == silent_record_img_  || child_obj == photo_img_  || child_obj == record_img_  || child_obj == set_img_ 
		//	 ||child_obj == playback_img_  || child_obj == lock_img_  || child_obj == lock_png_) {
		//		continue;
		//	}
		//   }
		//	lv_obj_clear_flag(child_obj, LV_OBJ_FLAG_HIDDEN);
		//}
		UpdateGsensorImg();
		UpdateCollisionStartupImg();
		lv_img_set_src(wifi_img_, image_path"no_wifi.png");
		XM_Middleware_WIFI_WifiEnable(false);
		GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
	}
	XM_CONFIG_VALUE cfg_value;
    cfg_value.int_value = enable;
    GlobalData::Instance()->car_config()->SetValue(CFG_Operation_WiFi, cfg_value);
}


void PageMain::AppMenuValue(void)
{

//传递初始化状态
		APP_PARAM_ITEMS_S param_items;
		param_items.record = (record_time_label_ == NULL) ? 0 : 1;
		param_items.sdcard_speed = GlobalData::Instance()->SDCard_write_speed_;
		XM_CONFIG_VALUE cfg_value;
		cfg_value.bool_value = true;

        param_items.ir_mode = -1;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Date_Watermark, cfg_value);
		param_items.osd_time = cfg_value.bool_value ? 1 : 0;
		
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Record_Voice, cfg_value);
		param_items.mic = cfg_value.bool_value ? 1 : 0;
      #if VOICE_CONTROL_EN
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
		param_items.voice_control = cfg_value.bool_value ? 1 : 0;
      #else
        param_items.voice_control = -1;
	  #endif
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Resolution, cfg_value);
		param_items.resolution = cfg_value.int_value;

		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Circular_Record_Time, cfg_value);
		param_items.circular_record_time = cfg_value.int_value;

		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Lcd_OffTime, cfg_value);
		param_items.screen_standby = cfg_value.int_value / 60;
#if G_SENSOR_EN
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Startup, cfg_value);
		param_items.parking_monitor = cfg_value.bool_value ? 1 : 0;

		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Sensitivity, cfg_value);
		if (cfg_value.int_value == Sensitivity_Close)
			param_items.gsr_sensitivity = 0;
		else if (cfg_value.int_value == Sensitivity_Low)
			param_items.gsr_sensitivity = 1;
		else if (cfg_value.int_value == Sensitivity_Mid)
			param_items.gsr_sensitivity = 2;
		else if (cfg_value.int_value == Sensitivity_High)
			param_items.gsr_sensitivity = 3;
#else
       param_items.parking_monitor =-1;
	   param_items.gsr_sensitivity=-1;
#endif

		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Key_Voice, cfg_value);
		param_items.key_tone = cfg_value.bool_value;

#if COMPACT_RECORD_EN
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Duration, cfg_value);
		param_items.park_record_time = cfg_value.int_value;

        GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Fps, cfg_value);
		param_items.timelapse_rate = cfg_value.int_value;
#else
       param_items.park_record_time = -1;
	   param_items.timelapse_rate = -1;
#endif

		strncpy(param_items.version, GlobalData::Instance()->version().c_str(), sizeof(param_items.version) - 1);

		MppMdl::Instance()->SubStreamEnable(true);

			HTTPSerMdl::Instance()->CfgInit(&param_items);
}	


void PageMain::AppDisconnectTimer(lv_timer_t* timer)
{
	XMLogW("App disconnect!");
	MppMdl::Instance()->SubStreamEnable(false);
	g_app_connect = false;
	HTTPSerMdl::Instance()->CloseAll();
	XM_Middleware_RTSP_Stop();
    lv_img_set_src(GlobalPage::Instance()->page_main()->wifi_img_, image_path"0wifi.png");
	GlobalData::Instance()->app_page_ = APP_PAGE_PREVIEW;
	if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape
		&& !lv_obj_has_flag(GlobalPage::Instance()->page_main()->main_page_, LV_OBJ_FLAG_HIDDEN)) {
		GlobalPage::Instance()->page_main()->StartRecord();
	}

	XM_CONFIG_VALUE cfg_value;
	cfg_value.bool_value = false;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
	XM_Middleware_Smart_EnableKeyword(cfg_value.bool_value);
	GlobalPage::Instance()->page_main()->app_disconnect_timer_ = NULL;
	GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
	lv_timer_del(timer);
}

void PageMain::UpdateCirRecordTimeLabel()
{
	XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value = 1;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Circular_Record_Time, cfg_value);
	char buf[16] = { 0 };
	if (cfg_value.int_value > 1) {
		sprintf(buf, "%d%s", cfg_value.int_value, GetParsedString("minutes"));
	}
	else {
		sprintf(buf, "%d%s", cfg_value.int_value, GetParsedString("minute"));
	}
	lv_label_set_text(circular_record_time_label_, buf);
	lv_obj_align_to(circular_record_time_label_, record_resolution_label_, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, size_h(20));
}

void PageMain::UpdateGsensorImg()
{
	XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value = Sensitivity_Close;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Sensitivity, cfg_value);	
	if (cfg_value.int_value == Sensitivity_Close) {
		lv_obj_add_flag(g_sensor_img_, LV_OBJ_FLAG_HIDDEN);
	}
	else {
		lv_obj_clear_flag(g_sensor_img_, LV_OBJ_FLAG_HIDDEN);
	}
}

void PageMain::UpdateCollisionStartupImg()
{
	XM_CONFIG_VALUE cfg_value;
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Startup, cfg_value);
	if (cfg_value.bool_value) {
		lv_obj_clear_flag(collision_startup_img_, LV_OBJ_FLAG_HIDDEN);
	}
	else {
		lv_obj_add_flag(collision_startup_img_, LV_OBJ_FLAG_HIDDEN);
	}
}

void PageMain::SDCardImg()
{
	if (SD_img_ == NULL) {
		SD_img_ = lv_img_create(main_page_);
		if(g_sd_status == XM_SD_NOEXIST)//XM_SD_NORMAL
			lv_img_set_src(SD_img_, image_path"TFcard_No.png");
		else
			lv_img_set_src(SD_img_, image_path"TFcard.png");
		lv_obj_align_to(SD_img_, battery_img_, LV_ALIGN_OUT_LEFT_MID, -size_w(0), 0);
	}
	else {
		if (g_sd_status == XM_SD_NOEXIST) {//XM_SD_NORMAL
			if(strcmp((char*)lv_img_get_src(SD_img_), image_path"TFcard_No.png") != 0)
				lv_img_set_src(SD_img_, image_path"TFcard_No.png");
		}
		else {
			if (strcmp((char*)lv_img_get_src(SD_img_), image_path"TFcard.png") != 0)
				lv_img_set_src(SD_img_, image_path"TFcard.png");
		}
	}

	if (remain_time_label_ != NULL && g_sd_status == XM_SD_NOEXIST) {
            lv_obj_del(remain_time_label_);
            remain_time_label_ = NULL;
     }
}
void PageMain::UpdateTime(lv_timer_t* timer)
{
	//XMLogI("-------------------------time_timer------------------------");
	PageMain* object = GlobalPage::Instance()->page_main();
	static int lower_flag=0;
	static int usb_status_flag=0;
	static int usb_status_curren=0;
#if 1
	static int wifi_sta_mode=0;
	static int time_cnt=0;
	FILE *file=NULL;
	if(time_cnt<10*10){
      time_cnt++;
	}
	if(!wifi_sta_mode&&time_cnt==10*10){
		file = fopen("/mnt/tfcard/wpa.conf","r");
		wifi_sta_mode=-1;
		if(file){
		XMLogI("wpa.conf open success");	
		wifi_sta_mode=1;
				std::string b("wpa_supplicant -ieth2 -Dnl80211 -c /mnt/tfcard/wpa.conf -B");
				system(b.c_str());
				fclose(file);
		}
	}
  if(time_cnt==10*9){
	file = fopen("/mnt/tfcard/iperf","r");
		if(file){
		//std::string b("/usr/bin/iperf -s &");
		std::string b("/mnt/tfcard/iperf -s &");
		system(b.c_str());
		fclose(file);
		}
  }
#endif
	if (object->update_time_) {
		time_t t = time(NULL);
		struct tm* current_time = localtime(&t);

		if (t > object->pre_sys_time_) {
			object->pre_sys_time_ = t;
			int time = current_time->tm_hour + current_time->tm_min + current_time->tm_sec;
			if (time != object->pre_time_) {
				if (!object->wifi_prepared_) {
					XMSDK_NET_ATTR_S pstNetAttr;
					int ret = XM_Middleware_WIFI_GetEthAttr("eth2", &pstNetAttr);
					if (!ret) {
						object->wifi_prepared_ = true;
						//if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape || GlobalData::Instance()->UI_mode_ == UIMode_CompactRecord)
							//lv_obj_clear_flag(object->wifi_img_, LV_OBJ_FLAG_HIDDEN);
							//初次升级需要生成wifi配置文件
						WIFI_PARAM_S wifi_param = {0};
						char id[5] = { 0 };
						strncat(id, &pstNetAttr.mac[12], 2);
						strncat(id, &pstNetAttr.mac[15], 2);
						//默认wifi名为X2C3加mac地址后4位，密码是1234567890
						std::string wifi_name = " X2V50_";//"X27W_";
						wifi_name += id;
						strcpy(wifi_param.name, wifi_name.c_str());
						strcpy(wifi_param.password, "12345678");
						XM_Middleware_WIFI_CreateWifiParamFile(&wifi_param);

						 XM_CONFIG_VALUE cfg_value;
                         cfg_value.int_value = 0;
                         GlobalData::Instance()->car_config()->GetValue(CFG_Operation_WiFi, cfg_value);
						 if(cfg_value.int_value ==1){
                          object->WifiEnable (true);
						 }
					}
				}
				
				object->pre_time_ = time;
				lv_label_set_text_fmt(object->time_label_, "%02d:%02d:%02d",
					current_time->tm_hour, current_time->tm_min, current_time->tm_sec);
			}

			int date = current_time->tm_year + current_time->tm_mon + current_time->tm_mday;
			if (date != object->pre_date_) {
				object->pre_date_ = date;
				lv_label_set_text_fmt(object->date_label_, "%04d-%02d-%02d",
					current_time->tm_year + 1900, current_time->tm_mon + 1, current_time->tm_mday);
			}
		}
	}
	if(object->battery_img_){
       usb_status_curren=usb_status();
         if(usb_status_curren){
			if(usb_status_flag!=usb_status_curren){
              	usb_status_flag=usb_status_curren;
			   lv_img_set_src(object->battery_img_, image_path"i_char.png");
			}
			object->battery_level_=255;
       }else{
		usb_status_flag=0;
		int battery_level = PeripheryManager::Instance()->GetAdcBattery();
		if (object->battery_level_ != battery_level && object->battery_level_ > battery_level) {
			object->battery_level_ = battery_level;
			XMLogI("battery_level_ = %d", object->battery_level_);

			if (battery_level >= 180) {
				if (strcmp((char*)lv_img_get_src(object->battery_img_), image_path"i_bat_ful.png"))
					lv_img_set_src(object->battery_img_, image_path"i_bat_ful.png");
			}
			else if (battery_level >= 172 && battery_level <= 179) {
				if (strcmp((char*)lv_img_get_src(object->battery_img_), image_path"i_bat_h.png"))
					lv_img_set_src(object->battery_img_, image_path"i_bat_h.png");
			}
			else if (battery_level >= 167 && battery_level <= 171) {
				if (strcmp((char*)lv_img_get_src(object->battery_img_), image_path"i_bat_half.png"))
					lv_img_set_src(object->battery_img_, image_path"i_bat_half.png");
			}
			else if (battery_level <= 166) {
				if (strcmp((char*)lv_img_get_src(object->battery_img_), image_path"i_bat_0.png"))
					lv_img_set_src(object->battery_img_, image_path"i_bat_0.png");
				if (battery_level <=164 && lower_flag==0){
					lower_flag=1;
					object->OpenTipBox("The battery is low and will be shut down");
				//当小于162时，power.cpp中会检测并关机
			  }
			}
		}
	  }
	}
}

void PageMain::OpenTipBox(const char* string, uint32_t timer_period, bool switch_string, int delay_time, int w, int h)
{
if (delay_time > 0) {
		int size = strlen(string) + 1;
		char* s = (char*)malloc(size);
		memset(s, 0, size);
		strncpy(s, string, strlen(string));
		lv_timer_create(OpenTipBoxTimer, delay_time, (void*)s);
		return;
	}

	lv_obj_t* tip_box = lv_create_page(lv_scr_act(), size_w(w), size_h(h),
        lv_color_make(99, 99, 99), 0, 2, lv_font_all, lv_color_white(), 0);
	lv_obj_align(tip_box, LV_ALIGN_CENTER, 0, 0);

    lv_obj_set_style_border_color(tip_box, lv_color_make(99, 99, 99), 0);
    lv_obj_set_style_radius(tip_box, 10, 0);
#if 0
	lv_obj_t* label = lv_create_label(tip_box, size_w(w), GetParsedString("Tip"),
		LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, size_h(15));

	static lv_point_t line_points[] = { {0, 0}, {size_w(w), 0} };
	lv_obj_t* line = lv_create_line(tip_box, 3, lv_color_make(66, 66, 66), 0, line_points, 2, 0);
	lv_obj_align_to(line, label, LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(0));
#else
    lv_obj_t* label = lv_create_image(tip_box, size_w(28), size_w(28), image_path"msgwarn1.png");
        lv_obj_align(label, LV_ALIGN_TOP_MID, 0, size_h(6));
#endif
	if (switch_string) {
		label = lv_create_label(tip_box, size_w(w), GetParsedString(string),
			LV_TEXT_ALIGN_CENTER, 0);
	}
	else {
		label = lv_create_label(tip_box, size_w(w), string,
			LV_TEXT_ALIGN_CENTER, 0);
	}
	
	lv_obj_align(label, LV_ALIGN_CENTER, 0, size_h(14));

	if (timer_period > 0)
		lv_timer_t* tip_timer = lv_timer_create(CloseTipBox, timer_period, (void*)tip_box);
}

void PageMain::OpenTipBoxTimer(lv_timer_t* timer)
{
	char* user_data = (char*)timer->user_data;
	GlobalPage::Instance()->page_main()->OpenTipBox(user_data);
	free(user_data);
	lv_timer_del(timer);
}

void PageMain::CloseTipBox(lv_timer_t* timer)
{
	lv_obj_t* user_data = (lv_obj_t*)timer->user_data;
	lv_obj_del(user_data);
	lv_timer_del(timer);
}

void PageMain::WiFi_switch(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        int user_data = (int)lv_event_get_user_data(e);
        PageMain* object = GlobalPage::Instance()->page_main();
        if (user_data == Btn_Yes) {
            object->WifiEnable(false);

        }
        else if (user_data == Btn_No) {

        }
        if (object->wifi_tip_win_) {
            lv_obj_del(object->wifi_tip_win_);
            object->wifi_tip_win_ = NULL;
        }
    }
}

void PageMain::WIFITipWin()
{
	if (wifi_tip_win_==NULL) {
    wifi_tip_win_ = lv_create_page(lv_scr_act(), 400, 200, lv_palette_darken(LV_PALETTE_GREY, 2), 20, 2,
        lv_font_all, lv_color_white(), 0);
    lv_obj_align(wifi_tip_win_, LV_ALIGN_CENTER, 0, 0);

    lv_obj_t* label = lv_create_label(wifi_tip_win_, 500, GetParsedString("Please turn off WiFi first"), LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_TOP_MID, 0, size_h(30));

    lv_obj_t* yes_btn = lv_create_btn(wifi_tip_win_, size_w(160), size_h(50),
        lv_color_make(85, 85, 85), 10, GlobalPage::Instance()->page_main()->WiFi_switch, LV_EVENT_ALL, (void*)Btn_Yes, 0);
    /*lv_obj_set_style_border_width(yes_btn, 2, 0);
    lv_obj_set_style_border_color(yes_btn, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), 0);*/
    lv_obj_align(yes_btn, LV_ALIGN_BOTTOM_LEFT, size_w(30), -size_h(20));

    lv_obj_t* btn_label = lv_label_create(yes_btn);
    lv_label_set_text(btn_label, GetParsedString("Confirm"));
    lv_obj_center(btn_label);

    lv_obj_t* no_btn = lv_create_btn(wifi_tip_win_, size_w(160), size_h(50),
        lv_color_make(85, 85, 85), 10, WiFi_switch, LV_EVENT_ALL, (void*)Btn_No, 0);
    /*lv_obj_set_style_border_width(no_btn, 2, 0);
    lv_obj_set_style_border_color(no_btn, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), 0);*/
    lv_obj_align(no_btn, LV_ALIGN_BOTTOM_RIGHT, -size_w(30), -size_h(20));

    btn_label = lv_label_create(no_btn);
    lv_label_set_text(btn_label, GetParsedString("Cancel"));
    lv_obj_center(btn_label);
  }
}

void PageMain::StartRecord(bool compact_record)
{
	 if (remain_time_label_ != NULL) {
        lv_obj_del(remain_time_label_);
        remain_time_label_ = NULL;
    }
	if (record_time_label_ == NULL && g_sd_status == XM_SD_NORMAL
		&& GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
		if (g_app_connect && GlobalData::Instance()->app_page_ != APP_PAGE_PREVIEW)
			return;

        record_label_ = lv_img_create(main_page_);
		lv_img_set_src(record_label_, image_path"red_circle.png");
        lv_obj_align(record_label_, LV_ALIGN_TOP_LEFT, size_w(10), size_h(2));


		record_time_label_ = lv_label_create(main_page_);
		lv_label_set_text(record_time_label_, "00:00");
		lv_obj_set_style_text_font(record_time_label_, &lv_font_montserrat_16, 0);
        lv_obj_align_to(record_time_label_, record_label_, LV_ALIGN_OUT_RIGHT_MID, size_w(5), size_h(0));

		lv_img_set_src(record_img_, image_path"rec_on.png");

		if (compact_record) {
			XM_Middleware_ParkRecord_Enable(true);
		}
		if(!MppMdl::Instance()->AdLoss())
			GlobalData::Instance()->ui_event_cb_(XM_UI_START_STORAGE, NULL, NULL);
		else {
			XM_MW_Media_Info media_info;
			MppMdl::Instance()->GetEncInfo(Direction_Front, 0, &media_info);
			media_info.frame_rate = kFrameRate;
			XM_Middleware_Storage_SetMediaInfo(Direction_Front, &media_info);
			XM_Middleware_Storage_Start(Direction_Front, 0);
		}
		HTTPSerMdl::Instance()->tcpRecStatus(g_engineId, 1);
		record_start_time_ = GetTickTime();
		record_timer_ = lv_timer_create(ChangeRecordTime, 100, NULL);

		//录像时关闭自动关机功能
		GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(false);
		Voice_prompts("Start recording_16k.pcm");
		GREEN_ON;
		RED_OFF;
	}
}
void PageMain::Remaining_update_time()
{
	XM_CONFIG_VALUE cfg_value;
	int rec_size=0;  //单位M
  if (g_sd_status == XM_SD_NORMAL && record_timer_ == NULL) {
		cfg_value.int_value = 0;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Resolution, cfg_value);
		if(cfg_value.int_value==k4KHeight) rec_size=6; //预估1秒视频大小为6M
		else if(cfg_value.int_value==1440) rec_size=3;
		else if(cfg_value.int_value==1080) rec_size=1;

		 if(!MppMdl::Instance()->AdLoss()) rec_size=rec_size+1;

		XM_MW_SDCard_Space sdcard_space;
        int remain_space=0;
		XM_Middleware_Storage_CheckSDSpace(XM_STORAGE_SDCard_0,&sdcard_space);
		//XMLogI("\n%lld=========%lld\n",sdcard_space.sdcard_total_space,sdcard_space.sdcard_free_space);
		if(sdcard_space.sdcard_free_space>1*1024+512+256){
		   remain_space=(sdcard_space.sdcard_free_space-1*1024-512-256)/rec_size;  //预估1秒视频大小为rec_size M
		  }else{
		   remain_space=0;
		  }
		if(remain_time_label_ == NULL){
          remain_time_label_ = lv_label_create(main_page_);
		}
        lv_label_set_text_fmt(remain_time_label_, "%02d:%02d:%02d", remain_space / 60 / 60, (remain_space / 60) % 60, remain_space % 60);
		lv_obj_set_style_text_font(remain_time_label_, &lv_font_montserrat_16, 0);
		lv_obj_align(remain_time_label_, LV_ALIGN_TOP_LEFT, size_w(20), size_h(7));
    }
}
void PageMain::CloseRecord(bool compact_record)
{
	if (record_time_label_ != NULL) {
		GlobalData::Instance()->ui_event_cb_(XM_UI_STOP_STORAGE, NULL, NULL);
		HTTPSerMdl::Instance()->tcpRecStatus(g_engineId, 0);
		if (compact_record) {
			XM_Middleware_ParkRecord_Enable(false);
		}
		
		lv_obj_del(record_time_label_);
		record_time_label_ = NULL;
		lv_obj_del(record_label_);
		record_label_ = NULL;

		lv_timer_del(record_timer_);
		record_timer_ = NULL;

		lv_img_set_src(record_img_, image_path"rec_off.png");

		record_start_time_ = 0;
		pre_record_time_ = 0;

		if (strcmp((char*)lv_img_get_src(lock_img_), image_path"lock_on.png") == 0) {
			lv_img_set_src(lock_img_, image_path"lock_off.png");
			//检测是否是碰撞开机录影
			if (collision_startup_) {
				CollisionStartupWork(CollisionStartup_StopRecord);
			}
			lock_current_recording_file_ = false;
		}
		lv_obj_add_flag(lock_png_, LV_OBJ_FLAG_HIDDEN);
		//控制蓝色灯常亮
		GlobalPage::Instance()->page_main()->led_on_ = true;
		PeripheryManager::Instance()->CameraLedControl(GlobalPage::Instance()->page_main()->led_on_);
		//结束录像后开启自动关机功能
		if(!g_app_connect)
			GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
	}
    GREEN_OFF;
    RED_ON;
	Voice_prompts("Stop recording_16k.pcm");
	Remaining_update_time();
}

void PageMain::TakePic()
{
	if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
	std::string sound_file = kAudioPath;
	sound_file += "picture_ok_16k.pcm";
	MppMdl::Instance()->PlaySound(sound_file.c_str());
	MppMdl::Instance()->GetPicJPEG(-1);
	}
}

void PageMain::ClearMainPageHidden(lv_timer_t* timer)
{
	lv_obj_clear_flag(GlobalPage::Instance()->page_main()->main_page_, LV_OBJ_FLAG_HIDDEN);
	lv_timer_del(timer);
}

void PageMain::ChangeRecordTime(lv_timer_t* timer)
{
	if (GlobalData::Instance()->UI_mode_ == UIMode_CompactRecord) {
		int64_t start_time = GlobalPage::Instance()->page_main()->record_start_time_;
		int64_t now = GetTickTime();
		int record_time = (now - start_time) / 1000; //从毫秒切换到秒

		XM_CONFIG_VALUE cfg_value;
		cfg_value.int_value = 8;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Duration, cfg_value);
		if (record_time >= cfg_value.int_value * 3600) {
			XMLogW("Compact recording completed!");
			GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_Acc, true, true);
			lv_timer_del(timer);
			GlobalPage::Instance()->page_main()->record_timer_ = NULL;
			return;
		}
	}
	/*if (record_time > 24 * 3600) {
		record_time = 0;
		GlobalPage::Instance()->page_main()->record_start_time_ = now;
	}*/

	int time = XM_Middleware_Storage_GetRecordedTime(XM_STORAGE_SDCard_0, Direction_Front) / 1000;
	if (GlobalPage::Instance()->page_main()->pre_record_time_ != time) {
		GlobalPage::Instance()->page_main()->pre_record_time_ = time;
		char buf[32] = { 0 };
		sprintf(buf, "%02d:%02d", time / 60, time % 60);
		lv_label_set_text(GlobalPage::Instance()->page_main()->record_time_label_, buf);
		//控制蓝色灯闪烁
		GlobalPage::Instance()->page_main()->led_on_ = !GlobalPage::Instance()->page_main()->led_on_;
		PeripheryManager::Instance()->CameraLedControl(GlobalPage::Instance()->page_main()->led_on_);
		//录像时红点闪烁
		if (GlobalPage::Instance()->page_main()->reverse_line_img_ == NULL) {
			if (lv_obj_has_flag(GlobalPage::Instance()->page_main()->record_label_, LV_OBJ_FLAG_HIDDEN))
				lv_obj_clear_flag(GlobalPage::Instance()->page_main()->record_label_, LV_OBJ_FLAG_HIDDEN);
			else
				lv_obj_add_flag(GlobalPage::Instance()->page_main()->record_label_, LV_OBJ_FLAG_HIDDEN);
		}
	}
}

void PageMain::CfgInit()
{
	XM_CONFIG_VALUE cfg_value;
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;

	//设置视频播放窗口大小
	GlobalPage::Instance()->page_playback()->SetPlaybackWinSize(0, 0, screen_width, 480);

	//读取车充配置
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Car_Charger, cfg_value);
	GlobalPage::Instance()->page_main()->car_charger_ = cfg_value.bool_value;

	//语言
	cfg_value.int_value = SimpChinese;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Language, cfg_value);
	set_language(cfg_value.int_value);

	//按键音
	cfg_value.bool_value = false;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Key_Voice, cfg_value);
	GlobalData::Instance()->key_tone_ = cfg_value.bool_value;

	//息屏时间
	MY_EVENT_SCREEN_CLICKED = lv_event_register_id();
	lv_obj_add_event_cb(lv_scr_act(), UpdateRestScreenTime, (lv_event_code_t)MY_EVENT_SCREEN_CLICKED, NULL);
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Lcd_OffTime, cfg_value);
	rest_screen_timer_ = lv_timer_create(RestScreen, cfg_value.int_value * 1000, NULL);
	if (cfg_value.int_value == 0) {
		lv_timer_pause(rest_screen_timer_);
	}

	//自动关机时间
	lv_obj_add_event_cb(lv_scr_act(), UpdateShutdownTime, (lv_event_code_t)MY_EVENT_SCREEN_CLICKED, NULL);
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_AutoShutdown_Time, cfg_value);
	shutdown_timer_ = lv_timer_create(AutoShutdownTimer, cfg_value.int_value * 1000, NULL);
	if (cfg_value.int_value == 0) {
		lv_timer_pause(shutdown_timer_);
	}

	//后拉镜像
	// cfg_value.bool_value = false;
	// GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Behind_Mirror, cfg_value);
	// event_inparam.set_mirror.chn = Direction_Behind;
	// event_inparam.set_mirror.open_mirror = cfg_value.bool_value;
	// GlobalData::Instance()->ui_event_cb_(XM_UI_MIRROR_SET_MIRROR, &event_inparam, &event_outparam);


	//声控
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
	event_inparam.smart_enable_keyword.enable = cfg_value.bool_value;
	GlobalData::Instance()->ui_event_cb_(XM_UI_SMART_ENABLE_KEYWORD, &event_inparam, &event_outparam);

	//录音开关
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Record_Voice, cfg_value);
	XM_Middleware_Storage_EnableRecordAudio(cfg_value.bool_value);

	//时间水印
	//cfg_value.bool_value = true;
	//GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Date_Watermark, cfg_value);
	//XM_Middleware_Encode_EnableOsdTime(cfg_value.bool_value, osd_x, osd_y);
	//osd_data_init();

	//曝光补偿
	cfg_value.int_value = 50;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Exposure_Compensation, cfg_value);
	MppMdl::Instance()->CameraSetRefrenceLevel(cfg_value.int_value);

	//白平衡
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_White_Balance, cfg_value);
	if (cfg_value.int_value == 0) {
		MppMdl::Instance()->CameraSetAwb(0, false, 6500);
	}
	else {
		MppMdl::Instance()->CameraSetAwb(0, true, cfg_value.int_value);
	}

	//光源频率
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Reject_Flicker, cfg_value);
	MppMdl::Instance()->CameraSetRejectFlicker(cfg_value.int_value);
	
	//缩时录影fps
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Fps, cfg_value);
	XMTimeLapse time_lapse = XM_TIME_LAPSE_Closed;
	if (cfg_value.int_value == 0) {
		time_lapse = XM_TIME_LAPSE_Closed;
	}
	else if (cfg_value.int_value == 1) {
		time_lapse = XM_TIME_LAPSE_Frame_1;
	}
	else if (cfg_value.int_value == 2) {
		time_lapse = XM_TIME_LAPSE_Frame_2;
	}
	else if (cfg_value.int_value == 5) {
		time_lapse = XM_TIME_LAPSE_Frame_5;
	}
	XM_Middleware_ParkRecord_SetTimeLapse(time_lapse);

	//缩时录影时长
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Duration, cfg_value);
	XMTimeLapseDuration dur = XM_TIME_LAPSE_Duration_Closed;
	if (cfg_value.int_value == 0) {
		dur = XM_TIME_LAPSE_Duration_Closed;
	}
	else if (cfg_value.int_value == 8) {
		dur = XM_TIME_LAPSE_Duration_8;
	}
	else if (cfg_value.int_value == 16) {
		dur = XM_TIME_LAPSE_Duration_16;
	}
	else if (cfg_value.int_value == 24) {
		dur = XM_TIME_LAPSE_Duration_24;
	}
	XM_Middleware_ParkRecord_SetTimeLapseDuration(dur);

	//碰撞灵敏度
	cfg_value.int_value = Sensitivity_Mid;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Sensitivity, cfg_value);
	PeripheryManager::Instance()->SensitivitySet(cfg_value.int_value);
	if (cfg_value.int_value == Sensitivity_Close) {
		PeripheryManager::Instance()->GsensorModeSelect(2);
	}
	else {
		PeripheryManager::Instance()->GsensorModeSelect(0);
	}

	//循环录影时长
	cfg_value.int_value = 1;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Circular_Record_Time, cfg_value);
	XMStorageRecordDuration record_dur = XM_STORAGE_Record_Duration_60;
	if (cfg_value.int_value == 1) {
		record_dur = XM_STORAGE_Record_Duration_60;
	}
	else if (cfg_value.int_value == 2) {
		record_dur = XM_STORAGE_Record_Duration_120;
	}
	else if (cfg_value.int_value == 3) {
		record_dur = XM_STORAGE_Record_Duration_180;
	}
	XM_Middleware_Storage_SetRecordDuration(record_dur);
}

void PageMain::UpdateShutdownTime(lv_event_t* e)
{
	if (GlobalPage::Instance()->page_main()->shutdown_timer_) {
		lv_timer_reset(GlobalPage::Instance()->page_main()->shutdown_timer_);
	}
}

void PageMain::AutoShutdownTimer(lv_timer_t* timer)
{
	XMLogI("----------------Auto Shutdown-------------");
	GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_Key, true, true);
}

void PageMain::UpdateRestScreenTime(lv_event_t* e)
{
	GlobalPage::Instance()->page_main()->OpenScreen();
}

void PageMain::RestScreen(lv_timer_t* timer)
{
	GlobalPage::Instance()->page_main()->CloseScreen();
}

void PageMain::CloseScreen()
{
	PeripheryManager::Instance()->SetLight(0);
	lv_timer_pause(rest_screen_timer_);
	rest_screen_ = true;
}

void PageMain::OpenScreen()
{
	if (rest_screen_) {
		if (rest_screen_timer_) {
			if (GlobalData::Instance()->UI_mode_ == UIMode_CompactRecord) {
				lv_timer_resume(rest_screen_timer_);
			}
			else {
				if (!close_rest_screen_fun_) {
					XM_CONFIG_VALUE cfg_value;
					cfg_value.int_value = 0;
					GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Lcd_OffTime, cfg_value);
					if (cfg_value.int_value) {
						lv_timer_resume(rest_screen_timer_);
					}
				}
			}
		}
		PeripheryManager::Instance()->SetLight(1);
		rest_screen_ = false;
	}

	if (rest_screen_timer_) {
		lv_timer_reset(rest_screen_timer_);
	}
}

int PageMain::ChangeMode(UI_MODE_E mode, bool return_selectfile_page)
{
	if (mode == UIMode_Photograph) {
		if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape && !lv_obj_has_flag(main_page_, LV_OBJ_FLAG_HIDDEN)) {
			CloseRecord();
			GlobalData::Instance()->UI_mode_ = UIMode_Photograph;
			lv_label_set_text(mode_label_, "\uF030");
			lv_obj_clear_flag(mode_label_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(record_resolution_label_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(silent_record_img_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(mic_record_img_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(lock_img_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_add_flag(lock_png_, LV_OBJ_FLAG_HIDDEN);
			if (wifi_prepared_) {
				lv_obj_add_flag(wifi_img_, LV_OBJ_FLAG_HIDDEN);
			}
			if (remain_time_label_ != NULL) {
            lv_obj_del(remain_time_label_);
            remain_time_label_ = NULL;
           }
		}
	}
	else if (mode == UIMode_Videotape) {
		if (GlobalData::Instance()->UI_mode_ == UIMode_Photograph && !lv_obj_has_flag(main_page_, LV_OBJ_FLAG_HIDDEN)){
			GlobalData::Instance()->UI_mode_ = UIMode_Videotape;
			lv_label_set_text(mode_label_, "\uF03D");
			lv_obj_add_flag(mode_label_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(record_resolution_label_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(mic_record_img_, LV_OBJ_FLAG_HIDDEN);
		if(touch_plan){
			lv_obj_clear_flag(silent_record_img_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(lock_img_, LV_OBJ_FLAG_HIDDEN);
		 }
			if (wifi_prepared_) {
				lv_obj_clear_flag(wifi_img_, LV_OBJ_FLAG_HIDDEN);
			}
			LeaveMainPageWork(false);
		}
		else if (GlobalData::Instance()->UI_mode_ == UIMode_Playback) {
			GlobalPage::Instance()->page_playback()->PlaybackPrepareWork(false);
			GlobalData::Instance()->UI_mode_ = UIMode_Videotape;
			lv_label_set_text(mode_label_, "\uF03D");
			lv_obj_add_flag(mode_label_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(record_resolution_label_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(mic_record_img_, LV_OBJ_FLAG_HIDDEN);
			if(touch_plan){
			lv_obj_clear_flag(silent_record_img_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(lock_img_, LV_OBJ_FLAG_HIDDEN);
			}
			if (wifi_prepared_) {
				lv_obj_clear_flag(wifi_img_, LV_OBJ_FLAG_HIDDEN);
			}
			GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
			GlobalPage::Instance()->page_sys_set()->OpenRestScreenFun(true);
			
			if (return_selectfile_page) {
				lv_obj_clear_flag(GlobalPage::Instance()->page_playback()->select_filetype_page_, LV_OBJ_FLAG_HIDDEN);
				lv_group_focus_obj(GlobalPage::Instance()->page_playback()->filetype_img_[GlobalPage::Instance()->page_playback()->file_type_]);
			}
			else {
				if (GlobalPage::Instance()->page_playback()->select_filetype_page_) {
					lv_obj_del(GlobalPage::Instance()->page_playback()->select_filetype_page_);
					GlobalPage::Instance()->page_playback()->select_filetype_page_ = NULL;
				}
			}

			lv_refr_now(NULL);
			if (GlobalPage::Instance()->page_playback()->close_preview_) {
				MppMdl::Instance()->StartPreview();
				GlobalPage::Instance()->page_playback()->close_preview_ = false;
			}
		}
		else if (GlobalData::Instance()->UI_mode_ == UIMode_Picture) {
			GlobalPage::Instance()->page_play_pic()->PlayPicPrepareWork(false);
			GlobalData::Instance()->UI_mode_ = UIMode_Videotape;
			lv_label_set_text(mode_label_, "\uF03D");
			lv_obj_add_flag(mode_label_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(record_resolution_label_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(mic_record_img_, LV_OBJ_FLAG_HIDDEN);
			if(touch_plan){
			lv_obj_clear_flag(silent_record_img_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(lock_img_, LV_OBJ_FLAG_HIDDEN);
			}
			if (wifi_prepared_) {
				lv_obj_clear_flag(wifi_img_, LV_OBJ_FLAG_HIDDEN);
			}
			GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
			GlobalPage::Instance()->page_sys_set()->OpenRestScreenFun(true);
			
			if (return_selectfile_page) {
				lv_obj_clear_flag(GlobalPage::Instance()->page_playback()->select_filetype_page_, LV_OBJ_FLAG_HIDDEN);
				lv_group_focus_obj(GlobalPage::Instance()->page_playback()->filetype_img_[GlobalPage::Instance()->page_play_pic()->file_type_]);
			}
			else {
				if (GlobalPage::Instance()->page_playback()->select_filetype_page_) {
					lv_obj_del(GlobalPage::Instance()->page_playback()->select_filetype_page_);
					GlobalPage::Instance()->page_playback()->select_filetype_page_ = NULL;
				}
			}

			lv_refr_now(NULL);
			if (GlobalPage::Instance()->page_playback()->close_preview_) {
				MppMdl::Instance()->StartPreview();
				GlobalPage::Instance()->page_playback()->close_preview_ = false;
			}
		}
	}

	return 0;
}

void PageMain::PlayBackModeReturnRecordWork()
{
	return;
}

void PageMain::LockCurrentFile()
{
	if (strcmp((char*)lv_img_get_src(lock_img_), image_path"lock_off.png") == 0) {
		if (record_time_label_) {
			lock_current_recording_file_ = true;
			lv_img_set_src(lock_img_, image_path"lock_on.png");
			lv_obj_clear_flag(lock_png_, LV_OBJ_FLAG_HIDDEN);
			if (record_label_) {
				XMLogI("record_label, yellow");
				lv_img_set_src(record_label_, image_path"yellow_circle.png");
			}
			
			XM_Middleware_Storage_LockCurrentFile(XM_STORAGE_SDCard_0, Direction_Front, true);
			XM_Middleware_Storage_LockCurrentFile(XM_STORAGE_SDCard_0, Direction_Behind, true);
			//OpenTipBox("The current file is locked");
			RED_ON;
			Voice_prompts("Video locked_16k.pcm");
		}
	}
	else {
		lv_img_set_src(lock_img_, image_path"lock_off.png");
		lv_obj_add_flag(lock_png_, LV_OBJ_FLAG_HIDDEN);
		if (record_label_) {
			XMLogI("record_label, red");
			lv_img_set_src(record_label_, image_path"red_circle.png");
		}
		
		if (lock_current_recording_file_) {
			lock_current_recording_file_ = false;
			XM_Middleware_Storage_LockCurrentFile(XM_STORAGE_SDCard_0, Direction_Front, false);
			XM_Middleware_Storage_LockCurrentFile(XM_STORAGE_SDCard_0, Direction_Behind, false);
		}
		RED_OFF;
	}
}

void PageMain::RecordPicModeKeypad(int key)
{
	if (key == KEYMAP_OK) {
		if (!lv_obj_has_flag(main_page_, LV_OBJ_FLAG_HIDDEN)) {
			CheckSDStatus();
			if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024) {
				if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape) {
					if (record_timer_ == NULL) {
						StartRecord();
					}
					else {
						CloseRecord();
					}
				}
				else if (GlobalData::Instance()->UI_mode_ == UIMode_Photograph) {
					TakePic();
				}
			}
		}
	}
	else if (key == KEYMAP_MENU || key == KEYMAP_MODE) {
		if (key == KEYMAP_MODE && !lv_obj_has_flag(main_page_, LV_OBJ_FLAG_HIDDEN)) {
			// if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape) {
			// 	ChangeMode(UIMode_Photograph);
			// }
			// else 
			if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape) {
				if (g_sd_status == XM_SD_NORMAL || g_sd_status == XM_SD_NEEDCHECKSPEED || g_sd_status == XM_SD_NOSPACE) {
					LeaveMainPageWork(true);
					GlobalPage::Instance()->page_playback()->OpenSelectFilePage();
					CloseRecord();
				}
				else {
					ChangeMode(UIMode_Videotape);
				}
			}
			return;
		}
		
		if (GlobalData::Instance()->opened_subpage_[1]) {
			lv_obj_del(GlobalData::Instance()->opened_subpage_[1]);
			GlobalData::Instance()->opened_subpage_[1] = NULL;
		}
		else if (GlobalData::Instance()->opened_subpage_[0]) {
			lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
			GlobalData::Instance()->opened_subpage_[0] = NULL;
		}
		else if (!lv_obj_has_flag(main_page_, LV_OBJ_FLAG_HIDDEN)) {
			if (key == KEYMAP_MENU) {
				LeaveMainPageWork(true);
				GlobalPage::Instance()->page_set()->OpenPage();
				CloseRecord();
			}
		}
		else {
			lv_obj_del(GlobalPage::Instance()->page_set()->set_page_);
			GlobalPage::Instance()->page_set()->set_page_ = NULL;
			LeaveMainPageWork(false);
		}
	}
}

void PageMain::ClosePreview(bool delay)
{
	if (delay) {
		lv_timer_create(ClosePreviewTimer, 50, NULL);
	}
	else {
		MppMdl::Instance()->SetPlayMode(XM_PLAY_NONE);
	}
}

void PageMain::ClosePreviewTimer(lv_timer_t* timer)
{
	MppMdl::Instance()->SetPlayMode(XM_PLAY_NONE);
	lv_timer_del(timer);
}

int PageMain::ChangeSplit(int camera_dir, bool show_tip)
{
	int ret = MppMdl::Instance()->SetPlayMode((XMRealPlayMode)camera_dir);
	if (ret == -1) {
		if(show_tip)
			GlobalPage::Instance()->page_main()->OpenTipBox("Please insert the rear camera");
		return -1;
	}
	return 0;
}

void PageMain::CheckSDStatus(bool checked_speed, bool need_speed_tip, bool need_check_speed_tip, bool nospace_tip)
{
	if (GlobalPage::Instance()->page_sys_set()->format_tip_win_) {	
			return;	
		}
	if (g_sd_status == XM_SD_NORMAL) {
		if (checked_speed) {
			if (need_speed_tip && GlobalData::Instance()->SDCard_write_speed_ < 1024) {
				OpenSDSpeedTipBox(GlobalData::Instance()->SDCard_write_speed_, true);
			}
		}
		else {
			int write_speed = 0;
			int ret;
			if (GlobalData::Instance()->gdb_debug_) {
				ret = 0;
				write_speed = 2048;
			} else {
				ret = XM_Middleware_Storage_GetSpeedState(XM_STORAGE_SDCard_0, &write_speed);
			}
			XMLogI("write_speed = %d", write_speed);
			GlobalData::Instance()->SDCard_write_speed_ = write_speed;
			if (ret < 0) {
				g_sd_status = XM_SD_NEEDCHECKSPEED;
				OpenTipBox("Please format the memory card");
			}
			else {
				if (write_speed < 1024) {
					OpenSDSpeedTipBox(write_speed, true);
				}
				else {
					OpenTipBox("SD card inserted");
				}
			}
		}
	}
	else if (g_sd_status == XM_SD_NEEDFORMAT) {
		OpenTipBox("Please format the memory card");
	}
	else if (g_sd_status == XM_SD_NEEDCHECKSPEED) {
		if(need_check_speed_tip)
			OpenTipBox("Please format the memory card");
	}
	else if (g_sd_status == XM_SD_ABNORMAL) {
		OpenTipBox("The memory card is damaged, please replace it");
	}
	else if (g_sd_status == XM_SD_NOTRW) {
		OpenTipBox("Memory card is not read-write, please format the memory card");
	}
	else if (g_sd_status == XM_SD_NOSPACE) {
		if(nospace_tip)
			OpenTipBox("SD card space is insufficient");
	}
	else {
		OpenTipBox("Please insert SD card");
		if (g_sd_status < 0) {
			XMLogI("SD card init error! g_sd_status = %d", g_sd_status);
		}
		else {
			XMLogI("g_sd_status = %d", g_sd_status);
		}
	}
}

void PageMain::OpenSDSpeedTipBox(int write_speed, bool low_speed, int delay_time)
{
	int speed = write_speed;
	char string[128];
	XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value = SimpChinese;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Language, cfg_value);
	if (cfg_value.int_value == English) {
		if (low_speed)
			sprintf(string, "The write speed of the memory card is only %dKB/s, please replace the memory card", speed);
		else
			sprintf(string, "The write speed of the memory card is %dKB/s", speed);
	}
	else {
		if (low_speed)
			sprintf(string, "内存卡写入速度仅%dKB/s,请更换内存卡", speed);
		else
			sprintf(string, "内存卡写入速度为%dKB/s", speed);
	}

	if (delay_time > 0) {
		OpenTipBox(string, 3000, false, delay_time);
	}
	else {
		OpenTipBox(string, 3000, false);
	}
}
void PageMain::ShutDownTimer_PullPowerOn(lv_timer_t* timer)
{
	static int cnt=0;
	cnt++;
	XMLogI("ShutDownTimer_PullPowerOn");
	if(cnt==1){
     POWER_OFF;
	}else if(cnt>2){
    SPEAK_OFF;
	}
	//lv_timer_del(timer);
}
void PageMain::ShutDown(ShutDownMode shutdownmode, bool play_pic, bool close_preview)
{
	if (play_pic) {
        //关闭录像
		GlobalData::Instance()->ui_event_cb_(XM_UI_STOP_STORAGE, NULL, NULL);
		//关闭预览
		lv_timer_create(ShutDownTimer_PullPowerOn, 350, (void*)shutdownmode);
		if (close_preview)
			GlobalPage::Instance()->page_main()->ClosePreview(false);
		//停止lvgl画面刷新
		XMLogI("before stop refresh screen!");
		GlobalData::Instance()->stop_screen_refresh_ = true;
		//显示关机图片
		XMLogI("before XM_Middleware_ShutDown_ShowShutDownLogo");
		MppMdl::Instance()->ShowLogo(PlayLogo_Shutdown);
		OpenScreen();
		//播放关机音乐,如果是电池供电不放音乐
		if (!battery_powered_) {
			XMLogI("before XM_Middleware_Sound_Play");
			std::string sound_file = kAudioPath;
			sound_file += "shutdown_16k.pcm";
			MppMdl::Instance()->PlaySound(sound_file.c_str());
		}
		LogFileManager::Instance()->write_log_sd_set(0,2);
		LogFileManager::Instance()->Write_sd();
		//释放资源
		XMLogI("before XM_Middleware_Exit");
		MppMdl::Instance()->StopADProc();
		MppMdl::Instance()->StopEncProc();
		XM_Middleware_Exit();
		//关闭热点
		XM_Middleware_WIFI_WifiEnable(false);
		//关机前将碰撞灵敏度设为高，若碰撞开机为关的话则关闭
		XMLogI("before SensitivitySet");
		XM_CONFIG_VALUE cfg_value;
		cfg_value.bool_value = true;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Startup, cfg_value);
		if (cfg_value.bool_value) {
			PeripheryManager::Instance()->SensitivitySet(Sensitivity_StartUp);
			PeripheryManager::Instance()->GsensorModeSelect(1);
		}
		else {
			PeripheryManager::Instance()->SensitivitySet(Sensitivity_Close);
			PeripheryManager::Instance()->GsensorModeSelect(2);
		}
		
		XMLogI("before Create ShutDownTimer");
		lv_timer_create(ShutDownTimer, 1500, (void*)shutdownmode);
		XMLogI("after Create ShutDownTimer");
	}
	else {
		XMLogI("before XM_Middleware_Exit");
		MppMdl::Instance()->StopADProc();
		MppMdl::Instance()->StopEncProc();
		XM_Middleware_Exit();
		XMLogI("Shutdown");
		PeripheryManager::Instance()->Shutdown(shutdownmode);
	}
}

void PageMain::ShutDownTimer(lv_timer_t* timer)
{
	XMLogI("Enter ShutDownTimer");
	int shutdownmode = (int)timer->user_data;
	XMLogI("Shutdown");
	//XM_Middleware_SPK_SpeakerEnable(false);
	PeripheryManager::Instance()->Shutdown((ShutDownMode)shutdownmode);
	lv_timer_del(timer);
}

void PageMain::DelayShutDownTimer(lv_timer_t* timer)
{
	int user_data = (int)timer->user_data;
	if (user_data) {
		GlobalPage::Instance()->page_main()->CollisionStartupWork(CollisionStartup_SDCardError);
	}
	else {
		GlobalPage::Instance()->page_main()->ShutDown(ShutDownMode_Acc, true, true);
	}
	
	lv_timer_del(timer);
}

void PageMain::OpenPicTip(const char* pic, const char* string)
{
	lv_obj_t* tip_box = lv_create_page(lv_scr_act(), size_w(400), size_h(300),
		lv_color_make(94, 91, 92), 10, 0, lv_font_all, lv_color_white(), 0);
	lv_obj_align(tip_box, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t* pic_label = lv_create_label(tip_box, size_w(400), pic,
		LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_set_style_text_font(tip_box, &lv_font_montserrat_40, 0);
	lv_obj_set_style_text_color(tip_box, lv_palette_main(LV_PALETTE_RED), 0);
	lv_obj_align(pic_label, LV_ALIGN_CENTER, 0, -size_h(50));

	lv_obj_t* label = lv_create_label(tip_box, size_w(400), GetParsedString(string),
		LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align_to(label, pic_label, LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(70));

	lv_timer_create(CloseTipBox, 1500, (void*)tip_box);
}

void PageMain::PWREvent()
{
	if (rest_screen_) {
		//点亮屏幕
		OpenScreen();
	}
	else {
		//关闭屏幕
		CloseScreen();
	}
}

void PageMain::SetListStyle(lv_obj_t* list, lv_coord_t width, lv_coord_t height)
{
	lv_obj_set_size(list, width+30, height);
	lv_obj_set_style_border_width(list, 0, 0);
	lv_obj_set_style_bg_color(list, lv_color_make(68, 68, 68), 0);
	lv_obj_set_style_radius(list, 0, 0);

	for (int i = 0; i < lv_obj_get_child_cnt(list); i++)
	{
		lv_obj_t* item = lv_obj_get_child(list, i);
		if (lv_obj_check_type(item, &lv_list_btn_class))
		{
			lv_obj_set_style_bg_color(item, lv_color_make(68, 68, 68), 0);
			lv_obj_set_style_border_color(item, lv_color_make(66, 66, 66), 0);
			lv_obj_set_style_bg_color(item, lv_color_make(140, 140, 140), LV_STATE_FOCUSED);
			lv_obj_set_style_bg_color(item, lv_color_make(140, 140, 140), LV_STATE_FOCUS_KEY);
			lv_obj_set_style_text_color(item, lv_color_white(), 0);
		}
		else
		{
			lv_obj_set_style_text_color(item, lv_color_white(), 0);
		}
	}
}

void PageMain::LeaveMainPageWork(bool leave)
{
	XM_CONFIG_VALUE cfg_value;
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	if (leave) {
		//隐藏主页面
		lv_obj_add_flag(main_page_, LV_OBJ_FLAG_HIDDEN);
		//关闭声控
		event_inparam.smart_enable_keyword.enable = false;
		GlobalData::Instance()->ui_event_cb_(XM_UI_SMART_ENABLE_KEYWORD, &event_inparam, &event_outparam);
	}
	else {
		//显示主页面
		lv_obj_clear_flag(main_page_, LV_OBJ_FLAG_HIDDEN);
		Remaining_update_time();
		//恢复声控
		cfg_value.bool_value = true;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
		event_inparam.smart_enable_keyword.enable = cfg_value.bool_value;
		GlobalData::Instance()->ui_event_cb_(XM_UI_SMART_ENABLE_KEYWORD, &event_inparam, &event_outparam);
		//切换摄像头
		XMRealPlayMode play_mode;
		MppMdl::Instance()->GetMode(&play_mode);
		int camera_dir = GlobalPage::Instance()->page_sys_set()->camera_dir_;
		if (camera_dir != play_mode) {
			ChangeSplit(camera_dir, false);
		}
		//检测ad是否连接
		// if (!MppMdl::Instance()->AdLoss()) {
		// 	if (strcmp((char*)lv_label_get_text(record_resolution_label_), "3K") == 0 || strcmp((char*)lv_label_get_text(record_resolution_label_), "4K") == 0)
		// 		OnADInsert(false);
		// }
		// else {
		// 	if(strcmp((char*)lv_label_get_text(record_resolution_label_), "3K") != 0 || strcmp((char*)lv_label_get_text(record_resolution_label_), "4K") != 0)
		// 		OnADExit();
		// }
		//开启录像
		if(GlobalData::Instance()->UI_mode_ == UIMode_Videotape)
			StartRecord();
	}
}

void PageMain::AccDisconnectWork()
{
	//正在格式化和升级时不对ACC进行处理
	if (GlobalData::Instance()->stop_keypad_ || GlobalPage::Instance()->page_main()->reverse_line_img_
		|| GlobalData::Instance()->UI_mode_ == UIMode_CompactRecord) {
		XMLogI("Formatting or Reversing, do not process ACC!");
		GlobalPage::Instance()->page_main()->acc_disconnect_=2;
		return;
	}
   GlobalPage::Instance()->page_main()->acc_disconnect_=true;
	//碰撞开机后，录制加锁视频时，断开ACC直接关机
	int ret = CollisionStartupWork(CollisionStartup_VehicleStall);
	if (ret) return;

	//检测缩时录影FPS
	XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Fps, cfg_value);
	int compact_record_fps = cfg_value.int_value;
	//检测缩时录影时长
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Compact_Record_Duration, cfg_value);
	int compact_record_dur = cfg_value.int_value;

	//打开屏幕
	OpenScreen();
	if (g_sd_status == XM_SD_NORMAL && GlobalData::Instance()->SDCard_write_speed_ >= 1024 && compact_record_fps > 0 && compact_record_dur > 0) {
		OpenTipBox("About to enter minitype video mode");
		if (acc_disconnect_timer_) {
			lv_timer_del(acc_disconnect_timer_);
			acc_disconnect_timer_ = NULL;
		}
		acc_disconnect_timer_ = lv_timer_create(CompactRecordTimer, 2000, NULL);
	}
	else {
		ShutDown(ShutDownMode_Acc, true, true);
	}
}

void PageMain::CompactRecordTimer(lv_timer_t* timer)
{
	if (GlobalPage::Instance()->page_main()->acc_disconnect_) {
		GlobalPage::Instance()->page_main()->CompactRecord();
	}
	
	GlobalPage::Instance()->page_main()->acc_disconnect_timer_ = NULL;
	lv_timer_del(timer);
}

void PageMain::CompactRecord()
{
	//退出wifi
	if (wifi_enable_) {
		bool flag=false;
		XM_CONFIG_VALUE cfg_value;
        cfg_value.bool_value = false;
        GlobalData::Instance()->car_config()->GetValue(CFG_Operation_WiFi, cfg_value);
        flag= cfg_value.bool_value;
		g_app_connect = false;
	   lv_event_send(GlobalPage::Instance()->page_main()->wifi_img_, LV_EVENT_CLICKED, NULL);
	   cfg_value.bool_value=flag;
	   GlobalData::Instance()->car_config()->SetValue(CFG_Operation_WiFi, cfg_value);
	}
	
	//进入录像界面
	ReturnMainPage();

	//关闭录像
	CloseRecord();

	//关闭自动关机
	GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(false);

	//设置5秒息屏
	if (rest_screen_timer_) {
		lv_timer_set_period(rest_screen_timer_, 5000);
		lv_timer_resume(rest_screen_timer_);
		lv_timer_reset(rest_screen_timer_);
	}

	//关闭声控
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	event_inparam.smart_enable_keyword.enable = false;
	GlobalData::Instance()->ui_event_cb_(XM_UI_SMART_ENABLE_KEYWORD, &event_inparam, &event_outparam);

	//修改图标
	lv_label_set_text(mode_label_, "\uF1C8");

	//进行缩时录影
	GlobalData::Instance()->UI_mode_ = UIMode_CompactRecord;
	StartRecord(true);
}

void PageMain::AccConnectWork()
{
	if (GlobalData::Instance()->UI_mode_ == UIMode_CompactRecord) {
		//ACC连接后开屏
		OpenScreen();
		OpenTipBox("About to exit minitype video mode");
		if (acc_connect_timer_) {
			lv_timer_del(acc_connect_timer_);
			acc_connect_timer_ = NULL;
		}
		acc_connect_timer_ = lv_timer_create(ExitCompactRecordTimer, 2000, NULL);
	}
}

void PageMain::ExitCompactRecordTimer(lv_timer_t* timer)
{
	if (!GlobalPage::Instance()->page_main()->acc_disconnect_ && GlobalData::Instance()->UI_mode_ == UIMode_CompactRecord) {
		GlobalPage::Instance()->page_main()->ExitCompactRecord();
	}
	
	GlobalPage::Instance()->page_main()->acc_connect_timer_ = NULL;
	lv_timer_del(timer);
}

void PageMain::ExitCompactRecord()
{
   XM_CONFIG_VALUE cfg_value;
	cfg_value.bool_value = false;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_WiFi, cfg_value);
	if(cfg_value.bool_value && !wifi_enable_){
	 WifiEnable(!wifi_enable_);
	}
	//关闭缩时录影
	CloseRecord(true);
	
	//息屏恢复正常
	if (rest_screen_timer_) {
		cfg_value.int_value = 0;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Lcd_OffTime, cfg_value);
		if (cfg_value.int_value == 0) {
			lv_timer_pause(rest_screen_timer_);
		}
		else {
			lv_timer_set_period(rest_screen_timer_, cfg_value.int_value * 1000);
			lv_timer_resume(rest_screen_timer_);
			lv_timer_reset(rest_screen_timer_);
		}
	}

	//恢复声控
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
	event_inparam.smart_enable_keyword.enable = cfg_value.bool_value;
	GlobalData::Instance()->ui_event_cb_(XM_UI_SMART_ENABLE_KEYWORD, &event_inparam, &event_outparam);

	//修改图标
	lv_label_set_text(mode_label_, "\uF03D");

	//进行正常录影
	GlobalData::Instance()->UI_mode_ = UIMode_Videotape;
	StartRecord();
}

void PageMain::ReturnMainPage()
{
	if (GlobalData::Instance()->opened_subpage_[1]) {
		lv_obj_del(GlobalData::Instance()->opened_subpage_[1]);
		GlobalData::Instance()->opened_subpage_[1] = NULL;
	}

	if (GlobalData::Instance()->opened_subpage_[0]) {
		lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
		GlobalData::Instance()->opened_subpage_[0] = NULL;
	}

	if (GlobalPage::Instance()->page_sys_set()->time_page_) {
		GlobalPage::Instance()->page_sys_set()->ChangeTime(KEYMAP_MENU);
	}

	if (GlobalPage::Instance()->page_sys_set()->voice_control_page_) {
		GlobalPage::Instance()->page_sys_set()->VoiceControlKeypadEvent(KEYMAP_MENU);
	}

	if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape) {
		if (lv_obj_has_flag(main_page_, LV_OBJ_FLAG_HIDDEN)) {
			if (GlobalPage::Instance()->page_playback()->select_filetype_page_) {
				lv_obj_del(GlobalPage::Instance()->page_playback()->select_filetype_page_);
				GlobalPage::Instance()->page_playback()->select_filetype_page_ = NULL;
			}
			else {
				lv_obj_del(GlobalPage::Instance()->page_set()->set_page_);
				GlobalPage::Instance()->page_set()->set_page_ = NULL;
				LeaveMainPageWork(false);
			}
		}
	}
	else if (GlobalData::Instance()->UI_mode_ == UIMode_Photograph) {
		if (lv_obj_has_flag(main_page_, LV_OBJ_FLAG_HIDDEN)) {
			lv_obj_del(GlobalPage::Instance()->page_set()->set_page_);
			GlobalPage::Instance()->page_set()->set_page_ = NULL;
			LeaveMainPageWork(false);
		}

		ChangeMode(UIMode_Videotape);
	}
	else if (GlobalData::Instance()->UI_mode_ == UIMode_Playback) {
		ChangeMode(UIMode_Videotape);
	}
	else if (GlobalData::Instance()->UI_mode_ == UIMode_Picture) {
		ChangeMode(UIMode_Videotape);
	}
}

void PageMain::ReverseEvent(bool start)
{
	if (start) {
		//返回主页面
		ReturnMainPage();
		//打开屏幕
		OpenScreen();
		//记录倒车之前的分屏
		pre_camera_status_ = GlobalPage::Instance()->page_sys_set()->camera_dir_;
		//后拉全屏
		int ret = ChangeSplit(XM_PLAY_AD);
		if (!ret) {
			GlobalPage::Instance()->page_sys_set()->camera_dir_ = XM_PLAY_AD;
			XM_CONFIG_VALUE cfg_value;
			cfg_value.int_value = XM_PLAY_AD;
			GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Camera, cfg_value);
			MppMdl::Instance()->SetUserMode((XMRealPlayMode)cfg_value.int_value);
			if (reverse_line_img_ == NULL) {
				if (GlobalPage::Instance()->page_sys_set()->format_tip_win_) {
					lv_obj_del(GlobalPage::Instance()->page_sys_set()->format_tip_win_);
					GlobalPage::Instance()->page_sys_set()->format_tip_win_ = NULL;
				}

				//停止自动关机和息屏功能
				GlobalPage::Instance()->page_sys_set()->OpenRestScreenFun(false);
				GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(false);

				//显示倒车图片
				reverse_line_img_ = lv_img_create(lv_scr_act());
				lv_img_set_src(reverse_line_img_, image_path"return_640_209_e.png");
				lv_obj_align(reverse_line_img_, LV_ALIGN_BOTTOM_MID, 0, -start_y);

				//隐藏图标
				lv_obj_add_flag(main_page_, LV_OBJ_FLAG_HIDDEN);

				//开启录像
				if (!wifi_enable_)
					StartRecord();
			}
		}
	}
	else {
		//打开屏幕
		OpenScreen();
		//切换回倒车之前的分屏
		if (pre_camera_status_ != GlobalPage::Instance()->page_sys_set()->camera_dir_) {
			int ret = ChangeSplit(pre_camera_status_);
			if (!ret) {
				GlobalPage::Instance()->page_sys_set()->camera_dir_ = pre_camera_status_;
				XM_CONFIG_VALUE cfg_value;
				cfg_value.int_value = pre_camera_status_;
				GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Camera, cfg_value);
				MppMdl::Instance()->SetUserMode((XMRealPlayMode)cfg_value.int_value);
			}
		}

		if (reverse_line_img_) {
			//显示图标
			lv_obj_clear_flag(main_page_, LV_OBJ_FLAG_HIDDEN);

			//删除倒车图片
			lv_obj_del(reverse_line_img_);
			reverse_line_img_ = NULL;

			//恢复自动关机和息屏功能
			GlobalPage::Instance()->page_sys_set()->OpenRestScreenFun(true);
			
			if (GlobalData::Instance()->UI_mode_ != UIMode_CompactRecord) {
				if (!g_app_connect)
					GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
			}
			else {
				lv_timer_resume(GlobalPage::Instance()->page_main()->rest_screen_timer_);
				lv_timer_reset(GlobalPage::Instance()->page_main()->rest_screen_timer_);
			}
		}
	}
}

void PageMain::OpenShutDownTip(const char* string)
{
	lv_obj_t* tip_box = lv_create_page(lv_scr_act(), screen_width, screen_height,
		lv_color_make(38, 38, 38), 0, 0, lv_font_all, lv_color_white(), 0);
	lv_obj_align(tip_box, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t* label = lv_create_label(tip_box, size_w(screen_width), GetParsedString(string), LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
}

bool PageMain::CollisionStartWithBattery()
{
	bool shutdown = false;
	if (battery_powered_) {
		shutdown = true;
	}
	else if (car_charger_) {
		shutdown = false;
	}
	else if (acc_disconnect_) {
		shutdown = true;
	}
	return shutdown;
}

int PageMain::CollisionStartupWork(CollisionStartup flag)
{
	if (collision_startup_) {
		XMLogI("acc_disconnect_ = %d, car_charger_ = %d, battery_powered_ = %d", acc_disconnect_, car_charger_, battery_powered_);
		bool shutdown = CollisionStartWithBattery();
		if (shutdown) {
			std::string str;
			if (flag == CollisionStartup_StopRecord)
				str = "The emergency video recording has ended. The vehicle is detected to be stalled and will be shut down soon";
			else if (flag == CollisionStartup_VehicleStall)
				str = "The vehicle is detected to be stalled and will be shut down soon";
			else if (flag == CollisionStartup_SDCardError) {
				if (g_sd_status == XM_SD_NOEXIST)
					str = "The SD card is not inserted, and the emergency video cannot be recorded. The vehicle is detected to be stalled and will be shut down soon";
				else
					str = "This SD card is unable to perform emergency video recording. The vehicle is detected to be stalled and will be shut down soon";
			}
			
			OpenShutDownTip(str.c_str());
			CloseRecord();
			lv_timer_create(DelayShutDownTimer, 1500, NULL);
			return -1;
		}
		else {
			collision_startup_ = false;
			if (record_time_label_ != NULL) {
				// if (!MppMdl::Instance()->AdLoss())
				// 	GlobalData::Instance()->ui_event_cb_(XM_UI_START_STORAGE, NULL, NULL);
				// else 
				{
					XM_MW_Media_Info media_info;
					MppMdl::Instance()->GetEncInfo(Direction_Front, 0, &media_info);
					media_info.frame_rate = kFrameRate;
					XM_Middleware_Storage_SetMediaInfo(Direction_Front, &media_info);
					XM_Middleware_Storage_Start(Direction_Front, 0);
				}
			}
		}
	}

	return 0;
}

void PageMain::ChangeCarChargerCfg(bool car_charger)
{
	if (car_charger == GlobalPage::Instance()->page_main()->car_charger_)
		return;

	GlobalPage::Instance()->page_main()->car_charger_ = car_charger;
	XM_CONFIG_VALUE cfg_value;
	cfg_value.bool_value = car_charger;
	int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Car_Charger, cfg_value);
	if (ret < 0) {
		XMLogE("set config error, opr=CFG_Operation_Car_Charger");
	}
}

void PageMain::OnADInsert(bool show_tip)
{
	lv_event_send(lv_scr_act(), (lv_event_code_t)GlobalPage::Instance()->page_main()->MY_EVENT_SCREEN_CLICKED, NULL);
	Remaining_update_time();
	if (show_tip)
		OpenTipBox("Pullback inserted");
	// XM_CONFIG_VALUE cfg_value;
	// int ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Resolution, cfg_value);
	// if (ret >= 0) {
	// 	//插上后拉时，把前录3K强制降为2K
	// 	if (cfg_value.int_value == k3KHeight || cfg_value.int_value == k4KHeight) {
	// 		XMLogI("switch to 1440P");
	// 		XMUIEventInParam event_inparam;
	// 		XMUIEventOutParam event_outparam;
	// 		event_inparam.set_enc.video_codec_type = PT_H265;
	// 		event_inparam.set_enc.width = 2560;
	// 		event_inparam.set_enc.height = 1440;
	// 		event_inparam.set_enc.bit_rate = 20480;
	// 		event_inparam.set_enc.channel = Direction_Front;
	// 		GlobalData::Instance()->ui_event_cb_(XM_UI_SET_ENCODE_PARAM, &event_inparam, &event_outparam);	
	// 		lv_label_set_text(record_resolution_label_, "2K");
	// 	}
	// }

	//插入后拉时，判断是否要切换双录
	XMRealPlayMode play_mode;
	MppMdl::Instance()->GetMode(&play_mode);
	int camera_dir = GlobalPage::Instance()->page_sys_set()->camera_dir_;
	if ((camera_dir == XM_PLAY_SENSOR && play_mode != XM_PLAY_SENSOR)
		|| (camera_dir == XM_PLAY_AD && play_mode != XM_PLAY_AD)
		|| (camera_dir == XM_PLAY_BOTH && play_mode != XM_PLAY_BOTH)) {
		ChangeSplit(camera_dir, false);
	}

	if (record_time_label_ != NULL) {
		XM_Middleware_Storage_Stop(-1);
		XM_MW_Media_Info media_info;
		MppMdl::Instance()->GetEncInfo(0, 0, &media_info);
		media_info.frame_rate = kFrameRate;
		XM_Middleware_Storage_SetMediaInfo(0, &media_info);
		MppMdl::Instance()->GetEncInfo(1, 0, &media_info);
		XM_Middleware_Storage_SetMediaInfo(1, &media_info);
		XM_Middleware_Storage_Start(-1, 0);
	}
}

void PageMain::OnADExit()
{
	lv_event_send(lv_scr_act(), (lv_event_code_t)GlobalPage::Instance()->page_main()->MY_EVENT_SCREEN_CLICKED, NULL);
	Remaining_update_time();
	// XM_CONFIG_VALUE cfg_value;
	// int ret = GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Resolution, cfg_value);
	// if (ret >= 0) {
	// 	//拔出后拉时，前录设为3K
	// 	if (cfg_value.int_value == k4KHeight) {
	// 		XMLogI("switch to 4K");
	// 		XMUIEventInParam event_inparam;
	// 		XMUIEventOutParam event_outparam;
	// 		event_inparam.set_enc.video_codec_type = PT_H265;
	// 		event_inparam.set_enc.width = 3840;
	// 		event_inparam.set_enc.height = k4KHeight;
	// 		event_inparam.set_enc.bit_rate = 20480;
	// 		event_inparam.set_enc.channel = Direction_Front;
	// 		GlobalData::Instance()->ui_event_cb_(XM_UI_SET_ENCODE_PARAM, &event_inparam, &event_outparam);
	// 		lv_label_set_text(record_resolution_label_, "4K");
	// 	}
	// 	else if (cfg_value.int_value == k3KHeight) {
	// 		XMLogI("switch to 3K");
	// 		XMUIEventInParam event_inparam;
	// 		XMUIEventOutParam event_outparam;
	// 		event_inparam.set_enc.video_codec_type = PT_H265;
	// 		event_inparam.set_enc.width = 3008;
	// 		event_inparam.set_enc.height = k3KHeight;
	// 		event_inparam.set_enc.bit_rate = 20480;
	// 		event_inparam.set_enc.channel = Direction_Front;
	// 		GlobalData::Instance()->ui_event_cb_(XM_UI_SET_ENCODE_PARAM, &event_inparam, &event_outparam);
	// 		lv_label_set_text(record_resolution_label_, "3K");
	// 	}
	// }

	if (record_time_label_ != NULL) {
		XM_Middleware_Storage_Stop(-1);
		XM_MW_Media_Info media_info;
		MppMdl::Instance()->GetEncInfo(Direction_Front, 0, &media_info);
		media_info.frame_rate = kFrameRate;
		XM_Middleware_Storage_SetMediaInfo(Direction_Front, &media_info);
		XM_Middleware_Storage_Start(Direction_Front, 0);
	}
}

void PageMain::Voice_prompts(const char* file_name)
{
	XM_CONFIG_VALUE cfg_value;
	 cfg_value.int_value = SimpChinese;
	 GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Language, cfg_value);
	if( cfg_value.int_value == English){
		 GlobalData::Instance()->car_config()->GetValue(CFG_Operation_boot_Voice, cfg_value);
        if(cfg_value.bool_value){
		std::string sound_file = kAudioPath;
		sound_file += file_name;
		MppMdl::Instance()->PlaySound(sound_file.c_str());
	  }
	}
}
void PageMain::ChangeMode_Video(void)
{
	XMLogI("ChangeMode_Video");
	if (GlobalPage::Instance()->page_sys_set()->time_page_) {
     lv_obj_del(GlobalPage::Instance()->page_sys_set()->time_page_);
     GlobalPage::Instance()->page_sys_set()->time_page_ = NULL;
    }
    if (GlobalData::Instance()->opened_subpage_[0]) {
		XMLogI("ChangeMode_Video1");
        lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
        GlobalData::Instance()->opened_subpage_[0] = NULL;
    }
    if (GlobalData::Instance()->opened_subpage_[1]) {
		XMLogI("ChangeMode_Video2");
        lv_obj_del(GlobalData::Instance()->opened_subpage_[1]);
        GlobalData::Instance()->opened_subpage_[1] = NULL;
    }
    if (GlobalPage::Instance()->page_set()->set_page_) {
		XMLogI("ChangeMode_Video3");
        lv_obj_del(GlobalPage::Instance()->page_set()->set_page_);
		GlobalPage::Instance()->page_set()->set_page_=NULL;
		XMLogI("ChangeMode_Video31");
        GlobalPage::Instance()->page_main()->LeaveMainPageWork(false);
		XMLogI("ChangeMode_Video32");
    }
    if (GlobalData::Instance()->UI_mode_ == UIMode_Videotape) {
        if (GlobalPage::Instance()->page_playback()->select_filetype_page_) {
			lv_obj_del(GlobalPage::Instance()->page_playback()->select_filetype_page_);
			GlobalPage::Instance()->page_playback()->select_filetype_page_=NULL;
            GlobalPage::Instance()->page_main()->LeaveMainPageWork(false);
        }
    }
    else if (GlobalData::Instance()->UI_mode_ == UIMode_Photograph) {
        if (GlobalPage::Instance()->page_playback()->select_filetype_page_) {
            GlobalData::Instance()->UI_mode_ = UIMode_Picture;
        }
        ChangeMode(UIMode_Videotape, false);
		XMLogI("ChangeMode_Video4");
     }
    else if (GlobalData::Instance()->UI_mode_ == UIMode_Playback || GlobalData::Instance()->UI_mode_ ==UIMode_Picture) {
        ChangeMode(UIMode_Videotape, false);
		XMLogI("ChangeMode_Video5");
    }
	
}