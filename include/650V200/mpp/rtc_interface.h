

#ifndef _RTC_INTERFACE_H_
#define _RTC_INTERFACE_H_

#ifdef __cplusplus
extern "C"
{
#endif

//���������������ýṹ��
typedef struct
{
	short UserMode;                       // �û�ģʽ 0���Զ�ģʽ 1���û�ģʽ
	short CngMode;                        // ��������ģʽ  0���ر� 1������
	short NearSilenceEnergyMode;          //�����ź�ǿ�������������ֵ
	short EchoFreq;                       //��������Ƶ��1
	short MixedFreq;                      //��������Ƶ��2
	short ERLEBand[6];                    //ERLE(�ز��������)����Ƶ���������
	short ERLE[7];                        //ERLE(�����������)Ƶ�α���ֵ���飬��ϲ���ERLBand[6]����ʹ��
	short SpeechProtectFreqLow;           // ���˵�Ƶ��������Ƶ���Ƶ����
	short SpeechProtectFreqHigh;          // ���˵�Ƶ��������Ƶ���Ƶ����
	short LatencyEstimation;              // �ӳٹ���ģʽ
	short EchoMode;                       //��������ģʽ
	short msInSndCardBuf;                 //�����ӳ�
} AEC_CONFIG_X;   

 // ��������������ýṹ��
typedef struct
{
	short UserMode;                     // �û�ģʽ 0:�Զ�ģʽ 1:�û�ģʽ
	short DenoiseLevel;                 // ����̶����ò���
	short NoiseGate;                    // �����������ò���
}ANS_CONFIG_X;
 
// �����Զ�����ģʽ
typedef struct
{
	short UserMode;                    // �û�ģʽ 0���Զ�ģʽ 1���û�ģʽ 
	short AgcMode;                     // ����ģʽ
	short TargetLevelDb;               // Ŀ���ƽ
	short CompressionGaindB;           // �������
    short LimiterEnable;               // ѹ��������
	short HighPassFilter;              // ��ͨ�˲���

} AGC_CONFIG_X; 

// ������������  farend��Զ���ź�  nearend�������ź� output������ź�
int AEC_Create(void** aecInst);                                             // ����
int AEC_Init(void* aecInst);                                                // ��ʼ��
int AEC_set_config(void * aecInst, AEC_CONFIG_X config);                    // ��������
int AEC_WORK(void* aecInst, short* farend, short* nearend, short* output);  // ����  
int AEC_Free(void* aecInst);                                                // �ͷ�
 
// �������뺯�� indata�������ź�  outsign������ź�
int ANS_Create(void **ppvHandle);                              // ����
int ANS_Init(void *ppvHandle);                                 // ��ʼ��
int ANS_set_config(void * ppvHandle, ANS_CONFIG_X config);     // ��������
int ANS_WORK(short* indata, short* outsign, void *ppvHandle);  // ����
int ANS_Free(void *ansInst);                                   // �ͷ�
 
 
// �������溯��  inNear: �����ź�   out������ź�
 int AGC_Create(void **agcInst);                            // ����
 int AGC_Init(void *agcInst);                               // ��ʼ��
 int AGC_set_config(void* agcInst, AGC_CONFIG_X config);    // ��������
 int AGC_WORK(void* agcInst, const short* inNear,
                      const short* inNear_H,
                      short samples,
                      short* out,
                      short* out_H,
                      int inMicLevel,
                      int* outMicLevel,
                      short echo,
                      unsigned char* saturationWarning);    // ����
 int AGC_Free(void *agcInst);                               // �ͷ�
 
  // ��ӡ�汾��
 void librtc_version_info(char *info, int size);
 
#ifdef  __cplusplus
}
#endif  /* end of __cplusplus */
#endif /* _IA_GMM_INTERFACE_H_ */
