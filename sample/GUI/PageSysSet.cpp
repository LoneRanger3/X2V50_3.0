#include "global_data.h"
#include "global_page.h"
#include "PageSysSet.h"
#include "set_config.h"
#include <time.h>
#ifndef WIN32
#include <unistd.h> 
#endif
#include "porting/lv_port_indev_template.h"
#include "TimeUtil.h"
#include "periphery/PeripheryManager.h"
#include "mpp/MppMdl.h"
#include "network/HTTPSerMdl.h"
#include "osd_user.h"

extern int g_sd_status;
#include "log/LogFileManager.h"
#include "gps.h"
PageSysSet::PageSysSet() :subpage_width_(screen_width), subpage_height_(size_h(298)), time_page_(NULL),
voice_control_page_(NULL), format_bar_timer_(NULL), camera_dir_(XM_PLAY_BOTH), upgrade_start_time_(0),gps_infomation_timer_(NULL)
{
	entry_page_[0] = NULL;
	entry_page_[1] = NULL;
}

PageSysSet::~PageSysSet()
{

}

void PageSysSet::OpenTimeSetPage()
{
	time_page_ = lv_obj_create(lv_scr_act());
    lv_obj_set_style_text_font(time_page_, lv_font_all, 0);
	lv_obj_add_style(time_page_, &GlobalPage::Instance()->page_set()->subpage_style_, 0);
	lv_obj_add_event_cb(time_page_, DeletedEvent, LV_EVENT_DELETE, NULL);
	lv_obj_align(time_page_, LV_ALIGN_TOP_MID, 0, size_h(50) + start_y);

	int roller_width = size_w(100), roller_space = size_w(23);

	static lv_style_t roller_style;
	lv_style_init(&roller_style);
	lv_style_set_bg_color(&roller_style, lv_color_make(66, 66, 66));
	lv_style_set_text_color(&roller_style, lv_color_make(255, 255, 255));
	lv_style_set_border_width(&roller_style, 2);
	lv_style_set_border_color(&roller_style, lv_color_make(38, 38, 38));
    lv_style_set_text_font(&roller_style, &lv_font_montserrat_30); 
	lv_style_set_radius(&roller_style, 5);

	static lv_style_t roller_selected_style;
	lv_style_init(&roller_selected_style);
	//lv_style_set_text_color(&roller_selected_style, lv_palette_darken(LV_PALETTE_ORANGE, 1));
	lv_style_set_text_font(&roller_selected_style, &lv_font_montserrat_30);
	lv_style_set_bg_color(&roller_selected_style, lv_color_make(91, 169, 151));
	lv_style_set_border_width(&roller_selected_style, 1);
	lv_style_set_border_color(&roller_selected_style, lv_color_make(38, 38, 38));
	lv_style_set_border_side(&roller_selected_style, LV_BORDER_SIDE_BOTTOM | LV_BORDER_SIDE_TOP);

	//year
	char options[256] = { 0 };
	GetRollerOptions(2022, 2037, options, false);
	year_roller_ = lv_create_roller(time_page_, roller_width, options, LV_ROLLER_MODE_NORMAL,
		3, RollerEvent, LV_EVENT_ALL, NULL);
	lv_obj_add_style(year_roller_, &roller_style, 0);
	lv_obj_add_style(year_roller_, &roller_selected_style, LV_PART_SELECTED);
	lv_roller_set_visible_row_count(year_roller_, 3);
	lv_obj_align(year_roller_, LV_ALIGN_TOP_LEFT, roller_space, size_h(50));

	//month
	char options2[256] = { 0 };
	GetRollerOptions(1, 12, options2, true);
	month_roller_ = lv_create_roller(time_page_, roller_width, options2, LV_ROLLER_MODE_NORMAL,
		3, RollerEvent, LV_EVENT_ALL, NULL);
	lv_obj_add_style(month_roller_, &roller_style, 0);
	lv_obj_add_style(month_roller_, &roller_selected_style, LV_PART_SELECTED);
	lv_roller_set_visible_row_count(month_roller_, 3);
	lv_obj_align_to(month_roller_, year_roller_, LV_ALIGN_OUT_RIGHT_MID, roller_space, 0);

	//day
	char options3[256] = { 0 };
	GetRollerOptions(1, 31, options3, true);
	day_roller_ = lv_create_roller(time_page_, roller_width, options3, LV_ROLLER_MODE_NORMAL,
		3, RollerEvent, LV_EVENT_ALL, NULL);
	lv_obj_add_style(day_roller_, &roller_style, 0);
	lv_obj_add_style(day_roller_, &roller_selected_style, LV_PART_SELECTED);
	lv_roller_set_visible_row_count(day_roller_, 3);
	lv_obj_align_to(day_roller_, month_roller_, LV_ALIGN_OUT_RIGHT_MID, roller_space, 0);

	//hour
	char options4[256] = { 0 };
	GetRollerOptions(0, 23, options4, true);
	hour_roller_ = lv_create_roller(time_page_, roller_width, options4, LV_ROLLER_MODE_NORMAL,
		3, RollerEvent, LV_EVENT_ALL, NULL);
	lv_obj_add_style(hour_roller_, &roller_style, 0);
	lv_obj_add_style(hour_roller_, &roller_selected_style, LV_PART_SELECTED);
	lv_roller_set_visible_row_count(hour_roller_, 3);
	lv_obj_align_to(hour_roller_, day_roller_, LV_ALIGN_OUT_RIGHT_MID, roller_space, 0);

	//minute
	char options5[256] = { 0 };
	GetRollerOptions(0, 59, options5, true);
	min_roller_ = lv_create_roller(time_page_, roller_width, options5, LV_ROLLER_MODE_NORMAL,
		3, RollerEvent, LV_EVENT_ALL, NULL);
	lv_obj_add_style(min_roller_, &roller_style, 0);
	lv_obj_add_style(min_roller_, &roller_selected_style, LV_PART_SELECTED);
	lv_roller_set_visible_row_count(min_roller_, 3);
	lv_obj_align_to(min_roller_, hour_roller_, LV_ALIGN_OUT_RIGHT_MID, roller_space, 0);

	//选择当前时间
	time_t t = time(NULL);
	struct tm* current_time = localtime(&t);

	lv_roller_set_selected(year_roller_, current_time->tm_year + 1900 - 2022, LV_ANIM_OFF);
	lv_roller_set_selected(month_roller_, current_time->tm_mon + 1 - 1, LV_ANIM_OFF);
	lv_roller_set_selected(day_roller_, current_time->tm_mday - 1, LV_ANIM_OFF);
	lv_roller_set_selected(hour_roller_, current_time->tm_hour, LV_ANIM_OFF);
	lv_roller_set_selected(min_roller_, current_time->tm_min, LV_ANIM_OFF);

	//btn
	yes_btn_ = lv_create_btn(time_page_, 120, 50, lv_color_make(95, 93, 93), 0, ChangeSysTimeEvent, LV_EVENT_ALL,
		(void*)Btn_Yes, 0);
	lv_obj_set_style_border_width(yes_btn_, 2, 0);                                 
	lv_obj_set_style_border_color(yes_btn_, lv_color_make(38, 38, 38), 0);
    lv_obj_set_style_radius(yes_btn_, 5, 0);
	lv_obj_set_ext_click_area(yes_btn_, 15);
	lv_obj_align(yes_btn_, LV_ALIGN_BOTTOM_MID, -size_w(100), -size_h(20));

	lv_obj_t* btn_label = lv_label_create(yes_btn_);
	lv_label_set_text(btn_label, GetParsedString("Confirm"));
	lv_obj_center(btn_label);

	no_btn_ = lv_create_btn(time_page_, 120, 50, lv_color_make(95, 93, 93), 0, ChangeSysTimeEvent, LV_EVENT_ALL,
		(void*)Btn_No, 0);
	lv_obj_set_style_border_width(no_btn_, 2, 0);
	lv_obj_set_style_border_color(no_btn_, lv_color_make(38, 38, 38), 0);
    lv_obj_set_style_radius(no_btn_, 5,0);
	lv_obj_set_ext_click_area(no_btn_, 15);
	lv_obj_align(no_btn_, LV_ALIGN_BOTTOM_MID, size_w(100), -size_h(20));

	btn_label = lv_label_create(no_btn_);
	lv_label_set_text(btn_label, GetParsedString("Cancel"));
	lv_obj_center(btn_label);
}

void PageSysSet::RollerEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_PRESSING || code == LV_EVENT_VALUE_CHANGED || code == LV_EVENT_RELEASED) {
		XMLogI("RollerEvent code = %d", code);
		PageSysSet* page_sys_set = GlobalPage::Instance()->page_sys_set();
		lv_obj_add_flag(page_sys_set->time_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(page_sys_set->time_page_, LV_OBJ_FLAG_HIDDEN);

		if (code == LV_EVENT_RELEASED) {
			lv_timer_t* timer = lv_timer_create(RefreshTimePage, 50, NULL);
			lv_timer_set_repeat_count(timer, 10);
		}
	}
}

