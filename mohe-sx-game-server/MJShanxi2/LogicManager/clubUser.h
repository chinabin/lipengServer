#pragma once

#ifndef _CLUB_USER_H_
#define _CLUB_USER_H_

#include "SafeResourceLock.h"
#include "LSingleton.h"
#include "LMsg.h"
#include "LMsgS2S.h"


/* �������״̬��0��������  1�����ߣ�*/
enum USER_IN_CLUB_STATUS
{
	USER_IN_CLUB_OFFLINE = 0,	//������
	USER_IN_CLUB_ONLINE = 1,	//����
};

/* ����Ƿ���Ϸ״̬��0��������Ϸ��  1������Ϸ�У�*/
enum USER_STATE
{
	USER_STATE_INIT = 0,	//������Ϸ��
	USER_STATE_PLAY = 1,	//��Ϸ��
};

/* �������ֵ�ṹ�� */
struct UserPowerPoint
{
	Lint  m_userId;			//���ID
	Lint  m_clubId;			//��Ҿ��ֲ�ID
	Lint  m_point;			//����ڸþ��ֲ�������ֵ
	Lint  m_medalPoint;		//�����������Ӯ����ͳ�ƣ�������ʿѫ��

	UserPowerPoint(Lint userId, Lint clubId, Lint point)
	{
		Lint t_oldPoint = m_point;

		m_userId = userId;
		m_clubId = clubId;
		m_point = point;
		m_medalPoint = 0;

		LLOG_ERROR("clubUser::UserPowerPoint Init... userId=[%d] clubId=[%d] cheangePoint=[%d], oldPoint[%d], medalPoint=[%d]", 
			m_userId, m_clubId, point, t_oldPoint, m_medalPoint);
	}

	void clear()
	{
		m_userId = 0;
		m_clubId = 0;
		m_point = 0;
		m_medalPoint = 0;
	}

	/* ��ȡ�������ֵ */
	Lint getPoint() { return m_point; }

	/* �����������ֵ */
	void setPoint(Lint point) { m_point = point; }

	/* ������������ֵ */
	void addPoint(Lint addPoint) 
	{  
		if (addPoint > 0) m_point += addPoint;
		LLOG_ERROR("clubUser::UserPowerPoint::addPoint() Run... userId=[%d] clubId=[%d] addPoint=[%d] totalPoint=[%d]", 
			m_userId, m_clubId, addPoint, m_point);
	}

	/* ����Ҽ�������ֵ */
	void reducePoint(Lint  delPoint) 
	{
		if (delPoint > 0) m_point -= delPoint;
		LLOG_ERROR("clubUser::UserPowerPoint::reducePoint() Run... userId=[%d] clubId=[%d] reducePoint=[%d] totalPoint=[%d]", 
			m_userId, m_clubId, delPoint, m_point);
	}
};

/* ������ڵ�ǰ���ֲ���Ϣ */
struct  UserClubInfo
{
	Lint  m_clubId;			//��ǰ���ھ��ֲ�ID
	Lint  m_playTypeId;		//��ǰ�淨��ϢID
	Lint  m_clubDeskId;		//��ǰ���ھ��ֲ�������ID
	//Lint  m_pos;			//�������ϵ�λ��

	UserClubInfo()
	{
		m_clubId = 0;
		m_playTypeId = 0;
		m_clubDeskId = 0;
		//m_pos = INVAILD_POS;
	}
	UserClubInfo(Lint clubId, Lint playTypeId, Lint clubDeskId)
	{
		m_clubId = clubId;
		m_playTypeId = playTypeId;
		m_clubDeskId = clubDeskId;
	}
	void reset()
	{
		m_clubId = 0;
		m_playTypeId = 0;
		m_clubDeskId = 0;
		//m_pos = 21;
	}
};

class ClubUserManager;

