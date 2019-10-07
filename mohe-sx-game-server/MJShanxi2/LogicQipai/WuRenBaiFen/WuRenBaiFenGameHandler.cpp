#include "WuRenBaiFenGameLogic.h"
#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "LMsgL2C.h"
#include "LMsgPoker.h"



/*
*  ���˰ٷ�Debug����ʹ��
*  cbRandCard �����������
*  m_desk Desk����
*  conFileName �����ļ�
*  playerCount �������
*  handCardsCount �����������
*  cellPackCount ������
*/
static bool debugSendCards_WuRenBaiFen(Lint* cbRandCard, Desk* m_desk, const char* conFileName, int playerCount, int handCardsCount, int cellPackCount)
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
		return false;
	}

	//��������������� && card.ini����
	if (can_assign && fp)
	{
		LLOG_ERROR("Send Cards...This is debug mode!!! desk_id=[%d]", m_desk ? m_desk->GetDeskId() : 0);

		//���ڴ洢���Ƶ��������
		Lint* t_cbRandCard = new Lint[cellPackCount];
		memset(t_cbRandCard, 0, sizeof(Lint) * cellPackCount);

		//����card.ini�õ�������
		std::vector<BYTE> t_sendCards;

		//�����ƺ�ÿ�������ƵĺϷ�����
		size_t* t_sendCount = new size_t[playerCount];
		memset(t_sendCount, 0, sizeof(size_t) * playerCount);
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
			for (int j = i * handCardsCount + t_sendCount[i]; j < (i + 1) * handCardsCount; )
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

		//����õ���д�����������
		memcpy(cbRandCard, t_cbRandCard, sizeof(Lint) * cellPackCount);

		if (t_cbRandCard) delete(t_cbRandCard);
		t_cbRandCard = NULL;

		if (t_sendCount) delete(t_sendCount);
		t_sendCount = NULL;

		if (fp) fclose(fp);
		fp = NULL;
		return true;
	}

	if (fp) fclose(fp);
	fp = NULL;

	return false;
}

/* �������Ϸ�еĸ����׶Σ�ö�٣� */
enum GAME_PLAY_STATE
{
	GAME_PLAY_BEGIN = 0,			//��Ϸ��ʼ
	GAME_PLAY_SEND_CARD = 1,		//���ƽ׶�
	GAME_PLAY_SELECT_SCORE = 2,		//���Ʒֽ׶�
	GAME_PLAY_SELECT_MAIN = 3,		//ׯ��ѡ���׶�
	GAME_PLAY_BASE_CARDS = 4,		//ׯ����׽׶�
	GAME_PLAY_SELECT_FRIEND = 5,	//ׯ��ѡ�Լҽ׶�
	GAME_PLAY_OUT_CARD = 6,			//���ƽ׶�
	GAME_PLAY_SETTLEMENT = 7,		//����׶�
	GAME_PLAY_END = 8,				//��Ϸ����
	GAME_PLAY_ZHUAGN_JIAOPAI = 9 // ׯ�ҽ���
};

enum JIAO_PAI_STATE
{
	JIAOPAI_STATE_INIT = 0, //Ĭ��ֵ
	JIAOPAI_STATE_REQUESTED = 1, // ׯ��������	 
	JIAOPAI_STATE_COMPLETE = 2, // �����,������ͬ��
	JIAOPAI_STATE_RQUEST_TIMEOUT = 3, // ���볬ʱ
	JIAOPAI_STATE_XIAN_NOT_AGREE = 4, // �м��Ѿܾ�
	JIAOPAI_STATE_XIAN_TIMEOUT = 5, // �м��ѳ�ʱ
	JIAOPAI_STATE_END = 6 // ����״̬
};

/* �������Ϸ�е���Ч */
enum GAME_PLAY_SOUND
{
	GAME_PLAY_SOUND_PA = 0,				//ž
	GAME_PLAY_SOUND_DIAOZHU = 1,		//����
	GAME_PLAY_SOUND_GUANSHANG = 2,		//����
	GAME_PLAY_SOUND_DANI = 3,			//����
	GAME_PLAY_SOUND_BIPAI = 4,			//����
	GAME_PLAY_SOUND_GAIBI = 5,			//�Ǳ�
	GAME_PLAY_SOUND_PO = 6,				//��
	GAME_PLAY_SOUND_XIANJIADEIFEN = 7,	//�мҵ÷�
	GAME_PLAY_SOUND_DIANPAI = 8,		//����
};

enum OVER_STATE
{
	GAME_OVER_A = 0,		//a ��ǰ��������ǰ����
	GAME_OVER_B = 1,		//b ��ɱ�����ׯ�ұ��ף�ׯ�ұ���
	GAME_OVER_C = 2,		//c ��ɱ�����ׯ�ұ��ף��м�һ��δ�ã��м�δ��֣�ׯ�ҷ���
	GAME_OVER_D = 3,		//d ��ɱ�����ׯ�ұ��ף��м�һ��δ�ã�ׯ�ҽз�100���м�δ��֣�ׯ�з�100
	GAME_OVER_E = 4,		//e ��ɱ������мҿ۵ף��мҿ۵ף��мҷ���
	GAME_OVER_F = 5,		//f ��ɱ������мҿ۵ף��мҿ۵ף�ׯ�ҽз�100���мҿ۵ף�ׯ�з�100
	GAME_OVER_G = 6     //g ��ɱ�����ׯ�ҽ���
};

enum WURENBAIFEN_OVER_STATE
{
	DIPAI_NONE_WIN = 0, 
	DIPAI_ZHUANG_JIAOPAI = 1,                               //ׯ�ҽ���
	DIPAI_ZHUANG_WIN = 2,                                     //ׯ�ұ���
	DIPAI_ZHUANG_WIN_XIAN_SCORE0 =3,           //ׯ�ұ��� ��ͷ
	DIPAI_XIAN_WIN = 4                                             //�мҿٵ�
};

/* �����Ĭ�ϲ���ʱ�� */
enum GAME_DEFAULT_TIME
{
	JIAOPAI_REQUEST_TIMEOUT =  15,
	JIAOPAI_RESPONSE_TIMEOUT  = 15
};


/* �������ϷСѡ��(�ṹ��) */
struct PlayType
{
protected:
	std::vector<Lint>	m_playtype;  //��ϷСѡ��

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
	std::vector<Lint> GetType()
	{
		return m_playtype;
	}

	/******************************************************
	* ��������   WuRenBaiFenBaseTimes()
	* ������     ��ȡ�ͻ���ѡ��ĵ׷�
	* ��ϸ��     �׷ֱ�����1��1�֣�2��2�֣�3��3��
	* ���أ�     (Lint)�ͻ���ѡ��ĵ׷�
	********************************************************/
	Lint WuRenBaiFenBaseTimes() const;

	/******************************************************
	* ��������   WuRenBaiFen2IsCommMain()
	* ������     ��ȡ�ͻ���ѡ���2�Ƿ�Ϊ����
	* ��ϸ��     2�Ƿ�Ϊ������false-2���ǳ���  true-2�ǳ���
	* ���أ�     (bool)�ͻ���ѡ���2�Ƿ�Ϊ����
	********************************************************/
	bool WuRenBaiFen2IsCommMain() const;

	/******************************************************
	* ��������   WuRenBaiFen2IsCommMain()
	* ������     ��ȡ�ͻ���ѡ�������λ�ý�ֹ����
	* ��ϸ��     2�Ƿ�Ϊ������false- ��ֹ����  true-����ֹ����
	* ���أ�     (bool)�ͻ���ѡ�������λ�ý�ֹ����
	********************************************************/
	/*bool PlayType::WuRenBaiFen2IsGPS() const;*/


	/******************************************************
	* ��������   WuRenBaiFen2IsCommMain()
	* ������     ��ȡ�ͻ���ѡ��ĶԼ�Ҫ�ƺ�ʮ
	* ��ϸ��     2�Ƿ�Ϊ������false- ��ֹ����  true-����ֹ����
	* ���أ�     (bool)�ͻ���ѡ�������λ�ý�ֹ����
	********************************************************/
	bool  WuRenBaiFen2IsPokerTen() const;


	/******************************************************
	* ��������   WuRenBaiFen2IsCheat()
	* ������     ר��������
	* ��ϸ��     �Ƿ�ʮ��false-����  true-��
	* ���أ�     (bool)�ͻ���ѡ�������λ�ý�ֹ����
	********************************************************/
	bool  WuRenBaiFen2IsCheat() const;

	/******************************************************
	* ��������   WuRenBaiFenIsGanKouDi()
	* ������     �мҸɿٵ�
	* ��ϸ��     �Ƿ�ʮ��false-���ɿٵ�  true-�ɿٵ�
	* ���أ�     (bool)�����мҸɿٵ�
	********************************************************/
	bool  WuRenBaiFenIsGanKouDi() const;

	/******************************************************
	* ��������   WuRenBaiFenIsJiaoPai()
	* ������     �Ƿ�����ׯ�ҽ���
	* ��ϸ��     �Ƿ�ʮ��false-������  true-����
	* ���أ�     (bool)�Ƿ�����ׯ�ҽ���
	********************************************************/
	Lint  WuRenBaiFenGetJiaoPaiOption() const;

	bool WuRenBaiFenGetAnyScoreCanSelectMainColor() const;
};



/******************************************************
* ��������   WuRenBaiFenBaseTimes()
* ������     ��ȡ�ͻ���ѡ��ĵ׷�
* ��ϸ��     �׷ֱ�����1��1�֣�2��2�֣�3��3��
* ���أ�     (Lint)�ͻ���ѡ��ĵ׷�
********************************************************/
Lint PlayType::WuRenBaiFenBaseTimes() const
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
* ��������   WuRenBaiFen2IsCommMain()
* ������     ��ȡ�ͻ���ѡ���2�Ƿ�Ϊ����
* ��ϸ��     2�Ƿ�Ϊ������false-2���ǳ���  true-2�ǳ���
* ���أ�     (bool)�ͻ���ѡ���2�Ƿ�Ϊ����
********************************************************/
bool PlayType::WuRenBaiFen2IsCommMain() const
{
	if (m_playtype.size() < 2)
	{
		return false;
	}

	return m_playtype[1] == 1;
}

/******************************************************
* ��������   WuRenBaiFen2IsCommMain()
* ������     ��ȡ�ͻ���ѡ��ĶԼ�Ҫ�ƺ�ʮ
* ��ϸ��     �Ƿ�ʮ��false-����  true-��ʮ
* ���أ�     (bool)�ͻ���ѡ�������λ�ý�ֹ����
********************************************************/
//bool PlayType::WuRenBaiFen2IsGPS() const
//{
//	if (m_playtype.size() < 3)
//	{
//		return false;
//	}
//
//	return m_playtype[2] == 1;
//}



/******************************************************
* ��������   WuRenBaiFen2IsCommMain()
* ������     ��ȡ�ͻ���ѡ��ĶԼ�Ҫ�ƺ�ʮ
* ��ϸ��     �Ƿ�ʮ��false-����  true-��ʮ
* ���أ�     (bool)�ͻ���ѡ�������λ�ý�ֹ����
********************************************************/
bool PlayType::WuRenBaiFen2IsPokerTen() const
{
	if (m_playtype.size() < 4)
	{
		return false;
	}

	return m_playtype[3] == 1;
}

/******************************************************
* ��������   WuRenBaiFen2IsCheat()
* ������     ר��������
* ��ϸ��     �Ƿ�ʮ��false-����  true-��
* ���أ�     (bool)�ͻ���ѡ�������λ�ý�ֹ����
********************************************************/
bool PlayType::WuRenBaiFen2IsCheat() const
{
	if (m_playtype.size() < 5)
	{
		return false;
	}

	return m_playtype[4] == 1;
}

/******************************************************
* ��������   WuRenBaiFenIsGanKouDi()
* ������     �мҸɿٵ�
* ��ϸ��     �Ƿ�ʮ��false-���ɿٵ�  true-�ɿٵ�
* ���أ�     (bool)�����мҸɿٵ�
********************************************************/
bool PlayType::WuRenBaiFenIsGanKouDi() const
{
	if (m_playtype.size() < 6)
	{
		return false;
	}

	return m_playtype[5] == 1;
}

/******************************************************
* ��������   WuRenBaiFenIsJiaoPai()
* ������     �Ƿ�����ׯ�ҽ���
* ��ϸ��      
* ���أ�     (int)�Ƿ�����ׯ�ҽ��� ѡ��
********************************************************/
Lint PlayType::WuRenBaiFenGetJiaoPaiOption() const
{
	if (m_playtype.size() < 7)
	{
		return 0;
	}

	return m_playtype[6] % 3;
}

bool PlayType::WuRenBaiFenGetAnyScoreCanSelectMainColor() const
{
	if (m_playtype.size() < 8)
	{
		return false;
	}

	return m_playtype[7] == 0 ? false : true;
}


