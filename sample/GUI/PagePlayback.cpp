#include "global_data.h"
#include "global_page.h"
#include "PagePlayback.h"
#include "porting/lv_port_indev_template.h"

extern int g_sd_status;

PagePlayback::PagePlayback():playback_page_(NULL), video_time_label_(NULL), video_set_page_(NULL),
playing_file_(false), play_preview_(false), video_count_(0), video_timer_(NULL), close_preview_(false), select_filetype_page_(NULL),OpenPageTimer_handle(NULL)
{
	playing_file_info_.direction = Direction_Front;
	playing_file_info_.file_num = 0;
	playing_file_info_.play_handle = -1;
	playing_file_info_.video_current_time = 0;
	playing_file_info_.video_duration = 0;
	playing_file_info_.width = 0;
	playing_file_info_.height = 0;
}

PagePlayback::~PagePlayback()
{

}

void PagePlayback::OpenPage()
{
	CreatePage();
	//进入回放界面开启自动关机功能
	GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
}

void PagePlayback::CreatePage()
{
	//回放页面
	playback_page_ = lv_create_page(lv_scr_act(), screen_width, screen_height, lv_color_make(10, 10, 10), 0, 0,
		 &lv_font_montserrat_16, lv_color_white(), 0);
	lv_obj_set_pos(playback_page_, 0, start_y);

	//文件名
	video_label_ = lv_label_create(playback_page_);
	//lv_obj_set_style_text_font(video_label_, &lv_font_montserrat_26, LV_PART_MAIN);
	lv_label_set_text(video_label_, playing_file_info_.file_name);
	lv_obj_align(video_label_, LV_ALIGN_TOP_RIGHT, -size_w(10), size_h(10));

	//播放条
	static lv_style_t style_main;
	static lv_style_t style_indicator;
	static lv_style_t style_knob;
	static lv_style_t style_pressed_color;
	lv_style_init(&style_main);
	lv_style_set_bg_opa(&style_main, LV_OPA_COVER);
	lv_style_set_bg_color(&style_main, lv_color_make(255, 255, 255));
	lv_style_set_radius(&style_main, LV_RADIUS_CIRCLE);
	lv_style_set_pad_ver(&style_main, -2); /*Makes the indicator larger*/
	lv_style_set_width(&style_main, size_w(360));
	lv_style_set_height(&style_main, size_h(4));

	lv_style_init(&style_indicator);
	lv_style_set_bg_opa(&style_indicator, LV_OPA_COVER);
	lv_style_set_bg_color(&style_indicator, lv_palette_main(LV_PALETTE_CYAN));
	lv_style_set_radius(&style_indicator, LV_RADIUS_CIRCLE);

	lv_style_init(&style_knob);
	lv_style_set_bg_opa(&style_knob, LV_OPA_30);
	lv_style_set_bg_color(&style_knob, lv_color_make(0, 206, 209));
	lv_style_set_border_color(&style_knob, lv_palette_darken(LV_PALETTE_CYAN, 3));
	lv_style_set_border_width(&style_knob, 2);
	lv_style_set_radius(&style_knob, LV_RADIUS_CIRCLE);
	lv_style_set_pad_all(&style_knob, 8); /*Makes the knob larger*/

	lv_style_init(&style_pressed_color);
	lv_style_set_bg_color(&style_pressed_color, lv_palette_darken(LV_PALETTE_CYAN, 2));

	play_slider_ = lv_slider_create(playback_page_);
	lv_obj_remove_style_all(play_slider_);
	int max_range = playing_file_info_.video_duration > 0 ? playing_file_info_.video_duration : 1;
	lv_slider_set_range(play_slider_, 0, max_range);

	lv_obj_add_style(play_slider_, &style_main, LV_PART_MAIN);
	lv_obj_add_style(play_slider_, &style_indicator, LV_PART_INDICATOR);
	lv_obj_add_style(play_slider_, &style_pressed_color, LV_PART_INDICATOR | LV_STATE_PRESSED);
	lv_obj_add_style(play_slider_, &style_knob, LV_PART_KNOB);
	lv_obj_add_style(play_slider_, &style_pressed_color, LV_PART_KNOB | LV_STATE_PRESSED);

	
	 if(GlobalPage::Instance()->page_main()->touch_plan){
		lv_obj_align(play_slider_, LV_ALIGN_BOTTOM_MID, 0, -size_h(110));
	 }else{
       lv_obj_align(play_slider_, LV_ALIGN_BOTTOM_MID, 0, -size_h(30));
	}
	lv_obj_add_event_cb(play_slider_, ChangeProgressEvent, LV_EVENT_ALL, NULL);
	lv_obj_set_ext_click_area(play_slider_, 20);

	//播放时间
	video_time_label_ = lv_label_create(playback_page_);
	lv_label_set_text(video_time_label_, "00:00");
	lv_obj_align_to(video_time_label_, play_slider_, LV_ALIGN_OUT_LEFT_MID, -size_w(30), 0);

	//文件总时间
	total_duration_label_ = lv_label_create(playback_page_);
	lv_label_set_text_fmt(total_duration_label_, "%02d:%02d", playing_file_info_.video_duration / 60, playing_file_info_.video_duration % 60);
	lv_obj_align_to(total_duration_label_, play_slider_, LV_ALIGN_OUT_RIGHT_MID, size_w(30), 0);

	//文件分辨率
	resolution_label_ = lv_label_create(playback_page_);
	if(playing_file_info_.height == k4KHeight)
		lv_label_set_text(resolution_label_, "4K");
   else if(playing_file_info_.height == k3KHeight)
		lv_label_set_text(resolution_label_, "3K");
	else if(playing_file_info_.height == 1440)		
	    lv_label_set_text(resolution_label_, "2K");
	else
		lv_label_set_text_fmt(resolution_label_, "%dP", playing_file_info_.height);
	lv_obj_align(resolution_label_, LV_ALIGN_TOP_RIGHT, -size_w(10), size_h(30));

	//文件是否加锁
	lock_label_ = lv_label_create(playback_page_);
	lv_label_set_text(lock_label_, "\uF084");
	lv_obj_align_to(lock_label_, resolution_label_, LV_ALIGN_OUT_RIGHT_MID, size_w(20), 0);
	if (!playing_file_info_.file_locked) {
		lv_obj_add_flag(lock_label_, LV_OBJ_FLAG_HIDDEN);
	}
	
	std::string img_path[PlayBackBtn_Total] = { image_path"return_rec.png", image_path"return_pre.png", image_path"pre.png",
	image_path"stop.png", image_path"next.png", image_path"menu.png" };
	for (int i = 0;i < PlayBackBtn_Total;i++) {
		playback_btn_[i] = lv_img_create(playback_page_);
		lv_img_set_src(playback_btn_[i], img_path[i].c_str());
		lv_obj_set_style_img_recolor_opa(playback_btn_[i], LV_OPA_30, LV_STATE_PRESSED);

		if (i == PlayBackBtn_ReturnRec) {
			lv_obj_align(playback_btn_[i], LV_ALIGN_BOTTOM_LEFT, size_w(71), -size_h(30));
		}
		else {
			lv_obj_align_to(playback_btn_[i], playback_btn_[i - 1], LV_ALIGN_OUT_RIGHT_MID, size_w(64), 0);
		}
		lv_obj_add_flag(playback_btn_[i], LV_OBJ_FLAG_CLICKABLE);
		lv_obj_set_ext_click_area(playback_btn_[i], 20);
		lv_obj_add_event_cb(playback_btn_[i], BtnEvent, LV_EVENT_ALL, (void*)i);
		 if(GlobalPage::Instance()->page_main()->touch_plan==0){
			lv_obj_add_flag(playback_btn_[i], LV_OBJ_FLAG_HIDDEN);
			}
	}
  
	video_timer_ = lv_timer_create(VideoTimer, 500, NULL);
	lv_timer_pause(video_timer_);

	if (playing_file_info_.play_handle == -1) {
		GlobalPage::Instance()->page_main()->OpenTipBox("Play error");
	}
	else {
		lv_obj_set_style_bg_opa(playback_page_, 0, 0);
	}
#if 1
    GlobalPage::Instance()->page_set()->Createfunction_bar(playback_page_,23,0,true,true);
#endif
}

