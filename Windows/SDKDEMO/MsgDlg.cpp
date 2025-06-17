// MsgDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "MsgDlg.h"


// CMsgDlg 对话框

IMPLEMENT_DYNAMIC(CMsgDlg, CDialog)

CMsgDlg::CMsgDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMsgDlg::IDD, pParent)
{

}

CMsgDlg::~CMsgDlg()
{
}

void CMsgDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, m_msgList);
}


BEGIN_MESSAGE_MAP(CMsgDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CMsgDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CMsgDlg::OnBnClickedCancel)
	ON_MESSAGE(WM_MSG_CALLBACK, &CMsgDlg::OnRecieveMsg)
	ON_BN_CLICKED(IDC_BUTTON_MSG_CLEAR, &CMsgDlg::OnBnClickedButtonMsgClear)
END_MESSAGE_MAP()


// CMsgDlg 消息处理程序

void CMsgDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CMsgDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}

LRESULT CMsgDlg::OnRecieveMsg( WPARAM wparam, LPARAM lparam )
{
	int n = m_msgList.GetItemCount();
	m_msgList.InsertItem(n, (LPCTSTR)lparam);
	return 0;
}

void CMsgDlg::OnBnClickedButtonMsgClear()
{
	m_msgList.DeleteAllItems();
}
