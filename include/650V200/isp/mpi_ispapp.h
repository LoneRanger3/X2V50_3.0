#ifndef __MPI_ISPAPP_H__
#define __MPI_ISPAPP_H__

#define CHN_NUM_MAX	(3)

typedef struct stOSD_INFO
{
	int s32AwbDiff;
	int s32ExpAll;
}OSD_INFO;
typedef struct stISPAPP_WIN_ATTR_S
{
/*********************************************
u8Mode:
	0: Real Data(ʵ��ֵ)
	1: ���ڱ�׼ֵ����ƫ��
	2: Refresh
	
u8Mirror:  �����
u8Flip: ��ת��

u16ValH: ˮƽ����ֵ(bit15:Ϊ����)    [0, 0xFFFE]
	0~0x7FFF			:
	0x8000 ~ 0xFFFE	:  <0

u16ValV: ��ֱ����ֵ(bit15:Ϊ����)	  [0, 0xFFFE]
	0~0x7FFF			:
	0x8000 ~ 0xFFFE	:  <0

note:  
	u16ValH/u16ValV = 0xFFFF ʱ��׼��д��
*********************************************/
	unsigned char u8Mode;
	//unsigned char u8Mirror;
	//unsigned char u8Flip;
	unsigned short u16ValH;
	unsigned short u16ValV;
	unsigned int au32Rsv[4];
} ISPAPP_WIN_ATTR_S;

