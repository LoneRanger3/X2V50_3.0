#include "global_data.h"
#include "global_page.h"
#include "PageSet.h"
#include "mpp/MppMdl.h"
#include "gps.h"
extern int g_sd_status;

PageSet::PageSet()
{
	lv_style_init(&subpage_style_);
	lv_style_set_width(&subpage_style_, screen_width);
	lv_style_set_height(&subpage_style_, size_h(298));
	lv_style_set_bg_color(&subpage_style_, list_backcolor);
	lv_style_set_radius(&subpage_style_, 0);
	lv_style_set_border_width(&subpage_style_, 0);
	lv_style_set_text_font(&subpage_style_, lv_font_all);
	lv_style_set_text_color(&subpage_style_, lv_color_white());
	lv_style_set_pad_all(&subpage_style_, 0);
}

PageSet::~PageSet()
{

}

void PageSet::OpenPage()
{
	CreatePage();
}
void SetMenuStyle(lv_obj_t* list, lv_coord_t width, lv_coord_t height)
{

    for (int i = 0; i < lv_obj_get_child_cnt(list); i++)
    {
        lv_obj_t* item = lv_obj_get_child(list, i);
        if (lv_obj_check_type(item, &lv_btn_class))
        {
            lv_obj_set_style_bg_color(item, lv_color_make(140, 140, 140), LV_STATE_FOCUSED);
            lv_obj_set_style_bg_color(item, lv_color_make(140, 140, 140), LV_STATE_FOCUS_KEY);
        }
        else
        {
            lv_obj_set_style_bg_color(item, lv_color_make(68, 68, 68), 0);
        }
    }
}

#define function_bar_ofs_y 7

void PageSet::Createfunction_bar(lv_obj_t* curr_page, lv_coord_t height, lv_coord_t y_ofs, bool ok_enable, bool delete_enable)
{
#if 1
	lv_obj_t* Settings_function_page_ = lv_create_page(curr_page, screen_width, size_h(height),
		lv_color_make(68, 68, 68), 0, 0, lv_font_all, lv_color_white(), 0);//46 36 30
	lv_obj_align(Settings_function_page_, LV_ALIGN_BOTTOM_MID, 0, 0);

	if(ok_enable)
	{
    	lv_obj_t* ok_img_= lv_img_create(Settings_function_page_);
    	lv_img_set_src(ok_img_, image_path"ok.png");
    	lv_obj_align(ok_img_, LV_ALIGN_BOTTOM_LEFT, size_w(40), -size_h(y_ofs));
	}

	#if 1
	if(delete_enable){
		
        lv_obj_t* lock_img_ = lv_img_create(Settings_function_page_);
        lv_img_set_src(lock_img_, image_path"lock_on.png");
        //lv_obj_align_to(lock_img_, ok_img_, LV_ALIGN_OUT_RIGHT_MID, size_w(19), 0);
		lv_obj_align(lock_img_, LV_ALIGN_BOTTOM_LEFT, size_w(53), 0);
		
        lv_obj_t* slash_img_ = lv_img_create(Settings_function_page_);
        lv_img_set_src(slash_img_, image_path"slash.png");
        lv_obj_align_to(slash_img_, lock_img_, LV_ALIGN_OUT_RIGHT_MID, -size_w(6), size_h(0));
    	
    	lv_obj_t* delete_img_ = lv_img_create(Settings_function_page_);
    	lv_img_set_src(delete_img_, image_path"delete.png");
    	lv_obj_align_to(delete_img_, slash_img_, LV_ALIGN_OUT_RIGHT_MID, -size_w(4), 0);
	}
	#else
	lv_obj_t* delete_img_= lv_img_create(Settings_function_page_);
	lv_img_set_src(delete_img_, image_path"ok.png");
	lv_obj_align(delete_img_, LV_ALIGN_BOTTOM_LEFT, size_w(72), -size_h(y_ofs));
    #endif
	
	lv_obj_t* up_img_= lv_img_create(Settings_function_page_);
	lv_img_set_src(up_img_, image_path"up.png");
	lv_obj_align(up_img_, LV_ALIGN_BOTTOM_MID, -size_w(0), -size_h(y_ofs));

	lv_obj_t* down_img_= lv_img_create(Settings_function_page_);
	lv_img_set_src(down_img_, image_path"down.png");
	//lv_obj_align(down_img_, LV_ALIGN_BOTTOM_MID, size_w(0), size_h(0));
	lv_obj_align_to(down_img_, up_img_, LV_ALIGN_OUT_RIGHT_MID, size_w(100), size_h(0));
	lv_obj_t* return_img_= lv_img_create(Settings_function_page_);
	lv_img_set_src(return_img_, image_path"set_return.png");
	lv_obj_align(return_img_, LV_ALIGN_BOTTOM_RIGHT, -size_w(53), -size_h(y_ofs));
#endif

}

