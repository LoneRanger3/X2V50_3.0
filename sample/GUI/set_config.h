#ifndef __SET_CONFIG_H__
#define __SET_CONFIG_H__

#include "lv_conf.h"
#include "lvgl.h"
#include <stdio.h>
#include "xm_common.h"
#include "xm_ia_comm.h"
#include "xm_middleware_def.h"
//2K:2560x1440
//3K:3008x1800
//4K:3840x2144
//1080P:1920x1080
//720P:1280x720
#define LVGL_ROATE_MODE//软件处理lvgl旋转
#define VOICE_CONTROL_EN    0    //0-屏蔽声控，注意默认菜单配置参数要关闭                  1-添加声控  
#define COMPACT_RECORD_EN   1    //0去掉缩时录影功能，注意默认菜单配置参数要关闭            1-添加缩时录影功能
#define G_SENSOR_EN         1    //0去掉重力功能 ，注意默认菜单配置参数要关闭               1-添加重力功能
#define GPS_EN              1    //0不支持GPS               1-支持GPS功能
#define OSD_SHOW_ADJUST     1    //搜不到gps时间水印下移，0关闭，1开启
#define X2V50_PROJ_DEBUG    1 
#define X2V50_2_PRODUCT_TEST    0    //生产测试(1.录像页面显示wifi名称密码。2.gsensor灵敏度打开。3.WiFi默认打开。4.gps模拟关闭)
#define SD_CARD_PLAY_INTERVAL_TIME 30  //SD卡播报间隔
#define AIPAIPAI_PROJECT_ARABIC 0  //爱拍拍阿拉伯地区定制

#if AIPAIPAI_PROJECT_ARABIC
#define COMPACT_RECORD_EN   0   //爱拍拍阿拉伯地区不开缩时录影，只开停车监控
#endif

#if defined SUPPORT_4K

#if 0 //720p开启
#define OSD_TIME_ADJUST_Y -190
#define OSD_GPS_ADJUST_Y  420
#else
#define OSD_TIME_ADJUST_Y 0
#define OSD_GPS_ADJUST_Y  420
#define OSD_TIME_OFS_X  750

#endif

#else

#define OSD_TIME_ADJUST_Y 0
#define OSD_GPS_ADJUST_Y  420

#endif

int AlarmRegionCoorSw(float coor, float length, bool flag);

//配置
struct _time
{
	int year;
	int mon;
	int day;
	int hour;
	int min;
	int sec;
};
enum Language
{
    SimpChinese, //简中
    TradChinese, //繁体
    Japanese,    //日语
    English,     //英语
    Thai,        //泰语
    Russian,     //俄语
    German,      //德语
    French,      //法语
    Italian,       //意大利
    Spanish,      //西班牙
    Portuguese,       //葡萄牙
    Polish,       //波兰语
};

enum Sensitivity
{
	Sensitivity_Close,
	Sensitivity_High,
	Sensitivity_Mid,
	Sensitivity_Low,
	Sensitivity_StartUp,
};

