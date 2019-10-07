#ifndef _DESK_MANAGER_H
#define _DESK_MANAGER_H

#include "LBase.h"
#include "LSingleton.h"
#include "LMsgS2S.h"


#define CLUB_DESK_ID_BEGIN_NUMBER  1000000

//clubdeskId ���ڵ�Club��Ϣ
struct ClubInfos
{
	Lint  m_clubId;
	Lint  m_playTypeId;
	Lint  m_clubDeskId;

	ClubInfos()
	{
		m_clubId=0;
		m_playTypeId=0;
		m_clubDeskId=0;
	}
	ClubInfos(Lint clubId, Lint playTypeId, Lint clubDeskId)
	{
		m_clubId=clubId;
		m_playTypeId=playTypeId;
		m_clubDeskId=clubDeskId;
	}
};


struct DeskInfos
{
	Lint m_logicServerId;   //logicserver id
	Lint m_creditsLimit;    //���� ��������ֵ��>0 �����ƣ�����û����������
	Lint m_MHfeeType;      // 1: ��̯ 2:����
	Lint m_MHfeeCost;      // �û�����
	Lint m_MHMaxCircle;    // �������
   ClubInfos m_clubInfo;
		//...
	//������չ
	DeskInfos(Lint serverid = 0,Lint climit = 0):m_logicServerId(serverid),m_creditsLimit(climit)
	{
		m_MHfeeType = 1;
		m_MHfeeCost = 1;
		m_MHMaxCircle = 0;

	}
};

class DeskManager:public LSingleton<DeskManager>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();
public:
	//����LogicServer ID
	Lint	RecycleDeskId(Lint iDeskId);

	Lint	GetFreeDeskId(Lint nLogicServerID,Lint nInCredits);

	Lint    GetDeskCreditLimit(Lint iDeskId);

	Lint	GetLogicServerIDbyDeskID(Lint nDeskID);

	void	RecycleAllDeskIdOnLogicServer(Lint nLogicServerID);

	void	RecycleAllDeskIdOnLogicServer(Lint nLogicServerID,std::vector<ClubInfos> &clubInfos);

	Lint	GetCoinsDeskSize();
	// ����������count��ʾ��������� deskid��ʾ����ֵ
	void	GetFreeCoinDeskID( int count, std::vector<Lint>& deskid );
	void	RecycleCoinsDeskId( const std::vector<int>& deskid );
	void	RecycleCoinsDeskId();
	Lint	GetShareVideoId();

private:
	boost::mutex  m_mutexDeskId;

	std::queue<Lint>		m_FreeDeskList;
	std::queue<Lint>		m_sharevideoId;
	std::map<Lint, DeskInfos>	m_mapDeskId2LogicServerId;
	std::list<Lint>			m_coinsDeskID;		// ��������б�

	std::set<Lint>             m_KeepDeskIdList;      //�����ģ���������������
	///////////////////////////////////////////////////////////////////////////

	//���ھ��ֲ�����
	boost::mutex              m_mutexClubDeskId;
	std::queue<Lint>		m_FreeClubDeskList;             //���ֲ������Ӻ��б�7λ��
	std::set<Lint>             m_KeepClubDeskIdList;      //�����ģ����������ľ��ֲ�����

	std::map<Lint, ClubInfos>	m_mapClubDeskId2ClubInfo;     //���ֲ�������Ϣ  key=clubDeskId

	//��������ֲ���6λ���Ӻ�
	std::set<Lint>           m_DeskIdBelongToClub;             //���ֲ���ʵ�����б�

	
public:
	void   initClubDeskList();
	Lint   GetFreeClubDeskIds(Lint clubId, Lint playTypeId,Lint deskIdCount, std::set<Lint>& clubDeskIds);
	bool   GetClubInfoByClubDeskId(Lint clubDeskId, ClubInfos& clubInfo);
	void   SetDeskId2LogicServerId(Lint deskId, Lint serverId, Lint creditsLimit, Lint  MHfeeType, Lint MHfeeCost, Lint MHMaxCircle);

	void  SetClubDeskIds(Lint clubId, Lint playTypeId, Lint deskIdCount, const std::vector<Lint>& clubDeskIds);

	//���ֲ������õ��ĺ���,һ����һ��server��������deskid�� ���������ʱ����   by wyz 20171121
	Lint  GetFreeDeskIds(Lint nServerID, Lint nInCredits, Lint requestCount, std::vector<Lint> & deskIds);

	//������ɸ����ֲ���6λ���Ӻ�
	Lint	 ClubGetFreeDeskId(Lint nServerID, Lint nInCredits,Lint clubId,Lint playTypeId,Lint clubDeskId);

	bool IsBelongToClub(Lint deskId);
	//Lint	RecycleDeskIdBelongToClub(Lint iDeskId);

	//logic ����managerͬ����Ϣ
	void   SynchroLogicServerDeskInfo(std::vector<LogicDeskInfo> deskInfo, Lint serverId);

	///////////////////////////////////////////////////////////////////////////
};
#define gDeskManager DeskManager::Instance()

#endif