/* �����ÿ�ֲ����ʼ���ֶΣ��ṹ�壩*/
struct WuRenBaiFenRoundState__c_part
{
	Lint				m_curPos;											// ��ǰ�������
	Lint				m_firstOutPos;										//ÿ�ֵ�һ���������
	Lint				m_wTurnWinner;										//ÿ�ֳ��Ƶ�ʤ�����
	Lint				m_friendZhuangPos;									// ��ׯ��λ��
	Lint				m_selectFriendCard;									//���ѡ�Լҵ���
	Lint				m_thisTurnScore;									//�����мҵ÷�
	Lint				nOverState;											//���ֽ���ׯ���м���ʾ�����ׯ�ұ��ף��мҿٵ�...
	WURENBAIFEN_OVER_STATE             m_wurenbaifen_overstate;               // �µĽ���״̬

	GAME_PLAY_STATE		m_play_status;										// �ƾ�״̬
	OVER_STATE			m_over_state;										//С��״̬

	Lint				m_user_status[WURENBAIFEN_PLAYER_COUNT];				// �û�״̬����λ�����Ƿ������ˣ�ʵ����Чλλ�ã�
	BOOL				m_tick_flag;										// ��ʱ������

	Lint				m_mainColor;										//���ƻ�ɫ

	Lint				m_max_select_score;									//ѡ�����Ľ��Ʒ���
	Lint				m_select_score[WURENBAIFEN_PLAYER_COUNT];				//��ҽ��Ʒ���

	Lint				m_base_cards_count;									//��������
	Lint				m_base_cards[WURENBAIFEN_BASE_CARDS_COUNT];				//����

	Lint				m_hand_cards_count[WURENBAIFEN_PLAYER_COUNT];			//���������������
	Lint				m_hand_cards[WURENBAIFEN_PLAYER_COUNT][WURENBAIFEN_HAND_CARDS_COUNT_MAX];		//�������

	Lint				m_out_cards[WURENBAIFEN_PLAYER_COUNT];					//���ÿ�ֳ�����
	Lint				m_prev_out_cards[WURENBAIFEN_PLAYER_COUNT];				//���ÿ�ֳ�����
	Lint             m_out_cards_last_turn[WURENBAIFEN_PLAYER_COUNT]; //���һ�ֳ���

	Lint				m_scoreCardsCount;									//�÷��˿���Ŀ
	Lint				m_scoreCards[12 * WURENBAIFEN_PACK_COUNT];				//�÷��˿�
	Lint             m_scoreCardsLastTurn[WURENBAIFEN_PLAYER_COUNT];     // ���һ�ַ���
	Lint             m_scoceCardsCountLastTurn;                                                      // ���һ�ַ��˿�
	Lint				m_leftover_score;									//����ʣ��ķ�

	Lint				m_player_score[WURENBAIFEN_PLAYER_COUNT];				//��ұ��ֵ÷�

	Lint				m_reset105[WURENBAIFEN_PLAYER_COUNT];					//��105�������
	Lint				m_resetTime105;										//ʱ��
	BOOL				m_b105;												//�����Ƿ������105��

	// ���ƿ���
	Lint				m_jiaoPai[WURENBAIFEN_PLAYER_COUNT];				//ÿ���˶Դ����ƵĴ����� -2:��ʼ״̬�� -1:�ȴ�ѡ�� 0:��ͬ��  1:ͬ�� 2:�ѳ�ʱ
	JIAO_PAI_STATE   m_jiaoPaiState;                                                                //����״̬
	BOOL			m_bJiaoPai;											//�����Ƿ��Ѿ������������	 
	Lint             m_nTimeSecondZhuangRequstJiaoPai;         // ��¼�����뽻�ƿ�ʼʱ��
	Lint             m_nTimeSecondZhuangReqesutJiaoPaiLeft; // ׯ��ʣ������ʱ��
	Lint             m_nTimeSecondXianResponseJiaoPai;          // �мҲ�����ʼʱ��
	Lint             m_nTimeSecondXianResponseJiaoPaiLeft;   // �м�ʣ�����ʱ��

	/* �ṹ�����㺯�� */
	void clear_round()
	{
		memset(this, 0, sizeof(*this));
		m_curPos = WURENBAIFEN_INVALID_POS;
		m_firstOutPos = WURENBAIFEN_INVALID_POS;
		m_wTurnWinner = WURENBAIFEN_INVALID_POS;
		m_friendZhuangPos = WURENBAIFEN_INVALID_POS;
		m_mainColor = 0x40;
		m_base_cards_count = WURENBAIFEN_BASE_CARDS_COUNT;
		m_selectFriendCard = 0;
		m_thisTurnScore = 0;
		m_resetTime105 = 0;
		m_b105 = false;
		m_leftover_score = 100;
		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			m_select_score[i] = -1;
			m_reset105[i] = -2;
			m_jiaoPai[i] = -2;
		}
	
		m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_INIT;
		m_bJiaoPai = false;
		m_nTimeSecondZhuangRequstJiaoPai = m_nTimeSecondXianResponseJiaoPai = 0;
		m_nTimeSecondZhuangReqesutJiaoPaiLeft = m_nTimeSecondXianResponseJiaoPaiLeft = 0;
	}
};

/* �����ÿ�ֻ��ʼ���ֶΣ��ṹ�壩*/
struct WuRenBaiFenRoundState__cxx_part
{
	LTime m_play_status_time;

	/* ����ÿ���ֶ� */
	void clear_round()
	{
		this->~WuRenBaiFenRoundState__cxx_part();
		new (this)WuRenBaiFenRoundState__cxx_part;
	}
};

/* �����ÿ�������ʼ�����ֶΣ��ṹ�壩*/
struct WuRenBaiFenMatchState__c_part
{
	Desk*		m_desk;									// ���Ӷ���
	Lint		m_round_offset;							// ��ǰ����
	Lint		m_round_limit;							// ��������ѡ���������
	Lint		m_accum_score[WURENBAIFEN_PLAYER_COUNT];	//�洢����ܵ÷�
	BOOL		m_dismissed;
	Lint		m_registered_game_type;
	Lint		m_player_count;							// �淨���Ĵ�����ʹ�õ���������ֶ�
	Lint		m_zhuangPos;							// ׯ��λ��
	BOOL		m_isFirstFlag;							// �׾���ʾ��ʼ��ť,true�����׾�
	Lint		m_baseCardCount;						// ��������
	Lint		m_firstSelectCardPos;					// ÿ�ֵ�һ�����Ƶ�λ��

	////�淨Сѡ��
	Lint		m_baseTimes;							//�׷ֱ���
	BOOL		m_is2ChangZhu;							//2�Ƿ�Ϊ������true��2�ǳ���  false��2���ǳ���
	BOOL		m_isPokertTen;							//�Ƿ�ʮ��0-����  1-��ʮ
	BOOL		m_cheatAgainst;							//ר�������ף� 0������  1����
	BOOL		m_isGanKouDi;							//�Ƿ������мҸɿٵף�  0��������  1������
	Lint 		m_nJiaoPaiOption;							//�Ƿ�����ׯ�ҽ��ƣ�    0��������  1: 8�ֽ���2:12�ֽ�
	bool m_bAnyScoreCanSelectMainColor; // �����ѡ����ɫ 

	Lint		m_packCount;							//�Ƹ���

														/* ����ṹ���ֶ� */
	void clear_match()
	{
		memset(this, 0, sizeof(*this));
		m_baseTimes = 1;
		m_packCount = 1;
		m_zhuangPos = WURENBAIFEN_PLAYER_COUNT;
		m_firstSelectCardPos = WURENBAIFEN_PLAYER_COUNT;
		m_nJiaoPaiOption = 0;
		m_isGanKouDi = false;
		m_bAnyScoreCanSelectMainColor = false;
	}
};

/* �����ÿ�����ʼ�����ֶΣ��ṹ�壩*/
struct WuRenBaiFenMatchState__cxx_part
{
	WRBFGameLogic   m_gamelogic;     // ��Ϸ�߼�
	PlayType       m_playtype;	    // �����淨Сѡ��

									/* ���ÿ���ṹ�� */
	void clear_match()
	{
		this->~WuRenBaiFenMatchState__cxx_part();
		new (this) WuRenBaiFenMatchState__cxx_part;
	}
};

/*
*  �����ÿ������Ҫ�������ֶΣ��ṹ�壩
*  �̳� ��WuRenBaiFenRoundState__c_pard, WuRenBaiFenRoundState_cxx_part
*/
struct WuRenBaiFenRoundState : WuRenBaiFenRoundState__c_part, WuRenBaiFenRoundState__cxx_part
{
	void clear_round()
	{
		WuRenBaiFenRoundState__c_part::clear_round();
		WuRenBaiFenRoundState__cxx_part::clear_round();
	}
};

/*
*  �����ÿ������Ҫ�������ֶΣ��ṹ�壩
*  �̳У�WuRenBaiFenMatchState__c_pard, WuRenBaiFenMatchState_cxx_pard
*/
struct WuRenBaiFenMatchState : WuRenBaiFenMatchState__c_part, WuRenBaiFenMatchState__cxx_part
{
	void clear_match()
	{
		WuRenBaiFenMatchState__c_part::clear_match();
		WuRenBaiFenMatchState__cxx_part::clear_match();
	}
};

/*
*  ���������״̬���ṹ�壩
*  �̳У�ShuangShengRoundState, NiuNiuMatchState
*/
struct WuRenBaiFenDeskState : WuRenBaiFenRoundState, WuRenBaiFenMatchState
{
	/* ����ÿ������ */
	void clear_round()
	{
		WuRenBaiFenRoundState::clear_round();
	}

	/* ����ÿ������*/
	void clear_match(int player_count)
	{
		WuRenBaiFenMatchState::clear_match();
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
		if (play_status > GAME_PLAY_BEGIN && play_status <= GAME_PLAY_END)
		{
			m_play_status = (GAME_PLAY_STATE)play_status;
		}
	}

	/* ��ȡ������������ϵ�λ�� */
	Lint GetUserPos(User* pUser)
	{
		if (NULL == pUser)
		{
			return WURENBAIFEN_INVALID_POS;
		}

		Lint pos = WURENBAIFEN_INVALID_POS;
		if (m_desk)
		{
			pos = m_desk->GetUserPos(pUser);
		}
		if (pos >= WURENBAIFEN_INVALID_POS)
		{
			pos = WURENBAIFEN_INVALID_POS;
		}
		return pos;
	}

	/* ��ȡ��һ����Чλ�� */
	Lint GetPrePos(Lint pos)
	{
		if (m_player_count == 0)
			return 0; // ����������	
		Lint nextPos = (pos - 1) % m_player_count;
		for (int i = 0; i < m_player_count; i++)
		{
			nextPos = (nextPos - 1) % m_player_count;
		}
		return nextPos;
	}

	/* ��ȡ��һ����Чλ�� */
	Lint GetNextPos(Lint pos)
	{
		if (m_player_count == 0)
			return 0; // ����������	
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
		return 0 <= pos && pos < WURENBAIFEN_INVALID_POS;
	}

	/* ���ֲ��᳤�Ƿ������ƹ�ս */
	bool IsClubOwerLookOn()
	{
		if (NULL == m_desk)
		{
			return false;
		}
		if (m_desk->m_clubOwerLookOn == 1 && m_desk->m_clubOwerLookOnUser)
		{
			return true;
		}
		return 0;
	}

	/* ��ȡ�����Գ��Ƶ����� */
	Lint GetHasOutPlayerCount()
	{
		int t_outPlayerCount = 0;
		if (m_firstOutPos == WURENBAIFEN_INVALID_POS
			|| !m_gamelogic.IsValidCard(m_out_cards[m_firstOutPos]))
		{
			return 0;
		}

		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			if (m_gamelogic.IsValidCard(m_out_cards[i]))
			{
				++t_outPlayerCount;
			}
		}
		return t_outPlayerCount;
	}
};

/* �����¼����(�ṹ��) */
struct WuRenBaiFenVideoSupport : WuRenBaiFenDeskState
{
	QiPaiVideoLog m_video; //¼��

						   ///////////////////////////////////////////////////////////////////
						   //��鼯�д�ź�¼���йصĺ���
						   ///////////////////////////////////////////////////////////////////
	void VideoSave()
	{
		if (NULL == m_desk)
			return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		m_video.m_playType = m_desk->getPlayType();
		m_video.m_flag = m_desk->m_state;
		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		LLOG_DEBUG("videoSave[%s]", video.m_sql.c_str());
		gWork.SendMsgToDb(video);
	}

	//���˰ٷ� oper ����  0--��  1--����   λ��   ���Ƹ���   ���� �������Ƹ���  ��������  �÷�
	void SaveUserOper(Lint oper, Lint pos, BYTE handcardCount, Lint *cbHandCardData, Lint cardCount = 0, Lint *cbCardData = NULL, Lint score = 0, Lint sound = -1)
	{
		LLOG_DEBUG("SaveUserOper  pos[%d],oper[%d]", pos, oper);

		std::vector<Lint> cards;
		std::vector<Lint> handcards;
		if (cardCount != 0)
		{
			if (cbCardData != NULL)
			{
				for (int i = 0; i < cardCount; i++)
				{
					LLOG_DEBUG("cards[%d]=[%d]", i, cbCardData[i]);
					cards.push_back(cbCardData[i]);
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
					handcards.push_back(cbHandCardData[i]);
				}
			}
		}
		m_video.AddOper(oper, pos, cards, score, handcards, sound);
	}
};

