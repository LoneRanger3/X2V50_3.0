﻿#ifndef WIN32
#include "MppMdl.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/prctl.h>
#include <signal.h>
#include <sys/time.h>
#include "mpi_sys.h"
#include "mpi_vi.h"
#include "mpi_vo.h"
#include "mpi_vpss.h"
#include "mpi_venc.h"
#include "mpi_vb.h"
#include "mpi_vdec.h"
#include "mpi_ai.h"
#include "mpi_ao.h"
#include "mpi_var.h"
#include "fvideo.h"
#include "isp_type.h"
#include "xm_middleware_api.h"
#include "Log.h"
#include "TimeUtil.h"
#include "CommDef.h"
#include "DvrAD.h"
#include "IpcAD.h"
#include "VideoDecUnit.h"
#include "setgpio.h"
#include "DemoDef.h"

#define DISP_ROTATE_SET ROTATE_90
#define DISP_ROTATE_180 0

static int HOR_RES = 640;//416
static int VER_RES = 368; //240
static int HOR_UI_RES = 640; 
static int VER_UI_RES = 480; 
static int BTN_LIMIT = 60; //0
const int AD_VER_LIMIT =368;//240

#if defined SUPPORT_2K
const int kSensorWidth = 1920;
const int kSensorHeight = 1080;
#else
const int kSensorWidth = 2560;
const int kSensorHeight = 1440;
#endif
const int kMainBitrate = 2048;
const int kSubBitrate = 500;
const int kGop = 50;
const int kSamplePerFrame = 320;
#if defined SUPPORT_4K
const int kMaxPicWidth = 3840;
const int kMaxPicHeight = 2144;
const int kSubStreamWidth = 640;
const int kSubStreamHeight = 480;
const int kSubStreambit_rate = 768;
//const int kSubStreambit_rate = 2048;
const int kSubStreamframe_rate = 15;
#elif defined SUPPORT_3K
const int kMaxPicWidth = 3008;
const int kMaxPicHeight = 1800;
const int kSubStreamWidth = 1280;
const int kSubStreamHeight = 720;
const int kSubStreambit_rate = 2048;
const int kSubStreamframe_rate = 20;
#elif defined SUPPORT_2K
const int kMaxPicWidth = 2560;
const int kMaxPicHeight = 1440;
const int kSubStreamWidth = 1280;
const int kSubStreamHeight = 720;
const int kSubStreambit_rate = 2048;
const int kSubStreamframe_rate = 20;
#endif


#define CHECK_RET(express,name) \
		do{ \
			XM_S32 Ret; \
			Ret = express; \
			if (XM_SUCCESS != Ret) \
			{ \
				printf("\033[0;31m%s failed at %s: LINE: %d with %#x!\033[0m\n", \
						name, __FUNCTION__, __LINE__, Ret); \
			} \
		}while(0)

extern "C" int camera_get_stabState(ISP_U8 *pu8StateFlg);
extern "C" int Fvideo_GetLossState(unsigned int *pData);
extern "C" int camera_set_refrence_level(ISP_S32 level);
extern "C" int camera_set_awb(ISP_S32 chn, ISP_U8 u8Enable, ISP_S32 s32Level);
extern "C" int camera_set_reject_flickerV2(int mode);
extern "C" int camera_set_fps(int chn, unsigned char u8Fps);

MppMdl* MppMdl::instance_ = 0;
MppMdl* MppMdl::Instance()
{
	if (0 == instance_) {
		instance_ = new MppMdl();
	}
	return instance_;
}

void MppMdl::Uninstance()
{
	if (0 != instance_) {
		delete instance_;
		instance_ = 0;
	}
}

MppMdl::MppMdl():
	thread_enc_init_(false), channel_num_(XM_MAX_CHANNEL_NUM),
	thread_enc_exit_(false), VoLayer_(0), yuv_width_(0), yuv_height_(0), 
	last_send_audio_time_(0), play_status_(PLAY_PREVIEW), 
	last_osd_set_time_(0), last_ad_loss_(false), ad_loss_time_(0),
	p_img_vir_attr_(NULL), init_(false), thread_genral_exit_(false), 
	sound_file_(""), sound_fp_(NULL), ao_sample_rate_(AUDIO_SAMPLE_RATE_16000),
	ao_config_(false), stop_sound_(false), time_lapse_(XM_TIME_LAPSE_Closed), 
	ad_frame_count_(0), user_mode_(XM_PLAY_BOTH), vod_enable_chn_(false), 
	ad_loss_(true), speak_enable_(false),
	img_pos_(0), u32ImgPhyAddr_(0), ad_type_(VIDEO_FMT_1080), 
	thread_audio_exit_(false), vb_status_(VB_NONE), user_volume_(80),startup_sound_over_(false),
	dec_height_(0)
{
	int i,j;
	for (i = 0; i < channel_num_; i++) {
		XM_MW_Media_Info media_info;
		media_info.video_codec = PT_H264;
		media_info.width = 1920;
		media_info.height = 1080;
		media_info.frame_rate = kFrameRate;
		media_info.bit_rate = kMainBitrate;
		media_info.audio_codec = PT_PCMA;
		media_info.audio_channel = 1;
		media_info.bit_per_sample = 16;
		media_info.samples_per_sec = 16000;
		SetEncInfo(i, 0, media_info);
		media_info.width = kSubStreamWidth;
		media_info.height = kSubStreamHeight;
		media_info.frame_rate = 6;
		media_info.bit_rate = kSubBitrate;
		SetEncInfo(i, 1, media_info);

		audio_remain_len_[i] = 0;
		catch_pic_[i] = false;
		start_recv_pic_[i] = false;
		last_catch_pic_time_[i] = 0;
		mirror_[i] = XM_MIRROR_NONE;
		thumbnail_status_[i] = Thumbnail_None;
		for (int j = 0; j < 2; j++)
			user_osd_.osd[i][j].enable = false;
		osd_time_x_[i] = 0;
		osd_time_y_[i] = 0;
		enable_osd_[i] = false;
		thumbnail_width_[i] = kThumbnailWidth;
		thumbnail_height_[i] = kThumbnailHeight;
	}
	osd_time_x_[4] = 0;
	osd_time_y_[4] = 0;
	enable_osd_[4] = false;
	playback_vpss_grp_ = channel_num_;
	user_rect_info_.s32X = 0;
	user_rect_info_.s32Y = 0;
	user_rect_info_.u32Width = 256;
	user_rect_info_.u32Height = 180;
	ad_proc_ = new IpcAD;
	vdec_proc_ = new VideoDecUnit(playback_vpss_grp_);
	XMLogI("MppMdl() end");
}

MppMdl::~MppMdl()
{
	if (ad_proc_) {
		delete ad_proc_;
	}
	if (vdec_proc_) {
		delete vdec_proc_;
	}
}

bool MppMdl::SendDecFrame(long handle, XM_MW_Media_Frame* media_frame)
{
	bool ret = true;
	if (vdec_proc_) {
		//在播放报警声音时，不播放音乐
        if (media_frame->codec_type == PT_PCMA) {
			//录像回放时，打开声音输出
			if (!speak_enable_) {
				SpeakerEnable(true);
			}
			last_send_audio_time_ = GetTickTime();
			if (sound_fp_) {
				//XMLogW("send audio frame, but sound file playing");
				return true;
			}
        }
		ret =vdec_proc_->SendFrame(handle, media_frame);
	}
	return ret;
}

int MppMdl::ResetDecDev()
{
	if (play_status_ == PLAY_PLAYBACK_VOD && vdec_proc_) {
		vdec_proc_->ResetDev();
	}
	return 0;
}

int MppMdl::PlaySound(const std::string& sound_file)
{
	if (!ao_config_) {
		AoConfig(ao_sample_rate_);
		StartAudioProc();
	}
	sound_file_ = sound_file;

	//播放时，降低采集音量
	SetAIVolume(20);
	return 0;
}

int MppMdl::SetVolume(int sound_volume)
{
	user_volume_ = sound_volume;
	return SetVolumeInternal(user_volume_);
}

int MppMdl::SetVolumeInternal(int sound_volume)
{
	AUDIO_DEV AoDev = 0;
	return XM_MPI_AO_SetVolume(AoDev, sound_volume);
}

int MppMdl::SetAudioOutInfo(int bit_per_sample,	int samples_per_sec, int audio_channel)
{
	return 0;
}

int MppMdl::StartPreview()
{
	if (init_) { 
		StopPlayback();
		//StopPreview();
	}
	else {
		init_ = true;
	}

	ISP_U8 pu8StateFlg;
	for (int i = 0; i < 10; i++) {
		int ret = camera_get_stabState(&pu8StateFlg);
		if (ret == 0) {
			if ((pu8StateFlg & 0xF) == 1) {
				XMLogI("camera_get_stabState, isp ok");
				break;
			}
			else {
				XMLogI("camera_get_stabState, pu8StateFlg=%x", pu8StateFlg);
			}
		}
		else {
			XMLogW("camera_get_stabState error, ret=%d", ret);
		}
		usleep(10000);
	}

	//使能解码器时钟
	XM_Middleware_Periphery_Notify(XM_EVENT_DECODE_ENABLE, "", 1);

	ViConfig();
	//开始编码，采集预览数据
	StartEncProc();
	ChnBind();
	StartGeneralProc();
	StartAudioProc();
	StartSoftVoProc();
	play_status_ = PLAY_PREVIEW;
	//stop_sound_ = true;
	//默认关闭解码器时钟
	XM_Middleware_Periphery_Notify(XM_EVENT_DECODE_ENABLE, "", 0);
	return 0;
}

int MppMdl::StopPreview()
{
	if (play_status_ != PLAY_PREVIEW) {
		return 0;
	}

	XMLogW("before StopPreview");
	//关闭编码，以省内存
	StopEncProc();

	XMLogW("XM_MPI_VI_DisableChn vi_chn_sensor");
	VI_CHN vi_chn_sensor = 1;
	CHECK_RET(XM_MPI_VI_DisableChn(vi_chn_sensor), "XM_MPI_VI_DisableChn");
	XMLogW("after StopPreview");
	return 0;
}

int MppMdl::StopPlayback()
{
	XMLogI("StopPlayback");
	mutex_vdec_.lock();
	vdec_proc_->Stop();
	vdec_ = false;
	mutex_vdec_.unlock();

	//关闭解码器时钟
	XM_Middleware_Periphery_Notify(XM_EVENT_DECODE_ENABLE, "", 0);	

	return 0;
}

int MppMdl::StartPlayback(CUR_PLAY_STATUS play_status, PAYLOAD_TYPE_E codec_type, RECT_S* display_region, bool audio_only, int height)
{	
	if (audio_only) {
		return 0;
	}

	XMLogI("StartPlayback, play staus=%d, height=%d", play_status, height);
	//使能解码器时钟
	XM_Middleware_Periphery_Notify(XM_EVENT_DECODE_ENABLE, "", 1);	
	if (play_status_ == play_status) {
		VBInit(height==kMaxPicHeight ? VB_4K_1080 : VB_2K_1080);
		//录像回放，且分辨率不一样，要重置解码器。因为加放4K，参考帧只有1个，2K和1080参考帧有2个
		if (dec_height_ != height && play_status == PLAY_PLAYBACK_VOD) {
			mutex_vdec_.lock();
			vdec_proc_->Stop();
			vdec_proc_->SetWidth(0, height);
			dec_height_ = height;
			vdec_proc_->Start(play_status == PLAY_PLAYBACK_VOD);
			vdec_ = true;
			mutex_vdec_.unlock();
		}
		return 0;
	}

	if (play_status == PLAY_PLAYBACK_VOD)
		XMLogI("change chn bind to play vod, codec=%d, display region[%d-%d-%d-%d]", 
			codec_type, display_region->s32X, display_region->s32Y, display_region->u32Width, display_region->u32Height);
	else 
		XMLogI("change chn bind to play picture, codec=%d, display region[%d-%d-%d-%d]", 
			codec_type, display_region->s32X, display_region->s32Y, display_region->u32Width, display_region->u32Height);
	//解绑
	ChnUnBind();
	//停止预览
	StopPreview();
	VBInit(height==kMaxPicHeight ? VB_4K_1080 : VB_2K_1080);
	vod_enable_chn_ = false;
	CUR_PLAY_STATUS pre_status = play_status_;
	play_status_ = play_status;
	mutex_vdec_.lock();
	vdec_proc_->SetWidth(0, height);
	dec_height_ = height;
	vdec_proc_->Start(play_status == PLAY_PLAYBACK_VOD);
	vdec_ = true;
	mutex_vdec_.unlock();

	return XM_SUCCESS;
}

int MppMdl::SystemInit()
{
	CHECK_RET(XM_MPI_SYS_Init(), "XM_MPI_SYS_Init");
	VB_CONF_S stVbConf;
	stVbConf.u32MaxPoolCnt = 2;
	stVbConf.astCommPool[0].u32BlkSize = VER_RES * HOR_RES * 3 / 2;
	stVbConf.astCommPool[0].u32BlkCnt = 4;
	stVbConf.astCommPool[1].u32BlkSize = kSubStreamWidth * kSubStreamHeight * 3 / 2;
	stVbConf.astCommPool[1].u32BlkCnt = 3;
	CHECK_RET(XM_MPI_VB_SetConf(&stVbConf), "XM_MPI_VB_SetConf");
	CHECK_RET(XM_MPI_VB_Init(), "XM_MPI_VB_Init()");
	XM_MPI_VB_SetMode(1);
	AdInit();
	VoConfig();

	int enc_substream = 4;
	for (int i = 0; i < channel_num_; i++)
		OsdTitleCreate(i, i, kOSDWidth*20, kOSDHeight);
	OsdTitleCreate(enc_substream, enc_substream, kOSDWidth*20, kOSDHeight);
	VBInit(VB_4K_1080);
	return 0;
}

int MppMdl::VBInit(VBStatus vb_st)
{
	XMLogI("VBInit, cur status=%d, vb_st=%d", vb_status_, vb_st);
	if (vb_status_ == vb_st) {
		return 0;
	}

	if (vb_status_ != VB_NONE) {
		usleep(100*1000);
	}
	
	VB_CPOOL_S stVbPool;
	if (vb_status_ == VB_4K_1080) {
		XMLogI("VBInit, 1111");
		stVbPool.u32BlkSize = kMaxPicWidth * kMaxPicHeight * 3 / 2;
		stVbPool.u32BlkCnt = 2;//3
		CHECK_RET(XM_MPI_VB_DestroyPool(&stVbPool), "XM_MPI_VB_DestroyPool");
		stVbPool.u32BlkSize = 1920 * 1080 * 3 / 2;
		stVbPool.u32BlkCnt = 3;
		CHECK_RET(XM_MPI_VB_DestroyPool(&stVbPool), "XM_MPI_VB_DestroyPool");
	}
	else if (vb_status_ == VB_2K_1080) {
		XMLogI("VBInit, 2222");
		stVbPool.u32BlkSize = 2560 * 1440 * 3 / 2;
		stVbPool.u32BlkCnt = 3;
		CHECK_RET(XM_MPI_VB_DestroyPool(&stVbPool), "XM_MPI_VB_DestroyPool");
		stVbPool.u32BlkSize = 1920 * 1080 * 3 / 2;
		stVbPool.u32BlkCnt = 3;
		CHECK_RET(XM_MPI_VB_DestroyPool(&stVbPool), "XM_MPI_VB_DestroyPool");
	}
	else if (vb_status_ == VB_1080_1080) {
		XMLogI("VBInit, 2223333");
		stVbPool.u32BlkSize = 1920 * 1080 * 3 / 2;
		stVbPool.u32BlkCnt = 6;
		CHECK_RET(XM_MPI_VB_DestroyPool(&stVbPool), "XM_MPI_VB_DestroyPool");
	}

	/************************************************
	 step2:  VB init
	*************************************************/
	if (vb_st == VB_4K_1080)	{
		XMLogI("VBInit, 3333");
		stVbPool.u32BlkSize = kMaxPicWidth * kMaxPicHeight * 3 / 2;
		stVbPool.u32BlkCnt = 2;//3
		CHECK_RET(XM_MPI_VB_CreatePool(&stVbPool), "XM_MPI_VB_CreatePool");
		stVbPool.u32BlkSize = 1920 * 1080 * 3 / 2;
		stVbPool.u32BlkCnt = 3;
		CHECK_RET(XM_MPI_VB_CreatePool(&stVbPool), "XM_MPI_VB_CreatePool");
	}
	else if (vb_st == VB_2K_1080)	{
		XMLogI("VBInit, 4444");
		stVbPool.u32BlkSize = 2560 * 1440 * 3 / 2;
		stVbPool.u32BlkCnt = 3;
		CHECK_RET(XM_MPI_VB_CreatePool(&stVbPool), "XM_MPI_VB_CreatePool");
		stVbPool.u32BlkSize = 1920 * 1080 * 3 / 2;
		stVbPool.u32BlkCnt = 3;
		CHECK_RET(XM_MPI_VB_CreatePool(&stVbPool), "XM_MPI_VB_CreatePool");
	}
	else if (vb_st == VB_1080_1080)    {
		XMLogI("VBInit, 6666");
		stVbPool.u32BlkSize = 1920 * 1080 * 3 / 2;
		stVbPool.u32BlkCnt = 6;
		CHECK_RET(XM_MPI_VB_CreatePool(&stVbPool), "XM_MPI_VB_CreatePool");
     }

	vb_status_ = vb_st;
	return 0;
}

