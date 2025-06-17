// SmartDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SmartDlg.h"
#include "afxdialogex.h"



#include "GridCellCombo.h"
#include "FormatDlg.h"
#include <WinSock2.h>
#include <Wincrypt.h>
#include "strdef.h"
#include <MMSystem.h>
#include "XMLParse.h"
#include "Base64.h"
#include <stdio.h>
#include <direct.h>
#include <iostream>
#include <io.h>
#include "atlimage.h"

// CSmartDlg 对话框

IMPLEMENT_DYNAMIC(CSmartDlg, CDialog)

CSmartDlg::CSmartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSmartDlg::IDD, pParent)
{
	eventType = BY_ALL;
	m_imgSorce = FROM_SNAP;
	isPanorama = 0;
}

CSmartDlg::~CSmartDlg()
{
}

void CSmartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IMAGE, m_imageListCtrl);
	DDX_Control(pDX, IDC_LIST_SEARCH_IMAGE_RESULT, m_searchResultListCtrl);
}
BOOL CSmartDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_searchDate = CTime::GetCurrentTime();
	m_imageList.RemoveAll();
	m_imageListCtrl.DeleteAllItems();
	m_imageListCtrl.SetExtendedStyle(m_imageListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_imageListCtrl.InsertColumn(0, "No");
	m_imageListCtrl.InsertColumn(1, "Id");
	m_imageListCtrl.InsertColumn(2, "channel");
	m_imageListCtrl.InsertColumn(3, "time");
	m_imageListCtrl.SetColumnWidth(0, 50);
	m_imageListCtrl.SetColumnWidth(1, 50);
	m_imageListCtrl.SetColumnWidth(2, 50);
	m_imageListCtrl.SetColumnWidth(3, 200);

	((CButton *)GetDlgItem(IDC_RADIO_BY_ALL))->SetCheck(true);
	((CButton *)GetDlgItem(IDC_RADIO_IMG_FROM_SNAP))->SetCheck(true);
	OnBnClickedRadioByAll();

	((CButton *)GetDlgItem(IDC_RADIO_SNAP))->SetCheck(true);
	OnBnClickedRadioSnap();
	return TRUE;
}

BEGIN_MESSAGE_MAP(CSmartDlg, CDialog)
	ON_BN_CLICKED(IDC_SEARCH_IMG_BY_IMG_V3_BUTTON, &CSmartDlg::OnBnClickedSearchImgByImgV3Button)
	ON_BN_CLICKED(IDC_GET_CH_SNAP_LIST_V3_BUTTON, &CSmartDlg::OnBnClickedGetChSnapListV3Button)
	ON_BN_CLICKED(IDC_RADIO_BY_ALL, &CSmartDlg::OnBnClickedRadioByAll)
	ON_BN_CLICKED(IDC_RADIO_BY_ALLOWLIST, &CSmartDlg::OnBnClickedRadioByAll)
	ON_BN_CLICKED(IDC_RADIO_BY_STRAGER, &CSmartDlg::OnBnClickedRadioByAll)
	ON_BN_CLICKED(IDC_RADIO_BY_BLOCKLIST, &CSmartDlg::OnBnClickedRadioByAll)
	ON_BN_CLICKED(IDC_BTN_VIEW_IMG, &CSmartDlg::OnBnClickedBtnViewImg)
	ON_BN_CLICKED(IDC_RADIO_SNAP, &CSmartDlg::OnBnClickedRadioSnap)
	ON_BN_CLICKED(IDC_RADIO_ORIGINAL, &CSmartDlg::OnBnClickedRadioSnap)
END_MESSAGE_MAP()


