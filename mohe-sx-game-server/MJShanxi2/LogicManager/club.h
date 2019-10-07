#ifndef _CLUB_H_
#define _CLUB_H_

#include "LBase.h"
#include "LMsgS2S.h"
#include "Work.h"
#include "DeskManager.h"
#include "SafeResourceLock.h"
#include "LTime.h"
#include "clubDesk.h"


//ǰ������
//class User;
class UserBaseInfo;
struct clubFeeType;
struct UserAddToClubDeskInfo;


#define   MAX_DESK_COUNT_EVERY_TYPE  100
#define  MAX_PLAY_TYPE_SIZE_EACH_CLUB   3

#define FIRST_TABLE_RED_PACKET_COUNT 5

struct First_Table_Red_Packet
{
	std::set<Lint>             m_redPacketDesk;            

	Lint                                m_showRedPacketDesk;              //��ʾ���������

	First_Table_Red_Packet()
	{
		
		m_showRedPacketDesk = 0;
	}

	~First_Table_Red_Packet(){}


	Lint getRedPacketDeskId()
	{
		return m_showRedPacketDesk;
	}

	void setRedPacketDeskId(Lint clubDeskId)
	{
		m_showRedPacketDesk = clubDeskId;
	}

	void unsetRedPacketDeskId()
	{
		m_redPacketDesk.insert(m_showRedPacketDesk);
		m_showRedPacketDesk = 0;

	}

	void clearRedPacketDesk()
	{
		m_redPacketDesk.clear();
	}

	bool isDeskRedPacket(Lint clubDeskId)
	{
		if (m_showRedPacketDesk == clubDeskId)return true;
		if (m_redPacketDesk.find(clubDeskId) != m_redPacketDesk.end())
		{
			return true;
		}
		return false;
	}


};

struct Red_Packet
{

	Red_Packet()
	{
		setRedPacketCount(FIRST_TABLE_RED_PACKET_COUNT);
		m_clubId = 0;
	}

	~Red_Packet() {};

	void init(Lint clubId)
	{
		m_clubId = clubId;
	}

	void setRedPacketCount(Lint redPacketCount)
	{
		m_count = redPacketCount;
	}

	bool haveRedPacket(LTime & cur)
	{
		if (cur.GetDate() >= m_time.GetDate() + 1)
		{
			m_count = FIRST_TABLE_RED_PACKET_COUNT;
			m_time = cur;
			//��շ������������
			for (auto ItFirstRedPacket = m_mapPlayTypeId2RedPacket.begin(); ItFirstRedPacket != m_mapPlayTypeId2RedPacket.end(); ItFirstRedPacket++)
			{
				ItFirstRedPacket->second.clearRedPacketDesk();
			}
			
		}
		LLOG_DEBUG("haveRedPacket: club[%d] curDate[%d],redTime[%d] redCount[%d]", m_clubId,cur.GetDate(), m_time.GetDate(), m_count);
		return m_count > 0 ? true : false;
	}

	void addFirstTableRedPacket(Lint playId)
	{
		auto ItRed = m_mapPlayTypeId2RedPacket.find(playId);
		if (ItRed == m_mapPlayTypeId2RedPacket.end())
		{
			First_Table_Red_Packet temp;
			m_mapPlayTypeId2RedPacket[playId] = temp;
		}
	}

	Lint  getRedPacketDeskId(Lint playId)
	{
		auto ItTableRed = m_mapPlayTypeId2RedPacket.find(playId);
		if (ItTableRed != m_mapPlayTypeId2RedPacket.end())
		{
			return ItTableRed->second.getRedPacketDeskId();
		}

		return -1;
	}

	void setRedPacketDeskId(Lint playId, Lint clubDeskId)
	{
		auto ItTableRed = m_mapPlayTypeId2RedPacket.find(playId);
		if (ItTableRed != m_mapPlayTypeId2RedPacket.end())
		{
			m_count--;
			LLOG_DEBUG("setRedPacketDeskId club[%d] redPacaketCount[%d]", m_clubId, m_count);

			return ItTableRed->second.setRedPacketDeskId(clubDeskId);
		}
		

	}

	void unsetRedPacketDeskId(Lint playId,Lint status=0)
	{
		if (status == 2)
		{
			m_count++;
			return;
		}
		auto ItTableRed = m_mapPlayTypeId2RedPacket.find(playId);
		if (ItTableRed != m_mapPlayTypeId2RedPacket.end())
		{
			 ItTableRed->second.unsetRedPacketDeskId();
		}
		if (status == 1)
		{
			m_count++;
		}
		return;
	}