void PageSysSet::RefreshTimePage(lv_timer_t* timer)
{
	PageSysSet* page_sys_set = GlobalPage::Instance()->page_sys_set();
	if (page_sys_set->time_page_) {
		lv_obj_add_flag(page_sys_set->time_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(page_sys_set->time_page_, LV_OBJ_FLAG_HIDDEN);
	}
}

void PageSysSet::ChangeSysTimeEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		PageSysSet* page_sys_set = GlobalPage::Instance()->page_sys_set();
		int user_data = (int)lv_event_get_user_data(e);
		if (user_data == Btn_Yes) {
			struct _time roller_time;
			char buf[32];
			memset(buf, 0, sizeof(buf));
			lv_roller_get_selected_str(page_sys_set->year_roller_, buf, 32);
			sscanf(buf, "%d", &roller_time.year);

			memset(buf, 0, sizeof(buf));
			lv_roller_get_selected_str(page_sys_set->month_roller_, buf, 32);
			sscanf(buf, "%d", &roller_time.mon);

			memset(buf, 0, sizeof(buf));
			lv_roller_get_selected_str(page_sys_set->day_roller_, buf, 32);
			sscanf(buf, "%d", &roller_time.day);

			memset(buf, 0, sizeof(buf));
			lv_roller_get_selected_str(page_sys_set->hour_roller_, buf, 32);
			sscanf(buf, "%d", &roller_time.hour);

			memset(buf, 0, sizeof(buf));
			lv_roller_get_selected_str(page_sys_set->min_roller_, buf, 32);
			sscanf(buf, "%d", &roller_time.min);

			time_t t = time(NULL);
			struct tm* current_time = localtime(&t);
			roller_time.sec = current_time->tm_sec;

			//判断日期是否合法
			int ret = page_sys_set->JudgeDateLegal(&roller_time);
			if (ret == -1) {
				GlobalPage::Instance()->page_main()->OpenTipBox("The date is illegal");
				page_sys_set->OpenRestScreenFun(true);
				page_sys_set->OpenAutoShutdownFun(true);
				return;
			}

			page_sys_set->OpenRestScreenFun(false);
			page_sys_set->OpenAutoShutdownFun(false);
		#if 0	
			//修改系统时间
			char buf1[128] = { 0 };
			sprintf(buf1, "date -s \"%d-%d-%d %d:%d:%d\"", roller_time.year, roller_time.mon,
				roller_time.day, roller_time.hour, roller_time.min, roller_time.sec);
			XMLogI("change time command string:%s", buf1);
			ret = system(buf1);
			XMLogI("system ret = %d", ret);
			if (ret != 0) {
				GlobalPage::Instance()->page_main()->OpenTipBox("The date is illegal");
				page_sys_set->OpenRestScreenFun(true);
				page_sys_set->OpenAutoShutdownFun(true);
				return;
			}
			//同步硬件时间
			std::string a("/sbin/hwclock -w");
			ret = system(a.c_str());
			XMLogI("system ret = %d", ret);
			if (ret != 0) {
				GlobalPage::Instance()->page_main()->OpenTipBox("The date is illegal");
				page_sys_set->OpenRestScreenFun(true);
				page_sys_set->OpenAutoShutdownFun(true);
				return;
			}
        #else
		//修改系统时间
		    page_sys_set->setSystemTime(roller_time.year, roller_time.mon,
				roller_time.day, roller_time.hour, roller_time.min, roller_time.sec);
			//同步硬件时间
			ret = page_sys_set->SystemTimeToRtcTimeSet();
			XMLogI("ret === %d", ret);
			if (ret != 0) {
				GlobalPage::Instance()->page_main()->OpenTipBox("The date is illegal");
				page_sys_set->OpenRestScreenFun(true);
				page_sys_set->OpenAutoShutdownFun(true);
				return;
			}
		#endif
			lv_obj_del(page_sys_set->time_page_);
			page_sys_set->time_page_ = NULL;
			GlobalPage::Instance()->page_main()->OpenTipBox("Time modified successfully", 2000);

			time_t t1 = time(NULL);
			GlobalPage::Instance()->page_main()->pre_sys_time_ = t1;
			
			page_sys_set->OpenRestScreenFun(true);
			page_sys_set->OpenAutoShutdownFun(true);
		}
		else if (user_data == Btn_No) {
			lv_obj_del(page_sys_set->time_page_);
			page_sys_set->time_page_ = NULL;
		}
	}
	else if (code == LV_EVENT_PRESSING) {
		PageSysSet* page_sys_set = GlobalPage::Instance()->page_sys_set();
		if (page_sys_set->time_page_) {
			lv_obj_add_flag(page_sys_set->time_page_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(page_sys_set->time_page_, LV_OBJ_FLAG_HIDDEN);
		}
	}
}
// 设置系统时间的函数
int PageSysSet::setSystemTime(int year, int month, int day, int hour, int min, int sec) 
{
    struct tm tm_time;
    struct timeval tv;
    
    // 设置tm_time结构体
    tm_time.tm_year = year - 1900;  // 年份需要减去1900
    tm_time.tm_mon = month - 1;     // 月份从0开始
    tm_time.tm_mday = day;          // 日期
    tm_time.tm_hour = hour;         // 小时
    tm_time.tm_min = min;           // 分钟
    tm_time.tm_sec = sec;           // 秒数
    
    // 将tm_time转换为time_t
    time_t time = mktime(&tm_time);
    
    if (time == -1) {
        XMLogE("mktime");
        return 1;
    }
    
    // 设置timeval结构体
    tv.tv_sec = time;
    tv.tv_usec = 0;
    
    // 调用settimeofday设置系统时间
    if (settimeofday(&tv, NULL) != 0) {
        XMLogE("settimeofday");
        return 1;
    }
    GlobalPage::Instance()->page_main()->pre_date_ =0;
    GlobalPage::Instance()->page_main()->pre_time_ =0;
    GlobalPage::Instance()->page_main()->pre_sys_time_ =0;
    XMLogI("System time set successfully.\n");
    
    return 0;
}
#include <sys/ioctl.h>
#include <fcntl.h>
#define RTC_SET_TIME    _IOW('p', 0x0a, struct linux_rtc_time) /* Set RTC time    */
#define DEV_RTC         "/dev/rtc"

struct linux_rtc_time {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};     

