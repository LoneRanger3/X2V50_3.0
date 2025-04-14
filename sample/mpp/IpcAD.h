#pragma once
#include "AdBase.h"

class IpcAD : public AdBase
{
public:
    IpcAD();
    ~IpcAD();

    virtual int SetADType(XM_VIDEO_FMT ad_type) override {ad_type_ = ad_type; return 0;};
    virtual int Init(int channel_num) override;
    virtual int Start() override;
    virtual int Stop() override;
    virtual int Clean() override;
    virtual int GetCapSolution(int channel, int& width, int& height) override;
    
private:
    XM_VIDEO_FMT ad_type_;
};

