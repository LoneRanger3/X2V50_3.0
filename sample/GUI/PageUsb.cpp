#include "global_data.h"
#include "global_page.h"
#include "PageUsb.h"
#include "xm_middleware_network.h"

extern int g_sd_status;
const int kSignalPort = 2222;
const int kMediaPort = 2223;

PageUsb::PageUsb():usb_page_(NULL), usb_mode_(USB_MODE_NONE), user_selected_usb_mode_(USB_MODE_NONE)
{

}

PageUsb::~PageUsb()
{

}

void PageUsb::OpenPage()
{
	usb_page_ = lv_create_page(lv_scr_act(), screen_width, screen_height, lv_color_make(80, 150, 190), 0, 0,
		lv_font_all, lv_color_white(), 0);
	lv_obj_set_pos(usb_page_, 0, start_y);

	static lv_style_t img_focus_style;
	lv_style_init(&img_focus_style);
	lv_style_set_border_opa(&img_focus_style, LV_OPA_COVER);
	lv_style_set_border_width(&img_focus_style, 3);
	lv_style_set_border_color(&img_focus_style, lv_color_make(250, 250, 250));

	mode_img_[0] = lv_img_create(usb_page_);
	lv_img_set_src(mode_img_[0], image_path"Memory_model.png");
	lv_obj_align(mode_img_[0], LV_ALIGN_LEFT_MID, size_w(81), 0);
	
	mode_label_[0] = lv_label_create(usb_page_);
	lv_label_set_text(mode_label_[0], GetParsedString("Memory mode"));
	lv_obj_align_to(mode_label_[0], mode_img_[0], LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));
	lv_obj_add_event_cb(mode_img_[0], SelectUsbFuncEvent, LV_EVENT_ALL, (void*)USB_MODE_MASS_STORAGE);
	lv_obj_add_style(mode_img_[0], &img_focus_style, LV_STATE_FOCUS_KEY);
	lv_group_add_obj(GlobalData::Instance()->group, mode_img_[0]);

	mode_img_[1] = lv_img_create(usb_page_);
	lv_img_set_src(mode_img_[1], image_path"Camera_mode.png");
	lv_obj_align(mode_img_[1], LV_ALIGN_CENTER, 0, 0);
	lv_obj_add_event_cb(mode_img_[1], SelectUsbFuncEvent, LV_EVENT_ALL, (void*)USB_MODE_RNDIS);
	lv_obj_add_style(mode_img_[1], &img_focus_style, LV_STATE_FOCUS_KEY);
	lv_group_add_obj(GlobalData::Instance()->group, mode_img_[1]);

	mode_label_[1] = lv_label_create(usb_page_);
	lv_label_set_text(mode_label_[1], GetParsedString("Camera mode"));
	lv_obj_align_to(mode_label_[1], mode_img_[1], LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));

	mode_img_[2] = lv_img_create(usb_page_);
	lv_img_set_src(mode_img_[2], image_path"Recording_mode.png");
	lv_obj_align(mode_img_[2], LV_ALIGN_RIGHT_MID, -size_w(81), 0);
	lv_obj_add_event_cb(mode_img_[2], SelectUsbFuncEvent, LV_EVENT_ALL, (void*)USB_MODE_NONE);
	lv_obj_add_style(mode_img_[2], &img_focus_style, LV_STATE_FOCUS_KEY);
	lv_group_add_obj(GlobalData::Instance()->group, mode_img_[2]);

	mode_label_[2] = lv_label_create(usb_page_);
	lv_label_set_text(mode_label_[2], GetParsedString("Recorder mode"));
	lv_obj_align_to(mode_label_[2], mode_img_[2], LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));
}

