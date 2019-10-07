#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "..\LogicQipai\LMsgPoker.h"
#include "LActive.h"

#include "SafeResourceLock.h"

#include "clubManager.h"

enum USER_CLUB_STATUS
{
	USER_CLUB_INIT_STATUS = 0,
	USER_CLUB_PIPEI_STATUS = 1,
	USER_CLUB_DESK_STATUS = 2,
};

class User : public CResourceLock
{
public:
	User(LUser data, Lint gateId);
	virtual ~User();

	virtual void	Tick();

	//��ȡ������ݿ���id
	Lint	GetUserDataId();

	void	SetUserGateId(Lint gateId);
	Lint	GetUserGateId();

	bool	GetOnline();

	void	Login();
	void	Logout();

	void	Send(LMsg& msg);
	void	Send(const LBuffPtr& buff);

	void	SendLoginInfo(Lint lastLoginTime);
	void	SendItemInfo();
	void	SendHorseInfo();

	void    SendFreeInfo();
	void    SendExchInfo();

	void	S2CeUserLogin();
	void	S2CeUserLogout();

	void	HanderMsg(LMsg* msg);
	//��Ҵ�������
	void	HanderUserCreateDesk(LMsgC2SCreateDesk* msg);
	//ʵ����Ҵ�������
	void	RealDoHanderUserCreateDesk(LMsgCe2LUpdateCoin* msg);
	//��Ҵ�������Ϊ�������
	void	HanderUserCreateDeskForOther(LMsgC2SCreateDeskForOther* msg);

	//��Ҵ�������Ϊ�������
	void	RealDoHanderUserCreateDeskForOther(LMsgCe2LUpdateCoin* msg);

	//��ȡ��Ҵ��������б�
	void	HanderUserCreatedDeskListForOther(LMsgC2SDeskList* msg);

	//��ȡ��Ҵ��������б�
	void	HanderUserDismissCreatedDeskForOther(LMsgC2SCreaterResetRoom* msg);

	//�᳤�����ɢ���ֲ�����
	void    HanderUserDismissClubDeskWithMaster(LMsgC2SClubMasterResetRoom*msg);

	//��Ҽ��뷿��
	void   HanderUserJoinRoomCheck(LMsgC2SJoinRoomCheck*msg);

	//��ҽ��뷿��
	void	HanderUserAddDesk(LMsgC2SAddDesk* msg,Lint addDeskFlag =0);

	//��ҽ��뷿��
	void	RealDoHanderUserAddDesk(LMsgCe2LUpdateCoinJoinDesk* msg);

	//�����������
	void	HanderUserEnterCoinsDesk(LMsgC2SEnterCoinsDesk* msg);
	//��ѯ����GPS��Ϣ
	void    HanderUserQueryRoomGPSInfo(MHLMsgC2SQueryRoomGPSLimitInfo * msg);

	
	//�������ս��
	void	HanderUserVipLog(LMsgC2SVipLog* msg);

	void    HanderUserCreLog(LMsgC2SCRELog* msg);

	void    HanderUserReqGTU(LMsgC2SREQ_GTU* msg);

	void    HanderUserReqEXCH(LMsgC2SREQ_Exch* msg);

	void	HanderUserRoomLog(LMsgC2SRoomLog* msg);

	//�������¼��
	void	HanderUserVideo(LMsgC2SVideo* msg);

	//��ȡָ�������Ϣ
	void	HanderGetUserInfo(LMsgC2SGetUserInfo* msg);

	void    HanderBindInviter(LMsgC2SBindInviter* msg);

	void    HanderGetInvitingInfo(LMsgC2SGetInvitingInfo *msg);

	// ����
	void    HanderActivityPhone(LMsgC2SActivityPhone *msg);
	void    HanderActivityRequestLog(LMsgC2SActivityRequestLog *msg);
	void    HanderActivityRequestDrawOpen(LMsgC2SActivityRequestDrawOpen *msg);
	void    HanderActivityRequestDraw(LMsgC2SActivityRequestDraw *msg);
	void    HanderActivityRequestShare(LMsgC2SActivityRequestShare *msg);

	//��������ȡĳ����id¼��
	void    HanderUserShareVideo(LMsgC2SShareVideo* msg);

	//����������ĳ¼��
	void    HanderUserReqShareVideo(LMsgC2SReqShareVideo* msg);

	//��������ȡ�����¼��
	void	HanderUserGetShareVideo(LMsgC2SGetShareVideo* msg); 

	Lstring GetIp();

	void	SetIp(Lstring& ip);
	
