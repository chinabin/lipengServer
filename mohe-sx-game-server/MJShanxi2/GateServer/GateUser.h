#ifndef _GATE_USER_H_
#define _GATE_USER_H_

#include "LSingleton.h"
#include "LSocket.h"
#include "LTime.h"

struct GateUser
{
	Lstring			m_strUUID;					//��ҵ�UUID
	Lstring			m_strKey;					//��ҷ����key
	unsigned long	m_uMsgOrder;				//�������
	Llong			m_timeActive;				//��һʱ��,����

	LSocketPtr		m_sp;
	Lint			m_login;
	Lint			m_userState;				//��ҵ�ǰ״̬
	Lint			m_logicID;					//������ĸ�logic����
	GateUser()
	{
		m_login		= 0;
		m_userState = 0;
		m_logicID	= 0;
		m_uMsgOrder = 0;
		m_timeActive= 0;
	}
	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue){m_userState = nValue;}
	Lint	getUserLogicID(){return m_logicID;}
	void	setUserLogicID(Lint nValue){m_logicID = nValue;}
};

//////////////////////////////////////////////////////////////////////////
class GateUserManager :public LSingleton<GateUserManager>
{
public:
	virtual	bool	Init();
	virtual bool	Final();
public:
	boost::shared_ptr<GateUser> createUser(const Lstring& strUUID, LSocketPtr sp);
	
	boost::shared_ptr<GateUser> findUser(const Lstring& strUUID);
	boost::shared_ptr<GateUser> findUser(LSocketPtr sp);

	void	AddUser(boost::shared_ptr<GateUser> user);
	void	DelUser(const Lstring& strUUID);

	const std::map<Lstring, boost::shared_ptr<GateUser> >& getAllUsers();
protected:
	Lstring _generateKey();
private:
	std::map<LSocketPtr, boost::shared_ptr<GateUser> >	m_mapSp2GU;
	std::map<Lstring, boost::shared_ptr<GateUser> >		m_mapUUID2GU;
};

#define gGateUserManager GateUserManager::Instance()

#endif