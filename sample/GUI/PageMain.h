#ifndef __PAGEMAIN_H__
#define __PAGEMAIN_H__

#include <mutex>
#include "global_data.h"

enum CollisionStartup
{
	CollisionStartup_StopRecord,	//碰撞开机，ACC断开时，紧急录像结束
	CollisionStartup_VehicleStall,	//碰撞开机，紧急录像时，熄火
	CollisionStartup_SDCardError,	//碰撞开机，ACC断开时，SD卡没有或不正常
};

enum RecordPageBtnFlag
{
	RecordPageBtnFlag_RecordSound,
	RecordPageBtnFlag_PlayPthoto,
	RecordPageBtnFlag_Record,
	RecordPageBtnFlag_Set,
	RecordPageBtnFlag_Playback,
	RecordPageBtnFlag_Lock,
	RecordPageBtnFlag_Wifi,
};

class PageMain
{
public:
	PageMain();
	~PageMain();

	void OpenPage();
	void OpenTipBox(const char* string, uint32_t timer_period = 3000, bool switch_string = true, int delay_time = 0, int w = 220, int h = 140);
	void OpenShutDownTip(const char* string);
	void CfgInit();
	void StartRecord(bool compact_record = false);
	void CloseRecord(bool compact_record = false);
	void TakePic();
	int ChangeMode(UI_MODE_E mode, bool return_selectfile_page = false);
	void RecordPicModeKeypad(int key);
	void CheckSDStatus(bool checked_speed = true, bool need_speed_tip = true, bool need_check_speed_tip = true, bool nospace_tip = true);
	void ShutDown(ShutDownMode shutdownmode, bool play_pic, bool close_preview);
	void OpenPicTip(const char* pic, const char* string);
	void PWREvent();
	void SetListStyle(lv_obj_t* list, lv_coord_t width, lv_coord_t height);
	void LeaveMainPageWork(bool leave);
	void ClosePreview(bool delay);
	int ChangeSplit(int camera_dir, bool show_tip = true);
	void CloseScreen();
	void OpenScreen();
	void OpenSDSpeedTipBox(int write_speed, bool low_speed, int delay_time = 0);
	void SDCardImg();
	void AccDisconnectWork();
	void AccConnectWork();
	void ReverseEvent(bool start);
	void ReturnMainPage();
	void LockCurrentFile();
	int CollisionStartupWork(CollisionStartup flag);
	void ChangeCarChargerCfg(bool car_charger);
	void OnADInsert(bool show_tip);
	void OnADExit();
	void UpdateCirRecordTimeLabel();
	void PlayBackModeReturnRecordWork();
	void UpdateGsensorImg();
	void UpdateCollisionStartupImg();
	void RecordOnStartup();//开机后开启录像
	bool CollisionStartWithBattery(); //是否电池供电下碰撞开机
    void WIFITipWin();
    static void WiFi_switch(lv_event_t* e);
	void Voice_prompts(const char* file_name);
	static void DelayShutDownTimer(lv_timer_t* timer);
	static void AppDisconnectTimer(lv_timer_t* timer);
	static void ShutDownTimer_PullPowerOn(lv_timer_t* timer);
	void ChangeMode_Video(void);
	int WifiEnable(bool enable);
	void AppMenuValue(void);
	void Remaining_update_time();
    void Function_bar(bool flag);
	void HandOpenTipBox(void);
	void PlaySdCardStatus(void);
	
private:
	void CreatePage();
	void CompactRecord();
	void ExitCompactRecord();
	
	static void UpdateTime(lv_timer_t* timer);
	static void CloseTipBox(lv_timer_t* timer);
	static void ChangeRecordTime(lv_timer_t* timer);
	static void UpdateRestScreenTime(lv_event_t* e);
	static void RestScreen(lv_timer_t* timer);
	static void ClearMainPageHidden(lv_timer_t* timer);
	static void UpdateShutdownTime(lv_event_t* e);
	static void AutoShutdownTimer(lv_timer_t* timer);
	static void ShutDownTimer(lv_timer_t* timer);
	static void ClosePreviewTimer(lv_timer_t* timer);
	static void OpenTipBoxTimer(lv_timer_t* timer);
	static void CompactRecordTimer(lv_timer_t* timer);
	static void ExitCompactRecordTimer(lv_timer_t* timer);
	static void BtnEvent(lv_event_t* e);

public:
	lv_obj_t* wifi_tip_win_;
	lv_obj_t* record_resolution_label_;
	lv_obj_t* main_page_;
	lv_obj_t* Function_page_;
	lv_obj_t* Settings_function_page_;
	lv_obj_t* SD_img_;
	lv_obj_t* mode_label_;
	lv_obj_t* logo_label_;
	lv_obj_t* silent_record_img_;
	lv_obj_t* x2_logo_img_;
	lv_obj_t* mic_record_img_;
	lv_obj_t* battery_img_;
	lv_obj_t* record_time_label_;
	lv_obj_t* remain_time_label_;
	lv_obj_t* circular_record_time_label_;
	lv_obj_t* g_sensor_img_;
	lv_obj_t* voice_control_img_;
	lv_obj_t* collision_startup_img_;
	lv_timer_t* rest_screen_timer_;
	lv_timer_t* shutdown_timer_;
	lv_timer_t* record_timer_;
	lv_timer_t* acc_disconnect_timer_;
	lv_timer_t* acc_connect_timer_;
	lv_timer_t* app_disconnect_timer_;
	uint32_t MY_EVENT_SCREEN_CLICKED;
	bool rest_screen_;					//是否关闭屏幕变量
	bool close_rest_screen_fun_;		//是否关闭息屏功能变量
	int acc_disconnect_;				//acc是否断开变量
	int collision_startup_;				//是否是碰撞开机变量(录完紧急录像后会置为0)
	bool lock_current_recording_file_;	//是否正在录制加锁录像（加锁键控制）
	bool car_charger_;					//是否是车充
	bool battery_powered_;				//是否是电池供电
	bool connect_computer_;
	bool check_battery_;
	int battery_level_;
	int touch_plan;                     //触摸方案标志
	lv_obj_t* lock_img_;
	lv_obj_t* lock_png_;
    lv_obj_t* record_label_;
	time_t pre_sys_time_;
	int pre_date_;
	int pre_time_;
	bool wifi_enable_;
	lv_obj_t* wifi_img_;
	lv_obj_t* wifi_id_label_;
	lv_obj_t* wifi_password_label_;
	bool wifi_prepared_;
	lv_obj_t* reverse_line_img_;
	int Fatigue_reminder_cnt;
	int Fatigue_reminder_value;
	int Function_bar_cnt;
    bool Function_bar_flag;
	lv_obj_t* Handtip_box_;
	int audio_flag_;
	int language_value_;
	int playsound_flag_;
	
private:
	lv_obj_t* time_label_;
	lv_obj_t* date_label_;
	int64_t record_start_time_;
	int pre_record_time_;
	bool update_time_;
	int pre_camera_status_;
	bool led_on_;
	lv_obj_t* record_img_;
	lv_obj_t* photo_img_;
	lv_obj_t* set_img_;
	lv_obj_t* playback_img_;
};

#endif // !__PAGEMAIN_H__
