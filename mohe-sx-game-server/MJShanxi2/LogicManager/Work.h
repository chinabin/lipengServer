#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"

#define INVALID_GATEID			0xFFFFFFFF
#define INVALID_LOGICSERVERID	0xFFFFFFFF

struct LOGIC_SERVER_INFO
{
	LOGIC_SERVER_INFO()
	{
		m_tickTime = 0;
	}

	LogicInfo m_logicInfo;
	Llong	  m_tickTime;								//�����logic��tick��ʱ���¼ʱ�䣬3���Ӳ��ָ��Ž����޳�.Ĭ��ֵΪ0
};

class Work:public LRunnable,public LSingleton<Work>
{
public:
	virtual bool	Init();
	virtual bool	Final();

	virtual void	Push(void* msg);
	virtual void	Start();
	virtual void	Run();
	virtual void	Clear();
	virtual	void	Join();
	virtual	void	Stop();

protected:
	void			Tick(LTime& cur);
	LTime&			GetCurTime();
protected:
	void			HanderMsg(LMsg* msg);

	void			HanderClientIn(LMsgIn* msg);
	void			HanderUserKick(LMsgKick* msg);

	//center�������id��Ϣ
	void			HanderCenterUserInInfo(LMsgCe2LUserIdInfo*msg);
	void			HanderCenterGMHorse(LMsgCe2LGMHorse* msg);
	void			HanderCenterGMBuyInfo(LMsgCe2LGMBuyInfo* msg);
	void			HanderCenterGMHide(LMsgCe2LGMHide* msg);
	void			HanderLogicSetGameFree(LMsgCE2LSetGameFree* msg);
	void			HanderLogicSetPXActive(LMsgCE2LSetPXActive* msg);
	void			HanderLogicSetOUGCActive(LMsgCE2LSetOUGCActive* msg);

	void            HanderLogicSetExchActive(LMsgCE2LSetExchActive* msg);
	void            HanderLogicSetActivity(LMsgCE2LSetActivity* msg);
	void            HanderCenterConfig(LMsgCE2LMGConfig* msg);
	void            HanderCenterSpecActive(LMsgLMG2LSpecActive* msg);

	void			HanderHeartBeatRequest(LMsgHeartBeatRequestMsg* msg);
	void			HanderHearBeatReply(LMsgHeartBeatReplyMsg* msg);
protected:
	void			HanderGateLogin(LMsgG2LMGLogin* msg);
	void			HanderGateLogout(LMsgKick* msg);
	void            HanderUpdateGateIpFromCenter(MHLMsgCe2LUpdateGateIp * msg);
	

	//����������Ϣ��center
	void			SendGateInfoToCenter();

	//����logic��Ϣ������gate
	void			SendLogicInfoToGates(Lint nGateID = 0);

	//��center ���ݾ��ֲ������Ϣ�Ľӿ�
private:
	void			HanderCenterClubInfo(LMsgCe2LClubInfo*msg);
	void           HanderAddClub(LMsgCe2LAddClub* msg);
	void           HanderClubAddPlayType(LMsgCe2LMGClubAddPlayType *msg);
	void           HanderClubAddUser(LMsgCe2LMGClubAddUser *msg);
	void           HanderClubDelUser(LMsgCe2LMGClubDelUser *msg);
	void           HanderClubHidePlayType(LMsgCe2LMGClubHidePlayType *msg);
	void           HanderClubModifyName(LMsgCe2LMGClubModifyClubName *msg);
   void           HanderClubModifyFeeType(LMsgCe2LMGClubModifyClubFee *msg);
   void           HanderUserApplyClub(LMsgCe2LMGClubAddUser *msg);

   void           HanderDismissRoom(LMsgCe2LMGDismissDesk *msg);

   void           HanderDelUserFromMap(LMsgCe2LMGDelUserFromManager *msg);

	//����logic,gate���͵�ͬ������
private:
	void           HanderLogicSynchroData(LMsgL2LMGSynchroData*msg);
	void           HanderGateSynchroData(LMsgG2LMGSynchroData*msg);
	void           SendRequestInfoToGateOrLogic(Lint GOrL, Lint serverId, Lint needData=1);    //  1-gate  2-logic,
	void           SendRequestInfoToAllGateOrLogic(Lint GOrL);//  1-gate  2-logic,

