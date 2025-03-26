#include "GUI.h"


lv_obj_t* lv_create_btn(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_color_t color, lv_coord_t radius,
	lv_event_cb_t event_cb, lv_event_code_t event_code, void* user_data, lv_style_selector_t selector)
{
	lv_obj_t* btn = lv_btn_create(parent);
	lv_obj_set_size(btn, width, height);
	lv_obj_set_style_bg_color(btn, color, selector);
	lv_obj_set_style_radius(btn, radius, selector);
	lv_obj_add_event_cb(btn, event_cb, event_code, user_data);
	lv_obj_set_style_shadow_width(btn, 0, 0);
	
	return btn;
}

lv_obj_t* lv_create_imgbtn(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_coord_t radius, 
	lv_imgbtn_state_t state, const void* src_left, const void* src_mid, const void* src_right,
	lv_event_cb_t event_cb, lv_event_code_t event_code, void * user_data, lv_style_selector_t selector)
{
	lv_obj_t* imgbtn = lv_imgbtn_create(parent);
	lv_obj_set_size(imgbtn, width, height);
	lv_obj_set_style_radius(imgbtn, radius, selector);
	lv_imgbtn_set_src(imgbtn, state, src_left, src_mid, src_right);
	lv_obj_add_event_cb(imgbtn, event_cb, event_code, user_data);
	lv_obj_set_style_img_recolor_opa(imgbtn, LV_OPA_30, LV_STATE_PRESSED);

	return imgbtn;
}

lv_obj_t* lv_create_label(lv_obj_t* parent, lv_coord_t width, const char* text, lv_text_align_t text_align, lv_style_selector_t selector)
{
	lv_obj_t* label = lv_label_create(parent);
	lv_obj_set_width(label, width);
	lv_label_set_text(label, text);
	lv_obj_set_style_text_align(label, text_align, selector);

	return label;
}

lv_obj_t* lv_create_image(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, const void* img_path)
{
	lv_obj_t* img = lv_img_create(parent);
	lv_obj_set_size(img, width, height);
	lv_img_set_src(img, img_path);

	return img;
}

lv_obj_t* lv_create_switch(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_event_cb_t event_cb, 
	lv_event_code_t event_code, void* user_data)
{
	lv_obj_t* sw = lv_switch_create(parent);
	lv_obj_set_size(sw, width, height);
	lv_obj_add_event_cb(sw, event_cb, event_code, user_data);

	return sw;
}

void lv_set_list_style(lv_obj_t* list, lv_coord_t width, lv_coord_t height, lv_color_t list_bg_color, lv_coord_t border_width, 
	lv_style_selector_t list_selector, lv_color_t btn_bg_color, lv_color_t btn_text_color, lv_opa_t opa,
	lv_style_selector_t btn_selector, lv_color_t label_text_color, lv_style_selector_t label_selector)
{
	lv_obj_set_size(list, width, height);
	lv_obj_set_style_border_width(list, border_width, list_selector);
	lv_obj_set_style_bg_color(list, list_bg_color, list_selector);

	for(int i=0; i<lv_obj_get_child_cnt(list); i++)
	{
		lv_obj_t* item = lv_obj_get_child(list, i);
		if(lv_obj_check_type(item, &lv_list_btn_class))
		{
			lv_obj_set_style_bg_color(item, btn_bg_color, btn_selector);
			lv_obj_set_style_text_color(item, btn_text_color, btn_selector);
			lv_obj_set_style_bg_opa(item, opa, btn_selector);
			//lv_obj_set_style_border_width(item, 0, 0);
			for (int i = 0; i < lv_obj_get_child_cnt(item); i++) {
				lv_obj_t* label = lv_obj_get_child(item, i);
				if (lv_obj_check_type(label, &lv_label_class)) {
					lv_label_set_long_mode(label, LV_LABEL_LONG_CLIP);
				}
			}
		}
		else
		{
			lv_obj_set_style_text_color(item, label_text_color, label_selector);
		}
	}
}

lv_obj_t* lv_list_add_btn_self(lv_obj_t * list, const void * left_icon, const char * txt, const void* right_icon)
{
	LV_LOG_INFO("begin");
	lv_obj_t * obj = lv_obj_class_create_obj(&lv_list_btn_class, list);
	lv_obj_class_init_obj(obj);
	lv_obj_set_size(obj, LV_PCT(100), 77);

#if LV_USE_IMG == 1
	if (left_icon) {
		lv_obj_t * img = lv_img_create(obj);
		lv_img_set_src(img, left_icon);
		lv_obj_align(img, LV_ALIGN_LEFT_MID, 0, 0);
	}

	if (right_icon) {
		lv_obj_t* right_img = lv_img_create(obj);
		lv_img_set_src(right_img, right_icon);
		lv_obj_align(right_img, LV_ALIGN_RIGHT_MID, 0, 0);
	}
#endif

	if (txt) {
		lv_obj_t * label = lv_label_create(obj);
		lv_label_set_text(label, txt);
		lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
		if (left_icon) {
			lv_obj_align(label, LV_ALIGN_LEFT_MID, 50, 0);
		}
		else {
			lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 0);
		}
	}

	return obj;
}