/* ������о��ֲ���Ϣ */
struct ClubUser :public CResourceLock
{
	friend ClubUserManager;
private:
	Lint            m_userId;				//�û�id

protected:
	Lint            m_online;               // ����Ƿ����� 0--������  1-���� 
	Lint            m_userState;            // ����Ƿ�����Ϸ�� 0--������Ϸ�� ��1--��Ϸ��

	/* ������о��ֲ�������ֵ map<clubId, ��Ҿ��ֲ��е�������Ϣ> */
	std::map <Lint, boost::shared_ptr<UserPowerPoint>>  m_powerPoint;

public:
	//�û�������Ϣ
	Lstring			m_nike;                 //�û��ǳ�
	Lstring			m_headImageUrl;         //�û�ͷ��
	Lstring			m_ip;					//�û�ip
	Lstring			m_unionId;				//�û��ǳ�
	Lint            m_userLevel;
	Lint			m_offLineTime;          //����ʱ��
	Lint            m_lastEnterTime;        //���¼�����ֲ�ʱ��     //��ʱ����
	Lint            m_lastLeaveTime;        //�����뿪���ֲ�ʱ��     //��ʱ����

	UserClubInfo    m_currClubInfo;			//�û���ǰ���ֲ���Ϣ

	 //�û����о��ֲ���Ϣ
	std::set<Lint>  m_allClubId;			//�û���������о��ֲ�Id
	std::set<Lint>  m_adminClub;			//�û��ǹ���Ա���ֲ�id
	std::set<Lint>  m_ownerClub;			//�û��ǻ᳤���ֲ�id


	Lint			m_currWaitPlayType;


	ClubUser()
	{
		m_userId = 0;
		m_currWaitPlayType = 0;
		m_userLevel = 0;
		m_online = 0;
		m_lastEnterTime = 0;
		m_lastLeaveTime = 0;
		m_offLineTime = 0;
		m_userState = USER_STATE_INIT;
		//m_type = 1;
	}

	~ClubUser() {}

	ClubUser(Lint userId, Lint clubId)
	{
		m_userId = userId;
		m_allClubId.insert(clubId);
		m_userLevel = 0;
		m_online = 0;
		m_lastEnterTime = 0;
		m_lastLeaveTime = 0;
		m_offLineTime = time(NULL);
		m_userState = USER_STATE_INIT;
		//m_type = 1;
	}

	/* ��ȡ���ID */
	Lint getID() { return m_userId; }

	/* ��ȡ����Ƿ��ھ��ֲ� */
	Lint getLine() { return m_online; }

	/* ��������Ƿ��ھ��ֲ��еı�־ */
	void setLine(Lint line) { m_online = line; }

	/* �ж�����Ƿ�����Ϸ�� */
	bool isPlay() { return m_userState == USER_STATE_PLAY ? true : false; }

	/* ��������Ƿ�����Ϸ�еı�־ */
	void setState(Lint userState) { m_userState = userState; }

	//���ظ��ͻ���״̬  0-�����ߣ���ȡʱ�䣩  1-���ߣ�δ�μ���Ϸ�� 2-���ߣ���Ϸ�У�
	Lint getClientState()
	{
		Lint  clientState = getLine();
		if (isPlay())clientState = 2;
		return clientState;
	}

	/* �����type��ݼ���clubId�ľ��ֲ��� */
	void addNewClub(Lint clubId, Lint type)
	{
		//��������еľ��ֲ�
		m_allClubId.insert(clubId);

		//���Ϊ�þ��ֲ��Ĺ���Ա
		if (type == USER_IN_CLUB_TYPE_ADMIN)
		{
			m_adminClub.insert(clubId);
		}
		//���Ϊ�þ��ֲ��Ļ᳤
		else if (type == USER_IN_CLUB_TYPE_OWNER)
		{
			m_ownerClub.insert(clubId);
		}
	}

	/* ��clubId�ľ��ֲ���ɾ������� */
	void delFromClub(Lint clubId, Lint type)
	{
		m_allClubId.erase(clubId);
		m_adminClub.erase(clubId);
		m_ownerClub.erase(clubId);
	}

	void setCurrWaitPlayType(Lint playType)
	{
		m_currWaitPlayType = playType;
	}

