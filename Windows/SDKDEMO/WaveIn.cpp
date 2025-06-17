
#include "stdafx.h"
#include "WaveIn.h"
#include "DDPublic.h"
CWaveIn::CWaveIn(void)
{
	m_nBuffSize = WAVEIN_BUFF_LEN_DEF;
	m_nBuffNum  = WAVEIN_BUFF_NUM_DEF;
	m_ThreadHandle = PUB_THREAD_ID_NOINIT;

	m_pTalkDataCallBack = NULL;
	m_pTalkDataCBParam = NULL;
	m_pWaveHeader = NULL;
	m_hWaveIn = NULL;
}

CWaveIn::~CWaveIn(void)
{
	
}


LONG CWaveIn::Start(WAVEFORMATEX wavf, WAVEIN_DATA_CB_FUN fWaveInDataCBFun, void *pUser)
{
	if(m_ThreadHandle != PUB_THREAD_ID_NOINIT)
	{
		//PUB_ExitThread(&m_ThreadHandle, &m_bStartRec);
		WaitForSingleObject(m_ThreadHandle, INFINITE);
		CloseHandle(m_ThreadHandle);


	m_ThreadHandle = PUB_THREAD_ID_NOINIT;
	}

	m_bStartRec = true;
	m_ThreadHandle = CreateThread(NULL, 0, WaveInThread, this, 0, &m_ThreadID);
	if(PUB_CREATE_THREAD_FAIL == m_ThreadHandle)
	{
		printf("Create Thread Failed!");
		return ERRNO_WAVEIN_NO_MEM;
	}

	m_pTalkDataCallBack = fWaveInDataCBFun;
	m_pTalkDataCBParam = pUser;

	MMRESULT mmRet = MMSYSERR_NOERROR;
	if (MMSYSERR_NOERROR != (mmRet = waveInOpen(&m_hWaveIn, WAVE_MAPPER, &wavf, m_ThreadID, (DWORD)this, CALLBACK_THREAD)))
	{
		return ERRNO_WAVEIN_OPEN;
	}

	assert(m_hWaveIn != NULL);
	
	m_pWaveHeader = new WAVEHDR[m_nBuffNum];

	assert(m_pWaveHeader != NULL);
	if (NULL == m_pWaveHeader)
	{
		waveInClose(m_hWaveIn);
		m_hWaveIn = NULL;

		return ERRNO_WAVEIN_NO_MEM;
	}

	DWORD i;
	for(i = 0; i < m_nBuffNum; i++)
	{
		m_pWaveHeader[i].lpData = new char[m_nBuffSize];
		assert(m_pWaveHeader[i].lpData);
		if (NULL == m_pWaveHeader[i].lpData)
		{
			waveInClose(m_hWaveIn);
			m_hWaveIn = NULL;

			return ERRNO_WAVEIN_NO_MEM;
		}

		m_pWaveHeader[i].dwBufferLength  = m_nBuffSize;
		m_pWaveHeader[i].dwBytesRecorded = 0;
		m_pWaveHeader[i].dwLoops         = 1;
		m_pWaveHeader[i].dwUser          = 0;
		m_pWaveHeader[i].dwFlags         = 0;
		m_pWaveHeader[i].lpNext = NULL;
		m_pWaveHeader[i].reserved = 0;

		mmRet = waveInPrepareHeader(m_hWaveIn, &m_pWaveHeader[i], sizeof(WAVEHDR));
		if (MMSYSERR_NOERROR != mmRet)
		{
			waveInClose(m_hWaveIn);
			m_hWaveIn = NULL;

			return ERRNO_WAVEIN_PREPARE;
		}

		//初始化音频采集设备后，必须调用waveInAddBuffer
		mmRet = waveInAddBuffer(m_hWaveIn, &m_pWaveHeader[i], sizeof(WAVEHDR));
		if (MMSYSERR_NOERROR != mmRet)
		{
			waveInClose(m_hWaveIn);
			m_hWaveIn = NULL;

			return ERRNO_WAVEIN_ADDBUFFER;
		}
	}

	//开始采集
	mmRet = waveInStart(m_hWaveIn);
	if (MMSYSERR_NOERROR != mmRet)
	{
		waveInClose(m_hWaveIn);
		m_hWaveIn = NULL;

		return ERRNO_WAVEIN_START;
	}

	return ERRNO_WAVEIN_OK;
}	


