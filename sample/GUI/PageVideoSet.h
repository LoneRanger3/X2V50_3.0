#ifndef __PAGEVIDEOSET_H__
#define __PAGEVIDEOSET_H__

#include "set_config.h"
enum DateWatermark
{
	DateWatermark_Close,
	DateWatermark_Open,
};

class PageVideoSet
{
public:
	PageVideoSet();
	~PageVideoSet();

	void OpenDateWatermarkPage();
	static void ChangeVideoResolution(lv_event_t* e);
	static void ChangeExposureCompensation(lv_event_t* e);
	static void ChangeWhiteBalance(lv_event_t* e);
	static void ChangeRejectFlicker(lv_event_t* e);
	static void DeletedEvent(lv_event_t* e);
	static void ChangCompactRecordFps(lv_event_t* e);
	static void ChangeCompactRecordDur(lv_event_t* e);
	static void ChangeCirRecordTime(lv_event_t* e);
	static void CollisionStartupSwitch(lv_event_t* e);
	static void ChangeRecordSoundEvent(lv_event_t* e);
	static void ChangeDateWatermarkEvent(lv_event_t* e);
	static void ChangeMirrorSwitch(lv_event_t* e);
private:
	int subpage_width_;
	int subpage_height_;
	lv_obj_t* foucused_obj_;
};

#endif // !__PAGEVIDEOSET_H__
