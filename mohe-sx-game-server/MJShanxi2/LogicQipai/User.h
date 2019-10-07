#ifndef _USER_H_
#define _USER_H_

#include "LUser.h"
#include "LMsgS2S.h"
#include "LMsgPoker.h"
#include "LActive.h"
#include "LTime.h"

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

	//�����ս�������
	void    HanderLookOnUserSeatDown(LMsgG2LLookOnUserSearDown* msg);

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

	//������ҽ������
	void HanderUpdateUserCoins(LMsgLMG2LUpdatePointCoins* msg);

	//����������ҳ�����������µ������ 61184
	void HanderStandPlayerReSeat(LMsgC2SStandPlayerReSeat* msg);

	//��������ս����б���Ϣ
	void HanderViewLookOnList(LMsgC2SViewLookOnList* msg);

	//�����˿���Ϸ�Ľӿ�
	void HanderPokerGameMessage(LMsgC2SGameCommon *msg);
	void HanderYingSanZhangAddScore(YingSanZhangC2SAddScore*msg);
	void HanderYingSanZhangUserSelectBigSmall(YingSanZhangC2SUserSelectBigSmall* msg);
	////��Ͳ��
	void  HanderTuiTongZiAddScore( TuiTongZiC2SScore*msg);
	void  HanderTuiTongZiSelectZhuang(TuiTongZiC2SSelectZhuang*msg);
	void  HanderTuiTongZiDoShaiZi( TuiTongZiC2SDoShaiZi*msg);
	void  HanderTuiTongOpenCard( TuiTongZiC2SOpenCard*msg);

	//ţţ
	void HanderNiuNiuSelectZhuang(NiuNiuC2SSelectZhuang* msg);
	void HanderNiuNiuMaiMa(NiuNiuC2SMaiMa* msg);
	void HanderNiuNiuAddScore(NiuNiuC2SAddScore* msg);
	void HanderNiuNiuOpenCard(NiuNiuC2SOpenCard* msg);

	/*
	ţţ����Ҵ깫��
	MSG_C_2_S_NIUNIU_CUO_GONG_PAI = 62088
	*/
	void HandNiuNiuCuoGongPai(NiuNiuC2SCuoGongPai* msg);

	/*
	ţţ�����ѡ���й���Ϣ
	MSG_C_2_S_NIUNIU_TUO_GUAN = 62084
	*/
	void HandNiuNiuTuoGuan(NiuNiuC2STuoGuan* msg);

	/*
	ţţ�����ȡ���й���Ϣ
	MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN = 62085
	*/
	void HandNiuNiuCancelTuoGuan(NiuNiuC2SCancelTuoGuan* msg);

	/*
	�ͻ���֪ͨ��������ʼ��ʱ
	MSG_C_2_S_START_TIME = 61198
	*/
	void HandNiuNiuStartTime(NiuNiuC2SStartTime* msg);


	//������
	void HanderDouDiZhuGameMessage(MHLMsgDouDiZhuC2SGameMessage * msg);

	/*
	˫����C->S ��ұ�������
	MSG_C_2_S_SHUANGSHENG_SELECT_ZHU = 62202
	*/
	void HandleShuangShengSelectZhu(ShuangShengC2SSelectZhu* msg);

	/*
	˫����C->S ��ҷ�������
	MSG_C_2_S_SHUANGSHENG_FAN_ZHU = 62204
	*/
	void HandleShangShengFanZhu(ShuangShengC2SFanZhu* msg);

	/*
	˫����C->S ��Ҹǵ��Ʋ���
	MSG_C_2_S_SHUANGSHENG_BASE_CARDS = 62206
	*/
	void HandleShuangShengBaseCards(ShuangShengC2SBaseCards* msg);

	/*
	˫����C->S ��ҳ��Ʋ���
	MSG_C_2_S_SHUANGSHENG_OUT_CARDS = 62208
	*/
	void HandleShuangShengOutCards(ShuangShengC2SOutCards* msg);

	/*
	3��2��C->S ���ѡ����Ʒ�
	MSG_C_2_S_SANDAER_SELECT_SCORE = 62253
	*/
	void HandleSanDaErSelectScore(SanDaErC2SSelectScore* msg);

	/*
	3��2��C->S ���ѡ����
	MSG_C_2_S_SANDAER_SELECT_MAIN = 62255
	*/
	void HandleSanDaErSelectMain(SanDaErC2SSelectMain* msg);

	/*
	3��2��C->S ������
	MSG_C_2_S_SANDAER_BASE_CARD = 62257
	*/
	void HandleSanDaErBaseCard(SanDaErC2SBaseCard* msg);

	/*
	3��2��C->S ���ѡ�Լ���
	MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259
	*/
	void HandleSanDaErSelectFriend(SanDaErC2SSelectFriend* msg);

	/*
	3��2��C->S ��ҳ���
	MSG_C_2_S_SANDAER_OUT_CARD = 62261
	*/
	void HandleSanDaErOutCard(SanDaErC2SOutCard* msg);

	/*
	3��2��C->S ��һظ��мҵ÷���105��,�Ƿ�ֹͣ��Ϸ
	MSG_C_2_S_SANDAER_SCORE_105_RET = 62266,
	*/
	void HandleSanDaErScore105Ret(SanDaErC2SScore105Ret* msg);

	/*
	3��2��C->S ׯ���������
	MSG_C_2_S_USER_GET_BASECARDS = 61027,		//
	*/
	void HandleSanDaErGetBaseCards(SanDaErC2SGetBaseCards* msg);

	/*
	3��2��C->S �ͻ���ׯ��ѡ����
	MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI = 62272
	*/
	void HandleSanDaErZhuangRequestJiaoPai(SanDaErC2SZhuangJiaoPai* msg);

	/*
	C->S ��ׯ��  �ͻ���ѡ��ׯ�ҽ��Ƶ�����
	MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI = 62274
	*/
	void HandleSanDaErXianSelectJiaoPai(SanDaErC2SXianSelectJiaoPai* msg);

	/*
	3��1��C->S ���ѡ����Ʒ�
	MSG_C_2_S_SANDAYI_SELECT_SCORE = 62253
	*/
	void HandleSanDaYiSelectScore(SanDaYiC2SSelectScore* msg);

	/*
	3��1��C->S ���ѡ����
	MSG_C_2_S_SANDAYI_SELECT_MAIN = 62255
	*/
	void HandleSanDaYiSelectMain(SanDaYiC2SSelectMain* msg);

	/*
	3��1��C->S ������
	MSG_C_2_S_SANDAYI_BASE_CARD = 62257
	*/
	void HandleSanDaYiBaseCard(SanDaYiC2SBaseCard* msg);

	/*
	3��1��C->S ��ҳ���
	MSG_C_2_S_SANDAYI_OUT_CARD = 62261
	*/
	void HandleSanDaYiOutCard(SanDaYiC2SOutCard* msg);

	/*
	3��1��C->S ��һظ��мҵ÷���105��,�Ƿ�ֹͣ��Ϸ
	MSG_C_2_S_SANDAYI_SCORE_105_RET = 62266,
	*/
	void HandleSanDaYiScore105Ret(SanDaYiC2SScore105Ret* msg);

	/*
	���˰ٷ֣�C->S ���ѡ����Ʒ�
	MSG_C_2_S_WURENBAIFEN_SELECT_SCORE = 62353
	*/
	void HandleWuRenBaiFenSelectScore(WuRenBaiFenC2SSelectScore* msg);

	/*
	���˰ٷ֣�C->S ���ѡ����
	MSG_C_2_S_WURENBAIFEN_SELECT_MAIN = 62355
	*/
	void HandleWuRenBaiFenSelectMain(WuRenBaiFenC2SSelectMain* msg);

	/*
	���˰ٷ֣�C->S ������
	MSG_C_2_S_WURENBAIFEN_BASE_CARD = 62357
	*/
	void HandleWuRenBaiFenBaseCard(WuRenBaiFenC2SBaseCard* msg);

	/*
	���˰ٷ֣�C->S ���ѡ�Լ���
	MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND = 62359
	*/
	void HandleWuRenBaiFenSelectFriend(WuRenBaiFenC2SSelectFriend* msg);

	/*
	���˰ٷ֣�C->S ��ҳ���
	MSG_C_2_S_WURENBAIFEN_OUT_CARD = 62361
	*/
	void HandleWuRenBaiFenOutCard(WuRenBaiFenC2SOutCard* msg);

	/*
	���˰ٷ֣�C->S ��һظ��мҵ÷���105��,�Ƿ�ֹͣ��Ϸ
	MSG_C_2_S_WURENBAIFEN_SCORE_105_RET = 62366,
	*/
	void HandleWuRenBaiFenScore105Ret(WuRenBaiFenC2SScore105Ret* msg);
	

	/*
	���˰ٷ֣�����	 
	*/
	void HandleWuRenBaiFenZhuangJiaoPai(WuRenBaiFenC2SZhuangJiaoPai * msg);
	void HandleWuRenBaiFenXianAckJiaoPai(WuRenBaiFenC2SXianSelectJiaoPai * msg);


	/*
	���ţ�C->S ��������𲧲�
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401
	*/
	void HandlerPlayerSelectBoBo(CheXuanC2SPlayerSelectBoBo* msg);

	/*
	���ţ�C->S �������ѡ�����
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 622408
	*/
	void HandlerPlayerSelectOpt(CheXuanC2SPlayerSelectOpt* msg);

	/*
	���ţ�C->S ������ҳ��Ʋ���
	MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 622411
	*/
	void HandlerPlayerChePai(CheXuanC2SPlayerChePai* msg);

	/*
	C->S ��Ҵ��Ʋ���
	MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417
	*/
	void HandlerPlayerCuoPai(CheXuanC2SPlayerCuoPai* msg);

	/*
	C->S �ͻ����ƾ�������ս��
	MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD = 62420
	*/
	void HandlerPlayerRequestRecord(CheXuanC2SPlayerRequestRecord* msg);

	/*
	C->S �ͻ��˲���Ĭ��
	MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422
	*/
	void HandlerPlayerOptMoFen(CheXuanC2SPlayerOptMoFen* msg);

	/*
	C->S �ͻ���ѡ���Զ�����
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424
	*/
	void HandlerPlayerSelectAutoOpt(CheXuanC2SPlayerSelectAutoOpt* msg);

	/*
	C->S �ͻ���ѡ��վ�������
	MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427
	*/
	void HandlerPlayerStandOrSeat(CheXuanC2SPlayerSelectStandOrSeat* msg);

	
	//�����������û�gps
	void HanderUserRequestGPS(LMsgC2SRequestUserGps* msg);

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
	Lint	m_isMedal;		 //����Ƿ�����ʿѫ��
};

#endif