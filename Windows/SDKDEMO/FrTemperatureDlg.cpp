// ToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "FrTemperatureDlg.h"
#include "GridCellCombo.h"
#include "FormatDlg.h"
#include <WinSock2.h>
#include <Wincrypt.h>
#include "strdef.h"
#include <MMSystem.h>
#include "XMLParse.h"
//#include "DDPublic.h"
#include "Base64.h"


#include <stdio.h>

#include <direct.h>
#include <iostream>
#include <io.h>
#include "atlimage.h"

IMPLEMENT_DYNAMIC(CFrTemperatureDlg, CDialog)

CFrTemperatureDlg::CFrTemperatureDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFrTemperatureDlg::IDD, pParent)
	
{
	
}

CFrTemperatureDlg::~CFrTemperatureDlg()
{
	
}

void CFrTemperatureDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	
}


BEGIN_MESSAGE_MAP(CFrTemperatureDlg, CDialog)
	//ON_BN_CLICKED(IDOK, &CFrTemperatureDlg::OnBnClickedOk)
	//ON_BN_CLICKED(IDCANCEL, &CFrTemperatureDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_get_temperature, &CFrTemperatureDlg::OnBnClickeGetTemperature)
	ON_BN_CLICKED(IDC_BUTTON_set_temperature, &CFrTemperatureDlg::OnBnClickeSetTemperature)
	ON_BN_CLICKED(IDC_BUTTON_get_mask, &CFrTemperatureDlg::OnBnClickeGetMaskConfig)
	ON_BN_CLICKED(IDC_BUTTON_set_mask, &CFrTemperatureDlg::OnBnClickeSetMaskConfig)
	ON_BN_CLICKED(IDC_BUTTON_get_thermal, &CFrTemperatureDlg::OnBnClickeGetThermalConfig)
	ON_BN_CLICKED(IDC_BUTTON_set_thermal, &CFrTemperatureDlg::OnBnClickeSetThermalConfig)
	ON_BN_CLICKED(IDC_BUTTON_get_passline, &CFrTemperatureDlg::OnBnClickeGetPasslineConfig)
	ON_BN_CLICKED(IDC_BUTTON_set_passline, &CFrTemperatureDlg::OnBnClickeSetPasslineConfig)
	ON_CBN_SELCHANGE(IDC_COMBO_unit, &CFrTemperatureDlg::OnBnClickeChangeUnit)
	
	ON_BN_CLICKED(IDC_BUTTON_passline_reset, &CFrTemperatureDlg::OnBnClickedButtonpasslineReset)
	ON_BN_CLICKED(IDC_BUTTON_get_passline_count, &CFrTemperatureDlg::OnBnClickedButtongetpasslinecount)
END_MESSAGE_MAP()


// CToolDlg 消息处理程序

void CFrTemperatureDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CFrTemperatureDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}




BOOL CFrTemperatureDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	memset(&m_DevSupport, 0, sizeof(NET_SDK_DEV_SUPPORT));

	CComboBox * combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_unit));
	combo->AddString("℃");
	combo->AddString("℉");

	combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_alarmtime));
	combo->AddString("3s");
	combo->AddString("5s");
	combo->AddString("10s");
	combo->AddString("20s");
	combo->AddString("30s");
	combo->AddString("60s");
	combo->AddString("2min");

	combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_mask_alarmtime));
	combo->AddString("3s");
	combo->AddString("5s");
	combo->AddString("10s");
	combo->AddString("20s");
	combo->AddString("30s");
	combo->AddString("60s");
	combo->AddString("2min");
	//UpdateUI();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

bool UTF8ToGBEx(char* pDest, int nDestLen, const char* pSrc, int nSrcLen)
{
	if(!pDest || !pSrc || nDestLen <= 0 || nSrcLen <= 0)
		return false;

#ifdef WIN32
	WCHAR *strSrc = NULL;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, NULL, 0);
	strSrc = new WCHAR[i+1];
	MultiByteToWideChar(CP_UTF8, 0, pSrc, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, pDest, i, NULL, NULL);

	delete []strSrc;
#else