enum XM_CONFIG_VALUE_TYPE
{
	CFG_Operation_Value_Int = 0,
	CFG_Operation_Value_Bool,
	CFG_Operation_Value_Float,
};
enum Volume
{
	Volume_Close,
	Volume_Low,
	Volume_Mid,
    Volume_High,
};
enum SpeeduUnit
{
    SpeeduUnit_Km = 0, //公里/小时
    SpeeduUnit_Mile,  //英里/小时
};
enum XM_CONFIG_OPERATION
{
	//系统设置
	CFG_Operation_Lcd_OffTime,
	CFG_Operation_Lcd_Light,
	CFG_Operation_Language,
	CFG_Operation_Key_Voice,
	CFG_Operation_boot_Voice,
	CFG_Operation_Acc_Power_Supply,
	CFG_Operation_Voice_Control,
	CFG_Operation_AutoShutdown_Time,
	//行车设置
	CFG_Operation_Circular_Record_Time,
	CFG_Operation_Video_Resolution,
	CFG_Operation_Video_Encode_Format,
	CFG_Operation_Behind_Mirror,
	//行车记录仪
	CFG_Operation_Exposure_Compensation,
	CFG_Operation_White_Balance,
	CFG_Operation_Reject_Flicker,
	CFG_Operation_Compact_Record_Fps,
	CFG_Operation_Compact_Record_Duration,
	CFG_Operation_Collision_Sensitivity,
	CFG_Operation_Record_Voice,
	CFG_Operation_Collision_Startup,
	CFG_Operation_Car_Charger,
	CFG_Operation_Camera,
	CFG_Operation_Date_Watermark,
    CFG_Operation_WiFi,
    CFG_Operation_Fatigue_reminder, //疲劳提醒
    CFG_Operation_GPS_Watermark,
    CFG_Operation_GPS_Unit,
	CFG_Operation_NEED_REPAIR_SDCARD,	//需要fsck修复sd卡
    CFG_Operation_GAMMA_DELTA,
    CFG_Operation_NULL,
};

union XM_CONFIG_VALUE
{
	int int_value;
	bool bool_value;
	float float_value;
};

struct XM_CONFIG_UNIT
{
	XM_CONFIG_OPERATION	 opr_type;
	const char*	obj_str;//默认是"root", 
	const char*	opr_str;
	XM_CONFIG_VALUE_TYPE value_type;
	XM_CONFIG_VALUE default_value;
};

const XM_CONFIG_UNIT CFG_ALL_OPERATION_UNITS[] =
{
	//系统设置初始化
	{ CFG_Operation_Lcd_OffTime,			"",		"lcd_offtime",			CFG_Operation_Value_Int,	0},//0
	{ CFG_Operation_Lcd_Light,			    "",		"lcd_light",			CFG_Operation_Value_Int,	60},
	{ CFG_Operation_Language,				"",		"language",				CFG_Operation_Value_Int,	English},//  English  SimpChinese TradChinese
	{ CFG_Operation_Key_Voice,				"",		"key_voice",			CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_boot_Voice,				"",		"boot_voice",			CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Acc_Power_Supply,		"",		"acc_power_supply",		CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Voice_Control,		    "",		"Voice_Control",		CFG_Operation_Value_Bool,	false},// false true
	{ CFG_Operation_AutoShutdown_Time,			"",	"AutoShutdown_Time",	CFG_Operation_Value_Int,	0},
	//行车设置初始化
	{ CFG_Operation_Circular_Record_Time,	"",		"circular_record_time",	CFG_Operation_Value_Int,	1},//1
	#ifdef SUPPORT_2K
	{ CFG_Operation_Video_Resolution,		"",		"video_resolution",		CFG_Operation_Value_Int,	1440},
	#else
	{ CFG_Operation_Video_Resolution,		"",		"video_resolution",		CFG_Operation_Value_Int,	2144},	
	#endif
	{ CFG_Operation_Video_Encode_Format,	"",		"video_encode_format",	CFG_Operation_Value_Int,	PT_H265},
	{ CFG_Operation_Behind_Mirror,			"",		"Behind_Mirror",		CFG_Operation_Value_Bool,	false},
	//行车记录仪
	{ CFG_Operation_Exposure_Compensation,	"",		"Exposure_Compensation",CFG_Operation_Value_Int,	50},
	{ CFG_Operation_White_Balance,			"",		"White_Balance",		CFG_Operation_Value_Int,	0},
	{ CFG_Operation_Reject_Flicker,			"",		"Reject_Flicker",		CFG_Operation_Value_Int,	0},
	{ CFG_Operation_Compact_Record_Fps,			"",		"Compact_Record_Fps",			CFG_Operation_Value_Int,	0},
	{ CFG_Operation_Compact_Record_Duration,	"",		"Compact_Record_Duration",		CFG_Operation_Value_Int,	0},
	{ CFG_Operation_Collision_Sensitivity,		"",		"Collision_Sensitivity",		CFG_Operation_Value_Int,	Sensitivity_Close}, //Sensitivity_Low  Sensitivity_Mid
	{ CFG_Operation_Record_Voice,				"",		"Record_Voice",					CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Collision_Startup,			"",		"Collision_Startup",			CFG_Operation_Value_Bool,	false},
	{ CFG_Operation_Car_Charger,				"",		"Car_Charger",					CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_Camera,						"",		"Camera",						CFG_Operation_Value_Int,	XM_PLAY_BOTH},
	{ CFG_Operation_Date_Watermark,				"",		"Date_Watermark",				CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_WiFi,				        "",		"wifi",				            CFG_Operation_Value_Bool,	true},
    { CFG_Operation_Fatigue_reminder,		    "",		"fatigue",				        CFG_Operation_Value_Int,	0},
    { CFG_Operation_GPS_Watermark,		         "",		"gps",			    CFG_Operation_Value_Bool,	true},
    { CFG_Operation_GPS_Unit,				    "",		"speed_unit",				    CFG_Operation_Value_Int,	SpeeduUnit_Km},
	{ CFG_Operation_NEED_REPAIR_SDCARD,		"",		"Fsck Sdcard",			            CFG_Operation_Value_Bool,	true},
	{ CFG_Operation_GAMMA_DELTA,		    "",		"DELTA",			                CFG_Operation_Value_Int,	100},
};

