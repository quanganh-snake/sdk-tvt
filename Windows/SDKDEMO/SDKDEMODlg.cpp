// SDKDEMODlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "SDKDEMODlg.h"
#include "LoginDlg.h"
#include "LiveDlg.h"
#include "SearchDlg.h"

#include "FaceMatchDlg.h"
#include "ToolDlg.h"
#include "ConfigDlg.h"
#include "MsgDlg.h"
#include "SmartDlg.h"
#include "FrTemperatureDlg.h"
#include "FaceMatchAlarmDisplayDlg.h"
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
#include "IPTool_SearchAndNotifySDK.h"
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "strdef.h"


// CSDKDEMODlg 对话框
//#define test_ipv6
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
void WINAPI SearchCallBack(char* hwaddr, int opt, const char* szXmlData, void *pParam)
{
	TRACE("hwaddr = %s, opt = %d, szXmlData=%s \n", hwaddr, opt, szXmlData);
	return ;
}
#endif
CSDKDEMODlg::CSDKDEMODlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSDKDEMODlg::IDD, pParent)
	, m_userID(-1)
	, m_liveDlg(NULL)
	, m_bankSignedDlg(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_liveDlg = new CLiveDlg;
	m_searchDlg = new CSearchDlg;
	m_toolDlg = new CToolDlg;
	m_configDlg = new CConfigDlg;
	m_msgDlg = new CMsgDlg;
	m_faceMatchDlg = new CFaceMatchlDlg;
	m_frTemperatureDlg = new CFrTemperatureDlg;
	m_smartDlg = new CSmartDlg;
	m_faceMatchAlarmDisplayDlg = new CFaceMatchAlarmDisplayDlg;
	m_othersDlg = new COthersDlg;
	ZeroMemory(&m_deviceInfo, sizeof(NET_SDK_DEVICEINFO));
}

CSDKDEMODlg::~CSDKDEMODlg()
{
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
	IPTOOL_SEARCH_AND_NOTIFY_Stop_Search();
#endif
	if (m_liveDlg)
	{
		delete m_liveDlg;
		m_liveDlg = NULL;
	}
	if (m_toolDlg)
	{
		delete m_toolDlg;
		m_toolDlg = NULL;
	}
	if (m_configDlg)
	{
		delete m_configDlg;
		m_configDlg = NULL;
	}
	if (m_searchDlg)
	{
		delete m_searchDlg;
		m_searchDlg = NULL;
	}

	if (m_msgDlg)
	{
		delete m_msgDlg;
		m_msgDlg = NULL;
	}
	if (m_faceMatchDlg)
	{
		delete m_faceMatchDlg;
		m_faceMatchDlg = NULL;
	}
	if (m_frTemperatureDlg)
	{
		delete m_frTemperatureDlg;
		m_frTemperatureDlg = NULL;
	}
	
	if(m_smartDlg)
	{
		delete m_smartDlg;
		m_smartDlg = NULL;
	}
	if(m_faceMatchAlarmDisplayDlg)
	{
		delete m_faceMatchAlarmDisplayDlg;
		m_faceMatchAlarmDisplayDlg = NULL;
	}
	if(m_othersDlg)
	{
		delete m_othersDlg;
		m_othersDlg = NULL;
	}
}


void CSDKDEMODlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MAIN_TAB, m_tab);
}

BEGIN_MESSAGE_MAP(CSDKDEMODlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CSDKDEMODlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSDKDEMODlg::OnBnClickedCancel)
	ON_WM_DESTROY()
	ON_WM_MOVE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_MAIN_TAB, &CSDKDEMODlg::OnTcnSelchangeMainTab)
	ON_MESSAGE(WM_SHOW_MSG_DLG, &CSDKDEMODlg::OnShowMsgDlg)
	ON_BN_CLICKED(IDC_BUTTON_LogTest, &CSDKDEMODlg::OnBnClickedButtonLogtest)
END_MESSAGE_MAP()


// CSDKDEMODlg 消息处理程序

BOOL CSDKDEMODlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_tab.InsertItem(0, STR_LIVE);
	m_tab.InsertItem(1, STR_SEARCH_BACKUP);
	m_tab.InsertItem(2, STR_CONFIG);
	m_tab.InsertItem(3, STR_TOOL);
	m_tab.InsertItem(4, STR_FACE_MATCH);
	m_tab.InsertItem(5, STR_SMART);
	m_tab.InsertItem(6, STR_FR_TEMPERATURE);
	m_tab.InsertItem(7, STR_FACE_MATCH_ALARM_DISPLAY);
	//m_tab.InsertItem(8, STR_OTHERS);
	CRect tabRect;
	m_tab.GetClientRect(&tabRect);
	tabRect.left += 2;
	tabRect.top += 20;
	tabRect.bottom -= 3;
	tabRect.right -= 3;

	
	m_liveDlg->Create(IDD_LIVE_DIALOG, &m_tab);
	m_liveDlg->MoveWindow(&tabRect);
	
	m_searchDlg->Create(IDD_SEARCH_DIALOG, &m_tab);
	m_searchDlg->MoveWindow(&tabRect);

	m_toolDlg->Create(IDD_TOOL_DIALOG, &m_tab);
	m_toolDlg->MoveWindow(&tabRect);
	m_toolDlg->setMainDlg(this);

	m_configDlg->Create(IDD_CONFIG_DIALOG, &m_tab);
	m_configDlg->MoveWindow(&tabRect);

	m_faceMatchDlg->Create(IDD_FACE_MATCH_DIALOG, &m_tab);
	m_faceMatchDlg->MoveWindow(&tabRect);

	m_frTemperatureDlg->Create(IDD_TEMPERATURE_DIALOG, &m_tab);
	m_frTemperatureDlg->MoveWindow(&tabRect);
	
	m_smartDlg->Create(IDD_SMART_DIALOG, &m_tab);
	m_smartDlg->MoveWindow(&tabRect);

	m_faceMatchAlarmDisplayDlg->Create(IDD_FACE_MATCH_ALARM_DISPLAY_DIALOG,&m_tab);
	m_faceMatchAlarmDisplayDlg->MoveWindow(&tabRect);

	//m_othersDlg->Create(IDD_OTHERS_DIALOG, &m_tab);
	//m_othersDlg->MoveWindow(&tabRect);

	m_msgDlg->Create(IDD_MSG_DIALOG, NULL);
	m_msgDlg->ShowWindow(FALSE);

	

	m_CurDlg = NULL;
	NET_SDK_DEVICE_IP_INFO pDeviceIPInfo;
	memset(&pDeviceIPInfo, 0, sizeof(NET_SDK_DEVICE_IP_INFO));
	strcpy(pDeviceIPInfo.szMac, "00:18:AE:4F:41:30");	//设备当前mac地址
	strcpy(pDeviceIPInfo.szIpAddr, "192.168.52.189");		
	strcpy(pDeviceIPInfo.szMark, "255.255.255.0");		
	strcpy(pDeviceIPInfo.szGateway, "192.168.52.1");			
    strcpy(pDeviceIPInfo.szDdns1, "192.168.52.1");
    strcpy(pDeviceIPInfo.szDdns2, "8.8.8.8");
	strcpy(pDeviceIPInfo.szPassword, "12345678");				//admin用户的密码
	pDeviceIPInfo.ucIPMode = 0;
	//bool bRet = NET_SDK_ModifyDeviceNetInfo(&pDeviceIPInfo);

	NET_SDK_SetSDKMessageCallBack(0, 0, excertionCallback, NULL);
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
	IPTOOL_SEARCH_AND_NOTIFY_Start_Search(SearchCallBack, this);
