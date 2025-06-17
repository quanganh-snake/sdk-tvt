// ToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "ToolDlg.h"
#include "GridCellCombo.h"
#include "FormatDlg.h"
#include <WinSock2.h>
#include <Wincrypt.h>
#include "strdef.h"
#include <MMSystem.h>

#include <Iphlpapi.h>
using namespace std;
#pragma comment(lib,"Iphlpapi.lib")

TCHAR *logTypeStr[9][70] = {
	{
		STR_SYSTEM_BOOT,
		STR_SYSTEM_SHUTDOWN,
		STR_SYSTEM_REBOOT,
		STR_FORMAT_DISK_SUCCESS,
		STR_FORMAT_DISK_FAILED,
		STR_UPDATE_SUCCESS,
		STR_UPDATE_FAILED,
		STR_CLEAR_ALARM,
		STR_OPEN_ALARM,
		STR_OPEN_MANUAL_RECORD,
		STR_STOP_MANUAL_RECORD,
		STR_PTZ_CTRL,
		STR_PTZ_OPERATE,
		STR_PTZ_CTRL_FIN,
		STR_CHANGE_AUDIO,
		STR_CHANGE_VOLUME,
		STR_START_MUTE,
		STR_STOP_MUTE,
		STR_START_POLL,
		STR_STOP_POLL,
		STR_LOGIN,
		STR_LOGOUT,
		STR_CHANGE_SYS_TIME,
		STR_SNAP_SUCCESS,
		STR_SNAP_FAILED
	},

	{
		STR_CHANGE_VIDEO_TYPE,
		STR_CHANGE_VIDEO_RESOLUTION,
		STR_CHANGE_LANUAGE,
		STR_CHANGE_USER_NAME,
		STR_CHANGE_TIMEZONE,
		STR_CHANGE_TIME,
		STR_START_AUTO_ADJUST_TIME,
		STR_STOP_AUTO_ADJUST_TIME,
		STR_CHANGE_NTP_SERVER,
		STR_CHANGE_DST,
		STR_START_PASSWORD,
		STR_STOP_PASSWORD,

		STR_CHANGE_CHANNEL_NAME,
		STR_CHANGE_COLOR,
		STR_SET_MAIN_MONITOR,
		STR_SET_SPOT_MONITOR,
		STR_SET_OSD,

		STR_SET_REC_ENCODE,
		STR_SET_REC_ON,
		STR_SET_REC_AUDIO_ON,
		STR_SET_REREC_ON,
		STR_SET_PRE_ALARM_TIME,
		STR_SET_POSTS_ALARM_TIME,
		STR_SET_REC_EXPIRE_TIME,

		STR_SET_SCHEDULE_PLAN,
		STR_SET_MOTION_PLAN,
		STR_SET_ALARM_REC_PLAN,

		STR_SET_ALARM_IN_ON,
		STR_SET_ALARM_IN_TYPE,
		STR_SET_ALARM_IN_APPROACH,
		STR_SET_ALARM_IN_PLAN,

		STR_SET_MOTION_ON,
		STR_SET_MOTION_SEN,
		STR_SET_MOTION_AREA,
		STR_SET_MOTION_APPROACH,
		STR_SET_MOTION_PLAN,

		STR_SET_VLOSS_APPROACH,

		STR_SET_ALARM_OUT_ON,
		STR_SET_ALARM_OUT_PLAN,

		STR_START_AUDIO_ALARM,
		STR_STOP_AUDIO_ALARM,
		STR_SET_AUDIO_ALARM_PLAN,

		STR_CHANGE_HTTP_PORT,
		STR_CHANGE_NETWORK_PORT,
		STR_SET_IP,
		STR_DHCP_SUCCESS,
		STR_DHCP_FAILED,
		STR_SET_PPPOE,
		STR_SET_DDNS,
		STR_SET_NET_ENCODE,

		STR_CHANGE_SERIAL_PORT,
		STR_CHANGE_PRESET,
		STR_CHANGE_CRUISE,
		STR_CHANGE_TRACK,

		STR_ADD_USER,
		STR_CHANGE_AUTH,
		STR_DEL_USER,
		STR_CHANGE_PASSWORD,

		STR_DEFAULT_CONFIG,
		STR_IMPORT_CONFIG,
		STR_EXPORT_CONFIG,

		STR_SHELTER,
		STR_LOOP_RECORD_START,
		STR_LOOP_RECORD_STOP,
		STR_ADJUST_DISK_ALARM,

		STR_SET_EMAIL_SND,
		STR_SET_EMAIL_RECV,
		STR_SET_SNAP
	},

	{
		STR_PLAY,
		STR_PAUSE,
		STR_RESUME,
		STR_FF,
		STR_REW,
		STR_STOP,
		STR_NEXT,
		STR_PRE
	},

	{
		STR_BACKUP_START,
		STR_BACKUP_FIN,
		STR_BACKUP_CANCEL,
		STR_BACKUP_FAILED
	},

	{
		STR_SEARCH_TIME,
		STR_SEARCH_EVENT,
		STR_SEARCH_FILE,
		STR_SEARCH_PIC,
		STR_DEL_FILE,
		STR_LOCK_FILE,
		STR_UNLOCK_FILE,
		STR_DEL_PIC,
		STR_LOCK_PIC,
		STR_UNLOCK_PIC
	},

	{
		STR_SYS_INFO,
		STR_EVENT_INFO,
		STR_EXPORT_LOG,
		STR_NETWORK_INFO,
		STR_ONLINE_USER,
		STR_EXPORT_LOG,
		STR_EXPORT_EVENT
	},

	{
		STR_SENSOR_ALARM_START,
		STR_SENSOR_ALARM_FIN,
		STR_MOTION_START,
		STR_MOTION_STOP,
		STR_VLOSS_START,
		STR_VLOSS_STOP,
		STR_SHELTER_START,
		STR_SHELTER_STOP
	},

	{
		_T("adfsdfsd"),
	},

	{
		STR_IP_CONFLICT,
		STR_NET_WORK_ERROR,
		STR_DDNS_ERROR,
		STR_DISK_ERROR,
		STR_POWER_OFF,
		STR_UNKNOWN_ERROR	
	}
};

// CToolDlg 对话框

IMPLEMENT_DYNAMIC(CToolDlg, CDialog)

CToolDlg::CToolDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CToolDlg::IDD, pParent)
	, m_logStartTime(0)
	, m_logEndTime(0)
	, m_isStartVoiceComm(FALSE)
	, m_isStartVoiceComm_MR(FALSE)
	, m_isOpenMsgCallback(FALSE)
	, m_runTalkTransThread(FALSE)
	, m_talkDataFile(INVALID_HANDLE_VALUE)
	, m_voiceHandle(-1)
	, m_audioDecoderHandle(-1)
	, m_voiceVolume(0)
	, m_SubscribAVD(0)
	, m_SubscribVFD(0)
	, m_SubscribPEA(0)
	, m_SubscribOSC(0)
	, m_SubscribFaceMatch(0)
	, m_SubscribVehicle(0)
	, m_SubscribAOIENTRY(0)
	, m_SubscribAOILEAVE(0)
	, m_SubscribPassLine(0)
	, m_sysTime(CTime::GetCurrentTime())
	,searched(false)
	,m_nSearchDevice(0)
	, m_strPwd(_T(""))
{

#ifdef N9000_CONFIG
	InitN9000LogTypeList(); //初始化N9000日志列表
#endif
	m_WaveIn.SetBuffSize(3200);
	m_WaveIn.SetBuffNum(4);

}

CToolDlg::~CToolDlg()
{
}

void CToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_LOG, m_logList);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_START_TIME, m_logStartTime);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_END_TIME, m_logEndTime);
	DDX_Slider(pDX, IDC_SLIDER_VOICECOMM_VOLUME, m_voiceVolume);
	DDX_Control(pDX, IDC_DATETIMEPICKER_START_TIME, m_startTimeCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER_END_TIME, m_endTimeCtrl);
	DDX_Control(pDX, IDC_DATETIMEPICKER_SYSTIME, m_systimeCtrl);
	DDX_DateTimeCtrl(pDX, IDC_DATETIMEPICKER_SYSTIME, m_sysTime);
	DDX_GridControl(pDX, IDC_CUSTOM_DISK_LIST, m_diskGrid);
	DDX_Control(pDX, IDC_LIST_SEARCHDEVICE, m_listSearchDevice);
	DDX_Text(pDX, IDC_EDIT_ACTIVEPWD, m_strPwd);
	DDX_Control(pDX, IDC_EDIT_ACTIVEPWD, m_editActivePwd);
	DDX_Control(pDX, IDC_COMBO_CHANNEL, m_comChannel);
}


BEGIN_MESSAGE_MAP(CToolDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CToolDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CToolDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_SEARCHLOG, &CToolDlg::OnBnClickedButtonSearchlog)
	ON_BN_CLICKED(IDC_BUTTON_VOICE_COMM, &CToolDlg::OnBnClickedButtonVoiceComm)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_BUTTON_CHANGESYSTIME, &CToolDlg::OnBnClickedButtonChangesystime)
	ON_BN_CLICKED(IDC_BUTTON_DISK_REFRESH, &CToolDlg::OnBnClickedButtonDiskRefresh)
	ON_BN_CLICKED(IDC_BUTTON_CHANGE_DISK_PROPERTY, &CToolDlg::OnBnClickedButtonChangeDiskProperty)
	ON_BN_CLICKED(IDC_BUTTON_DISK_FORMAT, &CToolDlg::OnBnClickedButtonDiskFormat)
	ON_BN_CLICKED(IDC_BUTTON_MSG_CALLBACK, &CToolDlg::OnBnClickedButtonMsgCallback)
	ON_BN_CLICKED(IDC_BUTTON_TALKT_RANS, &CToolDlg::OnBnClickedButtonTalktRans)
	ON_BN_CLICKED(IDC_BUTTON_GetSYSTIME, &CToolDlg::OnBnClickedButtonGetsystime)
	ON_BN_CLICKED(IDC_BtnReboot, &CToolDlg::OnBnClickedBtnreboot)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_AVD, &CToolDlg::OnBnClickedButtonSmartSubscribAVD)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_VFD, &CToolDlg::OnBnClickedButtonSmartSubscribVFD)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_VFD_MATCH, &CToolDlg::OnBnClickedButtonSmartSubscribFaceMatch)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_OSC, &CToolDlg::OnBnClickedButtonSmartSbscribOsc)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_PEA, &CToolDlg::OnBnClickedButtonSmartSbscribPea)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_VIHICLE, &CToolDlg::OnBnClickedButtonSmartSbscribVihicle)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_AOIENTRY, &CToolDlg::OnBnClickedButtonSmartSbscribAoientry)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_AOILEAVE, &CToolDlg::OnBnClickedButtonSmartSbscribAoileave)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_PASSLINE, &CToolDlg::OnBnClickedButtonSmartSbscribPassline)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH_DEVICE, &CToolDlg::OnBnClickedButtonSearchDevice)
	ON_BN_CLICKED(IDC_BUTTON_SET_LOG, &CToolDlg::OnBnClickedButtonSetLog)
	ON_BN_CLICKED(IDC_BUTTON_SEARCH_ACTIVEDEV, &CToolDlg::OnBnClickedButtonSearchActivedev)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE_LOGPATH, &CToolDlg::OnBnClickedButtonBrowseLogpath)
	ON_BN_CLICKED(IDC_CHECK_TALK_TO_CHANNEL, &CToolDlg::OnBnClickedCheckTalkToChannel)