void PagePlayback::BtnEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		PagePlayback* page_playback = GlobalPage::Instance()->page_playback();
		int user_data = (int)lv_event_get_user_data(e);
		if (user_data == PlayBackBtn_Play) {
			page_playback->PlaybackKeypadEvent(KEYMAP_OK);
		}
		else if (user_data == PlayBackBtn_Pre) {
			page_playback->PlaybackKeypadEvent(KEYMAP_UP);
		}
		else if (user_data == PlayBackBtn_Next) {
			page_playback->PlaybackKeypadEvent(KEYMAP_DOWN);
		}
		else if (user_data == PlayBackBtn_ReturnRec) {
			GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape);
		}
		else if (user_data == PlayBackBtn_ReturnPre) {
			GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape, true);
		}
		else if (user_data == PlayBackBtn_Menu) {
			if (page_playback->playing_file_) {
				page_playback->PlaybackKeypadEvent(KEYMAP_OK);
			}
			
			lv_obj_add_flag(page_playback->playback_page_, LV_OBJ_FLAG_HIDDEN);
			page_playback->OpenSetPage();
		}
	}
	else if (code == LV_EVENT_PRESSED) {
		PagePlayback* page_playback = GlobalPage::Instance()->page_playback();
		if (page_playback->playback_page_ == NULL)
			return;
		
		lv_obj_add_flag(page_playback->playback_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(page_playback->playback_page_, LV_OBJ_FLAG_HIDDEN);
	}
}

