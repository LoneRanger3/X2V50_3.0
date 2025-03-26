/***********************************************************************************

------------------------------------------------------------------------------------

                            IA_Keyword_Interface.h
    ��Ʒ��: Keyword Recognize
    ģ����:
  ��������:
      ����:
  �ļ�����: ·��ͣ��ͷ�ļ�

------------------------------------------------------------------------------
   �޸���ʷ
   ����        ����             ����
  --------------------------------------------------------------------------
  �汾�ţ�   V0.1.1.0
*****************************************************************************/
#ifndef _XM_IA_KEYWORD_INTERFACE_H_
#define _XM_IA_KEYWORD_INTERFACE_H_

#ifdef  __cplusplus
extern "C"{
#endif

#include "xm_ia_comm.h"

#define KEYWORD_MAJOR_VERSION               (0)             /* �ؼ����㷨���汾�� */
#define KEYWORD_MINOR_VERSION               (1)             /* �ؼ����㷨�ΰ汾�� */
#define KEYWORD_REVERSION                   (1)             /* �ؼ����㷨�޸İ汾�� */
#define KEYWORD_BUILD                       (0)             /* �ؼ����㷨����汾��(�ݲ���) */

#define KEYWORD_MAX_ROI_NUM (1600)          /* ����������߶�(����) */
#define KEYWORD_MAX_RESULT_NUM (200)        /* ����ؼ�������(���) = (���߶� / 8) */
#define KEYWORD_MAX_CHANNEL_NUM (1)         /* ���֧��1ͨ��(����) */


/* ������ */
typedef enum
{
    XM_8K_FPS = 8000,              /* 8K������ */
    XM_16K_FPS = 16000              /* 16K������ */
}XM_IA_VOICE_FPS;


/* Ŀ����Ϣ */
typedef struct
{
    unsigned int uiId;      /* Ŀ��ID�� */
    float flConfidence;
    int tableNum;
}XM_IA_KEYWORD_INFO_S;


/* ������ṹ�� */
typedef struct
{
    int iCount;      /* ������� */
    XM_IA_KEYWORD_INFO_S astKeywordInfo[KEYWORD_MAX_RESULT_NUM];
    int aiResver[4];
} XM_IA_KEYWORD_RESULT_S;


/* Ŀ�����㷨work������� */
typedef struct
{
    int iPortNums;                  /* ���������*/
    XM_IA_IMAGE_S *pstImage;        /* ͼ��ṹ�� */
    XM_IA_POINT_S astKeywordRect;   /* ��������� */
} XM_IA_KEYWORD_IN_S;

typedef struct
{
    XM_IA_SENSE_E eAlgSense;        /* �㷨������ */
}XM_IA_KEYWORD_CONFIG_S;

/* Ŀ�����㷨��ʼ���ṹ�� */
typedef struct
{
    /* ������Ϣ */
    unsigned int uiInStructSize;                /* Run��β����ṹ����Size */
    unsigned int uiInitStructSize;              /* ��ʼ�������ṹ����Size  */
    unsigned int uiConfigStructSize;            /* ���ò����ṹ����Size    */
    unsigned int uiRsltStructSize;              /* �㷨��������ṹ����Size  */

    unsigned int uiKeywordMaxNum;               /* ���Ŀ������*/
    char acClassBinPath[XM_IA_PATH_MAX_LEN];    /* ������Bin�ļ�·�� */
    XM_IA_PLATFORM_E ePlatForm;                 /* оƬƽ̨ѡ�� */
    XM_IA_IMG_TYPE_E eDevImgType;               /* �豸ͼ������ */
    XM_IA_IMG_SIZE_S stDevImgSize;              /* �豸ͼ��ߴ� */
    XM_IA_KEYWORD_CONFIG_S stKeywordCfg;        /* �ؼ���ʶ�������� */
    XM_IA_VOICE_FPS fps;                        /* ������ */
    int iTime;                                  /* ����ʱ��/ms */
    int iStrTime;                               /* ����ʱ��/ms */
    int iNum;                                   /* ������ */
    int iNfft;                                  /* fft���� */

    /* OUTPUT */
    XM_IA_IMG_SIZE_S stKeywordAlgImgSize;           /* �㷨ͼ��ߴ� */

    int iResver[3];

} XM_IA_KEYWORD_INIT_S;


#ifdef  __cplusplus
}

#endif /* end of __cplusplus */

#endif