#endif
	if(m_userID == -1)//主动登录方式，未登录
	{
		//登录
		CLoginDlg dlg;	
		dlg.SetIp(m_IP);
		dlg.SetPort(m_port);

		while(true)
		{
			if (IDOK == dlg.DoModal())
			{
#ifdef test_ipv6
				LONG userId = NET_SDK_LoginEx("[fe80::14eb:95ff:fe40:c812]"/*dlg.GetIP().GetBuffer()*/, dlg.GetPort(), dlg.GetUserName().GetBuffer(), dlg.GetPassword().GetBuffer(), &m_deviceInfo,\
					dlg.GetConnectType(), dlg.GetDeviceSN().GetBuffer());
#else
				LONG userId = NET_SDK_LoginEx(dlg.GetIP().GetBuffer(), dlg.GetPort(), dlg.GetUserName().GetBuffer(), dlg.GetPassword().GetBuffer(), &m_deviceInfo,\
					dlg.GetConnectType(), dlg.GetDeviceSN().GetBuffer());
#endif
				if (userId >= 0)
				{
					m_IP = dlg.GetIP().GetBuffer();
					m_userID = userId;
					dlg.SaveConfig();
					break;
				}
				else
				{
					LONG error = NET_SDK_GetLastError();
					CString msg = NET_SDK_GetErrorMsg(&error);
					if (error == NET_SDK_DEVICE_OFFLINE)
					{
						AfxMessageBox("Device offline!");
					}
					else
					{
						AfxMessageBox(STR_LOGIN_FAILED);
					}
				}
			}
			else
			{
				OnCancel();
				return FALSE;
			}
		}
		m_deviceInfo.deviceIP = inet_addr(m_IP);
	}
	else	//被动登录方式
	{
		m_deviceInfo.deviceIP = m_deviceInfo.deviceIP;;
	}

	//m_deviceInfo.deviceIP = inet_addr(m_IP);
	m_liveDlg->SetDeviceInfo(&m_deviceInfo, m_userID);
	m_liveDlg->ShowWindow(SW_SHOW);
	m_searchDlg->SetDeviceInfo(&m_deviceInfo, m_userID);
	m_toolDlg->setDeviceInfo(&m_deviceInfo, m_userID);
	m_configDlg->setDeviceInfo(&m_deviceInfo, m_userID);
	m_faceMatchDlg->setDeviceInfo(&m_deviceInfo, m_userID);
	m_smartDlg->setDeviceInfo(&m_deviceInfo, m_userID);
	m_frTemperatureDlg->setDeviceInfo(&m_deviceInfo, m_userID);
	m_faceMatchAlarmDisplayDlg->setDeviceInfo(&m_deviceInfo, m_userID);
	//m_othersDlg->setDeviceInfo(&m_deviceInfo, m_userID);
	NET_SDK_CAMERA_TYPE pCameraType;
	NET_SDK_GetPTZCameraType(m_userID, &pCameraType);

	NET_SDK_DEVICE_FIRMWARE_TYPE pFirmware;
	NET_SDK_GetDeviceFirmwareType(m_userID, &pFirmware);

	//NET_SDK_SetSDKMessageCallBack(0, 0, excertionCallback, NULL);
	//NET_SDK_SetDVRMessageCallBack(msgCallback, this);
	NET_SDK_SetDVRMessageCallBackEx(msgCallbackEx, this);
	NET_SDK_SetSubscribCallBack(SubscribCallBack, this);
	m_ipMap[m_userID] = m_IP;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSDKDEMODlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CSDKDEMODlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSDKDEMODlg::OnBnClickedOk()
{
	
}

void CSDKDEMODlg::OnBnClickedCancel()
{
	//OnCancel();
	if (IDYES == AfxMessageBox(STR_CONFIRM_QUIT, MB_YESNO))
	{
		if (m_userID >= 0)
		{
			m_liveDlg->ShowWindow(SW_HIDE);
			NET_SDK_Logout(m_userID);
			m_userID = -1;
		}
		OnCancel();
	}
}


void CSDKDEMODlg::OnDestroy()
{
	CDialog::OnDestroy();
}

void CSDKDEMODlg::OnMove(int x, int y)
{
	CDialog::OnMove(x, y);
	
	if (!IsWindow(m_tab.GetSafeHwnd()))
	{
		return ;
	}
	if (m_tab.GetCurSel() == 0)
	{
		m_liveDlg->OnMove(x, y);
	}
}

void CSDKDEMODlg::OnTcnSelchangeMainTab(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_CurDlg = NULL;
	int sel = m_tab.GetCurSel();
	if (m_liveDlg->IsWindowVisible())
	{
		m_liveDlg->ShowWindow(SW_HIDE);
	}
	
	if (m_searchDlg->IsWindowVisible())
	{
		m_searchDlg->ShowWindow(SW_HIDE);
	}

	if (m_toolDlg->IsWindowVisible())
	{
		m_toolDlg->ShowWindow(SW_HIDE);
	}
	
	if (m_configDlg->IsWindowVisible())
	{
		m_configDlg->ShowWindow(SW_HIDE);
	}

	if (m_faceMatchDlg->IsWindowVisible())
	{
		m_faceMatchDlg->ShowWindow(SW_HIDE);
	}
	if(m_smartDlg->IsWindowVisible())
	{
		m_smartDlg->ShowWindow(SW_HIDE);
	}
	if(m_frTemperatureDlg->IsWindowVisible())
	{
		m_frTemperatureDlg->ShowWindow(SW_HIDE);
	}
	if(m_faceMatchAlarmDisplayDlg->IsWindowVisible())
	{
		m_faceMatchAlarmDisplayDlg->ShowWindow(SW_HIDE);
	}
	/*if (m_othersDlg->IsWindowVisible())
	{
		m_othersDlg->ShowWindow(SW_HIDE);
	}*/
	switch (sel)
	{
	case 0:
		{
			m_liveDlg->ShowWindow(SW_SHOW);
			break;
		}
	case 1:
		{
			m_searchDlg->ShowWindow(SW_SHOW);
			break;
		}
	case 2:
		{
			m_configDlg->ShowWindow(SW_SHOW);
			break;
		}
	case 3:
		{
			m_toolDlg->ShowWindow(SW_SHOW);
			break;
		}
	case 4:
		{
			if(m_faceMatchDlg->IsSupportFace())
			{
				m_faceMatchDlg->ShowWindow(SW_SHOW);
			}			
			break;
		}
	case 5:
		{
			if(m_smartDlg->IsSupportFace())
			{
				m_smartDlg->ShowWindow(SW_SHOW);
			}
			break;			
		}
	case 6:
		{
			m_frTemperatureDlg->ShowWindow(SW_SHOW);
			m_CurDlg = (CMsgBase*)m_frTemperatureDlg;
			break;			
		}
	case 7:
		{
			if(m_faceMatchAlarmDisplayDlg->IsSupportFace())
			{
				m_faceMatchAlarmDisplayDlg->ShowWindow(SW_SHOW);
				m_CurDlg = (CMsgBase*)m_faceMatchAlarmDisplayDlg;
			}			
			break;		
		}
	case 8:
		{
			/*m_othersDlg->ShowWindow(SW_SHOW);
			m_CurDlg = (CMsgBase*)m_othersDlg;
			break;	*/		
		}
	default:break;
	}
	*pResult = 0;
}

void CALLBACK CSDKDEMODlg::excertionCallback( DWORD dwType, LONG lUserID, LONG lHandle, void *pUser )
{
	/*CString temp;
	temp.Format(_T("type:%d, userID:%d, handle:%d"), dwType, lUserID, lHandle);
	AfxMessageBox(temp);*/
	if (NETWORK_DISCONNECT == dwType)
	{
		CString temp;
		temp.Format(_T("\r\ntype:%d, userID:%d, handle:%d---NETWORK_DISCONNECT\r\n"), dwType, lUserID, lHandle);
		::OutputDebugString(temp);
	}
	else if (NETWORK_RECONNECT == dwType)
	{
		CString temp;
		temp.Format(_T("\r\ntype:%d, userID:%d, handle:%d---NETWORK_RECONNECT\r\n"), dwType, lUserID, lHandle);
		::OutputDebugString(temp);
	}
	else if (NETWORK_CH_DISCONNECT == dwType)
	{
		CString temp;
		temp.Format(_T("\r\ntype:%d, userID:%d, channel:%d---NETWORK_CH_DISCONNECT\r\n"), dwType, lUserID, lHandle);
		::OutputDebugString(temp);
	}
	else if (NETWORK_CH_RECONNECT == dwType)
	{
		CString temp;
		temp.Format(_T("\r\ntype:%d, userID:%d, channel :%d---NETWORK_CH_RECONNECT\r\n"), dwType, lUserID, lHandle);
		::OutputDebugString(temp);
	}
}

