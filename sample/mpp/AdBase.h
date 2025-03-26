#pragma once
#include "xm_middleware_def.h"
class AdBase
{
public:
    AdBase();
    virtual ~AdBase();

    virtual int Init(int channel_num);
    virtual int Start();
    virtual int Stop();
    virtual int Clean();
    virtual int GetCapSolution(int channel, int& width, int& height);
    virtual int SetADType(XM_VIDEO_FMT ad_type);
};