// LiveDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "LiveDlg.h"
#include "PTZ.h"
#include "CruiseDlg.h"
#include "strdef.h"
#include "DisplayDefine.h"

// CLiveDlg 对话框

//#define _WRITE_USER_FILE_
//#define TEST_DECODE
struct devInfo
{
	std::string addr;
	unsigned short port;
	std::string user;
	std::string pwd;
};
devInfo  _dev[8]={
	{"192.168.250.133", 9008, "admin", "123456"},
	{"192.168.250.168", 9008, "admin", "123456"},
	{"192.168.250.179", 9008, "admin", "123456"},
	{"192.168.250.213", 9008, "admin", "123456"},
	{"192.168.250.212", 9008, "admin", "123456"},
	{"192.168.250.171", 9008, "admin", "123456"},
	{"192.168.250.173", 9008, "admin", "123456"},
	{"192.168.250.156", 9008, "admin", "123456"},
	//{"192.168.250.179", 9008, "admin", "123456"},

};


IMPLEMENT_DYNAMIC(CLiveDlg, CDialog)

CLiveDlg::CLiveDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLiveDlg::IDD, pParent)
	, m_isOpenAudio(FALSE)
	, m_chnn(0)
	, m_brightness(0)
	, m_hue(0)
	, m_saturation(0)
	, m_contrast(0)
	, m_volume(0)
	, m_currentDivide(0)
	, m_audioDivide(0)
	, m_presetNo(0)
	, m_cruiseNo(0)
	, m_isSingleLive(FALSE)
	, m_bManualAlarm(FALSE)
	, m_pAudioOutBuffer(NULL)
	, m_bPTZ3DCtrling(FALSE)
	, m_bEnablePTZ3DCtrl(FALSE)
	,m_sPointX(0)
	,m_sPointY(0)
	,m_ePointX(0)
	,m_ePointY(0)
	,m_bMouseLBDown(FALSE)
    ,m_bDrawRect(FALSE)
{
	m_frist = true;
	memset(m_playHandle, 0x0, sizeof(LONG) * MAX_DIVIDE_NUM);
	memset(m_playChnn, 0x0, sizeof(int) * MAX_DIVIDE_NUM);
	memset(m_isStartLive, 0x0, sizeof(BOOL) * MAX_DIVIDE_NUM);
	memset(m_isOpenIris, 0x0, sizeof(BOOL) * MAX_DIVIDE_NUM);
	memset(m_isOpenAutoScan, 0x0, sizeof(BOOL) * MAX_DIVIDE_NUM);
	memset(m_isOpenRandomScan, 0x0, sizeof(BOOL) * MAX_DIVIDE_NUM);
	memset(m_isTrackStart, 0x0, sizeof(BOOL) * MAX_DIVIDE_NUM);
	memset(m_isTrackRecording, 0x0, sizeof(BOOL) * MAX_DIVIDE_NUM);
	memset(m_isCruiseStart, 0x0, sizeof(BOOL) * MAX_DIVIDE_NUM);
	memset(m_isSavingData, 0x0, sizeof(BOOL) * MAX_DIVIDE_NUM);
	memset(m_presetSel, 0x0, sizeof(int) * MAX_DIVIDE_NUM);
	memset(m_cruiseSel, 0x0, sizeof(int) * MAX_DIVIDE_NUM);
    memset(m_szFisheyeMode, 0x0, sizeof(int) * MAX_DIVIDE_NUM);
}

CLiveDlg::~CLiveDlg()
{
	if (m_pAudioOutBuffer)
	{
		delete []m_pAudioOutBuffer;
		m_pAudioOutBuffer = NULL;
	}
}

void CLiveDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBO_CHANNEL, m_chnn);
	DDX_Slider(pDX, IDC_SLIDER_BRIGHTNESS, m_brightness);
	DDX_Slider(pDX, IDC_SLIDER_HUE, m_hue);
	DDX_Slider(pDX, IDC_SLIDER_SATURATION, m_saturation);
	DDX_Slider(pDX, IDC_SLIDER_CONTRAST, m_contrast);
	DDX_Slider(pDX, IDC_SLIDER_VOLUME, m_volume);
	DDX_Control(pDX, IDC_SLIDER_PTZ_SPEED, m_PTZSpeedCtrl);
	DDX_CBIndex(pDX, IDC_COMBO_PRESET, m_presetNo);
	DDX_CBIndex(pDX, IDC_COMBO_CRUISE, m_cruiseNo);
    DDX_CBIndex(pDX, IDC_COMBO_FISHEYEMODE, m_iFisheyeMode);
	DDX_Control(pDX, IDC_CMB_DEVICE, m_deviceList);
}


BEGIN_MESSAGE_MAP(CLiveDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLiveDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CLiveDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_LIVE, &CLiveDlg::OnBnClickedButtonLive)
	ON_CBN_SELCHANGE(IDC_COMBO_CHANNEL, &CLiveDlg::OnCbnSelchangeComboChannel)
	ON_COMMAND(ID_LIVE_CAPTURE, &CLiveDlg::OnLiveCapture)
	ON_COMMAND(ID_LIVE_AUDIO, &CLiveDlg::OnLiveAudio)
	ON_UPDATE_COMMAND_UI(ID_LIVE_AUDIO, &CLiveDlg::OnUpdateLiveAudio)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_DEFAULT, &CLiveDlg::OnBnClickedButtonColorDefault)
	ON_BN_CLICKED(IDC_BUTTON_COLOR_SAVE, &CLiveDlg::OnBnClickedButtonColorSave)
	ON_WM_SHOWWINDOW()
	ON_WM_CONTEXTMENU()
	ON_WM_INITMENUPOPUP()
	ON_WM_HSCROLL()
	ON_CONTROL_RANGE(STN_CLICKED, IDC_LIVE_DIVIDE_1, IDC_LIVE_DIVIDE_16, OnClickedDivide)
	ON_CONTROL_RANGE(STN_DBLCLK, IDC_LIVE_DIVIDE_1, IDC_LIVE_DIVIDE_16, OnDBClickedDivide)
	ON_STN_DBLCLK(IDC_LIVE_AREA, &CLiveDlg::OnStnDblclickLiveArea)
	ON_WM_PAINT()
	//ON_BN_CLICKED(IDC_BUTTON_PTZ1, &CLiveDlg::OnBnClickedButtonPtz1)
	//ON_BN_CLICKED(IDC_BUTTON_PTZ2, &CLiveDlg::OnBnClickedButtonPtz2)
	//ON_BN_CLICKED(IDC_BUTTON_PTZ3, &CLiveDlg::OnBnClickedButtonPtz3)
	//ON_BN_CLICKED(IDC_BUTTON_PTZ4, &CLiveDlg::OnBnClickedButtonPtz4)
	//ON_BN_CLICKED(IDC_BUTTON_PTZ5, &CLiveDlg::OnBnClickedButtonPtz5)
	//ON_BN_CLICKED(IDC_BUTTON_PTZ6, &CLiveDlg::OnBnClickedButtonPtz6)
	//ON_BN_CLICKED(IDC_BUTTON_PTZ7, &CLiveDlg::OnBnClickedButtonPtz7)
	//ON_BN_CLICKED(IDC_BUTTON_PTZ8, &CLiveDlg::OnBnClickedButtonPtz8)
	//ON_BN_CLICKED(IDC_BUTTON_PTZ9, &CLiveDlg::OnBnClickedButtonPtz9)
	ON_BN_CLICKED(IDC_BUTTON_NEAR, &CLiveDlg::OnBnClickedButtonNear)
	ON_BN_CLICKED(IDC_BUTTON_FAR, &CLiveDlg::OnBnClickedButtonFar)
	ON_BN_CLICKED(IDC_BUTTON_OUT, &CLiveDlg::OnBnClickedButtonOut)
	ON_BN_CLICKED(IDC_BUTTON_IN, &CLiveDlg::OnBnClickedButtonIn)
	ON_BN_CLICKED(IDC_BUTTON_IRIS, &CLiveDlg::OnBnClickedButtonIris)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_SCAN, &CLiveDlg::OnBnClickedButtonAutoScan)
	ON_BN_CLICKED(IDC_BUTTON_PRESET_SET, &CLiveDlg::OnBnClickedButtonPresetSet)
	ON_BN_CLICKED(IDC_BUTTON_PRESET_GO, &CLiveDlg::OnBnClickedButtonPresetGo)
	ON_BN_CLICKED(IDC_BUTTON_PRESET_DEL, &CLiveDlg::OnBnClickedButtonPresetDel)
	ON_BN_CLICKED(IDC_BUTTON_CRUISE_SETUP, &CLiveDlg::OnBnClickedButtonCruiseSetup)
	ON_BN_CLICKED(IDC_BUTTON_TRACK_RECORD, &CLiveDlg::OnBnClickedButtonTrackRecord)
	ON_BN_CLICKED(IDC_BUTTON_TRACK, &CLiveDlg::OnBnClickedButtonTrack)
	ON_BN_CLICKED(IDC_BUTTON_CRUISE, &CLiveDlg::OnBnClickedButtonCruise)
	ON_CBN_SELCHANGE(IDC_COMBO_PRESET, &CLiveDlg::OnCbnSelchangeComboPreset)
	ON_CBN_SELCHANGE(IDC_COMBO_CRUISE, &CLiveDlg::OnCbnSelchangeComboCruise)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE, &CLiveDlg::OnBnClickedButtonCapture)
	ON_BN_CLICKED(IDC_BUTTON_LIVESAVE, &CLiveDlg::OnBnClickedButtonLivesave)
	ON_BN_CLICKED(IDC_BUTTON_Manual_Aram, &CLiveDlg::OnBnClickedButtonManualAram)
	ON_BN_CLICKED(IDC_BTN_PTZ_3D_CTRL, &CLiveDlg::OnBnClickedBtnPtz3dCtrl)
	ON_BN_CLICKED(IDC_BUTTON_CAPTURE_JPEG, &CLiveDlg::OnBnClickedButtonCaptureJpeg)
	ON_BN_CLICKED(IDC_BTN_MAKEIFRAME, &CLiveDlg::OnBnClickedBtnMakeiframe)
	ON_BN_CLICKED(IDC_BUTTON_AUTO_SCAN2, &CLiveDlg::OnBnClickedButtonAutoScan2)
	ON_CBN_SELCHANGE(IDC_COMBO_StreamType, &CLiveDlg::OnCbnSelchangeComboStreamtype)
    ON_CBN_SELCHANGE(IDC_COMBO_FISHEYEMODE, &CLiveDlg::OnCbnSelchangeComboFisheyemode)
    ON_BN_CLICKED(IDC_BUTTON_DrawRect, &CLiveDlg::OnBnClickedButtonDrawrect)