int PageSysSet::SystemTimeToRtcTimeSet(void)
{


	int			fd;
	struct tm*  ptm;
	time_t		now;
	#if 0
	if(temp_time_start ==1)
		{
		RtcTimeToSystemTimeSet();
		rtc_init();
		temp_time_start = 0;
		}
	#endif

	/*
	* Open the RTC device
	*/
	fd = open(DEV_RTC, O_RDWR);
	
	if (fd < 0)
	{
		XMLogE("Cannot Open Rtc Device\n");
		return 1;
	}

	/*
	* Grab current time and convert to chip format
	*/
	now	= time(NULL);
	ptm	= gmtime(&now);


	#if 0
	XMLogI("SystemTimeToRtcTimeSet :%d-%d-%d, today is weekday %d \ntime:%d:%d:%d\n", 
			tm.year, tm.month, tm.date,tm.weekday,
			tm.hour, tm.minute, tm.second);
	#endif
	if(ioctl(fd, RTC_SET_TIME, ptm)<0)
	{
		XMLogE ("System Time To Rtc Time Update Failed\n");
		close(fd);
		return -1;
	}


	close(fd);
//RtcTimeToSystemTimeSet();
	return 0;
} 
int PageSysSet::JudgeDateLegal(struct _time* modify_time)
{
	if (!modify_time) {
		return -1;
	}

	int a[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
	int year = modify_time->year, m = modify_time->mon, d = modify_time->day;
	if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {//如果是闰年
		a[1] = 29;
		if (d > 0 && d <= a[m - 1] && m > 0 && m <= 12)
			return 0;
		else{
            modify_time->day=a[modify_time->mon-1];
			//return -1;
			}
	}
	else {
		if (d > 0 && d <= a[m - 1] && m > 0 && 12 >= m)
			return 0;
		else{
			modify_time->day=a[modify_time->mon-1];
           //return -1;
		}
			
	}

	return 0;
}

void PageSysSet::GetRollerOptions(int head, int end, char* options, bool two)
{
	char buf[32];
	if (two) {
		for (int i = head; i <= end; i++)
		{
			sprintf(buf, "%02d\n", i);
			strcat(options, buf);
		}
	}
	else {
		for (int i = head; i <= end; i++)
		{
			sprintf(buf, "%04d\n", i);
			strcat(options, buf);
		}
	}
	options[strlen(options) - 1] = '\0';
}

bool PageSysSet::BigSmallMonth(int month) {
	if (month == 1 || month == 3 || month == 5 ||
		month == 7 || month == 8 || month == 10 || month == 12)
		return true;
	return false;
}

int PageSysSet::ChangeTime(int key)
{
	 int ret = 0;
	 int day_value[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
     int year_cnt,month_cnt;
	if (key == KEYMAP_DOWN) {
		lv_obj_t* focused_obj = lv_group_get_focused(GlobalData::Instance()->group);
		if (lv_obj_check_type(focused_obj, &lv_btn_class)) {
			if (focused_obj == yes_btn_) {
				lv_group_focus_obj(no_btn_);
			}
			else {
				lv_group_focus_obj(yes_btn_);
			}
		}
		else {
			int index_num = lv_roller_get_option_cnt(focused_obj);
		  if (focused_obj == day_roller_) {
			year_cnt = lv_roller_get_selected(year_roller_)+2022;
			month_cnt = lv_roller_get_selected(month_roller_)+1;
			index_num = day_value[month_cnt-1];
			if (month_cnt==2) {
				if ((year_cnt%4==0&& year_cnt%100!=0) ||(year_cnt % 400==0)){
					index_num = 29;
				}
			}
 }
			int index = lv_roller_get_selected(focused_obj);
			int set_index = ((index + 1) < index_num) ? (index + 1) : 0;
			lv_roller_set_selected(focused_obj, set_index, LV_ANIM_OFF);
		}
		
		lv_obj_add_flag(time_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(time_page_, LV_OBJ_FLAG_HIDDEN);
	}
	else if (key == KEYMAP_UP) {
		lv_obj_t* focused_obj = lv_group_get_focused(GlobalData::Instance()->group);
		if (lv_obj_check_type(focused_obj, &lv_btn_class)) {
			if (focused_obj == yes_btn_) {
				lv_group_focus_obj(no_btn_);
			}
			else {
				lv_group_focus_obj(yes_btn_);
			}
		}
		else {
			int index_num = lv_roller_get_option_cnt(focused_obj);
		    if (focused_obj == day_roller_) {
			year_cnt = lv_roller_get_selected(year_roller_)+2022;
			month_cnt = lv_roller_get_selected(month_roller_)+1;
			index_num = day_value[month_cnt-1];
			if (month_cnt==2) {
				if ((year_cnt%4==0&& year_cnt%100!=0) ||(year_cnt % 400==0)){
					index_num = 29;
				}
			}
 }
			int index = lv_roller_get_selected(focused_obj);
			int set_index = ((index - 1) < 0) ? (index_num - 1) : (index - 1);
			lv_roller_set_selected(focused_obj, set_index, LV_ANIM_OFF);
		}

		lv_obj_add_flag(time_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(time_page_, LV_OBJ_FLAG_HIDDEN);
	}
	else if (key == KEYMAP_MODE) {
		ret = KEYMAP_DOWN;
		lv_obj_add_flag(time_page_, LV_OBJ_FLAG_HIDDEN);
		lv_obj_clear_flag(time_page_, LV_OBJ_FLAG_HIDDEN);
	}
	else if (key == KEYMAP_OK) {
		lv_obj_t* focused_obj = lv_group_get_focused(GlobalData::Instance()->group);
		if (lv_obj_check_type(focused_obj, &lv_btn_class)) {
			ret = KEYMAP_OK;
		}
		else {
			ret = KEYMAP_DOWN;
			lv_obj_add_flag(time_page_, LV_OBJ_FLAG_HIDDEN);
			lv_obj_clear_flag(time_page_, LV_OBJ_FLAG_HIDDEN);
		}
	}
	else if (key == KEYMAP_MENU) {
		lv_obj_del(time_page_);
		time_page_ = NULL;
	}

	return ret;
}

void PageSysSet::DeletedEvent(lv_event_t* e)
{
	lv_obj_clear_flag(GlobalPage::Instance()->page_set()->sys_set_list_, LV_OBJ_FLAG_HIDDEN);
	lv_label_set_text(GlobalPage::Instance()->page_set()->title_label_, GetParsedString("Set"));
	lv_group_focus_obj(GlobalPage::Instance()->page_set()->foucused_obj);
	if (GlobalPage::Instance()->page_sys_set()->gps_infomation_timer_) {
		lv_timer_del(GlobalPage::Instance()->page_sys_set()->gps_infomation_timer_);
		GlobalPage::Instance()->page_sys_set()->gps_infomation_timer_ = NULL;
    }
}

void PageSysSet::ChangeLanguage(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		XM_CONFIG_VALUE cfg_value;
		cfg_value.int_value = user_data;
		set_language(cfg_value.int_value);
		GlobalPage::Instance()->page_main()->language_value_ = cfg_value.int_value;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Language, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Language");
		}

		if(GlobalPage::Instance()->page_main()->language_value_ == Russian){
			
			kAudioPathLan = "/mnt/custom/Audio/russian/";
		}else{
		
			kAudioPathLan = "/mnt/custom/Audio/english/";
		}

		//重新加载页面
		//GlobalPage::Instance()->page_main()->UpdateCirRecordTimeLabel();

		if (GlobalData::Instance()->opened_subpage_[0]) {
			lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
			GlobalData::Instance()->opened_subpage_[0] = NULL;
		}
		lv_obj_del(GlobalPage::Instance()->page_set()->set_page_);
		GlobalPage::Instance()->page_set()->OpenPage();
		for(int i=0;i<2;i++){
			lv_group_focus_next(GlobalData::Instance()->group);
		}
	}
}
void PageSysSet::ChangeScreenList(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        int user_data = (int)lv_event_get_user_data(e);
        lv_obj_t* target = lv_event_get_target(e);
        GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);

        XM_CONFIG_VALUE cfg_value;
        cfg_value.int_value = user_data;
        int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Lcd_Light, cfg_value);
        if (ret < 0) {
            XMLogE("set config error, opr=CFG_Operation_Lcd_Light");
        }
        else {
            PeripheryManager::Instance()->SetLight(1);
        }
        GlobalPage::Instance()->page_set()->ReturnPreMenu();
    }

}

void PageSysSet::ChangeRestScreenTime(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
		int lcd_off_time = user_data;
		lv_timer_t* timer = GlobalPage::Instance()->page_main()->rest_screen_timer_;
		if (timer) {
			if (lcd_off_time != 0) {
				lv_timer_set_period(timer, lcd_off_time * 1000);
				lv_timer_resume(timer);
				lv_timer_reset(timer);
			}
			else {
				lv_timer_pause(timer);
			}
		}

		XM_CONFIG_VALUE cfg_value;
		cfg_value.int_value = lcd_off_time;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Lcd_OffTime, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Lcd_OffTime");
		}
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}

void PageSysSet::OpenRestScreenFun(bool flag)
{
	lv_timer_t* timer = GlobalPage::Instance()->page_main()->rest_screen_timer_;
	if (flag) {
		XM_CONFIG_VALUE cfg_value;
		cfg_value.int_value = 0;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Lcd_OffTime, cfg_value);
		if (cfg_value.int_value != 0 && timer) {
			lv_timer_resume(timer);
			lv_timer_reset(timer);
		}
		GlobalPage::Instance()->page_main()->close_rest_screen_fun_ = false;
	}
	else {
		lv_timer_pause(timer);
		GlobalPage::Instance()->page_main()->close_rest_screen_fun_ = true;
	}
}

void PageSysSet::OpenFormatPage()
{
	lv_obj_t* page = lv_obj_create(lv_scr_act());
	 lv_obj_set_style_text_font(page, lv_font_all, 0);
	lv_obj_add_style(page, &GlobalPage::Instance()->page_set()->subpage_style_, 0);
	lv_obj_add_event_cb(page, DeletedEvent, LV_EVENT_DELETE, NULL);
	lv_obj_align(page, LV_ALIGN_TOP_MID, 0, size_h(50) + start_y);
	 lv_obj_set_style_bg_color(page, lv_color_make(68, 68, 68), 0);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_t* label = lv_create_label(page, subpage_width_,
		GetParsedString("Formatting will delete all data of the memory card"), LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, size_h(10));

	lv_obj_t* list = lv_list_create(page);
	lv_obj_t* btn = lv_list_chile_btn(list, NULL, GetParsedString("Cancel"));
	lv_obj_add_event_cb(btn, FormatSD, LV_EVENT_ALL, (void*)Btn_No);

	btn = lv_list_chile_btn(list, NULL, GetParsedString("Confirm"));
	lv_obj_add_event_cb(btn, FormatSD, LV_EVENT_ALL, (void*)Btn_Yes);

	GlobalPage::Instance()->page_main()->SetListStyle(list, subpage_width_, subpage_height_);
	lv_obj_align_to(list, label, LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));

	GlobalData::Instance()->opened_subpage_[0] = page;
}

void PageSysSet::OpenFormatTipWin()
{
	if(GlobalData::Instance()->stop_keypad_ == false && !format_tip_win_){
	format_tip_win_ = lv_create_page(lv_scr_act(), 400, 200, lv_palette_darken(LV_PALETTE_GREY, 2), 20, 2,
		lv_font_all, lv_color_white(), 0);
	lv_obj_align(format_tip_win_, LV_ALIGN_CENTER, 0, 0);

	lv_obj_t* label = lv_create_label(format_tip_win_, 500, GetParsedString("Please format the memory card"), LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, size_h(30));

	lv_obj_t* yes_btn = lv_create_btn(format_tip_win_, size_w(160), size_h(50),
        lv_color_make(85, 85, 85), 10, FormatSD, LV_EVENT_ALL, (void*)Btn_Yes, 0);
	/*lv_obj_set_style_border_width(yes_btn, 2, 0);
	lv_obj_set_style_border_color(yes_btn, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), 0);*/
	lv_obj_align(yes_btn, LV_ALIGN_BOTTOM_LEFT, size_w(30), -size_h(20));

	lv_obj_t* btn_label = lv_label_create(yes_btn);
	lv_label_set_text(btn_label, GetParsedString("Confirm"));
	lv_obj_center(btn_label);

	lv_obj_t* no_btn = lv_create_btn(format_tip_win_, size_w(160), size_h(50),
		lv_color_make(85,85,85), 10, FormatSD, LV_EVENT_ALL, (void*)Btn_No, 0);
	/*lv_obj_set_style_border_width(no_btn, 2, 0);
	lv_obj_set_style_border_color(no_btn, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), 0);*/
	lv_obj_align(no_btn, LV_ALIGN_BOTTOM_RIGHT, -size_w(30), -size_h(20));

	btn_label = lv_label_create(no_btn);
	lv_label_set_text(btn_label, GetParsedString("Cancel"));
	lv_obj_center(btn_label);
	}
}