void PagePlayback::SelectFileStyle(lv_obj_t* list, lv_coord_t width, lv_coord_t height)
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
void PagePlayback::OpenSetPage()
{
	video_set_page_ = lv_create_page(lv_scr_act(), screen_width, screen_height, lv_color_make(16, 16, 16), 0, 0,
		lv_font_all, lv_color_white(), 0);
	lv_obj_add_event_cb(video_set_page_, DelSetPageEvent, LV_EVENT_DELETE, NULL);
	lv_obj_set_pos(video_set_page_, 0, start_y);

    lv_obj_t* top_page = lv_create_page(video_set_page_, screen_width, size_h(60),
        lv_color_make(68, 68, 68), 0, 0, lv_font_all, lv_color_white(), 0);
    lv_obj_align(top_page, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* title_label_ = lv_label_create(top_page);
    lv_label_set_text(title_label_, GetParsedString("Set"));
    lv_obj_align(title_label_, LV_ALIGN_CENTER, 0, size_h(3));

    lv_obj_t* menu_list_ = lv_create_page(video_set_page_, screen_width, size_h(298),
        lv_color_make(16, 16, 16), 0, 0, lv_font_all, lv_color_white(), 0);
    lv_obj_align(menu_list_, LV_ALIGN_TOP_MID, size_w(0), size_h(62));
    lv_obj_add_flag(menu_list_, LV_OBJ_FLAG_SCROLLABLE);

	std::string text[PlaybackSubpage_Total] = { GetParsedString("Delete current"),GetParsedString("Delete all"),
     GetParsedString("Lock current"), GetParsedString("Unlock current"),
	 GetParsedString("Lock all"), GetParsedString("Unlock all"),
	 /*GetParsedString("Main interface"), GetParsedString("Return")*/ };
	for (int i = 0;i < PlaybackSubpage_Total;i++) {
        set_btn_[i] = lv_create_btn(menu_list_, screen_width, 58, lv_color_make(68, 68, 68), 2,
            OpenSubpage, LV_EVENT_ALL, (void*)i, 0);
        lv_obj_remove_style(set_btn_[i], NULL, LV_STATE_FOCUS_KEY);
        if (i == PlaybackSubpage_DelCur) {
            lv_obj_align(set_btn_[i], LV_ALIGN_TOP_LEFT, size_w(0), size_h(0));
        }
        else {
            lv_obj_align_to(set_btn_[i], set_btn_[i - 1], LV_ALIGN_OUT_BOTTOM_LEFT, size_w(0), size_h(2));
        }

		lv_obj_add_flag(set_btn_[i], LV_OBJ_FLAG_CLICKABLE);
		lv_group_add_obj(GlobalData::Instance()->group, set_btn_[i]);

		lv_obj_t* label = lv_create_label(set_btn_[i], size_w(screen_width), text[i].c_str(), LV_ALIGN_TOP_LEFT, 0);
		lv_obj_align(label, LV_ALIGN_TOP_LEFT, size_w(20), size_h(3));
	}

     SelectFileStyle(menu_list_, screen_width, size_h(298));
	lv_group_focus_next(GlobalData::Instance()->group);
#if 1
	GlobalPage::Instance()->page_set()->Createfunction_bar(video_set_page_,35,7,true,false);
#endif
}

void PagePlayback::DelSetPageEvent(lv_event_t* e)
{
	PagePlayback* page_playback = GlobalPage::Instance()->page_playback();
	for (int i = 0;i < PlaybackSubpage_Total;i++) {
		lv_obj_remove_event_cb(page_playback->set_btn_[i], page_playback->OpenSubpage);
	}
}

void PagePlayback::OpenSubpage(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		PagePlayback* page_playback = GlobalPage::Instance()->page_playback();
		int user_data = (int)lv_event_get_user_data(e);
		if (user_data == PlaybackSubpage_Main) {
			GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape);
		}
		else if (user_data == PlaybackSubpage_Return) {
			if (page_playback->video_count_ == 0) {
				//GlobalPage::Instance()->page_main()->OpenTipBox("No video files");
				GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape, true);
			}
			else {
				lv_obj_del(page_playback->video_set_page_);
				page_playback->video_set_page_ = NULL;
				lv_obj_clear_flag(page_playback->playback_page_, LV_OBJ_FLAG_HIDDEN);
			}
		}
		else {
			page_playback->foucused_obj_[0] = lv_group_get_focused(GlobalData::Instance()->group);
			
			for (int i = 0;i < PlaybackSubpage_Total;i++) {
				lv_group_remove_obj(page_playback->set_btn_[i]);
			}
			
			page_playback->OpenConfirmPage(user_data);
			lv_group_focus_next(GlobalData::Instance()->group);
			//lv_event_send(page_playback->set_btn_[PlaybackSubpage_Return], LV_EVENT_DEFOCUSED, (void*)PlaybackSubpage_Return);
			lv_event_send(page_playback->set_btn_[PlaybackSubpage_UnlockAll], LV_EVENT_DEFOCUSED, (void*)PlaybackSubpage_UnlockAll);
		}
	}
	else if (code == LV_EVENT_FOCUSED) {
		PagePlayback* page_playback = GlobalPage::Instance()->page_playback();
		if (page_playback->video_set_page_ == NULL)
			return;
		
		int user_data = (int)lv_event_get_user_data(e);

		lv_obj_add_flag(page_playback->video_set_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(page_playback->video_set_page_, LV_OBJ_FLAG_HIDDEN);

		// char path[128] = { 0 };
		// strncpy(path, (char*)lv_img_get_src(page_playback->set_btn_[user_data]), sizeof(path) - 1);
		// int path_size = strlen(path);
		// if (strncmp(&path[path_size - 5], "-.png", 5) == 0)
		// 	return;

		// memset(&path[path_size - 4], 0, 4);
		// strncpy(&path[path_size - 4], "-.png", 5);
		// XMLogI("LV_EVENT_FOCUSED--path:%s", path);
		// lv_img_set_src(page_playback->set_btn_[user_data], path);

		lv_obj_add_flag(page_playback->video_set_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(page_playback->video_set_page_, LV_OBJ_FLAG_HIDDEN);
	}
	else if (code == LV_EVENT_DEFOCUSED) {
		PagePlayback* page_playback = GlobalPage::Instance()->page_playback();
		if (page_playback->video_set_page_ == NULL)
			return;
		
		int user_data = (int)lv_event_get_user_data(e);

		// char path[128] = { 0 };
		// strncpy(path, (char*)lv_img_get_src(page_playback->set_btn_[user_data]), sizeof(path) - 1);
		// int path_size = strlen(path);
		// if (strncmp(&path[path_size - 5], "-.png", 5) != 0)
		// 	return;

		// memset(&path[path_size - 5], 0, 5);
		// strncpy(&path[path_size - 5], ".png", 4);
		// XMLogI("LV_EVENT_DEFOCUSED--path:%s", path);
		// lv_img_set_src(page_playback->set_btn_[user_data], path);
	}
}

void PagePlayback::DeletedEvent(lv_event_t* e)
{
	for (int i = 0;i < PlaybackSubpage_Total;i++) {
		lv_group_add_obj(GlobalData::Instance()->group, GlobalPage::Instance()->page_playback()->set_btn_[i]);
	}
	lv_group_focus_obj(GlobalPage::Instance()->page_playback()->foucused_obj_[0]);
}

void PagePlayback::OpenConfirmPage(int flag)
{
	lv_obj_t* page = lv_create_page(lv_scr_act(), screen_width, screen_height, lv_color_black(), 0, 0,
		lv_font_all, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(page, 0, 0);
	lv_obj_set_pos(page, 0, start_y);
	lv_obj_add_event_cb(page, DeletedEvent, LV_EVENT_DELETE, NULL);

	lv_obj_t* tip_win = lv_create_page(page, screen_width, 298, lv_palette_darken(LV_PALETTE_GREY, 2), 5, 2,
		lv_font_all, lv_color_white(), 0);
	lv_obj_align(tip_win, LV_ALIGN_CENTER, 0, size_h(30));

	std::string text[PlaybackSubpage_Total] = { GetParsedString("Delete current"), GetParsedString("Delete all"),
    GetParsedString("Lock current"), GetParsedString("Unlock current"),
    GetParsedString("Lock all"), GetParsedString("Unlock all") };
	lv_obj_t* label = lv_create_label(tip_win, screen_width-100, text[flag].c_str(), LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, size_h(40));

	lv_obj_t* yes_btn = lv_create_btn(tip_win, size_w(240), size_h(60),
        lv_color_make(85, 85, 85), 10, ManageFileEvent, LV_EVENT_ALL, (void*)flag, 0);
	/*lv_obj_set_style_border_width(yes_btn, 2, 0);
	lv_obj_set_style_border_color(yes_btn, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), 0);*/
	lv_obj_align(yes_btn, LV_ALIGN_TOP_MID, size_w(0), size_h(80));

	lv_obj_t* btn_label = lv_label_create(yes_btn);
	lv_label_set_text(btn_label, GetParsedString("Confirm"));
	lv_obj_center(btn_label);

	lv_obj_t* no_btn = lv_create_btn(tip_win, size_w(240), size_h(60),
		lv_color_make(85, 85,85), 10, ManageFileEvent, LV_EVENT_ALL, (void*)-1, 0);
	/*lv_obj_set_style_border_width(no_btn, 2, 0);
	lv_obj_set_style_border_color(no_btn, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), 0);*/
	lv_obj_align(no_btn, LV_ALIGN_TOP_MID, size_w(0), size_h(180));

	btn_label = lv_label_create(no_btn);
	lv_label_set_text(btn_label, GetParsedString("Cancel"));
	lv_obj_center(btn_label);

	GlobalData::Instance()->opened_subpage_[0] = page;
}

void PagePlayback::ManageFileEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		PagePlayback* page_playback = GlobalPage::Instance()->page_playback();

		lv_obj_add_flag(page_playback->video_set_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(page_playback->video_set_page_, LV_OBJ_FLAG_HIDDEN);
		
		int user_data = (int)lv_event_get_user_data(e);
		if (user_data == PlaybackSubpage_LockCur || user_data == PlaybackSubpage_UnlockCur
			|| user_data == PlaybackSubpage_LockAll || user_data == PlaybackSubpage_UnlockAll) {
			XMUIEventInParam event_inparam;
			XMUIEventOutParam event_outparam;
			if (page_playback->video_count_ == 0) {
				if (GlobalData::Instance()->opened_subpage_[0]) {
					lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
					GlobalData::Instance()->opened_subpage_[0] = NULL;
				}
				GlobalPage::Instance()->page_main()->OpenTipBox("No video files");
				return;
			}

			if (user_data == PlaybackSubpage_LockCur) {
				event_inparam.vod_lockhistoryfile.file_param.sdc_num = XM_STORAGE_SDCard_0;
				event_inparam.vod_lockhistoryfile.file_param.channel = page_playback->playing_file_info_.direction;
				event_inparam.vod_lockhistoryfile.file_param.type = XM_Record_Video;
				strncpy(event_inparam.vod_lockhistoryfile.file_param.file_name,
					page_playback->playing_file_info_.file_name, XM_MAX_STORAGE_FILE_LEN - 1);
				GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_LOCK_HISTORY_FILE, &event_inparam, &event_outparam);
				GlobalPage::Instance()->page_main()->OpenTipBox("Successfully locked", 2000);
			}
			else if (user_data == PlaybackSubpage_UnlockCur) {
				event_inparam.vod_unlockhistoryfile.file_param.sdc_num = XM_STORAGE_SDCard_0;
				event_inparam.vod_unlockhistoryfile.file_param.channel = page_playback->playing_file_info_.direction;
				event_inparam.vod_unlockhistoryfile.file_param.type = XM_Record_Video;
				strncpy(event_inparam.vod_unlockhistoryfile.file_param.file_name,
					page_playback->playing_file_info_.file_name, XM_MAX_STORAGE_FILE_LEN - 1);
				GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_UNLOCK_HISTORY_FILE, &event_inparam, &event_outparam);
				GlobalPage::Instance()->page_main()->OpenTipBox("Unlock successfully", 2000);
			}
			else if (user_data == PlaybackSubpage_LockAll) {
				if (page_playback->file_type_ == FileType_FrontVideo) {
					XM_Middleware_Storage_LockAllHistoryFile(XM_STORAGE_SDCard_0, Direction_Front, XM_Record_Video);
				}
				else if (page_playback->file_type_ == FileType_BehindVideo) {
					XM_Middleware_Storage_LockAllHistoryFile(XM_STORAGE_SDCard_0, Direction_Behind, XM_Record_Video);
				}

				GlobalPage::Instance()->page_main()->OpenTipBox("Successfully locked", 2000);
			}
			else if (user_data == PlaybackSubpage_UnlockAll) {
				if (page_playback->file_type_ == FileType_FrontLockedVideo) {
					XM_Middleware_Storage_UnLockAllHistoryFile(XM_STORAGE_SDCard_0, Direction_Front, XM_Record_Video);
				}
				else if (page_playback->file_type_ == FileType_BehindLockedVideo) {
					XM_Middleware_Storage_UnLockAllHistoryFile(XM_STORAGE_SDCard_0, Direction_Behind, XM_Record_Video);
				}

				GlobalPage::Instance()->page_main()->OpenTipBox("Unlock successfully", 2000);
			}

			page_playback->SearchFile(page_playback->file_type_);
			if (page_playback->video_count_ == 0) {
				
			}
			else {
				page_playback->SwitchFile();
			}
		}
		else if (user_data == PlaybackSubpage_DelCur) {
			page_playback->DeleteVideoFile();
		}
		else if (user_data == PlaybackSubpage_DelAll) {
			page_playback->DeleteAllVideoFile();
		}

		if (GlobalData::Instance()->opened_subpage_[0]) {
			lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
			GlobalData::Instance()->opened_subpage_[0] = NULL;
		}
	}
}