END_MESSAGE_MAP()


// CLiveDlg 消息处理程序

void CLiveDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CLiveDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}

void CLiveDlg::SetDeviceInfo( NET_SDK_DEVICEINFO* di, LONG userId)
{
	m_mapDeviceInfo[userId] = di;

	in_addr addrs;
	addrs.s_addr = di->deviceIP;
	int index = m_deviceList.AddString(inet_ntoa(addrs));
	m_deviceList.SetItemData(index, userId);
	m_deviceList.SetCurSel(index);
	
	CString temp;
	CComboBox * combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_CHANNEL));
	for (int i = 0; i < di->videoInputNum; i++)
	{
		temp.Format(STR_CHANNEL_D, i + 1);
		combo->AddString(temp);
	}
	combo->SetCurSel(m_playChnn[m_currentDivide]);

	if (userId >= 0)
	{
		m_userID = userId;
	}

	GetChnnInfo(0);

	NET_SDK_CHANNEL_PTZ *pOutChannelPtz = new NET_SDK_CHANNEL_PTZ[di->videoInputNum];
	int returnListNum = 0;
	NET_SDK_GetSupportPtzList(m_userID, di->videoInputNum, pOutChannelPtz, &returnListNum);
	for (int i=0; i<returnListNum; i++)
	{
		TRACE("dwChannel= %d, eChanneltype = %d \n", pOutChannelPtz[i].dwChannel, pOutChannelPtz[i].eChanneltype);
	}
	delete(pOutChannelPtz);

	int streamNum = NET_SDK_SupportStreamNum(m_userID, 0);
	CComboBox* pStreamTypeCtrl = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_StreamType));
	if (pStreamTypeCtrl)
	{
		pStreamTypeCtrl->AddString(_T("MainStream"));
		pStreamTypeCtrl->AddString(_T("SubStream"));
		for (int i=2; i<streamNum; i++)
		{
			CString Stream;
			Stream.Format(_T("Stream%d"),i);

			pStreamTypeCtrl->AddString(Stream);
		}
		pStreamTypeCtrl->SetCurSel((int)m_eLiveStreamType);
	}


}

void CLiveDlg::OnBnClickedButtonLive()
{
	UpdateData();
#ifdef TEST_DECODE
	static BOOL openvideo = FALSE;
	if(!openvideo)
	{
		for (int i = 0; i < 8; i++)
		{
			NET_SDK_DEVICEINFO m_deviceInfo;
			m_userid[i] = NET_SDK_Login((char*)_dev[i].addr.c_str(), _dev[i].port,(char*)_dev[i].user.c_str(), (char*)_dev[i].pwd.c_str() , &m_deviceInfo);

			if(m_userid[i] > 0)
			{
				NET_SDK_CLIENTINFO clientInfo;
				ZeroMemory(&clientInfo, sizeof(NET_SDK_CLIENTINFO));

				clientInfo.lChannel = 0;    // channel index
				clientInfo.streamType = 1;  // main

				HWND handle = GetDlgItem(IDC_LIVE_DIVIDE_1 + i)->GetSafeHwnd();
				clientInfo.hPlayWnd = handle;

				long playId = NET_SDK_LivePlay(m_userid[i], &clientInfo, NULL, (LPVOID)this);
				m_playHandle[i] = playId;


				handle = GetDlgItem(IDC_LIVE_DIVIDE_1 + i+8)->GetSafeHwnd();
				clientInfo.hPlayWnd = handle;

				playId = NET_SDK_LivePlay(m_userid[i], &clientInfo, NULL, (LPVOID)this);
				m_playHandle[i+8] = playId;
			}

		}
		openvideo = TRUE;
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			if(m_userid[i] > 0)
			{
				

				NET_SDK_StopLivePlay(m_playHandle[i]);
				//NET_SDK_PlayBackControl(m_playId[i], NET_SDK_PLAYCTRL_STOPAUDIO, 0, NULL);
				//NET_SDK_StopPlayBack(m_playId[i]);
				NET_SDK_StopLivePlay(m_playHandle[i+8]);


				NET_SDK_Logout(m_userid[i]);
			} 
		}
		openvideo = FALSE;
	}
	
#else

    //for (int i = 0; i < 8; i++)
    //{
     //  m_currentDivide = i;
      //  m_chnn = i;


#if 0
	int chl = 0;
	if (m_frist)
	{
		chl = 0;
	}
	else
	{
		chl = 4;
	}
	int i=0;
	for (i=0; i<4; i++)
	{
		if (m_isStartLive[i])
		{
			StopLiveTest(i+chl);
		}	
	}
	m_frist = !m_frist;
	if (m_frist)
	{
		chl = 0;
	}
	else
	{
		chl = 4;
	}
	for (i=0; i<4; i++)
	{
		StartLiveTest(i+chl);	
	}
	
	
#else
	if (m_isStartLive[m_currentDivide])
	{
		StopLive();
	}
	else
	{
		StartLive();
	}
#endif
#endif
	UpdateUI();
    }

void CLiveDlg::StartLive()
{
	NET_SDK_CLIENTINFO clientInfo = {0};
	clientInfo.hPlayWnd = GetDlgItem(IDC_LIVE_DIVIDE_1 + m_currentDivide)->GetSafeHwnd();
//	clientInfo.hPlayWnd = NULL;
	clientInfo.lChannel = m_chnn    ;
	//clientInfo.streamType = NET_SDK_MAIN_STREAM /*NET_SDK_SUB_STREAM*/;
	CComboBox* pStreamTypeCtrl = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_StreamType));
	if (pStreamTypeCtrl)
	{
		clientInfo.streamType = (LONG)pStreamTypeCtrl->GetCurSel();
	}
	int device = m_deviceList.GetCurSel();
	if(device != LB_ERR)
	{
		m_userID = m_deviceList.GetItemData(device);
	}

#ifdef _WRITE_USER_FILE_
	if (m_chnn < MAX_DIVIDE_NUM)
	{
		CString strRec;
		strRec.Format(_T("./test%d.rec"), m_chnn);
		m_RecFilehandle[m_chnn] = CreateFile(strRec, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		m_recNeedFormat[m_chnn] = true;
	}
	
#endif
#if 1
	LONG playHandle = NET_SDK_LivePlay(m_userID, &clientInfo, CLiveDlg::LiveCallBack, this);
#else
	LONG playHandle = NET_SDK_LivePlay(m_userID, &clientInfo, NULL, NULL);
#endif
	//NET_SDK_RegisterDrawFun(playHandle, &CLiveDlg::drawCallBack, this);
	NET_SDK_SetYUVCallBack(playHandle,&CLiveDlg::yuvCallBack, this);

	if (playHandle > 0)
	{
		m_playHandle[m_currentDivide] = playHandle;
		m_playChnn[m_currentDivide] = m_chnn;
		m_isStartLive[m_currentDivide] = TRUE;

		if(0 == m_currentDivide)
		{
			WAVEFORMATEX wfx;
			wfx.wFormatTag = WAVE_FORMAT_PCM;
			wfx.nChannels = 1;
			wfx.wBitsPerSample = 16;
			wfx.nSamplesPerSec = 8000;
			wfx.nBlockAlign = 2;
			wfx.nAvgBytesPerSec = 16000;
			wfx.cbSize = 0;
			m_waveOut.Start(wfx);
		}
	}
	else
	{
		DWORD dwErrorCode = NET_SDK_GetLastError();
		CString strMsg;
		strMsg.Format(_T("%s\r\n%s：%d"), STR_LIVE_FAILED,STR_INFO_ERROR_CODE,dwErrorCode);
		AfxMessageBox(/*STR_LIVE_FAILED*/strMsg);
	}
}

void CLiveDlg::StopLive()
{
	if (!m_isStartLive[m_currentDivide])
	{
		return;
	}
	NET_SDK_StopLivePlay(m_playHandle[m_currentDivide]);
	NET_SDK_ReleaseAudioDecoder(m_lAudioDecHandle[m_currentDivide]);
	if(0 == m_currentDivide)
	{
		m_waveOut.Stop();
	}
	m_playHandle[m_currentDivide] = 0;
	m_isStartLive[m_currentDivide] = FALSE;

#ifdef _WRITE_USER_FILE_
	if (m_playChnn[m_currentDivide] < MAX_DIVIDE_NUM)
	{
		CloseHandle(m_RecFilehandle[m_playChnn[m_currentDivide]]);
	}
	
#endif
	CRect rect;
	CWnd *wnd = NULL;
	
	wnd = GetDlgItem(IDC_LIVE_DIVIDE_1 + m_currentDivide);
	wnd->Invalidate();
}
void CLiveDlg::StartLiveTest(int chnl)
{
	int Indexchl = 0;
	if (m_frist)
	{
		Indexchl = 0;
	}
	else
	{
		Indexchl = 4;
	}
	chnl -=Indexchl;
	NET_SDK_CLIENTINFO clientInfo = {0};
	clientInfo.hPlayWnd = GetDlgItem(IDC_LIVE_DIVIDE_1 + chnl)->GetSafeHwnd();
	clientInfo.lChannel = chnl+Indexchl;
	//clientInfo.streamType = NET_SDK_MAIN_STREAM /*NET_SDK_SUB_STREAM*/;
	CComboBox* pStreamTypeCtrl = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_StreamType));
	if (pStreamTypeCtrl)
	{
		clientInfo.streamType = (LONG)pStreamTypeCtrl->GetCurSel();
	}
	int device = m_deviceList.GetCurSel();
	if(device != LB_ERR)
	{
		m_userID = m_deviceList.GetItemData(device);
	}

