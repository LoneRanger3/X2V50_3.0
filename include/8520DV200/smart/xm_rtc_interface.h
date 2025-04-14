/*
* xm_rtc_interface.h
*
*	Created on: 2021��11��10��
*		Author: mdp
*/

#ifndef _XM_RTC_INTERFACE_H_
#define _XM_RTC_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif


#define RTC_MAJOR_VERSION               (0)             /* ��������㷨���汾�� */
#define RTC_MINOR_VERSION               (1)             /* ��������㷨�ΰ汾�� */
#define RTC_REVERSION                   (1)             /* ��������㷨�޸İ汾�� */
#define RTC_BUILD                       (0)             /* ��������㷨����汾��(�ݲ���) */


 
 int XM_RTC_INIT(int fps, void **ppvHandle);
 
 int XM_RTC_WORK(short* indata, short* outsign, void **ppvHandle);
 
 
 
#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */
#endif /* _IA_GMM_INTERFACE_H_ */