/*************************************************************************************************************************
ISPAPP_TASK_CFG
����˵��:
	1.u32ProductType:		��Ʒ�ͺ�;�����ڲ����ֲ�Ʒ/����������Ĭ��Ϊ0
	2.u32DspType:			DSP�ͺ�;��������DSP�ͺţ�����Ĭ��Ϊ0
	3.u32StdType:			��Ƶ��ʽPAL/NTSC
								1: PAL
								2: NTSC
	4.u32RsltType:			isp����ֱ���(����ֵ�ο�ö��RESL_TYPE) --- ��������Ҫƥ�����(�����֧��)��Ĭ�Ͻ�������0
							bit[0,7]:sensor0
							bit[8,15]:senesor1
							������Χ:
								0, --- ������ڲ���
								P1080_ = 1,
								P960_ = 2,	
								P1536_ = 3,    
								P4M_ = 4,    
								P5M_ = 5,    
								P4K_ = 6,  
								P5M_2 = 7,		// 16:9  (2880*1620)
								P3M_2 = 8,		// 16:9  (2304*1296)							
	5.u32RsltTypeSns 		sensor�ֱ���--- ��������Ҫƥ�����(�����֧��)��Ĭ�Ͻ�������0
							ͬu32RsltType
	6.au32SoftLed[2]:			���������ص���أ�����Ĭ��Ϊ0 (au32SoftLed[0]:ͨ��0  ; au32SoftLed[1]:ͨ��1)
		bit31:
			0	not use
			1	use
		bit23:	���ư׹�Ƶĵ�ƽ
		bit16~bit22:	���ư׹�Ƶ�GPIO	(0x00: choice by source,100:��ʾGPIO00)
		bit15: 	���ƺ���Ƶĵ�ƽ
		bit8~bit14:	���ƺ���Ƶ�GPIO (0x00: choice by source,100:��ʾGPIO00)
		bit4:
			0	�����
			1	Ӳ����
		bit0:
			0  	��ͨģʽ������ͬ��/�Զ�ͬ��... (Ӳ�����ƺ����)��
			1	������Ƶ�	
	7.au32RsltCh:			������δ֧�֣�����Ĭ��Ϊ0
	8.u32Infrared:		Ӳ�����ư��ź���أ�����Ĭ��Ϊ0
		bit31:
			0	not use		---�ӿڲ����ã�����ڲ���
			1	use			---�ýӿ�������Ч
		bit16~bit23: InfraredGPIO_chn1 (0x00: choice by source,100:��ʾGPIO00)	   �ܽ����֣�ͨ��1��
		bit8~bit15: InfraredGPIO_chn0 (0x00: choice by source,100:��ʾGPIO00)	   �ܽ����֣�ͨ��0��
		bit1: InfraredSwap_chn1		�ɼ����Է���ͨ��1��
		bit0: InfraredSwap_chn0		�ɼ����Է���ͨ��0��
	9.au32IrCut[2]:      IRCUT������ã�����Ĭ��Ϊ0
		au32IrCut[0]:  ͨ��0
		au32IrCut[1]:  ͨ��1
	      bit31:
	        0  not use    ---�ӿڲ����ã�����ڲ���
	        1  use      ---�ýӿ�������Ч
	      bit30:  IRCUT Swap
	      bit8~bit15: IrcutGPIO_B (0x00: choice by source,100:��ʾGPIO00)���ܽ�B��
	      bit0~bit7: IrcutGPIO_A (0x00: choice by source,100:��ʾGPIO00)���ܽ�A��  	
	10.u8IrCutTestIO:	����ʱ��������IRC�����Ƿ���õ�����IO֮һ(����һ��IO�̶�ΪADC)
		bit8~bit15:	u8IrCutTestIO_chn1 (0x00: choice by source,100:��ʾGPIO00)��ͨ��1��
		bit0~bit7: 	u8IrCutTestIO_chn0 (0x00: choice by source,100:��ʾGPIO00)��ͨ��0��
	11.u32CmosPort:		���˫��Sensor1�Ľӿ�(�����֧��)��Ĭ�Ͻ�������0
		0: ����ڲ���
		1: ǿ��ΪMIPI
		2: ǿ��ΪDVP
	12:u32Fps:			ǿ��ָ��PAL/NTSC�µ�֡��(�����֧��)   
		bit0~bit7:	sensor0_PAL		
		bit8~bit15: sensor0_NTSC
		bit16~bit23: sensor1_PAL
		bit24~bit31: sensor1_NTSC
		������Χ: 
			0: ������ڲ���
			12/15/25/30
	13.u32FunExt:		���ܱ��
		bit0~bit7:
			0: Ĭ��(ͨ��IPC  )
			1: ˫��
			0x11: ˫Ŀ������佹����isp������Ҫͬ��
			0x13: ˫Ŀǹ��
			0x20: RAW+YUV_DVP
			0x40: ץ����
			0x50: ֱ����
			0x60: IPC+AHD(AHDʱ��������������)
		bit8~bit15: 	HDR���
			2:HDR(line-by-line)
		bit16~bit19:    LDC���
			bit16: ͨ��0 LDCʹ�ܿ���
			bit17: ͨ��1 LDCʹ�ܿ���
			bit18: ͨ��2 LDCʹ�ܿ���
	14.u32SnsResetIO:
		bit31:
			0	not use		---�ӿڲ����ã�����ڲ���
			1	use			---�ýӿ�������Ч
		bit30:		��λ�ܽ�1��ƽ
			0:  �͵�ƽ��λ���ߵ�ƽ�ͷ�
			1:  �ߵ�ƽ��λ���͵�ƽ�ͷ�
		bit29:		��λ�ܽ�2��ƽ
			0:  �͵�ƽ��λ���ߵ�ƽ�ͷ�
			1:  �ߵ�ƽ��λ���͵�ƽ�ͷ�
		bit8~bit15: ��λ�ܽ�2
			255: Ӳ��RC��·��λ
			166: GPIO166��λ
		bit0~bit7:  ��λ�ܽ�1
			255: Ӳ��RC��·��λ
			166: GPIO166��λ
	15.u32Flag
		bit0: 
			0: Normal(��׼�汾)
			1: MemorySaver Mode(����ģʽʡ�ڴ�汾)
	16.u32MultiSensor:     
			bit8~bit11: sensor����(<=2ʱ��Ӧ2)
			bit0~bit7:��ͨ��ͨ����
						0:mipi
						1:dvp	   		
*************************************************************************************************************************/
//��ϸ��ע����
typedef struct stISPAPP_TASK_CFG
{
	unsigned int u32ProductType;
	unsigned int u32DspType;			// 0:Rsv
	unsigned int u32StdType;			// 0:Rsv 1:PAL 2:NTSC
	unsigned int u32RsltType;			// 0:Rssv  1:1080P	2:960P 3:1536P ...  bit[0,7]:sensor0   bit[8,15]:senesor1
	unsigned int u32RsltTypeSns;			// 0:Rssv  1:1080P	2:960P 3:1536P ...  bit[0,7]:sensor0   bit[8,15]:senesor1
	unsigned int au32SoftLed[CHN_NUM_MAX];
	unsigned int au32RsltCh[4][4];	// ����
	unsigned int u32Infrared;
	unsigned int au32IrCut[CHN_NUM_MAX];
	unsigned int u32IrCutTestIO;			//��������IRC�����Ƿ���õ�����IO֮һ(����һ��IO�̶�ΪADC)
	unsigned int u32CmosPort;			// 0:Rsv  1:MIPI  2:DVP
	unsigned int u32Fps;				// 0:Rsv 
	unsigned int u32FunExt;			// 0:Rsv
	unsigned int u32SnsResetIO;
	unsigned int u32Flag;			// ������Ա��	
	unsigned int u32SensorType;		// ָ��sensor���ͣ�0����ָ�� ��bit0-15:sensor1��bit 16-31sensor2
	unsigned int u32MultiSensor;        //��ͨ�� 0:mipi   1:dvp	
	unsigned int u32Rsv;				// ����
}ISPAPP_TASK_CFG;  

