#pragma once
#include <map>
#include "xm_middleware_def.h"
#define MAX_NAL_NUM		10//最大nal单元数
#define MAX_PNG_FILE_LEN 51200 //缩略图最大50KB
//文件系统返回值定义
#define VFS_RESULT				    int32_t
#define VFS_RESULT_SUCCESS			0
#define VFS_RESULT_UNKNOWN			0xFFFFFFFF
#define VFS_RESULT_FILE_TOO_LARGE	1
#define VFS_RESULT_FILE_WRITE_ERROR	3

extern const char* kSepartorStr;
extern const int kMaxRecordFileLen;
extern const int64_t kMaxRecordFileLenInByte;
extern const int64_t kReserveLen;
extern const int kSpaceReserved;
extern const int kPictureSpace;
extern const int kMaxFrameLen;
extern const int kAiChn;
extern const char* kClassifierPath;
extern const char* kAdasCfgPath;
extern const XM_IA_PLATFORM_E kIAPlatform;
extern const int kViChnMap[];
extern const int kMaxPicWidth;
extern const int kMaxPicHeight;
extern const int kVodDecDev;
extern const int kPicDecDev;
extern int kFrameRate;
extern const int kFileMinLen;

extern XM_MW_LOG_CallBack g_log_cb;
extern int64_t g_log_user;

struct FragmentationHeader {
	int nal_begin_pos[MAX_NAL_NUM];
	int nal_len[MAX_NAL_NUM];
	int nal_count;
	int sps_index;
	int pps_index;
	int vps_index;
};

typedef std::map<int64_t, XM_MW_Record_Info> TimeRecordInfoMap;
typedef std::multimap<int64_t, XM_MW_Record_Info> TimeRecordInfoMultiMap;