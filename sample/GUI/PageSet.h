#ifndef __PAGESET_H__
#define __PAGESET_H__

#include "set_config.h"

enum SetPageSubpage1
{
	
};

enum SetPageSubpageUnused
{
	Subpage_ImageParam,
	Subpage_SwitchCamera,
	Subpage_PullBackMirror,
	Subpage_Sensitivity,
	Subpage_CompactRecord,
	Subpage_CompactRecordDuration,
};

class PageSet
{
public:
	PageSet();
	~PageSet();

	void OpenPage();
	void SetSwitchStyle(lv_obj_t* sw);
	void RecordVoiceEnable(bool enable);
	void ReturnPreMenu();
	static void CloseSubPage(lv_event_t* e);
private:
	void CreatePage();
	static void OpenSubpage(lv_event_t* e);
	static void OpenSubpage_Function(lv_event_t* e);
	static void MirrorSwitch(lv_event_t* e);
	static void DelSetPageEvent(lv_event_t* e);
private:
	
public:
	lv_obj_t* set_page_;
	uint8_t menu_position;
	lv_obj_t* record_set_list_;
	lv_obj_t* sys_set_list_;
	lv_obj_t* title_label_;
	lv_obj_t* foucused_obj;
	lv_obj_t* cfg_label_[Subpage_Total];
	lv_style_t subpage_style_;
	lv_obj_t* menu_img_[Subpage_Total];
};

#endif // !__PAGESET_H__