END_MESSAGE_MAP()


// CToolDlg 消息处理程序

void CToolDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CToolDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}

void CToolDlg::InitN9000LogTypeList()
{
	//报警日志
	m_strMinorLogTypeList[LOG_ALARM_MOTION]				= "移动侦测报警";
	m_strMinorLogTypeList[LOG_ALARM_SENSOR]				= "传感器报警";
	m_strMinorLogTypeList[LOG_ALARM_ALARMOUTPUT]		= "报警输出";
	m_strMinorLogTypeList[LOG_ALARM_OCCLUSION]		= "遮蔽报警";
	m_strMinorLogTypeList[LOG_ALARM_VLOSS]			= "视频丢失报警";
	m_strMinorLogTypeList[LOG_ALARM_VEHICLE_PLATE_MATCH]	= "车牌识别报警";
	m_strMinorLogTypeList[LOG_ALARM_INTELLIGENT]		= "智能侦测报警";
	m_strMinorLogTypeList[LOG_ALARM_FACE_MATCH]		= "人脸对比报警";
	m_strMinorLogTypeList[LOG_ALARM_COMBINED]		= "组合报警";
	//操作日志
	m_strMinorLogTypeList[LOG_OPERATE_RECORD_SPB]		= "录像检索/回放/备份";
	m_strMinorLogTypeList[LOG_OPERATE_MANUAL_RECORD]	= "手动录像";
	m_strMinorLogTypeList[LOG_OPERATE_MANUAL_ALARM]	= "手动报警";
	m_strMinorLogTypeList[LOG_OPERATE_SYSTEM_MAINTENANCE] = "系统维护";
	m_strMinorLogTypeList[LOG_OPERATE_PTZ_CONTROL]		= "云台控制";
	m_strMinorLogTypeList[LOG_OPERATE_AUDIO_TALK]		= "语音对讲";
	m_strMinorLogTypeList[LOG_OPERATE_SYSTEM_SCR]		= "开关机";
	m_strMinorLogTypeList[LOG_OPERATE_LOGIN_LOGOUT]		= "登录/登出";
	m_strMinorLogTypeList[LOG_OPERATE_SNAPSHOT_MSPB]	= "图片";
	m_strMinorLogTypeList[LOG_OPERATE_FORMAT_HD]		= "格式化磁盘";
	m_strMinorLogTypeList[LOG_OPERATE_HDD_INSERT]	= "磁盘插入";
	m_strMinorLogTypeList[LOG_OPERATE_FEATURELIBRARY]		= "特征库增加";
	//设置日志
	m_strMinorLogTypeList[LOG_CONFIG_CHANNEL]			= "通道参数";
	m_strMinorLogTypeList[LOG_CONFIG_RECORD]			= "录像参数";
	m_strMinorLogTypeList[LOG_CONFIG_ALARM]				= "报警参数";
	m_strMinorLogTypeList[LOG_CONFIG_DISK]				= "磁盘参数";
	m_strMinorLogTypeList[LOG_CONFIG_NETWORK]			= "网络参数";
	m_strMinorLogTypeList[LOG_CONFIG_SCHEDULE]			= "排程参数";
	m_strMinorLogTypeList[LOG_CONFIG_USER]				= "用户参数";
	m_strMinorLogTypeList[LOG_CONFIG_BASIC]				= "基本配置";
	//异常日志
	m_strMinorLogTypeList[LOG_EXCEPTION_UNLAWFUL_ACCESS] = "非法访问";
	m_strMinorLogTypeList[LOG_EXCEPTION_DISK_FULL]		= "磁盘满";
	m_strMinorLogTypeList[LOG_EXCEPTION_DISK_IO_ERROR]	= "磁盘读写出错";
	m_strMinorLogTypeList[LOG_EXCEPTION_IP_COLLISION]	= "IP地址冲突";
	m_strMinorLogTypeList[LOG_EXCEPTION_INTERNET_DISCONNECT] = "网络断开";
	m_strMinorLogTypeList[LOG_EXCEPTION_IPC_DISCONNECT] = "前端掉线";
	m_strMinorLogTypeList[LOG_EXCEPTION_ABNORMAL_SHUTDOWN] = "系统异常关机";
	m_strMinorLogTypeList[LOG_EXCEPTION_NO_DISK]		= "无磁盘";
	m_strMinorLogTypeList[LOG_EXCEPTION_VIDEO_LOSS]		= "视频丢失";
}

void CToolDlg::OnBnClickedButtonSearchlog()
{
	UpdateData();
	m_logList.DeleteAllItems();

	DWORD type = 0x0;

#ifdef N9000_CONFIG
	//N9000_LOG_MAJOR_TYPE
	type = LOG_ALL;
#else
	CButton *button;
	for (int i = IDC_CHECK_LOG_SYSOP; i <= IDC_CHECK_LOG_ERROR; i++)
	{
		button = reinterpret_cast<CButton*>(GetDlgItem(i));
		if (button->GetCheck())
		{
			type |= (0x1) << (i - IDC_CHECK_LOG_SYSOP); 
		}
	}
#endif

	DD_TIME start = {m_logStartTime.GetSecond(), m_logStartTime.GetMinute(), m_logStartTime.GetHour(), 0,
			 m_logStartTime.GetDay(), m_logStartTime.GetMonth() - 1, m_logStartTime.GetYear() - 1900};

	DD_TIME end = {m_logEndTime.GetSecond(), m_logEndTime.GetMinute(), m_logEndTime.GetHour(), 0,
		m_logEndTime.GetDay(), m_logEndTime.GetMonth() - 1, m_logEndTime.GetYear() - 1900};

	LONG loghandle = NET_SDK_FindDVRLog(m_userID, type, &start, &end);
	LONG result = 0;

	NET_SDK_LOG log;
	
	if (loghandle > 0)
	{
		while (true)
		{
			result = NET_SDK_FindNextLog(loghandle, &log);
			if (result == NET_SDK_FILE_NOFIND)
			{
				AfxMessageBox(STR_FIND_NO_LOG);
				break;
			}
			if (result == NET_SDK_NOMOREFILE)
			{
				break;
			}

			int n = m_logList.GetItemCount();
			m_logList.InsertItem(n, _T(""));
			 
#ifdef N9000_CONFIG
			//日志子类型
			string strlogType;
			map<int, string>::iterator pIter = m_strMinorLogTypeList.find(log.dwMinorType);
			if (pIter != m_strMinorLogTypeList.end())
			{
				strlogType = pIter->second;
			}
			m_logList.SetItemText(n, 0, strlogType.c_str());
#else
			m_logList.SetItemText(n, 0, logTypeStr[log.dwMajorType - 1][log.dwMinorType - 1]);
#endif
			
			m_logList.SetItemText(n, 1, log.sNetUser);
			CTime time(log.strLogTime.year + 1900, log.strLogTime.month + 1, log.strLogTime.mday,
				log.strLogTime.hour, log.strLogTime.minute, log.strLogTime.second);
			m_logList.SetItemText(n, 2, time.Format(_T("%Y-%m-%d %H:%M:%S")));
			
			IN_ADDR addr;
			addr.S_un.S_addr = log.dwRemoteHostAddr;
			m_logList.SetItemText(n, 3, inet_ntoa(addr));

			m_logList.SetItemText(n, 4, log.sContent);
		}

		NET_SDK_FindLogClose(loghandle);
	}	
	else
	{
		AfxMessageBox(_T("Can not find Log!"));
	}
}

BOOL CToolDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_diskGrid.SetRowCount(1);
	m_diskGrid.SetFixedRowCount(1);
	m_diskGrid.SetListMode(TRUE);
	m_diskGrid.SetColumnCount(4);

	m_diskGrid.SetItemText(0, 0, STR_TOTAL_SIZE);
	m_diskGrid.SetItemText(0, 1, STR_LEFT_SIZE);
	m_diskGrid.SetItemText(0, 2, STR_STATUS);
	m_diskGrid.SetItemText(0, 3, STR_PROPERTY);

	m_diskGrid.SetColumnWidth(0, 80);
	m_diskGrid.SetColumnWidth(1, 80);
	m_diskGrid.SetColumnWidth(2, 68);
	m_diskGrid.SetColumnWidth(3, 68);

	m_logList.InsertColumn(0, STR_TYPE);
	m_logList.InsertColumn(1, STR_USERNAME);
	m_logList.InsertColumn(2, STR_TIME);
	m_logList.InsertColumn(3, STR_IP);
	m_logList.InsertColumn(4, STR_CONTENT);

	m_logList.SetColumnWidth(0, 120);
	m_logList.SetColumnWidth(1, 75);
	m_logList.SetColumnWidth(2, 130);
	m_logList.SetColumnWidth(3, 130);
	m_logList.SetColumnWidth(4, 200);

	m_listSearchDevice.InsertColumn(0, "IP");
	m_listSearchDevice.InsertColumn(1, STR_HTTP_PORT);
	m_listSearchDevice.InsertColumn(2, STR_ACTIVATED_STATUS);
	m_listSearchDevice.InsertColumn(3, STR_SECOND_IP);
	m_listSearchDevice.InsertColumn(4, STR_DEVICE_TYPE);
	m_listSearchDevice.InsertColumn(5, STR_PRODUCT_TYPE);
	m_listSearchDevice.InsertColumn(6, STR_MAC_ADDR);
	m_listSearchDevice.InsertColumn(7, STR_PASSWORD_LEVEL);

	m_listSearchDevice.SetColumnWidth(0, 90);
	m_listSearchDevice.SetColumnWidth(1, 0);
	m_listSearchDevice.SetColumnWidth(2, 65);
	m_listSearchDevice.SetColumnWidth(3, 0);
	m_listSearchDevice.SetColumnWidth(4, 80);
	m_listSearchDevice.SetColumnWidth(5, 85);
	m_listSearchDevice.SetColumnWidth(6, 110);
	m_listSearchDevice.SetColumnWidth(7, 50);

	m_logList.SetExtendedStyle(m_logList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_listSearchDevice.SetExtendedStyle(m_logList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

	m_editActivePwd.SetLimitText(16);

	m_startTimeCtrl.SetFormat(_T("yyyy-MM-dd HH:mm:ss"));
	m_endTimeCtrl.SetFormat(_T("yyyy-MM-dd HH:mm:ss"));
	m_systimeCtrl.SetFormat(_T("yyyy-MM-dd HH:mm:ss"));

	SYSTEMTIME st;
	CTime::GetCurrentTime().GetAsSystemTime(st);
	st.wHour = 0;
	st.wSecond = 0;
	st.wMinute = 0;
	m_logStartTime = CTime(st);
	st.wHour = 23;
	st.wSecond = 59;
	st.wMinute = 59;
	m_logEndTime = CTime(st);
	
	m_voiceVolume = 50;
	
	GetDlgItem(IDC_EDIT_LOGPATH)->SetWindowText("D:\\");
	CComboBox* pLogLevel = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_LOG_LEVEL));
	if (pLogLevel)
	{

		pLogLevel->AddString(_T("0"));
		pLogLevel->AddString(_T("1"));
		pLogLevel->AddString(_T("2"));
		pLogLevel->AddString(_T("3"));
		pLogLevel->AddString(_T("4"));
		pLogLevel->AddString(_T("5"));
		pLogLevel->SetCurSel(4);
	}

	UpdateData(FALSE);
	CButton *button = NULL;
	for (int i = IDC_CHECK_LOG_SYSOP; i <= IDC_CHECK_LOG_ERROR; i++)
	{	
		button = reinterpret_cast<CButton *>(GetDlgItem(i));
		button->SetCheck(TRUE);
	}
	UpdateUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CToolDlg::OnBnClickedButtonVoiceComm()
{
	if (m_isStartVoiceComm_MR)
	{
		return;
	}
	if (!m_isStartVoiceComm)
	{
		LONG lChannel = -1;
		if (reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_TALK_TO_CHANNEL))->GetCheck())
		{
			lChannel = m_comChannel.GetCurSel();
		}
		LONG handle = NET_SDK_StartVoiceCom(m_userID, FALSE, NULL, NULL, lChannel);
		if (handle > 0)
		{
			m_voiceHandle = handle;
			m_isStartVoiceComm = TRUE;
		}
		else
		{
			AfxMessageBox(STR_START_FAILED);
		}
	}
	else
	{
		NET_SDK_StopVoiceCom(m_voiceHandle);
		m_isStartVoiceComm = FALSE;
		m_voiceHandle = -1;
	}
	UpdateUI();
}

void CToolDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	CPoint point;
	GetCursorPos(&point);

	CRect rect;
	GetDlgItem(IDC_SLIDER_VOICECOMM_VOLUME)->GetWindowRect(&rect);
	
	UpdateData();

	if (rect.PtInRect(point))
	{
		unsigned short volume = 0;
		double temp = (((double)m_voiceVolume) / 100) * 0xffff;
		volume = (short )temp;

		NET_SDK_SetVoiceComClientVolume(m_voiceHandle, volume);	
	}

	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CToolDlg::UpdateUI()
{
	if (m_isStartVoiceComm)
	{
		SetDlgItemText(IDC_BUTTON_VOICE_COMM, STR_CLOSE);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_VOICE_COMM, STR_START);
	}

	if (m_isStartVoiceComm_MR)
	{
		SetDlgItemText(IDC_BUTTON_TALKT_RANS, STR_CLOSE);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_TALKT_RANS, STR_TRANS);
	}
	
	if (m_isOpenMsgCallback)
	{
		SetDlgItemText(IDC_BUTTON_MSG_CALLBACK, STR_CLOSE);
	}
	else
	{
		SetDlgItemText(IDC_BUTTON_MSG_CALLBACK, STR_START);
	}

	GetDlgItem(IDC_SLIDER_VOICECOMM_VOLUME)->EnableWindow(m_isStartVoiceComm);

	BOOL hasDisk = m_diskGrid.GetRowCount() > 1;
	
	GetDlgItem(IDC_BUTTON_CHANGE_DISK_PROPERTY)->EnableWindow(hasDisk);
	GetDlgItem(IDC_BUTTON_DISK_FORMAT)->EnableWindow(hasDisk);
	GetDlgItem(IDC_BUTTON_VOICE_COMM)->EnableWindow(!m_isStartVoiceComm_MR);
	GetDlgItem(IDC_BUTTON_TALKT_RANS)->EnableWindow(!m_isStartVoiceComm);
}

void CToolDlg::OnBnClickedButtonChangesystime()
{
	UpdateData();
	BOOL ret = NET_SDK_ChangTime(m_userID, m_sysTime.GetTime());
	if (!ret)
	{
		AfxMessageBox(STR_SET_TIME_FAILED);
	}
	else
	{
		AfxMessageBox(STR_SUCCESS, MB_ICONINFORMATION);
	}
}

void CToolDlg::OnBnClickedButtonDiskRefresh()
{
	m_diskGrid.DeleteNonFixedRows();
	m_diskList.RemoveAll();

	LONG diskHandle = NET_SDK_FindDisk(m_userID);
	if (diskHandle > 0)
	{
		while (true)
		{
			NET_SDK_DISK_INFO diskInfo;
			if (!NET_SDK_GetNextDiskInfo(diskHandle, &diskInfo))
			{
				NET_SDK_FindDiskClose(diskHandle);
				break;
			}

			m_diskList.AddTail(diskInfo);

			CString temp;
			int n = m_diskGrid.InsertRow(_T(""));

			temp.Format(_T("%0.2f"), (double)diskInfo.diskTotalSpace / 1024.0);
			m_diskGrid.SetItemText(n, 0, temp);
			m_diskGrid.SetItemState(n, 0, GVIS_READONLY);

			temp.Format(_T("%0.2f"), (double)diskInfo.diskFreeSpace / 1024.0);
			m_diskGrid.SetItemText(n, 1, temp);
			m_diskGrid.SetItemState(n, 1, GVIS_READONLY);

			if (diskInfo.diskStatus == NET_SDK_DISK_NORMAL)
			{
				temp = STR_NORMAL;
			}
			else if (diskInfo.diskStatus == NET_SDK_DISK_NO_FORMAT)
			{
				temp = STR_UNFORMAT;
			}
			else if (diskInfo.diskStatus == NET_SDK_DISK_EXCEPTION)
			{
				temp = "Unavailable";
			}
			m_diskGrid.SetItemText(n, 2, temp);
			m_diskGrid.SetItemState(n, 2, GVIS_READONLY);

			if (diskInfo.diskProperty == NET_SDK_DISK_READ_WRITE)
			{
				temp = STR_READ_WRITE;
			}
			else if (diskInfo.diskProperty == NET_SDK_DISK_ONLY_READ)
			{
				temp = STR_READ_ONLY;
			}
			else
			{
				temp = _T("");
			}

			m_diskGrid.SetItemText(n, 3, temp);
			m_diskGrid.SetCellType(n, 3, RUNTIME_CLASS(CGridCellCombo));
			CGridCellCombo *cell = (CGridCellCombo *)m_diskGrid.GetCell(n, 3);
			cell->SetStyle(cell->GetStyle() | CBS_DROPDOWNLIST);
			CStringArray options;
			temp = STR_READ_WRITE;
			options.Add(temp);
			temp = STR_READ_ONLY;
			options.Add(temp);
			cell->SetOptions(options);
		}
		m_diskGrid.Refresh();
	}
	else
	{
		AfxMessageBox(STR_FIND_DISK_FAILED);
	}
	UpdateUI();
}

void CToolDlg::OnBnClickedButtonChangeDiskProperty()
{
	int n = m_diskGrid.GetFocusCell().row;
	if (n <= 0)
	{
		AfxMessageBox(STR_SELECT_DISK);
		return ;
	}
	NET_SDK_DISK_INFO &diskInfo = m_diskList.GetAt(m_diskList.FindIndex(n - 1));
	CString temp = m_diskGrid.GetItemText(n, 3);
	LONG newProperty = -1;
	if (temp.CompareNoCase(STR_READ_ONLY) == 0)
	{
		newProperty = NET_SDK_DISK_ONLY_READ;
	}
	else if (temp.CompareNoCase(STR_READ_WRITE) == 0)
	{
		newProperty = NET_SDK_DISK_READ_WRITE;
	}

	if (diskInfo.diskProperty == newProperty)
	{
		AfxMessageBox(STR_PROPERTY_NOT_CHANGE);
		return ;
	}
	if (AfxMessageBox(STR_CONFIRM_MODIFY_DISK, MB_YESNO) == IDNO)
	{
		return;
	}
	
	BOOL ret = NET_SDK_ChangeDiskProperty(m_userID, diskInfo.diskIndex, newProperty);
	if (ret)
	{
		diskInfo.diskProperty = newProperty;
		AfxMessageBox(STR_SUCCESS, MB_ICONINFORMATION);
	}
	else
	{
		if (diskInfo.diskProperty == NET_SDK_DISK_READ_WRITE)
		{
			temp = STR_READ_WRITE;
		}
		else if (diskInfo.diskProperty == NET_SDK_DISK_ONLY_READ)
		{
			temp = STR_READ_ONLY;
		}
		m_diskGrid.SetItemText(n, 3, temp);
		m_diskGrid.RedrawRow(n);
		AfxMessageBox(STR_MODIFY_FAILED);
	}
}

void CToolDlg::OnBnClickedButtonDiskFormat()
{
	int n = m_diskGrid.GetFocusCell().row;
	if (n <= 0)
	{
		AfxMessageBox(STR_SELECT_DISK);
		return ;
	}
	NET_SDK_DISK_INFO &diskInfo = m_diskList.GetAt(m_diskList.FindIndex(n - 1));
	if (AfxMessageBox(STR_CONFIRM_FORMAT, MB_YESNO) == IDNO)
	{
		return;
	}

	LONG formatHandle = NET_SDK_FormatDisk(m_userID, diskInfo.diskIndex);
	if (formatHandle > 0)
	{
		CFormatDlg dlg;
		dlg.SetFormatHandle(formatHandle);
		dlg.DoModal();
		NET_SDK_CloseFormatHandle(formatHandle);
	}
	else
	{
		AfxMessageBox(STR_FAILED);
	}
}

void CToolDlg::OnBnClickedButtonMsgCallback()
{
	if (m_isOpenMsgCallback)
	{
		NET_SDK_CloseAlarmChan(m_msgCBHandle);
		m_mainDlg->SendMessage(WM_SHOW_MSG_DLG, 0, 0);
		m_isOpenMsgCallback = FALSE;

		WaitForSingleObject(m_alarmStatusThread, INFINITE);

		

	}
	else
	{
		LONG handle  = NET_SDK_SetupAlarmChan(m_userID);
		if (handle > 0)
		{
			m_msgCBHandle = handle;
			m_mainDlg->SendMessage(WM_SHOW_MSG_DLG, 0, 1);
			m_isOpenMsgCallback = TRUE;
			m_alarmStatusThread = CreateThread(NULL, 0, &CToolDlg::AlarmStatusThread, this, 0, NULL);
		}
		else
		{
			AfxMessageBox(STR_FAILED);
		}
		
	}
	UpdateUI();
}

