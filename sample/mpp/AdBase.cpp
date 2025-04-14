#include "AdBase.h"
AdBase::AdBase()
{

}

AdBase::~AdBase()
{

}

int AdBase::Init(int channel_num)
{
    return 0;
}

int AdBase::Start()
{
    return 0;
}

int AdBase::Stop()
{
    return 0;
}

int AdBase::Clean()
{
    return 0;
}

int AdBase::GetCapSolution(int channel, int& width, int& height)
{
    return -1;
}

int AdBase::SetADType(XM_VIDEO_FMT ad_type)
{
    return 0;
}