int MppMdl::SetPreviewRegion(int hor_res, int ver_res, int physical_hor_res, int physical_ver_res)
{
	XM_S32 s32ImgSize = physical_hor_res * physical_ver_res * 4;
	CHECK_RET(XM_MPI_SYS_MmzAlloc(&u32ImgPhyAddr_, &p_img_vir_attr_, "Image", NULL, s32ImgSize), "XM_MPI_SYS_MmzAlloc");
	if (p_img_vir_attr_ == NULL) {
		XMLogE("XM_MPI_SYS_MmzAlloc, img is null");
		return -1;
	}
	
	//ShowLogo(PlayLogo_Start);
	memset(p_img_vir_attr_, 0, s32ImgSize);
	
    VO_IMAGE_LAYER_ATTR_S stImageLayerAttr = {0};
	stImageLayerAttr.u32PhyAddr = u32ImgPhyAddr_;
	stImageLayerAttr.stImageSize.u32Width = ver_res;
	stImageLayerAttr.stImageSize.u32Height = hor_res; 
	stImageLayerAttr.stDispRect.s32X = 0;
	stImageLayerAttr.stDispRect.s32Y = 0;
	stImageLayerAttr.stDispRect.u32Width = physical_ver_res;
	stImageLayerAttr.stDispRect.u32Height = physical_hor_res;
	stImageLayerAttr.enPixFormat = PIXEL_FORMAT_RGB_1555;
	stImageLayerAttr.u32Effect = 0x00FF10FF;

	XM_MPI_VO_SetImageLayerAttr(VoLayer_, &stImageLayerAttr);
	XM_MPI_VO_EnableImageLayer(VoLayer_);

	return 0;
}

void* MppMdl::GetImgVirAttr() 
{
	return img_pos_==0 ? p_img_vir_attr_ : (p_img_vir_attr_+ HOR_UI_RES * VER_UI_RES * 2);
}

void* MppMdl::SwapImgVirAttr()
{
	void* src;
	void* dst;
	if (img_pos_ == 0) {
		src = p_img_vir_attr_;
		dst = p_img_vir_attr_ + VER_UI_RES * HOR_UI_RES * 2;
		img_pos_ = 1;
	} else {
		dst = p_img_vir_attr_;
		src = p_img_vir_attr_ + VER_UI_RES * HOR_UI_RES * 2;
		img_pos_ = 0;
	}
	memcpy(dst, src, VER_UI_RES * HOR_UI_RES * 2);
	return dst;
}

void MppMdl::CommitSwapImg()
{
	VO_IMAGE_LAYER_ATTR_S stImageLayerAttr = {0};
	XM_MPI_VO_GetImageLayerAttr(0, &stImageLayerAttr);
	stImageLayerAttr.u32PhyAddr = u32ImgPhyAddr_ + img_pos_ * VER_UI_RES * HOR_UI_RES * 2;
	XM_MPI_VO_SetImageLayerAttr(0, &stImageLayerAttr);
}

void MppMdl::ShowLogo(PlayLogo logo)
{
	std::string logo_path;
	if (logo == PlayLogo_Start) {
		logo_path = "/usr/share/240x320.rgb";
	}
	else if (logo == PlayLogo_Shutdown) {
		logo_path = "/usr/share/shut_down480x640.rgb";
	}
	else {
		return;
	}

	FILE* fp = fopen(logo_path.c_str(), "rb");
	if (!fp) {
		XMLogE("open logo file error");
		memset(p_img_vir_attr_, 255, VER_UI_RES*HOR_UI_RES*2);
		return;
	}

	uint8_t* logo_buf = new uint8_t[VER_UI_RES * HOR_UI_RES * 2];
	unsigned short* ptr_dst = (unsigned short*)logo_buf;
	int bytes = fread(logo_buf, HOR_UI_RES * VER_UI_RES * 2, 1, fp);
	if (bytes <= 0) {
		XMLogE("read logo file error");
		return;
	}

	void* img_attr = GetImgVirAttr();
	memcpy(img_attr, logo_buf, HOR_UI_RES * VER_UI_RES * 2);
	fclose(fp);
	delete logo_buf;
}

void* WINAPI MPPGeneralThreadProc(void* param)
{
#ifndef WIN32
	prctl(PR_SET_NAME, "mpp_general");
#endif
	MppMdl* manager = (MppMdl*)param;
	manager->GeneralProc();
	return 0;
}
void* WINAPI MPPAudioThreadProc(void* param)
{
#ifndef WIN32
	prctl(PR_SET_NAME, "mpp_audio");
#endif
	MppMdl* manager = (MppMdl*)param;
	manager->AudioProc();
	return 0;
}

int MppMdl::Init()
{
	XMLogI("Init");
	
	AoConfig(ao_sample_rate_);
	AiConfig();

	char pic_name[32];
	int i;
	for (i = 0; i < XM_MAX_OSD_TIME_NUM; i++) {
		memset(pic_name, 0, sizeof(pic_name));
#if 1
		if (i < 10)
			sprintf(pic_name, "/mnt/custom/Pic/osd_str%d.rgb", i);
		else if (i == 10)
			sprintf(pic_name, "/mnt/custom/Pic/osdfh.rgb");
		else if (i == 11)
			sprintf(pic_name, "/mnt/custom/Pic/osdxg.rgb");	
#else
		if (i < 10)
			sprintf(pic_name, "/mnt/custom/Pic/num_%d.rgb", i);
		else if (i == 10)
			sprintf(pic_name, "/mnt/custom/Pic/maohao.rgb");
		else if (i == 11)
			sprintf(pic_name, "/mnt/custom/Pic/fenge.rgb");	
#endif
		else if (i == 12)
			sprintf(pic_name, "/mnt/custom/Pic/kongge.rgb");		
		else if (i == 13)
			sprintf(pic_name, "/mnt/custom/Pic/char_A.rgb");	
		else if (i == 14)
			sprintf(pic_name, "/mnt/custom/Pic/char_P.rgb");	
		else if (i == 15)
			sprintf(pic_name, "/mnt/custom/Pic/char_M.rgb");

        #if OSD_SYMBOL_TEST
		else if (i > 15 && i <= 41)
     		sprintf(pic_name, "/mnt/custom/Pic/osd_str_%c.rgb", 'a'+(i - 16));
		else if (i > 41 && i <= 67)
     		sprintf(pic_name, "/mnt/custom/Pic/osd_str%c.rgb", 'A'+(i - 42));
		#endif
		
		FILE* fp = fopen(pic_name, "rb");
		if (!fp) {
			XMLogE("Set osd, read rgb failed");
			return 0;
		}
		if (i == 12) {
			memset(osd_num_buf_[i], 0, kOSDWidth*kOSDHeight*2);
		} else {
			fread(osd_num_buf_[i], 1, kOSDWidth*kOSDHeight*2, fp);
		}
		fclose(fp);
	}
	memset(osd_show_buf_, 0, sizeof(osd_show_buf_));

	return 0;
}

int MppMdl::AdInit() 
{
	XMLogI("AdInit");
	if (ad_proc_)
		ad_proc_->Init(channel_num_);
	XMLogI("after AdInit");
	return 0;
}

int MppMdl::VoConfig()
{
	VO_CHN vo_chn_full = 0, vo_chn_half_l = 1, vo_chn_half_r = 2;
	XMLogI("VoConfig");
	/* video layer */	
	VO_VIDEO_LAYER_ATTR_S stVideoLayerAttr;
	stVideoLayerAttr.stDispRect.s32X = 0;
	stVideoLayerAttr.stDispRect.s32Y = 0;
	stVideoLayerAttr.stDispRect.u32Width = VER_RES;
	stVideoLayerAttr.stDispRect.u32Height = HOR_RES*2.5;
	stVideoLayerAttr.stImageSize.u32Width = VER_RES;
	stVideoLayerAttr.stImageSize.u32Height = HOR_RES*2.5;
	stVideoLayerAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;

	CHECK_RET(XM_MPI_VO_SetVideoLayerAttr(VoLayer_, &stVideoLayerAttr), "XM_MPI_VO_SetVideoLayerAttr");

	VO_CHN_ATTR_S stVoChnAttr;
	memset(&stVoChnAttr, 0, sizeof(stVoChnAttr));
	stVoChnAttr.stRect.s32X = BTN_LIMIT;
	stVoChnAttr.stRect.s32Y = 0;
	//占满整个显示区域
	stVoChnAttr.stRect.u32Width = VER_RES;
	stVoChnAttr.stRect.u32Height = HOR_RES;
	CHECK_RET(XM_MPI_VO_SetChnAttr(VoLayer_, vo_chn_full, &stVoChnAttr), "XM_MPI_VO_SetChnAttr");
	CHECK_RET(XM_MPI_VO_SetChnAttr(VoLayer_, vo_chn_half_l, &stVoChnAttr), "XM_MPI_VO_SetChnAttr");

	//获取用户设置的AD参数
	GetRectInfo(stVoChnAttr.stRect);
	XMLogE("GetRectInfo, %d-%d-%d-%d", 
		stVoChnAttr.stRect.s32X, stVoChnAttr.stRect.s32Y, 
		stVoChnAttr.stRect.u32Width, stVoChnAttr.stRect.u32Height);

	CHECK_RET(XM_MPI_VO_SetChnAttr(VoLayer_, vo_chn_half_r, &stVoChnAttr), "XM_MPI_VO_SetChnAttr");	

	CHECK_RET(XM_MPI_VO_EnableChn(VoLayer_, vo_chn_full), "XM_MPI_VO_EnableChn");
	CHECK_RET(XM_MPI_VO_EnableChn(VoLayer_, vo_chn_half_l), "XM_MPI_VO_EnableChn");
	CHECK_RET(XM_MPI_VO_EnableChn(VoLayer_, vo_chn_half_r), "XM_MPI_VO_EnableChn");

	if (AdLoss() || user_mode_ == XM_PLAY_SENSOR) {
		last_ad_loss_ = true;
		last_play_mode_ = XM_PLAY_SENSOR;
	}else {
		last_play_mode_ = user_mode_;
		last_ad_loss_ = false;
	}
	
	/***********************************************
	Config var
	************************************************/
	VAR_CHN var_chn_disp = 0;
	CHECK_RET(XM_MPI_VAR_SetRotate(var_chn_disp, DISP_ROTATE_SET), "XM_MPI_VAR_SetRotate");
	CHECK_RET(XM_MPI_VAR_EnableChn(var_chn_disp), "XM_MPI_VAR_EnableChn");

	return XM_SUCCESS;
}

/**
 * @brief 对后拉画面框的参数进行合法性判断
 * 
 * 
 * @param get_info 后拉参数
 * @return 0
 */
int MppMdl::SetADRectInfos(RECT_S& input_info)
{
	user_rect_info_.u32Width = input_info.u32Width;
	user_rect_info_.u32Height = input_info.u32Height;
	user_rect_info_.s32X = input_info.s32X;
	user_rect_info_.s32Y = input_info.s32Y;	
	return 0;
}

/**
 * @brief 对后拉画面框的参数进行坐标翻转处理
 *
 * 后拉框在屏幕内坐标受限，不能遮盖按钮
 * 画面270度翻转
 *
 * @param get_info 后拉参数
 * @return 0
 */
int MppMdl::GetRectInfo(RECT_S& get_info)
{
	// 获取合法性处理之后的参数，并对其坐标翻转处理
	get_info.u32Width = user_rect_info_.u32Height;
	get_info.u32Height = user_rect_info_.u32Width;
	get_info.s32X = AD_VER_LIMIT - user_rect_info_.s32Y - user_rect_info_.u32Height + BTN_LIMIT;
	get_info.s32Y = user_rect_info_.s32X;
	return 0;
}

int MppMdl::VencConfig(int channel, int stream)
{
	VENC_CHN_ATTR_S stVencChnAttr;
	memset(&stVencChnAttr, 0, sizeof(stVencChnAttr));
	SetEncChnAttr(channel, stream, stVencChnAttr);

	VENC_PARAM_REF_EX_S stRefParam;
	memset(&stRefParam, 0, sizeof(stRefParam));

	int height = enc_medias_[0][0].height;
	if (channel == 0 && height >= kMaxPicHeight) {
		stRefParam.rfcEnable = 0;
		stRefParam.rfwEnable = 1;
		stRefParam.u32HwCfgSet = 2;
		stRefParam.bPreReleaseVb = 1;
	} else {
		stRefParam.rfcEnable = 1;
		stRefParam.rfwEnable = 0;
		stRefParam.u32HwCfgSet = 1;
	}
	CHECK_RET(XM_MPI_VENC_SetRefParamEx(channel + stream, &stRefParam), "XM_MPI_VENC_SetRefParamEx");
	CHECK_RET(XM_MPI_VENC_CreateChn(channel + stream, &stVencChnAttr), "XM_MPI_VENC_CreateChn");
	XMLogI("XM_MPI_VENC_CreateChn, channel=%d", channel);
	return XM_SUCCESS;
}

int MppMdl::SetTimeLapse(XMTimeLapse time_lapse)
{
	XMLogI("SetTimeLapse, %d", (int)time_lapse);
	time_lapse_ = time_lapse;
	int rate = kFrameRate;
	if (time_lapse == XM_TIME_LAPSE_Frame_1)
		rate = 1;
	else if (time_lapse == XM_TIME_LAPSE_Frame_2)
		rate = 2;
	else if (time_lapse == XM_TIME_LAPSE_Frame_5)
		rate = 5;
	
	int channel = 0;
	enc_medias_[channel][0].frame_rate = rate;
	ViModify(channel, false);
	return 0;
}

int MppMdl::SetEncChnAttr(int channel, int stream, VENC_CHN_ATTR_S& enc_chn_attr)
{
	PAYLOAD_TYPE_E video_codec_type = PT_H265;
	int width = 1280;
	int height = 720;
	int bit_rate = 2048;
	int frame_rate = 25;

	if(channel == 4) {
		video_codec_type = enc_medias_[0][1].video_codec;
		width = enc_medias_[0][1].width;
		height = enc_medias_[0][1].height;
		bit_rate = enc_medias_[0][1].bit_rate;
		frame_rate = enc_medias_[0][1].frame_rate;
	}
	else {
		video_codec_type = enc_medias_[channel][stream].video_codec;
		width = enc_medias_[channel][stream].width;
		height = enc_medias_[channel][stream].height;
		bit_rate = enc_medias_[channel][stream].bit_rate;
		frame_rate = enc_medias_[channel][stream].frame_rate;
	}
	
	if (video_codec_type == PT_H265) {
		VENC_ATTR_H265_S stH265Attr = {0};
		VENC_ATTR_H265_CBR_S stH265Cbr = {0};	
		stH265Attr.u32MaxPicWidth = width;
		stH265Attr.u32MaxPicHeight = height;
		if (channel == 0)
			stH265Attr.u32BufSize  = kMaxFrameLen;
		else if (channel == 1)
			stH265Attr.u32BufSize  = 500*1024;
		else 
			stH265Attr.u32BufSize  = 200*1024;
		stH265Attr.u32Profile  = 1;
		stH265Attr.bByFrame = XM_TRUE;
		stH265Attr.u32PicWidth = width;
		stH265Attr.u32PicHeight = height;
		stH265Attr.u32BFrameNum = 0;
		stH265Attr.u32RefNum = 0;
		enc_chn_attr.stVeAttr.stAttrH265e = stH265Attr;
		stH265Cbr.u32Gop = (channel == 0 && height >= kMaxPicHeight) ? 1 : (frame_rate/2);
		stH265Cbr.u32StatTime = 1;
		stH265Cbr.u32SrcFrmRate = frame_rate;//kFrameRate;
		stH265Cbr.fr32DstFrmRate = frame_rate;
		stH265Cbr.u32BitRate = bit_rate;
		stH265Cbr.u32FluctuateLevel = 1;
		enc_chn_attr.stRcAttr.stAttrH265Cbr = stH265Cbr;
		enc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H265CBR;
	} else if (video_codec_type == PT_H264) {
		VENC_ATTR_H264_S stH264Attr = {0};
		VENC_ATTR_H264_CBR_S stH264Cbr = {0};
		stH264Attr.u32MaxPicWidth = width;
		stH264Attr.u32MaxPicHeight = height;
		stH264Attr.u32BufSize  = channel == 0 ? 1024000 : (width*height/4);
		stH264Attr.u32Profile  = 1;
		stH264Attr.bByFrame = XM_TRUE;
		stH264Attr.u32PicWidth = width;
		stH264Attr.u32PicHeight = height;
		stH264Attr.u32BFrameNum = 0;
		stH264Attr.u32RefNum = 0;
		enc_chn_attr.stVeAttr.stAttrH264e = stH264Attr;
		stH264Cbr.u32Gop = frame_rate/2;
		stH264Cbr.u32StatTime = 1;
		stH264Cbr.u32SrcFrmRate = kFrameRate;
		stH264Cbr.fr32DstFrmRate = frame_rate;
		stH264Cbr.u32BitRate = bit_rate;
		stH264Cbr.u32FluctuateLevel = 1;
		enc_chn_attr.stRcAttr.stAttrH264Cbr= stH264Cbr;
		enc_chn_attr.stRcAttr.enRcMode = VENC_RC_MODE_H264CBR;
	} 
	enc_chn_attr.stVeAttr.enType = video_codec_type;
	XMLogI("video enc config, channel=%d, stream=%d, width=%d, height=%d, rate=%d, codec_type=%d", 
		channel, stream, width, height, frame_rate, (int)video_codec_type);
	return 0;
}