BOOL CALLBACK CSDKDEMODlg::msgCallback( LONG lCommand, LONG lUserID, char *pBuf, DWORD dwBufLen, void *pUser )
{
	CSDKDEMODlg *pThis = (CSDKDEMODlg *)pUser;
	if (lCommand == NET_SDK_ALARM)
	{
		int num = dwBufLen / sizeof(NET_SDK_ALARMINFO);
		NET_SDK_ALARMINFO *ai = reinterpret_cast<NET_SDK_ALARMINFO *>(pBuf);

		for (int i = 0; i < num; i++)
		{
			CString temp;
			CString ttemp;
			CString ttype;
			switch(ai[i].dwAlarmType)
			{
			case NET_SDK_N9000_ALARM_TYPE_MOTION:
				ttype = STR_MOTION;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_SENSOR:
				ttype = STR_SENSOR;
				ttemp.Format(STR_SENSOR_D, ai[i].dwSensorIn, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_VLOSS:
				ttype = STR_VIDEO_LOSS;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_SIGNAL_SHELTER:
				ttype = STR_SHELTER;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_DISK_FULL:
				ttype = STR_DISK_FULL;
				ttemp.Format(STR_DISK_NO_D, ai[i].dwDisk);
				break;
				/*case NET_SDK_ALARM_TYPE_DISK_UNFORMATTED:
				ttype = STR_DISK_UNFORMAT;
				ttemp.Format(STR_DISK_NO_D, ai[i].dwDisk);
				break;
				case NET_SDK_ALARM_TYPE_DISK_WRITE_FAIL:
				ttype = STR_DISK_READ_WRITE_ERROR;
				ttemp.Format(STR_DISK_NO_D, ai[i].dwDisk);
				break;*/
			}
			temp.Format(STR_MSG_FORMAT, pThis->m_ipMap[lUserID], ttype, ttemp);
			pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp.GetBuffer());
			temp.ReleaseBuffer();
		}
	}
	else if (lCommand == NET_SDK_RECORD)
	{
		int num = dwBufLen / sizeof(NET_SDK_RECORD_STATUS);
		NET_SDK_RECORD_STATUS *rs = reinterpret_cast<NET_SDK_RECORD_STATUS *>(pBuf);
		
		for (int i = 0; i < num; i++)
		{
			CString temp;
#if 0
			CString ttype;
			switch(rs[i].dwRecordType)
			{
			case DD_RECORD_TYPE_MANUAL:
				ttype = STR_MANUAL_RECORD;
				break;
			case DD_RECORD_TYPE_SCHEDULE:
				ttype = STR_SCHEDULE_RECORD;
				break;
			case DD_RECORD_TYPE_MOTION:
				ttype = STR_MOTION_RECORD;
				break;
			case DD_RECORD_TYPE_SENSOR:
				ttype = STR_SENSOR_RECORD;
				break;
			case DD_RECORD_TYPE_BEHAVIOR:
				ttype = STR_BEHIVOR_RECORD;
				break;
			case DD_RECORD_TYPE_NONE:
				ttype = STR_NONE_RECORD;
				break;
			}
			temp.Format(STR_RECORD_MSG_FORMAT, pThis->m_ipMap[lUserID], rs->dwChannel, ttype);
#endif
			CString ttype = "";
			if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_MANUAL))
			{
				if (ttype.IsEmpty())
				{
					ttype = STR_MANUAL_RECORD;
				}
				else
				{
					ttype += "|";
					ttype += STR_MANUAL_RECORD;
				}
			}
			if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_SCHEDULE))
			{
				if (ttype.IsEmpty())
				{
					ttype = STR_SCHEDULE_RECORD;
				}
				else
				{
					ttype += "|";
					ttype += STR_SCHEDULE_RECORD;
				}
			}
			if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_MOTION))
			{
				if (ttype.IsEmpty())
				{
					ttype = STR_MOTION_RECORD;
				}
				else
				{
					ttype += "|";
					ttype += STR_MOTION_RECORD;
				}
			}
			if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_SENSOR))
			{
				if (ttype.IsEmpty())
				{
					ttype = STR_SENSOR_RECORD;
				}
				else
				{
					ttype += "|";
					ttype += STR_SENSOR_RECORD;
				}
			}
			if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_BEHAVIOR))
			{
				if (ttype.IsEmpty())
				{
					ttype = STR_BEHIVOR_RECORD;
				}
				else
				{
					ttype += "|";
					ttype += STR_BEHIVOR_RECORD;
				}
			}
			if (rs[i].dwRecordType == DD_RECORD_TYPE_NONE)
			{
				ttype = STR_NONE_RECORD;
			}
			/*temp.Format(STR_RECORD_MSG_FORMAT, pThis->m_ipMap[lUserID], rs[i].dwChannel, ttype);
			
			pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp.GetBuffer());
			temp.ReleaseBuffer();*/
		}
	}
	else if (lCommand == NET_SDK_IVM_RULE)
	{

		DWORD curLen = 0; ;
		while(curLen < dwBufLen)
		{
			CString temp;
			CString ttemp;
			CString ttype;

			char *tmpBuf = pBuf+curLen;
			NET_SDK_IVM_RULE_STATUS *pIVMRuleStatus = (NET_SDK_IVM_RULE_STATUS *)tmpBuf;
			curLen += sizeof(NET_SDK_IVM_RULE_STATUS);
			switch (pIVMRuleStatus->dwIVMType)
			{
				case NET_SDK_IVM_RULE_TYPE_VFD:
				case NET_SDK_IVM_RULE_TYPE_AVD_SCENE:
				case NET_SDK_IVM_RULE_TYPE_AVD_CLARITY:
				case NET_SDK_IVM_RULE_TYPE_AVD_COLOR:
					{
						if(pIVMRuleStatus->dwIVMSize != sizeof(NET_SDK_IVM_STATUS_INFO))//lib与头文件不匹配才会如此
						{
							ASSERT(FALSE);
							break;
						}

						if((curLen+pIVMRuleStatus->dwIVMSize)>dwBufLen)
						{
							ASSERT(FALSE);
							break;
						}
						tmpBuf = pBuf+curLen;
						NET_SDK_IVM_STATUS_INFO *pVFDStatus = (NET_SDK_IVM_STATUS_INFO*)tmpBuf;
						if(pIVMRuleStatus->dwIVMType == NET_SDK_IVM_RULE_TYPE_VFD)
							ttype = "NET_SDK_IVM_RULE_TYPE_VFD";
						else if(pIVMRuleStatus->dwIVMType == NET_SDK_IVM_RULE_TYPE_AVD_SCENE)
							ttype = "NET_SDK_IVM_RULE_TYPE_AVD_SCENE";
						else if(pIVMRuleStatus->dwIVMType == NET_SDK_IVM_RULE_TYPE_AVD_CLARITY)
							ttype = "NET_SDK_IVM_RULE_TYPE_AVD_CLARITY";
						else if(pIVMRuleStatus->dwIVMType == NET_SDK_IVM_RULE_TYPE_AVD_COLOR)
							ttype = "NET_SDK_IVM_RULE_TYPE_AVD_COLOR";
						ttemp.Format(STR_CHANNEL_D, pVFDStatus->dwChannel);

						curLen += sizeof(NET_SDK_IVM_STATUS_INFO);
						
						
					}
					break;
				default:
					TRACE("NET_SDK_IVM_RULE dwIVMType= %d, dwIVMSize=%d \n", pIVMRuleStatus->dwIVMType, pIVMRuleStatus->dwIVMSize);
					break;
			}

			temp.Format(STR_MSG_FORMAT, pThis->m_ipMap[lUserID], ttype, ttemp);
			pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp.GetBuffer());
			temp.ReleaseBuffer();
		}
	}
	else
	{
		ASSERT(FALSE);
	}
	return 0;
}
BOOL CALLBACK CSDKDEMODlg::msgCallbackEx( LONG lCommand, LONG lUserID, char *pBuf, DWORD dwBufLen, void *pUser )
{
	CSDKDEMODlg *pThis = (CSDKDEMODlg *)pUser;
	if (lCommand == NET_SDK_ALARM)
	{
		int num = dwBufLen / sizeof(NET_SDK_ALARMINFO_EX);
		NET_SDK_ALARMINFO_EX *ai = reinterpret_cast<NET_SDK_ALARMINFO_EX *>(pBuf);

		for (int i = 0; i < num; i++)
		{
			CString temp;
			CString ttemp;
			CString ttype;
			CString ttime;
			ttime = ai[i].alarmTime;
			switch(ai[i].dwAlarmType)
			{
			case NET_SDK_N9000_ALARM_TYPE_MOTION:
				ttype = STR_MOTION;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_SENSOR:
				ttype = STR_SENSOR;
				ttemp.Format(STR_SENSOR_E, ai[i].dwSensorIn, ai[i].dwChannel, ai[i].sensorName);
				break;
			case NET_SDK_N9000_ALARM_TYPE_VLOSS:
				ttype = STR_VIDEO_LOSS;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_SIGNAL_SHELTER:
				ttype = STR_SHELTER;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_DISK_FULL:
				ttype = STR_DISK_FULL;
				ttemp.Format(STR_DISK_NO_D, ai[i].dwDisk);
				break;
			case NET_SDK_N9000_ALARM_TYPE_ALARM_OUT:
				ttype = "alarm out";
				ttemp.Format("index:%d", ai[i].dwChannel);
				break;
			/*case NET_SDK_ALARM_TYPE_DISK_UNFORMATTED:
				ttype = STR_DISK_UNFORMAT;
				ttemp.Format(STR_DISK_NO_D, ai[i].dwDisk);
				break;
			case NET_SDK_ALARM_TYPE_DISK_WRITE_FAIL:
				ttype = STR_DISK_READ_WRITE_ERROR;
				ttemp.Format(STR_DISK_NO_D, ai[i].dwDisk);
				break;*/
			case NET_SDK_N9000_ALARM_TYPE_NO_DISK:
				ttype = "No disk";
				ttemp.Format(STR_DISK_NO_D, ai[i].dwDisk);
				break;
			case NET_SDK_N9000_ALARM_TYPE_HDD_PULL_OUT:
				ttype = "Disk pull out";
				ttemp.Format(STR_DISK_NO_D, ai[i].dwDisk);
				break;
			//front offline
			case NET_SDK_N9000_ALARM_TYPE_FRONT_OFFLINE:
				ttype = STR_ALARM_FRONT_OFFLINE;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			//pea perimeter:
			case NET_SDK_N9000_ALARM_TYPE_PEA_PERIMETER:
				ttype = STR_ALARM_PEA_PERIMETER;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_PEA_TRIPWIRE:
				ttype = STR_ALARM_PEA_TRIPWIRE;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_OSC:
				ttype = STR_ALARM_OSC;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_AVD_SCENE:
				ttype = STR_ALARM_AVD_SCENE;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_AVD_CLARITY:
				ttype = STR_ALARM_AVD_CLARITY;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_AVD_COLOR:
				ttype = STR_ALARM_AVD_COLOR;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_IP_CONFLICT:
				ttype = STR_ALARM_IP_CONFLICT;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_DISK_IO_ERROR:
				ttype = STR_ALARM_DISK_IO_ERROR;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_NET_DISCONNECT:
				ttype = STR_ALARM_NET_DISCONNECT;
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			case NET_SDK_N9000_ALARM_TYPE_DOOR_BELL:
				ttype = "Door Bell";
				ttemp.Format(STR_CHANNEL_D, ai[i].dwChannel);
				break;
			}
			//temp.Format(STR_MSG_FORMAT, pThis->m_ipMap[lUserID], ttype, ttemp);"%s device alarm message|IP:%s|Type:%s|%s"
			temp.Format("%s device alarm message|IP:%s|Type:%s|%s", ttime, pThis->m_ipMap[lUserID], ttype, ttemp);
			pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp.GetBuffer());
			temp.ReleaseBuffer();
		}
	}
	else if (lCommand == NET_SDK_RECORD)
	{
		int num = dwBufLen / sizeof(NET_SDK_RECORD_STATUS_EX);
		NET_SDK_RECORD_STATUS_EX *rs = reinterpret_cast<NET_SDK_RECORD_STATUS_EX *>(pBuf);

		for (int i = 0; i < num; i++)
		{
			CString temp;
#if 0
			CString ttype;
			switch(rs[i].dwRecordType)
			{
			case DD_RECORD_TYPE_MANUAL:
				ttype = STR_MANUAL_RECORD;
				break;
			case DD_RECORD_TYPE_SCHEDULE:
				ttype = STR_SCHEDULE_RECORD;
				break;
			case DD_RECORD_TYPE_MOTION:
				ttype = STR_MOTION_RECORD;
				break;
			case DD_RECORD_TYPE_SENSOR:
				ttype = STR_SENSOR_RECORD;
				break;
			case DD_RECORD_TYPE_BEHAVIOR:
				ttype = STR_BEHIVOR_RECORD;
				break;
			case DD_RECORD_TYPE_NONE:
				ttype = STR_NONE_RECORD;
				break;
			}
			temp.Format(STR_RECORD_MSG_FORMAT, pThis->m_ipMap[lUserID], rs->dwChannel, ttype);
#endif
			CString ttype = "";
			if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_MANUAL))
			{
				if (ttype.IsEmpty())
				{
					ttype = STR_MANUAL_RECORD;
				}
				else
				{
					ttype += "|";
					ttype += STR_MANUAL_RECORD;
				}
			}
			if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_SCHEDULE))
			{
				if (ttype.IsEmpty())
				{
					ttype = STR_SCHEDULE_RECORD;
				}
				else
				{
					ttype += "|";
					ttype += STR_SCHEDULE_RECORD;
				}
			}
			if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_MOTION))
			{
				if (ttype.IsEmpty())
				{
					ttype = STR_MOTION_RECORD;
				}
				else
				{
					ttype += "|";
					ttype += STR_MOTION_RECORD;
				}
			}
			if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_SENSOR))
			{
				if (ttype.IsEmpty())
				{
					ttype = STR_SENSOR_RECORD;
				}
				else
				{
					ttype += "|";
					ttype += STR_SENSOR_RECORD;
				}
			}
			if (0 != (rs[i].dwRecordType & DD_RECORD_TYPE_BEHAVIOR))
			{
				if (ttype.IsEmpty())
				{
					ttype = STR_BEHIVOR_RECORD;
				}
				else
				{
					ttype += "|";
					ttype += STR_BEHIVOR_RECORD;
				}
			}
			if (rs[i].dwRecordType == DD_RECORD_TYPE_NONE)
			{
				ttype = STR_NONE_RECORD;
			}

			CString tStatus = "";
			if(rs[i].dwRecordStatus == DD_RECORD_STATUS_OFF)
				tStatus = "record stoped";
			else if(rs[i].dwRecordStatus == DD_RECORD_STATUS_ON)
				tStatus = "recording";
			else if(rs[i].dwRecordStatus == DD_RECORD_STATUS_ABNORMAL)
				tStatus = "record abnormal";

			temp.Format("device record message|ip:%s|channel:%d|type:%s|status:%s", pThis->m_ipMap[lUserID], rs[i].dwChannel, ttype, tStatus);
			//temp.Format(STR_RECORD_MSG_FORMAT, pThis->m_ipMap[lUserID], rs[i].dwChannel, ttype);
			pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp.GetBuffer());
			temp.ReleaseBuffer();
		}
	}
	else if (lCommand == NET_SDK_IVM_RULE)
	{

		DWORD curLen = 0; ;
		while(curLen < dwBufLen)
		{
			CString temp;
			CString ttemp;
			CString ttype;

			char *tmpBuf = pBuf+curLen;
			NET_SDK_IVM_RULE_STATUS *pIVMRuleStatus = (NET_SDK_IVM_RULE_STATUS *)tmpBuf;
			curLen += sizeof(NET_SDK_IVM_RULE_STATUS);
			switch (pIVMRuleStatus->dwIVMType)
			{
			case NET_SDK_IVM_RULE_TYPE_VFD:
			case NET_SDK_IVM_RULE_TYPE_AVD_SCENE:
			case NET_SDK_IVM_RULE_TYPE_AVD_CLARITY:
			case NET_SDK_IVM_RULE_TYPE_AVD_COLOR:
				{
					if(pIVMRuleStatus->dwIVMSize != sizeof(NET_SDK_IVM_STATUS_INFO))//lib与头文件不匹配才会如此
					{
						ASSERT(FALSE);
						break;
					}

					if((curLen+pIVMRuleStatus->dwIVMSize)>dwBufLen)
					{
						ASSERT(FALSE);
						break;
					}
					tmpBuf = pBuf+curLen;
					NET_SDK_IVM_STATUS_INFO *pVFDStatus = (NET_SDK_IVM_STATUS_INFO*)tmpBuf;
					if(pIVMRuleStatus->dwIVMType == NET_SDK_IVM_RULE_TYPE_VFD)
						ttype = "NET_SDK_IVM_RULE_TYPE_VFD";
					else if(pIVMRuleStatus->dwIVMType == NET_SDK_IVM_RULE_TYPE_AVD_SCENE)
						ttype = "NET_SDK_IVM_RULE_TYPE_AVD_SCENE";
					else if(pIVMRuleStatus->dwIVMType == NET_SDK_IVM_RULE_TYPE_AVD_CLARITY)
						ttype = "NET_SDK_IVM_RULE_TYPE_AVD_CLARITY";
					else if(pIVMRuleStatus->dwIVMType == NET_SDK_IVM_RULE_TYPE_AVD_COLOR)
						ttype = "NET_SDK_IVM_RULE_TYPE_AVD_COLOR";
					ttemp.Format(STR_CHANNEL_D, pVFDStatus->dwChannel);

					curLen += sizeof(NET_SDK_IVM_STATUS_INFO);


				}
				break;
			default:
				TRACE("NET_SDK_IVM_RULE dwIVMType= %d, dwIVMSize=%d \n", pIVMRuleStatus->dwIVMType, pIVMRuleStatus->dwIVMSize);
				break;
			}

			temp.Format(STR_MSG_FORMAT, pThis->m_ipMap[lUserID], ttype, ttemp);
			pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp.GetBuffer());
			temp.ReleaseBuffer();
		}
	}
	else
	{
		ASSERT(FALSE);
	}
	return 0;
}