#ifdef _WRITE_USER_FILE_
	if (chnl < MAX_DIVIDE_NUM)
	{
		CString strRec;
		strRec.Format(_T("./test%d.rec"), chnl);
		m_RecFilehandle[chnl] = CreateFile(strRec, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		m_recNeedFormat[chnl] = true;
	}
	
#endif
#if 1
	LONG playHandle = NET_SDK_LivePlay(m_userID, &clientInfo, CLiveDlg::LiveCallBack, this);
#else
	LONG playHandle = NET_SDK_LivePlay(m_userID, &clientInfo, NULL, NULL);
#endif
	//NET_SDK_RegisterDrawFun(playHandle, &CLiveDlg::drawCallBack, this);
	if (playHandle > 0)
	{
		m_playHandle[chnl] = playHandle;
		m_playChnn[chnl] = chnl+Indexchl;
		m_isStartLive[chnl] = TRUE;

		if(0 == chnl)
		{
			WAVEFORMATEX wfx;
			wfx.wFormatTag = WAVE_FORMAT_PCM;
			wfx.nChannels = 1;
			wfx.wBitsPerSample = 16;
			wfx.nSamplesPerSec = 8000;
			wfx.nBlockAlign = 2;
			wfx.nAvgBytesPerSec = 16000;
			wfx.cbSize = 0;
			m_waveOut.Start(wfx);
		}
	}
	else
	{
		DWORD dwErrorCode = NET_SDK_GetLastError();
		CString strMsg;
		strMsg.Format(_T("%s\r\n%s：%d"), STR_LIVE_FAILED,STR_INFO_ERROR_CODE,dwErrorCode);
		AfxMessageBox(/*STR_LIVE_FAILED*/strMsg);
	}
}

void CLiveDlg::StopLiveTest(int chnl)
{
	int Indexchl = 0;
	if (m_frist)
	{
		Indexchl = 0;
	}
	else
	{
		Indexchl = 4;
	}
	chnl -=Indexchl;

	if (!m_isStartLive[chnl])
	{
		return;
	}
	NET_SDK_StopLivePlay(m_playHandle[chnl]);
	NET_SDK_ReleaseAudioDecoder(m_lAudioDecHandle[chnl]);
	if(0 == chnl)
	{
		m_waveOut.Stop();
	}
	m_playHandle[chnl] = 0;
	m_isStartLive[chnl] = FALSE;

#ifdef _WRITE_USER_FILE_
	if (chnl < MAX_DIVIDE_NUM)
	{
		CloseHandle(m_RecFilehandle[chnl]);
	}
	
#endif
	CRect rect;
	CWnd *wnd = NULL;
	
	wnd = GetDlgItem(IDC_LIVE_DIVIDE_1 + chnl);
	wnd->Invalidate();
}
BOOL CLiveDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CSliderCtrl *slider = NULL;
	for (int i = IDC_SLIDER_BRIGHTNESS; i <= IDC_SLIDER_CONTRAST; i++)
	{
		slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(i));
		slider->SetRangeMax(255);
	}
	
	CComboBox *combo = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_PRESET));
	CString temp;
	for (int i = 0; i < /*128*/360; i++)
	{
		temp.Format(_T("preset %d"), i + 1);
		combo->AddString(temp);
	}
	
	combo = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_CRUISE));
	for (int i = 0; i < 32; i++)
	{
		temp.Format(_T("cruise %d"), i + 1);
		combo->AddString(temp);
	}

    combo = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_FISHEYEMODE));
    combo->AddString("ORIGNAL");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_ORIGNAL);
    combo->AddString("ROOF");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_ORIGNAL);
//     combo->AddString("ROOF_360");
//     combo->SetItemData(combo->GetCount()-1, FISHEYE_ROOF_360);
    combo->AddString("ROOF_2x180");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_ROOF_2x180);
    combo->AddString("ROOF_FISH_3PTZ");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_ROOF_FISH_3PTZ);
    combo->AddString("ROOF_FISH_4PTZ");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_ROOF_FISH_4PTZ);
//     combo->AddString("ROOF_360_6PTZ");
//     combo->SetItemData(combo->GetCount()-1, FISHEYE_ROOF_360_6PTZ);
    combo->AddString("ROOF_FISH_8PTZ");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_ROOF_FISH_8PTZ);

    combo->AddString("WALL");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_ORIGNAL);
    combo->AddString("WALL_180");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_WALL_180);
    combo->AddString("WALL_180_3PTZ");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_WALL_180_3PTZ);
    combo->AddString("WALL_180_4PTZ");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_WALL_180_4PTZ);
    combo->AddString("WALL_180_8PTZ");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_WALL_180_8PTZ);

    combo->AddString("DESKTOP");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_ORIGNAL);
//     combo->AddString("DESKTOP_360");
//     combo->SetItemData(combo->GetCount()-1, FISHEYE_DESKTOP_360);
    combo->AddString("DESKTOP_2x180");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_DESKTOP_2x180);
    combo->AddString("DESKTOP_FISH_3PTZ");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_DESKTOP_FISH_3PTZ);
    combo->AddString("DESKTOP_FISH_4PTZ");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_DESKTOP_FISH_4PTZ);
//     combo->AddString("DESKTOP_360_6PTZ");
//     combo->SetItemData(combo->GetCount()-1, FISHEYE_DESKTOP_360_6PTZ);
    combo->AddString("DESKTOP_FISH_8PTZ");
    combo->SetItemData(combo->GetCount()-1, FISHEYE_DESKTOP_FISH_8PTZ);
	
	slider = reinterpret_cast<CSliderCtrl *>(GetDlgItem(IDC_SLIDER_VOLUME));
	slider->SetRangeMax(100);
	
	m_volume = 50;
	
	m_PTZSpeedCtrl.SetRangeMax(8);
	m_PTZSpeedCtrl.SetRangeMin(1);
	CButton *button = NULL;
	
	for (int i = IDC_BUTTON_PTZ1, j = IDB_BITMAP_PTZ1; i <= IDC_BUTTON_PTZ9; i++, j++)
	{
		button = reinterpret_cast<CButton*>(GetDlgItem(i));
		m_ptzBitmap[i - IDC_BUTTON_PTZ1].LoadBitmap(j);
		button->SetBitmap(m_ptzBitmap[i - IDC_BUTTON_PTZ1]);
	}

	CRect rect, drect;
	GetDlgItem(IDC_LIVE_AREA)->GetWindowRect(&rect);
	ScreenToClient(&rect);

	for(int row = 0; row < 4; row++)
	{
		for(int col = 0; col < 4; col++)
		{
			drect.top = rect.top + row * rect.Height()/4 + 1;
			drect.left = rect.left + col * rect.Width()/4 + 1;
			drect.right = drect.left + rect.Width()/4 - 1;
			drect.bottom = drect.top + rect.Height()/4 - 1;
			//GetDlgItem(IDC_LIVE_DIVIDE_1+row*2+col)->MoveWindow(&drect);//4 screens
			GetDlgItem(IDC_LIVE_DIVIDE_1+row*4+col)->MoveWindow(&drect);//16 screens
		}
	}

	//码流类型

	m_eLiveStreamType = NET_SDK_MAIN_STREAM;
	/*CComboBox* pStreamTypeCtrl = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_StreamType));
	if (pStreamTypeCtrl)
	{
		pStreamTypeCtrl->AddString(_T("MainStream"));
		pStreamTypeCtrl->AddString(_T("SubStream"));
		pStreamTypeCtrl->SetCurSel((int)m_eLiveStreamType);
	}*/
	UpdateUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CLiveDlg::GetChnnInfo( int chnn )
{
	DWORD brightness;
	DWORD contrast;
	DWORD saturation;
	DWORD hue;

	//NET_SDK_GetVideoEffect(m_userID, chnn, &brightness, &contrast, &saturation, &hue);
	m_brightness = 50;//brightness;
	m_contrast = 50;//contrast;
	m_saturation = 50;// saturation;
	m_hue = 50;//hue;
	
	UpdateData(FALSE);
}

void CLiveDlg::OnCbnSelchangeComboChannel()
{
	UpdateData();
	
	GetChnnInfo(m_chnn);

	if (m_isStartLive[m_currentDivide])
	{
		StopLive();
		StartLive();
	}
}

void CLiveDlg::OnLiveCapture()
{
	CFileDialog dlg(FALSE, _T("bmp"), _T("sample.bmp"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("bmp files(*.bmp)|*.bmp||"));
	if (IDOK == dlg.DoModal())
	{
		CString pathName = dlg.GetPathName();
		BOOL ret = NET_SDK_CapturePicture(m_playHandle[m_currentDivide], pathName.GetBuffer());
		if (!ret)
		{
			AfxMessageBox(STR_SNAP_FAILED);
		}
	}
}

void CLiveDlg::OnLiveAudio()
{
	if (m_isOpenAudio)
	{
		NET_SDK_CloseSound();
		m_isOpenAudio = FALSE;
	}	
	else
	{
		UpdateData();
		BOOL ret = NET_SDK_OpenSound(m_playHandle[m_currentDivide]);
		if (ret)
		{
			unsigned short volume = 0;
			double temp = (((double)m_volume) / 100) * 0xffff;
			volume = (short )temp;
			NET_SDK_Volume(m_playHandle[m_currentDivide], volume);
			m_isOpenAudio = TRUE;
			m_audioDivide = m_currentDivide;
		}
		else
		{
			AfxMessageBox(STR_START_AUDIO_FAILED);
		}
	}
}

void CLiveDlg::OnUpdateLiveAudio(CCmdUI *pCmdUI)
{
	if (m_isOpenAudio)
	{
		if (m_audioDivide == m_currentDivide)
		{
			pCmdUI->SetText(STR_CLOSE_AUDIO);
		}
		else
		{
			pCmdUI->Enable(FALSE);
		}
	}
	else
	{
		pCmdUI->SetText(STR_START_AUDIO);
	}
}

void CLiveDlg::OnBnClickedButtonColorDefault()
{
	DWORD brightness;
	DWORD contrast;
	DWORD saturation;
	DWORD hue;

	NET_SDK_GetDefaultVideoEffect(m_userID, &brightness, &contrast, &saturation, &hue);
	m_brightness = brightness;
	m_contrast = contrast;
	m_saturation = saturation;
	m_hue = hue;
	
	NET_SDK_SetVideoEffect(m_userID, m_chnn, m_brightness, m_contrast, m_saturation, m_hue);

	UpdateData(FALSE);
}

void CLiveDlg::OnBnClickedButtonColorSave()
{
	UpdateData();

	BOOL ret = NET_SDK_SaveVideoEffect(m_userID, m_chnn, m_brightness, m_contrast, m_saturation, m_hue);
	//if (ret)
	//{
	//	AfxMessageBox(_T("保存成功"), MB_ICONINFORMATION);
	//}
	//else
	//{
	//	DWORD error = NET_SDK_GetLastError();
	//	AfxMessageBox(_T("保存失败"));
	//}
}

void CLiveDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	if (!bShow)
	{
		int temp = m_currentDivide;
		for (int i = 0; i < MAX_DIVIDE_NUM; i++)
		{
			m_currentDivide = i;
			StopLive();
		}
		m_currentDivide = temp;
		
		NET_SDK_CloseSound();
		m_isOpenAudio = FALSE;
		UpdateUI();
	}
	else
	{
		CRect rect;
		GetDlgItem(IDC_LIVE_AREA)->GetClientRect(&rect);
		CDC *dc = GetDlgItem(IDC_LIVE_AREA)->GetDC();
		CBrush brush(RGB(0, 0, 0));
		dc->FillRect(&rect, &brush);
		GetDlgItem(IDC_LIVE_AREA)->ReleaseDC(dc);
		Invalidate(TRUE);
	}
	CDialog::OnShowWindow(bShow, nStatus);
}


void CLiveDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CRect rect;
	
	if (!m_isSingleLive)
	{
		for (int i = 0; i < MAX_DIVIDE_NUM; i++)
		{
			GetDlgItem(IDC_LIVE_DIVIDE_1 + i)->GetWindowRect(&rect);
			if (rect.PtInRect(point))
			{
				OnClickedDivide(IDC_LIVE_DIVIDE_1 + i);
			}
		}
	}

	GetDlgItem(IDC_LIVE_DIVIDE_1 + m_currentDivide)->GetWindowRect(&rect);

	if (m_isStartLive[m_currentDivide] && rect.PtInRect(point))
	{
		CPoint p;
		GetCursorPos(&p);

		CMenu menu;
		menu.LoadMenu(IDR_MENU_POPUP);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN, p.x, p.y, this);
	}
}

void CLiveDlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	DrawGrid();
}

void CLiveDlg::OnInitMenuPopup(CMenu *pPopupMenu, UINT nIndex,BOOL bSysMenu)
{
	//CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);

	// TODO: Add your message handler code here
	ASSERT(pPopupMenu != NULL);
	// Check the enabled state of various menu items.

	CCmdUI state;
	state.m_pMenu = pPopupMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// Determine if menu is popup in top-level menu and set m_pOther to
	// it if so (m_pParentMenu == NULL indicates that it is secondary popup).
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pPopupMenu->m_hMenu)
		state.m_pParentMenu = pPopupMenu;    // Parent == child for tracking popup.
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = this;
		// Child windows don't have menus--need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pPopupMenu->m_hMenu)
				{
					// When popup is found, m_pParentMenu is containing menu.
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
		state.m_nIndex++)
	{
		state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // Menu separator or invalid cmd - ignore it.

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// Possibly a popup menu, route to first item of that popup.
			state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // First item of popup can't be routed to.
			}
			state.DoUpdate(this, TRUE);   // Popups are never auto disabled.
		}
		else
		{
			// Normal menu item.
			// Auto enable/disable if frame window has m_bAutoMenuEnable
			// set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			state.DoUpdate(this, FALSE);
		}

		// Adjust for menu deletions and additions.
		UINT nCount = pPopupMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pPopupMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}

}

void CLiveDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	UpdateData();

	CPoint point;
	GetCursorPos(&point);
	CRect rect1, rect2, rect3, rect4, rect5, rect6;
	GetDlgItem(IDC_SLIDER_VOLUME)->GetWindowRect(&rect1);
	GetDlgItem(IDC_SLIDER_BRIGHTNESS)->GetWindowRect(&rect3);
	GetDlgItem(IDC_SLIDER_SATURATION)->GetWindowRect(&rect4);
	GetDlgItem(IDC_SLIDER_HUE)->GetWindowRect(&rect5);
	GetDlgItem(IDC_SLIDER_CONTRAST)->GetWindowRect(&rect6);
//	m_PTZSpeedCtrl.GetWindowRect(&rect2);
	if (rect1.PtInRect(point))
	{
		unsigned short volume = 0;
		double temp = (((double)m_volume) / 100) * 0xffff;
		volume = (short )temp;

		NET_SDK_Volume(m_playHandle[m_currentDivide], volume);
	}
	else if (rect3.PtInRect(point) || rect4.PtInRect(point) || rect5.PtInRect(point) || rect6.PtInRect(point))
	{
		NET_SDK_SetVideoEffect(m_userID, m_chnn, m_brightness, m_contrast, m_saturation, m_hue);
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CLiveDlg::UpdateUI()
{
	for (int i = IDC_SLIDER_BRIGHTNESS; i <= IDC_SLIDER_VOLUME; i++)
	{
		GetDlgItem(i)->EnableWindow(m_isStartLive[m_currentDivide]);
	}
	
	for (int i = IDC_BUTTON_PTZ1; i <= IDC_BUTTON_PTZ9; i++)
	{
		//GetDlgItem(i)->EnableWindow(m_isStartLive[m_currentDivide]);
		GetDlgItem(i)->EnableWindow(true);
	}

	m_PTZSpeedCtrl.EnableWindow(true);

	GetDlgItem(IDC_BUTTON_NEAR)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_FAR)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_OUT)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_IN)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_IRIS)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_AUTO_SCAN)->EnableWindow(true);

	GetDlgItem(IDC_COMBO_PRESET)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_PRESET_SET)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_PRESET_GO)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_PRESET_DEL)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_TRACK_RECORD)->EnableWindow(m_isStartLive[m_currentDivide]);
	GetDlgItem(IDC_BUTTON_TRACK)->EnableWindow(m_isStartLive[m_currentDivide]);
	GetDlgItem(IDC_COMBO_CRUISE)->EnableWindow(!m_isCruiseStart[m_currentDivide]);
	GetDlgItem(IDC_BUTTON_CRUISE)->EnableWindow(true);
	GetDlgItem(IDC_BUTTON_CRUISE_SETUP)->EnableWindow(true);
    GetDlgItem(IDC_COMBO_FISHEYEMODE)->EnableWindow(true);
	
	GetDlgItem(IDC_BUTTON_LIVESAVE)->EnableWindow(m_isStartLive[m_currentDivide]);

	m_chnn = m_playChnn[m_currentDivide];

	if(m_isStartLive[m_currentDivide])
	{
		SetDlgItemText(IDC_BUTTON_LIVE, STR_CLOSE);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_LIVE, STR_START);
	}

	if (m_isOpenIris[m_currentDivide])
	{
		SetDlgItemText(IDC_BUTTON_IRIS, STR_CLOSE);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_IRIS, STR_START);
	}

	if (m_isOpenRandomScan[m_currentDivide])
	{
		SetDlgItemText(IDC_BUTTON_AUTO_SCAN, STR_CLOSE);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_AUTO_SCAN, STR_START);
	}

	if (m_isOpenAutoScan[m_currentDivide])
	{
		SetDlgItemText(IDC_BUTTON_AUTO_SCAN2, STR_CLOSE);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_AUTO_SCAN2, STR_START);
	}

	if (m_isTrackStart[m_currentDivide])
	{
		SetDlgItemText(IDC_BUTTON_TRACK, STR_STOP);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_TRACK, STR_START);
	}

	if (m_isTrackRecording[m_currentDivide])
	{
		SetDlgItemText(IDC_BUTTON_TRACK_RECORD, STR_STOP);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_TRACK_RECORD, STR_RECORD);
	}

	if (m_isCruiseStart[m_currentDivide])
	{
		SetDlgItemText(IDC_BUTTON_CRUISE, STR_STOP);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_CRUISE, STR_START);
	}

	if(m_isSavingData[m_currentDivide])
	{
		SetDlgItemText(IDC_BUTTON_LIVESAVE, _T("stop"));
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_LIVESAVE, _T("save"));
	}

	m_presetNo = m_presetSel[m_currentDivide];
	m_cruiseNo = m_cruiseSel[m_currentDivide];

	GetChnnInfo(m_playChnn[m_currentDivide]);

	//PTZ 3D ctrl
	if (!m_bPTZ3DCtrling)
	{
		GetDlgItem(IDC_BTN_PTZ_3D_CTRL)->SetWindowText("Start");
	}
	else
	{
		GetDlgItem(IDC_BTN_PTZ_3D_CTRL)->SetWindowText("Stop");
	}
	
	//GetDlgItem(IDC_BTN_PTZ_3D_CTRL)->EnableWindow(!m_isSingleLive && m_isStartLive[m_currentDivide]);
	GetDlgItem(IDC_BTN_PTZ_3D_CTRL)->EnableWindow(true);
}

void CLiveDlg::OnClickedDivide( UINT index )
{
	m_currentDivide = index - IDC_LIVE_DIVIDE_1;    
	UpdateUI();
	DrawGrid();
}

void CLiveDlg::OnDBClickedDivide( UINT index )
{
	if (m_isSingleLive)
	{
		m_isSingleLive = FALSE;
		GetDlgItem(IDC_LIVE_DIVIDE_1 + m_currentDivide)->MoveWindow(m_oldRect);
		for (int i = 0; i < MAX_DIVIDE_NUM; i++)
		{
			GetDlgItem(IDC_LIVE_DIVIDE_1 + i)->ShowWindow(SW_SHOW);
		}
		DrawGrid();
	}
	else
	{
		if (m_isStartLive[m_currentDivide])
		{
			m_isSingleLive = TRUE;

			GetDlgItem(IDC_LIVE_DIVIDE_1 + m_currentDivide)->GetWindowRect(&m_oldRect);
			ScreenToClient(&m_oldRect);

			CRect rect;
			GetDlgItem(IDC_LIVE_AREA)->GetWindowRect(&rect);
			ScreenToClient(&rect);

			GetDlgItem(m_currentDivide + IDC_LIVE_DIVIDE_1)->MoveWindow(rect);

			for (int i = 0; i < MAX_DIVIDE_NUM; i++)
			{
				if (i != m_currentDivide)
				{
					GetDlgItem(IDC_LIVE_DIVIDE_1 + i)->ShowWindow(SW_HIDE);
				}
			}
		}
	}

	//PTZ 3D ctrl
	if(m_bEnablePTZ3DCtrl && m_isStartLive[m_currentDivide])
	{
		m_bPTZ3DCtrling = !m_isSingleLive;
		if (!m_bPTZ3DCtrling)
		{
			GetDlgItem(IDC_BTN_PTZ_3D_CTRL)->SetWindowText("Start");
		}
		else
		{
			GetDlgItem(IDC_BTN_PTZ_3D_CTRL)->SetWindowText("Stop");
		}
	}
	if(m_isStartLive[m_currentDivide])
		GetDlgItem(IDC_BTN_PTZ_3D_CTRL)->EnableWindow(!m_isSingleLive);
	
}

void CLiveDlg::OnStnDblclickLiveArea()
{
	if (m_isStartLive[m_currentDivide])
	{
		StopLive();
		StartLive();
	}
	
	GetDlgItem(IDC_LIVE_AREA)->ShowWindow(SW_HIDE);

	for (int i = 0; i < MAX_DIVIDE_NUM; i++)
	{
		GetDlgItem(IDC_LIVE_DIVIDE_1 + i)->ShowWindow(SW_SHOW);
	}

	DrawGrid();
}