	bool isDeskRedPacket(Lint playId, Lint clubDeskId)
	{
		if (clubDeskId == 0 || playId == 0)return false;
		auto ItTableRed = m_mapPlayTypeId2RedPacket.find(playId);
		if (ItTableRed != m_mapPlayTypeId2RedPacket.end())
		{
			return ItTableRed->second.isDeskRedPacket(clubDeskId);
		}
		return false;
	}


private:
	LTime   m_time;    //ʱ��
	Lint       m_count;   //�������
	Lint       m_clubId;  //���ֲ�Id

	std::map<Lint, First_Table_Red_Packet>  m_mapPlayTypeId2RedPacket;

};

//���ֲ������û���Ϣ
struct clubDeskUserInfo
{
	Lstring    m_deskUserName;
	Lstring    m_deskUserUrl;
	Lint       m_pos;
};

class Club:public CResourceLock
{
public:
	Club(Lint clubId);
	~Club();
	//���ؾ��ֲ���Ϣ
	void initData(LMsgCe2LAddClub *msg);
	void resetData();
	
	//���ֲ��᳤�Ķ����ӿ�
	bool  addClubPlayType(clubPlayType &  aClubPlayType);
	bool  hidePlayType(Lint playTypeId,Lstring playType,Lint playTypeIdStatus);                                                                                 //�����淨
	bool  alterClubName(Lstring newClubName);                                                             //�޸ľ��ֲ�����	
	bool  alterPlayTypeName(Lint playType, Lstring newPlayTypeName);

    bool  modifyClubFeeType(Lint feeType);

	//��������
	bool  isClubValid() { m_valid==0?false:true; }

private:
	
	bool _addClubPlayType(Lint playId);
	bool _addClubPlayTypeWithDesk(Lint playId, Lint deskCount, std::vector<ManagerClubDeskInfo>& clubdeskList);
	Lint _createClubDeskList(Lint deskCount, Lint playId);                                                                                //�������ֲ������б�
	Lint _fillClubDeskList(const std::vector<ManagerClubDeskInfo> &clubDeskinfo, Lint playId);



	//���ֲ���Ա��Ϊ�ӿ�
public:

	//���ؿͻ���ˢ�½��������
	//std::deque<Lint>  reorderClubDesk(Lint playId,std::list<Lint> clubDeskList);

	//�᳤�͹���Ա
	Lint getClubOwerId()
	{
		return m_ownerId;
	}
	Lstring  getClubOwerUUID()
	{
		return m_ownerUUioid;
	}
	Lstring  getClubOwerNike()
	{
		return m_ownerNike;
	}
	Lstring  getClubOwerPhoto()
	{
		return m_ownerHeadUrl;
	}
	void getClubOwnerAndAdmin(Lint *owner, std::set<Lint> &admin)  //��ȡ�᳤�͹���Ա
	{
		*owner = getClubOwerId();
		admin = m_adminUser;
	}
	//�ж��Ƿ��ǹ���Ա���߻᳤
	bool   isLeader(Lint userId) { if (userId == getClubOwerId() || m_adminUser.find(userId) != m_adminUser.end()) return true; return false; }
	Lint   setAdmin(Lint type, Lint setUserId, Lint adminUserId);

	//���ֲ�����
	Lint getClubId()
	{
		return m_id;
	}
	Lint getClubSq()
	{
		return m_sq;
	}
	Lstring getClubName() { return m_name; }
	//���ֲ�id ������
	clubInfo& getClubInfo()
	{
		clubInfo  info(getClubId());
		info.m_name = getClubName();
		info.m_clubType = getClubType();
		return info;
	}
	///��������
	Lint getClubFee()
	{
		return m_feeType;
	}
	///����
	Lint  getCard() { return m_coin; }
	void setCard(Lint coin) { m_coin = coin; }
	///���ֲ�����
	Lint getClubType() { return m_clubType; }
	void setClubType(Lint clubType) { 
		m_clubType = clubType; 
		if (m_clubType == 1)  //�����Ϊ����������С���뷿�����100
		{
			gClubPlayManager.updatePlayType(getCurrPlayId(), 100, 1);
		}
	
	}
	///����ԱȨ��
	Lint getAdminCanChange() { return m_adminAuth.m_changePoint ; }
	void setAdminCanChange(Lint info) { m_adminAuth.m_changePoint = info; }

	Lint getAdminCanSeeAll() { return m_adminAuth.m_lookupPoint; }
	void setAdminCanSeeAll(Lint info) { m_adminAuth.m_lookupPoint = info; }