void CALLBACK CSDKDEMODlg::SubscribCallBack(LONG lUserID, DWORD dwCommand, char *pBuf, DWORD dwBufLen, void *pUser)
{
	CSDKDEMODlg *pThis = (CSDKDEMODlg *)pUser;
	switch(dwCommand)
	{
		//IVE_VFD_RESULT_HEAD_T+[IVE_VFD_RESULT_DATA_INFO_T+源数据]+[IVE_VFD_RESULT_FACE_DATA_INFO_T+人脸数据+人脸特征值]+...+[IVE_VFD_RESULT_FACE_DATA_INFO_T+人脸数据+人脸特征值]
	case  NET_SDK_SMART_EVENT_TYPE_VFD:
		{
			TRACE("NET_DVR_SMART_VFD dwBufLen=%d\n", dwBufLen);
			if (dwBufLen < sizeof(NET_DVR_IVE_VFD_RESULT_HEAD_T))
			{
				break;
			}
			if(pThis->m_CurDlg)
			{
				pThis->m_CurDlg->UpdateMsg( lUserID, dwCommand, pBuf, dwBufLen);
			}

			char* tmp = pBuf;
			 //解析头
			NET_DVR_IVE_VFD_RESULT_HEAD_T* head = (NET_DVR_IVE_VFD_RESULT_HEAD_T*)tmp;
			tmp += sizeof(NET_DVR_IVE_VFD_RESULT_HEAD_T);
			 //解析原始数据
			NET_DVR_IVE_VFD_RESULT_DATA_INFO_T* src = (NET_DVR_IVE_VFD_RESULT_DATA_INFO_T*)tmp;     
			tmp += sizeof(NET_DVR_IVE_VFD_RESULT_DATA_INFO_T);
			if(src->dataLen > 0)
			{
				TRACE("src->dataLen=%d, src->width=%d, src->height=%d \n", src->dataLen, src->width, src->height);
				//源数据
				/*char* img = new char[src->dataLen];
				memcpy(img, tmp, src->dataLen);
				delete img;*/
				tmp += src->dataLen;
			}

			for(int i=0; i<head->faceCnt; i++)
			{
				NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T* face = (NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T*)tmp;
				tmp += sizeof(NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T);  

				 //face Image, 如果有
				if(face->stFaceImgData.dataLen > 0)
				{
					char* faceImg = new char[face->stFaceImgData.dataLen];
					memcpy(faceImg, tmp, face->stFaceImgData.dataLen);

				
					FILE *backfp = fopen("./FaceImg.jpg", "wb");
					if(backfp)
					{
						int fret = fwrite(faceImg, face->stFaceImgData.dataLen, 1, backfp );
						fclose(backfp);
					}

					delete faceImg;
					tmp+= face->stFaceImgData.dataLen;		
				}

				 //feature, 如果有
				if(face->featureSize > 0)
				{
					char* feature = new char[face->featureSize];
					memcpy(feature, tmp, face->featureSize);

					FILE *backfp = fopen("./feature.jpg", "wb");
					if(backfp)
					{
						int fret = fwrite(feature, face->featureSize, 1, backfp );
						fclose(backfp);
					}
					delete feature;
					tmp += face->featureSize;
				}
			}
		}
		break;
	case NET_SDK_SMART_EVENT_TYPE_AVD:
		{
			TRACE("NET_SDK_SMART_EVENT_TYPE_AVD dwBufLen=%d\n", dwBufLen);
			if(dwBufLen !=sizeof(NET_SDK_IVE_AVD_T))
			{
				TRACE("NET_SDK_SMART_EVENT_TYPE_AVD len error dwBufLen=%d\n", dwBufLen);
				return;
			}

			NET_SDK_IVE_AVD_T* sIVE_AVD = (NET_SDK_IVE_AVD_T*)pBuf;
			int i=0;

		}
		break;
		/*	----------------------
		|	NET_SDK_IVE_FACE_MATCH_T	|
		-------------------------
		|	图片数据(抓拍)	|
		-------------------------
		|	AVPSTORE_FACE_ABSTRACT_INFO		|
		-------------------------
		|	NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T		|
		-------------------------
		|	图片数据(人脸目标)	|
		-------------------------
		|	TLV_BUFFER_DESC		|
		-------------------------
		|	图片数据(背景)	|
		-------------------------*/
	case NET_SDK_SMART_EVENT_TYPE_FACE_MATCH:
		{
			TRACE("NET_SDK_SMART_EVENT_TYPE_FACE_MATCH dwBufLen=%d\n", dwBufLen);
			if(dwBufLen <= sizeof(NET_SDK_IVE_FACE_MATCH_T))
			{
				TRACE("NET_SDK_SMART_EVENT_TYPE_FACE_MATCH len error dwBufLen=%d\n", dwBufLen);
				return;
			}
			/*if(pThis->m_CurDlg)
			{
				pThis->m_CurDlg->UpdateMsg( lUserID, dwCommand, pBuf, dwBufLen);
			}*/
			
			/*
			NET_SDK_IVE_FACE_MATCH_T* sIVE_AVD = (NET_SDK_IVE_FACE_MATCH_T*)pBuf;
			TRACE("dwRealFaceID=%d, dwGrpID= %d dwLibFaceID = %d Channel=%d imgLen= %d dwSimilar=%d byName = %s \n",sIVE_AVD->dwRealFaceID, sIVE_AVD->dwGrpID, sIVE_AVD->dwLibFaceID,sIVE_AVD->Channel,sIVE_AVD->imgLen,
				sIVE_AVD->dwSimilar, sIVE_AVD->byName);
			FILE *fp = fopen("./ZhuaPai.jpg", "wb");
			if(fp)
			{
				int fret = fwrite(pBuf+sizeof(NET_SDK_IVE_FACE_MATCH_T) , sIVE_AVD->imgLen, 1, fp );
				fclose(fp);
			}

			pBuf += sizeof(NET_SDK_IVE_FACE_MATCH_T)+sIVE_AVD->imgLen;

			//////////
			/*NET_SDK_AVPSTORE_FACE_ABSTRACT_INFO* pFaceInfo = (NET_SDK_AVPSTORE_FACE_ABSTRACT_INFO*)pBuf;
			pBuf += sizeof(NET_SDK_AVPSTORE_FACE_ABSTRACT_INFO);

			NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T* pFaceDataInfo = (NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T*)pBuf;
			pBuf += sizeof(NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T);

			fp = fopen("./Target.jpg", "wb");
			if(fp)
			{
				int fret = fwrite(pBuf , pFaceDataInfo->stFaceImgData.dataLen, 1, fp );
				fclose(fp);
			}
			pBuf += pFaceDataInfo->stFaceImgData.dataLen;

			NET_SDK_TLV_BUFFER_DESC* pBufferDesc = (NET_SDK_TLV_BUFFER_DESC*)pBuf;
			pBuf += sizeof(NET_SDK_TLV_BUFFER_DESC);
			fp = fopen("./BeiJing.jpg", "wb");
			if(fp)
			{

				int fret = fwrite(pBuf , pBufferDesc->dwSize, 1, fp );
				fclose(fp);
			}*/
			//NET_SDK_IVE_FACE_MATCH_T* sIVE_AVD = (NET_SDK_IVE_FACE_MATCH_T*)pBuf;
			//#ifdef __CHONGQING_ZHONGRAN__
			//if(dwBufLen != (sizeof(NET_SDK_IVE_FACE_MATCH_T) + sIVE_AVD->imgLen  + sIVE_AVD->BackLen))
			//#else
			////if(dwBufLen != (sizeof(NET_SDK_IVE_FACE_MATCH_T)+sIVE_AVD->imgLen))
			//#endif
			//{
			//	TRACE("NET_DVR_SMART_FACE_MATCH len error dwBufLen=%d\n", dwBufLen);
			//	//return;
			//}
			//#ifdef __CHONGQING_ZHONGRAN__
			//TRACE("dwRealFaceID=%d, dwGrpID= %d dwLibFaceID = %d Channel=%d imgLen= %d BackLen = %d dwSimilar=%d byName = %s \n",sIVE_AVD->dwRealFaceID, sIVE_AVD->dwGrpID, sIVE_AVD->dwLibFaceID,sIVE_AVD->Channel,sIVE_AVD->imgLen,
			//	sIVE_AVD->BackLen, sIVE_AVD->dwSimilar, sIVE_AVD->byName);
			//#else
			//TRACE("dwRealFaceID=%d, dwGrpID= %d dwLibFaceID = %d Channel=%d imgLen= %d dwSimilar=%d byName = %s \n",sIVE_AVD->dwRealFaceID, sIVE_AVD->dwGrpID, sIVE_AVD->dwLibFaceID,sIVE_AVD->Channel,sIVE_AVD->imgLen,
			//	sIVE_AVD->dwSimilar, sIVE_AVD->byName);
			//#endif
			//FILE *fp = fopen("./ZhuaPai.jpg", "wb");
			//if(fp)
			//{
			//	
			//	int fret = fwrite(pBuf+sizeof(NET_SDK_IVE_FACE_MATCH_T) , sIVE_AVD->imgLen, 1, fp );
			//	fclose(fp);
			//}

			//pBuf += sizeof(NET_SDK_IVE_FACE_MATCH_T)+sIVE_AVD->imgLen;

			//NET_SDK_AVPSTORE_FACE_ABSTRACT_INFO* pFaceInfo = (NET_SDK_AVPSTORE_FACE_ABSTRACT_INFO*)pBuf;
			//pBuf += sizeof(NET_SDK_AVPSTORE_FACE_ABSTRACT_INFO);

			//NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T* pFaceDataInfo = (NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T*)pBuf;
			//pBuf += sizeof(NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T);

			//fp = fopen("./Target.jpg", "wb");
			//if(fp)
			//{

			//	int fret = fwrite(pBuf , pFaceDataInfo->stFaceImgData.dataLen, 1, fp );
			//	fclose(fp);
			//}
			//pBuf += pFaceDataInfo->stFaceImgData.dataLen;

			//NET_SDK_TLV_BUFFER_DESC* pBufferDesc = (NET_SDK_TLV_BUFFER_DESC*)pBuf;
			//pBuf += sizeof(NET_SDK_TLV_BUFFER_DESC);
			//fp = fopen("./BeiJing.jpg", "wb");
			//if(fp)
			//{

			//	int fret = fwrite(pBuf , pBufferDesc->dwSize, 1, fp );
			//	fclose(fp);
			//}

			//#ifdef __CHONGQING_ZHONGRAN__
			//if (sIVE_AVD->BackLen > 0)
			//{
			//	FILE *backfp = fopen("./testback.jpg", "wb");
			//	if(backfp)
			//	{
			//		int fret = fwrite(pBuf+sizeof(NET_SDK_IVE_FACE_MATCH_T)+sIVE_AVD->imgLen, sIVE_AVD->BackLen, 1, backfp );
			//		fclose(backfp);
			//	}
			//}
			//#endif


		}
		break;
		/*	----------------------
		|	NET_SDK_IVE_BASE_INFO	|
		-------------------------
		|	NET_SDK_IVE_PICTURE_INFO	|
		-------------------------
		|	图片数据(实时)		|
		-------------------------
		|	NET_SDK_IVE_PICTURE_INFO	|
		-------------------------
		|	图片数据(相册)		|
		-------------------------*/
	case NET_SDK_SMART_EVENT_TYPE_FACE_MATCH_FOR_IPC:
		{
			if(pThis->m_CurDlg)
			{
				pThis->m_CurDlg->UpdateMsg( lUserID, dwCommand, pBuf, dwBufLen);
			}
			TRACE("NET_SDK_SMART_EVENT_TYPE_FACE_MATCH_FOR_IPC dwBufLen=%d\n", dwBufLen);
			NET_SDK_IVE_BASE_INFO *baseInfo = (NET_SDK_IVE_BASE_INFO*)pBuf;
			TRACE("iSimilarity=%d, iType=%d, szName=%s, szIdentifyNum=%s \n",baseInfo->iSimilarity, baseInfo->iType, baseInfo->szName, baseInfo->szIdentifyNum);

			NET_SDK_IVE_PICTURE_INFO *pictureInfo = (NET_SDK_IVE_PICTURE_INFO*)(pBuf+sizeof(NET_SDK_IVE_BASE_INFO));
			TRACE("iHeight=%d, iWidth=%d, iPicSize=%d, iPicFormat=%d, \n",pictureInfo->iHeight, pictureInfo->iWidth, pictureInfo->iPicSize, pictureInfo->iPicFormat);
			if(dwBufLen >= (sizeof(NET_SDK_IVE_BASE_INFO)+sizeof(NET_SDK_IVE_PICTURE_INFO)+pictureInfo->iPicSize))
			{
				if (pictureInfo->iPicSize > 0)
				{
					FILE *backfp = fopen("./testback.jpg", "wb");
					if(backfp)
					{
						int fret = fwrite(pBuf+(sizeof(NET_SDK_IVE_BASE_INFO)+sizeof(NET_SDK_IVE_PICTURE_INFO)), pictureInfo->iPicSize, 1, backfp );
						fclose(backfp);
					}
				}
			}

			if(dwBufLen > (sizeof(NET_SDK_IVE_BASE_INFO)+sizeof(NET_SDK_IVE_PICTURE_INFO)+pictureInfo->iPicSize))
			{
				NET_SDK_IVE_PICTURE_INFO *pictureInfo2 = (NET_SDK_IVE_PICTURE_INFO*)(pBuf+(sizeof(NET_SDK_IVE_BASE_INFO)+sizeof(NET_SDK_IVE_PICTURE_INFO)+pictureInfo->iPicSize));
				TRACE("2222 iHeight=%d, iWidth=%d, iPicSize=%d, iPicFormat=%d, \n",pictureInfo2->iHeight, pictureInfo2->iWidth, pictureInfo2->iPicSize, pictureInfo2->iPicFormat);
				if(dwBufLen >= (sizeof(NET_SDK_IVE_BASE_INFO)+2*sizeof(NET_SDK_IVE_PICTURE_INFO)+pictureInfo->iPicSize+pictureInfo2->iPicSize))
				{
					if (pictureInfo2->iPicSize > 0)
					{
						FILE *backfp = fopen("./testback2.jpg", "wb");
						if(backfp)
						{
							int fret = fwrite(pBuf+(sizeof(NET_SDK_IVE_BASE_INFO)+2*sizeof(NET_SDK_IVE_PICTURE_INFO)+pictureInfo->iPicSize), pictureInfo2->iPicSize, 1, backfp );
							fclose(backfp);
						}
					}
				}
			}

		}
		break;
	case NET_SDK_SMART_EVENT_TYPE_PEA_FOR_IPC:
		{
			TRACE("NET_SDK_SMART_EVENT_TYPE_PEA_FOR_IPC dwBufLen=%d\n", dwBufLen);
			NET_SDK_IVE_PEA_T *pea = (NET_SDK_IVE_PEA_T*)pBuf;
			int a = pea->perimeter.count;
			int b = pea->perimeter.perInfo->targetId;

			int c = pea->tripwire.count;
			//pea->tripwire.tripInfo[0]
			break;
		}
	case NET_SDK_SMART_EVENT_TYPE_PEA_TARGET:
		{
			//IVE_PEA_T + IVE_EventData_T+【原图】+ IVE_EvenTargetData_T+【目标图】+ IVE_EvenTargetData_T+【目标图】
			TRACE("NET_SDK_SMART_EVENT_TYPE_PEA_TARGET dwBufLen=%d\n", dwBufLen);
			if (dwBufLen < sizeof(NET_SDK_IVE_PEA_T))
			{
				break;
			}
			char* tmp = pBuf;
			int nLeftBufLen = dwBufLen;
			NET_SDK_IVE_PEA_T* head = (NET_SDK_IVE_PEA_T*)tmp;
			int a = head->perimeter.count;
			int b = head->perimeter.perInfo->targetId;
			int c = head->tripwire.count;

			tmp += sizeof(NET_SDK_IVE_PEA_T);
			nLeftBufLen -= sizeof(NET_SDK_IVE_PEA_T);
			//目标框
			{
				NET_SDK_IVE_RECT_T rect[32]; // 目标矩形框
				tuint32 count = head->tripwire.count + head->perimeter.count; // 个数
				if (count > 32) //最多32个
				{
					count = 32;
				}
				int index = 0;
				for (tuint32 i=0; i<head->perimeter.count; i++ )
				{
					rect[index].X1 = head->perimeter.perInfo[i].rect.X1;
					rect[index].X2 = head->perimeter.perInfo[i].rect.X2;
					rect[index].Y1 = head->perimeter.perInfo[i].rect.Y1;
					rect[index].Y2 = head->perimeter.perInfo[i].rect.Y2;
					index++;
				}
				for (tuint32 i=0; (i<head->tripwire.count && index<32); i++ )
				{
					rect[index].X1 = head->tripwire.tripInfo[i].rect.X1;
					rect[index].X2 = head->tripwire.tripInfo[i].rect.X2;
					rect[index].Y1 = head->tripwire.tripInfo[i].rect.Y1;
					rect[index].Y2 = head->tripwire.tripInfo[i].rect.Y2;
					index++;
				}
				if (count != 0)
				{
				}
			}
			//原图
			NET_SDK_IVE_EventData_T *eventData = (NET_SDK_IVE_EventData_T*)tmp;
			tmp += sizeof(NET_SDK_IVE_EventData_T);
			nLeftBufLen -= sizeof(NET_SDK_IVE_EventData_T);
			if (eventData->dataLen > 0)
			{
				char* img = new char[eventData->dataLen];
				memcpy(img, tmp, eventData->dataLen);

				FILE *backfp = fopen("./PEA_Target_srcImg.jpg", "wb");
				if(backfp)
				{
					int fret = fwrite(img, eventData->dataLen, 1, backfp );
					fclose(backfp);
				}
				delete img;
				tmp += eventData->dataLen;
				nLeftBufLen -= eventData->dataLen;
			}
			//目标图
			while (nLeftBufLen > sizeof(NET_SDK_IVE_EvenTargetData_T))
			{
				NET_SDK_IVE_EvenTargetData_T *eventTargetData = (NET_SDK_IVE_EvenTargetData_T*)tmp;
				tmp += sizeof(NET_SDK_IVE_EvenTargetData_T);
				nLeftBufLen -= sizeof(NET_SDK_IVE_EvenTargetData_T);
				if (eventTargetData->dataLen > 0)
				{
					char* img = new char[eventTargetData->dataLen];
					memcpy(img, tmp, eventTargetData->dataLen);

					FILE *backfp = fopen("./PEA_Target_targetImg.jpg", "wb");
					if(backfp)
					{
						int fret = fwrite(img, eventTargetData->dataLen, 1, backfp );
						fclose(backfp);
					}
					delete img;
					tmp += eventTargetData->dataLen;
					nLeftBufLen -= eventTargetData->dataLen;
				}
			}
			break;
		}
	case NET_SDK_SMART_EVENT_TYPE_OSC:
		{
			//LONG lUserID, DWORD dwCommand, char *pBuf, DWORD dwBufLen, void *pUser
			if(dwBufLen !=sizeof(NET_SDK_IVE_OSC_T))
			{
				TRACE("NET_SDK_SMART_EVENT_TYPE_OSC len error dwBufLen=%d\n", dwBufLen);
				return;
			}
			NET_SDK_IVE_OSC_T *sIVE_OSC = (NET_SDK_IVE_OSC_T*)pBuf;					
			break;
		}
	case NET_SDK_SMART_EVENT_TYPE_CPC:
		{
			TRACE("NET_SDK_SMART_EVENT_TYPE_CPC dwBufLen=%d\n", dwBufLen);
			break;
		}
	case NET_SDK_SMART_EVENT_TYPE_CDD:
		{
			TRACE("NET_SDK_SMART_EVENT_TYPE_CDD dwBufLen=%d\n", dwBufLen);
			break;
		}
	case NET_SDK_SMART_EVENT_TYPE_IPD:
		{
			TRACE("NET_SDK_SMART_EVENT_TYPE_IPD dwBufLen=%d\n", dwBufLen);
			break;
		}
	case NET_SDK_SMART_EVENT_TYPE_TRAJECT:
		{
			TRACE("NET_SDK_SMART_EVENT_TYPE_TRAJECT dwBufLen=%d\n", dwBufLen);
			break;
		}
	case NET_SDK_SMART_EVENT_TYPE_VEHICLE:
		{
			if(pThis->m_CurDlg)
			{
				pThis->m_CurDlg->UpdateMsg( lUserID, dwCommand, pBuf, dwBufLen);
			}
			//NET_SDK_IVE_VEHICE_HEAD_INFO+[NET_SDK_IVE_VEHICE_ITEM_INFO+源数据]+[NET_SDK_IVE_VEHICE_ITEM_INFO+车牌数据]+...+[NET_SDK_IVE_VEHICE_ITEM_INFO+车牌数据] 
			//NET_SDK_IVE_VEHICE_HEAD_INFO *vehiceHeadInfo = (NET_SDK_IVE_VEHICE_HEAD_INFO*)pBuf;
			//TRACE("item_cnt=%d\n",vehiceHeadInfo->item_cnt);
			//unsigned int cntVheCle = vehiceHeadInfo->item_cnt;
			//unsigned int offset = sizeof(NET_SDK_IVE_VEHICE_HEAD_INFO);
		
			//if (cntVheCle > 0)
			//{
			//	for(int i = 0;i< cntVheCle;i++)
			//	{
			//		NET_SDK_IVE_VEHICE_ITEM_INFO vehicleinfo;
			//		memset(&vehicleinfo,0,sizeof(NET_SDK_IVE_VEHICE_ITEM_INFO));
			//		memcpy(&vehicleinfo, pBuf + offset,sizeof(NET_SDK_IVE_VEHICE_ITEM_INFO));
			//		offset += sizeof(NET_SDK_IVE_VEHICE_ITEM_INFO);//指针偏移
			//		
			//		if (vehicleinfo.jpeg_len > 0)
			//		{
			//			if(i == 0)//第1张是源图片
			//			{
			//				FILE *backfp = fopen("./vehicleSorce.jpg", "wb");
			//				if(backfp)
			//				{
			//					int fret = fwrite(pBuf+offset, vehicleinfo.jpeg_len, 1, backfp );
			//					fclose(backfp);
			//				}
			//			}
			//			else//其他是车牌照片
			//			{
			//				CString path;
			//				path.Format(_T("./vehicle%d.jpg"),i);
			//				
			//				FILE *backfp = fopen(path.GetString(), "wb");
			//				//FILE *backfp = fopen("./vehicle.jpg", "wb");
			//				if(backfp)
			//				{
			//					int fret = fwrite(pBuf+offset, vehicleinfo.jpeg_len, 1, backfp );
			//					fclose(backfp);
			//				}
			//			}
			//				
			//		}
			//		offset += (vehicleinfo.jpeg_vir_len - vehicleinfo.jpeg_len);
			//		//那么这个时候不用加偏移
			//		if (vehicleinfo.jpeg_vir_len <= vehicleinfo.jpeg_len)
			//		{
			//			offset += vehicleinfo.jpeg_len;
			//		}
			//		else
			//		{
			//			offset += vehicleinfo.jpeg_len;
			//		}											
			//	}
			//}
			break;
		}
	case NET_SDK_SMART_EVENT_TYPE_PASSLINE:
		{

			if(dwBufLen <= sizeof(NET_SDK_IVE_PASSLINECOUNT_T))
			{
				TRACE("NET_SDK_SMART_EVENT_TYPE_PASSLINE len error dwBufLen=%d\n", dwBufLen);
				return;
			}
			//过线统计
			NET_SDK_IVE_PASSLINECOUNT_T* sIVE_PassLine = (NET_SDK_IVE_PASSLINECOUNT_T*)pBuf;
			if (dwBufLen <= sizeof(NET_SDK_IVE_PASSLINECOUNT_T))
			{
				return;  //IPC只有过线统计消息 NVR有过线统计消息+全景图+抓拍图
			}

			//全景图
			NET_SDK_PASSLINE_PICTURE_INFO *pictureInfo = (NET_SDK_PASSLINE_PICTURE_INFO*)(pBuf+sizeof(NET_SDK_IVE_PASSLINECOUNT_T));
			TRACE("iHeight=%d, iWidth=%d, iPicSize=%d, iPicFormat=%d, \n",pictureInfo->iHeight, pictureInfo->iWidth, pictureInfo->iPicSize, pictureInfo->iPicFormat);
			if(dwBufLen >= (sizeof(NET_SDK_IVE_PASSLINECOUNT_T)+sizeof(NET_SDK_PASSLINE_PICTURE_INFO)+pictureInfo->iPicSize))
			{
				if (pictureInfo->iPicSize > 0)
				{
					FILE *backfp = fopen("./fullPicture.jpg", "wb");
					if(backfp)
					{
						int fret = fwrite(pBuf+(sizeof(NET_SDK_IVE_PASSLINECOUNT_T)+sizeof(NET_SDK_PASSLINE_PICTURE_INFO)), pictureInfo->iPicSize, 1, backfp );
						fclose(backfp);
					}
				}
			}

			//抓拍图
			NET_SDK_PASSLINE_PICTURE_INFO *snappictureInfo = (NET_SDK_PASSLINE_PICTURE_INFO*)(pBuf+sizeof(NET_SDK_IVE_PASSLINECOUNT_T)+sizeof(NET_SDK_PASSLINE_PICTURE_INFO)+pictureInfo->iPicSize);
			TRACE("iHeight=%d, iWidth=%d, iPicSize=%d, iPicFormat=%d, \n",pictureInfo->iHeight, pictureInfo->iWidth, pictureInfo->iPicSize, pictureInfo->iPicFormat);
			if(dwBufLen >= (sizeof(NET_SDK_IVE_PASSLINECOUNT_T)+sizeof(NET_SDK_PASSLINE_PICTURE_INFO)+pictureInfo->iPicSize+sizeof(NET_SDK_PASSLINE_PICTURE_INFO)+snappictureInfo->iPicSize))
			{
				if (snappictureInfo->iPicSize > 0)
				{
					FILE *backfp = fopen("./snapPicture.jpg", "wb");
					if(backfp)
					{
						int fret = fwrite(pBuf+(sizeof(NET_SDK_IVE_PASSLINECOUNT_T)+sizeof(NET_SDK_PASSLINE_PICTURE_INFO)+pictureInfo->iPicSize+sizeof(NET_SDK_PASSLINE_PICTURE_INFO)), snappictureInfo->iPicSize, 1, backfp );
						fclose(backfp);
					}
				}
			}

			/*if(pThis->m_CurDlg)
			{
			pThis->m_CurDlg->UpdateMsg( lUserID, dwCommand, pBuf, dwBufLen);
			}*/
			break;
		}
	case NET_SDK_SMART_EVENT_TYPE_TRAFFIC:
		{
			//IVE_TRAFFIC_T+ IVE_EventData_T+原图+ IVE_EvenTargetData_T+目标图+ IVE_EvenTargetData_T+目标图
			TRACE("NET_SDK_SMART_EVENT_TYPE_TRAFFIC dwBufLen=%d\n", dwBufLen);
			if(dwBufLen < sizeof(NET_SDK_IVE_TRAFFIC_T))
			{
				break;
			}
			char* tmp = pBuf;
			int nLeftBufLen = dwBufLen;
			//区域统计（流量统计）
			NET_SDK_IVE_TRAFFIC_T* head = (NET_SDK_IVE_TRAFFIC_T*)tmp;

			tmp += sizeof(NET_SDK_IVE_TRAFFIC_T);
			nLeftBufLen -= sizeof(NET_SDK_IVE_TRAFFIC_T);
			//原图
			NET_SDK_IVE_EventData_T *eventData = (NET_SDK_IVE_EventData_T*)tmp;
			tmp += sizeof(NET_SDK_IVE_EventData_T);
			nLeftBufLen -= sizeof(NET_SDK_IVE_EventData_T);
			if (eventData->dataLen > 0)
			{
				char* img = new char[eventData->dataLen];
				memcpy(img, tmp, eventData->dataLen);

				FILE *backfp = fopen("./Traffic_srcImg.jpg", "wb");
				if(backfp)
				{
					int fret = fwrite(img, eventData->dataLen, 1, backfp );
					fclose(backfp);
				}
				delete img;
				tmp += eventData->dataLen;
				nLeftBufLen -= eventData->dataLen;
			}
			//目标图
			while (nLeftBufLen > sizeof(NET_SDK_IVE_EvenTargetData_T))
			{
				NET_SDK_IVE_EvenTargetData_T *eventTargetData = (NET_SDK_IVE_EvenTargetData_T*)tmp;
				tmp += sizeof(NET_SDK_IVE_EvenTargetData_T);
				nLeftBufLen -= sizeof(NET_SDK_IVE_EvenTargetData_T);
				if (eventTargetData->dataLen > 0)
				{
					char* img = new char[eventTargetData->dataLen];
					memcpy(img, tmp, eventTargetData->dataLen);

					FILE *backfp = fopen("./Traffic_targetImg.jpg", "wb");
					if(backfp)
					{
						int fret = fwrite(img, eventTargetData->dataLen, 1, backfp );
						fclose(backfp);
					}
					delete img;
					tmp += eventTargetData->dataLen;
					nLeftBufLen -= eventTargetData->dataLen;
				}
			}
		}
	//case NET_SDK_SMART_EVENT_TYPE_DOOR_BELL:
	//	{
	//		CString temp;
	//		temp.Format("device event|IP:%s|Type:%s", pThis->m_ipMap[lUserID], "NET_SDK_SMART_EVENT_TYPE_DOOR_BELL");
	//		pThis->m_msgDlg->SendMessage(WM_MSG_CALLBACK, 0, (LPARAM)temp.GetBuffer());
	//		::OutputDebugString(temp);
	//		break;
	//	}
		break;
	}
}

