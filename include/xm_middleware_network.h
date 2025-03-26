#pragma once

#include <string>
#include <stdint.h>
#include "xm_middleware_def.h"
#include "HttpDef.h"

int XM_Middleware_Network_StartServer(int signal_port, int media_port);
int XM_Middleware_Network_StopServer();
int XM_Middleware_Network_SendBuffer(int engineId, int connId, uint8_t type, char * data, int contentLen);
int XM_Middleware_Network_SendTcpBuffer(int engineId, int connId, uint8_t type, char* data, int len);
int XM_Middleware_Network_SendFrame(int engineId, int connId, char* data, int len);
int XM_Middleware_Network_SendOnly(int engineId, int connId);
int XM_Middleware_Network_SetDataCallback(XM_Data_CallBack cb);
//int XM_Middleware_Network_SetEventCallback(XM_Event_CallBack cb);
int XM_Middleware_Network_SetFrameList(int count);
int XM_Middleware_Network_ClearFrameList();
int XM_Middleware_Network_CloseThreadCallback(XM_CloseThread_CallBack cb);
int XM_Middleware_Network_CloseThread(int connId);
int XM_Middleware_Network_CloseAllThread();
int XM_Middleware_Network_CloseHttpUnit(void);
int XM_Middleware_Network_CloseClient(int engineId, int connId);