	//��ǰ�淨
	bool  havePlayId()
	{
		return !m_showPlayType.empty();
	}
	Lint getCurrPlayId()
	{
		if (m_showPlayType.empty())return 0;
		return *(m_showPlayType.begin());
	}
	////////////////////////////////////////////////////////////////////////
	//��ȡ�淨�����б�
	std::list<Lint>  getClubDeskList();                                                      //��ȡ��ǰ���ֲ�չʾ���ӵ��б�
	Lint getCurrDeskCount() { return getClubDeskList().size(); }
	bool ChangeShowDesk(Lint deskCount);                                        //�ı䵱ǰչʾ���ӵĸ���
	Lint  getNotEmptyDeskCount();
	
private:
	//������ӣ�����showdeskId ���������ֵΪ0 ������ʧ��
	Lint insertClubDesk(Lint clubDesk)
	{
		for (auto ItDesk = m_map2ClubDesk.begin(); ItDesk != m_map2ClubDesk.end(); ItDesk++)
		{
			if (ItDesk->second == clubDesk)return 0;
		}
		Lint key = m_map2ClubDesk.size()+1;
		m_map2ClubDesk[key] = clubDesk;
		m_showDeskList.insert(key);
		return key;
	}


////////////////////////////////////////////////////////////////////////
//�᳤�͹���Ա�Ѿ�Ȩ��
private:
	Lint   _setAdmin(Lint type, Lint setUserId, Lint adminUserId);
	Lint  _delAdmin(Lint type, Lint setUserId, Lint adminUserId);


/////////////////////////////////////////////////////////////////////////
//������ֵ����  -��δʹ�ã�ֱ�Ӵ����ݿ��ѯ�������Ż�Ч�ʿ�����
public:
	void  LogPowerRecord(Lint type,boost::shared_ptr<UserPowerRecord> change);
	void  delPowerRecord();

	//�ܷ��ѯ�����б�  0==����   1==������   2==ֻ�ܿ��Լ�
	Lint  canPowPointList(Lint userId)
	{
		if (userId == getClubOwerId())return 1;
		if (m_adminUser.find(userId) == m_adminUser.end())return 0;
		if ( m_adminAuth.m_changePoint==1 &&  m_adminUser.find(userId) != m_adminUser.end())	return 1;
		if (m_adminAuth.m_lookupPoint == 1 && m_adminUser.find(userId) != m_adminUser.end())return 1;
		if (m_adminAuth.m_changePoint == 0 && m_adminAuth.m_lookupPoint == 0 && m_adminUser.find(userId) != m_adminUser.end())return 2;
		return 0;
	}
	//�ܷ�ı��û�����
	bool canChangePoint(Lint userId)
	{
		if (userId == getClubOwerId())return true;
		if (m_adminAuth.m_changePoint== 1)
		{
			if (m_adminUser.find(userId) != m_adminUser.end())
			{
				return true;
			}
		}
		return false;
	}
	//��ȡ��¼ҳ�������
	Lint  getRecordMode(Lint userId)
	{
		if (userId == getClubOwerId())
			return 3;
		if (m_adminAuth.m_lookupPoint == 1 && m_adminUser.find(userId) != m_adminUser.end())
			return 3;
		if (m_adminAuth.m_changePoint == 1 && m_adminUser.find(userId) != m_adminUser.end())
			return 3;
		if (m_adminAuth.m_lookupPoint == 0 && m_adminUser.find(userId) != m_adminUser.end())
			return 2;
		return 1;
	}

	//��ȡ�û������ܽ���¼
	std::list<boost::shared_ptr<UserPowerRecord> >   getUserRecord(Lint userId,Lint type= CHANGE_POINT_TYPE_ADD)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		if (CHANGE_POINT_TYPE_ADD == type)
		{
			record = _getUserAddRecord(userId);
		}
		else if (CHANGE_POINT_TYPE_REDUCE == type)
		{
			record = _getUserReduceRecord(userId);
		}
		return record;
	}
	//��ȡ�û����������ı仯
	std::list<boost::shared_ptr<UserPowerRecord> > getUserOperRecord(Lint userId, Lint type = CHANGE_POINT_TYPE_ADD)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		if (CHANGE_POINT_TYPE_ADD == type)
		{
			record = _getUserOperAddRecord(userId);
		}
		else if (CHANGE_POINT_TYPE_REDUCE == type)
		{
			record = _getUserOperReduceRecord(userId);
		}
		return record;
	}

	std::list<boost::shared_ptr<UserPowerRecord> > getClubOperRecord(Lint type = CHANGE_POINT_TYPE_ADD)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		if (CHANGE_POINT_TYPE_ADD == type)
		{
			record= m_PowerAddRecord;
		}
		else if (CHANGE_POINT_TYPE_REDUCE == type)
		{
			record= m_PowerReduceRecord;
		}
		return record;
	}