void PagePlayback::DeleteVideoFile()
{
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	if (video_count_ == 0) {
		GlobalPage::Instance()->page_main()->OpenTipBox("No video files");
		return;
	}

	//判断文件是否加锁
	if (playing_file_info_.file_locked) {
		GlobalPage::Instance()->page_main()->OpenTipBox("File locked");
		return;
	}

	event_inparam.storage_del_file.file_param.type = XM_Record_Video;
	event_inparam.storage_del_file.file_param.channel = playing_file_info_.direction;
	event_inparam.storage_del_file.file_param.sdc_num = XM_STORAGE_SDCard_0;
	strncpy(event_inparam.storage_del_file.file_param.file_name, playing_file_info_.file_name, XM_MAX_STORAGE_FILE_LEN - 1);
	GlobalData::Instance()->ui_event_cb_(XM_UI_STORAGE_DEL_FILE, &event_inparam, &event_outparam);
	GlobalPage::Instance()->page_main()->OpenTipBox("Successfully deleted");

	SearchFile(file_type_);
	if (video_count_ == 0) {
		
	}
	else {
		playing_file_info_.file_num -= 1;
		if (playing_file_info_.file_num < 0) {
			playing_file_info_.file_num = video_count_ - 1;
		}

		SwitchFile();
	}
}