struct cJSON;
class CarConfig
{
public:
	CarConfig();
	~CarConfig();

	int ReadConfigFromFile();
	int SetDefaultConfig();
	int GetValue(const XM_CONFIG_OPERATION& cfg_opr, XM_CONFIG_VALUE& out_value);
	int SetValue(const XM_CONFIG_OPERATION& cfg_opr, const XM_CONFIG_VALUE& in_value);
	int SetMultiValue(XM_CONFIG_OPERATION cfg_oprs[], XM_CONFIG_VALUE in_values[], int count);

private:
	int SaveToFile(cJSON* json_obj);

private:
	cJSON* json_obj_;
};

char* GetParsedString(const char *string);
void set_language(int language);

enum SetPageSubpage
{
    // Subpage_Return,  //返回
    Subpage_Resolution,//分辨率
    Subpage_RecordSound,    //录音
    Subpage_CirRecordTime,//循环录影
    Subpage_LanguageSet,        //语言
 #if G_SENSOR_EN
    Subpage_CollisionSensitivity, //重力感应
    Subpage_ParkMonitor,  //停车监控
 #endif
 #if COMPACT_RECORD_EN
    Subpage_Recordfps,  //缩时录影
    Subpage_Compactduration,  //缩时录影时长
 #endif
    Subpage_WifiSet,         //wifi设置
    Subpage_Mirror,          //后拉镜像开关
    Subpage_Lcd_Light,       //屏幕亮度
    Subpage_ScreenSaver,    //屏幕保护
#if VOICE_CONTROL_EN
    Subpage_VoiceControl,   //语音声控
#endif
    Subpage_KeyTone,       //音量大小
    Subpage_Watermark,     //时间水印
    Subpage_Fatigue_reminder,  //疲劳提醒
    //Subpage_HighDynamicRange,
    //Subpage_LightSourceFrequency,
    Subpage_ExposureCompensation, //曝光补偿
    Subpage_WhiteBalance,         //白平衡
    //Subpage_LicensePlate,
    //Subpage_DateWatermark,
    Subpage_RejectFlicker,      //光源频率
    // Subpage_AutoShutdown,      //自动关机
    Subpage_TimeSet,          //时间设置
    Subpage_Format,           //格式化存储卡
    Subpage_DefaultSet,      //恢复出厂设置
    Subpage_Edition,        //版本信息
#if GPS_EN 
    Subpage_GpsWatermark,//gps水印开关
    Subpage_SpeedUnit,          //gps速度单位
    Subpage_GpsInfo,          //gps信息
#endif
    Subpage_Total,

#if !G_SENSOR_EN
    Subpage_CollisionSensitivity, //重力感应
    Subpage_ParkMonitor,  //停车监控
#endif
#if !COMPACT_RECORD_EN
    Subpage_Recordfps,  //缩时录影
    Subpage_Compactduration,  //缩时录影时长
#endif
#if !VOICE_CONTROL_EN
    Subpage_VoiceControl,   //语音声控
#endif
 Subpage_BootTone,       //开机声音
 Subpage_AutoShutdown,      //自动关机
 Subpage_Return,  //返回
};

