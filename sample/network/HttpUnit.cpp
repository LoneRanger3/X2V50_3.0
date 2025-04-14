#include "HttpUnit.h"
#include <algorithm>
#include <string>
#include <sstream>
#include <fstream>
#include <thread>
#ifndef WIN32
#include <fcntl.h>
#include <sys/prctl.h>
#endif
#include "Log.h"
#include "xm_middleware_api.h"
#include "xm_middleware_network.h"
#include "CommDef.h"

const int kBufSize = 16384;
using namespace std;

HttpUnit::HttpUnit(): thread_exit_(false)
{
    msg_info_.engineId = -1;
    msg_info_.connId = -1;
}

HttpUnit::~HttpUnit()
{
    int engineId = msg_info_.engineId;
    int connId = msg_info_.connId;
    if (engineId >= 0 && connId >= 0) {
        XMLogW("~HttpUnit, connId=%d", connId);
        XM_Middleware_Network_CloseClient(engineId, connId);
    }
    Clean();
}

void ThreadPlayFunction(void* param)
{
	static_cast<HttpUnit*>(param)->Run();
}

int HttpUnit::Init(XMMsgInfo& msg_info)
{
	msg_info_ = msg_info;
	CreateThreadEx(send_thread_, (LPTHREAD_START_ROUTINE)ThreadPlayFunction, this);
	return 0;
}

int HttpUnit::Clean()
{
    thread_exit_ = true;
    WaitForSingleObjectEx(send_thread_, INFINITE);
    CloseThreadEx(send_thread_);
	return 0;
}

