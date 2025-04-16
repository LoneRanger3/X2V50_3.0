#include "global_data.h"
#include "global_page.h"
#include "PagePlayPic.h"

PagePlayPic::PagePlayPic():play_pic_page_(NULL), set_page_(NULL), playing_file_(false), pic_count_(0)
{
	playing_file_info_.direction = Direction_Front;
	playing_file_info_.file_len = 0;
	playing_file_info_.file_locked = false;
	playing_file_info_.file_num = 0;
	playing_file_info_.height = 0;
	playing_file_info_.width = 0;
}

PagePlayPic::~PagePlayPic()
{

}

void PagePlayPic::OpenPage()
{
	CreatePage();
	GlobalPage::Instance()->page_sys_set()->OpenAutoShutdownFun(true);
}

void PagePlayPic::CreatePage()
{
	//回放页面
	play_pic_page_ = lv_create_page(lv_scr_act(), screen_width, screen_height, lv_color_black(), 0, 0,
		&lv_font_montserrat_16, lv_color_white(), 0);
	lv_obj_set_style_bg_opa(play_pic_page_, 0, 0);
	lv_obj_set_pos(play_pic_page_, 0, start_y);

	//文件名
	name_label_ = lv_label_create(play_pic_page_);
	//lv_obj_set_style_text_font(name_label_, &lv_font_montserrat_26, LV_PART_MAIN);
	lv_label_set_text(name_label_, playing_file_info_.file_name);
	lv_obj_align(name_label_, LV_ALIGN_TOP_RIGHT, -size_w(10), size_h(10));

	//文件分辨率
	resolution_label_ = lv_label_create(play_pic_page_);
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
	lock_label_ = lv_label_create(play_pic_page_);
	lv_label_set_text(lock_label_, "\uF084");
	lv_obj_align_to(lock_label_, resolution_label_, LV_ALIGN_OUT_RIGHT_MID, size_w(20), 0);
	if (!playing_file_info_.file_locked) {
		lv_obj_add_flag(lock_label_, LV_OBJ_FLAG_HIDDEN);
	}

	std::string img_path[PlayPicBtn_Total] = { image_path"return_rec.png", image_path"return_pre.png", image_path"pre.png",
	image_path"next.png", image_path"menu.png" };
	for (int i = 0;i < PlayPicBtn_Total;i++) {
		playpic_btn_[i] = lv_img_create(play_pic_page_);
		lv_img_set_src(playpic_btn_[i], img_path[i].c_str());
		lv_obj_set_style_img_recolor_opa(playpic_btn_[i], LV_OPA_30, LV_STATE_PRESSED);

		if (i == PlayBackBtn_ReturnRec) {
			lv_obj_align(playpic_btn_[i], LV_ALIGN_BOTTOM_LEFT, size_w(82), -size_h(30));
		}
		else {
			lv_obj_align_to(playpic_btn_[i], playpic_btn_[i - 1], LV_ALIGN_OUT_RIGHT_MID, size_w(80), 0);
		}
		lv_obj_add_flag(playpic_btn_[i], LV_OBJ_FLAG_CLICKABLE);
		lv_obj_set_ext_click_area(playpic_btn_[i], 20);
		lv_obj_add_event_cb(playpic_btn_[i], BtnEvent, LV_EVENT_ALL, (void*)i);

		if(GlobalPage::Instance()->page_main()->touch_plan==0){
			lv_obj_add_flag(playpic_btn_[i], LV_OBJ_FLAG_HIDDEN);
		}
	}

#if 0
	GlobalPage::Instance()->page_set()->Createfunction_bar(play_pic_page_, 23, 0,false,true);
#endif
	
}

void PagePlayPic::BtnEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		PagePlayPic* object = GlobalPage::Instance()->page_play_pic();
		int user_data = (int)lv_event_get_user_data(e);
		if (user_data == PlayPicBtn_Pre) {
			object->PlayPicKeypadEvent(KEYMAP_UP);
		}
		else if (user_data == PlayPicBtn_Next) {
			object->PlayPicKeypadEvent(KEYMAP_DOWN);
		}
		else if (user_data == PlayPicBtn_ReturnRec) {
			GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape);
		}
		else if (user_data == PlayPicBtn_ReturnPre) {
			GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape, true);
		}
		else if (user_data == PlayPicBtn_Menu) {
			lv_obj_add_flag(object->play_pic_page_, LV_OBJ_FLAG_HIDDEN);
			object->OpenSetPage();
		}
	}
}