int MppMdl::JpegConfig(int channel)
{
	int iRet = 0;
	VENC_CHN VeChn = channel + channel_num_;
	VENC_CHN_ATTR_S stVencChnAttr;
	VENC_ATTR_JPEG_S stJPEGAttr;
	VENC_ATTR_MJPEG_CBR_S stAttrMjpegeCbr;

	memset(&stVencChnAttr, 0, sizeof(VENC_CHN_ATTR_S));
	memset(&stJPEGAttr, 0, sizeof(VENC_ATTR_JPEG_S));
	stVencChnAttr.stVeAttr.enType = PT_JPEG;
	stJPEGAttr.bByFrame = XM_TRUE;
	stJPEGAttr.bSupportDCF = XM_FALSE;
	int width = enc_medias_[channel][0].width;
	int height = enc_medias_[channel][0].height;
	stJPEGAttr.u32MaxPicWidth = width;
	stJPEGAttr.u32MaxPicHeight = height;
	stJPEGAttr.u32PicWidth = width;
	stJPEGAttr.u32PicHeight = height;
	stJPEGAttr.u32BufSize  = channel == 0 ? kMaxFrameLen : (1920*1080/4);
	memcpy(&stVencChnAttr.stVeAttr.stAttrJpeg, &stJPEGAttr, sizeof(VENC_ATTR_JPEG_S));
	iRet = XM_MPI_VENC_CreateChn(VeChn, &stVencChnAttr);
	if(iRet != XM_SUCCESS) {
		XMLogE("Create JPEG Chn failed!====>value:%#X\n",iRet);
		return XM_FAILURE;
	}
	
	return XM_SUCCESS;
}

int MppMdl::ViModify(int channel, bool mirror)
{
	VI_CHN vi_chn_enc = 0, vi_chn_sensor = 1, vi_chn_substream = 2, vi_chn_ad = 3;
	VI_CHN vi_chn = channel == 0 ? vi_chn_enc : vi_chn_ad;
	VI_CHN_ATTR_S stViChnAttr;
	XM_MPI_VI_GetChnAttr(vi_chn, &stViChnAttr);
	int width = enc_medias_[channel][0].width;
	int height = enc_medias_[channel][0].height;
	int rate = enc_medias_[channel][0].frame_rate;
	XMLogI("ViModify, vi_chn=%d, channel=%d, width=%d, height=%d, rate=%d, mirror=%d", 
		vi_chn, channel, width, height, rate, mirror);
	stViChnAttr.stDestSize.u32Width = width;
	stViChnAttr.stDestSize.u32Height = height;
	stViChnAttr.s32FrameRate = rate;

	stViChnAttr.bMirror = mirror ? XM_TRUE : XM_FALSE;
	CHECK_RET(XM_MPI_VI_DisableChn(vi_chn), "XM_MPI_VI_DisableChn");
	usleep(50*1000);
	XM_MPI_VI_ResetChn(vi_chn);
	CHECK_RET(XM_MPI_VI_SetChnAttr(vi_chn, &stViChnAttr), "XM_MPI_VI_SetChnAttr");
	CHECK_RET(XM_MPI_VI_EnableChn(vi_chn), "XM_MPI_VI_EnableChn");
}

int MppMdl::ViConfig()
{
	int width = enc_medias_[0][0].width;
    int height = enc_medias_[0][0].height;
    int rate = enc_medias_[0][0].frame_rate;

	VI_CHN vi_chn_enc = 0, vi_chn_sensor = 1, vi_chn_substream = 2, vi_chn_ad = 3;
	VI_CHN_ATTR_S stViChnAttr;
	memset(&stViChnAttr, 0, sizeof(VI_CHN_ATTR_S));
	stViChnAttr.stCapRect.s32X = 0;
	stViChnAttr.stCapRect.s32Y = 0;
	stViChnAttr.bMirror = XM_FALSE;
	stViChnAttr.bFlip = XM_FALSE;
	stViChnAttr.enScanMode = VI_SCAN_PROGRESSIVE;
	stViChnAttr.s32SrcFrameRate = rate;
    stViChnAttr.s32FrameRate = rate;
	stViChnAttr.stCapRect.u32Width = kSensorWidth;
	stViChnAttr.stCapRect.u32Height = kSensorHeight;
	stViChnAttr.stDestSize.u32Width = kMaxPicWidth;
	stViChnAttr.stDestSize.u32Height = kMaxPicHeight;
	CHECK_RET(XM_MPI_VI_SetChnAttr(vi_chn_enc, &stViChnAttr), "XM_MPI_VI_SetChnAttr");
	XM_MPI_VI_ResetChn(vi_chn_enc);
	stViChnAttr.stCapRect.u32Width = kSensorWidth;
	stViChnAttr.stCapRect.u32Height = kSensorHeight;
	stViChnAttr.stDestSize.u32Width = width;
	stViChnAttr.stDestSize.u32Height = height;
	stViChnAttr.s32FrameRate = rate;
	XMLogI("ViConfig, vi_chn_enc=%d, width=%d, height=%d, ad type=%d", vi_chn_enc, width, height, ad_type_);
	CHECK_RET(XM_MPI_VI_SetChnAttr(vi_chn_enc, &stViChnAttr), "XM_MPI_VI_SetChnAttr");
	CHECK_RET(XM_MPI_VI_EnableChn(vi_chn_enc), "XM_MPI_VI_EnableChn");
	stViChnAttr.s32FrameRate = rate;

	stViChnAttr.stDestSize.u32Width =HOR_RES ; //640;
	stViChnAttr.stDestSize.u32Height =VER_RES ; //368;
	CHECK_RET(XM_MPI_VI_SetChnAttr(vi_chn_sensor, &stViChnAttr), "XM_MPI_VI_SetChnAttr");

	// if (last_play_mode_ == XM_PLAY_BOTH || last_play_mode_ == XM_PLAY_AD) {
	// 	stViChnAttr.stDestSize.u32Width = 320;
	// 	CHECK_RET(XM_MPI_VI_SetChnAttr(vi_chn_sensor, &stViChnAttr), "XM_MPI_VI_SetChnAttr");
	// }
	CHECK_RET(XM_MPI_VI_EnableChn(vi_chn_sensor), "XM_MPI_VI_EnableChn");

	int substream_framerate = enc_medias_[0][1].frame_rate;
    stViChnAttr.s32FrameRate = substream_framerate;

	stViChnAttr.stDestSize.u32Width = kSubStreamWidth;
	stViChnAttr.stDestSize.u32Height = kSubStreamHeight;
	CHECK_RET(XM_MPI_VI_SetChnAttr(vi_chn_substream, &stViChnAttr), "XM_MPI_VI_SetChnAttr");
	CHECK_RET(XM_MPI_VI_EnableChn(vi_chn_substream), "XM_MPI_VI_EnableChn");

    //设置isp帧率
	kFrameRate=rate;
    camera_set_fps(0, rate);

	stViChnAttr.s32SrcFrameRate = 25;
	stViChnAttr.s32FrameRate = 25;
	if (ad_type_ == VIDEO_FMT_1080) {
		stViChnAttr.stCapRect.u32Width = 1920;
		stViChnAttr.stCapRect.u32Height = 1080;
	} else {
		stViChnAttr.stCapRect.u32Width = 1280;
		stViChnAttr.stCapRect.u32Height = 720;
	}
	stViChnAttr.stDestSize.u32Width = enc_medias_[1][0].width;
	stViChnAttr.stDestSize.u32Height = enc_medias_[1][0].height;
	
	if (mirror_[1] == XM_MIRROR_VI || mirror_[1] == XM_MIRROR_VENC)
		stViChnAttr.bMirror = XM_TRUE;

	CHECK_RET(XM_MPI_VI_SetChnAttr(vi_chn_ad, &stViChnAttr), "XM_MPI_VI_SetChnAttr");
	CHECK_RET(XM_MPI_VI_EnableChn(vi_chn_ad), "XM_MPI_VI_EnableChn");

	return XM_SUCCESS;
}

int MppMdl::AiConfig()
{
	XMLogI("AiConfig");
	XM_S32 s32Ret;
	AUDIO_DEV AiDev = 0;
	AUDIO_DEV AoDev = 0;
	AI_CHN AiChn = 0;
	AO_CHN AoChn = 0;
	AIO_ATTR_S stAiAttr;
	AI_VQE_CONFIG_S stAiVqeConfig;

	// AI public attribute
	memset(&stAiAttr, 0, sizeof(AIO_ATTR_S));
	stAiAttr.enSamplerate = AUDIO_SAMPLE_RATE_16000;
	stAiAttr.enBitwidth = AUDIO_BIT_WIDTH_16;
	stAiAttr.enWorkmode = AIO_MODE_I2S_MASTER;
	stAiAttr.enSoundmode = AUDIO_SOUND_MODE_MONO;
	stAiAttr.u32EXFlag = 1;
	stAiAttr.u32FrmNum = 30;
	stAiAttr.u32PtNumPerFrm = kSamplePerFrame;
	stAiAttr.u32ChnCnt = 1;
	stAiAttr.u32ClkSel = 1;
	s32Ret = XM_MPI_AI_SetPubAttr(AiDev, &stAiAttr);
	if (s32Ret != XM_SUCCESS) {
		XMLogE("XM_MPI_AI_SetPubAttr(%d) failed: %#x", AiDev, s32Ret);
		return XM_FAILURE;
	}

	// AI channels enable
	for (AiChn = 0; AiChn < 1; AiChn++) {
		memset(&stAiVqeConfig, 0, sizeof(AI_VQE_CONFIG_S));
		stAiVqeConfig.bAecOpen = 0;
		stAiVqeConfig.bAgcOpen = 0;
		stAiVqeConfig.bAnrOpen = 0;
		s32Ret = XM_MPI_AI_SetVqeAttr(AiDev, AiChn, AoDev, AoChn, &stAiVqeConfig);
		if (s32Ret) {
			XMLogE("XM_MPI_AI_SetVqeAttr(%d) failed: %#x\n", AiDev, s32Ret);
			return XM_FAILURE;
		}

		s32Ret = XM_MPI_AI_Enable(AiDev, AiChn);
		if (s32Ret) {
			XMLogE("XM_MPI_AI_Enable(%d,%d) failed: %#x\n", AiDev, AiChn, s32Ret);
			return XM_FAILURE;
		} 
	}

	SetAIVolume(20);
	return XM_SUCCESS;
}

int MppMdl::AoConfig(AUDIO_SAMPLE_RATE_E sample_rate)
{
	if (ao_config_) 
		return XM_SUCCESS;

	XM_S32 s32Ret;
	AUDIO_DEV AoDev = 0;
	AIO_ATTR_S stAoAttr;

	XMLogI("AoConfig, sample_rate=%d", (int)sample_rate);
	XM_MPI_AO_Disable(AoDev);
	// AO public attribute
	memset(&stAoAttr, 0, sizeof(AIO_ATTR_S));
	stAoAttr.enSamplerate = sample_rate;
	stAoAttr.enBitwidth = AUDIO_BIT_WIDTH_16; 
	stAoAttr.enWorkmode = AIO_MODE_I2S_MASTER;
	stAoAttr.enSoundmode = AUDIO_SOUND_MODE_MONO; 
	stAoAttr.u32EXFlag = 1;
	stAoAttr.u32FrmNum = 30;
	stAoAttr.u32PtNumPerFrm = 160;
	stAoAttr.u32ChnCnt = 2;
	stAoAttr.u32ClkSel = 1;
	s32Ret = XM_MPI_AO_SetPubAttr(AoDev,&stAoAttr);
	if(XM_SUCCESS != s32Ret) {
		XMLogE("XM_MPI_AO_SetPubAttr(%d) failed: %#x", AoDev, s32Ret);
		return  XM_FAILURE;
	}

	// AO enable
	s32Ret = XM_MPI_AO_Enable(AoDev);
	if(XM_SUCCESS != s32Ret) {
		XMLogE("XM_MPI_AO_Enable(%d) failed: %#x", AoDev, s32Ret);
		return  XM_FAILURE;
	}

	SetVolumeInternal(user_volume_);
	ao_config_ = true;
	return XM_SUCCESS;
}

int MppMdl::ChnBind()
{
	VO_CHN vo_chn_full = 0, vo_chn_half_l = 1, vo_chn_half_r = 2;
	VI_CHN vi_chn_enc = 0, vi_chn_sensor = 1, vi_chn_substream = 2, vi_chn_ad = 3;
	VO_DEV vo_dev = 0;
	VO_LAYER vo_Layer = 0;
	VAR_CHN var_chn_disp = 0, var_chn_ad = 1;
	int enc_chn = 0, enc_chn_ad = 1, jpg_enc_chn = 2, jpg_enc_ad = 3, enc_substream = 4;

	MPP_CHN_S stSrcChn[8] = {
		[0] = {XM_ID_VIU, 0, vi_chn_enc},
		[1] = {XM_ID_VIU, 0, vi_chn_sensor},
		[2] = {XM_ID_VAR, 0, var_chn_disp},
		[3] = {XM_ID_VENC, 0, enc_chn},
		[4] = {XM_ID_VENC, 0, enc_chn_ad},
		[5] = {XM_ID_VIU, 0, vi_chn_substream},
	};

	MPP_CHN_S stDestChn[8] = {
		[0] = {XM_ID_VENC, 0, enc_chn},
		[1] = {XM_ID_VAR, 0, var_chn_disp},
		[2] = {XM_ID_VOU, vo_dev, vo_chn_full},
		[3] = {XM_ID_VENC, 0, jpg_enc_chn},
		[4] = {XM_ID_VENC, 0, jpg_enc_ad},
		[5] = {XM_ID_VENC, 0, enc_substream},
	};

	if (last_play_mode_ == XM_PLAY_BOTH) {
		stDestChn[2].s32ChnId = vo_chn_half_l;
	}

	for (int i = 0; i < 6; i++) {
		//开机全屏AD，不绑定，防止闪屏
		if (i == 2 && last_play_mode_ == XM_PLAY_AD)
			continue;
		int ret = XM_MPI_SYS_Bind(&stSrcChn[i], &stDestChn[i]);
		if (XM_SUCCESS != ret) {
			XMLogE("XM_MPI_SYS_Bind error, i=%d, ret=%x", i, ret);
		}
	}

	if (last_play_mode_ == XM_PLAY_BOTH) {
		CHECK_RET(XM_MPI_VO_ShowChn(vo_Layer, vo_chn_half_l), "XM_MPI_VO_ShowChn");
		CHECK_RET(XM_MPI_VO_ShowChn(vo_Layer, vo_chn_half_r), "XM_MPI_VO_ShowChn");
		XMLogI("XM_MPI_VO, show 1/2");
	}
	else {
		CHECK_RET(XM_MPI_VO_ShowChn(vo_Layer, vo_chn_full), "XM_MPI_VO_ShowChn");
		XMLogI("XM_MPI_VO, show 0");
	}
    return 0;                                                                                     
}