	Lint getCurrWaitPlayType()
	{
		//LLOG_DEBUG("HHHWYZ getCurrWaitPlayType userId[%d]  m_currWaitPlayType=[%d]", m_userId, m_currWaitPlayType);
		return m_currWaitPlayType;
	}

	/* ��ȡ�û���clubId�ľ��ֲ������  1����ͨ��Ա  2������Ա  3���᳤ */
	Lint getUserType(Lint clubId)
	{
		if (m_ownerClub.find(clubId) != m_ownerClub.end())return USER_IN_CLUB_TYPE_OWNER;
		if (m_adminClub.find(clubId) != m_adminClub.end()) return USER_IN_CLUB_TYPE_ADMIN;
		return USER_IN_CLUB_TYPE_NORMAL;
	}

	/* ��ȡ�û���clubId�ľ��ֲ�������ֵ */
	Lint getClubPoint(Lint clubId)
	{
		auto ItPoint = m_powerPoint.find(clubId);
		if (ItPoint != m_powerPoint.end())
		{
			return ItPoint->second->m_point;
		}
		else
		{
			return 0;
		}
	}

	//ֻ���������û���ʱ���õ�
	void setClubPoint(Lint clubId, Lint point)
	{
		if (clubId == 0)return;
		auto ItPoint = m_powerPoint.find(clubId);
		if (ItPoint != m_powerPoint.end())
		{
			ItPoint->second->m_point = point;
			ItPoint->second->m_medalPoint = 0;
		}
		else
		{
			boost::shared_ptr<UserPowerPoint> userPoint(new UserPowerPoint(getID(), clubId, point));
			m_powerPoint[clubId] = userPoint;
		}
	}

	/* ���������clubId�ľ��ֲ�������ֵ */
	bool updateClubPoint(Lint clubId, Lint point)
	{
		if (clubId == 0)return false;
		auto ItPoint = m_powerPoint.find(clubId);
		if (ItPoint != m_powerPoint.end())
		{
			ItPoint->second->m_point += point;
			ItPoint->second->m_medalPoint += point;
		}
		else
		{
			boost::shared_ptr<UserPowerPoint> userPoint(new UserPowerPoint(getID(), clubId, point));
			m_powerPoint[clubId] = userPoint;
		}
	}

};

struct ClubOwner :public ClubUser
{
	
	//�û��������ľ��ֲ�
	std::set<Lint>   m_CreateClubId;

	ClubOwner()
	{
		
	}
	~ClubOwner() 
	{

	}
};


class ClubUserManager :public LSingleton<ClubUserManager>
{
public:
	virtual	bool Init();
	virtual	bool Final();
public:
	void addClubUser(clubUser &userInfo, Lint online = USER_IN_CLUB_OFFLINE);
	//void addClubUser(Lint clubId, Lint iUserId, Lstring nike, Lstring headurl,Lint type,Lint online= USER_IN_CLUB_OFFLINE);
	//void delClubUser(Lint clubId, Lint iUserId);
	void delClubUser(clubUser &userInfo);

	void userOffLine(Lint iUseId);
	void userOnLine(Lint iUseId , bool IsPlay);
	void setUserPlayStatus(Lint iUseId, bool IsPlay);

	void userLoginClub(Lint clubId, Lint userId);
	void userLogoutClub(Lint clubId, Lint userId);

	void  userAddDesk(LMsgL2LMGUserAddClubDesk*msg);
	void  userLeaveDesk(LMsgL2LMGUserLeaveClubDesk* msg);
	void  userLeaveDesk(LMsgL2LMGUserListLeaveClubDesk* msg);



