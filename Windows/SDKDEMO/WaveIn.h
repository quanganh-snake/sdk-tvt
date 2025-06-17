
#include "DDPublic.h"
#include <mmreg.h>

#define  WAVEIN_FMT_FORMATTAG_DEF      WAVE_FORMAT_PCM

const unsigned int WAVEIN_BUFF_LEN_DEF = 3200;
const unsigned int WAVEIN_BUFF_NUM_DEF = 4;

enum WAVE_IN_STATE
{
	ERRNO_WAVEIN_OK = 0x01,             
	ERRNO_WAVEIN_NO_MEM,          
	ERRNO_WAVEIN_OPEN,           
	ERRNO_WAVEIN_NOTOPEN,         
	ERRNO_WAVEIN_PREPARE,       
	ERRNO_WAVEIN_UNPREPARE,      
	ERRNO_WAVEIN_RESET,            
	ERRNO_WAVEIN_CLOSE,            
	ERRNO_WAVEIN_ALREADYOPEN,    
	ERRNO_WAVEIN_ADDBUFFER,
	ERRNO_WAVEIN_START,
	ERRNO_WAVEIN_STOP,             
	ERRNO_WAVEIN_THREAD_NOT_END
};

//��ʼ����Ƶ�ɼ��豸��ʹ�õĽṹ��WaveFormatEx��ȱʡֵ
const unsigned int WAVEIN_FMT_AVGBYTEPERSEC_DEF = 16000;
const unsigned int WAVEIN_FMT_BLOCKALIGN_DEF = 2;
const unsigned int WAVEIN_FMT_CHANNELS_DEF = 1;
const unsigned int WAVEIN_FMT_SAMPLESPERSEC_DEF = 8000;
const unsigned int WAVEIN_FMT_BITSPERSAMPLE_DEF = 16;

typedef void (*WAVEIN_DATA_CB_FUN)(unsigned char *pAudioData, unsigned int dataLen, void *pUser);

class CWaveIn
{
public:
	CWaveIn(void);
	~CWaveIn(void);

public:
	BOOL CheckAudioDevice();

	LONG Start(WAVEFORMATEX wavf, WAVEIN_DATA_CB_FUN fWaveInDataCBFun, void *pUser);
	LONG Stop();

	//���òɼ���������С
	void SetBuffSize(DWORD nBuffSize);
	//���òɼ�����������(����Start֮ǰ)
	void SetBuffNum (DWORD nBuffNum );

private:
	static PUB_THREAD_RESULT PUB_THREAD_CALL WaveInThread(LPVOID lpParameter);

	BOOL SelectMic();
private:
	DWORD				m_nBuffSize;
	DWORD				m_nBuffNum;

	//�������ݻص�
	WAVEIN_DATA_CB_FUN	m_pTalkDataCallBack;
	void *				m_pTalkDataCBParam;
	PWAVEHDR			m_pWaveHeader;
	HWAVEIN				m_hWaveIn;
//	WAVEFORMATEX		WaveFormat;

	PUB_thread_t		m_ThreadHandle;	////��ȡMic��Ƶ���ݵ��߳̾��
	DWORD				m_ThreadID;		///////�̵߳�ID
	bool				m_bStartRec;	//////�̵߳����б��
};


