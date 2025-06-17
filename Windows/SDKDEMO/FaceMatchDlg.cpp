// ToolDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SDKDEMO.h"
#include "FaceMatchDlg.h"
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
// CToolDlg 对话框

////////////////////////////////////////////////////////////////////////////
//typedef char			tchar, tint8;/////8bit signed
//typedef unsigned char	tbyte, tuchar, tuint8; ///////8bit unsigned
//typedef short			tint16; ////16bit  signed
//typedef unsigned short  tuint16; ////16bit  unsigned
//typedef int				tint32; /////32 bit  signed
//typedef unsigned int	tuint32; /////32 bit unsigned
//typedef __int64			tint64; //////64bit  signed
//typedef unsigned __int64   tuint64; ////64bit  unsigned
//
//
///************************************************************************************
// *基本类型的扩展类型
//************************************************************************************/
//#if defined(OS_IS_64BIT)
//    typedef tuint64     tuint_ptr;
//    typedef tint64      tint_ptr;
//#else
//    typedef tuint32     tuint_ptr;
//    typedef tint32      tint_ptr;
//#endif /////

//float JepgSize(int x)
//{
//    return x!=2?1:2.56;
//}

void GBtoUTF8(const char *ansi, long srcLen, char *utf8, long destLen)
{
	wchar_t *pUnicode = new wchar_t[srcLen];

	//ANSI多字节转换为Unicode
	memset(pUnicode, 0, sizeof(wchar_t)*srcLen);
	int retNum = MultiByteToWideChar(CP_ACP, 0, ansi, srcLen, pUnicode, srcLen);

	//Unicode转换为UTF8
	memset(utf8, 0, destLen);
	//预留最后一个字节存放字符串结束符
	WideCharToMultiByte(CP_UTF8, 0, pUnicode, retNum, (char *)utf8, destLen-1, NULL, NULL);
	delete [] pUnicode;
}

//大于0表示获取成功
int GetJepgSize1(char * buffer,long length, int *weidth,  int *height)
{
    if(!buffer)
        return -1;
    char * temp=buffer+length,*temp1=buffer;
    unsigned char ff,type=0xff;
    int m_ret=-1,m_pos=0,m_hpx=0,m_wpx=0,uits=0;
    if((unsigned char)*buffer++!=0xff||(unsigned char)*buffer++!=0xd8)
    {
        printf("此非jepg图片\n");
        return -1;
    }
    while(temp>buffer&&type!=0xDA)
    {
        do
        {
            ff=*buffer++;
        } while (ff!=0xff);
        do
        {
            type=*buffer++;
        } while (type==0xff);
        switch(type)
        {
        case 0x00:
        case 0x01:
        case 0xD0:
        case 0xD1:
        case 0xD2:
        case 0xD3:
        case 0xD4:
        case 0xD5:
        case 0xD6:
        case 0xD7:
            break;
        case 0xC0://SOF0段
            temp1=buffer;
            m_pos=((*buffer++)&0xff)<<8;
            m_pos+=(*buffer++)&0xff;
            buffer++; //舍弃精度值
            *height=((*buffer++)&0xff)<<8;
            *height+=(*buffer++)&0xff;
            *weidth=((*buffer++)&0xff)<<8;
            *weidth+=(*buffer)&0xff;
            return 1;
            //break;
        case 0xE0: //APP0段
            temp1=buffer;
            m_pos=((*buffer++)&0xff)<<8;
            m_pos+=(*buffer++)&0xff;
            buffer=buffer+7;        //丢弃APP0标记(5bytes)以及主版本号(1bytes)及次版本号(1bytes)
            uits=(*buffer++)&0xff;  //0: 无单位,units=1:点数/英寸,units=2:点数/厘米
            m_wpx=((*buffer++)&0xff)<<8;
            m_wpx+=(*buffer++)&0xff;
            m_hpx=((*buffer++)&0xff)<<8;
            m_hpx+=(*buffer++)&0xff;
            /*testhpx=m_hpx*JepgSize(uits);
            testwpx=m_wpx*JepgSize(uits);  */
            m_ret++;
            break;
        default:
            temp1=buffer;
            m_pos=((*buffer++)&0xff)<<8;
            m_pos+=(*buffer++)&0xff;
            break;
        }
        buffer=temp1+m_pos;
    }
    return 0;
}

IMPLEMENT_DYNAMIC(CFaceMatchlDlg, CDialog)

CFaceMatchlDlg::CFaceMatchlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFaceMatchlDlg::IDD, pParent)
	
{
	m_chnn = 0;
	m_faceAlarmTrigger = NULL;
	m_faceAlarmTrigger = new char[200*1024];
	m_imgSorce = FROM_ALBUM;
}

CFaceMatchlDlg::~CFaceMatchlDlg()
{
	if (m_faceAlarmTrigger)
	{
		delete []m_faceAlarmTrigger;
	}
	
}

void CFaceMatchlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_GROUPS, m_faceGroupListCtrl);
	DDX_Control(pDX, IDC_LIST_FACE, m_faceInfoListCtrl);
	DDX_Control(pDX, IDC_LIST_CH_SNAP, m_chSnapFaceListCtrl);

	DDX_Text(pDX, IDC_EDIT_GROUP, m_editGroupName);
	DDX_Text(pDX, IDC_EDIT_FACE_NAME, m_editFaceName);
	DDX_Text(pDX, IDC_EDIT_FACE_birthday, m_editFaceBirthday);
	DDX_Text(pDX, IDC_EDIT_FACE_nativePlace, m_editFaceNativePlace);
	DDX_Text(pDX, IDC_EDIT_FACE_certificateNum, m_editFaceCertificateNum);
	DDX_Text(pDX, IDC_EDIT_FACE_mobile, m_editFaceMobile);
	DDX_Text(pDX, IDC_EDIT_FACE_number, m_editFaceNumber);
	DDX_CBIndex(pDX, IDC_CH_COMBO, m_chnn);
	DDV_MaxChars(pDX, m_editGroupName, 64);


	DDX_Control(pDX, IDC_PROGRESS_EXPORT, m_ProgressExport);
	DDX_Control(pDX, IDC_LIST_SEARCH_IMAGE_RESULT, m_searchResultListCtrl);
}


BEGIN_MESSAGE_MAP(CFaceMatchlDlg, CDialog)
	//ON_BN_CLICKED(IDOK, &CFaceMatchlDlg::OnBnClickedOk)
	//ON_BN_CLICKED(IDCANCEL, &CFaceMatchlDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_GET_GROUPS_BUTTON, &CFaceMatchlDlg::OnBnClickedGetGroups)
	ON_BN_CLICKED(IDC_ADD_GROUPS_BUTTON, &CFaceMatchlDlg::OnBnClickedAddGroups)
	ON_BN_CLICKED(IDC_EDIT_GROUPS_BUTTON, &CFaceMatchlDlg::OnBnClickedEditGroups)
	ON_BN_CLICKED(IDC_DEL_GROUPS_BUTTON, &CFaceMatchlDlg::OnBnClickedDelGroups)
	ON_BN_CLICKED(IDC_ADD_FACE_BUTTON, &CFaceMatchlDlg::OnBnClickedAddFace)
	ON_BN_CLICKED(IDC_EDIT_FACE_BUTTON, &CFaceMatchlDlg::OnBnClickedEditFace)
	ON_BN_CLICKED(IDC_DEL_FACE_BUTTON, &CFaceMatchlDlg::OnBnClickedDelFace)
	ON_BN_CLICKED(IDC_GET_CH_SNAP_LIST_BUTTON, &CFaceMatchlDlg::OnBnClickeGetCHSnapFaceList)
	ON_BN_CLICKED(IDC_SEARCH_IMG_BY_IMG_BUTTON, &CFaceMatchlDlg::OnBnClickeSearchImgByImg)
	ON_BN_CLICKED(IDC_GET_ALARM_TRIGGER_BUTTON, &CFaceMatchlDlg::OnBnClickeGetAlarmTrigger)
	ON_BN_CLICKED(IDC_SET_ALARM_TRIGGER_BUTTON, &CFaceMatchlDlg::OnBnClickeSetAlarmTrigger)

	ON_NOTIFY(NM_DBLCLK, IDC_LIST_GROUPS, &CFaceMatchlDlg::OnNMDblclkListGroups)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FACE, &CFaceMatchlDlg::OnNMDblclkListFace)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_CH_SNAP, &CFaceMatchlDlg::OnNMDblclkListChSnap)
	
	ON_BN_CLICKED(IDC_COPY_FACE_BUTTON, &CFaceMatchlDlg::OnBnClickedCopyFaceButton)
	ON_BN_CLICKED(IDC_BTN_EXPORT_BROWSE, &CFaceMatchlDlg::OnBnClickedBtnExportBrowse)
	ON_BN_CLICKED(IDC_BTN_EXPORT, &CFaceMatchlDlg::OnBnClickedBtnExport)
	ON_BN_CLICKED(IDC_BTN_IMPORT, &CFaceMatchlDlg::OnBnClickedBtnImport)
	ON_STN_CLICKED(IDC_PICTURE, &CFaceMatchlDlg::OnStnClickedPicture)
	ON_BN_CLICKED(IDC_SEARCH_IMG_BY_IMG_V2_BUTTON, &CFaceMatchlDlg::OnBnClickedSearchImgByImgV2Button)
	ON_BN_CLICKED(IDC_GET_CH_SNAP_LIST_V2_BUTTON, &CFaceMatchlDlg::OnBnClickedGetChSnapListV2Button)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_SEARCH_IMAGE_RESULT, &CFaceMatchlDlg::OnNMDblclkListSearchImageResult)
	ON_NOTIFY(NM_CLICK, IDC_LIST_CH_SNAP, &CFaceMatchlDlg::OnNMClickListChSnap)
	ON_NOTIFY(NM_CLICK, IDC_LIST_FACE, &CFaceMatchlDlg::OnNMClickListFace)
END_MESSAGE_MAP()


// CToolDlg 消息处理程序

void CFaceMatchlDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnOK();
}

void CFaceMatchlDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}




