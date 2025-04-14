/*****************************************************************************

------------------------------------------------------------------------------
        xm_fr_interface.h
��Ʒ��:    ����ʶ��
ģ����:    ����ʶ��
��������:  2018-09-10
����:      ������
�ļ�����:  ����ʶ�����ͷ�ļ�
�汾�ţ�   V0.1.1
*****************************************************************************/

#ifndef _XM_FR_INTERFACE_H_
#define _XM_FR_INTERFACE_H_
/*--------------------------------------------------------------------------------
    1. include headers
--------------------------------------------------------------------------------*/
#ifdef __cplusplus
extern "C"
{
#endif
#include "xm_ia_comm.h"

/*--------------------------------------------------------------------------------
    2. constant defines
--------------------------------------------------------------------------------*/
#define FR_INPUT_WIDTH                  (112)       /* FR ����Input����������ͼ���� */
#define FR_INPUT_HEIGHT                 (112)       /* FR ����Input����������ͼ��߶� */
#define FR_INPUT_WIDTH_V2               (80)        /* FR ��������2��Input����������ͼ���� */
#define FR_INPUT_HEIGHT_V2              (80)        /* FR ��������2��Input����������ͼ��߶� */
#define FACE_REC_FEATURE_NUM            (512)       /* ����ʶ����������� */

/*--------------------------------------------------------------------------------
    3. data struct
--------------------------------------------------------------------------------*/

/* ����ʶ���㷨ģ�� */
typedef enum
{
    CLASS_ANGELA = 0,                       /* 40MB����ʶ������ */
    CLASS_BELLA = 1,                        /* 20MB����ʶ������ */
    CLASS_CAROL = 2,                        /* 25MB����ʶ������ */
    CLASS_DAISY = 3,                        /* 20MB����ʶ������ */
    CLASS_EARTHA = 4,                       /* 23MB����ʶ������ */
    CLASS_FAITHE = 5,                       /* 44MB����ʶ������ */
    CLASS_GABRIELLE = 6,                    /* 44MB����ʶ������ */
    CLASS_HANNAH = 7,                       /* 165MB����ʶ������ */
    CLASS_IDA = 8,                          /* 41MB 80x80����ʶ������ */
    CLASS_JACQUELINE = 9,                   /* 44MB 112x112����ʶ������,����ʶ���������� */
    CLASS_KAREN = 10,                       /* 41MB 80x80����ʶ������,����ʶ���������� */
    CLASS_LAURA = 11,                       /* 165MB 112x112����ʶ������,����ʶ���������� */
    CLASS_MADELINE = 12,                    /* 165MB 112x112����ʶ������,����ʶ����������,��λ����ѵ�� */
    CLASS_NANCY = 13,                       /* 165MB 112x112����ʶ������,���ʶ���Ż� */
    CLASS_OCTAVIA = 14,                     /* 165MB 112x42����ʶ������,����������ʶ��(����) */
    CLASS_PAG = 15                          /* 165MB 112x112����ʶ������,ʶ���Ż� */
    
}XM_IA_FR_CLASS_E;

/* ����ʶ���ʼ������ */
typedef struct
{
    /* INPUT */
    char acClassBinPath[XM_IA_PATH_MAX_LEN];    /* ������Bin�ļ�·�� */
    XM_IA_FR_CLASS_E eFrClass;                  /* ����ʶ������� */
    XM_IA_INFERENCE_ENGINE_E eInferenceEngine;  /* �������� */

    unsigned char ucReserved[64];               /* �����ֽ� */

}XM_IA_FR_INIT_S;

/* ��������,�̶��ṹ�� */
typedef struct
{
    float afFaceFeat[FACE_REC_FEATURE_NUM];     /* 512ά�������� */

}XM_IA_FR_FEATRUES;

/* ���������б� */
typedef struct
{
    int iFaceNumInList;                         /* ������������������ */
    XM_IA_FR_FEATRUES **ppstFeatureItem;        /* ���������б�ָ�� */

    unsigned char ucReserved[128];              /* �����ֽ� */
}XM_IA_FR_FEATRUES_LIST;

/* ���������ȶԽ�� */
typedef struct
{
    int iFaceRecIdx;                            /* ʶ�����������б����������� */
    float fMatchingRate;                        /* �����(0~100%) */

    unsigned char ucReserved[64];               /* �����ֽ� */
}XM_IA_FR_MATCH_RSLT_S;

/* �����ȶ����� */
typedef struct
{
    int iMaxMatchNum;                           /* �������ȶ��� */
    float fThresh;                              /* �����ȶ���ֵ */

    unsigned char ucReserved[64];               /* �����ֽ� */
}XM_IA_FR_MATCH_CFG_S;

/* �������б���ȶԳ����д����趨��ֵ�ıȶԽ�� */
typedef struct
{
    int iMatchNum;                              /* �ȶԳ��������� */
    XM_IA_FR_MATCH_RSLT_S *pstMatchList;        /* �ȶԽ���б� */

    unsigned char ucReserved[64];               /* �����ֽ� */
}XM_IA_MATCH_LISTS_RSLT_S;

/// ���������ȶ�,���1-2���������������
/// \param [in] pfFaceFeat      ��������
/// \param [in] pstFeatureList  ���������б�
/// \param [in] fThresh         �ȶ���ֵ(0~100%),������ֵ�����ƥ��ɹ�
/// \param [out] pstMatchRslt   ����ƥ����
/// \return 0  �ɹ�
/// \return !0 ʧ��
int XM_IA_FR_MatchFace(XM_IA_FR_FEATRUES *pstFaceFeat, XM_IA_FR_FEATRUES_LIST *pstFeatureList, float fThresh, XM_IA_FR_MATCH_RSLT_S *pstMatchRslt);

/// ���������ȶ�,���������ֵ���������
/// \param [in] pfFaceFeat      ��������
/// \param [in] pstFeatureList  ���������б�
/// \param [in] stMatchCfg      �����ȶ�������
/// \param [out] pstMatchRslt   ����ƥ����
/// \return 0  �ɹ�
/// \return !0 ʧ��
int XM_IA_FR_ListOfMatchFace(XM_IA_FR_FEATRUES *pstFaceFeat, XM_IA_FR_FEATRUES_LIST *pstFeatureList, XM_IA_FR_MATCH_CFG_S stMatchCfg, XM_IA_MATCH_LISTS_RSLT_S *pstMatchRslt);

/*************************************************
Author: WangZelang
Date: 2021-04-20
Description: �ṩ��׼������������ٱȶ�
INPUT:  pfFaceFeat      ��ʶ����������
        fNorm1          ��ʶ������������׼��
        pstFeatureList  ���������׿��е���������
        pfNormList      �׿�����������׼��
        fThresh         �ȶ���ֵ(0~100%),������ֵ�����ƥ��ɹ�,Ĭ��ʶ����ֵ0.50
OUTPUT: pstMatchRslt    �ȶԽ�����������������������ƶ�
*************************************************/
int XM_IA_FR_FastMatchFace(XM_IA_FR_FEATRUES *pfFaceFeat, float fNorm1, XM_IA_FR_FEATRUES_LIST *pstFeatureList, float *pfNormList, float fThresh, XM_IA_FR_MATCH_RSLT_S *pstMatchRslt);

#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */
#endif