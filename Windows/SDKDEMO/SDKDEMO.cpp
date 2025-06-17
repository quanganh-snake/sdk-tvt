// SDKDEMO.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "SDKDEMODlg.h"
#include "DeviceDlg.h"
#include "ConnectModeDlg.h"
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
#include "IPTool_SearchAndNotifySDK.h"
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSDKDEMOApp

BEGIN_MESSAGE_MAP(CSDKDEMOApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CSDKDEMOApp 构造

CSDKDEMOApp::CSDKDEMOApp():m_lUserID(-1)
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CSDKDEMOApp 对象

CSDKDEMOApp theApp;


void CALLBACK AcceptRegisterProc(LONG lUserID, LONG lRegisterID, LPNET_SDK_DEVICEINFO pDeviceInfo, void *pUser)
{
	CSDKDEMOApp* pThis = (CSDKDEMOApp*)(pUser);
	if (!pThis)
		return;

	pThis->m_lUserID = lUserID;
	memcpy(&(pThis->m_deviceInfo), pDeviceInfo, sizeof(NET_SDK_DEVICEINFO));
}

void CALLBACK AcceptUnRegisterProc(LONG lUserID, LONG lRegisterID, LPNET_SDK_DEVICEINFO pDeviceInfo, void *pUser)
{
	CSDKDEMOApp* pThis = (CSDKDEMOApp*)(pUser);
	if (!pThis)
		return;


	//
	char *test =  new char[1 * sizeof(REG_LOGIN_INFO)];
	char* pli =test;
	memset(pli,0,1 * sizeof(REG_LOGIN_INFO));

	REG_LOGIN_INFO* plii = (REG_LOGIN_INFO*)pli;
	plii->deviceId = lRegisterID;
	
	//这部分由客户自己获取自己平台的用户名密码
	memcpy(plii->m_szUserName,"admin",5);
	memcpy(plii->m_szPasswd,"123456",6);
	////
	NET_SDK_AddRegisterDeviceInfo((REG_LOGIN_INFO*)(test),1);

	delete[] test;
	test = NULL;
	return;
	//pThis->m_lUserID = lUserID;
	//memcpy(&(pThis->m_deviceInfo), pDeviceInfo, sizeof(NET_SDK_DEVICEINFO));
}
// CSDKDEMOApp 初始化

BOOL CSDKDEMOApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	AfxEnableControlContainer();
	
	CoInitialize(NULL);
	//初始化sdk
	NET_SDK_Init();
	NET_SDK_SetConnectTime(5000, 1);
	NET_SDK_SetReconnect();
	DWORD BuildVersion = NET_SDK_GetSDKBuildVersion();
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
	IPTOOL_SEARCH_AND_NOTIFY_Initial();
#endif
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CConnectModeDlg connectModeDlg;
	if(IDOK == connectModeDlg.DoModal())
	{
		if(CConnectModeDlg::CONNECT_MODE_LOGIN_TO_DEVICE == connectModeDlg.m_connectMode)
		{
			while(true)
			{
				CDeviceDlg *deviceDlg = new CDeviceDlg;
				CSDKDEMODlg dlg;
//#define USER_DEVICE_SERACH
#ifdef USER_DEVICE_SERACH
				if (deviceDlg->DoModal() == IDOK)//去掉扫描对话框
#else
				if(1)
#endif
				{
#ifdef USER_DEVICE_SERACH
					dlg.SetDeviceInfo(deviceDlg->GetIp(), deviceDlg->GetPort());
#else
					dlg.SetDeviceInfo("192.168.51.37", 9008);
#endif
					dlg.DoModal();

					delete deviceDlg;
					break;
				}
				else
				{
					delete deviceDlg;
					break;
				}
				delete deviceDlg;		
			}
		}
		else
		{
			
			char *test =  new char[3 * sizeof(REG_LOGIN_INFO)];
			char* pli =test;
			memset(pli,0,3 * sizeof(REG_LOGIN_INFO));

			REG_LOGIN_INFO* plii = (REG_LOGIN_INFO*)pli;
			plii->deviceId = ::GetPrivateProfileInt("RegDevInfo","id",0,"./SDK_config.ini");

			char reBuf[128] = {0};
			::GetPrivateProfileString("RegDevInfo","username","",reBuf,128,"./SDK_config.ini");
			memcpy(plii->m_szUserName,reBuf,sizeof(reBuf));
			::GetPrivateProfileString("RegDevInfo","password","",reBuf,128,"./SDK_config.ini");
			memcpy(plii->m_szPasswd,reBuf,sizeof(reBuf));
			/*memcpy(plii->m_szUserName,"admin",5);
			memcpy(plii->m_szPasswd,"123456987",9);*/

			pli+= sizeof(REG_LOGIN_INFO);
			REG_LOGIN_INFO* plii2 = (REG_LOGIN_INFO*)pli;
			plii2->deviceId = 20133;
			memcpy(plii2->m_szUserName,"admin",5);
			memcpy(plii2->m_szPasswd,"123456987",9);

			pli+= sizeof(REG_LOGIN_INFO);
			REG_LOGIN_INFO* plii3 = (REG_LOGIN_INFO*)pli;
			plii3->deviceId = 40134;
			memcpy(plii3->m_szUserName,"admin",5);
			memcpy(plii3->m_szPasswd,"11",2);

			NET_SDK_AddRegisterDeviceInfo((REG_LOGIN_INFO*)(test),3);
			delete[] test;
			test = NULL;
			NET_SDK_SetRegisterPort(connectModeDlg.m_registerPort);
			//NET_SDK_SetRegisterPort(connectModeDlg.m_registerPort,(REG_LOGIN_INFO*)(pli - 2*sizeof(REG_LOGIN_INFO)),3);
			NET_SDK_SetRegisterCallback(AcceptRegisterProc, this);
			NET_SDK_SetUnRegisterCallback(AcceptUnRegisterProc, this);

			int tryTimes = 0;
			CSDKDEMODlg dlg;
			while(-1 == m_lUserID)
			{
				Sleep(10);
				tryTimes++;

				if(tryTimes > 10000)
				{
					AfxMessageBox("No device register!");
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
					IPTOOL_SEARCH_AND_NOTIFY_Quit();
#endif
					NET_SDK_Cleanup();
					return FALSE;
				}
			}

			dlg.SetDeviceInfo(m_lUserID, &m_deviceInfo);
			dlg.DoModal();
		}
	}
#ifdef _IPTOOL_SEARCH_AND_NOTIFY_SDK_H
	IPTOOL_SEARCH_AND_NOTIFY_Quit();
#endif
	NET_SDK_Cleanup();
	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int CSDKDEMOApp::ExitInstance()
{
	// TODO: 在此添加专用代码和/或调用基类
	CoUninitialize();

	return CWinApp::ExitInstance();
}
