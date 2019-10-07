#pragma once
#ifndef _CLUB_DESK_H_
#define _CLUB_DESK_H_
#include "LBase.h"
#include "LMsgS2S.h"
#include "Work.h"
#include "SafeResourceLock.h"
#include "LTime.h"
#include "clubPlay.h"

////////////////////////////////////////////////////////////////////////////////////////////
//���ֲ�����
//������ֲ������ӵ���Ϣ
struct ClubDeskInfo :public CResourceLock {

private:
	Lint            m_isUsed;                              // 0 -û��  1-ʹ��

	Lint            m_clubId;                              //���ֲ�Id
	Lint            m_playTypeId;                    //�淨Id
	Lint            m_clubDeskId;                    //����
	Lint            m_showDeskId;                 //չʾ�����Ӻ�
	Lint            m_realDeskId;                   //��ʵ����
	Lint            m_lock;                               //�Ƿ���  0-����  1-����
	
public:
	std::list<deskUserInfo>   m_deskUserInfo;           //���������û��б�
	std::set<Lint>      m_lookonInfo;                              //�����Թ��û��б�

	Lint            m_roomFull;                      //�����Ƿ���
	Lint            m_currCircle;                   //��ǰ����
	Lint            m_totalCircle;                  //�����ܾ���
	Lint			    m_isClubOwerLooking;  //���ֲ��᳤�Ƿ��������ƹ�ս   0������    1�����ڹ�ս

	Lint            m_redPacketFlag;         //�����־   0--û��  1--��
	Lint            m_flag;                            //�淨
	Lint            m_maxUserCount;              //�������

	//playTypeDetail  m_playDetail;

	ClubDeskInfo(Lint clubId, Lint deskId, Lint showDeskId, Lint playTypeId=0)
	{
		//���ӻ�����Ϣ
		m_clubId = clubId;
		m_clubDeskId = deskId;
		m_showDeskId = showDeskId;
		
		rebootDesk();
		m_playTypeId = playTypeId;
		m_lock = 0;
	}

	void rebootDesk()
	{
		m_isUsed = 0;
		m_playTypeId = 0;
		m_realDeskId = 0;

		m_roomFull = 0;
		m_currCircle = 0;
		m_totalCircle = 0;

		
		m_redPacketFlag = 0;
		m_isClubOwerLooking = 0;
		m_flag = 0;
		m_maxUserCount = DESK_USER_COUNT;
		m_deskUserInfo.clear();
		m_lookonInfo.clear();
		//m_playDetail.clear();
	}

	//�������ֲ�
	Lint getClubId() { return m_clubId; }
	//7λ���ţ�Ψһ��
	Lint getClubDeskId() { return m_clubDeskId; }
	//չʾ���Ӻ�  1��2��3
	Lint getShowDeskId() { return m_showDeskId; }
	//����
	bool canBeLock() { if (getUserCount() > 0)return false; return true; }
	Lint getLock() { return m_lock; }
	bool isLock() { return m_lock == 1 ? true : false; }
	void setLock() { m_lock = 1; }
	void delLock() { m_lock = 0; }
	//�淨
	Lint getPlayId() { return m_playTypeId; }
	void setPlayTypeId(Lint playTypeId)
	{
		m_playTypeId = playTypeId;
	}
	//��ʵ���Ӻ�
	Lint getRealDeskId() { return m_realDeskId; }
	void setRealDeskId(Lint realId,Lint playId)
	{
		m_realDeskId = realId;
		m_playTypeId = playId;
	}
	//���ӳ�Ա
	bool  empty() { return (m_deskUserInfo.empty() && m_lookonInfo.empty()) ? true : false; }
	Lint  getUserCount() { return m_deskUserInfo.size(); }
	void insertUserInfo(deskUserInfo &userInfo)
	{
		auto ItUser = m_deskUserInfo.begin();
		for (; ItUser != m_deskUserInfo.end(); ItUser++)
		{
			if (ItUser->m_userId == userInfo.m_userId) break;
		 }
		if (ItUser == m_deskUserInfo.end())
		{
			m_deskUserInfo.push_back(userInfo);
		}
	}
	void delUserInfo(Lint  userId)
	{
		auto ItUser = m_deskUserInfo.begin();
		for (; ItUser != m_deskUserInfo.end(); ItUser++)
		{
			if (ItUser->m_userId == userId)
			{
				m_deskUserInfo.erase(ItUser);
				break;
			}
		}
	}

	void insertLookon(Lint userId) {
		m_lookonInfo.insert(userId);
	}
	void delLookon(Lint userId) {
		m_lookonInfo.erase(userId);
	}

	//�Ƿ񴴽���
	bool isUsed()
	{
		return m_realDeskId == 0 ? false : true;
	}

};


class ClubDeskManager :public LSingleton<ClubDeskManager>
{
public:
	virtual	bool Init();
	virtual	bool Final();
public:
	
	boost::shared_ptr<CSafeResourceLock<ClubDeskInfo> > getClubDeskInfoByClubDeskId(Lint clubDeskId);                           //��ȡ����������Ϣ

/////////////////////////////////////////////////////////////////
//�����ӽ�������
	//���������Ϣ
	void addClubDesk(Lint clubId,Lint clubDeskId, Lint showDeskId,Lint playId);                //�������
	void delClubDesk(Lint clubDeskId);                                                                                                 //ɾ������

	void setPlayId(Lint clubDeskId, Lint playId, playTypeDetail & pDetail);                                                   //�����������淨
	void setRealId(Lint clubDeskId, Lint realDeskId, Lint playId, playTypeDetail & pDetail);                  //������������ʵ����

	desksInfo  getClubDeskInfo(Lint clubDeskId,Lint currPlayId);                                                                            //��ȡ������Ϣ
	Lint          getDeskInClubId(Lint clubDeskId);                                                                               //��ȡ�������ڵľ��ֲ�ID

/////////////////////////////////////////////////////////////////
//�û�������Ӱ�죬��logic��������Ϣ
	void  addDesk(LMsgL2LMGUserAddClubDesk * addDesk);                                                     //��������
	void  leaveDesk(LMsgL2LMGUserLeaveClubDesk* leaveDesk);                                              //�뿪����
	void leaveDesk(LMsgL2LMGUserListLeaveClubDesk* leaveDesk);                                       // ����ͬʱ�뿪����
	void  updateDeskInfo(LMsgL2LMGFreshDeskInfo * freshDeskInfo);                                  //������������
	void  recycleClubDesk(Lint clubDeskId);                                                                                      //��ɢ����

	void modifyClubDeskInfoFromLogic(const LogicDeskInfo & deskInfo);                             //����logic�����ݣ��޸����ֲ�������Ϣ
	
	//����һ�������ľ��ֲ�����
	//void ResetClubDeskInfo(int clubId, int count);
	
private:


private:
	boost::mutex m_mutexClubDeskQueue;
	std::map<Lint, boost::shared_ptr<ClubDeskInfo> > m_mapId2ClubDeskInfo;          //�û�����  key=userId�� value=���ֲ���������



};






#define gClubDeskManager ClubDeskManager::Instance()


#endif