#endif
	return true;
}
void CFrTemperatureDlg::UpdateMsg(LONG ID, LONG lCommand, void *data, int length)
{
	
	switch(lCommand)
	{
		/*	----------------------
		face match:
		|	NET_SDK_IVE_BASE_INFO	|
		-------------------------
		|	NET_SDK_IVE_PICTURE_INFO	|
		-------------------------
		|	snap picture data		|
		-------------------------
		|	NET_SDK_IVE_PICTURE_INFO	|
		-------------------------
		|	album picture data		|


		card:
		|	NET_SDK_IVE_BASE_INFO	|
		-------------------------
		|	NET_SDK_IVE_PICTURE_INFO	|
		-------------------------
		|	album picture data		|
		-------------------------*/
	case NET_SDK_SMART_EVENT_TYPE_FACE_MATCH_FOR_IPC:
		{
			NET_SDK_IVE_BASE_INFO *baseInfo = (NET_SDK_IVE_BASE_INFO*)data;
			CString temp;
			char szDest[128] = {0};
			UTF8ToGBEx(szDest, sizeof(szDest), baseInfo->szName, DD_MAX_CAMERA_NAME_BUF_LEN);
			if(baseInfo->iSnapPicId == 0 && 1== baseInfo->comparisonRes)//刷卡回调 the information of card
			{
				temp.Format("name: %s \r\n cardNo: %s \r\n comparisonRes: %d \r\n livingBody:%d \r\n",baseInfo->szName, baseInfo->szIdentifyNum,baseInfo->comparisonRes, baseInfo->livingBody);
				SetDlgItemText(IDC_EDIT_msg, temp);
			}
			else
			{				
				float temperature = 0.0;
				if (baseInfo->temperature)
				{
					temperature = (float)baseInfo->temperature/100;
				}
				temp.Format("temperature: %.2f \r\n living: %d \r\n comparisonRes: %d \r\n name:%s \r\n iSimilarity: %d", temperature, baseInfo->livingBody, baseInfo->comparisonRes,  szDest, baseInfo->iSimilarity);

				SetDlgItemText(IDC_EDIT_msg, temp);
			}



			
		}
		break;
	case  NET_SDK_SMART_EVENT_TYPE_VFD:
		{
			char* tmp = (char*)data;
			// 解析头
			NET_DVR_IVE_VFD_RESULT_HEAD_T* head = (NET_DVR_IVE_VFD_RESULT_HEAD_T*)tmp;
			tmp += sizeof(NET_DVR_IVE_VFD_RESULT_HEAD_T);
			// 解析原始数据
			NET_DVR_IVE_VFD_RESULT_DATA_INFO_T* src = (NET_DVR_IVE_VFD_RESULT_DATA_INFO_T*)tmp;     
			tmp += sizeof(NET_DVR_IVE_VFD_RESULT_DATA_INFO_T);
			
			if(src->dataLen > 0)
			{
				tmp += src->dataLen;		       
			}

			for(int i=0; i<head->faceCnt; i++)
			{
				NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T* face = (NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T*)tmp;
				tmp += sizeof(NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T);  

				CString temp;
				float temperature = 0.0;
				if (face->temperature)
				{
					temperature = (float)face->temperature/100;
				}

				temp.Format("temperature: %.2f \r\n wearmask: %d \r\n comprehensive_score: %f \r\n faceliveness:%d", temperature, face->wearmask, face->comprehensive_score,  face->faceliveness);

				SetDlgItemText(IDC_EDIT_msg_vfd, temp);

				// face Image, 如果有
				if(face->stFaceImgData.dataLen > 0)
				{					
					tmp+= face->stFaceImgData.dataLen;		
				}

				// feature, 如果有
				if(face->featureSize > 0)
				{
					tmp += face->featureSize;

				}
			}
		}
		break;
	case NET_SDK_SMART_EVENT_TYPE_PASSLINE:
		{
			NET_SDK_IVE_PASSLINECOUNT_T* sIVE_PassLine = (NET_SDK_IVE_PASSLINECOUNT_T*)data;
			CString temp;
			temp.Format(" Person  enter:%d leave:%d\r\n Car  enter:%d leave:%d\r\n Bike:%d leave:%d\r\n ", sIVE_PassLine->enterPersonCount, sIVE_PassLine->leavePersonCount,
				sIVE_PassLine->enterCarCount, sIVE_PassLine->leaveCarCount, sIVE_PassLine->enterBikeCount, sIVE_PassLine->leaveBikeCount);

			SetDlgItemText(IDC_EDIT_msg_passline, temp);
		}
		break;
	}
}

void CFrTemperatureDlg::setMainDlg( CWnd* dlg )
{
	m_mainDlg = dlg;
}

void CFrTemperatureDlg::setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid)
{
	m_userID = userid; 
	m_deviceinfo = di;
	
	memset(&m_DevSupport, 0, sizeof(NET_SDK_DEV_SUPPORT));
	int ret = NET_SDK_GetDeviceSupportFunction(userid, &m_DevSupport);
	if (ret)
	{
		int test = 0;
	}
}