lv_obj_t* lv_create_roller(lv_obj_t* parent, lv_coord_t width, const char* options, lv_roller_mode_t roller_mode, 
	uint8_t row_cnt, lv_event_cb_t event_cb, lv_event_code_t event_code, void* user_data)
{
	lv_obj_t* roller = lv_roller_create(parent);
	lv_obj_set_width(roller, width);
	lv_roller_set_options(roller, options, roller_mode);
	lv_obj_add_event_cb(roller, event_cb, event_code, user_data);
	lv_roller_set_visible_row_count(roller, row_cnt);

	return roller;
}

lv_obj_t* lv_create_line(lv_obj_t* parent, lv_coord_t line_width, lv_color_t line_color, bool line_round, 
	const lv_point_t* line_points, uint16_t point_num, lv_style_selector_t selector)
{
	lv_obj_t* line = lv_line_create(parent);
	lv_obj_set_style_line_width(line, line_width, selector);
	lv_obj_set_style_line_color(line, line_color, selector);
	lv_obj_set_style_line_rounded(line, line_round, selector);
	lv_line_set_points(line, line_points, point_num);

	return line;
}

lv_obj_t* lv_create_checkbox(lv_obj_t* parent, const char* text, lv_event_cb_t event_cb, lv_event_code_t event_code, 
	void* user_data)
{
	lv_obj_t* checkbox = lv_checkbox_create(parent);
	lv_checkbox_set_text(checkbox, text);
	lv_obj_add_event_cb(checkbox, event_cb, event_code, user_data);

	return checkbox;
}

lv_obj_t* lv_create_tabview(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_color_t tabview_bg_color,
	lv_dir_t tab_pos, lv_coord_t tab_size, lv_style_selector_t selector)
{
	lv_obj_t* tabview = lv_tabview_create(parent, tab_pos, tab_size);
	lv_obj_set_size(tabview, width, height);
	lv_obj_set_style_bg_color(tabview, tabview_bg_color, selector);

	return tabview;
}

lv_obj_t* lv_create_page(lv_obj_t* parent, lv_coord_t width, lv_coord_t height, lv_color_t bg_color,
	lv_coord_t radius, lv_coord_t border_width, const lv_font_t* font, lv_color_t text_color, lv_style_selector_t selector)
{
	
	lv_obj_t* page = lv_obj_create(parent);
	lv_obj_set_size(page, width, height);
	lv_obj_set_style_bg_color(page, bg_color, selector);
	lv_obj_set_style_radius(page, radius, selector);
	lv_obj_set_style_border_width(page, border_width, selector);
	lv_obj_set_style_text_font(page, font, selector);
	lv_obj_set_style_text_color(page, text_color, selector);
	lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);
	lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_pad_all(page, 0, 0);

	return page;
}


lv_obj_t* lv_list_add_btn_menu(lv_obj_t* list, const void* left_icon, const char* txt, const void* right_icon)
{
#define MENU_HEIGHT_SIZE 77
    LV_LOG_INFO("begin");
    lv_obj_t* obj = lv_obj_class_create_obj(&lv_list_btn_class, list);
    lv_obj_class_init_obj(obj);
    lv_obj_set_size(obj, LV_PCT(100), MENU_HEIGHT_SIZE);
    const lv_font_t* font = lv_obj_get_style_text_font(obj, LV_PART_MAIN);
    lv_coord_t line_space = lv_obj_get_style_text_line_space(obj, LV_PART_MAIN);
    lv_coord_t font_h = lv_font_get_line_height(font);

#if LV_USE_IMG == 1
    if (left_icon) {
        if ((!strcmp((char*)left_icon, " ")) || (!strcmp((char*)left_icon, "-1"))) {

        }
        else {
            lv_obj_t* img = lv_img_create(obj);
            lv_img_set_src(img, left_icon);
            lv_obj_align(img, LV_ALIGN_LEFT_MID, 4, 0);
        }
    }

    if (right_icon) {
        if ((!strcmp((char*)right_icon, " ")) || (!strcmp((char*)right_icon, "-1"))) {

        }
        else {
            lv_obj_t* right_img = lv_img_create(obj);
            lv_img_set_src(right_img, right_icon);
            lv_obj_align(right_img, LV_ALIGN_RIGHT_MID, -20, 0);
        }
    }
#endif

    if (txt) {
        lv_obj_t* label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        if (left_icon) {
            lv_obj_align(label, LV_ALIGN_LEFT_MID, 54, 2);
        }
        else {
            lv_obj_align(label, LV_ALIGN_LEFT_MID, 0, 5);
        }
    }

    return obj;
}

lv_obj_t* lv_list_chile_btn(lv_obj_t* list, const void* icon, const char* txt)
{
    LV_LOG_INFO("begin");
    lv_obj_t* obj = lv_obj_class_create_obj(&lv_list_btn_class, list);
    lv_obj_class_init_obj(obj);
    lv_obj_set_size(obj, LV_PCT(100), 58);
   // lv_obj_set_size(obj, LV_PCT(100), LV_SIZE_CONTENT);
   // lv_obj_set_flex_flow(obj, LV_FLEX_FLOW_ROW);

#if LV_USE_IMG == 1
    if (icon) {
        lv_obj_t* img = lv_img_create(obj);
        lv_img_set_src(img, icon);
    }
#endif

    if (txt) {
        lv_obj_t* label = lv_label_create(obj);
        lv_label_set_text(label, txt);
        lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
        lv_obj_set_flex_grow(label, 1);
        lv_obj_center(label);
    }

    return obj;
}