LRESULT CSDKDEMODlg::OnShowMsgDlg(WPARAM wparam, LPARAM lparam)
{
	int ifShow = (int)lparam;
	if (ifShow)
	{
		m_msgDlg->ShowWindow(SW_SHOW);
	}
	else
	{
		m_msgDlg->ShowWindow(SW_HIDE);
	}
	return 0;
}
void CSDKDEMODlg::OnBnClickedButtonLogtest()
{
#if 0
	char szPath[MAX_PATH] = {0};
	DWORD dwLen = ::GetModuleFileName(NULL, szPath, MAX_PATH);
	if (dwLen == 0)
		return;
	int nIndex = dwLen - 1;
	while(nIndex > 0 && szPath[nIndex] != '\\')
	{
		szPath[nIndex] = '\0';
		nIndex--;
	}
	strcat(szPath, CONFIG_FILE_NAME);
	
	for (int i = 1; i <= 3; i++)
	{
		char szKey[16] = {0};
		char szBuf[256] = {0};
		sprintf_s(szKey, sizeof(szKey), "%d", i);
		::GetPrivateProfileString("MultiLogin", szKey, "", szBuf, sizeof(szBuf), szPath);
		CString strSource = szBuf;
		CString strIP, strPort, strUserName, strPasswd, strSN, strConnectType;
		AfxExtractSubString(strIP, strSource, 0, '|');
		AfxExtractSubString(strPort, strSource, 1, '|');
		AfxExtractSubString(strUserName, strSource, 2, '|');
		AfxExtractSubString(strPasswd, strSource, 3, '|');
		AfxExtractSubString(strSN, strSource, 4, '|');
		AfxExtractSubString(strConnectType, strSource, 5, '|');

		NET_SDK_LOGIN_INFO* pLoginInfo = new NET_SDK_LOGIN_INFO;
		memset(pLoginInfo, 0x00, sizeof(NET_SDK_LOGIN_INFO));
		strcpy(pLoginInfo->deviceIP, strIP.GetBuffer());
		pLoginInfo->devicePort = atoi(strPort.GetBuffer());
		strcpy(pLoginInfo->userName, strUserName.GetBuffer());
		strcpy(pLoginInfo->passwd, strPasswd.GetBuffer());
		strcpy(pLoginInfo->deviceSN, strSN.GetBuffer());
		pLoginInfo->connectType = atoi(strConnectType);

		HANDLE hThread = ::CreateThread(NULL, 0, OnLoginThread, (void*)pLoginInfo, NULL, NULL);
		::CloseHandle(hThread);

		::Sleep(2000);
	}
	
#else
	//登录
	CLoginDlg loginDlg;	
	loginDlg.SetIp("192.168.2.120");
	loginDlg.SetPort(6036);

	while(true)
	{
		if (IDOK == loginDlg.DoModal())
		{
#ifdef test_ipv6
			LONG userId = NET_SDK_LoginEx("[fe80::14eb:95ff:fe40:c812]"/*loginDlg.GetIP().GetBuffer()*/, loginDlg.GetPort(), loginDlg.GetUserName().GetBuffer(), loginDlg.GetPassword().GetBuffer(), &m_deviceInfo,\
				loginDlg.GetConnectType(), loginDlg.GetDeviceSN().GetBuffer());
#else
			LONG userId = NET_SDK_LoginEx(loginDlg.GetIP().GetBuffer(), loginDlg.GetPort(), loginDlg.GetUserName().GetBuffer(), loginDlg.GetPassword().GetBuffer(), &m_deviceInfo,\
				loginDlg.GetConnectType(), loginDlg.GetDeviceSN().GetBuffer());
			//LONG userId = NET_SDK_Login(loginDlg.GetIP().GetBuffer(), loginDlg.GetPort(), loginDlg.GetUserName().GetBuffer(), loginDlg.GetPassword().GetBuffer(), &m_deviceInfo);
#endif
			if (userId >= 0)
			{
				m_userID = userId;
				m_deviceInfo.deviceIP = inet_addr(loginDlg.GetIP());
				m_liveDlg->SetDeviceInfo(&m_deviceInfo, m_userID);
				m_searchDlg->SetDeviceInfo(&m_deviceInfo, m_userID);
				m_toolDlg->setDeviceInfo(&m_deviceInfo, m_userID);
				m_configDlg->setDeviceInfo(&m_deviceInfo, m_userID);
				m_faceMatchDlg->setDeviceInfo(&m_deviceInfo, m_userID);
				m_smartDlg->setDeviceInfo(&m_deviceInfo, m_userID);
				m_frTemperatureDlg->setDeviceInfo(&m_deviceInfo, m_userID);
				m_faceMatchAlarmDisplayDlg->setDeviceInfo(&m_deviceInfo, m_userID);
				//m_othersDlg->setDeviceInfo(&m_deviceInfo, m_userID);
				break;
			}
			else
			{
				LONG error = NET_SDK_GetLastError();
				CString msg = NET_SDK_GetErrorMsg(&error);
				if (error == NET_SDK_DEVICE_OFFLINE)
				{
					AfxMessageBox("Device offline!");
				}
				else
				{
					AfxMessageBox(STR_LOGIN_FAILED);
				}
			}
		}
		else
		{
			//OnCancel();
			break;
		}
	}

#endif
}

