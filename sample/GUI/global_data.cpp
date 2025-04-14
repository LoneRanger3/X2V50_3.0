#include "global_data.h"
#include "CommDef.h"
const char* kTFCardPath = "/mnt/tfcard";
const char* kUDiskPath = "/mnt/usb";
char* kAudioPath = "/mnt/custom/Audio/";
char* kAudioPathLan = "/mnt/custom/Audio/english/";
const char* kUpdateFileName = "/home/netall_XM650V200_CAR_X2V50_3.bin";
const char* kSysTime = "/mnt/mtd/Config/sys_time";
const int k3KHeight = 1800;
int kFrameRate = 25;//30;
const int k4KHeight = 2144;
extern lv_font_t lv_font_simpchinese_22;
extern lv_font_t lv_font_traditional_22;
extern lv_font_t lv_font_Japanese_20;
extern lv_font_t lv_font_europe_20;
extern lv_font_t lv_font_russian_18;
extern lv_font_t lv_font_thai_18;
extern lv_font_t lv_font_Arabic_18;

 lv_font_t* lv_font_size[] = {
  &lv_font_simpchinese_22,
  &lv_font_traditional_22,
  &lv_font_Japanese_20,
  & lv_font_europe_20,
  &lv_font_thai_18,
  &lv_font_russian_18,
  & lv_font_europe_20,
  & lv_font_europe_20,
  & lv_font_europe_20,
  & lv_font_europe_20,
  & lv_font_europe_20,
  & lv_font_europe_20,
  &lv_font_Arabic_18
};
lv_font_t* lv_font_all= NULL;
extern "C"
{
	int LcdInit(void);
	int LcdSetBacklight(int level);
}

GlobalData* GlobalData::instance_ = 0;
GlobalData* GlobalData::Instance()
{
    if (0 == instance_) {
		instance_ = new GlobalData();
	}
	return instance_;
}

void GlobalData::Uninstance()
{
    if (0 != instance_) {
		delete instance_;
		instance_ = 0;
	}
}

GlobalData::GlobalData() : g_pImgVirAttr(NULL), once_load_file_(50), version_(""),
	stop_keypad_(false), UI_mode_(UIMode_Videotape), SDCard_write_speed_(0), 
	stop_screen_refresh_(false), upgrading_(false), app_page_(APP_PAGE_PREVIEW),
	gdb_debug_(false)
{
	for (int i = 0; i < 2; i++) {
		opened_subpage_[i] = NULL;
	}
}

GlobalData::~GlobalData()
{
}

int GlobalData::LcdLightInit()
{
#ifdef BOARD8520DV200
	LcdInit();
#endif
	return 0;
}

int GlobalData::SetLcdLight(int level)
{
#ifdef BOARD8520DV200
	LcdSetBacklight(level);
#endif
	return 0;
}


int GlobalData::SetCurTime(int64_t sys_time)
{
	XMLogI("sys_time = %lld", sys_time);
    FILE* fp = fopen(kSysTime, "w+");
	if (!fp) {
		XMLogE("create %s error!", kSysTime);
		return -1;
	}

	char buf[32] = {};
	sprintf(buf, "%lld\n", sys_time);
	fputs(buf, fp);
    if (fp) {
        fclose(fp);
    }

    return 0;
}

int64_t GlobalData::GetCurTime()
{
	XMLogI("GetCurTime");
	FILE* fp = fopen(kSysTime, "r");
	if (!fp) {
		XMLogE("open %s error!", kSysTime);
		return -1;
	}

	char buf[32] = {};
	fgets(buf, 32 , fp);
    if (fp) {
        fclose(fp);
    }

	return atoll(buf);
}