/* �������Ϸ���崦���߼����ṹ�壩*/
struct WuRenBaiFenGameCore : GameHandler, WuRenBaiFenVideoSupport
{
	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
	}

	virtual void notify_lookon(LMsg &msg) {
	}

	virtual void notify_club_ower_lookon(LMsg& msg) {
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
		notify_lookon(msg);
		notify_club_ower_lookon(msg);
	}

	/********************************************************************************************
	* ��������      start_game()
	* ������        �����ÿһ����Ϸ��ʼ�ľ������
	* ���أ�        (void)
	*********************************************************************************************/
	void start_game()
	{
		//������Ϸ״̬Ϊ���ƽ׶�
		set_play_status(GAME_PLAY_SEND_CARD);

		//���õ�ǰ���Ϊ���ֵ�һ���������
		m_curPos = m_firstSelectCardPos;

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
		LLOG_DEBUG("WuRenBaiFenGameHandler::start_round() Run... WuRenBaiFen GAME_START!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! deskId=[%d],",
			m_desk ? m_desk->GetDeskId() : 0);

		LLOG_ERROR("WuRenBaiFenGameHandler::start_round() Run... This Round Play Type deskId=[%d], baseTimes=[%d], is2ChangZhu=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_baseTimes, m_is2ChangZhu);

		Lint t_user_status[WURENBAIFEN_PLAYER_COUNT];
		memcpy(t_user_status, player_status, sizeof(Lint) * WURENBAIFEN_PLAYER_COUNT);

		clear_round();

		//�㲥��ǰ���������
		notify_desk_match_state();

		set_desk_state(DESK_PLAY);

		memcpy(m_user_status, t_user_status, sizeof(m_user_status));
		
		if (m_desk && m_desk->m_vip)
		{
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);
		}
		LLOG_ERROR("desk_id=[%d], start_round=[%d/%d], player_count=%d", m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

		//��Ϸ��ʼ���
		start_game();
	}

	//������Ч
	int GetSound()
	{
		int nRet = 0;
		int nOutCardCount = GetHasOutPlayerCount();					//���ֳ��Ƶ��˵�����
																	//m_firstOutPos;							//ÿ�ֵ�һ���������

																	// ����ÿ����ҳ�����   ��һ�����Ƶ����  �������
		Lint nOutMax = m_gamelogic.CompareOutCardArray(m_out_cards, m_firstOutPos, nOutCardCount);   //����������
		Lint nFirstColor = m_gamelogic.GetCardLogicColor(m_out_cards[m_firstOutPos]);					 //��һ�����Ƶ��˵Ļ�ɫ
		Lint nCurrColor = m_gamelogic.GetCardLogicColor(m_out_cards[m_curPos]);

		if (nOutCardCount == 1 && nFirstColor == COLOR_NT)		//���ֵ�һ������,�ҳ������ƻ�ɫ
		{
			nRet = GAME_PLAY_SOUND_DIAOZHU;
			return nRet;
		}
		//�жϱк͸Ǳ�		����1:�������.����2:��һ���˳��Ĳ�����,֮ǰû���˱�,�����˼����ϲ�
		if (nOutMax == m_curPos && nFirstColor != COLOR_NT && nOutCardCount >= 2 && m_gamelogic.GetCardLogicColor(m_out_cards[m_curPos]) == COLOR_NT)
		{
			int iTmpMax = m_gamelogic.CompareOutCardArray(m_out_cards, m_firstOutPos, nOutCardCount - 1);					 //��ȥ�Լ�,˭���
																															 //�������������,���ǸǱ�,������Ĳ�������,���Ǳ���
			if (m_gamelogic.GetCardLogicColor(m_out_cards[iTmpMax]) == COLOR_NT)
			{
				return GAME_PLAY_SOUND_GAIBI;
			}
			else
			{
				return GAME_PLAY_SOUND_BIPAI;
			}
		}
		if (nOutCardCount >= 2 && nOutMax == m_curPos)
		{
			if ((gWork.GetCurTime().Secs() % 2) == 0)
			{
				return GAME_PLAY_SOUND_DANI;
			}
			else
			{
				return GAME_PLAY_SOUND_GUANSHANG;
			}
		}

		//����
		if (nOutCardCount >= 2 && nCurrColor != COLOR_NT && nCurrColor != nFirstColor)
		{
			return GAME_PLAY_SOUND_DIANPAI;
		}


		return GAME_PLAY_SOUND_PA;
	}

	/* ϴ�Ʒ��� */
	bool dispatch_user_cards()
	{
		//�������˿���
		Lint t_randCards[WURENBAIFEN_CELL_PACK * WURENBAIFEN_PACK_COUNT];

		//�طż�¼�������
		std::vector<Lint> vec_handCards[WURENBAIFEN_PLAYER_COUNT];
		std::vector<Lint> vec_baseCards;

		//�����˿�
		m_gamelogic.RandCardList(t_randCards, WURENBAIFEN_CELL_PACK * WURENBAIFEN_PACK_COUNT);

		//����
		debugSendCards_WuRenBaiFen(t_randCards, m_desk, "card_wurenbaifen.ini", WURENBAIFEN_PLAYER_COUNT, WURENBAIFEN_HAND_CARDS_COUNT, WURENBAIFEN_CELL_PACK);

		//��ÿ����ҷ�����
		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
		{
			m_hand_cards_count[i] = WURENBAIFEN_HAND_CARDS_COUNT;
			memcpy(&m_hand_cards[i], &t_randCards[i * m_hand_cards_count[i]], sizeof(Lint) * m_hand_cards_count[i]);
			m_gamelogic.SortCardList(m_hand_cards[i], m_hand_cards_count[i]);

			for (int n = 0; n < m_hand_cards_count[i]; n++)
			{
				vec_handCards[i].push_back(m_hand_cards[i][n]);
			}
		}

		//�����˿�
		m_baseCardCount = WURENBAIFEN_BASE_CARDS_COUNT;
		memcpy(m_base_cards, &t_randCards[WURENBAIFEN_CELL_PACK * WURENBAIFEN_PACK_COUNT - m_baseCardCount], sizeof(Lint) * m_baseCardCount);

		for (int i = 0; i < WURENBAIFEN_BASE_CARDS_COUNT; ++i)
		{
			vec_baseCards.push_back(m_base_cards[i]);
		}


		//����ҷ�������
		WuRenBaiFenS2CSendCards sendCards[WURENBAIFEN_PLAYER_COUNT];
		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
		{
			sendCards[i].m_outTime = 0;
			sendCards[i].m_pos = i;
			sendCards[i].m_nextSelectScorePos = m_firstSelectCardPos;
			sendCards[i].m_handCardsCount = m_hand_cards_count[i];
			sendCards[i].m_is2ChangZhu = m_is2ChangZhu ? 1 : 0;
			for (int j = 0; j < WURENBAIFEN_HAND_CARDS_COUNT; ++j)
			{
				sendCards[i].m_handCards[j] = (int)m_hand_cards[i][j];
			}

			memcpy(sendCards[i].m_totleScore, m_accum_score, sizeof(m_accum_score));

			//����ʱ�����������������ƣ�����ɫ�����޷�����
			m_gamelogic.SortCardList(m_hand_cards[i], m_hand_cards_count[i]);
		}
		notify_user(sendCards[0], 0);
		notify_user(sendCards[1], 1);
		notify_user(sendCards[2], 2);
		notify_user(sendCards[3], 3);
		notify_user(sendCards[4], 4);

		//��¼¼��ϴ�ƣ�
		m_video.DrawClear();
		m_video.DealCard(m_desk->m_vip->m_curCircle, WURENBAIFEN_PLAYER_COUNT, m_desk->m_vip->m_posUserId, vec_handCards, vec_baseCards, gWork.GetCurTime().Secs());

		//�㲥����ս��ҷ���
		WuRenBaiFenS2CSendCards sendCards_look;
		sendCards_look.m_outTime = 0;
		sendCards_look.m_pos = WURENBAIFEN_INVALID_POS;
		sendCards_look.m_handCardsCount = m_hand_cards_count[0];
		sendCards_look.m_is2ChangZhu = m_is2ChangZhu ? 1 : 0;
		sendCards_look.m_nextSelectScorePos = m_firstSelectCardPos;
		memcpy(sendCards_look.m_totleScore, m_accum_score, sizeof(m_accum_score));
		notify_lookon(sendCards_look);

		//�㲥���ֲ��᳤���ƹ�ս
		if (IsClubOwerLookOn())
		{
			memcpy(sendCards_look.m_clubAllHandCards, m_hand_cards, sizeof(m_hand_cards));
			notify_club_ower_lookon(sendCards_look);
		}

		//���0������־
		LLOG_DEBUG("notify_user0:Send Cards To player0 cardCount=[%d], deskId=[%d]", sendCards[0].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < WURENBAIFEN_HAND_CARDS_COUNT; ++i)
		{
			LLOG_DEBUG("Send Cards Player0.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[0].m_handCards[i], sendCards[0].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//���1������־
		LLOG_DEBUG("notify_user1:Send Cards To player1 cardCount=[%d], deskId=[%d]", sendCards[1].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < WURENBAIFEN_HAND_CARDS_COUNT; ++i)
		{
			LLOG_DEBUG("Send Cards Player1.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[1].m_handCards[i], sendCards[1].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//���2������־
		LLOG_DEBUG("notify_user2:Send Cards To player2 cardCount=[%d], deskId=[%d]", sendCards[2].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < WURENBAIFEN_HAND_CARDS_COUNT; ++i)
		{
			LLOG_DEBUG("Send Cards Player2.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[2].m_handCards[i], sendCards[2].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//���3������־
		LLOG_DEBUG("notify_user3:Send Cards To player3 cardCount=[%d], deskId=[%d]", sendCards[3].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < WURENBAIFEN_HAND_CARDS_COUNT; ++i)
		{
			LLOG_DEBUG("Send Cards Player3.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[3].m_handCards[i], sendCards[3].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//���4������־
		LLOG_DEBUG("notify_user4:Send Cards To player4 cardCount=[%d], deskId=[%d]", sendCards[4].m_handCardsCount, m_desk ? m_desk->GetDeskId() : 0);
		for (int i = 0; i < WURENBAIFEN_HAND_CARDS_COUNT; ++i)
		{
			LLOG_DEBUG("Send Cards Player4.................[%2d]=[0x%2x | %2d], deskId=[%d]", i, sendCards[4].m_handCards[i], sendCards[4].m_handCards[i], m_desk ? m_desk->GetDeskId() : 0);
		}

		//������������Ϸ״̬Ϊ���Ʒ�״̬
		set_play_status(GAME_PLAY_SELECT_SCORE);

		LLOG_DEBUG("Send Cards Over ^_^... Start Send Cards Tick... deskId=[%d], playStatus=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_play_status);
		//m_tick_flag = true;
		//m_play_status_time.Now();

		return true;
	}

	bool has_opt_over(GAME_PLAY_STATE playState)
	{
		switch (playState)
		{
		case GAME_PLAY_BEGIN:
			break;
		case GAME_PLAY_SEND_CARD:
			break;
		case GAME_PLAY_SELECT_SCORE:
			for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
			{
				if (m_select_score[i] == -1)
				{
					return false;
				}
			}
			return true;
			break;
		case GAME_PLAY_SELECT_MAIN:
			break;
		case GAME_PLAY_BASE_CARDS:
			break;
		case GAME_PLAY_SELECT_FRIEND:
			break;
		case GAME_PLAY_OUT_CARD:
			break;
		case GAME_PLAY_END:
			break;
		default:
			break;
		}

		return false;
	}

	/*
	3��2��C->S ���ѡ����Ʒ�
	MSG_C_2_S_WURENBAIFEN_SELECT_SCORE = 62253
	*/
	bool on_event_user_select_score(Lint pos, Lint selectScore)
	{
		m_select_score[pos] = selectScore;
		if ((selectScore <= m_max_select_score && selectScore != 0)
			|| m_select_score[pos] == -1
			|| m_curPos != pos)
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_select_score() Error!!! selectScore <= m_max_select_score Or Has Selected Or Is Not CurrPos... deskId=[%d], selectScore=[%d], currPos=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, selectScore, m_curPos, pos);
			return false;
		}


		if (selectScore > m_max_select_score)
		{
			m_max_select_score = selectScore;
		}

		//�����ѡ������ �� ��ǰ��ҽ���100�֣��������һ������
		if (has_opt_over(GAME_PLAY_SELECT_SCORE) || m_select_score[pos] == 100)
		{
			//û�˽��ƣ����¿�ʼϴ��
			if (m_max_select_score == 0)
			{
				//�㲥��ҽ��ƽ��
				WuRenBaiFenS2CSelecScoreR selectScoreR;
				selectScoreR.m_pos = pos;
				selectScoreR.m_selectScore = selectScore;
				selectScoreR.m_currMaxScore = m_max_select_score;
				selectScoreR.m_isFinish = 0;
				selectScoreR.m_nextSelectScorePos = WURENBAIFEN_INVALID_POS; //pos+1;
				notify_desk(selectScoreR);

				//�㲥����ս���
				notify_lookon(selectScoreR);

				//�㲥���ֲ��᳤���ƹ�ս
				notify_club_ower_lookon(selectScoreR);

				LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_select_score() Error!!! All Player Select 0 Score, So Restart Game ... deskId=[%d]",
					m_desk ? m_desk->GetDeskId() : 0);

				start_round(m_user_status);
				return false;
			}		

			//���ƽ�����ׯ���õ��ƣ�ѡ��
			set_play_status(GAME_PLAY_SELECT_MAIN);
			//��ǰ�������  ׯ  ÿ�ֵ�һ���������  �ǽз������
			m_curPos = 0;
			int iTmpScore = 0;
			for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
			{
				if (m_select_score[i] > iTmpScore)
				{
					m_curPos = i;
					iTmpScore = m_select_score[i];
				}
			}

			//m_curPos = pos;
			m_zhuangPos = m_curPos;
			m_firstOutPos = m_curPos;

			m_firstSelectCardPos = m_zhuangPos;

			//�㲥��ҽ��ƽ��
			WuRenBaiFenS2CSelecScoreR selectScoreR;
			selectScoreR.m_pos = pos;
			selectScoreR.m_selectScore = selectScore;
			selectScoreR.m_currMaxScore = m_max_select_score;
			selectScoreR.m_isFinish = 0;
			selectScoreR.m_nextSelectScorePos = selectScore == 0 ? WURENBAIFEN_INVALID_POS : m_curPos; //pos+1;

			notify_desk(selectScoreR);

			//�㲥����ս���
			notify_lookon(selectScoreR);

			//�㲥���ֲ��᳤���ƹ�ս
			notify_club_ower_lookon(selectScoreR);

			

			//֪ͨׯ���õ��ƺ�ѡ����ɫ
			for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
			{
				WuRenBaiFenS2CBaseCardsAndSelectMainN baseCardN;

				LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_select_score() Select Score Over Scuess^v^...i = [%d], deskId=[%d],pos=[%d],m_zhuangPos = [%d], selectMaxScore=[%d], zhuangPos=[%d]",
					i, m_desk ? m_desk->GetDeskId() : 0, pos, m_zhuangPos, m_max_select_score, m_zhuangPos);

				baseCardN.m_postmd = i;
				baseCardN.m_maxSelectScore = m_max_select_score;
				baseCardN.m_zhuangPos = m_zhuangPos;				 
				baseCardN.m_jiaopaiOption = m_nJiaoPaiOption;// ���ý��Ʊ�־
			 
				//ׯ���õ���
				if (m_zhuangPos == i)
				{
					baseCardN.m_baseCardsCount = WURENBAIFEN_BASE_CARDS_COUNT;
					for (Lint j = 0; j < WURENBAIFEN_BASE_CARDS_COUNT; ++j)
						baseCardN.m_baseCards[j] = m_base_cards[j];

					//���ֲ��᳤���ƹ�ս
					notify_club_ower_lookon(baseCardN);
					if (1) // ��¼���ƿ�ʼʱ��
					{
						m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_INIT;
						m_nTimeSecondZhuangReqesutJiaoPaiLeft = GAME_DEFAULT_TIME::JIAOPAI_REQUEST_TIMEOUT;
						m_nTimeSecondZhuangRequstJiaoPai = LTime().Secs();
					}

				}
				notify_user(baseCardN, i);
			}

			//�㲥����ս���
			WuRenBaiFenS2CBaseCardsAndSelectMainN baseCardN_look;
			baseCardN_look.m_postmd = WURENBAIFEN_INVALID_POS;
			baseCardN_look.m_maxSelectScore = m_max_select_score;
			baseCardN_look.m_zhuangPos = m_zhuangPos;
			notify_lookon(baseCardN_look);

			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_select_score() Select Score Over Scuess^v^... deskId=[%d], selectMaxScore=[%d], zhuangPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_max_select_score, m_zhuangPos);
		}
		else
		{
			//��ȡ��һ���˵�λ��
			m_curPos = GetNextPos(m_curPos);

			//�㲥��ҽ��ƽ��
			WuRenBaiFenS2CSelecScoreR selectScoreR;
			selectScoreR.m_pos = pos;
			selectScoreR.m_selectScore = selectScore;
			selectScoreR.m_currMaxScore = m_max_select_score;
			selectScoreR.m_isFinish = 0;
			selectScoreR.m_nextSelectScorePos = m_curPos != m_firstSelectCardPos ? m_curPos : WURENBAIFEN_INVALID_POS; //pos+1;
			notify_desk(selectScoreR);

			//�㲥����ս���
			notify_lookon(selectScoreR);

			//�㲥���ֲ��᳤���ƹ�ս
			notify_club_ower_lookon(selectScoreR);

			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_select_score() Scuess^v^... deskId=[%d], selectScore=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, selectScore, pos);
		}

		//����¼����ҽз֣�
		SaveUserOper(GAME_PLAY_SELECT_SCORE, pos, m_hand_cards_count[pos], m_hand_cards[pos], 4, m_base_cards, selectScore);

		return true;
	}

	/*
	3��2��C->S ���ѡ����
	MSG_C_2_S_WURENBAIFEN_SELECT_MAIN = 62255
	*/
	bool on_event_user_select_main(Lint pos, Lint mainColor)
	{
		if (pos != m_zhuangPos)
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_select_main() Error!!! Select Main Pos Is Not Zhuang... deskId=[%d], zhuangPos=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, pos);
			return false;
		}

		//ѡ�������ɫ���Ƿ�÷���
		if (!m_gamelogic.IsValidColor(mainColor))
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_select_main() Error!!! Select Main Not Valid... deskId=[%d], zhuangPos=[%d], optPos=[%d], selectMain=[0x%2x]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, pos, mainColor);
			return false;
		}

		m_mainColor = m_gamelogic.GetCardColor(mainColor);

		//��������ɫ
		m_gamelogic.SetMainColor(m_mainColor);

		WuRenBaiFenS2CSelectMainColorR selectMainR;
		selectMainR.m_pos = pos;
		selectMainR.m_selectMainColor = m_mainColor;

		notify_desk(selectMainR);

		//�㲥����ս���
		notify_lookon(selectMainR);

		//�㲥�����ֲ��᳤���ƹ�ս
		notify_club_ower_lookon(selectMainR);

		LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_select_main() Sucess^v^... deskId=[%d], zhuangPos=[%d], optPos=[%d], selectMain=[0x%2x]",
			m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, pos, mainColor);

		//������Ϸ״̬Ϊׯ�����
		set_play_status(GAME_PLAY_BASE_CARDS);

		Lint nVideoCara[1];
		nVideoCara[0] = mainColor;

		//����¼�����ѡ����ɫ��
		SaveUserOper(GAME_PLAY_SELECT_MAIN, pos, m_hand_cards_count[pos], m_hand_cards[pos], 1, nVideoCara, 0);
		return true;
	}

	/*
	3��2��C->S ������
	MSG_C_2_S_WURENBAIFEN_BASE_CARD = 62257
	*/
	bool on_event_user_base_cards(Lint pos, Lint baseCards[], Lint baseCardsCount)
	{
		//Lint nVideoCard[8] = {0};
		if (pos != m_zhuangPos)
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_base_cards() Error!!! Base Cards Pos Is Not Zhuang... deskId=[%d], zhuangPos=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, pos);
			return false;
		}

		if (baseCardsCount != WURENBAIFEN_BASE_CARDS_COUNT)
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_base_cards() Error!!! Base Cards Count Error... deskId=[%d], zhuangPos=[%d], baseCardsCount=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, baseCardsCount);
			return false;
		}

		//ׯ��������ٵ���
		Lint t_handBaseCards[WURENBAIFEN_HAND_CARDS_COUNT_MAX];
		memset(t_handBaseCards, 0, sizeof(t_handBaseCards));

		for (Lint i = 0; i < WURENBAIFEN_HAND_CARDS_COUNT; ++i)
		{
			t_handBaseCards[i] = m_hand_cards[pos][i];
		}
		for (Lint i = WURENBAIFEN_HAND_CARDS_COUNT; i < WURENBAIFEN_HAND_CARDS_COUNT_MAX; ++i)
		{
			t_handBaseCards[i] = m_base_cards[i - WURENBAIFEN_HAND_CARDS_COUNT];
			//nVideoCard[i - WURENBAIFEN_HAND_CARDS_COUNT] = m_base_cards[i - WURENBAIFEN_HAND_CARDS_COUNT];
		}

		//��Ҹǵ����Ƿ�ɹ���־
		bool t_flag = false;

		//��׳ɹ�
		if (m_gamelogic.RemoveCard(baseCards, baseCardsCount, t_handBaseCards, WURENBAIFEN_HAND_CARDS_COUNT_MAX))
		{
			t_flag = true;

			m_hand_cards_count[pos] = WURENBAIFEN_HAND_CARDS_COUNT;
			memcpy(m_hand_cards[pos], t_handBaseCards, sizeof(Lint) * WURENBAIFEN_HAND_CARDS_COUNT);
			memcpy(m_base_cards, baseCards, sizeof(Lint) * WURENBAIFEN_BASE_CARDS_COUNT);

			for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
			{
				WuRenBaiFenS2CBaseCardR baseCardR;
				baseCardR.m_pos = pos;
				baseCardR.m_flag = 1;
				baseCardR.m_pokerTen = m_isPokertTen ? 1 : 0;
				if (i == pos)
				{
					baseCardR.m_handCardsCount = m_hand_cards_count[pos];
					memcpy(baseCardR.m_handCards, m_hand_cards[pos], sizeof(Lint) * m_hand_cards_count[pos]);
					memcpy(baseCardR.m_baseCards, m_base_cards, sizeof(Lint) * WURENBAIFEN_BASE_CARDS_COUNT);

					//�㲥�����ֲ��᳤���ƹ�ս
					notify_club_ower_lookon(baseCardR);
				}
				notify_user(baseCardR, i);
			}
			/*for (int i=0; i<4; i++)
			{
			nVideoCard[4 + i] = m_base_cards[i];
			}*/

			//�㲥��ս���
			WuRenBaiFenS2CBaseCardR baseCardR_look;
			baseCardR_look.m_pos = pos;
			baseCardR_look.m_flag = 1;
			baseCardR_look.m_pokerTen = m_isPokertTen ? 1 : 0;
			notify_lookon(baseCardR_look);

			//������Ϸ�׶�Ϊѡ�Լ�
			set_play_status(GAME_PLAY_SELECT_FRIEND);

			//����¼�������ף�
			SaveUserOper(GAME_PLAY_BASE_CARDS, pos, m_hand_cards_count[pos], m_hand_cards[pos], 4, m_base_cards, 0);

			return true;
		}

		//���ʧ��
		LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_base_cards() Failed!!! Remove Base Cards Faild... deskId=[%d], optPos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos);
		for (Lint i = 0; i < WURENBAIFEN_BASE_CARDS_COUNT; ++i)
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_base_cards()...................selectBaseCard[%d]=[0x%02x|%2d]",
				i, baseCards[i], baseCards[i]);
		}
		WuRenBaiFenS2CBaseCardR baseCardR;
		baseCardR.m_pos = pos;
		baseCardR.m_flag = 0;
		notify_desk(baseCardR);

		//�㲥����ս���
		notify_lookon(baseCardR);

		//�㲥�����ֲ��᳤���ƹ�ս
		notify_club_ower_lookon(baseCardR);
		return false;
	}

	/*
	3��2��C->S ���ѡ�Լ���
	MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND = 62259
	*/
	bool on_event_user_select_friend(Lint pos, Lint selectCard)
	{
		if (pos != m_zhuangPos)
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_select_friend() Error!!! Select Friend Pos Is Not Zhuang... deskId=[%d], zhuangPos=[%d], optPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, pos);
			return false;
		}

		if (!m_gamelogic.IsValidFirendCard(selectCard, m_max_select_score, m_bAnyScoreCanSelectMainColor))
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_select_friend() Error!!! Select Friend Cards Is Not Valid... deskId=[%d], zhuangPos=[%d], selectMaxScore=[%d], selectCard=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, m_max_select_score, selectCard);
			return false;
		}

		LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_select_friend() Sucess^v^... deskId=[%d], zhuangPos=[%d], selectMaxScore=[%d], selectCard=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, m_max_select_score, selectCard);

		m_friendZhuangPos = pos;
		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			if (m_gamelogic.IncludeCard(m_hand_cards[i], m_hand_cards_count[i], selectCard))
			{
				m_friendZhuangPos = i;
				break;
			}
		}
		m_selectFriendCard = selectCard;
		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			WuRenBaiFenS2CSelectFriendCardBC sendSelectFriend;
			sendSelectFriend.m_pos = pos;
			sendSelectFriend.m_selectFriendCard = selectCard;
			sendSelectFriend.m_nextOutCardPos = pos;


			if (i == m_friendZhuangPos)
			{
				sendSelectFriend.m_firendZhuangPos = m_friendZhuangPos;

				//�㲥�����ֲ��᳤���ƹ�ս
				notify_club_ower_lookon(sendSelectFriend);
			}
			//������Ϸ�׶�Ϊ����
			set_play_status(GAME_PLAY_OUT_CARD);
			notify_user(sendSelectFriend, i);
		}
		std::vector<Lint> retCard;
		for (Lint j = 0; j < WURENBAIFEN_BASE_CARDS_COUNT; ++j)
		{
			retCard.push_back(m_base_cards[j]);
		}

		m_video.SetZhuang(retCard, m_zhuangPos, m_friendZhuangPos, m_max_select_score);

		Lint nvideoCard[1] = { 0 };
		nvideoCard[0] = selectCard;

		//����¼�����ѡ�Լң�
		SaveUserOper(GAME_PLAY_SELECT_FRIEND, pos, m_hand_cards_count[pos], m_hand_cards[pos], 1, nvideoCard, m_friendZhuangPos);

		//�㲥����ս���
		WuRenBaiFenS2CSelectFriendCardBC sendSelectFriend_look;
		sendSelectFriend_look.m_pos = pos;
		sendSelectFriend_look.m_selectFriendCard = selectCard;
		sendSelectFriend_look.m_nextOutCardPos = pos;
		notify_lookon(sendSelectFriend_look);
		return true;
	}

	/*
	3��2��C->S ��ҳ���
	MSG_C_2_S_WURENBAIFEN_OUT_CARD = 62261
	*/
	bool on_event_user_out_card(Lint pos, Lint outCard)
	{
		LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_out_card() Player Out Card Run... desk_id=[%d], pos=[%d], outCard=[0x%2x|%2d], is_first_out=[%d]"
			, m_desk ? m_desk->GetDeskId() : 0, pos, outCard, outCard, pos == m_firstOutPos);

		if (pos != m_curPos)
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_out_card() Error!!! Out Card Pos Is Not CurrPos... deskId=[%d], currPos=[%d], optPos=[%d], outCard=[0x%2x|%2d]",
				m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, outCard, outCard);
			return false;
		}

		//���������Ƿ������������
		if (!m_gamelogic.IncludeCard(m_hand_cards[pos], m_hand_cards_count[pos], outCard))
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_out_card() Error!!! Out Card Is Not Include In Hand Cards... deskId=[%d], currPos=[%d], optPos=[%d], outCard=[0x%2x|%2d]",
				m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, outCard, outCard);
			return false;
		}

		//���������
		if (pos != m_firstOutPos)
		{
			//У�������
			if (!m_gamelogic.EfficacyOutCard(outCard, m_hand_cards[pos], m_hand_cards_count[pos], m_out_cards[m_firstOutPos]))
			{
				LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_out_card() Error!!! Fllow Out Card Is Not Valid... deskId=[%d], optPos=[%d], outCard=[0x%2x|%2d], firstOutPos=[%d], OutCard=[0x%2x|%2d]",
					m_desk ? m_desk->GetDeskId() : 0, pos, outCard, outCard, m_firstOutPos, m_out_cards[m_firstOutPos], m_out_cards[m_firstOutPos]);

				//���Ʋ�����,��Ҫ���ؿͻ���
				WuRenBaiFenS2COutCardBC outCardBC;
				outCardBC.m_pos = pos;
				outCardBC.m_mode = -1;
				memcpy(outCardBC.m_handCardCount, m_hand_cards_count, sizeof(m_hand_cards_count));
				notify_user(outCardBC, pos);

				return false;
			}
		}

		//�������ƴ�������ɾ��
		Lint t_outCard[1];
		t_outCard[0] = outCard;
		if (!m_gamelogic.RemoveCard(t_outCard, 1, m_hand_cards[pos], m_hand_cards_count[pos]))
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::on_event_user_out_card() Error!!! Remove Out Card Failed... deskId=[%d], currPos=[%d], optPos=[%d], outCard=[0x%2x|%2d]",
				m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, outCard, outCard);
			return false;
		}

		//���һ�ֳ���
		if (m_hand_cards_count[pos] == 1)
		{
			//Lint nOverState = 0;
			//��ÿ���˵����Ƹ��Ƶ�������
			for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
			{
				if (i == m_firstOutPos)
				{
					m_out_cards[i] = outCard;
				}
				else
				{
					m_out_cards[i] = m_hand_cards[i][0];
				}
				m_hand_cards_count[i] = 0;
			}
			//�������
			CopyMemory(m_out_cards_last_turn, m_out_cards, sizeof(Lint) * WURENBAIFEN_PLAYER_COUNT);
			memset(m_hand_cards, 0, sizeof(m_hand_cards));

			//�����мҵ÷����
			int t_thisTurnScore = 0;

			//�м��ܵ÷�
			int t_getTotleScore = 0;

			//�ж����һ�ֳ��Ƶ�ʤ��,�ض��ж�5�ң�ûë��
			m_wTurnWinner = m_gamelogic.CompareOutCardArray(m_out_cards, m_firstOutPos);


			//�����мҵ÷�
			if (m_wTurnWinner != m_zhuangPos && m_wTurnWinner != m_friendZhuangPos)
			{
				//��ȡ�����еķ���
				int t_turnScoreCardsCount = 0;
				int t_turnScoreCards[5];
				memset(t_turnScoreCards, 0, sizeof(t_turnScoreCards));
				t_turnScoreCardsCount = m_gamelogic.GetScoreCards(m_out_cards, 5, t_turnScoreCards);

				//�����Ƹ��Ƶ��ܵ÷�����,���·�������
				//memcpy(&m_scoreCards[m_scoreCardsCount], t_turnScoreCards, sizeof(int) * t_turnScoreCardsCount);
				//m_scoreCardsCount += t_turnScoreCardsCount;
				memcpy(&m_scoreCardsLastTurn[m_scoceCardsCountLastTurn], t_turnScoreCards, sizeof(int) * t_turnScoreCardsCount);
				m_scoceCardsCountLastTurn += t_turnScoreCardsCount;

				//���㱾���мҵ÷�
				t_thisTurnScore = m_gamelogic.GetCardsScore(t_turnScoreCards, t_turnScoreCardsCount);
				m_wurenbaifen_overstate = WURENBAIFEN_OVER_STATE::DIPAI_XIAN_WIN; //�п۵�
			}
			else
			{
				m_wurenbaifen_overstate = WURENBAIFEN_OVER_STATE::DIPAI_ZHUANG_WIN;     //ׯ�۵�
				if (m_scoreCardsCount == 0)
				{
					m_wurenbaifen_overstate = WURENBAIFEN_OVER_STATE::DIPAI_ZHUANG_WIN_XIAN_SCORE0; //��ͷ
				}
			}

			//�����м��ܵ÷�
			t_getTotleScore = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);

			//�ڴ�ͳһ�ж�״̬
			if (m_wTurnWinner != m_zhuangPos && m_wTurnWinner != m_friendZhuangPos)
			{
				if (m_max_select_score == 100)
				{
					nOverState = GAME_OVER_F;
				}
				else
				{
					nOverState = GAME_OVER_E;
				}
			}
			//ׯ�ҿ۵�
			else
			{
				if (t_getTotleScore == 0 && m_max_select_score == 100)
				{
					nOverState = GAME_OVER_D;
				}
				else if (t_getTotleScore == 0 && m_max_select_score < 100)
				{
					nOverState = GAME_OVER_C;
				}
				else
				{
					nOverState = GAME_OVER_B;
				}
			}

			//���ι㲥ÿ����ҵĳ�����Ϣ
			for (int i = m_firstOutPos; i < m_player_count + m_firstOutPos; ++i)
			{
				WuRenBaiFenS2COutCardBC outCardBC;
				outCardBC.m_pos = i % m_player_count;
				outCardBC.m_outCard = m_out_cards[outCardBC.m_pos];
				outCardBC.m_nextPos = (i + 1) % m_player_count;
				outCardBC.m_thisTurnScore = t_thisTurnScore;
				outCardBC.m_getTotleScore = t_getTotleScore;
				outCardBC.m_scoreCardsCount = m_scoreCardsCount;
				outCardBC.m_soundType = -1;//���һȦ��Ҫ��Ч GetSound();
				memcpy(outCardBC.m_scoreCards, m_scoreCards, sizeof(m_scoreCards));
				memcpy(outCardBC.m_handCardCount, m_hand_cards_count, sizeof(m_hand_cards_count));
				memcpy(outCardBC.m_outCardArray, m_out_cards, sizeof(m_out_cards));
				//��һ������
				if (i == m_firstOutPos)
				{
					outCardBC.m_turnStart = 1;
				}

				//���һ���������
				else if (i == (m_player_count + m_firstOutPos - 1))
				{
					outCardBC.m_turnOver = 1;
					outCardBC.m_turnWiner = m_wTurnWinner;
					outCardBC.m_currBig = m_wTurnWinner;
					outCardBC.m_nextPos = WURENBAIFEN_INVALID_POS;
				}

				notify_desk(outCardBC);
				Lint voputcard[1] = { outCardBC.m_outCard };

				//����¼����ҳ��ƣ�
				SaveUserOper(GAME_PLAY_OUT_CARD, outCardBC.m_pos, m_hand_cards_count[pos], m_hand_cards[pos], 1, voputcard, t_getTotleScore, (i == m_firstOutPos) ? -2 : outCardBC.m_soundType);

				//�㲥����ս���
				notify_lookon(outCardBC);

				//�㲥�����ֲ��᳤���ƹ�ս
				notify_club_ower_lookon(outCardBC);
			}

			//������һ�ֵ�һ�����Ƶ���
			m_firstSelectCardPos = GetNextPos(m_firstSelectCardPos);

			m_over_state = (OVER_STATE)nOverState;

			finish_round();
			//CalcSore();
			//OnGameOver(WIN_INVALID, nOverState, 1);

		}
		//�м����
		else
		{
			//������������
			m_hand_cards_count[pos] -= 1;

			//��¼���˳�����
			m_out_cards[pos] = outCard;

			int t_soundType = GetSound();

			//�л���ǰλ��Ϊ��һ���������λ��
			m_curPos = (pos + 1) % m_player_count;

			//�����һ���������λ�� �� ��һ���������λ����ͬ�����ֳ��ƽ���
			if (m_curPos == m_firstOutPos)
			{
				m_curPos = WURENBAIFEN_INVALID_POS;
			}

			//Ŀǰ���������
			int t_currBigPos = WURENBAIFEN_INVALID_POS;

			WuRenBaiFenS2COutCardBC outCardBC;
			outCardBC.m_pos = pos;
			outCardBC.m_nextPos = m_curPos;
			outCardBC.m_outCard = outCard;
			outCardBC.m_soundType = t_soundType;
			memcpy(outCardBC.m_handCardCount, m_hand_cards_count, sizeof(m_hand_cards_count));
			memcpy(outCardBC.m_outCardArray, m_out_cards, sizeof(m_out_cards));
			//���ֵ�һ���������
			if (pos == m_firstOutPos)
			{
				outCardBC.m_turnStart = 1;
			}
			//���������
			else
			{
				t_currBigPos = m_gamelogic.CompareOutCardArray(m_out_cards, m_firstOutPos, GetHasOutPlayerCount());
				outCardBC.m_currBig = t_currBigPos;
			}

			Lint vScore = -1;
			//�������һ����ҳ��ƣ�ȷ������Ӯ�ң��мҵ÷�����������׳������
			if (m_curPos == WURENBAIFEN_INVALID_POS)
			{
				//���㱾�ֳ��Ƶ�Ӯ��
				m_wTurnWinner = t_currBigPos;

				//�����мҵ÷�
				int t_thisTurnScore = 0;

				//��ȡ�����еķ���
				int t_turnScoreCardsCount = 0;				//�����Ƶ÷���
				int t_turnScoreCards[5];					//�÷ֵ�������Щ
				memset(t_turnScoreCards, 0, sizeof(t_turnScoreCards));
				t_turnScoreCardsCount = m_gamelogic.GetScoreCards(m_out_cards, 5, t_turnScoreCards);

				//�����мҵ÷�
				if (m_wTurnWinner != m_zhuangPos && m_wTurnWinner != m_friendZhuangPos)
				{

					//�����Ƹ��Ƶ��ܵ÷�����,���·�������
					memcpy(&m_scoreCards[m_scoreCardsCount], t_turnScoreCards, sizeof(int) * t_turnScoreCardsCount);
					m_scoreCardsCount += t_turnScoreCardsCount;

					//���㱾���мҵ÷�
					t_thisTurnScore = m_gamelogic.GetCardsScore(t_turnScoreCards, t_turnScoreCardsCount);
				}

				m_leftover_score -= m_gamelogic.GetCardsScore(t_turnScoreCards, t_turnScoreCardsCount);

				m_thisTurnScore = t_thisTurnScore;

				LLOG_DEBUG("WuRenBaiFenGameHandler::on_event_user_out_card() һ�ֽ�����2, deskId=[%d],m_thisTurnScore =[%d]",
					m_desk ? m_desk->GetDeskId() : 0, m_thisTurnScore, m_thisTurnScore);

				outCardBC.m_turnStart = 0;
				outCardBC.m_turnOver = 1;
				outCardBC.m_nextPos = m_wTurnWinner;
				outCardBC.m_turnWiner = m_wTurnWinner;
				outCardBC.m_thisTurnScore = t_thisTurnScore;
				outCardBC.m_leftover_score = m_leftover_score;

				m_curPos = m_wTurnWinner;		//���ֳ��Ƶ�Ӯ��Ϊ���ֳ��Ƶĵ�һ����

				outCardBC.m_getTotleScore = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);
				outCardBC.m_scoreCardsCount = m_scoreCardsCount;
				memcpy(outCardBC.m_scoreCards, m_scoreCards, sizeof(m_scoreCards));
				memcpy(outCardBC.m_prevOutCard, m_out_cards, sizeof(m_out_cards));
				memcpy(m_prev_out_cards, m_out_cards, sizeof(m_out_cards));

				//m_wTurnWinner = WURENBAIFEN_INVALID_POS;		//һ�ֳ�����,��������

				LLOG_DEBUG("WuRenBaiFenGameHandler::on_event_user_out_card() һ�ֽ�����1, deskId=[%d], currPos=[%d], optPos=[%d], outCard=[0x%2x|%2d]",
					m_desk ? m_desk->GetDeskId() : 0, m_curPos, pos, outCard, outCard);
				//�����������ֳ��Ƶ�һ�����Ƶ���ң���ձ��ֳ�����
				if (m_wTurnWinner != WURENBAIFEN_INVALID_POS && m_hand_cards[m_wTurnWinner] != 0)
				{
					m_firstOutPos = m_wTurnWinner;
					memset(m_out_cards, 0, sizeof(m_out_cards));
				}
				vScore = 0;
				vScore = m_thisTurnScore;
			}
			if (vScore == -1)
			{
				Lint voputcard[1] = { outCard };

				//����¼����ҳ��ƣ�
				SaveUserOper(GAME_PLAY_OUT_CARD, pos, m_hand_cards_count[pos], m_hand_cards[pos], 1, voputcard, m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount), (pos == m_firstOutPos) ? -2 : t_soundType);
			}
			else
			{
				Lint voputcard[2] = { outCard,0 };

				//����¼����ҳ��ƣ�
				SaveUserOper(GAME_PLAY_OUT_CARD, pos, m_hand_cards_count[pos], m_hand_cards[pos], 2, voputcard, m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount), (pos == m_firstOutPos) ? -2 : t_soundType);
			}


			notify_desk(outCardBC);

			//�㲥����ս���
			notify_lookon(outCardBC);

			//�㲥�����ֲ����ƹ�ս
			notify_club_ower_lookon(outCardBC);

			//�ж��¼ҵ÷��Ƿ����105

			if ((outCardBC.m_getTotleScore + m_max_select_score) >= WURENBAIFEN_WIN_SCORE_MAX && !m_b105)
			{
				for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
				{
					m_reset105[i] = -1;
				}

				m_b105 = true;
				//�㲥֪ͨ����Ƿ����ս��
				WuRenBaiFenS2CScore105 score105;
				score105.m_Score = outCardBC.m_getTotleScore + m_max_select_score;
				notify_desk(score105);
			}

		}

		return true;

	} //bool on_event_user_out_card(Lint pos, Lint outCard) END!!!

	/*
	���˰ٷ֣�C->S �ͻ���ׯ��ѡ����
	MSG_C_2_S_WURENBAIFEN_ZHUANG_JIAO_PAI = 62368
	*/
	bool on_event_zhuang_request_jiao_pai(Lint pos, bool bJiaoPai = false)
	{
		LLOG_DEBUG("User Request to jiaopai !!! Desk: %d pos:%d state:%d ",m_desk->GetDeskId(), pos, m_jiaoPaiState);
		if (pos != m_zhuangPos)
		{
			return false;
		}
		// ��齻��ѡ��
		//if (m_nJiaoPaiOption == 0)
		//{
		//	return false;
		//}

		//��齻��״̬
		if (m_jiaoPaiState != JIAO_PAI_STATE::JIAOPAI_STATE_INIT)
		{
			LLOG_ERROR("ERROR jiaopai state!!! Desk: %d pos:%d state:%d ", m_desk->GetDeskId(), pos, m_jiaoPaiState);
			return false;
		}

		//�㲥֪ͨ�м��Ƿ�ͬ��ׯ�ҽ���
		WuRenBaiFenS2CZhuangJiaoPaiBC jiaoPaiBC;
		Lint jiaopai_request = 1; //��
		//ׯ�Ҿܾ����ƣ���Ϸ����
		if (!bJiaoPai)
		{
			jiaopai_request = 2;
			m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_END;      
			m_nTimeSecondXianResponseJiaoPaiLeft = 0;
			m_nTimeSecondXianResponseJiaoPai = LTime().Secs();
			MHLOG_PLAYCARD("ׯ��ȡ������ desk: %d zhuangpos:%d", m_desk->GetDeskId(), m_zhuangPos);
		}
		else
		{
			MHLOG_PLAYCARD("ׯ�����뽻�� desk: %d zhuangpos:%d", m_desk->GetDeskId(), m_zhuangPos);
			for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
			{
				m_jiaoPai[i] = -1;
			}
			m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_REQUESTED; // ׯ��������
			jiaopai_request = 1;
			m_jiaoPai[m_zhuangPos] = 1;
			//���Ʊ�־
			m_bJiaoPai = true;
			m_nTimeSecondXianResponseJiaoPaiLeft = GAME_DEFAULT_TIME::JIAOPAI_RESPONSE_TIMEOUT;
			m_nTimeSecondXianResponseJiaoPai = LTime().Secs();
		}

		jiaoPaiBC.m_jiaopaiState = m_jiaoPaiState;
		jiaoPaiBC.m_zhuangPos = m_zhuangPos;
		jiaoPaiBC.m_jiaopaiRequest = jiaopai_request;
		jiaoPaiBC.m_xianResponseLeft = m_nTimeSecondXianResponseJiaoPaiLeft;
		notify_desk(jiaoPaiBC);
		return true;
	}

	/*
	���˰ٷ֣�C->S ��ׯ�ҿͻ���ѡ��ׯ�ҽ��Ƶ�����
	MSG_C_2_S_WURENBAIFEN_XIAN_SELECT_JIAO_PAI = 62370
	*/
	bool on_event_xian_select_jiao_pai(Lint pos, Lint selectRet)
	{
		if (selectRet != 0 && selectRet != 1)
		{
			return false;
		}

		if (pos == m_zhuangPos)
		{
			return false;
		}

		if (m_jiaoPaiState != JIAO_PAI_STATE::JIAOPAI_STATE_REQUESTED) //ׯ��������
		{
			LLOG_ERROR("ERROR jiaopai state!!! Desk: %d pos:%d state:%d ", m_jiaoPaiState, m_desk->GetDeskId(), pos);
			return false;
		}

		if (m_jiaoPai[pos] != -1)
		{
			return false;
		}

		//����״̬
		m_jiaoPai[pos] = selectRet;

		//�㲥��Ϣ
		WuRenBaiFenS2CJiaoPaiResult send;
		send.m_jiaoPaiSate = m_jiaoPaiState;
		send.m_zhuangPos = m_zhuangPos;
		memcpy(send.m_agree, m_jiaoPai, sizeof(m_jiaoPai));
		notify_desk(send);	 

		int iRet = 1;
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
		{
			if (i == m_zhuangPos)
			{
				continue;
			}

			if (m_jiaoPai[i] == 0)		//���˾ܾ�
			{
				iRet = 0;
				//��ս���״̬
				for (Lint n = 0; n < WURENBAIFEN_PLAYER_COUNT; ++n)
				{
					m_jiaoPai[n] = -2;
				}
				m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_XIAN_NOT_AGREE; //�м��Ѿܾ�			
				break;
			}
			if (m_jiaoPai[i] == -1)		//�ȴ���
			{
				iRet = 0;
				//break;
			}
		}
		if (iRet == 1) //ȫ��ͬ��
		{
			MHLOG_PLAYCARD("ׯ�ҽ��Ƴɹ� desk: %d zhuangpos:%d", m_desk->GetDeskId(), m_zhuangPos);
			m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_COMPLETE; //ȫ��ͬ��
			m_firstSelectCardPos = GetNextPos(m_firstSelectCardPos);		//������һ�ֵ�һ�����Ƶ���
			m_over_state = GAME_OVER_A;
			m_wurenbaifen_overstate = WURENBAIFEN_OVER_STATE::DIPAI_ZHUANG_JIAOPAI;
			m_wTurnWinner = m_zhuangPos;
			//ׯ�ҽ���
			SaveUserOper(GAME_PLAY_STATE::GAME_PLAY_ZHUAGN_JIAOPAI, m_zhuangPos, 0, NULL);
			finish_round();
		}
		else if (iRet == 0)  // ���˾ܾ������м�δ��Ӧ
		{		
		}
		return true;
	}

	//���˰ٷ����
	void CalcScoreForWuRenBaiFen()
	{
		Lint this_score = 0;
		bool b_first_xian = true;
		Lint  x_part = 0;
		Lint  y_part = 0;
		Lint max_call_score = m_max_select_score;
		Lint xian_desk_score = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount); //�мҼ��
		Lint last_turn_score = m_gamelogic.GetCardsScore(m_scoreCardsLastTurn, m_scoceCardsCountLastTurn); //���һ�ַ�ֵ
		Lint dipai_score = m_gamelogic.GetCardsScore(m_base_cards, m_base_cards_count); //���һ�ַ�ֵ
		x_part = (max_call_score + xian_desk_score - 100 + 5) / 10;
		y_part = (last_turn_score + dipai_score + 5) / 10;

		//ׯ����
		if (m_wurenbaifen_overstate == WURENBAIFEN_OVER_STATE::DIPAI_ZHUANG_JIAOPAI)
		{
			this_score = 0;
			if (m_nJiaoPaiOption == 1)
			{
				this_score = 2;
			}
			else if (2 == m_nJiaoPaiOption)
			{
				this_score = 3;
			}	 
			for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
			{			 
				if (i == m_zhuangPos)  continue;				
				m_player_score[i] += this_score;
				m_player_score[m_zhuangPos] -= this_score;				 
			}
		}
		else if (m_wurenbaifen_overstate == WURENBAIFEN_OVER_STATE::DIPAI_ZHUANG_WIN) //ׯ����
		{
			Lint judge_score = max_call_score + xian_desk_score - 100;
			if (judge_score > 0) // �м�Ӯ
			{
				this_score = -x_part;
			}
			else                            //ׯ��Ӯ
			{
				this_score = 2;
			}
			b_first_xian = true;
			for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
			{
				if (i == m_zhuangPos || i == m_friendZhuangPos) continue;
				m_player_score[i] -= this_score;
				if (b_first_xian)
				{
					m_player_score[m_friendZhuangPos] += this_score;
					b_first_xian = false;
				}
				else
				{
					m_player_score[m_zhuangPos] += this_score;
				}
			}
		}
		else if (m_wurenbaifen_overstate == WURENBAIFEN_OVER_STATE::DIPAI_ZHUANG_WIN_XIAN_SCORE0) // ׯ���׹�ͷ
		{
			this_score = 3;
			b_first_xian = true;
			for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
			{ 			 
				if (i == m_zhuangPos || i == m_friendZhuangPos) continue;				
				m_player_score[i] -= this_score;
				if (b_first_xian)
				{
					m_player_score[m_friendZhuangPos] += this_score;
					b_first_xian = false;
				}
				else
				{
					m_player_score[m_zhuangPos] += this_score;
				}
			}
		}
		else if (m_wurenbaifen_overstate == WURENBAIFEN_OVER_STATE::DIPAI_XIAN_WIN) //�п۵�
		{
			for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
			{
				if (m_isGanKouDi) //�ɿ۵��淨
				{
					this_score = 4;
					if (i == m_zhuangPos || i == m_friendZhuangPos) continue;
					m_player_score[i] += this_score;
					if (b_first_xian)
					{
						m_player_score[m_friendZhuangPos] -= this_score;
						b_first_xian = false;
					}
					else
					{
						m_player_score[m_zhuangPos] -= this_score;
					}
				}
				else
				{
					this_score = 3 + x_part + y_part;
					if (i == m_zhuangPos || i == m_friendZhuangPos) continue;
					m_player_score[i] += this_score;
					m_player_score[m_zhuangPos] -= this_score;
				}
			}
		}

		Lint base = m_playtype.WuRenBaiFenBaseTimes();

		// �ۼ��ܷ�
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
		{
			m_player_score[i] *= base;
			m_accum_score[i] += m_player_score[i];
		}
	}


	  //���
	void CalcSore()
	{
		Lint  t_thisTurnScore = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);
		int  nZhuangMoney = 0;				//����ׯ�һ���
		int  nViceZhuangMoney = 0;			//���ָ�ׯ����
		int  nXianMoney = 0;				//�����мһ���
											//���㱾����һ���

		if (m_wTurnWinner != m_zhuangPos && m_wTurnWinner != m_friendZhuangPos) //�мҿ۵ף�����ץ��,�÷ַ���
		{
			nZhuangMoney = ((m_max_select_score - 75) / 5) * -4;
			nViceZhuangMoney = ((m_max_select_score - 75) / 5) * -2;
			nXianMoney = ((m_max_select_score - 75) / 5) * 2;

		}
		else  //ׯ�ұ���
		{
			int iTmpScore = m_max_select_score + t_thisTurnScore;		//ׯ�ҽз�+�мҵ÷�
			if (iTmpScore >= 105)
			{
				if (m_max_select_score == 100)  //�з�100���÷ַ���
				{
					nZhuangMoney = ((m_max_select_score - 75) / 5) * -4;
					nViceZhuangMoney = ((m_max_select_score - 75) / 5) * -2;
					nXianMoney = ((m_max_select_score - 75) / 5) * 2;
				}
				else  //�з�С��100���÷ֲ�����
				{
					nZhuangMoney = ((m_max_select_score - 75) / 5) * -2;
					nViceZhuangMoney = ((m_max_select_score - 75) / 5) * -1;
					nXianMoney = ((m_max_select_score - 75) / 5) * 1;
				}
			}
			else if (iTmpScore < 105)
			{
				if (m_max_select_score == 100 || t_thisTurnScore == 0)  //�з�100�����߹�ͷ���÷ַ���
				{
					nZhuangMoney = ((m_max_select_score - 75) / 5) * 4;
					nViceZhuangMoney = ((m_max_select_score - 75) / 5) * 2;
					nXianMoney = ((m_max_select_score - 75) / 5) * -2;
				}
				else
				{
					nZhuangMoney = ((m_max_select_score - 75) / 5) * 2;
					nViceZhuangMoney = ((m_max_select_score - 75) / 5) * 1;
					nXianMoney = ((m_max_select_score - 75) / 5) * -1;
				}
			}
		}
		Lint base = m_playtype.WuRenBaiFenBaseTimes();

		nZhuangMoney = nZhuangMoney*base;
		nViceZhuangMoney = nViceZhuangMoney*base;
		nXianMoney = nXianMoney*base;

		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			if (i == m_zhuangPos)
			{
				m_player_score[m_zhuangPos] = nZhuangMoney;
				if (i == m_friendZhuangPos)
				{
					m_player_score[m_zhuangPos] = nXianMoney * -4;
				}
				m_accum_score[m_zhuangPos] += m_player_score[m_zhuangPos];
			}
			else if (i == m_friendZhuangPos)
			{
				m_accum_score[m_friendZhuangPos] += nViceZhuangMoney;
				m_player_score[m_friendZhuangPos] = nViceZhuangMoney;
			}
			else
			{
				m_accum_score[i] += nXianMoney;
				m_player_score[i] = nXianMoney;
			}
		}
		LLOG_DEBUG("CalcScore...  desk_id=[%d],ׯ=[%d],��ׯ=[%d],ׯ�з�=[%d],�мҵ÷�=[%d],�׷�=[%d],ׯ�һ���=[%d],��ׯ����=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_zhuangPos, m_friendZhuangPos, m_max_select_score, t_thisTurnScore, base,
			m_player_score[m_zhuangPos], m_player_score[m_friendZhuangPos]);
	}

	/**/
	void add_round_log(Lint* accum_score, Lint win_pos)
	{
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, accum_score, m_user_status, win_pos, m_video.m_Id);
		}
	}

	//����
	void finish_round(BOOL jiesan = false)
	{
		//��ҽ�ɢ���䣬����С������Ϣ
		if (jiesan)
		{
			LLOG_ERROR("ShuangShengGameHandler::finish_round() Run... Start Jie San ... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());
			WuRenBaiFenS2CDrawResult jiesan;
			jiesan.m_isFinish = 1;
			jiesan.m_type = 1;

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

		//���
		//CalcSore();

		CalcScoreForWuRenBaiFen();

		//����¼����Ϸ������
		SaveUserOper(GAME_PLAY_END, -1, 0, nullptr, 0, nullptr, 0);

		//����ط�
		m_video.addUserScore(m_accum_score);
		if (m_desk && m_desk->m_vip && m_desk->m_vip->m_reset == 0)
		{
			VideoSave();
		}

		//���Ӿ���
		increase_round();

		add_round_log(m_player_score, m_wTurnWinner);

		//��С������Ϣ
		WuRenBaiFenS2CDrawResult rest;
		rest.m_zhuangPos = m_zhuangPos;
		rest.m_type = (m_round_offset >= m_round_limit) ? 1 : 0;
		rest.m_state = nOverState;
		rest.m_fuzhuangPos = m_friendZhuangPos;
		rest.m_jiaofen = m_max_select_score;
		rest.m_defen = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);;
		memcpy(rest.m_baseCards, m_base_cards, sizeof(Lint) * WURENBAIFEN_BASE_CARDS_COUNT);
		rest.m_isFinish = 0;
		//�µĽ�����Ϣ
		rest.m_wurenbaifen_over_state = m_wurenbaifen_overstate;
		CopyMemory(rest.m_out_cards_last_turn, m_out_cards_last_turn, sizeof(m_out_cards_last_turn));

		for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
		{
			rest.m_score[i] = m_player_score[i];
			rest.m_totleScore[i] = m_accum_score[i];

			if (m_desk->m_user[i])
			{
				rest.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
				rest.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
				rest.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;			 
			}
		}
		notify_desk(rest);

		//�㲥��ս���
		notify_lookon(rest);

		//�㲥�᳤��ս
		notify_club_ower_lookon(rest);

		set_desk_state(DESK_WAIT);

		m_desk->HanderGameOver(1);
	}

};