	void setUserAdmin(Lint type, Lint clubId, Lint userId);    	//���þ��ֲ�����Ա
	bool isUserInClub(Lint clubId, Lint userId);                           //�ж��û��Ƿ��ھ��ֲ���
	Lint  getDefaultClubId(Lint userId);                                         //������ֲ�ʱ��ȡ�û�Ĭ�ϵ�clubId

/////////////////////////////////////////////////////////////////////////////////////////
//���ֲ�������
public:
	Lint  addPoint(Lint userId, Lint clubId, Lint point) { return _changePoint(userId, clubId, point, CHANGE_POINT_TYPE_ADD); }
	Lint  reducePoint(Lint userId, Lint clubId, Lint point) { return _changePoint(userId, clubId, point, CHANGE_POINT_TYPE_REDUCE); }
	Lint  updatePoint(Lint userId, Lint clubId,Lint point);
	Lint  getPoint(Lint userId, Lint clubId);

	//��ȡ���ֲ������Ӯ����������ֵ���ID
	std::list<Lint>  getMaxPointUserId(Lint clubId);

	//������ֲ����������Ӯ������ֵ
	void  clearClubUserMedal();
private:
	
	Lint  _changePoint(Lint userId, Lint clubId, Lint point, Lint type);             // type = 0 ��   1 =��
	

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public:
	boost::shared_ptr<CSafeResourceLock<ClubUser> > getClubUserbyUserId(Lint iUserId);                           //��ȡ�û�������Ϣ
	boost::shared_ptr<CSafeResourceLock<ClubUser>> getClubUserbyClubIdUserId(Lint clubId, Lint iUserId);        //��ȡ�û�������Ϣ���ݾ��ֲ�ID�����ID
	std::set<boost::shared_ptr<ClubUser>  > getClubUserbyUserNike(Lint clubId, Lstring nike);                      //�����ǳƻ�ȡ�û�������Ϣ

	
	std::set<boost::shared_ptr<ClubUser>  > getClubAllUser(Lint clubId);                            //��ȡ���ֲ�ȫ���û�������Ϣ
	std::set<boost::shared_ptr<ClubUser>  > getClubAllOnLineUser(Lint clubId);               //��ȡ���ֲ������û�������Ϣ

	std::set<boost::shared_ptr<ClubUser>  > getClubUserInGame(Lint clubId);                      //��ȡ����Ϸ���û���Ϣ
	std::set<boost::shared_ptr<ClubUser>  > getClubAllOnLineUserNoGame(Lint clubId);   //��ȡ������Ϸ�������û���Ϣ
	std::set<boost::shared_ptr<ClubUser>  > getClubAllOffLineUserNoGame(Lint clubId);  //��ȡ������Ϸ�е����û���Ϣ

	std::set<Lint> getClubAllOnLineUserId(Lint clubId);                                               //��ȡ���ֲ������û�Id
	std::set<Lint> getClubPlayTypeUserId(Lint clubId, Lint playTypeId);               //��ȡ���ֲ��淨���û�id �����ڷ����
	std::set<Lint> getNeedFreshSceneUserId(Lint clubId);                                         //��ȡ��Ҫˢ��������ֲ��������û����ھ��ֲ�ͬʱû���ڷ������û�)

	Lint getUserInAllClubCount(const std::set<Lint> &clubIdList, std::vector<clubOnlineUserSize> &clubOnlineSize); // ��ȡ���ֲ�����������������ֲ��ģ�

	Lint getClubUserCountOnline(Lint clubId);        //��ȡ���ֲ���������
	Lint getClubUserCount(Lint clubId);         //��ȡ���ֲ�����


private:
	boost::mutex m_mutexClubUserQueue;
	std::map<Lint, boost::shared_ptr<ClubUser> > m_mapId2ClubUser;          //�û�����  key=userId�� value=���ֲ��û�����

	std::map <Lint, std::set<Lint> > m_mapClubId2ClubUserIdList;                  //���ֲ��û��б� key=clubId   value=���þ��ֲ��û�

	std::map<Lint, std::set<Lint> >   m_mapClubId2ClubOnLineUserIdList;   //���ֲ��û��ھ��ֲ������û��б�  key=clubId  value=�ھ��ֲ����û��б����ܵ��ߣ�

};


#define gClubUserManager ClubUserManager::Instance()








#endif