struct menu_table {
    uint32_t menu_id;//对应的菜单枚举值
    const char* img_path[2];//一级菜单(默认，选中)的图片
    const char* menu_name;//一级菜单文案
    const char* two_table_str[16]; //二级菜单列表对应的文案
    uint32_t table_value[16]; //二级菜单列表对应的实际选项值，此值是实际存在FLASH的值
    uint8_t two_table_num;//二级菜单个数
    XM_CONFIG_OPERATION set_name; //flash菜单设置的名称
};



enum VideoResolution
{
    VideoResolution_4K = 2144,
    VideoResolution_3K = 1800,
    VideoResolution_1440P=1440,
    VideoResolution_1080P=1080,
};
enum CirRecordTimeBtn
{
    CirRecordTimeBtn_1min = 1,
    CirRecordTimeBtn_2min,
    CirRecordTimeBtn_3min,
};
enum WhiteBalance
{
    WhiteBalance_Close = 0,
    WhiteBalance_4000=4000,
    WhiteBalance_5000=5000,
    WhiteBalance_6000=6000,
    WhiteBalance_7000=7000,
};

enum RejectFlicker
{
    RejectFlicker_50 = 0,
    RejectFlicker_60,
};

enum CompactRecordFps
{
    CompactRecordFps_Close = 0,
    CompactRecordFps_1,
    CompactRecordFps_2,
    CompactRecordFps_5,
};

enum CompactRecordDur
{
    CompactRecordDur_Close=0,
    CompactRecordDur_8hour=8,
    CompactRecordDur_16hour=16,
    CompactRecordDur_24hour=24,
};


enum RecordSound
{
    RecordSound_Close,
    RecordSound_Open,
};
enum Switchonoff
{
    Switchoff,
    Switchon,
};
enum RestScreenTime
{
    RestScreenTime_Close = 0,
    RestScreenTime_1Min=60,
    RestScreenTime_3Min=180,
    RestScreenTime_5Min=300,
};

enum AutoShutdownTime
{
    AutoShutdownTime_Close =0,
    AutoShutdownTime_1Min=60,
    AutoShutdownTime_3Min=180,
    AutoShutdownTime_5Min=300,
};

enum KeyToneSwitch
{
    KeyTone_Close,
    KeyTone_Open,
};
enum ScreenLight
{
    ScreenLight_s1 = 85,//100
    ScreenLight_s2 = 60,//85
    ScreenLight_s3 = 50,//75
    ScreenLight_s4 = 40,//60
    ScreenLight_s5 = 30,//40
};
#ifdef LVGL_SIM_VISUAL_STUDIO
#define image "A:./sample/custom/Pic/"
#else
#define image "A:/mnt/custom/Pic/"
#endif 
 

