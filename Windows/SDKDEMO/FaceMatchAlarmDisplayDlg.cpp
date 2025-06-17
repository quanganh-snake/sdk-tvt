// FaceMatchAlarmDisplayDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "FaceMatchAlarmDisplayDlg.h"
#include "afxdialogex.h"
#include "atlimage.h"
#include "LiveDlg.h"
#include "strdef.h"

// CFaceMatchAlarmDisplayDlg 对话框

IMPLEMENT_DYNAMIC(CFaceMatchAlarmDisplayDlg, CDialog)

CFaceMatchAlarmDisplayDlg::CFaceMatchAlarmDisplayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFaceMatchAlarmDisplayDlg::IDD, pParent)
	, m_SubscribAVD(0)
	, m_SubscribVFD(0)
	, m_SubscribPEA(0)
	, m_SubscribOSC(0)
	, m_SubscribFaceMatch(0)
	, m_SubscribVehicle(0)
	, m_SubscribAOIENTRY(0)
	, m_SubscribAOILEAVE(0)
	, m_SubscribPassLine(0)
	, m_SubscribNVRPassLine(0)
	, m_SubscribTraffic(0)
{

}

CFaceMatchAlarmDisplayDlg::~CFaceMatchAlarmDisplayDlg()
{
}

void CFaceMatchAlarmDisplayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}
BOOL CFaceMatchAlarmDisplayDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CFaceMatchAlarmDisplayDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_AVD, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribAvd)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_VFD, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribVfd)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_VFD_MATCH, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribVfdMatch)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_OSC, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribOsc)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_PEA, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribPea)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_VIHICLE, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribVihicle)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_AOIENTRY, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribAoientry)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_AOILEAVE, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribAoileave)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_PASSLINE, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribPassline)
	ON_BN_CLICKED(IDC_BUTTON_SUBSCRIBE_NVRPASSLINE, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSubscribeNvrpassline)
	ON_BN_CLICKED(IDC_BUTTON_UNSUBSCRIBE_NVRPASSLINE, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonUnsubscribeNvrpassline)
	ON_BN_CLICKED(IDC_BUTTON_SMART_SBSCRIB_TRAFFIC, &CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribTraffic)
END_MESSAGE_MAP()

void CFaceMatchAlarmDisplayDlg::setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid)
{
	m_userID = userid; 
	m_deviceinfo = di;

	CString temp;
	CComboBox * combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_NVREVENTCOMBO));

	for (int i = 0; i < m_deviceinfo->videoInputNum; i++)
	{
		temp.Format(STR_CHANNEL_D, i + 1);
		combo->AddString(temp);
	}
	combo->SetCurSel(0);
}
BOOL CFaceMatchAlarmDisplayDlg::IsSupportFace()
{

	/*DWORD SUPPORT = 0; 
	DWORD  lpBytesReturned = 0;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_MATCH_SUPPORT, NULL, 0, &SUPPORT,  sizeof(DWORD), &lpBytesReturned);
	if(SUPPORT == 0)
	{
	m_support = FALSE;
	return FALSE; 
	}

	m_support = TRUE;*/
	return TRUE;  // return TRUE unless you set the focus to a control

}
bool UTF8ToGBExEx(char* pDest, int nDestLen, const char* pSrc, int nSrcLen)
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