/* �������Ϸ������ */
struct WuRenBaiFenGameHandler : WuRenBaiFenGameCore
{
	/* ���캯�� */
	WuRenBaiFenGameHandler()
	{
		LLOG_DEBUG("ShuangSheng GameHandler Init Over...");
	}

	/* ����ҷ���Ϣ */
	void notify_user(LMsg &msg, int pos) override
	{
		if (NULL == m_desk)
			return;
		if (pos < 0 || pos >= m_player_count)
			return;
		User *u = m_desk->m_user[pos];
		if (NULL == u)
			return;
		u->Send(msg);
	}

	/* �㲥��Ϣ */
	void notify_desk(LMsg &msg) override
	{
		if (NULL == m_desk)
			return;
		m_desk->BoadCast(msg);
	}

	/* �㲥��ս��� */
	void notify_lookon(LMsg &msg) override
	{
		if (NULL == m_desk)
		{
			return;
		}
		if (m_desk->m_desk_Lookon_user.size() == 0)
		{
			return;
		}
		m_desk->MHBoadCastDeskLookonUser(msg);
	}

	/* �㲥���ֲ��᳤���ƹ�ս */
	void notify_club_ower_lookon(LMsg& msg) override
	{
		if (NULL == m_desk)
		{
			return;
		}
		if (m_desk->m_clubOwerLookOn == 1 && m_desk->m_clubOwerLookOnUser)
		{
			m_desk->MHBoadCastClubOwerLookOnUser(msg);
		}
	}

