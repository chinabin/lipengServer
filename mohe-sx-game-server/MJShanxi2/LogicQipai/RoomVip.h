#ifndef _ROOM_VIP_H
#define _ROOM_VIP_H

#include "LMsg.h"
#include "mhgamedef.h"
#include "Desk.h"
//#include "LVipLog.h"

struct MHUserScore
{
	int m_pos;
	int m_userId;
	int m_score;
	MHUserScore() :m_pos(-1), m_userId(-1), m_score(-1) {}
	MHUserScore(int pos, int userId, int score) : m_pos(pos), m_userId(userId), m_score(score) {}

};

inline bool operator < (const MHUserScore & l, const MHUserScore &r)
{
	return l.m_score < r.m_score;
}
inline bool operator == (const MHUserScore & l, const MHUserScore &r)
{
	return l.m_score == r.m_score;
}
inline bool operator > (const MHUserScore & l, const MHUserScore &r)
{
	return l.m_score > r.m_score;
}

struct  VipDeskLogForQiPai
{
	Lstring	 m_videoId;					//¼��id
	Lint	 m_gold[MAX_CHAIR_COUNT];   //ÿ�־ַ���
	Lint	 m_zhuangPos;				//��ׯ��λ��
	Lint	 m_time;					//����ʱ��

	Lstring  m_str;						//�ƾּ�¼
	
	Lint	 m_win[MAX_CHAIR_COUNT];    //���ڴ���
	

	VipDeskLogForQiPai()
	{
		memset(m_gold, 0, sizeof(m_gold));		
		m_zhuangPos=INVAILD_POS_QIPAI;
		m_time=0;
		m_videoId="";	
		memset(m_win, 0, sizeof(m_win));	
	}

	Lstring	ScoreToString()
	{
		std::stringstream ss;
		ss << m_time << ";";
		ss << m_videoId << ";";
		for (Lint i = 0; i < MAX_CHAIR_COUNT; i++)
		{
			ss << m_gold[i];
			if (i < MAX_CHAIR_COUNT - 1)
				ss << ",";
		}
		//ss << ";";
		return ss.str();
	}
};


struct LVipLogItemForQiPai
{
	Lstring			m_id;							//id
	Lint			m_time;							//ʱ��
	Lint			m_state;						//�����淨��101����������103��ţţ��106��������107��3��2
	Lint			m_deskId;						//����id
	Lstring			m_secret;						//��������
	Lint			m_maxCircle;					//��Ȧ��
	Lint			m_curCircle;					//��ǰȦ������ˢ���ͻ��˵ģ���ʾ���ơ���1/8�֡�����
	Lint			m_curCircleReal;				//ʵ�ʵĵ�ǰȦ������Ϊ�е��淨��ׯ����Ȧ������ֵ�������жϵ�1��insert������update�����ݿ�
	Lint        m_isInsertDB;                  //Ȥζ�����Ƿ�������ݿ�����
	Lint			m_posUserId[MAX_CHAIR_COUNT];	//����λ���ϵ����id
	Lint            m_playerState[MAX_CHAIR_COUNT];	//��Ӧλ������Ƿ������Ϸ��ÿ�ֿ�ʼʱ���� 
	Lint			m_curZhuangPos;					//��ǰׯ��
	Lint			m_score[MAX_CHAIR_COUNT];		//����λ������Ļ���
	Lint			m_coins[MAX_CHAIR_COUNT];		 //�������ֵ
	Lint			m_reset;						//�Ƿ��ɢ
	std::vector<Lint> m_playtype;					//�淨Сѡ���б�
	std::vector<VipDeskLogForQiPai*> m_log;         //ÿһ�ѵļ�¼
	Lint			m_checkTing[MAX_CHAIR_COUNT];	//��������
	Lint			m_iPlayerCapacity;				//��������������ܳ���DESK_USER_COUNT

	Lint			m_qiangCount[MAX_CHAIR_COUNT];		//ţţ������ׯ����
	Lint			m_zhuangCount[MAX_CHAIR_COUNT];		//ţţ������ׯ����
	Lint			m_tuiCount[MAX_CHAIR_COUNT];		//ţţ������ע����

	LVipLogItemForQiPai();

	virtual  ~LVipLogItemForQiPai();

	virtual  Lstring		ToString();

	virtual Lstring        ScoreToString();

	virtual  void			FromString(const Lstring& str);

	virtual  Lstring		PlayTypeToStrint();

	virtual  void			PlayTypeFromString(const Lstring& str);
};

struct MHExtendVipLogItem
{
	Lstring m_tableId;
	std::map<Lint, Lstring> m_mapUserIdToDrawId;	
	virtual void CreateTableId() = 0;         // ������Id
	virtual void SaveDeskInfoToDB() = 0;      // �洢�����ݿ�
	//���ʹ�Ӯ�Ҹ�manager
	void SendBigWinnerInfo(Lint clubId, Lint deskId, Lint userId, Lstring nike, Lint state, Lint type);
};

struct VipLogItem : public LVipLogItemForQiPai, MHExtendVipLogItem
{
public:
	Desk*			m_desk;
public:
	VipLogItem();
	~VipLogItem();
	Lint		GetOwerId();

	void		AddLog(User** user, Lint* gold, HuInfo* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting);

	void		AddLog(User** user, Lint* gold, std::vector<HuInfo>* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting);

