/*****************************************************************************

------------------------------------------------------------------------------
                            xm_sc_interface.h
��Ʒ��:    ��������㷨
ģ����:    mrcg
��������:  2019-10-14
����:      �����
�ļ�����:  ��������㷨����ͷ�ļ�
�汾�ţ�   V0.1.1.0
*****************************************************************************/
#ifndef _XM_VAD_INTERFACE_H_
#define _XM_VAD_INTERFACE_H_

#include "xm_ia_comm.h"


#ifdef __cplusplus
extern "C" {
#endif

#define VAD_MAJOR_VERSION               (0)             /* ��������㷨���汾�� */
#define VAD_MINOR_VERSION               (1)             /* ��������㷨�ΰ汾�� */
#define VAD_REVERSION                   (1)             /* ��������㷨�޸İ汾�� */
#define VAD_BUILD                       (0)             /* ��������㷨����汾��(�ݲ���) */

#define  RECORD                     (0)
#define  XM_VOICE_GATHER_NUM		(16)

typedef struct{
	unsigned int  u32PhyAddr;
	unsigned char *pu8VirAddr;
	unsigned int  u32Size;
}XM_IA_VOICE_MEM_S;

typedef struct{
	int iChannel;	//��·��Ƶ·�����
	XM_IA_VOICE_MEM_S stVoiceMem;	//������Ƶ
}XM_IA_VOICE_S;

typedef struct{
	int iMaxChannel;	//��·��·��
	float fSampleRate;	//8000	
}XM_VAD_CFG_S;

typedef  struct{
	int iChannel;	//��·��Ƶ·�����
	unsigned char ucResult[XM_VOICE_GATHER_NUM];	//���ؽ��
}XM_IA_VAD_RSLT_S;

typedef struct
{
	int iRoiLen;				/* 16,ÿ��ȡ�ô�������ÿ������10ms */
	int iNfft;					/* fft���ȣ�֧��Ϊ1024��512 */
	int iBufferSize;			/* ��������iRoiLen*160��10msһ�����ڣ�160Ϊһ�����ڵ��������� */
	int gL;						/* fftfilter�������趨Ϊ512 */
	int iFeatureDim;			/* mrcg���ͨ���� */
	int W;						/* 19 */
	int u;						/* 9 */
	int winLen1;				/* ����ͼ1���ڴ�С��0.025*fFs (fFsȡ��VAD_CONFIG_S��*/
	int winLen2;				/* ����ͼ2���ڴ�С��0.2*fFs��fFsȡ��VAD_CONFIG_S��*/
	int winShift;				/* 0.01* fFs��fFsȡ��VAD_CONFIG_S��*/
	int previous1;				/* winLen1 - winShift;����ͼ1֡��λ����ص��� */
	int previous2;				/* winLen2 - winShift;����ͼ2֡��λ����ص��� */
	int iRoiSize;				/* ���������λ��ƫ����������������*/
	int iCochleaOffset;			/* �ƺ�ͬiRoiLen */
	int iBitPadCocLen1;			/* ����ͼ1�������ڣ�ǰ�����߳��ȣ���֡11*XM_MRCG_CHAN_NUM���ڶ�֡ 43*XM_MRCG_CHAN_NUM */
	int iBitPadCocLen2;			/* ����ͼ2�������ڣ�ǰ�����߳��ȣ���֡0���ڶ�֡ 34*XM_MRCG_CHAN_NUM */
	int iCochOffLen;			/* �������ڼ���洢�ռ����ݳ��� */


	int fps;					/* ��Ƶ������ */
	int iNumChan;				/* fftͨ���� */
	XM_IA_IMG_TYPE_E eImgType;  /* ����ͼ���ʽ */

}XM_IA_VAD_INIT_S;

typedef struct
{
	unsigned char ucIsFirstFrm;     /* �Ƿ��ǵ�һ֡��1�ǵ�һ֡��0�Ǻ���֡ */
	int size;
	int fps;						/* ������ */
	short *pstImage[XM_VOICE_GATHER_NUM];				/* �������ݽṹ�� */
}XM_IA_VAD_IN_S;

typedef struct
{
	int pstResult[XM_VOICE_GATHER_NUM];		/* �������0��������1������ */
}XM_IA_VAD_RESULTS_S;







#ifdef __cplusplus
}
#endif

#endif