// CSmartDlg 消息处理程序
void CSmartDlg::setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid)
{
	m_userID = userid; 
	m_deviceinfo = di;
	/*CString temp;
	CComboBox * combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_CH_COMBO));
	for (int i = 0; i < m_deviceinfo->videoInputNum; i++)
	{
	temp.Format(STR_CHANNEL_D, i + 1);
	combo->AddString(temp);
	}
	combo->SetCurSel(0);*/
}
BOOL CSmartDlg::IsSupportFace()
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
void CSmartDlg::OnBnClickedSearchImgByImgV3Button()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	m_searchResultList.RemoveAll();
	m_searchResultListCtrl.DeleteAllItems();

	char *tempBuf = new char[100*1024];
	memset(tempBuf, 0 , 100*1024);
	NET_SDK_SEARCH_IMAGE_BY_IMAGE_V2 pSEARCH_IMAGE_BY_IMAGE;
	memset(&pSEARCH_IMAGE_BY_IMAGE, 0, sizeof(NET_SDK_SEARCH_IMAGE_BY_IMAGE_V2));
	DWORD lpBytesReturned = 0;
	if(m_imgSorce == FROM_SNAP)
	{
		int index = m_imageListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		if (index < 0)
		{
			return ;
		}
		POSITION pos = NULL;
		pos = m_imageList.GetHeadPosition();
		while (index > 0)
		{
			m_imageList.GetNext(pos);
			index--;
		}
		NET_SDK_FACE_IMG_INFO_CH &item = m_imageList.GetNext(pos);	
		pSEARCH_IMAGE_BY_IMAGE.limitNum = 10;

		pSEARCH_IMAGE_BY_IMAGE.startTime.year = m_searchDate.GetYear();
		pSEARCH_IMAGE_BY_IMAGE.startTime.month = m_searchDate.GetMonth();
		pSEARCH_IMAGE_BY_IMAGE.startTime.mday = m_searchDate.GetDay();


		pSEARCH_IMAGE_BY_IMAGE.endTime.year = m_searchDate.GetYear();
		pSEARCH_IMAGE_BY_IMAGE.endTime.month = m_searchDate.GetMonth();
		pSEARCH_IMAGE_BY_IMAGE.endTime.mday = m_searchDate.GetDay();
		pSEARCH_IMAGE_BY_IMAGE.endTime.hour = 23;
		pSEARCH_IMAGE_BY_IMAGE.endTime.minute = 59;
		pSEARCH_IMAGE_BY_IMAGE.endTime.second = 59;

		pSEARCH_IMAGE_BY_IMAGE.similarity = 75;

		pSEARCH_IMAGE_BY_IMAGE.searchType = SEARCH_IMAGE_BY_IMAGE;
		pSEARCH_IMAGE_BY_IMAGE.imgSourceType = m_imgSorce;

		pSEARCH_IMAGE_BY_IMAGE.sfaceImgsNum = 1;
		pSEARCH_IMAGE_BY_IMAGE.sfaceImgs = new NET_SDK_FACE_IMG_INFO_CH[1];
		memset(pSEARCH_IMAGE_BY_IMAGE.sfaceImgs,0,sizeof(pSEARCH_IMAGE_BY_IMAGE.sfaceImgs));	
		memcpy(pSEARCH_IMAGE_BY_IMAGE.sfaceImgs,&item,sizeof(pSEARCH_IMAGE_BY_IMAGE.sfaceImgs));

	}
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SEARCH_IMAGE_BY_IMG_V2,&pSEARCH_IMAGE_BY_IMAGE, 
		sizeof(NET_SDK_SEARCH_IMAGE_BY_IMAGE_V2), tempBuf,  100*1024, &lpBytesReturned);
	delete[] pSEARCH_IMAGE_BY_IMAGE.sfaceImgs;
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
			TRACE("%d:%d:%d ----- %d\n", pFACE_IMG_INFO_CH[i].frameTime.hour, pFACE_IMG_INFO_CH[i].frameTime.minute, pFACE_IMG_INFO_CH[i].frameTime.second,pFACE_IMG_INFO_CH[i].snapImgId);
			NET_SDK_FACE_IMG_INFO_CH sFACE_IMG_INFO_CH;
			memcpy(&sFACE_IMG_INFO_CH, pFACE_IMG_INFO_CH+i, sizeof(NET_SDK_FACE_IMG_INFO_CH));
			m_searchResultList.AddTail(sFACE_IMG_INFO_CH);

			int n = m_searchResultListCtrl.GetItemCount();
			m_searchResultListCtrl.InsertItem(n, _T(""));
			CString temp;
			temp.Format(_T("%d"), i+1);
			m_searchResultListCtrl.SetItemText(n , 0, temp);

			temp.Format(_T("%d"), sFACE_IMG_INFO_CH.snapImgId);
			m_searchResultListCtrl.SetItemText(n , 1, temp);

			temp.Format(_T("%d"), sFACE_IMG_INFO_CH.chl);
			m_searchResultListCtrl.SetItemText(n, 2, temp);
			temp.Format(_T("%d-%d-%d %d:%d:%d:%d"), sFACE_IMG_INFO_CH.frameTime.year, sFACE_IMG_INFO_CH.frameTime.month, sFACE_IMG_INFO_CH.frameTime.mday,
				sFACE_IMG_INFO_CH.frameTime.hour, sFACE_IMG_INFO_CH.frameTime.minute, sFACE_IMG_INFO_CH.frameTime.second, sFACE_IMG_INFO_CH.frameTime.nMicrosecond);
			m_searchResultListCtrl.SetItemText(n, 3, temp);
		}
	}
	delete[]tempBuf;
}
void CSmartDlg::OnBnClickedGetChSnapListV3Button()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	m_imageList.RemoveAll();
	m_imageListCtrl.DeleteAllItems();


	char *tempBuf = new char[100*1024];
	memset(tempBuf, 0 , 100*1024);
	NET_SDK_SEARCH_IMAGE_BY_IMAGE_V2 sCH_SNAP_FACE_IMG_LIST_SEARCH;
	memset(&sCH_SNAP_FACE_IMG_LIST_SEARCH, 0, sizeof(NET_SDK_SEARCH_IMAGE_BY_IMAGE_V2));


	sCH_SNAP_FACE_IMG_LIST_SEARCH.limitNum = 10;

	sCH_SNAP_FACE_IMG_LIST_SEARCH.startTime.year = m_searchDate.GetYear();
	sCH_SNAP_FACE_IMG_LIST_SEARCH.startTime.month = m_searchDate.GetMonth();
	sCH_SNAP_FACE_IMG_LIST_SEARCH.startTime.mday = m_searchDate.GetDay();


	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.year = m_searchDate.GetYear();
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.month = m_searchDate.GetMonth();
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.mday = m_searchDate.GetDay();
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.hour = 23;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.minute = 59;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.second = 59;

	sCH_SNAP_FACE_IMG_LIST_SEARCH.similarity = 75;
	DWORD lpBytesReturned = 0;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.searchType = SEARCH_IMAGE_BY_EVENT;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.eventType = eventType;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SEARCH_IMAGE_BY_IMG_V2,&sCH_SNAP_FACE_IMG_LIST_SEARCH, 
		sizeof(NET_SDK_SEARCH_IMAGE_BY_IMAGE_V2), tempBuf,  100*1024, &lpBytesReturned);
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
			TRACE("%d:%d:%d ----- %d\n", pFACE_IMG_INFO_CH[i].frameTime.hour, pFACE_IMG_INFO_CH[i].frameTime.minute, pFACE_IMG_INFO_CH[i].frameTime.second,pFACE_IMG_INFO_CH[i].snapImgId);
			NET_SDK_FACE_IMG_INFO_CH sFACE_IMG_INFO_CH;
			memcpy(&sFACE_IMG_INFO_CH, pFACE_IMG_INFO_CH+i, sizeof(NET_SDK_FACE_IMG_INFO_CH));
			m_imageList.AddTail(sFACE_IMG_INFO_CH);

			int n = m_imageListCtrl.GetItemCount();
			m_imageListCtrl.InsertItem(n, _T(""));
			CString temp;
			temp.Format(_T("%d"), i+1);
			m_imageListCtrl.SetItemText(n , 0, temp);

			temp.Format(_T("%d"), sFACE_IMG_INFO_CH.snapImgId);
			m_imageListCtrl.SetItemText(n , 1, temp);

			temp.Format(_T("%d"), sFACE_IMG_INFO_CH.chl);
			m_imageListCtrl.SetItemText(n, 2, temp);
			temp.Format(_T("%d-%d-%d %d:%d:%d:%d"), sFACE_IMG_INFO_CH.frameTime.year, sFACE_IMG_INFO_CH.frameTime.month, sFACE_IMG_INFO_CH.frameTime.mday,
				sFACE_IMG_INFO_CH.frameTime.hour, sFACE_IMG_INFO_CH.frameTime.minute, sFACE_IMG_INFO_CH.frameTime.second, sFACE_IMG_INFO_CH.frameTime.nMicrosecond);
			m_imageListCtrl.SetItemText(n, 3, temp);
		}
	}
	delete[]tempBuf;
}