LONG CWaveIn::Stop()
{
	m_bStartRec = FALSE;
	if(PUB_THREAD_ID_NOINIT != m_ThreadHandle)
	{
		//PUB_ExitThread(&m_ThreadHandle, &m_bStartRec);
		WaitForSingleObject(m_ThreadHandle, INFINITE);
		CloseHandle(m_ThreadHandle);


		m_ThreadHandle = PUB_THREAD_ID_NOINIT;
	}
	
	if(NULL != m_hWaveIn)
	{
		waveInReset(m_hWaveIn);
		waveInStop(m_hWaveIn);

		for (DWORD i = 0; i < m_nBuffNum; i++)
		{
			waveInUnprepareHeader(m_hWaveIn, &m_pWaveHeader[i], sizeof(WAVEHDR));
		}

		waveInClose(m_hWaveIn);
		m_hWaveIn = NULL;
	}


	if(m_pWaveHeader != NULL)
	{
		for(DWORD i = 0; i < m_nBuffNum; i++)
		{
			if(NULL != m_pWaveHeader[i].lpData)
			{
				delete [] m_pWaveHeader[i].lpData;
			}
		}
		delete [] m_pWaveHeader;
		m_pWaveHeader = NULL;
	}

	return ERRNO_WAVEIN_OK;
}

PUB_THREAD_RESULT PUB_THREAD_CALL CWaveIn::WaveInThread(LPVOID lpParameter)
{
	CWaveIn *pThis = (CWaveIn *)lpParameter;

	int errorNo = 0;
	PWAVEHDR pWaveHdr = NULL;
	while(pThis->m_bStartRec)
	{
		MSG Msg;
		if(::PeekMessage(&Msg, NULL, 0, 0, PM_REMOVE))
		{
			switch(Msg.message)
			{
			case MM_WIM_CLOSE:
				{
					break;
				}
			case MM_WIM_DATA:
				{
					pWaveHdr = (PWAVEHDR)Msg.lParam;

					if(NULL != pThis->m_pTalkDataCallBack)
					{
						pThis->m_pTalkDataCallBack((BYTE*)pWaveHdr->lpData, pWaveHdr->dwBytesRecorded, pThis->m_pTalkDataCBParam);
					}

					//如果不停地waveInAddBuffer将会导致waveInProc无法结束
					//注意：waveInProc是创建在一个新线程里	
					MMRESULT mmRet = waveInUnprepareHeader(pThis->m_hWaveIn, pWaveHdr, sizeof(WAVEHDR));
					if (MMSYSERR_NOERROR != mmRet)
					{
						if(MMSYSERR_NOERROR != waveInUnprepareHeader(pThis->m_hWaveIn, pWaveHdr, sizeof(WAVEHDR)))
						{
							Sleep(200);
							continue;
						}
						if(MMSYSERR_NOERROR != waveInPrepareHeader(pThis->m_hWaveIn, pWaveHdr, sizeof(WAVEHDR)))
						{
							Sleep(200);
							continue;
						}
						if(MMSYSERR_NOERROR != waveInAddBuffer(pThis->m_hWaveIn, pWaveHdr, sizeof(WAVEHDR)))
						{
							Sleep(200);
							continue;
						}
						
						break;
					}

					mmRet = waveInPrepareHeader(pThis->m_hWaveIn, pWaveHdr, sizeof(WAVEHDR));
					assert(MMSYSERR_NOERROR == mmRet);
					if (MMSYSERR_NOERROR != mmRet)
					{
						if(MMSYSERR_NOERROR != waveInPrepareHeader(pThis->m_hWaveIn, pWaveHdr, sizeof(WAVEHDR)))
						{
							Sleep(200);
							continue;
						}
						if(MMSYSERR_NOERROR != waveInAddBuffer(pThis->m_hWaveIn, pWaveHdr, sizeof(WAVEHDR)))
						{
							Sleep(200);
							continue;
						}
						
						break;
					}

					//waveInAddBuffer使得设备可以不停地采集
					mmRet = waveInAddBuffer(pThis->m_hWaveIn, pWaveHdr, sizeof(WAVEHDR));
					if (MMSYSERR_NOERROR != mmRet)
					{
						if(MMSYSERR_NOERROR != waveInAddBuffer(pThis->m_hWaveIn, pWaveHdr, sizeof(WAVEHDR)))
						{
							Sleep(200);
							continue;
						}
					
						break;
					}
				}
				break;
			case MM_WIM_OPEN:
				{
					break;
				}
			
			default:
				break;
			}
		}
		else
		{
			Sleep(200);
		}
	}

	return 0L;
}