	/* ÿ�ֿ�ʼ*/
	bool startup(Desk *desk) {
		if (NULL == desk) return false;
		clear_match(desk->m_desk_user_count);
		m_desk = desk;
		return true;
	}

	/* ÿ�ֽ���*/
	void shutdown(void)
	{
		clear_match(0);
	}


	/* �����淨Сѡ���Desk::SetVip()���� */
	void SetPlayType(std::vector<Lint>& l_playtype)
	{
		// �� Desk �� SetVip ���ã���ʱ�ܵõ� m_vip->m_maxCircle
		int round_limit = m_desk && m_desk->m_vip ? m_desk->m_vip->m_maxCircle : 0;
		setup_match(l_playtype, round_limit);

		//Сѡ��׷�
		m_baseTimes = m_playtype.WuRenBaiFenBaseTimes();

		//Сѡ�2�Ƿ�Ϊ����
		m_is2ChangZhu = m_playtype.WuRenBaiFen2IsCommMain();

		//Сѡ��Ƿ�ʮ
		m_isPokertTen = m_playtype.WuRenBaiFen2IsPokerTen();

		//Сѡ��Ƿ���ר��������
		m_cheatAgainst = m_playtype.WuRenBaiFen2IsCheat();

		//�ɿ�
		m_isGanKouDi = m_playtype.WuRenBaiFenIsGanKouDi();

		// ���ƿ���
		m_nJiaoPaiOption = m_playtype.WuRenBaiFenGetJiaoPaiOption();

		// ѡ��
		m_bAnyScoreCanSelectMainColor = m_playtype.WuRenBaiFenGetAnyScoreCanSelectMainColor();

		if (m_playtype.WuRenBaiFen2IsCommMain())
		{
			m_gamelogic.SetCommMainValue(2);
		}
		m_video.FillDeskBaseInfo(m_desk ? m_desk->GetDeskId() : 0, 0, m_round_limit, l_playtype);
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
		LLOG_DEBUG("WuRenBaiFenGameHandler::MHSetDeskPlay() Run... deskId=[%d], play_user_count=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, play_user_count);

		if (!m_desk || !m_desk->m_vip)
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::MHSetDeskPlay() Error!!!!  m_desk or m_desk->m_vip is NULL");
			return;
		}

		m_player_count = play_user_count;

		//���÷���Ϊ��һ�����Ʒֵ����
		if (m_round_offset == 0)
		{
			m_firstSelectCardPos = m_desk->MHSpecPersonPos();
		}

		start_round(player_status);
	}