DWORD WINAPI CSDKDEMODlg::OnLoginThread(LPVOID lParam)
{
	NET_SDK_LOGIN_INFO *pLoginInfo = static_cast<NET_SDK_LOGIN_INFO*>(lParam);
	if (!pLoginInfo)
	{
		return -1;
	}

	NET_SDK_DEVICEINFO deviceInfo = {0};
	DWORD dwTime = ::GetTickCount();
	TRACE("NET_SDK_Login begin, IP=%s,SN=%s, connectType=%d \r\n", pLoginInfo->deviceIP, pLoginInfo->deviceSN, pLoginInfo->connectType);
#ifdef test_ipv6
	LONG lUserID = NET_SDK_LoginEx("[fe80::14eb:95ff:fe40:c812]"/*pLoginInfo->deviceIP*/, pLoginInfo->devicePort, pLoginInfo->userName, pLoginInfo->passwd, &deviceInfo,\
		(NET_SDK_CONNECT_TYPE)pLoginInfo->connectType, pLoginInfo->deviceSN);
#else
	LONG lUserID = NET_SDK_LoginEx(pLoginInfo->deviceIP, pLoginInfo->devicePort, pLoginInfo->userName, pLoginInfo->passwd, &deviceInfo,\
		(NET_SDK_CONNECT_TYPE)pLoginInfo->connectType, pLoginInfo->deviceSN);
#endif
	TRACE("NET_SDK_Login end, IP=%s,SN=%s, connectType=%d, use time=%d \r\n", pLoginInfo->deviceIP, pLoginInfo->deviceSN, pLoginInfo->connectType, GetTickCount()-dwTime);

	if (pLoginInfo)
	{
		delete pLoginInfo;
		pLoginInfo = NULL;
	}
	return 0;
}