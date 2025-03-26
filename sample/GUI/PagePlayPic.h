#ifndef __PAGEPLAYPIC_H__
#define __PAGEPLAYPIC_H__

typedef struct _CurrentPicInfo
{
	char file_path[XM_MAX_STORAGE_PATH_LEN];
	char file_name[XM_MAX_STORAGE_FILE_LEN];
	int file_num;
	int direction;
	bool file_locked;

	int width;
	int height;
	int file_len;
}CurrentPicInfo;

enum PlayPicBtn
{
	PlayPicBtn_ReturnRec,
	PlayPicBtn_ReturnPre,
	PlayPicBtn_Pre,
	PlayPicBtn_Next,
	PlayPicBtn_Menu,
	PlayPicBtn_Total,
};

class PagePlayPic
{
public:
	PagePlayPic();
	~PagePlayPic();

	void OpenPage();
	int PlayPicPrepareWork(bool open_page);
	void PlayPicKeypadEvent(int key);
	void UnplugSDCard();
	int SearchFile(int file_type);
private:
	void CreatePage();
	void PlayPic();
	void OpenSetPage();
	void SwitchFile();
	void OpenConfirmDelPage(int flag);
	void DelPicFile();
	void DelAllPicFile();
	static void OpenSubpage(lv_event_t* e);
	static void DeletedEvent(lv_event_t* e);
	static void BtnEvent(lv_event_t* e);
	static void DelSetPageEvent(lv_event_t* e);
	static void ManageFileEvent(lv_event_t* e);
public:
	lv_obj_t* play_pic_page_;
	lv_obj_t* set_page_;
	int pic_count_;
	int file_type_;
private:
	CurrentPicInfo playing_file_info_;
	XM_MW_Record_List file_list_;

	lv_obj_t* name_label_;
	lv_obj_t* resolution_label_;
	lv_obj_t* lock_label_;
	lv_obj_t* pre_label_;
	lv_obj_t* next_label_;
	lv_obj_t* foucused_obj_[2];

	bool playing_file_;
	std::map<int, int> file_index_map_;
	lv_obj_t* set_btn_[PlaybackSubpage_Total];
	lv_obj_t* playpic_btn_[PlayPicBtn_Total];
};

#endif // !__PAGEPLAYPIC_H__
