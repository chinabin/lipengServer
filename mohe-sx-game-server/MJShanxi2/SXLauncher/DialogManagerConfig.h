#pragma once


// CDialogManagerConfig �Ի���

class CDialogManagerConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogManagerConfig)

public:
	CDialogManagerConfig(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogManagerConfig();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_MANAGER_CONFIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_Id;
	CString m_Ip;
	CString m_Name;
	int m_Port;
	BOOL m_bClub;
};
