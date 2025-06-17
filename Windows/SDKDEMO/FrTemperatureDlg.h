#pragma once
#include "afxcmn.h"
#include "DVR_NET_SDK.h"
#include "afxdtctl.h"
#include "gridctrl.h"
#include <map>
#include <string>
#include "XMLParse.h"
#include "MsgBase.h"
#define N9000_CONFIG
// CToolDlg 对话框
using namespace std;

class CFrTemperatureDlg  : public CDialog ,public CMsgBase
{
	DECLARE_DYNAMIC(CFrTemperatureDlg)

public:
	CFrTemperatureDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFrTemperatureDlg();
	

// 对话框数据
	enum { IDD = IDD_TEMPERATURE_DIALOG };
public:
	afx_msg void OnBnClickeGetTemperature();
	afx_msg void OnBnClickeSetTemperature();
	afx_msg void OnBnClickeGetMaskConfig();
	afx_msg void OnBnClickeSetMaskConfig();
	afx_msg void OnBnClickeGetThermalConfig();
	afx_msg void OnBnClickeSetThermalConfig();
	afx_msg void OnBnClickeGetPasslineConfig();
	afx_msg void OnBnClickeSetPasslineConfig();
	afx_msg void OnBnClickedButtonpasslineReset();
	
	afx_msg void OnBnClickeChangeUnit();
protected:
	
	NET_SDK_DEVICEINFO	*m_deviceinfo;
	LONG				m_userID;
	CWnd*				m_mainDlg;
	CXMLParse		m_temperatureXmlParse;
	CXMLParse		m_maskXmlParse;
	CXMLParse		m_thermalParse;
	CXMLParse		m_passLineParse;
	


	void UpdateUI();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void test();
	DECLARE_MESSAGE_MAP()
public:
	
	void setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	
	
	virtual BOOL OnInitDialog();
	virtual void UpdateMsg(LONG ID, LONG lCommand, void *data, int length);
	
	void setMainDlg( CWnd* dlg );
	
	CProgressCtrl m_ProgressExport;
private:
	NET_SDK_DEV_SUPPORT m_DevSupport;
	
public:
	
	afx_msg void OnBnClickedButtongetpasslinecount();
};