void PagePlayPic::OpenSetPage()
{
	set_page_ = lv_create_page(lv_scr_act(), screen_width, screen_height, lv_color_make(16, 16, 16), 0, 0,
		lv_font_all, lv_color_white(), 0);
	lv_obj_add_event_cb(set_page_, DelSetPageEvent, LV_EVENT_DELETE, NULL);
	lv_obj_set_pos(set_page_, 0, start_y);

    lv_obj_t* top_page = lv_create_page(set_page_, screen_width, size_h(60),
        lv_color_make(68, 68, 68), 0, 0, lv_font_all, lv_color_white(), 0);
    lv_obj_align(top_page, LV_ALIGN_TOP_MID, 0, 0);

    lv_obj_t* title_label_ = lv_label_create(top_page);
    lv_label_set_text(title_label_, GetParsedString("Set"));
    lv_obj_align(title_label_, LV_ALIGN_CENTER, 0, size_h(3));

    lv_obj_t* menu_list_ = lv_create_page(set_page_, screen_width, size_h(PAGE_MENU_LIST_H),
        lv_color_make(16, 16, 16), 0, 0, lv_font_all, lv_color_white(), 0);
    lv_obj_align(menu_list_, LV_ALIGN_TOP_MID, size_w(0), size_h(62));
    lv_obj_add_flag(menu_list_, LV_OBJ_FLAG_SCROLLABLE);

	std::string text[PlaybackSubpage_Total] = {GetParsedString("Delete current"), GetParsedString("Delete all"), 
	GetParsedString("Lock current"), GetParsedString("Unlock current"),
	 GetParsedString("Lock all"), GetParsedString("Unlock all"),
	/* GetParsedString("Main interface"),GetParsedString("Return")*/ };
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
    GlobalPage::Instance()->page_playback()->SelectFileStyle(menu_list_, screen_width, size_h(PAGE_MENU_LIST_H));
	lv_group_focus_next(GlobalData::Instance()->group);

#if 1
	GlobalPage::Instance()->page_set()->Createfunction_bar(set_page_,58,11,true,false);
#endif
}
void PagePlayPic::DelSetPageEvent(lv_event_t* e)
{
	PagePlayPic* object = GlobalPage::Instance()->page_play_pic();
	for (int i = 0;i < PlaybackSubpage_Total;i++) {
		lv_obj_remove_event_cb(object->set_btn_[i], object->OpenSubpage);
	}
}