void PageSet::CreatePage()
{
	set_page_ = lv_create_page(lv_scr_act(), screen_width, screen_height, lv_color_make(16, 16, 16), 0, 0,
        lv_font_all, lv_color_white(), 0);
	lv_obj_set_pos(set_page_, 0, start_y);
	//lv_obj_add_event_cb(set_page_, DelSetPageEvent, LV_EVENT_DELETE, NULL);

	lv_obj_t* top_page = lv_create_page(set_page_, screen_width, size_h(60),
		lv_color_make(68, 68, 68), 0, 0, lv_font_all, lv_color_white(), 0);
	lv_obj_align(top_page, LV_ALIGN_TOP_MID, 0, 0);
	
	title_label_ = lv_label_create(top_page);
	lv_label_set_text(title_label_, GetParsedString("Set"));
	lv_obj_align(title_label_, LV_ALIGN_CENTER, 0, size_h(3));

	sys_set_list_ = lv_create_page(set_page_, screen_width, size_h(PAGE_MENU_LIST_H),
		lv_color_make(16, 16, 16), 0, 0, lv_font_all, lv_color_white(), 0);
	lv_obj_align(sys_set_list_, LV_ALIGN_TOP_MID, size_w(0), size_h(62));
	lv_obj_set_scrollbar_mode(sys_set_list_, LV_SCROLLBAR_MODE_OFF);
	lv_obj_add_flag(sys_set_list_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_add_event_cb(sys_set_list_, OpenSubpage, LV_EVENT_ALL, NULL);
   uint8_t Total_num =sizeof(sys_menu_config_table)/sizeof(sys_menu_config_table[0]);
	lv_obj_t* set_btn[100] = { NULL };
    #if GPS_EN
    if (!gps_insert_flag) {
        Total_num=Total_num - 3;
        }
    #endif
	for (int i = 0;i < Total_num;i++) {

		set_btn[i] = lv_create_btn(sys_set_list_, screen_width, 58, lv_color_make(68, 68, 68), 2,
		OpenSubpage, LV_EVENT_ALL, (void*)i, 0);
        lv_obj_remove_style(set_btn[i], NULL, LV_STATE_FOCUS_KEY);
        if (i == 0) {
    		lv_obj_align(set_btn[i], LV_ALIGN_TOP_LEFT, size_w(0), size_h(0));
    	}
    	else {
            lv_obj_align_to(set_btn[i], set_btn[i - 1], LV_ALIGN_OUT_BOTTOM_LEFT, 0, size_h(2));
    	}

       lv_obj_t* label = NULL;
       label = lv_label_create(set_btn[i]);
       lv_label_set_text(label, GetParsedString(sys_menu_config_table[i].menu_name));
       lv_obj_set_width(label, size_w(screen_width));
       lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_LEFT, 0);
	   lv_obj_align(label, LV_ALIGN_TOP_LEFT, size_h(20), size_h(3));
	}

    SetMenuStyle(sys_set_list_, screen_width, size_h(PAGE_MENU_LIST_H));
	#if 1
	Createfunction_bar(set_page_,58,7,true,false);
	#endif
}

