#pragma once
#include "afxcmn.h"
#include "DVR_NET_SDK.h"
#include "afxdtctl.h"
#include "gridctrl.h"
#include <map>
#include <string>
#include "SDKDEMO.h"

// CSmartDlg 对话框

class CSmartDlg : public CDialog
{
	DECLARE_DYNAMIC(CSmartDlg)

public:
	CSmartDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSmartDlg();

	void setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid);
	BOOL IsSupportFace();
// 对话框数据
	enum { IDD = IDD_SMART_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
	NET_SDK_DEVICEINFO	*m_deviceinfo;
	LONG				m_userID;
	BOOL				m_support;//是否支持人脸
	CTime					m_searchDate;

	CList<NET_SDK_FACE_IMG_INFO_CH> m_imageList;
	CList<NET_SDK_FACE_IMG_INFO_CH> m_searchResultList;
public:
	afx_msg void OnBnClickedSearchImgByImgV3Button();
	afx_msg void OnBnClickedGetChSnapListV3Button();
	virtual BOOL OnInitDialog();
	CListCtrl m_imageListCtrl;
	afx_msg void OnBnClickedRadioByAll();
private:
	int eventType;
	int m_imgSorce;
	int isPanorama;//1代表完整图片，0代表抓拍图
public:
	CListCtrl m_searchResultListCtrl;
	afx_msg void OnBnClickedBtnViewImg();
	afx_msg void OnBnClickedRadioSnap();
};