void CFrTemperatureDlg::OnBnClickeGetTemperature()
{
	if (! m_DevSupport.supportThermometry)
	{
		AfxMessageBox("not supportThermometry!");
		return;
	}
	USES_CONVERSION;
	std::string url = "GetTakeTemperatureConfig";
	char *recvbuf = new char[1024*1024];
	memset(recvbuf, 0, 1024*1024);
	DWORD lReturned = 0;
	BOOL ret = NET_SDK_TransparentConfig(m_userID, "", (char*)url.c_str(), recvbuf, 1024*1024, &lReturned);
	if(ret == 0)
	{
		delete recvbuf;
		AfxMessageBox(" GetTakeTemperatureConfig faild!");
		return;
	}
	else if(lReturned)
	{	
		m_temperatureXmlParse.LoadXML(recvbuf);
		std::string tempValueStr;
		std::string tempUnits;
		std::string highThreshold;
		std::string lowThreshold;
		if (m_temperatureXmlParse.GetValueStr("config/TakeTemperature/takeEanble", tempValueStr))
		{
			if (tempValueStr == "true")
			{
					reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_enable))->SetCheck(TRUE);
			}
			else
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_enable))->SetCheck(FALSE);
			}
		}
		if (m_temperatureXmlParse.GetValueStr("config/TakeTemperature/tempUnits", tempUnits))
		{
			if (tempUnits == "centigrade")
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_unit))->SetCurSel(0);
				highThreshold = "highThreshold";
				lowThreshold = "lowThreshold";
			}
			else
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_unit))->SetCurSel(1);
				highThreshold = "FhighThreshold";
				lowThreshold = "FlowThreshold";
			}
		}
		tuint_ptr ptrhighThreshold = m_temperatureXmlParse.GetFirstItem("config/TakeTemperature/"+highThreshold);
		if (ptrhighThreshold)
		{
			if (m_temperatureXmlParse.GetValueStr(ptrhighThreshold, "switch", tempValueStr))
			{
				if (tempValueStr == "true")
				{
					reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_high_temperature))->SetCheck(TRUE);
				}
				else
				{
					reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_high_temperature))->SetCheck(FALSE);
				}
			}
			tuint_ptr ptrvalue = m_temperatureXmlParse.GetFirstItem(ptrhighThreshold, "value");
			if (ptrvalue)
			{
				int Value=0;
				int Valuel=0;
				float Value_f = 0.0;
				float Value_fl = 0.0;
				char buf_s [40]={0};
				if (m_temperatureXmlParse.GetValue_i32(ptrvalue, Value))
				{
					Value_f = (float)Value/100;
					sprintf (buf_s, "%.2f", Value_f);
					SetDlgItemText(IDC_EDIT_high_temperature, A2T(buf_s));
				}
				Value_f = 0.0;
				
				
				m_temperatureXmlParse.GetAttribute_i32(ptrvalue, "min", Value);
				m_temperatureXmlParse.GetAttribute_i32(ptrvalue, "max", Valuel);
				Value_f = (float)Value/100;
				Value_fl = (float)Valuel/100;
				memset(buf_s, 0, sizeof(buf_s));
				if (tempUnits == "centigrade")
				{
					sprintf (buf_s, "(%.2f-%.2f)℃", Value_f, Value_fl);
				}
				else
				{
					sprintf (buf_s, "(%.2f-%.2f)℉", Value_f, Value_fl);
				}
	
				SetDlgItemText(IDC_STATIC_high, A2T(buf_s));
			}
		}

		tuint_ptr ptrlowThreshold = m_temperatureXmlParse.GetFirstItem("config/TakeTemperature/"+lowThreshold);
		if (ptrlowThreshold)
		{
			if (m_temperatureXmlParse.GetValueStr(ptrlowThreshold, "switch", tempValueStr))
			{
				if (tempValueStr == "true")
				{
					reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_low_temperature))->SetCheck(TRUE);
				}
				else
				{
					reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_low_temperature))->SetCheck(FALSE);
				}
			}
			tuint_ptr ptrvalue = m_temperatureXmlParse.GetFirstItem(ptrlowThreshold, "value");
			if (ptrvalue)
			{
				int Value=0;
				int Valuel=0;
				float Value_f = 0.0;
				float Value_fl = 0.0;
				char buf [40]={0};
				if (m_temperatureXmlParse.GetValue_i32(ptrvalue, Value))
				{
					Value_f = (float)Value/100;
					sprintf (buf, "%.2f", Value_f);
					SetDlgItemText(IDC_EDIT_low_temperature, A2T(buf));
				}
				Value_f = 0.0;
				m_temperatureXmlParse.GetAttribute_i32(ptrvalue, "min", Value);
				m_temperatureXmlParse.GetAttribute_i32(ptrvalue, "max", Valuel);
				Value_f = (float)Value/100;
				Value_fl = (float)Valuel/100;
				memset(buf, 0, sizeof(buf));
				if (tempUnits == "centigrade")
				{
					sprintf (buf, "(%.2f-%.2f)℃", Value_f, Value_fl);
				}
				else
				{
					sprintf (buf, "(%.2f-%.2f)℉", Value_f, Value_fl);
				}
				SetDlgItemText(IDC_STATIC_low, A2T(buf));
			}
		}
		int nValue;
		if (m_temperatureXmlParse.GetValue_i32("config/TakeTemperature/alarmHoldTime", nValue))
		{
			if (0 <= nValue && nValue <=3)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_alarmtime))->SetCurSel(0);
			}
			else if (3 < nValue && nValue <=5)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_alarmtime))->SetCurSel(1);
			}
			else if (5 < nValue && nValue <=10)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_alarmtime))->SetCurSel(2);
			}
			else if (10 < nValue && nValue <=20)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_alarmtime))->SetCurSel(3);
			}
			else if (20 < nValue && nValue <=30)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_alarmtime))->SetCurSel(4);
			}
			else if (30 < nValue && nValue <=60)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_alarmtime))->SetCurSel(5);
			}
			else if (60 < nValue)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_alarmtime))->SetCurSel(6);
			}
		}
		if (m_temperatureXmlParse.GetValueStr("config/TakeTemperature/savePicSwitch", tempValueStr))
		{
			if (tempValueStr == "true")
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_capture))->SetCheck(TRUE);
			}
			else
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_capture))->SetCheck(FALSE);
			}
		}
		if (m_temperatureXmlParse.GetValueStr("config/TakeTemperature/sdRecSwitch", tempValueStr))
		{
			if (tempValueStr == "true")
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_recode))->SetCheck(TRUE);
			}
			else
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_recode))->SetCheck(FALSE);
			}
		}
		if (m_temperatureXmlParse.GetValueStr("config/TakeTemperature/audioSwitch", tempValueStr))
		{
			if (tempValueStr == "true")
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_voice))->SetCheck(TRUE);
			}
			else
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_voice))->SetCheck(FALSE);
			}
		}	
	}
	delete recvbuf;
}