void CToolDlg::setMainDlg( CWnd* dlg )
{
	m_mainDlg = dlg;
}

void CToolDlg::OnBnClickedButtonTalktRans()
{
	if (m_isStartVoiceComm)
	{
		return;
	}

	if (m_isStartVoiceComm_MR)
	{
		if(m_audioDecoderHandle >0 )
		{
			//NET_SDK_ReleaseAudioDecoder(m_audioDecoderHandle);
			m_audioDecoderHandle = -1;
		}
		NET_SDK_StopVoiceCom(m_talkTransHandle);
		
		m_WaveIn.Stop();
		m_waveOut.Stop();
		m_runTalkTransThread = FALSE;
		//WaitForSingleObject(m_talkTransThread, INFINITE);
		m_isStartVoiceComm_MR = FALSE;
		m_talkTransHandle = -1;
	
	}
	else
	{
		LONG lChannel = -1;
		if (reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_TALK_TO_CHANNEL))->GetCheck())
		{
			lChannel = m_comChannel.GetCurSel();
		}
		LONG handle = NET_SDK_StartVoiceCom_MR(m_userID, TRUE, &CToolDlg::TalkTransCallBack, this, lChannel);
		if (handle > 0)
		{
			char *pAudioInfo = new char[20];
			BOOL bRet = NET_SDK_GetAudioInfo(handle, pAudioInfo, 20);
			if(bRet)
			{
				WAVEFORMATEX *pWaveFormat = (WAVEFORMATEX *)pAudioInfo;
				TRACE("tag:%d, bits:%d, bytes:%d\n", pWaveFormat->wFormatTag, pWaveFormat->wBitsPerSample, pWaveFormat->nAvgBytesPerSec);
				
			}
			delete [] pAudioInfo;
			m_talkTransHandle = handle;
			m_isStartVoiceComm_MR = TRUE;

			WAVEFORMATEX wfx;
			memset(&wfx, 0, sizeof(WAVEFORMATEX));
			wfx.wFormatTag = WAVE_FORMAT_PCM;
			wfx.nChannels = 1;
			wfx.nSamplesPerSec = 8000;
			wfx.wBitsPerSample = 16;
			wfx.nBlockAlign = 2;
			wfx.nAvgBytesPerSec = 16000;
			m_waveOut.Start(wfx);

			int ret = m_WaveIn.Start(wfx, &CToolDlg::TalkTransWaveInCallBack, this);
			if (ret != ERRNO_WAVEIN_OK)
			{
				NET_SDK_StopVoiceCom(m_talkTransHandle);

				m_WaveIn.Stop();
				m_waveOut.Stop();
				m_runTalkTransThread = FALSE;
				//WaitForSingleObject(m_talkTransThread, INFINITE);
				m_isStartVoiceComm_MR = FALSE;
				m_talkTransHandle = -1;
				return;
			}

			/*LONG audioDecoderHandle = NET_SDK_InitAudioDecoder( &pAudioFormat, sizeof(WAVEFORMATEX));
			if (handle > 0)
			{
				m_audioDecoderHandle = audioDecoderHandle;
			}*/

			//m_runTalkTransThread = TRUE;
			//m_talkTransThread = CreateThread(NULL, 0, &CToolDlg::TalkTransSendThread, this, 0, NULL);
			
			//m_talkDataFile = CreateFile(_T("D:\\TalkTrans.data"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		else
		{
			AfxMessageBox(STR_FAILED);
		}
	}

	/*if (m_isStartVoiceComm_MR)
	{
		NET_SDK_StopVoiceCom(m_talkTransHandle);
		m_runTalkTransThread = FALSE;
		WaitForSingleObject(m_talkTransThread, INFINITE);
		m_isStartVoiceComm_MR = FALSE;
		m_talkTransHandle = -1;
		CloseHandle(m_talkDataFile);
		m_talkDataFile = INVALID_HANDLE_VALUE;
	}
	else
	{
		LONG handle = NET_SDK_StartVoiceCom_MR(m_userID, TRUE, &CToolDlg::TalkTransCallBack, this);
		if (handle > 0)
		{
			char *pAudioInfo = new char[20];
			BOOL bRet = NET_SDK_GetAudioInfo(handle, pAudioInfo, 20);
			if(bRet)
			{
				WAVEFORMATEX *pWaveFormat = (WAVEFORMATEX *)pAudioInfo;
				TRACE("tag:%d, bits:%d, bytes:%d\n", pWaveFormat->wFormatTag, pWaveFormat->wBitsPerSample, pWaveFormat->nAvgBytesPerSec);
			}
			delete [] pAudioInfo;
			m_runTalkTransThread = TRUE;
			m_talkTransThread = CreateThread(NULL, 0, &CToolDlg::TalkTransSendThread, this, 0, NULL);
			m_talkTransHandle = handle;
			m_isStartVoiceComm_MR = TRUE;
			m_talkDataFile = CreateFile(_T("D:\\TalkTrans.data"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		}
		else
		{
			AfxMessageBox(STR_FAILED);
		}
	}*/
	UpdateUI();
}

//unsigned char buf[10000]={0};
void CToolDlg::TalkTransCallBack( POINTERHANDLE lVoiceComHandle, char *pRecvDataBuffer, DWORD dwBufSize, BYTE byAudioFlag, void* pUser )
{
	DWORD written = 0;
	CToolDlg *pThis = reinterpret_cast<CToolDlg *>(pUser);
	pThis->m_waveOut.WriteBuf((BYTE*)pRecvDataBuffer, dwBufSize);

	//WriteFile(pThis->m_talkDataFile, pRecvDataBuffer, dwBufSize, &written, NULL);
	//WAVEFORMATEX pAudioInfo;
	//BOOL bRet = NET_SDK_GetAudioInfo(pThis->m_talkTransHandle, &pAudioInfo, sizeof(WAVEFORMATEX));
	//if(bRet)
	//{
	//	//WAVEFORMATEX *pWaveFormat = (WAVEFORMATEX *)pAudioInfo;
	//	TRACE("tag:%d, bits:%d, bytes:%d\n", pAudioInfo.wFormatTag, pAudioInfo.wBitsPerSample, pAudioInfo.nAvgBytesPerSec);



	//}
	/*if(pThis->m_audioDecoderHandle>0)
	{
		int pOutLen = 0;
		NET_SDK_DecodeAudioFrame( pThis->m_audioDecoderHandle, (unsigned char *)pRecvDataBuffer, dwBufSize, buf, &pOutLen);
		TRACE("NET_SDK_DecodeAudioFrame dwBufSize = %d, pOutLen=%d \n", dwBufSize, pOutLen);
	}*/
}

void CToolDlg::TalkTransWaveInCallBack(unsigned char *pWaveInDataBuffer, unsigned int dwBufSize, void* pUser)
{
	DWORD written = 0;
	CToolDlg *pThis = reinterpret_cast<CToolDlg *>(pUser);
	BOOL bRet= NET_SDK_VoiceComSendData(pThis->m_talkTransHandle, (char *)pWaveInDataBuffer, dwBufSize);
}
DWORD WINAPI CToolDlg::TalkTransSendThread( LPVOID params )
{
	CToolDlg *pThis = reinterpret_cast<CToolDlg *>(params);
	pThis->TalkTransSendProc();
	return 0;
}

void CToolDlg::TalkTransSendProc()
{
	unsigned char *buff = new unsigned char[3200];
	HCRYPTPROV provider;
	CryptAcquireContext(&provider, NULL, NULL, PROV_RSA_FULL, 0);

	while(m_runTalkTransThread)
	{
		CryptGenRandom(provider, 3200, buff);
		NET_SDK_VoiceComSendData(m_talkTransHandle, (char *)buff, 3200);
		Sleep(100);
	}
	CryptReleaseContext(provider, 0);
	delete[] buff;
}

void CToolDlg::OnBnClickedButtonGetsystime()
{
	DD_TIME time = {0};
	int nTryTime = 10;
	BOOL bRet = FALSE;
	while(nTryTime > 0)
	{
		if(NET_SDK_GetDeviceTime(m_userID, &time))
		{
			bRet = TRUE;
			time.year += 1900; //年数需要加上1900偏移
			time.month += 1; //月份需要加上偏移
			char szLog[128] = {0};
			sprintf_s(szLog, sizeof(szLog), "Device Time:%d-%d-%d %d:%d:%d", time.year, time.month, time.mday, time.hour, time.minute, time.second);
			AfxMessageBox(szLog);
			break;
		}
		else
		{
			nTryTime--;
			::Sleep(500);
		}
	}
	if (!bRet)
	{
		AfxMessageBox("NET_SDK_GetDeviceTime() faild!");
	}
}

void CToolDlg::OnBnClickedBtnreboot()
{
	BOOL bRet = NET_SDK_RebootDVR(m_userID);
	if (bRet)
	{
		AfxMessageBox("Reboot success!");
	}
	else
	{
		AfxMessageBox("Reboot faild!");
	}
}

DWORD WINAPI CToolDlg::AlarmStatusThread( LPVOID params )
{
	CToolDlg *pThis = reinterpret_cast<CToolDlg *>(params);
	pThis->AlarmStatusProc();
	return 0;
}
void CToolDlg::AlarmStatusProc()
{
	char *buff = new char[4096];
	//DD_ALARM_STATUS_INFO ec[128];
	while(m_isOpenMsgCallback)
	{
		
		DWORD lpBytesReturned = 0;
		int exNum = 0;
		bool bret = NET_SDK_GetAlarmStatusEx(m_userID, buff, 4096, &lpBytesReturned, &exNum);
		if (bret)
		{
			if (exNum)
			{
				DD_ALARM_STATUS_INFO_Ex *pALARM_STATUS_INFO_Ex = (DD_ALARM_STATUS_INFO_Ex*)buff;
				for (int i=0; i<exNum; i++)
				{
					/*char szlog[512] = {0};
					sprintf(szlog, "alarmType=%d, alarmNode=%s \n", pALARM_STATUS_INFO_Ex[i].alarmType, pALARM_STATUS_INFO_Ex[i].alarmNode);
					OutputDebugStringA(szlog);*/
					//TRACE("alarmType=%d, alarmNode=%s \n", pALARM_STATUS_INFO_Ex[i].alarmType, pALARM_STATUS_INFO_Ex[i].alarmNode);
				}
			}
			int size = lpBytesReturned - exNum*sizeof(DD_ALARM_STATUS_INFO_Ex);
			int num = size/sizeof(DD_ALARM_STATUS_INFO);
			char *tmp = buff + exNum*sizeof(DD_ALARM_STATUS_INFO_Ex);
			DD_ALARM_STATUS_INFO *pALARM_STATUS_INFO = (DD_ALARM_STATUS_INFO*)tmp;
			for (int i=0; i<num; i++)
			{
				/*char szlog[512] = {0};
				sprintf(szlog, "alarmType=%d, chanl=%d \n", pALARM_STATUS_INFO[i].alarmType, pALARM_STATUS_INFO[i].chanl);
				OutputDebugStringA(szlog);*/
				//TRACE("alarmType=%d, chanl=%d \n", pALARM_STATUS_INFO[i].alarmType, pALARM_STATUS_INFO[i].chanl);
			}
		}
		Sleep(3000);
	}

	delete[] buff;
}

