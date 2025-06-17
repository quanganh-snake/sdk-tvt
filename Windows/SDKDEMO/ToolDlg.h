#pragma once
#include "afxcmn.h"
#include "DVR_NET_SDK.h"
#include "afxdtctl.h"
#include "gridctrl.h"
#include <map>
#include <string>
#include "MyWaveOut.h"
#include "WaveIn.h"
#include "afxwin.h"
#define N9000_CONFIG
// CToolDlg 对话框
using namespace std;
class CToolDlg : public CDialog
{
	DECLARE_DYNAMIC(CToolDlg)

public:
	CToolDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CToolDlg();

// 对话框数据
	enum { IDD = IDD_TOOL_DIALOG };

protected:
	CListCtrl			m_logList;
	CTime				m_logStartTime;
	CTime				m_logEndTime;
	CTime				m_sysTime;
	BOOL				m_isStartVoiceComm;
	BOOL				m_isOpenMsgCallback;
	BOOL				m_isStartVoiceComm_MR;
	BOOL				m_runTalkTransThread;
	HANDLE				m_talkTransThread;
	HANDLE				m_talkDataFile;
	HANDLE				m_alarmStatusThread;
	LONG				m_voiceHandle;
	LONG				m_talkTransHandle;
	LONG				m_audioDecoderHandle;
	LONG				m_msgCBHandle;
	NET_SDK_DEVICEINFO	*m_deviceinfo;
	LONG				m_userID;
	int					m_voiceVolume;
	BOOL				m_SubscribVFD;
	BOOL				m_SubscribAVD;
	BOOL				m_SubscribFaceMatch;
	BOOL				m_SubscribPEA;
	BOOL				m_SubscribOSC;
	BOOL				m_SubscribVehicle;
	BOOL				m_SubscribAOIENTRY;
	BOOL				m_SubscribAOILEAVE;
	BOOL				m_SubscribPassLine;
	char				m_serverAddressAVD[256];
	char				m_serverAddressVFD[256];
	char				m_serverAddressFaceMatch[256];
	char				m_serverAddressFaceMatchFailed[256];
	char				m_serverAddressPassLine[256];

	CDateTimeCtrl		m_startTimeCtrl;
	CDateTimeCtrl		m_endTimeCtrl;
	CDateTimeCtrl		m_systimeCtrl;
	CGridCtrl			m_diskGrid;
	CList<NET_SDK_DISK_INFO> m_diskList;
	CWnd*				m_mainDlg;
	CMyWaveOut	m_waveOut;
	CWaveIn					m_WaveIn;


	std::map<int, string>	m_strMinorLogTypeList;          //n9000用
	void UpdateUI();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	static void TalkTransWaveInCallBack(unsigned char *pWaveInDataBuffer, unsigned int dwBufSize, void* pUser);
	bool searched;
	unsigned int m_nSearchDevice;
public:
	static void CALLBACK TalkTransCallBack(POINTERHANDLE lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, void* pUser);
	

	static DWORD WINAPI TalkTransSendThread(LPVOID);
	void TalkTransSendProc();

	void setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid);

	void InitN9000LogTypeList();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonSearchlog();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonVoiceComm();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedButtonChangesystime();
	afx_msg void OnBnClickedButtonDiskRefresh();
	afx_msg void OnBnClickedButtonChangeDiskProperty();
	afx_msg void OnBnClickedButtonDiskFormat();
	afx_msg void OnBnClickedButtonMsgCallback();
	void setMainDlg( CWnd* dlg );
	afx_msg void OnBnClickedButtonTalktRans();
	afx_msg void OnBnClickedButtonGetsystime();
	afx_msg void OnBnClickedBtnreboot();
	afx_msg void OnBnClickedButtonSmartSubscribAVD();
	afx_msg void OnBnClickedButtonSmartSubscribVFD();
	afx_msg void OnBnClickedButtonSmartSubscribFaceMatch();
	static DWORD WINAPI AlarmStatusThread( LPVOID params );
	void AlarmStatusProc();
	afx_msg void OnBnClickedButtonSmartSbscribOsc();
	afx_msg void OnBnClickedButtonSmartSbscribPea();
	afx_msg void OnBnClickedButtonSmartSbscribVihicle();
	afx_msg void OnBnClickedButtonSmartSbscribAoientry();
	afx_msg void OnBnClickedButtonSmartSbscribAoileave();
	afx_msg void OnBnClickedButtonSmartSbscribPassline();
	afx_msg void OnBnClickedButtonSearchDevice();
	afx_msg void OnBnClickedButtonSetLog();
	CListCtrl m_listSearchDevice;
	afx_msg void OnBnClickedButtonSearchActivedev();
	CString ConvertDeviceType(int iDevType);
	CString m_strPwd;
	CEdit m_editActivePwd;
	afx_msg void OnBnClickedButtonBrowseLogpath();
	afx_msg void OnBnClickedCheckTalkToChannel();
	CComboBox m_comChannel;
};
