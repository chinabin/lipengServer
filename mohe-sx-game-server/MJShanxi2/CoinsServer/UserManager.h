#ifndef _USERMANAGER_H_
#define _USERMANAGER_H_

#include "LBase.h"
#include "User.h"
#include "LSingleton.h"

class UserManager :public LSingleton<UserManager>
{
public:
	virtual	bool Init();
	virtual	bool Final();
public:
	void AddUser(const UserPtr& user);
	void DelUser(Lint iUserId);

	int GetUsersCount();
	void ClearUsers();

	boost::shared_ptr<CSafeResourceLock<User> > getUserbyUserId(Lint iUserId);
	boost::shared_ptr<CSafeResourceLock<User> > getUserbyUserUUID(const Lstring& uuid);

	// ��ȡ���ͽ�ҵĴ���
	int GetGiveCount( Lint iUserId );
	// �������͵Ĵ���
	void IncreaseGiveCount( Lint iUserId );

private:
	boost::mutex m_mutexUserQueue;
	std::map<Lstring, UserPtr > m_mapUUID2User;
	std::map<Lint, UserPtr > m_mapId2User;

	// �û���ѽ����ȡ�Ĵ���
	std::map<Lint,Lint> m_userFreeCoins;
	Lint m_timeFreeCoins;	// ��¼m_userFreeCoins��ʱ��
};

#define gUserManager UserManager::Instance()

#endif