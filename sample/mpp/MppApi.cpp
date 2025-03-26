#include "MppMdl.h"

int XM_Middleware_Mpp_StartPlayback(CUR_PLAY_STATUS play_status, 
    PAYLOAD_TYPE_E codec_type, RECT_S * display_region, bool audio_only, int height)
{
    return MppMdl::Instance()->StartPlayback(play_status, codec_type, display_region, audio_only, height);
}

int XM_Middleware_Mpp_SendDecFrame(long handle, XM_MW_Media_Frame * media_frame)
{
    return MppMdl::Instance()->SendDecFrame(handle, media_frame);
}

int XM_Middleware_Mpp_SetAudioOutInfo(int bit_per_sample, int samples_per_sec, int audio_channel)
{
    return MppMdl::Instance()->SetAudioOutInfo(bit_per_sample, samples_per_sec, audio_channel);
}

int XM_Middleware_Mpp_ResetDecDev()
{
    return MppMdl::Instance()->ResetDecDev();
}

int XM_Middleware_Mpp_ForceIFrame(int channel)
{
    return MppMdl::Instance()->ForceIFrame(channel);
}

int XM_Middleware_Mpp_SetTimeLapse(XMTimeLapse time_lapse)
{
    return MppMdl::Instance()->SetTimeLapse(time_lapse);
}

bool XM_Middleware_Mpp_ADLoss()
{
    return MppMdl::Instance()->AdLoss();
}

int XM_Middleware_Sound_SetVolume(int volume)
{
    return MppMdl::Instance()->SetVolume(volume);
}
