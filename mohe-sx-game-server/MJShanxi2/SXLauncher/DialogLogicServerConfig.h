#pragma once


// CDialogLogicServerConfig �Ի���

class CDialogLogicServerConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogLogicServerConfig)

public:
	CDialogLogicServerConfig(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogLogicServerConfig();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LOGIC_SERVER };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nServerCount;
	int m_nServerIdStart;
	CString m_strServerIP;
	int m_nServerPortStart;
	int m_nLogicServerType;
};