int MppMdl::ChnUnBind()
{
	XMLogI("ChnUnBind");
	VI_CHN vi_chn_enc = 0, vi_chn_sensor = 1, vi_chn_substream = 2, vi_chn_ad = 3;
	VO_DEV vo_dev = 0;
	VO_CHN vo_chn_full = 0, vo_chn_half_l = 1, vo_chn_half_r = 2;
	VO_LAYER vo_Layer = 0;
	VAR_CHN var_chn_disp = 0, var_chn_ad = 1;
	int enc_chn = 0, enc_chn_ad = 1, jpg_enc_chn = 2, jpg_enc_ad = 3, enc_substream = 4;
	MPP_CHN_S stSrcChn[8] = {
		[0] = {XM_ID_VIU, 0, vi_chn_enc},
		[1] = {XM_ID_VIU, 0, vi_chn_sensor},
		[2] = {XM_ID_VAR, 0, var_chn_disp},
		[3] = {XM_ID_VENC, 0, enc_chn},
		[4] = {XM_ID_VENC, 0, enc_chn_ad},
		[5] = {XM_ID_VIU, 0, vi_chn_substream},
	};

	MPP_CHN_S stDestChn[8] = {
		[0] = {XM_ID_VENC, 0, enc_chn},
		[1] = {XM_ID_VAR, 0, var_chn_disp},
		[2] = {XM_ID_VOU, vo_dev, vo_chn_full},
		[3] = {XM_ID_VENC, 0, jpg_enc_chn},
		[4] = {XM_ID_VENC, 0, jpg_enc_ad},
		[5] = {XM_ID_VENC, 0, enc_substream},
	};

	if (last_play_mode_ == XM_PLAY_BOTH || last_play_mode_ == XM_PLAY_AD) {
		stDestChn[2].s32ChnId = vo_chn_half_l;
	}

	for (int i = 0; i < 6; i++) {
		int ret = XM_MPI_SYS_UnBind(&stSrcChn[i], &stDestChn[i]);
		if (XM_SUCCESS != ret) {
			XMLogE("XM_MPI_SYS_UnBind error, i=%d, ret=%x", i, ret);
		}
	}

	system("cat /proc/umap/sys");

	return 0;
}

int MppMdl::GetJpgPicInternal(int channel)
{
	int iRet = 0;
	VENC_STREAM_S stVencStream;
	VENC_PACK_S stPack;
	memset(&stVencStream, 0, sizeof(VENC_STREAM_S));
	memset(&stPack, 0, sizeof(stPack));
	stVencStream.pstPack = &stPack;
	stVencStream.u32PackCount = 1;

	iRet = XM_MPI_VENC_GetStream(channel+channel_num_, &stVencStream, XM_FALSE);
	if(iRet == XM_SUCCESS){
		XM_MW_Media_Frame media_frame = {0};
		media_frame.video = true;
		media_frame.codec_type = PT_JPEG;
		media_frame.frame_buffer = stVencStream.pstPack[0].pu8Addr;
		media_frame.frame_size = stVencStream.pstPack[0].u32Len;
		int width = enc_medias_[channel][0].width;
		int height = enc_medias_[channel][0].height;
		XM_Middleware_Media_VideoStream(channel, 0, &media_frame, width, height);
		XMLogI("channel:%d width=%d, height=%d, save Jpeg over!", channel, width, height);
		XM_MPI_VENC_ReleaseStream(channel+channel_num_, &stVencStream);
	}

	return iRet;
}

int MppMdl::GetPicJPEG(int channel)
{
	XMLogI("GetPicJPEG, chn=%d", channel);
	int iRet = 0;
	int index = 0;
	
	unsigned int u32Stat = 0;
	if (AdLoss()) {
		u32Stat = u32Stat|(1<<1);
	}
	if (channel >= 0 && channel < channel_num_) {
		if (u32Stat & (1 << channel)) {
			XMLogW("The camera is not inserted into the channel:%d\n", channel);
			return XM_FAILURE;
		}
		catch_pic_[channel] = true;
	}
	else if (channel == -1) {//所有通道抓图
		for (int i = 0; i < channel_num_; i++) {
			if (u32Stat & (1 << channel)) {
				XMLogW("The camera is not inserted into the channel:%d\n", channel);
				continue;
			}
			catch_pic_[i] = true;
		}
	}
	return XM_SUCCESS;
}

#include "global_page.h"
int MppMdl::EnableOsdTime(int channel, bool enable, int x, int y)
{
	//channel 4表示app子码流预览
	XMLogW("EnableOsdTime, channel=%d, enable=%d", channel, enable);
	last_osd_set_time_ = 0;//重置osd设置时间
	if (enable_osd_[channel] == enable) {
	   if(enable == false){
	     return 0;
	   }
	}

	if (!enable) {
		CloseOSD(channel, channel);
	}
	else {
	     if (enable_osd_[channel]) {			
		   CloseOSD(channel, channel);			
		   enable_osd_[channel] = false;		
		  }
		osd_time_x_[channel] = x;
		osd_time_y_[channel] = y;
		OsdTitleCreate(channel, channel, kOSDWidth*20, kOSDHeight);
	}
	enable_osd_[channel] = enable;

	return 0;
}

int MppMdl::UpdateOSDInfo(int channel, XM_MW_OSD_UPDATE_INFO& osd_update_info)
{
	int i = channel;
	metex_user_osd_.lock();
	memcpy(user_osd_.osd[i][1].osd_buf, osd_update_info.osd_buf, XM_MAX_OSD_WIDTH*XM_MAX_OSD_HEIGHT*2);
	metex_user_osd_.unlock();
	return 0;
}

int MppMdl::SetOSDInfos(XM_MW_OSD_INFOS& osd_infos)
{
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			user_osd_.osd[i][j].enable = osd_infos.osd[i][j].enable;
			user_osd_.osd[i][j].x = osd_infos.osd[i][j].x;
			user_osd_.osd[i][j].y = osd_infos.osd[i][j].y;
			user_osd_.osd[i][j].width = osd_infos.osd[i][j].width;
			user_osd_.osd[i][j].height = osd_infos.osd[i][j].height;
			user_osd_.osd[i][j].up = osd_infos.osd[i][j].up;
			memcpy(user_osd_.osd[i][j].osd_buf, osd_infos.osd[i][j].osd_buf, XM_MAX_OSD_WIDTH*XM_MAX_OSD_HEIGHT*2);
			RGN_HANDLE handle = 7 - j;
			int channel = (i == 2) ? 4 : i;
			if (osd_infos.osd[i][j].enable) {
				int width = osd_infos.osd[i][j].width;
				int height = osd_infos.osd[i][j].height;
			#if 0	
				if (width > XM_MAX_OSD_WIDTH) {
					user_osd_.osd[i][j].width = XM_MAX_OSD_WIDTH;
					width = XM_MAX_OSD_WIDTH;
				}
				if (height > XM_MAX_OSD_HEIGHT) {
					user_osd_.osd[i][j].height = XM_MAX_OSD_HEIGHT;
					height = XM_MAX_OSD_HEIGHT;
				}
			#else
				if (width*height > XM_MAX_OSD_WIDTH*XM_MAX_OSD_HEIGHT) {
					user_osd_.osd[i][j].width = XM_MAX_OSD_WIDTH;
					width = XM_MAX_OSD_WIDTH;
					user_osd_.osd[i][j].height = XM_MAX_OSD_HEIGHT;
					height = XM_MAX_OSD_HEIGHT;
				}
			#endif
				OsdTitleCreate(handle, channel, width, height);
				SetUserOsd(handle, channel, user_osd_.osd[i][j], j==1);
			}
			else {
				CloseOSD(handle, channel);
			}
		}
	}
	return 0;
}

int MppMdl::OsdTitleCreate(RGN_HANDLE Handle, int nChannel, int width, int height)//添加创建接口，先创建，后续更改只detach,不销毁
{
	int iRet = 0;
	RGN_HANDLE Handle_t = nChannel*8+Handle;
	RGN_ATTR_S Region;
	RGN_ATTR_S *pstRegion = &Region;
	memset(pstRegion, 0, sizeof(RGN_ATTR_S));
	pstRegion->enType = RGN_OVERLAY;
	pstRegion->unAttr.stOverlay.eFormat = OVERLAY_ARGB_1555;
	pstRegion->unAttr.stOverlay.stRect.s32X = 0;
	pstRegion->unAttr.stOverlay.stRect.s32Y = 0;
	pstRegion->unAttr.stOverlay.stRect.u32Height = height;
	pstRegion->unAttr.stOverlay.stRect.u32Width = width;
 
	pstRegion->unAttr.stOverlay.u32Color[0] = 0x00000000;
	pstRegion->unAttr.stOverlay.u32Color[1] = 0xffffffff;
	pstRegion->unAttr.stOverlay.u32Alpha = 0xFFFF00FF;
	XMLogI("OsdTitleCreate, handle=%d", Handle_t);
	iRet = XM_MPI_RGN_Create(Handle_t, pstRegion);
	if(iRet != XM_SUCCESS)
	{
		XMLogE("Create osd rgn failed!====>value:%#X\n",iRet);
		return XM_FAILURE;
	}
	return XM_SUCCESS;
}

int MppMdl::OSDSetNum(uint8_t* buf, int num, int pos, int width, bool sub)
{
	int osd_width = sub ? (kOSDWidth/2) : kOSDWidth;
	uint8_t* src = osd_num_buf_[num];
	uint8_t* dst = buf + pos*osd_width*2;
	if (sub) {
		for (int i = 0; i < kOSDHeight/2; i++) {
			for (int j = 0; j < kOSDWidth/2; j++) {
				dst[2*j] = src[4*j];
				dst[2*j+1] = src[4*j+1];
			}
			src += (kOSDWidth*2*2);
			dst += (width*kOSDWidth);
		}
	}
	else {
		for (int i = 0; i < kOSDHeight; i++) {
			memcpy(dst, src, kOSDWidth*2);
			src += kOSDWidth*2;
			dst += (width*kOSDWidth*2);
		}
	}
	return 0;
}

int MppMdl::SetUserOsd(RGN_HANDLE Handle, int nChannel, XM_MW_OSD_INFO& osd_info, bool auto_up)
{
	int iRet = 0;
	XM_MW_Media_Info media_info;
	MPP_CHN_S pstChn;
	pstChn.enModId = XM_ID_VENC;
	pstChn.s32DevId = 0;
	pstChn.s32ChnId = nChannel;

	int width;
	int height;
	if(nChannel == 4) {
		width = enc_medias_[0][1].width;
		height = enc_medias_[0][1].height;
	}
	else {
		width = enc_medias_[nChannel][0].width;
		height = enc_medias_[nChannel][0].height;
	}

	RGN_HANDLE Handle_t = nChannel*8+Handle;
	RGN_ATTR_S Region;
	RGN_ATTR_S *pstRegion = &Region;
	memset(pstRegion, 0, sizeof(RGN_ATTR_S));
	pstRegion->enType = RGN_OVERLAY;
	pstRegion->unAttr.stOverlay.eFormat = OVERLAY_ARGB_1555;
	pstRegion->unAttr.stOverlay.stRect.s32X = (osd_info.x*width/8192)/16*16;
	pstRegion->unAttr.stOverlay.stRect.s32Y = (osd_info.y*height/8192)/16*16;

#if 1
    #if 1//X2V60_S_DEBUG1
	int reserved_h = height >= 1800 ? 40 : 20;
    #else
	int reserved_h = height >= 1800 ? 128 : 64;
    #endif
	if (pstRegion->unAttr.stOverlay.stRect.s32Y > height-reserved_h) 
		pstRegion->unAttr.stOverlay.stRect.s32Y = (height-reserved_h)/16*16;
#endif

	pstRegion->unAttr.stOverlay.stRect.u32Height = osd_info.height;
	pstRegion->unAttr.stOverlay.stRect.u32Width = osd_info.width;
 
	pstRegion->unAttr.stOverlay.u32Color[0] = 0x00000000;
	pstRegion->unAttr.stOverlay.u32Color[1] = 0xffffffff;

	if (osd_info.up || (auto_up && height >= 2144))
		pstRegion->unAttr.stOverlay.u32Up = 0x11;
	pstRegion->unAttr.stOverlay.u32Alpha = 0xFFFF00FF;
	// XMLogI("SetUserOsd, handle=%d, x=%d, y=%d, width=%d, height=%d", Handle_t, pstRegion->unAttr.stOverlay.stRect.s32X, 
	// 	pstRegion->unAttr.stOverlay.stRect.s32Y, osd_info.width, osd_info.height);
	iRet = XM_MPI_RGN_SetAttr(Handle_t,pstRegion);
	if(iRet != XM_SUCCESS) {
		XMLogE("Set osd rgn failed!====>value:%#X\n",iRet);
		return XM_FAILURE;
	}

	BITMAP_S pstBitmap;
	pstBitmap.u32Size = osd_info.width*osd_info.height*2;
	pstBitmap.pData = osd_info.osd_buf;
	XM_MPI_RGN_SetBitMap(Handle_t, &pstBitmap);
	XM_MPI_RGN_AttachToChn(Handle_t, &pstChn, RGN_OVERLAY);

	return XM_SUCCESS;
}

int MppMdl::SetOsdTitle(RGN_HANDLE Handle, int nChannel, const SystemTime& sys_time, const POINT_S& Point, bool sub)
{
	int osd_width = sub ? (kOSDWidth/2) : kOSDWidth;
	int osd_height = sub ? (kOSDHeight/2) : kOSDHeight;
	int iRet = 0;
	#if OSD_SYMBOL_TEST
	int num_count = 17;
	#else
	int num_count = 19;
	#endif
	XM_MW_Media_Info media_info;
	MPP_CHN_S pstChn;
	pstChn.enModId = XM_ID_VENC;
	pstChn.s32DevId = 0;
	pstChn.s32ChnId = nChannel;

	int width;
	int height;
	if (nChannel == 4) {
		width = enc_medias_[0][1].width;
		height = enc_medias_[0][1].height;
	} else {
		width = enc_medias_[nChannel][0].width;
	 	height = enc_medias_[nChannel][0].height;
	}
	RGN_HANDLE Handle_t = nChannel*8+Handle;
	RGN_ATTR_S Region;
	RGN_ATTR_S *pstRegion = &Region;
	memset(pstRegion, 0, sizeof(RGN_ATTR_S));
	pstRegion->enType = RGN_OVERLAY;
	pstRegion->unAttr.stOverlay.eFormat = OVERLAY_ARGB_1555;
	pstRegion->unAttr.stOverlay.stRect.s32X = Point.s32X;
	pstRegion->unAttr.stOverlay.stRect.s32Y = Point.s32Y;
	pstRegion->unAttr.stOverlay.stRect.u32Height = osd_height;
	pstRegion->unAttr.stOverlay.stRect.u32Width = osd_width*num_count;
 
	pstRegion->unAttr.stOverlay.u32Color[0] = 0x00000000;
	pstRegion->unAttr.stOverlay.u32Color[1] = 0xffffffff;
	if (height >= 2144)
		pstRegion->unAttr.stOverlay.u32Up = 0x11;
	pstRegion->unAttr.stOverlay.u32Alpha = 0xFFFF00FF;

	iRet = XM_MPI_RGN_SetAttr(Handle_t,pstRegion);
	if(iRet != XM_SUCCESS)
	{
		XMLogE("Set osd rgn failed!====>value:%#X\n",iRet);
		return XM_FAILURE;
	}

#if OSD_SYMBOL_TEST
	static int count_year=0,count_month=16,count_sec=42;
	if(nChannel == 4){
		count_month++;
		count_sec++;
		count_year++;
	
		if(count_year > 4096){
			
			count_year=0;
		}
	
		if(count_month > 41){
			
			count_month=16;
		}
	
		if(count_sec > 67){
			
			count_sec=42;
		}
	}
#endif

	int i;
	int pos = 0;
	char str_date[32] = {0};
	//年
#if OSD_SYMBOL_TEST
    sprintf(str_date, "%04d", count_year);
#else
	sprintf(str_date, "%04d", sys_time.year);
#endif

	for (i = 0; i < 4; i++) {
		OSDSetNum(osd_show_buf_, str_date[i]-'0', pos++, num_count, sub);
	}
	OSDSetNum(osd_show_buf_, 11, pos++, num_count, sub);//fenge
	//月
	#if OSD_SYMBOL_TEST
	OSDSetNum(osd_show_buf_, count_sec, pos++, num_count, sub);
	#else
	memset(str_date, 0, sizeof(str_date));
	sprintf(str_date, "%02d", sys_time.month);
	for (i = 0; i < 2; i++) {
		OSDSetNum(osd_show_buf_, str_date[i]-'0', pos++, num_count, sub);
	}
	#endif
	
	OSDSetNum(osd_show_buf_, 11, pos++, num_count, sub);//fenge
	//日
	memset(str_date, 0, sizeof(str_date));
	sprintf(str_date, "%02d", sys_time.day);
	for (i = 0; i < 2; i++) {
		OSDSetNum(osd_show_buf_, str_date[i]-'0', pos++, num_count, sub);
	}
	OSDSetNum(osd_show_buf_, 12, pos++, num_count, sub);//kongge
	//时
	memset(str_date, 0, sizeof(str_date));
	sprintf(str_date, "%02d", sys_time.hour);
	for (i = 0; i < 2; i++) {
		OSDSetNum(osd_show_buf_, str_date[i]-'0', pos++, num_count, sub);
	}
	OSDSetNum(osd_show_buf_, 10, pos++, num_count, sub);//maohao
	//分
	memset(str_date, 0, sizeof(str_date));
	sprintf(str_date, "%02d", sys_time.min);
	for (i = 0; i < 2; i++) {
		OSDSetNum(osd_show_buf_, str_date[i]-'0', pos++, num_count, sub);
	}
	OSDSetNum(osd_show_buf_, 10, pos++, num_count, sub);//maohao
	//秒
    #if OSD_SYMBOL_TEST
	OSDSetNum(osd_show_buf_, count_month, pos++, num_count, sub);
	#else
	memset(str_date, 0, sizeof(str_date));
	sprintf(str_date, "%02d", sys_time.sec);
	for (i = 0; i < 2; i++) {
		OSDSetNum(osd_show_buf_, str_date[i]-'0', pos++, num_count, sub);
	}
	#endif

	BITMAP_S pstBitmap;
	pstBitmap.u32Size = osd_width*osd_height*2*num_count;
	pstBitmap.pData = (void *)osd_show_buf_;
	XM_MPI_RGN_SetBitMap(Handle_t, &pstBitmap);
	XM_MPI_RGN_AttachToChn(Handle_t, &pstChn, RGN_OVERLAY);

	return XM_SUCCESS;
}