void PagePlayPic::OpenSubpage(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		PagePlayPic* object = GlobalPage::Instance()->page_play_pic();
		int user_data = (int)lv_event_get_user_data(e);
		if (user_data == PlaybackSubpage_Return) {
			if (object->pic_count_ == 0) {
				//GlobalPage::Instance()->page_main()->OpenTipBox("No picture file");
				GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape, true);
			}
			else {
				lv_obj_del(object->set_page_);
				object->set_page_ = NULL;
				lv_obj_clear_flag(object->play_pic_page_, LV_OBJ_FLAG_HIDDEN);
			}
		}
		else if (user_data == PlaybackSubpage_Main) {
			GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape);
		}
		else {
			object->foucused_obj_[0] = lv_group_get_focused(GlobalData::Instance()->group);

			for (int i = 0;i < PlaybackSubpage_Total;i++) {
				lv_group_remove_obj(object->set_btn_[i]);
			}
			
			object->OpenConfirmDelPage(user_data);
			lv_group_focus_next(GlobalData::Instance()->group);
			//lv_event_send(object->set_btn_[PlaybackSubpage_Return], LV_EVENT_DEFOCUSED, (void*)PlaybackSubpage_Return);
			lv_event_send(object->set_btn_[PlaybackSubpage_UnlockAll], LV_EVENT_DEFOCUSED, (void*)PlaybackSubpage_UnlockAll);
		}
	}
	else if (code == LV_EVENT_FOCUSED) {
		PagePlayPic* object = GlobalPage::Instance()->page_play_pic();
		if (object->set_page_ == NULL)
			return;

		int user_data = (int)lv_event_get_user_data(e);

		lv_obj_add_flag(object->set_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(object->set_page_, LV_OBJ_FLAG_HIDDEN);

		// char path[128] = { 0 };
		// strncpy(path, (char*)lv_img_get_src(object->set_btn_[user_data]), sizeof(path) - 1);
		// int path_size = strlen(path);
		// if (strncmp(&path[path_size - 5], "-.png", 5) == 0)
		// 	return;

		// memset(&path[path_size - 4], 0, 4);
		// strncpy(&path[path_size - 4], "-.png", 5);
		// XMLogI("LV_EVENT_FOCUSED--path:%s", path);
		// lv_img_set_src(object->set_btn_[user_data], path);

		lv_obj_add_flag(object->set_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(object->set_page_, LV_OBJ_FLAG_HIDDEN);
	}
	else if (code == LV_EVENT_DEFOCUSED) {
		PagePlayPic* object = GlobalPage::Instance()->page_play_pic();
		if (object->set_page_ == NULL)
			return;

		int user_data = (int)lv_event_get_user_data(e);

		// char path[128] = { 0 };
		// strncpy(path, (char*)lv_img_get_src(object->set_btn_[user_data]), sizeof(path) - 1);
		// int path_size = strlen(path);
		// if (strncmp(&path[path_size - 5], "-.png", 5) != 0)
		// 	return;

		// memset(&path[path_size - 5], 0, 5);
		// strncpy(&path[path_size - 5], ".png", 4);
		// XMLogI("LV_EVENT_DEFOCUSED--path:%s", path);
		// lv_img_set_src(object->set_btn_[user_data], path);
	}
}

void PagePlayPic::DeletedEvent(lv_event_t* e)
{
	for (int i = 0;i < PlaybackSubpage_Total;i++) {
		lv_group_add_obj(GlobalData::Instance()->group, GlobalPage::Instance()->page_play_pic()->set_btn_[i]);
	}
	lv_group_focus_obj(GlobalPage::Instance()->page_play_pic()->foucused_obj_[0]);
}

void PagePlayPic::OpenConfirmDelPage(int flag)
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
	GetParsedString("Lock all"), GetParsedString("Unlock all")};
	lv_obj_t* label = lv_create_label(tip_win, screen_width-100, text[flag].c_str(), LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, size_h(40));

		lv_obj_t* yes_btn = lv_create_btn(tip_win, size_w(240), size_h(60),
        lv_color_make(85, 85, 85), 10, ManageFileEvent, LV_EVENT_ALL, (void*)flag, 0);
	//lv_obj_set_style_border_width(yes_btn, 2, 0);
	//lv_obj_set_style_border_color(yes_btn, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), 0);
    lv_obj_align(yes_btn, LV_ALIGN_TOP_MID, size_w(0), size_h(80));

	lv_obj_t* btn_label = lv_label_create(yes_btn);
	lv_label_set_text(btn_label, GetParsedString("Confirm"));
	lv_obj_center(btn_label);

	lv_obj_t* no_btn = lv_create_btn(tip_win, size_w(240), size_h(60),
        lv_color_make(85, 85, 85), 10, ManageFileEvent, LV_EVENT_ALL, (void*)-1, 0);
	//lv_obj_set_style_border_width(no_btn, 2, 0);
	//lv_obj_set_style_border_color(no_btn, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), 0);
    lv_obj_align(no_btn, LV_ALIGN_TOP_MID, size_w(0), size_h(180));

	btn_label = lv_label_create(no_btn);
	lv_label_set_text(btn_label, GetParsedString("Cancel"));
	lv_obj_center(btn_label);

	GlobalData::Instance()->opened_subpage_[0] = page;
}

