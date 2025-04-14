#ifndef __GUI_H__
#define __GUI_H__

#include "lvgl.h"


lv_obj_t* lv_create_btn(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_color_t color, lv_coord_t radius, 
	lv_event_cb_t event_cb, lv_event_code_t event_code, void* user_data, lv_style_selector_t selector);

lv_obj_t* lv_create_imgbtn(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_coord_t radius, 
	lv_imgbtn_state_t state, const void* src_left, const void* src_mid, const void* src_right,
	lv_event_cb_t event_cb, lv_event_code_t event_code, void * user_data, lv_style_selector_t selector);

lv_obj_t* lv_create_label(lv_obj_t* parent, lv_coord_t width, const char* text, lv_text_align_t text_align, 
	lv_style_selector_t selector);

lv_obj_t* lv_create_image(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, const void* img_path);

lv_obj_t* lv_create_switch(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_event_cb_t event_cb, 
	lv_event_code_t event_code, void* user_data);

void lv_set_list_style(lv_obj_t* list, lv_coord_t width, lv_coord_t height, lv_color_t list_bg_color, 
	lv_coord_t border_width, lv_style_selector_t list_selector, lv_color_t btn_bg_color, 
	lv_color_t btn_text_color, lv_opa_t opa, lv_style_selector_t btn_selector, lv_color_t label_text_color, lv_style_selector_t label_selector);

lv_obj_t* lv_create_roller(lv_obj_t* parent, lv_coord_t width, const char* options, lv_roller_mode_t roller_mode, 
	uint8_t row_cnt, lv_event_cb_t event_cb, lv_event_code_t event_code, void* user_data);

lv_obj_t* lv_create_page(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_color_t bg_color, lv_coord_t radius, 
	lv_coord_t border_width, const lv_font_t* font, lv_color_t text_color, lv_style_selector_t selector);

lv_obj_t* lv_create_line(lv_obj_t* parent, lv_coord_t line_width, lv_color_t line_color, bool line_round, 
	const lv_point_t* line_points, uint16_t point_num, lv_style_selector_t selector);

lv_obj_t* lv_create_checkbox(lv_obj_t* parent, const char* text, lv_event_cb_t event_cb, lv_event_code_t event_code,
	void* user_data);

lv_obj_t* lv_list_add_btn_self(lv_obj_t* list, const void* left_icon, const char* txt, const void* right_icon);

lv_obj_t* lv_create_tabview(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_color_t tabview_bg_color,
	lv_dir_t tab_pos, lv_coord_t tab_size, lv_style_selector_t selector);

lv_obj_t* lv_list_add_btn_menu(lv_obj_t* list, const void* left_icon, const char* txt, const void* right_icon);
lv_obj_t* lv_list_chile_btn(lv_obj_t* list, const void* left_icon, const char* txt);

#endif