	/**********************************************************************************************
	***user: user refer, from desk
	***gold: score array
	***playerState: user jion game state
	***zhuangpos: zhuang pos
	**********************************************************************************************/

	/* �����������ֵ */
	void		UpdatePlayerCoins(Lint* changeCoins, Lint playerCount);
	
	//����ţţ���ÿ��������ͳ��
	void		UpdateNiuNiuOptCount(Lint* qiangCount, Lint* zhuangCount, Lint* tuiCount, Lint playerCount);

	//ţţ��Ҳ���ͳ�����л������1��ׯ���������2��ׯ����...|���1��ׯ���������2��ׯ����...|���1��ע���������2�˳�������
	Lstring		NiuNiuOptCountToString();

	void		AddLogForQiPai(User** user, Lint* gold, Lint * playerState, Lint zhuangPos, Lstring& videoId, const  std::vector<std::string> & srcPaiXing = std::vector<std::string>());

	bool		ExiestUser(Lint id);

	void		InsertToDb();

	void		UpdateToDb();

	bool		AddUser(User* user);

	bool        ResetUser( User  *user[] );

	Lint		GetUserPos(User* user);

	Lint		GetUserScore(User* user);

	bool		isEnd();

	bool        isNormalEnd();

	bool		IsFull(User* user);

	//���ν����������
	void		SendEnd();

	Lstring		PlayTypeToString4RLog();

	void		SendInfo();

	bool		IsBegin();

	void		RemoveUser(Lint id);

	Lint        GetCurrentPlayerCount();

	virtual void CreateTableId();        // ������Id
	virtual void SaveDeskInfoToDB();      // �洢�����ݿ�	
	virtual void UpdateDeskTotalFeeValueToDB(Lint nFeeValue); // �޸������ܷ���
	virtual void InsertUserScoreToDB(Lint pos, Lstring &drawId, Lint isWin, Lint score, const std::string & srcPaiXing);
	virtual void UpdateUserFeeValueToDB(Lint userId, Lint feeValue);
	virtual void InsertDeskWinnerInfo();

};

class VipLogMgr :public LSingleton<VipLogMgr>
{
public:	
	virtual	bool	Init();
	virtual	bool	Final();

	void			SetVipId(Lint id);

	Lstring			GetVipId();

	void			SetVideoId(Lint id);

	Lstring			GetVideoId();

	VipLogItem*		GetNewLogItem(Lint maxCircle,Lint usrId);

	VipLogItem*		GetLogItem(Lstring& logId);

	VipLogItem*		FindUserPlayingItem(Lint id);//��ѯ������ڽ����е�����

	void			AddPlayingItem(VipLogItem* item);//���δ��ɵ�����

	void			Tick();
private:
	std::map<Lstring,VipLogItem*> m_item;
	Lint			m_id;

	Lint			m_videoId;
};

#define gVipLogMgr VipLogMgr::Instance()


//vip����
class RoomVip:public LSingleton<RoomVip>
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	/* ��ʱ�� */
	void			Tick(LTime& cur);

	/* Ѱ��һ�������ӣ�����ʼ������ID ����Ϸ���淨*/
	Desk*			GetFreeDesk(Lint nDeskID, QiPaiGameType gameType);

	/* ��������ID����ȡ���Ӷ���*/
	Desk*			GetDeskById(Lint id);

	/* ��Ҵ������� */
	Lint			CreateVipDesk(LMsgLMG2LCreateDesk* pMsg,User* pUser);

	/* ���Ϊ������Ҵ������� */
	Lint			CreateVipDeskForOther(LMsgLMG2LCreateDeskForOther* pMsg, User* pUser);

	//���ֲ�  �����ʱû���ã�����������ӵ�
	Lint			CreateVipDeskForClub(LMsgLMG2LCreateDeskForClub* pMsg);

	Lint			CreateVipDeskForClub(LMsgLMG2LCreateClubDeskAndEnter* pMsg, User* pJoinUser);

	Lint			AddToVipClubDesk(LMsgLMG2LEnterClubDesk* pMsg, User* pJoinUser);


	// pUsers��λ���Ǹ���λ���ź����
	Lint			CreateVipCoinDesk(LMsgCN2LCreateCoinDesk*pMsg,User* pUsers[]);

	Lint			AddToVipDesk(User* pUser, Lint nDeskID);

	Lint			AddToVipDesk(User* pUser, LMsgLMG2LAddToDesk* msg);

	//�����ս�������
	Lint			VipLookOnUserSeatDown(User* pUser, LMsgG2LLookOnUserSearDown* msg);

	Lint			SelectSeatInVipDesk(User* pUser, Lint nDeskID);

	bool			LeaveToVipDesk(LMsgC2SLeaveDesk* pMsg, User* pUser);
	void            MHPrintAllDeskStatus();

//��manager�������Ӻ���managerͬ��������Ϣ
	void SynchroDeskData(std::vector<LogicDeskInfo> & DeskInfo);

	//����Ϸ����
	Lint  getGameDrawCount(Lint game_state, Lint no);

	//����Сѡ���ж���Ϸ����
	bool getDeskUserMax(Lint game_state, std::vector<Lint>& playType, Lint * userMax);

private:
	DeskMap		m_deskMap;

	std::queue<Lint>  m_deskId;
};

#define gRoomVip RoomVip::Instance()



#endif