void CLiveDlg::OnPaint()
{
	CDialog::OnPaint();
	//TRACE(_T("called paint\n"));
	DrawGrid();
	/*for(int i = 0; i < 4; i++)
	{
		if(m_isStartLive[i])
		{
			NET_SDK_RefreshPlay(m_playHandle[m_currentDivide]);
		}
	}*/
}

void CLiveDlg::DrawGrid()
{
	//if (GetDlgItem(IDC_LIVE_AREA)->IsWindowVisible())
	//{
	//	return;
	//}
	if (m_isSingleLive)
	{
		return;
	}
	CDC *dc = GetDC();
	CWnd *wnd = NULL;
	CRect rect;
	CBrush bb(RGB(0, 0, 255));
	CBrush rb(RGB(255, 0, 0));

	for (int i = 0; i < MAX_DIVIDE_NUM; i++)
	{
		wnd = GetDlgItem(IDC_LIVE_DIVIDE_1 + i);
		wnd->GetWindowRect(&rect);
		ScreenToClient(&rect);
		rect.top--;
		rect.left--;
		rect.right++;
		rect.bottom++;
		//dc = wnd->GetDC();
		if (i == m_currentDivide)
		{
			dc->FrameRect(&rect, &rb);
		}
		else
		{
			dc->FrameRect(&rect, &bb);
		}

		//wnd->ReleaseDC(dc);
	}
	ReleaseDC(dc);
}

void CLiveDlg::OnBnClickedButtonNear()
{
	NET_SDK_PTZControl(m_playHandle[m_currentDivide], PTZ_CMD_NEAR, m_PTZSpeedCtrl.GetPos());
}

void CLiveDlg::OnBnClickedButtonFar()
{
	NET_SDK_PTZControl(m_playHandle[m_currentDivide], PTZ_CMD_FAR, m_PTZSpeedCtrl.GetPos());
}

void CLiveDlg::OnBnClickedButtonOut()
{
	NET_SDK_PTZControl(m_playHandle[m_currentDivide], PTZ_CMD_ZOOM_OUT, m_PTZSpeedCtrl.GetPos());
}

void CLiveDlg::OnBnClickedButtonIn()
{
	NET_SDK_PTZControl(m_playHandle[m_currentDivide], PTZ_CMD_ZOOM_IN, m_PTZSpeedCtrl.GetPos());
}

void CLiveDlg::OnBnClickedButtonIris()
{
	if (m_isOpenIris[m_currentDivide])
	{
		NET_SDK_PTZControl(m_playHandle[m_currentDivide], PTZ_CMD_IRIS_CLOSE, m_PTZSpeedCtrl.GetPos());
		m_isOpenIris[m_currentDivide] = FALSE;
	}
	else
	{
		NET_SDK_PTZControl(m_playHandle[m_currentDivide], PTZ_CMD_IRIS_OPEN, m_PTZSpeedCtrl.GetPos());
		m_isOpenIris[m_currentDivide] = TRUE;
	}
	UpdateUI();
}

void CLiveDlg::OnBnClickedButtonAutoScan()
{
	if (m_isOpenRandomScan[m_currentDivide])
	{
		NET_SDK_PTZAutoScan(m_playHandle[m_currentDivide], PTZ_CMD_AUTO_SCAN_STOP, 0);
		m_isOpenRandomScan[m_currentDivide] = FALSE;
	}
	else
	{
		NET_SDK_PTZAutoScan(m_playHandle[m_currentDivide], PTZ_CMD_AUTO_SCAN_START, m_PTZSpeedCtrl.GetPos(), FALSE);
		m_isOpenRandomScan[m_currentDivide] = TRUE;
	}
	m_isOpenAutoScan[m_currentDivide] = FALSE;
	UpdateUI();
}

void CLiveDlg::OnBnClickedButtonPresetSet()
{
	UpdateData();
	NET_SDK_PTZPreset(m_playHandle[m_currentDivide], PTZ_CMD_PRESET_SET, m_presetNo + 1); 
	
}

void CLiveDlg::OnBnClickedButtonPresetGo()
{
	UpdateData();
	NET_SDK_PTZPreset_Other(m_userID, m_chnn, PTZ_CMD_PRESET_GO, m_presetNo + 1);
}

void CLiveDlg::OnBnClickedButtonPresetDel()
{
	UpdateData();
	//NET_SDK_PTZPreset_Other(m_userID, m_chnn, PTZ_CMD_PRESET_DEL, m_presetNo + 1);
	BOOL ret = NET_SDK_PTZPreset_Other(m_userID, m_chnn, PTZ_CMD_CRUISE_DEL, m_presetNo + 1);
	TRACE("set cruise ret: %d", ret);
	
}

void CLiveDlg::OnBnClickedButtonCruiseSetup()
{
	UpdateData();
	CCruiseDlg dlg;
	if (IDOK == dlg.DoModal())
	{
		int n = dlg.GetCruiseNum();
		DD_CRUISE_POINT_INFO * cpi = new DD_CRUISE_POINT_INFO[n];
		memset(cpi, 0x0, sizeof(DD_CRUISE_POINT_INFO) * n);
		dlg.GetCruiseInfo((LPVOID)cpi);
		BOOL ret = NET_SDK_PTZSetCruise_Other(m_userID, m_chnn, m_cruiseNo + 1, cpi, n);
		TRACE("set cruise ret: %d", ret);
	}
}

void CLiveDlg::OnBnClickedButtonTrackRecord()
{
	if (m_isTrackRecording[m_currentDivide])
	{
		NET_SDK_PTZTrack(m_playHandle[m_currentDivide], PTZ_CMD_TRACK_START_RECORD);
		m_isTrackRecording[m_currentDivide] = FALSE;
	}
	else
	{
		NET_SDK_PTZTrack(m_playHandle[m_currentDivide], PTZ_CMD_TRACK_STOP_RECORD);
		m_isTrackRecording[m_currentDivide] = TRUE;
	}
	UpdateUI();
}

void CLiveDlg::OnBnClickedButtonTrack()
{
	if (m_isTrackStart[m_currentDivide])
	{
		NET_SDK_PTZTrack(m_playHandle[m_currentDivide], PTZ_CMD_TRACK_STOP);
		m_isTrackStart[m_currentDivide] = FALSE;
	}
	else
	{
		NET_SDK_PTZTrack(m_playHandle[m_currentDivide], PTZ_CMD_TRACK_START);
		m_isTrackStart[m_currentDivide] = TRUE;
	}
	UpdateUI();
}

void CLiveDlg::OnBnClickedButtonCruise()
{
	UpdateData();
	if (m_isCruiseStart[m_currentDivide])
	{
		NET_SDK_PTZCruise(m_playHandle[m_currentDivide], PTZ_CMD_CRUISE_STOP, m_cruiseNo + 1);
		m_isCruiseStart[m_currentDivide] = FALSE;
	}
	else
	{
		BOOL ret = NET_SDK_PTZCruise(m_playHandle[m_currentDivide], PTZ_CMD_CRUISE_RUN, 1);
		m_isCruiseStart[m_currentDivide] = TRUE;
	}
	UpdateUI();
}

void CLiveDlg::OnCbnSelchangeComboPreset()
{
	UpdateData();
	m_presetSel[m_currentDivide] = m_presetNo;
}

void CLiveDlg::OnCbnSelchangeComboCruise()
{
	UpdateData();
	m_cruiseSel[m_currentDivide] = m_cruiseNo;
}

void CLiveDlg::OnCbnSelchangeComboFisheyemode()
{
    UpdateData();
    CComboBox* combo = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_FISHEYEMODE));
    m_szFisheyeMode[m_currentDivide] = (FISHEYE_MODE)combo->GetItemData(m_iFisheyeMode);
    
    NET_SDK_SetFishEyeAdjust(m_playHandle[m_currentDivide], m_szFisheyeMode[m_currentDivide]);
}

BOOL CLiveDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_LBUTTONDOWN)
	{
		for (int i = IDC_BUTTON_PTZ1; i <= IDC_BUTTON_PTZ9; i++)
		{
			if (pMsg->hwnd == GetDlgItem(i)->GetSafeHwnd())
			{
				OnPTZCtrl(i);
				break;
			}
		}

		if (m_bPTZ3DCtrling)
		{
			for (int i = IDC_LIVE_DIVIDE_1; i <= IDC_LIVE_DIVIDE_16; i++)
			{
				if (pMsg->hwnd == GetDlgItem(i)->GetSafeHwnd())
				{
					POINT ptBegain, ptEnd;
					ptBegain.x = pMsg->pt.x;
					ptBegain.y = pMsg->pt.y;
					::ScreenToClient(pMsg->hwnd, &ptBegain);
					m_sPointX = ptBegain.x;
					m_sPointY = ptBegain.y;
					m_bMouseLBDown = TRUE;
					break;
				}
			}
		}
	}
	else if (pMsg->message == WM_LBUTTONUP)
	{
		for (int i = IDC_BUTTON_PTZ1; i <= IDC_BUTTON_PTZ9; i++)
		{
			if (pMsg->hwnd == GetDlgItem(i)->GetSafeHwnd())
			{
				OnPTZCtrlStop(i);
				break;
			}
		}

		if (m_bPTZ3DCtrling)
		{
			for (int i = IDC_LIVE_DIVIDE_1; i <= IDC_LIVE_DIVIDE_16; i++)
			{
				if (pMsg->hwnd == GetDlgItem(i)->GetSafeHwnd())
				{
					m_bMouseLBDown = FALSE;
					POINT ptEnd;
					ptEnd.x = pMsg->pt.x;
					ptEnd.y = pMsg->pt.y;
					::ScreenToClient(pMsg->hwnd, &ptEnd);
					m_ePointX = ptEnd.x;
					m_ePointY = ptEnd.y;

					PTZ_3D_POINT_INFO ptz3DInfo = {0};
					CRect rc;
					GetDlgItem(i)->GetWindowRect(&rc);
					GetDlgItem(i)->ScreenToClient(rc);
					ptz3DInfo.selBeginX = m_sPointX;
					ptz3DInfo.selBeginY = m_sPointY;
					ptz3DInfo.selEndX	= m_ePointX;
					ptz3DInfo.selEndY	= m_ePointY;
					ptz3DInfo.displayWidth = rc.Width();
					ptz3DInfo.displayHeight = rc.Height();
					TRACE("sX=%d, sY=%d, eX=%d, eY=%d, W=%d, H=%d\r\n", ptz3DInfo.selBeginX, ptz3DInfo.selBeginY, ptz3DInfo.selEndX, ptz3DInfo.selEndY, \
						ptz3DInfo.selBeginX-ptz3DInfo.selEndX, ptz3DInfo.selBeginY-ptz3DInfo.selEndY);
					BOOL bRet = NET_SDK_PTZControl_3D(m_playHandle[m_currentDivide], 0, &ptz3DInfo);
					if(!bRet)
						bRet = NET_SDK_PTZControl_3D_Ex(m_userID, m_chnn, &ptz3DInfo);
					break;
				}
			}
		}
	}
	return CDialog::PreTranslateMessage(pMsg);
}