void CToolDlg::OnBnClickedButtonSmartSubscribAVD()
{
	
	BOOL bret;
	if(!m_SubscribAVD)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		bret = NET_SDK_SmartSubscrib(m_userID, NET_DVR_SMART_AVD, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			m_SubscribAVD = TRUE;
			memcpy(m_serverAddressAVD,sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
	}
	else
	{
		int dwResult = 0;
		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_DVR_SMART_AVD, 0, m_serverAddressAVD, &dwResult);
		if(!bret)
		{

			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}
		else
		{
			m_SubscribAVD = FALSE;
		}
	}
	//DWORD SUPPORT = 0; 
	//DWORD  lpBytesReturned = 0;
	//BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_MATCH_SUPPORT, NULL, 0, &SUPPORT,  sizeof(DWORD), &lpBytesReturned);
	//if(SUPPORT == 0)
	//	return;
	//char *tempBuf = new char[100*1024];


	//NET_SDK_GET_FACE_INFO_LIST,//queryFacePersonnalInfoList in:NET_SDK_FACE_INFO_LIST_GET, out:NET_SDK_FACE_INFO_LIST
	/*NET_SDK_FACE_INFO_LIST_GET sFACE_INFO_LIST_GET;
	memset(&sFACE_INFO_LIST_GET, 0, sizeof(NET_SDK_FACE_INFO_LIST_GET));
	sFACE_INFO_LIST_GET.groupId = 1;
	sFACE_INFO_LIST_GET.pageIndex = 1;
	sFACE_INFO_LIST_GET.pageSize = 10;
	
	lpBytesReturned = 0;
	
	ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_INFO_LIST, &sFACE_INFO_LIST_GET, sizeof(NET_SDK_FACE_INFO_LIST_GET), tempBuf,  100*1024, &lpBytesReturned);
	if(ret)
	{
		NET_SDK_FACE_INFO_LIST *pFACE_INFO_LIST = (NET_SDK_FACE_INFO_LIST*)tempBuf;
		TRACE("listNum =%d , pFACE_INFO_LIST->totalNum =%d lpBytesReturned=%d \n", pFACE_INFO_LIST->listNum, pFACE_INFO_LIST->totalNum, lpBytesReturned);
		NET_SDK_FACE_INFO_LIST_ITEM *pFACE_INFO_LIST_ITEM = pFACE_INFO_LIST->pFaceInfoListItem;
		for(int i=0; i< pFACE_INFO_LIST->listNum; i++)
		{
			TRACE("itemId =%d , name =%s birthday=%d\n", pFACE_INFO_LIST_ITEM[i].itemId, pFACE_INFO_LIST_ITEM[i].name, pFACE_INFO_LIST_ITEM[i].birthday);
		}
	}*/
	 //NET_SDK_GET_FACE_INFO_IMG,//requestFacePersonnalInfoImage in:NET_SDK_FACE_INFO_IMG_GET out:NET_SDK_FACE_INFO_IMG_DATA
	/*memset(tempBuf, 0, 100*1024);
	NET_SDK_FACE_INFO_IMG_GET sFACE_INFO_IMG_GET;
	sFACE_INFO_IMG_GET.index = 1;
	sFACE_INFO_IMG_GET.itemId = 1;
	ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_INFO_IMG, &sFACE_INFO_IMG_GET, sizeof(NET_SDK_FACE_INFO_IMG_GET), tempBuf,  100*1024, &lpBytesReturned);
	if(ret)
	{
		NET_SDK_FACE_INFO_IMG_DATA *pFACE_INFO_IMG_DATA = (NET_SDK_FACE_INFO_IMG_DATA*)tempBuf;
		TRACE("imgLen =%d ,  lpBytesReturned=%d \n", pFACE_INFO_IMG_DATA->imgLen,  lpBytesReturned);
		FILE *file = fopen("./face.jpg", "wb");
		if (file)
		{
			fwrite(pFACE_INFO_IMG_DATA->imgData, 1, pFACE_INFO_IMG_DATA->imgLen, file);
			fclose(file);
		}
	}
*/
	//////,//createFacePersonnalInfo in:NET_SDK_FACE_INFO_ADD out:NULL
	//char *inBuf = new char[10*1024];
	//memset(inBuf, 0, 10*1024);
	// 
	//NET_SDK_FACE_INFO_ADD *pFACE_INFO_ADD = (NET_SDK_FACE_INFO_ADD*)inBuf;

	//strcpy(pFACE_INFO_ADD->sFaceInfoItem.name, "sdk test");
	//strcpy(pFACE_INFO_ADD->sFaceInfoItem.mobile, "1234567890");
	//strcpy((char*)pFACE_INFO_ADD->sFaceInfoItem.groups[0].guid, "{B33EF2A3-3FDB-454A-944C-F3E42FF36AF8}");

	//pFACE_INFO_ADD->imgNum = 1;
	//pFACE_INFO_ADD->haveImgData = 1;
	///*sFACE_INFO_ADD.sFaceImgInfo[0].chl = 1;
	//sFACE_INFO_ADD.sFaceImgInfo[0].imgId = 1837;
	//sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.year = 2018;
	//sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.month = 5;
	//sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.mday = 22;
	//sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.hour = 23;
	//sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.minute = 48;
	//sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.second = 49;
	//sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.nMicrosecond = 1277790;*/
	//pFACE_INFO_ADD->sFaceInfoItem.birthday = 20180123;
	//pFACE_INFO_ADD->imgData = (unsigned char*)inBuf+sizeof(NET_SDK_FACE_INFO_ADD);
	//FILE *fp = fopen("./face.jpg", "rb");
	//fseek(fp,0,SEEK_END);
	//int length=ftell(fp);
	//fseek(fp,0,SEEK_SET);
	//int fret = fread(pFACE_INFO_ADD->imgData , length, 1, fp );
	//fclose(fp);
	//pFACE_INFO_ADD->imgWidth = 256;
	//pFACE_INFO_ADD->imgHeight = 256;
	//pFACE_INFO_ADD->imgLen = length;
	//ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_ADD_FACE_INFO, inBuf, sizeof(NET_SDK_FACE_INFO_ADD)+length, NULL,  0, &lpBytesReturned);
	//if(ret)
	//{
	//	TRACE("return true \n");
	//}
	//else
	//{
	//	AfxMessageBox(" faild!");
	//}
	//delete[] inBuf;

	////NET_SDK_GET_FACE_INFO_GROUP_LIST,//queryFacePersonnalInfoGroupList in:NULL, out:NET_SDK_FACE_INFO_GROUP_ITEM list
	//memset(tempBuf, 0, 100*1024);
	//lpBytesReturned = 0;
	//ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_INFO_GROUP_LIST,NULL, 0, tempBuf,  100*1024, &lpBytesReturned);
	//if(ret)
	//{
	//	
	//	TRACE("return true %d \n", lpBytesReturned);
	//	int num = lpBytesReturned/sizeof(NET_SDK_FACE_INFO_GROUP_ITEM);
	//	NET_SDK_FACE_INFO_GROUP_ITEM *pFACE_INFO_GROUP_ITEM = (NET_SDK_FACE_INFO_GROUP_ITEM*)tempBuf;
	//	for ( int i=0; i<num; i++)
	//	{
	//		TRACE("%d %s \n", pFACE_INFO_GROUP_ITEM[i].groupId,  pFACE_INFO_GROUP_ITEM[i].guid);
	//	}
	//}

	//NET_SDK_ADD_FACE_INFO_GROUP,//createFacePersonnalInfoGroup in:NET_SDK_FACE_INFO_GROUP_ADD out:NULL
	//memset(tempBuf, 0, 100*1024);
	//lpBytesReturned = 0;
	//NET_SDK_FACE_INFO_GROUP_ADD sFACE_INFO_GROUP_ADD;
	//memset(&sFACE_INFO_GROUP_ADD, 0, sizeof(NET_SDK_FACE_INFO_GROUP_ADD));
	//strcpy(sFACE_INFO_GROUP_ADD.name, "sdk group");
	//sFACE_INFO_GROUP_ADD.property = NET_SDK_FACE_INFO_GROUP_PROPERTY_ALLOW;
	//ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_ADD_FACE_INFO_GROUP,&sFACE_INFO_GROUP_ADD, sizeof(NET_SDK_FACE_INFO_GROUP_ADD), NULL,  0, &lpBytesReturned);
	//if(!ret)
	//{
	//	AfxMessageBox(" faild!");
	//}

	//NET_SDK_SET_FACE_INFO_GROUP,// //editFacePersonnalInfoGroup in:NET_SDK_FACE_INFO_GROUP_ITEM  out:NULL

	/*NET_SDK_FACE_INFO_GROUP_ITEM sFACE_INFO_GROUP_ITEM;
	memset(&sFACE_INFO_GROUP_ITEM, 0, sizeof(NET_SDK_FACE_INFO_GROUP_ITEM));
	strcpy(sFACE_INFO_GROUP_ITEM.name, "sdk test2");
	strcpy((char*)sFACE_INFO_GROUP_ITEM.guid, "{B4F53B79-C82C-EE4C-B7ED-82C71964E9B5}");
	sFACE_INFO_GROUP_ITEM.groupId = 15;
	sFACE_INFO_GROUP_ITEM.property = NET_SDK_FACE_INFO_GROUP_PROPERTY_ALLOW;
	ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SET_FACE_INFO_GROUP,&sFACE_INFO_GROUP_ITEM, sizeof(NET_SDK_FACE_INFO_GROUP_ITEM), tempBuf,  100*1024, &lpBytesReturned);
	if(!ret)
	{
		AfxMessageBox(" faild!");
	}
*/
	//NET_SDK_DEL_FACE_INFO_GROUP,//delFacePersonnalInfoGroups in:NET_SDK_FACE_INFO_GROUP_DEL  out:NULL
	
	//NET_SDK_FACE_INFO_GROUP_DEL sFACE_INFO_GROUP_DEL;
	//memset(&sFACE_INFO_GROUP_DEL, 0, sizeof(NET_SDK_FACE_INFO_GROUP_DEL));
	//
	//strcpy((char*)sFACE_INFO_GROUP_DEL.guid, "{27AFF9A9-7084-7644-AE3A-F50E64779F66}");
	//
	//ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_DEL_FACE_INFO_GROUP,&sFACE_INFO_GROUP_DEL, sizeof(NET_SDK_FACE_INFO_GROUP_DEL), NULL,  0, &lpBytesReturned);
	//if(!ret)
	//{
	//	AfxMessageBox(" faild!");
	//}

	//NET_SDK_SET_FACE_INFO,//editFacePersonnalInfo in:NET_SDK_FACE_INFO_EDIT out:NULL
//memset(tempBuf, 0, 100*1024);
//NET_SDK_FACE_INFO_EDIT *pFACE_INFO_EDIT = (NET_SDK_FACE_INFO_EDIT*)tempBuf;
//pFACE_INFO_EDIT->delFaceImgs[0] = 1;
//pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.itemId = 3;
//strcpy(pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.name, "sdk test");
//pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.groups[0].groupId  = 1;
//strcpy((char *)pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.groups[0].guid, "{B33EF2A3-3FDB-454A-944C-F3E42FF36AF8}");
//pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.faceImgCount = 1;
//pFACE_INFO_EDIT->sFaceInfoItem.imgNum = 1;
//pFACE_INFO_EDIT->sFaceInfoItem.haveImgData = 1;
//pFACE_INFO_EDIT->sFaceInfoItem.imgData = (unsigned char *)tempBuf+sizeof(NET_SDK_FACE_INFO_EDIT);;
//
//
//FILE *fp = fopen("./chSnap.jpg", "rb");
//fseek(fp,0,SEEK_END);
//int length=ftell(fp);
//fseek(fp,0,SEEK_SET);
//int fret = fread(pFACE_INFO_EDIT->sFaceInfoItem.imgData , length, 1, fp );
//fclose(fp);
//pFACE_INFO_EDIT->sFaceInfoItem.imgLen = length;
//
///*FACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].chl = 16;
//pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].imgId = 226;
//pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.year = 2018;
//pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.month = 5;
//pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.mday = 24;
//pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.hour = 0;
//pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.minute = 48;
//pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.second = 39;
//pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.nMicrosecond = 8483680;*/
//
//	ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SET_FACE_INFO, tempBuf, length+sizeof(NET_SDK_FACE_INFO_EDIT), NULL,  0, &lpBytesReturned);
//if(!ret)
//{
//	AfxMessageBox(" faild!");
//}

	//NET_SDK_DEL_FACE_INFO,//delFacePersonnalInfo in:NET_SDK_FACE_INFO_DEL  out:NULL
	/*NET_SDK_FACE_INFO_DEL sFACE_INFO_DEL;
	memset(&sFACE_INFO_DEL, 0, sizeof(NET_SDK_FACE_INFO_DEL));
	sFACE_INFO_DEL.faceInfoListItemId = 11107;
	sFACE_INFO_DEL.groupsId[0] = 11;
	
	ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_DEL_FACE_INFO,&sFACE_INFO_DEL, sizeof(NET_SDK_FACE_INFO_DEL), tempBuf,  100*1024, &lpBytesReturned);
	if(!ret)
	{
		AfxMessageBox(" faild!");
	}*/

	//NET_SDK_SEARCH_IMAGE_BY_IMG, //searchImageByImage in:NET_SDK_SEARCH_IMAGE_BY_IMAGE out:NET_SDK_SEARCH_IMAGE_BY_IMAGE_LIST
	//memset(tempBuf, 0, 100*1024);
	//char *inBuf = new char[10*1024];
	//memset(inBuf, 0, 10*1024);

	//NET_SDK_SEARCH_IMAGE_BY_IMAGE *pSEARCH_IMAGE_BY_IMAGE = (NET_SDK_SEARCH_IMAGE_BY_IMAGE*)inBuf;

	//pSEARCH_IMAGE_BY_IMAGE->pageIndex = 1;
	//pSEARCH_IMAGE_BY_IMAGE->pageSize = 10;
	//pSEARCH_IMAGE_BY_IMAGE->similarity = 75;
	//pSEARCH_IMAGE_BY_IMAGE->resultCountLimit = 100;
	//pSEARCH_IMAGE_BY_IMAGE->startTime.year = 2018;
	//pSEARCH_IMAGE_BY_IMAGE->startTime.month = 5;
	//pSEARCH_IMAGE_BY_IMAGE->startTime.mday = 24;

	//pSEARCH_IMAGE_BY_IMAGE->endTime.year = 2018;
	//pSEARCH_IMAGE_BY_IMAGE->endTime.month = 5;
	//pSEARCH_IMAGE_BY_IMAGE->endTime.mday = 24;
	//pSEARCH_IMAGE_BY_IMAGE->endTime.hour = 23;
	//pSEARCH_IMAGE_BY_IMAGE->endTime.minute = 59;
	//pSEARCH_IMAGE_BY_IMAGE->endTime.second = 59;
	//pSEARCH_IMAGE_BY_IMAGE->searchType = SEARCH_IMAGE_BY_FACE_IMAGE_DATA;
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.chl = 16;
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.imgId = 226;
	//
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.frameTime.year = 2018;
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.frameTime.month = 5;
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.frameTime.mday = 24;
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.frameTime.hour = 0;
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.frameTime.minute = 48;
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.frameTime.second = 39;
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.frameTime.nMicrosecond = 8483680;
	//
	//
	//pSEARCH_IMAGE_BY_IMAGE->sfaceFeatureGroups.groupsId = 1;
	//
	//pSEARCH_IMAGE_BY_IMAGE->srecognizedFilter.groupsId = 1;
	//pSEARCH_IMAGE_BY_IMAGE->srecognizedFilter.isContainNotRecognized = 1;
	//pSEARCH_IMAGE_BY_IMAGE->srecognizedFilter.isContainRecognized = 1;
	//
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgData.imgHeight = 256;
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgData.imgWidth = 256;
	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgData.imgData = (unsigned char *)inBuf+sizeof(NET_SDK_SEARCH_IMAGE_BY_IMAGE);
	//int length = 0;
	//FILE *fp = fopen("./face.jpg", "rb");
	//fseek(fp,0,SEEK_END);
	//length=ftell(fp);
	//fseek(fp,0,SEEK_SET);
	//int fret = fread((char*)pSEARCH_IMAGE_BY_IMAGE->sfaceImgData.imgData , length, 1, fp );
	//fclose(fp);

	//pSEARCH_IMAGE_BY_IMAGE->sfaceImgData.imgLen = length;

	//ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SEARCH_IMAGE_BY_IMG,inBuf, sizeof(NET_SDK_SEARCH_IMAGE_BY_IMAGE)+length, tempBuf,  100*1024, &lpBytesReturned);
	//if(!ret)
	//{
	//	AfxMessageBox(" faild!");
	//}
	//else
	//{
	//	NET_SDK_SEARCH_IMAGE_BY_IMAGE_LIST *pSEARCH_IMAGE_BY_IMAGE_LIST = (NET_SDK_SEARCH_IMAGE_BY_IMAGE_LIST*)tempBuf;
	//	TRACE("listNum=%d, bEnd=%d \n", pSEARCH_IMAGE_BY_IMAGE_LIST->listNum, pSEARCH_IMAGE_BY_IMAGE_LIST->bEnd);
	//	NET_SDK_SEARCH_IMAGE_ITEM *pSEARCH_IMAGE_ITEM = pSEARCH_IMAGE_BY_IMAGE_LIST->pSearchImageItem;
	//	for(int i=0; i< pSEARCH_IMAGE_BY_IMAGE_LIST->listNum; i++)
	//	{
	//		TRACE("%d:%d:%d ----- %d:%d:%d\n", pSEARCH_IMAGE_ITEM[i].recStartTime.hour, pSEARCH_IMAGE_ITEM[i].recStartTime.minute, pSEARCH_IMAGE_ITEM[i].recStartTime.second,
	//			pSEARCH_IMAGE_ITEM[i].recEndTime.hour, pSEARCH_IMAGE_ITEM[i].recEndTime.minute, pSEARCH_IMAGE_ITEM[i].recEndTime.second);
	//	}
	//}

	//delete[]inBuf;
	//NET_SDK_SEARCH_CH_SNAP_FACE_IMG_LIST,//queryChSnapFaceImageList in:NET_SDK_CH_SNAP_FACE_IMG_LIST_SEARCH out:NET_SDK_CH_SNAP_FACE_IMG_LIST
	// 
	/*memset(tempBuf, 0 , 100*1024);
	NET_SDK_CH_SNAP_FACE_IMG_LIST_SEARCH sCH_SNAP_FACE_IMG_LIST_SEARCH;
	memset(&sCH_SNAP_FACE_IMG_LIST_SEARCH, 0, sizeof(NET_SDK_CH_SNAP_FACE_IMG_LIST_SEARCH));
	sCH_SNAP_FACE_IMG_LIST_SEARCH.dwChannel = 16;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.pageIndex = 1;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.pageSize = 15;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.startTime.year = 2018;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.startTime.month = 5;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.startTime.mday = 25;

	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.year = 2018;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.month = 5;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.mday = 25;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.hour = 23;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.minute = 59;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.second = 59;

	ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SEARCH_CH_SNAP_FACE_IMG_LIST,&sCH_SNAP_FACE_IMG_LIST_SEARCH, sizeof(NET_SDK_CH_SNAP_FACE_IMG_LIST_SEARCH), tempBuf,  100*1024, &lpBytesReturned);
	if(!ret)
	{
		AfxMessageBox(" faild!");
	}
	else
	{
		NET_SDK_CH_SNAP_FACE_IMG_LIST *pCH_SNAP_FACE_IMG_LIST = (NET_SDK_CH_SNAP_FACE_IMG_LIST*)tempBuf;
		TRACE("listNum=%d, bEnd=%d \n", pCH_SNAP_FACE_IMG_LIST->listNum, pCH_SNAP_FACE_IMG_LIST->bEnd);
		NET_SDK_FACE_IMG_INFO_CH *pFACE_IMG_INFO_CH = pCH_SNAP_FACE_IMG_LIST->pCHFaceImgItem;
		for(int i=0; i< pCH_SNAP_FACE_IMG_LIST->listNum; i++)
		{
			TRACE("%d:%d:%d ----- %d\n", pFACE_IMG_INFO_CH[i].frameTime.hour, pFACE_IMG_INFO_CH[i].frameTime.minute, pFACE_IMG_INFO_CH[i].frameTime.second,pFACE_IMG_INFO_CH[i].imgId);
		}
	}*/

//NET_SDK_SEARCH_CH_SNAP_FACE_IMG requestChSnapFaceImage in:NET_SDK_FACE_IMG_INFO_CH out:NET_SDK_FACE_INFO_IMG_DATA

/*memset(tempBuf, 0, 100*1024);
NET_SDK_FACE_IMG_INFO_CH sIMG_INFO_CH;
sIMG_INFO_CH.chl = 16;
sIMG_INFO_CH.imgId = 296;
sIMG_INFO_CH.frameTime.year = 2018;
sIMG_INFO_CH.frameTime.month = 5;
sIMG_INFO_CH.frameTime.mday = 25;
sIMG_INFO_CH.frameTime.hour = 5;
sIMG_INFO_CH.frameTime.minute = 30;
sIMG_INFO_CH.frameTime.second = 22;
sIMG_INFO_CH.frameTime.nMicrosecond = 4525720;
ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SEARCH_CH_SNAP_FACE_IMG, &sIMG_INFO_CH, sizeof(NET_SDK_FACE_IMG_INFO_CH), tempBuf,  100*1024, &lpBytesReturned);
if(ret)
{
NET_SDK_FACE_INFO_IMG_DATA *pFACE_INFO_IMG_DATA = (NET_SDK_FACE_INFO_IMG_DATA*)tempBuf;
TRACE("imgLen =%d ,  lpBytesReturned=%d \n", pFACE_INFO_IMG_DATA->imgLen,  lpBytesReturned);
FILE *file = fopen("./chSnap.jpg", "wb");
if (file)
{
fwrite(pFACE_INFO_IMG_DATA->imgData, 1, pFACE_INFO_IMG_DATA->imgLen, file);
fclose(file);
}
}*/

//NET_SDK_GET_FACE_MATCH_ALARM,//queryFaceMatchAlarm  in:NULL out:NET_SDK_FACE_MATCH_ALARM
// 
	
//memset(tempBuf, 0, 100*1024);
//ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_MATCH_ALARM, NULL, 0, tempBuf,  100*1024, &lpBytesReturned);
//if(ret)
//{
//	NET_SDK_FACE_MATCH_ALARM *pFACE_MATCH_ALARM = (NET_SDK_FACE_MATCH_ALARM*)tempBuf;
//	TRACE("lpBytesReturned=%d, faceFeatureGroupsNum=%d \n", lpBytesReturned, pFACE_MATCH_ALARM->faceFeatureGroupsNum);
//	NET_SDK_FACE_MATCH_ALARM_TRIGGER *pFaceMatchAlarmTrigger = pFACE_MATCH_ALARM->pFaceMatchAlarmTrigger;
//	for(int i=0; i< pFACE_MATCH_ALARM->faceFeatureGroupsNum; i++)
//	{
//		TRACE("%s %d-%d-%d-%d-%d-%d-%d-%d-%d\n", pFaceMatchAlarmTrigger[i].guid, pFaceMatchAlarmTrigger[i].groupSwitch, pFaceMatchAlarmTrigger[i].alarmOutSwitch, 
//			 pFaceMatchAlarmTrigger[i].recSwitch, pFaceMatchAlarmTrigger[i].snapSwitch, pFaceMatchAlarmTrigger[i].popVideo,
//			 pFaceMatchAlarmTrigger[i].buzzerSwitch,pFaceMatchAlarmTrigger[i].emailSwitch, pFaceMatchAlarmTrigger[i].msgPushSwitch, pFaceMatchAlarmTrigger[i].popMsgSwitch);
//	}
//}
//else
//{
//	AfxMessageBox(" faild!");
//}
//
//
////	NET_SDK_SET_FACE_MATCH_ALARM,//editFaceMatchAlarm in:NET_SDK_FACE_MATCH_ALARM out:NULL
//NET_SDK_FACE_MATCH_ALARM *pFACE_MATCH_ALARM = (NET_SDK_FACE_MATCH_ALARM*)tempBuf;
//pFACE_MATCH_ALARM->similarity+=5;
//ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SET_FACE_MATCH_ALARM, tempBuf, 100*1024, NULL,  0, &lpBytesReturned);
//if(ret)
//{
//	
//}
//else
//{
//	AfxMessageBox(" faild!");
//}









/*int k=0;
delete[]tempBuf; */
	
}