void PageSysSet::FormatSD(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		PageSysSet* object = GlobalPage::Instance()->page_sys_set();
		if (user_data == Btn_Yes) {
			if (g_sd_status == XM_SD_NORMAL || g_sd_status == XM_SD_NEEDFORMAT || g_sd_status == XM_SD_NOTRW
				|| g_sd_status == XM_SD_NEEDCHECKSPEED || g_sd_status == XM_SD_NOSPACE) {
				object->OpenAutoShutdownFun(false);
				object->format_result_ = 0;
				object->format_bar_value_ = 0;
				LogFileManager::Instance()->write_log_sd_set(0,100);
				CreateThreadEx(object->thread_format_SD_, (LPTHREAD_START_ROUTINE)FormatSDThread, NULL);
				GlobalData::Instance()->stop_keypad_ = true;
				object->OpenFormatProgressPage();
			}
			else if (g_sd_status == XM_SD_ABNORMAL) {
				GlobalPage::Instance()->page_main()->OpenTipBox("The memory card is damaged, please replace it");
			}
			else {
				GlobalPage::Instance()->page_main()->OpenTipBox("Please insert SD card");
				XMLogI("g_sd_status = %d", g_sd_status);
			}
		}
		else if (user_data == Btn_No) {
			
		}
		if (GlobalData::Instance()->opened_subpage_[0]) {
			lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
			GlobalData::Instance()->opened_subpage_[0] = NULL;
		}

		if (object->format_tip_win_) {
			lv_obj_del(object->format_tip_win_);
			object->format_tip_win_ = NULL;
		}
	}
}

void* PageSysSet::FormatSDThread(void* param)
{
#ifndef WIN32
	prctl(PR_SET_NAME, "format_sdcard");
#endif
	int ret = XM_Middleware_Storage_FormatSDCard(XM_STORAGE_SDCard_0);
	int write_speed = 0;
	g_sd_status = XM_Middleware_Storage_CheckSDCard(XM_STORAGE_SDCard_0);
	if (g_sd_status == XM_SD_NORMAL) {
		XM_Middleware_Storage_WriteSpeedToFile(XM_STORAGE_SDCard_0, &write_speed);
		GlobalData::Instance()->SDCard_write_speed_ = write_speed;
		XMLogI("write_speed : %dKB/s", write_speed);
	}
  #if GPS_EN //写gps播放器下载链接
	if(gps_insert_flag){ 
		GPS_Player();
	}
  #endif
	if (ret < 0) {
		GlobalPage::Instance()->page_sys_set()->format_result_ = -1;
		XMLogE("SD card format error");
	}
	else {
		GlobalPage::Instance()->page_sys_set()->format_result_ = 1;
	}
	return 0;
}

void PageSysSet::OpenFormatProgressPage()
{
	lv_obj_t* page = lv_create_page(lv_scr_act(), screen_width, screen_height,
		lv_color_make(38, 38, 38), 0, 0, lv_font_all, lv_color_white(), 0);
	lv_obj_set_pos(page, 0, start_y);
	lv_obj_set_style_bg_opa(page, 0, 0);

	int page_width = size_w(400);
	format_progress_page_ = lv_create_page(page, page_width, size_h(150),
		lv_color_make(38, 38, 38), 0, 2, lv_font_all, lv_color_white(), 0);
	lv_obj_center(format_progress_page_);

	lv_obj_t* title_label = lv_create_label(format_progress_page_, page_width,
		GetParsedString("Format"), LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, size_h(10));

	static lv_point_t line_points[] = { {0, 0}, {(lv_coord_t)page_width, 0} };
	lv_obj_t* line = lv_create_line(format_progress_page_, 3, lv_palette_darken(LV_PALETTE_ORANGE, 1), 0,
		line_points, 2, 0);
	lv_obj_align_to(line, title_label, LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));

	format_bar_ = lv_bar_create(format_progress_page_);
	lv_obj_set_size(format_bar_, page_width, size_h(70));
	lv_obj_align_to(format_bar_, line, LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(5));
	lv_bar_set_range(format_bar_, 0, 100);
	lv_obj_set_style_radius(format_bar_, 0, 0);
	lv_obj_set_style_radius(format_bar_, 0, LV_PART_INDICATOR);
	format_bar_timer_ = lv_timer_create(SetFormatBarValue, 100, NULL);
}

void PageSysSet::SetFormatBarValue(lv_timer_t* timer)
{
	PageSysSet* object = GlobalPage::Instance()->page_sys_set();
	if (object->format_result_ == 0) {
		object->format_bar_value_ += 1;
		if (object->format_bar_value_ < 90) {
			lv_bar_set_value(object->format_bar_, object->format_bar_value_, LV_ANIM_ON);
		}
	}
	else {
		lv_bar_set_value(object->format_bar_, 100, LV_ANIM_ON);
		lv_timer_create(FormatEnd, 1000, NULL);
		object->format_bar_timer_ = NULL;
		lv_timer_del(timer);
	}
}

void PageSysSet::FormatEnd(lv_timer_t* timer)
{
	PageSysSet* object = GlobalPage::Instance()->page_sys_set();

	GlobalPage::Instance()->page_main()->SDCardImg();

	lv_obj_del(lv_obj_get_parent(object->format_progress_page_));
	GlobalData::Instance()->stop_keypad_ = false;

	if (g_sd_status != XM_SD_NORMAL) {
		XMLogI("FormatEnd g_sd_status = %d", g_sd_status);
		if (g_sd_status == XM_SD_NEEDFORMAT)
			GlobalPage::Instance()->page_main()->OpenTipBox("SD card is damaged");
		else
			GlobalPage::Instance()->page_main()->CheckSDStatus();
	}
	else {
		if (object->format_result_ == -1) {
			GlobalPage::Instance()->page_main()->OpenTipBox("Format failure");
		}
		else {
			GlobalPage::Instance()->page_main()->OpenTipBox("Format successful");
			LogFileManager::Instance()->write_log_sd_set(1,1);
		}

		if (!lv_obj_has_flag(GlobalPage::Instance()->page_main()->main_page_, LV_OBJ_FLAG_HIDDEN) && (GlobalData::Instance()->UI_mode_ == UIMode_Videotape)) {
			GlobalPage::Instance()->page_main()->StartRecord();
		}
	}

	object->OpenAutoShutdownFun(true);
	CloseThreadEx(object->thread_format_SD_);
	if (!GlobalPage::Instance()->page_main()->battery_powered_ && GlobalPage::Instance()->page_main()->acc_disconnect_==2) {
		GlobalPage::Instance()->page_main()->AccDisconnectWork();
	}
	lv_timer_del(timer);
}