const struct menu_table sys_menu_config_table[] = {

    // //返回
    // {
    //     Subpage_Return,                       //菜单列表名
    //    {image"return.png",image"return-.png"},            //一级菜单图片路径
    //    "Return",     //一级菜单列表文案
    //     { NULL },//二级菜单列表显示的文案
    //     { 0 },    //此菜单实际生效的值，默认选项值必定为其中之一
    //       0,       //二级菜单个数
    //      CFG_Operation_NULL,
    // },

    //分辨率
    {
        Subpage_Resolution,                       //菜单列表名
        {image"resolution.png",image"resolution-.png"},            //一级菜单图片路径
         "Resolution ratio",     //一级菜单列表文案
		#if defined SUPPORT_4K
         {"4K+1080P", "2K+1080P",	"1080P+1080P"},//二级菜单列表显示的文案
         {VideoResolution_4K ,VideoResolution_1440P,VideoResolution_1080P},    //此菜单实际生效的值，默认选项值必定为其中之一
		 3, //二级菜单个数
        #elif defined SUPPORT_3K
         {"3K+1080P", "2K+1080P",	"1080P+1080P"},//二级菜单列表显示的文案
         {VideoResolution_3K ,VideoResolution_1440P,VideoResolution_1080P},    //此菜单实际生效的值，默认选项值必定为其中之一
		 3,       //二级菜单个数
        #elif defined SUPPORT_2K
         {"2K+1080P",	"1080P+1080P"},//二级菜单列表显示的文案
         {VideoResolution_1440P,VideoResolution_1080P},    //此菜单实际生效的值，默认选项值必定为其中之一
          2, //二级菜单个数
        #endif 
         CFG_Operation_Video_Resolution,
     },
  //录音
  {
     Subpage_RecordSound,                       //菜单列表名
     {image"record_voice_off.png",image"record_voice_off-.png"},            //一级菜单图片路径
     "Sound recording",     //一级菜单列表文案
     {"Close",	"Open"},//二级菜单列表显示的文案
     {Switchoff, Switchon},    //此菜单实际生效的值，默认选项值必定为其中之一
      2,       //二级菜单个数
     CFG_Operation_Record_Voice,
  },
     //录像时间
    {
        Subpage_CirRecordTime,                       //菜单列表名
        {image"record_time.png",image"record_time-.png"},            //一级菜单图片路径
         "Recording duration",     //一级菜单列表文案
         {"1 minute",		"2 minutes",		"3 minutes"},//二级菜单列表显示的文案
         {CirRecordTimeBtn_1min,CirRecordTimeBtn_2min,CirRecordTimeBtn_3min},    //此菜单实际生效的值，默认选项值必定为其中之一
          3,       //二级菜单个数
         CFG_Operation_Circular_Record_Time,
     },
     
    //语言
    {
       Subpage_LanguageSet,                       //菜单列表名
       {image"language.png",image"language-.png"},            //一级菜单图片路径
       "Language",     //一级菜单列表文案
       {"SimpChinese","TradChinese","Japanese",	"English",/*"Thai",*/"Russian","German","French","Italian","Spanish","Polish"},//二级菜单列表显示的文案
       {SimpChinese,TradChinese, Japanese, English,/*Thai,*/Russian, German, French, Italian, Spanish, Polish},    //此菜单实际生效的值，默认选项值必定为其中之一
        10,       //二级菜单个数
       CFG_Operation_Language,
    },
#if G_SENSOR_EN
     //碰撞灵敏度
    {
        Subpage_CollisionSensitivity,                       //菜单列表名
        {image"collision_sensitivity_off.png",image"collision_sensitivity_off-.png"},            //一级菜单图片路径
         "Collision sensitivity",     //一级菜单列表文案
         {"Close",	"high",		"middle", "low"},//二级菜单列表显示的文案
         {Sensitivity_Close, Sensitivity_High,Sensitivity_Mid,Sensitivity_Low },    //此菜单实际生效的值，默认选项值必定为其中之一
          4,       //二级菜单个数
          CFG_Operation_Collision_Sensitivity,
     },

     //停车监控
   {
       Subpage_ParkMonitor,                       //菜单列表名
       {image"park_monitor_off.png",image"park_monitor_off-.png"},            //一级菜单图片路径
        "Park Monitor",     //一级菜单列表文案
        {"Close",	"Open"},//二级菜单列表显示的文案
        {Switchoff, Switchon },    //此菜单实际生效的值，默认选项值必定为其中之一
         2,       //二级菜单个数
       CFG_Operation_Collision_Startup,
    },
#endif
#if COMPACT_RECORD_EN
    //缩时录影
  {
       Subpage_Recordfps,                       //菜单列表名
       {image"monitor_fps.png",image"monitor_fps-.png"},            //一级菜单图片路径
       "Compact record",     //一级菜单列表文案
       {"Close",	"1 fps", "2 fps" ,"5 fps"},//二级菜单列表显示的文案
       {CompactRecordFps_Close, CompactRecordFps_1, CompactRecordFps_2,CompactRecordFps_5},    //此菜单实际生效的值，默认选项值必定为其中之一
        4,       //二级菜单个数
       CFG_Operation_Compact_Record_Fps,
   },
   //缩时录影时长
  {
       Subpage_Compactduration,                       //菜单列表名
       {image"compact_monitor_off.png",image"compact_monitor_off-.png"},            //一级菜单图片路径
       "Compact record duration",     //一级菜单列表文案
       {"Close",	"8 hours", "16 hours" ,"24 hours"},//二级菜单列表显示的文案
       {CompactRecordDur_Close, CompactRecordDur_8hour, CompactRecordDur_16hour,CompactRecordDur_24hour},    //此菜单实际生效的值，默认选项值必定为其中之一
        4,       //二级菜单个数
       CFG_Operation_Compact_Record_Duration,
   },
#endif
   
   //wifi设置
  {
       Subpage_WifiSet,                       //菜单列表名
       {image"wifi_set_off.png",image"wifi_set_off-.png"},            //一级菜单图片路径
       "WiFi settings",     //一级菜单列表文案
       {"Close",	"Open"},//二级菜单列表显示的文案
       {Switchoff, Switchon},    //此菜单实际生效的值，默认选项值必定为其中之一
        2,       //二级菜单个数
       CFG_Operation_WiFi,
   },

   //后拉镜像开关
  {
       Subpage_Mirror,                       //菜单列表名
       {image"camera_mirror.png",image"camera_mirror-.png"},            //一级菜单图片路径
       "Pull the mirror back",     //一级菜单列表文案
       {"Close",	"Open"},//二级菜单列表显示的文案
       {Switchoff, Switchon},    //此菜单实际生效的值，默认选项值必定为其中之一
        2,       //二级菜单个数
       CFG_Operation_Behind_Mirror,
   },
   //屏幕亮度
  {
       Subpage_Lcd_Light,                       //菜单列表名
       {image"lcd_light.png",image"lcd_light-.png"},            //一级菜单图片路径
       "Screen brightness",     //一级菜单列表文案
       {"120",	"100",		"80",	"60", "40"},//二级菜单列表显示的文案
       {ScreenLight_s1,ScreenLight_s2,ScreenLight_s3,ScreenLight_s4,ScreenLight_s5},    //此菜单实际生效的值，默认选项值必定为其中之一
        5,       //二级菜单个数
       CFG_Operation_Lcd_Light,
   },
  //屏幕保护
  {
       Subpage_ScreenSaver,                       //菜单列表名
       {image"screen_save_off.png",image"screen_save_off-.png"},            //一级菜单图片路径
       "Screen saver",     //一级菜单列表文案
        {"Close",	"1 minute",		"3 minutes",		"5 minutes"},//二级菜单列表显示的文案
       {RestScreenTime_Close, RestScreenTime_1Min,RestScreenTime_3Min,RestScreenTime_5Min},    //此菜单实际生效的值，默认选项值必定为其中之一
        4,       //二级菜单个数
       CFG_Operation_Lcd_OffTime,
   },
#if VOICE_CONTROL_EN
   //语音声控
  {
     Subpage_VoiceControl,                       //菜单列表名
     {image"voice_control_off.png",image"voice_control_off-.png"},            //一级菜单图片路径
     "Voice control",     //一级菜单列表文案
     {"Close",	"Open"},//二级菜单列表显示的文案
     {Switchoff, Switchon},    //此菜单实际生效的值，默认选项值必定为其中之一
      2,       //二级菜单个数
     CFG_Operation_Voice_Control,
  },
#endif

  //按键音
  {
     Subpage_KeyTone,                       //菜单列表名
     {image"key_tone_off.png",image"key_tone_off-.png"},            //一级菜单图片路径
     "Key tone",     //一级菜单列表文案
     {"Close",	"Open"},//二级菜单列表显示的文案
     {Switchoff, Switchon},    //此菜单实际生效的值，默认选项值必定为其中之一
      2,       //二级菜单个数
     CFG_Operation_Key_Voice,
  },
  //时间水印
  {
     Subpage_Watermark,                       //菜单列表名
     {image"key_tone_off.png",image"key_tone_off-.png"},            //一级菜单图片路径
     "Date watermark",     //一级菜单列表文案
     {"Close",	"Open"},//二级菜单列表显示的文案
     {Switchoff, Switchon},    //此菜单实际生效的值，默认选项值必定为其中之一
      2,       //二级菜单个数
      CFG_Operation_Date_Watermark,
  },
  //疲劳驾驶
  {
    Subpage_Fatigue_reminder,  //疲劳提醒,                       //菜单列表名
     {image"key_tone_off.png",image"key_tone_off-.png"},            //一级菜单图片路径
     "Fatigue reminder",     //一级菜单列表文案
     {"Close",	"1 hour","2 hours","3 hours"},//二级菜单列表显示的文案
     {0, 1,2,3},    //此菜单实际生效的值，默认选项值必定为其中之一
      4,       //二级菜单个数
      CFG_Operation_Fatigue_reminder,
  },
//   //开机音
//   {
//      Subpage_BootTone,                       //菜单列表名
//      {image"key_tone_off.png",image"key_tone_off-.png"},            //一级菜单图片路径
//      "Boot sound",     //一级菜单列表文案
//      {"Close",	"Open"},//二级菜单列表显示的文案
//      {Switchoff, Switchon},    //此菜单实际生效的值，默认选项值必定为其中之一
//       2,       //二级菜单个数
//      CFG_Operation_boot_Voice,
//   },
  //曝光补偿
  {
     Subpage_ExposureCompensation,                       //菜单列表名
     {image"exposure_compensation.png",image"exposure_compensation-.png"},            //一级菜单图片路径
     "Exposure compensation",     //一级菜单列表文案
     {"-3",	"-2","-1","0","+1","+2","+3"},//二级菜单列表显示的文案
     {20, 30,40,50,60,70,80},    //此菜单实际生效的值，默认选项值必定为其中之一
      7,       //二级菜单个数
     CFG_Operation_Exposure_Compensation,
  },
  //白平衡
  {
     Subpage_WhiteBalance,                       //菜单列表名
     {image"white_balance.png",image"white_balance-.png"},            //一级菜单图片路径
     "White balance",     //一级菜单列表文案
     {"Automatic",	"Sunlight","Cloudy day","Tungsten lamp","Fluorescent lamp"},//二级菜单列表显示的文案
     {WhiteBalance_Close, WhiteBalance_4000,WhiteBalance_5000,WhiteBalance_6000,WhiteBalance_7000},    //此菜单实际生效的值，默认选项值必定为其中之一
      5,       //二级菜单个数
     CFG_Operation_White_Balance,
  },
  //光源频率
  {
     Subpage_RejectFlicker,                       //菜单列表名
     {image"light_source_frequency.png",image"light_source_frequency-.png"},            //一级菜单图片路径
     "Reject Flicker",     //一级菜单列表文案
     {"50Hz",	"60Hz"},//二级菜单列表显示的文案
     {RejectFlicker_50, RejectFlicker_60},    //此菜单实际生效的值，默认选项值必定为其中之一
      2,       //二级菜单个数
     CFG_Operation_Reject_Flicker,
  },

  //自动关机
//   {
//      Subpage_AutoShutdown,                       //菜单列表名
//      {image"auto_shutdown.png",image"auto_shutdown-.png"},            //一级菜单图片路径
//      "Automatic shutdown",     //一级菜单列表文案
//      {"Close",	"1 minute",		"3 minutes",		"5 minutes"},//二级菜单列表显示的文案
//      {AutoShutdownTime_Close, AutoShutdownTime_1Min,AutoShutdownTime_3Min,AutoShutdownTime_5Min},    //此菜单实际生效的值，默认选项值必定为其中之一
//       4,       //二级菜单个数
//      CFG_Operation_AutoShutdown_Time,
//   },

  //时间设置
  {
     Subpage_TimeSet,                       //菜单列表名
     {image"time_set.png",image"time_set-.png"},            //一级菜单图片路径
     "Time set",     //一级菜单列表文案
     { NULL},//二级菜单列表显示的文案
     {0},    //此菜单实际生效的值，默认选项值必定为其中之一
      0,       //二级菜单个数
     CFG_Operation_NULL,
  },

  //格式化存储卡
  {
     Subpage_Format,                       //菜单列表名
     {image"format_sd.png",image"format_sd-.png"},            //一级菜单图片路径
     "Format memory card",     //一级菜单列表文案
     {"Cancel","Confirm"},//二级菜单列表显示的文案
     {0},    //此菜单实际生效的值，默认选项值必定为其中之一
      2,       //二级菜单个数
     CFG_Operation_NULL,
  },

  //恢复出厂设置
  {
     Subpage_DefaultSet,                       //菜单列表名
     {image"default_set.png",image"default_set-.png"},            //一级菜单图片路径
     "Restore factory settings",     //一级菜单列表文案
     {"Cancel","Confirm"},//二级菜单列表显示的文案
     {0},    //此菜单实际生效的值，默认选项值必定为其中之一
      2,       //二级菜单个数
     CFG_Operation_NULL,
  },

  //版本信息
  {
     Subpage_Edition,                       //菜单列表名
     {image"edition.png",image"edition-.png"},            //一级菜单图片路径
     "Version information",     //一级菜单列表文案
     { NULL},//二级菜单列表显示的文案
     {0},    //此菜单实际生效的值，默认选项值必定为其中之一
      0,       //二级菜单个数
      CFG_Operation_NULL,
  },
#if GPS_EN 
  //GPS开关
  {
      Subpage_GpsWatermark,                       //菜单列表名
      { NULL},            //一级菜单图片路径
          "GPS Switch",     //一级菜单列表文案
      { "Close",	"Open" },//二级菜单列表显示的文案
      { Switchoff, Switchon },    //此菜单实际生效的值，默认选项值必定为其中之一
          2,       //二级菜单个数
          CFG_Operation_GPS_Watermark,
    },
    //GPS速度单位
    {
        Subpage_SpeedUnit,                       //菜单列表名
        {NULL},            //一级菜单图片路径
         "Speed unit",     //一级菜单列表文案
         {"km/h","mile/h"},//二级菜单列表显示的文案
         {0,1},    //此菜单实际生效的值，默认选项值必定为其中之一
          2,       //二级菜单个数
         CFG_Operation_GPS_Unit,
    },

    //GPS信息
    {
        Subpage_GpsInfo,                       //菜单列表名
        {NULL},            //一级菜单图片路径
         "GPS information",     //一级菜单列表文案
         {NULL},//二级菜单列表显示的文案
         {0},    //此菜单实际生效的值，默认选项值必定为其中之一
          0,       //二级菜单个数
         CFG_Operation_NULL,
    },
#endif
};


#endif