int MppMdl::CloseOSD(RGN_HANDLE Handle, int nChannel)
{
	MPP_CHN_S pstChn;
	pstChn.enModId = XM_ID_VENC;
	pstChn.s32DevId = 0;
	pstChn.s32ChnId = nChannel;

	RGN_HANDLE Handle_t = nChannel * 8 + Handle;

	RGN_ATTR_S Region;
	RGN_ATTR_S *pstRegion = &Region;
	memset(pstRegion, 0, sizeof(RGN_ATTR_S));
	pstRegion->enType = RGN_OVERLAY;
	pstRegion->unAttr.stOverlay.eFormat = OVERLAY_ARGB_CLUT1;

	int iRet = XM_MPI_RGN_DetachFromChn(Handle_t, &pstChn, RGN_OVERLAY);
	if (iRet != XM_SUCCESS)
	{
		XMLogI("Detach Handle(%d) from Chn(%d) failed!\n", Handle_t, nChannel);
		return XM_FAILURE;
	}
	iRet = XM_MPI_RGN_Destroy(Handle_t, pstRegion);
	if (iRet != XM_SUCCESS)
	{
		XMLogI("Destroy Handle(%d)failed!\n", Handle_t);
		return XM_FAILURE;
	}

	return XM_SUCCESS;
}

void* XMVideoEncodeThreadProc(void* param)
{
#ifndef WIN32
	prctl(PR_SET_NAME, "video_enc_proc");
#endif
	MppMdl* mpp_mdl = (MppMdl*)param;
	mpp_mdl->VideoEncProc();
}

void* XMAudioEncodeThreadProc(void* param)
{
#ifndef WIN32
	prctl(PR_SET_NAME, "audio_enc_proc");
#endif
	MppMdl* mpp_mdl = (MppMdl*)param;
	mpp_mdl->AudioEncProc();
}

int MppMdl::StartADProc()
{
	if (ad_proc_)
		ad_proc_->Start();
	return 0;
}

int MppMdl::StopADProc()
{
	if (ad_proc_)
		ad_proc_->Stop();
	return 0;
}

int MppMdl::SetEncInfo(const XM_MW_Encode_Info& enc_info)
{
	XMLogI("SetEncInfo, channel num=%d", enc_info.channel_num);
	bool enc_init = thread_enc_init_;
	if (enc_init) {
		ChnUnBind();
		StopEncProc();
	}

	for (int i = 0; i < enc_info.channel_num; i++) {
		for (int j = 0; j < 2; j++) {
			enc_medias_[i][j] = enc_info.enc_medias[i][j];
			if (enc_medias_[i][j].width > kMaxPicWidth)
				enc_medias_[i][j].width = kMaxPicWidth;
			if (enc_medias_[i][j].height > kMaxPicHeight)
				enc_medias_[i][j].height = kMaxPicHeight;
		}
	}

	if (enc_init) {
		ViConfig();
		StartEncProc();
		ChnBind();
	}
	return 0;
}

int MppMdl::SetEncInfo(int channel, int stream, const XM_MW_Media_Info& media_info)
{
	if (channel >= channel_num_ || stream < 0 || stream > 1) {
		XMLogE("set encode param error, channel=%d, stream=%d", channel, stream);
		return -1;
	}

	XMLogI("SetEncInfo, channel=%d, stream=%d", channel, stream);
	bool enc_init = thread_enc_init_;
	if (enc_init) {
		ChnUnBind();
		StopEncProc();
	}

	enc_medias_[channel][stream] = media_info;
	if (enc_medias_[channel][stream].width > kMaxPicWidth)
		enc_medias_[channel][stream].width = kMaxPicWidth;
	if (enc_medias_[channel][stream].height > kMaxPicHeight)
		enc_medias_[channel][stream].height = kMaxPicHeight;
	if (enc_init) {
		ViConfig();
		StartEncProc();
		ChnBind();
	}

	return 0;
}

int MppMdl::GetEncInfo(int channel, int stream, XM_MW_Media_Info* media_info)
{
	if (channel >= channel_num_ || stream < 0 || stream > 1) {
		XMLogE("set encode param error, channel=%d, stream=%d", channel, stream);
		return -1;
	}
	*media_info = enc_medias_[channel][stream];
	XMLogI("get encode param, channel=%d, stream=%d, width=%d, height=%d, video codec type=%d", 
		channel, stream, media_info->width, media_info->height, media_info->video_codec);
	return 0;
}

int GetLocalTime(SystemTime& sys_time)
{
	struct timeval tv = { 0 };
	gettimeofday(&tv, 0);
	tm nowTime;
	localtime_r(&tv.tv_sec, &nowTime);
	sys_time.year = nowTime.tm_year + 1900;
	sys_time.month = nowTime.tm_mon + 1;
	sys_time.day = nowTime.tm_mday;
	sys_time.hour = nowTime.tm_hour;
	sys_time.min = nowTime.tm_min;
	sys_time.sec = nowTime.tm_sec;
	return 0;
}

int MppMdl::StartEncProc()
{
	if (thread_enc_init_) 
		return 0;
	XMLogW("StartEncProc");	
	for (int i = 0; i < channel_num_; i++) {
		StartEnc(i);
	}
	StartEnc(4);
	thread_enc_exit_ = false;
	pthread_create(&thread_video_enc_handle_, NULL, XMVideoEncodeThreadProc, (void*)this);
	pthread_create(&thread_audio_enc_handle_, NULL, XMAudioEncodeThreadProc, (void*)this);
	thread_enc_init_ = true;

	//切换分辨率时立即刷新osd时间
	SystemTime local_time;
	GetLocalTime(local_time);
	for (int i = 0; i < channel_num_; i++) {
		if (enable_osd_[i])
			UpdateOsdTime(i, local_time);
	}
	//子码流
	if (enable_osd_[4])
		UpdateOsdTime(4, local_time);
	//UpdateUsrOSD();
}

int MppMdl::StopEncProc()
{
	XMLogW("before StopEncProc");
	thread_enc_exit_ = true;
	if (thread_enc_init_) {
		pthread_join(thread_video_enc_handle_,NULL);
		pthread_join(thread_audio_enc_handle_,NULL);
		thread_enc_init_ = false;
	}
	int height = enc_medias_[0][0].height;
	for (int i = 0; i < channel_num_; i++) {
        StopEnc(i);
	}
	StopEnc(4);
	XMLogW("after StopEncProc");
}

int MppMdl::StartEnc(int channel)
{
	int height = enc_medias_[0][0].height;
	if (height == kMaxPicHeight) {
		//4K编码时，把vb切换成4K
		VBInit(VB_4K_1080);	
	} else if (height == 1440) {
		VBInit(VB_2K_1080);
	} else if (height == 1080) {
		VBInit(VB_1080_1080);
	}

	VI_CHN vi_chn_enc = 0, vi_chn_sensor = 1, vi_chn_substream = 2, vi_chn_ad = 3;
	VI_CHN vi_channel = channel;
	if (channel == 4)
		vi_channel = vi_chn_substream;
	else if (channel == 1)
		vi_channel = vi_chn_ad;

	if(channel == 4) {
		VencConfig(channel, 0);
		CHECK_RET(XM_MPI_VI_EnableChn(vi_channel), "XM_MPI_VI_EnableChn");
		return 0;
	}

	VencConfig(channel, 0);
	JpegConfig(channel);	

	CHECK_RET(XM_MPI_VI_EnableChn(vi_channel), "XM_MPI_VI_EnableChn");
	return 0;
}

int MppMdl::StopEnc(int channel)
{
	XMLogW("StopEncProc XM_MPI_VENC_StopRecvPic, channel=%d", channel);
	CHECK_RET(XM_MPI_VENC_StopRecvPic(channel), "XM_MPI_VENC_StopRecvPic");
	XMLogW("StopEncProc XM_MPI_VENC_DestroyChn");
	CHECK_RET(XM_MPI_VENC_DestroyChn(channel), "XM_MPI_VENC_DestroyChn");
	VI_CHN vi_chn_enc = 0, vi_chn_sensor = 1, vi_chn_substream = 2, vi_chn_ad = 3;
	VI_CHN vi_channel = channel;
	if (channel == 4)
		vi_channel = vi_chn_substream;
	else if (channel == 1)
		vi_channel = vi_chn_ad;
	CHECK_RET(XM_MPI_VI_SetFrameDepth(vi_channel, 0), "XM_MPI_VI_SetFrameDepth");
	XMLogW("StopEncProc XM_MPI_VI_DisableChn, vi channel=%d", vi_channel);
	CHECK_RET(XM_MPI_VI_DisableChn(vi_channel), "XM_MPI_VI_DisableChn");
	if (channel == 4) {
		return 0;
	}
	else {
		if (channel == 0) {
			usleep(50*1000);
			XM_MPI_VI_ResetChn(0);
		}
	}

	int jpg_chn = channel_num_ + channel;
	XMLogW("StopEncProc XM_MPI_VENC_StopRecvPic");
	CHECK_RET(XM_MPI_VENC_StopRecvPic(jpg_chn), "XM_MPI_VENC_StopRecvPic");
	XMLogW("StopEncProc XM_MPI_VENC_DestroyChn");
	CHECK_RET(XM_MPI_VENC_DestroyChn(jpg_chn), "XM_MPI_VENC_DestroyChn");
	return 0;
}

int MppMdl::AudioEncProc() 
{
	AUDIO_DEV ai_master_dev = 0;
	AUDIO_DEV ai_sub_dev = 1;
	// Reqest memory for audio frame
	char audio_buffer[kSamplePerFrame * 2];
	AUDIO_FRAME_S stAudioFrame;
	memset(&stAudioFrame, 0, sizeof(AUDIO_FRAME_S));
	stAudioFrame.pVirAddr[0] = (void*)audio_buffer;

#ifdef BOARD8520DV200
	// set ai volume
	XM_U32 u32Volume = 10;
	for (int s32Chn = 0; s32Chn < 1; s32Chn++) {
		int s32Ret = XM_VIDEO_SetAiVolume(s32Chn, u32Volume);
		if (s32Ret != XM_SUCCESS) {
			XMLogE("XM_VIDEO_SetAiVolume(%d) failed.", s32Chn);
			return XM_FAILURE;
		}
	}
#endif

	XMLogI("AudioEncProc!!!!");
	while(!thread_enc_exit_) {
		int ret = GetAIFrame(ai_master_dev, 1, &stAudioFrame);
		//目前只有主设备有音频
		//GetAIFrame(ai_sub_dev, channel_num_, &stAudioFrame);
		//没取到数据，sleep
		if (ret < 0)
			usleep(1000);
	}

	return XM_SUCCESS;
}

int MppMdl::GetAIFrame(int ai_dev, int channel_num, AUDIO_FRAME_S* audio_frame)
{
	int ret = -1;
	for(int chn = 0; chn < channel_num; chn++) {
		int s32Ret = XM_MPI_AI_GetFrame(ai_dev, chn, audio_frame, NULL, XM_FALSE);
		if (XM_SUCCESS == s32Ret ) {
			ret = 0;
			if (audio_remain_len_[chn] + audio_frame->u32Len > kMaxAudioBufferLen) {
				XM_MW_Media_Frame media_frame = {0};
				media_frame.video = false;
				media_frame.codec_type = enc_medias_[chn][0].audio_codec;
				media_frame.frame_buffer = audio_buffer_[chn];
				media_frame.frame_size = audio_remain_len_[chn];
				media_frame.key_frame = false;
				media_frame.timestamp = audio_frame->u64TimeStamp/1000; //转换成毫秒
				XM_Middleware_Media_AudioStream(ai_dev, chn, &media_frame);
				audio_remain_len_[chn] = 0;
				memcpy(audio_buffer_[chn] , audio_frame->pVirAddr[0], audio_frame->u32Len);
				audio_remain_len_[chn] = audio_frame->u32Len;
			} else {
				//把多帧音频合并成一帧，降低写avi文件压力
				memcpy(audio_buffer_[chn] + audio_remain_len_[chn], audio_frame->pVirAddr[0], audio_frame->u32Len);
				audio_remain_len_[chn] += audio_frame->u32Len;
			}
		}
	}
	return ret;
}

int MppMdl::ForceIFrame(int channel)
{
	XMLogI("ForceIFrame, channel=%d", channel);
	int ret = XM_MPI_VENC_RequestIDR(channel);
	if (ret != XM_SUCCESS) {
		XMLogE("force i frame error, chan=%d", channel);
	}

	thumbnail_status_[channel] = Thumbnail_VI;
	return 0;
}

int MppMdl::VideoEncProc() 
{
	XMLogI("VideoEncProc!!!!");
	XM_S32 s32Chn;
	XM_S32 s32Ret;
	
	VENC_PACK_S stPack;
	VENC_STREAM_S stStream;
	VENC_STREAM_S *pstStream = &stStream;
	pstStream->pstPack = &stPack;

	ISP_U8 pu8StateFlg;
	for (int i = 0; i < 10; i++) {
		int ret = camera_get_stabState(&pu8StateFlg);
		if (ret == 0) {
			if ((pu8StateFlg & 0xF) == 1) {
				XMLogI("camera_get_stabState, isp ok");
				break;
			}
			else {
				XMLogI("camera_get_stabState, pu8StateFlg=%x", pu8StateFlg);
			}
		}
		else {
			XMLogW("camera_get_stabState error, ret=%d", ret);
		}
		usleep(10000);
	}

	XMLogI("after camera_get_stabState");

	int height = enc_medias_[0][0].height;
	if (height == kMaxPicHeight) {
		//4K编码时，等待80ms，丢掉2帧数据，以免出现绿屏
		usleep(80000);
	}
	for (int channel = 0; channel < channel_num_; channel++) {
		CHECK_RET(XM_MPI_VENC_StartRecvPic(channel), "XM_MPI_VENC_StartRecvPic");
	}

	// int vi_chn_substream = 2;
	// CHECK_RET(XM_MPI_VI_SetFrameDepth(vi_chn_substream, 1), "XM_MPI_VI_SetFrameDepth");
	int venc_chn_substream = 4;
	while(!thread_enc_exit_) {
		//缩略图
		if (thumbnail_status_[0] == Thumbnail_VI) {
			int ret = GetViFrame(0);
			if (XM_SUCCESS == ret) {
				XM_Middleware_Media_VideoThumbnail(0, (char*)thumbnail_buf_[0], 
					thumbnail_width_[0], thumbnail_height_[0]);    
				thumbnail_status_[0] = Thumbnail_None;					
			}						
		}

		bool recv_frame = false;
		for(s32Chn=0; s32Chn < channel_num_ + 3; s32Chn++) {
			if (s32Chn == 2 || s32Chn == 3)
				continue;

			s32Ret = XM_MPI_VENC_GetStream(s32Chn, pstStream, XM_FALSE);
			if (s32Ret != XM_SUCCESS || pstStream->u32PackCount <= 0) {
				continue;
			}
			recv_frame = true;
			int channel = s32Chn;
			int stream = 0;
			if (s32Chn == venc_chn_substream){
				channel = 0;
				stream = 1;
			}

			XM_MW_Media_Frame media_frame = {0};
			media_frame.video = true;
			media_frame.codec_type = enc_medias_[channel][stream].video_codec;
			media_frame.frame_buffer = pstStream->pstPack[0].pu8Addr;
			media_frame.frame_size = pstStream->pstPack[0].u32Len;
			if (media_frame.codec_type == PT_H264) {
				media_frame.key_frame = pstStream->pstPack[0].DataType.enH264EType == H264E_NALU_SPS;
			}
			else if (media_frame.codec_type == PT_H265) {
				media_frame.key_frame = (pstStream->pstPack[0].DataType.enH265EType == H265E_NALU_SPS) ||
										(pstStream->pstPack[0].DataType.enH265EType == H265E_NALU_VPS);
			}
			media_frame.timestamp = pstStream->pstPack[0].u64PTS/1000; //转换成毫秒
			XM_Middleware_Media_VideoStream(channel, stream, &media_frame, 0, 0);
			if (time_lapse_ != XM_TIME_LAPSE_Closed) {
				XMLogI("chn=%d, len=%d, time=%lld, key_frame=%d", 
					channel, media_frame.frame_size, media_frame.timestamp, media_frame.key_frame);
			}
			// if (media_frame.key_frame) {
			// 	XMLogI("key frame, chn=%d, len=%d, time=%lld", channel, media_frame.frame_size, media_frame.timestamp);
			// }

			s32Ret = XM_MPI_VENC_ReleaseStream(s32Chn, pstStream);
			if (s32Ret != XM_SUCCESS) {
				XMLogI("XM_MPI_VENC_ReleaseStream failed:%#x", s32Ret);
				return XM_FAILURE;
			}
		}
		if (!recv_frame)
			usleep(10000);
	}	
	return XM_SUCCESS;
}