void PageUsb::SelectUsbFuncEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		PageUsb* page_usb = GlobalPage::Instance()->page_usb();
		USB_MODE_E user_data = (USB_MODE_E)(int)lv_event_get_user_data(e);
		page_usb->user_selected_usb_mode_ = user_data;
		page_usb->SetUsbFunc(user_data);
		lv_obj_del(page_usb->usb_page_);
		page_usb->usb_page_ = NULL;
		if (user_data == USB_MODE_MASS_STORAGE) {
			GlobalData::Instance()->stop_keypad_ = true;
			page_usb->OpenMemoryModePage(USB_MODE_MASS_STORAGE);
		}
		else if (user_data == USB_MODE_RNDIS) {
			GlobalData::Instance()->stop_keypad_ = true;
			page_usb->OpenMemoryModePage(USB_MODE_RNDIS);
		}
		else {
			GlobalPage::Instance()->page_main()->connect_computer_ = false;
			GlobalPage::Instance()->page_main()->StartRecord();
		}
	}
	else if (code == LV_EVENT_FOCUSED) {
		PageUsb* page_usb = GlobalPage::Instance()->page_usb();
		USB_MODE_E user_data = (USB_MODE_E)(int)lv_event_get_user_data(e);
		if (user_data == USB_MODE_MASS_STORAGE) {
			lv_img_set_src(page_usb->mode_img_[0], image_path"Memory_model-.png");
			lv_obj_align(page_usb->mode_img_[0], LV_ALIGN_LEFT_MID, size_w(51), 0);
			lv_obj_align_to(page_usb->mode_label_[0], page_usb->mode_img_[0], LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));
		}
		else if (user_data == USB_MODE_RNDIS) {
			lv_img_set_src(page_usb->mode_img_[1], image_path"Camera_mode-.png");
			lv_obj_align(page_usb->mode_img_[1], LV_ALIGN_CENTER, 0, 0);
			lv_obj_align_to(page_usb->mode_label_[1], page_usb->mode_img_[1], LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));
		}
		else {
			lv_img_set_src(page_usb->mode_img_[2], image_path"Recording_mode-.png");
			lv_obj_align(page_usb->mode_img_[2], LV_ALIGN_RIGHT_MID, -size_w(51), 0);
			lv_obj_align_to(page_usb->mode_label_[2], page_usb->mode_img_[2], LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));
		}

		lv_obj_add_flag(page_usb->usb_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(page_usb->usb_page_, LV_OBJ_FLAG_HIDDEN);
	}
	else if (code == LV_EVENT_DEFOCUSED) {
		PageUsb* page_usb = GlobalPage::Instance()->page_usb();
		USB_MODE_E user_data = (USB_MODE_E)(int)lv_event_get_user_data(e);
		if (user_data == USB_MODE_MASS_STORAGE) {
			lv_img_set_src(page_usb->mode_img_[0], image_path"Memory_model.png");
			lv_obj_align(page_usb->mode_img_[0], LV_ALIGN_LEFT_MID, size_w(81), 0);
			lv_obj_align_to(page_usb->mode_label_[0], page_usb->mode_img_[0], LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));
		}
		else if (user_data == USB_MODE_RNDIS) {
			lv_img_set_src(page_usb->mode_img_[1], image_path"Camera_mode.png");
			lv_obj_align(page_usb->mode_img_[1], LV_ALIGN_CENTER, 0, 0);
			lv_obj_align_to(page_usb->mode_label_[1], page_usb->mode_img_[1], LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));
		}
		else {
			lv_img_set_src(page_usb->mode_img_[2], image_path"Recording_mode.png");
			lv_obj_align(page_usb->mode_img_[2], LV_ALIGN_RIGHT_MID, -size_w(81), 0);
			lv_obj_align_to(page_usb->mode_label_[2], page_usb->mode_img_[2], LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));
		}
	}
}

int PageUsb::SetUsbFunc(USB_MODE_E mode)
{
	XMLogI("SetUsbFunc mode = %d", mode);
	int ret = 0;
	if (usb_mode_ != USB_MODE_NONE) {
		ret = XM_Middleware_USBFunc_Destroy(usb_mode_);
		if (ret < 0) {
			XMLogW("XM_Middleware_USBFunc_Destroy Failed! ret = %d", ret);
			return -1;
		}
	}
	usb_mode_ = USB_MODE_NONE;
	
	if (mode != USB_MODE_NONE) {
		ret = XM_Middleware_USBFunc_Create(mode);
		if (ret < 0) {
			XMLogW("XM_Middleware_USBFunc_Create Failed! ret = %d", ret);
			return -1;
		}
		if (mode == USB_MODE_RNDIS) {
			int ret = XM_Middleware_Network_StartServer(kSignalPort, kMediaPort);
			if (ret < 0) {
				XMLogE("start server error");
			}	
		}
	}
	usb_mode_ = mode;

	return 0;
}

void PageUsb::OpenMemoryModePage(USB_MODE_E mode)
{
	lv_obj_t* page = lv_create_page(lv_scr_act(), screen_width, screen_height, lv_color_make(80, 150, 190), 0, 0,
		lv_font_all, lv_color_white(), 0);
	lv_obj_set_pos(page, 0, start_y);

	lv_obj_t* storage_img = lv_img_create(page);
	if (mode == USB_MODE_MASS_STORAGE)
		lv_img_set_src(storage_img, image_path"s_MASS.png");
	else if (mode == USB_MODE_RNDIS)
		lv_img_set_src(storage_img, image_path"s_PC.png");
	lv_obj_center(storage_img);
}