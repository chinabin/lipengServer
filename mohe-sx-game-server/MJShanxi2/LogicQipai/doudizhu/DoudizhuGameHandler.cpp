#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "../Config.h"
#include "../RoomVip.h"
#include "../Work.h"
#include "LMsgL2C.h"
#include "../GameHandler.h"
#include "../Utils.h" 
#include "../User.h"
#include "../Desk.h"
#include "../mhgamedef.h"

#include "CMD_Game.h"
#include "CMD_Game_Linfen.h"
#include "CMD_Game_OneLaizi.h"

#include "GameLogicClassical.h"
#include "GameLogicLaizi.h"
#include "GameLogicLinfen.h"

//#include "../LQiPaiVideo.h"


#define ASSERT(f)
#define VERIFY ASSERT

//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))


//����������
static bool debugSendCards_doudizhu(BYTE* cbRandCard, Desk* m_desk, const char* conFileName, int playerCount, int handCardsCount, int cellPackCount)
{
	if (conFileName == NULL || std::strlen(conFileName) == 0)
	{
		return false;
	}

	//�����ļ����Ƿ������������
	bool can_assign = gConfig.GetDebugModel();

	FILE *fp = NULL;
	try
	{
		fp = fopen(conFileName, "r");
	}
	catch (const std::exception& e)
	{
		LLOG_ERROR("File card.ini is open failed...  desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);
		if (fp) fclose(fp);
		fp = NULL;
	}

	//��������������� && card.ini����
	if (can_assign && fp)
	{
		LLOG_DEBUG("Send Cards...This is debug mode!!! desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);

		//���ڴ洢���Ƶ��������
		//BYTE t_cbRandCard[PLAY_TYPE_NORMAL_CARD_TOTAL];
		BYTE* t_cbRandCard = new BYTE[cellPackCount];
		memset(t_cbRandCard, 0, sizeof(BYTE) * cellPackCount);

		//����card.ini�õ�������
		std::vector<BYTE> t_sendCards;

		//�����ƺ�ÿ�������ƵĺϷ�����
		size_t* t_sendCount = new size_t[playerCount];
		memset(t_sendCount, 0, sizeof(t_sendCount));
		size_t t_count = 0;

		//��ÿ����ҽ�����Ч������
		for (int i = 0; i < playerCount; ++i)
		{
			t_sendCards.clear();
			//���������������
			LoadPlayerCards(t_sendCards, i + 1, conFileName);
			t_count = 0;

			for (int j = 0; j < t_sendCards.size(); ++j)  //�����vector
			{
				for (int k = 0; k < cellPackCount; ++k)  //1����
				{
					if (t_sendCards[j] == cbRandCard[k])
					{
						t_cbRandCard[i * handCardsCount + t_count++] = cbRandCard[k];
						cbRandCard[k] = 0;
						break;
					}
				}
			}
			t_sendCount[i] = t_count;
		}

		//����������Ч����
		t_sendCards.clear();
		LoadBaseCards(t_sendCards, conFileName);
		size_t t_baseCount = 0;
		for (int j = 0; j < t_sendCards.size(); ++j)  //�����vector
		{
			for (int k = 0; k < cellPackCount; ++k)  //1����
			{
				if (t_sendCards[j] == cbRandCard[k])
				{
					t_cbRandCard[playerCount * handCardsCount + t_baseCount++] = cbRandCard[k];
					cbRandCard[k] = 0;
					break;
				}
			}
		}

		//��ȫ����
		for (int i = 0; i < playerCount; ++i)
		{
			for (int j = i * handCardsCount + t_sendCount[i]; j < (i+1) * handCardsCount; )
			{
				for (int k = 0; k < cellPackCount; ++k)
				{
					if (cbRandCard[k] != 0)
					{
						t_cbRandCard[j++] = cbRandCard[k];
						cbRandCard[k] = 0;
						break;
					}
				}
			}
		}

		//��ȫ����
		for (int j = playerCount * handCardsCount + t_baseCount; j < cellPackCount; )
		{
			for (int k = 0; k < cellPackCount; ++k)
			{
				if (cbRandCard[k] != 0)
				{
					t_cbRandCard[j++] = cbRandCard[k];
					cbRandCard[k] = 0;
					break;
				}
			}
		}

		//�����õ��Ƹ��Ƹ���������
		memcpy(cbRandCard, t_cbRandCard, sizeof(BYTE) * cellPackCount);

		if(t_cbRandCard) delete(t_cbRandCard);
		t_cbRandCard = NULL;

		if (t_sendCount) delete(t_sendCount);
		t_sendCount = NULL;

		if (fp) fclose(fp);
		fp = NULL;
		return true;
	} //if (can_assign && fp)

	if (fp) fclose(fp);
	fp = NULL;

	return false;
}


//�����������淨
enum DOUDIZHU_PLAY_TYPE
{
	PLAY_TYPE_NORMAL=1,                       //��ͨ������
	PLAY_TYPE_LAIZI =2,                             //һ����Ӷ�����
	PLAY_TYPE_LINFEN=3,                        //�ٷ�ȥ����������
};

//�Ʊ��� 
#define CARD_BACK_PATTERN      0x43

//////////////////////////////////////////////////////////////////////////////
//״̬����
#define DOUDIZHU_GAME_SCENE_CALL   1
#define DOUDIZHU_GAME_SCENE_PLAY  3

#define GAME_SCENE_LINFEN_TIPAI              21  			//����״̬
#define GAME_SCENE_LINFEN_GENTI              22  			//����״̬
#define GAME_SCENE_LINFEN_HUITI              23  			//����״̬


//////////////////////////////////////////////////////////////////////////////


enum GAMG_CONCLUDE_REASON
{
	GAMG_CONCLUDE_COMPARECARD,
	GAMG_CONCLUDE_NOPLAYER,
	GAMG_CONCLUDE_SYSTEM_COMPARECARD,
	GAMG_CONCLUDE_DISMISS
};

//�淨Сѡ��
struct DouDiZhuPlayType
{
public:
	void clear() { m_playtype.clear(); }
	void setAllTypes(const std::vector<Lint>& v) { m_playtype = v; }
	const std::vector<Lint>& getAllTypes() const { return m_playtype; }

	Lint   DouDiZhuGetGameMode() const;                      //1�淨����              1�����䶷����  2:һ����ӵĶ�����   3���ٷ�ȥ����
	Lint   DouDiZhuDiPaiType() const;                            //2������ʾ�����         1����������    2�����ƿ��ƣ�ֻ�Ե����ɼ���
	Lint   DouDiZhuBombMaxCount() const;               //3ը���ⶥ����          0��Ϊ����ס�  ����Ϊ�������
	Lint   DouDiZhuCellScore() const;                            //4�׷�
	Lint   DouDiZhuAnonymous() const;                       //5����
	Lint   DouDiZhuPiPeiRenCount() const;                //6ƥ���������manager ���õ���logic��û��������ֶ�
	Lint   DouDiZhuCallZhuangMode() const;            //7��ׯ����
	Lint   DouDiZhuIsTi() const;                                    //8����ʹ���������ѡ��
	Lint   DouDiZhuIsTongHua3() const;                   //9ֻ��ͬ��ɫ��3Ϊը
	Lint   DouDiZhuLinfenTiToZhaFengDing()const;       //10�ٷ�ѡ�������Ƿ�����ը�ⶥ
	
protected:
	std::vector<Lint>	m_playtype;
};

Lint DouDiZhuPlayType::DouDiZhuGetGameMode() const
{
	if (m_playtype.empty() || m_playtype[0]<= 0 || m_playtype[0] > 3)  return 1;
	return m_playtype[0];
}

Lint DouDiZhuPlayType::DouDiZhuDiPaiType() const
{
	if (m_playtype.size()<2)return 1;
	return m_playtype[1];
}

Lint DouDiZhuPlayType::DouDiZhuBombMaxCount() const
{
	if (m_playtype.size()<3) return 100;           //�����
	return m_playtype[2]==0 ?100 : m_playtype[2];
}

Lint  DouDiZhuPlayType::DouDiZhuCellScore() const
{
	if (m_playtype.size()<4) return 1;           //
	if (m_playtype[3] > 0 && m_playtype[3] <= 5)return m_playtype[3];
	else
		return 1;
}

Lint DouDiZhuPlayType::DouDiZhuAnonymous() const
{
	if (m_playtype.size()<5) return 0;           //����
	return m_playtype[4];
}

Lint DouDiZhuPlayType::DouDiZhuCallZhuangMode() const
{
	if (m_playtype.size()<7) return 0;           //��ׯ����
	return m_playtype[6];
}

Lint DouDiZhuPlayType::DouDiZhuIsTi() const
{
	if (m_playtype.size()<8) return 0;           //��������
	if (m_playtype[0] == 1 || m_playtype[0] == 2)
		return m_playtype[7];
	else
		return 0;
}

Lint   DouDiZhuPlayType::DouDiZhuIsTongHua3() const
{
	if (m_playtype.size()<9) return 0;           //ͬ��ɫ��3Ϊը
	return m_playtype[8];
}

//10�ٷ�ѡ�������Ƿ�����ը�ⶥ  1-��ⶥ  0-����
Lint   DouDiZhuPlayType::DouDiZhuLinfenTiToZhaFengDing()const
{
	if (m_playtype.size()<10) return 0;           //
	if (DouDiZhuGetGameMode() != 3)return 0;
	return m_playtype[9];

}


// �����Զ���ʼ�����ֶη����ÿ���Զ�����
struct DouDiZhuRoundState__c_part
{
	Lint         m_user_status[DOUDIZHU_PLAY_USER_COUNT];         //�û�״̬
	Lint         m_play_status;	                                                                     // ��Ϸ״̬
	
   //  ��������������
	WORD							m_wFirstUser;						//�׽��û�
	WORD							m_wBankerUser;				   //ׯ���û�
	WORD							m_wCurrentUser;						//��ǰ���
	BYTE							m_cbOutCardCount[DOUDIZHU_PLAY_USER_COUNT];		//���ƴ���
	

	//ը����Ϣ
	BYTE							m_cbBombCount;						//ը������
	BYTE							m_cbEachBombCount[DOUDIZHU_PLAY_USER_COUNT];		//ը������

	//�з���Ϣ
	BYTE							m_cbCallScoreCount;				//�зִ���
	BYTE							m_cbBankerScore;					//ׯ�ҽз�
	BYTE							m_cbScoreInfo[DOUDIZHU_PLAY_USER_COUNT];		//�з���Ϣ ,Ĭ��255

	//������Ϣ
	WORD							m_wTurnWiner;						   //ʤ�����
	BYTE							m_cbTurnCardCount;					//������Ŀ
	BYTE							m_cbTurnCardData[PLAY_TYPE_CARD_REST_MAX + PLAY_TYPE_CARD_HAND_MAX];		//�������� ��Ϊ21��
	
	//�˿���Ϣ
	BYTE							m_cbBankerCard[PLAY_TYPE_CARD_REST_MAX];					//��Ϸ����-3��  ��Ϊ4��
	BYTE							m_cbHandCardCount[DOUDIZHU_PLAY_USER_COUNT];		//�˿���Ŀ 0-21
	BYTE							m_cbHandCardData[DOUDIZHU_PLAY_USER_COUNT][PLAY_TYPE_CARD_REST_MAX +PLAY_TYPE_CARD_HAND_MAX];	//�����˿� ��Ϊ21��

	BYTE                          m_BombCountLimit;                        //ը������  
	BYTE						  m_cbBombCountReal;						//ʵ��ը������,    m_cbBombCount �������ﵽlimit���ټ���
	BYTE						  m_cbEachBombCountReal[DOUDIZHU_PLAY_USER_COUNT];		//ÿ���û�ʵ��ը������, ͬ��

	//�ٷ�ȥ����
	LONG							m_cbBeiShu[DOUDIZHU_PLAY_USER_COUNT];			//�û�����

	BYTE							m_cbIsTi[DOUDIZHU_PLAY_USER_COUNT];			//��  0xff-��ʼ״̬��0-û��ѡ��1-���ƣ�2-����,3-����
	WORD                       m_wTiPaiUser;                        //�����û�                         
	WORD                       m_wGenTiUser;                        // �����û�

	WORD                       m_onlyCallUser;                     //��Ϊ�������ͣ�ֻ�ܽ�ׯ���û�

	
	void clear_round()
	{
		memset(this, 0, sizeof(*this));
		m_play_status = DOUDIZHU_GAME_SCENE_CALL;	
		//m_curPos = DOUDIZHU_INVAILD_POS;
		m_wFirstUser = DOUDIZHU_INVAILD_POS;
		m_wBankerUser = DOUDIZHU_INVAILD_POS;
		m_wCurrentUser = DOUDIZHU_INVAILD_POS;
		m_wTurnWiner = DOUDIZHU_INVAILD_POS;
		m_wTiPaiUser = DOUDIZHU_INVAILD_POS;
		m_wGenTiUser = DOUDIZHU_INVAILD_POS;
		m_onlyCallUser = 100;
		memset(m_cbIsTi, 0xff, sizeof(m_cbIsTi));
		memset(m_cbScoreInfo, 0xff, sizeof(m_cbScoreInfo));
	}

};

// �й��캯�����ֶη����ÿ���Զ�����
struct DouDiZhuRoundState__cxx_part
{
	LTime m_play_status_time;
	void clear_round()
	{
		this->~DouDiZhuRoundState__cxx_part();
		new (this) DouDiZhuRoundState__cxx_part;
	}
};

struct DouDiZhuMatchState__c_part
{
	Desk*         m_desk;
	int           m_round_offset;
	int           m_round_limit;
	int           m_accum_score[DOUDIZHU_PLAY_USER_COUNT];
	bool          m_dismissed;

	int           m_registered_game_type;
	int           m_player_count;   // �淨���Ĵ�����ʹ�õ���������ֶ�
	Lint          m_zhuangPos;      //ׯ��λ��
	Lint          m_firstCallZhuang;
	//BYTE           m_cbCustomGameRule[128];

	void clear_match()
	{
		memset(this, 0, sizeof(*this));
	}
};

struct DouDiZhuMatchState__cxx_part
{	
	DouDiZhuPlayType m_playtype;	                                         // �����淨

	CGameLogicClassical		 m_GameLogicClassical;			//��Ϸ�߼�-����
	CGameLogicLinfen             m_GameLogicLinfen;              //��Ϸ�߼�-�ٷ�					
	CGameLogicLaizi             m_GameLogicLaizi;                   //��Ϸ�߼�-���

	void clear_match() {
		this->~DouDiZhuMatchState__cxx_part();
		new (this) DouDiZhuMatchState__cxx_part;
	}
};


struct DouDiZhuRoundState : DouDiZhuRoundState__c_part, DouDiZhuRoundState__cxx_part {
	// ��Ҫ��������ֶ�
	void clear_round() {
		DouDiZhuRoundState__c_part::clear_round();
		DouDiZhuRoundState__cxx_part::clear_round();
	}
};

// һ��Match���кü���Round
struct DouDiZhuMatchState : DouDiZhuMatchState__c_part, DouDiZhuMatchState__cxx_part {
	// ��Ҫ��������ֶ�
	void clear_match() {
		DouDiZhuMatchState__c_part::clear_match();
		DouDiZhuMatchState__cxx_part::clear_match();
	}
};

struct DouDiZhuDeskState :DouDiZhuRoundState, DouDiZhuMatchState {
	void clear_match(int player_count) {
		DouDiZhuMatchState::clear_match();
		clear_round();
		m_player_count = player_count;
	}

	void clear_round() {
		DouDiZhuRoundState::clear_round();
	}

	void setup_match(std::vector<Lint> &l_playtype, int round_limit) {
		m_round_limit = round_limit;
		//// Сѡ��
		m_playtype.setAllTypes(l_playtype);


	}

	void increase_round() {
		++m_round_offset;
		if (m_desk && m_desk->m_vip) ++m_desk->m_vip->m_curCircle;
	}

	void set_desk_state(DESK_STATE v) {
		if (m_desk) {
			if (v == DESK_WAIT) m_desk->SetDeskWait();
			else m_desk->setDeskState(v);
		}
	}
	DESK_STATE get_desk_state() {
		// ���Ӿ� DESK_PLAY/DESK_WAIT/DESK_FREE ����״̬
		// ��Ԫ����ʱû��Desk����һֱ�� DESK_PLAY ״̬
		if (m_desk) return (DESK_STATE)m_desk->getDeskState();
		else return DESK_PLAY;
	}

	void set_play_status(Lint play_status)
	{
		//if (play_status >= GAME_PLAY_SELECT_ZHUANG&&play_status <= GAME_PLAY_END)
			m_play_status = play_status;
	}

	Lint GetUserPos(User* pUser)
	{
		if (pUser == NULL) return DOUDIZHU_INVAILD_POS;
		Lint pos = DOUDIZHU_INVAILD_POS;
		if (m_desk)
			pos = m_desk->GetUserPos(pUser);
		return pos == INVAILD_POS_QIPAI ?  DOUDIZHU_INVAILD_POS : pos;
	}

	//ȡ����Ч���¸�λ��
	int GetNextPos(int pos) {
		if (m_player_count == 0) return 0; // ����������	
		Lint nextPos = (pos + 1) % m_player_count;
		for (int i = 0; (i<m_player_count) && (m_user_status[nextPos] != 1); i++)
		{
			nextPos = (nextPos + 1) % m_player_count;
		}
		return nextPos;
	}

	bool is_pos_valid(int pos) {
		if (NULL == m_desk) return false;
		return 0 <= pos && pos < DOUDIZHU_PLAY_USER_COUNT;
	}

};


struct DouDiZhuVideoSupport : DouDiZhuDeskState {
	QiPaiVideoLog		m_video;						//¼��

	///////////////////////////////////////////////////////////////////
    // ��鼯�д�ź�¼���йصĺ���
	///////////////////////////////////////////////////////////////////
	void VideoSave() {
		if (NULL == m_desk) return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		//m_video.m_playType = m_desk->getPlayType();
		//m_video.m_flag= m_desk->m_state;
		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		LLOG_DEBUG("videoSave[%s]",video.m_sql.c_str());
		gWork.SendMsgToDb(video);
	}

	//������ oper ����  0--��  1--����
	void SaveUserOper(Lint oper, Lint pos, BYTE handcardCount, BYTE* cbHandCardData, Lint cardCount = 0, BYTE *cbCardData = NULL,Lint score=0)
	{
		LLOG_DEBUG("SaveUserOper  pos[%d],oper[%d]",pos,oper);
		
		std::vector<Lint> cards;
		std::vector<Lint> handcards;
		if (cardCount != 0)
		{
			if (cbCardData != NULL)
			{
				for (int i = 0; i < cardCount; i++)
				{
					LLOG_DEBUG("cards[%d]=[%d]",i, cbCardData[i]);
					cards.push_back((Lint)cbCardData[i]);
				}
			}
		}
		if (handcardCount != 0)
		{
			if (cbHandCardData != NULL)
			{
				for (int i = 0; i < handcardCount; i++)
				{
					LLOG_DEBUG("cbHandCardData[%d]=[%d]", i, cbHandCardData[i]);
					handcards.push_back((Lint)cbHandCardData[i]);
				}
			}

		}
		m_video.AddOper(oper, pos, cards,score,handcards);
	}

};

struct DouDiZhuGameCore : GameHandler, DouDiZhuVideoSupport{

	inline bool is_normal()
	{
		return m_playtype.DouDiZhuGetGameMode() == PLAY_TYPE_NORMAL;
	}
	inline bool is_linfen() {
		return m_playtype.DouDiZhuGetGameMode() == PLAY_TYPE_LINFEN;
	}
	inline bool is_laizi()
	{
		return m_playtype.DouDiZhuGetGameMode() == PLAY_TYPE_LAIZI;
	}
	
	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
	}

	virtual void notify_desk_playing_user(LMsg &msg) {
	}

	virtual void notify_desk_without_user(LMsg &msg, User* pUser) {
	}

	//�㲥��Ϸ�����������û�
	void notify_desk_match_state() {
		LMsgS2CVipInfo msg;
		msg.m_curCircle = m_round_offset + 1;
		msg.m_curMaxCircle = m_round_limit;	
		notify_desk(msg);
	}

	void add_round_log(Lint *score, int zhuangPos) {

		//����ط�
		m_video.addUserScore(score);
		if(m_desk && m_desk->m_vip&&m_desk->m_vip->m_reset==0)
			VideoSave();

		// ������	
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, score, m_user_status, zhuangPos, m_video.m_Id);			
		}

	}

	//֪ͨ������ʼ��Ϸ
	void OnEventBankerOutCard()
	{
		LLOG_DEBUG("OnEventBankerOutCard");
	
		MHLMsgDouDiZhuS2CGameMessage msg;
		CMD_S_BankerOutCard bankerOutCard;
		bankerOutCard.m_Pos = m_wBankerUser;
		CopyMemory(&msg.cmdData.cmd_s2c_bank_out_card, &bankerOutCard, sizeof(bankerOutCard));
		msg.sub_cmd_code = SUB_S_LINFEN_BANKER_OUT_CARD;
		notify_desk(msg);
		//notify_user(msg, m_wBankerUser);

		//�����û�
		m_wCurrentUser = m_wBankerUser;

		//����״̬
		this->set_play_status(DOUDIZHU_GAME_SCENE_PLAY);

		Lint tmpTipai[DOUDIZHU_PLAY_USER_COUNT];
		memset(tmpTipai, 0x00, sizeof(tmpTipai));
		for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			//LLOG_DEBUG("m_cbIsTi [%d]=[%d]", i, m_cbIsTi[i]);
			tmpTipai[i] =(Lint)( (m_cbIsTi[i] == 0xff) ? 0 : m_cbIsTi[i]);
			//LLOG_DEBUG("tempTipai [%d]=[%d]",i, tmpTipai[i]);
		}
		m_video.doudizhuTiPai(DOUDIZHU_PLAY_USER_COUNT, tmpTipai);
		if (is_linfen()&&m_playtype.DouDiZhuLinfenTiToZhaFengDing()!=0)
		{
			Lint tiCount = 0;
			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (tmpTipai[i] != 0 && tmpTipai[i] != 3)tiCount++;
				if (tmpTipai[i] == 3)m_cbBombCount++;
			}
			if (tiCount == 2)m_cbBombCount++;
		}

	}

	//��¼ը��
	void OnEventRecordBombCount(WORD wChairID)
	{
		//if (m_cbBombCount < m_BombCountLimit || m_pGameServiceOption->wServerType == GAME_GENRE_SAISHI)
		if (m_cbBombCount < m_playtype.DouDiZhuBombMaxCount())
		{
			m_cbBombCount++;
			m_cbEachBombCount[wChairID]++;
			LLOG_ERROR("OnEventRecordBombCount desk:%d ,pos[%d]  bombNum[%d],maxBomb[%d] ", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_cbBombCount, m_playtype.DouDiZhuBombMaxCount());
			//char szString[512] = ("");
			//_snprintf(szString, CountArray(szString), "ը����ը������[%d]", m_cbBombCount);
			//LLOG_DEBUG(szString);
			for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
				m_cbBeiShu[i] *= 2;

			OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
		}
		else
		{
			LLOG_ERROR("OnEventRecordBombCount desk:%d  bombNum[%d],maxBomb[%d] ", m_desk ? m_desk->GetDeskId() : 0, m_cbBombCount,m_playtype.DouDiZhuBombMaxCount());
			//char szString[512] = ("");
			//_snprintf(szString, CountArray(szString), "ը���ⶥ���ټƷ֣�ը������[%d]", m_cbBombCount);
			//LLOG_DEBUG(szString);
		}

		m_cbBombCountReal++;
		m_cbEachBombCountReal[wChairID]++;

	}

	//ˢ�±���
	bool OnEventUserFlushScore(WORD wChairID)
	{
		LLOG_DEBUG("OnEventUserFlushScore ");
		//Ⱥ����Ϣ
		if (wChairID == DOUDIZHU_INVAILD_POS)
		{
			//����ˢ�±���
			for (BYTE i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				MHLMsgDouDiZhuS2CGameMessage msg;
				struct CMD_S_ScoreFresh ScoreFresh;
				ZeroMemory(&ScoreFresh, sizeof(ScoreFresh));
				ScoreFresh.score = m_cbBeiShu[i];
				CopyMemory(&msg.cmdData.cmd_s2c_score_fresh, &ScoreFresh, sizeof(CMD_S_ScoreFresh));
				//msg.cmdData.cmd_s2c_score_fresh.score = m_cbBeiShu[i];
				msg.sub_cmd_code = SUB_S_LINFEN_SCORE_FRESH;
				notify_user(msg, i);
			}

		}
		if (wChairID >= 0 && wChairID<DOUDIZHU_PLAY_USER_COUNT)
		{
			MHLMsgDouDiZhuS2CGameMessage msg;
			//����ˢ�±���
			struct CMD_S_ScoreFresh ScoreFresh;
			ZeroMemory(&ScoreFresh, sizeof(ScoreFresh));
			ScoreFresh.score = m_cbBeiShu[wChairID];
			CopyMemory(&msg.cmdData.cmd_s2c_score_fresh, &ScoreFresh, sizeof(CMD_S_ScoreFresh));
			//msg.cmdData.cmd_s2c_score_fresh.score = m_cbBeiShu[wChairID];
			msg.sub_cmd_code = SUB_S_LINFEN_SCORE_FRESH;
			notify_user(msg, wChairID);
		}
		return true;
	}

	//�㲥�û��ߵ���Ϣ
	void OnEventBroadcaseTiResult(BYTE cmdType, WORD wChairID, BOOL isYes)
	{
		LLOG_DEBUG("OnEventBroadcaseTiResult  [%d],pos[%d],isYes[%d]", cmdType, wChairID, isYes);
		CMD_S_TiResult  TiResult;
		ZeroMemory(&TiResult, sizeof(TiResult));
		TiResult.wActUser = wChairID;
		TiResult.wActSelect = isYes;
		
		MHLMsgDouDiZhuS2CGameMessage msg;
		CopyMemory(&msg.cmdData.cmd_s2c_ti_result, &TiResult, sizeof(TiResult));
		switch (cmdType)
		{
		case SUB_S_LINFEN_TIPAI:
		{
			msg.sub_cmd_code = SUB_S_LINFEN_TIPAI_RESULT;
			break;
		}
		case SUB_S_LINFEN_GENTI:
		{
			msg.sub_cmd_code = SUB_S_LINFEN_GENTI_RESULT;
			break;
		}
		case SUB_S_LINFEN_HUIPAI:
		{
			msg.sub_cmd_code = SUB_S_LINFEN_HUIPAI_RESULT;
			break;
		}
		}
		notify_desk(msg);

	}

	//��Ϸ��ʼ
	bool OnEventGameStart()
	{
		
		////ը������		
		m_BombCountLimit = m_playtype.DouDiZhuBombMaxCount();		 

		//������Ϣ
		m_cbTurnCardCount = 0;
		m_wTurnWiner = DOUDIZHU_INVAILD_POS;
		ZeroMemory(m_cbTurnCardData, sizeof(m_cbTurnCardData));

		//����״̬
		set_play_status(DOUDIZHU_GAME_SCENE_CALL);

		//�����˿�
		BYTE cbRandCard[PLAY_TYPE_CARD_MAX];

		switch (m_playtype.DouDiZhuGetGameMode())
		{
		case PLAY_TYPE_NORMAL:
		{
			m_GameLogicClassical.RandCardList(cbRandCard, PLAY_TYPE_NORMAL_CARD_TOTAL);

			//����������
			debugSendCards_doudizhu(cbRandCard, m_desk, "card_doudizhu.ini", DOUDIZHU_PLAY_USER_COUNT, 17, PLAY_TYPE_NORMAL_CARD_TOTAL);

			break;
		}
		case PLAY_TYPE_LAIZI:
		{
			m_GameLogicLaizi.RandCardList(cbRandCard, PLAY_TYPE_LAIZI_CARD_TOTAL);

			//����������
			debugSendCards_doudizhu(cbRandCard, m_desk, "card_doudizhu.ini", DOUDIZHU_PLAY_USER_COUNT, 17, PLAY_TYPE_LAIZI_CARD_TOTAL);

			break;
		}
		case PLAY_TYPE_LINFEN:
		{
			m_GameLogicLinfen.RandCardList(cbRandCard, PLAY_TYPE_LINFEN_CARD_TOTAL);

			//����������
			debugSendCards_doudizhu(cbRandCard, m_desk, "card_doudizhu.ini", DOUDIZHU_PLAY_USER_COUNT, 16, PLAY_TYPE_LINFEN_CARD_TOTAL);

			break;
		}

		}
		
		//��ȡ����
		BYTE cbValidCardData = 0;
		BYTE cbValidCardIndex = 0;
		WORD wStartUser = m_wFirstUser;
		WORD wCurrentUser = m_wFirstUser;

		//��ȡ���
		if (wStartUser == DOUDIZHU_INVAILD_POS)
		{
			//��ȡ�˿�
			cbValidCardIndex = rand() % 51;
			cbValidCardData = cbRandCard[cbValidCardIndex];

			////�����û�
			wStartUser = m_GameLogicClassical.GetCardValue(cbValidCardData) % DOUDIZHU_PLAY_USER_COUNT;
			wCurrentUser = (wStartUser + cbValidCardIndex / 54) % DOUDIZHU_PLAY_USER_COUNT;

			wStartUser = 0;

			wCurrentUser = wStartUser;

		}


		//�û��˿�
		Lint userHandCardCount = PLAY_TYPE_CARD_HAND_COUNT;
		Lint bankerCardCount = PLAY_TYPE_CARD_REST_COUNT;
		for (WORD i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			WORD wUserIndex = (wStartUser + i) % DOUDIZHU_PLAY_USER_COUNT;
			
			switch (m_playtype.DouDiZhuGetGameMode())
			{
			case PLAY_TYPE_NORMAL:
			{
				m_cbHandCardCount[wUserIndex] = PLAY_TYPE_CARD_HAND_COUNT;
				break;
			}
			case PLAY_TYPE_LAIZI:
			{
				m_cbHandCardCount[wUserIndex] = PLAY_TYPE_CARD_HAND_COUNT;
				bankerCardCount = PLAY_TYPE_CARD_REST_COUNT_4;
				break;
			}
			case PLAY_TYPE_LINFEN:
			{
				userHandCardCount = PLAY_TYPE_CARD_HAND_COUNT_LINFEN;
				m_cbHandCardCount[wUserIndex] = PLAY_TYPE_CARD_HAND_COUNT_LINFEN;
				bankerCardCount = PLAY_TYPE_CARD_REST_COUNT_4;
				break;
			}
			}
			//m_cbHandCardCount[wUserIndex] = NORMAL_COUNT;
			CopyMemory(&m_cbHandCardData[wUserIndex], &cbRandCard[i*m_cbHandCardCount[wUserIndex]], sizeof(BYTE)*m_cbHandCardCount[wUserIndex]);
		}

		//���õ���
		CopyMemory(m_cbBankerCard, &cbRandCard[userHandCardCount*DOUDIZHU_PLAY_USER_COUNT], sizeof(BYTE)*bankerCardCount);

		if (is_linfen())
		{
			m_onlyCallUser = 100;
			//���������û�
			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (m_GameLogicLinfen.HaveSpecCard(m_cbHandCardData[i], m_cbHandCardCount[i]))
					m_onlyCallUser = i;
			}
		}
		else if (is_normal())
		{
			m_onlyCallUser = 100;
			//���������û�
			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (m_GameLogicClassical.HaveSpecCard(m_cbHandCardData[i], m_cbHandCardCount[i]))
					m_onlyCallUser = i;
			}
		}


		//�����û�
		if (m_desk&&m_desk->m_vip->m_curCircle == 0)
		{
			m_wFirstUser = wCurrentUser;
			m_wCurrentUser = wCurrentUser;
			m_firstCallZhuang = wCurrentUser;
		}
		else
		{
			m_wFirstUser = m_firstCallZhuang;
			m_wCurrentUser = m_firstCallZhuang;
		}

		//��������
		for (WORD i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			MHLMsgDouDiZhuS2CGameMessage msg;
			//�����˿�
			CopyMemory(msg.cmdData.cmd_s2c_game_start.cbCardData, m_cbHandCardData[i], sizeof(BYTE)*m_cbHandCardCount[i]);		
			msg.sub_cmd_code = SUB_S_GAME_START;
			msg.cmdData.cmd_s2c_game_start.wStartUser = m_wFirstUser;
			msg.cmdData.cmd_s2c_game_start.wCurrentUser = m_wCurrentUser;
			msg.cmdData.cmd_s2c_game_start.cbValidCardData = cbValidCardData;
			msg.cmdData.cmd_s2c_game_start.cbValidCardIndex = cbValidCardIndex;
			msg.cmdData.cmd_s2c_game_start.cbCardCount = userHandCardCount;
			for (Lint i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (m_desk && m_desk->m_vip)
				{
					msg.m_totalScore.push_back(m_desk->m_vip->m_score[i]);
					//LLOG_DEBUG("startgame.score[%d]=[%d]", i, msg.cmdData.cmd_s2c_game_start.cbScore[i]);
				}
				else
					LLOG_ERROR("m_desk or  m_desk->m_vip is null");
			}

			if (is_linfen() && i == m_onlyCallUser)
				msg.cmdData.cmd_s2c_game_start.cbOnlyCall = 1;
			if (is_normal() && i == m_onlyCallUser)
				msg.cmdData.cmd_s2c_game_start.cbOnlyCall = 1;

		

			notify_user(msg, i);
			std::stringstream ss;
			for (int k = 0; k < ARRAYSIZE( msg.cmdData.cmd_s2c_game_start.cbCardData); k++)
			{
				ss << (int)msg.cmdData.cmd_s2c_game_start.cbCardData[k] << ",";
				//LLOG_DEBUG(" desk[%d] pos[%d] Card index: %d value: %d", m_desk?m_desk->GetDeskId():0,i,k, msg.cmdData.cmd_s2c_game_start.cbCardData[k]);
				//LLOG_DEBUG("m_cbHandCardData card index: %d value: %d", k, m_cbHandCardData[i][k]);
			}
			LLOG_ERROR(" desk[%d] pos[%d] Card [%s]", m_desk ? m_desk->GetDeskId() : 0, i, ss.str().c_str());
		}

		if (is_linfen() || m_playtype.DouDiZhuIsTi()!=0)
		{
			//����������Ϊ1
			for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
				m_cbBeiShu[i] = 1;
		
			//ˢ�±���
			//if (is_linfen())
				OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
		}

		//�����˿�
		for (WORD i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			switch (m_playtype.DouDiZhuGetGameMode())
			{
			case PLAY_TYPE_NORMAL:
			{
				m_GameLogicClassical.SortCardList(m_cbHandCardData[i], m_cbHandCardCount[i], ST_ORDER);
				break;
			}
			case PLAY_TYPE_LAIZI:
			{
				m_GameLogicLaizi.SortCardList(m_cbHandCardData[i], m_cbHandCardCount[i]);
				break;
			}
			case PLAY_TYPE_LINFEN:
			{
				m_GameLogicLinfen.SortCardList(m_cbHandCardData[i], m_cbHandCardCount[i], ST_ORDER);
				break;
			}

			}
			//m_GameLogicClassical.SortCardList(m_cbHandCardData[i], m_cbHandCardCount[i], ST_ORDER);
		}		

		return true;
	}

	//�û��з�
	bool OnUserCallScore(WORD wChairID, BYTE cbCallScore)
	{
		LLOG_ERROR("OnUserCallScore desk:%d pos:%d call_score:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, cbCallScore);
	
		//Ч��״̬
		if (wChairID != m_wCurrentUser)
		{
			LLOG_ERROR("OnUserCallScore userPos[%d] not currentPos[%d]", wChairID, m_wCurrentUser);
			return true;
		}

		//���ýз�
		m_cbScoreInfo[wChairID] = cbCallScore;

		//Ч�����
		if (  ((cbCallScore<1) || (cbCallScore>3) || (cbCallScore <= m_cbBankerScore))    && (cbCallScore != 255)  )
			cbCallScore = 255;

		//����״̬
		if (cbCallScore != 0xFF)
		{
			m_cbBankerScore = cbCallScore;
			m_wBankerUser = m_wCurrentUser;
		}

	

		//�����û�
		if ((m_cbBankerScore == 3) || (m_wFirstUser ==GetNextPos (wChairID) )  ) 
		{
			m_wCurrentUser = DOUDIZHU_INVAILD_POS;
		}
		else
		{
			m_wCurrentUser = GetNextPos(wChairID);
		}		

		//���ͽз�
		MHLMsgDouDiZhuS2CGameMessage msg;
		msg.sub_cmd_code = SUB_S_CALL_SCORE;
		msg.cmdData.cmd_s2c_call_score.wCallScoreUser = wChairID;
		msg.cmdData.cmd_s2c_call_score.wCurrentUser = m_wCurrentUser;
		msg.cmdData.cmd_s2c_call_score.cbUserCallScore = m_cbBankerScore;
		msg.cmdData.cmd_s2c_call_score.cbCurrentScore = m_cbScoreInfo[wChairID];

		if (is_linfen() && m_wCurrentUser == m_onlyCallUser)
			msg.cmdData.cmd_s2c_call_score.cbOnlyCall = 1;
		if (is_normal() && m_wCurrentUser == m_onlyCallUser)
			msg.cmdData.cmd_s2c_call_score.cbOnlyCall = 1;

		notify_desk(msg);
	

		//��ʼ�ж�
		if ((m_cbBankerScore == 3) || (m_wFirstUser == (wChairID + 1) % DOUDIZHU_PLAY_USER_COUNT))
		{
			//���˽з�
			if (m_cbBankerScore == 0)
			{
				//if ((m_pGameServiceOption->wServerType&GAME_GENRE_MATCH) == 0)
				//{
				//	if (m_cbCallScoreCount < 2)
				//	{
				//		m_cbCallScoreCount++;
				//		return OnEventGameStart();
				//	}
				//}
				//return OnEventGameStart();
				//m_wBankerUser = m_wFirstUser;
				//m_cbBankerScore = 1;
				 start_round(m_user_status);
				 return true;
			}

		
			//����״̬			
			set_play_status(DOUDIZHU_GAME_SCENE_PLAY);

			//���ñ���
			if (m_cbBankerScore == 0) m_cbBankerScore = 1;
			if (m_playtype.DouDiZhuIsTi() != 0 || is_linfen())m_cbBankerScore = 1;
			if (m_wBankerUser == DOUDIZHU_INVAILD_POS) m_wBankerUser = m_wFirstUser;
			m_cbCallScoreCount = 0;

			//���͵���
			Lint userHandCardCount = PLAY_TYPE_CARD_HAND_COUNT;
			Lint restCardCount = PLAY_TYPE_CARD_REST_COUNT;
			switch (m_playtype.DouDiZhuGetGameMode())
			{
			case PLAY_TYPE_NORMAL:
			{
				m_cbHandCardCount[m_wBankerUser] += PLAY_TYPE_CARD_REST_COUNT;
				CopyMemory(&m_cbHandCardData[m_wBankerUser][userHandCardCount], m_cbBankerCard, sizeof(m_cbBankerCard));
				m_GameLogicClassical.SortCardList(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], ST_ORDER);
				break;
			}
			case PLAY_TYPE_LAIZI:
			{
				restCardCount = PLAY_TYPE_CARD_REST_COUNT_4;
				m_cbHandCardCount[m_wBankerUser] += PLAY_TYPE_CARD_REST_COUNT_4;
				CopyMemory(&m_cbHandCardData[m_wBankerUser][userHandCardCount], m_cbBankerCard, sizeof(m_cbBankerCard));
				m_GameLogicLaizi.SortCardList(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser]);
				break;
			}
			case PLAY_TYPE_LINFEN:
			{
				restCardCount = PLAY_TYPE_CARD_REST_COUNT_4;
				userHandCardCount = PLAY_TYPE_CARD_HAND_COUNT_LINFEN;
				m_cbHandCardCount[m_wBankerUser] += PLAY_TYPE_CARD_REST_COUNT_4;
				CopyMemory(&m_cbHandCardData[m_wBankerUser][userHandCardCount], m_cbBankerCard, sizeof(m_cbBankerCard));
				m_GameLogicLinfen.SortCardList(m_cbHandCardData[m_wBankerUser], m_cbHandCardCount[m_wBankerUser], ST_ORDER);
				break;
			}
			}
		

			//�����û�
			m_wTurnWiner = (m_wBankerUser+2)% DOUDIZHU_PLAY_USER_COUNT;
			m_wCurrentUser = m_wBankerUser;

			//������Ϣ--ׯ����Ϣ
			for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				//����ׯ�ҽз�
				MHLMsgDouDiZhuS2CGameMessage msg;
				CMD_S_BankerInfo & bankerInfo = msg.cmdData.cmd_s2c_banker_info;
				msg.sub_cmd_code = SUB_S_BANKER_INFO;
				bankerInfo.wBankerUser = m_wBankerUser;
				bankerInfo.wCurrentUser = m_wCurrentUser;
				bankerInfo.cbBankerScore = m_cbBankerScore;
				bankerInfo.cbBankerCardCount = restCardCount;

			
				if (m_playtype.DouDiZhuDiPaiType() == 2 && i != m_wBankerUser)
				{
					RtlFillMemory(bankerInfo.cbBankerCard, sizeof(m_cbBankerCard), CARD_BACK_PATTERN);
					notify_user(msg, i);					
				}
				else
				{
					CopyMemory(bankerInfo.cbBankerCard, m_cbBankerCard, sizeof(m_cbBankerCard));
					notify_user(msg, i);					
				}
			}


			//����ط�����,���ƺ;���
			if (m_desk)
			{
				std::vector<Lint> handCard[DOUDIZHU_PLAY_USER_COUNT];
				std::vector<Lint> retCard;
				for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
				{
					for (int j = 0; j < PLAY_TYPE_CARD_REST_MAX + PLAY_TYPE_CARD_HAND_MAX; j++)
						handCard[i].push_back((Lint)m_cbHandCardData[i][j]);
				}
				for (int i = 0; i < PLAY_TYPE_CARD_REST_MAX; i++)
					retCard.push_back((Lint)m_cbBankerCard[i]);
				LTime  tempNow;
				m_video.DealCard(m_round_offset, DOUDIZHU_PLAY_USER_COUNT, m_desk->m_vip->m_posUserId, handCard, retCard, tempNow.Secs(), (Lint)m_wBankerUser, m_cbBankerScore);
			}
	
			if (is_linfen() || m_playtype.DouDiZhuIsTi()!=0)
			{
				
					//ׯ�ұ���+1
				m_cbBeiShu[m_wBankerUser]++;
					//ˢ�±���
				//if (is_linfen())
				//{
					OnEventUserFlushScore(m_wBankerUser);
				//}

				//�������Ƶ���Ϣ
				if ((m_wBankerUser + 1) % DOUDIZHU_PLAY_USER_COUNT != m_wFirstUser)
				{

					WORD tiPaiUser = (m_wBankerUser + 1) % DOUDIZHU_PLAY_USER_COUNT;  //�����û�
					 //���ͽз�
					MHLMsgDouDiZhuS2CGameMessage msg;
					msg.sub_cmd_code = SUB_S_LINFEN_TIPAI;
					msg.cmdData.cmd_s_notify_user_ti.m_pos = (int)tiPaiUser;
					notify_desk(msg);
					//notify_user(msg, tiPaiUser);

					this->set_play_status(GAME_SCENE_LINFEN_TIPAI);

					//�����û�
					m_wCurrentUser = tiPaiUser;
					m_wTiPaiUser = tiPaiUser;

				}
				else
				{
					OnEventBankerOutCard();
				}

			}
		}
		

		return true;
	}

	//�û�����
	bool OnUserOutCardLaizi(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
	{
		LLOG_ERROR("OnUserOutCardLaizi desk:%d pos:%d curPos:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser);
		//Ч��״̬
		if (wChairID != m_wCurrentUser) return true;

		//��ȡ����
		BYTE cbCardType;
		if (m_GameLogicLaizi.IsHadRoguishCard(cbCardData, cbCardCount))     //  �����
			cbCardType = m_GameLogicLaizi.GetMagicCardType(cbCardData, cbCardCount);
		else                                                                                                               //�����
			cbCardType = m_GameLogicLaizi.GetCardType(cbCardData, cbCardCount, m_playtype.DouDiZhuIsTongHua3() == 0 ? false : true);

	
		//if ((cbCardType == CT_THREE && m_cbHandCardCount[wChairID] > 3) || (cbCardType == CT_THREE_LINE && m_cbHandCardCount[wChairID] > 9) || (cbCardType == ��C��//_HREE_LINE && m_cbHandCardCount[wChairID] > 12))
		//{
		//	LLOG_ERROR("OnUserOutCardLaizi desk:%d pos:%d curPos:%d     cardtype [%d] error ", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser, cbCardType);
		//	return false;
		//}
		

		//�����ж�
		if (cbCardType == CT_ERROR)
		{
			LLOG_ERROR("OnUserOutCardLaizi desk:%d pos:%d curPos:%d   ERROR  cardtype ", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser);
			return false;
		}

		//�����ж�
		if (m_cbTurnCardCount != 0)
		{
			//�Ա��˿�
			if (m_GameLogicLaizi.CompareCard(cbCardData, m_cbTurnCardData, cbCardCount, m_cbTurnCardCount,m_playtype.DouDiZhuIsTongHua3()==0?false:true) == false)
			{
				LLOG_ERROR("OnUserOutCardLaizi desk:%d pos:%d curPos:%d   ERROR  CompareCard ", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser);
				return false;
			}
		}

		//ɾ���˿�
		if (m_GameLogicLaizi.RemoveCardList(cbCardData, cbCardCount, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]) == false)
		{
			ASSERT(FALSE);
			LLOG_ERROR("OnUserOutCardLaizi desk:%d pos:%d curPos:%d     RemoveCardList ", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser, cbCardType);
			return false;
		}

		//���Ʊ���
		m_cbOutCardCount[wChairID]++;

		//���ñ���
		m_cbTurnCardCount = cbCardCount;
		m_cbHandCardCount[wChairID] -= cbCardCount;
		CopyMemory(m_cbTurnCardData, cbCardData, sizeof(BYTE)*cbCardCount);

		//��ӡ��־
		//OnEventPrintCards(wChairID, m_cbHandCardDataLaizi[wChairID], m_cbHandCardCount[wChairID], TEXT("���ƺ�����"));

		//ը���ж�
		if ((cbCardType == CT_BOMB_CARD) || (cbCardType == CT_MISSILE_CARD) || (cbCardType == CT_BOMB_SOFT))
		{
			OnEventRecordBombCount(wChairID);
			//m_cbBombCount++;
			//m_cbEachBombCount[wChairID]++;
		}

		//�л��û�
		m_wTurnWiner = wChairID;
		if (m_cbHandCardCount[wChairID] != 0)
		{
			if (cbCardType != CT_MISSILE_CARD)
			{
				m_wCurrentUser = (m_wCurrentUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
			}
		}
		else m_wCurrentUser = DOUDIZHU_INVAILD_POS;

		//��������
		//CMD_S_OutCard OutCard;
		//OutCard.wOutCardUser = wChairID;
		//OutCard.cbCardCount = cbCardCount;
		//OutCard.wCurrentUser = m_wCurrentUser;
		//CopyMemory(OutCard.cbCardData, m_cbTurnCardData, m_cbTurnCardCount * sizeof(BYTE));

		//��������
		//WORD wHeadSize = sizeof(OutCard) - sizeof(OutCard.cbCardData);
		//WORD wSendSize = wHeadSize + OutCard.cbCardCount * sizeof(BYTE);

		//MHLMsgDouDiZhuS2CGameMessage msg;
		//msg.sub_cmd_code = SUB_S_LAIZI_OUT_CARD;
		//CopyMemory(&msg.cmdData.cmd_s2c_out_card, &OutCard, sizeof(OutCard));
		//notify_desk(msg);

		MHLMsgDouDiZhuS2CGameMessage msg;
		CMD_S_OutCard & cmdOutCard = msg.cmdData.cmd_s2c_out_card;
		msg.sub_cmd_code = SUB_S_OUT_CARD;
		cmdOutCard.wOutCardUser = wChairID;
		cmdOutCard.cbCardCount = cbCardCount;
		cmdOutCard.wCurrentUser = m_wCurrentUser;
		CopyMemory(cmdOutCard.cbCardData, m_cbTurnCardData, m_cbTurnCardCount * sizeof(BYTE));
		notify_desk(msg);

		//����طã��û�����
		SaveUserOper(1, wChairID, m_cbHandCardCount[wChairID], m_cbHandCardData[wChairID],cbCardCount, cbCardData);

		//�������
		if (cbCardType == CT_MISSILE_CARD) m_cbTurnCardCount = 0;

		//�����ж�
		if (m_wCurrentUser == DOUDIZHU_INVAILD_POS)
		{
			finish_round(1, wChairID,1);
			//OnEventGameConcludeLaizi(wChairID, NULL, GER_NORMAL);
		    // todo ������Ϸ
		}
		
		return true;
	}

	//�û�����
	bool OnUserOutCardLinfen(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
	{
		LLOG_ERROR("OnUserOutCardLinfen desk:%d pos:%d curPos:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser);
		//Ч��״̬
		ASSERT(wChairID == m_wCurrentUser);
		if (wChairID != m_wCurrentUser) return true;

		//��ȡ����
		BYTE cbCardType = m_GameLogicLinfen.GetCardType(cbCardData, cbCardCount);

		//�����ж�
		if (cbCardType == CT_ERROR)
		{
			ASSERT(FALSE);
			return false;
		}

		//if (cbCardType==CT_FOUR_TAKE_TWO)return false;
		//if (cbCardType==CT_THREE_TAKE_TWO)return false;
		if ((cbCardType == CT_THREE && m_cbHandCardCount[wChairID]>3) || (cbCardType == CT_THREE_LINE && m_cbHandCardCount[wChairID]>9) || (cbCardType == CT_THREE_LINE && m_cbHandCardCount[wChairID]>12))return false;

		//�����ж�
		if (m_cbTurnCardCount != 0)
		{
			//�Ա��˿�
			if (m_GameLogicLinfen.CompareCard(m_cbTurnCardData, cbCardData, m_cbTurnCardCount, cbCardCount) == false)
			{
				ASSERT(FALSE);
				return false;
			}
		}

		//ɾ���˿�
		if (m_GameLogicLinfen.RemoveCardList(cbCardData, cbCardCount, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]) == false)
		{
			ASSERT(FALSE);
			return false;
		}

		//���Ʊ���
		m_cbOutCardCount[wChairID]++;

		//���ñ���
		m_cbTurnCardCount = cbCardCount;
		m_cbHandCardCount[wChairID] -= cbCardCount;
		CopyMemory(m_cbTurnCardData, cbCardData, sizeof(BYTE)*cbCardCount);

	

		//ը���ж�
		if ((cbCardType == CT_BOMB_CARD) || (cbCardType == CT_MISSILE_CARD))
		{
			if (m_cbBombCount<m_playtype.DouDiZhuBombMaxCount())
			{
				m_cbBombCount++;
				m_cbEachBombCount[wChairID]++;

				if (cbCardCount == 4 && m_GameLogicLinfen.GetCardLogicValue(cbCardData[0]) == 3)
				{
					for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
						m_cbBeiShu[i] *= 2;

					for (int tempi = 0; tempi < 2; tempi++)
					{
						if (m_cbBombCount < m_playtype.DouDiZhuBombMaxCount())
						{
							for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
								m_cbBeiShu[i] *= 2;
							m_cbBombCount++;
							m_cbEachBombCount[wChairID]++;
						}
					}


				}
				else
				{    //����
					for (BYTE i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
					{
						m_cbBeiShu[i] *= 2;
					}
				}

				OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);

				//��ӡ��־
				//char szString[512] =("");
				//_snprintf(szString, CountArray(szString), ("ը����ը������[%d]���û���������[0]=[%d],[1]-[%d],[2]=[%d]"), m_cbBombCount, m_cbBeiShu[0], m_cbBeiShu[1], m��//_BeiShu��[2]);
				//LLOG_DEBUG(szString);
			}
			else
			{
				//��ӡ��־
				//char szString[512] = ("");
				//_snprintf(szString, CountArray(szString), ("ը���ⶥ���ټƷ֣�ը������[%d]"), m_cbBombCount);
				//LLOG_DEBUG(szString);
				LLOG_ERROR("desk[%d] bomb  max ,score no add .bombcount[%d],bombLimit[%d]", m_desk ? m_desk->GetDeskId() : 0, m_cbBombCount, m_playtype.DouDiZhuBombMaxCount());
			}

			m_cbBombCountReal++;
			m_cbEachBombCountReal[wChairID]++;

		}

		//�л��û�
		m_wTurnWiner = wChairID;
		if (m_cbHandCardCount[wChairID] != 0)
		{
			if (cbCardType != CT_MISSILE_CARD)
			{
				m_wCurrentUser = (m_wCurrentUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
			}
		}
		else m_wCurrentUser = DOUDIZHU_INVAILD_POS;

		MHLMsgDouDiZhuS2CGameMessage msg;
		CMD_S_OutCard & cmdOutCard = msg.cmdData.cmd_s2c_out_card;
		msg.sub_cmd_code = SUB_S_OUT_CARD;
		cmdOutCard.wOutCardUser = wChairID;
		cmdOutCard.cbCardCount = cbCardCount;
		cmdOutCard.wCurrentUser = m_wCurrentUser;
		CopyMemory(cmdOutCard.cbCardData, m_cbTurnCardData, m_cbTurnCardCount * sizeof(BYTE));
		notify_desk(msg);
	
		//����طã��û�����
		SaveUserOper(1, wChairID, m_cbHandCardCount[wChairID], m_cbHandCardData[wChairID], cbCardCount, cbCardData, m_cbBeiShu[wChairID]);

		//�������
		if (cbCardType == CT_MISSILE_CARD) m_cbTurnCardCount = 0;

		//�����ж�
		if (m_wCurrentUser == DOUDIZHU_INVAILD_POS)
		{
			finish_round(1, wChairID, 1);
			//OnEventGameConcludeLinfen(wChairID, NULL, GER_NORMAL);
			//todo ������Ϸ
		}
		return true;
	}

	//�û�����
	bool OnUserOutCard(WORD wChairID, BYTE cbCardData[], BYTE cbCardCount)
	{

		LLOG_ERROR("OnUserOutCard desk:%d pos:%d curPos:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, m_wCurrentUser);
		
		//Ч��״̬
		if (wChairID != m_wCurrentUser) return true;

		//for (int i = 0; i < cbCardCount; i++)
		//{
		//	LLOG_ERROR("card %d value: %d", i, cbCardData[i]);
		//}
		//��ӡ�û�����
		//OnEventPrintCards(wChairID, cbCardData, cbCardCount, TEXT("����"));

		//��ȡ����
		BYTE cbCardType = m_GameLogicClassical.GetCardType(cbCardData, cbCardCount);

		//�����ж�
		if (cbCardType == CT_ERROR)
		{
			ASSERT(FALSE);
			LLOG_ERROR("m_GameLogicClassical.GetCardType Error");
			return false;
		}

		//�����ж�
		if (m_cbTurnCardCount != 0)
		{
			//�Ա��˿�
			if (m_GameLogicClassical.CompareCard(m_cbTurnCardData, cbCardData, m_cbTurnCardCount, cbCardCount) == false)
			{
				return false;
			}
		}

		//ɾ���˿�
		if (m_GameLogicClassical.RemoveCardList(cbCardData, cbCardCount, m_cbHandCardData[wChairID], m_cbHandCardCount[wChairID]) == false)
		{
			return false;
		}

		//���Ʊ���
		m_cbOutCardCount[wChairID]++;

		//���ñ���
		m_cbTurnCardCount = cbCardCount;
		m_cbHandCardCount[wChairID] -= cbCardCount;
		CopyMemory(m_cbTurnCardData, cbCardData, sizeof(BYTE)*cbCardCount);

		//ը���ж�
		if ((cbCardType == CT_BOMB_CARD) || (cbCardType == CT_MISSILE_CARD))
		{
			OnEventRecordBombCount(wChairID);
			//m_cbBombCount++;
			//m_cbEachBombCount[wChairID]++;
		}

		//�л��û�
		m_wTurnWiner = wChairID;
		if (m_cbHandCardCount[wChairID] != 0)
		{
			if (cbCardType != CT_MISSILE_CARD)
			{
				m_wCurrentUser = (m_wCurrentUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
			}
		}
		else m_wCurrentUser = DOUDIZHU_INVAILD_POS;

	
		MHLMsgDouDiZhuS2CGameMessage msg;
		CMD_S_OutCard & cmdOutCard = msg.cmdData.cmd_s2c_out_card;
		msg.sub_cmd_code = SUB_S_OUT_CARD;
		cmdOutCard.wOutCardUser = wChairID;
		cmdOutCard.cbCardCount = cbCardCount;
		cmdOutCard.wCurrentUser = m_wCurrentUser;
		CopyMemory(cmdOutCard.cbCardData, m_cbTurnCardData, m_cbTurnCardCount * sizeof(BYTE));
		notify_desk(msg);

		//����طã��û�����
		SaveUserOper(1, wChairID, m_cbHandCardCount[wChairID], m_cbHandCardData[wChairID], cbCardCount, cbCardData);

		//�������
		if (cbCardType == CT_MISSILE_CARD) m_cbTurnCardCount = 0;

		//�����ж�
		if (m_wCurrentUser == DOUDIZHU_INVAILD_POS)
		{
			finish_round(1, wChairID,1);
			//this->OnGameOver(1, wChairID, DOUDIZHU_INVAILD_POS);
		}
	

		return true;
	}

	//�û�����
	bool OnUserTiPai(WORD wChairID, BYTE cbIsYes)
	{
		LLOG_ERROR("OnUserTiPai desk:%d pos:%d cbIsYes:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, cbIsYes);
		//Ч��״̬
		if (wChairID != m_wCurrentUser) return true;

		//�㲥�û�TI���
		OnEventBroadcaseTiResult(SUB_S_LINFEN_TIPAI, wChairID, cbIsYes);

		//�û�����
		if (cbIsYes)
		{
			m_cbBeiShu[wChairID]++;
			m_cbBeiShu[m_wBankerUser]++;

			m_cbIsTi[wChairID] = 1;   //�û�����

			//����ˢ�±���
			if (is_linfen()|| m_playtype.DouDiZhuIsTi()!=0)
			{
				OnEventUserFlushScore(m_wBankerUser);
				OnEventUserFlushScore(wChairID);
			}

			//�ҵ������û�
			WORD nextUser = (wChairID + 1) % DOUDIZHU_PLAY_USER_COUNT;
			if (nextUser == m_wBankerUser)
				nextUser = (wChairID + 2) % DOUDIZHU_PLAY_USER_COUNT;
			//�Ƿ����
			//m_pITableFrame->SendTableData(nextUser, SUB_S_LINFEN_GENTI, NULL, 0);
			MHLMsgDouDiZhuS2CGameMessage msg;
			msg.sub_cmd_code = SUB_S_LINFEN_GENTI;
			msg.cmdData.cmd_s_notify_user_ti.m_pos = nextUser;
			notify_desk(msg);
			//notify_user(msg, nextUser);

			//����״̬
			//m_pITableFrame->SetGameStatus(GAME_SCENE_LINFEN_GENTI);
			set_play_status(GAME_SCENE_LINFEN_GENTI);

			//�����û�
			m_wCurrentUser = nextUser;
			m_wGenTiUser = nextUser;

			//��ӡ��־
			//char szString[512]=("");
			//_snprintf(szString, CountArray(szString), ("�������û�chairid[%d]������Ϣ������[%d]����[%d],�����û�[%d]����[%d]"), nextUser, m_wBankerUser, m_cbBeiShu//[m_wBankerUser],wChairID, m_cbBeiShu[wChairID]);
			//LLOG_DEBUG(szString);
		}
		else // �û�����
		{
			//�����û�����
			m_cbIsTi[wChairID] = 0;   //�û�����

			WORD  nextUser = (wChairID + 1) % DOUDIZHU_PLAY_USER_COUNT;
			if (nextUser == m_wBankerUser || nextUser == m_wFirstUser)
			{
				OnEventBankerOutCard();
			}
			else
			{     //�¸��û��Ƿ�����
				  //������Ϣ
				//m_pITableFrame->SendTableData(nextUser, SUB_S_LINFEN_TIPAI, NULL, 0);
				//m_pITableFrame->SendLookonData(nextUser, SUB_S_LINFEN_TIPAI, NULL, 0);

				MHLMsgDouDiZhuS2CGameMessage msg;
				msg.sub_cmd_code = SUB_S_LINFEN_TIPAI;
				msg.cmdData.cmd_s_notify_user_ti.m_pos = (Lint)nextUser;
				notify_desk(msg);
				//notify_user(msg, nextUser);

				//�����û�
				m_wCurrentUser = nextUser;
				m_wTiPaiUser = nextUser;
			}

		}

		return true;
	}

	//�û�����
	bool OnUserGenTi(WORD wChairID, BYTE cbIsYes)
	{
		LLOG_ERROR("OnUserGenTi desk:%d pos:%d cbIsYes:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, cbIsYes);
		//Ч��״̬
		if (wChairID != m_wCurrentUser) return true;

		//Ч�����

		//�㲥�û�TI���
		OnEventBroadcaseTiResult(SUB_S_LINFEN_GENTI, wChairID, cbIsYes);

		//�û�����
		if (cbIsYes)
		{
			m_cbBeiShu[wChairID]++;
			m_cbBeiShu[m_wBankerUser]++;

			m_cbIsTi[wChairID] = 2;   //�û�����

			 //����ˢ�±���
			if (is_linfen()|| m_playtype.DouDiZhuIsTi() != 0)
			{
				OnEventUserFlushScore(wChairID);
				OnEventUserFlushScore(m_wBankerUser);
			}
		}
		else
			m_cbIsTi[wChairID] = 0;   //�û�������


		MHLMsgDouDiZhuS2CGameMessage msg;
		msg.sub_cmd_code = SUB_S_LINFEN_HUIPAI;
		msg.cmdData.cmd_s_notify_user_ti.m_pos = m_wBankerUser;
		notify_desk(msg);
		//notify_user(msg, m_wBankerUser);

		//�����û�
		m_wCurrentUser = m_wBankerUser;

		//����״̬
		//m_pITableFrame->SetGameStatus(GAME_SCENE_LINFEN_HUITI);
		this->set_play_status(GAME_SCENE_LINFEN_HUITI);

		//��ӡ��־
		//TCHAR szString_end[512]=TEXT("");
		//_sntprintf(szString_end, CountArray(szString_end), TEXT("����[%d]����[%d],�����û�[%d]����[%d]"), m_wBankerUser, m_cbBeiShu[m_wBankerUser], wChairID, m_cbBeiShu[wChairID]);
		//CTraceService::TraceString(szString_end, TraceLevel_Normal);

		return true;
	}

	//�û�����
	bool OnUserHuiPai(WORD wChairID, BYTE cbIsYes)
	{
		LLOG_ERROR("OnUserHuiPai desk:%d pos:%d cbIsYes:%d", m_desk ? m_desk->GetDeskId() : 0, wChairID, cbIsYes);
		//��ӡ��־
		//TCHAR szString[512]=TEXT("");
		//_sntprintf(szString, CountArray(szString), TEXT("�û�chairId=[%d], �Ƿ����[%d]"), wChairID, cbIsYes);
		//CTraceService::TraceString(szString, TraceLevel_Normal);

		//Ч��״̬
		if (wChairID != m_wCurrentUser) return true;

		if (wChairID != m_wBankerUser) return true;

		//Ч�����

		//�㲥�û�TI���
		OnEventBroadcaseTiResult(SUB_S_LINFEN_HUIPAI, wChairID, cbIsYes);

		//�û�����
		if (cbIsYes)
		{
			m_cbIsTi[wChairID] = 3;   //�û�����

									  //����ˢ�±���
			for (BYTE i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (i == m_wBankerUser)continue;
				if (m_cbIsTi[i]>0)
				{
					m_cbBeiShu[i] += 2;
					m_cbBeiShu[m_wBankerUser] += 2;
					//OnEventUserFlushScore(i);
				}
			}
			if(is_linfen()|| m_playtype.DouDiZhuIsTi() != 0)
				OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);

		}
		else
			m_cbIsTi[wChairID] = 0;   //�û�������

		OnEventBankerOutCard();

		return true;
	}

	//�û�����
	bool OnUserPassCard(WORD wChairID)
	{
		LLOG_ERROR("OnUserPassCard desk:%d pos:%d ", m_desk ? m_desk->GetDeskId() : 0, wChairID);
		//Ч��״̬
		if ((wChairID != m_wCurrentUser) || (m_cbTurnCardCount == 0)) return true;

		//���ñ���
		m_wCurrentUser = (m_wCurrentUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
		if (m_wCurrentUser == m_wTurnWiner) m_cbTurnCardCount = 0;

		
		//֪ͨ����
		MHLMsgDouDiZhuS2CGameMessage msg;
		msg.cmdData.cmd_s2c_pass_card.wPassCardUser = wChairID;
		msg.cmdData.cmd_s2c_pass_card.wCurrentUser = m_wCurrentUser;
		msg.cmdData.cmd_s2c_pass_card.cbTurnOver = (m_cbTurnCardCount == 0) ? TRUE : FALSE;
		msg.sub_cmd_code = SUB_S_PASS_CARD;
		
		notify_desk(msg);

		//����طã��û�����
		SaveUserOper(0, wChairID, m_cbHandCardCount[wChairID], m_cbHandCardData[wChairID]);

		return true;
	}

	//��Ϸ��Ϣ
	bool OnGameMessage(WORD wSubCmdID, VOID * pData, WORD wDataSize, User * pUser)
	{
		switch (wSubCmdID)
		{
		case SUB_C_CALL_SCORE:	//�û��з�
		{
			if (m_play_status != DOUDIZHU_GAME_SCENE_CALL)
			{
				LLOG_ERROR("�û��з�m_play_status_error ");
				return true;
			}

			//��������
			CMD_C_CallScore * pCallScore = (CMD_C_CallScore *)pData;

			//��Ϣ����
			WORD wChairID = GetUserPos(pUser);
			return OnUserCallScore(wChairID, pCallScore->cbCallScore);
		}
		case SUB_C_OUT_CARD:	//�û�����
		{
			//��������
			CMD_C_OutCard * pOutCard = (CMD_C_OutCard *)pData;
			
			if (m_play_status != DOUDIZHU_GAME_SCENE_PLAY)
			{
				LLOG_ERROR("m_play_status error");
			}

			//��Ϣ����
			WORD wChairID = GetUserPos(pUser);
			
			switch (m_playtype.DouDiZhuGetGameMode())
			{
			case PLAY_TYPE_NORMAL:
			{
				return OnUserOutCard(wChairID, pOutCard->cbCardData, pOutCard->cbCardCount);
			}
			case PLAY_TYPE_LAIZI:
			{
				return OnUserOutCardLaizi(wChairID, pOutCard->cbCardData, pOutCard->cbCardCount);

			}
			case PLAY_TYPE_LINFEN:
			{
				return OnUserOutCardLinfen(wChairID, pOutCard->cbCardData, pOutCard->cbCardCount);

			}
			}

			
		}
		case SUB_C_PASS_CARD:	//�û�����
		{
			//״̬Ч��
			if (m_play_status != DOUDIZHU_GAME_SCENE_PLAY) return true;

			//��Ϣ����
			return OnUserPassCard(GetUserPos(pUser));
		}
		case SUB_C_TIPAI:
		{
			if (m_play_status != GAME_SCENE_LINFEN_TIPAI)
			{
				LLOG_ERROR("m_play_status != GAME_SCENE_LINFEN_TIPAI ERROR");
				return true;
			}

			//��������
			CMD_C_Ti * pTiPai = (CMD_C_Ti *)pData;

			//��Ϣ����
			WORD wChairID = GetUserPos(pUser);
			LLOG_DEBUG("OnGameMessage....SUB_C_TIPAI[%d]", pTiPai->cbIsYes);
			return OnUserTiPai(wChairID, pTiPai->cbIsYes);

		}
		case SUB_C_GENTI:
		{
		
			if (m_play_status != GAME_SCENE_LINFEN_GENTI) return true;

			//��������
			CMD_C_Ti * pGenTi = (CMD_C_Ti *)pData;

			//��Ϣ����
			WORD wChairID = GetUserPos(pUser);
			return OnUserGenTi(wChairID, pGenTi->cbIsYes);

		}
		case SUB_C_HUIPAI:
		{
		
			if (m_play_status != GAME_SCENE_LINFEN_HUITI) return true;

			//��������
			CMD_C_Ti * pHuiPai = (CMD_C_Ti *)pData;

			//��Ϣ����
			WORD wChairID = GetUserPos(pUser);
			return OnUserHuiPai(wChairID, pHuiPai->cbIsYes);

		}
		}

		return false;
	}

	//һ����Ϸ��ʼ·��
	void start_game()
	{
		//LLOG_DEBUG("start_game  ....[%d]", m_play_status);
		//
		m_zhuangPos = m_desk->m_zhuangPos;

	
		OnEventGameStart();
		return;
	
			
	}

	//��ʼһ��
	void start_round(Lint * PlayerStatus) {
		//LLOG_DEBUG("Logwyz  ...    START  GAME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!");
		if (NULL == PlayerStatus)  return;

		Lint t_user_status[DOUDIZHU_PLAY_USER_COUNT];
		memcpy(t_user_status, PlayerStatus, sizeof(Lint) * DOUDIZHU_PLAY_USER_COUNT);

		clear_round();
		m_video.DrawClear();   //�����һ�ֻطü�¼
		notify_desk_match_state();
		set_desk_state(DESK_PLAY);

		memcpy(m_user_status, t_user_status, sizeof(m_user_status));

		//֪ͨmanager�����仯
		if (m_desk && m_desk->m_vip)
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);
		LLOG_ERROR("****Desk %d start round %d/%d   player_count=%d", m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);
		//LLOG_DEBUG("GameMode=[%d],TuiTongZiGetScoreTime=[%d],TuiTongZiGetAuto=[%d],TuiTongZiGetCuoPai=[%d]", m_playtype., m_playtype.TuiTongZiGetScoreTime(), m_playtype.TuiTongZiGetAuto(), m_playtype.TuiTongZiGetCuoPai());



		//��Ϸ��ʼ���
		start_game();
		LLOG_ERROR("****Desk %d start round %d/%d, player_count=%d", m_desk->GetDeskId(), m_round_offset, m_round_limit, m_player_count);
	}

	//����һ��
	void  finish_round(Lint result, Lint winpos, Lint reason)
	{
		LLOG_DEBUG("Doudizhu - finish_round desk:%d", m_desk ? m_desk->GetDeskId() : 0);

		increase_round();

		MHLMsgDouDiZhuS2CGameMessage send;
		send.sub_cmd_code = SUB_S_GAME_CONCLUDE;

		CMD_S_GameConclude &over = send.cmdData.cmd_s2c_game_conclude;
		over.cbIsFinalDraw = m_dismissed || m_round_offset >= m_round_limit;
		over.cbBankerUser = m_wBankerUser;
		over.lCellScore = m_playtype.DouDiZhuCellScore();
		over.cbBombCount = m_cbBombCountReal;
		if(is_linfen())
			over.cbBombCount = m_cbBombCount;
		over.cbBankerCall = m_cbBankerScore;
		CopyMemory(over.cbEachBombCount, m_cbEachBombCountReal, sizeof(m_cbEachBombCountReal));

		for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			over.cbActSelect[i] = (m_cbIsTi[i] == 0xff) ? 0 : m_cbIsTi[i];
		}

		for (int i = 0; i < PLAY_TYPE_CARD_REST_MAX; i++)
		{
			over.cbRestCardData[i] = m_cbBankerCard[i];
		}


		for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			over.cbCardCount[i] = m_cbHandCardCount[i];
			for (int j = 0; j < PLAY_TYPE_CARD_HAND_MAX + PLAY_TYPE_CARD_REST_MAX; j++)
			{
				over.cbHandCardData[i][j] = m_cbHandCardData[i][j];
			}
		}

		for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
		{
			if (m_desk->m_user[i])
			{
				send.m_usernikes.push_back(m_desk->m_user[i]->GetUserData().m_nike);
				send.m_userids.push_back(m_desk->m_user[i]->GetUserDataId());
				send.m_headUrl.push_back(m_desk->m_user[i]->GetUserData().m_headImageUrl);
			}
		}


		// �洢����
		//Lint scores[DOUDIZHU_PLAY_USER_COUNT] = { 0 };
		if (result == 1) // ��������
		{		
			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				over.cbBankerScore[i] = m_cbBankerScore;
			}
			
			if (is_linfen())
			{
				//�����ж�
				if (winpos == (int)m_wBankerUser)
				{
					//�û�����
					WORD wUser1 = (m_wBankerUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
					WORD wUser2 = (m_wBankerUser + 2) % DOUDIZHU_PLAY_USER_COUNT;

					//�û��ж�
					if ((m_cbOutCardCount[wUser1] == 0) && (m_cbOutCardCount[wUser2] == 0))
					{
						over.bChunTian = TRUE;
						if (m_cbBombCount < m_playtype.DouDiZhuBombMaxCount())
						{
							for (int k = 0; k < DOUDIZHU_PLAY_USER_COUNT; k++)
							{
								m_cbBeiShu[k] *= 2;
							}
							//�ٷ�ˢ�±���
							if (is_linfen())
							{
								OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
							}
						}
					}
				}
				else if (m_cbOutCardCount[m_wBankerUser] == 1)
				{
					over.bFanChunTian = TRUE;
					if (m_cbBombCount < m_playtype.DouDiZhuBombMaxCount())
					{
						for (BYTE k = 0; k < DOUDIZHU_PLAY_USER_COUNT; k++)
							m_cbBeiShu[k] *= 2;
						//�ٷ�ˢ�±���
						if (is_linfen())
						{
							OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
						}
					}
				}
				for (WORD i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
				{
					if ((winpos != m_wBankerUser && i != m_wBankerUser) || (winpos == m_wBankerUser && i == m_wBankerUser))
					{
						over.lGameScore[i] = m_playtype.DouDiZhuCellScore()*m_cbBeiShu[i];
					}
					else if ((winpos != m_wBankerUser && i == m_wBankerUser) || (winpos == m_wBankerUser && i != m_wBankerUser))
					{
						over.lGameScore[i] = (-1)*m_playtype.DouDiZhuCellScore()*m_cbBeiShu[i];
					}
				}
			}
			else
			{
				Lint lScoreTimes = 1 * m_cbBankerScore;

				//if ((is_laizi() || is_normal()) && m_playtype.  DouDiZhuIsTi() == 1)
				//	lScoreTimes = 1 * 1;

				for (BYTE i = 0; i < m_cbBombCount; i++)
				{
					lScoreTimes *= 2;
				}

				//�����ж�
				if (winpos == (int)m_wBankerUser)
				{
					//�û�����
					WORD wUser1 = (m_wBankerUser + 1) % DOUDIZHU_PLAY_USER_COUNT;
					WORD wUser2 = (m_wBankerUser + 2) % DOUDIZHU_PLAY_USER_COUNT;

					//�û��ж�
					if ((m_cbOutCardCount[wUser1] == 0) && (m_cbOutCardCount[wUser2] == 0))
					{
						lScoreTimes *= 2;
						over.bChunTian = TRUE;
						for (int k = 0; k < DOUDIZHU_PLAY_USER_COUNT; k++)
						{
							m_cbBeiShu[k] *= 2;
						}
						//�ٷ�ˢ�±���
						if (m_playtype.DouDiZhuIsTi()!=0)
						{
							OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
						}
					}
				}
				else if (m_cbOutCardCount[m_wBankerUser] == 1)
				{
					lScoreTimes *= 2;
					over.bFanChunTian = TRUE;
					for (BYTE k = 0; k < DOUDIZHU_PLAY_USER_COUNT; k++)
						m_cbBeiShu[k] *= 2;
					//�ٷ�ˢ�±���
					if (m_playtype.DouDiZhuIsTi() != 0)
					{
						OnEventUserFlushScore(DOUDIZHU_INVAILD_POS);
					}
				}

				//ͳ�ƻ���
				if (m_playtype.DouDiZhuIsTi() == 0)
				{
					for (WORD i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
					{
						//��������
						Lint lUserScore = 0L;
						Lint lCellScore = m_playtype.DouDiZhuCellScore();
						//���ֻ���
						if (i == m_wBankerUser)
						{
							lUserScore = (0 == m_cbHandCardCount[m_wBankerUser]) ? 2 : -2;
							//lUserScore = (0 == m_cbHandCardCount[m_wBankerUser]) ? m_cbBeiShu[m_wBankerUser]: -1* m_cbBeiShu[m_wBankerUser];
						}
						else
						{
							if (m_cbHandCardCount[m_wBankerUser] == 0)
							{
								lUserScore = -1;
							}
							else
							{
								lUserScore = 1;
							}
						}

						//�������
						over.lGameScore[i] = lUserScore * lCellScore * lScoreTimes;

						//m_accum_score[i] += scores[i];
						//over.lGameScore[i] = scores[i];		 
					}
				}
				else {
					for (WORD i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
					{
						if ((winpos != m_wBankerUser && i != m_wBankerUser) || (winpos == m_wBankerUser && i == m_wBankerUser))
						{
							over.lGameScore[i] = m_playtype.DouDiZhuCellScore()*m_cbBeiShu[i];
						}
						else if ((winpos != m_wBankerUser && i == m_wBankerUser) || (winpos == m_wBankerUser && i != m_wBankerUser))
						{
							over.lGameScore[i] = (-1)*m_playtype.DouDiZhuCellScore()*m_cbBeiShu[i];
						}
					}
				}
			}
			//�л��û�
			m_wFirstUser = (WORD)winpos;
		
			add_round_log(over.lGameScore, m_wBankerUser);

			for (Lint i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (m_desk && m_desk->m_vip)
				{
					send.m_totalScore.push_back(m_desk->m_vip->m_score[i]);
					//LLOG_DEBUG("over.score[%d]=[%d]", i, send.m_totalScore[i]);
				}
				else
					LLOG_ERROR("m_desk or  m_desk->m_vip is null");
			}

		}
		else if (result == 2)
		{
			add_round_log(over.lGameScore, m_wBankerUser);

			for (Lint i = 0; i<DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				if (m_desk && m_desk->m_vip)
				{
					send.m_totalScore.push_back(m_desk->m_vip->m_score[i]);
					//LLOG_DEBUG("over.score[%d]=[%d]", i, send.m_totalScore[i]);
				}
				else
					LLOG_ERROR("m_desk or  m_desk->m_vip is null");
			}
		}

		notify_desk(send);

		if (m_playtype.DouDiZhuCallZhuangMode() == 0)
			m_firstCallZhuang = winpos;
		else
			m_firstCallZhuang = GetNextPos(m_firstCallZhuang);

		set_desk_state(DESK_WAIT);
		if (m_desk) m_desk->HanderGameOver(1);		
	}
};


struct DouDiZhuGameHandler : DouDiZhuGameCore{

	void notify_user(LMsg &msg, int pos) override {
		if (NULL == m_desk) return;
		if (pos < 0 || pos >= m_player_count) return;
		User *u = m_desk->m_user[pos];
		if (NULL == u) return;
		u->Send(msg);
	}

	void notify_desk(LMsg &msg) override {
		if (NULL == m_desk) return;
		m_desk->BoadCast(msg);
	}

	void notify_desk_playing_user(LMsg &msg) {
		if (NULL == m_desk) return;
		m_desk->BoadCast2PlayingUser(msg);
	}

	void notify_desk_without_user(LMsg &msg, User* pUser)
	{
		if (NULL == m_desk) return;
		m_desk->BoadCastWithOutUser(msg, pUser);
	}

	bool startup(Desk *desk) {
		if (NULL == desk) return false;
		clear_match(desk->m_desk_user_count);
		m_desk = desk;
		return true;
	}

	void shutdown(void) {
		clear_match(0);
	}

	void SetPlayType(std::vector<Lint> &l_playtype) {
		// �� Desk �� SetVip ���ã���ʱ�ܵõ� m_vip->m_maxCircle
		int round_limit = m_desk && m_desk->m_vip ? m_desk->m_vip->m_maxCircle : 0;
		setup_match(l_playtype, round_limit);

		//����طû�����Ϣ
		if(m_desk)
			m_video.FillDeskBaseInfo(m_desk->GetDeskId(), m_desk->m_state, round_limit, l_playtype);

	}

	void MHSetDeskPlay(Lint PlayUserCount, Lint * PlayerStatus, Lint PlayerCount) {
		LLOG_DEBUG("Logwyz-------------MHSetDeskPlay(Lint PlayUserCount)=[%d]", PlayUserCount);
		if (!m_desk || !m_desk->m_vip) {
			LLOG_ERROR("MHSetDeskPlay  error !!!! m_desk or  m_desk->m_vip  null");
			return;
		}

		if (PlayUserCount != DOUDIZHU_PLAY_USER_COUNT || PlayerStatus == NULL || PlayerCount != DOUDIZHU_PLAY_USER_COUNT)
		{
			LLOG_ERROR("MHSetDeskPlay  error  !!!!PlayUserCount=[%d] ", PlayUserCount);
			return;
		}

		m_player_count = PlayUserCount;
		//memcpy(m_user_playing_status, PlayerStatus, sizeof(m_user_playing_status));
		//
		//for(int i=0; i<YINGSANZHANG_PLAY_USER_MAX;i++)
		//	LLOG_DEBUG("m_user_playing_status[%d]=[%d]",i, m_user_playing_status[i]);


		start_round(PlayerStatus);
	}

	//��Ϸ����
	void OnGameOver(Lint result, Lint winpos, Lint bombpos) {
		if (m_desk == NULL || m_desk->m_vip == NULL) {
			LLOG_ERROR("OnGameOver NULL ERROR ");
			return;
		}

		m_dismissed = !!m_desk->m_vip->m_reset;	
		finish_round(2, winpos, bombpos);
		LLOG_ERROR("***desk game over. desk:%d round_limit: %d round: %d dismiss: %d", m_desk ? m_desk->GetDeskId() : 0, m_round_limit, m_round_offset, m_dismissed);
	}

	//��Ϸ�ж�������
	void OnUserReconnect(User *pUser) {
		if (pUser == NULL || m_desk == NULL) 
		{
			return;
		}

		// ���͵�ǰȦ����Ϣ
		// ΪʲôҪ�������ӣ�������ֻ�������������������ң�
		notify_desk_match_state();

		Lint pos = GetUserPos(pUser);
		MHLOG_PLAYCARD("*******��������desk:%d userid:%d pos:%d m_wCurrentUser =%d ", m_desk->GetDeskId(), pUser->GetUserDataId(), pos, m_wCurrentUser);
		MHLMsgDouDiZhuS2CReconn reconn;
	
		reconn.kPlayStatus = (m_play_status > 20 ? 2 : m_play_status);
		switch(m_play_status)
		{
		case DOUDIZHU_GAME_SCENE_PLAY:
		{
			//reconn.kCellScore = m_desk->m_cellscore;
			//reconn.kCurrentUser = m_wCurrentUser;
			//reconn.kBankerUser = m_wBankerUser;
			//reconn.kBankerScore = m_cbBankerScore;
			//
			//for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			//{
			//	reconn.kScoreInfo[i] = m_cbScoreInfo[i];
			//	reconn.kHandCardCount[i] = m_cbHandCardCount[i];
			//}
			//for (int i = 0; i < PLAY_TYPE_CARD_HAND_MAX + PLAY_TYPE_CARD_REST_MAX; i++)
			//{
			//	reconn.kHandCardData[i] = m_cbHandCardData[pos][i];
			//}

			reconn.kBombCount = m_cbBombCount;
			reconn.kTurnWiner = m_wTurnWiner;
			reconn.kTurnCardCount = m_cbTurnCardCount;
			for (int i = 0; i < PLAY_TYPE_CARD_HAND_MAX + PLAY_TYPE_CARD_REST_MAX; i++)
			{
				reconn.kTurnCardData[i] = m_cbTurnCardData[i];
			}

			if (m_playtype.DouDiZhuDiPaiType())
			{
				for (int i = 0; i < PLAY_TYPE_CARD_REST_MAX; i++)
				{
					reconn.kBankerCard[i] = m_cbBankerCard[i];
				}
			}
			
		}
		case GAME_SCENE_LINFEN_TIPAI:
		case GAME_SCENE_LINFEN_GENTI:
		case GAME_SCENE_LINFEN_HUITI:
		{
			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				switch (m_cbIsTi[i])
				{
				case 0:
					reconn.ktype[i] = 131;
					break;
				case 1:
					reconn.ktype[i] = 131;
					reconn.kActSelect[i] = 1;
					break;
				case 2:
					reconn.ktype[i] = 132;
					reconn.kActSelect[i] = 1;
					break;
				case 3:
					reconn.ktype[i] = 133;
					reconn.kActSelect[i] = 1;
					break;

				}
			}

			if (m_playtype.DouDiZhuDiPaiType() != 1 && pos != m_wBankerUser)
			{
				for (int i = 0; i < PLAY_TYPE_CARD_REST_MAX; i++)
				{
					reconn.kBankerCard[i] = CARD_BACK_PATTERN;
				}
			}
			else
			{
				for (int i = 0; i < PLAY_TYPE_CARD_REST_MAX; i++)
				{
					reconn.kBankerCard[i] = m_cbBankerCard[i];
				}
			}
			
		}
		case DOUDIZHU_GAME_SCENE_CALL:
		{
			reconn.kCellScore = m_playtype.DouDiZhuCellScore();
			reconn.kCurrentUser = m_wCurrentUser;
			reconn.kBankerUser = m_wBankerUser;
			reconn.kBankerScore = m_cbBankerScore;

			if (is_linfen() && m_onlyCallUser == m_wCurrentUser)
				reconn.kOnlyCall = 1;
			if (is_normal() && m_onlyCallUser == m_wCurrentUser)
				reconn.kOnlyCall = 1;

			for (int i = 0; i < DOUDIZHU_PLAY_USER_COUNT; i++)
			{
				reconn.kScoreInfo[i] = m_cbScoreInfo[i];
				reconn.kHandCardCount[i] = m_cbHandCardCount[i];
			}
			for (int i = 0; i < PLAY_TYPE_CARD_HAND_MAX + PLAY_TYPE_CARD_REST_MAX; i++)
			{
				reconn.kHandCardData[i] = m_cbHandCardData[pos][i];
			}

			break;
		}
	
		default:break;
		}
		notify_user(reconn, pos);	

		if(is_linfen()|| m_playtype.DouDiZhuIsTi()!=0)
			OnEventUserFlushScore(pos);

		switch (m_play_status)
		{
		case GAME_SCENE_LINFEN_TIPAI:
		{
			if (pos == m_wTiPaiUser)
			{
				MHLMsgDouDiZhuS2CGameMessage msg;
				msg.sub_cmd_code = SUB_S_LINFEN_TIPAI;
				msg.cmdData.cmd_s_notify_user_ti.m_pos = pos;
				notify_desk(msg);
				//notify_user(msg, pos);
			}
			break;
		}
		case GAME_SCENE_LINFEN_GENTI:
		{
			if (pos == m_wGenTiUser)
			{
				MHLMsgDouDiZhuS2CGameMessage msg;
				msg.sub_cmd_code = SUB_S_LINFEN_GENTI;
				msg.cmdData.cmd_s_notify_user_ti.m_pos = pos;
				notify_desk(msg);
				//notify_user(msg, pos);
			}
			break;
		}
		case GAME_SCENE_LINFEN_HUITI:
		{
			if (pos == m_wBankerUser)
			{
				MHLMsgDouDiZhuS2CGameMessage msg;
				msg.sub_cmd_code = SUB_S_LINFEN_HUIPAI;
				msg.cmdData.cmd_s_notify_user_ti.m_pos = m_wBankerUser;
				notify_desk(msg);
				//notify_user(msg, pos);
			}
			break;
		}

		}
	}

	// �ͻ�����Ϣ
	bool HandleDouDiZhuGameMessage(User * pUser, MHLMsgDouDiZhuC2SGameMessage * msg)
	{
		if (!pUser) return false;
		Lint pos =  GetUserPos(pUser);
		if (!is_pos_valid(pos))
		{
			LLOG_ERROR("HandleDouDiZhuGameMessage- desk:%d invalid pos:%d", m_desk->GetDeskId(), pos);
			return false;
		}

		LLOG_DEBUG("***�������ͻ�����Ϣ deskId: %d userId: %d msg->sub_cmd_code: %d", m_desk->GetDeskId(), pUser->GetUserDataId(), msg->sub_cmd_code);

		//������Ϸ������ȡ��ÿ������������ݴ�С�����͸��²㴦��
		return OnGameMessage(msg->sub_cmd_code, &msg->cmdData, sizeof(msg->cmdData), pUser);
		
	}



	void Tick(LTime& curr)
	{
		//if (m_playtype.TuiTongZiGetAuto() == 0)return;
		//LLOG_DEBUG("TuiTongZiGameHandler--Tick,  play_status=[%d]", m_play_status);		
	}

};


DECLARE_GAME_HANDLER_CREATOR(101, DouDiZhuGameHandler);

