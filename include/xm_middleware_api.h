#pragma once

#ifdef WIN32
#ifdef API_EXPORTS
#define XM_MIDDLEWARE_API extern "C" __declspec(dllexport)
#else
#define XM_MIDDLEWARE_API extern "C" __declspec(dllimport)
#endif
#define CALLMETHOD __stdcall
#else
#define XM_MIDDLEWARE_API 
#define CALLMETHOD 
#endif

#include "xm_middleware_def.h"

/*------------------------全局-----------------------------*/
//设置通道数，默认为XM_MAX_CHANNEL_NUM。该接口需要在XM_Middleware_Init之前调用
XM_MIDDLEWARE_API int XM_Middleware_SetChannelNum(int channel_num);
XM_MIDDLEWARE_API int XM_Middleware_Init();
XM_MIDDLEWARE_API const char* XM_Middleware_GetVersion();
XM_MIDDLEWARE_API int XM_Middleware_Exit();
XM_MIDDLEWARE_API int XM_Middleware_SetLogLevel(XMLogLevel level);

/*----------------------设置回调---------------------------*/
XM_MIDDLEWARE_API int XM_Middleware_SetRealStreamCallback(XM_MW_RealStream_CallBack proc, int64_t user);
XM_MIDDLEWARE_API int XM_Middleware_SetVodStreamCallback(XM_MW_VodStream_CallBack proc, int64_t user);
XM_MIDDLEWARE_API int XM_Middleware_SetEventCallback(XM_MW_Event_CallBack proc, int64_t user);
XM_MIDDLEWARE_API int XM_Middleware_SetLogCallback(XM_MW_LOG_CallBack proc, int64_t user);

/*------------------------存储-----------------------------*/
//放在最开始调用。设置坏卡检测超时时间，单位毫秒。默认8000毫秒
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetBadSdcardCheckTime(int time);
//设置sd卡mount路径
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetSDCardConfig(XM_MW_Storage_SDCard_Config* sdcard_cfg);
//获取速度状态
XM_MIDDLEWARE_API int XM_Middleware_Storage_GetSpeedState(XMStorageSDCardNum sdc_num, int* write_speed);
//写速度到文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_WriteSpeedToFile(XMStorageSDCardNum sdc_num, int* write_speed);
//检测sd卡,挂载sd卡及检测文件系统
XM_MIDDLEWARE_API int XM_Middleware_Storage_CheckSDCard(XMStorageSDCardNum sdc_num);
//检测sd卡上的文件系统
XM_MIDDLEWARE_API int XM_Middleware_Storage_CheckFileSystem(XMStorageSDCardNum sdc_num);
//查询sd卡容量
XM_MIDDLEWARE_API int XM_Middleware_Storage_CheckSDSpace(XMStorageSDCardNum sdc_num, XM_MW_SDCard_Space* sdcard_space);
//格式化sd卡
XM_MIDDLEWARE_API int XM_Middleware_Storage_FormatSDCard(XMStorageSDCardNum sdc_num);
//设置录像时长，1、2、3分钟
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetRecordDuration(XMStorageRecordDuration record_duration);
//设置停车监控唤醒后紧急视频时长，默认30秒, 范围10-100秒
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetAlarmRecordDuration(int dura);
//指定视频通道存储到哪张sd卡
XM_MIDDLEWARE_API int XM_Middleware_Storage_BindSDCardNum(int channel, XMStorageSDCardNum sdc_num);
//设置存储媒体格式，只针对pc版本有效，板子上会自动从mpp模块获取
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetMediaInfo(int channel, XM_MW_Media_Info* media_info);
//开始录像/停止录像, channel为-1，表所有通道开启和关闭。刚插上后拉时，画面不稳定，delay_time用于延迟存储，过滤异常画面
XM_MIDDLEWARE_API int XM_Middleware_Storage_Start(int channel, int delay_time);
//碰撞开机录像
XM_MIDDLEWARE_API int XM_Middleware_Storage_CollisionRecord(int channel);
//录音使能
XM_MIDDLEWARE_API int XM_Middleware_Storage_EnableRecordAudio(bool enable);
//AddVideoFrame\AddAudioFrame\AddPicture只针对pc版本有效，板子上会自动从mpp模块取流
XM_MIDDLEWARE_API int XM_Middleware_Storage_AddVideoFrame(int channel, bool key_frame, uint8_t* data, int len, int64_t pts);
XM_MIDDLEWARE_API int XM_Middleware_Storage_AddAudioFrame(int channel, uint8_t* data, int len, int64_t pts);
XM_MIDDLEWARE_API int XM_Middleware_Storage_AddPicture(int channel, uint8_t* data, int len);
XM_MIDDLEWARE_API int XM_Middleware_Storage_Stop(int channel);
XM_MIDDLEWARE_API bool XM_Middleware_Storage_ReallyStop();
//锁定文件，关键录像，不允许被覆盖。锁定历史录像文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_LockHistoryFile(XM_MW_File_Param* file_param);
//锁定所有文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_LockAllHistoryFile(XMStorageSDCardNum sdc_num, int channel, XMRecordType type);
//解锁历史文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_UnLockHistoryFile(XM_MW_File_Param* file_param);
//解锁所有文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_UnLockAllHistoryFile(XMStorageSDCardNum sdc_num, int channel, XMRecordType type);
//锁定当前正在录的一个录像文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_LockCurrentFile(XMStorageSDCardNum sdc_num, int channel, bool lock_file);
//设置加锁文件空间占比
XM_MIDDLEWARE_API int XM_Middleware_Storage_SetLockFilePercent(XM_LOCKFILE_PERCENT lockfile_percent);
//删除文件，删除前需停止播放
XM_MIDDLEWARE_API int XM_Middleware_Storage_DelFile(XM_MW_File_Param* file_param);
//删除所有文件
XM_MIDDLEWARE_API int XM_Middleware_Storage_DelAllFile(XMStorageSDCardNum sdc_num, int channel, XMRecordType type);
//获取当前录像文件已录的时间
XM_MIDDLEWARE_API int XM_Middleware_Storage_GetRecordedTime(XMStorageSDCardNum sdc_num, int channel);
//返回剩余空间,输出参数remain_space（单位MB)，目前有1.5G做为预留空间。函数返回值小于0表失败，一般是尚未开始录像，此时上层不显示剩余录像时间
XM_MIDDLEWARE_API int XM_Middleware_Storage_GetRemainSpace(XMStorageSDCardNum sdc_num, int* remain_space);

