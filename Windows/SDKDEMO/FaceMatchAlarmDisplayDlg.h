#pragma once
#include "DVR_NET_SDK.h"
#include "SDKDEMO.h"
#include "MsgBase.h"
// CFaceMatchAlarmDisplayDlg 对话框

class CFaceMatchAlarmDisplayDlg : public CDialog,public CMsgBase
{
	DECLARE_DYNAMIC(CFaceMatchAlarmDisplayDlg)

public:
	CFaceMatchAlarmDisplayDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFaceMatchAlarmDisplayDlg();

// 对话框数据
	enum { IDD = IDD_FACE_MATCH_ALARM_DISPLAY_DIALOG };

protected:
	NET_SDK_DEVICEINFO	*m_deviceinfo;
	LONG				m_userID;
	BOOL				m_support;//是否支持人脸
	BOOL				m_SubscribVFD;
	BOOL				m_SubscribAVD;
	BOOL				m_SubscribFaceMatch;
	BOOL				m_SubscribPEA;
	BOOL				m_SubscribOSC;
	BOOL				m_SubscribVehicle;
	BOOL				m_SubscribAOIENTRY;
	BOOL				m_SubscribAOILEAVE;
	BOOL				m_SubscribPassLine;
	BOOL				m_SubscribNVRPassLine;
	BOOL				m_SubscribTraffic;
	char				m_serverAddressAVD[256];
	char				m_serverAddressVFD[256];
	char				m_serverAddressFaceMatch[256];
	char				m_serverAddressFaceMatchFailed[256];
	char				m_serverAddressPassLine[256];
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	virtual void UpdateMsg(LONG ID, LONG lCommand, void *data, int length);
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
public:
	void setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid);
	BOOL IsSupportFace();
private:
	void ShowPicture(int id,LPCTSTR path);
	char* U2G(const char* utf8);
public:
	afx_msg void OnBnClickedButtonSmartSbscribAvd();
	afx_msg void OnBnClickedButtonSmartSbscribVfd();
	afx_msg void OnBnClickedButtonSmartSbscribVfdMatch();
	afx_msg void OnBnClickedButtonSmartSbscribOsc();
	afx_msg void OnBnClickedButtonSmartSbscribPea();
	afx_msg void OnBnClickedButtonSmartSbscribVihicle();
	afx_msg void OnBnClickedButtonSmartSbscribAoientry();
	afx_msg void OnBnClickedButtonSmartSbscribAoileave();
	afx_msg void OnBnClickedButtonSmartSbscribPassline();
	afx_msg void OnBnClickedButtonSubscribeNvrpassline();
	afx_msg void OnBnClickedButtonUnsubscribeNvrpassline();
	afx_msg void OnBnClickedButtonSmartSbscribTraffic();
};