BOOL CFaceMatchlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	m_searchDate = CTime::GetCurrentTime();

	m_faceGroupList.RemoveAll();
	m_faceGroupListCtrl.DeleteAllItems();
	
	m_faceGroupListCtrl.SetExtendedStyle(m_faceGroupListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES
		| LVS_EX_FULLROWSELECT);
	m_faceGroupListCtrl.InsertColumn(0, "No");
	m_faceGroupListCtrl.InsertColumn(1, "Id");
	m_faceGroupListCtrl.InsertColumn(2, "Guid");
	m_faceGroupListCtrl.InsertColumn(3, "Name");
	m_faceGroupListCtrl.InsertColumn(4, "");
	m_faceGroupListCtrl.SetColumnWidth(0, 50);
	m_faceGroupListCtrl.SetColumnWidth(1, 50);
	m_faceGroupListCtrl.SetColumnWidth(2, 220);
	m_faceGroupListCtrl.SetColumnWidth(3, 100);
	m_faceGroupListCtrl.SetColumnWidth(4, 80);

	m_ipcFaceInfoList.RemoveAll();
	m_faceInfoList.RemoveAll();
	m_faceInfoListCtrl.DeleteAllItems();
	m_faceInfoListCtrl.SetExtendedStyle(m_faceInfoListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES
	| LVS_EX_FULLROWSELECT);

	m_faceInfoListCtrl.InsertColumn(0, "No");
	m_faceInfoListCtrl.InsertColumn(1, "Id");
	m_faceInfoListCtrl.InsertColumn(2, "Name");
	m_faceInfoListCtrl.InsertColumn(3, "");
	m_faceInfoListCtrl.SetColumnWidth(0, 50);
	m_faceInfoListCtrl.SetColumnWidth(1, 100);
	m_faceInfoListCtrl.SetColumnWidth(2, 100);
	m_faceInfoListCtrl.SetColumnWidth(3, 100);

	m_chSnapFaceList.RemoveAll();
	m_chSnapFaceListCtrl.DeleteAllItems();
	m_chSnapFaceListCtrl.SetExtendedStyle(m_chSnapFaceListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_chSnapFaceListCtrl.InsertColumn(0, "No");
	m_chSnapFaceListCtrl.InsertColumn(1, "Id");
	m_chSnapFaceListCtrl.InsertColumn(2, "channel");
	m_chSnapFaceListCtrl.InsertColumn(3, "time");
	m_chSnapFaceListCtrl.SetColumnWidth(0, 50);
	m_chSnapFaceListCtrl.SetColumnWidth(1, 50);
	m_chSnapFaceListCtrl.SetColumnWidth(2, 50);
	m_chSnapFaceListCtrl.SetColumnWidth(3, 200);
	CString temp; 
	temp.Format(" %s\r\n %s\r\n %s",STR_INFO_FACE_MATCH_INFO1,STR_INFO_FACE_MATCH_INFO2,STR_INFO_FACE_MATCH_INFO3);
	SetDlgItemText(IDC_STATIC_text, temp);

	m_searchResultList.RemoveAll();
	m_searchResultListCtrl.DeleteAllItems();
	m_searchResultListCtrl.SetExtendedStyle(m_searchResultListCtrl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_searchResultListCtrl.InsertColumn(0, "No");
	m_searchResultListCtrl.InsertColumn(1, "Id");
	m_searchResultListCtrl.InsertColumn(2, "channel");
	m_searchResultListCtrl.InsertColumn(3, "time");
	m_searchResultListCtrl.SetColumnWidth(0, 50);
	m_searchResultListCtrl.SetColumnWidth(1, 50);
	m_searchResultListCtrl.SetColumnWidth(2, 50);
	m_searchResultListCtrl.SetColumnWidth(3, 200);
	//UpdateUI();
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

BOOL CFaceMatchlDlg::IsSupportFace()
{
	
	DWORD SUPPORT = 0; 
	DWORD  lpBytesReturned = 0;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_MATCH_SUPPORT, NULL, 0, &SUPPORT,  sizeof(DWORD), &lpBytesReturned);
	if(SUPPORT == 0)
	{
		m_support = FALSE;
		return FALSE; 
	}

	m_support = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control

}


void CFaceMatchlDlg::setMainDlg( CWnd* dlg )
{
	m_mainDlg = dlg;
}

void CFaceMatchlDlg::setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid)
{
	m_userID = userid; 
	m_deviceinfo = di;
	CString temp;
	CComboBox * combo = reinterpret_cast<CComboBox *>(GetDlgItem(IDC_CH_COMBO));
	for (int i = 0; i < m_deviceinfo->videoInputNum; i++)
	{
		temp.Format(STR_CHANNEL_D, i + 1);
		combo->AddString(temp);
	}
	combo->SetCurSel(0);
}
void CFaceMatchlDlg::test()
{
	


	//char *tempBuf = new char[100*1024];


	
	
	/*	lpBytesReturned = 0;

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


void CFaceMatchlDlg::OnBnClickedGetGroups()
{
	//NET_SDK_GET_FACE_INFO_GROUP_LIST,//queryFacePersonnalInfoGroupList in:NULL, out:NET_SDK_FACE_INFO_GROUP_ITEM list
	if(m_deviceinfo->deviceType == NET_SDK_NVR)
	{
		m_faceGroupList.RemoveAll();
		m_faceGroupListCtrl.DeleteAllItems();

		char *tempBuf = new char[100*1024];
		memset(tempBuf, 0, 100*1024);
		DWORD lpBytesReturned = 0;
		BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_INFO_GROUP_LIST,NULL, 0, tempBuf,  100*1024, &lpBytesReturned);
		if(ret)
		{

			TRACE("return true %d \n", lpBytesReturned);
			int num = lpBytesReturned/sizeof(NET_SDK_FACE_INFO_GROUP_ITEM);
			NET_SDK_FACE_INFO_GROUP_ITEM *pFACE_INFO_GROUP_ITEM = (NET_SDK_FACE_INFO_GROUP_ITEM*)tempBuf;
			for ( int i=0; i<num; i++)
			{
				NET_SDK_FACE_INFO_GROUP_ITEM sFACE_INFO_GROUP_ITEM;
				memcpy(&sFACE_INFO_GROUP_ITEM, pFACE_INFO_GROUP_ITEM+i, sizeof(NET_SDK_FACE_INFO_GROUP_ITEM));
				m_faceGroupList.AddTail(sFACE_INFO_GROUP_ITEM);

				int n = m_faceGroupListCtrl.GetItemCount();
				m_faceGroupListCtrl.InsertItem(n, _T(""));
				CString temp;
				temp.Format(_T("%d"), i+1);
				m_faceGroupListCtrl.SetItemText(n , 0, temp);

				temp.Format(_T("%d"), sFACE_INFO_GROUP_ITEM.groupId);
				m_faceGroupListCtrl.SetItemText(n , 1, temp);

				temp.Format(_T("%s"), sFACE_INFO_GROUP_ITEM.guid);
				m_faceGroupListCtrl.SetItemText(n, 2, temp);
				temp.Format(_T("%s"), Utf8ToGbk(sFACE_INFO_GROUP_ITEM.name).c_str());
				m_faceGroupListCtrl.SetItemText(n, 3, temp);
			}
		}
		else
		{
			AfxMessageBox(" faild!");
		}

		delete[]tempBuf;
		tempBuf = NULL;
	}
	else if(m_deviceinfo->deviceType == NET_SDK_IPCAMERA)
	{
		m_ipcFaceInfoList.RemoveAll();
		m_faceInfoListCtrl.DeleteAllItems();
		char *tempBuf = new char[sizeof(NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_INFO)];
		memset(tempBuf,0,sizeof(NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_INFO));
		NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_INFO *pAlbumInfo = (NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_INFO*)tempBuf;

		pAlbumInfo->iPageNum = 0;
		pAlbumInfo->iPageSize = 10;
		pAlbumInfo->bUseTypeFilter = true;
		pAlbumInfo->iType = 1;

		char *outBuf = new char[1000*1024];
		memset(outBuf, 0, 1000*1024);
		DWORD lpBytesReturned = 0;
		BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_IPC_LIST,tempBuf, sizeof(NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_INFO), outBuf,  1000*1024, &lpBytesReturned);
		delete []tempBuf;
		tempBuf = NULL;
		if(ret)
		{
			if(lpBytesReturned > 0)
			{
				char *rrOut = new char[lpBytesReturned];
				char *realOutBuf = rrOut;
				memset(realOutBuf,0,lpBytesReturned);
				memcpy(realOutBuf,outBuf,lpBytesReturned);
				delete[]outBuf;
				outBuf = NULL;


				int *totalNum = (int*)realOutBuf;
				realOutBuf += sizeof(int);
				int *currentNum = (int*)realOutBuf;
				realOutBuf += sizeof(int);
				for(int i = 0;i< *currentNum;i++)
				{
					NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_REPLY_INFO *pReplyAlbum = (NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_REPLY_INFO*)realOutBuf;
					
					realOutBuf += sizeof(NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_REPLY_INFO);
					realOutBuf += pReplyAlbum->stBaseInfo.iPicSize;
					m_ipcFaceInfoList.AddTail(pReplyAlbum->stBaseInfo);

					int n = m_faceInfoListCtrl.GetItemCount();
					m_faceInfoListCtrl.InsertItem(n, _T(""));
					CString temp;
					temp.Format(_T("%d"), i+1);
					m_faceInfoListCtrl.SetItemText(n , 0, temp);

					temp.Empty();
					temp.Format(_T("%d"), pReplyAlbum->iKey);
					m_faceInfoListCtrl.SetItemText(n , 1, temp);

					temp.Empty();
					temp.Format(_T("%s"), Utf8ToGbk(pReplyAlbum->stBaseInfo.szName).c_str());
					m_faceInfoListCtrl.SetItemText(n, 2, temp);

				}
				delete[] rrOut;
				
			}
		}

		if(outBuf)
		{
			delete []outBuf;
			outBuf = NULL;
		}
			
	}
	// TODO: 在此添加控件通知处理程序代码
	//OnCancel();
}

void CFaceMatchlDlg::OnBnClickedAddGroups()
{
	UpdateData();
	if(m_editGroupName.GetLength() == 0)
	{
		AfxMessageBox(" GroupName is null !");
		return;
	}
	DWORD lpBytesReturned = 0;
	NET_SDK_FACE_INFO_GROUP_ADD sFACE_INFO_GROUP_ADD;
	memset(&sFACE_INFO_GROUP_ADD, 0, sizeof(NET_SDK_FACE_INFO_GROUP_ADD));
	strcpy(sFACE_INFO_GROUP_ADD.name, GbkToUtf8(m_editGroupName.GetBuffer()).c_str());
	m_editGroupName.ReleaseBuffer();
	sFACE_INFO_GROUP_ADD.property = NET_SDK_FACE_INFO_GROUP_PROPERTY_LIMITED;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_ADD_FACE_INFO_GROUP,&sFACE_INFO_GROUP_ADD, sizeof(NET_SDK_FACE_INFO_GROUP_ADD), NULL,  0, &lpBytesReturned);
	if(!ret)
	{
		AfxMessageBox(" faild!");
	}
	else
	{
		AfxMessageBox(" success!");
	}
	
}

void CFaceMatchlDlg::OnBnClickedEditGroups()
{
	UpdateData();
	if(m_editGroupName.GetLength() == 0)
	{
		AfxMessageBox(" GroupName is null !");
		return;
	}
	
	int index = m_faceGroupListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (index < 0)
	{
		return ;
	}
	POSITION pos = NULL;
	pos = m_faceGroupList.GetHeadPosition();
	while (index > 0)
	{
		m_faceGroupList.GetNext(pos);

		index--;
	}

	NET_SDK_FACE_INFO_GROUP_ITEM &item = m_faceGroupList.GetNext(pos);
	strcpy(item.name, GbkToUtf8(m_editGroupName.GetBuffer()).c_str());
	
	DWORD lpBytesReturned = 0;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SET_FACE_INFO_GROUP,&item, sizeof(NET_SDK_FACE_INFO_GROUP_ITEM), NULL,  0, &lpBytesReturned);
	if(!ret)
	{
		AfxMessageBox(" faild!");
	}
	else
	{
		AfxMessageBox(" success!");
	}

}
void CFaceMatchlDlg::OnBnClickedDelGroups()
{
	int index = m_faceGroupListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (index < 0)
	{
		return ;
	}
	POSITION pos = NULL;
	pos = m_faceGroupList.GetHeadPosition();
	while (index > 0)
	{
		m_faceGroupList.GetNext(pos);

		index--;
	}

	NET_SDK_FACE_INFO_GROUP_ITEM &item = m_faceGroupList.GetNext(pos);


	NET_SDK_FACE_INFO_GROUP_DEL_EX sFACE_INFO_GROUP_DEL;
	memset(&sFACE_INFO_GROUP_DEL, 0, sizeof(NET_SDK_FACE_INFO_GROUP_DEL));

	//strcpy((char*)sFACE_INFO_GROUP_DEL.guid, (char*)item.guid);
	sFACE_INFO_GROUP_DEL.groupId = item.groupId;

	DWORD lpBytesReturned = 0;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_DEL_FACE_INFO_GROUP,&sFACE_INFO_GROUP_DEL, sizeof(NET_SDK_FACE_INFO_GROUP_DEL_EX), NULL,  0, &lpBytesReturned);
	if(!ret)
	{
		AfxMessageBox(" faild!");
	}
	else
	{
		AfxMessageBox(" success!");
	}

}

void CFaceMatchlDlg::OnBnClickedAddFace()
{
	//createFacePersonnalInfo in:NET_SDK_FACE_INFO_ADD out:NULL



	UpdateData();
	if (m_editFaceName.IsEmpty())
	{
		AfxMessageBox(" Face Name is null !!");
		return ;
	}
	CString path;
	GetDlgItemText(IDC_STATIC_PICTURE_PATH, path);
	if (path.IsEmpty())
	{
		AfxMessageBox(STR_SELECT_PIC_PATH);
		return ;
	}

	if(m_deviceinfo->deviceType == NET_SDK_NVR)
	{
		
		int index = m_faceGroupListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		if (index < 0)
		{
			AfxMessageBox(" select GroupList first !!");
			return ;
		}
		POSITION pos = NULL;
		pos = m_faceGroupList.GetHeadPosition();
		while (index > 0)
		{
			m_faceGroupList.GetNext(pos);
			index--;
		}

		NET_SDK_FACE_INFO_GROUP_ITEM &item = m_faceGroupList.GetNext(pos);

		char *tempBuf = new char[1024*1024];
		memset(tempBuf, 0, 1024*1024);
		NET_SDK_FACE_INFO_ADD *pFACE_INFO_ADD = (NET_SDK_FACE_INFO_ADD*)tempBuf;

		strcpy(pFACE_INFO_ADD->sFaceInfoItem.name, GbkToUtf8(m_editFaceName.GetBuffer()).c_str());
		m_editFaceName.ReleaseBuffer();
		strcpy(pFACE_INFO_ADD->sFaceInfoItem.mobile, m_editFaceMobile.GetBuffer());
		m_editFaceMobile.ReleaseBuffer();

		strcpy(pFACE_INFO_ADD->sFaceInfoItem.nativePlace, GbkToUtf8(m_editFaceNativePlace.GetBuffer()).c_str());
		m_editFaceNativePlace.ReleaseBuffer();

		strcpy(pFACE_INFO_ADD->sFaceInfoItem.certificateNum, m_editFaceCertificateNum.GetBuffer());
		m_editFaceCertificateNum.ReleaseBuffer();

		strcpy(pFACE_INFO_ADD->sFaceInfoItem.mobile, m_editFaceMobile.GetBuffer());
		m_editFaceMobile.ReleaseBuffer();

		strcpy(pFACE_INFO_ADD->sFaceInfoItem.number, m_editFaceNumber.GetBuffer());
		m_editFaceNumber.ReleaseBuffer();

		pFACE_INFO_ADD->sFaceInfoItem.birthday = atoi(m_editFaceBirthday.GetBuffer());
		m_editFaceBirthday.ReleaseBuffer();

	#ifdef __CHONGQING_ZHONGRAN__
		pFACE_INFO_ADD->sFaceInfoItem.groups[0].groupId = item.groupId;
	#else
		strcpy((char*)pFACE_INFO_ADD->sFaceInfoItem.groups[0].guid, (char*)item.guid);
	#endif


		pFACE_INFO_ADD->imgNum = 1;
		pFACE_INFO_ADD->haveImgData = 1;
		/*sFACE_INFO_ADD.sFaceImgInfo[0].chl = 1;
		sFACE_INFO_ADD.sFaceImgInfo[0].imgId = 1837;
		sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.year = 2018;
		sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.month = 5;
		sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.mday = 22;
		sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.hour = 23;
		sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.minute = 48;
		sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.second = 49;
		sFACE_INFO_ADD.sFaceImgInfo[0].frameTime.nMicrosecond = 1277790;*/
		pFACE_INFO_ADD->imgData = (unsigned char*)tempBuf+sizeof(NET_SDK_FACE_INFO_ADD);
		//FILE *fp = fopen("./face_1.jpg", "rb");
		FILE *fp = fopen(path.GetBuffer(), "rb");
		path.ReleaseBuffer();
		int length = 0;
		if(fp)
		{
			fseek(fp,0,SEEK_END);
			length=ftell(fp);
			fseek(fp,0,SEEK_SET);
			int readLen = fread((char *)pFACE_INFO_ADD->imgData , length, 1, fp );
			fclose(fp);
		}
		//GetSize((char *)pFACE_INFO_ADD->imgData , length);
		pFACE_INFO_ADD->imgWidth = 440;
		pFACE_INFO_ADD->imgHeight = 620;
		pFACE_INFO_ADD->imgLen = length;
		DWORD lpBytesReturned = 0;
		int len = sizeof(NET_SDK_FACE_INFO_ADD);

		unsigned int faceId = 0;
		BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_ADD_FACE_INFO, tempBuf, sizeof(NET_SDK_FACE_INFO_ADD)+length, &faceId,  sizeof(unsigned int), &lpBytesReturned);
		if(ret)
		{
			TRACE("NET_SDK_ADD_FACE_INFO success faceId=%d \n", faceId);
		}
		else
		{
			AfxMessageBox(" NET_SDK_ADD_FACE_INFO faild!");
		}
		delete[] tempBuf;
		tempBuf = NULL;
	}
	else if(m_deviceinfo->deviceType == NET_SDK_IPCAMERA)
	{
#if 0
		char *tempBuf = new char[1024*1024];
		memset(tempBuf, 0, 1024*1024);

		NET_SDK_IVE_FACE_MATCH_ADD_ALBUM_INFO *itemToAdd = (NET_SDK_IVE_FACE_MATCH_ADD_ALBUM_INFO*)tempBuf;

		itemToAdd->iType = 0;
		memcpy(itemToAdd->szName,m_editFaceName.GetBuffer(),sizeof(m_editFaceName.GetBuffer()));	
		m_editFaceName.ReleaseBuffer();
		itemToAdd->iMale = 1;
		itemToAdd->iAge = 10;
		memcpy(itemToAdd->szIdentifyNum,m_editFaceCertificateNum.GetBuffer(),sizeof(m_editFaceCertificateNum.GetBuffer()));	
		m_editFaceCertificateNum.ReleaseBuffer();
		memcpy(itemToAdd->szTel,m_editFaceMobile.GetBuffer(),sizeof(m_editFaceMobile.GetBuffer()));	
		m_editFaceMobile.ReleaseBuffer();


		char* imgData = tempBuf + sizeof(NET_SDK_IVE_FACE_MATCH_ADD_ALBUM_INFO);

		FILE *fp = fopen(path.GetBuffer(), "rb");
		path.ReleaseBuffer();
		int length = 0;
		if(fp)
		{
			fseek(fp,0,SEEK_END);
			length=ftell(fp);
			fseek(fp,0,SEEK_SET);
			int readLen = fread(imgData , length, 1, fp );	
			fclose(fp);
		}
		itemToAdd->iPicSize = length;

		DWORD lpBytesReturned = 0;
		
		unsigned char *rret  = new unsigned char[1024];
		BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_ADD_FACE_IPC, tempBuf, sizeof(NET_SDK_IVE_FACE_MATCH_ADD_ALBUM_INFO)+length, rret,  1024, &lpBytesReturned);
		if(ret)
		{
			if(lpBytesReturned >= sizeof(NET_SDK_IVE_FACE_MATCH_ADD_FACE_REPLY_T))
			{
				NET_SDK_IVE_FACE_MATCH_ADD_FACE_REPLY_T *preply = (NET_SDK_IVE_FACE_MATCH_ADD_FACE_REPLY_T*)rret;
				TRACE("NET_SDK_ADD_FACE_IPC success faceId=%d \n", preply->iPersonId);
				int a = 0;
			}						
		}
		else
		{
			AfxMessageBox(" NET_SDK_ADD_FACE_IPC faild!");
		}
		delete[] rret;
		delete[] tempBuf;
		rret = NULL;
		tempBuf = NULL;
