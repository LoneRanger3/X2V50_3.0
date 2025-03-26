/*****************************************************************************
------------------------------------------------------------------------------
xm_ia_rule.h
��Ʒ��:    �㷨����
ģ����:    �����ļ�
��������:  2020-08-28
����:     ������
�ļ�����:  ������⹫��ͷ�ļ�
*****************************************************************************/
#ifndef _XM_IA_RULE_H_
#define _XM_IA_RULE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "xm_ia_comm.h"

#define XM_IA_RULE_NUM_MAX           (4)         /* ���������㷨���������� */

/* ����ѡ�� */
typedef enum
{
    IA_TRIPWIRE,                        /* ���� */
    IA_PERIMETER,                       /* ���� */
    IA_PERIMETER_V1,                    /* �½��� */
    IA_RULE_NUM,
}XM_IA_RULE_E;

/* ����������ѡ�� */
typedef enum
{
    IA_ALARM_GROUND,                    /* �Ե���Ϊ���գ���������Ŀ���Ƿ��ھ��������Ӧ�ĵ��������� */
    IA_ALARM_BOX,                       /* ��������Ŀ����Ӧ���ĸ������Ƿ��ھ������ */
    IA_ALARM_NUM,
}XM_IA_ALARM_POINT_E;

/* ����״̬ */
typedef enum
{
    IA_RULE_STATUS_INSIDE,                        /* ������ */
    IA_RULE_STATUS_OUTSIDE,                       /* ������ */
}XM_IA_STATUS_PERIMETER_E;

/* ����״̬ */
typedef enum
{
    IA_RULE_STATUS_LEFT,                        /* ������� */
    IA_RULE_STATUS_RIGHT,                       /* �����Ҳ� */
}XM_IA_STATUS_TRIPWIRE_E;

/* ״̬������ */
typedef struct
{
    XM_IA_RULE_E eRuleType;             /* �������� */
    union
    {
        XM_IA_STATUS_TRIPWIRE_E eStatusTripwire;       /* ����״̬ */
        XM_IA_STATUS_PERIMETER_E eStatusPerimeter;     /* ����״̬ */
    }unRuleStatus;

    unsigned int uiReserved[16];        /* �������� */
}XM_IA_RULE_STATUS_S;

/* �߹���ṹ�� */
typedef struct
{
    XM_IA_LINE_S stLine;                /* ����λ��X1,Y1Ϊ��ʼ��,X2,Y2Ϊ��ֹ��(�ٷֱȱ�ʾ0~8192��Ӧ0��100%) */
    XM_IA_DIRECTION_E eAlarmDirect;     /* ˳��ʼ����ֹ�㷨�߷���Ϊ����,����ʼ����ֹ�㷨�߷���Ϊ���� */

    unsigned int uiReserved[16];        /* �������� */
}XM_IA_RULE_LINE_S;

/* �������ṹ�� */
typedef struct
{
    int iBoundaryPtsNum;                /* ��������� */
    XM_IA_POINT_S astBoundaryPts[8];    /* �������λ��(�ٷֱȱ�ʾ0~8192��Ӧ0��100%) */
    XM_IA_DIRECTION_E eAlarmDirect;     /* �������Ϊ����,�뿪����Ϊ���� */

    unsigned int uiReserved[16];        /* �������� */
}XM_IA_RULE_REGION_S;

/* �������ṹ�� */
typedef struct
{
    int iBoundaryPtsNum;                    /* ��������� */
    XM_IA_POINT_S astBoundaryPts[16];       /* �������λ��(�ٷֱȱ�ʾ0~8192��Ӧ0��100%) */
    XM_IA_DIRECTION_E eAlarmDirect;         /* �������Ϊ����,�뿪����Ϊ���� */

    unsigned int uiReserved[8];             /* �������� */
}XM_IA_RULE_REGIONV1_S;

/* ���������� */
typedef union
{
    XM_IA_RULE_LINE_S stRuleLine;               /* ���߹��� */
    XM_IA_RULE_REGION_S stRuleRegion;           /* �������� */
    XM_IA_RULE_REGIONV1_S stRuleRegionV1;     /* �½����������ӽ����˵���Ŀ��16 */
    unsigned int uiReserved[16];                /* �������� */
}XM_IA_PD_RULE_U;

/* ���й���ṹ�� */
typedef struct
{
    unsigned char ucRuleEnable;         /* �����Ƿ�ʹ��:0��ʹ��/1ʹ�� */

    XM_IA_RULE_E eRuleType;             /* �������� */
    XM_IA_PD_RULE_U unPdRule;           /* ������� */
    XM_IA_ALARM_POINT_E eAlarmType;     /* ����������ѡ�� */

    unsigned int uiReserved[15];        /* ��������:ʹ�� */
}XM_IA_PD_RULES_S;

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */

#endif
