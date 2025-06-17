// OthersDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "OthersDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include "XMLParse.h"

// COthersDlg 对话框

IMPLEMENT_DYNAMIC(COthersDlg, CDialog)

COthersDlg::COthersDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COthersDlg::IDD, pParent)
{

}

COthersDlg::~COthersDlg()
{
}

void COthersDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_WATERMASKON, m_comWatermaskOn);
}


BEGIN_MESSAGE_MAP(COthersDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_WATERMASKSETTING, &COthersDlg::OnBnClickedButtonWatermasksetting)
END_MESSAGE_MAP()


BOOL COthersDlg::OnInitDialog()
{
	CComboBox * combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_WATERMASKON));
	combo->AddString("开");
	combo->AddString("关");

	combo->SetCurSel(0);

	
	return TRUE;
}
void COthersDlg::setDeviceInfo( NET_SDK_DEVICEINFO *di, LONG userid )
{
	m_userID = userid; 
	m_devieInfo = di;

	//以下命令都为透传 Demo界面演示
	getChlGUIDInfo();

	CString temp;
	CComboBox * combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_WATERSETCHL));

	/*map<int, N9000_GUID_CHANNEL_TYPE>::iterator ChannelIter; 

	for (ChannelIter = m_ChannelGUIDInfo.begin();ChannelIter != m_ChannelGUIDInfo.end();ChannelIter++)
	{
		temp.Format("通道:%d",ChannelIter->first);
		combo->AddString(temp);
	}
	combo->SetCurSel(0);

	QueryChlWatermark(combo->GetCurSel());*/
}
// COthersDlg 消息处理程序


void COthersDlg::OnBnClickedButtonWatermasksetting()
{
	//水印设置 通过透传
	/*std::string url = "SetPassLineCountConfig";
	std::string sendXML = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <config version=\"1.7\" xmlns=\"http://www.ipc.com/ver10\">"
		"<passlinecount>"	
		"<forceReset type=\"boolean\">true</forceReset>"
		"</passlinecount>"
		"</config>";

	char *recvbuf = new char[1024];
	memset(recvbuf, 0, 1024);
	DWORD lReturned = 0;
	BOOL ret = NET_SDK_TransparentConfig(m_userID, (char *)sendXML.c_str(), (char*)url.c_str(), recvbuf, 1024, &lReturned);*/
}

bool COthersDlg::QueryChlWatermark(int chlId)
{
	return true;
}


void COthersDlg::getChlGUIDInfo()
{
	CString strUrl = "queryNodeList";

	CString sendXML = "<?xml version=\"1.0\" encoding=\"utf-8\" ?><request version=\"1.0\" systemType=\"NVMS-9000\" clientType=\"WEB\">\
								<types><nodeType><enum>chls</enum><enum>sensors</enum><enum>alarmOuts</enum></nodeType><chlType><enum>analog</enum>\
								<enum>digital</enum><enum>all</enum></chlType></types>\
								<nodeType type=\"nodeType\">chls</nodeType><requireField><name/></requireField><condition><chlType type=\"chlType\">analog</chlType>\
								</condition></request>";

	DWORD lpBytesReturned = 0;
	char *lpOutBuffer = new char[10*1024];
	memset(lpOutBuffer, 0, 10*1024);
	BOOL bSendRet = NET_SDK_TransparentConfig(m_userID,sendXML.GetBuffer(0), strUrl.GetBuffer(0), lpOutBuffer, 10*1024,&lpBytesReturned);
	if (bSendRet && (lpOutBuffer != NULL))
	{
		CXMLParse xmlParse;
		xmlParse.LoadXML(lpOutBuffer);

		std::string tempValueStr;
		xmlParse.GetValueStr("response/status", tempValueStr);
		if(tempValueStr != "success")
		{
			delete[] lpOutBuffer;
			lpOutBuffer = NULL;
			lpBytesReturned = 0;
			//MyAssert(FALSE);
			printf("%s, %s,%d  GetXmlStatus \n", __FILE__, __FUNCTION__, __LINE__);
			return;
		}
		tuint_ptr ptrContent = xmlParse.GetFirstItem("response/content");
		int chNum = 0;
		xmlParse.GetAttribute_i32(ptrContent, "total", chNum);
		tuint_ptr ptrItem = xmlParse.GetFirstItem(ptrContent, "item");
		std::string strChID;
		while(ptrItem)
		{
			strChID.clear();
			xmlParse.GetAttributeStr(ptrItem, "id", strChID);
			if (strChID != "")
			{
				int channel = -1;
				GUID chlGUID = GetGuidFromString(strChID.c_str());
				channel = chlGUID.Data1;
				//m_ChannelGUIDInfo[channel-1].chlGuid = GetGuidFromString(strChID.c_str());
				/*if (i == DIGITAL_CHANE_LIST_XML)
				{
					m_ChannelGUIDInfo[channel-1].chlType = DIGITAL_CHL_TYPE;
				}
				else
				{
					m_ChannelGUIDInfo[channel-1].chlType = ANALOG_CHL_TYPE;
				}*/
			}

			ptrItem =  xmlParse.GetNextSiblingItem(ptrItem);
		}

		//m_ChannelListLock.UnLock();
		delete[] lpOutBuffer;
		lpOutBuffer = NULL;
		lpBytesReturned = 0;
	}
	return;
}

GUID COthersDlg::GetGuidFromString(const char *pGUIDStr)
{
	GUID guid = GUID_NULL;

	int Data2, Data3;
	int d0, d1, d2, d3, d4, d5, d6, d7;
	int n = sscanf(pGUIDStr,"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		&guid.Data1, &Data2, &Data3, &d0, &d1,
		&d2, &d3, &d4, &d5, &d6, &d7
		);

	if (n!=11)
	{
		//MyAssert(false);
		return GUID_NULL;
	}

	guid.Data2 = Data2; guid.Data3 = Data3;
	guid.Data4[0] = d0; guid.Data4[1] = d1;
	guid.Data4[2] = d2; guid.Data4[3] = d3;
	guid.Data4[4] = d4; guid.Data4[5] = d5;
	guid.Data4[6] = d6; guid.Data4[7] = d7;

	return guid;
}
