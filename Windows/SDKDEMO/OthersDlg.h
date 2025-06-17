#pragma once
#include "afxcmn.h"
#include "afxdtctl.h"
#include "gridctrl.h"
#include "resource.h"
#include "DVR_NET_SDK.h"
#include "afxwin.h"
#include "LiveDlg.h"

// COthersDlg 对话框

class COthersDlg : public CDialog
{
	DECLARE_DYNAMIC(COthersDlg)

public:
	COthersDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~COthersDlg();

// 对话框数据
	enum { IDD = IDD_OTHERS_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	NET_SDK_DEVICEINFO		*m_devieInfo;
	LONG					m_userID;


	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	void setDeviceInfo( NET_SDK_DEVICEINFO *di, LONG userid );
	afx_msg void OnBnClickedButtonWatermasksetting();
	bool QueryChlWatermark(int chlId);
	void getChlGUIDInfo();
	GUID GetGuidFromString(const char *pGUIDStr);
	CComboBox m_comWatermaskOn;

	//map<int, N9000_GUID_CHANNEL_TYPE>				m_ChannelGUIDInfo; 
};
#pragma once