#endif
#if 1
		std::string szUrl = "AddTargetFace";
		
		std::string sendXML = "<?xml version=\"1.0\" encoding=\"UTF-8\"?> <config version=\"1.7\" xmlns=\"http://www.ipc.com/ver10\">"
			"<types>"
			"<listType>"
			"<enum>strangerList</enum>"
			"<enum>whiteList</enum>"
			"<enum>blackList</enum>"
			"</listType>"
			"<sex>"
			"<enum>male</enum>"
			"<enum>female</enum>"
			"</sex>"
			"<format>"
			"<enum>jpg</enum>"
			"<enum>png</enum>"
			"<enum>bmp</enum>"
			"</format>"
			"</types>"
			"<personInfo>"
			"<listType>%s</listType>"
			"<name><![CDATA[%s]]></name>"
			"<sex>%s</sex>"
			"<age>%d</age>"
			"<identifyNumber><![CDATA[%s]]></identifyNumber>"
			"<telephone><![CDATA[%s]]></telephone>"
			"</personInfo>"
			"<faceImgs type=\"list\" maxCount=\"5\" count=\"0\">"
			"</faceImgs>"
			"</config>";
		char *buf = new char[1024*1024];
		memset(buf, 0, 1024*1024);
		std::string listType = "whiteList";
		std::string sex = "female";
		int age = 18;

		//char szDest[512] = {0};
		//memset(szDest, 0, sizeof(szDest));
		//strcpy(szDest, strDeviceName.GetBuffer(0));
		//ANSItoUTF82(szDest, sizeof(szDest), m_editFaceName.GetBuffer(0), DD_MAX_CAMERA_NAME_BUF_LEN);

		char szDest[512] = {0};
		//memset(szDest, 0, sizeof(szDest));
		strcpy(szDest, m_editFaceName.GetBuffer(0));
		GBtoUTF8(szDest, sizeof(szDest), szDest, DD_MAX_CAMERA_NAME_BUF_LEN);
		sprintf(buf, sendXML.c_str(), listType.c_str(), szDest,sex.c_str(), age, m_editFaceCertificateNum.GetBuffer(), m_editFaceMobile.GetBuffer() );

		m_editFaceName.ReleaseBuffer();
		m_editFaceCertificateNum.ReleaseBuffer();
		m_editFaceMobile.ReleaseBuffer();
		std::string SendXml = buf;
		CXMLParse SendParse;
		SendParse.LoadXML(SendXml);
		memset(buf, 0, 1024*1024);
		//FILE *fp = fopen("C:\\Users\\8590\\Desktop\\face_16.jpg", "rb");
		FILE *fp = fopen(path.GetBuffer(), "rb");
		path.ReleaseBuffer();
		int length = 0;
		if(fp)
		{
			fseek(fp,0,SEEK_END);
			length=ftell(fp);
			fseek(fp,0,SEEK_SET);
			int readLen = fread(buf , length, 1, fp );
			fclose(fp);
		}
		
		int weidth=0;
		int height=0;
		int GetJepg = GetJepgSize1(buf,length, &weidth,  &height);
		if(GetJepg<=0)
		{
			AfxMessageBox(" NET_SDK_ADD_FACE_INFO faild!");
			delete []buf;
			return;
		}
		tuint_ptr ptrFaceImgs = SendParse.GetFirstItem("config/faceImgs");
		if(ptrFaceImgs)
		{
			SendParse.SetAttribute_i32(ptrFaceImgs, "count", 1);
			tuint_ptr ptrItem = SendParse.AddValue(ptrFaceImgs, "item");

			int outSize = ((length+2) / 3 * 4 + 1);//AV_BASE64_SIZE
			char *szTemp = new char[outSize];
			CBase64 base64;
			base64.Encode((const unsigned char*)buf, length, szTemp, outSize);
			SendParse.AddValue(ptrItem, "pictureData", szTemp, TRUE);
			delete[] szTemp;
			SendParse.AddValue_i32(ptrItem, "pictureNum", 1);
			SendParse.AddValue_i32(ptrItem, "Width", weidth);
			SendParse.AddValue_i32(ptrItem, "Height", height);
			SendParse.AddValue_i32(ptrItem, "Height", height);
			SendParse.AddValue(ptrItem, "format", "jpg");
			SendParse.AddValue_i32(ptrItem, "size", length);
		}	
		/*FILE *pFile = fopen("./test.xml", "w");
		int len = 0;
		len = fwrite(SendParse.GetDoc().c_str(), SendParse.GetDoc().length(), 1, pFile);
		fclose( pFile);*/
		memset(buf, 0, 1024*1024);
		DWORD lReturned = 0;
		BOOL ret = NET_SDK_TransparentConfig(m_userID, (char*)SendParse.GetDoc().c_str(), (char*)szUrl.c_str(), buf, 1024*1024, &lReturned);
		if(ret == 0)
		{
			AfxMessageBox(" NET_SDK_ADD_FACE_INFO faild!");
		}
		else if(lReturned)
		{
			CXMLParse xmlParse;
			xmlParse.LoadXML(buf);
			tuint_ptr ptrvalue = xmlParse.GetFirstItem("config/addFace/resultInfo/item/personID");
			if(ptrvalue)
			{
				int tempValue = 0;
				xmlParse.GetValue_i32(ptrvalue, tempValue);
				if(tempValue > 0)
				{
					AfxMessageBox(" NET_SDK_ADD_FACE_INFO success!");
				}
				else
				{
					AfxMessageBox(" NET_SDK_ADD_FACE_INFO faild!");
				}
			}
			else
				AfxMessageBox(" NET_SDK_ADD_FACE_INFO faild!");
			
		}
		delete []buf;
		return;