void PageSysSet::OpenDefaultSetPage()
{
	lv_obj_t* page = lv_obj_create(lv_scr_act());
    lv_obj_set_style_text_font(page, lv_font_all, 0);
	lv_obj_add_style(page, &GlobalPage::Instance()->page_set()->subpage_style_, 0);
	lv_obj_add_event_cb(page, DeletedEvent, LV_EVENT_DELETE, NULL);
	lv_obj_align(page, LV_ALIGN_TOP_MID, 0, size_h(50) + start_y);
	 lv_obj_set_style_bg_color(page, lv_color_make(68, 68, 68), 0);
    lv_obj_clear_flag(page, LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_t* label = lv_create_label(page, subpage_width_,
		GetParsedString("Restore default settings"), LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, size_h(20));

	lv_obj_t* list = lv_list_create(page);
	lv_obj_t* btn = lv_list_chile_btn(list, NULL, GetParsedString("Cancel"));
	lv_obj_add_event_cb(btn, RestoreDefaultSetEvent, LV_EVENT_ALL, (void*)Btn_No);

	btn = lv_list_chile_btn(list, NULL, GetParsedString("Confirm"));
	lv_obj_add_event_cb(btn, RestoreDefaultSetEvent, LV_EVENT_ALL, (void*)Btn_Yes);

	GlobalPage::Instance()->page_main()->SetListStyle(list, subpage_width_, subpage_height_);
	lv_obj_align_to(list, label, LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(20));

	GlobalData::Instance()->opened_subpage_[0] = page;
}

void PageSysSet::RestoreDefaultSetEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		if (user_data == Btn_Yes) {
			GlobalPage::Instance()->page_sys_set()->RestoreDefaultSet();
		}
		else if (user_data == Btn_No) {
			lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
			GlobalData::Instance()->opened_subpage_[0] = NULL;
		}
	}
}
void on_reboot(lv_timer_t* timer)
{
	XM_Middleware_SafeSystem("/sbin/reboot");
    lv_timer_del(timer);
}
void PageSysSet::RestoreDefaultSet()
{
	if(GlobalData::Instance()->opened_subpage_[0]){
      lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
	  GlobalData::Instance()->opened_subpage_[0] = NULL;
	  GlobalPage::Instance()->page_main()->OpenTipBox("Factory reset, rebooting...");
	}
	lv_timer_t* timer_r=lv_timer_create(on_reboot,2000,NULL);
        
	XM_Middleware_SafeSystem("rm /mnt/mtd/Config/hostapd.conf");
	GlobalData::Instance()->car_config()->SetDefaultConfig();

	return;

	XMUIEventInParam event_inparam;
	XMUIEventOutParam event_outparam;

	//语言
	XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value = SimpChinese;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Language, cfg_value);
	set_language(cfg_value.int_value);

	//按键音
	cfg_value.bool_value = false;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Key_Voice, cfg_value);
	GlobalData::Instance()->key_tone_ = cfg_value.bool_value;

	//视频分辨率
	cfg_value.int_value = PT_H264;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Encode_Format, cfg_value);
	if (cfg_value.int_value == PT_H264) {
		event_inparam.set_enc.video_codec_type = PT_H264;
	}
	else {
		event_inparam.set_enc.video_codec_type = PT_H265;
	}

	cfg_value.int_value = 1440;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Video_Resolution, cfg_value);
	if (cfg_value.int_value == 1080) {
		event_inparam.set_enc.width = 1920;
		event_inparam.set_enc.height = 1080;
		event_inparam.set_enc.bit_rate = 4096;
		lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "1080P");
	}
	else if (cfg_value.int_value == 1440) {
		event_inparam.set_enc.width = 2560;
		event_inparam.set_enc.height = 1440;
		event_inparam.set_enc.bit_rate = 20480;
		lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "2K");
	}
	else if (cfg_value.int_value == k3KHeight) {
		event_inparam.set_enc.width = 3008;
		event_inparam.set_enc.height = k3KHeight;
		event_inparam.set_enc.bit_rate = 20480;
		lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "3K");
	}
	else if (cfg_value.int_value == k4KHeight) {
		event_inparam.set_enc.width = 3840;
		event_inparam.set_enc.height = k4KHeight;
		event_inparam.set_enc.bit_rate = 20480;
		lv_label_set_text(GlobalPage::Instance()->page_main()->record_resolution_label_, "4K");
	}
	event_inparam.set_enc.channel = Direction_Front;
	GlobalData::Instance()->ui_event_cb_(XM_UI_SET_ENCODE_PARAM, &event_inparam, &event_outparam);

	//息屏时间
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Lcd_OffTime, cfg_value);
	lv_timer_t* timer = GlobalPage::Instance()->page_main()->rest_screen_timer_;
	if (timer) {
		if (cfg_value.int_value != 0) {
			lv_timer_set_period(timer, cfg_value.int_value * 1000);
			lv_timer_resume(timer);
			lv_timer_reset(timer);
		}
		else {
			lv_timer_pause(timer);
		}
	}

	//自动关机时间
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_AutoShutdown_Time, cfg_value);
	timer = GlobalPage::Instance()->page_main()->shutdown_timer_;
	if (cfg_value.int_value == 0) {
		lv_timer_pause(timer);
	}
	else {
		lv_timer_set_period(timer, cfg_value.int_value * 1000);
		lv_timer_resume(timer);
		lv_timer_reset(timer);
	}

	//后拉镜像
	cfg_value.bool_value = false;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Behind_Mirror, cfg_value);
	event_inparam.set_mirror.chn = Direction_Behind;
	event_inparam.set_mirror.open_mirror = cfg_value.bool_value;
	GlobalData::Instance()->ui_event_cb_(XM_UI_MIRROR_SET_MIRROR, &event_inparam, &event_outparam);

	//录音开关
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Record_Voice, cfg_value);
	XM_Middleware_Storage_EnableRecordAudio(cfg_value.bool_value);
	if (cfg_value.bool_value) {
		lv_img_set_src(GlobalPage::Instance()->page_main()->silent_record_img_, image_path"mic_on.png");
	    lv_img_set_src(GlobalPage::Instance()->page_main()->mic_record_img_, image_path"i_aud_o.png");
	}
	else {
		lv_img_set_src(GlobalPage::Instance()->page_main()->silent_record_img_, image_path"mic_off.png");
	    lv_img_set_src(GlobalPage::Instance()->page_main()->mic_record_img_, image_path"i_aud.png");
	}

	//声控
	cfg_value.bool_value = true;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
	if (cfg_value.bool_value) {
	//	lv_img_set_src(GlobalPage::Instance()->page_main()->voice_control_img_, image_path"people-speak.png");
	}
	else {
		//lv_img_set_src(GlobalPage::Instance()->page_main()->voice_control_img_, image_path"people-nospeak.png");
	}

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

	//时间水印
	//cfg_value.bool_value = true;
	//GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Date_Watermark, cfg_value);
	//XM_Middleware_Encode_EnableOsdTime(cfg_value.bool_value, osd_x, osd_y);
	osd_data_init();
	//光源频率
	cfg_value.int_value = 0;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Reject_Flicker, cfg_value);
	MppMdl::Instance()->CameraSetRejectFlicker(cfg_value.int_value);
	//碰撞开机
	cfg_value.int_value = Sensitivity_Close;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Collision_Startup, cfg_value);
	if (!GlobalPage::Instance()->page_main()->wifi_enable_) {
		GlobalPage::Instance()->page_main()->UpdateCollisionStartupImg();
	}

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
	if (!GlobalPage::Instance()->page_main()->wifi_enable_) {
		GlobalPage::Instance()->page_main()->UpdateGsensorImg();
	}
	
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
	//GlobalPage::Instance()->page_main()->UpdateCirRecordTimeLabel();

	if (!GlobalPage::Instance()->page_main()->wifi_enable_) {
		//重新加载页面
		if (GlobalData::Instance()->opened_subpage_[0]) {
			lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
			GlobalData::Instance()->opened_subpage_[0] = NULL;
		}
		lv_obj_del(GlobalPage::Instance()->page_set()->set_page_);
		GlobalPage::Instance()->page_set()->set_page_ = NULL;
		GlobalPage::Instance()->page_set()->OpenPage();
		GlobalPage::Instance()->page_main()->OpenTipBox("Default settings restored");
	}
}

void PageSysSet::OpenEditionPage()
{
	lv_obj_t* page = lv_obj_create(lv_scr_act());
    lv_obj_set_style_text_font(page, lv_font_all, 0);
	lv_obj_add_style(page, &GlobalPage::Instance()->page_set()->subpage_style_, 0);
	lv_obj_add_event_cb(page, DeletedEvent, LV_EVENT_DELETE, NULL);
	lv_obj_align(page, LV_ALIGN_TOP_MID, 0, size_h(50) + start_y);
	lv_obj_add_event_cb(page, GlobalPage::Instance()->page_set()->CloseSubPage, LV_EVENT_ALL, (void*)Subpage_Edition);

	lv_obj_t* label = lv_create_label(page, subpage_width_,
		GlobalData::Instance()->version().c_str(), LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(label, LV_ALIGN_TOP_MID, 0, size_h(130));
	lv_group_add_obj(GlobalData::Instance()->group, label);

	GlobalData::Instance()->opened_subpage_[0] = page;
}

void PageSysSet::OpenSwitchCameraPage()
{
	lv_obj_t* page = lv_obj_create(lv_scr_act());
	lv_obj_add_style(page, &GlobalPage::Instance()->page_set()->subpage_style_, 0);
	lv_obj_add_event_cb(page, DeletedEvent, LV_EVENT_DELETE, NULL);
	lv_obj_align(page, LV_ALIGN_TOP_MID, 0, size_h(50) + start_y);

	lv_obj_t* list = lv_list_create(page);
	lv_obj_t* btn = lv_list_add_btn(list, NULL, GetParsedString("Camera1"));
	lv_obj_add_event_cb(btn, SwitchCameraEvent, LV_EVENT_ALL, (void*)XM_PLAY_SENSOR);

	btn = lv_list_add_btn(list, NULL, GetParsedString("Camera2"));
	lv_obj_add_event_cb(btn, SwitchCameraEvent, LV_EVENT_ALL, (void*)XM_PLAY_AD);

	btn = lv_list_add_btn(list, NULL, GetParsedString("Camera1+Camera2"));
	lv_obj_add_event_cb(btn, SwitchCameraEvent, LV_EVENT_ALL, (void*)XM_PLAY_BOTH);

	GlobalPage::Instance()->page_main()->SetListStyle(list, subpage_width_, subpage_height_);
	lv_obj_center(list);

	for (int i = 0; i < camera_dir_; i++) {
		lv_group_focus_next(GlobalData::Instance()->group);
	}
	AddCb(list, camera_dir_);

	GlobalData::Instance()->opened_subpage_[0] = page;
}

void PageSysSet::SwitchCameraEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		XMUIEventInParam event_inparam;
		XMUIEventOutParam event_outparam;
		GlobalPage::Instance()->page_sys_set()->camera_dir_ = user_data;
		XM_CONFIG_VALUE cfg_value;
		cfg_value.int_value = user_data;
		GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Camera, cfg_value);
		MppMdl::Instance()->SetUserMode((XMRealPlayMode)cfg_value.int_value);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}

void PageSysSet::SwitchCamera(int key)
{
	XMRealPlayMode play_mode;
	MppMdl::Instance()->GetMode(&play_mode);
	int camera_dir = play_mode;

	if (key == KEYMAP_UP) {
		camera_dir -= 1;
		if (camera_dir < XM_PLAY_SENSOR)
			camera_dir = XM_PLAY_BOTH;
	}
		
	if (key == KEYMAP_DOWN) {
		camera_dir += 1;
		if (camera_dir > XM_PLAY_BOTH)
			camera_dir = XM_PLAY_SENSOR;
	}
		
	int ret = GlobalPage::Instance()->page_main()->ChangeSplit(camera_dir);
	if (!ret) {
		camera_dir_ = camera_dir;
		XM_CONFIG_VALUE cfg_value;
		cfg_value.int_value = camera_dir_;
		GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Camera, cfg_value);
		MppMdl::Instance()->SetUserMode((XMRealPlayMode)cfg_value.int_value);
	}
}

void PageSysSet::ChangeShutdownTime(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
		int lcd_off_time = user_data;
		lv_timer_t* timer = GlobalPage::Instance()->page_main()->shutdown_timer_;
		if (timer) {
			if (lcd_off_time == 0) {
				lv_timer_pause(timer);
			}
			else {
				lv_timer_set_period(timer, lcd_off_time * 1000);
				lv_timer_resume(timer);
				lv_timer_reset(timer);
			}
		}
		

		XM_CONFIG_VALUE cfg_value;
		cfg_value.int_value = lcd_off_time;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_AutoShutdown_Time, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_AutoShutdown_Time");
		}
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}

void PageSysSet::OpenAutoShutdownFun(bool flag)
{
	lv_timer_t* timer = GlobalPage::Instance()->page_main()->shutdown_timer_;
	if (flag) {
		XM_CONFIG_VALUE cfg_value;
		cfg_value.int_value = 0;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_AutoShutdown_Time, cfg_value);
		if (cfg_value.int_value != 0 && timer) {
			lv_timer_resume(timer);
			lv_timer_reset(timer);
		}
	}
	else {
		lv_timer_pause(timer);
	}
}