	/* ������Ҷ������� */
	void OnUserReconnect(User* pUser)
	{
		if (NULL == pUser || NULL == m_desk)
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::OnUserReconnect() Error!!! pUser or m_desk Is NULL...");
			return;
		}

		//Desk �Ѿ�����
		notify_desk_match_state();

		Lint pos = GetUserPos(pUser);

		if (WURENBAIFEN_INVALID_POS == pos && !(m_desk->MHIsLookonUser(pUser) || m_desk->m_clubOwerLookOnUser == pUser))
		{
			LLOG_ERROR("WuRenBaiFenGameHandler::OnUserReconnect() Error!!! Reconnect Pos is invial... desk_id=[%d], userId=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId());
			return;
		}

		LLOG_DEBUG("WuRenBaiFenGameHandler::OnUserReconnect() Run... deskId=[%d], userId=[%d], pos=[%d], curPos=[%d], zhuangPos=[%d], playStatus=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), pos, m_curPos, m_zhuangPos, m_play_status);


		WuRenBaiFenS2CReConnect reConnect;
		reConnect.m_pos = pos;
		reConnect.m_curPos = m_curPos;
		reConnect.m_zhuang = m_zhuangPos;
		reConnect.m_fuzhuangCard = m_selectFriendCard;
		reConnect.m_state = m_play_status;
		reConnect.m_selectMainColor = m_mainColor;
		reConnect.m_Monoy = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);// m_thisTurnScore;
		reConnect.m_max_select_score = m_max_select_score;
		if (pos == m_friendZhuangPos)
		{
			reConnect.m_fuzhuangPos = m_friendZhuangPos;
		}
		reConnect.m_leftover_score = m_leftover_score;
		reConnect.m_firstOutPos = m_firstOutPos;
		memcpy(reConnect.m_handCardsCountArr, m_hand_cards_count, sizeof(m_hand_cards_count));

		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
		{
			reConnect.m_score[i] = m_accum_score[i];
			reConnect.m_outCard[i] = m_out_cards[i];
			reConnect.m_prevOutCard[i] = m_prev_out_cards[i];
		}

		for (int i = 0; i < 12; i++)
		{
			reConnect.m_scoreCards[i] = m_scoreCards[i];
		}

		for (int i = 0; i < WURENBAIFEN_HAND_CARDS_COUNT_MAX && pos != WURENBAIFEN_INVALID_POS; i++)
		{
			reConnect.m_handCards[i] = m_hand_cards[reConnect.m_pos][i];
		}
		//�жϵ�����������ׯ��,��״̬��ѡ����,����ϵ���
		if ((m_play_status == GAME_PLAY_SELECT_MAIN || m_play_status == GAME_PLAY_BASE_CARDS)
			&& reConnect.m_pos == reConnect.m_zhuang)
		{
			for (Lint j = 0; j < WURENBAIFEN_BASE_CARDS_COUNT; ++j)
				reConnect.m_handCards[10 + j] = m_base_cards[j];
		}
		//ѡ����ͼ��뽻��״̬
		if (m_play_status == GAME_PLAY_SELECT_MAIN)
		{
			reConnect.m_nJiaoPaiState = m_jiaoPaiState;
			reConnect.m_nTimeLeftJiaoPaiAck = m_nTimeSecondXianResponseJiaoPaiLeft;
			reConnect.m_nTimeLeftJiaoPaiRequest = m_nTimeSecondZhuangReqesutJiaoPaiLeft;
			memcpy(reConnect.m_jiaopaiAgree, m_jiaoPai, sizeof(int) * WURENBAIFEN_PLAYER_COUNT);
		}
		for (int i = 0; i<WURENBAIFEN_BASE_CARDS_COUNT; i++)
		{
			reConnect.m_baseCards[i] = m_base_cards[i];
		}

		reConnect.m_playType[0] = m_playtype.WuRenBaiFenBaseTimes();
		reConnect.m_playType[1] = m_playtype.WuRenBaiFen2IsCommMain();
		reConnect.m_playType[2] = m_playtype.WuRenBaiFen2IsPokerTen();
		//reConnect.m_playType[3] = m_playtype.WuRenBaiFen2IsGPS();
		reConnect.m_playType[4] = m_playtype.WuRenBaiFen2IsCheat();

		if (m_play_status == GAME_PLAY_SELECT_SCORE)		//����ǽзֽ׶�,��m_score�����Ѿ��еķ���
		{
			for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
			{
				reConnect.m_score[i] = m_select_score[i];
			}
		}

		if (GAME_PLAY_OUT_CARD == m_play_status)
		{
			reConnect.m_currBig = GetHasOutPlayerCount() > 1 ? m_gamelogic.CompareOutCardArray(m_out_cards, m_firstOutPos, GetHasOutPlayerCount()) : WURENBAIFEN_INVALID_POS;
		}

		if (WURENBAIFEN_INVALID_POS != pos)
		{
			reConnect.m_hand_cards_count = m_hand_cards_count[reConnect.m_pos];
			notify_user(reConnect, reConnect.m_pos);
		}
		else if (m_desk->MHIsLookonUser(pUser))
		{
			//���͸���ս���
			pUser->Send(reConnect);
		}
		//�㲥�����ֲ��᳤���ƹ�ս
		else if (IsClubOwerLookOn())
		{
			memcpy(reConnect.m_clubAllHandCards, m_hand_cards, sizeof(m_hand_cards));
			notify_club_ower_lookon(reConnect);
		}

		//�ж�105��Ϣ
		// �ٷ���Ϸ������Ҫ�����Ӧ
		/*
		if (m_reset105[pos] != -2 && m_b105 && pos != WURENBAIFEN_INVALID_POS)	//��105��Ϣ  ��δѡ��
		{
			//������˾ܾ�,Ҫ������Ϸ,�򲻷�����ʾ105
			bool bState = true;
			for (int i = 0; i<WURENBAIFEN_PLAYER_COUNT; i++)
			{
				if (m_reset105[i] == 0)//����Ҿܾ�
				{
					bState = false;
					break;
				}

			}
			if (bState)
			{
				if (m_reset105[pos] == -1)  //�Ѿ�֪ͨ,δ����
				{
					//�㲥֪ͨ����Ƿ����ս��
					WuRenBaiFenS2CScore105 score105;
					score105.m_Score = 105;
					notify_user(score105, pos);
				}

				if (m_reset105[pos] == 1 || m_reset105[pos] == 0)  //�Ѿ�ͬ����Ѿ��ܾ� ,֪ͨ���
				{
					WuRenBaiFenS2CScore105Result score105Result;
					memcpy(score105Result.m_agree, m_reset105, sizeof(m_accum_score));
					notify_user(score105Result, pos);
				}
			}
		}
		*/

		//�ж��ǻ���С��׶�
		if (m_play_status == GAME_PLAY_SETTLEMENT)
		{
			//����С����Ϣ
			WuRenBaiFenS2CDrawResult rest;
			rest.m_zhuangPos = m_zhuangPos;
			rest.m_type = 0;
			rest.m_state = m_over_state;
			rest.m_fuzhuangPos = m_friendZhuangPos;
			rest.m_jiaofen = m_max_select_score;
			rest.m_defen = m_gamelogic.GetCardsScore(m_scoreCards, m_scoreCardsCount);;
			memcpy(rest.m_baseCards, m_base_cards, sizeof(Lint) * WURENBAIFEN_BASE_CARDS_COUNT);

			for (Lint i = 0; i < WURENBAIFEN_PLAYER_COUNT; ++i)
			{
				rest.m_score[i] = m_player_score[i];
			}
			notify_desk(rest);
		}

	}