void CFaceMatchAlarmDisplayDlg::UpdateMsg(LONG ID, LONG lCommand, void *data, int length)
{
	int dwBufLen = length;
	switch(lCommand)
	{
		//IVE_VFD_RESULT_HEAD_T+[IVE_VFD_RESULT_DATA_INFO_T+源数据]+[IVE_VFD_RESULT_FACE_DATA_INFO_T+人脸数据+人脸特征值]+...+[IVE_VFD_RESULT_FACE_DATA_INFO_T+人脸数据+人脸特征值]
	case  NET_SDK_SMART_EVENT_TYPE_VFD:
		{
			
			TRACE("NET_DVR_SMART_VFD dwBufLen=%d\n", dwBufLen);
			if (dwBufLen < sizeof(NET_DVR_IVE_VFD_RESULT_HEAD_T))
			{
				break;
			}
			char* tmp = (char*)data;
			// 解析头
			NET_DVR_IVE_VFD_RESULT_HEAD_T* head = (NET_DVR_IVE_VFD_RESULT_HEAD_T*)tmp;
			tmp += sizeof(NET_DVR_IVE_VFD_RESULT_HEAD_T);
			// 解析原始数据
			NET_DVR_IVE_VFD_RESULT_DATA_INFO_T* src = (NET_DVR_IVE_VFD_RESULT_DATA_INFO_T*)tmp;
			tmp += sizeof(NET_DVR_IVE_VFD_RESULT_DATA_INFO_T);
			if(src->dataLen > 0)
			{
				TRACE("src->dataLen=%d, src->width=%d, src->height=%d \n", src->dataLen, src->width, src->height);
				//源数据
				char* img = new char[src->dataLen];
				memcpy(img, tmp, src->dataLen);

				FILE *backfp = fopen("./SourceImg.jpg", "wb");
				if(backfp)
				{
					int fret = fwrite(img, src->dataLen, 1, backfp );
					fclose(backfp);
				}
				ShowPicture(IDC_SOURCE_PICUTRE,"./SourceImg.jpg");
				delete img;
				tmp += src->dataLen;		       
			}

			for(int i=0; i<head->faceCnt; i++)
			{
				NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T* face = (NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T*)tmp;
				tmp += sizeof(NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T);  

				// face Image, 如果有
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
					ShowPicture(IDC_SNAP_PICTURE,"./FaceImg.jpg");
					delete faceImg;
					tmp+= face->stFaceImgData.dataLen;		
				}

				// feature, 如果有
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
			int restLen = length;
			TRACE("NET_SDK_SMART_EVENT_TYPE_FACE_MATCH dwBufLen=%d\n", length);
			if(length <= sizeof(NET_SDK_IVE_FACE_MATCH_T))
			{
				TRACE("NET_SDK_SMART_EVENT_TYPE_FACE_MATCH len error dwBufLen=%d\n", length);
				return;
			}
			char* pBuf = (char*)data;

			NET_SDK_IVE_FACE_MATCH_T* sIVE_AVD = (NET_SDK_IVE_FACE_MATCH_T*)pBuf;
			restLen -= sizeof(NET_SDK_IVE_FACE_MATCH_T);

			CString temp;
			temp.Format("%d-%d-%d %d:%d:%d",sIVE_AVD->frameTime.year,sIVE_AVD->frameTime.month,sIVE_AVD->frameTime.mday,sIVE_AVD->frameTime.hour,sIVE_AVD->frameTime.minute,sIVE_AVD->frameTime.second);
			SetDlgItemText(IDC_STATIC_TIME, temp);

			temp.Format("%d",sIVE_AVD->dwSimilar);
			SetDlgItemText(IDC_STATIC_SIMILARITY, temp);


			if(restLen < sIVE_AVD->imgLen)
			{
				TRACE("NET_DVR_SMART_FACE_MATCH len error dwBufLen=%d\n", length);
				return;
			}
			restLen -= sIVE_AVD->imgLen;
			TRACE("dwRealFaceID=%d, dwGrpID= %d dwLibFaceID = %d Channel=%d imgLen= %d dwSimilar=%d byName = %s \n",sIVE_AVD->dwRealFaceID, sIVE_AVD->dwGrpID, sIVE_AVD->dwLibFaceID,sIVE_AVD->Channel,sIVE_AVD->imgLen,
				sIVE_AVD->dwSimilar, sIVE_AVD->byName);
			FILE *fp = fopen("./ZhuaPai.jpg", "wb");
			if(fp)
			{
				
				int fret = fwrite(pBuf+sizeof(NET_SDK_IVE_FACE_MATCH_T) , sIVE_AVD->imgLen, 1, fp );
				fclose(fp);
			}

			ShowPicture(IDC_SNAP_PICTURE,"./ZhuaPai.jpg");

			pBuf += sizeof(NET_SDK_IVE_FACE_MATCH_T)+sIVE_AVD->imgLen;

			if(restLen < sizeof(NET_SDK_AVPSTORE_FACE_ABSTRACT_INFO))
			{
				TRACE("NET_DVR_SMART_FACE_MATCH len error dwBufLen=%d\n", length);
				return;
			}
			restLen -= sizeof(NET_SDK_AVPSTORE_FACE_ABSTRACT_INFO);

			NET_SDK_AVPSTORE_FACE_ABSTRACT_INFO* pFaceInfo = (NET_SDK_AVPSTORE_FACE_ABSTRACT_INFO*)pBuf;
			pBuf += sizeof(NET_SDK_AVPSTORE_FACE_ABSTRACT_INFO);




			char szDest[128] = {0};
			UTF8ToGBExEx(szDest, sizeof(szDest), pFaceInfo->szName, DD_MAX_CAMERA_NAME_BUF_LEN);


			temp.Format("%s",szDest);
			SetDlgItemText(IDC_STATIC_NAME, temp);

			temp.Format("%s",pFaceInfo->szCredential);
			SetDlgItemText(IDC_STATIC_CREDENTIAL, temp);

			if(restLen < sizeof(NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T))
			{
				TRACE("NET_DVR_SMART_FACE_MATCH len error dwBufLen=%d\n", length);
				return;
			}
			restLen -= sizeof(NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T);

			NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T* pFaceDataInfo = (NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T*)pBuf;
			pBuf += sizeof(NET_DVR_IVE_VFD_RESULT_FACE_DATA_INFO_T);


			if(restLen < pFaceDataInfo->stFaceImgData.dataLen)
			{
				TRACE("NET_DVR_SMART_FACE_MATCH len error dwBufLen=%d\n", length);
				return;
			}
			restLen -= pFaceDataInfo->stFaceImgData.dataLen;

			fp = fopen("./Target.jpg", "wb");
			if(fp)
			{

				int fret = fwrite(pBuf , pFaceDataInfo->stFaceImgData.dataLen, 1, fp );
				fclose(fp);
			}

			ShowPicture(IDC_TARGET_PICUTRE,"./Target.jpg");

			pBuf += pFaceDataInfo->stFaceImgData.dataLen;

			if(restLen < sizeof(NET_SDK_TLV_BUFFER_DESC))
			{
				TRACE("NET_DVR_SMART_FACE_MATCH len error dwBufLen=%d\n", length);
				return;
			}
			restLen -= sizeof(NET_SDK_TLV_BUFFER_DESC);

			NET_SDK_TLV_BUFFER_DESC* pBufferDesc = (NET_SDK_TLV_BUFFER_DESC*)pBuf;
			pBuf += sizeof(NET_SDK_TLV_BUFFER_DESC);

			if(restLen < pBufferDesc->dwSize)
			{
				TRACE("NET_DVR_SMART_FACE_MATCH len error dwBufLen=%d\n", length);
				return;
			}
			restLen -= pBufferDesc->dwSize;
			fp = fopen("./BeiJing.jpg", "wb");
			if(fp)
			{

				int fret = fwrite(pBuf , pBufferDesc->dwSize, 1, fp );
				fclose(fp);
			}

			ShowPicture(IDC_SOURCE_PICUTRE,"./BeiJing.jpg");
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
			int restLen = length;
			char* pBuf = (char*)data;
			NET_SDK_IVE_BASE_INFO *baseInfo = (NET_SDK_IVE_BASE_INFO*)pBuf;
			pBuf += sizeof(NET_SDK_IVE_BASE_INFO);
			restLen -= sizeof(NET_SDK_IVE_BASE_INFO);
			if(restLen <= 0)
				break;


			CString temp;
			temp.Format("%lld,ID=%d,wear mask:%d,temperature:%d",baseInfo->i64SnapTime,baseInfo->iPersonId,baseInfo->wearmask,baseInfo->temperature);
			SetDlgItemText(IDC_STATIC_TIME, temp);

			NET_SDK_IVE_PICTURE_INFO *pictureInfo = (NET_SDK_IVE_PICTURE_INFO*)pBuf;
			pBuf += sizeof(NET_SDK_IVE_PICTURE_INFO);
			restLen -= sizeof(NET_SDK_IVE_PICTURE_INFO);
			
			if(pictureInfo->iPicSize <= 0)
				break;

			if(restLen - pictureInfo->iPicSize < 0)
				break;
			FILE *backfp = fopen("./snap.jpg", "wb");
			if(backfp)
			{
				int fret = fwrite(pBuf, pictureInfo->iPicSize, 1, backfp );
				fclose(backfp);
			}
			ShowPicture(IDC_SNAP_PICTURE,"./snap.jpg");

			pBuf += pictureInfo->iPicSize;
			restLen -= pictureInfo->iPicSize;

			if(restLen <= 0)
				break;
			NET_SDK_IVE_PICTURE_INFO *pictureInfo2 = (NET_SDK_IVE_PICTURE_INFO*)pBuf;
			pBuf += sizeof(NET_SDK_IVE_PICTURE_INFO);
			restLen -= sizeof(NET_SDK_IVE_PICTURE_INFO);

			if(pictureInfo2->iPicSize <= 0)
				break;
			if(restLen - pictureInfo2->iPicSize < 0)
				break;

			backfp = fopen("./Target.jpg", "wb");
			if(backfp)
			{

				int fret = fwrite(pBuf , pictureInfo2->iPicSize, 1, backfp );
				fclose(backfp);
			}

			ShowPicture(IDC_TARGET_PICUTRE,"./Target.jpg");
		}
		break;
	case NET_SDK_SMART_EVENT_TYPE_VEHICLE:
		{
			int restLen = length;
			char* pBuf = (char*)data;
			//NET_SDK_IVE_VEHICE_HEAD_INFO+[NET_SDK_IVE_VEHICE_ITEM_INFO+源数据]+[NET_SDK_IVE_VEHICE_ITEM_INFO+车牌数据]+...+[NET_SDK_IVE_VEHICE_ITEM_INFO+车牌数据] 
			NET_SDK_IVE_VEHICE_HEAD_INFO *vehiceHeadInfo = (NET_SDK_IVE_VEHICE_HEAD_INFO*)pBuf;
			TRACE("item_cnt=%d\n",vehiceHeadInfo->item_cnt);
			unsigned int cntVheCle = vehiceHeadInfo->item_cnt;
			unsigned int offset = sizeof(NET_SDK_IVE_VEHICE_HEAD_INFO);
			
			if (cntVheCle > 0)
			{
				for(int i = 0;i< cntVheCle;i++)
				{
					NET_SDK_IVE_VEHICE_ITEM_INFO vehicleinfo;
					memset(&vehicleinfo,0,sizeof(NET_SDK_IVE_VEHICE_ITEM_INFO));
					memcpy(&vehicleinfo, pBuf + offset,sizeof(NET_SDK_IVE_VEHICE_ITEM_INFO));
					offset += sizeof(NET_SDK_IVE_VEHICE_ITEM_INFO);//指针偏移

					if (vehicleinfo.jpeg_len > 0)
					{
						if(i == 0)//第1张是源图片
						{
							FILE *backfp = fopen("./vehicleSorce.jpg", "wb");
							if(backfp)
							{
								int fret = fwrite(pBuf+offset, vehicleinfo.jpeg_len, 1, backfp );
								fclose(backfp);
							}
							ShowPicture(IDC_SOURCE_PICUTRE,"./vehicleSorce.jpg");
						}
						else//其他是车牌照片
						{
							CString path;
							path.Format(_T("./vehicle%d.jpg"),i);

							FILE *backfp = fopen(path.GetString(), "wb");
							//FILE *backfp = fopen("./vehicle.jpg", "wb");
							if(backfp)
							{
								int fret = fwrite(pBuf+offset, vehicleinfo.jpeg_len, 1, backfp );
								fclose(backfp);
							}
							ShowPicture(IDC_SNAP_PICTURE,path);

							CString temp;
							temp.Format("palte number is:%s,charactor count:%d",U2G(vehicleinfo.plate),vehicleinfo.plateCharCount);
							SetDlgItemText(IDC_STATIC_TIME, temp);
						}

					}
					offset += (vehicleinfo.jpeg_vir_len - vehicleinfo.jpeg_len);
					//那么这个时候不用加偏移
					if (vehicleinfo.jpeg_vir_len <= vehicleinfo.jpeg_len)
					{
						offset += vehicleinfo.jpeg_len;
					}
					else
					{
						offset += vehicleinfo.jpeg_len;
					}											
				}
			}
			break;
		}
	}
}
char* CFaceMatchAlarmDisplayDlg::U2G(const char* utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len+1];
	memset(wstr, 0, len+1);
	MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
	len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len+1];
	memset(str, 0, len+1);
	WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
	if(wstr) delete[] wstr;
	return str;
}
void CFaceMatchAlarmDisplayDlg::ShowPicture(int id,LPCTSTR path)
{
	CImage image;
	CWnd* pWnd;
	pWnd = GetDlgItem(id);
	CDC* pDC = pWnd->GetDC();
	HDC hDC = pDC->m_hDC;
	CRect rect_frame;
	pWnd->GetClientRect(&rect_frame);
	image.Load(path);
	::SetStretchBltMode(hDC,HALFTONE);
	::SetBrushOrgEx(hDC,0,0,NULL);
	image.Draw(hDC,rect_frame);
	ReleaseDC(pDC);//释放picture控件的DC
	image.Destroy();
}