void PageSysSet::AddCb(lv_obj_t* list, int num)
{
	// for (int i = 0; i < lv_obj_get_child_cnt(list); i++)
	// {
	// 	lv_obj_t* item = lv_obj_get_child(list, i);
	// 	if (lv_obj_check_type(item, &lv_list_btn_class))
	// 	{
	// 		lv_obj_t* cb = lv_label_create(item);
	// 		if (i == num) {
	// 			lv_label_set_text(cb, "\uF046");
	// 		}
	// 		else {
	// 			lv_label_set_text(cb, "\uF096");
	// 		}
	// 	}
	// }
}

void PageSysSet::SetFocusedObjStyle(lv_obj_t* obj)
{
	// lv_obj_t* label = NULL;
	// lv_obj_t* list = lv_obj_get_parent(obj);
	// for (int i = 0; i < lv_obj_get_child_cnt(list); i++)
	// {
	// 	lv_obj_t* item = lv_obj_get_child(list, i);
	// 	if (lv_obj_check_type(item, &lv_list_btn_class))
	// 	{
	// 		label = lv_obj_get_child(item, 1);
	// 		if (strcmp("\uF096", lv_label_get_text(label)) != 0) {
	// 			lv_label_set_text(label, "\uF096");
	// 		}
	// 	}
	// }

	// label = lv_obj_get_child(obj, 1);
	// if (lv_obj_check_type(label, &lv_label_class))
	// {
	// 	lv_label_set_text(label, "\uF046");
	// }
}

void PageSysSet::OpenVoiceControlPage()
{
	lv_obj_t* page = lv_obj_create(lv_scr_act());
    lv_obj_set_style_text_font(page, lv_font_all, 0);
    lv_obj_set_scroll_dir(page,LV_DIR_VER);
	lv_obj_add_style(page, &GlobalPage::Instance()->page_set()->subpage_style_, 0);
	lv_obj_add_event_cb(page, DeletedEvent, LV_EVENT_DELETE, NULL);
	lv_obj_align(page, LV_ALIGN_TOP_MID, 0, size_h(50) + start_y);

	lv_obj_t* list = lv_list_create(page);
	lv_obj_t* btn = lv_list_add_btn(list, NULL, GetParsedString("Voice control"));
	lv_obj_t* sw = lv_switch_create(btn);
	GlobalPage::Instance()->page_set()->SetSwitchStyle(sw);
	lv_group_remove_obj(sw);
	lv_obj_clear_flag(sw, LV_OBJ_FLAG_CLICKABLE);
	XM_CONFIG_VALUE cfg_value;
	cfg_value.bool_value = false;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
	if (cfg_value.bool_value) {
		lv_obj_add_state(sw, LV_STATE_CHECKED);
	}
	lv_obj_add_event_cb(btn, VoiceControlSwitch, LV_EVENT_ALL, (void*)sw);

	GlobalPage::Instance()->page_main()->SetListStyle(list, subpage_width_, 70);
	lv_obj_align(list, LV_ALIGN_TOP_MID, 0, 0);

	voice_control_page_ = page;
	OpenEntryPage();
	
	GlobalData::Instance()->opened_subpage_[0] = page;
}
void close_voice_control_lay(lv_timer_t* timer)
{
    GlobalPage::Instance()->page_sys_set()->VoiceControlKeypadEvent(KEYMAP_MENU);

    lv_timer_del(timer);
}
void PageSysSet::VoiceControlSwitch(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		lv_obj_t* user_data = (lv_obj_t*)lv_event_get_user_data(e);
		XM_CONFIG_VALUE cfg_value;
		cfg_value.bool_value = false;
		GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Voice_Control, cfg_value);
		if (cfg_value.bool_value) {
			cfg_value.bool_value = false;
			lv_obj_clear_state(user_data, LV_STATE_CHECKED);
		}
		else {
			cfg_value.bool_value = true;
			lv_obj_add_state(user_data, LV_STATE_CHECKED);
		}

		// if (cfg_value.bool_value) {
		// 	lv_img_set_src(GlobalPage::Instance()->page_main()->voice_control_img_, image_path"people-speak.png");
		// }
		// else {
		// 	lv_img_set_src(GlobalPage::Instance()->page_main()->voice_control_img_, image_path"people-nospeak.png");
		// }

		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Voice_Control, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Voice_Control");
		}
		// GlobalPage::Instance()->page_sys_set()->VoiceControlKeypadEvent(KEYMAP_MENU);
		lv_timer_create(close_voice_control_lay, 300, NULL);
	}
}

void PageSysSet::OpenEntryPage()
{
	lv_obj_t* entry_label[12] = { 0 };
	std::string entry[12] = { GetParsedString("Open prerecord"), GetParsedString("Open postrecord"), GetParsedString("Open screen"),
		GetParsedString("Close screen"), GetParsedString("View All"), GetParsedString("Open sound record"), GetParsedString("Close sound record"),
		GetParsedString("I want pictures"), GetParsedString("I want a video"), GetParsedString("Emergency video"),
		GetParsedString("Open hotspot"), GetParsedString("Turn off hotspots") };

	XM_CONFIG_VALUE cfg_value;
	cfg_value.int_value = SimpChinese;
	GlobalData::Instance()->car_config()->GetValue(CFG_Operation_Language, cfg_value);
	if (cfg_value.int_value == SimpChinese) {
		if (entry_page_[0]) return;

		entry_page_[0] = lv_obj_create(voice_control_page_);
        lv_obj_set_style_text_font(entry_page_[0], lv_font_all, 0);
		lv_obj_add_style(entry_page_[0], &GlobalPage::Instance()->page_set()->subpage_style_, 0);
		lv_obj_set_size(entry_page_[0], screen_width, size_h(240));
		lv_obj_align(entry_page_[0], LV_ALIGN_BOTTOM_MID, 0, 0);
		lv_obj_add_event_cb(entry_page_[0], GlobalPage::Instance()->page_set()->CloseSubPage, LV_EVENT_ALL, (void*)Subpage_VoiceControl);

		for (int i = 0; i < 12; i++) {
			entry_label[i] = lv_create_label(entry_page_[0], size_w(213), entry[i].c_str(),
				LV_TEXT_ALIGN_CENTER, 0);

			if(i == 0)
				lv_obj_align(entry_label[i], LV_ALIGN_TOP_LEFT, 0, size_h(20));
			else if(i == 4)
				lv_obj_align(entry_label[i], LV_ALIGN_TOP_MID, 0, size_h(20));
			else if(i == 8)
				lv_obj_align(entry_label[i], LV_ALIGN_TOP_RIGHT, 0, size_h(20));
			else 
				lv_obj_align_to(entry_label[i], entry_label[i - 1], LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(20));
		}
	}
	else {
		if (entry_page_[0]) return;

		entry_page_[0] = lv_obj_create(voice_control_page_);
        lv_obj_set_style_text_font(entry_page_[0], lv_font_all, 0);
		lv_obj_add_style(entry_page_[0], &GlobalPage::Instance()->page_set()->subpage_style_, 0);
		lv_obj_set_size(entry_page_[0], screen_width, size_h(240));
		lv_obj_align(entry_page_[0], LV_ALIGN_BOTTOM_MID, 0, 0);
		lv_obj_add_event_cb(entry_page_[0], GlobalPage::Instance()->page_set()->CloseSubPage, LV_EVENT_ALL, (void*)Subpage_VoiceControl);

		for (int i = 0; i < 12; i++) {
			entry_label[i] = lv_create_label(entry_page_[0], size_w(320), entry[i].c_str(),
			LV_TEXT_ALIGN_CENTER, 0);
			
			if(i == 0)
				lv_obj_align(entry_label[i], LV_ALIGN_TOP_LEFT, 0, size_h(8));
			else if(i == 6)
				lv_obj_align(entry_label[i], LV_ALIGN_TOP_RIGHT, 0, size_h(8));
			else 
				lv_obj_align_to(entry_label[i], entry_label[i - 1], LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));
		}
	}
}

void PageSysSet::VoiceControlKeypadEvent(int key)
{
	if (key == KEYMAP_MENU || key == KEYMAP_MODE) {
		if (GlobalData::Instance()->opened_subpage_[0]) {
			lv_obj_del(GlobalData::Instance()->opened_subpage_[0]);
			GlobalData::Instance()->opened_subpage_[0] = NULL;
		}
		
		voice_control_page_ = NULL;
		entry_page_[0] = NULL;
		entry_page_[1] = NULL;
	}
	else if (key == KEYMAP_UP || key == KEYMAP_DOWN) {
		OpenEntryPage();
	}
}

void PageSysSet::ChangeCollisionSens(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		XM_CONFIG_VALUE cfg_value;
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		PageSet* page_set = GlobalPage::Instance()->page_set();
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);

		cfg_value.int_value = user_data;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Collision_Sensitivity, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Collision_Sensitivity");
		}

		PeripheryManager::Instance()->SensitivitySet(cfg_value.int_value);
		GlobalPage::Instance()->page_main()->UpdateGsensorImg();
		if (cfg_value.int_value == Sensitivity_Close) {
			PeripheryManager::Instance()->GsensorModeSelect(2);
		}
		else {
			PeripheryManager::Instance()->GsensorModeSelect(0);
		}
		
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}

