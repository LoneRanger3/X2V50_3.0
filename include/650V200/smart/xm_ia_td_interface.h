/*****************************************************************************

------------------------------------------------------------------------------
                            xm_ia_td_interface.h
��Ʒ��:    ����Ŀ�����㷨
ģ����:    ����Ŀ�����㷨
��������:   2022-07-10
����:      ������
�ļ�����:   ����Ŀ�����㷨����ͷ�ļ�
�汾�ţ�    V1.1.0
*****************************************************************************/
#ifndef _XM_IA_TD_INTERFACE_H_
#define _XM_IA_TD_INTERFACE_H_

#include "xm_ia_comm.h"
#include "xm_ia_rule.h"

#define TD_MAJOR_VERSION                (1)             /* Ŀ�����㷨���汾�� */
#define TD_MINOR_VERSION                (1)             /* Ŀ�����㷨�ΰ汾�� */
#define TD_REVERSION                    (0)             /* Ŀ�����㷨�޸İ汾�� */
#define TD_BUILD                        (0)             /* Ŀ�����㷨����汾��(�ݲ���) */
#define XM_IA_TD_MAX_PIXEL_NUM          (4)             /* ���֧��ͼ��ֱ��ʸ��� */
#define XM_IA_TD_FEATURE_NUM            (512)           /* ���������Ŀ */