void CLiveDlg::OnPTZCtrl( int id )
{
	long command = 0;
	switch (id)
	{
	case IDC_BUTTON_PTZ1:
		command = PTZ_CMD_LEFT_DOWN;
		break;
	case IDC_BUTTON_PTZ2:
		command = PTZ_CMD_DOWN;
		break;
	case IDC_BUTTON_PTZ3:
		command = PTZ_CMD_RIGHT_DOWN;
		break;
	case IDC_BUTTON_PTZ4:
		command = PTZ_CMD_LEFT;
		break;
	case IDC_BUTTON_PTZ5:
		command = PTZ_CMD_STOP;
		break;
	case IDC_BUTTON_PTZ6:
		command = PTZ_CMD_RIGHT;
		break;
	case IDC_BUTTON_PTZ7:
		command = PTZ_CMD_LEFT_UP;
		break;
	case IDC_BUTTON_PTZ8:
		command = PTZ_CMD_UP;
		break;
	case IDC_BUTTON_PTZ9:
		command = PTZ_CMD_RIGHT_UP;
		break;
	default:
		break;
	}
	//BOOL ret = NET_SDK_PTZControl(m_playHandle[m_currentDivide], command, m_PTZSpeedCtrl.GetPos());
	BOOL ret = NET_SDK_PTZControl_Other(m_userID, m_chnn,command, m_PTZSpeedCtrl.GetPos());
	TRACE("ptz ctrl ret: %d\n", ret);
}

void CLiveDlg::OnPTZCtrlStop( int id )
{
	BOOL ret = NET_SDK_PTZControl(m_playHandle[m_currentDivide], PTZ_CMD_STOP, m_PTZSpeedCtrl.GetPos());
	TRACE("ptz ctrl stop ret: %d\n", ret);
}

