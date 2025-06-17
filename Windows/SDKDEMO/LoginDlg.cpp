// LoginDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "LoginDlg.h"
#include <WinSock2.h>

// CLoginDlg 对话框

IMPLEMENT_DYNAMIC(CLoginDlg, CDialog)

CLoginDlg::CLoginDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLoginDlg::IDD, pParent)
	, m_username(_T(""))
	, m_password(_T(""))
	, m_port(0)
	, m_eConnectType(NET_SDK_CONNECT_TCP)
{

}

CLoginDlg::~CLoginDlg()
{
}

void CLoginDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_USERNAME, m_username);
	DDV_MaxChars(pDX, m_username, 65);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_password);
	DDV_MaxChars(pDX, m_password, 65);
	DDX_Text(pDX, IDC_EDIT_PORT,m_port);
}


BEGIN_MESSAGE_MAP(CLoginDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CLoginDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_IP, &CLoginDlg::OnBnClickedRadioIp)
	ON_BN_CLICKED(IDC_RADIO_DOMAIN, &CLoginDlg::OnBnClickedRadioDomain)
	ON_BN_CLICKED(IDC_RADIO_NAT, &CLoginDlg::OnBnClickedRadioNAT)
	ON_BN_CLICKED(IDC_RADIO_NAT2, &CLoginDlg::OnBnClickedRadioNat2)
END_MESSAGE_MAP()


// CLoginDlg 消息处理程序

void CLoginDlg::OnBnClickedOk()
{
	IsDlgButtonChecked(IDC_RADIO_IP) ?
		GetDlgItemText(IDC_IPADDRESS, m_ip) : GetDlgItemText(IDC_EDIT_DOMAIN, m_ip);

	m_port = GetDlgItemInt(IDC_EDIT_PORT);
	if(IsDlgButtonChecked(IDC_RADIO_NAT) || IsDlgButtonChecked(IDC_RADIO_NAT2))
	{
		GetDlgItemText(IDC_EDIT_DOMAIN, m_ip);
		if(IsDlgButtonChecked(IDC_RADIO_NAT2))
		{
			NET_SDK_SetNat2Addr(m_ip.GetBuffer(),m_port);
		}
		GetDlgItemText(IDC_EDIT_DEVICE_SN, m_strDeviceSN);
		if (m_strDeviceSN.IsEmpty())
		{
			AfxMessageBox(_T("Device SN error!"));
			return;
		}
	}

	OnOK();
}

CString getString(const char* pKeyName)
{
	char reBuf[128] = {0};
	::GetPrivateProfileString("Login",pKeyName,"",reBuf,128,"./SDK_config.ini");
	return reBuf;
}

BOOL CLoginDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//SetDlgItemText(IDC_IPADDRESS, m_ip);
	//SetDlgItemInt(IDC_EDIT_PORT, m_port, FALSE);

	//m_username = _T("admin");
	//m_password = _T("12345");

	m_ip = getString("ip");
	m_port = ::GetPrivateProfileInt("Login","port",6036,"./SDK_config.ini");
	m_username = getString("username");
	m_password = getString("password");
	SetDlgItemText(IDC_IPADDRESS, m_ip);
	
	SetDlgItemText(IDC_EDIT_DOMAIN, _T("c2020.autonat.com"));
	SetDlgItemText(IDC_EDIT_DEVICE_SN, _T("N8EAD137G506"));
	GetDlgItem(IDC_EDIT_DEVICE_SN)->EnableWindow(FALSE);
	SetDlgItemText(IDC_EDIT_PORT, _T("7968"));

	UpdateData(FALSE);
	
	CheckDlgButton(IDC_RADIO_IP, BST_CHECKED);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CLoginDlg::SaveConfig()
{
	::WritePrivateProfileString("Login","ip",m_ip,"./SDK_config.ini");
	char tmp[20] = {0};
	sprintf(tmp,"%d",m_port);
	::WritePrivateProfileString("Login","port",tmp,"./SDK_config.ini");
	::WritePrivateProfileString("Login","username",m_username,"./SDK_config.ini");
	::WritePrivateProfileString("Login","password",m_password,"./SDK_config.ini");
}

void CLoginDlg::OnBnClickedRadioIp()
{
	GetDlgItem(IDC_IPADDRESS)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_DOMAIN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_DEVICE_SN)->EnableWindow(FALSE);
	SetDlgItemText(IDC_EDIT_PORT, _T("6036"));
	m_eConnectType = NET_SDK_CONNECT_TCP;
}

void CLoginDlg::OnBnClickedRadioDomain()
{
	GetDlgItem(IDC_IPADDRESS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_DOMAIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_DEVICE_SN)->EnableWindow(FALSE);
	SetDlgItemText(IDC_EDIT_PORT, _T("6036"));
	m_eConnectType = NET_SDK_CONNECT_TCP;
}

void CLoginDlg::OnBnClickedRadioNAT()
{
	GetDlgItem(IDC_IPADDRESS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_DOMAIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_DEVICE_SN)->EnableWindow(TRUE);
	SetDlgItemText(IDC_EDIT_DOMAIN, _T("c2.autonat.com"));
	SetDlgItemText(IDC_EDIT_PORT, _T("40002"));
	m_eConnectType = NET_SDK_CONNECT_NAT;//NET_SDK_CONNECT_NAT,NET_SDK_CONNECT_NAT20
}


void CLoginDlg::OnBnClickedRadioNat2()
{
	GetDlgItem(IDC_IPADDRESS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_EDIT_DOMAIN)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_EDIT_DEVICE_SN)->EnableWindow(TRUE);
	SetDlgItemText(IDC_EDIT_DOMAIN, _T("c2020.autonat.com"));
	SetDlgItemText(IDC_EDIT_PORT, _T("7968"));
	m_eConnectType = NET_SDK_CONNECT_NAT20;//NET_SDK_CONNECT_NAT,NET_SDK_CONNECT_NAT20
}