void PagePlayback::DeleteAllVideoFile()
{
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	if (video_count_ == 0) {
		GlobalPage::Instance()->page_main()->OpenTipBox("No video files");
		return;
	}

	//删除所有文件
	int ret = 0;
	if (file_type_ == FileType_FrontVideo || file_type_ == FileType_FrontLockedVideo) {
		if (file_type_ == FileType_FrontVideo) {
			ret = XM_Middleware_Storage_DelAllFile(XM_STORAGE_SDCard_0, Direction_Front, XM_Record_Video);
			ret = 0;
		}
		else {
			ret = 1;
		}
	}
	else if (file_type_ == FileType_BehindVideo || file_type_ == FileType_BehindLockedVideo) {
		if (file_type_ == FileType_BehindVideo) {
			ret = XM_Middleware_Storage_DelAllFile(XM_STORAGE_SDCard_0, Direction_Behind, XM_Record_Video);
			ret = 0;
		}
		else {
			ret = 1;
		}
	}

	if (ret) {
		GlobalPage::Instance()->page_main()->OpenTipBox("File locked");
	}
	else {
		GlobalPage::Instance()->page_main()->OpenTipBox("Successfully deleted");
	}

	SearchFile(file_type_);
	if (video_count_ == 0) {

	}
	else {
		playing_file_info_.file_num = 0;
		SwitchFile();
	}
}

void PagePlayback::ChangeProgressEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_VALUE_CHANGED) {
		PagePlayback* page_playback = GlobalPage::Instance()->page_playback();
		int slider_value = lv_slider_get_value(page_playback->play_slider_);
		lv_label_set_text_fmt(page_playback->video_time_label_, "%02d:%02d", slider_value / 60, slider_value % 60);
		
		XMUIEventInParam event_inparam;
		XMUIEventOutParam event_outparam;
		event_inparam.vod_seek.handle = page_playback->playing_file_info_.play_handle;
		event_inparam.vod_seek.seek_time = slider_value * 1000;
		GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_SEEK, &event_inparam, &event_outparam);
	}
}

void PagePlayback::VideoTimer(lv_timer_t* timer)
{
	PagePlayback* object = GlobalPage::Instance()->page_playback();
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	event_inparam.vod_getcurduration.handle = object->playing_file_info_.play_handle;
	GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_GET_CURRENT_DURATION, &event_inparam, &event_outparam);
	int video_current_time = event_outparam.vod_getcurduration.cur_duration / 1000;
	object->playing_file_info_.video_current_time = video_current_time;

	int slider_value = lv_slider_get_value(object->play_slider_);
	bool flag = true;
	if (video_current_time >= object->playing_file_info_.video_duration && slider_value >= object->playing_file_info_.video_duration) {
		flag = false;
	}
	
	if (flag)
	{
		if (object->video_time_label_) {
			lv_label_set_text_fmt(object->video_time_label_, "%02d:%02d", video_current_time / 60,
				video_current_time % 60);
		}

		lv_slider_set_value(object->play_slider_, video_current_time, LV_ANIM_OFF);
	}
	else
	{
		if (object->playing_file_info_.play_handle != -1) {
			event_inparam.vod_stop.handle = object->playing_file_info_.play_handle;
			GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_STOP, &event_inparam, &event_outparam);
			object->playing_file_info_.play_handle = -1;
		}

		object->playing_file_ = false;
		lv_timer_pause(object->video_timer_);
		lv_img_set_src(object->playback_btn_[PlayBackBtn_Play], image_path"stop.png");
		GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
		lv_obj_add_flag(object->playback_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(object->playback_page_, LV_OBJ_FLAG_HIDDEN);
		XMLogW("%s play over!", object->playing_file_info_.file_name);
	}
}

int PagePlayback::SearchFile(int file_type)
{
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	event_inparam.query_record.rec_query.channel = -1;
	event_inparam.query_record.rec_query.sdc_num = XM_STORAGE_SDCard_0;
	event_inparam.query_record.rec_query.type = XM_Record_Video;
	event_inparam.query_record.rec_query.start_time = 0;
	event_inparam.query_record.rec_query.end_time = 0xffffffff;
	GlobalData::Instance()->ui_event_cb_(XM_UI_QUERY_RECORD, &event_inparam, &event_outparam);
	file_list_ = event_outparam.query_record.rec_list;
	std::string flag_str;
	int channel = 0;
	if (file_type == FileType_FrontVideo) {
		flag_str = "REC";
		channel = 0;
	}
	else if (file_type == FileType_FrontLockedVideo) {
		flag_str = "SOS";
		channel = 0;
	}
	else if (file_type == FileType_BehindVideo) {
		flag_str = "REC";
		channel = 1;
	}
	else if (file_type == FileType_BehindLockedVideo) {
		flag_str = "SOS";
		channel = 1;
	}
	else {
		return -1;
	}

	file_type_ = file_type;

	if (!file_index_map_.empty()) {
		file_index_map_.clear();
	}

	int j = 0;
	for (int i = 0;i < file_list_.record_count;i++) {
		if (file_list_.records[i].channel == channel && strstr(file_list_.records[i].file_name, flag_str.c_str())) {
			file_index_map_[j] = i;
			j++;
		}
	}
	video_count_ = j;
	/*for (int i = 0; i < file_list_.record_count; i++) {
		XMLogI("name = %s", file_list_.records[i].file_name);
	}*/
	return video_count_;
}

