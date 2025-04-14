#ifndef __PAGESYSSET_H__
#define __PAGESYSSET_H__

#include "set_config.h"
#include "XMThread.h"

enum Btn_YesOrNo
{
	Btn_Yes = 1,
	Btn_No,
};



class PageSysSet
{
public:
	PageSysSet();
	~PageSysSet();

	void OpenTimeSetPage();
    void ChangeRestScreenTime(lv_event_t* e);
    void ChangeScreenList(lv_event_t* e);
	void OpenFormatPage();
	void OpenDefaultSetPage();
	void OpenEditionPage();
	void OpenSwitchCameraPage();
    void ChangeShutdownTime(lv_event_t* e);
	int ChangeTime(int key);
	void SwitchCamera(int key);
	void OpenRestScreenFun(bool flag);
	void OpenAutoShutdownFun(bool flag);
	void AddCb(lv_obj_t* list, int num);
	void SetFocusedObjStyle(lv_obj_t* obj);
	static void DeletedEvent(lv_event_t* e);
	bool BigSmallMonth(int month);
	void OpenVoiceControlPage();
	void VoiceControlKeypadEvent(int key);
    void ChangeCollisionSens(lv_event_t* e);
    void ChangeKeyToneEvent(lv_event_t* e);
    void ChangeBootToneEvent(lv_event_t* e);
	void StartUpgrade();
	void RestoreDefaultSet();
	void OpenFormatTipWin();
	void OpenWifiSetPage();
    void ChangeLanguage(lv_event_t* e);
	int setSystemTime(int year, int month, int day, int hour, int min, int sec);
	int SystemTimeToRtcTimeSet(void);
private:
	void OpenFormatProgressPage();
	void OpenEntryPage();
	void OpenUpgraderProgressPage();
	void GetRollerOptions(int head, int end, char* options, bool two);
	int JudgeDateLegal(struct _time* modify_time);
	static void FormatSD(lv_event_t* e);
	static void* FormatSDThread(void* param);
	static void SetFormatBarValue(lv_timer_t* timer);
	static void FormatEnd(lv_timer_t* timer);
	static void RestoreDefaultSetEvent(lv_event_t* e);
	static void SwitchCameraEvent(lv_event_t* e);
	static void VoiceControlSwitch(lv_event_t* e);
	static void UpgraderThread(void* param);
	static void SetBarValue(lv_timer_t* timer);
	static void ChangeSysTimeEvent(lv_event_t* e);
	static void RollerEvent(lv_event_t* e);
	static void RefreshTimePage(lv_timer_t* timer);
	static void WifiEvent(lv_event_t* e);
public:
	lv_obj_t* time_page_;
	lv_obj_t* voice_control_page_;
	int camera_dir_;
	int subpage_width_;
	int subpage_height_;
	lv_timer_t* format_bar_timer_;
	lv_obj_t* format_tip_win_;

private:
	int format_result_;
	int format_bar_value_;
	lv_obj_t* entry_page_[2];

	COSThread thread_format_SD_;
	lv_obj_t* format_progress_page_;
	lv_obj_t* format_bar_;
	lv_obj_t* upgrade_bar_;
	int64_t upgrade_start_time_;

	lv_obj_t* year_roller_;
	lv_obj_t* month_roller_;
	lv_obj_t* day_roller_;
	lv_obj_t* hour_roller_;
	lv_obj_t* min_roller_;
	lv_obj_t* yes_btn_;
	lv_obj_t* no_btn_;
	int upgrader_result_;
};

#endif // !__PAGESYSSET_H__
