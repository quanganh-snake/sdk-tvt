#pragma once
#include "afxcmn.h"
#include "DVR_NET_SDK.h"
#include "afxdtctl.h"
#include "gridctrl.h"
#include <map>
#include <string>

#define N9000_CONFIG
// CToolDlg 对话框
using namespace std;
class CFaceMatchlDlg : public CDialog
{
	DECLARE_DYNAMIC(CFaceMatchlDlg)

public:
	CFaceMatchlDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFaceMatchlDlg();

// 对话框数据
	enum { IDD = IDD_FACE_MATCH_DIALOG };

protected:
	
	NET_SDK_DEVICEINFO	*m_deviceinfo;
	LONG				m_userID;
	CWnd*				m_mainDlg;
	BOOL				m_support;//是否支持人脸
	int					m_chnn;
	CTime					m_searchDate;
	CString					m_editGroupName;
	CString					m_editFaceName;
	CString					m_editFaceBirthday;
	CString					m_editFaceNativePlace;
	CString					m_editFaceCertificateNum;
	CString					m_editFaceMobile;
	CString					m_editFaceNumber;

	NET_SDK_FACE_INFO_LIST_ITEM  m_curFaceItem;
	char 					*m_faceAlarmTrigger;

	CList<NET_SDK_FACE_INFO_GROUP_ITEM> m_faceGroupList;
	CList<NET_SDK_FACE_INFO_LIST_ITEM> m_faceInfoList;
	CList<NET_SDK_FACE_IMG_INFO_CH> m_chSnapFaceList;
	CList<NET_SDK_FACE_IMG_INFO_CH> m_searchResultList;

	CList<NET_SDK_IVE_FACE_MATCH_ADD_ALBUM_INFO> m_ipcFaceInfoList;
	NET_SDK_IVE_FACE_MATCH_ADD_ALBUM_INFO m_curIpcFaceItem;


	CListCtrl				m_faceGroupListCtrl;
	CListCtrl				m_faceInfoListCtrl;
	CListCtrl				m_chSnapFaceListCtrl;
	CListCtrl				m_searchResultListCtrl;

	void UpdateUI();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	void test();
	DECLARE_MESSAGE_MAP()
public:
	
	void setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid);
	BOOL IsSupportFace();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedGetGroups();
	afx_msg void OnBnClickedAddGroups();
	afx_msg void OnBnClickedEditGroups();
	afx_msg void OnBnClickedDelGroups();
	afx_msg void OnBnClickedAddFace();
	afx_msg void OnBnClickedEditFace();
	afx_msg void OnBnClickedDelFace();
	afx_msg void OnBnClickeGetCHSnapFaceList();
	afx_msg void OnBnClickeSearchImgByImg();
	afx_msg void OnBnClickeGetAlarmTrigger();
	afx_msg void OnBnClickeSetAlarmTrigger();

	afx_msg void OnNMDblclkListGroups(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListFace(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListChSnap(NMHDR *pNMHDR, LRESULT *pResult);
	
	virtual BOOL OnInitDialog();
	
	void setMainDlg( CWnd* dlg );
	string GbkToUtf8(const char *src_str);
	string Utf8ToGbk(const char *src_str);
	afx_msg void OnBnClickedCopyFaceButton();
	afx_msg void OnBnClickedBtnExportBrowse();
	afx_msg void OnBnClickedBtnExport();
	CProgressCtrl m_ProgressExport;
	afx_msg void OnBnClickedBtnImport();
	afx_msg void OnStnClickedPicture();
	afx_msg void OnBnClickedSearchImgByImgV2Button();
	afx_msg void OnBnClickedGetChSnapListV2Button();
	
	afx_msg void OnNMDblclkListSearchImageResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListChSnap(NMHDR *pNMHDR, LRESULT *pResult);
private:
	unsigned int m_imgSorce;//以图搜图的图片来源
public:
	afx_msg void OnNMClickListFace(NMHDR *pNMHDR, LRESULT *pResult);
};