void CToolDlg::OnBnClickedButtonSmartSubscribVFD()
{
	BOOL bret;
	NET_SDK_DEVICE_FUNC_IPC ipcFun;
	memcpy(&ipcFun, m_deviceinfo->function, sizeof(NET_SDK_DEVICE_FUNC_IPC));
	if (!ipcFun.intelist_Vfd && NET_SDK_IPCAMERA == m_deviceinfo->deviceType)
	{
		AfxMessageBox(" unsupport VFD!");
		return;
	}
	if(!m_SubscribVFD)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		bret = NET_SDK_SmartSubscrib(m_userID, NET_DVR_SMART_VFD, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			m_SubscribVFD = TRUE;
			memcpy(m_serverAddressVFD,sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
	}
	else
	{
		int dwResult = 0;
		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_DVR_SMART_VFD, 0, m_serverAddressVFD, &dwResult);
		if(!bret)
		{

			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}
		else
		{
			m_SubscribVFD = FALSE;
		}
	}
}

void CToolDlg::OnBnClickedButtonSmartSubscribFaceMatch()
{
	BOOL bret;
	NET_SDK_DEVICE_FUNC_IPC ipcFun;
	memcpy(&ipcFun, m_deviceinfo->function, sizeof(NET_SDK_DEVICE_FUNC_IPC));
	if (!ipcFun.intelist_Vfd_Match && NET_SDK_IPCAMERA == m_deviceinfo->deviceType)
	{
		AfxMessageBox(" unsupport FaceMatch!");
		return;
	}
	
	if(!m_SubscribFaceMatch)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		memset(&sSmartSubscrib, 0, sizeof(NET_DVR_SUBSCRIBE_REPLY));
		bret = NET_SDK_SmartSubscrib(m_userID, NET_DVR_SMART_VFD_MATCH, 0, &sSmartSubscrib);
		//bret = NET_SDK_SmartSubscrib(m_userID, NET_DVR_SMART_IPD, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			m_SubscribFaceMatch = TRUE;
			memcpy(m_serverAddressFaceMatch,sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
		memset(&sSmartSubscrib, 0, sizeof(NET_DVR_SUBSCRIBE_REPLY));
		bret = NET_SDK_SmartSubscrib(m_userID, NET_DVR_SMART_VFD_MATCH_FAILED, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			memcpy(m_serverAddressFaceMatchFailed,sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
		
		
	}
	else
	{
		int dwResult = 0;
		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_DVR_SMART_VFD_MATCH, 0, m_serverAddressFaceMatch, &dwResult);
		//bret = NET_SDK_UnSmartSubscrib(m_userID, NET_DVR_SMART_IPD, 0, m_serverAddressFaceMatch, &dwResult);
		if(!bret)
		{

			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}
		else
		{
			m_SubscribFaceMatch = FALSE;
		}

		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_DVR_SMART_VFD_MATCH_FAILED, 0, m_serverAddressFaceMatchFailed, &dwResult);
		if(!bret)
		{

			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}	
	}
}