void PageSet::DelSetPageEvent(lv_event_t* e)
{
    int Total_num=Subpage_Total;
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_DELETE) {
		PageSet* page_set = GlobalPage::Instance()->page_set();
    #if GPS_EN
		if (!gps_insert_flag) Total_num=Subpage_Total - 3;
	 #endif	
		for (int i = 0;i < Total_num;i++) {
			lv_obj_remove_event_cb(lv_obj_get_parent(page_set->menu_img_[i]), page_set->OpenSubpage);
		}
	}
}

void PageSet::MirrorSwitch(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		lv_obj_t* user_data = (lv_obj_t*)lv_event_get_user_data(e);
		XM_CONFIG_VALUE cfg_value;
		cfg_value.bool_value = false;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Behind_Mirror, cfg_value);
		if (cfg_value.bool_value) {
			cfg_value.bool_value = false;
			lv_obj_clear_state(user_data, LV_STATE_CHECKED);
		}
		else {
			cfg_value.bool_value = true;
			lv_obj_add_state(user_data, LV_STATE_CHECKED);
		}

		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Behind_Mirror, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Behind_Mirror");
		}

		XMUIEventInParam event_inparam;
		XMUIEventOutParam event_outparam;
		event_inparam.set_mirror.chn = Direction_Behind;
		event_inparam.set_mirror.open_mirror = cfg_value.bool_value;
		GlobalData::Instance()->ui_event_cb_(XM_UI_MIRROR_SET_MIRROR, &event_inparam, &event_outparam);
	}
}