void CFrTemperatureDlg::OnBnClickeSetTemperature()
{
	if (! m_DevSupport.supportThermometry)
	{
		AfxMessageBox("not supportThermometry!");
		return;
	}
	std::string tempValueStr;
	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_enable))->GetCheck())
	{
		tempValueStr = "true";
	}
	else
	{
		tempValueStr = "false";
	}
	m_temperatureXmlParse.SetValue("config/TakeTemperature/takeEanble", tempValueStr);

	std::string tempUnits;
	std::string highThreshold;
	std::string lowThreshold;
	int index = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_unit))->GetCurSel();
	if (index == 0)
	{
		tempUnits = "centigrade";
	}
	else
	{
		tempUnits = "Fahrenheit";
	}
	m_temperatureXmlParse.SetValue("config/TakeTemperature/tempUnits", tempUnits);
	if (tempUnits == "centigrade")
	{
		highThreshold = "highThreshold";
		lowThreshold = "lowThreshold";
	}
	else
	{
		highThreshold = "FhighThreshold";
		lowThreshold = "FlowThreshold";
	}
	USES_CONVERSION;
	CString CstrValue;
	GetDlgItemText(IDC_EDIT_high_temperature, CstrValue);
	float Value_f = atof(CstrValue);
	int Value = Value_f * 100;
	tempValueStr = "config/TakeTemperature/"+highThreshold;
	tempValueStr += "/value";
	tuint_ptr ptrhighValue = m_temperatureXmlParse.GetFirstItem(tempValueStr);
	if (ptrhighValue)
	{
		int Valuemin=0;
		int Valuemax=0;
		m_temperatureXmlParse.GetAttribute_i32(ptrhighValue, "min", Valuemin);
		m_temperatureXmlParse.GetAttribute_i32(ptrhighValue, "max", Valuemax);
		if (Value > Valuemax || Value < Valuemin)
		{
			AfxMessageBox(" error  temperature value!");
			return;
		}
		m_temperatureXmlParse.SetValue_i32(ptrhighValue, Value);
	}
	
	GetDlgItemText(IDC_EDIT_low_temperature, CstrValue);
	Value_f = atof(CstrValue);
	Value = Value_f * 100;
	tempValueStr = "config/TakeTemperature/"+lowThreshold;
	tempValueStr += "/value";
	tuint_ptr ptrlowValue = m_temperatureXmlParse.GetFirstItem(tempValueStr);
	if (ptrlowValue)
	{
		int Valuemin=0;
		int Valuemax=0;
		m_temperatureXmlParse.GetAttribute_i32(ptrlowValue, "min", Valuemin);
		m_temperatureXmlParse.GetAttribute_i32(ptrlowValue, "max", Valuemax);
		if (Value > Valuemax || Value < Valuemin)
		{
			AfxMessageBox(" error  temperature value!");
			return;
		}
		m_temperatureXmlParse.SetValue_i32(ptrlowValue, Value);
	}

	tempValueStr = "config/TakeTemperature/"+highThreshold;
	tempValueStr += "/switch";

	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_high_temperature))->GetCheck())
	{
		m_temperatureXmlParse.SetValue(tempValueStr, "true");
	}
	else
	{
		m_temperatureXmlParse.SetValue(tempValueStr, "false");
	}

	tempValueStr = "config/TakeTemperature/"+lowThreshold;
	tempValueStr += "/switch";

	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_low_temperature))->GetCheck())
	{
		m_temperatureXmlParse.SetValue(tempValueStr, "true");
	}
	else
	{
		m_temperatureXmlParse.SetValue(tempValueStr, "false");
	}
	
	index = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_alarmtime))->GetCurSel();
	int times = 3;
	switch(index)
	{
	case 0:
		times = 3;
		break;
	case 1:
		times = 5;
		break;
	case 2:
		times = 10;
		break;
	case 3:
		times = 20;
		break;
	case 4:
		times = 30;
		break;
	case 5:
		times = 60;
		break;
	case 6:
		times = 120;
		break;
	default:
		break;
	}
	m_temperatureXmlParse.SetValue_i32("config/TakeTemperature/alarmHoldTime", times);

	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_capture))->GetCheck())
	{
		tempValueStr = "true";
	}
	else
	{
		tempValueStr = "false";
	}
	m_temperatureXmlParse.SetValue("config/TakeTemperature/savePicSwitch", tempValueStr);

	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_recode))->GetCheck())
	{
		tempValueStr = "true";
	}
	else
	{
		tempValueStr = "false";
	}
	m_temperatureXmlParse.SetValue("config/TakeTemperature/sdRecSwitch", tempValueStr);

	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_voice))->GetCheck())
	{
		tempValueStr = "true";
	}
	else
	{
		tempValueStr = "false";
	}
	m_temperatureXmlParse.SetValue("config/TakeTemperature/audioSwitch", tempValueStr);

	std::string url = "SetTakeTemperatureConfig";
	char *recvbuf = new char[1024*1024];
	memset(recvbuf, 0, 1024*1024);
	DWORD lReturned = 0;
	BOOL ret = NET_SDK_TransparentConfig(m_userID, (char *)m_temperatureXmlParse.GetDoc().c_str(), (char*)url.c_str(), recvbuf, 1024*1024, &lReturned);
	if(ret == 0)
	{
		delete recvbuf;
		AfxMessageBox(" SetTakeTemperatureConfig faild!");
		return;
	}
	else if(lReturned)
	{
		CXMLParse xmlParse;
		xmlParse.LoadXML(recvbuf);
		std::string tempValueStr;
		xmlParse.GetAttributeStr("config", "status", tempValueStr);
		if(tempValueStr == "success")
		{
			AfxMessageBox(" SetTakeTemperatureConfig success!");
		}
		else
		{
			AfxMessageBox(" SetTakeTemperatureConfig faild!");
		}
		delete recvbuf;
	}
	
}