void PagePlayback::PlayVideo()
{
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	if (playing_file_info_.play_handle != -1) {
		event_inparam.vod_stop.handle = playing_file_info_.play_handle;
		GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_STOP, &event_inparam, &event_outparam);
		playing_file_info_.play_handle = -1;
	}

	//更新当前文件信息
	int file_num = file_index_map_[playing_file_info_.file_num];
	playing_file_info_.direction = file_list_.records[file_num].channel;
	playing_file_info_.file_locked = file_list_.records[file_num].video_info.file_locked;
	memset(playing_file_info_.file_name, 0, sizeof(playing_file_info_.file_name));
	memset(playing_file_info_.file_path, 0, sizeof(playing_file_info_.file_path));
	strcpy(playing_file_info_.file_name, file_list_.records[file_num].file_name);
	sprintf(playing_file_info_.file_path, "/mnt/tfcard/record/record_%d/%s",
		file_list_.records[file_num].channel,
		file_list_.records[file_num].file_name);
	
	event_inparam.vod_start.channel = playing_file_info_.direction;
	event_inparam.vod_start.sdcard_num = XM_STORAGE_SDCard_0;
	strncpy(event_inparam.vod_start.file_full_name, playing_file_info_.file_path, XM_MAX_STORAGE_PATH_LEN - 1);
	event_inparam.vod_start.file_format = XM_FILE_FORMAT_AVI;
	XMLogI("playing_file_info_ file_path:%s, file_len=%d", event_inparam.vod_start.file_full_name,
		file_list_.records[file_num].file_len);
	GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_OPEN, &event_inparam, &event_outparam);
	playing_file_info_.play_handle = event_outparam.vod_start.handle;
	if (playing_file_info_.play_handle == 0) {
		playing_file_ = false;
		play_preview_ = false;
		if (video_timer_) {
			lv_timer_pause(video_timer_);
			lv_img_set_src(playback_btn_[PlayBackBtn_Play], image_path"stop.png");
			lv_obj_add_flag(playback_page_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(playback_page_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_set_style_bg_opa(playback_page_, LV_OPA_COVER, 0);
		}
		playing_file_info_.play_handle = -1;
		GlobalPage::Instance()->page_main()->OpenTipBox("Play error");
		GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
		return;
	}

	if (playback_page_) {
		int bg_opa = lv_obj_get_style_opa(playback_page_, 0);
		if (bg_opa != 0) {
			lv_obj_set_style_bg_opa(playback_page_, 0, 0);
		}
	}

	event_inparam.vod_getfileinfo.handle = playing_file_info_.play_handle;
	GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_GET_FILE_INFO, &event_inparam, &event_outparam);
	playing_file_info_.video_duration = event_outparam.vod_getfileinfo.duration / 1000;
	XMLogW("video_duration = %d", playing_file_info_.video_duration);
	playing_file_info_.width = event_outparam.vod_getfileinfo.media_info.width;
	playing_file_info_.height = event_outparam.vod_getfileinfo.media_info.height;

	event_inparam.vod_play.handle = playing_file_info_.play_handle;
	event_inparam.vod_play.play_one = play_preview_;
	event_inparam.vod_play.dst_rc = GlobalData::Instance()->dst_rc;
	GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_PLAY, &event_inparam, &event_outparam);
	play_preview_ = false;
}

void PagePlayback::SwitchFile()
{
	VideoPreview();

	lv_label_set_text(video_label_, playing_file_info_.file_name);
	if(playing_file_info_.height == k4KHeight)
		lv_label_set_text(resolution_label_, "4K");
	else if(playing_file_info_.height == k3KHeight)
		lv_label_set_text(resolution_label_, "3K");
	else if(playing_file_info_.height == 1440)		
		lv_label_set_text(resolution_label_, "2K");
	else
		lv_label_set_text_fmt(resolution_label_, "%dP", playing_file_info_.height);

	int max_range = playing_file_info_.video_duration > 0 ? playing_file_info_.video_duration : 1;
	lv_slider_set_range(play_slider_, 0, max_range);
	lv_slider_set_value(play_slider_, 0, LV_ANIM_OFF);
	lv_label_set_text(video_time_label_, "00:00");
	lv_label_set_text_fmt(total_duration_label_, "%02d:%02d", playing_file_info_.video_duration / 60, playing_file_info_.video_duration % 60);
	if (playing_file_info_.file_locked) {
		lv_obj_clear_flag(lock_label_, LV_OBJ_FLAG_HIDDEN);
	}
	else {
		lv_obj_add_flag(lock_label_, LV_OBJ_FLAG_HIDDEN);
	}
}

void PagePlayback::VideoPreview()
{
	play_preview_ = true;
	PlayVideo();
}

void PagePlayback::PauseVideo(bool pause_video)
{
	if (playing_file_info_.play_handle != -1) {
		XMUIEventInParam event_inparam;
		XMUIEventOutParam event_outparam;
		event_inparam.vod_pause.handle = playing_file_info_.play_handle;
		event_inparam.vod_pause.pause = pause_video;
		GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_PAUSE, &event_inparam, &event_outparam);
	}
}

void PagePlayback::SetPlaybackWinSize(int x, int y, int width, int height)
{
	GlobalData::Instance()->dst_rc.s32X = x;
	GlobalData::Instance()->dst_rc.s32Y = y;
	GlobalData::Instance()->dst_rc.u32Width = width;
	GlobalData::Instance()->dst_rc.u32Height = height;
}