void CToolDlg::OnBnClickedButtonSmartSbscribOsc()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bret;
	if(!m_SubscribOSC)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		bret = NET_SDK_SmartSubscrib(m_userID, NET_DVR_SMART_OSC, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			m_SubscribOSC = TRUE;
			memcpy(m_serverAddressAVD,sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
	}
	else
	{
		int dwResult = 0;
		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_DVR_SMART_OSC, 0, m_serverAddressAVD, &dwResult);
		if(!bret)
		{

			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}
		else
		{
			m_SubscribOSC = FALSE;
		}
	}
}


void CToolDlg::OnBnClickedButtonSmartSbscribPea()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bret;
	if(!m_SubscribPEA)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		bret = NET_SDK_SmartSubscrib(m_userID, NET_DVR_SMART_PEA, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			m_SubscribPEA = TRUE;
			memcpy(m_serverAddressAVD,sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
	}
	else
	{
		int dwResult = 0;
		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_DVR_SMART_PEA, 0, m_serverAddressAVD, &dwResult);
		if(!bret)
		{

			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}
		else
		{
			m_SubscribPEA = FALSE;
		}
	}
}


void CToolDlg::OnBnClickedButtonSmartSbscribVihicle()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bret;
	if(!m_SubscribVehicle)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		bret = NET_SDK_SmartSubscrib(m_userID, NET_IPC_SMART_VIHICLE, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			m_SubscribVehicle = TRUE;
			memcpy(m_serverAddressAVD,sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
	}
	else
	{
		int dwResult = 0;
		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_IPC_SMART_VIHICLE, 0, m_serverAddressAVD, &dwResult);
		if(!bret)
		{

			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}
		else
		{
			m_SubscribVehicle = FALSE;
		}
	}
}


void CToolDlg::OnBnClickedButtonSmartSbscribAoientry()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bret;
	if(!m_SubscribAOIENTRY)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		bret = NET_SDK_SmartSubscrib(m_userID, NET_IPC_SMART_AOIENTRY, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			m_SubscribAOIENTRY = TRUE;
			memcpy(m_serverAddressAVD,sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
	}
	else
	{
		int dwResult = 0;
		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_IPC_SMART_AOIENTRY, 0, m_serverAddressAVD, &dwResult);
		if(!bret)
		{

			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}
		else
		{
			m_SubscribAOIENTRY = FALSE;
		}
	}
}


void CToolDlg::OnBnClickedButtonSmartSbscribAoileave()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bret;
	if(!m_SubscribAOILEAVE)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		bret = NET_SDK_SmartSubscrib(m_userID, NET_IPC_SMART_AOILEAVE, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			m_SubscribAOILEAVE = TRUE;
			memcpy(m_serverAddressAVD,sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
	}
	else
	{
		int dwResult = 0;
		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_IPC_SMART_AOILEAVE, 0, m_serverAddressAVD, &dwResult);
		if(!bret)
		{

			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}
		else
		{
			m_SubscribAOILEAVE = FALSE;
		}
	}
}

void CToolDlg::OnBnClickedButtonSmartSbscribPassline()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bret;
	if(!m_SubscribPassLine)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		bret = NET_SDK_SmartSubscrib(m_userID, NET_IPC_SMART_PASSLINE, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			m_SubscribPassLine = TRUE;
			memcpy(m_serverAddressPassLine,sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
	}
	else
	{
		int dwResult = 0;
		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_IPC_SMART_PASSLINE, 0, m_serverAddressPassLine, &dwResult);
		if(!bret)
		{

			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}
		else
		{
			m_SubscribPassLine = FALSE;
		}
	}
}