#ifdef  __cplusplus
extern "C" {
#endif

/* ���ε���״̬ */
typedef enum
{
    XM_IA_STANDING,                 /* վ�� */
    XM_IA_FALLDOWN,                 /* ���� */
    XM_IA_LYING,                    /* ƽ���� */
    XM_IA_GETUP,                    /* վ���� */
}XM_IA_FALLDOWN_E;

/* ���γ��̴�绰��Ϊ */
typedef enum
{
    XM_IA_TD_ACT_NORMAL = 0,                /* �����̲���绰 */
    XM_IA_TD_ACT_SMOKE = 1,                 /* ���� */
    XM_IA_TD_ACT_PHONE = 2,                 /* ��绰 */
}XM_IA_TD_ACTION_E;

/* Ŀ�������� */
typedef enum
{
    XM_IA_TD_PED_Near = 1,          /* ���������μ��汾 cif��300ms */
    XM_IA_TD_PED_N = 3,             /* ���μ��nano�汾 */
    XM_IA_TD_PED_T = 4,             /* ���μ��tiny�汾 */
    XM_IA_TD_PED_S = 5,             /* ���μ��small�汾 */
    XM_IA_TD_PED_M = 6,             /* ���μ��middle�汾 */
    XM_IA_TD_PED_L = 7,             /* ���μ��large�汾 */
    XM_IA_TD_PED_X = 8,             /* ���μ��extra large�汾 */
    XM_IA_TD_DRIVER = 9,             /* ��ʻ��˾����� �����Ŀ */

    XM_IA_TD_PED_HEAD = 10,         /* ��ͷ��⣬��ͷ���� */
    XM_IA_TD_BIKE = 30,             /* �ǻ�������� */
    XM_IA_TD_PBC = 50,              /* �����˼�⣺���������ﳵ���˺�����*/

    XM_IA_TD_InVehicle_Front = 160, /* ��������ͷϵ�У�ǰװ */
    XM_IA_TD_InVehicle_Side = 161,  /* ��������ͷϵ�У���װ */
    XM_IA_TD_InVehicle_After = 162, /* ��������ͷϵ�У���װ */

    XM_IA_TD_DRIVER_BEHAVIOR = 200, /* DMSʹ��,��ʻԱ��Ϊ��⡣�����̡���绰��ϵ��ȫ������ˮ�������ڵ��� */
    XM_IA_TD_NUM                    /* Ŀ�����㷨ģ�͸��� */
}XM_IA_TD_TYPE_E;

/* �ͼ�ͼ��ԽС��������Խ��������Խʡ */
typedef enum
{
    XM_IA_TD_IMG_HCIF = 0,          /* Half CIFͼ���ͼ� */
    XM_IA_TD_IMG_CIF = 1,           /* CIFͼ���ͼ� */
    XM_IA_TD_IMG_2CIF = 2,          /* 2CIFͼ���ͼ� */
    XM_IA_TD_IMG_D1 = 3,            /* D1ͼ���ͼ� */
    XM_IA_TD_IMG_CIF_2W = 4,        /* CIFͼ���ͼ�,��ͼ����ƴ�� */
}XM_IA_TD_PATTERN_E;

/* ���ٷ��� */
typedef enum
{
    XM_IA_TD_TRACK_FEATURE = 0,     /* ���ѧϰ������ */
    XM_IA_TD_TRACK_IOU = 1,         /* IOU�ص��� */
    XM_IA_TD_TRACK_SC = 2,          /* SHIFT������ */
    XM_IA_TD_TRACK_SINGLE = 3,      /* ��Ŀ��SHIFT��������(��npu�ݲ�֧��)Ĭ�Ͽ������� */
}XM_IA_TD_TRACK_TYPE_E;

/* �������Ƶ�ʷ��� */
typedef enum
{
    XM_IA_TD_FACE_NORMAL = 0,       /* ��������ģʽ��������������� */
    XM_IA_TD_FACE_SINGLE = 10,      /* ��������ģʽ��һ��ͼֻ���һ������ */
    XM_IA_TD_FACE_HIGHF = 20,       /* ��Ƶģʽ�������,ÿ��3֡/�� */
    XM_IA_TD_FACE_MIDF = 22,        /* ��Ƶģʽ�������,ÿ��6֡/�� */
    XM_IA_TD_FACE_LOWF = 24,        /* ��Ƶģʽ�������,ÿ��12֡/�� */
    XM_IA_TD_FACE_ONCE = 100,       /* ÿ������ֻ���һ�� δ֧��*/
}XM_IA_TD_FACE_FREQ_TYPE_E;

/* �������� */
typedef struct
{
    int iTrackFlag;                                 /* ���ٿ��أ�0:�ر� 1:������*/
    XM_IA_TD_TRACK_TYPE_E iTrackType;               /* ���ٷ��� */

    unsigned char ucReserved[128];                  /* Ԥ��λ */
}XM_IA_TD_TRACK_S;

/* ���⹦������ */
typedef struct
{
    unsigned char ucPedFallDownEnable;      /* ���ε������� */
    unsigned char ucTDFeatureEnable;        /* Ŀ���������� ��0:�ر� 1:������*/
    unsigned char ucSmokeEnable;            /* ���̴�绰��⿪�� */
    unsigned char ucFaceDetEnable;          /* ������⿪�� */
    unsigned char ucFaceRegEnable;          /* ����ʶ�𿪹� */
    unsigned char ucPlateDetEnable;         /* ���Ƽ�⿪�� */
    unsigned char ucPlateRegEnable;         /* ����ʶ�𿪹� */
    unsigned char acReservedEnable[32];     /* ����������Ԥ��λ */

    XM_IA_TD_FACE_FREQ_TYPE_E efdfreq;      /* �������Ƶ������ */
    unsigned char acReserved[92];           /* ����������Ԥ��λ */
}XM_IA_TD_EXTRAFUNC_S;

/* ������Ϣ */
typedef struct
{
    float afTDFeature[XM_IA_TD_FEATURE_NUM];     /* Ŀ������ */
}XM_IA_TD_FEATRUES_S;

/* ������Ϣ */
typedef struct
{
    unsigned char ucFaceDetFlag;                /* �Ƿ����������1�����ڣ�0 ������ */
    unsigned char ucFaceRegFlag;                /* �Ƿ�ʶ��������1��ʶ��0 δʶ�� */
    float fScore;                               /* �������Ÿ��� */
    float fClearness;                           /* ������������ֵ,ֵԽ��Խ���� */
    XM_IA_RECT_S stRect;                        /* �������� (ȡֵ��Χ[0,8191]����Ӧ0��100%) */
    XM_IA_TD_FEATRUES_S *pstFeature;            /* ����������� */

    unsigned char ucReserved[128];              /* Ԥ��λ */
}XM_IA_TD_FACERELT_S;

/* ������Ϣ */
typedef struct
{
    unsigned char ucPlateDetFlag;               /* �Ƿ���ڳ��ƣ�1�����ڣ�0 ������ */
    unsigned char ucPlateRegFlag;               /* �Ƿ�ʶ���ƣ�1��ʶ��0 δʶ�� */
    float fScore;                               /* �������Ÿ��� */
    char acPlateColor[3];                       /* ������ɫ�������ڰ׻�"��"����δ֪���ַ���������"\0" */
    char acPlateCode[20];                       /* ���ƺ��룬β�����ַ���������"\0" */
    XM_IA_RECT_S stRect;                        /* �������� (ȡֵ��Χ[0,8191]����Ӧ0��100%) */

    unsigned char ucReserved[128];              /* Ԥ��λ */
}XM_IA_TD_PLATERELT_S;

/* ���򴥷���Ϣ */
typedef struct
{
    unsigned int uiAlarmRuleIdx;                            /* ��������� */
    XM_IA_DIRECTION_E eAlarmDirection;                      /* �������� */
    XM_IA_RULE_STATUS_S astRuleStatus[XM_IA_RULE_NUM_MAX];  /* ÿ�������Ӧ״̬ */
    unsigned char ucAllAlarm[XM_IA_RULE_NUM_MAX];           /* ÿ�������Ӧ�����źţ�0Ϊδ����/1Ϊ���� */

    unsigned char ucReserved[124];                          /* Ԥ��λ */
}XM_IA_TD_RULE_INFO_S;

typedef struct
{
    XM_IA_FALLDOWN_E eStatus;               /* ���ε���״̬ */
    XM_IA_TD_FEATRUES_S *pstFeature;        /* Ŀ������ */
    XM_IA_TD_ACTION_E eAction;              /* ����Ŀ����Ϊ�����̡���绰�ȣ� */
    XM_IA_TD_FACERELT_S stFaceInfo;         /* ���������Ϣ */
    XM_IA_TD_PLATERELT_S stPlateInfo;       /* ���ƽ����Ϣ */

    unsigned char ucReserved[512];          /* Ԥ��λ */
}XM_IA_TD_CHARACTER_S;

/* Ŀ����Ϣ */
typedef struct
{
    unsigned int uiId;                          /* Ŀ��ID�� */
    unsigned char ucAlarm;                      /* �����ź�(0:δ���� 1:����) */
    float fScore;                               /* Ŀ�����Ŷ�0.0-1.0 */
    XM_IA_RECT_S stRect;                        /* Ŀ������ (ȡֵ��Χ[0,8191]����Ӧ0��100%) */
    XM_IA_TARGET_TYPE_E eTargetType;            /* Ŀ������ */
    XM_IA_TD_RULE_INFO_S stRuleinfo;            /* ���򴥷������Ϣ */
    XM_IA_TD_CHARACTER_S stCharacter;           /* Ŀ�긽�ӹ��ܽ�� */
    unsigned char ucSingleTrackFlag;            /* ��Ŀ�����ģʽ�£���ǰid�Ƿ񱻸���(1:������ 0:δ������) */

    unsigned char ucReserved[128];              /* Ԥ��λ */
}XM_IA_TD_INFO_S;

/* ����Ŀ�����㷨��� */
typedef struct
{
    unsigned int uiTargetNum;                   /* Ŀ���� */
    XM_IA_TD_INFO_S *pstInfo;                   /* Ŀ����Ϣ���ӿڵ���ʱ��Ҫ�������Ŀ�����������ڴ� */

    unsigned char ucReserved[128];              /* Ԥ��λ */
}XM_IA_TD_INFOS_S;

/* ����Ŀ�����㷨������ṹ�� */
typedef struct
{
    int iMDFlag;                                                /* ���쿪�� ��0:�ر� 1:������*/
    unsigned char ucDetectDisable;                              /* Ŀ���⹦�ܹرգ��رպ�ֻ���ƶ����; 0��������1���ر� */
    XM_IA_TD_TYPE_E iDetectType;                                /* Ŀ��������(��ͬ���Ͷ�Ӧ��ͬ������) */
    XM_IA_TD_PATTERN_E ePattern;                                /* ��Ŀ���ͼ���Сѡ��ͼ��Խ�󣬼�����ԽԶ�������ڴ�Խ��֡��Խ�� */
    XM_IA_SENSE_E eSensitivity;                                 /* Ŀ���������� */
    XM_IA_RECT_S stDetRegion;                                   /* ������� */
    XM_IA_PD_RULES_S stTdRules[XM_IA_RULE_NUM_MAX];             /* Ŀ������� */

    unsigned char ucReserved[128];                              /* Ԥ��λ */
}XM_IA_TD_CONFIG_S;

/* Ŀ�����㷨��ʼ���ṹ�� */
typedef struct
{
    /* INPUT */
    unsigned int uiInStructSize;                    /* Run��β����ṹ�壨XM_IA_TD_IN_S�����Size */
    unsigned int uiInitStructSize;                  /* ��ʼ�������ṹ��(XM_IA_TD_INIT_S)���Size */
    unsigned int uiConfigStructSize;                /* ���ò����ṹ��(XM_IA_TD_CONFIG_S)���Size */
    unsigned int uiRsltStructSize;                  /* �㷨��������ṹ��(XM_IA_TD_RESULT_S)���Size */

    unsigned int uiTdMaxNum;                        /* ���Ŀ��������Ĭ������32 */
    char acClassBinPath[XM_IA_PATH_MAX_LEN];        /* �������ļ�(.xmnn, .wk,.bin��)·�� */
    XM_IA_PLATFORM_E ePlatForm;                     /* оƬƽ̨ѡ�� */
    XM_IA_IMG_TYPE_E eDevImgType;                   /* �豸ͼ������ */
    XM_IA_IMG_SIZE_S stDevImgSize;                  /* �豸ͼ��ߴ� */
    XM_IA_TD_CONFIG_S stTdCfg;                      /* Ŀ���������� */
    XM_IA_TD_TRACK_S stTrack;                       /* �������� */
    XM_IA_TD_EXTRAFUNC_S stExtraFunc;               /* Ŀ���⹦�ܿ��ؿ����� */

    /* OUTPUT */
    XM_IA_IMG_SIZE_S stAlgImgSize;                  /* �㷨ͼ��ߴ� */
    unsigned int uiBufferSize;                      /* ��Ҫ������ڴ��С��work����Ҫ�õ� */

    unsigned char ucReserved[128];                  /* Ԥ��λ */
}XM_IA_TD_INIT_S;

/* Ŀ�����㷨work������� */
typedef struct
{
    void *pcBuffer;                                         /* ���ݳ�ʼ�����ص�uiBufferSize��С�������ڴ� */
    XM_IA_IMAGE_S *pstImage[XM_IA_TD_MAX_PIXEL_NUM];        /* ͼ��ṹ��,��ǰֻ��pstImage[0] */

    unsigned char ucReserved[128];                          /* Ԥ��λ */
}XM_IA_TD_IN_S;

/* ����Ŀ�����㷨��� */
typedef struct
{
    XM_IA_TD_INFOS_S stTargetInfo;              /* Ŀ����Ϣ����ǰ������ */
    XM_IA_TD_INFOS_S stLostInfo;                /* Ŀ�궪ʧ��Ϣ����ǰ��ʧ��Ŀ�� */

    unsigned char ucReserved[128];              /* Ԥ��λ */
}XM_IA_TD_RESULT_S;

/* ���������б� */
typedef struct
{
    int iFeatureListNum;                        /* ������������������ */
    XM_IA_TD_FEATRUES_S **ppstFeatureItem;      /* ���������б�ָ�� */

    unsigned char ucReserved[128];              /* Ԥ��λ */
}XM_IA_TD_FEATRUES_LIST_S;

/* ���������ȶԽ�� */
typedef struct
{
    int iTDMatchId;                             /* ƥ�䵽��Ŀ��λ��XM_IA_TD_FEATRUES_LIST_S�������±� */
    float fMatchingRate;                        /* �����(0~100%) */

    unsigned char ucReserved[128];              /* Ԥ��λ */
}XM_IA_TD_MATCH_RSLT_S;

/* Ŀ�������ȶ�,���1-2��������Ŀ����
param [in] pstFeature      Ŀ������
param [in] pstFeatureList  Ŀ�������б�
param [in] fThresh         �ȶ���ֵ(0~100%),������ֵ�����ƥ��ɹ�
param [out] pstMatchRslt   Ŀ��ƥ����
return 0  �ɹ�
return !0 ʧ�� */
int XM_IA_TD_MatchFeature(XM_IA_TD_FEATRUES_S *pstFeature, XM_IA_TD_FEATRUES_LIST_S *pstFeatureList, float fThresh, XM_IA_TD_MATCH_RSLT_S *pstMatchRslt);

#ifdef __cplusplus
}
#endif

#endif