/*------------------------回放-----------------------------*/
//mtv播放使能。解码mtv时通知到sdk，sdk内部关闭一些功能，释放更多资源来做mtv软解
XM_MIDDLEWARE_API int XM_Middleware_Playback_MTV(bool enable);
//录像(包括视频和图片)查询
XM_MIDDLEWARE_API int XM_Middleware_Playback_SetUSBPath(const char* dir_path);
XM_MIDDLEWARE_API int XM_Middleware_Playback_Query(XM_MW_Record_Query* query_info, XM_MW_Record_List* record_list);
//打开视频录像
XM_MIDDLEWARE_API long XM_Middleware_Playback_Open(XMStorageSDCardNum sdc_num, int channel, XMFileFormat file_format, 
	const char* file_full_name);
//获取录像文件信息
XM_MIDDLEWARE_API int XM_Middleware_Playback_GetFileInfo(long handle, XM_MW_File_Info* file_info);
//display_region是渲染区域的位置和大小。play_one为true时只播放一帧
XM_MIDDLEWARE_API int XM_Middleware_Playback_Play(long handle, bool play_one, RECT_S* display_region);
//seek_time，单位毫秒
XM_MIDDLEWARE_API int XM_Middleware_Playback_Seek(long handle, int seek_time);
//pause true表暂停，false表恢复
XM_MIDDLEWARE_API int XM_Middleware_Playback_Pause(long handle, bool pause);
//获取到当前播放进度，单位毫秒
XM_MIDDLEWARE_API int XM_Middleware_Playback_GetCurrentDuration(long handle);
//关闭录像文件
XM_MIDDLEWARE_API int XM_Middleware_Playback_Close(long handle);
//挂载U盘
XM_MIDDLEWARE_API int XM_Middleware_Playback_CheckUDisk();
//格式化u盘
XM_MIDDLEWARE_API int XM_Middleware_Playback_FormatUDisk();
//卸载U盘
XM_MIDDLEWARE_API int XM_Middleware_Playback_UmountUDisk();

/*------------------------图片-----------------------------*/
//解码渲染图片
XM_MIDDLEWARE_API int XM_Middleware_Picture_Play(const char* file_full_name, int file_len, RECT_S* display_region);
//从录像文件中获取缩略图
XM_MIDDLEWARE_API int XM_Middleware_Picture_GetFromRecord(XMFileFormat file_format, 
	const char* file_full_name, char* jpg_buf, int* jpg_len);

/*------------------------算法-----------------------------*/
//设置各通道ai算法类型，默认XM_AI_TYPE_BSD
XM_MIDDLEWARE_API int XM_Middleware_Smart_SetAIType(int channel, XMAIType ai_type);
//设置adas参数
XM_MIDDLEWARE_API int XM_Middleware_Smart_SetAdasParam(int channel, XM_MW_ADAS_Param* adas_param);
//设置bsd参数
XM_MIDDLEWARE_API int XM_Middleware_Smart_SetBsdParam(int channel, XM_MW_BSD_Param* bsd_param);
//ai使能
XM_MIDDLEWARE_API int XM_Middleware_Smart_EnableAI(int channel, bool enable);
//声控使能
XM_MIDDLEWARE_API int XM_Middleware_Smart_EnableKeyword(bool enable);