void PageSysSet::StartUpgrade()
{
	upgrader_result_ = 0;
	OS_THREAD thread_upgrader;
	CreateThreadEx(thread_upgrader, (LPTHREAD_START_ROUTINE)UpgraderThread, NULL);

	OpenUpgraderProgressPage();
}

void PageSysSet::UpgraderThread(void* param)
{
	GlobalPage::Instance()->page_sys_set()->upgrader_result_ =     XM_Middleware_SDCard_Upgrade(kUpdateFileName);
}

void PageSysSet::OpenUpgraderProgressPage()
{
	lv_obj_set_style_bg_color(lv_scr_act(), lv_color_make(80, 150, 190), 0);
	int page_width = size_w(440);
	lv_obj_t* upgrade_progress_page_ = lv_create_page(lv_scr_act(), page_width, size_h(150),
		lv_color_make(38, 38, 38), 0, 2, lv_font_all, lv_color_white(), 0);
	lv_obj_center(upgrade_progress_page_);

	lv_obj_t* title_label = lv_create_label(upgrade_progress_page_, page_width,
		GetParsedString("System upgrade"), LV_TEXT_ALIGN_CENTER, 0);
	lv_obj_align(title_label, LV_ALIGN_TOP_MID, 0, 10);

	static lv_point_t line_points[] = { {0, 0}, {(lv_coord_t)page_width, 0} };
	lv_obj_t* line = lv_create_line(upgrade_progress_page_, 3, lv_palette_darken(LV_PALETTE_ORANGE, 1), 0,
		line_points, 2, 0);
	lv_obj_align_to(line, title_label, LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(10));

	upgrade_bar_ = lv_bar_create(upgrade_progress_page_);
	lv_obj_set_size(upgrade_bar_, page_width, size_h(65));
	lv_obj_align_to(upgrade_bar_, line, LV_ALIGN_OUT_BOTTOM_MID, 0, size_h(5));
	lv_bar_set_range(upgrade_bar_, 0, 100);
	lv_obj_set_style_radius(upgrade_bar_, 0, 0);
	lv_obj_set_style_radius(upgrade_bar_, 0, LV_PART_INDICATOR);

	upgrade_start_time_ = GetTickTime();
	lv_timer_create(SetBarValue, 500, NULL);
}

void PageSysSet::SetBarValue(lv_timer_t* timer)
{
	PageSysSet* object = GlobalPage::Instance()->page_sys_set();

	//升级失败提示
	if (object->upgrader_result_ < 0) {
		printf("Upgrade failed");
		GlobalPage::Instance()->page_main()->OpenTipBox("Upgrade failed", 0);
		lv_obj_del(lv_obj_get_parent(object->upgrade_bar_));
		lv_timer_del(timer);
		return;
	}

	int64_t start_time = object->upgrade_start_time_;
	int64_t now = GetTickTime();
	int upgrader_time = (now - start_time) / 1000;

	int progress = XM_Middleware_SDCard_GetUpgradeProgress();
	printf("Get upgrade progress = %d\n", progress);
	lv_bar_set_value(object->upgrade_bar_, progress, LV_ANIM_ON);
	if (progress == 0 && upgrader_time > 10) {
		printf("10s progress = 0, about to restart!\n");
		system("reboot");
	}
}


void PageSysSet::ChangeKeyToneEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
		
		XM_CONFIG_VALUE cfg_value;
		cfg_value.bool_value = user_data;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Key_Voice, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Key_Voice");
		}

		GlobalData::Instance()->key_tone_ = cfg_value.bool_value;
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}
void PageSysSet::ChangeBootToneEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
		
		XM_CONFIG_VALUE cfg_value;
		cfg_value.int_value = user_data;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_boot_Voice, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_boot_Voice");
		}
        if (cfg_value.int_value==Volume_Close) {
			XM_Middleware_Sound_SetVolume(0);
		} else if (cfg_value.int_value==Volume_Low) {
			XM_Middleware_Sound_SetVolume(33);
		} else if (cfg_value.int_value==Volume_Mid) {
			XM_Middleware_Sound_SetVolume(66);
		} else {
			XM_Middleware_Sound_SetVolume(100);
		}
		GlobalPage::Instance()->page_main()->playsound_flag_ = cfg_value.int_value;
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}

void PageSysSet::ChangeFatigueReminder(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
		lv_obj_t* target = lv_event_get_target(e);
		GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);
		
		XM_CONFIG_VALUE cfg_value;
		cfg_value.int_value = user_data;
		int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Fatigue_reminder, cfg_value);
		if (ret < 0) {
			XMLogE("set config error, opr=CFG_Operation_Fatigue_reminder");
		}
		GlobalPage::Instance()->page_main()->Fatigue_reminder_value=cfg_value.int_value;
		GlobalPage::Instance()->page_set()->ReturnPreMenu();
	}
}

void PageSysSet::OpenWifiSetPage()
{
	lv_obj_t* page = lv_obj_create(lv_scr_act());
    lv_obj_set_style_text_font(page, lv_font_all, 0);
	lv_obj_add_style(page, &GlobalPage::Instance()->page_set()->subpage_style_, 0);
	lv_obj_add_event_cb(page, DeletedEvent, LV_EVENT_DELETE, NULL);
	lv_obj_align(page, LV_ALIGN_TOP_MID, 0, size_h(50) + start_y);
	lv_obj_set_scrollbar_mode(page, LV_SCROLLBAR_MODE_OFF);

	lv_obj_t* qr_img = lv_img_create(page);
	lv_img_set_src(qr_img, image_path"qr_code.png");
	lv_obj_align(qr_img, LV_ALIGN_TOP_LEFT, size_w(100), size_h(30));

	WIFI_MAC_PARAM_S param;
	memset(param.ssid, 0, sizeof(param.ssid));
	memset(param.password, 0, sizeof(param.password));
	XM_Middleware_WIFI_GetMacParam(&param);

	char text[128] = { 0 };
	memset(text, 0, sizeof(text));
	sprintf(text, "%s:%s", GetParsedString("WiFi name"), param.ssid);
	lv_obj_t* label = lv_label_create(page);
	lv_label_set_text(label, text);
	lv_obj_align_to(label, qr_img, LV_ALIGN_OUT_RIGHT_MID, size_w(10), -size_h(10));

	char text1[128] = { 0 };
	memset(text1, 0, sizeof(text1));
	sprintf(text1, "%s:%s", GetParsedString("WiFi password"), param.password);
	lv_obj_t* label1 = lv_label_create(page);
	lv_label_set_text(label1, text1);
	lv_obj_align_to(label1, label, LV_ALIGN_OUT_BOTTOM_LEFT, 0, size_h(10));
#if SEVEN_KEYS
  if(!GlobalPage::Instance()->page_main()->touch_plan){
	lv_obj_t* label2 = lv_label_create(page);
	lv_label_set_text(label2, GetParsedString("Recording mode, down key to control WiFi switch"));
	lv_obj_align_to(label2, GlobalPage::Instance()->page_set()->title_label_, LV_ALIGN_OUT_BOTTOM_MID, -size_w(0), size_h(200));
  }
 #endif 
	lv_obj_t* yes_btn = lv_create_btn(page, 100, 50, lv_color_make(95, 93, 93), 0, WifiEvent, LV_EVENT_ALL,
		(void*)Btn_Yes, 0);
	// lv_obj_set_style_border_width(yes_btn, 2, 0);
	// lv_obj_set_style_border_color(yes_btn, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), 0);
	lv_obj_set_ext_click_area(yes_btn, 15);
	lv_obj_align(yes_btn, LV_ALIGN_BOTTOM_MID, -size_w(100), -size_h(20));

	lv_obj_t* btn_label = lv_label_create(yes_btn);
	lv_label_set_text(btn_label, GetParsedString("Open"));
	lv_obj_center(btn_label);

	lv_obj_t* no_btn = lv_create_btn(page, 100, 50, lv_color_make(95, 93, 93), 0, WifiEvent, LV_EVENT_ALL,
		(void*)Btn_No, 0);
	// lv_obj_set_style_border_width(no_btn, 2, 0);
	// lv_obj_set_style_border_color(no_btn, lv_palette_lighten(LV_PALETTE_BLUE_GREY, 2), 0);
	lv_obj_set_ext_click_area(no_btn, 15);
	lv_obj_align(no_btn, LV_ALIGN_BOTTOM_MID, size_w(100), -size_h(20));

	btn_label = lv_label_create(no_btn);
	lv_label_set_text(btn_label, GetParsedString("Close"));
	lv_obj_center(btn_label);

    XM_CONFIG_VALUE cfg_value;
    cfg_value.int_value = 0;
    GlobalData::Instance()->car_config()->GetValue(CFG_Operation_WiFi, cfg_value);


    if (cfg_value.int_value==0) {
        lv_group_focus_next(GlobalData::Instance()->group);
    }

	GlobalData::Instance()->opened_subpage_[0] = page;
}