void CFrTemperatureDlg::OnBnClickeGetMaskConfig()
{
	if (! m_DevSupport.supportThermometry)
	{
		AfxMessageBox("not supportThermometry!");
		return;
	}
	USES_CONVERSION;
	std::string url = "GetWearmaskDetectConfig";
	char *recvbuf = new char[1024*1024];
	memset(recvbuf, 0, 1024*1024);
	DWORD lReturned = 0;
	BOOL ret = NET_SDK_TransparentConfig(m_userID, "", (char*)url.c_str(), recvbuf, 1024*1024, &lReturned);
	if(ret == 0)
	{
		delete recvbuf;
		AfxMessageBox(" GetWearmaskDetectConfig faild!");
		return;
	}
	else if(lReturned)
	{	
		m_maskXmlParse.LoadXML(recvbuf);
		std::string tempValueStr;
		std::string tempUnits;
		std::string highThreshold;
		std::string lowThreshold;
		if (m_maskXmlParse.GetValueStr("config/WearmaskDetect/switch", tempValueStr))
		{
			if (tempValueStr == "true")
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_enable))->SetCheck(TRUE);
			}
			else
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_enable))->SetCheck(FALSE);
			}
		}
		
		int nValue;
		if (m_maskXmlParse.GetValue_i32("config/WearmaskDetect/alarmHoldTime", nValue))
		{
			if (0 <= nValue && nValue <=3)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_mask_alarmtime))->SetCurSel(0);
			}
			else if (3 < nValue && nValue <=5)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_mask_alarmtime))->SetCurSel(1);
			}
			else if (5 < nValue && nValue <=10)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_mask_alarmtime))->SetCurSel(2);
			}
			else if (10 < nValue && nValue <=20)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_mask_alarmtime))->SetCurSel(3);
			}
			else if (20 < nValue && nValue <=30)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_mask_alarmtime))->SetCurSel(4);
			}
			else if (30 < nValue && nValue <=60)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_mask_alarmtime))->SetCurSel(5);
			}
			else if (60 < nValue)
			{
				reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_mask_alarmtime))->SetCurSel(6);
			}
		}
		if (m_maskXmlParse.GetValueStr("config/WearmaskDetect/savePicSwitch", tempValueStr))
		{
			if (tempValueStr == "true")
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_capture))->SetCheck(TRUE);
			}
			else
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_capture))->SetCheck(FALSE);
			}
		}
		if (m_maskXmlParse.GetValueStr("config/WearmaskDetect/sdRecSwitch", tempValueStr))
		{
			if (tempValueStr == "true")
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_recode))->SetCheck(TRUE);
			}
			else
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_recode))->SetCheck(FALSE);
			}
		}
		if (m_maskXmlParse.GetValueStr("config/WearmaskDetect/audioSwitch", tempValueStr))
		{
			if (tempValueStr == "true")
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_voice))->SetCheck(TRUE);
			}
			else
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_voice))->SetCheck(FALSE);
			}
		}	
	}
	delete recvbuf;
}
void CFrTemperatureDlg::OnBnClickeSetMaskConfig()
{
	if (! m_DevSupport.supportThermometry)
	{
		AfxMessageBox("not supportThermometry!");
		return;
	}
	std::string tempValueStr;
	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_enable))->GetCheck())
	{
		tempValueStr = "true";
	}
	else
	{
		tempValueStr = "false";
	}
	m_maskXmlParse.SetValue("config/WearmaskDetect/switch", tempValueStr);

	int index = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_mask_alarmtime))->GetCurSel();
	int times = 3;
	switch(index)
	{
	case 0:
		times = 3;
		break;
	case 1:
		times = 5;
		break;
	case 2:
		times = 10;
		break;
	case 3:
		times = 20;
		break;
	case 4:
		times = 30;
		break;
	case 5:
		times = 60;
		break;
	case 6:
		times = 120;
		break;
	default:
		break;
	}
	m_maskXmlParse.SetValue_i32("config/WearmaskDetect/alarmHoldTime", times);

	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_capture))->GetCheck())
	{
		tempValueStr = "true";
	}
	else
	{
		tempValueStr = "false";
	}
	m_maskXmlParse.SetValue("config/WearmaskDetect/savePicSwitch", tempValueStr);

	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_recode))->GetCheck())
	{
		tempValueStr = "true";
	}
	else
	{
		tempValueStr = "false";
	}
	m_maskXmlParse.SetValue("config/WearmaskDetect/sdRecSwitch", tempValueStr);

	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_mask_voice))->GetCheck())
	{
		tempValueStr = "true";
	}
	else
	{
		tempValueStr = "false";
	}
	m_maskXmlParse.SetValue("config/WearmaskDetect/audioSwitch", tempValueStr);

	std::string url = "SetWearmaskDetectConfig";
	char *recvbuf = new char[1024*1024];
	memset(recvbuf, 0, 1024*1024);
	DWORD lReturned = 0;
	BOOL ret = NET_SDK_TransparentConfig(m_userID, (char *)m_maskXmlParse.GetDoc().c_str(), (char*)url.c_str(), recvbuf, 1024*1024, &lReturned);
	if(ret == 0)
	{
		delete recvbuf;
		AfxMessageBox(" SetWearmaskDetectConfig faild!");
		return;
	}
	else if(lReturned)
	{
		CXMLParse xmlParse;
		xmlParse.LoadXML(recvbuf);
		std::string tempValueStr;
		xmlParse.GetAttributeStr("config", "status", tempValueStr);
		if(tempValueStr == "success")
		{
			AfxMessageBox(" SetWearmaskDetectConfig success!");
		}
		else
		{
			AfxMessageBox(" SetWearmaskDetectConfig faild!");
		}
		delete recvbuf;
	}

}