#endif
	}
	
}
void CFaceMatchlDlg::OnBnClickedEditFace()
{
	UpdateData();
	CString path;
	GetDlgItemText(IDC_STATIC_PICTURE_PATH, path);
	if (path.IsEmpty())
	{
		AfxMessageBox(STR_SELECT_PIC_PATH);
		return ;
	}
	if(m_deviceinfo->deviceType == NET_SDK_NVR)
	{
	//NET_SDK_SET_FACE_INFO,//editFacePersonnalInfo in:NET_SDK_FACE_INFO_EDIT out:NULL
	//解析选中的目标组
	int index = m_faceGroupListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (index < 0)
	{
		return ;
	}
	POSITION pos = NULL;
	pos = m_faceGroupList.GetHeadPosition();
	while (index > 0)
	{
		m_faceGroupList.GetNext(pos);

		index--;
	}
	NET_SDK_FACE_INFO_GROUP_ITEM &item = m_faceGroupList.GetNext(pos);
	
	unsigned int groupId = item.groupId;
	char groupGuid[48] = {0};
	strcpy(groupGuid, (char*)item.guid);

	char *tempBuf = new char[100*1024];
	memset(tempBuf, 0, 100*1024);
	NET_SDK_FACE_INFO_EDIT *pFACE_INFO_EDIT = (NET_SDK_FACE_INFO_EDIT*)tempBuf;
	pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.groups[0].groupId  = groupId;
	strcpy((char *)pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.groups[0].guid, groupGuid);

	pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.itemId = m_curFaceItem.itemId;//双击时选中的目标
	pFACE_INFO_EDIT->delFaceImgs[0] = 1;

	strcpy(pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.name, m_editFaceName.GetBuffer());
	m_editFaceName.ReleaseBuffer();
	
	strcpy(pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.mobile, m_editFaceMobile.GetBuffer());
	m_editFaceMobile.ReleaseBuffer();

	strcpy(pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.nativePlace, m_editFaceNativePlace.GetBuffer());
	m_editFaceNativePlace.ReleaseBuffer();

	strcpy(pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.certificateNum, m_editFaceCertificateNum.GetBuffer());
	m_editFaceCertificateNum.ReleaseBuffer();

	strcpy(pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.mobile, m_editFaceMobile.GetBuffer());
	m_editFaceMobile.ReleaseBuffer();

	strcpy(pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.number, m_editFaceNumber.GetBuffer());
	m_editFaceNumber.ReleaseBuffer();

	pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.birthday = atoi(m_editFaceBirthday.GetBuffer());
	m_editFaceBirthday.ReleaseBuffer();
	
	pFACE_INFO_EDIT->sFaceInfoItem.sFaceInfoItem.faceImgCount = 1;
	pFACE_INFO_EDIT->sFaceInfoItem.imgNum = 1;
	pFACE_INFO_EDIT->sFaceInfoItem.haveImgData = 1;
	pFACE_INFO_EDIT->sFaceInfoItem.imgWidth = 256;
	pFACE_INFO_EDIT->sFaceInfoItem.imgHeight = 256;
	pFACE_INFO_EDIT->sFaceInfoItem.imgData = (unsigned char *)tempBuf+sizeof(NET_SDK_FACE_INFO_EDIT);;
	
	int length = 0;
	FILE *fp = fopen(path, "rb");
	//FILE *fp = fopen("./face.jpg", "rb");
	if(fp)
	{
		fseek(fp,0,SEEK_END);
		length=ftell(fp);
		fseek(fp,0,SEEK_SET);
		int fret = fread(pFACE_INFO_EDIT->sFaceInfoItem.imgData , length, 1, fp );
		fclose(fp);
	}
	
	pFACE_INFO_EDIT->sFaceInfoItem.imgLen = length;
	
	/*FACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].chl = 16;
	pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].imgId = 226;
	pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.year = 2018;
	pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.month = 5;
	pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.mday = 24;
	pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.hour = 0;
	pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.minute = 48;
	pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.second = 39;
	pFACE_INFO_EDIT->sFaceInfoItem.sFaceImgInfo[0].frameTime.nMicrosecond = 8483680;*/
	
	DWORD lpBytesReturned = 0;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SET_FACE_INFO, tempBuf, length+sizeof(NET_SDK_FACE_INFO_EDIT), NULL,  0, &lpBytesReturned);
	if(!ret)
	{
		AfxMessageBox(" NET_SDK_SET_FACE_INFO faild!");
	}
	else
	{
		AfxMessageBox("NET_SDK_SET_FACE_INFO success!");
	}
	delete[] tempBuf;
	tempBuf = NULL;
	}
	else if(m_deviceinfo->deviceType == NET_SDK_IPCAMERA)
	{
		
	}
}

void CFaceMatchlDlg::OnBnClickedDelFace()
{
	if(m_deviceinfo->deviceType == NET_SDK_NVR)
	{
		NET_SDK_FACE_INFO_DEL sFACE_INFO_DEL;
		memset(&sFACE_INFO_DEL, 0, sizeof(NET_SDK_FACE_INFO_DEL));
		sFACE_INFO_DEL.faceInfoListItemId = m_curFaceItem.itemId;
		sFACE_INFO_DEL.groupsId[0] = m_curFaceItem.groups[0].groupId;
		DWORD lpBytesReturned = 0;
		BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_DEL_FACE_INFO,&sFACE_INFO_DEL, sizeof(NET_SDK_FACE_INFO_DEL), NULL,  0, &lpBytesReturned);
		if(!ret)
		{
			AfxMessageBox(" faild!");
		}
		else
		{
			AfxMessageBox("NET_SDK_DEL_FACE_INFO success!");
		}
	}
	else if(m_deviceinfo->deviceType == NET_SDK_IPCAMERA)
	{
		int index = m_faceInfoListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		if (index < 0)
		{
			return ;
		}
		NET_SDK_IVE_FACE_MATCH_DELE_ALBUM_INFO sFACE_INFO_DEL;
		memset(&sFACE_INFO_DEL, 0, sizeof(NET_SDK_IVE_FACE_MATCH_DELE_ALBUM_INFO));
		sFACE_INFO_DEL.bUseKeyFilter = true;
		CString tempStr = m_faceInfoListCtrl.GetItemText(index, 1);
		sFACE_INFO_DEL.iKey = _tcstol(tempStr, NULL, 10);

		DWORD lpBytesReturned = 0;
		BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_DEL_FACE_IPC,&sFACE_INFO_DEL, sizeof(NET_SDK_IVE_FACE_MATCH_DELE_ALBUM_INFO), NULL,  0, &lpBytesReturned);
		if(!ret)
		{
			AfxMessageBox(" faild!");
		}
		else
		{
			AfxMessageBox("NET_SDK_DEL_FACE_IPC success!");
		}
	}

	
}

