#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"

class User;

struct UserLoginInfo
{
	Lint	m_seed;
	Lint	m_time;
	LUser	m_user;
};

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
	//md5�����㷨
	Lstring			Md5Hex(const void* data,Lsize size);
public:
	//����ͻ�������������Ϣ
	void			HanderClientIn(LMsgIn* msg);
	//����ͻ��˵��ߵ���Ϣ 
	void			HanderUserKick(LMsgKick* msg);

	//������ҵ�½����Ϣ 
	void			HanderUserLogin(LMsgLMG2LLogin* msg);
	
	//������ҵ�½����Ϣ
	void			HanderDeskUserLogin(User* pUser, Lint gateId, Lstring& ip, bool firstLogin = false, Lstring buyInfo = "", Lint hide = 0);

	//������ҵǳ�����Ϣ 
	void			HanderUserLogout(Lint gateId, const Lstring& strUUID);

	//������Ҵ�������
	void			handerLMGCreateDesk(LMsgLMG2LCreateDesk* msg);

	void            handerLMGCreateDeskForOther(LMsgLMG2LCreateDeskForOther* msg);

	void            handerLMGCreatedDeskList(LMsgLMG2LCreatedDeskList* msg);

	void            handerLMGDisMissCreatedDesk(LMsgLMG2LDissmissCreatedDesk* msg);

	//������Ҽ�������
	void			handerLMGAddToDesk(LMsgLMG2LAddToDesk* msg);

	// ��ѯ������Ϣ
	void            handerLMGQueryGPSInfo(MHLMsgLMG2LQueryRoomGPSInfo * msg);

	//���ھ��ֲ��Ĳ���
	void              handerLMGCreateDeskForClub(LMsgLMG2LCreateDeskForClub* msg);
	void              handerLMGCreateClubDeskAndEnter(LMsgLMG2LCreateClubDeskAndEnter*msg);
	void              handerLMGEnterClubDesk(LMsgLMG2LEnterClubDesk*msg);
	void              handerLMGDisMissClubDesk(LMsgLMG2LDissmissClubDesk*msg);

	//����LM���͵����ͻ������Ϣ
	void			handerPXActiveInfo(LMsgLMG2LPXActive* msg);
	
	void            HanderLMGCreateCoinDesk(LMsgCN2LCreateCoinDesk* msg);

	void			HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg);

	void			HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg);

	//��������ID������ʧ�ܣ����������
	void			RecycleDeskId(Lint deskID);
	//���ֲ�
	void            RecycleDeskId(Lint deskID, Lint clubId, Lint playTypeId, Lint clubDeskId);
public:
	void			ConnectToLogicManager();

	void			SendToLogicManager(LMsg& msg);
public:
	void			HanderGateLogin(LMsgG2LLogin* msg);
	void			HanderGateLogout(LMsgKick* msg);
	GateInfo*		GetGateInfoBySp(LSocketPtr sp);
	GateInfo*		GetGateInfoById(Lint id);
	void			DelGateInfo(Lint id);

	/* �����û����Ƶ���Ϣ */
	void			HanderGateUserMsg(LMsgG2LUserMsg* msg,GateInfo* gate);

protected:
	//����LogicManagerͬ����Coins��Ϣ
	void			HanderCoinsServerInfo(LMsgLMG2GLCoinsServerInfo* msg);
public:
	void			SendToCoinsServer(LMsg& msg);

	//����manager�Ͽ����Ӻ��ٴ�����manager��ͬ��logic��Ϣ��manager��
private:
	//��manager������������managerͬ���Լ���Ϣ
	void           _SynchroData2Manager();
	//bool   m_needSynchroData2Manager=false;      //����manager��Ҫͬ�����ݱ�־

public:
	//DB�������
	void			ConnectToDb();
	void			SendMsgToDb(LMsg& msg);
protected:
	void			_initTickTime();
protected:
	void			_reconnectLogicManager();
	void			_reconnectLogicDB();
	void			_reconnectCoinsServer();

	void			_checkLogicManagerHeartBeat();
	void			_checkLogicDBHeartBeat();
	void			_checkCoinsServerHeartBeat();

	void			_checkRLogStart();
private:
	LTime		m_tickTimer;

	Llong		m_1SceTick;//1��ѭ��һ��
	Llong		m_15SecTick;//15��ѭ��һ��
	Llong		m_30SceTick;//30��ѭ��һ��
	Llong		m_600SceTick;//5����ѭ��һ��
private:
	LSocketPtr	m_logicManager;//����center
	LSocketPtr	m_dbClient;//����db
	CoinsInfo	m_coinsServer;

	std::map<Lint, GateInfo> m_gateInfo;
private:
	std::map<LSocketPtr, Llong> m_mapReconnect;
	std::map<LSocketPtr, bool> m_mapHeartBeat;
private:
	CMemeoryRecycle m_memeoryRecycle;
	Lstring m_logFileName;
	Lint m_logFileIndex;
};

#define gWork Work::Instance()

#endif