void  CWaveIn::SetBuffSize(DWORD nBuffSize)
{
	m_nBuffSize = nBuffSize;
}

void  CWaveIn::SetBuffNum (DWORD nBuffNum )
{
	m_nBuffNum = nBuffNum;
}

BOOL CWaveIn::SelectMic()
{
	UINT mixernum = mixerGetNumDevs();
	HMIXER hmx = NULL;
	MIXERLINE mxl;
	DWORD hr;

	for(DWORD i = 0; i < mixernum; i++)
	{
		hr = mixerOpen(&hmx, i, 0, 0, 0);
		if(hr != MMSYSERR_NOERROR)
		{
			return FALSE;
		}
		mxl.cbStruct		= sizeof(mxl);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

		//Find a LIST control, if any, for the wave in line
		hr = mixerGetLineInfo((HMIXEROBJ)hmx, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
		if(hr != MMSYSERR_NOERROR)
		{
			mixerClose(hmx);
			hmx = NULL;
			continue;
		}
		else
		{
			break;
		}
	}
	if(hmx != NULL)
	{
		LPMIXERCONTROL pmxctrl = (LPMIXERCONTROL)malloc(mxl.cControls * sizeof(MIXERCONTROL)); 
		MIXERLINECONTROLS mxlctrl = {sizeof(mxlctrl), mxl.dwLineID, 0, mxl.cControls, sizeof(MIXERCONTROL), pmxctrl};
		mixerGetLineControls((HMIXEROBJ)hmx, &mxlctrl, MIXER_GETLINECONTROLSF_ALL);
		// Now walk through each control to find a type of LIST control. This
		// can be either Mux, Single-select, Mixer or Multiple-select.
		DWORD i;
		for(i = 0; i < mxl.cControls; i++)
		{
			if (MIXERCONTROL_CT_CLASS_LIST == (pmxctrl[i].dwControlType&MIXERCONTROL_CT_CLASS_MASK))
			{
				break;
			}
		}
		if (i < mxl.cControls) 
		{
			// Found a LIST control,Check if the LIST control is a Mux or Single-select type
			BOOL bOneItemOnly = FALSE;
			switch (pmxctrl[i].dwControlType) 
			{
			case MIXERCONTROL_CONTROLTYPE_MUX:
			case MIXERCONTROL_CONTROLTYPE_SINGLESELECT:
				{
					bOneItemOnly = TRUE;
				}				
			}

			DWORD cChannels = mxl.cChannels, cMultipleItems = 0;

			if (MIXERCONTROL_CONTROLF_UNIFORM & pmxctrl[i].fdwControl)
			{
				cChannels = 1;
			}

			if (MIXERCONTROL_CONTROLF_MULTIPLE & pmxctrl[i].fdwControl)
			{
				cMultipleItems = pmxctrl[i].cMultipleItems;
			}

			//得到每一项的描述
			LPMIXERCONTROLDETAILS_LISTTEXT plisttext = (LPMIXERCONTROLDETAILS_LISTTEXT)malloc(cChannels * cMultipleItems * sizeof(MIXERCONTROLDETAILS_LISTTEXT)); 
			MIXERCONTROLDETAILS mxcd = {sizeof(mxcd), pmxctrl[i].dwControlID,cChannels,(HWND)cMultipleItems, sizeof(MIXERCONTROLDETAILS_LISTTEXT), (LPVOID)plisttext}; 
			mixerGetControlDetails((HMIXEROBJ)hmx, &mxcd, MIXER_GETCONTROLDETAILSF_LISTTEXT); 

			//得到每一项的值
			LPMIXERCONTROLDETAILS_BOOLEAN plistbool =(LPMIXERCONTROLDETAILS_BOOLEAN) malloc(cChannels * cMultipleItems * sizeof MIXERCONTROLDETAILS_BOOLEAN); 
			mxcd.cbDetails = sizeof MIXERCONTROLDETAILS_BOOLEAN;
			mxcd.paDetails = plistbool;
			mixerGetControlDetails((HMIXEROBJ)hmx, &mxcd,MIXER_GETCONTROLDETAILSF_VALUE); 

			//Select the "Microphone" item
			BOOL haveSel=FALSE;
			/*			//检查是否选择了，暂时不用
			for (DWORD j=0; j<cMultipleItems; j = j + cChannels)
			{
			if(plistbool[j].fValue = plistbool[j+ cChannels - 1].fValue == 1)
			haveSel=TRUE;
			}
			*/			
			for (DWORD j = 0; j < cMultipleItems; j += cChannels)
			{
				//				if (0 == strcmp(plisttext[j].szName, "Microphone"))// Select it for both left and right channels
				if (bOneItemOnly)
				{
					if(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == plisttext[j].dwParam2)
					{
						if(!haveSel)
						{
							haveSel = TRUE;
							plistbool[j].fValue = plistbool[j+cChannels-1].fValue = 1;
						}
						else
						{
							plistbool[j].fValue = plistbool[j+cChannels-1].fValue = 0;
						}
					}
					else
					{
						plistbool[j].fValue = plistbool[j+cChannels-1].fValue = 0;
					}
				}
				else
				{
					if(MIXERLINE_COMPONENTTYPE_SRC_MICROPHONE == plisttext[j].dwParam2)
					{
						plistbool[j].fValue = plistbool[j+ cChannels - 1].fValue = 1;
					}
				}
			}

			mixerSetControlDetails((HMIXEROBJ)hmx, &mxcd, MIXER_GETCONTROLDETAILSF_VALUE); 
			free(plisttext);
			free(plistbool);
			free(pmxctrl);
			mixerClose(hmx);

			return TRUE;
		}
		else
		{
			free(pmxctrl);
			mixerClose(hmx);

			return FALSE;
		}
	}

	return FALSE;
}

BOOL CWaveIn::CheckAudioDevice()
{
	UINT mixernum = mixerGetNumDevs();
	if(mixernum == 0)
	{
		return FALSE;
	}

	HMIXER hmx = NULL;
	MIXERLINE mxl;
	DWORD hr;
	for(DWORD i = 0; i < mixernum; i++)
	{
		hr = mixerOpen(&hmx, i, 0, 0, 0);
		if(hr != MMSYSERR_NOERROR)
		{
			return FALSE;
		}
		mxl.cbStruct		= sizeof(mxl);
		mxl.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

		//Find a LIST control, if any, for the wave in line
		hr = mixerGetLineInfo((HMIXEROBJ)hmx, &mxl, MIXER_GETLINEINFOF_COMPONENTTYPE);
		if(hr != MMSYSERR_NOERROR)
		{
			mixerClose(hmx);
			hmx = NULL;
			continue;
		}
		else
		{
			break;
		}
	}

	if(hmx != NULL)
	{
		mixerClose(hmx);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

