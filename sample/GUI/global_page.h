#ifndef __GLOBAL_PAGE_H__
#define __GLOBAL_PAGE_H__

#include "PageMain.h"
#include "PageSet.h"
#include "PageVideoSet.h"
#include "PageSysSet.h"
#include "PagePlayback.h"
#include "PagePlayPic.h"
#include "PageUsb.h"

class GlobalPage
{
public:
	static GlobalPage* Instance();
	static void Uninstance();

	PageMain* page_main() { return &page_main_; }
	PageSet* page_set() { return &page_set_; }
	PageVideoSet* page_video_set() { return &page_video_set_; }
	PageSysSet* page_sys_set() { return &page_sys_set_; }
	PagePlayback* page_playback() { return &page_playback_; }
	PagePlayPic* page_play_pic() { return &page_play_pic_; }
	PageUsb* page_usb() { return &page_usb_; }
private:
	GlobalPage();
	~GlobalPage();
private:
	static GlobalPage* instance_;

	PageMain page_main_;
	PageSet page_set_;
	PageVideoSet page_video_set_;
	PageSysSet page_sys_set_;
	PagePlayback page_playback_;
	PagePlayPic page_play_pic_;
	PageUsb page_usb_;
};

#endif // !__GLOBAL_PAGE_H__
