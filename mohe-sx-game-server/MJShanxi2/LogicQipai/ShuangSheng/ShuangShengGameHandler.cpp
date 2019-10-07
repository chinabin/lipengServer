#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "ShuangShengGameLogic.h"
#include "LMsgL2C.h"

/*�����ļ����ļ�����card.ini*/
#define DEBUG_CARD_FILE_SHUANGSHENG "card_shuangsheng.ini"

//�����ã�������
static int LoadGradeCard(std::vector<Lint>& grade, const char* conFileName)
{
	if (conFileName == NULL || std::strlen(conFileName) == 0)
	{
		return -1;
	}
	std::string fileName(conFileName);
	LIniConfig config;
	config.LoadFile(conFileName);

	std::string gradeString = config.GetString(Lstring("grade"), "");
	std::vector<std::string> gradeStringVec = splitString(gradeString, ",", true);

	if (gradeStringVec.size() > 0)
	{
		grade.clear();
		for (auto it = gradeStringVec.begin(); it < gradeStringVec.end(); it++)
		{
			char *str;
			int i = (int)strtol((*it).c_str(), &str, 16);
			BYTE nValue = (BYTE)i;
			grade.push_back(nValue);
		}
	}
	return 0;
}

//˫��Debug����ʹ��
static bool debugSendCards_Shuangsheng(BYTE* cbRandCard, Desk* m_desk, const char* conFileName, int playerCount, int handCardsCount, int cellPackCount)
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
		if(fp) fclose(fp);
		fp = NULL;
		return false;
	}

	//��������������� && card.ini����
	if (can_assign && fp)
	{
		LLOG_ERROR("Send Cards...This is debug mode!!! desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);

		//���ڴ洢���Ƶ��������
		BYTE* t_cbRandCard = new BYTE[cellPackCount];
		memset(t_cbRandCard, 0, sizeof(BYTE) * cellPackCount);

		//����card.ini�õ�������
		std::vector<BYTE> t_sendCards;

		//�����ƺ�ÿ�������ƵĺϷ�����
		size_t* t_sendCount = new size_t[playerCount];
		memset(t_sendCount, 0, sizeof(size_t) * playerCount);

		//��ÿ����ҽ�����Ч������
		for (int i = 0; i < playerCount; ++i)
		{
			t_sendCards.clear();
			//���������������
			LoadPlayerCards(t_sendCards, i + 1, conFileName);
			size_t t_count = 0;

			for (int j = 0; j < t_sendCards.size(); ++j)  //�����vector
			{
				for (int k = 0; k < cellPackCount; ++k)  //������
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
			for (int k = 0; k < cellPackCount; ++k)  //������
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

		memcpy(cbRandCard, t_cbRandCard, sizeof(BYTE) * cellPackCount);

		if (t_cbRandCard) delete(t_cbRandCard);
		t_cbRandCard = NULL;

		if (t_sendCount) delete(t_cbRandCard);
		t_sendCount = NULL;

		if(fp) fclose(fp);
		fp = NULL;
		return true;
	}

	if (fp) fclose(fp);
	fp = NULL;

	return false;
}

/* ��Ϸ�еĸ����׶Σ�ö�٣� */
static enum GAME_PLAY_STATE
{
	GAME_PLAY_BEGIN = -1,
	GAME_PLAY_SEND_CARDS = 1,	//���ƽ׶�
	GAME_PLAY_SELECT_ZHU = 2,	//����
	GAME_PLAY_FAN_ZHU = 3,		//����
	GAME_PLAY_BASE_CARD = 4,	//��׽׶�
	GAME_PLAY_PLAY_CARD = 5,	//���ƽ׶�
	GAME_PLAY_END = 6,			//���ƽ���
};

/* Ĭ�ϲ���ʱ�� */
static enum GAME_DEFAULT_TIME
{
	DEF_TIME_NULL = 0,
	DEF_TIME_SEND_CARDS = 10,
	DEF_TIME_SELECT_ZHU = 12,
	DEF_TIME_BASE_CARDS = 30,
	DEF_TIME_OUT_CARDS = 30,
};

/* ˫����ϷСѡ��(�ṹ��) */
struct PlayType
{
protected:
	std::vector<Lint>	m_playtype;  //��ϷСѡ��
									 //Index ------- 0: �淨ģʽ 1:2���� 

public:
	/* �ṹ����0���� */
	void clear()
	{
		m_playtype.clear();
	}
	/* �����淨Сѡ�� */
	void setAllTypes(const std::vector<Lint>& v)
	{
		m_playtype = v;
	}
	/* ��ȡ����Сѡ�� */
	const std::vector<Lint>& getAllTypes() const
	{
		return m_playtype;
	}

	/******************************************************
	* ��������   ShuangShengBaseScore()
	* ������     ��ȡ�ͻ���ѡ��ĵ׷�
	* ��ϸ��     �淨ģʽ��1��1�֣�2��2�֣�3��3��
	* ���أ�     (Lint)�ͻ���ѡ��ĵ׷�
	********************************************************/
	Lint ShuangShengBaseTimes() const;

	/******************************************************
	* ��������   ShuangShengRandZhu()
	* ������     ��ȡ�ͻ���ѡ���һ���������
	* ��ϸ��     �淨ģʽ��0-2��3��ʼ����1�������
	* ���أ�     (Lint)�ͻ���ѡ����Ƿ�Ϊ�����
	********************************************************/
	bool ShuangShengIsRandZhu() const;

	/******************************************************
	* ��������   ShuangShengAntiCheating()
	* ������     ��ȡ�ͻ���ѡ���Ƿ���������ѡ��
	* ��ϸ��     �淨ģʽ��0-��������1������������
	* ���أ�     (Lint)�ͻ���ѡ����淨ģʽ
	********************************************************/
	bool ShuangShengIsAntiCheating() const;

	/******************************************************
	* ��������   ShuangSheng2IsChangZhu()
	* ������     ��ȡ�ͻ���ѡ���2�Ƿ�Ϊ����
	* ��ϸ��     2�Ƿ�Ϊ������false-2���ǳ���  true-2�ǳ���
	* ���أ�     (bool)�ͻ���ѡ���2�Ƿ�Ϊ����
	********************************************************/
	bool ShuangSheng2IsChangZhu() const;



	/******************************************************
	* ��������   ShuangShengPlayMode()
	* ������     ��ȡ�ͻ���ѡ����淨ģʽ
	* ��ϸ��     �淨ģʽ��0-����ģʽ��1������ģʽ
	* ���أ�     (Lint)�ͻ���ѡ����淨ģʽ
	********************************************************/
	Lint ShuangShengPlayMode() const;

	/******************************************************
	* ��������   ShuangShengPackCount()
	* ������     ��ȡ�ͻ���ѡ���Ƶĸ���
	* ��ϸ��     ������1��1���ƣ�2��2����...
	* ���أ�     (int)�ͻ���ѡ���Ƹ���
	********************************************************/
	Lint ShuangShengPackCount() const;
};





/******************************************************
* ��������   ShuangShengBaseScore()
* ������     ��ȡ�ͻ���ѡ��ĵ׷�
* ��ϸ��     �淨ģʽ��1��1�֣�2��2�֣�3��3��
* ���أ�     (Lint)�ͻ���ѡ��ĵ׷�
********************************************************/
Lint PlayType::ShuangShengBaseTimes() const
{
	if (m_playtype.empty())
	{
		return 1;
	}
	if (m_playtype.size() > 0 && m_playtype[0] < 1 && m_playtype[0] > 3)
	{
		return 1;
	}
	return m_playtype[0];
}

/******************************************************
* ��������   ShuangShengRandZhu()
* ������     ��ȡ�ͻ���ѡ���һ���������
* ��ϸ��     �淨ģʽ��2-2��3��ʼ����1�������
* ���أ�     (Lint)�ͻ���ѡ����Ƿ�Ϊ�����
********************************************************/
bool PlayType::ShuangShengIsRandZhu() const
{
	if (m_playtype.size() < 2)
	{
		return 1;
	}
	return m_playtype[1] == 1;
}

/******************************************************
* ��������   ShuangShengAntiCheating()
* ������     ��ȡ�ͻ���ѡ���Ƿ���������ѡ��
* ��ϸ��     �淨ģʽ��0-��������1������������
* ���أ�     (Lint)�ͻ���ѡ����淨ģʽ
********************************************************/
bool PlayType::ShuangShengIsAntiCheating() const
{
	if (m_playtype.size() < 3)
	{
		return false;
	}
	return m_playtype[2] == 1;
}

/******************************************************
* ��������   ShuangSheng2IsChangZhu()
* ������     ��ȡ�ͻ���ѡ���2�Ƿ�Ϊ����
* ��ϸ��     2�Ƿ�Ϊ������false-2���ǳ���  true-2�ǳ���
* ���أ�     (bool)�ͻ���ѡ���2�Ƿ�Ϊ����
********************************************************/
bool PlayType::ShuangSheng2IsChangZhu() const
{
	if (m_playtype.size() < 4)
	{
		return false;
	}

	return m_playtype[3] == 1;
}






/******************************************************
* ��������   ShuangShengPlayMode()
* ������     ��ȡ�ͻ���ѡ����淨ģʽ
* ��ϸ��     �淨ģʽ��0-����ģʽ��1������ģʽ
* ���أ�     (Lint)�ͻ���ѡ����淨ģʽ
********************************************************/
Lint PlayType::ShuangShengPlayMode() const
{
	if (m_playtype.empty())
	{
		return 0;
	}
	return m_playtype[0];
}

/******************************************************
* ��������   ShuangShengPackCount()
* ������     ��ȡ�ͻ���ѡ���Ƶĸ���
* ��ϸ��     ������1��1���ƣ�2��2����...(��ʱĬ��������)
* ���أ�     (int)�ͻ���ѡ���Ƹ���
********************************************************/
Lint PlayType::ShuangShengPackCount() const
{
	return 2;
}

//////////////////////////////////////////�ƾֱ���////////////////////////////////////////////

/* ˫��ÿ�ֲ����ʼ���ֶΣ��ṹ�壩*/
struct ShuangShengRoundState__c_part
{
	Lint		m_curPos;						                   // ��ǰ�������
	Lint		m_firstOutPos;										//ÿ�ֵ�һ���������
	Lint		m_wTurnWinner;										//ÿ�ֳ��Ƶ�ʤ�����
	Lint		m_play_status;                                     // �ƾ�״̬
	Lint		m_user_status[SHUANGSHENG_PLAY_USER_COUNT];        // �û�״̬����λ�����Ƿ������ˣ�ʵ����Чλλ�ã�
	BOOL        m_tick_flag;                                       // ��ʱ������

	//Lint		m_curGrade;										   // ��ǰ����
	BYTE		m_mainColor;										//���ƻ�ɫ
	BYTE		m_mainValue;										//������ֵ
	BYTE		m_logicMainCard;									//�߼����ƣ������������Ϊ64

	bool		m_hasSelectZhu[4];									//���Ʊ�־
	BYTE		m_selectCard;										//��ҽе���
	BYTE		m_selectCardCount;									//��������
	Lint		m_selectCardPos;									//�������λ��

	BYTE		m_base_cards_count;									//��������
	BYTE		m_base_cards[SHUANGSHENG_BASE_CARDS_COUNT];			//����

	BYTE		m_hand_cards_count[SHUANGSHENG_PLAY_USER_COUNT];	//���������������
	BYTE		m_hand_cards[4][SHUANGSHENG_MAX_HAND_CARDS_COUNT];	//�������

	BYTE		m_out_cards_count[SHUANGSHENG_PLAY_USER_COUNT];		//��ҳ�����
	BYTE		m_out_cards[4][SHUANGSHENG_HAND_CARDS_COUNT];	//���ÿ�ֳ�����
	BYTE		m_cbLastTurnCard[4][SHUANGSHENG_HAND_CARDS_COUNT];		//�ϴγ���
	BYTE		m_cbLastTurnCount[4];								//�ϴγ�����Ŀ

	BYTE		m_cbScoreCardCount;									//�÷��˿���Ŀ
	BYTE		m_cbScoreCardData[12 * SHUANGSHENG_PACK_COUNT];	    //�÷��˿�
	Lint		m_player_score[SHUANGSHENG_PLAY_USER_COUNT];        //��ұ��ֵ÷�

	/* ����ÿ���ֶ� */
	void clear_round()
	{
		memset(this, 0, sizeof(*this));
		m_curPos = SHUANGSHENG_INVALID_POS;
		m_firstOutPos = SHUANGSHENG_INVALID_POS;
		m_wTurnWinner = SHUANGSHENG_INVALID_POS;
		m_selectCardPos = SHUANGSHENG_INVALID_POS;
		m_mainColor = 0x40;
		m_logicMainCard = 0x40;
	}
};

/* ˫��ÿ�ֻ��ʼ���ֶΣ��ṹ�壩*/
struct ShuangShengRoundState__cxx_part
{
	LTime m_play_status_time;

	/* ����ÿ���ֶ� */
	void clear_round()
	{
		this->~ShuangShengRoundState__cxx_part();
		new (this) ShuangShengRoundState__cxx_part;
	}
};

/* ˫��ÿ�������ʼ�����ֶΣ��ṹ�壩*/
struct ShuangShengMatchState__c_part
{
	Desk*		m_desk;           // ���Ӷ���
	Lint		m_round_offset;   // ��ǰ����
	Lint		m_round_limit;    // ��������ѡ���������
	Lint		m_accum_score[SHUANGSHENG_PLAY_USER_COUNT];  //�洢����ܵ÷�
	Lint		m_gradeCard[2];	 //���Ӽ������򼸣���Ĭ��2���ǳ�����
	BOOL		m_dismissed;
	int			m_registered_game_type;
	int			m_player_count;   // �淨���Ĵ�����ʹ�õ���������ֶ�
	Lint		m_zhuangPos;      // ׯ��λ��
	bool		m_isFirstFlag;    // �׾���ʾ��ʼ��ť,true�����׾�
	int			m_baseCardCount;  //��������

	////�淨Сѡ��
	int			m_baseTimes;		//�׷ֱ���
	bool		m_isRandZhu;		//�Ƿ������ true:�������false:��2��3��ʼ��
	bool		m_isAntiCheating;	//�Ƿ��������� true:����������  false:������������
	bool		m_is2ChangZhu;		//2�Ƿ�Ϊ������true��2�ǳ���  false��2���ǳ���

	int			m_playMode;			//�淨ģʽ��0�������淨  1������ģʽ
	int			m_packCount;		//�Ƹ���

	/* ����ṹ���ֶ� */
	void clear_match()
	{
		memset(this, 0, sizeof(*this));

		//Ĭ��2���ǳ�������2��ʼ��
		m_gradeCard[0] = 2;  
		m_gradeCard[1] = 2;

		m_baseTimes = 1;
		m_isRandZhu = 0;
		m_packCount = 2;
		m_zhuangPos = SHUANGSHENG_INVALID_POS;
	}
};

/* ˫��ÿ�����ʼ�����ֶΣ��ṹ�壩*/
struct ShuangShengMatchState__cxx_part
{
	SSGameLogic    m_gamelogic;     // ��Ϸ�߼�
	PlayType       m_playtype;	    // �����淨Сѡ��

									/* ���ÿ���ṹ�� */
	void clear_match()
	{
		this->~ShuangShengMatchState__cxx_part();
		new (this) ShuangShengMatchState__cxx_part;
	}
};


/*
*  ˫��ÿ������Ҫ�������ֶΣ��ṹ�壩
*  �̳� ��ShuangShengRoundState__c_pard, ShuangShengRoundState_cxx_part
*/
struct ShuangShengRoundState : ShuangShengRoundState__c_part, ShuangShengRoundState__cxx_part
{
	void clear_round()
	{
		ShuangShengRoundState__c_part::clear_round();
		ShuangShengRoundState__cxx_part::clear_round();
	}
};


/*
*  ˫��ÿ������Ҫ�������ֶΣ��ṹ�壩
*  �̳У�ShuangShengMatchState__c_pard, ShuangShengMatchState_cxx_pard
*/
struct ShuangShengMatchState : ShuangShengMatchState__c_part, ShuangShengMatchState__cxx_part
{
	void clear_match()
	{
		ShuangShengMatchState__c_part::clear_match();
		ShuangShengMatchState__cxx_part::clear_match();
	}
};

/*
*  ˫������״̬���ṹ�壩
*  �̳У�ShuangShengRoundState, NiuNiuMatchState
*/
struct ShuangShengDeskState : ShuangShengRoundState, ShuangShengMatchState
{

	/* ����ÿ������ */
	void clear_round()
	{
		ShuangShengRoundState::clear_round();
	}

	/* ����ÿ������*/
	void clear_match(int player_count)
	{
		ShuangShengMatchState::clear_match();
		this->clear_round();
		m_player_count = player_count;
	}

	/******************************************************************
	* ��������    setup_match()
	* ������      ���ñ����ľ������Լ��淨Сѡ��
	* ������
	*  @ l_playtype �淨Сѡ��
	*  @ round_limit ����������
	* ���أ�       (void)
	*******************************************************************/
	void setup_match(std::vector<Lint>& l_playtype, int round_limit)
	{
		//���ñ���������
		m_round_limit = round_limit;

		//�����淨Сѡ��
		m_playtype.setAllTypes(l_playtype);
	}

	/* ÿ�ֽ��������Ӿ��� */
	void increase_round()
	{
		//���Ӿ���
		++m_round_offset;
		//���µ�ǰ�������磺2/8�֣�
		if (m_desk && m_desk->m_vip)
		{
			++m_desk->m_vip->m_curCircle;
		}
	}

	/* ��������״̬ */
	void set_desk_state(DESK_STATE v)
	{
		if (m_desk)
		{
			if (v == DESK_WAIT)
			{
				m_desk->SetDeskWait();
			}
			else
			{
				m_desk->setDeskState(v);
			}
		}
	}

	/* ��ȡ����״̬ */
	DESK_STATE get_desk_state()
	{
		if (m_desk)
		{
			return (DESK_STATE)m_desk->getDeskState();
		}
		else
		{
			return DESK_PLAY;
		}
	}

	/* �����ƾ��е�״̬ */
	void set_play_status(Lint play_status)
	{
		if (play_status >= GAME_PLAY_SEND_CARDS && play_status <= GAME_PLAY_END)
		{ 
			m_play_status = play_status;
		}
	}

	/* ��ȡ������������ϵ�λ�� */
	Lint GetUserPos(User* pUser)
	{
		if (NULL == pUser)
		{
			return SHUANGSHENG_INVALID_POS;
		}
		Lint pos = SHUANGSHENG_INVALID_POS;
		if (m_desk)
		{
			pos = m_desk->GetUserPos(pUser);
		}
		if (pos >= SHUANGSHENG_PLAY_USER_COUNT)
		{
			pos = SHUANGSHENG_INVALID_POS;
		}
		return pos;
	}

	/* ��ȡ��һ����Чλ�� */
	Lint GetNextPos(Lint pos)
	{
		if (m_player_count == 0) return 0; // ����������	
		Lint nextPos = (pos + 1) % m_player_count;
		for (int i = 0; i < m_player_count; i++)
		{
			nextPos = (nextPos + 1) % m_player_count;
		}
		return nextPos;
	}

	/* �ж�λ���Ƿ�Ϊ��Чλ�� */
	bool is_pos_valid(Lint pos)
	{
		if (NULL == m_desk)
		{
			return false;
		}
		return 0 <= pos && pos < SHUANGSHENG_INVALID_POS;
	}

	//��ȡ�Գ��Ƶ�����
	Lint GetHasOutPlayerCount(Lint firstPos, Lint currPos)
	{
		if (!is_pos_valid(firstPos) || !is_pos_valid(currPos))
		{
			return 0;
		}
		Lint outPlayerCount = 1;
		for (int i = firstPos; i % SHUANGSHENG_INVALID_POS != currPos; i++)
		{
			++outPlayerCount;
		}
		return outPlayerCount;
	}
	
	/* ��ȡ�����Գ��Ƶ����� */
	Lint GetHasOutPlayerCount()
	{
		int t_outPlayerCount = 0;
		if (m_firstOutPos == SHUANGSHENG_INVALID_POS
			|| m_out_cards_count[m_firstOutPos] <= 0
			|| !m_gamelogic.IsValidCard(m_out_cards[m_firstOutPos][0]))
		{
			return 0;
		}

		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			if (m_out_cards_count[i] > 0 && m_gamelogic.IsValidCard(m_out_cards[i][0]))
			{
				++t_outPlayerCount;
			}
		}
		return t_outPlayerCount;
	}

};

/* ˫��¼����(�ṹ��) */
struct ShuangShengVideoSupport : ShuangShengDeskState 
{
	//������¼¼��Ľṹ��
	QiPaiVideoLog_ShengJi		m_video;

	void VideoSave()
	{
		if (NULL == m_desk) return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		m_video.m_playType = m_desk->getPlayType();
		//m_video.m_flag=m_registered_game_type;
		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.getInsertSql();
		gWork.SendMsgToDb(video);
	}

	/*************************************************************************************
	* ��������	saveUserOper()
	* ������	˫��������Ҳ�����ӵ�QiPaiVideoLog_ShengJi �ṹ����
	* ������
	*  @ pos ��ǰ�������λ��
	*  @ oper �������ͣ�1�õ��ƣ�2����ƣ�3��ͨ���ƣ�4˦��
	*  @ firstOutPos ÿ�ֵ�һ���������λ��
	*  @ operCardsCount �����Ƶ�����
	*  @ operCards �����Ƶļ���
	*  @ handCardsCount ���������
	*  @ handCards ���Ƶļ���
	*  @ totleScore �м���ץ��
	* ���أ�	(void)
	**************************************************************************************/
	void saveUserOper(Lint pos, Lint oper, Lint firstOutPos, Lint operCardsCount, BYTE* operCards,  BYTE handCardsCount, BYTE* handCards, Lint totleScore = 0)
	{
		LLOG_DEBUG("..................SaveUserOper  pos[%d], oper[%d]", pos, oper);

		std::vector<Lint> t_operCards;	//��������
		std::vector<Lint> t_handCards;	//����
		if (operCardsCount != 0)
		{
			if (operCards != NULL)
			{
				for (int i = 0; i < operCardsCount; i++)
				{
					LLOG_DEBUG("..................operCards[%2d]=[%2d]", i, operCards[i]);
					t_operCards.push_back((Lint)operCards[i]);
				}
			}
		}
		if (handCardsCount != 0)
		{
			if (handCards != NULL)
			{
				for (int i = 0; i < handCardsCount; i++)
				{
					LLOG_DEBUG("..................cbHandCardData[%2d]=[%2d]", i, handCards[i]);
					t_handCards.push_back((Lint)handCards[i]);
				}
			}

		}
		m_video.addOper(pos, oper, firstOutPos, totleScore, t_operCards, t_handCards);
	}
};


/* ˫����Ϸ���崦���߼����ṹ�壩*/
struct ShuangShengGameCore : GameHandler, ShuangShengVideoSupport
{
	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
	}

	virtual void notify_desk_playing_user(LMsg &msg) {
	}

	virtual void notify_desk_without_user(LMsg &msg, User* pUser) {
	}

	/* �㲥��Ϸ���������ϵ�������� */
	void notify_desk_match_state()
	{
		LMsgS2CVipInfo msg;
		msg.m_curCircle = m_round_offset + 1;
		msg.m_curMaxCircle = m_round_limit;
		notify_desk(msg);
	}

	/********************************************************************************************
	* ��������      start_game()
	* ������        ˫��ÿһ����Ϸ��ʼ�ľ������
	* ���أ�        (void)
	*********************************************************************************************/
	void start_game()
	{
		LLOG_DEBUG("desk_id=[%d], ShuangSheng_start_game....m_play_status:[%d]", m_desk ? m_desk->GetDeskId() : 0, m_play_status);

		//���ñ���������
		if (m_round_offset == 0)
		{
			m_mainValue = m_gamelogic.GetCardValue(m_gradeCard[0]);
		}
		else
		{
			m_mainValue = m_gamelogic.GetCardValue((m_zhuangPos == SHUANGSHENG_INVALID_POS) ? m_gradeCard[0] : m_gradeCard[m_zhuangPos % 2]);
		}
		
		//������Ϸ״̬Ϊ���ƽ׶�
		set_play_status(GAME_PLAY_SEND_CARDS);

		//ϴ�Ʒ���
		dispatch_user_cards();
	}

	/*********************************************************************************************
	* ��������     start_round()
	* ������       ˫����Ϸÿһ�ֵ��ܿ�ʼ����
	* ������
	*  @ *player_status ���״̬�б���ʾ��λ�����Ƿ�����ң�0����ʼ��1���þֲμӣ�2���¼��� OR �þֲ�������Ϸ(����)
	* ���أ�       (void)
	**********************************************************************************************/
	void start_round(Lint* player_status)
	{
		LLOG_DEBUG("desk_id=[%d], LogRen...SHUANGSHENG  START  GAME!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", m_desk ? m_desk->GetDeskId() : 0);

		LLOG_ERROR("ShuangSheng::start_round() Run... This Round Play Type deskId=[%d], baseTimes=[%d], isRandZhu=[%d], isAntiCheating=[%d], is2ChangZhu=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_baseTimes, m_isRandZhu, m_isAntiCheating, m_is2ChangZhu);

		clear_round();
		notify_desk_match_state();

		set_desk_state(DESK_PLAY);
		memcpy(m_user_status, player_status, sizeof(m_user_status));

		if (m_desk && m_desk->m_vip)
		{
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);
		}
		LLOG_ERROR("desk_id=[%d], start_round=[%d/%d], player_count=%d", m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

		//��Ϸ��ʼ���
		start_game();
	}

	/* ϴ�Ʒ��� */
	bool dispatch_user_cards()
	{

		//�������˿���
		BYTE cbRandCard[SHUANGSHENG_CELL_PACK * SHUANGSHENG_PACK_COUNT];

		//�����˿�
		m_gamelogic.RandCardList(cbRandCard, SHUANGSHENG_CELL_PACK * SHUANGSHENG_PACK_COUNT);

		//debugģʽ������
		debugSendCards_Shuangsheng(cbRandCard, m_desk, DEBUG_CARD_FILE_SHUANGSHENG, SHUANGSHENG_PLAY_USER_COUNT, SHUANGSHENG_HAND_CARDS_COUNT, SHUANGSHENG_CELL_PACK * SHUANGSHENG_PACK_COUNT);
		
		//��ÿ����ҷ���
		for (WORD i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; i++)
		{
			m_hand_cards_count[i] = SHUANGSHENG_HAND_CARDS_COUNT;
			memcpy(&m_hand_cards[i], &cbRandCard[i*m_hand_cards_count[i]], sizeof(BYTE)*m_hand_cards_count[i]);
		}

		//�����˿�
		m_baseCardCount = (SHUANGSHENG_CELL_PACK * m_packCount) - (m_player_count * m_gamelogic.GetDispatchCount());
		memcpy(m_base_cards, &cbRandCard[SHUANGSHENG_CELL_PACK * m_packCount - m_baseCardCount], sizeof(BYTE)*m_baseCardCount);
		

		//���û�����
		ShuangShengS2CSendCards sendCards[SHUANGSHENG_PLAY_USER_COUNT];
		for (WORD i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; i++)
		{
			sendCards[i].m_outTime = DEF_TIME_SELECT_ZHU;
			sendCards[i].m_pos = i;
			sendCards[i].m_zhuangPos = m_zhuangPos;
			sendCards[i].m_handCardsCount = m_hand_cards_count[i];
			for (int j = 0; j < SHUANGSHENG_HAND_CARDS_COUNT; ++j)
			{
				sendCards[i].m_handCards[j] = (int)m_hand_cards[i][j];
			}
			sendCards[i].m_currGrade = m_mainValue;
			sendCards[i].m_gradeCard[0] = m_gradeCard[0];
			sendCards[i].m_gradeCard[1] = m_gradeCard[1];

			memcpy(sendCards[i].m_totleScore, m_accum_score, sizeof(m_accum_score));

			//����ʱ�����������������ƣ�����ɫ�����޷�����
			m_gamelogic.SortCardList(m_hand_cards[i], m_hand_cards_count[i]);
		}
		notify_user(sendCards[0], 0);
		notify_user(sendCards[1], 1);
		notify_user(sendCards[2], 2);
		notify_user(sendCards[3], 3);

		//���0������־
		LLOG_DEBUG("notify_user0:Send Cards To player0 cardCount=[%d], deskId=[%d]", sendCards[0].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < 25; ++i)
		{
			LLOG_DEBUG("Send Cards Player0.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[0].m_handCards[i], sendCards[0].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//���1������־
		LLOG_DEBUG("notify_user1:Send Cards To player1 cardCount=[%d], deskId=[%d]", sendCards[1].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < 25; ++i)
		{
			LLOG_DEBUG("Send Cards Player1.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[1].m_handCards[i], sendCards[1].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//���2������־
		LLOG_DEBUG("notify_user2:Send Cards To player2 cardCount=[%d], deskId=[%d]", sendCards[2].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < 25; ++i)
		{
			LLOG_DEBUG("Send Cards Player2.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[2].m_handCards[i], sendCards[2].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//���3������־
		LLOG_DEBUG("notify_user3:Send Cards To player3 cardCount=[%d], deskId=[%d]", sendCards[3].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < 25; ++i)
		{
			LLOG_DEBUG("Send Cards Player3.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[3].m_handCards[i], sendCards[3].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//�������Զ�������Ϸ״̬Ϊ����״̬
		set_play_status(GAME_PLAY_SELECT_ZHU);

		LLOG_DEBUG("Send Cards Over ^_^... Start Send Cards Tick... deskId=[%d], playStatus=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_play_status);
		m_tick_flag = true;
		m_play_status_time.Now();

		return true;
	}

	/* ��ұ��� */
	bool on_event_user_select_zhu(int pos, BYTE selectCard, BYTE selectCardCount, int kCount)
	{
		//У�飺���������� <= 0 ,������Ҫ��
		if (selectCardCount <= 0)
		{
			LLOG_DEBUG("Select Zhu Error !!! select card count is null !!! desk_id=[%d], select_card_cound=[%d], kClickCount=[%d],",
				m_desk ? m_desk->GetDeskId() : 0, selectCardCount, kCount);
			return false;
		}

		//У�飺����������Ǽ��ƣ��������������ƣ���������
		if (!(m_gamelogic.GetCardValue(selectCard) == m_mainValue || ((selectCard == 0x4E || selectCard == 0x4F) && selectCardCount == 2)))
		{
			LLOG_DEBUG("Select Zhu Error !!! select card is not main card or not 2 same joker desk_id=[%d], select_card=[0x%x], select_card_cound=[%d], kClickCount=[%d], main_value=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, selectCard, selectCardCount, kCount, m_mainValue);
			return false;
		}

		//У�飺���������� > ����������������������Ҫ��
		BYTE tmpSelectCardCount = 0;
		for (int i = 0; i < m_hand_cards_count[pos]; ++i)
		{
			if (m_hand_cards[pos][i] == selectCard) ++tmpSelectCardCount;
		}
		if (selectCardCount > tmpSelectCardCount)
		{
			LLOG_DEBUG("Select Zhu Error !!! select card count moer than hand this card count !!!  desk_id=[%d], pos=[%d], select_card=[%d], select_card_count=[%d], hand_select_card_count=[%d], kClickCount=[%d], main_value=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, selectCard, selectCardCount, tmpSelectCardCount, kCount, m_mainValue);
			return false;
		}

		//��������״̬Ϊ�ѽй���״̬�����ٴν����������ϴβ�һ���������� <= �ϴν�������
		if (m_hasSelectZhu[pos] && selectCard != m_selectCard && selectCardCount <= m_selectCardCount)
		{
			LLOG_DEBUG("This Player Select Zhu Repeat Error!!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], kClickCount=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, kCount);
			return false;
		}
		
		//������Ҷ�������ֹ������ҷ�������ɫ��
		if (pos == m_selectCardPos)
		{
			if ((selectCard != m_selectCard) || (selectCardCount <= m_selectCardCount))
			{
				LLOG_DEBUG("This Player Select Zhu Faile < !!! fix main color failure !!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], main_value=[%d], kClickCount=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, m_mainValue, kCount);
				return true;
			}
		}
		//������ҵ�һ�ν��� �� ��ҷ���
		else
		{
			//������ұ��������� < �Ѿ����������� �Ǿ��Բ��ܱ�����
			//  selectCardCount = 0, m_selectCardCount = 1
			//  selectCardCount = 1, m_selectCardCount = 2 
			if (selectCardCount < m_selectCardCount || selectCardCount <= 0)
			{
				LLOG_DEBUG("This Player Select Zhu Faile < !!! this select zhu card count less than has select zhu card count!!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], main_value=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, m_mainValue);
				return true;
			}
			//��С��������������һ��
			if (selectCardCount < 2 && (selectCard == 0x4E || selectCard == 0x4F))
			{
				LLOG_DEBUG("This Player Select Zhu Faile !!! select joker count less than 2 !!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], main_value=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, m_mainValue);
				return true;
			}

			//  selectCardCount > 0
			//  selectCardCount = 1, m_selectCardCount = 1
			//  (selectCardCount = 2, m_selectCardCount = 2) && selectCard ������
			if (selectCardCount == m_selectCardCount && 
				(selectCardCount == 1 || (selectCardCount == 2 && m_gamelogic.GetCardValue(selectCard) < 0x0E)))
			{
				LLOG_DEBUG("This Player Select Zhu Faile = !!! this select zhu card count less than has select zhu card count!!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], main_value=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, m_mainValue);
				return true;
			}
			// selectCardCount = 1, m_selectCardCount = 0
			// selectCardCount = 2, m_selectCardCount = 1
			// selectCardCount = m_selectCardCount || selectCard ����
			if (!(selectCardCount > m_selectCardCount || m_gamelogic.GetCardValue(selectCard) > m_gamelogic.GetCardValue(m_selectCard)))
			{
				LLOG_DEBUG("This Player Select Zhu Faile > !!! this select zhu card less than has select zhu card!!!  desk_id=[%d], pos=[%d], has_select_card=[%d], select_card=[%d], has_select_count=[%d], select_count=[%d], main_value=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, m_selectCard, selectCard, m_selectCardCount, selectCardCount, m_mainValue);
				return true;
			}
			//if ((selectCardCount <= m_selectCardCount) && (selectCard != 0x4E || selectCard != 0x4F)) return true;
		}

		m_selectCard = selectCard;
		m_selectCardCount = selectCardCount;
		m_selectCardPos = pos;
		//�߼�ѡ�����ƣ������������Ϊ0x40
		m_logicMainCard = (m_selectCard == 0x4F || m_selectCard == 0x4E) ? 0x40 : m_selectCard;

		LLOG_DEBUG("This Player Select Zhu Success ^_^  desk_id=[%d], pos=[%d], select_card=[%d], select_card_count=[%d], main_value=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_selectCardPos, m_selectCard, m_selectCardCount, m_mainValue);

		if (GAME_PLAY_SEND_CARDS == m_play_status)
		{
			set_play_status(GAME_PLAY_SELECT_ZHU);
		}

		//��Ǳ��ν������
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			if (i == pos) m_hasSelectZhu[pos] = true;
			else m_hasSelectZhu[i] = false;
		}

		//�㲥��ҽ������
		ShuangShengS2CBCSelectZhu bcZhu;
		bcZhu.m_outTime = DEF_TIME_SELECT_ZHU;
		bcZhu.m_pos = pos;
		bcZhu.m_selectZhuCount = m_selectCardCount;
		bcZhu.m_selectZhu = m_selectCard;
		bcZhu.m_count = kCount;
		LLOG_DEBUG("notify_desk::Player Select Zhu deskId=[%d], selectPos=[%d], selectCard=[%d], selectCardCount=[%d], kCount=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, bcZhu.m_pos, bcZhu.m_selectZhu, bcZhu.m_selectZhuCount, bcZhu.m_count);
		notify_desk(bcZhu);

		//������������ֱ�ӽ�������
		/*if (m_selectCard == 0x4F && m_selectCardCount == 2 && m_play_status == GAME_PLAY_SELECT_ZHU)
		{
			select_zhu_over();
		}*/
		//�������������ȴ�ʱ��
		//else if(m_play_status == GAME_PLAY_SELECT_ZHU)
		//{
		//	//����������ʱ
		//	//m_tick_flag = true;
		//	//m_play_status_time.Now();
		//}
	}

	/* �������� */
	bool select_zhu_over()
	{
		//�������˱������
		if (m_selectCard == 0 || m_selectCardCount == 0 || m_selectCardPos == SHUANGSHENG_INVALID_POS)
		{
			//��������
			m_selectCard = 0x4F;

			//������������λ�ã���һ����Ҫ��λ��ȷ��ׯ��
			srand(time(NULL));
			m_selectCardPos = rand() % SHUANGSHENG_PLAY_USER_COUNT;

			LLOG_DEBUG("Select Zhu Over !!! But No Player Select, So This Round Is No Mian Color!!! desk_id=[%d], rand_pos=[%d], main_value=[%d],  main_color=[0x%x]"
				, m_desk ? m_desk->GetDeskId() : 0, m_selectCardPos, m_mainValue, m_gamelogic.GetCardColor(m_selectCard));
		}

		//�߼�ѡ�����ƣ������������Ϊ0x40
		m_logicMainCard = (m_selectCard == 0x4F || m_selectCard == 0x4E) ? 0x40 : m_selectCard;

		//��һ�ִ�ʱ����ȷ��ׯ��
		if (m_round_offset == 0) m_zhuangPos = m_selectCardPos;

		//��������ɫ
		m_mainColor = m_gamelogic.GetCardColor(m_selectCard);

		//�������ֵ�һ�����Ƶ�λ��
		m_firstOutPos = m_zhuangPos;

		//���õ�ǰ����λ��
		m_curPos = m_zhuangPos;

		//��������
		LLOG_DEBUG("Select Zhu Over... desk_id=[%d], zhuang_pos=[%d], main_value=[%d],  main_color=[%x]"
		, m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, m_mainValue, m_mainColor);

		//��ׯ�ҷ�������+����33���ƣ�ͬʱ֪ͨ�������ׯ�����������
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			ShuangShengS2CNoticeBaseInfo sendBase;
			sendBase.m_outTime = DEF_TIME_BASE_CARDS;
			sendBase.m_zhuangPos = m_zhuangPos;
			sendBase.m_selectCard = m_logicMainCard;
			if (i == m_zhuangPos)
			{
				for (int j = 0; j < SHUANGSHENG_BASE_CARDS_COUNT; ++j)
				{
					sendBase.m_baseCards[j] = (int)m_base_cards[j];
				}
				for (int k = 8; k < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++k)
				{
					sendBase.m_baseCards[k] = (int)m_hand_cards[m_zhuangPos][k - 8];
				}
			}
			LLOG_DEBUG("notify_user%d::Player Recive BaseInfo deskId=[%d], basePos=[%d], selectCard=[%d], baseCard=[%d]",
				i, m_desk ? m_desk->GetDeskId() : 0, sendBase.m_zhuangPos, sendBase.m_selectCard, sendBase.m_baseCards[0]);
			notify_user(sendBase, i);
		}

		//������Ϸ״̬Ϊ�ǵ��Ʋ���
		set_play_status(GAME_PLAY_BASE_CARD);

		//����¼������õ��Ʋ�����
		int t_pos = m_zhuangPos;
		int t_oper = 1;  //������1�õ���
		int t_firstOutPos = SHUANGSHENG_INVALID_POS;
		int t_totleScore = 0;
		saveUserOper(t_pos, t_oper, t_firstOutPos, m_baseCardCount, m_base_cards, m_hand_cards_count[t_pos], m_hand_cards[t_pos], t_totleScore);

		return true;
	}

	/* ��Ҹǵ��� */
	bool on_event_user_base_cards(int pos, ShuangShengC2SBaseCards* msg)
	{
		//У����Ҹǵ�������
		int t_baseCardsCount = msg->m_baseCardsCount;
		if (t_baseCardsCount != 8)
		{
			LLOG_DEBUG("Player Base Cards Count Error!!! desk_id=[%d], user_id=[%d] card_count=[%d] "
				, m_desk ? m_desk->GetDeskId() : 0, m_desk->m_user[pos]->GetUserDataId(), t_baseCardsCount);
			return false;
		}

		//��Ҹǵĵ���
		BYTE t_baseCards[SHUANGSHENG_BASE_CARDS_COUNT];
		memset(t_baseCards, 0, sizeof(t_baseCards));
		for (int i = 0; i < SHUANGSHENG_BASE_CARDS_COUNT; ++i)
		{
			t_baseCards[i] = (BYTE)msg->m_baseCards[i];
		}
		//���е��ƣ�ׯ��ѡ����׵���
		m_gamelogic.SortCardList(t_baseCards, SHUANGSHENG_BASE_CARDS_COUNT);

		//�����õ������ԭʼ���Ƽҵ�������33����
		BYTE t_orgHandCards[SHUANGSHENG_MAX_HAND_CARDS_COUNT];
		memset(t_orgHandCards, 0, sizeof(t_orgHandCards));
		for (int i = 0; i < SHUANGSHENG_BASE_CARDS_COUNT; ++i)
		{
			t_orgHandCards[i] = m_base_cards[i];
		}
		for (int i = SHUANGSHENG_BASE_CARDS_COUNT; i < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++i)
		{
			t_orgHandCards[i] = m_hand_cards[pos][i - 8];
		}
		//�������ƣ�ׯ���õ��ƺ��33������
		m_gamelogic.SortCardList(t_orgHandCards, SHUANGSHENG_MAX_HAND_CARDS_COUNT);

		//��Ҹǵ����Ƿ�ɹ���־
		bool t_flag = false;

		//ɾ�������ĵ��ƣ�����Ƴɹ�
		if (m_gamelogic.RemoveCard(t_baseCards, SHUANGSHENG_BASE_CARDS_COUNT, t_orgHandCards, SHUANGSHENG_MAX_HAND_CARDS_COUNT) == true)
		{
			ShuangShengS2CBCBaseCardOver baseOver;
			baseOver.m_pos = pos;

			t_flag = true;
			//������Ϸ״̬Ϊ����״̬
			set_play_status(GAME_PLAY_PLAY_CARD);
			
			//�����׵��Ʒŵ�������
			memset(m_base_cards, 0, sizeof(m_base_cards));
			memcpy(m_base_cards, t_baseCards, sizeof(m_base_cards));

			//������ʣ�µ��Ʒŵ����������
			m_hand_cards_count[pos] = SHUANGSHENG_HAND_CARDS_COUNT;
			memset(m_hand_cards[pos], 0, sizeof(m_hand_cards[pos]));
			for (int j = 0; j < SHUANGSHENG_HAND_CARDS_COUNT; ++j)
			{
				m_hand_cards[pos][j] = t_orgHandCards[j];
			}

			baseOver.m_selectCard = m_logicMainCard;
			/*if (m_gamelogic.GetCardColor(m_selectCard) == 0x40)
			{
				baseOver.m_selectCard = 0x40;
			}*/
			
			baseOver.m_handCardsCount = SHUANGSHENG_HAND_CARDS_COUNT;
			baseOver.m_flag = t_flag;
			for (int i = 0; i < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++i)
			{
				baseOver.m_handCards[i] = t_orgHandCards[i];	
			}
			LLOG_DEBUG("notify_user%d::Player Base Card Over deskId=[%d], basePos=[%d], selectCard=[%d], player%dHandCard=[%d], handCardCound=[%d], baseFlag=[%d]",
				baseOver.m_pos, m_desk ? m_desk->GetDeskId() : 0, baseOver.m_pos, baseOver.m_selectCard, baseOver.m_pos, baseOver.m_handCards[0], baseOver.m_handCardsCount, baseOver.m_flag);
			notify_user(baseOver, pos);
		}

		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			if (i == pos && t_flag) continue;

			//֪ͨ��Ҹǵ��ƽ��
			ShuangShengS2CBCBaseCardOver baseOver;
			baseOver.m_pos = pos;
			baseOver.m_flag = t_flag;
			LLOG_DEBUG("notify_user%d::Player Base Card Over deskId=[%d], basePos=[%d], player%dHandCard=[%d], handCardCound=[%d], baseFlag=[%d]",
				i, m_desk ? m_desk->GetDeskId() : 0, baseOver.m_pos, i, baseOver.m_handCards[0], baseOver.m_handCardsCount, baseOver.m_flag);
			notify_user(baseOver, i);
		}
		LLOG_DEBUG("Player base cards ... desk_id=[%d], user_id=[%d], is_base_cards_success=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_desk->m_user[pos]->GetUserDataId(), t_flag);

		//������ʧ��
		if (!t_flag) return false;
		
		//����¼���������Ʋ�����
		int t_pos = m_zhuangPos;
		int t_oper = 2;  //������2�����
		int t_firstOutPos = m_zhuangPos;
		int t_totleScore = 0;
		saveUserOper(t_pos, t_oper, t_firstOutPos, m_baseCardCount, m_base_cards, m_hand_cards_count[t_pos], m_hand_cards[t_pos], t_totleScore);

		//��ʼ����
		start_play_cards();
	}

	/* ��ʼ���� */
	bool start_play_cards()
	{
		LLOG_DEBUG("Start Play Cards... desk_id=[%d], start_out_pos=[%d], main_value=[%d],  main_color=[%d]"
			, m_desk ? m_desk->GetDeskId() : 0, m_firstOutPos, m_mainValue, m_mainColor);

		//����Logic������ɫ �� ����
		m_gamelogic.SetMainValue(m_gamelogic.GetCardValue(m_mainValue));
		m_gamelogic.SetMainColor(m_mainColor);

		//��������ÿ���������
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			//�������ƣ�ÿ����ҿ�ʼ����ǰ����25������
			m_gamelogic.SortCardList(m_hand_cards[i], SHUANGSHENG_HAND_CARDS_COUNT);
		}

		//�㲥��Ϸ��ʼ
		ShuangShengS2CBCGameStart start;
		start.m_pos = m_zhuangPos;
		notify_desk(start);

		return true;
	}

	/********************************************************
	* ��������	out_cards_sound()
	* ������	��ҳ�����ʾ�����ͣ� -1��Ĭ�ϲ�����  0���ܲ������ƣ�ž��  1����ͬ��ɫ���ϣ�����/���ϣ�  2����ɱ
	* ������
	*  @ pos ��ǰ�������
	*  @ currBig ��ǰ����������
	* ���أ�	(int)-1��Ĭ�ϲ�����  0���ܲ������ƣ�ž��  1����ͬ��ɫ���ϣ�����/���ϣ�  2����ɱ
	*********************************************************/
	int out_cards_sound(int pos, int currBig)
	{
		//��һ�����Ƶ��˲���ʾ
		if (m_firstOutPos == pos)
		{
			return -1;
		}
		//��ǰ���Ƶ��˲������ģ���ʾ��ž/���ơ�
		if (currBig != pos)
		{
			return 0;
		}
		int t_beforPos = (pos - 1 + SHUANGSHENG_PLAY_USER_COUNT) % SHUANGSHENG_PLAY_USER_COUNT;
		//�߼���ɫ
		BYTE t_outLogicColor = m_gamelogic.GetCardLogicColor(m_out_cards[pos], m_out_cards_count[pos]);
		BYTE t_beforeLogicColor = m_gamelogic.GetCardLogicColor(m_out_cards[t_beforPos], m_out_cards_count[t_beforPos]);
		BYTE t_firstLogicColor = m_gamelogic.GetCardLogicColor(m_out_cards[m_firstOutPos], m_out_cards_count[m_firstOutPos]);
		
		//��ǰ���Ƶ��ϼ����׼ҳ���λ��
		if (t_beforPos == m_firstOutPos)
		{
			//��ǰ����������׼ҳ����ƣ���ǰ�����ƣ���ʾ�����ˡ�
			if (t_firstLogicColor != COLOR_NT && t_outLogicColor == COLOR_NT)
			{
				return 2;
			}
			//��ǰ��������Һ��׼ҳ���ͬ��ɫ����ʾ������/���ϡ�
			else if (t_firstLogicColor == t_outLogicColor)
			{
				return 1;
			}
		}
		//��ǰ���Ƶ��ϼҲ����׼�
		else
		{
			//��ǰ����������׼ҳ����� && �ϼҳ����� && ��ǰ�����ƣ���ʾ�����ˡ�
			if (t_firstLogicColor != COLOR_NT && t_beforeLogicColor != COLOR_NT && t_outLogicColor == COLOR_NT)
			{
				return 2;
			}
			//��ǰ������� && �׼�ͬ��ɫ && �ϼ�ͬ��ɫ����ʾ������/���ϡ�
			if (t_firstLogicColor == t_outLogicColor && t_beforeLogicColor == t_outLogicColor)
			{
				return 1;
			}
			//��ǰ������� && �׼ҳ����� && �ϼұ��� && ��ǰ�ǱУ���ʾ������/���ϡ�
			if (t_firstLogicColor != COLOR_NT && t_beforeLogicColor == COLOR_NT && t_outLogicColor == COLOR_NT)
			{
				return 1;
			}
			//��ǰ������� && �׼ҳ����� && �ϼҲ������� && ��ǰ�����ƣ���ʾ������/���ϡ�
			if (t_firstLogicColor == COLOR_NT && t_beforeLogicColor != COLOR_NT && t_outLogicColor == COLOR_NT)
			{
				return 1;
			}
		}
		return 0;
	}

	/* ��ҳ��� */
	bool on_event_user_out_cards(int pos, BYTE cbCardData[], BYTE cbCardCount)
	{
		LLOG_DEBUG("Player Out Cards Run... desk_id=[%d], pos=[%d], out_cards_count=[%d], is_first_out=[%d]"
			, m_desk ? m_desk->GetDeskId() : 0, pos, cbCardCount, pos == m_firstOutPos);

		//��ӡ������ֵ
		LLOG_DEBUG("Player Out Cards={");
		for (int i = 0; i < cbCardCount; ++i)
		{
			LLOG_DEBUG("       outCard[%d]=[0x%2x|(%d)],", i, cbCardData[i], cbCardData[i]);
		}
		LLOG_DEBUG("                  }");
		LLOG_DEBUG("outCardsCount = [%d], pos = [%d], deskId = [%d]", cbCardCount, pos, m_desk ? m_desk->GetDeskId() : 0);

		//Ч��״̬
		//ASSERT(m_pITableFrame->GetGameStatus() == GS_UG_PLAY);
		//if (m_pITableFrame->GetGameStatus() != GS_UG_PLAY) return true;

		//Ч���û�
		//ASSERT((wChairID == m_wCurrentUser) && (cbCardCount <= MAX_COUNT));
		if ((pos != m_curPos) || (cbCardCount > SHUANGSHENG_HAND_CARDS_COUNT))
		{
			LLOG_DEBUG("Player Out Cards Error, Pos Or Cards Count error!!! desk_id=[%d], cur_pos=[%d], pos=[%d], out_cards_count=[%d]"
				, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, cbCardCount);
			return false;
		}

		//���г��ƣ�������ҳ�����
		m_gamelogic.SortCardList(cbCardData, cbCardCount);

		//���������ж�
		BYTE cbCardType = m_gamelogic.GetCardType(cbCardData, cbCardCount);

		//ÿ���׳������
		if (m_firstOutPos == pos)
		{
			LLOG_DEBUG("Player Out Cards..., The First Out Cards^_^, desk_id=[%d], cur_pos=[%d], pos=[%d], cardType=[%d]"
				, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, cbCardType);

			//�����ж�
			if (cbCardType == CT_ERROR)
			{
				LLOG_DEBUG("Player Out Cards Error..., The First Out Cards Type Error!!! desk_id=[%d], cur_pos=[%d], pos=[%d], cardType=[%d]"
					, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, cbCardType);
				return false;
			}

			//˦�ƴ���
			if (cbCardType == CT_THROW_CARD)
			{
				//˦�ƽ���ж�
				tagOutCardResult OutCardResult;
				ZeroMemory(&OutCardResult, sizeof(OutCardResult));

				LLOG_DEBUG("ShuangShengGameLogic::EfficacyThrowCard_Small() Run... ThrowPos%dOutCards, OutCardCount=[%d], handCardCount=[%d], mainColor=[0x%2x], deskId=[%d]",
					pos, cbCardCount, m_hand_cards_count[pos],m_mainColor, m_desk ? m_desk->GetDeskId() : 0);

				for (int j = 0; j < cbCardCount; ++j)
				{
					LLOG_DEBUG("ShuangShengGameLogic::EfficacyThrowCard_Small() ...............ThrowOutCard[%d]=[%d]", j, cbCardData[j]);
				}
				bool bThrowSuccess = m_gamelogic.EfficacyThrowCard_Small(cbCardData, cbCardCount, pos, m_hand_cards, m_hand_cards_count[pos], OutCardResult);

				//˦��ʧ�ܣ���С��
				if (bThrowSuccess == false)  
				{
					LLOG_DEBUG("Player Throw Cards Failed... Out Cards Result...  desk_id=[%d], cur_pos=[%d], pos=[%d]"
						, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos);

					//ɾ��������˦��ʧ�ܺ�Ӧ�ó�����
					if (m_gamelogic.RemoveCard(OutCardResult.cbResultCard, OutCardResult.cbCardCount, m_hand_cards[pos], m_hand_cards_count[pos]) == false)
					{
						LLOG_DEBUG("Player Throw Cards Failed...  Remove Out Cards Result Error!!! desk_id=[%d], cur_pos=[%d], pos=[%d]"
							, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos);
						return false;
					}
					m_hand_cards_count[pos] -= OutCardResult.cbCardCount;

					//���˦��ʧ�ܣ�ǿ�Ƴ�С�ƺ󣬴�����������
					//m_gamelogic.SortCardList(m_hand_cards[pos], m_hand_cards_count[pos]);

					//���Ƽ�¼
					m_out_cards_count[pos] = OutCardResult.cbCardCount; 
					memcpy(m_out_cards[pos], OutCardResult.cbResultCard, OutCardResult.cbCardCount * sizeof(BYTE));

					//�л��û�
					m_curPos = (pos + 1) % m_player_count;

					//62209���������㲥��һ���˳��ƽ��
					ShuangShengS2CBCOutCardsResult outCard;
					outCard.m_pos = pos;
					outCard.m_turnStart = 1;
					outCard.m_turnOver = 0;
					outCard.m_nextPos = m_curPos;
					outCard.m_mode = cbCardType;
					//���˦���ƣ�ʧ�ܵģ�
					outCard.m_outCardsCount = cbCardCount;
					for (int i = 0; i < cbCardCount; ++i)
					{
						outCard.m_outCards[i] = cbCardData[i];
					}
					//���˦��ʧ�ܺ�Ӧ�ó�����
					outCard.m_resultOutCardsCount = OutCardResult.cbCardCount;
					for (int i = 0; i < OutCardResult.cbCardCount; ++i)
					{
						outCard.m_resultOutCards[i] = OutCardResult.cbResultCard[i];
					}
					outCard.m_getTotleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
					outCard.m_scoreCardsCount = m_cbScoreCardCount;
					for (int i = 0; i < m_cbScoreCardCount; ++i)
					{
						outCard.m_scoreCards[i] = m_cbScoreCardData[i];
					}
					
					//�㲥������
					notify_desk(outCard);

					//����¼�����˦�Ʋ�����
					int t_pos = pos;
					int t_oper = 4;  //������1���õ��ƣ�2������ƣ�3����ͨ���ƣ�4��˦��
					int t_firstOutPos = m_firstOutPos;
					int t_totleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
					saveUserOper(t_pos, t_oper, t_firstOutPos, m_out_cards_count[t_pos], m_out_cards[t_pos], m_hand_cards_count[t_pos], m_hand_cards[t_pos], t_totleScore);

					return true;
				}
			}
		}
		//ÿ�ָ�������ң��ǵ�һ��������ң�
		else  
		{
			//У�������ҳ��������Ƿ�һ��
			if (cbCardCount != m_out_cards_count[m_firstOutPos])
			{
				LLOG_DEBUG("Player Follow Out Cards Failed...  Follow Out Cards Count Error!!!  desk_id=[%d], cur_pos=[%d], pos=[%d]��is_first_out=[%d]"
					, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, pos == m_firstOutPos);
				return false;
			}

			//����Ч��
			if (m_gamelogic.EfficacyOutCard(cbCardData, cbCardCount, m_out_cards[m_firstOutPos],
				m_out_cards_count[m_firstOutPos], m_hand_cards[pos], m_hand_cards_count[pos]) == false)
			{
				LLOG_DEBUG("Player Follow Out Cards Failed...  Follow Out Cards Type Error!!!  desk_id=[%d], cur_pos=[%d], pos=[%d]��is_first_out=[%d], mainValue=[%d], mainColor=[%d]"
					, m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, pos == m_firstOutPos, m_gamelogic.GetMainValue(), m_gamelogic.GetMainColor());
				return false;
			}
		}

		//ɾ�������г����ƣ�����һ������˦�Ƶ���ң�
		if (m_gamelogic.RemoveCard(cbCardData, cbCardCount, m_hand_cards[pos], m_hand_cards_count[pos]) == false)
		{
			LLOG_ERROR("Player Out Cards...  Remove Out Cards From HandCards Error!!! deskId=[%d], outCardsCount=[%d], curPos=[%d], pos=[%d], is_firstOut=[%d], mainValue=[%d], mainColor=[%d]"
				, m_desk ? m_desk->GetDeskId() : 0, cbCardCount, m_curPos, pos, pos == m_firstOutPos, m_gamelogic.GetMainValue(), m_gamelogic.GetMainColor());
			return false;
		}
		//���ƺ󣬴�����������
		//m_gamelogic.SortCardList(m_hand_cards[pos], m_hand_cards_count[pos] - cbCardCount);

		//���һ��˦�ƣ����������Ѿ������ж�4�ҵ��ƣ�������Ϸ����
		if (m_hand_cards_count[pos] == cbCardCount)
		{
			//���Ƽ�¼
			for (WORD i = 0; i < m_player_count; i++)
			{
				m_out_cards_count[i] = cbCardCount;
				if (i == pos)
				{ 
					memcpy(m_out_cards[i], cbCardData, cbCardCount * sizeof(BYTE));
				}
				else
				{ 
					memcpy(m_out_cards[i], m_hand_cards[i], cbCardCount * sizeof(BYTE));
					//���г��ƣ��������һ�ֳ�����
					m_gamelogic.SortCardList(m_out_cards[i], cbCardCount);
				}
				m_hand_cards_count[i] = 0;
				memset(m_hand_cards[i], 0, sizeof(m_hand_cards[i]));
			}

			//��һ����ʤ�ߣ�Ҳ���Ǳ��ֵ�һ���������
			Lint before_winner = m_wTurnWinner;

			//�ж����һ�ֳ��Ƶ�ʤ��,�ض��ж�4�ң�ûë��
			m_wTurnWinner = m_gamelogic.CompareCardArray(m_out_cards, m_out_cards_count[m_firstOutPos], m_firstOutPos);

			//����÷�
			int t_xianTureScore = 0;

			if ((m_wTurnWinner != m_zhuangPos) && ((m_wTurnWinner + 2) % m_player_count != m_zhuangPos))
			{
				BYTE tmp_turnScoreCardsCount = 0;
				BYTE tmp_turnScoreCards[24];
				memset(tmp_turnScoreCards, 0, sizeof(tmp_turnScoreCards));

				for (WORD i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; i++)
				{
					BYTE * tmp_pcbScoreCard = &tmp_turnScoreCards[tmp_turnScoreCardsCount];
					BYTE tmp_cbMaxCount = CountArray(tmp_turnScoreCards) - tmp_turnScoreCardsCount;
					tmp_turnScoreCardsCount += m_gamelogic.GetScoreCard(m_out_cards[i], m_out_cards_count[i], tmp_pcbScoreCard, tmp_cbMaxCount);

					BYTE * pcbScoreCard = &m_cbScoreCardData[m_cbScoreCardCount];
					BYTE cbMaxCount = CountArray(m_cbScoreCardData) - m_cbScoreCardCount;
					m_cbScoreCardCount += m_gamelogic.GetScoreCard(m_out_cards[i], m_out_cards_count[i], pcbScoreCard, cbMaxCount);
				}
				t_xianTureScore = m_gamelogic.GetCardScore(tmp_turnScoreCards, tmp_turnScoreCardsCount);
			}
			
			for(int i = before_winner; i < m_player_count + before_winner; ++i)
			{
				ShuangShengS2CBCOutCardsResult outCard;
				outCard.m_pos = i % m_player_count;
				outCard.m_mode = m_gamelogic.GetCardType(m_out_cards[i], m_out_cards_count[i]);
				outCard.m_currScore = t_xianTureScore;
				outCard.m_getTotleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
				//outCard.m_nextPos = (i + 1) % m_player_count;
				outCard.m_nextPos = SHUANGSHENG_INVALID_POS;
				outCard.m_outCardsCount = m_out_cards_count[outCard.m_pos];
				for (int j = 0; j < m_out_cards_count[outCard.m_pos]; ++j)
				{
					outCard.m_outCards[j] = (Lint)m_out_cards[outCard.m_pos][j];
				}
				outCard.m_scoreCardsCount = m_cbScoreCardCount;
				for (int i = 0; i < m_cbScoreCardCount; ++i)
				{
					outCard.m_scoreCards[i] = m_cbScoreCardData[i];
				}

				//��һ������
				if (i == before_winner)
				{
					outCard.m_turnStart = 1;
					outCard.m_nextPos = (i + 1) % m_player_count;
				}
				//���һ���������
				else if(i == (m_player_count + before_winner -1))
				{ 
					outCard.m_turnOver = 1;
					outCard.m_turnWiner = m_wTurnWinner;
					outCard.m_currBig = m_wTurnWinner;
					//outCard.m_nextPos = m_wTurnWinner;
				}
				//�����ͻ���������ʾ����
				//outCard.m_soundType = out_cards_sound(pos, outCard.m_currBig);
				notify_desk(outCard);

				//����¼��������һ�ֳ��ƣ�
				int t_pos = i;
				int t_oper = 3;  //������1���õ��ƣ�2������ƣ�3����ͨ���ƣ�4��˦��
				int t_firstOutPos = m_firstOutPos;
				int t_totleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
				saveUserOper(t_pos, t_oper, t_firstOutPos, cbCardCount, m_out_cards[t_pos], m_hand_cards_count[t_pos], m_hand_cards[t_pos], t_totleScore);

				//������Ϸ״̬Ϊ���ƽ���
				set_play_status(GAME_PLAY_END);
			}

			//һ����Ϸ��������
			finish_round();
		}
		//�������һ�ֳ���
		else  
		{
			//�������� - ���Ƶ�����
			m_hand_cards_count[pos] -= cbCardCount;

			//��¼���˳����� �� ��������
			m_out_cards_count[pos] = cbCardCount;
			memcpy(m_out_cards[pos], cbCardData, cbCardCount * sizeof(BYTE));

			//�л���ǰλ��Ϊ��һ���������λ��
			m_curPos = (pos + 1) % m_player_count;

			//�����һ���������λ�� �� ��һ���������λ����ͬ�����ֳ��ƽ���
			if (m_curPos == m_firstOutPos) m_curPos = SHUANGSHENG_INVALID_POS;

			//62209���������㲥��ҳ��ƽ��
			ShuangShengS2CBCOutCardsResult outCard;
			outCard.m_pos = pos;
			outCard.m_nextPos = m_curPos;
			outCard.m_mode = cbCardType;

			//������
			outCard.m_outCardsCount = cbCardCount;
			for (int i = 0; i < cbCardCount; ++i)
			{
				outCard.m_outCards[i] = cbCardData[i];
			}
			
			//���ֵ�һ���������
			if (pos == m_firstOutPos)
			{
				outCard.m_turnStart = 1;
				outCard.m_turnOver = 0;
			}
			else
			{
				//�ж��ѳ����������ƣ��߷�֧�����ж�
				outCard.m_currBig = (int)m_gamelogic.CompareCardArray_n(m_out_cards, m_out_cards_count[m_firstOutPos], m_firstOutPos, GetHasOutPlayerCount());
				
				//�����ͻ���������ʾ����
				outCard.m_soundType = out_cards_sound(pos, outCard.m_currBig);
			}

			//����¼���¼
			BYTE t_outCard[SHUANGSHENG_HAND_CARDS_COUNT];
			memset(t_outCard, 0, sizeof(t_outCard));
			memcpy(t_outCard, m_out_cards[pos], sizeof(t_outCard));
			Lint t_outCardCount = m_out_cards_count[m_firstOutPos];

			//һ�ֳ��ƽ���
			if (m_curPos == SHUANGSHENG_INVALID_POS)
			{
				//һ�ֳ��ƽ������ж�ʤ�ߣ��ض���4�ң�ûë��
				m_wTurnWinner = m_gamelogic.CompareCardArray(m_out_cards, m_out_cards_count[pos], m_firstOutPos);

				//���ֳ��� �м�һ��Ӯ��ץ�ֳɹ�
				int t_xianTureScore = 0;

				if ((m_wTurnWinner != m_zhuangPos) && ((m_wTurnWinner + 2) % m_player_count != m_zhuangPos))
				{
					BYTE tmp_turnScoreCardsCount = 0;
					BYTE tmp_turnScoreCards[24];
					memset(tmp_turnScoreCards, 0, sizeof(tmp_turnScoreCards));
					
					for (WORD i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; i++)
					{
						BYTE * tmp_pcbScoreCard = &tmp_turnScoreCards[tmp_turnScoreCardsCount];
						BYTE tmp_cbMaxCount = CountArray(tmp_turnScoreCards) - tmp_turnScoreCardsCount;
						tmp_turnScoreCardsCount += m_gamelogic.GetScoreCard(m_out_cards[i], m_out_cards_count[i], tmp_pcbScoreCard, tmp_cbMaxCount);

						BYTE * pcbScoreCard = &m_cbScoreCardData[m_cbScoreCardCount];
						BYTE cbMaxCount = CountArray(m_cbScoreCardData) - m_cbScoreCardCount;
						m_cbScoreCardCount += m_gamelogic.GetScoreCard(m_out_cards[i], m_out_cards_count[i], pcbScoreCard, cbMaxCount);
					}
					t_xianTureScore = m_gamelogic.GetCardScore(tmp_turnScoreCards, tmp_turnScoreCardsCount);
				}

				//��¼�ϴγ���,�Լ���������
				memcpy(m_cbLastTurnCard, m_out_cards, sizeof(m_out_cards));
				memcpy(m_cbLastTurnCount, m_out_cards_count, sizeof(m_out_cards_count));

				//�����������ֳ��Ƶ�һ�����Ƶ���ң���ձ��ֳ�����
				if (m_hand_cards[m_wTurnWinner] != 0)
				{
					m_firstOutPos = m_wTurnWinner;
					memset(m_out_cards, 0, sizeof(m_out_cards));
					memset(m_out_cards_count, 0, sizeof(m_out_cards_count));
				}
				
				//�û��л�
				m_curPos = (m_hand_cards_count[m_wTurnWinner] == 0) ? SHUANGSHENG_INVALID_POS : m_wTurnWinner;
				outCard.m_turnStart = 0;
				outCard.m_turnOver = 1;
				outCard.m_nextPos = m_curPos;
				outCard.m_turnWiner = m_wTurnWinner;
				outCard.m_currScore = t_xianTureScore;
			}
			outCard.m_getTotleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
			outCard.m_scoreCardsCount = m_cbScoreCardCount;
			for (int i = 0; i < m_cbScoreCardCount; ++i)
			{
				outCard.m_scoreCards[i] = m_cbScoreCardData[i];
			}
			//�㲥������Ϣ
			notify_desk(outCard);

			//����¼����ҳ��Ʋ�����
			int t_pos = pos;
			int t_oper = 3;  //������1���õ��ƣ�2������ƣ�3����ͨ���ƣ�4��˦��
			int t_firstOutPos = m_firstOutPos;
			int t_totleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
			saveUserOper(t_pos, t_oper, t_firstOutPos, t_outCardCount, t_outCard, m_hand_cards_count[t_pos], m_hand_cards[t_pos], t_totleScore);

		}
		return true;
	}

	/* ��¼�ܷ���д�����ݿ� */
	void add_round_log(Lint* accum_score, Lint win_pos)
	{
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, accum_score, m_user_status, win_pos, m_video.m_Id);
		}
	}

	/* С���� */
	void finish_round(BOOL jiesan = false)
	{
		//��ҽ�ɢ���䣬����С������Ϣ
		if (true == jiesan)
		{
			LLOG_ERROR("ShuangShengGameHandler::finish_round() Run... Start Jie San ... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());
			ShuangShengS2CBCDrawResult jiesan;
			jiesan.m_isFinish = 1;

			memcpy(jiesan.m_totleScore, m_accum_score, sizeof(m_accum_score));
			for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT && m_desk; ++i)
			{
				if (m_desk->m_user[i])
				{
					jiesan.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
					jiesan.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
					jiesan.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
				}
			}

			notify_desk(jiesan);

			LLOG_ERROR("Game Jie San Over... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());

			set_desk_state(DESK_WAIT);

			//������Ϸ
			if (m_desk) m_desk->HanderGameOver(1);
			return;
		}

		//���Ӿ���
		increase_round();

		//��ұ��ֽ������
		Lint t_roundScore[SHUANGSHENG_PLAY_USER_COUNT];
		memset(t_roundScore, 0 , sizeof(t_roundScore));
		
		//���汾��ׯ��λ��
		Lint t_zhuangPos = m_zhuangPos;

		//���汾���м�λ��
		Lint t_xianPos = (m_zhuangPos + 1) % SHUANGSHENG_PLAY_USER_COUNT;

		//ׯ�����������м������� 0��ׯ������  1���м�����  2���м���ׯ������������
		Lint t_whoZhuangNext = 2;

		//������������
		Lint t_shengJiShu = 0;

		//�м��ܵ÷֣���������мҿ۵ַ�����
		Lint t_getTotleScore = 0;
		t_getTotleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);

		//�м�ʤ���׷ֱ�������Ϊ2������ж��ӻ���������֮����ټӱ�
		Lint t_baseTimes = 0;

		//�м�ʤ���۵ַ���
		Lint t_baseSecor = 0;

		//���һ�γ����мҴ󣬼��м�Ӯ��
		if (m_wTurnWinner % 2 != t_zhuangPos % 2 && m_wTurnWinner >= 0 && m_wTurnWinner < SHUANGSHENG_INVALID_POS)
		{
			//�м�ʤ���׷ֱ�������Ϊ2������ж��ӻ���������֮����ټӱ�
			t_baseTimes = 2;

			//�����еķ���
			BYTE tmp_base_cards_score[SHUANGSHENG_BASE_CARDS_COUNT];
			memset(tmp_base_cards_score, 0, sizeof(tmp_base_cards_score));

			//�����з��Ƶ�����
			Lint tmp_base_cards_score_count = m_gamelogic.GetScoreCard(m_base_cards, SHUANGSHENG_BASE_CARDS_COUNT, tmp_base_cards_score, SHUANGSHENG_BASE_CARDS_COUNT);
			
			//�������ܷ֣�ʵ�ʷ��������ӱ���
			Lint t_getBaseScore = m_gamelogic.GetCardScore(tmp_base_cards_score, tmp_base_cards_score_count);

			//���һ�γ��ƶ�������
			Lint cbMaxCount = 0;

			//���һ�ֳ��Ʒ������ͣ������ƣ�
			tagAnalyseResult AnalyseResult;
			m_gamelogic.AnalyseCardData(m_out_cards[m_wTurnWinner], m_out_cards_count[m_wTurnWinner], AnalyseResult);

			//���ӷ���           CountArray(AnalyseResult.SameDataInfo) = 2
			for (BYTE i = 0; i < CountArray(AnalyseResult.SameDataInfo); i++)
			{
				//if (AnalyseResult.SameDataInfo[i].cbBlockCount > 0) cbMaxCount = __max(i, cbMaxCount);
				if (AnalyseResult.SameDataInfo[i].cbBlockCount > 0 && i > 0) cbMaxCount = 2;
			}

			//���Ʒ���
			for (BYTE i = 0; i < CountArray(AnalyseResult.TractorDataInfo); i++)
			{
				//��ȡ����
				BYTE cbSameCount = i + 2;
				BYTE cbMaxLength = AnalyseResult.TractorDataInfo[i].cbTractorMaxLength;

				//���ñ���
				cbMaxCount = __max(cbSameCount * cbMaxLength, cbMaxCount);
			}

			t_baseTimes = cbMaxCount + t_baseTimes;

			t_baseSecor = t_getBaseScore * t_baseTimes;

			//�м��ܵ÷�
			t_getTotleScore = t_getTotleScore + t_baseSecor;

			LLOG_DEBUG("ShuangShengGameHandler::finish_round()...  cbMaxCount=[%d], BaseTimes=[%d]", cbMaxCount,t_baseTimes);
		}
		

		//�м�ץ�� < 80�֣�ׯ������
		if (t_getTotleScore < 80)
		{
			//�¾�ׯ�ҶԼ���ׯ
			m_zhuangPos = (m_zhuangPos + 2) % m_player_count;

			//ׯ������
			t_whoZhuangNext = 0;

			//����ׯ��Ӯ��
			Lint t_roundCommonScore = 1;

			//ׯ��������
			Lint t_zhuangShengJi = 0;

			if (t_getTotleScore == 0)
			{
				t_zhuangShengJi = 3;
			}
			else
			{
				t_zhuangShengJi = 2 - (t_getTotleScore / 40);
			}

			//ׯ�������󳬹�K
			if (m_gradeCard[t_zhuangPos % 2] + t_zhuangShengJi > 13)
			{
				if (m_gradeCard[t_zhuangPos % 2] + t_zhuangShengJi == 14)
				{
					m_gradeCard[t_zhuangPos % 2] = 1;
				}
				else if(m_gradeCard[t_zhuangPos % 2] + t_zhuangShengJi > 14)
				{ 
					m_gradeCard[t_zhuangPos % 2] = (m_gradeCard[t_zhuangPos % 2] + t_zhuangShengJi - 14) + (m_is2ChangZhu ? 3 : 2) - 1;
				}
			}
			//ׯ�������󲻳���K
			else
			{
				m_gradeCard[t_zhuangPos % 2] += (m_gradeCard[t_zhuangPos % 2] == 1 && m_is2ChangZhu) ? t_zhuangShengJi + 1 : t_zhuangShengJi;
			}

			//���������ҵ÷֣�ׯ��3���п�8�֣�ׯ��2���п�4�֣�ׯ��1���п�2��
			for (int i = 0; i < t_zhuangShengJi; ++i)
			{
				t_roundCommonScore *= 2;
			}
			if (t_roundCommonScore >= 16)
			{
				t_roundCommonScore = 16;
			}
			t_roundCommonScore = t_roundCommonScore * m_baseTimes;
			for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
			{
				if (i % 2 == t_zhuangPos % 2) t_roundScore[i] += t_roundCommonScore;
				else t_roundScore[i] -= t_roundCommonScore;
			}
			t_shengJiShu = t_zhuangShengJi;

		}
		
		//�м�ץ�� >= 80�֣�ׯ���ֻ�������
		else if (t_getTotleScore >= 80)
		{
			//��ׯ����һ��ׯ�ҵ��¼���ׯ
			m_zhuangPos = (m_zhuangPos + 1) % m_player_count;

			if (t_getTotleScore < 120)
			{
				t_whoZhuangNext = 2;
			}
			else if (t_getTotleScore >= 120)
			{
				t_whoZhuangNext = 1;
			}

			//�����м�Ӯ��
			Lint t_roundCommonScore = 1;

			//�м�������
			Lint t_xianShengJi = (t_getTotleScore / 40) - 2;

			//�м������󳬹�K
			if (m_gradeCard[t_xianPos % 2] + t_xianShengJi > 13)
			{
				if (m_gradeCard[t_xianPos % 2] + t_xianShengJi == 14)
				{
					m_gradeCard[t_xianPos % 2] = 1;
				}
				else if (m_gradeCard[t_xianPos % 2] + t_xianShengJi > 14)
				{
					m_gradeCard[t_xianPos % 2] = (m_gradeCard[t_xianPos % 2] + t_xianShengJi - 14) + (m_is2ChangZhu ? 3 : 2) - 1;
				}
			}
			//�м������󲻳���K
			else
			{
				m_gradeCard[t_xianPos % 2] += (m_gradeCard[t_xianPos % 2] == 1 && m_is2ChangZhu) ? t_xianShengJi + 1 : t_xianShengJi;
			}

			//������ҵ÷�
			for (int i = 0; i <= t_xianShengJi; ++i)
			{
				t_roundCommonScore *= 2;
			}
			if (t_roundCommonScore >= 16)
			{
				t_roundCommonScore = 16;
			}
			t_roundCommonScore = t_roundCommonScore * m_baseTimes;
			for (int i = 0; i < m_player_count; ++i)
			{
				if (i % 2 == t_xianPos % 2) t_roundScore[i] += t_roundCommonScore;
				else t_roundScore[i] -= t_roundCommonScore;
			}

			t_shengJiShu = t_xianShengJi;
		}

		//������ܵ÷�
		for (int i = 0; i < m_player_count; ++i)
		{
			m_accum_score[i] += t_roundScore[i];
		}

		////��ҽ�ɢ���䣬����С������Ϣ
		//if (true == jiesan)
		//{
		//	LLOG_ERROR("ShuangShengGameHandler::finish_round() Run... Start Jie San ... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());
		//	ShuangShengS2CBCDrawResult jiesan;
		//	jiesan.m_isFinish = 1;

		//	memcpy(jiesan.m_totleScore, m_accum_score, sizeof(m_accum_score));
		//	for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT && m_desk; ++i)
		//	{
		//		if (m_desk->m_user[i])
		//		{
		//			jiesan.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
		//			jiesan.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
		//			jiesan.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
		//		}
		//	}

		//	notify_desk(jiesan);

		//	LLOG_ERROR("Game Jie San Over... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());

		//	set_desk_state(DESK_WAIT);

		//	//������Ϸ
		//	if (m_desk) m_desk->HanderGameOver(1);
		//	return;
		//}

		//��¼�ܷ֡�д�����ݿ���Ϣ
		add_round_log(t_roundScore, m_wTurnWinner);

		//��С������Ϣ
		ShuangShengS2CBCDrawResult result;
		result.m_winnerPos = m_wTurnWinner;
		result.m_zhuangPos = t_zhuangPos;  //�������λ��
		result.m_baseTimes = t_baseTimes;
		result.m_baseScore = t_baseSecor;
		result.m_nextZhuangPos = m_zhuangPos;  //��һ�����λ��
		for(int i = 0; i < SHUANGSHENG_BASE_CARDS_COUNT; ++i)
		{
			result.m_baseCards[i] = (Lint)m_base_cards[i];
		}
		result.m_getTotleScore = t_getTotleScore;
		memcpy(result.m_thisScore, t_roundScore, sizeof(t_roundScore));
		memcpy(result.m_totleScore, m_accum_score, sizeof(m_accum_score));
		memcpy(result.m_upDate, m_gradeCard, sizeof(m_gradeCard));
		
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT && m_desk; ++i)
		{
			if(m_desk->m_user[i])
			{ 
				result.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
				result.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
				result.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
			}
		}

		result.m_type = t_whoZhuangNext;
		result.m_nextCard = t_shengJiShu;
		result.m_isFinish = (m_round_offset >= m_round_limit) ? 1 : 0;

		set_desk_state(DESK_WAIT);
		notify_desk(result);

		m_desk->HanderGameOver(1);
	}

};

/* ˫����Ϸ������ */
struct ShuangShengGameHandler : ShuangShengGameCore
{

	/* ���캯�� */
	ShuangShengGameHandler()
	{
		LLOG_DEBUG("ShuangSheng GameHandler Init Over...");
	}

	/* ����ҷ���Ϣ */
	void notify_user(LMsg &msg, int pos) override {
		if (NULL == m_desk) return;
		if (pos < 0 || pos >= m_player_count) return;
		User *u = m_desk->m_user[pos];
		if (NULL == u) return;
		u->Send(msg);
	}

	/* �㲥��Ϣ */
	void notify_desk(LMsg &msg) override {
		if (NULL == m_desk) return;
		m_desk->BoadCast(msg);
	}

	/* ÿ�ֿ�ʼ*/
	bool startup(Desk *desk) {
		if (NULL == desk) return false;
		clear_match(desk->m_desk_user_count);
		m_desk = desk;
		return true;
	}
	/* ÿ�ֽ���*/
	void shutdown(void) {
		clear_match(0);
	}


	/* �����淨Сѡ���Desk::SetVip()���� */
	void SetPlayType(std::vector<Lint>& l_playtype)
	{
		// �� Desk �� SetVip ���ã���ʱ�ܵõ� m_vip->m_maxCircle
		int round_limit = m_desk && m_desk->m_vip ? m_desk->m_vip->m_maxCircle : 0;
		setup_match(l_playtype, round_limit);

		//Сѡ��׷�
		m_baseTimes = m_playtype.ShuangShengBaseTimes();

		//Сѡ��Ƿ������
		m_isRandZhu = m_playtype.ShuangShengIsRandZhu();

		//Сѡ��Ƿ������
		m_isAntiCheating = m_playtype.ShuangShengIsAntiCheating();

		//Сѡ�2�Ƿ�Ϊ����
		m_is2ChangZhu = m_playtype.ShuangSheng2IsChangZhu();

		//��һ����Ҫ����һЩ����
		if (m_round_offset == 0)
		{
			//�����˿˸���
			m_gamelogic.SetPackCount(m_packCount);

			srand(time(NULL));

			//2������
			if (m_is2ChangZhu)
			{
				m_gamelogic.SetChangZhu(0x02);
				m_gradeCard[0] = 3;
				m_gradeCard[1] = 3;
			}
			
			//���������
			if (m_isRandZhu)
			{
				Lint t_randZhu = rand() % 11 + 3;
				m_gradeCard[0] = t_randZhu;
				m_gradeCard[1] = t_randZhu;
			}

			//�����ļ����Ƿ������������
			bool can_assign = gConfig.GetDebugModel();

			FILE *fp = NULL;
			try
			{
				fp = fopen(DEBUG_CARD_FILE_SHUANGSHENG, "r");
			}
			catch (const std::exception& e)
			{
				LLOG_ERROR("File card.ini is open failed...  desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);
			}

			//��������������� && card.ini����
			if (can_assign && fp)
			{
				LLOG_DEBUG("Grade Cards...This is debug mode!!! desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);
				//����
				std::vector<Lint> grade;
				grade.resize(2);
				LoadGradeCard(grade, DEBUG_CARD_FILE_SHUANGSHENG);

				if(grade.size() > 1 && grade[0] >= (m_is2ChangZhu ? 3 : 2) && grade[0] <= 13)
				{
					m_gradeCard[0] = grade[0];
				}
				if (grade.size() > 2 && grade[1] >= (m_is2ChangZhu ? 3 : 2) && grade[1] <= 13)
				{
					m_gradeCard[1] = grade[1];
				}
				m_gamelogic.SetMainValue(m_gradeCard[0]);

				fclose(fp);
				fp = NULL;
			}
		}
		


		//��ʱ����..............................................................................
		//Сѡ�ׯ�ҹ���
		m_playMode = m_playtype.ShuangShengPlayMode();

		//Сѡ��Ƹ�����Ĭ��Ϊ2��
		m_packCount = m_playtype.ShuangShengPackCount();

		LLOG_ERROR("ShuangSheng::SetPlayType() Run... This Match Play Type deskId=[%d], baseTimes=[%d], isRandZhu=[%d], isAntiCheating=[%d], is2ChangZhu=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_baseTimes, m_isRandZhu, m_isAntiCheating, m_is2ChangZhu);
	}

	/***********************************************************************************************************
	* ��������    MHSetDeskPlay()
	* ������      ��Ϸ��ʼ����Desk::MHHanderStartGame()����
	* ������
	*  @ play_user_count ���淨���޶�������������
	*  @ player_status ���״̬�б���ʾ��λ�����Ƿ�����ң�0����ʼ��1���þֲμӣ�2���¼��� OR �þֲ�������Ϸ(����)
	* ���أ�      (void)
	************************************************************************************************************/
	void MHSetDeskPlay(Lint play_user_count, Lint* player_status, Lint player_count)
	{
		LLOG_DEBUG("desk_id=[%d], ShuangSheng---------------------MHSetDeskPlay(Lint play_user_count) = [%d]", m_desk ? m_desk->GetDeskId() : 0, play_user_count);
		if (!m_desk || !m_desk->m_vip)
		{
			LLOG_ERROR("MHSetDeskPlay  error !!!! m_desk or  m_desk->m_vip  null");
			return;
		}

		m_player_count = play_user_count;

		//��һ����Ҫ����һЩ�����������ף�������������Ϊ
		if (m_round_offset == 0 && m_isAntiCheating)
		{
			srand(time(NULL));
			ShuangShengS2CFirstRound firstRound;
			Lint t_newPosID[4] = { 0, 1, 2, 3 };
			int t_newPos1 = SHUANGSHENG_INVALID_POS;
			int t_newPos2 = SHUANGSHENG_INVALID_POS;
			t_newPos1 = rand() % 4;
			do
			{
				t_newPos2 = rand() % 4;
			} while (t_newPos1 == t_newPos2);

			User* p_user = NULL;

			p_user = m_desk->m_user[t_newPos1];
			m_desk->m_user[t_newPos1] = m_desk->m_user[(t_newPos1 + 1) % 4];
			m_desk->m_user[(t_newPos1 + 1) % 4] = p_user;

			for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
			{
				if (m_desk->m_user[i] == NULL) continue;
				t_newPosID[i] = m_desk->m_user[i]->GetUserDataId();
			}
			memcpy(firstRound.m_newSeatUserId, t_newPosID, sizeof(t_newPosID));
			notify_desk(firstRound);
		}

		start_round(player_status);
	}

	/********************************************************************************
	* ��������      HandleShuangShengSelectZhu()
	* ������		����˫����ұ���
	* ������
	*  @pUser* ��Ҷ���
	*  @msg* ��ұ�����Ϣ
	* ���أ�        (bool)
	*********************************************************************************/
	bool HandleShuangShengSelectZhu(User* pUser, ShuangShengC2SSelectZhu* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			return false;
		}
		if (!(GAME_PLAY_SELECT_ZHU == m_play_status || GAME_PLAY_SEND_CARDS == m_play_status))
		{
			LLOG_ERROR("HandleShuangShengSelectZhu() Error... Current Play Status is not GAME_PLAY_SELECT_ZHU or GAME_PLAY_SEND_CARDS... deskId=[%d], userId=[%d], currStatus=[%d]", 
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == SHUANGSHENG_INVALID_POS)
		{
			LLOG_ERROR("HandleShuangShengSelectZhu() Error... Select Zhu Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ", m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		BYTE selectCard = msg->m_selectCard;
		BYTE selectCardCount = msg->m_selectCardCount;

		return on_event_user_select_zhu(GetUserPos(pUser), selectCard, selectCardCount, msg->m_count);
	}

	/* ������Ҹǵ��� */
	bool HandleShuangShengBaseCards(User* pUser, ShuangShengC2SBaseCards* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			return false;
		}
		if (GAME_PLAY_BASE_CARD != m_play_status)
		{
			LLOG_ERROR("desk_id=[%d], HandleShuangShengBaseCards error, user[%d] , status not GAME_PLAY_BASE_CARD, [%d]", m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == SHUANGSHENG_INVALID_POS)
		{
			LLOG_ERROR("desk_id=[%d], HandleShuangShengBaseCards  user[%d] pos not ok [%d] ", m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		int t_baseCardsCount = msg->m_baseCardsCount;
		int t_baseCards[SHUANGSHENG_BASE_CARDS_COUNT];
		
		if (t_baseCardsCount != 8)
		{
			LLOG_ERROR("desk_id=[%d], HandleShuangShengBaseCards  user[%d] pos not ok [%d] ", m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		
		return on_event_user_base_cards(GetUserPos(pUser), msg);
	}

	/* ��ҳ��� */
	bool HandleShuangShengOutCards(User* pUser, ShuangShengC2SOutCards* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			return false;
		}

		if (GAME_PLAY_PLAY_CARD != m_play_status)
		{
			LLOG_ERROR("HandleShuangShengOutCards() Error..., Current Status is not GAME_PLAY_PLAY_CARD..., deskId=[%d], userId=[%d], currStatus=[%d]", 
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == SHUANGSHENG_INVALID_POS)
		{
			LLOG_ERROR("HandleShuangShengBaseCards() Error...,  user[%d] pos not ok [%d] ", m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		BYTE cbCardCount = (BYTE)msg->m_outCardsCount;
		BYTE cbCardData[SHUANGSHENG_HAND_CARDS_COUNT];
		memset(cbCardData, 0, sizeof(cbCardData));
		for (int i = 0; i < cbCardCount; ++i)
		{
			cbCardData[i] = (BYTE)msg->m_outCards[i];
		}

		return on_event_user_out_cards(GetUserPos(pUser), cbCardData, cbCardCount);
	}

	/* ������Ҷ������� */
	void OnUserReconnect(User* pUser)
	{
		if (NULL == pUser || NULL == m_desk) 
		{
			LLOG_ERROR("****OnUserReconnect...desk_id=[%d], ShuangSheng : NULL user or desk poniter user:%X, m_desk:%X", m_desk, pUser, m_desk);
			return;
		}

		//Desk �Ѿ�����
		notify_desk_match_state();

		Lint pos = GetUserPos(pUser);

		if (SHUANGSHENG_INVALID_POS == pos)
		{
			LLOG_ERROR("****OnUserReconnect...desk_id=[%d], ShuangSheng user[id:%d] pos error!!!", m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId());
			return;
		}

		LLOG_DEBUG("Handler::OnUserReconnect() Run... deskId=[%d], userId=[%d], pos=[%d], curPos=[%d], zhuangPos=[%d], playStatus=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), pos, m_curPos, m_zhuangPos, m_play_status);

		ShuangShengS2CRecon reconn;
		reconn.m_status = m_play_status;
		reconn.m_zhuangPos = m_zhuangPos;
		reconn.m_currGrade = m_mainValue;
		reconn.m_mainColor = m_mainColor;
		reconn.m_selectCardValue = m_selectCard;
		reconn.m_selectCard = m_logicMainCard;
		reconn.m_selectCardCount = m_selectCardCount;
		reconn.m_selectCardPos = m_selectCardPos;
		reconn.m_gradeCard[0] = m_gradeCard[0];
		reconn.m_gradeCard[1] = m_gradeCard[1];
		reconn.m_outCardPos = m_curPos;

		reconn.m_handCardsCount = m_hand_cards_count[pos];
		for (int i = 0; i < SHUANGSHENG_MAX_HAND_CARDS_COUNT; ++i)
		{
			reconn.m_handCards[i] = m_hand_cards[pos][i];
		}
		
		for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT; ++i)
		{
			reconn.m_outCardsCount[i] = m_out_cards_count[i];
			for (int j = 0; j < SHUANGSHENG_HAND_CARDS_COUNT; ++j)
			{
				reconn.m_outCards[i][j] = m_out_cards[i][j];
			}
		}
		reconn.m_getTotleScore = m_gamelogic.GetCardScore(m_cbScoreCardData, m_cbScoreCardCount);
		reconn.m_scoreCardsCount = m_cbScoreCardCount;
		for (int i = 0; i < m_cbScoreCardCount; ++i)
		{
			reconn.m_scoreCards[i] = m_cbScoreCardData[i];
		}

		//���������׶�
		switch (m_play_status)
		{

		//�����׶ζ���
		case GAME_PLAY_SELECT_ZHU:
		{

			break;
		}

		//�ǵ��ƽ׶ζ���
		case GAME_PLAY_BASE_CARD:
		{
			if (m_zhuangPos == pos)
			{
				for (int i = 0; i < SHUANGSHENG_BASE_CARDS_COUNT; ++i)
				{
					reconn.m_handCards[m_hand_cards_count[pos] + i] = m_base_cards[i];
					reconn.m_baseCards[i] = m_base_cards[i];
				}
				reconn.m_handCardsCount = SHUANGSHENG_MAX_HAND_CARDS_COUNT;
				LLOG_DEBUG("Handler::OnUserReconnect()... Zhuang reconnect on BaseCards status... deskId=[%d], handCardCount=[%d], playStatus=[%d]", 
					m_desk ? m_desk->GetDeskId() : 0, reconn.m_handCardsCount, m_play_status);
			}
			break;
		}

		//���ƽ׶ζ�������
		case GAME_PLAY_PLAY_CARD:
		{
			reconn.m_firstOutPos = m_firstOutPos;
			reconn.m_currBig = (int)m_gamelogic.CompareCardArray_n(m_out_cards, m_out_cards_count[m_firstOutPos], m_firstOutPos, GetHasOutPlayerCount());
			break;
		}

		//�����˳��ƽ���
		case GAME_PLAY_END:
		{
			for (int i = 0; i < SHUANGSHENG_BASE_CARDS_COUNT; ++i)
			{
				reconn.m_baseCards[i] = m_base_cards[i];
			}
			break;
		}
		}
		notify_user(reconn, pos);
	}


	/* ������Ϸ����(��ɢ) */
	void OnGameOver(Lint result, Lint winpos, Lint bombpos)
	{
		if (m_desk == NULL || m_desk->m_vip == NULL) {
			LLOG_ERROR("OnGameOver NULL ERROR ");
			return;
		}

		m_dismissed = !!m_desk->m_vip->m_reset;
		MHLOG_PLAYCARD("ShuangShengGameHandler::OnGameOver() Run...Desk game over. deskId=[%d] round_limit=[%d] round=[%d] dismiss=[%d]", 
			m_desk ? m_desk->GetDeskId() : 0, m_round_limit, m_round_offset, m_dismissed);
		
		//��ɢ���䣬��Ϸ����
		finish_round(true);
	}


	/* ��ʱ�� */
	void Tick(LTime& curr)
	{
		//���ƶ�ʱ������ʱ5s�����Զ�ת�������׶Σ���������
		/*if (m_tick_flag && m_play_status == GAME_PLAY_SEND_CARDS && curr.Secs() >= m_play_status_time.Secs() + DEF_TIME_SEND_CARDS)
		{
			LLOG_DEBUG("Tick: Client Send Cards Time Over, Start Select Main Color Tick... desk_id=[%d], zhuang_pos=[%d], main_value=[%d],  main_color=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, m_mainValue, m_mainColor);

			�Զ�������Ϸ״̬Ϊ����״̬
			set_play_status(GAME_PLAY_SELECT_ZHU);
			m_play_status_time.Now();
		}*/

		//������ʱ��
		if (m_tick_flag && (m_play_status == GAME_PLAY_SELECT_ZHU || m_play_status == GAME_PLAY_SEND_CARDS) && curr.Secs() >= m_play_status_time.Secs() + DEF_TIME_SELECT_ZHU + (m_isAntiCheating ? 5 : 0))
		{
			//�رն�ʱ������
			m_tick_flag = false;

			//������Ϸ״̬Ϊ�ǵ��Ʋ���
			//set_play_status(GAME_PLAY_BASE_CARD);

			//������������
			select_zhu_over();
		}

		/*if (m_tick_flag && m_play_status == GAME_PLAY_BASE_CARD && curr.Secs() >= m_play_status_time.Secs() + DEF_TIME_BASE_CARDS)
		{
			
		}*/
	}

};


DECLARE_GAME_HANDLER_CREATOR(106, ShuangShengGameHandler);