private:
	std::list<boost::shared_ptr<UserPowerRecord> >  _getUserAddRecord(Lint userId)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		auto ItRecord = m_mapUserRecordAdded.find(userId);
		if (ItRecord != m_mapUserRecordAdded.end())
		{
			record = ItRecord->second;
		}
		return record;
	}
	std::list<boost::shared_ptr<UserPowerRecord> >  _getUserReduceRecord(Lint userId)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		auto ItRecord = m_mapUserRecordReduced.find(userId);
		if (ItRecord != m_mapUserRecordReduced.end())
		{
			record = ItRecord->second;
		}
		return record;
	}
	
	std::list<boost::shared_ptr<UserPowerRecord> >  _getUserOperAddRecord(Lint userId)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		auto ItRecord = m_mapUserAddOperRecord.find(userId);
		if (ItRecord != m_mapUserAddOperRecord.end())
		{
			record = ItRecord->second;
		}
		return record;
	}
	std::list<boost::shared_ptr<UserPowerRecord> >  _getUserOperReduceRecord(Lint userId)
	{
		std::list<boost::shared_ptr<UserPowerRecord> > record;
		auto ItRecord = m_mapUserReduceOperRecord.find(userId);
		if (ItRecord != m_mapUserReduceOperRecord.end())
		{
			record = ItRecord->second;
		}
		return record;
	}




	//�ڲ��õĺ���
private:
	void _clearData();
	//�ͻ�����ʾ���ŵĹ���
	Lint  getShowDeskId(Lint sq);



private:
	//���ֲ�����
	Lint m_sq;                                              //���ֲ������ݿ��id
	Lint m_id;                                             // ���ֲ�ID
	Lstring m_name;                                //���ֲ�����
	Lint m_coin;                                        //�����᳤���  ��ɶ��

	Lint m_ownerId;                               // �᳤ID
	Lstring m_ownerNike;                    //�᳤�ǳ�
	Lstring m_ownerHeadUrl;             //�᳤ͷ��
	Lstring m_ownerUUioid;                 //�᳤

	std::set<Lint>  m_adminUser;            //����Ա

	Lint m_userSum;                               //��ɶ�û���������
	Lint m_onLineUserCount;            //���ֲ���������
	Lstring m_clubUrl;                          //���ֲ�ͷ��url
	Lint m_status;                                  //���ֲ�״̬   
	Lint m_type;                                     //���ֲ�����
	Lint m_feeType;                              //���ֲ��շ�����
	Lint m_valid;                                    //0-δ����״̬, 1-����״̬
	Lint m_maxMedalPointId;							//���ֲ������Ӯ������ֵ�����ID
	Lint m_maxMedalPoint;							//���ֲ�����Ӯ������ֵ����

	Lint m_clubType;                      //���ֲ�����   //���ͣ�0-��ͨ   1-������
	//Lint m_adminCanChange;      // // ����Ա���Ե�������   0-û��ѡ��  1-ѡ��
	//Lint m_adminCanSeeAll;     ////����Ա���Բ�ѯ���������仯ԭ��  0-û��  1-ѡ��

	ClubAdminAuth  m_adminAuth;         //����ԱȨ��

	std::set<Lint>     m_showDeskList;                       //��ǰ��ʾ���������� value = showdeskId
	std::map<Lint, Lint>      m_map2ClubDesk;          //key=showdeskId ,value=clubDeskId;
	//std::list<Lint>    m_clubDeskList;                             //���ֲ�����

	////���ֲ��淨
	std::set<Lint>                                                          m_playType;                            //���е��淨������+��ʾ
	std::set<Lint>                                                       m_showPlayType;                 //����չʾ�淨                 value=playTypeId   ����չʾ���淨

	
	//������ֵ
	std::list<boost::shared_ptr<UserPowerRecord> >   m_PowerAddRecord;              //�Ӽ�¼
	std::map<Lint, std::list<boost::shared_ptr<UserPowerRecord> > >   m_mapUserRecordAdded;   //�û�����������¼
	std::map<Lint, std::list<boost::shared_ptr<UserPowerRecord> > >   m_mapUserAddOperRecord;       //�û���������¼   ����Ա �ͻ᳤

	std::list<boost::shared_ptr<UserPowerRecord> >   m_PowerReduceRecord;        //����¼
	std::map<Lint, std::list<boost::shared_ptr<UserPowerRecord> > >   m_mapUserRecordReduced;   //�û�����������¼
	std::map<Lint, std::list<boost::shared_ptr<UserPowerRecord> > >   m_mapUserReduceOperRecord;       //�û���������¼   ����Ա �ͻ᳤

	//���
	Red_Packet                        m_redPacket;


};



#endif