#pragma once


// CDialogGateServerConfig �Ի���

class CDialogGateServerConfig : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogGateServerConfig)

public:
	CDialogGateServerConfig(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDialogGateServerConfig();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_GATE_SERVER_CONFIG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_IdStart;
	CString m_Ip;
	int m_PortStart;
	int m_iGaoFangType;
	int m_nServerCount;
};
