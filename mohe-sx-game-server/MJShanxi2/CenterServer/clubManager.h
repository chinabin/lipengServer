#ifndef _CLUB_MANAGER_H_
#define _CLUB_MANAGER_H_

#include "LSingleton.h"
#include "LBase.h"
#include "LMsgS2S.h"


class ClubManager : public LSingleton<ClubManager>
{
public:
	ClubManager();
	~ClubManager();

	virtual	bool		Init();
	virtual	bool		Final();

public:
	void    resetData();

public:
	bool    loadClubData();
	bool    loadClubInfoFromDB();
	bool    loadClubPlayTypesFromDB();
	bool    loadClubUsersFromDB();
	bool    loadClubAdminAuthFromDB();   //��Ҫ���ع���ԱȨ�ޣ��ڼ��ؾ��ֲ���Ϣ�Ժ�

public:
	bool   sendClubDataToLogicManager(Lint serverID);
public:
	bool    addClubDataWithPlayTypesAndUsers();

public:
	void   updateClubDeskInfo(LMsgLMG2CEUpdateData *pdata);       //manager ->centen ͬ�����ֲ�������Ϣ
	void   updateClubUserPoint(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg);  //ÿ����Ϸ�����û�����ֵ
	void   updatePointRecord(LMsgLMG2CEUpdatePointRecord*msg);                   //�᳤�����Ա�����û�����ֵ 
	void   updateClubInfo(LMsgCE2LMGClubInfo *msg);
	void   updatePlayInfo(LMsgCE2LMGPlayInfo *msg);
																
	bool   userRequestRecordLog(LMsgLMG2CERecordLog  *msg);      //��ѯ��¼
	bool  _loadRecordLog(LMsgLMG2CERecordLog  *msg, std::vector<UserPowerRecord>& recordList);


public:
	bool	addClub(Lint id);
	bool    addClubPlayType(Lint id);
	bool    addClubUser(Lint id);
	bool    delClubUser(Lint id, Lint clubId, Lint userId,Lint type);
	bool    hideClubPlayType(Lint id);
	bool    alterClubName(Lint id);
    bool    modifyClubFeeType(Lint id);
	bool    alterPlayTypeName(Lint id);
	bool    userApplyClub(Lint clubId);
	//��������þ��ֲ����һ�δ��ͬ������
	bool	calcApplayClubUser(Lint clubId);

	bool     Excute(const std::string& str);

	boost::shared_ptr<ClubItem> getClubItemByClubId(Lint clubId);

protected:
	bool	_loadClubInfoFromDB(Lint iClubCount);
	bool    _loadClubPlayTypesFromDB(Lint iPlayTypesCount);
	bool    _loadClubPlayUserFromDB(Lint iUserCount);
	bool    _updateClubUserPoint(Lint userId, Lint clubId, Lint point);
	bool   _updateClubUserPointSql(Lint userId, Lint clubId, Lint point);
	bool   _updateClubUserPointMemory(Lint userId, Lint clubId, Lint point);



private:
	boost::mutex m_mutexQueue;

	std::map<Lint, boost::shared_ptr<ClubItem>> m_mapId2Club;         //���ֲ�  key=clubId
	std::map<Lint, boost::shared_ptr<ClubItem>> m_mapclubId2Club;         //���ֲ�  key=id 
	std::list<boost::shared_ptr<clubPlayType>>  m_playTypeList;          //�淨
	std::list<boost::shared_ptr<clubUser>>      m_clubUserList;                //�û�

};

#define gClubManager ClubManager::Instance()

#endif