static void WINAPI SearchCallBack_Ex(char* hwaddr, char *szDevIP, int opt, const SEARCHED_DEVICE_INFO *pData, void *pParam, const char *szRecvFromNIC)
{
	int a =0;
	char ipaddr_2[20]={0}, maskAddr_2[20]={0};
	sprintf(ipaddr_2,"%d.%d.%d.%d",pData->dwSecondIP&0xff,(pData->dwSecondIP>>8)&0xff,(pData->dwSecondIP>>16)&0xff,(pData->dwSecondIP>>24)&0xff);
	sprintf(maskAddr_2,"%d.%d.%d.%d",pData->dwSecondMask&0xff,(pData->dwSecondMask>>8)&0xff,(pData->dwSecondMask>>16)&0xff,(pData->dwSecondMask>>24)&0xff);
	char abc[1024] = {0};
	sprintf(abc,"discovered: %s devname: %s type: %s model: %s factory: %s \r\n",szDevIP,pData->deviceType,pData->szproductModel,pData->szFactoryName);
	OutputDebugStringA(abc);
	AfxMessageBox(abc);
}

void CToolDlg::OnBnClickedButtonSearchDevice()
{
	m_listSearchDevice.DeleteAllItems();
	GetDlgItem(IDC_BUTTON_SEARCH_DEVICE)->EnableWindow(FALSE);
	NET_SDK_DEVICE_DISCOVERY_INFO devs[100];
	int num = NET_SDK_DiscoverDevice(devs,100,20);
	CString strTmp = "";
	CString strDeviceType = "";
	char macAddr[32] = {0};
	//std::string strMsg;
	//sprintf(abc,"num is : %d\r\n",num);
	//strMsg += abc;
	for (int i = 0;i < num;i++)
	{
		//memset(&abc,0,64);
		//sprintf(abc,"discovered: %s\r\n",devs[i].strIP);
		//strMsg += abc;
		m_listSearchDevice.InsertItem(i, _T(""));
		m_listSearchDevice.SetItemText(i, 0, devs[i].strIP);
		//strTmp.Format("%d",devs[i].httpPort);
		//m_listSearchDevice.SetItemText(i, 1, strTmp);
		if (devs[i].deviceActivated == 0x00) //貌似传的就是ACSII码值 不是传的'0'
		{
			m_listSearchDevice.SetItemText(i, 2, STR_IS_ACTIVATED);
		}
		else if (devs[i].deviceActivated == 0x01)
		{
			m_listSearchDevice.SetItemText(i, 2, STR_NOT_ACTIVATED);
		}
		else
		{
			m_listSearchDevice.SetItemText(i, 2, STR_UNKOWN_ACTIVATED);
		}
		if (devs[i].deviceType == NET_SDK_IPCAMERA)
		{
			//m_listSearchDevice.SetItemText(i, 3, devs[i].dwSecondIP);
		}
		//MessageBox(abc)


		strDeviceType = ConvertDeviceType(devs[i].deviceType);
		m_listSearchDevice.SetItemText(i, 4, strDeviceType);

		m_listSearchDevice.SetItemText(i, 5, devs[i].productType);
		sprintf(macAddr,"%02x:%02x:%02x:%02x:%02x:%02x",  devs[i].byMac[0], devs[i].byMac[1], devs[i].byMac[2], devs[i].byMac[3],
			devs[i].byMac[4], devs[i].byMac[5]);
		m_listSearchDevice.SetItemText(i, 6, macAddr);

		if (devs[i].pwdLevel == 0x01)
		{
			m_listSearchDevice.SetItemText(i, 7, STR_PWDLEVEL_LOW);
		}
		else if (devs[i].pwdLevel == 0x02)
		{
			m_listSearchDevice.SetItemText(i, 7, STR_PWDLEVEL_MEDIUM);
		}
		else
		{
			m_listSearchDevice.SetItemText(i, 7, STR_PWDLEVEL_HIGH);
		}
	}

	GetDlgItem(IDC_BUTTON_SEARCH_DEVICE)->EnableWindow(TRUE);
	//MessageBox(strMsg.c_str());
	// TODO: 在此添加控件通知处理程序代码
	/*if(!searched)
	{
		m_nSearchDevice = NET_SDK_DiscoverDeviceStart(NULL,SearchCallBack_Ex,this,_SEARCH_ALL,2000);
		if(m_nSearchDevice > 0)
		{
			searched = true;
			GetDlgItem(IDC_BUTTON_SEARCH_DEVICE)->SetWindowText("stop search");
		}
	}
	else
	{
		NET_SDK_DiscoverDeviceStop(m_nSearchDevice);
		m_nSearchDevice = 0;
		searched = false;
		GetDlgItem(IDC_BUTTON_SEARCH_DEVICE)->SetWindowText("search device");
	}*/

}



void CToolDlg::OnBnClickedButtonSetLog()
{
	UpdateData(TRUE);
	// TODO: 在此添加控件通知处理程序代码
	CString strPath;
	GetDlgItem(IDC_EDIT_LOGPATH)->GetWindowText(strPath);


	CButton *button;
	button = (CButton *)GetDlgItem(IDC_CHECK_LOG);

	bool blogEnable = button->GetCheck();

	int logLevel = YLOG_DEBUG;
	CComboBox* pLogLevelCtrl = reinterpret_cast<CComboBox*>(GetDlgItem(IDC_COMBO_LOG_LEVEL));
	if (pLogLevelCtrl)
	{
		logLevel = (LONG)pLogLevelCtrl->GetCurSel();
	}

	BOOL ret = NET_SDK_SetLogToFile(blogEnable,strPath.GetBuffer(),FALSE,logLevel);
	strPath.ReleaseBuffer();
}


void CToolDlg::OnBnClickedButtonSearchActivedev()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bRet = FALSE;
	//bRet = NET_SDK_ActiveDevice(m_userID);
	POSITION pos=m_listSearchDevice.GetFirstSelectedItemPosition();
	if(pos==NULL)
	{
		MessageBox(STR_INFO_SEL_UNACTIVATED,STR_INFO_TIPS,MB_ICONEXCLAMATION);
		return;
	}


	int iIndex =(int)m_listSearchDevice.GetNextSelectedItem(pos);

	CString strIp = "";
	CString strPort = "";
	CString strActiveState = "";
	CString strSecondIp = "";
	CString strDeviceType = "";
	CString strMac = "";
	strIp = m_listSearchDevice.GetItemText(iIndex,0);
	strPort = m_listSearchDevice.GetItemText(iIndex,1);
	strActiveState = m_listSearchDevice.GetItemText(iIndex,2);
	strSecondIp = m_listSearchDevice.GetItemText(iIndex,3);
	strDeviceType = m_listSearchDevice.GetItemText(iIndex,4);
	strMac = m_listSearchDevice.GetItemText(iIndex,6);

	UpdateData(TRUE);

	if(strIp == "" || strMac == "" || strActiveState == STR_IS_ACTIVATED)
	{
		MessageBox(STR_INFO_SEL_UNACTIVATED,STR_INFO_TIPS,MB_ICONEXCLAMATION);
		return;
	}

	int iPort = atoi(strPort);
	if(strDeviceType.Compare("IPC") != 0)
	{
		MessageBox(STR_INFO_SEL_IPC,STR_INFO_TIPS,MB_ICONEXCLAMATION);
		return;
	}

	/*GetNetAdaptIp();
	list<string>::iterator ipIter;
	for (ipIter = m_listIpAddress.begin();ipIter != m_listIpAddress.end();ipIter++)
	{
		AddZeroCfgRoute(*ipIter);
	}*/

	//bRet = NET_SDK_ActiveDevice(strSecondIp.GetBuffer(0),iPort,m_strPwd.GetBuffer(0));
	bRet = NET_SDK_ActiveDeviceByMac(strMac.GetBuffer(0),m_strPwd.GetBuffer(0));
	if (bRet)
	{
		MessageBox(STR_INFO_ACTIVATED_SUCCESS,STR_INFO_TIPS,MB_ICONEXCLAMATION);
	}
	else
	{
		int iError = NET_SDK_GetLastError();
		if (iError == NET_SDK_NO_PASSWORD_STRENGTH)
		{
			MessageBox(STR_INFO_NO_PWD_STRENGTH,STR_INFO_TIPS,MB_ICONEXCLAMATION);
			return;
		}
		MessageBox(STR_INFO_ACTIVATED_FAIL,STR_INFO_TIPS,MB_ICONEXCLAMATION);
	}
}

CString CToolDlg::ConvertDeviceType(int iDevType)
{
	CString strDeviceType;
	if ( iDevType == NET_SDK_DVR)
	{
		strDeviceType = "DVR";
	}
	else if ( iDevType == NET_SDK_DVS)
	{
		strDeviceType = "DVS";
	}
	else if (iDevType == NET_SDK_IPCAMERA)
	{
		strDeviceType = "IPC";
	}
	else if (iDevType == NET_SDK_NVR)
	{
		strDeviceType = "NVR";
	}
	else if (iDevType == NET_SDK_DECODER)
	{
		strDeviceType = STR_DECODER;
	}
	else if (iDevType == NET_SDK_N_DVR)
	{
		strDeviceType = STR_STORAGE_SERVER;
	}
	else
	{
		strDeviceType = "others";
	}	
	return strDeviceType;
}

void CToolDlg::OnBnClickedButtonBrowseLogpath()
{
	// TODO: 在此添加控件通知处理程序代码
	CString  FullPath;  

	BROWSEINFO bi;                        //BROWSEINFO结构体  
	TCHAR Buffer[512]="";  
	TCHAR FullPaths[512]="";  

	bi.hwndOwner = m_hWnd;                    //m_hWnd你的程序主窗口  
	bi.pidlRoot = NULL;  
	bi.pszDisplayName = Buffer;                //返回选择的目录名的缓冲区  
	bi.lpszTitle = "请选择目录";            //弹出的窗口的文字提示  
	bi.ulFlags = NULL;//BIF_RETURNONLYFSDIRS|BIF_EDITBOX|BIF_BROWSEFORCOMPUTER ;    //只返回目录。其他标志看MSDN  
	bi.lpfn = NULL;                        //回调函数，有时很有用  
	bi.lParam = 0;  
	bi.iImage = 0;  
	ITEMIDLIST* pidl = ::SHBrowseForFolder (&bi);        //显示弹出窗口，ITEMIDLIST很重要  
	::SHGetPathFromIDList (pidl,FullPaths);        //在ITEMIDLIST中得到目录名的整个路径  

	if(FullPaths[0]!=NULL)  
	{  
		GetDlgItem(IDC_EDIT_LOGPATH)->SetWindowText(FullPaths);
		//UpdateData(FALSE);
	}  
}

void CToolDlg::setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid)
{
	m_userID = userid;
	m_deviceinfo = di;
	
	CString temp;
	for (int i = 0; i < di->videoInputNum; i++)
	{
		temp.Format(STR_CHANNEL_D, i + 1);
		m_comChannel.AddString(temp);
	}
	m_comChannel.SetCurSel(0);
}

void CToolDlg::OnBnClickedCheckTalkToChannel()
{
	if (reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_TALK_TO_CHANNEL))->GetCheck())
	{
		m_comChannel.EnableWindow(TRUE);
	}
	else
	{
		m_comChannel.EnableWindow(FALSE);
	}
}
