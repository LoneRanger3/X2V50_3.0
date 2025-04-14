#include "global_data.h"
#include "global_page.h"
#include "PageVideoSet.h"
#include "mpp/MppMdl.h"
#include "set_config.h"
#include "osd_user.h"
PageVideoSet::PageVideoSet():subpage_width_(screen_width), subpage_height_(size_h(298))
{

}

PageVideoSet::~PageVideoSet()
{

}

void PageVideoSet::ChangeVideoResolution(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		XM_CONFIG_VALUE cfg_value;
		XMUIEventInParam event_inparam;
		XMUIEventOutParam event_outparam;
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		
		cfg_value.int_value = PT_H264;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Encode_Format, cfg_value);
		if (cfg_value.int_value == PT_H264) {
			event_inparam.set_enc.video_codec_type = PT_H264;
		}
		else {
			event_inparam.set_enc.video_codec_type = PT_H265;
		}
    	if (user_data == VideoResolution_4K) {
			event_inparam.set_enc.width = 3840;
			event_inparam.set_enc.height = k4KHeight;
			event_inparam.set_enc.bit_rate = 20480;
			cfg_value.int_value = k4KHeight;
			lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "4K");
		}
		else if (user_data == VideoResolution_3K) {
			event_inparam.set_enc.width = 3008;
			event_inparam.set_enc.height = k3KHeight;
			event_inparam.set_enc.bit_rate = 20480;
			cfg_value.int_value = k3KHeight;
			lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "3K");
		}
		else if (user_data == VideoResolution_1440P) {
			event_inparam.set_enc.width = 2560;
			event_inparam.set_enc.height = 1440;
			event_inparam.set_enc.bit_rate = 20480;
			cfg_value.int_value = 1440;
			lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "2K");
		}
		else if (user_data == VideoResolution_1080P) {
			event_inparam.set_enc.width = 1920;
			event_inparam.set_enc.height = 1080;
			event_inparam.set_enc.bit_rate = 4096;
			cfg_value.int_value = 1080;
			lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "1080P");
		}

		event_inparam.set_enc.channel = Direction_Front;
		//if (user_data == VideoResolution_3K || user_data == VideoResolution_4K) {
		//	if (MppMdl::Instance()->AdLoss()) {
			//    if(cfg_value.int_value == k3KHeight)
			// 	lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "3K");
			// 	else 
			// 	lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "4K");
			// 	GlobalData::Instance()->ui_event_cb_(XM_UI_SET_ENCODE_PARAM, &event_inparam, &event_outparam);
			// }else{
			// 	event_inparam.set_enc.width = 2560;
			// 	event_inparam.set_enc.height = 1440;
			// 	event_inparam.set_enc.bit_rate = 20480;
			// 	GlobalData::Instance()->ui_event_cb_(XM_UI_SET_ENCODE_PARAM, &event_inparam, &event_outparam);
			// 	lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "2K");
			// }
		// }
		// else {
		 	GlobalData::Instance()->ui_event_cb_(XM_UI_SET_ENCODE_PARAM, &event_inparam, &event_outparam);
		// }
		
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Video_Resolution, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Video_Resolution");
		}

		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
		
	     osd_data_init();
	}
}


void PageVideoSet::DeletedEvent(lv_event_t* e)
{
	int user_data = (int)lv_event_get_user_data(e);
	lv_obj_clear_flag(GlobalData::Instance()->opened_subpage_[0], LV_OBJ_FLAG_HIDDEN);
	if (user_data == Subpage_ParkMonitor) {
		lv_label_set_text(GlobalPage::Instance()->page_set()->title_label_, GetParsedString("Park Monitor"));
	}
	
	lv_group_focus_obj(GlobalPage::Instance()->page_video_set()->foucused_obj_);
}


void PageVideoSet::ChangeExposureCompensation(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		XM_CONFIG_VALUE cfg_value;
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);

		cfg_value.int_value = user_data;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Exposure_Compensation, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Exposure_Compensation");
		}
		MppMdl::Instance()->CameraSetRefrenceLevel(cfg_value.int_value);
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}

void PageVideoSet::ChangeRejectFlicker(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		XM_CONFIG_VALUE cfg_value;
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
		cfg_value.int_value = user_data;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Reject_Flicker, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Reject_Flicker");
		}
		MppMdl::Instance()->CameraSetRejectFlicker(cfg_value.int_value);
         GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Reject_Flicker, cfg_value);
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}

void PageVideoSet::ChangeWhiteBalance(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		XM_CONFIG_VALUE cfg_value;
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
		cfg_value.int_value = user_data;
		if (cfg_value.int_value == 0) {
			MppMdl::Instance()->CameraSetAwb(0, false, 6500);
		}
		else {
			MppMdl::Instance()->CameraSetAwb(0, true, cfg_value.int_value);
		}
		
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_White_Balance, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_White_Balance");
		}
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}
void PageVideoSet::ChangeMirrorSwitch(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        int user_data = (int)lv_event_get_user_data(e);
        lv_obj_t* target = lv_event_get_target(e);
        GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
        XM_CONFIG_VALUE cfg_value;
        cfg_value.int_value = user_data;
        int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Behind_Mirror, cfg_value);
        if (ret < 0) {
            XMLogE("set config error, opr=CFG_Operation_Behind_Mirror");
        }else{
			MppMdl::Instance()->SetMirror(1, user_data ? XM_MIRROR_VI : XM_MIRROR_NONE);
		}
        GlobalPage::Instance()->page_set()->ReturnPreMenu();
    }
}
	