	//���Ϳ�����Ϣ��logic
	void           HanderControlMsgToLogic(LMsgCe2LMGControlMsg *msg);

protected:
	// ��ҷ��������
	CoinsInfo		m_coinsServer;
	void			HanderCoinsLogin(LMsgCN2LMGLogin* msg);
	void			HanderCoinsLogout(LMsgKick* msg);
	void			HanderCoinFreeDeskReq(LMsgCN2LMGFreeDeskReq* msg);
	void			HanderCoinRecycleDesk(LMsgCN2LMGRecycleDesk* msg);

	void			_checkCoinsServerState();

	//����Coins��Ϣ��Gate
	void			SendCoinsInfoToGates(Lint nGateID = 0);
	//����Coins��Ϣ��Logic
	void			SendCoinsInfoToLogic(Lint nLogicID = 0);

public:
	bool			IsCoinsServerConnected();
	void			SendMessageToCoinsServer(LMsg& msg);

public:
	Lint			getGateIdBySp(LSocketPtr sp);
	void			SendMessageToAllGate(LMsg& msg);
	void			SendMessageToGate(Lint gateID, LMsg& msg);
private:
	boost::recursive_mutex m_mutexGateInfo;
	std::map<Lint, GateInfo> m_gateInfo;
	std::set<Lint>  m_mapGateId;
protected:
	void			HanderLogicLogin(LMsgL2LMGLogin* msg);
	void			HanderLogicLogout(LMsgKick* msg);
	void			_checkLogicServerState();
	void			_checkAndTouchLogicServer();
	void			_checkLogicServerOnlySupportGameType();
	void			_checkRLogStart();
protected:
	void            _checkFreeModel();
	void            _checkExchActive();
	void            _checkMaxDeskCountLimitForLogicServer();

public:
	bool			isLogicServerExist(int iLogicServerId);
	void			SendMessageToAllLogic(LMsg& msg);
	void			SendMessageToLogic(Lint iLogicServerId, LMsg& msg);
	Lint			allocateLogicServer(int iGameType);
	
	void			delDeskCountOnLogicServer(Lint iLogicServerId, int iDelCount = 1);
private:
	boost::recursive_mutex m_mutexLogicServerInfo;
	std::map<Lint, LOGIC_SERVER_INFO> m_logicServerInfo;
	std::map<Lint, std::map<Lint, Lint> > m_mapLogicServerOnlySupportType; //��Щ������ֻ֧����Щ��Ϸ���͡���ʽ�� ������logicserver����������Ϸ����
public:
	void			SendToCenter(LMsg& msg);
	void			SendMsgToDb(LMsg& msg);
protected:
	//�����ڳ���������ʱ�����Ӳ���Ҫ����
	void			ConnectToCenter();
	void			ConnectToDb();

	void			_reconnectDB();
	void			_reconncetCenter();
	void			_checkCenterHeartBeat();
	void			_checkLogicDBHeartBeat();
private:
	boost::mutex m_mutexCenterClient;
	boost::mutex m_mutexDBClient;

	LSocketPtr	m_centerClient;//����center
	LSocketPtr	m_dbClient;//����db

	std::map<LSocketPtr, bool> m_mapReconnect;
	std::map<LSocketPtr, bool> m_mapHeartBeat;
protected:
	void		_initTickTime();
private:
	LTime		m_tickTimer;
	Llong		m_1SceTick;		//1��ѭ��һ��
	Llong		m_5SecTick;		//5��ѭ��һ��
	Llong		m_15SceTick;	//15��ѭ��һ��
	Llong		m_30SceTick;	//30��ѭ��һ��
	Llong		m_180SecTick;	//3����ѭ��һ��
	Llong		m_600SceTick;	//5������ѭ��һ��
	Llong		m_1DaySceTick;  //һ��
	Lint        m_last_alert_time; // �ϴα���ʱ��

	Lint        m_maxDeskCountLimitForEachLogic; //Logic Server �����֧�ֵ���������
	bool		m_bHadLoadedData;
private:
	CMemeoryRecycle m_memeoryRecycle;
};

#define gWork Work::Instance()

#endif