void PageSet::RecordVoiceEnable(bool enable)
{
	XM_CONFIG_VALUE cfg_value;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Record_Voice, cfg_value);
	if (cfg_value.bool_value == enable) return;

	cfg_value.bool_value = enable;
	XM_Middleware_Storage_EnableRecordAudio(cfg_value.bool_value);
	int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Record_Voice, cfg_value);
	if (ret < 0) {
		XMLogE("set config error, opr=CFG_Operation_Record_Voice");
	}

	if (cfg_value.bool_value) {
		lv_img_set_src(GlobalPage::Instance()->page_main()->silent_record_img_, image_path"mic_on.png");
		lv_img_set_src(GlobalPage::Instance()->page_main()->mic_record_img_, image_path"i_aud_o.png");
	}
	else {
		lv_img_set_src(GlobalPage::Instance()->page_main()->silent_record_img_, image_path"mic_off.png");
	    lv_img_set_src(GlobalPage::Instance()->page_main()->mic_record_img_, image_path"i_aud.png");
	}

	if (lv_obj_has_flag(GlobalPage::Instance()->page_main()->main_page_, LV_OBJ_FLAG_HIDDEN)) {
		lv_obj_clear_flag(GlobalPage::Instance()->page_main()->main_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_add_flag(GlobalPage::Instance()->page_main()->main_page_, LV_OBJ_FLAG_HIDDEN);
	}
	else {
		lv_obj_add_flag(GlobalPage::Instance()->page_main()->main_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(GlobalPage::Instance()->page_main()->main_page_, LV_OBJ_FLAG_HIDDEN);
	}
}

void PageSet::OpenSubpage(lv_event_t* e)
{
   static int  user_position = 0;
   PageSet* page_set = GlobalPage::Instance()->page_set();
   PageVideoSet* page_video_set = GlobalPage::Instance()->page_video_set();
   PageSysSet* page_sys_set = GlobalPage::Instance()->page_sys_set();
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_add_flag(page_set->sys_set_list_, LV_OBJ_FLAG_HIDDEN);
		//获取当前的聚焦对象
		page_set->foucused_obj = lv_group_get_focused(GlobalData::Instance()->group);
        page_set->menu_position = user_data;
        lv_label_set_text(page_set->title_label_, GetParsedString(sys_menu_config_table[user_data].menu_name));
		if (user_data == Subpage_Return) {
			lv_obj_del(page_set->set_page_);
			page_set->set_page_=NULL;
		 	GlobalPage::Instance()->page_main()->LeaveMainPageWork(false);
        }else if (user_data == Subpage_WifiSet) {
            page_sys_set->OpenWifiSetPage();
        }else if (user_data == Subpage_VoiceControl) {
            page_sys_set->OpenVoiceControlPage();
        }else if (user_data == Subpage_TimeSet) {
            page_sys_set->OpenTimeSetPage(); 
        }else if (user_data == Subpage_Format) {
            page_sys_set->OpenFormatPage();
        }else if (user_data == Subpage_DefaultSet) {
            page_sys_set->OpenDefaultSetPage();
        }else if (user_data == Subpage_Edition) {
            page_sys_set->OpenEditionPage();
        }else if (user_data == Subpage_GpsInfo) {
            page_sys_set->OpenGpsInfoPage();
        }
        else {
            #if 1
			lv_obj_t* page = lv_create_page(lv_scr_act(), screen_width, size_h(PAGE_MENU_LIST_H),
				lv_color_make(16, 16, 16), 0, 0, lv_font_all, lv_color_white(), 0);
			lv_obj_set_pos(page, 0, 122);
			#else
            lv_obj_t* page = lv_obj_create(lv_scr_act());
			#endif
            lv_obj_set_style_text_font(page, lv_font_all, 0);
            lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_ACTIVE);
            lv_obj_set_scroll_dir(page,LV_DIR_VER);
            lv_obj_add_style(page, &GlobalPage::Instance()->page_set()->subpage_style_, 0);
            lv_obj_add_event_cb(page, GlobalPage::Instance()->page_sys_set()->DeletedEvent, LV_EVENT_DELETE, NULL);
            //lv_obj_align(page, LV_ALIGN_TOP_MID, 0, size_h(48) + start_y);

            lv_obj_t* list = lv_list_create(page);

            lv_obj_t* btn[20] = { NULL };
            XM_CONFIG_VALUE cfg_value;
            cfg_value.int_value = 0;
            int move_state = 1;
            for (int i = 0; i < sys_menu_config_table[user_data].two_table_num; i++) {
                 btn[i] = lv_list_chile_btn(list, NULL, GetParsedString(sys_menu_config_table[user_data].two_table_str[i]));
                 if (user_data == Subpage_LanguageSet) {
                     lv_obj_set_style_text_font(btn[i], (lv_font_t*)lv_font_size[sys_menu_config_table[user_data].table_value[i]], 0);
                 }
                 lv_obj_add_event_cb(btn[i], OpenSubpage_Function, LV_EVENT_ALL, (void*)sys_menu_config_table[user_data].table_value[i]);

                 GlobalData::Instance()->car_config()->GetValue(sys_menu_config_table[user_data].set_name , cfg_value);
                 if (cfg_value.int_value== sys_menu_config_table[user_data].table_value[i]) {
                     move_state = 0;
                     lv_obj_add_state(btn[i],  LV_STATE_FOCUS_KEY);
                 }
                 else  if(move_state){
                     lv_group_focus_next(GlobalData::Instance()->group);
                 }
              
            }
        //    if(user_data==Subpage_Resolution){
        //        if(!MppMdl::Instance()->AdLoss()  && (cfg_value.int_value==VideoResolution_3K || cfg_value.int_value==VideoResolution_4K)){
        //           lv_group_focus_next(GlobalData::Instance()->group);
		// 		}
		//     }
            GlobalPage::Instance()->page_main()->SetListStyle(list, GlobalPage::Instance()->page_sys_set()->subpage_width_,
                GlobalPage::Instance()->page_sys_set()->subpage_height_);
            lv_obj_center(list);

            GlobalData::Instance()->opened_subpage_[0] = page;
        }
        lv_group_focus_next(GlobalData::Instance()->group);
	}
	else if (code == LV_EVENT_FOCUSED) {
		PageSet* page_set = GlobalPage::Instance()->page_set();
		int user_data = (int)lv_event_get_user_data(e);


        user_position = user_data;

		lv_obj_add_flag(page_set->sys_set_list_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(page_set->sys_set_list_, LV_OBJ_FLAG_HIDDEN);
	}
	else if (code == LV_EVENT_DEFOCUSED ) {
		int user_data = (int)lv_event_get_user_data(e);
	}
	else if (code == LV_EVENT_PRESSING || code == LV_EVENT_RELEASED) {
		lv_obj_add_flag(page_set->sys_set_list_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(page_set->sys_set_list_, LV_OBJ_FLAG_HIDDEN);
	}
}

void PageSet::OpenSubpage_Function(lv_event_t* e)
{
    PageVideoSet* page_video_set = GlobalPage::Instance()->page_video_set();
    PageSet* page_set = GlobalPage::Instance()->page_set();
    PageSysSet* page_sys_set = GlobalPage::Instance()->page_sys_set();
    switch (page_set->menu_position) {
     case Subpage_Resolution:
          page_video_set->ChangeVideoResolution(e);
          break;
     case Subpage_CirRecordTime:
         page_video_set->ChangeCirRecordTime(e);
          break;
     case Subpage_CollisionSensitivity:
         page_sys_set->ChangeCollisionSens(e);
          break;
     case Subpage_ParkMonitor:
         page_video_set->CollisionStartupSwitch(e);
          break;
     case Subpage_Recordfps:
         page_video_set->ChangCompactRecordFps(e);
          break;
     case Subpage_Compactduration:
         page_video_set->ChangeCompactRecordDur(e);
          break;
     case Subpage_ScreenSaver:
         page_sys_set->ChangeRestScreenTime(e);
          break;
     case Subpage_Lcd_Light:
         page_sys_set->ChangeScreenList(e);
          break;
     case Subpage_RecordSound:
         page_video_set->ChangeRecordSoundEvent(e);
          break;
     case Subpage_KeyTone:
         //page_sys_set->ChangeKeyToneEvent(e);
         page_sys_set->ChangeBootToneEvent(e);
          break;
     case Subpage_Watermark:
	     page_video_set->ChangeDateWatermarkEvent(e);
          break;
     case Subpage_Fatigue_reminder:
	     page_sys_set->ChangeFatigueReminder(e);
          break;
     case Subpage_BootTone:
         page_sys_set->ChangeBootToneEvent(e);
          break;
     case Subpage_ExposureCompensation:
         page_video_set->ChangeExposureCompensation(e);
         break;
     case Subpage_WhiteBalance:
         page_video_set->ChangeWhiteBalance(e);
         break;
     case Subpage_RejectFlicker:
         page_video_set->ChangeRejectFlicker(e);
         break;
     case Subpage_LanguageSet:
         page_sys_set->ChangeLanguage(e);
         break;
     case Subpage_AutoShutdown:
         page_sys_set->ChangeShutdownTime(e);
         break;
     case Subpage_Mirror:
         page_video_set->ChangeMirrorSwitch(e);
         break;
     case Subpage_GpsWatermark:
         page_sys_set->GpsWatermark(e);
         break;
     case Subpage_SpeedUnit:
         page_sys_set->ChangeSpeedUnit(e);
         break;
    }

}


void PageSet::CloseSubPage(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		if (user_data == Subpage_ParkMonitor || user_data == Subpage_Edition) {
			if (GlobalData::Instance()->opened_subpage_[0]) {
				lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
				GlobalData::Instance()->opened_subpage_[0] = NULL;
			}
		}
		else if (user_data == Subpage_VoiceControl) {
			GlobalPage::Instance()->page_sys_set()->VoiceControlKeypadEvent(KEYMAP_MENU);
		}
	}
}

void PageSet::SetSwitchStyle(lv_obj_t* sw)
{
	lv_obj_set_style_bg_color(sw, lv_color_make(0, 200, 135), LV_PART_INDICATOR | LV_STATE_CHECKED);
	lv_obj_set_style_bg_color(sw, lv_color_white(), LV_PART_KNOB);
	lv_obj_align(sw, LV_ALIGN_RIGHT_MID, 0, 0);
}

void PageSet::ReturnPreMenu()
{
	if (GlobalData::Instance()->opened_subpage_[1]) {
		lv_obj_del(GlobalData::Instance()->opened_subpage_[1]);
		GlobalData::Instance()->opened_subpage_[1] = NULL;
	}
	else if (GlobalData::Instance()->opened_subpage_[0]) {
		lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
		GlobalData::Instance()->opened_subpage_[0] = NULL;
	}
}