void PagePlayPic::ManageFileEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		PagePlayPic* object = GlobalPage::Instance()->page_play_pic();

		lv_obj_add_flag(object->set_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(object->set_page_, LV_OBJ_FLAG_HIDDEN);

		int user_data = (int)lv_event_get_user_data(e);
		if (user_data == PlaybackSubpage_LockCur || user_data == PlaybackSubpage_UnlockCur
			|| user_data == PlaybackSubpage_LockAll || user_data == PlaybackSubpage_UnlockAll) {
			XMUIEventInParam event_inparam;
			XMUIEventOutParam event_outparam;
			if (object->pic_count_ == 0) {
				if (GlobalData::Instance()->opened_subpage_[0]) {
					lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
					GlobalData::Instance()->opened_subpage_[0] = NULL;
				}
				GlobalPage::Instance()->page_main()->OpenTipBox("No picture file");
				return;
			}

			if (user_data == PlaybackSubpage_LockCur) {
				event_inparam.vod_lockhistoryfile.file_param.sdc_num = XM_STORAGE_SDCard_0;
				event_inparam.vod_lockhistoryfile.file_param.channel = object->playing_file_info_.direction;
				event_inparam.vod_lockhistoryfile.file_param.type = XM_Record_Picture;
				strncpy(event_inparam.vod_lockhistoryfile.file_param.file_name,
					object->playing_file_info_.file_name, XM_MAX_STORAGE_FILE_LEN - 1);
				GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_LOCK_HISTORY_FILE, &event_inparam, &event_outparam);
				GlobalPage::Instance()->page_main()->OpenTipBox("Successfully locked", 2000);
			}
			else if (user_data == PlaybackSubpage_UnlockCur) {
				event_inparam.vod_unlockhistoryfile.file_param.sdc_num = XM_STORAGE_SDCard_0;
				event_inparam.vod_unlockhistoryfile.file_param.channel = object->playing_file_info_.direction;
				event_inparam.vod_unlockhistoryfile.file_param.type = XM_Record_Picture;
				strncpy(event_inparam.vod_unlockhistoryfile.file_param.file_name,
					object->playing_file_info_.file_name, XM_MAX_STORAGE_FILE_LEN - 1);
				GlobalData::Instance()->ui_event_cb_(XM_UI_VOD_UNLOCK_HISTORY_FILE, &event_inparam, &event_outparam);
				GlobalPage::Instance()->page_main()->OpenTipBox("Unlock successfully", 2000);
			}
			else if (user_data == PlaybackSubpage_LockAll) {
				if (object->file_type_ == FileType_FrontPhoto) {
					XM_Middleware_Storage_LockAllHistoryFile(XM_STORAGE_SDCard_0, Direction_Front, XM_Record_Picture);
				}
				else if (object->file_type_ == FileType_BehindPhoto) {
					XM_Middleware_Storage_LockAllHistoryFile(XM_STORAGE_SDCard_0, Direction_Behind, XM_Record_Picture);
				}

				GlobalPage::Instance()->page_main()->OpenTipBox("Successfully locked", 2000);
			}
			else if (user_data == PlaybackSubpage_UnlockAll) {
				if (object->file_type_ == FileType_FrontPhoto) {
					XM_Middleware_Storage_UnLockAllHistoryFile(XM_STORAGE_SDCard_0, Direction_Front, XM_Record_Picture);
				}
				else if (object->file_type_ == FileType_BehindPhoto) {
					XM_Middleware_Storage_UnLockAllHistoryFile(XM_STORAGE_SDCard_0, Direction_Behind, XM_Record_Picture);
				}

				GlobalPage::Instance()->page_main()->OpenTipBox("Unlock successfully", 2000);
			}

			object->SearchFile(object->file_type_);
			object->SwitchFile();
		}
		else if (user_data == PlaybackSubpage_DelCur) {
			object->DelPicFile();
		}
		else if (user_data == PlaybackSubpage_DelAll) {
			object->DelAllPicFile();
		}

		if (GlobalData::Instance()->opened_subpage_[0]) {
			lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
			GlobalData::Instance()->opened_subpage_[0] = NULL;
		}
	}
}

void PagePlayPic::DelPicFile()
{
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	if (pic_count_ == 0) {
		GlobalPage::Instance()->page_main()->OpenTipBox("No picture file");
		return;
	}

	//判断文件是否加锁
	if (playing_file_info_.file_locked) {
		GlobalPage::Instance()->page_main()->OpenTipBox("File locked");
		return;
	}

	event_inparam.storage_del_file.file_param.type = XM_Record_Picture;
	event_inparam.storage_del_file.file_param.channel = playing_file_info_.direction;
	event_inparam.storage_del_file.file_param.sdc_num = XM_STORAGE_SDCard_0;
	strncpy(event_inparam.storage_del_file.file_param.file_name, playing_file_info_.file_name, XM_MAX_STORAGE_FILE_LEN - 1);
	GlobalData::Instance()->ui_event_cb_(XM_UI_STORAGE_DEL_FILE, &event_inparam, &event_outparam);
	GlobalPage::Instance()->page_main()->OpenTipBox("Successfully deleted");

	SearchFile(file_type_);
	if (pic_count_ == 0) {
		
	}
	else {
		playing_file_info_.file_num -= 1;
		if (playing_file_info_.file_num < 0) {
			playing_file_info_.file_num = pic_count_ - 1;
		}

		SwitchFile();
	}
}

