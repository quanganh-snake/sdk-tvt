// BackupDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "BackupDlg.h"
#include "strdef.h"


// CBackupDlg 对话框

IMPLEMENT_DYNAMIC(CBackupDlg, CDialog)
std::list<FILE *>	m_fileList;
CCriticalSection   m_SafeLock;
CBackupDlg::CBackupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBackupDlg::IDD, pParent)
{
	
}

CBackupDlg::~CBackupDlg()
{
}

void CBackupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS, m_progress);
}


BEGIN_MESSAGE_MAP(CBackupDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CBackupDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CBackupDlg::OnBnClickedCancel)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CBackupDlg 消息处理程序

void CBackupDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CBackupDlg::OnBnClickedCancel()
{
	NET_SDK_StopGetFile(m_fileHanle);
	m_fileHanle = -1;
	OnCancel();
}

BOOL CBackupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_fileList.clear();
	
	CString fname;
	SYSTEMTIME time = {0};
	::GetLocalTime(&time);
	if (m_backupPath.Right(1) != "\\")
		m_backupPath += "\\";
	fname.Format("%sbackup_%d%02d%02d%02d%02d%02d.avi", m_backupPath.GetBuffer(0), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond);
	FILE	*pFile = fopen(fname.GetBuffer(), "wb");

	if(!pFile)
	{
		AfxMessageBox("fopen file error");
		EndDialog(IDCANCEL);
		return TRUE;
	}
	//LONG handle = NET_SDK_GetFileByTimeExV2(m_userID, m_backupFiles[0].dwChannel, &m_backupFiles[0].startTime, 
	//	&m_backupFiles[0].stopTime, /*NULL*/fname.GetBuffer(), FALSE, 0, TRUE, fBackupDataCallBack, pFile);

	LONG handle = NET_SDK_GetFileByTimeExV2(m_userID, m_backupFiles[0].dwChannel, &m_backupFiles[0].startTime, 
		&m_backupFiles[0].stopTime, /*NULL*/fname.GetBuffer(), FALSE, m_streamType, FALSE, NULL,  0);
	//LONG handle = NET_SDK_GetFileByTimeEx(m_userID, m_backupFiles[0].dwChannel, &m_backupFiles[0].startTime, 
	//	&m_backupFiles[0].stopTime, /*NULL*/fname.GetBuffer(), TRUE, TRUE, fBackupDataCallBack, pFile);
	//LONG handle = NET_SDK_GetFileByTimeEx(m_userID, m_backupFiles[0].dwChannel, &m_backupFiles[0].startTime, 
		//&m_backupFiles[0].stopTime, /*NULL*/fname.GetBuffer(), TRUE, FALSE, NULL, pFile);
	//LONG handle = NET_SDK_GetFileByTimeEx(m_userID, m_backupFiles[0].dwChannel, &m_backupFiles[0].startTime, 
	//	&m_backupFiles[0].stopTime, /*NULL*/fname.GetBuffer(), FALSE, FALSE, NULL, pFile);
	if (handle > 0)
	{
		m_fileHanle = handle;
		SetTimer(1010, 1000, NULL);
		m_SafeLock.Lock();
		m_fileList.push_back(pFile);
		m_SafeLock.Unlock();
	}
	else
	{
		if(pFile)
		{
			fclose(pFile);	
		}
		
		AfxMessageBox(STR_BACKUP_FAILED);
		EndDialog(IDCANCEL);
		return TRUE;
	}
	m_doneNum = 1;
	refreshTip();

	return TRUE;  // return TRUE unless you set the focus to a control
}
void CALLBACK CBackupDlg::fBackupDataCallBack(POINTERHANDLE lFileHandle, UINT dataType, BYTE *pBuffer, UINT dataLen, void *pUser)
{
	TRACE("lFileHandle = %d dataType= %d, len= %d \n",lFileHandle, dataType, dataLen );
	FILE * pFile = (FILE *)pUser;
	if(NET_DVR_BACKUP_DATA_TYPE_DVR == dataType)
	{
		NET_DVR_DOWNlOAD_FRAME_INFO *pFrameInfo = (NET_DVR_DOWNlOAD_FRAME_INFO*)pBuffer;
		if(dataLen == pFrameInfo->nLength+sizeof(NET_DVR_DOWNlOAD_FRAME_INFO))
		{
			printf("fBackupDataCallBack data error \n");
			if(pFile)
				fwrite(pBuffer, dataLen, 1, pFile);
			
		}

	}
	else if(NET_DVR_BACKUP_DATA_TYPE_AVI == dataType)
	{
	}
	else if(NET_DVR_BACKUP_DATA_TYPE_NULL == dataType)
	{
		int *status = (int *)pBuffer;
		
			if(*status == NET_DVR_BACKUP_STOP)
			{
				TRACE("fBackupDataCallBack status=%d \n", *status);
				m_SafeLock.Lock();
				for(std::list<FILE *>::iterator it = m_fileList.begin(); it != m_fileList.end();)
				{
					if(*it == pFile)
					{
						m_fileList.erase(it);
						if(pFile)
							fclose(pFile);
						break;
						
					}
				}
				m_SafeLock.Unlock();
				
			}
			else if(*status == NET_DVR_BACKUP_END)
			{
				TRACE("fBackupDataCallBack status=%d \n", *status);
				
			}
			
		
	}
}
void CBackupDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1010)
	{
		if (m_fileHanle<0)
		{
			return;
		}
		int pos = NET_SDK_GetDownloadPos(m_fileHanle);
		if (pos >= 100)
		{
			if (m_doneNum == m_fileNum)
			{
				delete[] m_backupFiles;
				EndDialog(IDOK);
				KillTimer(1010);
				NET_SDK_StopGetFile(m_fileHanle);
				m_fileHanle = -1;
				return;
			}
			NET_SDK_StopGetFile(m_fileHanle);
			m_fileHanle = -1;
			Sleep(1);
			CString fname;
			fname.Format(_T("%s\\backup_%d.avi"), m_backupPath, m_doneNum + 1);
			FILE	*pFile = fopen(fname.GetBuffer(), "wb");
			if(! pFile)
			{
				AfxMessageBox("fopen file error");
				EndDialog(IDCANCEL);
				return ;
			}
			
			LONG handle = NET_SDK_GetFileByTimeEx(m_userID, m_backupFiles[m_doneNum].dwChannel, &m_backupFiles[m_doneNum].startTime, 
				&m_backupFiles[m_doneNum].stopTime, fname.GetBuffer(), TRUE, TRUE, CBackupDlg::fBackupDataCallBack, pFile);
			if (handle > 0)
			{
				m_fileHanle = handle;
				m_SafeLock.Lock();
				m_fileList.push_back(pFile);
				m_SafeLock.Unlock();
			}
			else
			{
				if(pFile)
					fclose(pFile);
				DWORD error = NET_SDK_GetLastError();
				AfxMessageBox(STR_BACKUP_FAILED);
				EndDialog(IDCANCEL);
				KillTimer(1010);
				return;
			}
			m_doneNum++;
			refreshTip();
		}
		else
		{
			m_progress.SetPos(pos);
		}
	}
	CDialog::OnTimer(nIDEvent);
}

void CBackupDlg::refreshTip()
{
	CString temp;
	temp.Format(_T("%d/%d"), m_doneNum, m_fileNum);
	SetDlgItemText(IDC_STATIC_TIP, temp);
}
