#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LActive.h"
#include "LTime.h"
#include "mhmsghead.h"
class Desk;

class  User
{
public:
	User(LUser data, Lint gateId);
	virtual ~User();

	//��ȡ������ݿ��� id
	Lint	GetUserDataId();

	const LUser& GetUserData() const { return m_userData; };

	void	SetUserGateId(Lint gateId);
	Lint	GetUserGateId();

	bool	GetOnline();
	void	SendLoginInfo(Lstring& buyInfo, Lint hide);
	void	SendItemInfo();

	bool	Login();
	void	Logout();

	void	Send(LMsg& msg);

	Desk*	GetDesk();
	void	SetDesk(Desk* desk);

	void	HanderMsg(LMsg* msg);
	//��Ҵ�������
	Lint	HanderUserCreateDesk(LMsgLMG2LCreateDesk* msg);

	Lint    HanderUserCreateDeskForOther(LMsgLMG2LCreateDeskForOther* msg);

	//Lint    HanderUserCreateDeskForClub(LMsgLMG2LCreateDeskForClub* msg);

	//��ҽ��뷿��
	Lint	HanderUserAddDesk(Lint nDeskID);


	//��ҽ��뷿��
	Lint	HanderUserAddDesk(LMsgLMG2LAddToDesk* msg);

	//����˳�����
	void	HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg);

	//���ֲ�
	Lint	HanderUserAddClubDesk(LMsgLMG2LEnterClubDesk* msg);


	//����������
	void	HanderUserPlayCard(LMsgC2SUserPlay* msg);

	//�������˼��
	void	HanderUserThink(LMsgC2SUserOper* msg);

	//���ѡ�����ֺ�
	void	HanderStartHuSelect(LMsgC2SUserStartHuSelect* msg);

	//���ѡ�񺣵���
	void	HanderEndCardSelect(LMsgC2SUserEndCardSelect* msg);

	//�������֪ͨ����
	void	HanderUserReady(LMsgC2SUserReady* msg);

	//���ѡ����λ
	void	HanderUserSelectSeat(LMsgC2SUserSelectSeat* msg);

	//��ҿ�ʼ��ť
	void    HanderUserStartGameButton(LMsgC2SStartGame*msg);

	//������㷿��
	void	HanderResutDesk(LMsgC2SResetDesk* msg);

	//���ѡ��������
	void	HanderSelectResutDesk(LMsgC2SSelectResetDesk* msg);

	//�����������
	void	HanderUserSpeak(LMsgC2SUserSpeak* msg);

	// ��һ���
	void HanderUserChange(LMsgC2SUserChange* msg);

	void HanderUserTangReq(LMsgC2STangCard* msg);

	void HanderUserAIOper(LMsgC2SUserAIOper* msg);

	void HanderSendVideoInvitation(LMsgC2SSendVideoInvitation* msg);

	void HanderInbusyVideoInvitation(LMsgC2SReceiveVideoInvitation* msg);

	void HanderOnlineVideoInvitation(LMsgC2SOnlineVideoInvitation* msg);

	void HanderShutDownVideoInvitation(LMsgC2SShutDownVideoInvitation* msg);

	void HanderUploadGpsInformation(LMsgC2SUploadGPSInformation* msg);

	void HanderUploadVideoPermission(LMsgC2SUploadVideoPermission* msg);

	Lstring GetIp();

	void	SetIp(Lstring& ip);

	//���ӷ���
	void	AddCardCount(Lint cardType, Lint count,Lint operType,Lstring admin, bool bNeedSave = true);

	static void    AddCardCount(Lint id, Lstring strUUID, Lint cardType, Lint count, Lint operType,Lstring admin);
	//ɾ������
	void	DelCardCount(Lint cardType, Lint count, Lint operType, Lstring admin,Lint feeTye);

	//�ı����״̬
	void	ModifyUserState(bool bLeaveServer = false);
	Lint	getUserState(){return m_userState;}
	void	setUserState(Lint nValue){m_userState = nValue;}
	Lint	getUserGateID(){return m_gateId;}
	void	setUserGateID(Lint nValue){m_gateId = nValue;}
	inline void    updateInRoomTime() { m_timeInRoom = LTime(); }
	inline bool   IsInRoomFor15Second() { return  m_timeInRoom.Secs() > 0 && (LTime().Secs() - m_timeInRoom.Secs()) > 15 ; }
	inline void   ClearInRoomTime() { m_timeInRoom.SetSecs(0); }
	bool IsInLimitGPSPosition(User & u);	
public:
	LUser	m_userData;
	Lint	m_gateId;		//������ĸ�gate����
	Lint	m_userState;	//��ҵ�ǰ״̬
	Lint    m_videoPermission; //����video״̬
	bool	m_online;
	std::vector< LActive >	m_Active;
	Desk*	m_desk;
	Lstring	m_ip;
	LTime   m_timeInRoom;   //���뷿��ʱ��
	double   m_gps_lng;      //GPS����
	double   m_gps_lat;      //GPSγ��
};

#endif