void PagePlayback::PlaybackKeypadEvent(int key)
{
	if(OpenPageTimer_handle){
      return;
	}
	if (key == KEYMAP_MODE) {
		if (!lv_obj_has_flag(playback_page_, LV_OBJ_FLAG_HIDDEN) || video_count_ == 0) {
			GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape, true);
			return;
		}
	}

	if (key == KEYMAP_UP) {
		playing_file_ = false;
		lv_timer_pause(video_timer_);
		lv_img_set_src(playback_btn_[PlayBackBtn_Play], image_path"stop.png");
		GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
		
		playing_file_info_.file_num += 1;
		if (playing_file_info_.file_num > video_count_ - 1)
			playing_file_info_.file_num = 0;

		SwitchFile();
	}
	else if (key == KEYMAP_DOWN) {
		playing_file_ = false;
		lv_timer_pause(video_timer_);
		lv_img_set_src(playback_btn_[PlayBackBtn_Play], image_path"stop.png");
		GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
		
		playing_file_info_.file_num -= 1;
		if (playing_file_info_.file_num < 0)
			playing_file_info_.file_num = video_count_ - 1;

		SwitchFile();
	}

	if (!playing_file_) {
		if (key == KEYMAP_OK) {
			playing_file_ = true;
			play_preview_ = false;
			lv_img_set_src(playback_btn_[PlayBackBtn_Play], image_path"play.png");
			lv_timer_resume(video_timer_);
			GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(false);
			if (playing_file_info_.play_handle == -1) {
				PlayVideo();
			}
			else {
				PauseVideo(false);
			}
		}
		else if (key == KEYMAP_MENU || key == KEYMAP_MODE) {
			if (GlobalData::Instance()->opened_subpage_[1]) {
				lv_obj_del(GlobalData::Instance()->opened_subpage_[1]);
				GlobalData::Instance()->opened_subpage_[1] = NULL;
			}
			else if (GlobalData::Instance()->opened_subpage_[0]) {
				lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
				GlobalData::Instance()->opened_subpage_[0] = NULL;
			}
			else if (video_set_page_ == NULL) {
				lv_obj_add_flag(playback_page_, LV_OBJ_FLAG_HIDDEN);
				OpenSetPage();
			}
			else {
				if (video_count_ == 0) {
					GlobalPage::Instance()->page_main()->OpenTipBox("No video files");
				}
				else {
					lv_obj_del(video_set_page_);
					video_set_page_ = NULL;
					lv_obj_clear_flag(playback_page_, LV_OBJ_FLAG_HIDDEN);
				}
			}
		}
	}
	else {
		if (key == KEYMAP_OK) {
			playing_file_ = false;
			lv_timer_pause(video_timer_);
			PauseVideo(true);
			lv_img_set_src(playback_btn_[PlayBackBtn_Play], image_path"stop.png");
			GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
		}
	}

	if (key == KEYMAP_UP || key == KEYMAP_DOWN || key == KEYMAP_OK) {
		lv_obj_add_flag(playback_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(playback_page_, LV_OBJ_FLAG_HIDDEN);
	}
}

int PagePlayback::PlaybackPrepareWork(bool open_page)
{
	if (open_page) {
		if (video_count_ == 0) {
			OpenSetPage();
			GlobalPage::Instance()->page_main()->OpenTipBox("No video files");
		}
		else {
			playing_file_info_.file_num = video_count_ - 1;
			VideoPreview();
			//OpenPage();
			 OpenPageTimer_handle =lv_timer_create(OpenPageTimer, 200, NULL);
			close_preview_ = true;
		}
	}
	else {
		if (GlobalData::Instance()->opened_subpage_[1]) {
			lv_obj_del(GlobalData::Instance()->opened_subpage_[1]);
			GlobalData::Instance()->opened_subpage_[1] = NULL;
		}

		if (GlobalData::Instance()->opened_subpage_[0]) {
			lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
			GlobalData::Instance()->opened_subpage_[0] = NULL;
		}

		if (video_set_page_) {
			lv_obj_del(video_set_page_);
			video_set_page_ = NULL;
		}

		if (video_timer_) {
			lv_timer_del(video_timer_);
			video_timer_ = NULL;
		}
		
		if (playing_file_info_.play_handle != -1) {
			XMUIEventInParam event_inparam;
			XMUIEventOutParam event_outparam;
			event_inparam.vod_stop.handle = playing_file_info_.play_handle;
			GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_STOP, &event_inparam, &event_outparam);
			playing_file_info_.play_handle = -1;
		}

		if (playback_page_) {
			lv_obj_del(playback_page_);
			playback_page_ = NULL;
		}
		
		playing_file_ = false;
		video_count_ = 0;
	}
	
	return 0;
}

void PagePlayback::UnplugSDCard()
{
	GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape);
}

