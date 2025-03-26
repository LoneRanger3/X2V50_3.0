#ifndef __PAGEPLAYBACK_H__
#define __PAGEPLAYBACK_H__

#include <map>

enum PlaybackSubpage
{
    PlaybackSubpage_DelCur,
    PlaybackSubpage_DelAll,
	PlaybackSubpage_LockCur,
	PlaybackSubpage_UnlockCur,
	PlaybackSubpage_LockAll,
	PlaybackSubpage_UnlockAll,
    PlaybackSubpage_Total,
	PlaybackSubpage_Return,
    PlaybackSubpage_Main,
};

typedef struct _CurrentFileInfo
{
	char file_path[XM_MAX_STORAGE_PATH_LEN];
	char file_name[XM_MAX_STORAGE_FILE_LEN];
	int file_num;
	int direction;
	bool file_locked;

	int width;
	int height;
	int play_handle;
	int video_duration;
	int video_current_time;
}CurrentFileInfo;

enum FileType
{
	FileType_FrontVideo,
	FileType_FrontLockedVideo,
	FileType_FrontPhoto,
	FileType_BehindVideo,
	FileType_BehindLockedVideo,
	FileType_BehindPhoto,
	FileType_Total,
	FileType_Return,
};

enum PlayBackBtn
{
	PlayBackBtn_ReturnRec,
	PlayBackBtn_ReturnPre,
	PlayBackBtn_Pre,
	PlayBackBtn_Play,
	PlayBackBtn_Next,
	PlayBackBtn_Menu,
	PlayBackBtn_Total,
};

class PagePlayback
{
public:
	PagePlayback();
	~PagePlayback();

	void OpenPage();
	void SetPlaybackWinSize(int x, int y, int width, int height);
	void PlaybackKeypadEvent(int key);
	int PlaybackPrepareWork(bool open_page);
	void PauseVideo(bool pause_video);
	void UnplugSDCard();
	int SearchFile(int file_type);
	void OpenSelectFilePage();
	void SelectFileStyle(lv_obj_t* list, lv_coord_t width, lv_coord_t height);
private:
	void CreatePage();
	void PlayVideo();
	void VideoPreview();
	void OpenSetPage();
	void SwitchFile();
	void OpenConfirmPage(int flag);
	void DeleteVideoFile();
	void DeleteAllVideoFile();
	static void VideoTimer(lv_timer_t* timer);
	static void OpenSubpage(lv_event_t* e);
	static void DeletedEvent(lv_event_t* e);
	static void SelectFileTypeEvent(lv_event_t* e);
	static void DelSelectFilePageEvent(lv_event_t* e);
	static void BtnEvent(lv_event_t* e);
	static void ChangeProgressEvent(lv_event_t* e);
	static void DelSetPageEvent(lv_event_t* e);
	static void ManageFileEvent(lv_event_t* e);
	static void HiddenTopPageTimer(lv_timer_t* timer);
	static void OpenPageTimer(lv_timer_t* timer);
public:
	lv_obj_t* playback_page_;
	lv_obj_t* video_set_page_;
	bool play_preview_;
	bool playing_file_;
	int video_count_;
	bool close_preview_;
	lv_timer_t* video_timer_;
	lv_obj_t* select_filetype_page_;
	int file_type_;
	lv_obj_t* filetype_img_[FileType_Total];
	lv_timer_t * OpenPageTimer_handle;
private:
	lv_obj_t* video_label_;
	lv_obj_t* video_time_label_;
	lv_obj_t* foucused_obj_[2];
	lv_obj_t* total_duration_label_;
	lv_obj_t* lock_label_;
	lv_obj_t* resolution_label_;
	lv_obj_t* play_slider_;
	lv_obj_t* playback_btn_[PlayBackBtn_Total];
	XM_MW_Record_List file_list_;
	CurrentFileInfo playing_file_info_;
	std::map<int, int> file_index_map_;
	lv_obj_t* set_btn_[PlaybackSubpage_Total];
};

#endif // !__PAGEPLAYBACK_H__