typedef struct stISPAPP_INFO
{
	unsigned char u8FunExt;
	unsigned char u8LimitedUp;
	unsigned char u8ChnMask;
	unsigned char u8ChnMain;			//��ͨ�� 0:mipi   1:dvp
	unsigned char au8Rsv[4];
	unsigned int u32ProductType;
	unsigned int u32DspType;			// 0:Rsv
	unsigned int u32SnsNum;
	unsigned int au32SnsStdType[3];		// 0:Rsv 1:PAL 2:NTSC
	unsigned int au32SnsRsltType[3];	// 0:720P  1:1080P	2:960P 3:3M 4:4M 5:5M
	unsigned int au32SnsID[3];	
	unsigned int au32Rsv[6];			// ����
}ISPAPP_INFO;


int XM_MPI_ISPAPP_Run(ISPAPP_TASK_CFG stTaskCfg);

int XM_MPI_ISPAPP_Exit(unsigned int u32Rsv);

/*************************************************************************
��������:	����VI�ü�����(H��V)  ---��Ծ���ת��
�������:		IspDev:  isp�豸��
			pstWinAttr
�������: ��
���ز���: 0: �ɹ�    		-1: ʧ��
note:  
	u16ValH/u16ValV = 0xFFFF ʱ��׼��д��
*************************************************************************/
int XM_MPI_ISPAPP_WinSet(int IspDev, ISPAPP_WIN_ATTR_S *pstWinAttr);


/*************************************************************************
��������:	����/ˢ��PLL�������(���WDR�л�ģʽ)
�������:	ispDev
			ͨ����: 0
          u32WdrSnsMode: WDR sensor��ǰ��ģʽ
			0:Linear	2:HDR(line-by-line)
�������: ��
���ز���: 0: �ɹ� 
*************************************************************************/
int XM_MPI_ISPAPP_PllSet(unsigned int ispDev, unsigned int u32WdrSnsMode);


/*************************************************************************
��������:	��ȡlibispapp.a�е������Ϣ
�������:	��
�������: pstInfo: �����Ϣ
���ز���: 0: �ɹ� 
*************************************************************************/
int XM_MPI_ISPAPP_InfoGet(ISPAPP_INFO *pstInfo);

#endif

