#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LActive.h"

#include "SafeResourceLock.h"

class User : public CResourceLock
{
public:
	User(const LUser& data, Lint gateId, const Lstring& ip);
	virtual ~User();

	virtual bool IsRobot() { return false; }

	void	SetUserData( const LUser& data ) { m_userData = data; }
	const LUser& GetUserData() const { return m_userData; }

	//��ȡ������ݿ���id
	Lint	GetUserDataId() const { return m_userData.m_id; };

	// ��ҵ�½���������
	virtual void	Login();
	// ����˳��������
	virtual void	Logout();

	// ֱ�ӷ����û�
	virtual void	Send(LMsg& msg);
	virtual void	Send(const LBuffPtr& buff);

	static void	AddCoinsCount(Lint count, Lint operType, Lint userid, const Lstring& uuid);
	virtual void	AddCoinsCount(Lint count, Lint operType);
	virtual void	DelCoinsCount(Lint count, Lint operType);

	void	SetDeskRunID(Lint nValue) { m_deskRunID = nValue; }
	Lint	GetDeskRunID() const { return m_deskRunID; }

	void	SetDeskID(Lint nValue) { m_deskID = nValue; }
	Lint	GetDeskID() const { return m_deskID; }

	LGUserMsgState	GetUserState() const { return m_userState; }
	void	SetUserState(LGUserMsgState nValue) { m_userState = nValue; }

	void	SetUserLogicID(Lint nValue) { m_logicID = nValue; }
	Lint	GetUserLogicID() const { return m_logicID; }

	void	SetOnline( bool b ) { m_online = b; }
	bool	GetOnline() const { return m_online; }

	void	SetIp(const Lstring& ip) { m_ip = ip; }
	const Lstring& GetIp() const { return m_ip; }

	void	SetUserGateID(Lint nValue){ m_gateId = nValue; }
	Lint	GetUserGateID() const { return m_gateId; }

protected:
	LUser	m_userData;					//�������

	// ��Ϸ����Ϣ
	Lint	m_deskRunID;				//��������ʱID
	Lint	m_deskID;					//�������ID ����ѿ�ʼ����������

	// ���״̬��Ϣ
	LGUserMsgState	m_userState;				//��ҵ�ǰ״̬
	Lint	m_logicID;					//������ĸ�logic���� ��ҽ������ӳɹ� ���ؼ�¼�� Ҫ���Ƿ���������ʱ�� 

	// ������Ϣ
	bool	m_online;
	Lint	m_gateId;					//������ĸ�gate����
	Lstring	m_ip;
};

typedef boost::shared_ptr<User> UserPtr;

#endif