void CFaceMatchlDlg::OnBnClickeGetCHSnapFaceList()
{
		//NET_SDK_SEARCH_CH_SNAP_FACE_IMG_LIST,//queryChSnapFaceImageList in:NET_SDK_CH_SNAP_FACE_IMG_LIST_SEARCH out:NET_SDK_CH_SNAP_FACE_IMG_LIST
	// 
	UpdateData();
	m_chSnapFaceList.RemoveAll();
	m_chSnapFaceListCtrl.DeleteAllItems();

	char *tempBuf = new char[100*1024];
	memset(tempBuf, 0 , 100*1024);
	NET_SDK_CH_SNAP_FACE_IMG_LIST_SEARCH sCH_SNAP_FACE_IMG_LIST_SEARCH;
	memset(&sCH_SNAP_FACE_IMG_LIST_SEARCH, 0, sizeof(NET_SDK_CH_SNAP_FACE_IMG_LIST_SEARCH));
	sCH_SNAP_FACE_IMG_LIST_SEARCH.dwChannel = m_chnn;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.pageIndex = 1;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.pageSize = 10;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.startTime.year = m_searchDate.GetYear();
	sCH_SNAP_FACE_IMG_LIST_SEARCH.startTime.month = m_searchDate.GetMonth();
	sCH_SNAP_FACE_IMG_LIST_SEARCH.startTime.mday = m_searchDate.GetDay();

	
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.year = m_searchDate.GetYear();
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.month = m_searchDate.GetMonth();
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.mday = m_searchDate.GetDay();
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.hour = 23;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.minute = 59;
	sCH_SNAP_FACE_IMG_LIST_SEARCH.endTime.second = 59;
	DWORD lpBytesReturned = 0;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SEARCH_CH_SNAP_FACE_IMG_LIST,&sCH_SNAP_FACE_IMG_LIST_SEARCH, sizeof(NET_SDK_CH_SNAP_FACE_IMG_LIST_SEARCH), tempBuf,  100*1024, &lpBytesReturned);
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
			m_chSnapFaceList.AddTail(sFACE_IMG_INFO_CH);

			int n = m_chSnapFaceListCtrl.GetItemCount();
			m_chSnapFaceListCtrl.InsertItem(n, _T(""));
			CString temp;
			temp.Format(_T("%d"), i+1);
			m_chSnapFaceListCtrl.SetItemText(n , 0, temp);

			temp.Format(_T("%d"), sFACE_IMG_INFO_CH.snapImgId);
			m_chSnapFaceListCtrl.SetItemText(n , 1, temp);

			temp.Format(_T("%d"), sFACE_IMG_INFO_CH.chl);
			m_chSnapFaceListCtrl.SetItemText(n, 2, temp);
			temp.Format(_T("%d-%d-%d %d:%d:%d:%d"), sFACE_IMG_INFO_CH.frameTime.year, sFACE_IMG_INFO_CH.frameTime.month, sFACE_IMG_INFO_CH.frameTime.mday,
				sFACE_IMG_INFO_CH.frameTime.hour, sFACE_IMG_INFO_CH.frameTime.minute, sFACE_IMG_INFO_CH.frameTime.second, sFACE_IMG_INFO_CH.frameTime.nMicrosecond);
			m_chSnapFaceListCtrl.SetItemText(n, 3, temp);
		}
	}
	delete[]tempBuf;
	tempBuf = NULL;
}

//以图搜图这里只写一种， 若查更多见test里面屏蔽代码
void CFaceMatchlDlg::OnBnClickeSearchImgByImg()
{
	
	char *inBuf = new char[10*1024];
	memset(inBuf, 0, 10*1024);

	NET_SDK_SEARCH_IMAGE_BY_IMAGE *pSEARCH_IMAGE_BY_IMAGE = (NET_SDK_SEARCH_IMAGE_BY_IMAGE*)inBuf;

	pSEARCH_IMAGE_BY_IMAGE->pageIndex = 1;
	pSEARCH_IMAGE_BY_IMAGE->pageSize = 10;
	pSEARCH_IMAGE_BY_IMAGE->similarity = 75;
	pSEARCH_IMAGE_BY_IMAGE->resultCountLimit = 500;

	pSEARCH_IMAGE_BY_IMAGE->startTime.year = m_searchDate.GetYear();
	pSEARCH_IMAGE_BY_IMAGE->startTime.month = m_searchDate.GetMonth();
	pSEARCH_IMAGE_BY_IMAGE->startTime.mday = m_searchDate.GetDay();

	pSEARCH_IMAGE_BY_IMAGE->endTime.year = m_searchDate.GetYear();
	pSEARCH_IMAGE_BY_IMAGE->endTime.month = m_searchDate.GetMonth();
	pSEARCH_IMAGE_BY_IMAGE->endTime.mday = m_searchDate.GetDay();
	pSEARCH_IMAGE_BY_IMAGE->endTime.hour = 23;
	pSEARCH_IMAGE_BY_IMAGE->endTime.minute = 59;
	pSEARCH_IMAGE_BY_IMAGE->endTime.second = 59;
	pSEARCH_IMAGE_BY_IMAGE->searchType = SEARCH_IMAGE_BY_FACE_IMAGES;

	POSITION pos = NULL;
	pos = m_chSnapFaceList.GetHeadPosition();
	if(pos)
	{
		NET_SDK_FACE_IMG_INFO_CH &item = m_chSnapFaceList.GetNext(pos);

		pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.chl = item.chl;
		pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.snapImgId = item.snapImgId;
		pSEARCH_IMAGE_BY_IMAGE->sfaceImgs.frameTime = item.frameTime;
	}
	
	int length = 0;
	char *tempBuf = new char[100*1024];
	DWORD lpBytesReturned = 0;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SEARCH_IMAGE_BY_IMG,inBuf, sizeof(NET_SDK_SEARCH_IMAGE_BY_IMAGE)+length, tempBuf,  100*1024, &lpBytesReturned);
	if(!ret)
	{
		AfxMessageBox(" faild!");
	}
	else
	{
		NET_SDK_SEARCH_IMAGE_BY_IMAGE_LIST *pSEARCH_IMAGE_BY_IMAGE_LIST = (NET_SDK_SEARCH_IMAGE_BY_IMAGE_LIST*)tempBuf;
		TRACE("listNum=%d, bEnd=%d \n", pSEARCH_IMAGE_BY_IMAGE_LIST->listNum, pSEARCH_IMAGE_BY_IMAGE_LIST->bEnd);
		NET_SDK_SEARCH_IMAGE_ITEM *pSEARCH_IMAGE_ITEM = pSEARCH_IMAGE_BY_IMAGE_LIST->pSearchImageItem;
		for(int i=0; i< pSEARCH_IMAGE_BY_IMAGE_LIST->listNum; i++)
		{
			TRACE("faceFeatureId=%d, %d:%d:%d ----- %d:%d:%d\n", pSEARCH_IMAGE_ITEM[i].faceFeatureId, pSEARCH_IMAGE_ITEM[i].recStartTime.hour, pSEARCH_IMAGE_ITEM[i].recStartTime.minute, pSEARCH_IMAGE_ITEM[i].recStartTime.second,
				pSEARCH_IMAGE_ITEM[i].recEndTime.hour, pSEARCH_IMAGE_ITEM[i].recEndTime.minute, pSEARCH_IMAGE_ITEM[i].recEndTime.second);
		}
	}
	delete[]tempBuf;
	delete[]inBuf;
	tempBuf = NULL;
	inBuf = NULL;
}


void CFaceMatchlDlg::OnBnClickeGetAlarmTrigger()
{
	char *tempBuf = new char[200*1024];
	memset(tempBuf, 0, 200*1024);
	DWORD lpBytesReturned = 0;
	int len = sizeof(NET_SDK_FACE_MATCH_ALARM);
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_MATCH_ALARM, NULL, 0, tempBuf,  200*1024, &lpBytesReturned);
	if(ret)
	{
		NET_SDK_FACE_MATCH_ALARM *pFACE_MATCH_ALARM = (NET_SDK_FACE_MATCH_ALARM*)tempBuf;
		TRACE("lpBytesReturned=%d, faceFeatureGroupsNum=%d \n", lpBytesReturned, pFACE_MATCH_ALARM->faceFeatureGroupsNum);
		NET_SDK_FACE_MATCH_ALARM_TRIGGER *pFaceMatchAlarmTrigger = pFACE_MATCH_ALARM->pFaceMatchAlarmTrigger;
		for(int i=0; i< pFACE_MATCH_ALARM->faceFeatureGroupsNum; i++)
		{
			TRACE("%s groupId = %d, %d-%d-%d-%d-%d-%d-%d-%d-%d\n", pFaceMatchAlarmTrigger[i].guid, pFaceMatchAlarmTrigger[i].groupId,pFaceMatchAlarmTrigger[i].groupSwitch, pFaceMatchAlarmTrigger[i].alarmOutSwitch, 
				pFaceMatchAlarmTrigger[i].recSwitch, pFaceMatchAlarmTrigger[i].snapSwitch, pFaceMatchAlarmTrigger[i].popVideo,
				pFaceMatchAlarmTrigger[i].buzzerSwitch,pFaceMatchAlarmTrigger[i].emailSwitch, pFaceMatchAlarmTrigger[i].msgPushSwitch, pFaceMatchAlarmTrigger[i].popMsgSwitch);
		}
	#ifdef __CHONGQING_ZHONGRAN__
	for (int _i = 0; pFACE_MATCH_ALARM->enableCH[_i] > 0; ++_i)
		{
			for (int _j = 0; _j < pFACE_MATCH_ALARM->FaceMatchChanGroup[_i].faceFeatureGroupsNum; ++_j)
			{
				TRACE("---------------->>> ch=%d GroupsNum=%d, groupID=%d\n",pFACE_MATCH_ALARM->enableCH[_i], pFACE_MATCH_ALARM->FaceMatchChanGroup[_i].faceFeatureGroupsNum, pFACE_MATCH_ALARM->FaceMatchChanGroup[_i].pfaceFeatureGroupIDs[_j]);
			}
		}
	#endif
		if (m_faceAlarmTrigger)
		{
			memset(m_faceAlarmTrigger, 0, 200*1024);
			memcpy(m_faceAlarmTrigger, tempBuf, lpBytesReturned);
			NET_SDK_FACE_MATCH_ALARM *pFACE_MATCH_ALARM = (NET_SDK_FACE_MATCH_ALARM*)m_faceAlarmTrigger;
			TRACE(" faceFeatureGroupsNum=%d \n", pFACE_MATCH_ALARM->faceFeatureGroupsNum);
			char *tempData = m_faceAlarmTrigger+sizeof(NET_SDK_FACE_MATCH_ALARM);
			pFACE_MATCH_ALARM->pFaceMatchAlarmTrigger = (NET_SDK_FACE_MATCH_ALARM_TRIGGER*)tempData;

		}
		
		AfxMessageBox(" success !");
	}
	else
	{
		AfxMessageBox(" faild!");
	}
	delete[] tempBuf;
	tempBuf = NULL;

}

void CFaceMatchlDlg::OnBnClickeSetAlarmTrigger()
{
	if (!m_faceAlarmTrigger)
	{
		AfxMessageBox(" 111111111111");
		return;
	}


	DWORD lpBytesReturned = 0;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SET_FACE_MATCH_ALARM, m_faceAlarmTrigger, 200*1024, NULL,  0, &lpBytesReturned);
	if(ret)
	{
		AfxMessageBox(" success !");
	}
	else
	{
		AfxMessageBox(" faild!");
	}

}

