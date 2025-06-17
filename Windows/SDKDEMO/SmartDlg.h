#pragma once
#include "afxcmn.h"
#include "DVR_NET_SDK.h"
#include "afxdtctl.h"
#include "gridctrl.h"
#include <map>
#include <string>
#include "SDKDEMO.h"

// CSmartDlg �Ի���

class CSmartDlg : public CDialog
{
	DECLARE_DYNAMIC(CSmartDlg)

public:
	CSmartDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSmartDlg();

	void setDeviceInfo(NET_SDK_DEVICEINFO *di, LONG userid);
	BOOL IsSupportFace();
// �Ի�������
	enum { IDD = IDD_SMART_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
	NET_SDK_DEVICEINFO	*m_deviceinfo;
	LONG				m_userID;
	BOOL				m_support;//�Ƿ�֧������
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
	int isPanorama;//1��������ͼƬ��0����ץ��ͼ
public:
	CListCtrl m_searchResultListCtrl;
	afx_msg void OnBnClickedBtnViewImg();
	afx_msg void OnBnClickedRadioSnap();
};
