#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "DbServer.h"
#include "LMemoryRecycle.h"

class ShipInfo
{
public:
	ShipInfo(Lint inviter,Lint count)
	{
		m_inviter = inviter;
		m_count = count;
	}
	Lint m_inviter;
	Lint m_count;
private:
	ShipInfo();
};

class Work:public LRunnable,public LSingleton<Work>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void    Push(void* msg);
	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();
public:
	void	Tick(LTime& cur);
	LTime&	GetCurTime();
public:
	void	HanderMsg(LMsg* msg);

	//����ͻ�������������Ϣ
	void	HanderClientIn(LMsgIn* msg);

	//����ͻ��˵��ߵ���Ϣ 
	void	HanderUserKick(LMsgKick* msg);
protected:
	void	HanderLogicManagerLogin(LMsgLMG2LdbLogin* msg);
	void	HanderLogicServerLogin(LMsgL2LdbLogin* msg);
	void	HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg);
public:
	inline bool GetDBMode() { return m_bDBMode; }
	//��������Ŀǰ��û�����ߣ��Ժ���ȥ��
	void	HanderGetInvitingInfo(LMsgC2SGetInvitingInfo* msg);
	void	HanderBindInviterMsg(LMsgLM_2_LDBBindInviter* msg);
	void	HanderReqInviterInfo(LMsgLM_2_LDB_ReqInfo* msg);

	// ����
	void    HanderLogicSetActivity(LMsgCE2LSetActivity* msg);

	void    SetDrawActive(const Lstring& strSet,int id);
	void    SetShareActive(const Lstring& strSet,int id);

	// 
	void	SendToLogicManager(LMsg& msg);
	// ���͸����
	void	SendToUser( const Lstring& uuid, LMsg& msg );
protected:
	void	_initTickTime();

	//���24Сʱ������־�� ÿ������6��ִ�� 
	void	_clearOldLogVideo();

	void	_loadFriendship();
private:
	LTime		m_tickTimer;
	Llong		m_200SceTick;//2������ѭ��һ��	
	Llong		m_600SceTick;//5������ѭ��һ��	
private:
	bool m_bDBMode;     // ���ݿ��������л�ʹ��
	LDBSession	m_dbsession;
	LDBSession	m_dbsessionBK;

	CMemeoryRecycle	m_memeoryRecycle;

	boost::mutex m_mutexLogicManager;
	LSocketPtr	m_logicManager;//����manager

	std::map<int,ShipInfo> m_user2recommIdMap;
};


#define gWork Work::Instance()

#endif