void CFrTemperatureDlg::OnBnClickeChangeUnit()
{
	std::string highThreshold, lowThreshold;
	int index = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_unit))->GetCurSel();
	if (index == 0)
	{
		highThreshold = "highThreshold";
		lowThreshold = "lowThreshold";

	}
	else
	{
		highThreshold = "FhighThreshold";
		lowThreshold = "FlowThreshold";
	}
	
	tuint_ptr ptrhighThreshold = m_temperatureXmlParse.GetFirstItem("config/TakeTemperature/"+highThreshold);
	if (ptrhighThreshold)
	{
		tuint_ptr ptrvalue = m_temperatureXmlParse.GetFirstItem(ptrhighThreshold, "value");
		if (ptrvalue)
		{
			int Value=0;
			int Valuel=0;
			float Value_f = 0.0;
			float Value_fl = 0.0;
			char buf_s [40]={0};
			if (m_temperatureXmlParse.GetValue_i32(ptrvalue, Value))
			{
				Value_f = (float)Value/100;
				sprintf (buf_s, "%.2f", Value_f);
				SetDlgItemText(IDC_EDIT_high_temperature, A2T(buf_s));
			}
			Value_f = 0.0;


			m_temperatureXmlParse.GetAttribute_i32(ptrvalue, "min", Value);
			m_temperatureXmlParse.GetAttribute_i32(ptrvalue, "max", Valuel);
			Value_f = (float)Value/100;
			Value_fl = (float)Valuel/100;
			memset(buf_s, 0, sizeof(buf_s));
			if (index == 0)
			{
				sprintf (buf_s, "(%.2f-%.2f)℃", Value_f, Value_fl);
			}
			else
			{
				sprintf (buf_s, "(%.2f-%.2f)℉", Value_f, Value_fl);
			}

			SetDlgItemText(IDC_STATIC_high, A2T(buf_s));
		}
	}
	tuint_ptr ptrlowThreshold = m_temperatureXmlParse.GetFirstItem("config/TakeTemperature/"+lowThreshold);
	if (ptrlowThreshold)
	{
		
		tuint_ptr ptrvalue = m_temperatureXmlParse.GetFirstItem(ptrlowThreshold, "value");
		if (ptrvalue)
		{
			int Value=0;
			int Valuel=0;
			float Value_f = 0.0;
			float Value_fl = 0.0;
			char buf [40]={0};
			if (m_temperatureXmlParse.GetValue_i32(ptrvalue, Value))
			{
				Value_f = (float)Value/100;
				sprintf (buf, "%.2f", Value_f);
				SetDlgItemText(IDC_EDIT_low_temperature, A2T(buf));
			}
			Value_f = 0.0;
			m_temperatureXmlParse.GetAttribute_i32(ptrvalue, "min", Value);
			m_temperatureXmlParse.GetAttribute_i32(ptrvalue, "max", Valuel);
			Value_f = (float)Value/100;
			Value_fl = (float)Valuel/100;
			memset(buf, 0, sizeof(buf));
			if (index == 0)
			{
				sprintf (buf, "(%.2f-%.2f)℃", Value_f, Value_fl);
			}
			else
			{
				sprintf (buf, "(%.2f-%.2f)℉", Value_f, Value_fl);
			}
			SetDlgItemText(IDC_STATIC_low, A2T(buf));
		}
	}
}