// CFaceMatchAlarmDisplayDlg 消息处理程序


void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribAvd()
{
	// TODO: 在此添加控件通知处理程序代码
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
}


void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribVfd()
{
	// TODO: 在此添加控件通知处理程序代码
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


void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribVfdMatch()
{
	// TODO: 在此添加控件通知处理程序代码
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


void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribOsc()
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


void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribPea()
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


void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribVihicle()
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


void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribAoientry()
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


void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribAoileave()
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


void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribPassline()
{
	// TODO: 在此添加控件通知处理程序代码
	BOOL bret;
	if(!m_SubscribNVRPassLine)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		bret = NET_SDK_SmartSubscrib(m_userID, NET_IPC_SMART_PASSLINE, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			m_SubscribNVRPassLine = TRUE;
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
			m_SubscribNVRPassLine = FALSE;
		}
	}
}


void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSubscribeNvrpassline()
{
	CComboBox * combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_NVREVENTCOMBO));
	int iCurChl = combo->GetCurSel();
	BOOL bret;

	NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
	bret = NET_SDK_SmartSubscrib(m_userID, NET_IPC_SMART_PASSLINE, iCurChl, &sSmartSubscrib);
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


void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonUnsubscribeNvrpassline()
{
	CComboBox * combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_COMBO_NVREVENTCOMBO));
	int iCurChl = combo->GetCurSel();
	BOOL bret;

	int dwResult = 0;
	bret = NET_SDK_UnSmartSubscrib(m_userID, NET_IPC_SMART_PASSLINE, iCurChl, m_serverAddressPassLine, &dwResult);
	if(!bret)
	{

		TRACE(" NET_SDK_UnSmartSubscrib  error\n");
	}
	else
	{
		m_SubscribPassLine = FALSE;
	}
}

void CFaceMatchAlarmDisplayDlg::OnBnClickedButtonSmartSbscribTraffic()
{
	BOOL bret;
	if(!m_SubscribTraffic)
	{
		NET_DVR_SUBSCRIBE_REPLY sSmartSubscrib;
		bret = NET_SDK_SmartSubscrib(m_userID, NET_IPC_SMART_TRAFFIC, 0, &sSmartSubscrib);
		if(!bret)
		{
			TRACE(" NET_SDK_SmartSubscrib  error\n");
		}
		else
		{
			m_SubscribTraffic = TRUE;
			memcpy(m_serverAddressPassLine,sSmartSubscrib.serverAddress, sizeof(sSmartSubscrib.serverAddress) );
		}
	}
	else
	{
		int dwResult = 0;
		bret = NET_SDK_UnSmartSubscrib(m_userID, NET_IPC_SMART_TRAFFIC, 0, m_serverAddressPassLine, &dwResult);
		if(!bret)
		{

			TRACE(" NET_SDK_UnSmartSubscrib  error\n");
		}
		else
		{
			m_SubscribTraffic = FALSE;
		}
	}
}
