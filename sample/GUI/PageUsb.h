#ifndef __PAGEUSB_H__
#define __PAGEUSB_H__

class PageUsb
{
public:
    PageUsb();
    ~PageUsb();

    void OpenPage();
    int SetUsbFunc(USB_MODE_E mode);
private:
    void OpenMemoryModePage(USB_MODE_E mode);
    static void SelectUsbFuncEvent(lv_event_t* e);
public:
    lv_obj_t* usb_page_;
    USB_MODE_E user_selected_usb_mode_;
private:
    USB_MODE_E usb_mode_;
    lv_obj_t* mode_img_[3];
    lv_obj_t* mode_label_[3];
};

#endif // !__PAGEUSB_H__