void CFaceMatchlDlg::OnNMDblclkListGroups(NMHDR *pNMHDR, LRESULT *pResult)
{
	m_faceInfoList.RemoveAll();
	m_faceInfoListCtrl.DeleteAllItems();

	int index = m_faceGroupListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (index < 0)
	{
		return ;
	}
	POSITION pos = NULL;
	pos = m_faceGroupList.GetHeadPosition();
	while (index > 0)
	{
		m_faceGroupList.GetNext(pos);
		
		index--;
	}

	NET_SDK_FACE_INFO_GROUP_ITEM &item = m_faceGroupList.GetNext(pos);

	//NET_SDK_GET_FACE_INFO_LIST,//queryFacePersonnalInfoList in:NET_SDK_FACE_INFO_LIST_GET, out:NET_SDK_FACE_INFO_LIST
	NET_SDK_FACE_INFO_LIST_GET sFACE_INFO_LIST_GET;
	memset(&sFACE_INFO_LIST_GET, 0, sizeof(NET_SDK_FACE_INFO_LIST_GET));
	sFACE_INFO_LIST_GET.groupId = item.groupId;
	sFACE_INFO_LIST_GET.pageIndex = 1;
	sFACE_INFO_LIST_GET.pageSize = 10;
	//strcpy(sFACE_INFO_LIST_GET.certificateNum, "123456789");

	char *tempBuf = new char[100*1024];
	memset(tempBuf, 0, 100*1024);
	DWORD lpBytesReturned = 0;

	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_INFO_LIST, &sFACE_INFO_LIST_GET, sizeof(NET_SDK_FACE_INFO_LIST_GET), tempBuf,  100*1024, &lpBytesReturned);
	if(ret)
	{
		NET_SDK_FACE_INFO_LIST *pFACE_INFO_LIST = (NET_SDK_FACE_INFO_LIST*)tempBuf;
		TRACE("listNum =%d , pFACE_INFO_LIST->totalNum =%d lpBytesReturned=%d \n", pFACE_INFO_LIST->listNum, pFACE_INFO_LIST->totalNum, lpBytesReturned);
		NET_SDK_FACE_INFO_LIST_ITEM *pFACE_INFO_LIST_ITEM = pFACE_INFO_LIST->pFaceInfoListItem;
		for(int i=0; i< pFACE_INFO_LIST->listNum; i++)
		{
			TRACE("itemId =%d , name =%s birthday=%d\n", pFACE_INFO_LIST_ITEM[i].itemId, pFACE_INFO_LIST_ITEM[i].name, pFACE_INFO_LIST_ITEM[i].birthday);
			NET_SDK_FACE_INFO_LIST_ITEM sFACE_INFO_LIST_ITEM;
			memcpy(&sFACE_INFO_LIST_ITEM, pFACE_INFO_LIST_ITEM+i, sizeof(NET_SDK_FACE_INFO_LIST_ITEM));
			m_faceInfoList.AddTail(sFACE_INFO_LIST_ITEM);

			int n = m_faceInfoListCtrl.GetItemCount();
			m_faceInfoListCtrl.InsertItem(n, _T(""));
			CString temp;
			temp.Format(_T("%d"), i+1);
			m_faceInfoListCtrl.SetItemText(n , 0, temp);
			
			temp.Empty();
			temp.Format(_T("%d"), sFACE_INFO_LIST_ITEM.itemId);
			m_faceInfoListCtrl.SetItemText(n , 1, temp);

			temp.Empty();
			temp.Format(_T("%s"), Utf8ToGbk(sFACE_INFO_LIST_ITEM.name).c_str());
			m_faceInfoListCtrl.SetItemText(n, 2, temp);
			
		}
	}
	else
	{
		AfxMessageBox(" faild!");
	}
	delete[]tempBuf;
	tempBuf = NULL;

}

void CFaceMatchlDlg::OnNMDblclkListFace(NMHDR *pNMHDR, LRESULT *pResult)
{
	int index = m_faceInfoListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (index < 0)
	{
		return ;
	}
	POSITION pos = NULL;
	if(m_deviceinfo->deviceType == NET_SDK_IPCAMERA)
	{
		
		char *tempBuf = new char[sizeof(NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_INFO)];
		memset(tempBuf,0,sizeof(NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_INFO));
		NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_INFO *pAlbumInfo = (NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_INFO*)tempBuf;
		


		pAlbumInfo->iPageNum = 0;
		pAlbumInfo->iPageSize = 10;
		pAlbumInfo->bUseTypeFilter = true;
		pAlbumInfo->iType = 1;

		char *outBuf = new char[1000*1024];
		memset(outBuf, 0, 1000*1024);
		DWORD lpBytesReturned = 0;


		CString tempStr = m_faceInfoListCtrl.GetItemText(index, 1);
		int iKey = _tcstol(tempStr, NULL, 10);

		BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_IPC_LIST,tempBuf, sizeof(NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_INFO), outBuf,  1000*1024, &lpBytesReturned);
		delete []tempBuf;
		tempBuf = NULL;
		if(ret)
		{
			if(lpBytesReturned > 0)
			{
				char *rrOut = new char[lpBytesReturned];
				char *realOutBuf = rrOut;

				memset(realOutBuf,0,lpBytesReturned);
				memcpy(realOutBuf,outBuf,lpBytesReturned);
				delete[]outBuf;
				outBuf = NULL;


				int *totalNum = (int*)realOutBuf;
				realOutBuf += sizeof(int);
				int *currentNum = (int*)realOutBuf;
				realOutBuf += sizeof(int);
				for(int i = 0;i< *currentNum;i++)
				{
					NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_REPLY_INFO *pReplyAlbum = (NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_REPLY_INFO*)realOutBuf;
					if(pReplyAlbum->iKey == iKey)
					{
						realOutBuf += sizeof(NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_REPLY_INFO);		

						char filename[20] = {0};
						sprintf(filename, "./face_%d.jpg", pReplyAlbum->iKey);
						FILE *file = fopen(filename, "wb");
						if (file)
						{
							fwrite(realOutBuf, 1, pReplyAlbum->stBaseInfo.iPicSize, file);
							fclose(file);

						}

						CImage image;
						CWnd* pWnd;
						pWnd = GetDlgItem(IDC_PICTURE);
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

						SetDlgItemText(IDC_STATIC_PICTURE_PATH, filename);


						CString temp;
						temp.Format(_T("%s"), Utf8ToGbk(pReplyAlbum->stBaseInfo.szName).c_str());
						SetDlgItemText(IDC_EDIT_FACE_NAME, temp);

						temp.Empty();
						temp.Format(_T("%d"), pReplyAlbum->stBaseInfo.iAge);
						SetDlgItemText(IDC_EDIT_FACE_birthday, temp);

						temp.Empty();
						temp.Format(_T("%s"), pReplyAlbum->stBaseInfo.szIdentifyNum);
						SetDlgItemText(IDC_EDIT_FACE_certificateNum, temp);

						break;
					}
					else
					{
						realOutBuf += sizeof(NET_SDK_IVE_FACE_MATCH_QUERY_ALBUM_REPLY_INFO);
						realOutBuf += pReplyAlbum->stBaseInfo.iPicSize;
					}
				}
				delete[]rrOut;

			}
		}

		if(outBuf)
		{
			delete []outBuf;
			outBuf = NULL;
		}

	}
	else if(m_deviceinfo->deviceType == NET_SDK_NVR)
	{
		pos = m_faceInfoList.GetHeadPosition();
	while (index > 0)
	{
		m_faceInfoList.GetNext(pos);
		index--;
	}

	NET_SDK_FACE_INFO_LIST_ITEM &item = m_faceInfoList.GetNext(pos);
	memcpy(&m_curFaceItem, &item, sizeof(NET_SDK_FACE_INFO_LIST_ITEM));
	CString temp;
	temp.Format(_T("%s"), Utf8ToGbk(item.name).c_str());
	SetDlgItemText(IDC_EDIT_FACE_NAME, temp);

	temp.Empty();
	temp.Format(_T("%d"), item.birthday);
	SetDlgItemText(IDC_EDIT_FACE_birthday, temp);

	temp.Empty();
	temp.Format(_T("%s"), item.nativePlace);
	SetDlgItemText(IDC_EDIT_FACE_nativePlace, temp);
	
	temp.Empty();
	temp.Format(_T("%s"), item.certificateNum);
	SetDlgItemText(IDC_EDIT_FACE_certificateNum, temp);

	temp.Empty();
	temp.Format(_T("%s"), item.mobile);
	SetDlgItemText(IDC_EDIT_FACE_mobile, temp);

	temp.Empty();
	temp.Format(_T("%s"), item.number);
	SetDlgItemText(IDC_EDIT_FACE_number, temp);

	 //NET_SDK_GET_FACE_INFO_IMG,//requestFacePersonnalInfoImage in:NET_SDK_FACE_INFO_IMG_GET out:NET_SDK_FACE_INFO_IMG_DATA
	char *tempBuf = new char[100*1024];
	memset(tempBuf, 0, 100*1024);
	DWORD lpBytesReturned = 0;

	NET_SDK_FACE_INFO_IMG_GET sFACE_INFO_IMG_GET;
	sFACE_INFO_IMG_GET.index = 1;
	sFACE_INFO_IMG_GET.itemId = item.itemId;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_INFO_IMG, &sFACE_INFO_IMG_GET, sizeof(NET_SDK_FACE_INFO_IMG_GET), tempBuf,  100*1024, &lpBytesReturned);
	if(ret)
	{
		NET_SDK_FACE_INFO_IMG_DATA *pFACE_INFO_IMG_DATA = (NET_SDK_FACE_INFO_IMG_DATA*)tempBuf;
		TRACE("imgLen =%d ,  lpBytesReturned=%d \n", pFACE_INFO_IMG_DATA->imgLen,  lpBytesReturned);
		char filename[20] = {0};
		sprintf(filename, "./face_%d.jpg", item.itemId);
		FILE *file = fopen(filename, "wb");
		if (file)
		{
			fwrite(pFACE_INFO_IMG_DATA->imgData, 1, pFACE_INFO_IMG_DATA->imgLen, file);
			fclose(file);
			/*temp.Empty();
			temp.Format(_T("save img %s"), filename);
			AfxMessageBox(temp);*/
		}

		CImage image;
		CWnd* pWnd;
		pWnd = GetDlgItem(IDC_PICTURE);
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

		SetDlgItemText(IDC_STATIC_PICTURE_PATH, filename);

	}
	else
	{
		AfxMessageBox(" GET_FACE_INFO_IMG faild!");
	}
	delete[]tempBuf;
	tempBuf = NULL;
	}


	

}

void CFaceMatchlDlg::OnNMDblclkListChSnap(NMHDR *pNMHDR, LRESULT *pResult)
{
	int index = m_chSnapFaceListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (index < 0)
	{
		return ;
	}
	POSITION pos = NULL;
	pos = m_chSnapFaceList.GetHeadPosition();
	while (index > 0)
	{
		m_chSnapFaceList.GetNext(pos);
		index--;
	}

	NET_SDK_FACE_IMG_INFO_CH &item = m_chSnapFaceList.GetNext(pos);
	item.isPanorama = 1;
	char *tempBuf = new char[100*1024];
	memset(tempBuf, 0, 100*1024);
	DWORD lpBytesReturned = 0;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SEARCH_CH_SNAP_FACE_IMG, &item, sizeof(NET_SDK_FACE_IMG_INFO_CH), tempBuf,  100*1024, &lpBytesReturned);
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
	}
	delete[]tempBuf;
	tempBuf = NULL;

}
string CFaceMatchlDlg::GbkToUtf8(const char *src_str)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
	wchar_t* wstr = new wchar_t[len + 1];
	memset(wstr, 0, len + 1);
	MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
	char* str = new char[len + 1];
	memset(str, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
	string strTemp = str;
	if (wstr) delete[] wstr;
	if (str) delete[] str;
	return strTemp;
}
string CFaceMatchlDlg::Utf8ToGbk(const char *src_str)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	string strTemp(szGBK);
	if (wszGBK) delete[] wszGBK;
	if (szGBK) delete[] szGBK;
	return strTemp;
}