void PagePlayPic::DelAllPicFile()
{
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	if (pic_count_ == 0) {
		GlobalPage::Instance()->page_main()->OpenTipBox("No picture file");
		return;
	}

	//删除所有照片
	int ret = 0;
	if (file_type_ == FileType_FrontPhoto) {
		ret = XM_Middleware_Storage_DelAllFile(XM_STORAGE_SDCard_0, Direction_Front, XM_Record_Picture);
	}
	else if (file_type_ == FileType_BehindPhoto) {
		ret = XM_Middleware_Storage_DelAllFile(XM_STORAGE_SDCard_0, Direction_Behind, XM_Record_Picture);
	}

	if (ret) {
		GlobalPage::Instance()->page_main()->OpenTipBox("File locked");
	}
	else {
		GlobalPage::Instance()->page_main()->OpenTipBox("Successfully deleted");
	}

	SearchFile(file_type_);
	if (pic_count_ == 0) {
		
	}
	else {
		playing_file_info_.file_num = 0;
		SwitchFile();
	}
}

void PagePlayPic::PlayPic()
{
	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;

	//更新当前文件信息
	playing_file_info_.direction = file_list_.records[playing_file_info_.file_num].channel;
	playing_file_info_.file_len = file_list_.records[playing_file_info_.file_num].file_len;
	playing_file_info_.file_locked = file_list_.records[playing_file_info_.file_num].picture_info.file_locked;
	playing_file_info_.width = file_list_.records[playing_file_info_.file_num].picture_info.width;
	playing_file_info_.height = file_list_.records[playing_file_info_.file_num].picture_info.height;
	memset(playing_file_info_.file_name, 0, sizeof(playing_file_info_.file_name));
	memset(playing_file_info_.file_path, 0, sizeof(playing_file_info_.file_path));
	strcpy(playing_file_info_.file_name, file_list_.records[playing_file_info_.file_num].file_name);
	sprintf(playing_file_info_.file_path, "/mnt/tfcard/pic/pic_%d/%s",
		file_list_.records[playing_file_info_.file_num].channel,
		file_list_.records[playing_file_info_.file_num].file_name);

	strncpy(event_inparam.picture_render.file_full_name, playing_file_info_.file_path, XM_MAX_STORAGE_PATH_LEN - 1);
	XMLogI("playing_file_info_.file_path:%s", event_inparam.picture_render.file_full_name);
	event_inparam.picture_render.file_len = playing_file_info_.file_len;
	GlobalData::Instance()->ui_event_cb_(XM_UI_PICTURE_RENDER, &event_inparam, &event_outparam);
}

int PagePlayPic::SearchFile(int file_type)
{
	int channel = 0;
	if (file_type == FileType_FrontPhoto) {
		channel = 0;
	}
	else if (file_type == FileType_BehindPhoto) {
		channel = 1;
	}
	else {
		return -1;
	}

	file_type_ = file_type;

	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;
	event_inparam.query_record.rec_query.channel = channel;
	event_inparam.query_record.rec_query.sdc_num = XM_STORAGE_SDCard_0;
	event_inparam.query_record.rec_query.type = XM_Record_Picture;
	event_inparam.query_record.rec_query.start_time = 0;
	event_inparam.query_record.rec_query.end_time = 0xffffffff;
	GlobalData::Instance()->ui_event_cb_(XM_UI_QUERY_RECORD, &event_inparam, &event_outparam);
	file_list_ = event_outparam.query_record.rec_list;
	pic_count_ = file_list_.record_count;
	/*for (int i = 0; i < file_list_.record_count; i++) {
		XMLogI("name = %s", file_list_.records[i].file_name);
	}*/
	return file_list_.record_count;
}