void PagePlayback::OpenSelectFilePage()
{
	select_filetype_page_ = lv_create_page(lv_scr_act(), screen_width, screen_height, lv_color_make(16, 16, 16), 0, 0,
		lv_font_all, lv_color_white(), 0);
	lv_obj_set_pos(select_filetype_page_, 0, start_y);
	lv_obj_add_event_cb(select_filetype_page_, DelSelectFilePageEvent, LV_EVENT_DELETE, NULL);

    lv_obj_t* top_page = lv_create_page(select_filetype_page_, screen_width, size_h(60),
        lv_color_make(68, 68, 68), 0, 0, lv_font_all, lv_color_white(), 0);
    lv_obj_align(top_page, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* title_label_ = lv_label_create(top_page);
    lv_label_set_text(title_label_, GetParsedString("File management"));
    lv_obj_align(title_label_, LV_ALIGN_CENTER, 0, size_h(3));

    lv_obj_t *filetype_list_ = lv_create_page(select_filetype_page_, screen_width, size_h(298),
        lv_color_make(16, 16, 16), 0, 0, lv_font_all, lv_color_white(), 0);
    lv_obj_align(filetype_list_, LV_ALIGN_TOP_MID, size_w(0), size_h(62));
    lv_obj_add_flag(filetype_list_, LV_OBJ_FLAG_SCROLLABLE);

	std::string text[FileType_Total] = { GetParsedString("Front ordinary video recording"), GetParsedString("Front lock recording"),
	GetParsedString("Front road photos"), GetParsedString("Rear ordinary recording"), GetParsedString("Rear locking recording"),
	GetParsedString("Posterior photos")/*, GetParsedString("Return")*/ };
	lv_obj_t* label = NULL;
	for (int i = 0;i < FileType_Total;i++) {
        filetype_img_[i] = lv_create_btn(filetype_list_, screen_width, 58, lv_color_make(68, 68, 68), 2,
            SelectFileTypeEvent, LV_EVENT_ALL, (void*)i, 0);
        lv_obj_remove_style(filetype_img_[i], NULL, LV_STATE_FOCUS_KEY);
		if (i == FileType_FrontVideo) {
			lv_obj_align(filetype_img_[i], LV_ALIGN_TOP_LEFT, size_w(0), size_h(0));
		}
		else {
			lv_obj_align_to(filetype_img_[i], filetype_img_[i - 1], LV_ALIGN_OUT_BOTTOM_LEFT, size_w(0), size_h(2));
		}

		lv_obj_add_flag(filetype_img_[i], LV_OBJ_FLAG_CLICKABLE);
		lv_group_add_obj(GlobalData::Instance()->group, filetype_img_[i]);

		label = lv_create_label(filetype_img_[i], size_w(screen_width), text[i].c_str(), LV_ALIGN_TOP_LEFT, 0);
        lv_obj_align(label, LV_ALIGN_TOP_LEFT, size_h(20), size_h(3));
	}
    SelectFileStyle(filetype_list_, screen_width, size_h(298));
#if 1
    GlobalPage::Instance()->page_set()->Createfunction_bar(select_filetype_page_,36,7,true,false);
#endif
}

void PagePlayback::DelSelectFilePageEvent(lv_event_t* e)
{
	PagePlayback* page_playback = GlobalPage::Instance()->page_playback();
	for (int i = 0;i < FileType_Total;i++) {
		lv_obj_remove_event_cb(page_playback->filetype_img_[i], page_playback->SelectFileTypeEvent);
	}

	GlobalPage::Instance()->page_main()->LeaveMainPageWork(false);
}

void PagePlayback::SelectFileTypeEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		PagePlayback* page_playback = GlobalPage::Instance()->page_playback();
		int user_data = (int)lv_event_get_user_data(e);
		if (user_data == FileType_Return) {
			if (GlobalData::Instance()->UI_mode_ == UIMode_Photograph) {
				lv_obj_clear_flag(GlobalPage::Instance()->page_main()->main_page_, LV_OBJ_FLAG_HIDDEN);
				GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape);
			}
			
			lv_obj_del(page_playback->select_filetype_page_);
			page_playback->select_filetype_page_ = NULL;
		}
		else if (user_data == FileType_FrontVideo || user_data == FileType_FrontLockedVideo
			|| user_data == FileType_BehindVideo || user_data == FileType_BehindLockedVideo) {
			int file_count = page_playback->SearchFile(user_data);
			XMLogI("video file count = %d", file_count);
			if (file_count > 0) {
				//lv_obj_add_flag(page_playback->select_filetype_page_, LV_OBJ_FLAG_HIDDEN);
				GlobalData::Instance()->UI_mode_ = UIMode_Playback;
				GlobalPage::Instance()->page_sys_set()->OpenRestScreenFun(false);
				page_playback->PlaybackPrepareWork(true);
				lv_timer_create(HiddenTopPageTimer, 200, NULL);
			}
			else {
				GlobalPage::Instance()->page_main()->OpenTipBox("No files");
			}
		}
		else if (user_data == FileType_FrontPhoto || user_data == FileType_BehindPhoto) {
			int file_count = GlobalPage::Instance()->page_play_pic()->SearchFile(user_data);
			XMLogI("pic file count = %d", file_count);
			if (file_count > 0) {
				lv_obj_add_flag(page_playback->select_filetype_page_, LV_OBJ_FLAG_HIDDEN);
				GlobalData::Instance()->UI_mode_ = UIMode_Picture;
				GlobalPage::Instance()->page_sys_set()->OpenRestScreenFun(false);
				GlobalPage::Instance()->page_play_pic()->PlayPicPrepareWork(true);
			}
			else {
				GlobalPage::Instance()->page_main()->OpenTipBox("No files");
			}
		}
	}
	else if (code == LV_EVENT_FOCUSED) {
		PagePlayback* page_playback = GlobalPage::Instance()->page_playback();
		int user_data = (int)lv_event_get_user_data(e);

		lv_obj_add_flag(page_playback->select_filetype_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(page_playback->select_filetype_page_, LV_OBJ_FLAG_HIDDEN);

		// char path[128] = { 0 };
		// strncpy(path, (char*)lv_img_get_src(page_playback->filetype_img_[user_data]), sizeof(path) - 1);
		// int path_size = strlen(path);
		// if (strncmp(&path[path_size - 5], "-.png", 5) == 0)
		// 	return;
		
		// memset(&path[path_size - 4], 0, 4);
		// strncpy(&path[path_size - 4], "-.png", 5);
		// XMLogI("LV_EVENT_FOCUSED--path:%s", path);
		// lv_img_set_src(page_playback->filetype_img_[user_data], path);

		lv_obj_add_flag(page_playback->select_filetype_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(page_playback->select_filetype_page_, LV_OBJ_FLAG_HIDDEN);
	}
	else if (code == LV_EVENT_DEFOCUSED) {
		PagePlayback* page_playback = GlobalPage::Instance()->page_playback();
		int user_data = (int)lv_event_get_user_data(e);

		// char path[128] = { 0 };
		// strncpy(path, (char*)lv_img_get_src(page_playback->filetype_img_[user_data]), sizeof(path) - 1);
		// int path_size = strlen(path);
		// if (strncmp(&path[path_size - 5], "-.png", 5) != 0)
		// 	return;
		
		// memset(&path[path_size - 5], 0, 5);
		// strncpy(&path[path_size - 5], ".png", 4);
		// XMLogI("LV_EVENT_DEFOCUSED--path:%s", path);
		// lv_img_set_src(page_playback->filetype_img_[user_data], path);
	}
}
void PagePlayback::HiddenTopPageTimer(lv_timer_t* timer)
{
	lv_obj_add_flag(GlobalPage::Instance()->page_playback()->select_filetype_page_, LV_OBJ_FLAG_HIDDEN);
	lv_timer_del(timer);
}

void PagePlayback::OpenPageTimer(lv_timer_t* timer)
{
	GlobalPage::Instance()->page_playback()->OpenPage();
	lv_timer_del(timer);
	GlobalPage::Instance()->page_playback()->OpenPageTimer_handle=NULL;
}