	//������ز���
	//��ȡ��������
	bool	IfCardEnough(Lint cardType, Lint num);
	//���ӷ���
	void	AddCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, bool bNeedSave = true);
	//ɾ������
	void	DelCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin, bool bNeedSave = true);
	//�޸���Ҵ�ľ�������������
	void	AddPlayCount();

	//static void	AddCardCount(Lint id, Lstring strUUID, Lint cardType, Lint count, Lint operType, const Lstring& admin);
	void    AddCreditForUserlist(const std::vector<Lint> &userList);

	bool    DealExchangeCard(Lint inCard);

	void	AddCoinsCount(Lint count, Lint operType, bool bNeedSave = true);
	void	DelCoinsCount(Lint count, Lint operType, bool bNeedSave = true);

	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue){m_userState = nValue;}
	Lint	getUserLogicID(){return m_logicID;}
	void	setUserLogicID(Lint nValue){m_logicID = nValue;}
	Lint	getUserGateID(){return m_gateId;}
	void	setUserGateID(Lint nValue){m_gateId = nValue;}
	Lstring&	getUserIPStr(){return m_ip;}

	Lint	getLastCreateGameType() { return m_lastCreateGameType; }
	/////////////////////////////////////////////////////////////////////////////
	//���ھ��ֲ�
	//������ֲ�֮ǰ��鷿��
	void   HanderUserAddDeskBeforeCheckFee(LMsgC2SAddDesk* msg,Lint addDeskType=0);

	void HanderUserPokerRoomLog(LMsgC2SQiPaiLog* msg);

	//���������ֲ�����
	void  HanderUserJoinRoomAnonymous(MHLMsgC2SJoinRoomAnonymous *msg);
	//���ֲ�����������ƥ�������ֲ���ʽ
	void  HanderUserJoinRoomAnonymous2CheckFee(MHLMsgC2SJoinRoomAnonymous *msg);
	void  HanderUserJoinRoomAnonymous2(LMsgCe2LUpdateCoinJoinDesk *msg);
	void  HanderQiPaiUserLeaveWait(LMsgC2SPokerExitWait *msg);

	//����-����������ս��
	void HanderQiPaiUserReqDraw(LMsgC2SMatchLog *msg);
	void HanderQiPaiUserReqLog(LMsgC2SPokerVideo *msg);

	//

	void UpdateUserData(const LUser & userData);

	//�������ֲ�����
	void HanderUserCreateClubDesk(LMsgC2SClubUserCreateRoom* msg);
	void RealDoHanderUserCreateClubDesk(LMsgCe2LUpdateCoinJoinDesk* msg);


	void HanderUserRequestClubOnLineCount(LMsgC2SRequestClubOnLineCount *msg);

	//���ֲ������Ա
	void HanderUserRequestClubMemberOnlineNoGame(LMsgC2SRequestClubMemberOnlineNogame *msg);

	void HanderUserInviteClubMember(LMsgC2SInviteClubMember *msg);

	void HanderUserReplyClubMemberInvite(LMsgC2SReplyClubMemberInvite *msg);

	//����ֵ
	void HanderUserPointList(LMsgC2SPowerPointList* msg);
	
	/////////////////////////////////////////////////////////////////////////////
	void setUserClubStatus(Lint status) { m_clubStatus = status; }
	Lint getUserClubStatus() { return m_clubStatus; }
	
public:
	LUser	m_userData;					//�������
	std::vector< LActive >	m_Active;	//���Ϣ
	Lint	m_gateId;					//������ĸ�gate����
	Lint	m_logicID;					//������ĸ�logic����
	Lint	m_userState;				//��ҵ�ǰ״̬
	bool	m_online;
	
	Lstring	m_ip;

	Lint m_lastCreateGameType;			//������һ�δ�������Ϸ����

	Lint m_clubStatus;


};

class UserBaseInfo : public CResourceLock
{
public:
	UserBaseInfo()
	{
		m_id  = 0;
		m_sex = 1;
	}
	UserBaseInfo(const UserIdInfo& info)
	{
		copy(info);
	}

	virtual ~UserBaseInfo()
	{

	}
public:
	UserBaseInfo& operator =(const UserIdInfo& info)
	{
		copy(info);
		return *this;
	}

	void copy(const UserIdInfo& info)
	{
		m_id			= info.m_id;
		m_openId		= info.m_openId;
		m_unionId       = info.m_unionId;
		m_nike			= info.m_nike;
		m_headImageUrl	= info.m_headImageUrl;
		m_sex			= info.m_sex;
	}
public:
	Lint		m_id;
	Lstring		m_openId;
	Lstring		m_unionId;
	Lstring		m_nike;
	Lstring		m_headImageUrl;
	Lint		m_sex;
};

class UserLoginInfo : public CResourceLock
{
public:
	UserLoginInfo()
	{
		m_seed = 0;
		m_time = 0;
	}
	UserLoginInfo(Lint iSeed, Lint iTime, LUser& user)
	{
		m_seed = iSeed;
		m_time = iTime;
		m_user = user;
	}

	virtual ~UserLoginInfo()
	{

	}

public:
	Lint	m_seed;
	Lint	m_time;
	LUser	m_user;
};

class UserDeskList : public CResourceLock
{
public:
	UserDeskList()
	{
		m_cost = 0;
	}


	virtual ~UserDeskList()
	{

	}

public:
	std::list<int> m_DeskList;
	Lint m_cost;
};


#endif