void CFrTemperatureDlg::OnBnClickeGetThermalConfig()
{
	if (! m_DevSupport.supportThermal)
	{
		AfxMessageBox("not supportThermal!");
		return;
	}
	USES_CONVERSION;
	std::string url = "GetMeasureTemperatureConfig";
	char *recvbuf = new char[1024*1024];
	memset(recvbuf, 0, 1024*1024);
	DWORD lReturned = 0;
	BOOL ret = NET_SDK_TransparentConfig(m_userID, "", (char*)url.c_str(), recvbuf, 1024*1024, &lReturned);
	if(ret == 0)
	{
		delete recvbuf;
		AfxMessageBox(" GetMeasureTemperatureConfig faild!");
		return;
	}
	else if(lReturned)
	{	
		m_thermalParse.LoadXML(recvbuf);
		std::string tempValueStr;
		std::string tempUnits;
		std::string highThreshold;
		std::string lowThreshold;
		if (m_thermalParse.GetValueStr("config/TakeTemperature/takeEanble", tempValueStr))
		{
			if (tempValueStr == "true")
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_thermal_enable))->SetCheck(TRUE);
			}
			else
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_thermal_enable))->SetCheck(FALSE);
			}
		}		
	}
	delete recvbuf;
}
void CFrTemperatureDlg::OnBnClickeSetThermalConfig()
{
	if (! m_DevSupport.supportThermal)
	{
		AfxMessageBox("not supportThermal!");
		return;
	}
	std::string tempValueStr;
	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_thermal_enable))->GetCheck())
	{
		tempValueStr = "true";
	}
	else
	{
		tempValueStr = "false";
	}
	m_thermalParse.SetValue("config/TakeTemperature/takeEanble", tempValueStr);

	std::string url = "SetMeasureTemperatureConfig";
	char *recvbuf = new char[1024*1024];
	memset(recvbuf, 0, 1024*1024);
	DWORD lReturned = 0;
	BOOL ret = NET_SDK_TransparentConfig(m_userID, (char *)m_thermalParse.GetDoc().c_str(), (char*)url.c_str(), recvbuf, 1024*1024, &lReturned);
	if(ret == 0)
	{
		delete recvbuf;
		AfxMessageBox(" SetMeasureTemperatureConfig faild!");
		return;
	}
	else if(lReturned)
	{
		CXMLParse xmlParse;
		xmlParse.LoadXML(recvbuf);
		std::string tempValueStr;
		xmlParse.GetAttributeStr("config", "status", tempValueStr);
		if(tempValueStr == "success")
		{
			AfxMessageBox(" SetMeasureTemperatureConfig success!");
		}
		else
		{
			AfxMessageBox(" SetMeasureTemperatureConfig faild!");
		}
		delete recvbuf;
	}

}