	/* �����ɢ�ɹ���desk���ø÷��� */
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


	/*
	���˰ٷ֣�C->S ���ѡ����Ʒ�
	MSG_C_2_S_WURENBAIFEN_SELECT_SCORE = 62353
	*/
	bool HandleWuRenBaiFenSelectScore(User* pUser, WuRenBaiFenC2SSelectScore* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenSelectScore() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_SELECT_SCORE != m_play_status)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenSelectScore() Error!!! Current Play Status is not GAME_PLAY_SELECT_SCORE... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == WURENBAIFEN_INVALID_POS)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenSelectScore() Error!!! Select Score Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		//������ҽ��Ʒ�
		return on_event_user_select_score(GetUserPos(pUser), msg->m_selecScore);
	}

	/*
	���˰ٷ֣�C->S ���ѡ����
	MSG_C_2_S_WURENBAIFEN_SELECT_MAIN = 62355
	*/
	bool HandleWuRenBaiFenSelectMain(User* pUser, WuRenBaiFenC2SSelectMain* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenSelectMain() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_SELECT_MAIN != m_play_status)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenSelectMain() Error!!! Current Play Status is not GAME_PLAY_SELECT_MAIN... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == WURENBAIFEN_INVALID_POS)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenSelectMain() Error!!! Select Main Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		return on_event_user_select_main(GetUserPos(pUser), msg->m_selectMainColor);
		//
	}

	/*
	���˰ٷ֣�C->S ������
	MSG_C_2_S_WURENBAIFEN_BASE_CARD = 62357
	*/
	bool HandleWuRenBaiFenBaseCard(User* pUser, WuRenBaiFenC2SBaseCard* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenBaseCard() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_BASE_CARDS != m_play_status)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenBaseCard() Error!!! Current Play Status is not GAME_PLAY_BASE_CARDS... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == WURENBAIFEN_INVALID_POS)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenBaseCard() Error!!! Base Cards Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		//
		return on_event_user_base_cards(GetUserPos(pUser), msg->m_baseCards, msg->m_baseCardsCount);
	}

	/*
	���˰ٷ֣�C->S ���ѡ�Լ���
	MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND = 62359
	*/
	bool HandleWuRenBaiFenSelectFriend(User* pUser, WuRenBaiFenC2SSelectFriend* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenSelectFriend() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_SELECT_FRIEND != m_play_status)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenSelectFriend() Error!!! Current Play Status is not GAME_PLAY_SELECT_FRIEND... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == WURENBAIFEN_INVALID_POS)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenSelectFriend() Error!!! Select Friend Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		//
		return on_event_user_select_friend(GetUserPos(pUser), msg->m_selectFriendCard);
	}

	/*
	���˰ٷ֣�C->S ��ҳ���
	MSG_C_2_S_WURENBAIFEN_OUT_CARD = 62361
	*/
	bool HandleWuRenBaiFenOutCard(User* pUser, WuRenBaiFenC2SOutCard* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenOutCard() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_OUT_CARD != m_play_status)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenOutCard() Error!!! Current Play Status is not GAME_PLAY_OUT_CARD... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == WURENBAIFEN_INVALID_POS)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenOutCard() Error!!! Out Card Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		//
		return on_event_user_out_card(GetUserPos(pUser), msg->m_outCard);
	}

	/*
	���˰ٷ֣�C->S ��һظ��мҵ÷���105��,�Ƿ�ֹͣ��Ϸ
	MSG_C_2_S_WURENBAIFEN_SCORE_105_RET = 62366
	*/
	bool HandleWuRenBaiFenScore105Ret(User* pUser, WuRenBaiFenC2SScore105Ret* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenScore105Ret() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		if (GAME_PLAY_OUT_CARD != m_play_status)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenScore105Ret() Error!!! Current Play Status is not GAME_PLAY_OUT_CARD... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == WURENBAIFEN_INVALID_POS)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenScore105Ret() Error!!! Out Card Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		WuRenBaiFenS2CScore105Result score105Result;
		m_reset105[msg->m_pos] = msg->m_agree;
		memcpy(score105Result.m_agree, m_reset105, sizeof(m_accum_score));
		notify_desk(score105Result);

		int iRet = 1;
		for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
		{
			if (m_reset105[i] == 0)		//�ܾ�
			{
				iRet = 0;
				for (Lint n = 0; n < WURENBAIFEN_PLAYER_COUNT; ++n)
				{
					m_reset105[n] = -2;
				}
				break;
			}
			if (m_reset105[i] == -1)		//�ȴ���
			{
				iRet = 0;
				//break;
			}
		}
		if (iRet == 1) //ȫ��ͬ��
		{
			m_firstSelectCardPos = GetNextPos(m_firstSelectCardPos);				//������һ�ֵ�һ�����Ƶ���

			m_over_state = GAME_OVER_A;
			m_wTurnWinner = m_zhuangPos;
			finish_round();
		}
		
		return true;
	}

	/*
	���˰ٷ֣�C->S �ͻ���ׯ��ѡ����
	MSG_C_2_S_WURENBAIFEN_ZHUANG_JIAO_PAI = 62368
	*/
	bool HandleWuRenBaiFenZhuangRequestJiaoPai(User* pUser, WuRenBaiFenC2SZhuangJiaoPai* msg)
	{		 
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenZhuangRequestJiaoPai() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		MHLOG_PLAYCARD("ׯ�ҽ�����Ϣ desk: %d zhuangpos:%d code:%d", m_desk->GetDeskId(), m_zhuangPos, msg->m_agree);
		//����
		if (GAME_PLAY_SELECT_MAIN != m_play_status)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenZhuangRequestJiaoPai() Error!!! Current Play Status is not GAME_PLAY_SELECT_MAIN... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		Lint pos = GetUserPos(pUser);
		if (pos == WURENBAIFEN_INVALID_POS)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenZhuangRequestJiaoPai() Error!!! Out Card Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		bool bJiaoPai = false;
		if (msg->m_agree == 1) bJiaoPai = true;

		return on_event_zhuang_request_jiao_pai(pos, bJiaoPai);
	}

	/*
	C->S ��ׯ��  �ͻ���ѡ��ׯ�ҽ��Ƶ�����
	MSG_C_2_S_WURENBAIFEN_XIAN_SELECT_JIAO_PAI = 62370
	*/
	bool HandleWuRenBaiFenXianSelectJiaoPai(User* pUser, WuRenBaiFenC2SXianSelectJiaoPai* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenXianSelectJiaoPai() Error!!! pUser or msg m_desk Is NULL...");
			return false;
		}
		
		if (GAME_PLAY_SELECT_MAIN != m_play_status)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenXianSelectJiaoPai() Error!!! Current Play Status is not GAME_PLAY_SELECT_MAIN... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == WURENBAIFEN_INVALID_POS)
		{
			LLOG_ERROR("WuRenBaiFenHandler::HandleWuRenBaiFenXianSelectJiaoPai() Error!!! Out Card Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		return on_event_xian_select_jiao_pai(GetUserPos(pUser), msg->m_agree);
	}

	// ��齻��ʱ�����
	void Tick(LTime& curr)
	{		
		Lint t = 0;
		if ( (m_nJiaoPaiOption != 0) && (m_play_status == GAME_PLAY_SELECT_MAIN))
		{ 
			if (m_jiaoPaiState == JIAO_PAI_STATE::JIAOPAI_STATE_REQUESTED) //ׯ��������,   Ĭ��ͬ��
			{
				t = curr.Secs() - m_nTimeSecondXianResponseJiaoPai;
				if (t > GAME_DEFAULT_TIME::JIAOPAI_RESPONSE_TIMEOUT) // ����30��
				{
					m_nTimeSecondXianResponseJiaoPaiLeft = 0;
					for (int i = 0; i < WURENBAIFEN_PLAYER_COUNT; i++)
					{
						//ׯ�Լ����ü��
						if (i == m_zhuangPos) continue;
						if (m_jiaoPai[i] == -1)
						{
							MHLOG_PLAYCARD("��Ӧ���Ƴ�ʱ,�ܾ����� desk: %d zhuangpos:%d, userpos:%d", m_desk->GetDeskId(), m_zhuangPos, i);
							on_event_xian_select_jiao_pai(i, 0);
							break;
						}
					}
				}
				else
				{
					m_nTimeSecondXianResponseJiaoPaiLeft = GAME_DEFAULT_TIME::JIAOPAI_RESPONSE_TIMEOUT - t;
				}
			}
			else if (m_jiaoPaiState == JIAO_PAI_STATE::JIAOPAI_STATE_INIT) //��״̬�¼��ׯ�ҳ�ʱδѡ��Ĭ�ϲ�����
			{
				t = curr.Secs() - m_nTimeSecondZhuangRequstJiaoPai; // ������
				if (t > GAME_DEFAULT_TIME::JIAOPAI_REQUEST_TIMEOUT) // ����30��
				{
					m_nTimeSecondZhuangReqesutJiaoPaiLeft = 0;
					m_jiaoPaiState = JIAO_PAI_STATE::JIAOPAI_STATE_RQUEST_TIMEOUT;
					// ��ׯ�Ҽ�����Ϸ
					MHLOG_PLAYCARD("ׯ�ҽ�������ʱ,�ܾ����� desk: %d zhuangpos:%d", m_desk->GetDeskId(), m_zhuangPos);
					WuRenBaiFenS2CZhuangJiaoPaiBC bc;
					bc.m_jiaopaiState = m_jiaoPaiState;
					bc.m_jiaopaiRequest = 0;
					bc.m_zhuangPos = m_zhuangPos;
					notify_desk(bc);
				}
				else
				{
					m_nTimeSecondZhuangReqesutJiaoPaiLeft = GAME_DEFAULT_TIME::JIAOPAI_REQUEST_TIMEOUT - t;
				}			 
			}
		}
	}
};


DECLARE_GAME_HANDLER_CREATOR(110, WuRenBaiFenGameHandler);