void CSmartDlg::OnBnClickedRadioByAll()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = IDC_RADIO_BY_ALL; i <= IDC_RADIO_BY_BLOCKLIST; i++)
	{
		if (reinterpret_cast<CButton*>(GetDlgItem(i))->GetCheck())
		{
			switch(i)
			{
			case IDC_RADIO_BY_ALL:
				eventType = BY_ALL;
				break;
			case IDC_RADIO_BY_ALLOWLIST:
				eventType = BY_WHITELIST;
				break;
			case IDC_RADIO_BY_STRAGER:
				eventType = BY_STRANGERLIST;
				break;
			case IDC_RADIO_BY_BLOCKLIST:
				eventType = BY_BLACKLIST;
				break;
			}

		}
	}

}


void CSmartDlg::OnBnClickedBtnViewImg()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();

	int index = m_imageListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (index < 0)
	{
		return ;
	}
	POSITION pos = NULL;
	pos = m_imageList.GetHeadPosition();
	while (index > 0)
	{
		m_imageList.GetNext(pos);
		index--;
	}
	NET_SDK_FACE_IMG_INFO_CH &item = m_imageList.GetNext(pos);	
	/*NET_SDK_FACE_IMG_INFO_CH item;
	item.imgId = 2616;
	item.chl = 6;
	item.frameTime.year = 2020;
	item.frameTime.month = 8;
	item.frameTime.mday = 18;
	item.frameTime.hour = 5;
	item.frameTime.minute = 32;
	item.frameTime.second = 54;
	item.frameTime.nMicrosecond = 1739510;*/

	item.isPanorama = isPanorama;
	char *tempBuf = new char[1000*1024];
	memset(tempBuf, 0, 1000*1024);
	DWORD lpBytesReturned = 0;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SEARCH_CH_SNAP_FACE_IMG, &item, sizeof(NET_SDK_FACE_IMG_INFO_CH), tempBuf,  1000*1024, &lpBytesReturned);
	if(ret)
	{
		NET_SDK_FACE_INFO_IMG_DATA *pFACE_INFO_IMG_DATA = (NET_SDK_FACE_INFO_IMG_DATA*)tempBuf;
		TRACE("imgLen =%d ,  lpBytesReturned=%d \n", pFACE_INFO_IMG_DATA->imgLen,  lpBytesReturned);

		char filename[20] = {0};
		sprintf(filename, "./chSnap_%d_%d.jpg", item.chl, item.snapImgId);
		FILE *file = fopen(filename, "wb");
		if (file)
		{
			fwrite(pFACE_INFO_IMG_DATA->imgData, 1, pFACE_INFO_IMG_DATA->imgLen, file);
			fclose(file);
			CString temp;
			temp.Format(_T("save img %s"), filename);
			AfxMessageBox(temp);
		}
		CImage image;
		CWnd* pWnd;
		pWnd = GetDlgItem(IDC_PICTURE_SNAP);
		CDC* pDC = pWnd->GetDC();
		HDC hDC = pDC->m_hDC;
		CRect rect_frame;
		pWnd->GetClientRect(&rect_frame);
		image.Load(filename);
		::SetStretchBltMode(hDC,HALFTONE);
		::SetBrushOrgEx(hDC,0,0,NULL);
		image.Draw(hDC,rect_frame);
		ReleaseDC(pDC);//释放picture控件的DC
		image.Destroy();

		//SetDlgItemText(IDC_STATIC_PICTURE_PATH, filename);
	}
	delete[]tempBuf;
}


void CSmartDlg::OnBnClickedRadioSnap()
{
	// TODO: 在此添加控件通知处理程序代码
	for (int i = IDC_RADIO_SNAP; i <= IDC_RADIO_ORIGINAL; i++)
	{
		if (reinterpret_cast<CButton*>(GetDlgItem(i))->GetCheck())
		{
			switch(i)
			{
			case IDC_RADIO_SNAP:
				isPanorama = 0;
				break;
			case IDC_RADIO_ORIGINAL:
				isPanorama = 1;
				break;

			}

		}
	}
}
