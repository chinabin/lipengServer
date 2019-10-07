#ifndef _RUNTIME_INFO_MSG_H_
#define _RUNTIME_INFO_MSG_H_

#include "LBase.h"
#include "LSingleton.h"
#include "LMsg.h"

class CRuntimeInfoMsg : public LSingleton<CRuntimeInfoMsg>
{
public:
	CRuntimeInfoMsg();
	~CRuntimeInfoMsg();
public:
	virtual bool Init();
	virtual bool Final();
public:
	void setHorseInfoAndNotify(const Lstring& strInfo);
	Lstring getHorseInfo();

	void setBuyInfo(const Lstring& strInfo);
	Lstring getBuyInfo(int iUserId);

	void addNotice(const Notice& notice);
	std::vector<Notice> getNotice();

	void setHide(Lint iHide);
	Lint getHide();
public:
	void changeOnlineNum(Lint iGateId, Lint iUserId, bool bInc);
	Lint getOnlineNum(Lint iGateId);
	void clearOnlineNum(Lint iGateId);
public:
	void	setFreeTimeAndNotify(Lstring& strFreeTime);
	Lstring getFreeTime();
	bool	isFree( Lint gametype );	// gametype=-1��ʾ�Ƿ�ȫ�����
	bool	updateFree();	// ������� ������m_strFreeTime ����ֵ��ʾm_strFreeTime���޸�

private:
	//��̨�ƹ�����
	boost::mutex m_mutexForBackground;

	Lstring	m_strHorseInfo;		//�����
	Lstring	m_strBuyInfo;		//������ʾ��
	std::vector<Lstring>	m_vecBuyInfo;	//������Ϣ
	std::vector<Notice>		m_vecNotice;	//֪ͨ
	Lint	m_iHide;
private:
	boost::mutex m_mutexForOnlineNum;
	std::map<Lint, std::map<int , int> > m_mapOnlinNumOnGate;	//gate�ϵ���������
private:
	//�������ڲ�

	// �������
	boost::recursive_mutex m_mutexForFree;
	Lstring m_strFreeTime;			//������� json��ʽ �����Ǻ�̨���͹�����json ֻ�����Ѿ�������json����
	struct BeginEnd
	{
		BeginEnd()
		{
			m_bstart = false;
		}
		time_t m_begin;
		time_t m_end;

		// ���ڸ���m_strFreeTime
		bool m_bstart;	// �Ƿ��Ѿ�����
		Json::Value m_config;
	};
	std::map<Lint,BeginEnd> m_freeSet;	// -1��ʾȫ����ѵ����� ������ʾ��ӦGameType���
};

#define gRuntimeInfoMsg  CRuntimeInfoMsg::Instance()

#endif