void CFrTemperatureDlg::OnBnClickeGetPasslineConfig()
{
	if (! m_DevSupport.supportPassLine)
	{
		AfxMessageBox("not supportPassLine!");
		return;
	}
	USES_CONVERSION;
	std::string url = "GetPassLineCountConfig";
	char *recvbuf = new char[1024*1024];
	memset(recvbuf, 0, 1024*1024);
	DWORD lReturned = 0;
	BOOL ret = NET_SDK_TransparentConfig(m_userID, "", (char*)url.c_str(), recvbuf, 1024*1024, &lReturned);
	if(ret == 0)
	{
		delete recvbuf;
		AfxMessageBox(" GetPassLineCountConfig faild!");
		return;
	}
	else if(lReturned)
	{	
		m_passLineParse.LoadXML(recvbuf);
		std::string tempValueStr;
		if (m_passLineParse.GetValueStr("config/passlinecount/switch", tempValueStr))
		{
			if (tempValueStr == "true")
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_passline_enable))->SetCheck(TRUE);
			}
			else
			{
				reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_passline_enable))->SetCheck(FALSE);
			}
		}		
	}
	delete recvbuf;
}
void CFrTemperatureDlg::OnBnClickeSetPasslineConfig()
{
	if (! m_DevSupport.supportPassLine)
	{
		AfxMessageBox("not supportPassLine!");
		return;
	}
	std::string tempValueStr;
	if(reinterpret_cast<CButton *>(GetDlgItem(IDC_CHECK_passline_enable))->GetCheck())
	{
		tempValueStr = "true";
	}
	else
	{
		tempValueStr = "false";
	}
	m_passLineParse.SetValue("config/passlinecount/switch", tempValueStr);

	std::string url = "SetPassLineCountConfig";
	char *recvbuf = new char[1024];
	memset(recvbuf, 0, 1024);
	DWORD lReturned = 0;
	BOOL ret = NET_SDK_TransparentConfig(m_userID, (char *)m_passLineParse.GetDoc().c_str(), (char*)url.c_str(), recvbuf, 1024, &lReturned);
	if(ret == 0)
	{
		delete recvbuf;
		AfxMessageBox(" SetPassLineCountConfig faild!");
		return;
	}
	else if(lReturned)
	{
		CXMLParse xmlParse;
		xmlParse.LoadXML(recvbuf);
		std::string tempValueStr;
		xmlParse.GetAttributeStr("config", "status", tempValueStr);
		if(tempValueStr == "success")
		{
			AfxMessageBox(" SetPassLineCountConfig success!");
		}
		else
		{
			AfxMessageBox(" SetPassLineCountConfig faild!");
		}
		delete recvbuf;
	}

}

void CFrTemperatureDlg::OnBnClickedButtonpasslineReset()
{
	// TODO: 在此添加控件通知处理程序代码
	if (! m_DevSupport.supportPassLine)
	{
		AfxMessageBox("not supportPassLine!");
		return;
	}
	std::string url = "SetPassLineCountConfig";
	std::string sendXML = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <config version=\"1.7\" xmlns=\"http://www.ipc.com/ver10\">"
		"<passlinecount>"	
		"<forceReset type=\"boolean\">true</forceReset>"
		"</passlinecount>"
		"</config>";

	char *recvbuf = new char[1024];
	memset(recvbuf, 0, 1024);
	DWORD lReturned = 0;
	BOOL ret = NET_SDK_TransparentConfig(m_userID, (char *)sendXML.c_str(), (char*)url.c_str(), recvbuf, 1024, &lReturned);
	if(ret == 0)
	{
		delete recvbuf;
		AfxMessageBox(" SetPassLineCountConfig faild!");
		return;
	}
	else if(lReturned)
	{
		CXMLParse xmlParse;
		xmlParse.LoadXML(recvbuf);
		std::string tempValueStr;
		xmlParse.GetAttributeStr("config", "status", tempValueStr);
		if(tempValueStr == "success")
		{
			AfxMessageBox(" SetPassLineCountConfig success!");
		}
		else
		{
			AfxMessageBox(" SetPassLineCountConfig faild!");
		}
		delete recvbuf;
	}

}


void CFrTemperatureDlg::OnBnClickedButtongetpasslinecount()
{
	// TODO: 在此添加控件通知处理程序代码
	
		if (! m_DevSupport.supportPassLine)
		{
			AfxMessageBox("not supportPassLine!");
			return;
		}
		USES_CONVERSION;
		std::string url = "GetPassLineCountStatistics";
		char *recvbuf = new char[1024*1024];
		memset(recvbuf, 0, 1024*1024);
		DWORD lReturned = 0;
		BOOL ret = NET_SDK_TransparentConfig(m_userID, "", (char*)url.c_str(), recvbuf, 1024*1024, &lReturned);
		if(ret == 0)
		{
			delete recvbuf;
			AfxMessageBox(" GetPassLineCountStatistics faild!");
			return;
		}
		else if(lReturned)
		{	
			CXMLParse	passLineCountParse;
			passLineCountParse.LoadXML(recvbuf);
			int person = 0, bike=0, car=0;
			passLineCountParse.GetValue_i32("config/entranceCount/person", person);
			passLineCountParse.GetValue_i32("config/entranceCount/car", car);
			passLineCountParse.GetValue_i32("config/entranceCount/bike", bike);

			int personExit = 0, bikeExit = 0, carExit = 0;
			passLineCountParse.GetValue_i32("config/exitCount/person", personExit);
			passLineCountParse.GetValue_i32("config/exitCount/car", carExit);
			passLineCountParse.GetValue_i32("config/exitCount/bike", bikeExit);

			CString temp;
			temp.Format(" Person  enter:%d leave:%d\r\n Car  enter:%d leave:%d\r\n Bike:%d leave:%d\r\n ", person, personExit,car, carExit, bike, bikeExit);

			SetDlgItemText(IDC_EDIT_msg_passline, temp);
		}
		delete recvbuf;


		
}
