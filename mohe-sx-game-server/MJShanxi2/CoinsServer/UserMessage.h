#ifndef _USER_MESSAGE_H_DB
#define _USER_MESSAGE_H_DB

#include "LBase.h"
#include "LRunnable.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"
#include "LDBSession.h"
#include "LTime.h"

class CUserMessage : public LRunnable
{
public:
	CUserMessage();
	virtual ~CUserMessage();
public:
	virtual bool		Init();
	virtual bool		Final();

	virtual void Clear();
	virtual void Run();

	virtual void Start();
	virtual	void Stop();
	virtual	void Join();
protected:
	void	HanderMsg(LMsg* msg);

	// ��Ҽ���
	void	HanderEnterCoinDesk(LMsgLMG2CNEnterCoinDesk* msg);

	void	HanderUserMsg(LMsgG2LUserMsg* msg);

	// From Gate
	// �û�����
	void	HanderUserOutMsg(LMsgG2LUserOutMsg* msg);

	//����˳�����
	void	HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg, const std::string& uuid, Lint gateid);

	//�������֪ͨ����
	void	HanderUserReady(LMsgC2SUserReady* msg, const std::string& uuid);

	//������㷿��
	void	HanderResetDesk(LMsgC2SResetDesk* msg, const std::string& uuid);

	// ���������Ϸ
	void	HanderGoOnCoinsDesk(LMsgC2SGoOnCoinsDesk* msg, const std::string& uuid, Lint gateid);

	// �����ȡ���
	void	HanderGetCoins(LMsgLMG2CNGetCoins* msg);
	void	HanderGetCoins(LMsgC2SGetCoins* msg, const std::string& uuid, Lint gateid);
	void	HanderGetCoins(Lint userid, const std::string& uuid, Lint gateid);


	// From Mangager
	// �û���½
	void	HanderUserLogin(LMsgLMG2CNUserLogin* msg);

	// From LogicServer
	void	HanderModifyUserState(LMsgL2LMGModifyUserState* msg);

private:
	// ����뿪
	void	_userOut( Lint userid );

	CMemeoryRecycle m_memoryRecycle;
	LTime		m_tickTimer;

};

#endif