/*------------------------报警---------------------------*/
XM_MIDDLEWARE_API int XM_Middleware_Alarm_AlarmStart();

/*------------------------停车监控-----------------------*/
XM_MIDDLEWARE_API int XM_Middleware_ParkRecord_SetTimeLapse(XMTimeLapse time_lapse);
XM_MIDDLEWARE_API int XM_Middleware_ParkRecord_SetTimeLapseDuration(XMTimeLapseDuration time_lapse_duration);
XM_MIDDLEWARE_API int XM_Middleware_ParkRecord_Enable(bool enable);

/*----------------------看门狗---------------------------*/
//sec是看门狗超时时间，默认3秒
XM_MIDDLEWARE_API int XM_Middleware_WDT_WatchDogStart(int sec);
XM_MIDDLEWARE_API int XM_Middleware_WDT_WatchDogStop();

/*-----------------------SD卡升级------------------------*/
XM_MIDDLEWARE_API int XM_Middleware_SDCard_Upgrade(const char* file_name);
XM_MIDDLEWARE_API int XM_Middleware_SDCard_AlreadyUpgrade(const char* file_name);
XM_MIDDLEWARE_API int XM_Middleware_SDCard_CheckIsUbootUpgrade(const char* file_name);
XM_MIDDLEWARE_API int XM_Middleware_SDCard_GetUpgradeProgress();

/*-----------------------外设事件通知--------------------*/
XM_MIDDLEWARE_API int XM_Middleware_Periphery_Notify(XMEventType event_type, const char* msg, int param, char* out_msg = NULL);

/*-----------------------媒体推流------------------------*/
XM_MIDDLEWARE_API int XM_Middleware_Media_VideoStream(int channel, int stream, XM_MW_Media_Frame* media_frame, int width, int height);
XM_MIDDLEWARE_API int XM_Middleware_Media_AudioStream(int ai_dev, int channel, XM_MW_Media_Frame* media_frame);
XM_MIDDLEWARE_API int XM_Middleware_Media_VideoThumbnail(int channel, char* yuv, int width, int height);

/*-----------------------USB功能-------------------------*/
XM_MIDDLEWARE_API int XM_Middleware_USBFunc_Create(USB_MODE_E mode);
XM_MIDDLEWARE_API int XM_Middleware_USBFunc_Destroy(USB_MODE_E mode);
XM_MIDDLEWARE_API int XM_Middleware_USBFunc_ConnectComputer();

/*-----------------------WIFI----------------------------*/
//修改wifi（level =1：修改名称，level=2：修改密码，level=3：全部修改）
XM_MIDDLEWARE_API int XM_Middleware_WIFI_ChangeParam(WIFI_PARAM_S* param, int level);
XM_MIDDLEWARE_API int XM_Middleware_WIFI_GetMacParam(WIFI_MAC_PARAM_S* param);
//wifi开关，wifi关闭和开启建议间隔1s,否则可能会失败
XM_MIDDLEWARE_API int XM_Middleware_WIFI_WifiEnable(bool enable);
XM_MIDDLEWARE_API int XM_Middleware_WIFI_CreateWifiParamFile(WIFI_PARAM_S* param);
#ifndef WIN32
XM_MIDDLEWARE_API int XM_Middleware_WIFI_GetEthAttr(const char* pEthName, XMSDK_NET_ATTR_S* pstNetAttr);
#endif
/*-----------------------RTSP----------------------------*/
XM_MIDDLEWARE_API int XM_Middleware_RTSP_Start();
XM_MIDDLEWARE_API int XM_Middleware_RTSP_Stop();
XM_MIDDLEWARE_API int XM_Middleware_RTSP_PushAudioStream(int channel, uint8_t* data, int len);
XM_MIDDLEWARE_API int XM_Middleware_RTSP_PushVideoStream(int channel, int stream, uint8_t* data, int len, bool key_frame);

/*-----------------------时区----------------------------*/
XM_MIDDLEWARE_API int XM_Middleware_TIME_SetTimeZoneInfo(int minuteswest);

/*-----------------------system--------------------------*/
XM_MIDDLEWARE_API int XM_Middleware_SafeSystem(const char* pCmdLine);
XM_MIDDLEWARE_API int XM_Middleware_GetMemoryMsg(uint32_t& used_memory, uint32_t& available_memory);

/*-----------------------mpp-----------------------------*/
//在头文件中定义，在sample中实现，由sdk调用