int HttpUnit::Run()
{
#ifndef WIN32
	prctl(PR_SET_NAME, "http_unit");
#endif   
    int engineId = msg_info_.engineId;
    int connId = msg_info_.connId;
    char* msg = (char*)(msg_info_.msg.c_str());
    char* connType = (char*)(msg_info_.connType.c_str());

    bool play_thumbnail = false;
    if (strstr(msg, "getthumbnail") != NULL) {
        play_thumbnail = true;
    }

    if (play_thumbnail) {
        char* file_path = strstr(msg, "=") + 1;
        XMLogW("play thumbnail-->filename:%s", msg);
        char thumbnail_buf[MAX_PNG_FILE_LEN];
        int thumbnail_len = 0;
        int ret = XM_Middleware_Picture_GetFromRecord(XM_FILE_FORMAT_MP4, file_path, thumbnail_buf, &thumbnail_len);
        if (ret < 0 || thumbnail_len <= 0 || thumbnail_len > MAX_PNG_FILE_LEN) {
            XMLogW("XM_Middleware_Picture_GetFromRecord failed! path:%s, len:%d", file_path, thumbnail_len);
            thread_exit_ = true;
            return -1;
        }

        // static int zgf = 1000;
        // char str_file[64] = {0};
        // sprintf(str_file, "/mnt/tfcard/%d.png", zgf++);
        // FILE* fp = fopen(str_file, "wb");
        // if (fp) {
        //     fwrite(thumbnail_buf, 1, thumbnail_len, fp);
        //     fclose(fp);
        // }
        StatusCode status_code = Ok;
        ostringstream oss;
        oss << "HTTP/1.1 " << status_code << " message" << "\r\n";
        oss << "Access-Control-Allow-Origin: *\r\n";
        oss << "Accept-Ranges: bytes\r\n";
        oss << "Content-Type: application/octet-stream\r\n";
        oss << "Content-Length: " << thumbnail_len << "\r\n";
        oss << "\r\n";

        int result = XM_Middleware_Network_SendFrame(engineId, connId, (char*)oss.str().c_str(), oss.str().length());
        unsigned int remain_len = thumbnail_len;
        XMLogW("-------connId=%d, total len=%d--------", connId, remain_len);

        char* beg_ptr = thumbnail_buf;
        chrono::milliseconds dura(5);
        while (remain_len > 0 && !thread_exit_) {
            int read_len = remain_len > kBufSize ? kBufSize : remain_len;
            int isend = XM_Middleware_Network_SendFrame(engineId, connId, beg_ptr, read_len);
            if (isend < 0) {
                XMLogI("connId=%d,isend:%d,remain_len:%d", connId, isend, remain_len);
                if (isend == -99) {
                    int cir_num = 0;
                    while (isend < 0) {
                        XMLogI("connId=%d,isend:%d,cir_num:%d", connId, isend, cir_num);
                        std::this_thread::sleep_for(dura);
                        isend = XM_Middleware_Network_SendFrame(engineId, connId, beg_ptr, read_len);
                        if (isend == -102 || cir_num > 10) {
                            thread_exit_ = true;
                            return -1;
                        }
                        cir_num++;
                    }
                }
                else {
                    thread_exit_ = true;
                    return -1;
                }
            }

            beg_ptr += read_len;
            remain_len -= read_len;
        }
    }
    else {
        ifstream ifs;
        XMLogW("play video-->filename:%s", msg);
        ifs.open(msg, ios::in | ios::binary);
        if (!ifs.is_open())
            XMLogE("%s is open failed");

        int s, e;
        s = ifs.tellg();
        ifs.seekg(0, ios::end);
        e = ifs.tellg();
        int video_length = e - s;
        ifs.seekg(0, ios::beg);

        char buf[kBufSize];
        int beg_num = 0, end_num = 0;
        char* ptmp = strstr(connType, "bytes");
        StatusCode status_code = PartialContent;
        if (ptmp == NULL) {
            beg_num = 0;
            end_num = video_length - 1;
            status_code = Ok;
        }
        else {
            string range_value(connType);
            // XMLogW("range_value:%d",range_value.length());
            int pos = range_value.find("-");
            string beg = range_value.substr(7, pos - 7);
            string end = range_value.substr(pos + 1);
            // XMLogW("brg:%s,pos:%d",beg.c_str(), pos);
            if (pos == range_value.length() - 2) {
                end_num = video_length - 1;
                beg_num = atoi(beg.c_str());
                // status_code = Ok;
            }
            else if (pos == 7) {
                beg_num = video_length - atoi(end.c_str());
                end_num = video_length - 1;
            }
            else {
                beg_num = atoi(beg.c_str());
                end_num = atoi(end.c_str());
            }
        }

        if (beg_num < 0 || beg_num > end_num || beg_num > video_length || end_num < 0 || end_num > video_length) {
            XMLogW("playback params error! beg_num:%d, end_num:%d, video_length:%d", beg_num, end_num, video_length);
            ifs.close();
            thread_exit_ = true;
            return -1;
        }

        ostringstream oss;
        oss << "HTTP/1.1 " << status_code << " message" << "\r\n";
        oss << "Access-Control-Allow-Origin: *\r\n";
        oss << "Accept-Ranges: bytes\r\n";
        oss << "Content-Type: application/octet-stream\r\n";
        if (status_code == PartialContent)
            oss << "Content-Range: bytes " << beg_num << "-" << end_num << "/" << video_length << "\r\n";
        oss << "Content-Length: " << video_length << "\r\n";
        oss << "\r\n";
        //  XMLogI("resp_body:%s", oss.str().c_str());
        int result = XM_Middleware_Network_SendFrame(engineId, connId, (char*)oss.str().c_str(), oss.str().length());
        // ��Ҫ��need_len���ֽ�
        unsigned int remain_len = end_num - beg_num + 1;
        XMLogW("-------connId=%d, total len=%d, beg_num:%d, end_num:%d--------",
            connId, remain_len, beg_num, end_num);

        ifs.seekg(beg_num, ios::beg);
        chrono::milliseconds dura(50);
        while (remain_len > 0 && !thread_exit_) {
            int read_len = remain_len > kBufSize ? kBufSize : remain_len;
            ifs.read(buf, read_len);
            int isend = XM_Middleware_Network_SendFrame(engineId, connId, buf, read_len);
            if (isend < 0) {
                // XMLogI("connId:%d,isend:%d,remain_len:%d", connId, isend, remain_len);
                if (isend == -99) {
                    int cir_num = 0;
                    while (isend < 0 && !thread_exit_) {
                        // XMLogI("connId:%d,isend:%d,cir_num:%d", connId, isend, cir_num);
                        std::this_thread::sleep_for(dura);
                        isend = XM_Middleware_Network_SendFrame(engineId, connId, buf, read_len);
                        if (isend == -102 || cir_num > 20) {
                            ifs.close();
                            thread_exit_ = true;
                            return -1;
                        }
                        cir_num++;
                    }
                }
                else {
                    ifs.close();
                    thread_exit_ = true;
                    return -1;
                }
            }
            remain_len -= read_len;
        }
        ifs.close();
    }

    XMLogW("file is send over ,connid:%d close thread", connId);
    thread_exit_ = true;
	return 0;
}