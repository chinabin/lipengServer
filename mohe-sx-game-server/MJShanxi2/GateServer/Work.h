#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "GateUser.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"

class Work:public LRunnable,public LSingleton<Work>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

	void			Tick(LTime& cur);
	void			HanderMsg(LMsg* msg);

	LTime&			GetCurTime();
	
public:
	//����ͻ�������������Ϣ
	void			HanderClientIn(LMsgIn* msg);
	//����ͻ��˵��ߵ���Ϣ 
	void			HanderUserKick(LMsgKick* msg);
	//�������ͻ���ID��ͻ��ֹͣ���ÿͻ���
	void			HanderKillClient(LMsgKillClient* msg);
	//����LogicManagerͬ����Logic��Ϣ
	void			HanderLogicInfo(LMsgLMG2GateLogicInfo* msg);
	//�����������Ϣ
	void			HanderHorseInfo(LMsgLMG2GHorseInfo* msg);
	//�޸�User״̬
	void			HanderModifyUserState(LMsgL2GModifyUserState* msg);

	void			HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg);
protected:
	void			_doWithClientMessage(LMsgConvertClientMsg* pMsg);
	void			_doWithClientLoginGate(LMsgC2SLoginGate* msg);
	void			_doWithClientHeartBeat(LMsgC2SHeart* pMsg);
	void			_doWithClientOtherMsg(boost::shared_ptr<LMsg> msgEntity, boost::shared_ptr<LBuff> msgOriginData);

	bool			_isClientMessageSafe(boost::shared_ptr<LMsgC2SVerifyHead> msgHead);
public:
	void			ConnectToLogicManager();
	
	//������Ϣ������User
	void			SendMessageToAllUse(LMsg* msg);

	void			SendToLogicUserOut(boost::shared_ptr<GateUser> user);
	void			HanderLogicUserOut(Lint id);

	LogicInfo*		GetLogicInfoBySp(LSocketPtr sp);
	LogicInfo*		GetLogicInfoById(Lint id);

protected:
	//����LogicManagerͬ����Coins��Ϣ
	void			HanderCoinsServerInfo(LMsgLMG2GLCoinsServerInfo* msg);
public:
	void			SendToCoinsServer(LMsg& msg);

protected:
	void			_initTickTime();
protected:
	//true: ���ҵ�����ɾ���� false��û�в�ѯ��
	bool			_findLogicSPAndDel(LSocketPtr sp); 
	bool			_isLogicSP(LSocketPtr sp);
	LogicInfo*		_getLogicSP(LSocketPtr sp);

	void			_reconnectLogicManager();
	void			_reconnectLogicServer(int iLogicId);
	void			_reconnectCoinsServer();

	void			_checkLogicManagerHeartBeat();
	void			_checkLogicServerHeartBeat();
	void			_checkCoinsServerHeartBeat();
protected:
	void			_checkGateUserState();
	void			_checkClientSp();

private:
	//������
	//void        testPrintLog(Lstring uuid, Lint flag);

	//�����Ժ�ͬ��manager��Ϣ
private:
	void	SendToLogicManager(LMsg& msg);
	//��manager������������managerͬ���û���Ϣ
	void    _SynchroData2Manager();
	//bool   m_needSynchroData2Manager=false;      //����manager��Ҫͬ�����ݱ�־

private:
	LTime		m_tickTimer;
	Llong		m_1SceTick;//1��ѭ��һ��
	Llong		m_5SceTick;//5��ѭ��һ��
	Llong		m_30SceTick;//30��ѭ��һ��
private:
	std::map<LSocketPtr, Lint> m_mapClientSp;	//��¼����û�б���֤���Ŀͻ�������
private:
	LSocketPtr  m_logicManager;
	std::map<Lint, LogicInfo>	m_logicInfo;
	CoinsInfo	m_coinsServer;

	std::map<LSocketPtr, bool> m_mapReconnect;
	std::map<LSocketPtr, bool> m_mapHeartBeat;

	CMemeoryRecycle	m_memeoryRecycle;
};


#define gWork Work::Instance()

#endif