void PageVideoSet::CollisionStartupSwitch(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
        lv_obj_t* target = lv_event_get_target(e);
		lv_obj_t* user_data = (lv_obj_t*)lv_event_get_user_data(e);
		XM_CONFIG_VALUE cfg_value;
		cfg_value.bool_value = user_data;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Collision_Startup, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Collision_Startup");
		}
		GlobalPage::Instance()->page_main()->UpdateCollisionStartupImg();
        GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
        GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}


void PageVideoSet::ChangCompactRecordFps(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		XM_CONFIG_VALUE cfg_value;
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		PageSet* page_set = GlobalPage::Instance()->page_set();
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);

        cfg_value.int_value = user_data;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Compact_Record_Fps, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Compact_Record_Fps");
		}
		XM_Middleware_ParkRecord_SetTimeLapse((XMTimeLapse)user_data);
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}


void PageVideoSet::ChangeCompactRecordDur(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		XM_CONFIG_VALUE cfg_value;
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		PageSet* page_set = GlobalPage::Instance()->page_set();
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
        cfg_value.int_value = user_data;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Compact_Record_Duration, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Compact_Record_Duration");
		}
		XM_Middleware_ParkRecord_SetTimeLapseDuration((XMTimeLapseDuration)user_data);
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}

void PageVideoSet::ChangeCirRecordTime(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		XM_CONFIG_VALUE cfg_value;
		XMUIEventInParam event_inparam;
		XMUIEventOutParam event_outparam;
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);

		XMStorageRecordDuration dur = XM_STORAGE_Record_Duration_60;
		if (user_data == CirRecordTimeBtn_1min) {
			dur = XM_STORAGE_Record_Duration_60;
		}
		else if (user_data == CirRecordTimeBtn_2min) {
			dur = XM_STORAGE_Record_Duration_120;
		}
		else if (user_data == CirRecordTimeBtn_3min) {
			dur = XM_STORAGE_Record_Duration_180;
		}
		XM_Middleware_Storage_SetRecordDuration(dur);

		cfg_value.int_value = user_data;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Circular_Record_Time, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Circular_Record_Time");
		}
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
		//GlobalPage::Instance()->page_main()->UpdateCirRecordTimeLabel();
	}
}


void PageVideoSet::ChangeRecordSoundEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
		
		XM_CONFIG_VALUE cfg_value;
		cfg_value.bool_value = user_data;
		GlobalPage::Instance()->page_set()->RecordVoiceEnable(cfg_value.bool_value);
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}

void PageVideoSet::OpenDateWatermarkPage()
{
	lv_obj_t* page = lv_obj_create(lv_scr_act());
	lv_obj_add_style(page, &GlobalPage::Instance()->page_set()->subpage_style_, 0);
	lv_obj_add_event_cb(page, GlobalPage::Instance()->page_sys_set()->DeletedEvent, LV_EVENT_DELETE, NULL);
	lv_obj_align(page, LV_ALIGN_TOP_MID, 0, size_h(50) + start_y);

	lv_obj_t* list = lv_list_create(page);
	lv_obj_t* btn = lv_list_add_btn(list, NULL, GetParsedString("Close"));
	lv_obj_add_event_cb(btn, ChangeDateWatermarkEvent, LV_EVENT_ALL, (void*)DateWatermark_Close);

	btn = lv_list_add_btn(list, NULL, GetParsedString("Open"));
	lv_obj_add_event_cb(btn, ChangeDateWatermarkEvent, LV_EVENT_ALL, (void*)DateWatermark_Open);

	GlobalPage::Instance()->page_main()->SetListStyle(list, GlobalPage::Instance()->page_sys_set()->subpage_width_,
		GlobalPage::Instance()->page_sys_set()->subpage_height_);
	lv_obj_center(list);

	XM_CONFIG_VALUE cfg_value;
	cfg_value.bool_value = true;
	int move_num = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Date_Watermark, cfg_value);
	if (cfg_value.bool_value) {
		move_num = 1;
	}

	for (int i = 0; i < move_num; i++) {
		lv_group_focus_next(GlobalData::Instance()->group);
	}

	GlobalPage::Instance()->page_sys_set()->AddCb(list, move_num);

	GlobalData::Instance()->opened_subpage_[0] = page;
}

void PageVideoSet::ChangeDateWatermarkEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);

		XM_CONFIG_VALUE cfg_value;
		cfg_value.bool_value = false;
		if (user_data == DateWatermark_Close) {
			cfg_value.bool_value = false;
		}
		else if (user_data == DateWatermark_Open) {
			cfg_value.bool_value = true;
		}

		//XM_Middleware_Encode_EnableOsdTime(cfg_value.bool_value, osd_x, osd_y);
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Date_Watermark, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Date_Watermark");
		}
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
		osd_data_init();
	}
}