#ifndef _WORK_H_
#define _WORK_H_

#include "LRunnable.h"
#include "LSingleton.h"
#include "LTime.h"
#include "LMsg.h"
#include "LNet.h"
#include "LMsgS2S.h"
#include "Card.h"

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
	//����http��Ϣ 
	//void			HanderHttp(LMsgHttp* msg);	
	//����ͻ�������������Ϣ
	void			HanderClientIn(LMsgIn* msg);
	//����ͻ��˵��ߵ���Ϣ 
	void			HanderUserKick(LMsgKick* msg);

	//������ҵ�½����Ϣ 
	void			HanderUserLogin(LMsgS2CLogin* msg);
	//���������Ʒ��Ϣ 
	void			HanderUserItem(LMsgS2CItemInfo* msg);

	void			HanderUserCreateRoom(LMsgS2CCreateDeskRet* msg);

	void			HanderUserAddRoom(LMsgS2CAddDeskRet* msg);

	void			HanderUserIntoDesk(LMsgS2CIntoDesk* msg);

	void			HanderUserAddDesk(LMsgS2CDeskAddUser* msg);

	//�㲥��Ϸ��ʼ
	void			HanderUserPlayStart(LMsgS2CPlayStart* msg);
	//�㲥�������
	void			HanderUserGetCard(LMsgS2COutCard* msg);
	//�������㲥��ҳ���
	void			HanderUserPlayerCard(LMsgS2CUserPlay* msg);
	//�������㲥���˼��
	void			HanderUserThink(LMsgS2CThink* msg);
	//�������㲥�������
	void			HanderUserOper(LMsgS2CUserOper* msg);
	//�������㲥��Ϸ����
	void			HanderGameOver(LMsgS2CGameOver* msg);
	//����
	void			HanderReconnect(LMsgS2CDeskState* msg);
	// ���㷿��
	void			HanderReset(LMsgS2CResetDesk* msg);

	//������ҵ�½center����Ϣ 
	void			HanderServerMsg(LMsgS2CMsg* msg);

	void			SendCreateRoom();

	//����
	void			OutCard();

private:
	
	//center������������¼
	void			HanderCenterUserLogin(LMsgCe2LUserLogin*msg);
	//center�������ر�
	void			HanderCenterLogout();


private:
	void			ConnectCenterServer();
	
	void			HanderLoginRet();

private:
	LTime		m_tickTimer;

	Llong		m_30MSceTick;//30����ѭ��һ��

	Llong		m_200MSceTick;//200����ѭ��һ��

	Llong		m_1SceTick;//1��ѭ��һ��

	Llong		m_30SceTick;//30��ѭ��һ��

	Llong		m_600SceTick;//5������ѭ��һ��


	Lint		m_state;

private:
	LSocketPtr	m_clientSocket;//

	std::map<Lint, GateInfo*> m_gateInfo;

	CardVector	m_handCard;
	CardVector	m_pengCard;
	CardVector	m_gangCard;
	CardVector	m_agangCard;

	Lint		m_myPos;
};


#define gWork Work::Instance()

#endif