int PagePlayPic::PlayPicPrepareWork(bool open_page)
{
	if (open_page) {
		//SearchFile();
		if (pic_count_ == 0) {
			OpenSetPage();
			GlobalPage::Instance()->page_main()->OpenTipBox("No picture file");
		}
		else {
			playing_file_info_.file_num = pic_count_ - 1;
			PlayPic();
			OpenPage();
			GlobalPage::Instance()->page_playback()->close_preview_ = true;
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

		if (set_page_) {
			lv_obj_del(set_page_);
			set_page_ = NULL;
		}
		
		if (play_pic_page_) {
			lv_obj_del(play_pic_page_);
			play_pic_page_ = NULL;
		}
		
		playing_file_ = false;
		pic_count_ = 0;
	}

	return 0;
}

void PagePlayPic::PlayPicKeypadEvent(int key)
{
	if (key == KEYMAP_MODE) {
		if (pic_count_ == 0 || !lv_obj_has_flag(play_pic_page_, LV_OBJ_FLAG_HIDDEN)) {
			GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape, true);
			return;
		}
	}
	if (GlobalPage::Instance()->page_main()->touch_plan == 0) {
     if (key == KEYMAP_OK) {
         return;
     }
    }
	if (!playing_file_) {
		if (key == KEYMAP_UP) {
			playing_file_info_.file_num += 1;
			if (playing_file_info_.file_num > pic_count_ - 1)
				playing_file_info_.file_num = 0;

			SwitchFile();
		}
		else if (key == KEYMAP_DOWN) {
			playing_file_info_.file_num -= 1;
			if (playing_file_info_.file_num < 0)
				playing_file_info_.file_num = pic_count_ - 1;

			SwitchFile();
		}
		else if (key == KEYMAP_OK) {
			playing_file_ = true;
			for (int i = 0; i < lv_obj_get_child_cnt(play_pic_page_); i++) {
				lv_obj_t* item = lv_obj_get_child(play_pic_page_, i);
				lv_obj_add_flag(item, LV_OBJ_FLAG_HIDDEN);
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
			else if (set_page_ == NULL) {
				lv_obj_add_flag(play_pic_page_, LV_OBJ_FLAG_HIDDEN);
				OpenSetPage();
			}
			else {
				if (pic_count_ == 0) {
					GlobalPage::Instance()->page_main()->OpenTipBox("No picture file");
				}
				else {
					lv_obj_del(set_page_);
					set_page_ = NULL;
					lv_obj_clear_flag(play_pic_page_, LV_OBJ_FLAG_HIDDEN);
				}
			}
		}
	}
	else {
		XMUIEventInParam event_inparam;
		XMUIEventOutParam event_outparam;
		if (key == KEYMAP_OK) {
			playing_file_ = false;
			for (int i = 0; i < lv_obj_get_child_cnt(play_pic_page_); i++) {
				lv_obj_t* item = lv_obj_get_child(play_pic_page_, i);
				lv_obj_clear_flag(item, LV_OBJ_FLAG_HIDDEN);
			}

			if (!playing_file_info_.file_locked) {
				lv_obj_add_flag(lock_label_, LV_OBJ_FLAG_HIDDEN);
			}

			//由于局部显示不完整，所以将整个页面隐藏再显示
			lv_obj_add_flag(play_pic_page_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(play_pic_page_, LV_OBJ_FLAG_HIDDEN);
		}
	}
}

void PagePlayPic::SwitchFile()
{
	PlayPic();

	lv_label_set_text(name_label_, playing_file_info_.file_name);
	if(playing_file_info_.height == k4KHeight)
		lv_label_set_text(resolution_label_, "4K");
	else if(playing_file_info_.height == k3KHeight)
		lv_label_set_text(resolution_label_, "3K");
	else if(playing_file_info_.height == 1440)		
		lv_label_set_text(resolution_label_, "2K");
	else
		lv_label_set_text_fmt(resolution_label_, "%dP", playing_file_info_.height);

	if (playing_file_info_.file_locked) {
		lv_obj_clear_flag(lock_label_, LV_OBJ_FLAG_HIDDEN);
	}
	else {
		lv_obj_add_flag(lock_label_, LV_OBJ_FLAG_HIDDEN);
	}
}

void PagePlayPic::UnplugSDCard()
{
	GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape);
}