void CFaceMatchlDlg::OnBnClickedCopyFaceButton()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_deviceinfo->deviceType == NET_SDK_NVR)
	{
		int index = m_faceGroupListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		if (index < 0)
		{
			AfxMessageBox(" select GroupList first !!");
			return ;
		}
		POSITION pos = NULL;
		pos = m_faceGroupList.GetHeadPosition();
		while (index > 0)
		{
			m_faceGroupList.GetNext(pos);
			index--;
		}

		NET_SDK_FACE_INFO_GROUP_ITEM &item = m_faceGroupList.GetNext(pos);//获取目标库


		index = m_faceInfoListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		if (index < 0)
		{
			return ;
		}
		pos = NULL;
		pos = m_faceInfoList.GetHeadPosition();
		while (index > 0)
		{
			m_faceInfoList.GetNext(pos);
			index--;
		}
		NET_SDK_FACE_INFO_LIST_ITEM &pFaceInfoListItem = m_faceInfoList.GetNext(pos);

		unsigned int groupId = item.groupId;
		char groupGuid[48] = {0};
		strcpy(groupGuid, (char*)item.guid);

		char *tempBuf = new char[100*1024];
		memset(tempBuf, 0, 100*1024);
		NET_SDK_FACE_INFO_COPY *pFACE_INFO_COPY = (NET_SDK_FACE_INFO_COPY*)tempBuf;
		pFACE_INFO_COPY->birthday = pFaceInfoListItem.birthday;
		memcpy(pFACE_INFO_COPY->certificateNum, pFaceInfoListItem.certificateNum, DD_MAX_CERTIFICATE_NUM);
		pFACE_INFO_COPY->certificateType = pFaceInfoListItem.certificateType;
		strcpy((char *)(pFACE_INFO_COPY->guid), groupGuid);

		pFACE_INFO_COPY->itemId = pFaceInfoListItem.itemId;
		strcpy(pFACE_INFO_COPY->mobile, pFaceInfoListItem.mobile);
		strcpy(pFACE_INFO_COPY->name, pFaceInfoListItem.name);
		strcpy(pFACE_INFO_COPY->nativePlace,pFaceInfoListItem.nativePlace);
		strcpy(pFACE_INFO_COPY->number, pFaceInfoListItem.number);
		pFACE_INFO_COPY->sex = pFaceInfoListItem.sex;
		
		DWORD lpBytesReturned = 0;
		unsigned int faceId = 0;
		BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_COPY_FACE_INFO, tempBuf, sizeof(NET_SDK_FACE_INFO_COPY), &faceId,  sizeof(unsigned int), &lpBytesReturned);
		if(ret)
		{
			TRACE("NET_SDK_COPY_FACE_INFO success faceId=%d \n", faceId);
		}
		else
		{
			AfxMessageBox(" NET_SDK_COPY_FACE_INFO faild!");
		}
		delete[] tempBuf;
		tempBuf = NULL;
	}			
}


void CFaceMatchlDlg::OnBnClickedBtnExportBrowse()
{
	BROWSEINFO bi;
	char name[MAX_PATH];
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle = "选择文件夹目录";
	bi.ulFlags = BIF_RETURNFSANCESTORS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (idl == NULL)
		return;
	CString strDirectoryPath;
	SHGetPathFromIDList(idl, strDirectoryPath.GetBuffer(MAX_PATH));
	strDirectoryPath.ReleaseBuffer();

	if (strDirectoryPath.IsEmpty())
		return;
	if (strDirectoryPath.Right(1) != "\\")
		strDirectoryPath += "\\";
	SetDlgItemText(IDC_EDIT_EXPORT_PATH, strDirectoryPath);
}


void CFaceMatchlDlg::OnBnClickedBtnExport()
{
	UpdateData();
	USES_CONVERSION;
	CString path;
	GetDlgItemText(IDC_EDIT_EXPORT_PATH, path);
	if (path.IsEmpty())
	{
		AfxMessageBox(STR_SELECT_PATH);
		return ;
	}
	m_faceInfoList.RemoveAll();
	// TODO: 在此添加控件通知处理程序代码
	int index = m_faceGroupListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (index < 0)
	{
		return ;
	}
	POSITION pos = NULL;
	pos = m_faceGroupList.GetHeadPosition();
	while (index > 0)
	{
		m_faceGroupList.GetNext(pos);

		index--;
	}

	NET_SDK_FACE_INFO_GROUP_ITEM &item = m_faceGroupList.GetNext(pos);

	NET_SDK_FACE_INFO_LIST_GET sFACE_INFO_LIST_GET;
	memset(&sFACE_INFO_LIST_GET, 0, sizeof(NET_SDK_FACE_INFO_LIST_GET));
	sFACE_INFO_LIST_GET.groupId = item.groupId;
	sFACE_INFO_LIST_GET.pageIndex = 1;
	sFACE_INFO_LIST_GET.pageSize = 10;

	char *tempBuf = new char[1000*1024];
	memset(tempBuf, 0, 1000*1024);
	DWORD lpBytesReturned = 0;

	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_INFO_LIST, &sFACE_INFO_LIST_GET, sizeof(NET_SDK_FACE_INFO_LIST_GET), tempBuf,  1000*1024, &lpBytesReturned);
	if(ret)
	{
		NET_SDK_FACE_INFO_LIST *pFACE_INFO_LIST = (NET_SDK_FACE_INFO_LIST*)tempBuf;
		TRACE("listNum =%d , pFACE_INFO_LIST->totalNum =%d lpBytesReturned=%d \n", pFACE_INFO_LIST->listNum, pFACE_INFO_LIST->totalNum, lpBytesReturned);

		NET_SDK_FACE_INFO_LIST_ITEM *pFACE_INFO_LIST_ITEM = pFACE_INFO_LIST->pFaceInfoListItem;
		for(int i=0; i< pFACE_INFO_LIST->listNum; i++)
		{
			m_faceInfoList.AddTail(pFACE_INFO_LIST_ITEM[i]);
		}
		if(pFACE_INFO_LIST->totalNum / 10 > 0)
		{
			int pages = pFACE_INFO_LIST->totalNum / 10;
			int restNum = pFACE_INFO_LIST->totalNum % 10;
			if(restNum > 0)
				pages++;
			for(int j = 2; j <= pages; j++)
			{
				NET_SDK_FACE_INFO_LIST_GET sFilg;
				memset(&sFilg, 0, sizeof(NET_SDK_FACE_INFO_LIST_GET));
				sFilg.groupId = item.groupId;
				sFilg.pageIndex = j;
				sFilg.pageSize = 10;

				memset(tempBuf, 0, 1000*1024);
				lpBytesReturned = 0;
				ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_INFO_LIST, &sFilg, sizeof(NET_SDK_FACE_INFO_LIST_GET), tempBuf,  1000*1024, &lpBytesReturned);
				if(ret)
				{
					NET_SDK_FACE_INFO_LIST *pFil = (NET_SDK_FACE_INFO_LIST*)tempBuf;
					NET_SDK_FACE_INFO_LIST_ITEM *pFili = pFil->pFaceInfoListItem;
					for(int i=0; i< pFil->listNum; i++)
					{						
						m_faceInfoList.AddTail(pFili[i]);
					}
				}				
			}
		}

	}
	else
	{
		AfxMessageBox(" faild!");
	}
	

	pos = NULL;
	pos = m_faceInfoList.GetHeadPosition();
	m_ProgressExport.SetRange(0, m_faceInfoList.GetCount());
	int k = 1;
	int iSuccessed = 0;int iFailed = 0;
	while (pos)
	{
		NET_SDK_FACE_INFO_LIST_ITEM &item = m_faceInfoList.GetAt(pos);


		memset(tempBuf, 0, 1000*1024);
		lpBytesReturned = 0;
		NET_SDK_FACE_INFO_IMG_GET sFACE_INFO_IMG_GET;
		sFACE_INFO_IMG_GET.index = 1;
		sFACE_INFO_IMG_GET.itemId = item.itemId;
		BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_GET_FACE_INFO_IMG, &sFACE_INFO_IMG_GET, sizeof(NET_SDK_FACE_INFO_IMG_GET), tempBuf,  1000*1024, &lpBytesReturned);
		if(ret)
		{
			NET_SDK_FACE_INFO_IMG_DATA *pFACE_INFO_IMG_DATA = (NET_SDK_FACE_INFO_IMG_DATA*)tempBuf;
			TRACE("imgLen =%d ,  lpBytesReturned=%d \n", pFACE_INFO_IMG_DATA->imgLen,  lpBytesReturned);
			char filename[20] = {0};
			sprintf(filename, "%s/face_%d.jpg", path.GetBuffer(), item.itemId);
			path.ReleaseBuffer();
			FILE *file = fopen(filename, "wb");
			if (file)
			{
				fwrite(pFACE_INFO_IMG_DATA->imgData, 1, pFACE_INFO_IMG_DATA->imgLen, file);
				fclose(file);
			}
			iSuccessed++;
		}
		else
		{
			iFailed++;
		}
		CString temp;
		temp.Format(_T("Export successed:%d,failed:%d/total:%d"), iSuccessed,iFailed,m_faceInfoList.GetCount());
		SetDlgItemText(IDC_STATIC_TOTAL,temp);

		m_faceGroupList.GetNext(pos);
		m_ProgressExport.SetPos(k);
		k++;
		Sleep(100);
	}
	delete[]tempBuf;
	tempBuf = NULL;
}