void CLiveDlg::OnBnClickedButtonCapture()
{
	/*DD_NETWORK_PLATFORM ts = {0};
	unsigned long retLen = 0;
	BOOL ret1 = NET_SDK_GetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_PLATFORM, -1, &ts, sizeof(ts), &retLen, FALSE);

	ts.Port = 123;
	strcpy(ts.szAddressGB, "10.0.0.100");
	
	ret1 =  NET_SDK_SetDVRConfig(m_userID, DD_CONFIG_ITEM_NETWORK_PLATFORM, -1, &ts, sizeof(ts));
	return;*/

#if 0
	CString fname;
	CFileDialog dlg(FALSE, _T(".jpg"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("bmp files(*.jpg)|*.jpg||"));

	if (dlg.DoModal() == IDOK)
	{
		fname = dlg.GetPathName();
		char *data = new char[3000 * 3000 * 3];
		DWORD size;
		BOOL ret = NET_SDK_CaptureJPEGPicture(m_userID, m_chnn, NULL, data, 3000 *3000 *3, &size);
		if (ret)
		{
			HANDLE h = CreateFile(fname.GetBuffer(), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
			if (h == INVALID_HANDLE_VALUE)
			{
				AfxMessageBox(_T("Faild to Create File！"));
				return ;
			}
			DWORD written;
			WriteFile(h, data, size, &written, NULL);
			CloseHandle(h);
		}
		delete[] data;
	}
#endif

#if 1
	BOOL ret = NET_SDK_RemoteSnap(m_userID, m_chnn);
	if (!ret)
	{
		DWORD dwErrorCode = NET_SDK_GetLastError();
		AfxMessageBox(STR_SNAP_FAILED);
		return;
	}
	else
	{
		AfxMessageBox(STR_SUCCESS);
		return;
	}
#endif

#if 0
	BOOL ret = NET_SDK_StopDVRRecord(m_userID,m_chnn );
	if(ret==FALSE)
		TRACE("NET_SDK_StopDVRRecord error \n");
#endif
#if 0
	BOOL ret = NET_SDK_StartDVRRecord(m_userID,m_chnn,0 );
	if(ret==FALSE)
		TRACE("NET_SDK_StartDVRRecord error \n");
#endif
#if 0
	//DD_ENCODE_CONFIG_N9000_Ex test;
	
	//for (int i=4; i<8; i++)
	{
		
		int ret = 0;
		bool bret = FALSE;
		bret = NET_SDK_GetDVRConfig_SubStreamEncodeInfo_Ex(m_userID, m_chnn, m_ex, 10*sizeof(DD_ENCODE_CONFIG_N9000_Ex), (LPDWORD)&ret, 0);


		
		int a =0;
	}
	//NET_SDK_SetDVRConfig_SubStreamEncodeInfo(m_userID, 18, &ec,  sizeof(DD_ENCODE_CONFIG_N9000));
#endif
	
#if 0
	bool bret = FALSE;
	DWORD len = 0;

	NET_SDK_AVD_CONFIG sAvdConfig;
	memset(&sAvdConfig, 0 , sizeof(sAvdConfig));
	bret = NET_SDK_GetIVMRuleConfig(m_userID, IVM_RULE_AVD_CONFIG, 0, &sAvdConfig, sizeof(sAvdConfig), &len);
	if(!bret)
	{
		TRACE("get IVM_RULE_AVD_CONFIG  error\n");
	}
	sAvdConfig.sceneChangeSwitch = !sAvdConfig.sceneChangeSwitch;
	sAvdConfig.clarityAbnormalSwitch = !sAvdConfig.clarityAbnormalSwitch;
	sAvdConfig.colorAbnormalSwitch = !sAvdConfig.colorAbnormalSwitch;
	sAvdConfig.alarmHoldTime = 20;
	sAvdConfig.sensitivity = 20;

	bret = NET_SDK_SetIVMRuleConfig(m_userID, IVM_RULE_AVD_CONFIG, 0, &sAvdConfig, sizeof(sAvdConfig) );
	if(!bret)
	{
		TRACE("set IVM_RULE_AVD_CONFIG  error\n");
	}

	NET_SDK_VFD_CONFIG sVfdConfig;
	memset(&sVfdConfig, 0 , sizeof(sVfdConfig));
	
	bret = NET_SDK_GetIVMRuleConfig(m_userID, IVM_RULE_VFD_CONFIG, 0, &sVfdConfig, sizeof(sVfdConfig), &len);
	if(!bret)
	{
		TRACE("get IVM_RULE_VFD_CONFIG  error\n");
	}
	sVfdConfig.bSwitch = !sVfdConfig.bSwitch;
	sVfdConfig.rectangle.X1 +=10;
	sVfdConfig.rectangle.Y1 +=10;
	sVfdConfig.rectangle.X2 -=10;
	sVfdConfig.rectangle.Y2 -=10;

	bret = NET_SDK_SetIVMRuleConfig(m_userID, IVM_RULE_VFD_CONFIG, 0, &sVfdConfig, sizeof(sVfdConfig) );
	if(!bret)
	{
		TRACE("set IVM_RULE_VFD_CONFIG  error\n");
	}

	NET_SDK_VFD_TRIGGER_CONFIG sTriggerConfig;
	memset(&sTriggerConfig, 0 , sizeof(sTriggerConfig));
	bret = NET_SDK_GetIVMRuleConfig(m_userID, IVM_RULE_VFD_TRIGGER_CONFIG, 0, &sTriggerConfig, sizeof(sTriggerConfig), &len);
	if(!bret)
	{
		TRACE("get IVM_RULE_VFD_TRIGGER_CONFIG  error\n");
	}
	sTriggerConfig.snapBswitch = !sTriggerConfig.snapBswitch;
	sTriggerConfig.recordBswitch = !sTriggerConfig.recordBswitch;
	sTriggerConfig.alarmOutBswitch = !sTriggerConfig.alarmOutBswitch;

	bret = NET_SDK_SetIVMRuleConfig(m_userID, IVM_RULE_VFD_TRIGGER_CONFIG, 0, &sTriggerConfig, sizeof(sTriggerConfig) );
	if(!bret)
	{
		TRACE("set IVM_RULE_VFD_TRIGGER_CONFIG  error\n");
	}


	NET_SDK_VFD_SCHEDULE_CONFIG sScheduleConfig;
	memset(&sScheduleConfig, 0 , sizeof(sScheduleConfig));
	bret = NET_SDK_GetIVMRuleConfig(m_userID, IVM_RULE_VFD_SCHEDULE_CONFIG, 0, &sScheduleConfig, sizeof(sScheduleConfig), &len);
	if(!bret)
	{
		TRACE("get IVM_RULE_VFD_SCHEDULE_CONFIG  error\n");
	}
	sScheduleConfig.sScheduleDate[sScheduleConfig.dayCount].day = 10;
	sScheduleConfig.sScheduleDate[sScheduleConfig.dayCount].mounth = 2;
	sScheduleConfig.sScheduleDate[sScheduleConfig.dayCount].sScheduleTime.byStartHour = 9;
	sScheduleConfig.sScheduleDate[sScheduleConfig.dayCount].sScheduleTime.byStartMin = 0;
	sScheduleConfig.sScheduleDate[sScheduleConfig.dayCount].sScheduleTime.byStopHour = 18;
	sScheduleConfig.sScheduleDate[sScheduleConfig.dayCount].sScheduleTime.byStopMin = 0;
	sScheduleConfig.dayCount+=1;

	sScheduleConfig.sScheduleWeek[sScheduleConfig.wDayCount].wDay = 6;
	sScheduleConfig.sScheduleWeek[sScheduleConfig.wDayCount].sScheduleTime.byStartHour = 9;
	sScheduleConfig.sScheduleWeek[sScheduleConfig.wDayCount].sScheduleTime.byStartMin = 0;
	sScheduleConfig.sScheduleWeek[sScheduleConfig.wDayCount].sScheduleTime.byStopHour = 18;
	sScheduleConfig.sScheduleWeek[sScheduleConfig.wDayCount].sScheduleTime.byStopMin = 0;
	sScheduleConfig.wDayCount+=1;

	bret = NET_SDK_SetIVMRuleConfig(m_userID, IVM_RULE_VFD_SCHEDULE_CONFIG, 0, &sScheduleConfig, sizeof(sScheduleConfig) );
	if(!bret)
	{
		TRACE("set IVM_RULE_VFD_SCHEDULE_CONFIG  error\n");
	}



	int i = 0;
#endif
#if 0
	static BOOL Subscrib = FALSE;
	static BOOL bret;
	if(!Subscrib)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		bret = NET_SDK_SmartSubscrib(m_userID, NET_DVR_SMART_AVD, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			Subscrib = TRUE;
			memcpy(InServerAddress[m_userID],sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
	}
	else
	{
		int dwResult = 0;
		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_DVR_SMART_AVD, 0, InServerAddress[m_userID], &dwResult);
		if(!bret)
		{
			
			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}
		else
		{
			Subscrib = FALSE;
		}
	}
	

#endif
}

void CLiveDlg::OnBnClickedButtonLivesave()
{
	if (m_isSavingData[m_currentDivide])
	{
		BOOL ret = NET_SDK_StopSaveLiveData(m_playHandle[m_currentDivide]);
		if (ret)
		{
			m_isSavingData[m_currentDivide] = FALSE;
		}
	}
	else
	{
		CFileDialog dlg(FALSE, _T(".avi"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("avi files(*.avi)|*.avi||"));
		if (IDOK == dlg.DoModal())
		{
			
			BOOL ret = NET_SDK_SaveLiveData(m_playHandle[m_currentDivide], dlg.GetPathName().GetBuffer());
			if (ret)
			{
				m_isSavingData[m_currentDivide] = TRUE;
			}
			else
			{
				AfxMessageBox(_T("ERROR!"));
			}
		}
	}
	UpdateUI();
}

#pragma pack(4)
typedef struct _sdk_frame_info
{
	unsigned long		nLength;	//帧数据长度
	unsigned short		frameType;	//参考DD_FRAME_TYPE
	unsigned short		keyFrame;	//0：非关键帧 1：关键帧
	unsigned short		nWidth;		//画面宽, 单位像素, 如果是音频数据则为0
	unsigned short		nHeight;	//画面高, 如果是音频数据则为0
	LONGLONG			nStamp;		//帧时间, 单位毫秒, 从1970年1月1日0点0分0秒开始的绝对时间
}SDK_FRAME_INFO;
#pragma pack()

void CALLBACK CLiveDlg::LiveCallBack( POINTERHANDLE lLiveHandle, NET_SDK_FRAME_INFO frameInfo, BYTE *pBuffer, void *pUser )
{
	CLiveDlg *pThis = reinterpret_cast<CLiveDlg *>(pUser);
	
	DWORD written = 0;

	int curDivide = -1;
	for(int i = 0; i < MAX_DIVIDE_NUM; i++)
	{
		if(pThis->m_playHandle[i] == lLiveHandle)
		{
			curDivide = i;
		}
	}

	if(curDivide < 0)
	{
		//ASSERT(FALSE);
		return;
	}

	if (frameInfo.frameType == DD_FRAME_TYPE_VIDEO_FORMAT)
	{
		TRACE("frameInfo.length = %d, sizeof(BITMAPINFOHEADER) = %d \n", frameInfo.length , sizeof(BITMAPINFOHEADER));
		if(frameInfo.length == sizeof(BITMAPINFOHEADER))
		{
			BITMAPINFOHEADER *bitmap = (BITMAPINFOHEADER *)pBuffer;
			if(bitmap->biCompression == mmioFOURCC('H', '2', '6', '4'))
			{
				TRACE("is H264 \n");
			}
			else if(bitmap->biCompression == mmioFOURCC('H', 'E', 'V', 'C'))
			{
				TRACE("is H265 \n");
			}
			TRACE("biCompression = %d, biWidth =%d, biHeight = %d 264=%d, 265=%d \n", bitmap->biCompression, bitmap->biWidth, bitmap->biHeight, mmioFOURCC('H', '2', '6', '4'), mmioFOURCC('H', 'E', 'V', 'C'));

		}
				
		SDK_FRAME_INFO info;
		info.keyFrame = frameInfo.keyFrame;
		info.nLength = frameInfo.length;
		info.nHeight = frameInfo.height;
		info.nWidth = frameInfo.width;
		info.nStamp = frameInfo.time;
		info.frameType = frameInfo.frameType;
#ifdef _WRITE_USER_FILE_
		if (frameInfo.channel < MAX_DIVIDE_NUM)
		{
			WriteFile(pThis->m_RecFilehandle[frameInfo.channel], &info, sizeof(SDK_FRAME_INFO), &written, NULL);
			WriteFile(pThis->m_RecFilehandle[frameInfo.channel], pBuffer, frameInfo.length, &written, NULL);
		}
		
		
		//TRACE("%02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x \n", pBuffer[0],pBuffer[1],
			//pBuffer[2],pBuffer[3],pBuffer[4],pBuffer[5],pBuffer[6],pBuffer[7],pBuffer[8],pBuffer[9],pBuffer[10],pBuffer[11],pBuffer[12],pBuffer[13],pBuffer[14],pBuffer[15]);
		pThis->m_recNeedFormat[frameInfo.channel] = false;
#endif
	}
	else if (frameInfo.frameType == DD_FRAME_TYPE_VIDEO)
	{
		SDK_FRAME_INFO info;
		info.keyFrame = frameInfo.keyFrame;
		info.nLength = frameInfo.length;
		info.nHeight = frameInfo.height;
		info.nWidth = frameInfo.width;
		info.nStamp = frameInfo.time;
		info.frameType = frameInfo.frameType;
#ifdef _WRITE_USER_FILE_
		if (frameInfo.channel < MAX_DIVIDE_NUM)
		{
			WriteFile(pThis->m_RecFilehandle[frameInfo.channel], &info, sizeof(SDK_FRAME_INFO), &written, NULL);
			WriteFile(pThis->m_RecFilehandle[frameInfo.channel], pBuffer, frameInfo.length, &written, NULL);
		}
		
		if (info.keyFrame)
		{
			//TRACE("key=%d,length=%d %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x %02x%02x%02x%02x \n", info.keyFrame, frameInfo.length, pBuffer[0],pBuffer[1],
			//	pBuffer[2],pBuffer[3],pBuffer[4],pBuffer[5],pBuffer[6],pBuffer[7],pBuffer[8],pBuffer[9],pBuffer[10],pBuffer[11],pBuffer[12],pBuffer[13],pBuffer[14],pBuffer[15]);

		}
#endif
	}
 	else if((frameInfo.frameType == DD_FRAME_TYPE_AUDIO_FORMAT) && (0 == curDivide))
 	{
		int i=0 ;
 		pThis->m_lAudioDecHandle[curDivide] = NET_SDK_InitAudioDecoder(pBuffer, frameInfo.length);
 	}
	else if(frameInfo.frameType == DD_FRAME_TYPE_AUDIO)
	{
		//TRACE("DD_FRAME_TYPE_AUDIO lLiveHandle= %0xld\n", lLiveHandle);
 		if (!pThis->m_pAudioOutBuffer)
 		{
 			pThis->m_pAudioOutBuffer = new unsigned char [3200];
 		}
 		ASSERT(pThis->m_pAudioOutBuffer);
 		int outLen = 0;
 		NET_SDK_DecodeAudioFrame(pThis->m_lAudioDecHandle[curDivide], pBuffer, frameInfo.length, pThis->m_pAudioOutBuffer, &outLen);
 		pThis->m_waveOut.WriteBuf(pThis->m_pAudioOutBuffer, outLen);
	}

}

void CALLBACK CLiveDlg::drawCallBack( POINTERHANDLE lLiveHandle, HDC hDC, void *pUser )
{
	CLiveDlg *pThis = reinterpret_cast<CLiveDlg *>(pUser);

	if (lLiveHandle == pThis->m_playHandle[0])
	{
		//TextOut(hDC, 10, 10, _T("hello world!"), _tcslen(_T("hello world!")));
	}
	for(int i = 0; i < MAX_DIVIDE_NUM; i++)
	{
		if (lLiveHandle == pThis->m_playHandle[i])
		{
			HWND hWnd = pThis->GetDlgItem(IDC_LIVE_DIVIDE_1 + i)->GetSafeHwnd();
			if (pThis->m_bMouseLBDown)
			{
				POINT point; 
				::GetCursorPos(&point);
				::ScreenToClient(hWnd, &point);
				HGDIOBJ hOld = ::SelectObject(hDC, GetStockObject(NULL_BRUSH));
				::Rectangle(hDC, pThis->m_sPointX, pThis->m_sPointY, point.x, point.y);
				::SelectObject(hDC, hOld);
			}

			break;
		}
	}
}

void CLiveDlg::OnBnClickedButtonManualAram()
{
	int index = m_deviceList.GetCurSel();
	LONG lUserID = m_deviceList.GetItemData(index);
	if(lUserID < 0)
	{
		return;
	}
	LONG lManualCount = m_mapDeviceInfo[lUserID]->sensorOutputNum;
	LONG *pChannel = new LONG[lManualCount];
	memset(pChannel, 0x00, sizeof(lManualCount*sizeof(LONG)));
	LONG *pValue = new LONG[lManualCount];
	memset(pValue, 0x00, sizeof(lManualCount*sizeof(LONG)));

	LONG testCh = 0;
	LONG testpValue = 1;
	if (!m_bManualAlarm)
	{
		for (int i = 0; i < lManualCount; i++)
		{
			pChannel[i] = i;
			pValue[i] = 1; //pValue表示是否打开手动报警
		}


		BOOL bResult = NET_SDK_SetDeviceManualAlarm(m_userID, pChannel, pValue, lManualCount, TRUE);
		//BOOL bResult = NET_SDK_SetDeviceManualAlarm(m_userID, &testCh, &testpValue, 1, TRUE);
		
		if(!bResult)
		{
			AfxMessageBox("NET_SDK_SetDeviceAlarm faild");
		}

		GetDlgItem(IDC_BUTTON_Manual_Aram)->SetWindowText("ClearnAlarm");
		m_bManualAlarm = TRUE;
	}
	else
	{
		for (int i = 0; i < lManualCount; i++)
		{
			pChannel[i] = i;
			pValue[i] = 0; //pValue表示是否打开手动报警
		}
		BOOL bResult = NET_SDK_SetDeviceManualAlarm(m_userID, pChannel, pValue, lManualCount, FALSE);
		//BOOL bResult = NET_SDK_SetDeviceManualAlarm(m_userID, &testCh, &testpValue, 1, FALSE);
		if(!bResult)
		{
			AfxMessageBox("NET_SDK_SetDeviceAlarm faild");
		}

		GetDlgItem(IDC_BUTTON_Manual_Aram)->SetWindowText("ManualAlarm");
		m_bManualAlarm = FALSE;
	}

	delete []pChannel;
	delete []pValue;
}

void CLiveDlg::OnBnClickedBtnPtz3dCtrl()
{
	m_bPTZ3DCtrling = !m_bPTZ3DCtrling;
	m_bEnablePTZ3DCtrl = m_bPTZ3DCtrling;
	if (!m_bPTZ3DCtrling)
	{
		GetDlgItem(IDC_BTN_PTZ_3D_CTRL)->SetWindowText("Start");
	}
	else
	{
		GetDlgItem(IDC_BTN_PTZ_3D_CTRL)->SetWindowText("Stop");
	}
}

//#define CAPTURE_JPEG_FILE
void CLiveDlg::OnBnClickedButtonCaptureJpeg()
{
/*	DD_ENCODE_CONFIG_N9000 ec;
	ec.iSize = sizeof(DD_ENCODE_CONFIG_N9000);
	ec.resolution = m_ex[1].resolution;//0x0200;
	ec.rate = m_ex[1].rate;
	ec.encodeType = 2;
	ec.quality = m_ex[1].quality;
	ec.minBitrate = 320;
	ec.maxBitrate = 1024;

	BOOL bret = NET_SDK_SetDVRConfig_SubStreamEncodeInfo(m_userID, m_chnn, &ec,  sizeof(DD_ENCODE_CONFIG_N9000));
*/
	/*DD_PTZ_PRESET_CONFIG_Ex sPreset[128];
	DWORD length = 0;
	BOOL ret = NET_SDK_GetPTZConfig(m_userID, m_chnn, DD_PTZ_CONFIG_PRESET, NULL, 0, sPreset, 128*sizeof(DD_PTZ_PRESET_CONFIG_Ex), &length);
	if (!ret)
	{
	AfxMessageBox("DD_PTZ_CONFIG_PRESET fail");
	}

	DD_CH_CRUISE sCruise[128];
	ret = NET_SDK_GetPTZConfig(m_userID, m_chnn, DD_PTZ_CONFIG_CRUISE, NULL, 0, sCruise, 128*sizeof(DD_CH_CRUISE), &length);
	if (!ret)
	{
	AfxMessageBox("DD_PTZ_CONFIG_CRUISE fail");
	}

	int cruiseIndex = sCruise[1].cruiseIndex;
	DD_CRUISE_POINT_INFO sCruisePoint[128];
	ret = NET_SDK_GetPTZConfig(m_userID, m_chnn, DD_PTZ_CONFIG_CRUISE_POINT, &cruiseIndex, sizeof(int), sCruisePoint, 128*sizeof(DD_CRUISE_POINT_INFO), &length);
	if (!ret)
	{
	AfxMessageBox("DD_PTZ_CONFIG_CRUISE fail");
	}*/

	//NET_SDK_IMAGE_EFFECT_T BrightValue, ContrastValue, SaturationValue, HueValue;
	//BOOL ret = NET_SDK_GetVideoEffect_Ex(m_userID, 0, &BrightValue, &ContrastValue, &SaturationValue, &HueValue);
	//ret = NET_SDK_SetVideoEffect_Ex(m_userID, 0,80, 80, 80, 80);
	/*test*/
#if 0
	unsigned int i = 0;
	//while(TRUE)
	{
		time_t now_time=time(NULL);  
		tm*  lastTm = localtime(&now_time);
		char pathName[512] = {0};
		sprintf(pathName, "D:\\抓图测试\\%d年%d月%d日%d时%d分%d秒.jpg", lastTm->tm_year + 1900,lastTm->tm_mon + 1,lastTm->tm_mday,lastTm->tm_hour,lastTm->tm_min,lastTm->tm_sec);

		DWORD dwRetLen = 0;
		const int nJpgBufLen = 1024*1024*5;
		char *pJpegBuf = new char[nJpgBufLen];
		if (!pJpegBuf)
			return;
		memset(pJpegBuf, 0x00, nJpgBufLen);
		BOOL ret = NET_SDK_CaptureJPEGData_V2(m_userID, m_chnn, pJpegBuf, nJpgBufLen, &dwRetLen);
		if (!ret)
		{
			AfxMessageBox(STR_SNAP_FAILED);
			return;
		}
		FILE *file = fopen(pathName, "wb");
		if (file)
		{
			fwrite(pJpegBuf, sizeof(char), dwRetLen, file);
			fclose(file);
		}

		if(pJpegBuf)
		{
			delete []pJpegBuf;
			pJpegBuf = NULL;
		}
		i++;
		//Sleep(60000);

	}
#endif

	if (m_playHandle[m_currentDivide] != NULL)
	{
		CFileDialog dlg(FALSE, _T("bmp"), _T("sample.bmp"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("bmp files(*.bmp)|*.bmp||"));
		if (IDOK == dlg.DoModal())
		{
			CString pathName = dlg.GetPathName();
			BOOL ret = NET_SDK_CapturePicture(m_playHandle[m_currentDivide], pathName.GetBuffer());
			if (!ret)
			{
				AfxMessageBox(STR_SNAP_FAILED);
			}
		}
		return;
	}
	

	time_t now_time=time(NULL);  
	tm*  lastTm = localtime(&now_time);
	char pathName[512] = {0};
	sprintf(pathName, "./%d_%d_%d_%d_%d_%d.jpg", lastTm->tm_year + 1900,lastTm->tm_mon + 1,lastTm->tm_mday,lastTm->tm_hour,lastTm->tm_min,lastTm->tm_sec);

	DWORD dwRetLen = 0;
	const int nJpgBufLen = 1024*1024*5;
	char *pJpegBuf = new char[nJpgBufLen];
	if (!pJpegBuf)
		return;
	memset(pJpegBuf, 0x00, nJpgBufLen);
	long lResolution = 0;
	BOOL ret = NET_SDK_CaptureJpeg(m_userID,m_chnn, lResolution,pJpegBuf, nJpgBufLen, &dwRetLen);
	if (!ret)
	{
		AfxMessageBox(STR_SNAP_FAILED);
		return;
	}
	FILE *file = fopen(pathName, "wb");
	if (file)
	{
		fwrite(pJpegBuf, sizeof(char), dwRetLen, file);
		fclose(file);
	}

	if(pJpegBuf)
	{
		delete []pJpegBuf;
		pJpegBuf = NULL;
	}
	/*******/
	/*CFileDialog dlg(FALSE, _T("jpg"), _T("capture.jpg"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("JPEG files(*.jpg)|*.jpg||"));
	if (IDOK == dlg.DoModal())
	{
	CString pathName = dlg.GetPathName();
	#ifdef CAPTURE_JPEG_FILE
	BOOL ret = NET_SDK_CaptureJPEGFile_V2(m_userID, m_chnn, pathName.GetBuffer(0));
	if (!ret)
	{
	AfxMessageBox(STR_SNAP_FAILED);
	return;
	}
	#else
	DWORD dwRetLen = 0;
	const int nJpgBufLen = 1024*1024*5;
	char *pJpegBuf = new char[nJpgBufLen];
	if (!pJpegBuf)
	return;
	memset(pJpegBuf, 0x00, nJpgBufLen);
	BOOL ret = NET_SDK_CaptureJPEGData_V2(m_userID, m_chnn, pJpegBuf, nJpgBufLen, &dwRetLen);
	if (!ret)
	{
	AfxMessageBox(STR_SNAP_FAILED);
	return;
	}
	FILE *file = fopen(pathName.GetBuffer(0), "wb");
	if (file)
	{
	fwrite(pJpegBuf, sizeof(char), dwRetLen, file);
	fclose(file);
	}

	if(pJpegBuf)
	{
	delete []pJpegBuf;
	pJpegBuf = NULL;
	}
	#endif
	}*/

}

void CLiveDlg::OnBnClickedBtnMakeiframe()
{
	// TODO: Add your control notification handler code here
	UpdateData();

	if (m_isStartLive[m_currentDivide])
	{
		CComboBox* pStreamTypeCtrl = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_StreamType));
		if (pStreamTypeCtrl)
		{
			if (0 == pStreamTypeCtrl->GetCurSel())
			{
				/*CStringA strOutput;
				strOutput.Format("\r\n[[[NET_SDK_MakeKeyFrame]]]\r\n");
				::OutputDebugStringA(strOutput.GetBuffer());*/

				NET_SDK_MakeKeyFrame(m_userID, 0);
			}
			else
			{
				/*CStringA strOutput;
				strOutput.Format("\r\n[[[NET_SDK_MakeKeyFrameSub]]]\r\n");
				::OutputDebugStringA(strOutput.GetBuffer());*/

				NET_SDK_MakeKeyFrameSub(m_userID, 0);
			}
		}
	}
}

void CLiveDlg::OnBnClickedButtonAutoScan2()
{
	// TODO: Add your control notification handler code here
	if (m_isOpenAutoScan[m_currentDivide])
	{
		NET_SDK_PTZAutoScan(m_playHandle[m_currentDivide], PTZ_CMD_AUTO_SCAN_STOP, 0);
		m_isOpenAutoScan[m_currentDivide] = FALSE;
	}
	else
	{
		NET_SDK_PTZAutoScan(m_playHandle[m_currentDivide], PTZ_CMD_AUTO_SCAN_START, m_PTZSpeedCtrl.GetPos());
		m_isOpenAutoScan[m_currentDivide] = TRUE;
	}
	m_isOpenRandomScan[m_currentDivide] = FALSE;
	UpdateUI();
}

// 回调的数据格式是 YUV420
void CALLBACK CLiveDlg::yuvCallBack( POINTERHANDLE lLiveHandle, DECODE_FRAME_INFO frameInfo, void *pUser )
{
	//FILE *pFile = fopen("./test.yuv", "wb");
	//int len = 0;
	//len = fwrite(frameInfo.pData, frameInfo.dwLen, 1, pFile);
	//fclose( pFile);
}


void CLiveDlg::OnCbnSelchangeComboStreamtype()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	GetChnnInfo(m_chnn);

	if (m_isStartLive[m_currentDivide])
	{
		StopLive();
		StartLive();
	}
}


void CLiveDlg::OnBnClickedButtonDrawrect()
{
    // TODO: 在此添加控件通知处理程序代码
    m_bDrawRect = !m_bDrawRect;
    NET_SDK_EnableShowVFD(m_bDrawRect);
}