void* WINAPI MPPSoftVoThreadProc(void* param)
{
#ifndef WIN32
	prctl(PR_SET_NAME, "vo_proc");
#endif
	MppMdl* manager = (MppMdl*)param;
	manager->SoftVoProc();
	return 0;
}

int MppMdl::StartSoftVoProc()
{
	thread_vi_exit_ = false;
	if (!vi_thread_.created_)
		CreateThreadEx(vi_thread_, (LPTHREAD_START_ROUTINE)MPPSoftVoThreadProc, this);
	return 0;
}

int MppMdl::StopSoftVOProc()
{
	thread_vi_exit_ = true;
	WaitForSingleObjectEx(vi_thread_, INFINITE);
	CloseThreadEx(vi_thread_);
	return 0;
}

#define MID(a, b, c)    MIN(MAX((a), MIN((b), (c))), MAX((b), (c)))
//2寸屏OK
int Anti_Aliasing_downscale2(int s_w, int s_h, int d_w, int d_h, unsigned char *s_y_addr, unsigned char *d_y_addr, int h_pos, int mirror)
{
	// int s_w, s_h;
	// int d_w, d_h;
	unsigned char *s_uv_addr = NULL,  *d_uv_addr = NULL;
	unsigned int s_phy_addr = 0, d_phy_addr = 0;
	unsigned int x, y, xrate = 0, yrate = 0;
    int xerror = 0, yerror = 0;
	int phi_x = 0, phi_y = 0;
	unsigned int i = 0, j = 0, n = 0, m = 0;
	uint8_t* ptr_y;
	uint8_t* ptr_uv;
	unsigned int y_sw =  0;
	// s_w = 1920;
	// s_h = 1080;
	// d_w = 640;
	// d_h = 320;
	int pos = 0;
	int d_h_realy = d_h;
	phi_y = 0;
	xrate = ((s_w << 16) / d_w);
	yrate = ((s_h << 16) / d_h_realy);
	xerror = -(1 << (16 - 1)) + (xrate >> 1);
	for(i = 0; i < d_h_realy; i++)
	{
		phi_x = xerror;
		phi_y = i * yrate;
		y = MID(phi_y >> 16, 0, s_h - 2);
		
		if (mirror) {
			ptr_y = d_y_addr + i;
		}
		else
			ptr_y = d_y_addr + i + (d_w - 1) * d_h;
			
		//y_sw =  y * s_w;
		for(j = 0; j < d_w; j++)
		{
			x = MID(phi_x >> 16, 0, s_w - 2);
			//*ptr_y = *(s_y_addr + (y_sw + x));

			int yy = 0;
			//(c*4 + 2*d*2 + 2*b*2 + 4*a*1)/16;
			uint8_t* c = s_y_addr + (y * s_w + x);
			int i_c = *c;
			yy = i_c << 2;

			//d1、b1、a3
			uint8_t* d1 = c + s_w;
			uint8_t* a3 = d1 + 1;
			uint8_t* b1 = c + 1;
			yy += ((*d1) << 1);
			yy += (*a3);
			yy += ((*b1) << 1);

			//a0、d0、a1
			if (y > 0) {
				uint8_t* d0 = c - s_w;
				yy += ((*d0) << 1);
				uint8_t* a1 = d0 + 1;
				yy += (*a1);
				if (x > 0) {
					uint8_t* a0 = d0 - 1;
					yy += (*a0);
				}
				else {
					yy += (*d0);
				}
			}
			else {
				yy += (i_c << 2); //
			}

			//b0、a2
			if (x > 0) {
				uint8_t* b0 = c - 1;
				yy += ((*b0) * 2);
				uint8_t* a2 = d1 - 1;
				yy += (*a2);
			}
			else {
				yy += (i_c*3);
			}

			int value = yy >> 4;
			*ptr_y = (value > 255) ? 255: value;

			if (mirror) {
				ptr_y += d_h;
			}
			else {
				ptr_y -= d_h;
			}
			phi_x = phi_x + xrate;
		}
	}

	d_uv_addr = d_y_addr + d_w * d_h;
	s_uv_addr = s_y_addr + s_w * s_h;
	unsigned int d_w_d_h_realy =  0;
	unsigned int d_w_2 =  d_w / 2;
	unsigned int d_h_realy_2 = d_h_realy / 2;
	if (mirror){
		d_w_d_h_realy =  (d_w_2 - 1) * d_h;
	}

	for(i = 0; i < d_h_realy_2; i++)
	{
		if (mirror)
			ptr_uv = d_uv_addr + 2 * i;
		else
			ptr_uv = d_uv_addr + 2 * i + (d_w / 2 - 1) * d_h;
			
		y = MID(i * yrate >> 16, 0, (s_h >> 1) - 1);
		y_sw =  y * s_w;
		for(j = 0; j < d_w_2; j++)
		{
			x = MID((j * xrate) >> 15, 0, s_w - 2);
			x = (x >> 1) << 1;
			ptr_uv[0] = *(s_uv_addr + y_sw + x);
			ptr_uv[1] = *(s_uv_addr + y_sw + x + 1);

			if (mirror) {
				ptr_uv += d_h;
			}
			else {
				ptr_uv -= d_h;
			}
		}
	}
	
	return 0;
}

//2寸屏翻转180°
int Anti_Aliasing_downscale(int s_w, int s_h, int d_w, int d_h, unsigned char *s_y_addr, unsigned char *d_y_addr, int h_pos, int mirror)
{
	// int s_w, s_h;
	// int d_w, d_h;
	unsigned char *s_uv_addr = NULL,  *d_uv_addr = NULL;
	unsigned int s_phy_addr = 0, d_phy_addr = 0;
	unsigned int x, y, xrate = 0, yrate = 0;
    int xerror = 0, yerror = 0;
	int phi_x = 0, phi_y = 0;
	unsigned int i = 0, j = 0, n = 0, m = 0;
	uint8_t* ptr_y;
	uint8_t* ptr_uv;
	unsigned int y_sw =  0;
	// s_w = 1920;
	// s_h = 1080;
	// d_w = 640;
	// d_h = 320;
	int pos = 0;
	int d_h_realy = d_h;
	phi_y = 0;
	xrate = ((s_w << 16) / d_w);
	yrate = ((s_h << 16) / d_h_realy);
	xerror = -(1 << (16 - 1)) + (xrate >> 1);
	for(i = 0; i < d_h_realy; i++)
	{
		phi_x = xerror;
		phi_y = i * yrate;
		y = MID(phi_y >> 16, 0, s_h - 2);

		if (mirror) {
			ptr_y = d_y_addr + d_h_realy-1-i + (d_w-1) * d_h;
		}else{
			ptr_y = d_y_addr + d_h_realy-1-i;
		}
		//y_sw =  y * s_w;
		for(j = 0; j < d_w; j++)
		{
			x = MID(phi_x >> 16, 0, s_w - 2);
			//*ptr_y = *(s_y_addr + (y_sw + x));

			int yy = 0;
			//(c*4 + 2*d*2 + 2*b*2 + 4*a*1)/16;
			uint8_t* c = s_y_addr + (y * s_w + x);
			int i_c = *c;
			yy = i_c << 2;

			//d1、b1、a3
			uint8_t* d1 = c + s_w;
			uint8_t* a3 = d1 + 1;
			uint8_t* b1 = c + 1;
			yy += ((*d1) << 1);
			yy += (*a3);
			yy += ((*b1) << 1);

			//a0、d0、a1
			if (y > 0) {
				uint8_t* d0 = c - s_w;
				yy += ((*d0) << 1);
				uint8_t* a1 = d0 + 1;
				yy += (*a1);
				if (x > 0) {
					uint8_t* a0 = d0 - 1;
					yy += (*a0);
				}
				else {
					yy += (*d0);
				}
			}
			else {
				yy += (i_c << 2); //
			}

			//b0、a2
			if (x > 0) {
				uint8_t* b0 = c - 1;
				yy += ((*b0) * 2);
				uint8_t* a2 = d1 - 1;
				yy += (*a2);
			}
			else {
				yy += (i_c*3);
			}

			int value = yy >> 4;
			*ptr_y = (value > 255) ? 255: value;

			if (mirror) {
				ptr_y -= d_h;
			}
			else {
				ptr_y += d_h;
			}
			phi_x = phi_x + xrate;
		}
	}

	d_uv_addr = d_y_addr + d_w * d_h;
	s_uv_addr = s_y_addr + s_w * s_h;
	unsigned int d_w_d_h_realy =  0;
	unsigned int d_w_2 =  d_w / 2;
	unsigned int d_h_realy_2 = d_h_realy / 2;
	if (mirror){
		d_w_d_h_realy =  (d_w_2 - 1) * d_h;
	}

	for(i = 0; i < d_h_realy_2; i++)
	{
		if (mirror)
			ptr_uv = d_uv_addr + 2 * (d_h_realy/2-1-i) + d_w_d_h_realy;
		else
			ptr_uv = d_uv_addr + 2 * (d_h_realy/2-1-i);

		y = MID(i * yrate >> 16, 0, (s_h >> 1) - 1);
		y_sw =  y * s_w;
		for(j = 0; j < d_w_2; j++)
		{
			x = MID((j * xrate) >> 15, 0, s_w - 2);
			x = (x >> 1) << 1;
			ptr_uv[0] = *(s_uv_addr + y_sw + x);
			ptr_uv[1] = *(s_uv_addr + y_sw + x + 1);

			if (mirror) {
				ptr_uv -= d_h;
			}
			else {
				ptr_uv += d_h;
			}
		}
	}
	
	return 0;
}