void PageSysSet::WifiEvent(lv_event_t* e)
{
	lv_event_code_t code = lv_event_get_code(e);
	if (code == LV_EVENT_CLICKED) {
		int user_data = (int)lv_event_get_user_data(e);
        XM_CONFIG_VALUE cfg_value;
		if (user_data == Btn_Yes) {
			if (GlobalPage::Instance()->page_main()->wifi_prepared_) {
				GlobalPage::Instance()->page_set()->ReturnPreMenu();
				lv_obj_del(GlobalPage::Instance()->page_set()->set_page_);
				GlobalPage::Instance()->page_set()->set_page_ = NULL;
				GlobalPage::Instance()->page_main()->LeaveMainPageWork(false);
                /*	if(GlobalData::Instance()->UI_mode_ == UIMode_Photograph)
					GlobalPage::Instance()->page_main()->ChangeMode(UIMode_Videotape);*/
				//lv_event_send(GlobalPage::Instance()->page_main()->wifi_img_, LV_EVENT_CLICKED, NULL);
				GlobalPage::Instance()->page_main()->WifiEnable(true);
                lv_img_set_src(GlobalPage::Instance()->page_main()->wifi_img_, image_path"0wifi.png");
			}
			else {
				GlobalPage::Instance()->page_main()->OpenTipBox("No WiFi detected");
			}
		}
		else if (user_data == Btn_No) {
			GlobalPage::Instance()->page_set()->ReturnPreMenu();
			//lv_event_send(GlobalPage::Instance()->page_main()->wifi_img_, LV_EVENT_CLICKED, NULL);
			GlobalPage::Instance()->page_main()->WifiEnable(false);
            lv_img_set_src(GlobalPage::Instance()->page_main()->wifi_img_, image_path"no_wifi.png");
		}
      //  GlobalData::Instance()->car_config()->SetValue(CFG_Operation_WiFi, cfg_value);
	}
}
void PageSysSet::OpenGpsInfoPage()
{
    if (GlobalData::Instance()->opened_subpage_[0]) return;
    lv_obj_t* page = lv_create_page(lv_scr_act(), screen_width, screen_height - 4, lv_color_make(38, 38, 38), 0, 2,
        lv_font_all, lv_color_white(), 0);
    lv_obj_align(page, LV_ALIGN_TOP_MID, 0, size_h(0) + start_y);
    lv_obj_add_style(page, &GlobalPage::Instance()->page_set()->subpage_style_, 0);
    lv_obj_add_event_cb(page, DeletedEvent, LV_EVENT_DELETE, NULL);
    lv_obj_add_event_cb(page, GlobalPage::Instance()->page_set()->CloseSubPage, LV_EVENT_ALL, (void*)Subpage_Edition);

    static lv_point_t line_points[] = { {0, 80}, {200, 80} };
    lv_create_line(page, 2, lv_color_make(255, 255, 255), 0, line_points, 2, 0);

    // static lv_point_t line_points1[] = { {210, 0}, {210, 80} };
    // lv_create_line(page, 2, lv_color_make(255, 255, 255), 0, line_points1, 2, 0);

    static lv_point_t line_points2[] = { {200, 0}, {200, screen_height} };
    lv_create_line(page, 2, lv_color_make(255, 255, 255), 0, line_points2, 2, 0);

    static lv_point_t line_points3[] = { {200, 120}, {screen_width, 120} };
    lv_create_line(page, 2, lv_color_make(255, 255, 255), 0, line_points3, 2, 0);

    // static lv_point_t line_points4[] = { {420, 240}, {screen_width, 240} };
    // lv_create_line(page, 2, lv_color_make(255, 255, 255), 0, line_points4, 2, 0);
    for (int i = 0; i < 10; i++) {
        gps_bar_graph[i] = lv_create_page(page, 15, 0, lv_color_make(0, 255, 0), 0, 0,
            lv_font_all, lv_color_white(), 0);
        lv_obj_set_size(gps_bar_graph[i], 15, 0);
        lv_obj_align(gps_bar_graph[i], LV_ALIGN_BOTTOM_LEFT, size_w(5) + 19 * i, -size_h(20));
        gps_labe_graph[i] = lv_label_create(page);
        lv_label_set_text(gps_labe_graph[i], " ");
        lv_obj_set_style_text_font(gps_labe_graph[i], &lv_font_montserrat_10, 0);
        lv_obj_align_to(gps_labe_graph[i], gps_bar_graph[i], LV_ALIGN_BOTTOM_MID, size_w(0), size_h(15));
    }

    lv_obj_t* label = lv_create_label(page, size_w(130), GetParsedString("GPS status"), LV_TEXT_ALIGN_CENTER, 0);
    gps_status_label = lv_create_label(page, size_w(100), " ", LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, size_w(5), size_h(10));
    lv_obj_align_to(gps_status_label, label, LV_ALIGN_BOTTOM_MID, size_w(0), size_h(30));

    // label = lv_create_label(page, size_w(210), GetParsedString("Altitude(m)"), LV_TEXT_ALIGN_CENTER, 0);
    // Altitude_label= lv_create_label(page, size_w(210), " ", LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align(label, LV_ALIGN_TOP_LEFT, size_w(210), size_h(10));
    // lv_obj_align_to(Altitude_label,label, LV_ALIGN_BOTTOM_MID, size_w(0), size_h(30));

    // label = lv_create_label(page, size_w(210), GetParsedString("orientation(degree)"), LV_TEXT_ALIGN_CENTER, 0);
    // orientation_label= lv_create_label(page, size_w(210), " ", LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align(label, LV_ALIGN_TOP_LEFT, size_w(420), size_h(10));
    // lv_obj_align_to(orientation_label,label, LV_ALIGN_BOTTOM_MID, size_w(0), size_h(30));

    longitude_label = lv_create_label(page, size_w(210), " ", LV_TEXT_ALIGN_CENTER, 0);
    latitude_label = lv_create_label(page, size_w(210), " ", LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(longitude_label, &lv_font_montserrat_10, 0);
    lv_obj_set_style_text_font(latitude_label, &lv_font_montserrat_10, 0);
    lv_obj_align(longitude_label, LV_ALIGN_TOP_LEFT, size_w(150), size_h(20));
    lv_obj_align_to(latitude_label, longitude_label, LV_ALIGN_BOTTOM_MID, size_w(2), size_h(20));

    // label = lv_create_label(page, size_w(210), GetParsedString("speed"), LV_TEXT_ALIGN_CENTER, 0);
    // gps_speed_label = lv_create_label(page, size_w(210), " ", LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align(label, LV_ALIGN_TOP_LEFT, size_w(420), size_h(170));
    // lv_obj_align_to(gps_speed_label, label, LV_ALIGN_BOTTOM_MID, size_w(0), size_h(30));

    label = lv_create_label(page, size_w(210), "UTC", LV_TEXT_ALIGN_CENTER, 0);
    utc_label = lv_create_label(page, size_w(210), " ", LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_style_text_font(utc_label, &lv_font_montserrat_10, 0);
    lv_obj_align(label, LV_ALIGN_TOP_LEFT, size_w(150), size_h(140));
    lv_obj_align_to(utc_label, label, LV_ALIGN_BOTTOM_MID, size_w(0), size_h(20));

    // label = lv_create_label(page, size_w(210), GetParsedString("visual"), LV_TEXT_ALIGN_CENTER, 0);
    // visual_label = lv_create_label(page, size_w(40), " ", LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align(label, LV_ALIGN_TOP_LEFT, size_w(0), size_h(90));
    // lv_obj_align_to(visual_label, label, LV_ALIGN_OUT_RIGHT_MID, -size_w(60), size_h(0));

    // label = lv_create_label(page, size_w(210), GetParsedString("available"), LV_TEXT_ALIGN_CENTER, 0);
    // available_label = lv_create_label(page, size_w(40), " ", LV_TEXT_ALIGN_CENTER, 0);
    // lv_obj_align(label, LV_ALIGN_TOP_LEFT, size_w(200), size_h(90));
    // lv_obj_align_to(available_label, label, LV_ALIGN_OUT_RIGHT_MID, -size_w(60), size_h(0));

    if (!gps_infomation_timer_)
        gps_infomation_timer_ = lv_timer_create(update_gps_infomation_ui, 500, NULL);
    lv_group_add_obj(GlobalData::Instance()->group, page);
    GlobalData::Instance()->opened_subpage_[0] = page;
    gps_info_page_ = page;
}

#if OSD_SHOW_ADJUST
extern int osd_time_ofs_y;
#endif
#include "mpp/MppMdl.h"
#include "DemoDef.h"

void PageSysSet::GpsWatermark(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        int user_data = (int)lv_event_get_user_data(e);
        lv_obj_t* target = lv_event_get_target(e);
        GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);

        XM_CONFIG_VALUE cfg_value;
        cfg_value.bool_value = user_data;
        int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_GPS_Watermark, cfg_value);
        if (ret < 0) {
            XMLogE("set config error, opr=CFG_Operation_GPS_Watermark");
        }
		
#if 0//OSD_SHOW_ADJUST
		if(user_data){
			
			osd_time_ofs_y = OSD_TIME_ADJUST_Y;

		}else{
		
			//clean_gps_osd_data(0);
			osd_time_ofs_y = OSD_GPS_ADJUST_Y;
		}
		
		//MppMdl::Instance()->EnableOsdTime(4,1, 128, 8192*(kSubStreamHeight-60-(kSubStreamHeight/360)*8)/kSubStreamHeight + osd_time_ofs_y);
#endif
		
		if(cfg_value.bool_value){
			
		    GlobalData::Instance()->car_config()->SetValue(CFG_Operation_Date_Watermark, cfg_value);
		}
		
		osd_data_init();

        GlobalPage::Instance()->page_set()->ReturnPreMenu();
    }
}
void PageSysSet::ChangeSpeedUnit(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_CLICKED) {
        int user_data = (int)lv_event_get_user_data(e);
        lv_obj_t* target = lv_event_get_target(e);
        GlobalPage::Instance()->page_sys_set()->SetFocusedObjStyle(target);

        XM_CONFIG_VALUE cfg_value;
        cfg_value.int_value = user_data;
        int ret = GlobalData::Instance()->car_config()->SetValue(CFG_Operation_GPS_Unit, cfg_value);
        if (ret < 0) {
            XMLogE("set config error, opr=CFG_Operation_GPS_Unit");
        }
        GlobalPage::Instance()->page_set()->ReturnPreMenu();
    }
}