void CFaceMatchlDlg::OnBnClickedBtnImport()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	USES_CONVERSION;
	CString path;
	GetDlgItemText(IDC_EDIT_EXPORT_PATH, path);
	if (path.IsEmpty())
	{
		AfxMessageBox(STR_SELECT_PATH);
		return ;
	}
	if(m_deviceinfo->deviceType == NET_SDK_NVR)
	{
		int index = m_faceGroupListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		if (index < 0)
		{
			AfxMessageBox(" select GroupList first !!");
			return ;
		}
		POSITION pos = NULL;
		pos = m_faceGroupList.GetHeadPosition();
		while (index > 0)
		{
			m_faceGroupList.GetNext(pos);
			index--;
		}

		NET_SDK_FACE_INFO_GROUP_ITEM &item = m_faceGroupList.GetNext(pos);

		char *tempBuf = new char[1024*1024];
		
		int num = 0;
		//文件句柄  
		long hFile = 0;
		//文件信息  
		struct _finddata_t fileinfo;
		CString p = path + _T("\*.jpg");
		if ((hFile = _findfirst(p.GetBuffer(), &fileinfo)) != -1) 
		{
			do 
			{
				num ++;				
			} while (_findnext(hFile, &fileinfo) == 0); 
			_findclose(hFile);
		}
		p.ReleaseBuffer();

		m_ProgressExport.SetRange(0, num);
		int iProcess = 1;
		int iSuccessed = 0;
		int iFailed = 0;
		if ((hFile = _findfirst(p.GetBuffer(), &fileinfo)) != -1) 
		{
			do 
			{
				memset(tempBuf, 0, 1024*1024);
				NET_SDK_FACE_INFO_ADD *pFACE_INFO_ADD = (NET_SDK_FACE_INFO_ADD*)tempBuf;

				strcpy(pFACE_INFO_ADD->sFaceInfoItem.name, fileinfo.name);

				strcpy(pFACE_INFO_ADD->sFaceInfoItem.nativePlace, "abc");
				strcpy(pFACE_INFO_ADD->sFaceInfoItem.certificateNum, "123");
				strcpy(pFACE_INFO_ADD->sFaceInfoItem.mobile, "123");
				strcpy(pFACE_INFO_ADD->sFaceInfoItem.number, "123");
				pFACE_INFO_ADD->sFaceInfoItem.birthday = 123;
				strcpy((char*)pFACE_INFO_ADD->sFaceInfoItem.groups[0].guid, (char*)item.guid);


				pFACE_INFO_ADD->imgNum = 1;
				pFACE_INFO_ADD->haveImgData = 1;
				pFACE_INFO_ADD->imgData = (unsigned char*)tempBuf + sizeof(NET_SDK_FACE_INFO_ADD);
				CString fullname = path + _T("\\") + _T(fileinfo.name);
				FILE *fp = fopen(fullname.GetBuffer(), "rb");
				fullname.ReleaseBuffer();
				int length = 0;
				if(fp)
				{
					fseek(fp,0,SEEK_END);
					length=ftell(fp);
					fseek(fp,0,SEEK_SET);
					int readLen = fread((char *)pFACE_INFO_ADD->imgData , length, 1, fp );
					fclose(fp);
				}
				pFACE_INFO_ADD->imgWidth = 440;
				pFACE_INFO_ADD->imgHeight = 620;
				pFACE_INFO_ADD->imgLen = length;
				DWORD lpBytesReturned = 0;
				int len = sizeof(NET_SDK_FACE_INFO_ADD);

				unsigned int faceId = 0;
				BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_ADD_FACE_INFO, tempBuf, sizeof(NET_SDK_FACE_INFO_ADD)+length, &faceId,  sizeof(unsigned int), &lpBytesReturned);
				if(ret)
				{
					TRACE("NET_SDK_ADD_FACE_INFO success faceId=%d \n", faceId);
					iSuccessed++;
				}
				else
				{
					iFailed ++;
				}
				CString temp;
				temp.Format(_T("import successed:%d,failed:%d/total:%d"), iSuccessed,iFailed,num);
				SetDlgItemText(IDC_STATIC_TOTAL,temp);
				m_ProgressExport.SetPos(iProcess);
				iProcess ++;
			} while (_findnext(hFile, &fileinfo) == 0); 
			_findclose(hFile);
		}
		p.ReleaseBuffer();



		delete[] tempBuf;
		tempBuf = NULL;
	}
}


void CFaceMatchlDlg::OnStnClickedPicture()
{
	// TODO: 在此添加控件通知处理程序代码
	CFileDialog dlg(TRUE, _T("jpg"), NULL, OFN_HIDEREADONLY, _T("jpg files(*.jpg)|*.jpg||"));
	if (dlg.DoModal() == IDOK)
	{
		//SetDlgItemText(IDC_EDIT_IMPORT_PATH, dlg.GetPathName());
		CImage image;
		CWnd* pWnd;
		pWnd = GetDlgItem(IDC_PICTURE);
		CDC* pDC = pWnd->GetDC();
		HDC hDC = pDC->m_hDC;
		CRect rect_frame;
		pWnd->GetClientRect(&rect_frame);
		image.Load(dlg.GetPathName().GetBuffer());
		dlg.GetPathName().ReleaseBuffer();
		::SetStretchBltMode(hDC,HALFTONE);
		::SetBrushOrgEx(hDC,0,0,NULL);
		image.Draw(hDC,rect_frame);
		ReleaseDC(pDC);//释放picture控件的DC
		image.Destroy();

		SetDlgItemText(IDC_STATIC_PICTURE_PATH,dlg.GetPathName());
	}
}


void CFaceMatchlDlg::OnBnClickedSearchImgByImgV2Button()
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
		int index = m_chSnapFaceListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		if (index < 0)
		{
			return ;
		}
		POSITION pos = NULL;
		pos = m_chSnapFaceList.GetHeadPosition();
		while (index > 0)
		{
			m_chSnapFaceList.GetNext(pos);
			index--;
		}
		NET_SDK_FACE_IMG_INFO_CH &item = m_chSnapFaceList.GetNext(pos);	
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
		pSEARCH_IMAGE_BY_IMAGE.imgSourceType = FROM_SNAP;

		pSEARCH_IMAGE_BY_IMAGE.sfaceImgsNum = 1;
		pSEARCH_IMAGE_BY_IMAGE.sfaceImgs = new NET_SDK_FACE_IMG_INFO_CH[1];
		pSEARCH_IMAGE_BY_IMAGE.sfaceImgs[0].chl = item.chl;
		pSEARCH_IMAGE_BY_IMAGE.sfaceImgs[0].snapImgId = item.snapImgId;
		pSEARCH_IMAGE_BY_IMAGE.sfaceImgs[0].frameTime = item.frameTime;
	}
	else
	{
		int index = m_faceInfoListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		if (index < 0)
		{
			return ;
		}
		POSITION pos = NULL;
		pos = m_faceInfoList.GetHeadPosition();
		while (index > 0)
		{
			m_faceInfoList.GetNext(pos);
			index--;
		}
		NET_SDK_FACE_INFO_LIST_ITEM &item = m_faceInfoList.GetNext(pos);	
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
		pSEARCH_IMAGE_BY_IMAGE.imgSourceType = FROM_ALBUM;

		pSEARCH_IMAGE_BY_IMAGE.imgNum = 1;
		pSEARCH_IMAGE_BY_IMAGE.imgId = new unsigned int[1];
		pSEARCH_IMAGE_BY_IMAGE.imgId[0] = item.itemId;

	}

	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SEARCH_IMAGE_BY_IMG_V2,&pSEARCH_IMAGE_BY_IMAGE, 
		sizeof(NET_SDK_SEARCH_IMAGE_BY_IMAGE_V2), tempBuf,  100*1024, &lpBytesReturned);
	if(pSEARCH_IMAGE_BY_IMAGE.imgId)
	{
		delete[] pSEARCH_IMAGE_BY_IMAGE.imgId;
		pSEARCH_IMAGE_BY_IMAGE.imgId = NULL;
	}
	if(pSEARCH_IMAGE_BY_IMAGE.sfaceImgs)
	{
		delete[] pSEARCH_IMAGE_BY_IMAGE.sfaceImgs;
		pSEARCH_IMAGE_BY_IMAGE.sfaceImgs = NULL;
	}
		
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
	tempBuf = NULL;
}

void CFaceMatchlDlg::OnBnClickedGetChSnapListV2Button()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData();
	m_chSnapFaceList.RemoveAll();
	m_chSnapFaceListCtrl.DeleteAllItems();


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
	sCH_SNAP_FACE_IMG_LIST_SEARCH.eventType = BY_ALL;
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
			m_chSnapFaceList.AddTail(sFACE_IMG_INFO_CH);

			int n = m_chSnapFaceListCtrl.GetItemCount();
			m_chSnapFaceListCtrl.InsertItem(n, _T(""));
			CString temp;
			temp.Format(_T("%d"), i+1);
			m_chSnapFaceListCtrl.SetItemText(n , 0, temp);

			temp.Format(_T("%d"), sFACE_IMG_INFO_CH.snapImgId);
			m_chSnapFaceListCtrl.SetItemText(n , 1, temp);

			temp.Format(_T("%d"), sFACE_IMG_INFO_CH.chl);
			m_chSnapFaceListCtrl.SetItemText(n, 2, temp);
			temp.Format(_T("%d-%d-%d %d:%d:%d:%d"), sFACE_IMG_INFO_CH.frameTime.year, sFACE_IMG_INFO_CH.frameTime.month, sFACE_IMG_INFO_CH.frameTime.mday,
				sFACE_IMG_INFO_CH.frameTime.hour, sFACE_IMG_INFO_CH.frameTime.minute, sFACE_IMG_INFO_CH.frameTime.second, sFACE_IMG_INFO_CH.frameTime.nMicrosecond);
			m_chSnapFaceListCtrl.SetItemText(n, 3, temp);
		}
	}
	delete[]tempBuf;
	tempBuf = NULL;
}


void CFaceMatchlDlg::OnNMDblclkListSearchImageResult(NMHDR *pNMHDR, LRESULT *pResult)
{
	int index = m_searchResultListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (index < 0)
	{
		return ;
	}
	POSITION pos = NULL;
	pos = m_searchResultList.GetHeadPosition();
	while (index > 0)
	{
		m_searchResultList.GetNext(pos);
		index--;
	}

	NET_SDK_FACE_IMG_INFO_CH &item = m_searchResultList.GetNext(pos);
	char *tempBuf = new char[100*1024];
	memset(tempBuf, 0, 100*1024);
	DWORD lpBytesReturned = 0;
	BOOL ret = NET_SDK_FaceMatchOperate(m_userID, NET_SDK_SEARCH_CH_SNAP_FACE_IMG, &item, sizeof(NET_SDK_FACE_IMG_INFO_CH), tempBuf,  100*1024, &lpBytesReturned);
	if(ret)
	{
		NET_SDK_FACE_INFO_IMG_DATA *pFACE_INFO_IMG_DATA = (NET_SDK_FACE_INFO_IMG_DATA*)tempBuf;
		TRACE("imgLen =%d ,  lpBytesReturned=%d \n", pFACE_INFO_IMG_DATA->imgLen,  lpBytesReturned);

		char filename[20] = {0};
		sprintf(filename, "./searchImage_%d_%d.jpg", item.chl, item.snapImgId);
		FILE *file = fopen(filename, "wb");
		if (file)
		{
			fwrite(pFACE_INFO_IMG_DATA->imgData, 1, pFACE_INFO_IMG_DATA->imgLen, file);
			fclose(file);
			CString temp;
			temp.Format(_T("save img %s"), filename);
			AfxMessageBox(temp);
		}
	}
	delete[]tempBuf;
	tempBuf = NULL;
}


void CFaceMatchlDlg::OnNMClickListChSnap(NMHDR *pNMHDR, LRESULT *pResult)
{
	int index = m_chSnapFaceListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
	if (index < 0)
	{
		return ;
	}
	m_imgSorce = FROM_SNAP;
	return;


}


void CFaceMatchlDlg::OnNMClickListFace(NMHDR *pNMHDR, LRESULT *pResult)
{
	if(m_deviceinfo->deviceType == NET_SDK_IPCAMERA)
	{
		int index = m_faceInfoListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		if (index < 0)
		{
			return ;
		}
		POSITION pos = NULL;
		pos = m_ipcFaceInfoList.GetHeadPosition();
		while (index > 0)
		{
			m_ipcFaceInfoList.GetNext(pos);
			index--;
		}

		NET_SDK_IVE_FACE_MATCH_ADD_ALBUM_INFO &item = m_ipcFaceInfoList.GetNext(pos);
		memcpy(&m_curIpcFaceItem, &item, sizeof(NET_SDK_IVE_FACE_MATCH_ADD_ALBUM_INFO));

	}
	else
	{
		int index = m_faceInfoListCtrl.GetNextItem( -1, LVNI_ALL | LVNI_SELECTED);
		if (index < 0)
		{
			return ;
		}
		m_imgSorce = FROM_ALBUM;
		return;
	}
	



	
}