#if !DISP_ROTATE_180 //不旋转
int video_downscale(VIDEO_FRAME_INFO_S *src_frame, VIDEO_FRAME_INFO_S *dst_frame, bool mirror)
{
	int s_w, s_h;
	int d_w, d_h;
	unsigned char *s_y_addr = NULL, *s_uv_addr = NULL, *d_y_addr = NULL, *d_uv_addr = NULL;
	unsigned int s_phy_addr = 0, d_phy_addr = 0;
	unsigned int x, y, xrate = 0, yrate = 0;
    int xerror = 0, yerror = 0; 
	int phi_x = 0, phi_y = 0;
	unsigned char *addr0;
	unsigned int i = 0, j = 0;
	s_w = src_frame->stVFrame.u32Width;
	s_h = src_frame->stVFrame.u32Height;
	d_h = dst_frame->stVFrame.u32Stride[0];
	d_w = dst_frame->stVFrame.u32Height;
	//为提升效率，不做判断，debug时可打开
	if (d_h == 0 || d_w == 0 || s_w == 0 || s_h == 0) {
		XMLogE("video_downscale, %d-%d-%d-%d", s_w, s_h, d_w, d_h);
		return -1;
	}
	s_phy_addr = src_frame->stVFrame.u32PhyAddr[0];
	s_y_addr = (unsigned char *)XM_MPI_SYS_Mmap_Cached(s_phy_addr, s_w * s_h * 3 / 2);
	s_uv_addr = s_y_addr + s_w * s_h;

	int d_h_realy = d_h == 360 ? 368 : d_h;
	d_phy_addr = dst_frame->stVFrame.u32PhyAddr[0];
	d_y_addr = (unsigned char *)XM_MPI_SYS_Mmap_Cached(d_phy_addr, d_w * d_h * 3 / 2);
	d_uv_addr = d_y_addr + d_w * d_h;
	XM_MPI_SYS_MmzFlushCache(s_phy_addr, s_y_addr, s_w * s_h * 3 / 2);

	phi_y = 0;
	xrate = ((s_w << 16) / d_w);
	yrate = ((s_h << 16) / d_h_realy);
	xerror = -(1 << (16 - 1)) + (xrate >> 1);
	//yerror = -(1 << (16 - 1)) + (yrate >> 1);
	for(i = 0; i < d_h_realy; i++)
	{
		phi_x = xerror; 
		phi_y = i * yrate;
		y = MID(phi_y >> 16, 0, s_h - 2);
		uint8_t* ptr_y;
		if (mirror) {
			ptr_y = d_y_addr + d_h_realy-1-i + (d_w-1) * d_h;
		}
		else  
			ptr_y = d_y_addr + d_h_realy-1-i;
		for(j = 0; j < d_w; j++)
		{
			x = MID(phi_x >> 16, 0, s_w - 2);
			addr0 = s_y_addr + (y * s_w + x);
			int value = *addr0;
			if (mirror) {
				*ptr_y = value;
				ptr_y -= d_h;
			}
			else {
				*ptr_y = value;
				ptr_y += d_h;
			}
			phi_x = phi_x + xrate;
		}
	}

	for(i = 0; i < d_h_realy / 2; i++)
	{
		uint8_t* ptr_uv;
		if (mirror) 
			ptr_uv = d_uv_addr + 2 * (d_h_realy/2-1-i) + (d_w/2-1) * d_h;
		else
			ptr_uv = d_uv_addr + 2 * (d_h_realy/2-1-i);
		y = MID(i * yrate >> 16, 0, (s_h >> 1) - 1);
		for(j = 0; j < d_w / 2; j++)
		{
			x = MID((j * xrate) >> 15, 0, s_w - 2);
			x = (x >> 1) << 1;
			if (mirror) {
				ptr_uv[0] = *(s_uv_addr + y * s_w + x);
				ptr_uv[1] = *(s_uv_addr + y * s_w + x + 1);
				ptr_uv -= d_h;
			}
			else {
				ptr_uv[0] = *(s_uv_addr + y * s_w + x);
				ptr_uv[1] = *(s_uv_addr + y * s_w + x + 1);
				ptr_uv += d_h;
			}
		}
	}

	Anti_Aliasing_downscale(s_w, s_h, d_w, d_h, s_y_addr, d_y_addr, 0, mirror);
	XM_MPI_SYS_MmzFlushCache(d_phy_addr, d_y_addr, d_w * d_h * 3 / 2);
	XM_MPI_SYS_Munmap(s_y_addr, s_w * s_h * 3 / 2);
	XM_MPI_SYS_Munmap(d_y_addr, d_w * d_h * 3 / 2);
	return 0;
}
#else
int video_downscale(VIDEO_FRAME_INFO_S *src_frame, VIDEO_FRAME_INFO_S *dst_frame, bool mirror)
{
	int s_w, s_h;
	int d_w, d_h;
	unsigned char *s_y_addr = NULL, *s_uv_addr = NULL, *d_y_addr = NULL, *d_uv_addr = NULL;
	unsigned int s_phy_addr = 0, d_phy_addr = 0;
	unsigned int x, y, xrate = 0, yrate = 0;
    int xerror = 0, yerror = 0; 
	int phi_x = 0, phi_y = 0;
	unsigned char *addr0;
	unsigned int i = 0, j = 0;
	s_w = src_frame->stVFrame.u32Width;
	s_h = src_frame->stVFrame.u32Height;
	d_h = dst_frame->stVFrame.u32Stride[0];
	d_w = dst_frame->stVFrame.u32Height;
	//为提升效率，不做判断，debug时可打开
	if (d_h == 0 || d_w == 0 || s_w == 0 || s_h == 0) {
		XMLogE("video_downscale, %d-%d-%d-%d", s_w, s_h, d_w, d_h);
		return -1;
	}
	s_phy_addr = src_frame->stVFrame.u32PhyAddr[0];
	s_y_addr = (unsigned char *)XM_MPI_SYS_Mmap_Cached(s_phy_addr, s_w * s_h * 3 / 2);
	s_uv_addr = s_y_addr + s_w * s_h;

	int d_h_realy = d_h == 360 ? 368 : d_h;
	d_phy_addr = dst_frame->stVFrame.u32PhyAddr[0];
	d_y_addr = (unsigned char *)XM_MPI_SYS_Mmap_Cached(d_phy_addr, d_w * d_h * 3 / 2);
	d_uv_addr = d_y_addr + d_w * d_h;
	XM_MPI_SYS_MmzFlushCache(s_phy_addr, s_y_addr, s_w * s_h * 3 / 2);

	phi_y = 0;
	xrate = ((s_w << 16) / d_w);
	yrate = ((s_h << 16) / d_h_realy);
	xerror = -(1 << (16 - 1)) + (xrate >> 1);
	//yerror = -(1 << (16 - 1)) + (yrate >> 1);
	for(i = 0; i < d_h_realy; i++)
	{
		phi_x = xerror; 
		phi_y = i * yrate;
		y = MID(phi_y >> 16, 0, s_h - 2);
		uint8_t* ptr_y;
		if (mirror) {
			ptr_y = d_y_addr + d_h_realy - 1 - i;
		}
		else
			ptr_y = d_y_addr + i + (d_w - 1) * d_h;//d_h_realy - 1 - i;
		for(j = 0; j < d_w; j++)
		{
			x = MID(phi_x >> 16, 0, s_w - 2);

			addr0 = s_y_addr + (y * s_w + x);
           int value = *addr0;   
			if (mirror) {
				*ptr_y = value;
				ptr_y += d_h;
			}
			else {
				*ptr_y = value;
				ptr_y -= d_h;
			}
			phi_x = phi_x + xrate;
		}
	}

	for(i = 0; i < d_h_realy / 2; i++)
	{
		uint8_t* ptr_uv;
		if (mirror)
			ptr_uv = d_uv_addr + 2 * (d_h_realy / 2 - 1 - i);
		else
			ptr_uv = d_uv_addr + 2 * i + (d_w / 2 - 1) * d_h;
		y = MID(i * yrate >> 16, 0, (s_h >> 1) - 1);
		for(j = 0; j < d_w / 2; j++)
		{
			x = MID((j * xrate) >> 15, 0, s_w - 2);
			x = (x >> 1) << 1;
			if (mirror) {
				ptr_uv[0] = *(s_uv_addr + y * s_w + x);
				ptr_uv[1] = *(s_uv_addr + y * s_w + x + 1);
				ptr_uv += d_h;
			}
			else {
				ptr_uv[0] = *(s_uv_addr + y * s_w + x);
				ptr_uv[1] = *(s_uv_addr + y * s_w + x + 1);
				ptr_uv -= d_h;
			}
		}
	}

	XM_MPI_SYS_MmzFlushCache(d_phy_addr, d_y_addr, d_w * d_h * 3 / 2);
	XM_MPI_SYS_Munmap(s_y_addr, s_w * s_h * 3 / 2);
	XM_MPI_SYS_Munmap(d_y_addr, d_w * d_h * 3 / 2);
	return 0;
}
#endif
int MppMdl::SoftVoProc()
{
	XMLogI("SoftVoProc thread");
	VO_CHN vo_chn_full = 0, vo_chn_half_l = 1, vo_chn_half_r = 2;
	VI_CHN vi_chn_ad = 3;
	int enc_chn_ad = 1;
	VIDEO_FRAME_INFO_S frame;
	VIDEO_FRAME_INFO_S vo_frame;
	while (!thread_vi_exit_) {
		int get_frame_ret = 1;
		if (play_status_ == PLAY_PREVIEW) {
			if (AdLoss()) {
				//后拉视频丢失
				if (last_play_mode_ == XM_PLAY_BOTH || last_play_mode_ == XM_PLAY_AD) {
					//前录全屏显示
					SetPlayMode(XM_PLAY_SENSOR);
					last_play_mode_ = XM_PLAY_SENSOR;
					CHECK_RET(XM_MPI_VO_ClearChnBuffer(VoLayer_, vo_chn_half_r, XM_TRUE), "XM_MPI_VO_ClearChnBuffer");
					XM_Middleware_Periphery_Notify(XM_EVENT_AD_EXIT, "", 0);	
				}
				else if (!last_ad_loss_) {
					XM_Middleware_Periphery_Notify(XM_EVENT_AD_EXIT, "", 0);	
				}
				last_ad_loss_ = true;
				ad_loss_time_ = 0;
				usleep(10000);
				continue;
			} else {
				int64_t now = GetTickTime();
				//延迟800毫秒再回调ad丢失，规避屏显花问题
				if (ad_loss_time_ > 0 && now > ad_loss_time_+800) {
					ad_loss_time_ = 0;
					XMLogI("ad lost call back, time=%lld", now);
					XM_Middleware_Periphery_Notify(XM_EVENT_AD_CONNECT, "", 0);
				}
				if (!(last_play_mode_ == XM_PLAY_BOTH || last_play_mode_ == XM_PLAY_AD) && last_ad_loss_) {
					ad_loss_time_ = now;
					XMLogI("ad lost, time=%lld", now);
				}
				last_ad_loss_ = false;
				if (ad_loss_time_ > 0) {
					usleep(10000);
					continue;
				}
			}

			get_frame_ret = XM_MPI_VI_GetFrame(vi_chn_ad, &frame, 200);
			if (get_frame_ret == 0) {
				//抓后拉缩略图
				int ad_channel = 1;
				if (thumbnail_status_[ad_channel] == Thumbnail_VI) {
					ZoomResolution(ad_channel, &frame);
					XMLogI("channel=%d, XM_Middleware_Media_VideoThumbnail, width=%d, height=%d", ad_channel,
						thumbnail_width_[ad_channel], thumbnail_height_[ad_channel]);
					XM_Middleware_Media_VideoThumbnail(ad_channel, (char*)thumbnail_buf_[ad_channel], 
						thumbnail_width_[ad_channel], thumbnail_height_[ad_channel]);
					thumbnail_status_[ad_channel] = Thumbnail_None;
				}

				int ret;
				static const int kNormalADCount = 10; //正常ad帧
				if (ad_frame_count_++ == 0) {
					XMLogI("first ad frame");
				}
				//过滤ad画面异常
				// if (ad_frame_count_ < kNormalADCount) {
				// 	XM_MPI_VI_ReleaseFrame(enc_chn_ad, &frame);
				// 	usleep(10000);
				// 	continue;
				// } else if (ad_frame_count_ == kNormalADCount) {
				// 	XMLogI("begin render ad");
				// }
				mutex_render_ad_.lock();
				if (last_play_mode_ == XM_PLAY_BOTH || last_play_mode_ == XM_PLAY_AD) {
					int ad_chn = vo_chn_half_r;
					if (XM_PLAY_AD == last_play_mode_) {
						ad_chn = vo_chn_full;
					}

					ret = XM_MPI_VO_GetEmptyFrame(VoLayer_, ad_chn, &vo_frame, 1000);
					if (ret != XM_SUCCESS) {
						XMLogE("XM_MPI_VO_GetEmptyFrame error, ret=%x", ret);
					}
					else {
						video_downscale(&frame, &vo_frame, 
							(mirror_[1] == XM_MIRROR_VO || mirror_[1] == XM_MIRROR_VENC));
						XM_MPI_VO_SendFrame(VoLayer_, ad_chn, &vo_frame, 1000);
					}
				}
				mutex_render_ad_.unlock();
				
				bool need_enc = true;
				if (time_lapse_ == XM_TIME_LAPSE_Frame_1)
					need_enc = (ad_frame_count_%kFrameRate) == 0;
				else if (time_lapse_ == XM_TIME_LAPSE_Frame_2)
					need_enc = (ad_frame_count_%15) == 0;
				else if (time_lapse_ == XM_TIME_LAPSE_Frame_5) 
					need_enc = (ad_frame_count_%6) == 0;
				int height = enc_medias_[0][0].height;
				// if (height == kMaxPicHeight)
				// 	need_enc = false;
				if (need_enc) {
					ret = XM_MPI_VENC_SendFrame(enc_chn_ad, &frame, 1000);
					if (ret != XM_SUCCESS) {
						XMLogE("XM_MPI_VENC_SendFrame error, ret=%x", ret);
					}
				}
				else {
					ret = XM_MPI_VI_ReleaseFrame(enc_chn_ad, &frame);
					if (ret != XM_SUCCESS) {
						XMLogE("XM_MPI_VI_ReleaseFrame error, ret=%x", ret);
					}
				}
			} else {
				CHECK_RET(XM_MPI_VI_SetFrameDepth(vi_chn_ad, 1), "XM_MPI_VI_SetFrameDepth");
			}
		} else if (play_status_ == PLAY_PLAYBACK_VOD || play_status_ == PLAY_PLAYBACK_PICTURE) {
			int dev_id = play_status_ == PLAY_PLAYBACK_VOD ? kVodDecDev : kPicDecDev;
			mutex_vdec_.lock();
			if (vdec_) {
				get_frame_ret = XM_MPI_VDEC_GetChnFrame(dev_id, 0, &frame, 200);
				if (get_frame_ret == 0) {
					if (!vod_enable_chn_) {
						vod_enable_chn_ = true;
						XMLogI("XM_MPI_VO, show 0, hide 1/2");
						CHECK_RET(XM_MPI_VO_ShowChn(VoLayer_, vo_chn_full), "XM_MPI_VO_ShowChn");
						CHECK_RET(XM_MPI_VO_HideChn(VoLayer_, vo_chn_half_l), "XM_MPI_VO_HideChn");
						CHECK_RET(XM_MPI_VO_HideChn(VoLayer_, vo_chn_half_r), "XM_MPI_VO_HideChn");
					}
					int ret = XM_MPI_VO_GetEmptyFrame(VoLayer_, vo_chn_full, &vo_frame, 1000);
					if (ret != XM_SUCCESS) {
						XMLogE("XM_MPI_VO_GetEmptyFrame error, ret=%x", ret);
					}
					else {
						int64_t t1 = GetTickTime();
						video_downscale(&frame, &vo_frame, false);
						int64_t t2 = GetTickTime();
						//XMLogI("video_downscale, time spend %lldms", t2-t1);
						XM_MPI_VO_SendFrame(VoLayer_, vo_chn_full, &vo_frame, 1000);
					}
					XM_MPI_VDEC_ReleaseChnFrame(dev_id, 0, &frame);
				}
			}
			mutex_vdec_.unlock();
		}

		if (get_frame_ret != 0) {
			usleep(10000);
		}
	}

	return 0;
}

int MppMdl::StartGeneralProc()
{
	thread_genral_exit_ = false;
	if (!general_thread_.created_)
		CreateThreadEx(general_thread_, (LPTHREAD_START_ROUTINE)MPPGeneralThreadProc, this);
	return 0;
}

int MppMdl::StopGeneralProc()
{
	thread_genral_exit_ = true;
	WaitForSingleObjectEx(general_thread_, INFINITE);
	CloseThreadEx(general_thread_);
	return 0;
}

int MppMdl::UpdateOsdTime(int channel, const SystemTime& local_time)
{
	int width;
	int height;
	if(channel == 4) {
		width = enc_medias_[0][1].width;
		height = enc_medias_[0][1].height;
	}
	else {
		width = enc_medias_[channel][0].width;
		height = enc_medias_[channel][0].height;
	}
	POINT_S point;
	GetOSDPoint(channel, point, width, height);
	bool sub = channel == 4;
	SetOsdTitle(channel, channel, local_time, point, sub);
	return 0;
}

int MppMdl::UpdateUsrOSD()
{
	metex_user_osd_.lock();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			RGN_HANDLE handle = 7 - j;
			int channel = (i == 2) ? 4 : i;
			if (user_osd_.osd[i][j].enable) {
				SetUserOsd(handle, channel, user_osd_.osd[i][j], j==1);
			}
		}
	}
	metex_user_osd_.unlock();
	return 0;
}

int MppMdl::GeneralProc()
{
	while (!thread_genral_exit_) {
		int64_t now = GetTickTime();
		//jpg抓图
		int i;
		for (i = 0; i < channel_num_; i++) {
			if (catch_pic_[i]) {
				if (!start_recv_pic_[i]) {
					XMLogI("channel=%d, begin catch jpg pic", i);
					XM_MPI_VENC_StartRecvPic(i + channel_num_);
					start_recv_pic_[i] = true;
					last_catch_pic_time_[i] = now;
				}

				int ret = GetJpgPicInternal(i);
				bool time_out = (now - last_catch_pic_time_[i] > 1000) ? true : false;
				if (ret == XM_SUCCESS || time_out) {
					XM_MPI_VENC_StopRecvPic(i + channel_num_);
					start_recv_pic_[i] = false;
					last_catch_pic_time_[i] = 0;
					catch_pic_[i] = false;
					if (time_out) {
						XMLogE("channel=%d, jpg pic timeout, ret=%#X", i, ret);
					}
				}
			}
		}

		//每隔0.1秒刷新下osd时间
		if (now > 100 + last_osd_set_time_) {
			SystemTime local_time;
			GetLocalTime(local_time);
			if (last_osd_time_.compare(local_time) != 0) {
				for (int i = 0; i < channel_num_; i++) {
					if (enable_osd_[i]) {
						UpdateOsdTime(i, local_time);		
					}
				}
				if (enable_osd_[4])
					UpdateOsdTime(4, local_time);
				last_osd_time_ = local_time;	
				last_osd_set_time_ = now;
			}
			UpdateUsrOSD();
		}
		if (last_osd_set_time_ > now + 2000) {
			//检测用户是否更改了系统时间，导致出现较大偏差，此时重置osd刷新时间
			last_osd_set_time_ = 0;
		}

		usleep(10000);
	}

	return 0;
}

int MppMdl::StartAudioProc()
{
	thread_audio_exit_ = false;
	if (!audio_thread_.created_)
		CreateThreadEx(audio_thread_, (LPTHREAD_START_ROUTINE)MPPAudioThreadProc, this);
	return 0;
}

int MppMdl::StopAudioProc()
{
	thread_audio_exit_ = true;
	WaitForSingleObjectEx(audio_thread_, INFINITE);
	CloseThreadEx(audio_thread_);
	return 0;
}

int MppMdl::AudioProc()
{
	while (!thread_audio_exit_) {
		int64_t now = GetTickTime();
		if (stop_sound_) {
			XMLogW("close device start sound");
			if (sound_fp_) {
				fclose(sound_fp_);
				sound_fp_ = NULL;
				//延迟100ms，让开机音效多播放一会
				usleep(100000);
			}
			sound_file_ = "";
			stop_sound_ = false;
			SpeakerEnable(false);
			//停止声音播放时，恢复采集音量
			SetAIVolume(99);
			last_send_audio_time_ = 0;
		}

		//播放声音文件
		if (sound_fp_) {
			AUDIO_DEV AoDev = 0;
			AO_CHN AoChn = 0;
			AUDIO_FRAME_S audio_frame;
			memset(&audio_frame, 0, sizeof(audio_frame));
			audio_frame.enBitwidth = AUDIO_BIT_WIDTH_16;
			audio_frame.enSoundmode = AUDIO_SOUND_MODE_MONO;
			audio_frame.u64TimeStamp = 1;
			audio_frame.u32Seq = 1;
			audio_frame.pVirAddr[0] = audio_play_buffer_;
			int len = fread(audio_play_buffer_, 1, kMaxAudioBufferLen, sound_fp_);
			if (len <= 0) {
				fclose(sound_fp_);
				sound_fp_ = NULL;
				XMLogW("sound file play over");
				//声音文件播放完后，拉低音频
				usleep(100000);
				for (int ii = 0; ii < 10; ii++) {
					AO_CHN_STATE_S ao_status = {0};
					XM_MPI_AO_QueryChnStat(AoDev, AoChn, &ao_status);
					if (ao_status.u32ChnBusyNum == 0) {
						break;
					}
					usleep(20000);
				}
				SpeakerEnable(false);
				AUDIO_DEV AiDev = 0;
				AI_CHN AiChn = 0;
				//停止声音播放时，恢复采集音量
				SetAIVolume(99);
				XMLogW("after SetAIVolume");
				last_send_audio_time_ = 0;
				if (!startup_sound_over_) {
					startup_sound_over_ = true;
					//XM_Middleware_Periphery_Notify(XM_EVENT_STARTUP_SOUND_OVER, "", 0);
				}
				continue;
			}
			//播放声音文件时，打开声音输出
			if (!speak_enable_) {
				SpeakerEnable(true);
			}

			audio_frame.u32Len = len;
			//XMLogW("XM_MPI_AO_SendFrame, len=%d", len);
			int ret = XM_MPI_AO_SendFrame(AoDev, AoChn, &audio_frame, XM_TRUE);
			if (XM_SUCCESS != ret ) {
				XMLogE("XM_MPI_AO_SendFrame[%d, %d] failed:%#x", AoDev, AoChn, ret);
			}
			last_send_audio_time_ = GetTickTime();
			continue;
		}
		else if (sound_file_.length() > 0) {
			sound_fp_ = fopen(sound_file_.c_str(), "rb");
			if (!sound_fp_) {
				XMLogE("can't open sound file, path=%s", sound_file_.c_str());
			} 
			sound_file_ = "";
		}

		//一段时间内(暂定1秒)，没收到新的音频数据，拉低音频输出
		if (last_send_audio_time_ != 0 && now > last_send_audio_time_+1000) {
			SpeakerEnable(false);
			AUDIO_DEV AiDev = 0;
			AI_CHN AiChn = 0;
			//停止声音播放时，恢复采集音量
			SetAIVolume(99);
			last_send_audio_time_ = 0;
		}

		usleep(10000);
	}
	return 0;
}


int MppMdl::GetOSDPoint(int channel, POINT_S& point, int width, int height)
{
	point.s32X = (osd_time_x_[channel]*width/8192)/16*16;
	point.s32Y = (osd_time_y_[channel]*height/8192)/16*16;
	return 0;
}
    
