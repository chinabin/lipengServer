#ifndef _GATE_MANAGER_H_
#define _GATE_MANAGER_H_

#include "LBase.h"
#include "User.h"
#include "LSingleton.h"
//#include "clubManager.h"


class UserManager :public LSingleton<UserManager>
{
public:
	virtual	bool Init();
	virtual	bool Final();
public:
	void addUser(boost::shared_ptr<User> user);
	void delUser(Lint iUserId);

	//�������ʿѫ�±�ǵ����ID����
	void clearSetMaxMedalUserId();

	//ͳ�������Ҫ�����ʿѫ�±��
	void statisticsUserMedal();

	//�������ID�жϸ����������ʿѫ�±��
	bool isMedalUserByUserId(Lint userId);

	boost::shared_ptr<CSafeResourceLock<User> > getUserByGateIdAndUUID(Lint iGateId, const Lstring& strUUID);
	boost::shared_ptr<CSafeResourceLock<User> > getUserbyUserId(Lint iUserId);
	boost::shared_ptr<CSafeResourceLock<User> > getUserbyUserUUID(const Lstring& uuid);

public:
	void addUserBaseInfo(boost::shared_ptr<UserBaseInfo> userBaseInfo);
	boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > getUserBaseInfo(Lint iUserId);
	boost::shared_ptr<CSafeResourceLock<UserBaseInfo> > getUserBaseInfoByUUID(const Lstring& strUUID);
public:
	void addUserLoginInfo(boost::shared_ptr<UserLoginInfo> userLoginInfo);
	void delUserLoginInfo(Lint iUserId);
	boost::shared_ptr<CSafeResourceLock<UserLoginInfo> > getUserLoginInfo(Lint iUserId);

public:
	boost::shared_ptr<CSafeResourceLock<UserDeskList> > getUserDeskListbyUserId(Lint iUserId);
	//UserDeskList  getUserDeskListbyUserId(Lint iUserId);
	void delUserDeskId(Lint iUserId, Lint iDeskId,Lint ifee,Lint delType);
	void addUserDeskId(Lint iUserId, Lint iDeskId,Lint ifee);
	bool verifyUserDeskID(Lint iUserId, Lint iDeskId);
	void broadcastAllUser(LMsgLDB2LMBCast* pMessage);

	//���¾��ֲ���
public:
	//�û����뷿���logict֪ͨmanager���·�������Ϣ
	void  UserAddClubDesk(LMsgL2LMGUserAddClubDesk*msg);
	//�û��뿪�����logict֪ͨmanager���·�������Ϣ
	void  UserLeaveClubDesk(LMsgL2LMGUserLeaveClubDesk* msg);
	void  UserListLeaveClubDesk(LMsgL2LMGUserListLeaveClubDesk* msg);
	//����ˢ��������Ϣ,��Ҫ����
	void  FreshDeskInfo(LMsgL2LMGFreshDeskInfo* msg);
	//���淨�������ˣ��������������棩������Ϣ
	void broadcastClubPlayTypeSceneUser(Lint clubId, Lint playType, LMsgSC& msg);
	void broadcastClubPlayTypeSceneUser(Lint clubId, Lint playType, LMsgSC& msg, std::vector<desksBaseInfo>& desksbaseInfo);
	void broadcastClubScene(Lint clubId);

	void checkPlayTypeJoinUserInRoom(LTime& cur);

	void broadcastUserList(std::set<Lint> userList, LMsgSC& msg);

private:
	boost::mutex m_mutexUserQueue;

	std::map<Lstring, boost::shared_ptr<User> > m_mapUUID2User;
	std::map<Lint, boost::shared_ptr<User> > m_mapId2User;
private:
	boost::mutex m_mutexUserBaseInfoQueue;
	std::map<Lint, boost::shared_ptr<UserBaseInfo> >  m_mapUserBaseInfo;
	std::map<Lstring, boost::shared_ptr<UserBaseInfo> >  m_mapUUID2UserBaseInfo;

private:
	boost::mutex m_mutexUserLoginInfoQueue;
	std::map<Lint, boost::shared_ptr<UserLoginInfo> > m_mapUserLoginInfo;
private:
	boost::mutex m_mutexUserDeskListQueue;
	std::map<Lint, boost::shared_ptr<UserDeskList>> m_mapUserDeskList;

	//����ʿѫ�µ����ID
	std::set<Lint> m_setMaxMedalUserId;
	//typedef std::map<Lint, UserDeskList> mapUserDeskList;
	//mapUserDeskList m_mapUserDeskList;
};

#define gUserManager UserManager::Instance()

#endif