int MppMdl::SetMirror(int chn, XM_MIRROR_TYPE mirror_type)
{
	//暂不支持前摄镜像
	if (chn == 0) {
		return -1;
	}
	mirror_[chn] = mirror_type;
	if (chn == 1 && thread_enc_init_) {
		ViModify(chn, (mirror_[1]==XM_MIRROR_VI || mirror_[1]==XM_MIRROR_VENC) ? true : false);
	}
	return 0;
}

int MppMdl::ResetVO()
{
	XMLogI("ResetVO");
	VO_CHN vo_chn_full = 0, vo_chn_half_l = 1, vo_chn_half_r = 2;
	VAR_CHN var_chn_disp = 0;
	VI_CHN vi_chn_sensor = 1;
	VO_DEV vo_dev = 0;

	CHECK_RET(XM_MPI_VAR_DisableChn(var_chn_disp), "XM_MPI_VAR_EnableChn");
	CHECK_RET(XM_MPI_VI_DisableChn(vi_chn_sensor), "XM_MPI_VI_DisableChn");

	CHECK_RET(XM_MPI_VO_HideChn(VoLayer_, vo_chn_full), "XM_MPI_VO_HideChn");
	CHECK_RET(XM_MPI_VO_HideChn(VoLayer_, vo_chn_half_l), "XM_MPI_VO_HideChn");
	CHECK_RET(XM_MPI_VO_HideChn(VoLayer_, vo_chn_half_r), "XM_MPI_VO_HideChn");
	CHECK_RET(XM_MPI_VO_DisableChn(VoLayer_, vo_chn_full), "XM_MPI_VO_DisableChn");
	CHECK_RET(XM_MPI_VO_DisableChn(VoLayer_, vo_chn_half_l), "XM_MPI_VO_DisableChn");
	CHECK_RET(XM_MPI_VO_DisableChn(VoLayer_, vo_chn_half_r), "XM_MPI_VO_DisableChn");

	return 0;
}

bool MppMdl::AdLoss()
{
	bool ret1 = false, ret2 = false, pre_adloss = ad_loss_;
	uint32_t ad_status = 0;
	Fvideo_GetLossState(&ad_status);
	if ((ad_status&0x1) == 1)
		ret1 = true;
	else
		ret1 = false;

	if (pre_adloss != ret1) {
		ad_status = 0;
		usleep(20*1000);
		Fvideo_GetLossState(&ad_status);
		if ((ad_status & 0x1) == 1)
			ret2 = true;
		else
			ret2 = false;
		if (ret1 == ret2) {
			pre_adloss = ret1;
			ad_loss_ = ret1;
		} else {
			XMLogW("AdLoss, status change in 20ms");
		}
	}

	return pre_adloss;
}

int MppMdl::SetADType(XM_VIDEO_FMT ad_type) 
{
	ad_type_ = ad_type; 
	if (ad_proc_)
		ad_proc_->SetADType(ad_type);
	return 0;
}

int MppMdl::SetPlayMode(XMRealPlayMode play_mode)
{
	int iRet = 0;
	int index = 0;
	VoLayer_ = 0;
	VO_CHN vo_chn_full = 0, vo_chn_half_l = 1, vo_chn_half_r = 2;
	VAR_CHN var_chn_disp = 0;
	VI_CHN vi_chn_sensor = 1;
	VO_DEV vo_dev = 0;

 	XMLogI("SetPlayMode, last mode=%d, cur mode=%d", (int)last_play_mode_, (int)play_mode);
	if (AdLoss() && (play_mode == XM_PLAY_AD || play_mode == XM_PLAY_BOTH)) {
		XMLogE("fvideo loss, but render ad");
		return -1;
	}

	if (play_mode == last_play_mode_) return 0;

	mutex_render_ad_.lock();
	MPP_CHN_S stSrcChn[2] = {
		[0]	= {XM_ID_VAR, 0, var_chn_disp},
		[1]	= {XM_ID_VAR, 0, var_chn_disp},
	};

	MPP_CHN_S stDestChn[2] = {
		[0]	= {XM_ID_VOU, vo_dev, vo_chn_full},
		[1]	= {XM_ID_VOU, vo_dev, vo_chn_half_l},
	};

	CHECK_RET(XM_MPI_VAR_DisableChn(var_chn_disp), "XM_MPI_VAR_EnableChn");
	CHECK_RET(XM_MPI_VI_DisableChn(vi_chn_sensor), "XM_MPI_VI_DisableChn");

	if (play_mode == XM_PLAY_NONE) {
		//关闭屏幕，一般是关机
		last_play_mode_ = play_mode;
		ResetVO();
		mutex_render_ad_.unlock();
		return 0;
	}

	VI_CHN_ATTR_S stViChnAttr;
	CHECK_RET(XM_MPI_VI_GetChnAttr(vi_chn_sensor, &stViChnAttr), "XM_MPI_VI_SetChnAttr");

	if (last_play_mode_ == XM_PLAY_BOTH) {
		int ret = XM_MPI_SYS_UnBind(&stSrcChn[1], &stDestChn[1]);
		if (XM_SUCCESS != ret) {
				XMLogE("XM_MPI_SYS_UnBind error, ret=%x", ret);
		}

		if (play_mode == XM_PLAY_SENSOR) {
			ret = XM_MPI_SYS_Bind(&stSrcChn[0], &stDestChn[0]);	
			if (XM_SUCCESS != ret) {
				XMLogE("XM_MPI_SYS_Bind error, ret=%x", ret);
			}

			stViChnAttr.stDestSize.u32Width =HOR_RES ;// 640;
			stViChnAttr.stDestSize.u32Height = VER_RES ;//368;
			CHECK_RET(XM_MPI_VI_SetChnAttr(vi_chn_sensor, &stViChnAttr), "XM_MPI_VI_SetChnAttr");
			CHECK_RET(XM_MPI_VI_EnableChn(vi_chn_sensor), "XM_MPI_VI_EnableChn");
			CHECK_RET(XM_MPI_VAR_SetRotate(var_chn_disp, DISP_ROTATE_SET), "XM_MPI_VAR_SetRotate");
			CHECK_RET(XM_MPI_VAR_EnableChn(var_chn_disp), "XM_MPI_VAR_EnableChn");
		}	
		else if (play_mode == XM_PLAY_AD) {
		}
		else {
			XMLogI("11111111111111, should not happen");
			mutex_render_ad_.unlock();
			return 0;
		}
		XMLogI("XM_MPI_VO, show 0, hide 1/2");
		CHECK_RET(XM_MPI_VO_ShowChn(VoLayer_, vo_chn_full), "XM_MPI_VO_ShowChn");
		CHECK_RET(XM_MPI_VO_HideChn(VoLayer_, vo_chn_half_l), "XM_MPI_VO_HideChn");
		CHECK_RET(XM_MPI_VO_HideChn(VoLayer_, vo_chn_half_r), "XM_MPI_VO_HideChn");
	}
	else if (last_play_mode_ == XM_PLAY_SENSOR) {
		if (play_mode == XM_PLAY_BOTH) {
			XM_MPI_SYS_UnBind(&stSrcChn[0], &stDestChn[0]);
			XM_MPI_SYS_Bind(&stSrcChn[1], &stDestChn[1]);
			stViChnAttr.stDestSize.u32Width = HOR_RES ;//640;
			stViChnAttr.stDestSize.u32Height =VER_RES ;// 368;
			CHECK_RET(XM_MPI_VI_SetChnAttr(vi_chn_sensor, &stViChnAttr), "XM_MPI_VI_SetChnAttr");
			CHECK_RET(XM_MPI_VI_EnableChn(vi_chn_sensor), "XM_MPI_VI_EnableChn");
			CHECK_RET(XM_MPI_VAR_SetRotate(var_chn_disp, DISP_ROTATE_SET), "XM_MPI_VAR_SetRotate");
			CHECK_RET(XM_MPI_VAR_EnableChn(var_chn_disp), "XM_MPI_VAR_EnableChn");

			CHECK_RET(XM_MPI_VO_ShowChn(VoLayer_, vo_chn_half_r), "XM_MPI_VO_ShowChn");
			CHECK_RET(XM_MPI_VO_ShowChn(VoLayer_, vo_chn_half_l), "XM_MPI_VO_ShowChn");
			CHECK_RET(XM_MPI_VO_HideChn(VoLayer_, vo_chn_full), "XM_MPI_VO_HideChn");	
			XMLogI("XM_MPI_VO, hide 0, show 1/2");
		}
		else if (play_mode == XM_PLAY_AD) {
			XM_MPI_SYS_UnBind(&stSrcChn[0], &stDestChn[0]);
		}
	}
	else if (last_play_mode_ == XM_PLAY_AD) {
		if (play_mode == XM_PLAY_BOTH) {
			XM_MPI_SYS_Bind(&stSrcChn[1], &stDestChn[1]);
			stViChnAttr.stDestSize.u32Width =HOR_RES ;// 640;
			stViChnAttr.stDestSize.u32Height =VER_RES ;// 368;
			CHECK_RET(XM_MPI_VI_SetChnAttr(vi_chn_sensor, &stViChnAttr), "XM_MPI_VI_SetChnAttr");
			CHECK_RET(XM_MPI_VI_EnableChn(vi_chn_sensor), "XM_MPI_VI_EnableChn");
			CHECK_RET(XM_MPI_VAR_SetRotate(var_chn_disp, DISP_ROTATE_SET), "XM_MPI_VAR_SetRotate");
			CHECK_RET(XM_MPI_VAR_EnableChn(var_chn_disp), "XM_MPI_VAR_EnableChn");

			CHECK_RET(XM_MPI_VO_ShowChn(VoLayer_, vo_chn_half_r), "XM_MPI_VO_ShowChn");
			CHECK_RET(XM_MPI_VO_ShowChn(VoLayer_, vo_chn_half_l), "XM_MPI_VO_ShowChn");
			CHECK_RET(XM_MPI_VO_HideChn(VoLayer_, vo_chn_full), "XM_MPI_VO_HideChn");	
			XMLogI("XM_MPI_VO, hide 0, show 1/2");
		}
		else if (play_mode == XM_PLAY_SENSOR) {
			stViChnAttr.stDestSize.u32Width = HOR_RES ;//640;
			stViChnAttr.stDestSize.u32Height = VER_RES ;//368;
			CHECK_RET(XM_MPI_VI_SetChnAttr(vi_chn_sensor, &stViChnAttr), "XM_MPI_VI_SetChnAttr");
			CHECK_RET(XM_MPI_VI_EnableChn(vi_chn_sensor), "XM_MPI_VI_EnableChn");
			CHECK_RET(XM_MPI_VAR_SetRotate(var_chn_disp, DISP_ROTATE_SET), "XM_MPI_VAR_SetRotate");
			CHECK_RET(XM_MPI_VAR_EnableChn(var_chn_disp), "XM_MPI_VAR_EnableChn");
			XM_MPI_SYS_Bind(&stSrcChn[0], &stDestChn[0]);
		}
	}
	else {
		XMLogI("22222222, should not happen");
		mutex_render_ad_.unlock();
		return 0;
	}

	last_play_mode_ = play_mode;
	mutex_render_ad_.unlock();

	return 0;
}

int MppMdl::CameraSetRefrenceLevel(int level)
{
	XMLogI("SetRefrenceLevel level = %d", level);
	int ret = camera_set_refrence_level(level);
	if (ret != XM_SUCCESS) {
		XMLogE("camera_set_refrence_level failed; ret = %d", ret);
	}
	return ret;
}

int MppMdl::CameraSetAwb(int chn, bool enable, int level)
{
	XMLogI("SetAwb chn = %d, enable = %d, level = %d", chn, enable, level);
	ISP_U8 u8Enable = enable ? 1 : 0;
	int ret = camera_set_awb(chn, u8Enable, level);
	if (ret != XM_SUCCESS) {
		XMLogE("camera_set_awb failed; ret = %d", ret);
	}
	return ret;
}

int MppMdl::CameraSetRejectFlicker(int mode)
{
	XMLogI("CameraSetRejectFlicker, mode=%d", mode);
	camera_set_reject_flickerV2(mode);
	return 0;
}

int MppMdl::SetAIVolume(int sound_volume)
{
	XMLogI("SetAIVolume, sound_volume=%d", sound_volume);
	AUDIO_DEV AiDev = 0;
	int ret = XM_MPI_AI_SetVolume(AiDev, sound_volume);
	if (ret) {
		XMLogE("XM_MPI_AI_SetVolume(%d) failed: %#x", AiDev, ret);
		return XM_FAILURE;
	} 
}

int MppMdl::SpeakerEnable(bool enable)
{
	AUDIO_DEV AoDev = 0;
	XM_BOOL bEnable = XM_TRUE;
	XM_BOOL *pbEnable = &bEnable;
	AUDIO_FADE_S *pstFade = NULL;
	XM_MPI_AO_GetMute(AoDev, pbEnable, pstFade);
	if(enable == false && *pbEnable == XM_FALSE){
		XM_MPI_AO_SetMute(AoDev, XM_TRUE, pstFade);
	}
	else if(enable == true  && *pbEnable == XM_TRUE ){
		XM_MPI_AO_SetMute(AoDev, XM_FALSE, pstFade);
	}
    speak_enable_ = enable;

	SetVolumeInternal(enable ? user_volume_ : 0);
	return 0;
}

int MppMdl::SubStreamEnable(bool enable)
{
	XMLogI("SubStreamEnable, enable=%d", enable);
	int vi_chn_substream = 2, venc_chn_substream = 4;
	if (enable) {
		CHECK_RET(XM_MPI_VENC_StartRecvPic(venc_chn_substream), "XM_MPI_VENC_StartRecvPic");
	}
	else {
		CHECK_RET(XM_MPI_VENC_StopRecvPic(venc_chn_substream), "XM_MPI_VENC_StopRecvPic");
	}

	return 0;
}

int MppMdl::ZoomResolution(int channel, VIDEO_FRAME_INFO_S* src_frame)
{
	int s_w, s_h;
	int d_w, d_h;
	uint8_t* s_y_addr = NULL, * s_uv_addr = NULL;
	unsigned int s_phy_addr = 0;
	s_w = src_frame->stVFrame.u32Width;
	s_h = src_frame->stVFrame.u32Height;
	s_phy_addr = src_frame->stVFrame.u32PhyAddr[0];
	int src_size = s_w * s_h * 3 / 2;
	s_y_addr = (unsigned char*)XM_MPI_SYS_Mmap_Cached(s_phy_addr, src_size);

	//等比缩放
	int xrate = s_w/kThumbnailWidth;
	d_w = s_w/xrate;
	d_h = s_h/xrate;

	thumbnail_width_[channel] = d_w;
	thumbnail_height_[channel] = d_h;
	XMLogI("channel=%d, d_w=%d, d_h=%d", channel, d_w, d_h);

	uint8_t* d_y_addr = thumbnail_buf_[channel];
	uint8_t* d_u_addr = d_y_addr + d_w * d_h;
	uint8_t* d_v_addr = d_y_addr + d_w * d_h * 5 / 4;

	XM_MPI_SYS_MmzFlushCache(s_phy_addr, s_y_addr, src_size);

	uint8_t* y_ptr = s_y_addr;
	int i, j;
	for (i = 0; i < d_h; i++){
		for (j = 0; j < d_w; j++) {
			*d_y_addr++ = *y_ptr;
			y_ptr += xrate;
		}
		y_ptr += ((xrate-1)*s_w);
	}

	s_uv_addr = s_y_addr + s_w * s_h;
	for (i = 0; i < d_h / 2; i++) {
		for (j = 0; j < d_w / 2; j++) {
			*d_u_addr++ = *(s_uv_addr + i * xrate * s_w + 2 * j * xrate);
			*d_v_addr++ = *(s_uv_addr + i * xrate * s_w + 2 * j * xrate + 1);
		}
	}
	XM_MPI_SYS_Munmap(s_y_addr, src_size);

	return 0;
}

int MppMdl::GetViFrame(int chn)
{
	VI_CHN vi_chn_enc = 0, vi_chn_sensor = 1, vi_chn_substream = 2, vi_chn_ad = 3;
	VI_CHN vi_chn = chn == 0 ? vi_chn_substream : vi_chn_ad;
	VIDEO_FRAME_INFO_S frame;
	int ret = XM_MPI_VI_GetFrame(vi_chn, &frame, 40);
	if (ret == XM_SUCCESS) {
		ZoomResolution(chn, &frame);
		XM_MPI_VI_ReleaseFrame(vi_chn, &frame);	
		//CHECK_RET(XM_MPI_VI_SetFrameDepth(vi_chn_substream, 0), "XM_MPI_VI_SetFrameDepth");
	} else {
		XMLogE("XM_MPI_VI_GetFrame failed , vi_chn=%d, ret=%x", vi_chn, ret);
		CHECK_RET(XM_MPI_VI_SetFrameDepth(vi_chn_substream, 1), "XM_MPI_VI_SetFrameDepth");
	}
	
	return ret;
}

#endif //#ifndef WIN32
