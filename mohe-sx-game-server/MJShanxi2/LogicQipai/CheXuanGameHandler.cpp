#include "CheXuanGameLogic.h"
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
*  �����Debug����ʹ��
*  cbRandCard �����������
*  m_desk Desk����
*  conFileName �����ļ�
*  playerCount �������
*  handCardsCount �����������
*  cellPackCount ������
*/
static bool debugSendCards_CheXuan(Lint* cbRandCard, Desk* m_desk, const char* conFileName, int playerCount, int handCardsCount, int cellPackCount)
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

/* ������Ϸ�еĸ����׶Σ�ö�٣� */
enum GAME_PLAY_STATE
{
	GAME_PLAY_BEGIN = 0,			/* 0��Ϸ��ʼ */
	GAME_PLAY_QIBOBO,				/* 1�²��� */
	GAME_PLAY_ADD_MANGGUO,			/* 2��â�� */
	GAME_PLAY_ADD_MANGSCORE,		/* 3��âע */
	GAME_PLAY_SEND_CARD,			/* 4���� */
	GAME_PLAY_ADD_SCORE,			/* 5��ע */
	GAME_PLAY_CUO_PAI,				/* 6���� */
	GAME_PLAY_CHE_PAI,				/* 7���� */
	GAME_PLAY_END,					/* 8��Ϸ���� */
};

/* ��ע�������� */
enum ADD_SCORE_OPT
{
	ADD_OPT_INVALID = -1,			/* ��Ч���� */
	ADD_OPT_FIRST_QIAO = 0,			/* �ƣ����ҵ�һ����һ����ע(֮ǰΪ����) */
	ADD_OPT_GEN = 1,				/* �� */
	ADD_OPT_FA_ZHAO = 2,			/* ���У�֮ǰΪ�� */
	ADD_OPT_QIAO_BO = 3,			/* �ò� */
	ADD_OPT_XIU = 4,				/* �� */
	ADD_OPT_SHUAI = 5,				/* ˦ */
	ADD_OPT_SANHUA = 6,				/* ���� */
	ADD_OPT_MAX_INVALID				/* �����Ч���� */
};

/* ������Ϸ�е���Ч */
enum GAME_PLAY_SOUND
{

};

enum OVER_STATE
{

};

#define DEF_BASE_TIME 0
/* ����Ĭ�ϲ���ʱ�� */
enum GAME_DEFAULT_TIME
{
	DEF_TIME_INVALID = -1,
	DEF_TIME_QIBOBO = DEF_BASE_TIME + 60,		//�𲧲�
	DEF_TIME_OPTSCORE = DEF_BASE_TIME + 60,		//ǰ������ע
	DEF_TIME_3RD_OPTSCORE = DEF_BASE_TIME + 65,	//��������ע
	DEF_TIME_CUOPAI = DEF_BASE_TIME + 60,		//����
	DEF_TIME_CHEPAI = DEF_BASE_TIME + 60,		//����

	DEF_TIME_FINISH = DEF_BASE_TIME + 40,		//�Զ���һ��ʱ�䣬desk�ϵ�ʱ��Ҫͬ����

	DEF_TIME_COINS_LOW = 30,					//�������ֵ������ʾ�ļ��ʱ��
};

enum GAME_DEFAULT_TIME_FAST
{
	DEF_TIME_FAST_INVALID = -1,
	DEF_TIME_FAST_QIBOBO = DEF_BASE_TIME + 5,		//�𲧲�
	DEF_TIME_FAST_OPTSCORE = DEF_BASE_TIME + 10,		//ǰ������ע
	DEF_TIME_FAST_3RD_OPTSCORE = DEF_BASE_TIME + 15,	//��������ע
	DEF_TIME_FAST_CUOPAI = DEF_BASE_TIME + 5,		//����
	DEF_TIME_FAST_CHEPAI = DEF_BASE_TIME + 20,		//����

	DEF_TIME_FAST_FINISH = DEF_BASE_TIME + 3,		//�Զ���һ��ʱ�䣬desk�ϵ�ʱ��Ҫͬ����

	DEF_TIME_FAST_COINS_LOW = 30,					//�������ֵ������ʾ�ļ��ʱ��
};


/* ������ϷСѡ��(�ṹ��) */
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
	* ��������   CheXuanBaseTimes()
	* ������     ��ȡ�ͻ���ѡ��ĵ׷�
	* ��ϸ��     �׷ֱ�����1��1�֣�2��2�֣�3��3��
	* ���أ�     (Lint)�ͻ���ѡ��ĵ׷�
	********************************************************/
	Lint PlayType::CheXuanBaseTimes() const;

	/******************************************************
	* ��������   CheXuanDiPi()
	* ������     �ͻ���ѡ��ס�Ƥ
	* ��ϸ��     0��1/3     1��5/1(5/10)     2��5/2(5/20)		3��2/5
	* ���أ�     (Lint)�ͻ���ѡ��ĵ�Ƥ
	********************************************************/
	Lint PlayType::CheXuanDiPi() const;

	/******************************************************
	* ��������   CheXuanMaxBoBo()
	* ������     �ͻ�������������
	* ��ϸ��     500   1000   2000
	* ���أ�     (Lint)
	********************************************************/
	Lint PlayType::CheXuanMaxBoBo() const;

	/******************************************************
	* ��������   CheXuanPlayerCount()
	* ������     �ͻ��˹涨���������
	* ��ϸ��     2��2��    5:5��    8��8��
	* ���أ�     (Lint)
	********************************************************/
	Lint PlayType::CheXuanPlayerCount() const;

	/******************************************************
	* ��������   CheXuanDynamicStart()
	* ������     �Ƿ�̬����
	* ��ϸ��     0:��֧��   1��֧��
	* ���أ�     (bool)
	********************************************************/
	bool PlayType::CheXuanDynamicIn() const;

	/******************************************************
	* ��������   CheXuanIsZouMang()
	* ������     �Ƿ�������â
	* ��ϸ��     0������â    1����â
	* ���أ�     (bool)
	********************************************************/
	bool PlayType::CheXuanIsZouMang() const;

	/******************************************************
	* ��������   CheXuanIsXiuMang()
	* ������     �Ƿ�������â
	* ��ϸ��     0������â    1����â
	* ���أ�     (bool)
	********************************************************/
	bool PlayType::CheXuanIsXiuMang() const;

	/******************************************************
	* ��������   CheXuanIsShouShouMang()
	* ������     �Ƿ���������â
	* ��ϸ��     0��������â    1������â
	* ���أ�     (bool)
	********************************************************/
	bool PlayType::CheXuanIsShouShouMang() const;

	/******************************************************
	* ��������   CheXuanIsLianXuDaMangFanBei()
	* ������     ������â����
	* ��ϸ��     0��������    1������
	* ���أ�     (bool)
	********************************************************/
	bool PlayType::CheXuanIsLianXuDaMangFanBei() const;

	/******************************************************
	* ��������   CheXuanIsFanBeiFengDing()
	* ������     �Ƿ��������ⶥ
	* ��ϸ��     0�����ⶥ    1���ⶥ���ⶥ���٣�������
	* ���أ�     (Lint)�Ƿ�����ׯ�ҽ���
	********************************************************/
	bool  PlayType::CheXuanIsFanBeiFengDing() const;

	/******************************************************
	* ��������   CheXuanIsClearPool()
	* ������     �Ƿ�ÿ�����â����
	* ��ϸ��     0��������    1������
	* ���أ�     (Lint)�Ƿ���ÿ�����â����Ƥ��
	********************************************************/
	bool  PlayType::CheXuanIsClearPool() const;

	/******************************************************
	* ��������   CheXuanLookOnDarkResult()
	* ������     �Ƿ��ս���ƽ���
	* ��ϸ��     0����    1����
	* ���أ�     (Lint)
	********************************************************/
	bool  PlayType::CheXuanLookOnDarkResult() const;

	/******************************************************
	* ��������   CheXuanStartNotInRoom()
	* ������     �Ƿ񿪾ֺ��ֹ���뷿��
	* ��ϸ��     0����ʼ������    1����ʼ��������뷿��
	* ���أ�     (Lint)
	********************************************************/
	bool  PlayType::CheXuanStartNotInRoom() const;

	/******************************************************
	* ��������   CheXuanMoFen()
	* ������     ����Ĭ��ѡ��
	* ��ϸ��     0����Ĭ��    1��Ĭ��
	* ���أ�     (Lint)
	********************************************************/
	bool PlayType::CheXuanMoFen() const;

	/******************************************************
	* ��������   IsFirstAddBigMang()
	* ������     �����Ƿ��׼ҷִ���â��
	* ��ϸ��     0������ģʽ    1���׼ҷִ���â��
	* ���أ�     (Lint)
	********************************************************/
	bool PlayType::IsFirstAddBigMang() const;

	/******************************************************
	* ��������   IsAllowDiJiu()
	* ������     �����Ƿ�����ؾ���
	* ��ϸ��     0��������ؾ���    1������ؾ���
	* ���أ�     (Lint)
	********************************************************/
	bool PlayType::IsAllowDiJiu() const;

	/******************************************************
	* ��������   IsFastMode()
	* ������     �����Ƿ����ģʽ
	* ��ϸ��     0������ģʽ    1������ģʽ
	* ���أ�     (Lint)
	********************************************************/
	bool PlayType::IsFastMode() const;

	/******************************************************
	* ��������   IsQiPaiNotShowCards()
	* ������     �����Ƿ����ƺ�С���㲻��ʾ����
	* ��ϸ��     0������ģʽ    1�����ƺ�С���㲻��ʾ����
	* ���أ�     (Lint)
	********************************************************/
	bool PlayType::IsQiPaiNotShowCards() const;

	/******************************************************
	* ��������   IsLongXinFeiMaxScore()
	* ������     �����Ƿ�������ķηⶥ��
	* ��ϸ��     0��������   1������
	* ���أ�     (Lint)
	********************************************************/
	bool PlayType::IsLongXinFeiMaxScore() const;
};



/******************************************************
* ��������   CheXuanBaseTimes()
* ������     ��ȡ�ͻ���ѡ��ĵ׷�
* ��ϸ��     �׷ֱ�����1��1�֣�2��2�֣�3��3��
* ���أ�     (Lint)�ͻ���ѡ��ĵ׷�
********************************************************/
Lint PlayType::CheXuanBaseTimes() const
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
* ��������   CheXuanDiPi()
* ������     �ͻ���ѡ��ס�Ƥ
* ��ϸ��     0��1/3     1��5/1(5/10)     2��5/2(5/20)		3��2/5
* ���أ�     (Lint)�ͻ���ѡ��ĵ�Ƥ
********************************************************/
Lint PlayType::CheXuanDiPi() const
{
	if (m_playtype.size() < 2)
	{
		return false;
	}

	return m_playtype[1] % 4;
}

/******************************************************
* ��������   CheXuanMaxBoBo()
* ������     �ͻ�������������
* ��ϸ��     500   1000   2000   3000   5000
* ���أ�     (Lint)
********************************************************/
Lint PlayType::CheXuanMaxBoBo() const
{
	if (m_playtype.size() < 3)
	{
		return false;
	}

	return m_playtype[2];
}



/******************************************************
* ��������   CheXuanPlayerCount()
* ������     �ͻ��˹涨���������
* ��ϸ��     2��2��    5:5��    8��8��
* ���أ�     (Lint)
********************************************************/
Lint PlayType::CheXuanPlayerCount() const
{
	if (m_playtype.size() < 4)
	{
		return false;
	}

	return m_playtype[3];
}

/******************************************************
* ��������   CheXuanDynamicStart()
* ������     �Ƿ�̬����
* ��ϸ��      0:��֧��   1��֧��
* ���أ�     (bool)
********************************************************/
bool PlayType::CheXuanDynamicIn() const
{
	if (m_playtype.size() < 5)
	{
		return false;
	}

	return m_playtype[4];
}

/******************************************************
* ��������   CheXuanIsZouMang()
* ������     �Ƿ�������â
* ��ϸ��     0������â    1����â
* ���أ�     (bool)
********************************************************/
bool PlayType::CheXuanIsZouMang() const
{
	if (m_playtype.size() < 6)
	{
		return false;
	}

	return m_playtype[5] == 1;
}

/******************************************************
* ��������   CheXuanIsXiuMang()
* ������     �Ƿ�������â
* ��ϸ��     0������â    1����â
* ���أ�     (bool)
********************************************************/
bool PlayType::CheXuanIsXiuMang() const
{
	if (m_playtype.size() < 7)
	{
		return false;
	}

	return m_playtype[6] == 1;
}

/******************************************************
* ��������   CheXuanIsShouShouMang()
* ������     �Ƿ���������â
* ��ϸ��     0��������â    1������â
* ���أ�     (bool)
********************************************************/
bool PlayType::CheXuanIsShouShouMang() const
{
	if (m_playtype.size() < 8)
	{
		return 0;
	}
	return m_playtype[7] == 1;
}

/******************************************************
* ��������   CheXuanIsLianXuDaMangFanBei()
* ������     ������â����
* ��ϸ��     0��������    1������
* ���أ�     (bool)
********************************************************/
bool PlayType::CheXuanIsLianXuDaMangFanBei() const
{
	if (m_playtype.size() < 9)
	{
		return 0;
	}
	return m_playtype[8] == 1;
}

/******************************************************
* ��������   CheXuanIsFanBeiFengDing()
* ������     �Ƿ��������ⶥ
* ��ϸ��     0�����ⶥ    1���ⶥ���ⶥ���٣�������
* ���أ�     (Lint)�Ƿ�����ׯ�ҽ���
********************************************************/
bool  PlayType::CheXuanIsFanBeiFengDing() const
{
	if (m_playtype.size() < 10)
	{
		return 0;
	}
	return m_playtype[9] == 1;
}

/******************************************************
* ��������   CheXuanIsClearPool()
* ������     �Ƿ�ÿ�����â����
* ��ϸ��     0�������    1�����
* ���أ�     (Lint)�Ƿ���ÿ�����â����Ƥ��
********************************************************/
bool  PlayType::CheXuanIsClearPool() const
{
	if (m_playtype.size() < 11)
	{
		return 1;
	}
	return m_playtype[10] == 0;
}

/******************************************************
* ��������   CheXuanLookOnDarkResult()
* ������     �Ƿ��ս���ƽ���
* ��ϸ��     0����    1����
* ���أ�     (Lint)
********************************************************/
bool  PlayType::CheXuanLookOnDarkResult() const
{
	if (m_playtype.size() < 12)
	{
		return false;
	}
	return m_playtype[11] == 1;
}

/******************************************************
* ��������   CheXuanStartNotInRoom()
* ������     �Ƿ񿪾ֺ��ֹ���뷿��
* ��ϸ��     0����ʼ������    1����ʼ��������뷿��
* ���أ�     (Lint)
********************************************************/
bool  PlayType::CheXuanStartNotInRoom() const
{
	if (m_playtype.size() < 13)
	{
		return false;
	}

	return m_playtype[12] == 1;
}

/******************************************************
* ��������   CheXuanMoFen()
* ������     ����Ĭ��ѡ��
* ��ϸ��     0����Ĭ��    1��Ĭ��
* ���أ�     (Lint)
********************************************************/
bool PlayType::CheXuanMoFen() const
{
	if (m_playtype.size() < 14)
	{
		return false;
	}

	return m_playtype[13] == 1;
}

/******************************************************
* ��������   IsFirstAddBigMang()
* ������     �����Ƿ��׼ҷִ���â��
* ��ϸ��     0������ģʽ    1���׼ҷִ���â��
* ���أ�     (Lint)
********************************************************/
bool PlayType::IsFirstAddBigMang() const
{
	if (m_playtype.size() < 15)
	{
		return false;
	}

	return m_playtype[14] == 1;
}

/******************************************************
* ��������   IsAllowDiJiu()
* ������     �����Ƿ�����ؾ���
* ��ϸ��     0��������ؾ���    1������ؾ���
* ���أ�     (Lint)
********************************************************/
bool PlayType::IsAllowDiJiu() const
{
	if (m_playtype.size() < 16)
	{
		return false;
	}

	return m_playtype[15] == 1;
}

/******************************************************
* ��������   IsFastMode()
* ������     �����Ƿ����ģʽ
* ��ϸ��     0������ģʽ    1������ģʽ
* ���أ�     (Lint)
********************************************************/
bool PlayType::IsFastMode() const
{
	if (m_playtype.size() < 17)
	{
		return false;
	}

	return m_playtype[16] == 1;
}

/******************************************************
* ��������   IsQiPaiNotShowCards()
* ������     �����Ƿ����ƺ�С���㲻��ʾ����
* ��ϸ��     0������ģʽ    1�����ƺ�С���㲻��ʾ����
* ���أ�     (Lint)
********************************************************/
bool PlayType::IsQiPaiNotShowCards() const
{
	if (m_playtype.size() < 18)
	{
		return false;
	}

	return m_playtype[17] == 1;
}

/******************************************************
* ��������   IsLongXinFeiMaxScore()
* ������     �����Ƿ�������ķηⶥ��
* ��ϸ��     0��������   1������
* ���أ�     (Lint)
********************************************************/
bool PlayType::IsLongXinFeiMaxScore() const
{
	if (m_playtype.size() < 20)
	{
		return false;
	}

	return m_playtype[19] == 1;
}

/* ����ÿ�ֲ����ʼ���ֶΣ��ṹ�壩*/
struct CheXuanRoundState__c_part
{
	GAME_PLAY_STATE		m_play_status;										//�ƾ�״̬

	Lint				m_user_status[CHEXUAN_PLAYER_COUNT];				//���״̬��1������Ҳ�����Ϸ

	Lint				m_finish_round_type;								//���ֽ��������ͣ�0����������   1���ݹ���  2����â   3��һ��������������   4����â

	Lint				m_curPos;											//��ǰ�������
	Lint				m_dang_player_pos;									//����λ��
	Lint				m_turn_first_speak_pos;								//ÿ����ע�����е����λ�ã�ÿ�ַ�����ʼ��ҵ��¼�
	Lint				m_firstOutPos;										//ÿ�ֵ�һ���������
	Lint				m_qipai_last_pos;									//���һ��������ҵ�λ��
	BOOL				m_oneNotQiao;										//һ�˲���û���ã������˶�����  false������   true����
	BOOL				m_OnlyQiaoAndXiu;									//�þ�ֻ���ú��ݵĲ��������ݵ���ҷֵĸ�ƽ  false������   true:��
	Lint				m_allQiao;											//�����˶�����
	Lint				m_selectMoFen;										//ѡ��Ĭ��ѡ��  -1��Ĭ�ϳ�ʼֵ   0������  1������

	Lint				m_rest_remain_time;									//�����ɢ���붨ʱ���Ѿ�ִ�е�ʱ��  -1����ʼֵ
	BOOL				m_tick_flag;										//��ʱ������
	Lint				m_turn_count;										//�ڼ�����ע��0����������ʱ���һ����ע��  1����������ʱ��ڶ�����ע   2����������ʱ���������ע
	Lint				m_shuai_count;										//���������������������������

	Lint				m_hand_cards_count[CHEXUAN_PLAYER_COUNT];			//���������������
	Lint				m_hand_cards[CHEXUAN_PLAYER_COUNT][CHEXUAN_HAND_CARDS_COUNT];		//�������
	Lint				m_hand_cards_backup[CHEXUAN_PLAYER_COUNT][CHEXUAN_HAND_CARDS_COUNT];		//�������(ԭʼ��û�г����������)
	Lint				m_hand_cards_type[CHEXUAN_PLAYER_COUNT][2];			//������������

	Lint				m_player_score[CHEXUAN_PLAYER_COUNT];				//��ұ��ֵ÷�
	Lint				m_total_add_mang_score[CHEXUAN_PLAYER_COUNT];		//������������µ�â����
	Lint				m_total_add_score[CHEXUAN_PLAYER_COUNT];			//������ע���ܷ�
	Lint				m_qipai_score[CHEXUAN_PLAYER_COUNT];				//������Ƶ�ʱ���Ѿ��µķ���
	Lint				m_player_add_score[CHEXUAN_PLAYER_COUNT];			//�����ע�ķ��ۼƣ�ֻ�����ƺ���0(��ǰ�����ķ�)
	Lint				m_player_pi_score[CHEXUAN_PLAYER_COUNT];			//���Ӯ��Ƥ��
	Lint				m_player_mang_score[CHEXUAN_PLAYER_COUNT];			//���Ӯ��â����
	Lint				m_player_bobo_add_di_pi[CHEXUAN_PLAYER_COUNT];		//�𲧲���Ҫ���⿼�ǵĶ���Ĳ��֣���������һ����ע

	Lint				m_allow_opt_type[CHEXUAN_PLAYER_COUNT][10];			//ÿ����ҵ�ǰ����Ĳ�������
	Lint				m_allow_opt_min_score[CHEXUAN_PLAYER_COUNT][10];	//ÿ����ҵ�ǰ���������Ӧ����Сֵ
	Lint				m_allow_opt_max_score[CHEXUAN_PLAYER_COUNT][10];	//ÿ����ҵ�ǰ���������Ӧ�����ֵ

	Lint				m_turn_round_opt[CHEXUAN_PLAYER_COUNT];				//ÿһ����ÿһȦ��ע��־��0����Ҫ��ע  1��Ҫ��ע
	Lint				m_turn_opt_type[3][CHEXUAN_PLAYER_COUNT];			//һ����ע����������һ�β���
	Lint				m_turn_opt_score[3][CHEXUAN_PLAYER_COUNT];			//һ����ע����Ҳ������ܷ�
	Lint				m_turn_first_opt_type[3][CHEXUAN_PLAYER_COUNT];		//һ����ע���״���ע������
	Lint				m_turn_first_opt_score[3][CHEXUAN_PLAYER_COUNT];	//һ����ע���״���ע�ķ���
	Lint				m_player_last_opt[CHEXUAN_PLAYER_COUNT];			//�������һ�β�������

	Lint				m_is_need_add_bo[CHEXUAN_PLAYER_COUNT];				//�Ƿ���Ҫ�𲧷�  0������Ҫ  1����Ҫ

	Lint				m_qibobo_status[CHEXUAN_PLAYER_COUNT];				//ÿ������𲧲�״̬  0��δ����  1���Ѳ���
	Lint				m_xiu_status[CHEXUAN_PLAYER_COUNT];					//ÿ������ݵ�״̬ 0��δ��  1���Ѿ�ѡ����
	Lint				m_qipai_status[CHEXUAN_PLAYER_COUNT];				//ÿ������Ƿ����ƣ�0��δ����  1��������  2����������
	Lint				m_cuopai_status[CHEXUAN_PLAYER_COUNT];				//ÿ����Ҵ���״̬  0��δ����  1���Ѿ�����
	Lint				m_chepai_status[CHEXUAN_PLAYER_COUNT];				//ÿ���˳��Ʋ���״̬ 0��δ����  1���Ѿ����ƹ���
	Lint				m_qiao_status[CHEXUAN_PLAYER_COUNT];				//ÿ������õ�״̬  0��û����  1������

	Lint				m_san_hua_status[CHEXUAN_PLAYER_COUNT];				//�Ƿ�������
	Lint				m_select_san_hua_status[CHEXUAN_PLAYER_COUNT];		//ѡ��������״̬��0��û��ѡ��   1��ѡ������
	Lint				m_double_san_hua_status[CHEXUAN_PLAYER_COUNT];		//������״̬��  0������������   1��������

	Lint				m_min_qi_bo_score[CHEXUAN_PLAYER_COUNT];			//ÿ�����������𲧲�����Сֵ
	Lint				m_max_qi_bo_score[CHEXUAN_PLAYER_COUNT];			//ÿ�����������𲧲������ֵ

	Lint				m_first_shou_mang_time;								//�״η��Ƶ�����â�ӳ�ʱ��
	Lint				m_first_xiu_mang_time;								//�״η��Ƶ���â�ӳ�ʱ��
	Lint				m_first_zou_mang_time;								//�״η��Ƶ���â�ӳ�ʱ��

	Lint				m_real_player_count;								//����ʱ�������Ϸ��ʵ���������������Ѿ��������
	Lint				m_after_turn_shuai_delay;							//��������������ơ���������ʱ����һ���������¼ҷ��ƻ���һ����ʱʱ��
	bool				m_is_first_opt_fa_zhao;								//�Ƿ����˵�һ�β������״η�����Ҵ���â��ѡ�

	HandCardsTypeInfo	m_handCardsTypeInfo[CHEXUAN_PLAYER_COUNT];			//���������Ϣ���������Ľ��

	//�Զ�����ѡ��
	Lint				m_playerAutoOpt[CHEXUAN_PLAYER_COUNT];				//���ѡ���Զ�����ѡ��  0��δѡ��   1���Զ�����   2���Զ���������   3���Զ���


	/* �ṹ�����㺯�� */
	void clear_round()
	{
		memset(this, 0, sizeof(*this));
		m_dang_player_pos = CHEXUAN_INVALID_POS;
		m_turn_first_speak_pos = CHEXUAN_INVALID_POS;
		m_qipai_last_pos = CHEXUAN_INVALID_POS;
		m_rest_remain_time = -1;
		m_selectMoFen = -1;

		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < CHEXUAN_PLAYER_COUNT; ++j)
			{
				m_turn_opt_type[i][j] = ADD_OPT_INVALID;
			}
		}

		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			for (int j = 0; j < 10; ++j)
			{
				m_allow_opt_type[i][j] = -1;
			}
			m_player_last_opt[i] = -1;
		}
	}
};

/* ����ÿ�ֻ��ʼ���ֶΣ��ṹ�壩*/
struct CheXuanRoundState__cxx_part
{
	LTime m_play_status_time;
	LTime m_is_coins_low_time[CHEXUAN_PLAYER_COUNT];

	/* ����ÿ���ֶ� */
	void clear_round()
	{
		this->~CheXuanRoundState__cxx_part();
		new (this)CheXuanRoundState__cxx_part;
	}
};

/* ����ÿ�������ʼ�����ֶΣ��ṹ�壩*/
struct CheXuanMatchState__c_part
{
	Desk*		m_desk;									// ���Ӷ���
	Lint		m_round_offset;							// ��ǰ����
	Lint		m_round_limit;							// ��������ѡ���������
	Lint		m_accum_score[CHEXUAN_PLAYER_COUNT];	// �洢����ܵ÷�
	BOOL		m_dismissed;
	Lint		m_registered_game_type;
	Lint		m_player_count;							// �淨���Ĵ�����ʹ�õ���������ֶΣ�����Сѡ��ѡ���������仯
	Lint		m_zhuangPos;							// ׯ��λ��
	BOOL		m_isFirstFlag;							// �׾���ʾ��ʼ��ť,true�����׾�
	Lint		m_frist_send_card;						// ÿ�ֵ�һ�����Ƶ�λ��

	Lint		m_scoreBase;							//��С��ע����  1/3��3     5/1��10    5/2: 20
	Lint		m_scoreTimes;							//��ע�ӱ��ʷ֣�1/3��1     5/1��5    5/2:5

	Lint		m_per_pi_pool;							//��һ��Ƥ��
	Lint		m_per_mang_pool;						//��һ��â����

	Lint		m_last_get_yu_mang;						//��ɢ�����ƾֽ���������ҷֵ�â��ƽ�ֹ���ʣ����ֵ����λ��

	Lint		m_pi_pool;								//Ƥ��
	Lint		m_mang_pool;							//â����
	Lint		m_rest_can_start_bo_score[CHEXUAN_PLAYER_COUNT];  //ʣ����𲧲�������
	Lint		m_has_start_total_bo_score[CHEXUAN_PLAYER_COUNT];	//�Ѿ�����Ĳ�����
	Lint		m_last_bo_score[CHEXUAN_PLAYER_COUNT];	//ÿ����������Ĳ�����
	Lint		m_total_bo_score[CHEXUAN_PLAYER_COUNT];	//ÿ����ҵ�ǰ����
	Lint		m_total_bo_score_after_bobo[CHEXUAN_PLAYER_COUNT];	//ÿ���𲧲������Ҳ������ܷ��������ڼ�¼����Ӯ���ٷ�
	Lint		m_is_coins_low[CHEXUAN_PLAYER_COUNT];	//�������ֵ����Ԥ��  0������ֵ����  1������ֵ����Ԥ��

	Lint		m_mangScore;							//äע
	Lint		m_commonMangCount;
	Lint		m_xiuMangCount;

	Lint		m_is_need_add_xiuMang[CHEXUAN_PLAYER_COUNT];		//�Ƿ���Ҫ����â��  0������Ҫ  1����Ҫ
	Lint		m_zouMangCount;
	Lint		m_is_need_add_zouMang[CHEXUAN_PLAYER_COUNT];		//�Ƿ���Ҫ����â��  0������Ҫ  1����Ҫ
	Lint		m_shouMangCount;
	Lint		m_is_need_add_shouMang[CHEXUAN_PLAYER_COUNT];	//�Ƿ���Ҫ������â  0������Ҫ  1����Ҫ

	//վ����
	Lint		m_player_stand_up[CHEXUAN_PLAYER_COUNT];		//���վ�����±�־  0��û�в���(Ĭ������״̬)   1�����ֵ��վ��(���ֻ�δվ��)    2���Ѿ�վ��״̬     3�����ֵ�����£�����δ���£�

	//�ƾ���ս����¼
	Lint		m_has_record_bobo[CHEXUAN_PLAYER_COUNT];		//�Ƿ��Ѿ���ֵ����������  0��û�и�ֵ��   1���Ѿ���ֵ����
	Lint		m_record_bobo_score[CHEXUAN_PLAYER_COUNT];		//���������𲧲��� ���������ܻ�仯���Ǿ���������仯

	Lint		m_tui_mang_score[CHEXUAN_PLAYER_COUNT];			//â������գ��������ʱ�˵�â��
	Lint		m_tui_pi_score[CHEXUAN_PLAYER_COUNT];			//Ƥ����գ��������ʱ�˵�Ƥ

	//�����׶���ʱʱ��
	Lint		m_def_time_qibobo;						//�𲧲�
	Lint		m_def_time_optscore;					//ǰ������ע
	Lint		m_def_time_3rd_optscore;				//��������ע
	Lint		m_def_time_cuopai;						//����
	Lint		m_def_time_chepai;						//����
	Lint		m_def_time_finish;						//�Զ���һ��ʱ�䣬desk�ϵ�ʱ��Ҫͬ����


	////�淨Сѡ��
	Lint		m_baseTimes;							//�׷ֱ���
	Lint		m_diPi;									//�ס�Ƥ  0��1 / 3     1��5 / 1(5 / 10)     2��5 / 2(5 / 20)		3��2/5
	Lint		m_maxBoBo;								//����������  0:500   1:1000   2:2000
	Lint		m_playCount;							//�ͻ���ѡ��������������   2��2��    5:5��    8��8��
	BOOL		m_isDynamicIn;							//�Ƿ�����̬����  true:����   false:������
	BOOL		m_isZouMang;							//�Ƿ�������â
	BOOL		m_isXiuMang;							//�Ƿ�������â
	BOOL		m_isShouMang;							//�Ƿ���������â
	BOOL		m_isDaMangFanBei;						//�Ƿ������â����
	BOOL 		m_isFanBeiFengDing;						//�Ƿ������Ƿ񷭱��ⶥ  0:�ⶥ  1�����ⶥ
	Lint		m_isClearPool;							//�Ƿ�����ÿ�����Ƥ��â���� 0�������   1�����
	Lint		m_isLookOnDarkResult;					//�Ƿ��ս���ƽ�ɢ  0����   1����ս���ƽ���
	Lint		m_isStartNotInRoom;						//�Ƿ���Ϸ��ʼ��ֹ���뷿��  0��������  1����Ϸ��ʼ���ֹ����
	bool		m_isMoFen;								//�Ƿ���Ĭ�� false:������   true:����
	bool		m_isFirstAddBigMang;					//�Ƿ��׼ҷִ���â��  0������   1���׼Ҵ���â��
	bool		m_isAllowDiJiu;							//�Ƿ�����ؾ���  0��������ؾ���  1������ؾ���
	bool		m_isFastMode;							//�Ƿ�ʹ�ü���ģʽ  0������ģʽ  1������ģʽ
	bool		m_isQiPaiNotShowCards;					//�Ƿ����ƺ�С���㲻��ʾ����  0������ģʽ    1�����ƺ�С���㲻��ʾ����
	bool		m_isLongXinFeiMaxScore;					//�Ƿ������ķ����������ѡ��  0��������   1������

	/* ����ṹ���ֶ� */
	void clear_match()
	{
		memset(this, 0, sizeof(*this));
		m_baseTimes = 1;
		m_playCount = CHEXUAN_PLAYER_COUNT;
		m_last_get_yu_mang = CHEXUAN_INVALID_POS;
	}
};

/* ����ÿ�����ʼ�����ֶΣ��ṹ�壩*/
struct CheXuanMatchState__cxx_part
{
	CXGameLogic   m_gamelogic;     // ��Ϸ�߼�
	PlayType      m_playtype;	    // �����淨Сѡ��

									/* ���ÿ���ṹ�� */
	void clear_match()
	{
		this->~CheXuanMatchState__cxx_part();
		new (this) CheXuanMatchState__cxx_part;
	}
};

/*
*  ����ÿ������Ҫ�������ֶΣ��ṹ�壩
*  �̳� ��CheXuanRoundState__c_pard, CheXuanRoundState_cxx_part
*/
struct CheXuanRoundState : CheXuanRoundState__c_part, CheXuanRoundState__cxx_part
{
	void clear_round()
	{
		CheXuanRoundState__c_part::clear_round();
		CheXuanRoundState__cxx_part::clear_round();
	}
};

/*
*  ����ÿ������Ҫ�������ֶΣ��ṹ�壩
*  �̳У�CheXuanMatchState__c_pard, CheXuanMatchState_cxx_pard
*/
struct CheXuanMatchState : CheXuanMatchState__c_part, CheXuanMatchState__cxx_part
{
	void clear_match()
	{
		CheXuanMatchState__c_part::clear_match();
		CheXuanMatchState__cxx_part::clear_match();
	}
};

/*
*  ��������״̬���ṹ�壩
*  �̳У�CheXuanRoundState, CheXuanMatchState
*/
struct CheXuanDeskState : CheXuanRoundState, CheXuanMatchState
{
	/* ����ÿ������ */
	void clear_round()
	{
		CheXuanRoundState::clear_round();
	}

	/* ����ÿ������*/
	void clear_match(int player_count)
	{
		CheXuanMatchState::clear_match();
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
			return CHEXUAN_INVALID_POS;
		}

		Lint pos = CHEXUAN_INVALID_POS;
		if (m_desk)
		{
			pos = m_desk->GetUserPos(pUser);
		}
		if (pos >= CHEXUAN_INVALID_POS)
		{
			pos = CHEXUAN_INVALID_POS;
		}
		return pos;
	}

	/* ��ȡ��һ����Чλ�� */
	Lint GetPrePos(Lint pos)
	{
		if (m_player_count == 0)
		{
			return CHEXUAN_INVALID_POS; // ����������
		}
		Lint t_prePos = pos % m_player_count;
		do
		{
			t_prePos = (t_prePos + m_player_count - 1) % m_player_count;
		} while (m_user_status[t_prePos] != 1 || m_qipai_status[t_prePos] != 0);

		return t_prePos;
	}

	/* ��ȡ��һ��λ��(���˾���) */
	Lint GetRealPrePos(Lint pos)
	{
		if (m_player_count == 0)
		{
			return CHEXUAN_INVALID_POS; // ����������
		}
		Lint t_prePos = pos % m_player_count;
		do
		{
			t_prePos = (t_prePos + m_player_count - 1) % m_player_count;
		} while (m_user_status[t_prePos] != 1);

		return t_prePos;
	}


	/* ��ȡ��һ����Чλ�� */
	Lint GetNextPos(Lint pos)
	{
		if (m_player_count == 0)
		{
			return CHEXUAN_INVALID_POS;
		}

		Lint t_nextPos = pos % m_player_count;
		do
		{
			t_nextPos = (t_nextPos + 1) % m_player_count;
			LLOG_DEBUG("GetNextPos() do...  pos=[%d], status=[%d], qiPai=[%d]", t_nextPos, m_user_status[t_nextPos], m_qipai_status[t_nextPos]);
		} while (m_user_status[t_nextPos] != 1 || m_qipai_status[t_nextPos] != 0);

		return t_nextPos;
	}
	
	/* ��ȡ��һ���˵�λ�� */
	Lint GetRealNextPos(Lint pos)
	{
		if (m_player_count == 0)
		{
			return CHEXUAN_INVALID_POS;
		}

		Lint t_nextPos = pos % m_player_count;
		do
		{
			t_nextPos = (t_nextPos + 1) % m_player_count;
			LLOG_DEBUG("GetRealNextPos() do...  pos=[%d], status=[%d], qiPai=[%d]", t_nextPos, m_user_status[t_nextPos], m_qipai_status[t_nextPos]);
		} while (m_user_status[t_nextPos] != 1);

		return t_nextPos;
	}

	/* �жϸ������ע�Ƿ�Ϊ�� */
	Lint CheckIsTuo(Lint pos)
	{
		if (m_player_count == 0)
		{
			return 0; // ����������
		}

		Lint t_tuoPrePos = CHEXUAN_INVALID_POS;
		Lint t_tmpPos = pos % m_player_count;

		for (int i = pos; i < pos + m_player_count; ++i)
		{
			t_tmpPos = (t_tmpPos + m_player_count - 1) % m_player_count;

			if (m_user_status[t_tmpPos] != 1) continue;

			if (t_tmpPos == m_turn_first_speak_pos && m_turn_opt_score[m_turn_count][t_tmpPos] == 0)
			{
				break;
			}

			if (m_turn_opt_score[m_turn_count][t_tmpPos] != 0)
			{
				t_tuoPrePos = t_tmpPos;
				break;
			}
		}

		if (t_tuoPrePos != CHEXUAN_INVALID_POS && m_turn_opt_score[m_turn_count][pos] >= m_turn_opt_score[m_turn_count][t_tuoPrePos] * 2)
		{
			return 1;
		}

		return 0;
	}

	/* �ж�λ���Ƿ�Ϊ��Чλ�� */
	bool is_pos_valid(Lint pos)
	{
		if (NULL == m_desk)
		{
			return false;
		}
		return 0 <= pos && pos < CHEXUAN_INVALID_POS;
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
};

/* ����¼����(�ṹ��) */
struct CheXuanVideoSupport : CheXuanDeskState
{
	QiPaiVideoLog m_video; //¼��

	void VideoSave()
	{
		if (NULL == m_desk)
			return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		m_video.m_deskId = m_desk->GetDeskId();
		m_video.m_playType = m_desk->getPlayType();
		m_video.m_flag = m_desk->m_state;
		m_video.m_maxCircle = m_round_offset;

		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		LLOG_DEBUG("videoSave[%s]", video.m_sql.c_str());
		gWork.SendMsgToDb(video);
	}

	// oper ����  0--��  1--����   λ��   ���Ƹ���   ���� �������Ƹ���  ��������  �÷�
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

/* ������Ϸ���崦���߼����ṹ�壩*/
struct CheXuanGameCore : GameHandler, CheXuanVideoSupport
{
	//֪ͨ���������
	virtual void notify_user(LMsg &msg, int pos) {}

	//�㲥�������������������
	virtual void notify_desk(LMsg &msg) {}

	//�㲥����ǰ�������
	virtual void notify_desk_playing_user(LMsg &msg) {}

	//�㲥����ǰ��������û�в�����Ϸ���
	virtual void notify_desk_seat_no_playing_user(LMsg &msg) {}

	//�㲥��δ�����Ĺ�ս���
	virtual void notify_lookon(LMsg &msg) {}

	//
	virtual void notify_desk_seating_user(LMsg &msg) {}

	//
	virtual void notify_desk_without_user(LMsg &msg, User* pUser) {}

	/*
	virtual void notify_club_ower_lookon(LMsg& msg) {}
	*/

	/* �㲥��Ϸ���������ϵ�������� */
	void notify_desk_match_state()
	{
		LMsgS2CVipInfo msg;
		msg.m_curCircle = m_round_offset + 1;
		msg.m_curMaxCircle = m_round_limit;
		notify_desk(msg);
		notify_lookon(msg);
		//notify_club_ower_lookon(msg);
	}

	/* 
	  ÿ�ֽ���������Ӯ���������������������ֵ
	*/
	void update_vip_player_coins()
	{
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->UpdatePlayerCoins(m_player_score, m_player_count);
		}
	}

	/* �ж��Ƿ��������� */
	bool check_has_player_qiao()
	{
		bool t_flag = false;
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_qiao_status[i] == 1)
			{
				t_flag = true;
				break;
			}
		}
		return t_flag;
	}

	//�жϲ����Ƿ����
	bool has_opt_over(GAME_PLAY_STATE playState)
	{
		switch (playState)
		{
		case GAME_PLAY_BEGIN:
			break;

		//�𲧲��׶�
		case GAME_PLAY_QIBOBO:
		{
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_qibobo_status[i] != 1 && m_user_status[i] == 1)
				{
					return false;
				}
			}
			return true;
			break;
		}

		//��ע�׶�
		case GAME_PLAY_ADD_SCORE:
		{
			//�ܹ���ע���ֵ
			Lint t_totalMaxAddScore = 0;

			//���������עֵ
			Lint t_maxAddSocre = 0;

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_total_add_score[i] > t_totalMaxAddScore)
				{
					t_totalMaxAddScore = m_total_add_score[i];
				}
				if (m_turn_opt_score[m_turn_count][i] > t_maxAddSocre)
				{
					t_maxAddSocre = m_turn_opt_score[m_turn_count][i];
				}
			}
			if (t_maxAddSocre < 0 || t_totalMaxAddScore < 0)
			{
				return false;
			}

			int t_playerCount = 0;
			int t_qipaiCount = 0;
			int t_xiuCount = 0;
			int t_qiaoCount = 0;
			bool t_flag = true;

			//ÿ����Ч�����ע���ͺ���ע���������ֵ���
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1) ++t_playerCount;
				if (m_qipai_status[i] != 0) ++t_qipaiCount;
				else if (m_xiu_status[i] == 1) ++t_xiuCount;
				else if (m_qiao_status[i] != 0) ++t_qiaoCount;

				//���һ���������֮ǰ�����ȫ�����ƣ��������һ����Ҳ����ˣ��жϽ���������Ϊ��â
				if (t_playerCount - t_qipaiCount == 1 && i == m_player_count - 1)
				{
					return true;
				}
				//������Ҷ�����
				else if (t_qipaiCount + t_xiuCount == t_playerCount && i == m_player_count - 1)
				{
					return true;
				}
				/*else if (t_qiaoCount + t_xiuCount == t_playerCount && i == m_player_count - 1)
				{
					return true;
				}*/
				

				//ֻ��һ����û���ã��Ҳ�������
				else if ((t_playerCount - t_qipaiCount - t_qiaoCount) == 1 && i == m_player_count - 1)
				{
					Lint t_noQiaoPos = CHEXUAN_INVALID_POS;
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_user_status[i] == 1 && m_qipai_status[i] == 0 && m_qiao_status[i] == 0)
						{
							t_noQiaoPos = i;
							break;
						}
					}

					//if (t_noQiaoPos != CHEXUAN_INVALID_POS && m_turn_opt_score[m_turn_count][t_noQiaoPos] >= t_maxAddSocre &&
					if (t_noQiaoPos != CHEXUAN_INVALID_POS && m_total_add_score[t_noQiaoPos] >= t_totalMaxAddScore &&
						(m_turn_opt_type[m_turn_count][t_noQiaoPos] == ADD_OPT_FIRST_QIAO ||
							m_turn_opt_type[m_turn_count][t_noQiaoPos] == ADD_OPT_GEN ||
							m_turn_opt_type[m_turn_count][t_noQiaoPos] == ADD_OPT_FA_ZHAO)
						)
					{
						//���ñ��ֻ��һ�˲���û���ã������˶�����
						m_oneNotQiao = true;
						return true;
					}
				}

				//���ֲ���ֻ���ú��ݣ����ݵ���ҷ����Ѿ���ƽ
				else if (t_qiaoCount + t_xiuCount + t_qipaiCount == t_playerCount && t_qiaoCount > 0 && t_xiuCount > 0 && i == m_player_count - 1)
				{
					m_OnlyQiaoAndXiu = true;
					for (Lint j = 0; j < m_player_count; ++j)
					{
						if (m_user_status[j] != 1 || m_qiao_status[j] == 1 || m_qipai_status[j] != 0) continue;

						if (m_total_add_score[j] != t_totalMaxAddScore)
						{
							m_OnlyQiaoAndXiu = false;
							return false;
						}
					}
					return true;
				}


				if (m_user_status[i] != 1 || m_qipai_status[i] != 0)
				{
					continue;
				}

				//����Ƿ��������
				bool t_isCanSanHua = false;
				bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[i], m_hand_cards_count[i]);
				bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[i], m_hand_cards_count[i]);
				t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[i];

				if (
					!(
					(m_turn_opt_type[m_turn_count][i] == ADD_OPT_FIRST_QIAO && m_turn_opt_score[m_turn_count][i] == t_maxAddSocre) ||
						(m_turn_opt_type[m_turn_count][i] == ADD_OPT_GEN && m_turn_opt_score[m_turn_count][i] == t_maxAddSocre) ||
						(m_turn_opt_type[m_turn_count][i] == ADD_OPT_FA_ZHAO && m_turn_opt_score[m_turn_count][i] == t_maxAddSocre) ||
						m_turn_opt_type[m_turn_count][i] == ADD_OPT_QIAO_BO ||
						m_turn_opt_type[m_turn_count][i] == ADD_OPT_SHUAI ||
						m_turn_opt_type[m_turn_count][i] == ADD_OPT_SANHUA ||
						(m_qiao_status[i] != 0 && !t_isCanSanHua)
						)
					)
				{
					t_flag = false;
				}
			}
			return t_flag;
			break;
		}
		//����
		case GAME_PLAY_CHE_PAI:
		{
			int t_playerCount = 0;
			int t_chePaiCount = 0;
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_qipai_status[i] == 0) ++t_playerCount;
				if (m_chepai_status[i] == 1) ++t_chePaiCount;
			}
			if (t_playerCount == t_chePaiCount) return true;
			else return false;
			break;
		}
		//����
		case GAME_PLAY_CUO_PAI:
		{
			int t_playerCount = 0;
			int t_cuoPaiCount = 0;
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_qipai_status[i] == 0) ++t_playerCount;
				if (m_cuopai_status[i] == 1) ++t_cuoPaiCount;
			}
			if (t_playerCount <= t_cuoPaiCount) return true;
			else return false;
			break;
		}

		case GAME_PLAY_END:
			break;
		default:
			break;
		}

		return false;
	}

	/*
	�ж��Ƿ�Ϊ��â
	������Ҷ��ݣ�����������һ���ݣ�����ȫ�������ƣ���Ϊ��â
	*/
	bool is_xiu_mang()
	{
		Lint t_playerCount = 0;
		Lint t_optShuaiCount = 0;
		Lint t_optXiuCount = 0;

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			if (m_user_status[i] == 1)
			{
				++t_playerCount;
			}

			if (m_turn_opt_type[m_turn_count][i] == ADD_OPT_XIU)
			{
				++t_optXiuCount;
			}

			if (m_qipai_status[i] != 0)
			{
				++t_optShuaiCount;
			}
		}
		return t_playerCount == (t_optXiuCount + t_optShuaiCount);
	}

	/*
	�ж��Ƿ���â
	1�˳��ݡ���������Ĳ����������˶�����
	*/
	bool is_zou_mang()
	{
		Lint t_playerCount = 0;
		Lint t_optShuaiCount = 0;
		Lint t_optXiuCount = 0;

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			if (m_user_status[i] == 1)
			{
				++t_playerCount;
			}

			if (m_qipai_status[i] != 0)
			{
				++t_optShuaiCount;
			}

			if (m_turn_opt_type[m_turn_count][i] == ADD_OPT_XIU)
			{
				++t_optXiuCount;
			}

		}

		if (t_playerCount - t_optShuaiCount == 1 && t_optXiuCount == 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	/* �������ֵ, �������ֵ */
	Lint find_max_score(Lint arr[], Lint arrSize, Lint* maxScoreIndex)
	{
		Lint t_index = 0;
		Lint t_max = -1;
		if (arr == NULL || arrSize < 1)
		{
			return -1;
		}

		for (int i = 0; i < m_player_count; ++i)
		{
			if (arr[i] > t_max)
			{
				t_index = i;
				t_max = arr[i];
			}
		}
		*maxScoreIndex = t_index;
		return t_max;
	}

	/* ���㵱ǰ��ҿɲ����������Լ�����ֵ��Χ */
	void calc_opt_score()
	{
		//�ϼ�λ��
		Lint t_prePos = GetPrePos(m_curPos);

		//�ϸ����˵�λ��
		Lint t_preRealPos = GetRealPrePos(m_curPos);

		//�¼�λ��
		//Lint t_nextPos = GetNextPos(m_curPos);

		//�Ƿ�Ϊ��һ����ע,  true:��һ��   false:�ڶ�����
		bool t_isFirstTurn = (m_turn_count == 0);

		//�Ƿ�Ϊ���ң���һ����ע���׸�˵���˵�λ��
		bool t_isDangJia = (m_curPos == m_dang_player_pos && m_turn_count == 0);

		//�Ƿ�Ϊ��һ��˵����λ��
		bool t_isFirstSpeaker = (m_curPos == m_turn_first_speak_pos);

		//�����Ƿ�Ϊ���ֵ�һ�β���
		bool t_isTurnFirstOpt = (m_turn_opt_type[m_turn_count][m_curPos] == ADD_OPT_INVALID);

		//�����Ƿ�ѡ��Ĭ���еĿ��ƣ�����ֻ�ܲ�����
		bool t_isDangPosMoLookCard = (m_isMoFen && t_isDangJia && m_selectMoFen == 1);

		//��ǰ�����ע���������λ��
		Lint t_currMaxIndex = CHEXUAN_INVALID_POS;

		//��ǰ��ע��������
		Lint t_currMaxScore = find_max_score(m_turn_opt_score[m_turn_count], m_player_count, &t_currMaxIndex);

		//��ǰ����������С��ע����
		Lint t_minScore = t_currMaxScore <= 0 ? m_scoreBase : t_currMaxScore;		//Lint t_minScore = m_scoreBase > t_currMaxScore ? m_scoreBase : t_currMaxScore;

		//��ʱλ�ñ���
		Lint t_tmpPos = m_curPos;

		//�����Ƿ�����ƣ�֮ǰΪ�����У������ҵ�һ�ֵ�һ����ע
		bool t_isEnoughFirstQiao = m_total_bo_score[m_curPos] >= (m_scoreBase - m_mangScore);
		bool t_isCanFirstQiao = t_isEnoughFirstQiao && t_isDangJia && t_isTurnFirstOpt;
		Lint t_minFirstQiaoScore = t_isCanFirstQiao ? (m_scoreBase - m_mangScore) : 0;

		//����Ƿ���Ը���֮ǰΪ���ƣ�������ÿ����ע��һ��˵���˵ĵ�һ�β���λ��
		bool t_isEnoughGen = m_total_bo_score[m_curPos] > (t_minScore - m_turn_opt_score[m_turn_count][m_curPos]);
		bool t_isCanOptGen = m_turn_opt_type[m_turn_count][m_curPos] == ADD_OPT_INVALID ? false : true;
		t_tmpPos = m_turn_first_speak_pos;
		for (Lint i = m_turn_first_speak_pos; i < m_turn_first_speak_pos + m_player_count && !t_isCanOptGen; ++i)
		{
			t_tmpPos = i % m_player_count;

			if (m_user_status[t_tmpPos] != 1) continue;
			
			if (m_curPos == m_turn_first_speak_pos)
			{
				t_isCanOptGen = false;
				break;
			}
			else if (t_tmpPos == m_curPos)
			{
				break;
			}
			else if (m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_SANHUA &&
				m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_SHUAI &&
				m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_XIU &&
				m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_INVALID)
			{
				t_isCanOptGen = true;
				break;
			}
		}
		bool t_isCanGen = t_isEnoughGen && !(t_isFirstSpeaker && t_isTurnFirstOpt) && t_isCanOptGen;	//bool t_isCanGen = t_isEnoughGen && !(t_isFirstSpeaker && t_isTurnFirstOpt);
		Lint t_minGenScore = t_isCanGen ? t_minScore - m_turn_opt_score[m_turn_count][m_curPos] : 0;

		//����Ƿ���Է��У�֮ǰΪ���󣩣����ǵ�һ�ֵ�һ��˵��λ�õ��״β���
		bool t_isEnoughFaZhao = false;
		bool t_isCanFaZhao = false;
		Lint t_minFaZhaoScore = 0;
		//���ҵ�һ�ֵ�һ�β������Դ�
		if (t_isDangJia && t_isTurnFirstOpt)
		{
			if (m_diPi == 0)
			{
				t_minFaZhaoScore = 10;
			}
			else if (m_diPi == 1)
			{
				t_minFaZhaoScore = 30;
			}
			else if (m_diPi == 2)
			{
				t_minFaZhaoScore = 50;
			}
			else if (m_diPi == 3)
			{
				t_minFaZhaoScore = 10;
			}

			//ѡ���״η��д���â�� && ��һ�β������� && â���ش���0
			if (m_isFirstAddBigMang && !m_is_first_opt_fa_zhao && m_mang_pool > 0)
			{
				t_minFaZhaoScore = m_mang_pool;
			}

			t_isEnoughFaZhao = m_total_bo_score[m_curPos] >= t_minFaZhaoScore;
			t_isCanFaZhao = t_isEnoughFaZhao && !t_isDangPosMoLookCard;
			t_minFaZhaoScore = t_isCanFaZhao ? t_minFaZhaoScore : 0;
		}
		//ÿ�ֵ�һ��˵����Ҳ������Բ����󣬴����Сֵ����Ϊ�Ͼ�ƽ�ֵ�2��
		else if (!t_isFirstTurn && t_isFirstSpeaker && t_isTurnFirstOpt)
		{
			//�Ͼ�ƽ��
			Lint t_prePingScore = 0;
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				//�޸Ĵ���·ַ�����Ren:2019-02-25��
				if (m_total_add_score[i] > t_prePingScore)
				{
				t_prePingScore = m_total_add_score[i];
				}
			}

			t_minFaZhaoScore = 2 * t_prePingScore;
			if (m_diPi == 0 && t_minFaZhaoScore < 10)
			{
				t_minFaZhaoScore = 10;
			}
			else if (m_diPi == 1 && t_minFaZhaoScore < 30)
			{
				t_minFaZhaoScore = 30;
			}
			else if (m_diPi == 2 && t_minFaZhaoScore < 50)
			{
				t_minFaZhaoScore = 50;
			}
			else if (m_diPi == 3 && t_minFaZhaoScore < 10)
			{
				t_minFaZhaoScore = 10;
			}

			//ѡ���״η��д���â�� && ��һ�β������� && â���ش���0
			if (m_isFirstAddBigMang && m_mang_pool > 0)
			{
				if (!m_is_first_opt_fa_zhao)
				{
					t_minFaZhaoScore = m_mang_pool;
				}
				else
				{
					t_minFaZhaoScore = 2 * t_prePingScore;
				}
			}
			/*
			if (m_isFirstAddBigMang && !m_is_first_opt_fa_zhao && m_mang_pool > 0)
			{
				t_minFaZhaoScore = m_mang_pool;
			}
			else if (m_isFirstAddBigMang && m_is_first_opt_fa_zhao)
			{
				t_minFaZhaoScore = 2 * t_prePingScore;
			}
			*/

			//t_isEnoughFaZhao = m_total_bo_score[m_curPos] >= (t_prePingScore * 2);
			t_isEnoughFaZhao = m_total_bo_score[m_curPos] >= t_minFaZhaoScore;
			t_isCanFaZhao = t_isEnoughFaZhao && !(t_isDangJia && t_isTurnFirstOpt);
			t_minFaZhaoScore = t_isCanFaZhao ? t_minFaZhaoScore : 0;
		}
		//������ң���ķ�����������Ϊ��һ��������ҵ�2�����õ���Ҳ��㣩
		else
		{
			//��ǰ�����עֵ
			Lint t_tCurMaxScore = 0;
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;
				
				if (m_total_add_score[i] >= t_tCurMaxScore)
				{
					t_tCurMaxScore = m_total_add_score[i];
				}
			}

			t_minFaZhaoScore = 2 * t_tCurMaxScore;
			if (m_diPi == 0 && t_minFaZhaoScore < 10)
			{
				t_minFaZhaoScore = 10;
			}
			else if (m_diPi == 1 && t_minFaZhaoScore < 30)
			{
				t_minFaZhaoScore = 30;
			}
			else if (m_diPi == 2 && t_minFaZhaoScore < 50)
			{
				t_minFaZhaoScore = 50;
			}
			else if (m_diPi == 3 && t_minFaZhaoScore < 10)
			{
				t_minFaZhaoScore = 10;
			}

			//ѡ���״η��д���â�� && ��һ�β������� && â���ش���0
			if (m_isFirstAddBigMang && m_mang_pool > 0)
			{
				if (!m_is_first_opt_fa_zhao)
				{
					t_minFaZhaoScore = m_mang_pool;
				}
				else
				{
					t_minFaZhaoScore = 2 * t_tCurMaxScore;
				}
			}
			/*
			if (m_isFirstAddBigMang && !m_is_first_opt_fa_zhao && m_mang_pool > 0)
			{
				t_minFaZhaoScore = m_mang_pool;
			}
			else if (m_isFirstAddBigMang && m_is_first_opt_fa_zhao)
			{
				t_minFaZhaoScore = 2 * t_tCurMaxScore;
			}
			*/

			t_isEnoughFaZhao = m_total_bo_score[m_curPos] >= t_minFaZhaoScore;
			t_isCanFaZhao = t_isEnoughFaZhao && !(t_isDangJia && t_isTurnFirstOpt);
			t_minFaZhaoScore = t_isCanFaZhao ? t_minFaZhaoScore : 0;
		}

		//����Ƿ������
		bool t_isCanXiu = false;
		bool t_allPrePlayerXiu = true;  //��ǰ���֮ǰ��������Ƿ���ѡ�����/����
		t_tmpPos = m_turn_first_speak_pos;
		for (int i = m_turn_first_speak_pos; i < m_player_count + m_turn_first_speak_pos; ++i)
		{
			t_tmpPos = i % m_player_count;
			if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0) continue;

			//��������Ѿ��¹�ע�ģ�������ֵ������������Բ�����
			if (t_tmpPos == m_curPos && m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_INVALID)
			{
				t_allPrePlayerXiu = false;
				break;
			}

			//�жϽ�������
			if (t_tmpPos == m_curPos)
			{
				break;
			}

			if (t_tmpPos == m_turn_first_speak_pos && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID)
			{
			}
			//�ӵ��ҿ�ʼ���ڸ����֮ǰ��������ѡ��Ĳ�����/���ƣ���û���ݵ�Ȩ��
			else if (m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_XIU && m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_SHUAI && m_turn_opt_type[m_turn_count][t_tmpPos] != ADD_OPT_INVALID)
			{
				t_allPrePlayerXiu = false;
				break;
			}
		}
		t_isCanXiu = t_allPrePlayerXiu && !t_isFirstTurn;

		//����Ƿ��������
		bool t_isCanSanHua = false;
		bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[m_curPos], m_hand_cards_count[m_curPos]);
		bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[m_curPos], m_hand_cards_count[m_curPos]);
		t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[m_curPos];
		// m_san_hua_status[m_curPos] = t_isSanHua ? 1 : 0;
		//if (t_isCanSanHua && !m_san_hua_status[m_curPos]) m_san_hua_status[m_curPos] = 1;

		//����Ƿ�������ƣ�֮ǰΪ��˦��
		bool t_isCanShuai = !((t_isDangJia && t_isTurnFirstOpt) || t_isCanSanHua);
		Lint t_shuaiScore = 0;
		//if (t_isCanShuai && t_isFirstTurn && m_turn_opt_score[0][m_curPos] == 0)
		if (t_isCanShuai && t_isFirstTurn && m_total_add_score[m_curPos] == 0)
		{
			if (m_diPi == 1) t_shuaiScore = 5;
			else if (m_diPi == 2) t_shuaiScore = 5;
			else if (m_diPi == 3) t_shuaiScore = 2;
			else t_shuaiScore = 1;
		}

		//����Ƿ�����ò���
		//bool t_isCanQiaoBo = !(t_isDangJia && t_isTurnFirstOpt) || (t_isDangJia && t_isTurnFirstOpt && !t_isEnoughFirstQiao);
		bool t_isCanQiaoBo = m_qiao_status[m_curPos] == 0 && !(t_isDangPosMoLookCard && t_isDangJia && t_isTurnFirstOpt && t_isFirstTurn);
		Lint t_minQiaoBoScore = t_isCanQiaoBo ? m_total_bo_score[m_curPos] : 0;


		//�ɲ�����ѡ��
		m_allow_opt_type[m_curPos][ADD_OPT_FIRST_QIAO] = t_isCanFirstQiao ? ADD_OPT_FIRST_QIAO : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_GEN] = t_isCanGen ? ADD_OPT_GEN : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_FA_ZHAO] = t_isCanFaZhao ? ADD_OPT_FA_ZHAO : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_QIAO_BO] = t_isCanQiaoBo ? ADD_OPT_QIAO_BO : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_XIU] = t_isCanXiu ? ADD_OPT_XIU : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_SHUAI] = t_isCanShuai ? ADD_OPT_SHUAI : ADD_OPT_INVALID;
		m_allow_opt_type[m_curPos][ADD_OPT_SANHUA] = t_isCanSanHua ? ADD_OPT_SANHUA : ADD_OPT_INVALID;

		//�ɲ���ѡ�����С����ֵ
		m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO] = t_isCanFirstQiao ? t_minFirstQiaoScore : 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_GEN] = t_isCanGen ? t_minGenScore : 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_FA_ZHAO] = t_isCanFaZhao ? t_minFaZhaoScore : 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_QIAO_BO] = t_isCanQiaoBo ? t_minQiaoBoScore : 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_XIU] = 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_SHUAI] = t_isCanShuai ? t_shuaiScore : 0;
		m_allow_opt_min_score[m_curPos][ADD_OPT_SANHUA] = (int)t_isSanHuaDouble;

		//�ɲ���ѡ���������ֵ
		m_allow_opt_max_score[m_curPos][ADD_OPT_FIRST_QIAO] = t_isCanFirstQiao ? t_minFirstQiaoScore : 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_GEN] = t_isCanGen ? t_minGenScore : 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_FA_ZHAO] = t_isCanFaZhao ? m_total_bo_score[m_curPos] : 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_QIAO_BO] = t_isCanQiaoBo ? t_minQiaoBoScore : 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_XIU] = 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_SHUAI] = t_isCanShuai ? t_shuaiScore : 0;
		m_allow_opt_max_score[m_curPos][ADD_OPT_SANHUA] = (int)t_isSanHuaDouble;


		LLOG_ERROR("CheXuanGameHandler::calc_opt_score() Run..., deskId = [%d], curPos = [%d], allowOptType=[%d, %d, %d, %d, %d, %d, %d], allowOptMinScore=[%d, %d, %d, %d, %d, %d, %d], allowOptMaxScore=[%d, %d, %d, %d, %d, %d, %d]",
			m_desk ? m_desk->GetDeskId() : 0, m_curPos,
			m_allow_opt_type[m_curPos][0], m_allow_opt_type[m_curPos][1], m_allow_opt_type[m_curPos][2], m_allow_opt_type[m_curPos][3], m_allow_opt_type[m_curPos][4], m_allow_opt_type[m_curPos][5], m_allow_opt_type[m_curPos][6],
			m_allow_opt_min_score[m_curPos][0], m_allow_opt_min_score[m_curPos][1], m_allow_opt_min_score[m_curPos][2], m_allow_opt_min_score[m_curPos][3], m_allow_opt_min_score[m_curPos][4], m_allow_opt_min_score[m_curPos][5], m_allow_opt_min_score[m_curPos][6],
			m_allow_opt_max_score[m_curPos][0], m_allow_opt_max_score[m_curPos][1], m_allow_opt_max_score[m_curPos][2], m_allow_opt_max_score[m_curPos][3], m_allow_opt_max_score[m_curPos][4], m_allow_opt_max_score[m_curPos][5], m_allow_opt_max_score[m_curPos][6]);
	}

	/* ����ʵ�ʲ�����Ϸ��������� */
	Lint calc_real_player_count()
	{
		Lint t_realPlayerCount = 0;
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1)
			{
				++t_realPlayerCount;
			}
		}

		return t_realPlayerCount;
	}

	/* �뵲�������������С�����λ�� */
	Lint findMinCardsAndNearDangPos(Lint* minCount /*out*/, Lint minPos[] /*out*/)
	{
		HandCardsTypeInfo t_minTypeInfo = m_handCardsTypeInfo[0];

		//��ʱ���뵲�������С��ҵ�λ��
		Lint t_minAndNearDangPos = m_frist_send_card;

		//��ʼ������С����ҵ�λ��
		for (Lint i = 0; i < m_player_count; ++i)
		{
			minPos[i] = CHEXUAN_INVALID_POS;
		}

		//������С����
		for (int i = 1; i < m_real_player_count; ++i)
		{
			if (
				(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType <= t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType <= t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType)
				)
			{
				t_minTypeInfo = m_handCardsTypeInfo[i];
			}
		}

		//���ݲ��ҵ�����С���ͣ����ҳ�������С���͵����λ��
		for (int i = 0; i < m_real_player_count; ++i)
		{
			if (
				(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType > t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType > t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType == t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType == t_minTypeInfo.m_secondType)
				)
			{
				minPos[(*minCount)++] = m_handCardsTypeInfo[i].m_userIndex;
			}
		}

		//����ѡ������С���͵����λ����ʱ���뵲�������λ��
		Lint t_pos = CHEXUAN_INVALID_POS;
		bool t_flag = false;
		for (int i = t_minAndNearDangPos; i < m_player_count + t_minAndNearDangPos; ++i)
		{
			t_pos = i % m_player_count;
			if (m_user_status[t_pos] != 0) continue;

			for (int j = 0; j < *minCount; ++j)
			{
				if (t_pos == minPos[j])
				{
					t_minAndNearDangPos = minPos[j];
					t_flag = true;
					break;
				}
			}
			if (t_flag) break;
		}

		return t_minAndNearDangPos;
	}

	/* �뵲����Զ��������С�����λ�� */
	Lint findMinCardsAndFarDangPos(Lint* minCount /*out*/, Lint minPos[] /*out*/)
	{
		HandCardsTypeInfo t_minTypeInfo = m_handCardsTypeInfo[0];

		//��ʱ���뵲�������С��ҵ�λ��
		Lint t_minAndFarDangPos = m_frist_send_card;

		//��ʼ������С����ҵ�λ��
		for (Lint i = 0; i < m_player_count; ++i)
		{
			minPos[i] = CHEXUAN_INVALID_POS;
		}

		//������С����
		for (int i = 1; i < m_real_player_count; ++i)
		{
			if (
				(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType <= t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType <= t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType)
				)
			{
				t_minTypeInfo = m_handCardsTypeInfo[i];
			}
		}

		//���ݲ��ҵ�����С���ͣ����ҳ�������С���͵����λ��
		for (int i = 0; i < m_real_player_count; ++i)
		{
			if (
				(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType > t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType > t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType) ||
				(m_handCardsTypeInfo[i].m_firstType == t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType == t_minTypeInfo.m_secondType)
				)
			{
				minPos[(*minCount)++] = m_handCardsTypeInfo[i].m_userIndex;
			}
		}

		//����ѡ������С���͵����λ����ʱ���뵲����Զ��λ��
		Lint t_pos = CHEXUAN_INVALID_POS;
		bool t_flag = false;
		for (int i = t_minAndFarDangPos; i < m_player_count + t_minAndFarDangPos; ++i)
		{
			t_pos = i % m_player_count;
			if (m_user_status[t_pos] != 1) continue;

			for (int j = 0; j < *minCount; ++j)
			{
				if (t_pos == minPos[j])
				{
					t_minAndFarDangPos = minPos[j];
				}
			}
		}

		return t_minAndFarDangPos;
	}

	/* �뵲����Զ��������С�����λ�ã���������Ƶģ������������С�ģ�*/
	Lint findMinCardsAndFarDangPos_QiPai()
	{
		//�洢��С����ҵ�λ��
		Lint t_minPos[CHEXUAN_PLAYER_COUNT];

		//��С���������
		Lint t_minCount = 0;

		//�����С�Ƶļ���
		HandCardsTypeInfo t_minTypeInfo = m_handCardsTypeInfo[0];

		//��ʱ���뵲�������С��ҵ�λ��
		Lint t_minAndFarDangPos = m_frist_send_card;

		//��ʼ������С����ҵ�λ��
		for (Lint i = 0; i < m_player_count; ++i)
		{
			t_minPos[i] = CHEXUAN_INVALID_POS;
		}

		//��������ң����������������������С�������뵲����Զ�����λ��
		if (m_shuai_count > 0)
		{
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_qipai_status[i] != 0)
				{
					t_minPos[t_minCount++] = i;
				}
			}
		}
		//û�����ƣ������������
		else
		{
			//������С����
			for (int i = 1; i < m_real_player_count; ++i)
			{
				if (
					(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType <= t_minTypeInfo.m_secondType) ||
					(m_handCardsTypeInfo[i].m_firstType <= t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType)
					)
				{
					t_minTypeInfo = m_handCardsTypeInfo[i];
				}
			}

			//���ݲ��ҵ�����С���ͣ����ҳ�������С���͵����λ��
			for (int i = 0; i < m_real_player_count; ++i)
			{
				if (
					(m_handCardsTypeInfo[i].m_firstType < t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType > t_minTypeInfo.m_secondType) ||
					(m_handCardsTypeInfo[i].m_firstType > t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType < t_minTypeInfo.m_secondType) ||
					(m_handCardsTypeInfo[i].m_firstType == t_minTypeInfo.m_firstType && m_handCardsTypeInfo[i].m_secondType == t_minTypeInfo.m_secondType)
					)
				{
					t_minPos[t_minCount++] = m_handCardsTypeInfo[i].m_userIndex;
				}
			}
		}
		
		//����ѡ������С���͵����λ����ʱ���뵲����Զ��λ��
		Lint t_pos = CHEXUAN_INVALID_POS;
		bool t_flag = false;
		for (int i = t_minAndFarDangPos; i < m_player_count + t_minAndFarDangPos; ++i)
		{
			t_pos = i % m_player_count;
			if (m_user_status[t_pos] != 1) continue;

			for (int j = 0; j < t_minCount; ++j)
			{
				if (t_pos == t_minPos[j])
				{
					t_minAndFarDangPos = t_minPos[j];
				}
			}
		}

		return t_minAndFarDangPos;
	}

	/* �뵲����Զ��������������λ�� */
	Lint findMaxLoseAndFarDangPos(const Lint playerScore[] /*out*/)
	{
		Lint t_maxLoseAndFarPos = m_dang_player_pos;
		Lint t_maxLoseScore = 0;
		//������С����
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;
			if (playerScore[i] < t_maxLoseScore)
			{
				t_maxLoseScore = playerScore[i];
			}
		}

		//����ѡ������С���͵����λ����ʱ���뵲����Զ��λ��
		Lint t_pos = CHEXUAN_INVALID_POS;
		bool t_flag = false;
		for (Lint i = t_maxLoseAndFarPos; i < m_player_count + t_maxLoseAndFarPos; ++i)
		{
			t_pos = i % m_player_count;
			if (m_user_status[t_pos] != 1) continue;

			if (playerScore[i] == t_maxLoseScore)
			{
				t_maxLoseAndFarPos = i;
			}
		}

		return t_maxLoseAndFarPos;
	}

	/* �뵲����Զ�ģ��������ࣨ��������������ң�*/
	Lint findMaxLoseAndFarDangPos_QiShen(const Lint playerScore[] /*out*/)
	{
		Lint t_maxLoseAndFarPos = m_dang_player_pos;
		Lint t_maxLoseScore = 0;
		//������С����
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 0) continue;
			if (playerScore[i] < t_maxLoseScore)
			{
				t_maxLoseScore = playerScore[i];
			}
		}

		//����ѡ������С���͵����λ����ʱ���뵲����Զ��λ��
		Lint t_pos = CHEXUAN_INVALID_POS;
		bool t_flag = false;
		for (Lint i = t_maxLoseAndFarPos; i < m_player_count + t_maxLoseAndFarPos; ++i)
		{
			t_pos = i % m_player_count;
			if (m_user_status[t_pos] == 0) continue;

			if (playerScore[t_pos] == t_maxLoseScore)
			{
				t_maxLoseAndFarPos = t_pos;
			}
		}

		return t_maxLoseAndFarPos;
	}

	/* ��������𲧲��ķ�Χ */
	void calc_start_bobo_limit(int* minSelectBoScore, int* maxSelectBoScore, Lint pos = CHEXUAN_INVALID_POS)
	{
		//��Ƥ��2:5/20 ,�ײ�������1000��֮��ÿ��������500
		if (m_diPi == 2)
		{
			*minSelectBoScore = 500;
			if (m_round_offset == 0)
			{
				*minSelectBoScore = 1000;
			}
		}
		//��Ƥ��1:5/10���ײ�������500��֮��ÿ��������300
		else if (m_diPi == 1)
		{
			*minSelectBoScore = 300;
			if (m_round_offset == 0)
			{
				*minSelectBoScore = 500;
			}
		}
		//��Ƥ��2/5���ײ�������200�� ֮��ÿ��������100
		else if (m_diPi == 3)
		{
			*minSelectBoScore = 100;
			if (m_round_offset == 0)
			{
				*minSelectBoScore = 200;
			}
		}
		//��Ƥ��0��1/3���ײ�������100��֮��ÿ��������50
		else
		{
			*minSelectBoScore = 50;
			if (m_round_offset == 0)
			{
				*minSelectBoScore = 100;
			}
		}

		if (pos == CHEXUAN_INVALID_POS || !(m_desk && m_desk->m_vip && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame == 1))
		{
			*maxSelectBoScore = m_maxBoBo;
		}
		else
		{
			*maxSelectBoScore = m_rest_can_start_bo_score[pos];
		}

	}

	/* �ж��Ƿ��򲧲��������������������� */
	bool must_stand_up()
	{
		//��Ҫ��â������
		Lint t_needAddMangScore = 0;
		memset(m_player_bobo_add_di_pi, 0, sizeof(m_player_bobo_add_di_pi));
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			//������Ҫ����Ĳ����֣�â����âע����һ����ע��
			if (GetNextPos(m_dang_player_pos) == i)
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 3 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 10 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 20 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 5 + 1;
			}
			else
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 1 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 2 + 1;
			}

			//��������Ƿ���Ҫ����â�ı�־
			m_is_need_add_shouMang[i] = m_isShouMang ? 1 : 0;

			t_needAddMangScore = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];
			
			/*
			//������
			if (m_desk && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame)
			{
				//����ֵ��������
				if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
				{
					m_desk->m_standUpPlayerSeatDown[i] = 2;
					m_user_status[i] = 2;
				}
				//�Ͼ�����ֵ���㣬�����������ֵ��û�е������������Ϸ��ť����Ȼ�����Բ�����Ϸ
				else if (m_desk->m_standUpPlayerSeatDown[i] == 2  || m_desk->m_standUpPlayerSeatDown[i] == 3)
				{
					m_user_status[i] = 2;
				}
				//�Ͼ�����ֵ���㣬�����������ֵ���ҵ������������Ϸ��ť��������Ϸ
				else if(m_desk->m_standUpPlayerSeatDown[i] == 1)
				{
					m_user_status[i] = 1;
					m_desk->m_standUpPlayerSeatDown[i] = 0;
				}

				LLOG_ERROR("CheXuanGameHandler::must_stand_up() Run..., deskId = [%d], pos=[%d], status=[%d] needAddMangScore=[%d], totalBoScore=[%d], deskStandUp=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, i, m_user_status[i], t_needAddMangScore + m_mangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i], m_desk->m_standUpPlayerSeatDown[i]);
			}
			//��ͨ��
			else
			{
				//�������ʣ��Ĳ����� + ������󲧲��� < ��Ҫ��â��������
				if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
				{
					m_user_status[i] = 2;
					LLOG_ERROR("CheXuanGameHandler::must_stand_up() Run..., Player has no enought bo score, So stand up, deskId = [%d], pos = [%d], needAddMang=[%d], totalBoScore=[%d]",
						m_desk ? m_desk->GetDeskId() : 0, i, t_needAddMangScore + m_mangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i]);
				}
			}
			*/

			
			//����ֵ��������
			if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
			{
				m_desk->m_standUpPlayerSeatDown[i] = 2;
				m_user_status[i] = 2;
			}
			//�Ͼ�����ֵ���㣬�����������ֵ��û�е������������Ϸ��ť����Ȼ�����Բ�����Ϸ
			else if (m_desk->m_standUpPlayerSeatDown[i] == 2 || m_desk->m_standUpPlayerSeatDown[i] == 3)
			{
				m_user_status[i] = 2;
			}
			//�Ͼ�����ֵ���㣬�����������ֵ���ҵ������������Ϸ��ť��������Ϸ
			else if (m_desk->m_standUpPlayerSeatDown[i] == 1)
			{
				m_user_status[i] = 1;
				m_desk->m_standUpPlayerSeatDown[i] = 0;
			}

			LLOG_ERROR("CheXuanGameHandler::must_stand_up() Run..., deskId = [%d], pos=[%d], status=[%d] needAddMangScore=[%d], mangSocre=[%d], totalBoScore=[%d], deskStandUp=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, i, m_user_status[i], t_needAddMangScore, m_mangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i], m_desk->m_standUpPlayerSeatDown[i]);

		}

		Lint t_playerCount = 0;
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1) ++t_playerCount;
		}
		if (t_playerCount <= 1)
		{
			//���������Ľ�ɢ��Ҫ���¼�����ֲ�������
			memcpy(m_total_bo_score_after_bobo, m_total_bo_score, sizeof(m_total_bo_score_after_bobo));

			LLOG_ERROR("CheXuanGameHandler::must_stand_up() Run..., Has one player real play,so game finish..., deskId = [%d], realPlayerCount=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, t_playerCount);
			set_play_status(GAME_PLAY_END);
			finish_round(2);
			return true;
		}
		return false;
	}

	/* ����������ֵ����Ԥ�� */
	bool cheack_player_coins_warn(Lint pos)
	{
		if (!m_desk || m_desk->m_clubInfo.m_clubId == 0 || !m_desk->m_isCoinsGame)
		{
			return false;
		}
		if (m_rest_can_start_bo_score[pos] + m_total_bo_score[pos] <= m_desk->m_warnScore && m_is_coins_low[pos] != 1 && m_user_status[pos] != 0)
		{
			//�����������ֵ���ͱ��
			m_is_coins_low[pos] = 1;

			//������������ֵ���͵Ķ�ʱ��
			m_is_coins_low_time[pos].Now();

			//֪ͨ�������ֵ����
			LMsgS2CWarnPointLow warnPoint;
			warnPoint.m_msgType = 0;
			if (m_desk->m_user[pos])
			{
				warnPoint.m_userId = m_desk->m_user[pos]->GetUserDataId();
				warnPoint.m_nike = m_desk->m_user[pos]->m_userData.m_nike;
				warnPoint.m_point = m_rest_can_start_bo_score[pos] + m_total_bo_score[pos];
				warnPoint.m_warnPoint = m_desk->m_warnScore;
				m_desk->m_user[pos]->Send(warnPoint);
			}
		}
		else if (m_rest_can_start_bo_score[pos] + m_total_bo_score[pos] > m_desk->m_warnScore && m_user_status[pos] != 0)
		{
			//�����������ֵ���ͱ��
			m_is_coins_low[pos] = 0;
		}
	}

	/********************************************************************************************
	* ��������		start_bobo()
	* ������		�����𲧲�
	* ���أ�		(void)
	*********************************************************************************************/
	void start_bobo()
	{
		//���õ�ǰ״̬Ϊ�𲧲�״̬
		set_play_status(GAME_PLAY_QIBOBO);

		//ÿ����â��ǰ�����Ƿ���Ҫ������â�ı�־��0
		memset(m_is_need_add_shouMang, 0, sizeof(m_is_need_add_shouMang));

		bool t_nextStatus = true;
		Lint t_playerMaxSelectBoScore = 0;
		Lint t_playerMinSelectBoScore = 0;
		Lint t_needAddMang = 0;

		memset(m_player_bobo_add_di_pi, 0, sizeof(m_player_bobo_add_di_pi));

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1)
			{
				continue;
			}

			//������Ҫ����Ĳ����֣�â����âע����һ����ע��
			if (m_dang_player_pos == i)
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 3 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 10 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 20 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 5 + 1;
			}
			else
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 1 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 2 + 1;
			}

			//TODO:ȷ�������𲧲��ķ�Χ
			Lint t_minSelectBoScore = 50;
			Lint t_maxSelectBoScore = 500;

			calc_start_bobo_limit(&t_minSelectBoScore, &t_maxSelectBoScore, i);

			//��һ��ÿ����Ҷ���Ҫ�𲧲�
			if (0 == m_round_limit)
			{
				m_is_need_add_bo[i] = 1;
			}
			//���ǵ�һ�ֵģ�Ҫ�ж��Ƿ���Ҫ�𲧲�������Ϊ�գ�������������â�����������Ҫ�𲧲�
			else
			{
				//������Ҫ�µ�â������
				Lint t_needAddMangScore = 0;

				//��������Ƿ���Ҫ����â�ı�־
				m_is_need_add_shouMang[i] = m_isShouMang ? 1 : 0;

				//t_needAddMangScore = m_total_bo_score[i] - m_is_need_add_shouMang[i] * m_shouMangCount - m_is_need_add_xiuMang[i] * m_xiuMangCount - m_is_need_add_zouMang[i] * m_zouMangCount - m_mangScore;
				t_needAddMangScore = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];

				//�����Ҳ���Ϊ�� || ��������������Ҫ�µ�â�������ˣ���������Ҫ�𲧲�
				if (m_total_bo_score[i] <= 0 || m_total_bo_score[i] < t_needAddMangScore)
				{
					m_is_need_add_bo[i] = 1;
				}
			}

			t_playerMaxSelectBoScore = t_maxSelectBoScore;
			t_playerMinSelectBoScore = t_minSelectBoScore;

			t_needAddMang = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];
			t_playerMaxSelectBoScore = t_maxSelectBoScore;
			t_playerMinSelectBoScore = t_minSelectBoScore;

			if (m_rest_can_start_bo_score[i] < t_playerMaxSelectBoScore)
			{
				t_playerMaxSelectBoScore = m_rest_can_start_bo_score[i];
			}
			if (t_playerMinSelectBoScore + m_total_bo_score[i] < t_needAddMang)
			{
				t_playerMinSelectBoScore = t_needAddMang - m_total_bo_score[i];
			}

			if (m_rest_can_start_bo_score[i] < t_playerMinSelectBoScore)
			{
				t_playerMinSelectBoScore = m_rest_can_start_bo_score[i];
				t_playerMaxSelectBoScore = m_rest_can_start_bo_score[i];
			}

			//��¼ÿ������𲧲��������Сֵ
			m_min_qi_bo_score[i] = t_playerMinSelectBoScore;
			m_max_qi_bo_score[i] = t_playerMaxSelectBoScore;

			t_nextStatus = false;

			if (m_rest_can_start_bo_score[i] <= 0)
			{
				m_qibobo_status[i] = 0;
				m_min_qi_bo_score[i] = 0;
				m_max_qi_bo_score[i] = 0;
			}

			//֪ͨ����𲧲�
			CheXuanS2CNotifyPlayerQiBoBo qiBoBo;
			qiBoBo.m_pos = i;
			qiBoBo.m_restTime = m_def_time_qibobo;
			qiBoBo.m_dangPos = GetNextPos(m_frist_send_card);
			qiBoBo.m_piPool = m_pi_pool;
			qiBoBo.m_mangPool = m_mang_pool;
			qiBoBo.m_minSelectBoScore = m_min_qi_bo_score[i];
			qiBoBo.m_maxSelectBoScore = m_max_qi_bo_score[i];
			memcpy(qiBoBo.m_userStatus, m_user_status, sizeof(qiBoBo.m_userStatus));
			memcpy(qiBoBo.m_userMustQiBoBo, m_is_need_add_bo, sizeof(qiBoBo.m_userMustQiBoBo));
			memcpy(qiBoBo.m_playerStandUp, m_player_stand_up, sizeof(qiBoBo.m_playerStandUp));

			notify_desk(qiBoBo);
			//�㲥��ս��ң��𲧲�
			notify_lookon(qiBoBo);

			//��ʼ��ʱ
			m_play_status_time.Now();
		}

		//��������˶�����Ҫ�𲧲���ֱ�ӽ�����â��״̬
		if (t_nextStatus)
		{
			set_play_status(GAME_PLAY_ADD_MANGGUO);
			//��â��
			add_mang_guo();
		}
	}

	/* ��â�� */
	void add_mang_guo()
	{
		//ÿ����â��֮ǰ��¼��ֵĲ����������ڼ���
		memcpy(m_total_bo_score_after_bobo, m_total_bo_score, sizeof(m_total_bo_score_after_bobo));

		//��Ҫ�µ�â������ = ����â + ��â + ��â
		Lint t_addMangCount = 0;
		//��Ҫ�µ�����â
		Lint t_addShouMangCount = 0;
		//��Ҫ�µ���â
		Lint t_addXiuMangCount = 0;
		//��Ҫ�µ���â
		Lint t_addZouMangCount = 0;

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			t_addShouMangCount = m_isShouMang ? (m_is_need_add_shouMang[i] * m_shouMangCount) : 0;
			t_addXiuMangCount = m_isXiuMang ? (m_is_need_add_xiuMang[i] * m_xiuMangCount) : 0;
			t_addZouMangCount = m_isZouMang ? (m_is_need_add_zouMang[i] * m_zouMangCount) : 0;
			t_addMangCount = t_addShouMangCount + t_addXiuMangCount + t_addZouMangCount;


			m_first_shou_mang_time = t_addShouMangCount ? t_addShouMangCount : m_first_shou_mang_time;
			m_first_xiu_mang_time = t_addXiuMangCount ? t_addXiuMangCount : m_first_xiu_mang_time;
			m_first_zou_mang_time = t_addZouMangCount ? t_addZouMangCount : m_first_zou_mang_time;

			//�������ȥ����µ�â��
			m_total_bo_score[i] -= t_addMangCount;

			//â�����м�������µ�â��
			m_mang_pool += t_addMangCount;

			//��¼â����Ӯ��������â���ȵ������â��
			m_player_mang_score[i] -= t_addMangCount;

			//�������â���Ľ���㲥���������
			CheXuanS2CNotifyPlayerAddMangGuoBC addMangGuo;
			addMangGuo.m_pos = i;
			//addMangGuo.m_restTime = m_isMoFen ? ((Lint)m_def_time_optscore + 2 + (m_first_shou_mang_time ? 1 : 0) + (m_first_xiu_mang_time ? 1 : 0) + (m_first_zou_mang_time ? 1 : 0)) : 0;
			addMangGuo.m_totalBoScore = m_total_bo_score[i];
			addMangGuo.m_totalAddMangScore = t_addMangCount;
			addMangGuo.m_addShouShouMangCount = t_addShouMangCount;
			addMangGuo.m_addXiuMangCount = t_addXiuMangCount;
			addMangGuo.m_addZouMangCount = t_addZouMangCount;
			addMangGuo.m_mangPoolCount = m_mang_pool;
			addMangGuo.m_piPoolCount = m_pi_pool;
			addMangGuo.m_mangScore = m_mangScore;
			addMangGuo.m_isZouMang = m_zouMangCount == 0 ? 0 : 1;
			addMangGuo.m_isXiuMang = m_xiuMangCount == 0 ? 0 : 1;

			notify_desk(addMangGuo);

			//�㲥��ս��ң��𲧲�
			notify_lookon(addMangGuo);
			//�㲥���ֲ��᳤���ƹ�ս���𲧲�
			//notify_club_ower_lookon(addMangGuo);

			//��¼��ұ����µ�����â������
			m_total_add_mang_score[i] = t_addMangCount;
		}

		//����â����ձ�������â���ı�־
		memset(m_is_need_add_shouMang, 0, sizeof(m_is_need_add_shouMang));
		memset(m_is_need_add_xiuMang, 0, sizeof(m_is_need_add_xiuMang));
		memset(m_is_need_add_zouMang, 0, sizeof(m_is_need_add_zouMang));

		//��äע
		set_play_status(GAME_PLAY_ADD_MANGSCORE);
		add_mang_score();
	}

	/* ��äע */
	void add_mang_score()
	{
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			m_total_bo_score[i] -= m_mangScore;
			m_total_add_score[i] += m_mangScore;
			m_player_add_score[i] += m_mangScore;

			//����������ֵ�Ƿ����Ԥ��
			cheack_player_coins_warn(i);
		}

		//����
		send_card();
	}

	/********************************************************************************************
	* ��������		send_card()
	* ������		���ƣ���1�ַ������Ű��ƣ� ��2�ַ���һ�����ƣ���3�ַ���һ�Ű���
	* ������
	*  @turnCount �ڼ��ַ���
	* ���أ�		��bool��
	*********************************************************************************************/
	void send_card()
	{
		set_play_status(GAME_PLAY_SEND_CARD);
		Lint t_pos = 0;

		memset(m_turn_round_opt, 0, sizeof(m_turn_round_opt));
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1 || m_qipai_status[i] != 0 || m_qiao_status[i] != 0)
			{
				continue;
			}
			m_turn_round_opt[i] = 1;
		}

		//��1�ַ��ƣ���2�Ű���
		if (0 == m_turn_count)
		{
			for (int cardIndex = 0; cardIndex < 2; ++cardIndex)
			{
				for (int i = m_frist_send_card; i < m_player_count + m_frist_send_card; ++i)
				{
					//�������ʵ��λ��
					t_pos = i % m_player_count;

					//�����λ��û�� �� ������Ѿ����ƣ��򲻸�����ҷ���
					if (m_user_status[t_pos] != 1 || m_qipai_status[t_pos] != 0) continue;

					//��λ��������������
					++m_hand_cards_count[t_pos];

					for (int j = 0; j < m_player_count; ++j)
					{
						if (m_user_status[j] != 1) continue;
						CheXuanS2CSendCard sendCard;
						sendCard.m_pos = t_pos;
						sendCard.m_turnCount = m_turn_count;
						sendCard.m_handCardCount = m_hand_cards_count[t_pos];
						if (j == t_pos)
						{
							memcpy(sendCard.m_handCards, m_hand_cards[t_pos], sizeof(Lint) * m_hand_cards_count[t_pos]);
							LLOG_ERROR("CheXuanGameHandler::send_card() Run(msg)..., deskId=[%d], turnCount=[%d], handCard%d=[%x,%x,%x,%x], handCardsCount=[%d]",
								m_desk ? m_desk->GetDeskId() : 0, sendCard.m_turnCount, sendCard.m_pos,
								sendCard.m_handCards[0], sendCard.m_handCards[1], sendCard.m_handCards[2], sendCard.m_handCards[3], sendCard.m_handCardCount);
						}
						notify_user(sendCard, j);
					}
					//�㲥����ͨ��ս���
					CheXuanS2CSendCard sendCard_look;
					sendCard_look.m_pos = t_pos;
					sendCard_look.m_turnCount = m_turn_count;
					sendCard_look.m_handCardCount = m_hand_cards_count[t_pos];
					notify_lookon(sendCard_look);
					notify_desk_seat_no_playing_user(sendCard_look);
				}
			}

			//�����ƾ�״̬Ϊ����ע
			set_play_status(GAME_PLAY_ADD_SCORE);

			//TODO:֪ͨ��һ��˵�����˿�ʼ����
			m_curPos = m_turn_first_speak_pos;

			//���ѡ��Ĭ��
			if (m_isMoFen)
			{
				//TODO����ʼ��ʱ���ȴ����Ҳ���Ĭ��ѡ��

				//��ʼ��¼����ʱ
				m_play_status_time.Now();

				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = (Lint)m_def_time_optscore;// +(m_first_shou_mang_time ? 1 : 0) + (m_first_xiu_mang_time ? 1 : 0) + (m_first_zou_mang_time ? 1 : 0);
				notifyOpt.m_isMoFen = 1;
				notify_desk(notifyOpt);
				notify_lookon(notifyOpt);

			}
			//����ҪĬ��
			else
			{
				bool t_flag = false;
				//���������Ѿ�����...������һ��û���õ���� �� ����������� λ��
				if (m_qiao_status[m_curPos] == 1 || m_oneNotQiao || m_OnlyQiaoAndXiu)
				{
					Lint t_curPos = m_curPos;
					m_curPos = CHEXUAN_INVALID_POS;
					int t_tmpPos = CHEXUAN_INVALID_POS;

					//for (int i = t_curPos; i < m_player_count + t_curPos && !m_oneNotQiao; ++i)
					for (int i = t_curPos; i < m_player_count + t_curPos; ++i)  //(Ren:2019-02-25)
					{
						t_tmpPos = i % m_player_count;
						if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0)
						{
							continue;
						}

						//���������עֵ(Ren��2019-02-25)
						Lint t_maxAddSocre = 0;
						for (int i = 0; i < m_player_count; ++i)
						{
							if (m_total_add_score[i] > t_maxAddSocre)
							{
								t_maxAddSocre = m_total_add_score[i];
							}
						}

						//�ж�û�� ������ �״γ��ֵ��������ͣ�Ҫ������ҷ��Ͳ�����ʾ��Ϣ
						bool t_isCanSanHua = false;
						bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
						bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
						t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[t_tmpPos];
						//if (m_qiao_status[t_tmpPos] == 0 || (t_isCanSanHua))	// && m_qiao_status[t_tmpPos] == 0))
						//if ((m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || t_isCanSanHua) //(Ren:2019-02-25)
						if (
							(m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
							(m_OnlyQiaoAndXiu && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
							(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
							(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID) ||
							t_isCanSanHua) //(Ren:2019-03-15)
						{
							m_turn_round_opt[t_tmpPos] = 1;
							m_curPos = t_tmpPos;
							t_flag = true;
							break;
						}
						m_curPos = CHEXUAN_INVALID_POS;
					}
				}

				//�ҵ���һ��û����/�������͵����λ�ã���������������Ϣ
				if (t_flag || m_curPos != CHEXUAN_INVALID_POS)
				{
					//������������Ĳ���
					calc_opt_score();

					CheXuanS2CNotifyPlayerOpt notifyOpt;
					notifyOpt.m_pos = m_curPos;
					notifyOpt.m_restTime = (Lint)m_def_time_optscore;// + (m_first_shou_mang_time ? 1 : 0) + (m_first_xiu_mang_time ? 1 : 0) + (m_first_zou_mang_time ? 1 : 0);
					notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
					for (int i = 0; i < 10; ++i)
					{
						notifyOpt.m_optType[i] = m_allow_opt_type[m_curPos][i];
						notifyOpt.m_optMinScore[i] = m_allow_opt_min_score[m_curPos][i];
						notifyOpt.m_optMaxScore[i] = m_allow_opt_max_score[m_curPos][i];
					}
					notify_desk(notifyOpt);
					notify_lookon(notifyOpt);
					//notify_club_ower_lookon(notifyOpt);

					//��ʼ��¼����ʱ
					m_play_status_time.Now();

					return;
				}
				//�����ˣ�����ֻ��һ���˲���������
				else
				{
					++m_turn_count;
					send_card();
					return;
				}
			}
		}
		//��2�ַ��ƣ���1������
		else if (1 == m_turn_count)
		{
			for (int i = m_frist_send_card; i < m_player_count + m_frist_send_card; ++i)
			{
				t_pos = i % m_player_count;

				if (m_user_status[t_pos] != 1 || m_qipai_status[t_pos] != 0) continue;

				++m_hand_cards_count[t_pos];

				for (int j = 0; j < m_player_count; ++j)
				{
					if (m_user_status[j] != 1) continue;

					CheXuanS2CSendCard sendCard;
					sendCard.m_pos = t_pos;
					sendCard.m_turnCount = m_turn_count;
					sendCard.m_currSendCard = m_hand_cards[t_pos][m_hand_cards_count[t_pos] - 1];
					sendCard.m_handCardCount = m_hand_cards_count[t_pos];

					memset(sendCard.m_handCards, 0, sizeof(sendCard.m_handCards));
					if (t_pos == j)
					{
						memcpy(sendCard.m_handCards, m_hand_cards[t_pos], sizeof(Lint) * m_hand_cards_count[t_pos]);
						LLOG_ERROR("CheXuanGameHandler::send_card() Run(msg)..., deskId=[%d], turnCount=[%d], handCard%d=[%x,%x,%x,%x], handCardsCount=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, sendCard.m_turnCount, sendCard.m_pos,
							sendCard.m_handCards[0], sendCard.m_handCards[1], sendCard.m_handCards[2], sendCard.m_handCards[3], sendCard.m_handCardCount);
					}
					else
					{
						sendCard.m_handCards[2] = m_hand_cards[t_pos][2];
					}
					notify_user(sendCard, j);
				}

				//�㲥����ͨ��ս���
				CheXuanS2CSendCard sendCard_look;
				sendCard_look.m_pos = t_pos;
				sendCard_look.m_turnCount = m_turn_count;
				sendCard_look.m_handCardCount = m_hand_cards_count[t_pos];
				//���ƹ�ս
				if (m_isLookOnDarkResult != 1)
				{
					sendCard_look.m_handCards[2] = m_hand_cards[t_pos][2];
				}
				notify_lookon(sendCard_look);
				notify_desk_seat_no_playing_user(sendCard_look);
				//notify_desk_seating_user(sendCard_look);
			}

			//�����ƾ�״̬Ϊ����ע
			set_play_status(GAME_PLAY_ADD_SCORE);

			//�ҳ����������������λ��
			Lint t_thredCards[CHEXUAN_PLAYER_COUNT];
			memset(t_thredCards, 0, sizeof(Lint)*CHEXUAN_PLAYER_COUNT);
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

				t_thredCards[i] = m_hand_cards[i][2];
			}
			m_turn_first_speak_pos = m_gamelogic.getMaxCardPos(t_thredCards, m_player_count, m_player_count, m_dang_player_pos);
			m_curPos = m_turn_first_speak_pos;


			bool t_flag = false;
			//���������Ѿ�����...������һ��û���õ���� �� ����������� λ��
			if (m_qiao_status[m_curPos] == 1 || m_oneNotQiao || m_OnlyQiaoAndXiu)
			{
				Lint t_curPos = m_curPos;
				m_curPos = CHEXUAN_INVALID_POS;
				int t_tmpPos = CHEXUAN_INVALID_POS;
				//for (int i = t_curPos; i < m_player_count + t_curPos && !m_oneNotQiao; ++i)
				for (int i = t_curPos; i < m_player_count + t_curPos; ++i)  //(Ren:2019-02-25)
				{
					t_tmpPos = i % m_player_count;
					if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0)
					{
						continue;
					}

					//���������עֵ(Ren��2019-02-25)
					Lint t_maxAddSocre = 0;
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_total_add_score[i] > t_maxAddSocre)
						{
							t_maxAddSocre = m_total_add_score[i];
						}
					}

					//�ж�û�� ������ �״γ��ֵ��������ͣ�Ҫ������ҷ��Ͳ�����ʾ��Ϣ
					bool t_isCanSanHua = false;
					bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[t_tmpPos];
					//if (m_qiao_status[t_tmpPos] == 0 || (t_isCanSanHua))	// && m_qiao_status[t_tmpPos] == 0))
					//if ((m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || t_isCanSanHua) //(Ren:2019-02-25)
					if (
						(m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(m_OnlyQiaoAndXiu && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || 
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID) ||
						t_isCanSanHua) //(Ren:2019-03-15)
					{
						m_turn_round_opt[t_tmpPos] = 1;
						m_curPos = t_tmpPos;
						t_flag = true;
						break;
					}
					m_curPos = CHEXUAN_INVALID_POS;
				}
			}
			//�ҵ���һ��û����/�������͵����λ�ã���������������Ϣ
			if (t_flag || m_curPos != CHEXUAN_INVALID_POS)
			{
				calc_opt_score();

				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = m_def_time_optscore;
				notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
				for (int i = 0; i < 10; ++i)
				{
					notifyOpt.m_optType[i] = m_allow_opt_type[m_curPos][i];
					notifyOpt.m_optMinScore[i] = m_allow_opt_min_score[m_curPos][i];
					notifyOpt.m_optMaxScore[i] = m_allow_opt_max_score[m_curPos][i];
				}
				notify_desk(notifyOpt);
				notify_lookon(notifyOpt);
				//notify_club_ower_lookon(notifyOpt);

				//��ʼ��¼����ʱ
				m_play_status_time.Now();
				return;
			}
			//������
			else
			{
				++m_turn_count;
				send_card();
				return;
			}
		}
		//��3�ַ��ƣ���1�Ű��ƣ���Ӵ��ƽ׶�
		else
		{
			for (int i = m_frist_send_card; i < m_player_count + m_frist_send_card; ++i)
			{
				t_pos = i % m_player_count;

				if (m_user_status[t_pos] != 1 || m_qipai_status[t_pos] != 0) continue;

				++m_hand_cards_count[t_pos];

				for (int j = 0; j < m_player_count; ++j)
				{
					if (m_user_status[j] != 1) continue;

					CheXuanS2CSendCard sendCard;
					sendCard.m_pos = t_pos;
					sendCard.m_turnCount = m_turn_count;
					sendCard.m_handCardCount = m_hand_cards_count[t_pos];

					memset(sendCard.m_handCards, 0, sizeof(sendCard.m_handCards));
					if (t_pos == j)
					{
						memcpy(sendCard.m_handCards, m_hand_cards[t_pos], sizeof(Lint) * m_hand_cards_count[t_pos]);
						LLOG_ERROR("CheXuanGameHandler::send_card() Run(msg)..., deskId=[%d], turnCount=[%d], handCard%d=[%x,%x,%x,%x], handCardsCount=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, sendCard.m_turnCount, sendCard.m_pos,
							sendCard.m_handCards[0], sendCard.m_handCards[1], sendCard.m_handCards[2], sendCard.m_handCards[3], sendCard.m_handCardCount);
					}
					else
					{
						sendCard.m_handCards[2] = m_hand_cards[t_pos][2];
					}
					notify_user(sendCard, j);
				}

				//�㲥����ͨ��ս���
				CheXuanS2CSendCard sendCard_look;
				sendCard_look.m_pos = t_pos;
				sendCard_look.m_turnCount = m_turn_count;
				sendCard_look.m_handCardCount = m_hand_cards_count[t_pos];
				//���ƽ����ս
				if (m_isLookOnDarkResult != 1)
				{
					sendCard_look.m_handCards[2] = m_hand_cards[t_pos][2];
				}
				notify_lookon(sendCard_look);
				notify_desk_seat_no_playing_user(sendCard_look);
				//notify_desk_seating_user(sendCard_look);
			}

			//�����ƾ�״̬Ϊ����״̬
			set_play_status(GAME_PLAY_CUO_PAI);
			CheXuanS2CNotifyCuoPai cuoPai;
			cuoPai.m_restTime = m_def_time_cuopai;
			memcpy(cuoPai.m_userStatus, m_user_status, sizeof(cuoPai.m_userStatus));
			memcpy(cuoPai.m_cuoPaiStatus, m_cuopai_status, sizeof(cuoPai.m_cuoPaiStatus));
			memcpy(cuoPai.m_qiaoStatus, m_qiao_status, sizeof(cuoPai.m_qiaoStatus));
			memcpy(cuoPai.m_qiPaiStatus, m_qipai_status, sizeof(cuoPai.m_qiPaiStatus));

			notify_desk(cuoPai);
			notify_lookon(cuoPai);

			//��ʼ��¼����ʱ
			m_play_status_time.Now();
			return;
		}
	}

	/********************************************************************************************
	* ��������      start_game()
	* ������        ����ÿһ����Ϸ��ʼ�ľ������
	* ���أ�        (void)
	*********************************************************************************************/
	void start_game()
	{
		//���ó���ÿ�ֽ�������һ�̶ֹ�����ʱʱ��
		if (m_desk)
		{
			m_desk->m_next_delay_time = 7;
			if (m_isFastMode)
			{
				m_desk->m_next_delay_time = 3;
			}
		}

		//ÿ����â��ǰ�����Ƿ���Ҫ������â�ı�־��0
		memset(m_is_need_add_shouMang, 0, sizeof(m_is_need_add_shouMang));

		//�����𲧲��ֲ��������������ս
		if (must_stand_up())
		{
			return;
		}

		if (m_round_offset != 0)
		{
			m_frist_send_card = GetNextPos(m_frist_send_card);
		}

		//����λ��
		m_dang_player_pos = GetNextPos(m_frist_send_card);

		//���ñ�����ע��ʼ˵����λ��
		m_turn_first_speak_pos = m_dang_player_pos;

		//���ÿ�ʼ˵���˵�λ��Ϊ��ǰλ��
		m_curPos = m_turn_first_speak_pos;

		//�طż�¼��������
		m_video.setCommonInfo(gWork.GetCurTime().Secs(), m_round_offset, m_player_count, m_desk->m_vip->m_posUserId, m_frist_send_card);

		//ϴ��
		dispatch_user_cards();

		//�𲧲���ϵͳ�Զ���â��
		start_bobo();

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
		LLOG_DEBUG("CheXuanGameHandler::start_round() Run... SANDAER GAME_START!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! deskId=[%d],",
			m_desk ? m_desk->GetDeskId() : 0);

		Lint t_user_status[CHEXUAN_PLAYER_COUNT];
		memcpy(t_user_status, player_status, sizeof(Lint) * CHEXUAN_PLAYER_COUNT);

		clear_round();

		//�㲥��ǰ���������
		notify_desk_match_state();

		set_desk_state(DESK_PLAY);

		memcpy(m_user_status, t_user_status, sizeof(m_user_status));

		//ÿ����â��֮ǰ��¼��ֵĲ����������ڼ���
		memcpy(m_total_bo_score_after_bobo, m_total_bo_score, sizeof(m_total_bo_score_after_bobo));

		//��������ʼ��ÿ����ҿ��𲧲�������ÿ����ֻ���ʼ��һ��
		if (m_desk && m_desk->m_vip && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame == 1)
		{
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 0 && !m_has_record_bobo[i])
				{
					m_rest_can_start_bo_score[i] = m_desk->m_vip->m_coins[i];	//��������ֵ
					m_record_bobo_score[i] = m_desk->m_vip->m_coins[i];
					m_has_record_bobo[i] = true;
				}
			}
		}
		
		if (m_desk && m_desk->m_vip)
		{
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);
		}
		LLOG_ERROR("desk_id=[%d], start_round=[%d/%d], player_count=%d", m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

		//��Ϸ��ʼ���
		start_game();
	}

	/* ϴ�ƣ����ڷ���˸�ÿ����ҷ�������е��� */
	bool dispatch_user_cards()
	{
		//�������˿���(32����)
		Lint t_randCards[CHEXUAN_CELL_PACK];

		//�طż�¼�������
		//std::vector<Lint> vec_handCards[CHEXUAN_CELL_PACK];
		//std::vector<Lint> vec_baseCards;

		//�����˿�
		m_gamelogic.RandCardList(t_randCards, CHEXUAN_CELL_PACK);

		//����
		debugSendCards_CheXuan(t_randCards, m_desk, "card_chexuan.ini", m_player_count, CHEXUAN_HAND_CARDS_COUNT, CHEXUAN_CELL_PACK);

		//����ֻ��һ���԰������Ʒ����ÿ����ң������������������
		for (int i = 0; i < CHEXUAN_PLAYER_COUNT; ++i)
		{
			memcpy(m_hand_cards[i], t_randCards + (i * 4), sizeof(Lint) * CHEXUAN_HAND_CARDS_COUNT);

			LLOG_ERROR("CheXuanGameHandler::dispatch_user_cards() Run... deskId=[%d], Player%d handCards=[%x, %x, %x, %x]",
				m_desk ? m_desk->GetDeskId() : 0, i, m_hand_cards[i][0], m_hand_cards[i][1], m_hand_cards[i][2], m_hand_cards[i][3]);
		}
		memcpy(m_hand_cards_backup, m_hand_cards, sizeof(m_hand_cards_backup));

		return true;
	}

	/* ��������𲧲� */
	bool on_event_player_select_bobo(Lint pos, Lint selectBoScore)
	{
		//TODO:ȷ�������𲧲��ķ�Χ
		/*Lint t_minSelectBoScore = 50;
		Lint t_maxSelectBoScore = 500;

		calc_start_bobo_limit(&t_minSelectBoScore, &t_maxSelectBoScore);
		if (m_rest_can_start_bo_score[pos] < t_maxSelectBoScore)
		{
		t_maxSelectBoScore = m_rest_can_start_bo_score[pos];
		}
		if (t_playerMinSelectBoScore + m_total_bo_score[i] < t_needAddMang)
		{
		t_playerMinSelectBoScore = t_needAddMang - m_total_bo_score[i];
		}
		if (m_rest_can_start_bo_score[pos] < t_minSelectBoScore)
		{
		t_minSelectBoScore = m_rest_can_start_bo_score[pos];
		t_maxSelectBoScore = m_rest_can_start_bo_score[pos];
		}*/

		//�����𲧲��׶β�����
		if (GAME_PLAY_QIBOBO != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_bobo() Error!!! this play status is not GAME_PLAY_QIBOBO...,deskId=[%d], pos=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_status);
			return false;
		}

		//�Ѿ��������ģ������ظ�����
		if (m_qibobo_status[pos] == 1)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_bobo() Error!!! this palyer has opt selectBoScore ...,deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		//У��selectBoScore�Ƿ���Ϲ淶
		if (!(selectBoScore == 0 && (m_is_need_add_bo[pos] != 1 || m_rest_can_start_bo_score[pos] == 0)) && (selectBoScore < m_min_qi_bo_score[pos] || selectBoScore > m_max_qi_bo_score[pos]))
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_bobo() Error!!! selectBoScore is invalid...,deskId=[%d], pos=[%d], minSelectBoScore=[%d], maxSelectBoScore=[%d], selectBoScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_min_qi_bo_score[pos], m_max_qi_bo_score[pos], selectBoScore);
			return false;
		}

		//��ӡ����ҳɹ��𲧲�����
		LLOG_ERROR("CheXuanGameHandler::on_event_player_select_bobo() Player Start BoBo Sucess^_^..., deskId=[%d], pos=[%d], selectBoScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, selectBoScore);

		m_last_bo_score[pos] = selectBoScore; // ? selectBoScore : m_last_bo_score[pos];
		m_total_bo_score[pos] += selectBoScore;
		m_total_bo_score_after_bobo[pos] = m_total_bo_score[pos];

		if (m_rest_can_start_bo_score[pos] - selectBoScore < 0)
		{
			selectBoScore = m_rest_can_start_bo_score[pos];
			m_rest_can_start_bo_score[pos] = 0;
		}
		else
		{
			m_rest_can_start_bo_score[pos] -= selectBoScore;
		}
		m_has_start_total_bo_score[pos] += selectBoScore;
		m_is_need_add_bo[pos] = 0;
		m_qibobo_status[pos] = 1;

		//�𲧲�
		CheXuanS2CPlayerSelectBoBoBC qiBoBoBC;
		qiBoBoBC.m_pos = pos;
		memcpy(qiBoBoBC.m_lastBoScore, m_last_bo_score, sizeof(qiBoBoBC.m_lastBoScore));
		memcpy(qiBoBoBC.m_totalBoScore, m_total_bo_score, sizeof(qiBoBoBC.m_totalBoScore));
		memcpy(qiBoBoBC.m_userStatus, m_user_status, sizeof(qiBoBoBC.m_userStatus));
		memcpy(qiBoBoBC.m_optBoBoStatus, m_qibobo_status, sizeof(qiBoBoBC.m_optBoBoStatus));

		notify_desk(qiBoBoBC);
		notify_lookon(qiBoBoBC);

		//���и��𲧲�����Ҳ�����ɺ󣬽�����â���׶�
		if (has_opt_over(GAME_PLAY_QIBOBO))
		{
			//ÿ����â��֮ǰ��¼��ֵĲ����������ڼ���
			memcpy(m_total_bo_score_after_bobo, m_total_bo_score, sizeof(m_total_bo_score_after_bobo));

			//������Ϸ״̬Ϊ��â���׶�
			set_play_status(GAME_PLAY_ADD_MANGGUO);

			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_bobo() All Player Opt Start BoBo Over^o^..., deskId=[%d]",
				m_desk ? m_desk->GetDeskId() : 0);

			//�𲧲��׶ν�������������Ƿ���Ҫ����״̬
			memset(m_is_need_add_bo, 0, sizeof(m_is_need_add_bo));

			//������â��
			add_mang_guo();
		}

		return true;
	}

	/* ���������ע */
	bool on_event_player_select_opt(Lint pos, Lint selectOptType, Lint selectOptScore)
	{
		//������ע�׶β�����
		if (GAME_PLAY_ADD_SCORE != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! this play status is not GAME_PLAY_ADD_SCORE...,deskId=[%d], pos=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_status);
			return false;
		}

		//�˴�У�����ѡ������Ƿ�Ϸ�
		if (pos != m_curPos)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! pos is not curPos!!! deskId=[%d], turnCount=[%d], pos=[%d], curPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, m_curPos);
			return false;
		}
		if (m_qipai_status[pos] != 0)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! this user has qi pai!!! deskId=[%d], turnCount=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos);
			return false;
		}
		if (selectOptType < 0 || selectOptType >= ADD_OPT_MAX_INVALID)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! selectOptType is not valid!!! deskId=[%d], turnCount=[%d], pos=[%d], curPos=[%d], selectOptType=[%d], selectOptScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, m_curPos, selectOptType, selectOptScore);
			return false;
		}
		if (m_allow_opt_type[pos][selectOptType] == -1)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! not allow [%d] opt!!! deskId=[%d], turnCount=[%d], pos=[%d], selectOptType=[%d], selectOptScore=[%d]",
				selectOptType, m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, selectOptType, selectOptScore);
			return false;
		}
		if (selectOptScore < m_allow_opt_min_score[pos][selectOptType] || selectOptScore > m_allow_opt_max_score[pos][selectOptType])
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! selectOptScore is not valid!!! deskId=[%d], turnCount=[%d], pos=[%d], selectOptType=[%d], selectOptScore=[%d], minOptScore=[%d], maxOptScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, selectOptType, selectOptScore, m_allow_opt_min_score[pos][selectOptType], m_allow_opt_max_score[pos][selectOptType]);
			return false;
		}

		//�µķ���������ǰ��������
		if (selectOptScore > m_total_bo_score[pos])
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Error!!! selectOptScore has more than total bo score deskId=[%d], turnCount=[%d], pos=[%d], totalBoScore=[%d], selectOptType=[%d], selectOptScore=[%d], minOptScore=[%d], maxOptScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, m_total_bo_score[pos], selectOptType, selectOptScore, m_allow_opt_min_score[pos][selectOptType], m_allow_opt_max_score[pos][selectOptType]);
			return false;
		}

		//�����ע��������
		LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() Player Opt Sucess^o^... deskId=[%d], turnCount=[%d], pos=[%d], selectOptType=[%d], selectOptScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_turn_count, pos, selectOptType, selectOptScore);

		//�����ɹ��������������������һ��������ʱ
		m_after_turn_shuai_delay = 0;

		//��¼һ����ÿλ����״β����ķ���
		if (m_turn_opt_type[m_turn_count][pos] == ADD_OPT_INVALID)
		{
			m_turn_first_opt_score[m_turn_count][pos] = selectOptScore;
		}

		//���ÿ����ע�У�����һȦ�Ժ�֮ǰ�ݵ�״̬Ҫ�����
		if (pos == m_turn_first_speak_pos && m_turn_opt_type[m_turn_count][pos] != ADD_OPT_INVALID)
		{
			memset(m_xiu_status, 0, sizeof(m_xiu_status));
		}

		//������⴦����ѡ�ķ�����Ren:2019-02-25��
		if (selectOptType == ADD_OPT_FA_ZHAO)
		{
			selectOptScore -= m_total_add_score[pos];

			//����һ�β���������
			m_is_first_opt_fa_zhao = true;
		}

		m_turn_opt_type[m_turn_count][pos] = selectOptType;
		m_turn_opt_score[m_turn_count][pos] += selectOptScore;
		m_total_bo_score[pos] -= selectOptScore;
		m_total_add_score[pos] += selectOptScore;
		m_player_add_score[pos] += selectOptScore;
		m_turn_round_opt[pos] = 0;
		m_player_last_opt[pos] = selectOptType;

		//����������ֵ�Ƿ�
		cheack_player_coins_warn(pos);

		bool t_isAllHasOptFalg = true;
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_turn_round_opt[i] != 0)
			{
				t_isAllHasOptFalg = false;
			}
		}
		if (t_isAllHasOptFalg)
		{
			memset(m_turn_round_opt, 0, sizeof(m_turn_round_opt));
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_qipai_status[i] == 0)
				{
					m_turn_round_opt[i] = 1;
				}
			}
		}


		//���ѡ�����ƣ���������ע�ŵ�Ƥ���У�����Ǹ��������
		if (selectOptType == ADD_OPT_SHUAI)
		{
			m_pi_pool += m_total_add_score[pos];
			//��¼���������ʱ���Ѿ��µķ�
			m_qipai_score[pos] = m_total_add_score[pos];
			//��¼�����Ӯ��Ƥ��
			m_player_pi_score[pos] -= m_total_add_score[pos];
			m_total_add_score[pos] = 0;
			m_player_add_score[pos] = 0;
			m_qipai_status[pos] = 1;	//����Ѿ�����
			m_qipai_last_pos = pos;		//��¼����һ�����Ƶ����λ��

			//������������һ��������ʱ
			if (m_turn_count != 0)
			{
				m_after_turn_shuai_delay = 0;
			}
			++m_shuai_count;
		}
		//���ѡ�����ݣ�������
		else if (selectOptType == ADD_OPT_XIU)
		{
			m_xiu_status[pos] = 1;
		}
		//���ѡ�����ã��������
		else if (m_qiao_status[pos] == 0 && (selectOptType == ADD_OPT_QIAO_BO || m_total_bo_score[pos] == 0))
		{
			m_qiao_status[pos] = 1;

			//����ú������ע�ִ���â������ǵ�һ�η���
			if (m_total_add_score[pos] >= m_mang_pool && !m_is_first_opt_fa_zhao)
			{
				//m_is_first_opt_fa_zhao = true;
			}
		}
		//���ѡ����������������ע�ûأ�������â�����������Ʊ��
		else if (selectOptType == ADD_OPT_SANHUA)
		{
			m_select_san_hua_status[pos] = 1;
			
			//����û���ע����
			m_total_bo_score[pos] += m_total_add_score[pos];

			//����Ѿ���ע�ķ���
			m_total_add_score[pos] = 0;

			//���Ϊ�Ѿ�ѡ�����������ƣ�
			m_qipai_status[pos] = 2;

			//������������һ��������ʱ
			if (m_turn_count != 0)
			{
				m_after_turn_shuai_delay = 0;
			}
		}

		//��һ�γ�������������û��ѡ���������Ժ󽫲�����ѡ������
		if (m_allow_opt_type[pos][ADD_OPT_SANHUA] != -1 && selectOptType != ADD_OPT_SANHUA)
		{
			m_san_hua_status[pos] = 1;
		}

		//�жϱ��ֲ����Ƿ����, ���������ǵ����ڶ�����ң���ǰ�涼�����ˣ��������һ����Ҳ����ˣ�����
		bool t_isOver = has_opt_over(GAME_PLAY_ADD_SCORE);

		CheXuanS2CPlayerOptBC optBC;
		optBC.m_pos = pos;
		optBC.m_selectType = selectOptType;
		optBC.m_selectScore = selectOptScore;
		optBC.m_turnAddTotalScore = m_turn_opt_score[m_turn_count][pos];
		optBC.m_addTotalScore = m_total_add_score[pos];
		optBC.m_totalBoScore = m_total_bo_score[pos];
		optBC.m_piScore = m_pi_pool;
		optBC.m_mangScore = m_mang_pool;
		optBC.m_nextOptPos = t_isOver ? CHEXUAN_INVALID_POS : GetNextPos(pos);
		optBC.m_is2TimesPre = CheckIsTuo(pos);  //�ж��Ƿ�Ϊ��
		optBC.m_delayTime = m_after_turn_shuai_delay;
		notify_desk(optBC);
		notify_lookon(optBC);

		//���������עû�н�������֪ͨ��һλ��ע���
		if (!t_isOver)
		{
			m_curPos = GetNextPos(m_curPos);

			bool t_flag = false;
			//���������Ѿ�����...������һ��û���õ���� �� ����������� λ��
			if (m_qiao_status[m_curPos] == 1 || m_oneNotQiao || m_OnlyQiaoAndXiu)
			{
				Lint t_curPos = m_curPos;
				m_curPos = CHEXUAN_INVALID_POS;
				int t_tmpPos = CHEXUAN_INVALID_POS;
				//for (int i = t_curPos; i < m_player_count + t_curPos && !m_oneNotQiao; ++i)
				for (int i = t_curPos; i < m_player_count + t_curPos; ++i)  //(Ren:2019-02-25)
				{
					t_tmpPos = i % m_player_count;
					if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0)
					{
						continue;
					}

					//���������עֵ(Ren��2019-02-25)
					Lint t_maxAddSocre = 0;
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_total_add_score[i] > t_maxAddSocre)
						{
							t_maxAddSocre = m_total_add_score[i];
						}
					}

					m_curPos = t_tmpPos;
					//�ж�û�� ������ �״γ��ֵ��������ͣ�Ҫ������ҷ��Ͳ�����ʾ��Ϣ
					bool t_isCanSanHua = false;
					bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[t_tmpPos];
					//if (m_qiao_status[t_tmpPos] == 0 || (t_isCanSanHua))	// && m_qiao_status[t_tmpPos] == 0))
					//if ((m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || t_isCanSanHua) //(Ren:2019-02-25)
					if (
						(m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(m_OnlyQiaoAndXiu && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID) ||
						t_isCanSanHua) //(Ren:2019-03-15)
					{
						m_curPos = t_tmpPos;
						t_flag = true;
						break;
					}
					m_curPos = CHEXUAN_INVALID_POS;
				}
			}

			//�ҵ���һ��û����/�������͵����λ�ã���������������Ϣ
			if (t_flag || m_curPos != CHEXUAN_INVALID_POS)
			{
				Lint t_isMeiDaDong = 0;
				calc_opt_score();

				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = m_def_time_optscore;
				notifyOpt.m_isMeiDaDong = t_isMeiDaDong;
				notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
				memcpy(notifyOpt.m_optType, m_allow_opt_type[m_curPos], sizeof(notifyOpt.m_optType));
				memcpy(notifyOpt.m_optMinScore, m_allow_opt_min_score[m_curPos], sizeof(notifyOpt.m_optMinScore));
				memcpy(notifyOpt.m_optMaxScore, m_allow_opt_max_score[m_curPos], sizeof(notifyOpt.m_optMaxScore));
				notify_desk(notifyOpt);
				notify_lookon(notifyOpt);

				//��ʼ��¼����ʱ
				m_play_status_time.Now();
				return true;
			}
			//���������
			else
			{
				if (m_turn_count < 2)
				{
					++m_turn_count;
					send_card();
				}
				//��������ע���������볶�ƽ׶�
				else if (m_turn_count == 2)
				{
					//����û����â �� ��â���������â����â�ı���
					m_xiuMangCount = 0;
					m_zouMangCount = 0;

					set_play_status(GAME_PLAY_CHE_PAI);
					CheXuanS2CNotifyChePai chePai;
					chePai.m_restTime = m_def_time_chepai;
					memcpy(chePai.m_userStatus, m_user_status, sizeof(chePai.m_userStatus));
					memcpy(chePai.m_chePaiStatus, m_chepai_status, sizeof(chePai.m_chePaiStatus));
					memcpy(chePai.m_qiPaiStatus, m_qipai_status, sizeof(chePai.m_qiPaiStatus));
					memcpy(chePai.m_qiaoStatus, m_qiao_status, sizeof(chePai.m_qiaoStatus));
					notify_desk(chePai);
					notify_lookon(chePai);

					m_play_status_time.Now();
				}
				return true;
			}
		}

		//���������ע������������һ�ַ��ơ���ע
		if (t_isOver)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_opt() All Player Opt Over this turn count^o^... deskId=[%d], turnCount=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_turn_count);

			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_player_last_opt[i] != ADD_OPT_QIAO_BO && m_player_last_opt[i] != ADD_OPT_SHUAI && m_player_last_opt[i] != ADD_OPT_SANHUA)
				{
					m_player_last_opt[i] = ADD_OPT_INVALID;
				}
			}
			
			m_curPos = CHEXUAN_INVALID_POS;

			//���������Ҷ����ˣ���������֣������¾���â������
			if (is_xiu_mang())
			{
				//���ñ��ֽ���״̬Ϊ�ݹ���
				m_finish_round_type = 1;

				if (m_isXiuMang)
				{
					//���ñ��ֽ���״̬Ϊ��â
					m_finish_round_type = 2;
					//������һ����â������
					if (m_xiuMangCount == 0) m_xiuMangCount = m_commonMangCount;
					else if (m_isDaMangFanBei) m_xiuMangCount *= 2;  //������â����
					else m_xiuMangCount = m_commonMangCount;

					//�����ⶥ
					if (!m_isFanBeiFengDing)
					{
						if (m_xiuMangCount > m_commonMangCount * 4)
						{
							m_xiuMangCount = m_commonMangCount * 4;
						}
					}

					//������һ����Ҫ��â����ұ�־
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_user_status[i] == 1)
						{
							m_is_need_add_xiuMang[i] = 1;
						}
					}
				}

				//�����â��
				m_zouMangCount = 0;

				finish_round();
				return true;
			}

			//������˴�â�ɹ�����������֣������¾ִ�â������
			if (is_zou_mang())
			{
				//���ñ��ֽ���״̬Ϊһ���������˶�����
				m_finish_round_type = 3;

				if (m_isZouMang && m_turn_count == 0)
				{
					//���ñ��ֽ���״̬Ϊ��â
					m_finish_round_type = 4;

					//������һ����â�ı���
					if (m_zouMangCount == 0) m_zouMangCount = m_commonMangCount;
					else if (m_isDaMangFanBei) m_zouMangCount *= 2;  //������â����
					else m_zouMangCount = m_commonMangCount;

					//�����ⶥ
					if (!m_isFanBeiFengDing)
					{
						if (m_zouMangCount > m_commonMangCount * 4)
						{
							m_zouMangCount = m_commonMangCount * 4;
						}
					}

					//������һ����Ҫ��â����ұ�־
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_qipai_status[i] == 1 && m_user_status[i] == 1)
						{
							m_is_need_add_zouMang[i] = 1;
						}
					}
				}
				else
				{
					m_zouMangCount = 0;
				}

				//�����â��
				m_xiuMangCount = 0;

				finish_round();
				return true;
			}

			//�����������������
			memset(m_xiu_status, 0, sizeof(m_xiu_status));

			//�ڶ��֡������ַ���
			if (m_turn_count < 2)
			{
				++m_turn_count;
				send_card();
			}
			//��������ע���������볶�ƽ׶�
			else if (m_turn_count == 2)
			{
				//����û����â �� ��â���������â����â�ı���
				m_xiuMangCount = 0;
				m_zouMangCount = 0;

				set_play_status(GAME_PLAY_CHE_PAI);

				CheXuanS2CNotifyChePai chePai;
				chePai.m_restTime = m_def_time_chepai;
				memcpy(chePai.m_userStatus, m_user_status, sizeof(chePai.m_userStatus));
				memcpy(chePai.m_chePaiStatus, m_chepai_status, sizeof(chePai.m_chePaiStatus));
				memcpy(chePai.m_qiPaiStatus, m_qipai_status, sizeof(chePai.m_qiPaiStatus));
				memcpy(chePai.m_qiaoStatus, m_qiao_status, sizeof(chePai.m_qiaoStatus));
				notify_desk(chePai);
				notify_lookon(chePai);

				m_play_status_time.Now();
			}
		}
		return true;
	}

	/* ������ҳ��� */
	bool on_event_player_che_pai(Lint pos, Lint* handCards, Lint handCardsCount)
	{
		//�����³��ƽ׶β�����
		if (GAME_PLAY_CHE_PAI != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_che_pai() Error!!! this play status is not GAME_PLAY_CHE_PAI...,deskId=[%d], pos=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_status);
			return false;
		}

		//TODO:У�����Ƿ���ԭʼ����һ��
		if (!m_gamelogic.checkTwoHandCardsIsSame(m_hand_cards[pos], handCards, handCardsCount))
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_che_pai() Error!!! srcCards are not same as desCards!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		if (m_chepai_status[pos] != 0)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_che_pai() Error!!! this player has che pai!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		//���������е������ƣ����һ����ǰ
		m_gamelogic.sortHandCardsByDouble(handCards, handCardsCount, m_hand_cards_type[pos]);
		memcpy(m_hand_cards[pos], handCards, sizeof(Lint) * handCardsCount);

		//����Ѿ���������
		m_chepai_status[pos] = 1;

		CheXuanS2CPlayerChePaiBC palyerChePaiBC;
		palyerChePaiBC.m_pos = pos;
		palyerChePaiBC.m_totalPiScore = 0;
		palyerChePaiBC.m_totalMangScore = 0;
		palyerChePaiBC.m_totalAddScore = 0;
		memcpy(palyerChePaiBC.m_handCards[pos], m_hand_cards[pos], sizeof(Lint) * 4);
		memcpy(palyerChePaiBC.m_cardsType[pos], m_hand_cards_type[pos], sizeof(Lint) * 2);
		memcpy(palyerChePaiBC.m_userStatus, m_user_status, sizeof(palyerChePaiBC.m_userStatus));
		notify_desk(palyerChePaiBC);

		notify_lookon(palyerChePaiBC);

		LLOG_ERROR("CheXuanGameHandler::on_event_player_che_pai() Run... Player che pai Sucess^o^... deskId=[%d], pos=[%d], cardsTypes=[%d, %d], handCards=[%x, %x, %x, %x], handCardsCount=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, palyerChePaiBC.m_cardsType[pos][0], palyerChePaiBC.m_cardsType[pos][1],
			palyerChePaiBC.m_handCards[pos][0], palyerChePaiBC.m_handCards[pos][1], palyerChePaiBC.m_handCards[pos][2], palyerChePaiBC.m_handCards[pos][3],
			m_hand_cards_count[pos]);

		//������ҳ��ƽ���
		if (has_opt_over(GAME_PLAY_CHE_PAI))
		{
			set_play_status(GAME_PLAY_END);
			//���ó���ÿ�ֽ�������һ�̶ֹ�����ʱʱ��
			if (m_desk)
			{
				m_desk->m_next_delay_time = 10;
				if (m_isFastMode)
				{
					m_desk->m_next_delay_time = 6;
				}
			}

			Lint t_count = 0;
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

				m_handCardsTypeInfo[t_count].m_userIndex = i;
				m_handCardsTypeInfo[t_count].m_firstType = m_gamelogic.getDoubleCardsTypeValue(m_hand_cards[i][0], m_hand_cards[i][1]);
				m_handCardsTypeInfo[t_count].m_secondType = m_gamelogic.getDoubleCardsTypeValue(m_hand_cards[i][2], m_hand_cards[i][3]);
				m_handCardsTypeInfo[t_count].m_pretHandCards[0] = m_hand_cards[i][0];
				m_handCardsTypeInfo[t_count].m_pretHandCards[1] = m_hand_cards[i][1];
				m_handCardsTypeInfo[t_count].m_nextHandCards[0] = m_hand_cards[i][2];
				m_handCardsTypeInfo[t_count].m_nextHandCards[1] = m_hand_cards[i][3];
				++t_count;
			}

			//����ʵ�ʲ�����Ϸ������
			m_real_player_count = t_count;

			//���ڶ����ư���С��������
			m_gamelogic.sortAllPlayerCards(m_handCardsTypeInfo, t_count, GetNextPos(m_frist_send_card), m_player_count);

			for (size_t i = 0; i < t_count; ++i)
			{
				LLOG_ERROR("CheXuanGameHandler::on_event_player_che_pai(), deskId=[%d], No.%d, pos=[%d], cardType=[%d, %d] handCards=[%x, %x, %x, %x], handCardsCount=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, i, m_handCardsTypeInfo[i].m_userIndex, m_handCardsTypeInfo[i].m_firstType, m_handCardsTypeInfo[i].m_secondType,
					m_handCardsTypeInfo[i].m_pretHandCards[0], m_handCardsTypeInfo[i].m_pretHandCards[1],
					m_handCardsTypeInfo[i].m_nextHandCards[0], m_handCardsTypeInfo[i].m_nextHandCards[1],
					m_hand_cards_count[m_handCardsTypeInfo[i].m_userIndex]);
			}

			//���������ע�ķ���
			Lint t_totalAddScore = 0;
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_qipai_status[i] == 0)
				{
					t_totalAddScore += m_total_add_score[i];
				}
			}

			//���ƽ����ս
			if (m_isLookOnDarkResult)
			{
				CheXuanS2CPlayerChePaiBC chePaiBC;
				chePaiBC.m_pos = CHEXUAN_INVALID_POS;
				chePaiBC.m_totalPiScore = m_pi_pool;
				chePaiBC.m_totalMangScore = m_mang_pool;
				chePaiBC.m_totalAddScore = t_totalAddScore;
				memcpy(chePaiBC.m_userStatus, m_user_status, sizeof(chePaiBC.m_userStatus));
				memcpy(chePaiBC.m_qiPaiStatus, m_qipai_status, sizeof(chePaiBC.m_qiPaiStatus));
				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 0)
					{
						continue;
					}
					memcpy(chePaiBC.m_handCards[i], m_hand_cards[i], sizeof(Lint) * 4);
					memcpy(chePaiBC.m_cardsType[i], m_hand_cards_type[i], sizeof(Lint) * 2);
				}
				notify_desk_playing_user(chePaiBC);

				//���ƹ㲥����ս���
				CheXuanS2CPlayerChePaiBC gz_chePaiBC;
				gz_chePaiBC.m_pos = CHEXUAN_INVALID_POS;
				gz_chePaiBC.m_totalPiScore = m_pi_pool;
				gz_chePaiBC.m_totalMangScore = m_mang_pool;
				gz_chePaiBC.m_totalAddScore = t_totalAddScore;
				memcpy(gz_chePaiBC.m_userStatus, m_user_status, sizeof(gz_chePaiBC.m_userStatus));
				memcpy(gz_chePaiBC.m_qiPaiStatus, m_qipai_status, sizeof(gz_chePaiBC.m_qiPaiStatus));
				
				notify_desk_seat_no_playing_user(gz_chePaiBC);
				notify_lookon(gz_chePaiBC);
			}
			else
			{
				CheXuanS2CPlayerChePaiBC chePaiBC;
				chePaiBC.m_pos = CHEXUAN_INVALID_POS;
				chePaiBC.m_totalPiScore = m_pi_pool;
				chePaiBC.m_totalMangScore = m_mang_pool;
				chePaiBC.m_totalAddScore = t_totalAddScore;
				memcpy(chePaiBC.m_userStatus, m_user_status, sizeof(chePaiBC.m_userStatus));
				memcpy(chePaiBC.m_qiPaiStatus, m_qipai_status, sizeof(chePaiBC.m_qiPaiStatus));
				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 0)
					{
						continue;
					}
					memcpy(chePaiBC.m_handCards[i], m_hand_cards[i], sizeof(Lint) * 4);
					memcpy(chePaiBC.m_cardsType[i], m_hand_cards_type[i], sizeof(Lint) * 2);
				}

				notify_desk(chePaiBC);
				notify_lookon(chePaiBC);
			}

			finish_round();
		}
		return true;
	}

	/* ������Ҵ��� */
	bool on_event_player_cuo_pai(Lint pos)
	{
		//���Ǵ��ƽ׶β�����
		if (GAME_PLAY_CUO_PAI != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_cuo_pai() Error!!! this play status is not GAME_PLAY_CUO_PAI...,deskId=[%d], pos=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_status);
			return false;
		}
		//
		if (m_qipai_status[pos] != 0 || m_user_status[pos] != 1)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_cuo_pai() Error!!! this player can not cuo pai!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}
		if (m_cuopai_status[pos] == 1)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_cuo_pai() Error!!! this player has cuo pai!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::on_event_player_cuo_pai() Run... Player cuo pai... deskId=[%d], pos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos);

		//��Ǹ�����Ѿ����ƹ�
		m_cuopai_status[pos] = 1;

		//���ƹ�ս
		if (m_isLookOnDarkResult)
		{
			CheXuanS2CPlayerCuoPaiBC cuoPaiBC;
			cuoPaiBC.m_pos = pos;
			cuoPaiBC.m_cuoCard = m_hand_cards[pos][3];
			notify_desk_playing_user(cuoPaiBC);
			
			CheXuanS2CPlayerCuoPaiBC gz_cuoPaiBC;
			gz_cuoPaiBC.m_pos = pos;
			notify_desk_seat_no_playing_user(gz_cuoPaiBC);
			notify_lookon(gz_cuoPaiBC);
		}
		else
		{
			CheXuanS2CPlayerCuoPaiBC cuoPaiBC;
			cuoPaiBC.m_pos = pos;
			cuoPaiBC.m_cuoCard = m_hand_cards[pos][3];
			notify_desk(cuoPaiBC);
			notify_lookon(cuoPaiBC);
		}

		//�����˴��ƽ���
		if (has_opt_over(GAME_PLAY_CUO_PAI))
		{
			//�����ƾ�״̬Ϊ����ע
			set_play_status(GAME_PLAY_ADD_SCORE);
			//�ҳ����������������λ��
			Lint t_thredCards[CHEXUAN_PLAYER_COUNT];
			memset(t_thredCards, 0, sizeof(Lint)*CHEXUAN_PLAYER_COUNT);
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

				t_thredCards[i] = m_hand_cards[i][3];
			}
			m_turn_first_speak_pos = m_gamelogic.getMaxCardPos(t_thredCards, m_player_count, m_player_count, m_dang_player_pos);
			m_curPos = m_turn_first_speak_pos;

			bool t_flag = false;
			//���������Ѿ�����...������һ��û���õ���� �� ����������� λ��
			if (m_qiao_status[m_curPos] == 1 || m_oneNotQiao || m_OnlyQiaoAndXiu)
			{
				Lint t_curPos = m_curPos;
				m_curPos = CHEXUAN_INVALID_POS;
				int t_tmpPos = m_curPos;
				//for (int i = t_curPos; i < m_player_count + t_curPos && !m_oneNotQiao; ++i)
				for (int i = t_curPos; i < m_player_count + t_curPos; ++i)  //(Ren:2019-02-25)
				{
					t_tmpPos = i % m_player_count;
					if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0)
					{
						continue;
					}

					//���������עֵ(Ren��2019-02-25)
					Lint t_maxAddSocre = 0;
					for (int i = 0; i < m_player_count; ++i)
					{
						if (m_total_add_score[i] > t_maxAddSocre)
						{
							t_maxAddSocre = m_total_add_score[i];
						}
					}

					//�ж�û�� ������ �״γ��ֵ��������ͣ�Ҫ������ҷ��Ͳ�����ʾ��Ϣ
					bool t_isCanSanHua = false;
					bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
					t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[t_tmpPos];
					//if (m_qiao_status[t_tmpPos] == 0 || (t_isCanSanHua))	// && m_qiao_status[t_tmpPos] == 0))
					//if ((m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || t_isCanSanHua) //(Ren:2019-02-25)
					if (
						(m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(m_OnlyQiaoAndXiu && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
						(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID) ||
						t_isCanSanHua) //(Ren:2019-03-15)
					{
						m_turn_round_opt[t_tmpPos] = 1;
						m_curPos = t_tmpPos;
						t_flag = true;
						break;
					}
					m_curPos = CHEXUAN_INVALID_POS;
				}
			}

			//�ҵ���һ��û����/�������͵����λ�ã���������������Ϣ
			if (t_flag || m_curPos != CHEXUAN_INVALID_POS)
			{
				Lint t_isMeiDaDong = 0;
				calc_opt_score();

				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = m_def_time_3rd_optscore;
				notifyOpt.m_isMeiDaDong = t_isMeiDaDong;
				notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
				for (int i = 0; i < 10; ++i)
				{
					notifyOpt.m_optType[i] = m_allow_opt_type[m_curPos][i];
					notifyOpt.m_optMinScore[i] = m_allow_opt_min_score[m_curPos][i];
					notifyOpt.m_optMaxScore[i] = m_allow_opt_max_score[m_curPos][i];
				}
				notify_desk(notifyOpt);
				notify_lookon(notifyOpt);

				//��ʼ��¼����ʱ
				m_play_status_time.Now();
				return true;
			}
			else
			{
				//����û����â �� ��â���������â����â�ı���
				m_xiuMangCount = 0;
				m_zouMangCount = 0;

				set_play_status(GAME_PLAY_CHE_PAI);
				CheXuanS2CNotifyChePai chePai;
				chePai.m_restTime = m_def_time_chepai;
				memcpy(chePai.m_userStatus, m_user_status, sizeof(chePai.m_userStatus));
				memcpy(chePai.m_chePaiStatus, m_chepai_status, sizeof(chePai.m_chePaiStatus));
				memcpy(chePai.m_qiPaiStatus, m_qipai_status, sizeof(chePai.m_qiPaiStatus));
				memcpy(chePai.m_qiaoStatus, m_qiao_status, sizeof(chePai.m_qiaoStatus));
				notify_desk(chePai);
				notify_lookon(chePai);

				m_play_status_time.Now();
				return true;
			}
		}

		return true;
	}

	/* �������ѡ���� */
	bool on_event_player_opt_mo_fen(Lint pos, Lint opt)
	{
		if (pos != m_dang_player_pos)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_opt_mo_fen() Error!!! opt mo fen is not dang pos...,deskId=[%d], pos=[%d], dangPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_dang_player_pos);
			return false;
		}

		if (m_turn_count != 0)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_opt_mo_fen() Error!!! opt mo fen is not dang pos...,deskId=[%d], pos=[%d], dangPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_dang_player_pos);
			return false;
		}

		if (m_selectMoFen != -1)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_opt_mo_fen() Error!!! has opt mo fen ...,deskId=[%d], pos=[%d], selectMoFen=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_selectMoFen);
			return false;
		}

		//ѡ���Ĭ��
		m_selectMoFen = opt;

		//����ҷ��ز������֪ͨ
		CheXuanS2CPlayerOptMoFen moFen;
		moFen.m_pos = pos;
		moFen.m_optMo = opt;
		notify_desk(moFen);
		notify_lookon(moFen);

		//������ҿ�ѡ�����ͷ���
		bool t_flag = false;
		//���������Ѿ�����...������һ��û���õ���� �� ����������� λ��
		if (m_qiao_status[m_curPos] == 1 || m_oneNotQiao || m_OnlyQiaoAndXiu)
		{
			Lint t_curPos = m_curPos;
			m_curPos = CHEXUAN_INVALID_POS;
			int t_tmpPos = CHEXUAN_INVALID_POS;
			//for (int i = t_curPos; i < m_player_count + t_curPos && !m_oneNotQiao; ++i)
			for (int i = t_curPos; i < m_player_count + t_curPos; ++i)  //(Ren:2019-02-25)
			{
				t_tmpPos = i % m_player_count;
				if (m_user_status[t_tmpPos] != 1 || m_qipai_status[t_tmpPos] != 0)
				{
					continue;
				}

				//���������עֵ(Ren��2019-02-25)
				Lint t_maxAddSocre = 0;
				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_total_add_score[i] > t_maxAddSocre)
					{
						t_maxAddSocre = m_total_add_score[i];
					}
				}

				//�ж�û�� ������ �״γ��ֵ��������ͣ�Ҫ������ҷ��Ͳ�����ʾ��Ϣ
				bool t_isCanSanHua = false;
				bool t_isSanHua = m_gamelogic.checkSanHuaType(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
				bool t_isSanHuaDouble = m_gamelogic.checkSanHuaDouble(m_hand_cards[t_tmpPos], m_hand_cards_count[t_tmpPos]);
				t_isCanSanHua = t_isSanHua && !t_isSanHuaDouble && !m_san_hua_status[t_tmpPos];
				//if (m_qiao_status[t_tmpPos] == 0 || (t_isCanSanHua))	// && m_qiao_status[t_tmpPos] == 0))
				//if ((m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) || t_isCanSanHua) //(Ren:2019-02-25)
				if (
					(m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
					(m_OnlyQiaoAndXiu && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
					(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_total_add_score[t_tmpPos] < t_maxAddSocre) ||
					(!m_OnlyQiaoAndXiu && !m_oneNotQiao && m_qiao_status[t_tmpPos] == 0 && m_turn_opt_type[m_turn_count][t_tmpPos] == ADD_OPT_INVALID) ||
					t_isCanSanHua) //(Ren:2019-03-15)
				{
					m_turn_round_opt[t_tmpPos] = 1;
					m_curPos = t_tmpPos;
					t_flag = true;
					break;
				}
				m_curPos = CHEXUAN_INVALID_POS;
			}
		}

		//�ҵ���һ��û����/�������͵����λ�ã���������������Ϣ
		if (t_flag || m_curPos != CHEXUAN_INVALID_POS)
		{
			//������������Ĳ���
			calc_opt_score();

			CheXuanS2CNotifyPlayerOpt notifyOpt;
			notifyOpt.m_pos = m_curPos;
			notifyOpt.m_restTime = 0;
			notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
			for (int i = 0; i < 10; ++i)
			{
				notifyOpt.m_optType[i] = m_allow_opt_type[m_curPos][i];
				notifyOpt.m_optMinScore[i] = m_allow_opt_min_score[m_curPos][i];
				notifyOpt.m_optMaxScore[i] = m_allow_opt_max_score[m_curPos][i];
			}
			notify_desk(notifyOpt);
			notify_lookon(notifyOpt);
			//notify_club_ower_lookon(notifyOpt);

			//��ʼ��¼����ʱ
			//m_play_status_time.Now();

			return true;
		}
		//�����ˣ�����ֻ��һ���˲���������
		else
		{
			++m_turn_count;
			send_card();
			return true;
		}

		return true;
	}

	/* �������ѡ���Զ����� */
	bool on_event_player_select_auto_opt(Lint pos, Lint autoOptType)
	{
		if (autoOptType < 0 || autoOptType > 3)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_select_auto_opt() Error!!! Auto opt type is invalid...,deskId=[%d], pos=[%d], autoOptType=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, autoOptType);

			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::on_event_player_select_auto_opt() Run... deskId=[%d], pos=[%d], autoOptType=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, autoOptType);

		m_playerAutoOpt[pos] = autoOptType;
		
		//֪ͨ��Ҳ������
		CheXuanS2CPlayerSelectAutoOptRet optAutoRet;
		optAutoRet.m_pos = pos;
		optAutoRet.m_autoOptType = autoOptType;
		notify_user(optAutoRet, pos);

		return true;
	}

	/* �������ѡ��վ������²��� */
	bool on_event_player_stand_or_seat(Lint pos, Lint standOrSeat)
	{
		//����Ѿ�������վ���������
		if (m_player_stand_up[pos] == 1 || m_player_stand_up[pos] == 3)
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_stand_or_seat() Error!!! Player has select stand or seat!!!,deskId=[%d], pos=[%d], m_player_stand_up=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_player_stand_up[pos]);

			return false;
		}
		//��Ҳ����ظ�����
		if ((m_player_stand_up[pos] == 0 && standOrSeat == 0) || (m_player_stand_up[pos] == 2 && standOrSeat == 1))
		{
			LLOG_ERROR("CheXuanGameHandler::on_event_player_stand_or_seat() Error!!! Player can not repeat opt stand or seat!!!,deskId=[%d], pos=[%d], m_player_stand_up=[%d], standOrSeat=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_player_stand_up[pos], standOrSeat);

			return false;
		}

		//����
		if (standOrSeat == 0)
		{
			m_player_stand_up[pos] = 3;
		}
		//վ��
		else if (standOrSeat == 1)
		{
			m_player_stand_up[pos] = 1;
		}
		CheXuanS2CPlayerSelectStandOrSeatRet ret;
		ret.m_pos = pos;
		ret.m_standOrSeat = m_player_stand_up[pos];
		notify_user(ret, pos);

		return true;
	}

	//�������
	void calc_score(Lint finishRoundType, Lint* maxWinnerScore, Lint* maxWinnerPos)
	{
		LLOG_ERROR("CheXuanGameHandler::calc_score() Run... this round finish type=[%d]... round=[%d/%d], deskId=[%d]",
			finishRoundType, m_round_offset, m_round_limit, m_desk ? m_desk->GetDeskId() : 0);

		switch (finishRoundType)
		{
		//��������
		case 0:
			if (m_isClearPool)
			{
				calc_score_nomal();
			}
			else
			{
				calc_score_nomal_noClear();

				if (m_round_offset == m_round_limit - 1)
				{
					LLOG_ERROR("CheXuanGameHandler::calc_score() Run... clear all score... deskId=[%d]", 
						m_desk ? m_desk->GetDeskId() : 0);
					clear_all_score();
				}
			}
			break;
		//�ݹ��ӡ���â
		case 1:
		case 2:
			if (m_isClearPool)
			{
				calc_score_xiuMang();
			}
			else
			{
				calc_score_xiuMang_noClear();

				if (m_round_offset == m_round_limit - 1)
				{
					LLOG_ERROR("CheXuanGameHandler::calc_score() Run... clear all score...");
					clear_all_score();
				}
			}
			break;
			//��һ�������������ơ���â
		case 3:
		case 4:
			if (m_isClearPool || m_round_offset == m_round_limit - 1)
			{
				calc_score_zouMang();
			}
			else
			{
				calc_score_zouMang_noClear();

				if (m_round_offset == m_round_limit - 1)
				{
					LLOG_ERROR("CheXuanGameHandler::calc_score() Run... clear all score...");
					clear_all_score();
				}
			}
			break;
		default:
			break;
		}

		Lint t_maxWinner = 0;
		Lint t_maxWinnerPos = CHEXUAN_INVALID_POS;
		//����ÿ����ұ��ֵõķ�
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 0) continue;

			m_player_score[i] = m_total_bo_score[i] - m_total_bo_score_after_bobo[i];
		}

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			if (m_player_score[i] > t_maxWinner)
			{
				t_maxWinner = m_player_score[i];
				t_maxWinnerPos = i;
			}
		}
		*maxWinnerScore = t_maxWinner;
		*maxWinnerPos = t_maxWinnerPos;
	}

	//�������������Ӯ��ע������Ƥ�֡�â������
	void calc_common_score()
	{
		//ѡ�����ķ���ַⶥѡ�
		//ÿ��ÿ�������ע������ = �Ʊ��Լ�����������ע��������ҷ�ֵ���䵽�ֺ�㲻����֣���ֻ��ڱ��ƽ������û�
		Lint t_playerLoseMaxScore[CHEXUAN_PLAYER_COUNT];
		memcpy(t_playerLoseMaxScore, m_total_add_score, sizeof(t_playerLoseMaxScore));
		LLOG_ERROR("CheXuanGameHandler::calc_common_score() Run... deskId=[%d], m_isLongXinFeiMaxScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_isLongXinFeiMaxScore);
		if (m_isLongXinFeiMaxScore)
		{
			memset(t_playerLoseMaxScore, 0, sizeof(t_playerLoseMaxScore));
			for (int i = 0; i < m_real_player_count; ++i)
			{
				for (int j = 0; j < i; ++j)
				{
					//i���֮ǰ�����ֻҪ����i�ľ͸���һ��i��������ķ���
					if (
						(m_handCardsTypeInfo[j].m_firstType > m_handCardsTypeInfo[i].m_firstType && m_handCardsTypeInfo[j].m_secondType >= m_handCardsTypeInfo[i].m_secondType) ||
						(m_handCardsTypeInfo[j].m_firstType >= m_handCardsTypeInfo[i].m_firstType && m_handCardsTypeInfo[j].m_secondType > m_handCardsTypeInfo[i].m_secondType)
						)
					{
						if (m_total_add_score[m_handCardsTypeInfo[j].m_userIndex] > t_playerLoseMaxScore[m_handCardsTypeInfo[i].m_userIndex])
						{
							t_playerLoseMaxScore[m_handCardsTypeInfo[i].m_userIndex] = m_total_add_score[m_handCardsTypeInfo[j].m_userIndex];
						}
					}
				}
			}

			//������Ҿ����µķ����ٴθ���������������
			Lint t_playerRealPos = CHEXUAN_INVALID_POS;
			for (int i = 0; i < m_real_player_count; ++i)
			{
				t_playerRealPos = m_handCardsTypeInfo[i].m_userIndex;
				t_playerLoseMaxScore[t_playerRealPos] = (t_playerLoseMaxScore[t_playerRealPos] > m_total_add_score[t_playerRealPos]) ? m_total_add_score[t_playerRealPos] : t_playerLoseMaxScore[t_playerRealPos];
			}
		}

		//ÿ�������α���
		for (int i = 0; i < m_real_player_count - 1; ++i)
		{
			//ÿ���˸���������ұ��ƣ���Ӯ��ע����
			for (int j = i + 1; j < m_real_player_count; ++j)
			{
				Lint t_winPos = CHEXUAN_INVALID_POS;
				Lint t_losPos = CHEXUAN_INVALID_POS;
				Lint t_pingPos1 = CHEXUAN_INVALID_POS;
				Lint t_pingPos2 = CHEXUAN_INVALID_POS;

				//i > j
				if (
					(m_handCardsTypeInfo[i].m_firstType > m_handCardsTypeInfo[j].m_firstType && m_handCardsTypeInfo[i].m_secondType >= m_handCardsTypeInfo[j].m_secondType) ||
					(m_handCardsTypeInfo[i].m_firstType >= m_handCardsTypeInfo[j].m_firstType && m_handCardsTypeInfo[i].m_secondType > m_handCardsTypeInfo[j].m_secondType)
					)
				{
					t_winPos = m_handCardsTypeInfo[i].m_userIndex;
					t_losPos = m_handCardsTypeInfo[j].m_userIndex;
				}
				// i < j
				else if (
					(m_handCardsTypeInfo[i].m_firstType < m_handCardsTypeInfo[j].m_firstType && m_handCardsTypeInfo[i].m_secondType <= m_handCardsTypeInfo[j].m_secondType) ||
					(m_handCardsTypeInfo[i].m_firstType <= m_handCardsTypeInfo[j].m_firstType && m_handCardsTypeInfo[i].m_secondType < m_handCardsTypeInfo[j].m_secondType)
					)
				{
					t_losPos = m_handCardsTypeInfo[i].m_userIndex;
					t_winPos = m_handCardsTypeInfo[j].m_userIndex;
				}
				// i == j
				else
				{
					t_pingPos1 = i;
					t_pingPos2 = j;
				}

				//�ֳ���С���ƴ�����Ӯ��С��ҵ���ע����
				if (t_winPos != CHEXUAN_INVALID_POS && t_losPos != CHEXUAN_INVALID_POS &&
					t_pingPos1 == CHEXUAN_INVALID_POS && t_pingPos2 == CHEXUAN_INVALID_POS)
				{
					//Ӯ����ҷ���(֮ǰ�߼�)
					/*
					if (m_total_add_score[t_losPos] >= m_total_bo_score_after_bobo[t_winPos] && m_total_add_score[t_losPos] > 0)
					{
						m_total_add_score[t_losPos] -= m_total_bo_score_after_bobo[t_winPos];
						m_total_bo_score[t_winPos] += m_total_bo_score_after_bobo[t_winPos];
					}
					else if (m_total_add_score[t_losPos] < m_total_bo_score_after_bobo[t_winPos] && m_total_add_score[t_losPos] > 0)
					{
						m_total_bo_score[t_winPos] += m_total_add_score[t_losPos];
						m_total_add_score[t_losPos] = 0;
					}
					*/
					//Ӯ����ҷ���(�������ķ���ַⶥ���߼�)
					if (t_playerLoseMaxScore[t_losPos] >= m_total_bo_score_after_bobo[t_winPos] && t_playerLoseMaxScore[t_losPos] > 0)
					{
						t_playerLoseMaxScore[t_losPos] -= m_total_bo_score_after_bobo[t_winPos];
						m_total_add_score[t_losPos] -= m_total_bo_score_after_bobo[t_winPos];
						m_total_bo_score[t_winPos] += m_total_bo_score_after_bobo[t_winPos];
					}
					else if (t_playerLoseMaxScore[t_losPos] < m_total_bo_score_after_bobo[t_winPos] && t_playerLoseMaxScore[t_losPos] > 0)
					{
						m_total_bo_score[t_winPos] += t_playerLoseMaxScore[t_losPos];
						m_total_add_score[t_losPos] -= t_playerLoseMaxScore[t_losPos];
						t_playerLoseMaxScore[t_losPos] = 0;
					}

				}
				//���ƽ��ƽ�֣�˭Ҳ��Ӯ˭����ע����
				else
				{

				}
			}

			//���α���ӮƤ��â�����λ��
			Lint t_turnWinPos = m_handCardsTypeInfo[i].m_userIndex;

			//������δ���Ƶ���ұ��ƽ����󣬿�ʼӮ���ƣ��������⣩���Ƥ
			if (m_shuai_count > 0)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 1 || t_turnWinPos == i) continue;

					if (m_qipai_score[i] >= m_total_bo_score_after_bobo[t_turnWinPos] && m_qipai_score[i] > 0)
					{
						m_pi_pool -= m_total_bo_score_after_bobo[t_turnWinPos];
						m_qipai_score[i] -= m_total_bo_score_after_bobo[t_turnWinPos];
						m_total_bo_score[t_turnWinPos] += m_total_bo_score_after_bobo[t_turnWinPos];
						m_player_pi_score[t_turnWinPos] += m_total_bo_score_after_bobo[t_turnWinPos];
					}
					else if (m_qipai_score[i] < m_total_bo_score_after_bobo[t_turnWinPos] && m_qipai_score[i] > 0)
					{
						m_pi_pool -= m_qipai_score[i];
						m_total_bo_score[t_turnWinPos] += m_qipai_score[i];
						m_player_pi_score[t_turnWinPos] += m_qipai_score[i];
						m_qipai_score[i] = 0;
					}
				}
			}

			//Ӯ�����������˵�Ƥ�󣬿�ʼӮһ��â����
			if (m_mang_pool > 0)
			{
				if (m_mang_pool >= m_total_bo_score_after_bobo[t_turnWinPos])
				{
					m_mang_pool -= m_total_bo_score_after_bobo[t_turnWinPos];
					m_total_bo_score[t_turnWinPos] += m_total_bo_score_after_bobo[t_turnWinPos];
					m_player_mang_score[t_turnWinPos] += m_total_bo_score_after_bobo[t_turnWinPos];
				}
				else if (m_mang_pool < m_total_bo_score_after_bobo[t_turnWinPos])
				{
					m_total_bo_score[t_turnWinPos] += m_mang_pool;
					m_player_mang_score[t_turnWinPos] += m_mang_pool;
					m_mang_pool = 0;
				}
			}
		}

		if (m_real_player_count > 0)
		{
			//�����������ң���û�����Ƶ���С�����ҪӮÿ��������ҵ�Ƥһ��
			Lint t_tureMinWinPos = m_handCardsTypeInfo[m_real_player_count - 1].m_userIndex;

			//��С�����Ӯÿ���������Ƥһ��
			if (m_shuai_count > 0)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 1) continue;

					if (m_qipai_score[i] >= m_total_bo_score_after_bobo[t_tureMinWinPos] && m_qipai_score[i] > 0)
					{
						m_pi_pool -= m_total_bo_score_after_bobo[t_tureMinWinPos];
						m_qipai_score[i] -= m_total_bo_score_after_bobo[t_tureMinWinPos];
						m_total_bo_score[t_tureMinWinPos] += m_total_bo_score_after_bobo[t_tureMinWinPos];
						m_player_pi_score[t_tureMinWinPos] += m_total_bo_score_after_bobo[t_tureMinWinPos];
					}
					else if (m_qipai_score[i] < m_total_bo_score_after_bobo[t_tureMinWinPos] && m_qipai_score[i] > 0)
					{
						m_pi_pool -= m_qipai_score[i];
						m_total_bo_score[t_tureMinWinPos] += m_qipai_score[i];
						m_player_pi_score[t_tureMinWinPos] += m_qipai_score[i];
						m_qipai_score[i] = 0;
					}
				}
			}
		}

		//�����ע�ķ���ʣ�࣬�����û�
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_total_add_score[i] > 0 && m_user_status[i] == 1 && m_qipai_status[i] == 0)
			{
				m_total_bo_score[i] += m_total_add_score[i];
				m_total_add_score[i] = 0;
			}
		}
	}

	//���â��Ƥ�� ����С����
	void calc_score_nomal()
	{
		//�������������Ӯ��ע������Ƥ�֡�â������
		calc_common_score();

		/*
		  ���â��Ƥ��â����ʣ�����ƽ�������ھ���ң�����������������ҡ���ս��ң���
		  ����ֵĸ��ھ����������С������ھ�����������ƣ�������С��������ͬ���ƣ�����ң�
		  �����ֶ��������С��ң�����ָ��뵲����Զ����ң�
		*/

		//���뱾����Ϸʵ���������������ơ�������ң�
		Lint t_playerCount = calc_real_player_count();

		//���â����ƽ�ֺ���ֵ����λ�ã�����С����ʱ���뵲����Զ�����λ��
		Lint t_getYuMangPos = findMinCardsAndFarDangPos_QiPai();

		//���Ƥ��ƽ�ֺ���ֵ����λ�ã��뵲����Զ�����λ��
		Lint t_getYuPiPos = GetRealPrePos(m_dang_player_pos);
		

		//â������ʣ�ࣺƽ�������ھ���ң�����������������ң�������ֵĸ��ھ����������С����ң������ֶ��������С��ң�����ָ��뵲����Զ�����
		if (m_mang_pool > 0)
		{
			//����ƽ�ֵ�����
			Lint t_yushu = 0;

			//����ƽ�ֵ��˻ط���
			Lint t_tuihui = 0;

			if (t_playerCount != 0)
			{
				t_yushu = m_mang_pool % t_playerCount;
				t_tuihui = (m_mang_pool - t_yushu) / t_playerCount;
			}
			
			for (int i = 0; i < t_playerCount; ++i)
			{
				m_total_bo_score[i] += t_tuihui;
				m_player_mang_score[i] += t_tuihui;
			}

			//����ƽ��â���������뵲���������С�����
			m_total_bo_score[t_getYuMangPos] += t_yushu;
			m_player_mang_score[t_getYuMangPos] += t_yushu;

			//���â����
			m_mang_pool = 0;
		}

		//Ƥ����ʣ�ࣺƤ��ʣ�����ƽ�������ھ���ң�����������������ң��������ʱ��������������Ƶ���ң���������������ʱ������ָ��뵲����Զ����ң�
		/*
		if (m_pi_pool > 0)
		{
			//����ƽ�ֵ�����
			Lint t_yushu = 0;

			//����ƽ�ֵ��˻ط���
			Lint t_tuihui = 0;

			if (t_playerCount != 0)
			{
				t_yushu = m_pi_pool % t_playerCount;
				t_tuihui = (m_pi_pool - t_yushu) / t_playerCount;
			}

			for (int i = 0; i < t_playerCount; ++i)
			{
				m_total_bo_score[i] += t_tuihui;
				m_player_pi_score[i] += t_tuihui;
			}

			//������������������ƣ�����ַָ�����������
			t_getYuPiPos = m_qipai_last_pos == CHEXUAN_INVALID_POS ? t_getYuPiPos : m_qipai_last_pos;

			//����ƽ��Ƥ�������뵲���������С�����
			m_total_bo_score[t_getYuPiPos] += t_yushu;
			m_player_pi_score[t_getYuPiPos] += t_yushu;

			//���Ƥ��
			m_pi_pool = 0;
		}
		*/
	}

	//�����â����Ƥ�� �������
	void calc_score_nomal_noClear()
	{
		calc_common_score();
	}

	//���â����Ƥ�� ��â���
	void calc_score_xiuMang()
	{
		//�ָ�ÿ����ҵķ��������ֿ�ʼ֮ǰ�ķ���
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			m_total_bo_score[i] = m_total_bo_score_after_bobo[i];
			m_player_mang_score[i] = 0;
			m_player_pi_score[i] = 0;
			m_total_add_score[i] = 0;
		}
		m_mang_pool = 0;
		m_pi_pool = 0;
	}

	//�����â����Ƥ�� ��â���
	void calc_score_xiuMang_noClear()
	{
		//�ݵ�����û��Լ���ע����
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			if (m_qipai_status[i] == 0)
			{
				m_total_bo_score[i] += m_total_add_score[i];
				m_total_add_score[i] = 0;
			}
		}

		//��������û��Լ���ע�ķ���
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			if (m_qipai_status[i] == 1)
			{
				if (m_pi_pool - m_qipai_score[i] >= 0 && m_pi_pool >= 0)
				{
					m_total_bo_score[i] += m_qipai_score[i];
					m_pi_pool -= m_qipai_score[i];
					m_player_pi_score[i] += m_qipai_score[i];
				}
				else if (m_pi_pool - m_qipai_score[i] < 0 && m_pi_pool >= 0)
				{
					m_total_bo_score[i] += m_pi_pool;
					m_player_pi_score[i] += m_pi_pool;
					m_pi_pool = 0;
				}
			}
		}
	}

	//���â����Ƥ�� ��â���
	void calc_score_zouMang()
	{
		//���ֿ�ʼ�����в�����Ϸ�������������m_user_state[i] == 1
		Lint t_playerCount = 0;

		//�ݵ��������
		Lint t_xiuCount = 0;

		//�����������
		Lint t_shuaiCount = 0;

		//��â��ҵ�λ��
		Lint t_zouMangPos = CHEXUAN_INVALID_POS;

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1) ++t_playerCount;
			if (m_qipai_status[i] == 1) ++t_shuaiCount;

			if (m_user_status[i] != 1 || m_qipai_status[i] != 0 || m_turn_opt_type[m_turn_count][i] == ADD_OPT_XIU) continue;

			t_zouMangPos = i;
		}

		//��â���Ӯ�����������Ƥ
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1 || m_qipai_status[i] != 1 || i == t_zouMangPos) continue;

			if (m_qipai_score[i] >= m_total_bo_score_after_bobo[t_zouMangPos] && m_qipai_score[i] > 0)
			{
				m_pi_pool -= m_total_bo_score_after_bobo[t_zouMangPos];
				m_qipai_score[i] -= m_total_bo_score_after_bobo[t_zouMangPos];
				m_total_bo_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
				m_player_pi_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
			}
			else if (m_qipai_score[i] < m_total_bo_score_after_bobo[t_zouMangPos] && m_qipai_score[i] > 0)
			{
				m_pi_pool -= m_qipai_score[i];
				m_total_bo_score[t_zouMangPos] += m_qipai_score[i];
				m_player_pi_score[t_zouMangPos] += m_qipai_score[i];
				m_qipai_score[i] = 0;
			}
		}
		
		//��â��Ӯһ��â��
		if (m_mang_pool >= m_total_bo_score_after_bobo[t_zouMangPos] && m_mang_pool > 0)
		{
			m_mang_pool -= m_total_bo_score_after_bobo[t_zouMangPos];
			m_total_bo_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
			m_player_mang_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
		}
		else if (m_mang_pool < m_total_bo_score_after_bobo[t_zouMangPos] && m_mang_pool > 0)
		{
			m_total_bo_score[t_zouMangPos] += m_mang_pool;
			m_player_mang_score[t_zouMangPos] += m_mang_pool;
			m_mang_pool = 0;
		}

		//��â����û��Լ���ע�ķ���
		m_total_bo_score[t_zouMangPos] += m_total_add_score[t_zouMangPos];
		m_total_add_score[t_zouMangPos] = 0;



		//����ʵ�ʲ�������
		Lint t_realPlayerCount = calc_real_player_count();

		//���â����ƽ�ֺ���ֵ����λ�ã�����ָ��뵲����Զ�����
		Lint t_getYuMangPos = GetRealPrePos(m_dang_player_pos);

		//���Ƥ��ƽ�ֺ���ֵ����λ�ã��뵲����Զ�����λ��
		Lint t_getYuPiPos = GetRealPrePos(m_dang_player_pos);

		//ʣ���â����â����ʣ�����ƽ�������ھ���ң�����������������ҡ���ս��ң�������ָ��뵲����Զ����ң�
		if (m_mang_pool > 0)
		{
			//����ƽ�ֵ�����
			Lint t_yushu = 0;

			//����ƽ�ֵ��˻ط���
			Lint t_tuihui = 0;

			if (t_realPlayerCount != 0)
			{
				t_yushu = m_mang_pool % t_realPlayerCount;
				t_tuihui = (m_mang_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_mang_score[i] += t_tuihui;
			}

			//����ƽ��â���������뵲���������С�����
			m_total_bo_score[t_getYuMangPos] += t_yushu;
			m_player_mang_score[t_getYuMangPos] += t_yushu;

			//���â����
			m_mang_pool = 0;
		}

		//ʣ�µ�Ƥ:ƽ�������ھ���ң�����������������ң��������ʱ���������뵲����Զ����ң�
		/*
		if (m_pi_pool > 0)
		{
			//����ƽ�ֵ�����
			Lint t_yushu = 0;

			//����ƽ�ֵ��˻ط���
			Lint t_tuihui = 0;

			if (t_realPlayerCount != 0)
			{
				t_yushu = m_pi_pool % t_realPlayerCount;
				t_tuihui = (m_pi_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_pi_score[i] += t_tuihui;
			}

			//������������������ƣ�����ַָ�����������
			t_getYuPiPos = m_qipai_last_pos == CHEXUAN_INVALID_POS ? t_getYuPiPos : m_qipai_last_pos;

			//����ƽ��Ƥ�������뵲���������С�����
			m_total_bo_score[t_getYuPiPos] += t_yushu;
			m_player_pi_score[t_getYuPiPos] += t_yushu;

			//���Ƥ��
			m_pi_pool = 0;
		}
		*/
	}

	//�����â����Ƥ�� ��â���
	void calc_score_zouMang_noClear()
	{
		//���ֿ�ʼ�����в�����Ϸ�������������m_user_state[i] == 1
		Lint t_playerCount = 0;

		//�ݵ��������
		Lint t_xiuCount = 0;

		//�����������
		Lint t_shuaiCount = 0;

		//��â��ҵ�λ��
		Lint t_zouMangPos = CHEXUAN_INVALID_POS;

		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1) ++t_playerCount;
			if (m_qipai_status[i] == 1) ++t_shuaiCount;

			if (m_user_status[i] != 1 || m_qipai_status[i] != 0 || m_turn_opt_type[m_turn_count][i] == ADD_OPT_XIU) continue;

			t_zouMangPos = i;
		}

		//��â���Ӯ�����������Ƥ
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1 || m_qipai_status[i] != 1 || i == t_zouMangPos) continue;

			if (m_qipai_score[i] >= m_total_bo_score_after_bobo[t_zouMangPos] && m_qipai_score[i] > 0)
			{
				m_pi_pool -= m_total_bo_score_after_bobo[t_zouMangPos];
				m_qipai_score[i] -= m_total_bo_score_after_bobo[t_zouMangPos];
				m_total_bo_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
				m_player_pi_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
			}
			else if (m_qipai_score[i] < m_total_bo_score_after_bobo[t_zouMangPos] && m_qipai_score[i] > 0)
			{
				m_pi_pool -= m_qipai_score[i];
				m_total_bo_score[t_zouMangPos] += m_qipai_score[i];
				m_player_pi_score[t_zouMangPos] += m_qipai_score[i];
				m_qipai_score[i] = 0;
			}
		}

		//��â��Ӯһ��â��
		if (m_mang_pool >= m_total_bo_score_after_bobo[t_zouMangPos] && m_mang_pool > 0)
		{
			m_mang_pool -= m_total_bo_score_after_bobo[t_zouMangPos];
			m_total_bo_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
			m_player_mang_score[t_zouMangPos] += m_total_bo_score_after_bobo[t_zouMangPos];
		}
		else if (m_mang_pool < m_total_bo_score_after_bobo[t_zouMangPos] && m_mang_pool > 0)
		{
			m_total_bo_score[t_zouMangPos] += m_mang_pool;
			m_player_mang_score[t_zouMangPos] += m_mang_pool;
			m_mang_pool = 0;
		}

		//��â����û��Լ���ע�ķ���
		m_total_bo_score[t_zouMangPos] += m_total_add_score[t_zouMangPos];
		m_total_add_score[t_zouMangPos] = 0;
	}

	//������з�,�ڲ����ÿ�ֵַ�ѡ���£����һ��������з���
	void clear_all_score()
	{
		//����ʵ�ʲ�������
		Lint t_realPlayerCount = calc_real_player_count();

		//���â����ƽ�ֺ���ֵ����λ�ã�����С����ʱ���뵲����Զ�����λ��
		Lint t_getYuMangPos = findMaxLoseAndFarDangPos_QiShen(m_accum_score);

		//���Ƥ��ƽ�ֺ���ֵ����λ�ã��뵲����Զ�����λ��
		Lint t_getYuPiPos = GetRealPrePos(m_dang_player_pos);

		
		//â������ʣ��:â����ʣ�����ƽ�������ھ���ң�����������������ң�������ֵĸ�ǰ�漸�����������ң������ֶ����������ң�����ָ����һ���뵲����Զ����ң�
		if (m_mang_pool > 0)
		{
			//����ƽ�ֵ�����
			Lint t_yushu = 0;

			//����ƽ�ֵ��˻ط���
			Lint t_tuihui = 0;

			if (t_realPlayerCount > 0)
			{
				t_yushu = m_mang_pool % t_realPlayerCount;
				t_tuihui = (m_mang_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_mang_score[i] += t_tuihui;
				m_tui_mang_score[i] = t_tuihui;
			}

			//����ֵĸ�ǰ�漸�����������ң���������������ң��������ֶ����������ң�����ָ������뵲����Զ����ң�
			m_total_bo_score[t_getYuMangPos] += t_yushu;
			m_player_mang_score[t_getYuMangPos] += t_yushu;
			m_tui_mang_score[t_getYuMangPos] += t_yushu;

			//���û�����â���������λ��
			if (t_getYuMangPos != CHEXUAN_INVALID_POS && m_user_status[t_getYuMangPos] == 2 && t_yushu > 0)
			{
				m_last_get_yu_mang = t_getYuMangPos;
			}

			//���â����
			m_mang_pool = 0;
		}

		//Ƥ����ʣ��:Ƥ��ʣ�����ƽ�������ھ���ң�����������������ң�������ָ��뵲����Զ����ң�
		/*
		if (m_pi_pool > 0)
		{
			//����ƽ�ֵ�����
			Lint t_yushu = 0;

			//����ƽ�ֵ��˻ط���
			Lint t_tuihui = 0;

			if (t_realPlayerCount > 0)
			{
				t_yushu = m_pi_pool % t_realPlayerCount;
				t_tuihui = (m_pi_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_pi_score[i] += t_tuihui;
			}

			//����ƽ��Ƥ�������뵲���������С�����
			m_total_bo_score[t_getYuPiPos] += t_yushu;
			m_player_pi_score[t_getYuPiPos] += t_yushu;

			//���Ƥ��
			m_pi_pool = 0;
		}
		*/
	}

	//�ƾ��н�ɢ���������ע����
	void jie_san_clear_all_score()
	{
		//����ʵ�ʲ�������
		Lint t_realPlayerCount = calc_real_player_count();

		//�����˻ָ������ֿ�����ǰ���ܷ���
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			m_total_bo_score[i] = m_total_bo_score_after_bobo[i];
			m_player_mang_score[i] = 0;
			m_player_pi_score[i] = 0;
			m_total_add_score[i] = 0;
		}

		//â��Ƥ�ػָ����Ͼֽ���ʱ��â��Ƥ��
		m_pi_pool = m_per_pi_pool;
		m_mang_pool = m_per_mang_pool;

		//���â����ƽ�ֺ���ֵ����λ�ã�����С����ʱ���뵲����Զ�����λ��
		Lint t_getYuMangPos = findMaxLoseAndFarDangPos_QiShen(m_accum_score);
		//���Ƥ��ƽ�ֺ���ֵ����λ�ã��뵲����Զ�����λ��
		Lint t_getYuPiPos = GetRealPrePos(m_dang_player_pos);

		//â����ʣ�����ƽ�������ھ���ң�����������������ң�������ֵĸ�ǰ�漸�����������ң������ֶ����������ң�����ָ������뵲����Զ����ң�
		if (m_mang_pool > 0)
		{
			//����ƽ�ֵ�����
			Lint t_yushu = 0;

			//����ƽ�ֵ��˻ط���
			Lint t_tuihui = 0;

			if (t_realPlayerCount != 0)
			{
				t_yushu = m_mang_pool % t_realPlayerCount;
				t_tuihui = (m_mang_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_mang_score[i] += t_tuihui;
				m_tui_mang_score[i] = t_tuihui;
			}

			//����ֵĸ�ǰ�漸�����������ң���������������ң��������ֶ����������ң�����ָ������뵲����Զ����ң�
			m_total_bo_score[t_getYuMangPos] += t_yushu;
			m_player_mang_score[t_getYuMangPos] += t_yushu;
			m_tui_mang_score[t_getYuMangPos] += t_yushu;

			//���û�����â���������λ��
			if (t_getYuMangPos != CHEXUAN_INVALID_POS && m_user_status[t_getYuMangPos] == 2 && t_yushu > 0)
			{
				m_last_get_yu_mang = t_getYuMangPos;
			}

			//���â����
			m_mang_pool = 0;
		}

		//Ƥ��ʣ�����ƽ�������ھ���ң�����������������ң�������ָ��뵲����Զ�����
		/*
		if (m_pi_pool > 0)
		{
			//����ƽ�ֵ�����
			Lint t_yushu = 0;

			//����ƽ�ֵ��˻ط���
			Lint t_tuihui = 0;

			if (t_realPlayerCount != 0)
			{
				t_yushu = m_pi_pool % t_realPlayerCount;
				t_tuihui = (m_pi_pool - t_yushu) / t_realPlayerCount;
			}

			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				m_total_bo_score[i] += t_tuihui;
				m_player_pi_score[i] += t_tuihui;
				m_tui_pi_score[i] = t_tuihui;
			}

			//����ƽ��Ƥ�������뵲���������С�����
			m_total_bo_score[t_getYuPiPos] += t_yushu;
			m_player_pi_score[t_getYuPiPos] += t_yushu;
			m_tui_pi_score[t_getYuPiPos] += t_yushu;

			//���Ƥ��
			m_pi_pool = 0;
		}
		*/

		//����ÿ����ұ��ֵõķ�
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 0) continue;

			m_player_score[i] = m_total_bo_score[i] - m_total_bo_score_after_bobo[i];
		}
	}

	/* ��¼������Ϣ�����ݿ� */
	void add_round_log(Lint* player_score, Lint win_pos)
	{
		Lint t_user_status[CHEXUAN_PLAYER_COUNT];
		memcpy(t_user_status, m_user_status, sizeof(t_user_status));
		if (m_last_get_yu_mang != CHEXUAN_INVALID_POS)
		{
			t_user_status[m_last_get_yu_mang] = 1;
		}

		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, player_score, t_user_status, win_pos, m_video.m_Id);
		}

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 0) continue;

			m_accum_score[i] += player_score[i];
		}
	}

	//����
	void finish_round(Lint jieSan = 0)
	{
		if (m_desk == NULL || m_desk->m_vip == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::finish_round() Error!!! m_desk is NULL or m_desk->m_vip is NULL");
			return;
		}

		//��ҽ�ɢ���䣬����С������Ϣ
		if (0 != jieSan)
		{
			LLOG_ERROR("CheXuanGameHandler::finish_round() Run... Start Jie San ... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());

			//�������գ����ɢʱ��Ҫ���һ�£�������һ��
			if (!m_isClearPool)
			{
				//����յ��淨�½�ɢ
				jie_san_clear_all_score();

				//[0]����  [1]��â��  [2]��Ƥ  [3][4]��Ч����  [5]ʣ�����
				std::vector<Lint> t_handCards[CHEXUAN_PLAYER_COUNT];
				Lint t_optType = 100;
				for (int i = 0; i < m_player_count; ++i)
				{
					t_handCards[i].clear();
					if (m_user_status[i] != 1) continue;

					// 100:����  101:����   102������   103������   104������   105�����ֽ�ɢ���â����Ƥ��
					t_optType = 105;

					//��¼����
					t_handCards[i].push_back(t_optType);

					//��¼�˵�â����Ƥ
					t_handCards[i].push_back(m_tui_mang_score[i]);
					t_handCards[i].push_back(m_tui_pi_score[i]);

					//��¼���ƣ�����4�ŵ���0����
					for (int j = 0; j < CHEXUAN_HAND_CARDS_COUNT - 2; ++j)
					{
						t_handCards[i].push_back(0);
					}

					//ʣ�����
					t_handCards[i].push_back(m_rest_can_start_bo_score[i] + m_total_bo_score[i]);
				}

				//��������
				m_video.setHandCards(m_player_count, t_handCards);

				//����ط�
				m_video.addUserScore(m_accum_score);
				if (m_desk && m_desk->m_vip)
				{
					VideoSave();
				}

				add_round_log(m_player_score, CHEXUAN_INVALID_POS);
			}

			CheXuanS2CDrawResult jiesanRet;
			jiesanRet.m_isFinish = 1;
			jiesanRet.m_type = 1;
			jiesanRet.m_curCircle = m_round_offset + 1;
			jiesanRet.m_curMaxCircle = m_round_limit;
			jiesanRet.m_lastGetYuMang = m_last_get_yu_mang;
			memcpy(jiesanRet.m_score, m_player_score, sizeof(jiesanRet.m_score));
			memcpy(jiesanRet.m_totleScore, m_accum_score, sizeof(m_accum_score));
			memcpy(jiesanRet.m_curBoBoScore, m_total_bo_score, sizeof(jiesanRet.m_curBoBoScore));
			memcpy(jiesanRet.m_userStatus, m_user_status, sizeof(jiesanRet.m_userStatus));
			memcpy(jiesanRet.m_playerPiScore, m_tui_pi_score, sizeof(jiesanRet.m_playerPiScore));
			memcpy(jiesanRet.m_playerMangScore, m_tui_mang_score, sizeof(jiesanRet.m_playerMangScore));
			memcpy(jiesanRet.m_handCardsCount, m_hand_cards_count, sizeof(jiesanRet.m_handCardsCount));
			for (int i = 0; i < SHUANGSHENG_PLAY_USER_COUNT && m_desk; ++i)
			{
				memcpy(jiesanRet.m_handCards[i], m_hand_cards[i], sizeof(Lint) * m_hand_cards_count[i]);
				if (m_desk->m_user[i])
				{
					jiesanRet.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
					jiesanRet.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
					jiesanRet.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
				}
			}
			notify_desk(jiesanRet);
			notify_lookon(jiesanRet);

			LLOG_ERROR("Game Jie San Over... desk_id=[%d], desk_status=[%d]", m_desk ? m_desk->GetDeskId() : 0, m_desk->getDeskState());

			set_desk_state(DESK_WAIT);

			//������Ϸ
			if (m_desk) m_desk->HanderGameOver(jieSan);
			return;
		}

		//���
		Lint t_maxWinnerScore = 0;
		Lint t_maxWinnerPos = CHEXUAN_INVALID_POS;
		calc_score(m_finish_round_type, &t_maxWinnerScore, &t_maxWinnerPos);

		//�������ķΣ��Լ������ע������
		Lint t_longXinFeiScore[CHEXUAN_PLAYER_COUNT];
		memset(t_longXinFeiScore, 0, sizeof(t_longXinFeiScore));

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			t_longXinFeiScore[i] = m_player_score[i] - m_player_mang_score[i] - m_player_pi_score[i];
		}

		//�����䡢Ӯ��ƽ�ı��
		Lint t_notAllowMangScore = 0;
		Lint t_loseOrWin[CHEXUAN_PLAYER_COUNT];		// 0:Ĭ��״̬  -1������   1��ʤ��   2��ƽ��
		memset(t_loseOrWin, 0, sizeof(t_loseOrWin));

		for (Lint i = 0; i < m_player_count; ++i)
		{
			t_notAllowMangScore = 0;
			if (m_user_status[i] != 1) continue;

			t_notAllowMangScore = m_player_score[i] - m_player_mang_score[i];

			if (t_notAllowMangScore > 0) t_loseOrWin[i] = 1;
			else if (t_notAllowMangScore < 0) t_loseOrWin[i] = -1;
			else t_loseOrWin[i] = 2;
		}

		std::vector<Lint> t_handCards[CHEXUAN_PLAYER_COUNT];
		Lint t_optType = 100;
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			// 100:����  101:����   102������   103������   104������   105�����ֽ�ɢ���â����Ƥ��   106����â(ûѡ����â)
			t_optType = 100;

			//�޹��ӣ�����ʾ����
			if (m_finish_round_type == 1 || m_finish_round_type == 2) t_optType = 101;
			//����
			else if (m_finish_round_type == 4 && m_qipai_status[i] == 0) t_optType = 102;
			//����(ûѡ��â)
			else if (m_finish_round_type == 3 && m_qipai_status[i] == 0) t_optType = 106;
			//����
			else if (m_qipai_status[i] == 1) t_optType = 103;
			//����
			else if (m_qipai_status[i] == 2) t_optType = 104;
			//����
			else t_optType = 100;

			//��¼����
			t_handCards[i].push_back(t_optType);

			//��¼���ƣ�����4�ŵ���0����
			for (int j = 0; j < m_hand_cards_count[i]; ++j)
			{
				t_handCards[i].push_back(m_hand_cards[i][j]);
			}
			for (int j = 0; j < CHEXUAN_HAND_CARDS_COUNT - m_hand_cards_count[i]; ++j)
			{
				t_handCards[i].push_back(0);
			}

			//ʣ�����
			t_handCards[i].push_back(m_rest_can_start_bo_score[i] + m_total_bo_score[i]);
		}

		//��������
		m_video.setHandCards(m_player_count, t_handCards);

		//����ط�
		m_video.addUserScore(m_accum_score);
		if (m_desk && m_desk->m_vip && m_desk->m_vip->m_reset == 0)
		{
			VideoSave();
		}

		//�ж���һ���Ƿ���Ҫ��������
		Lint t_standUpStatus[CHEXUAN_PLAYER_COUNT];
		memset(t_standUpStatus, 0, sizeof(t_standUpStatus));
		Lint t_needAddMangScore = 0;

		//֪ͨ�ͻ�����Ҫ�����һ�ֵ���ҵ�״̬
		memset(m_player_bobo_add_di_pi, 0, sizeof(m_player_bobo_add_di_pi));
		for (int i = 0; i < m_player_count; ++i)
		{
			m_desk->m_handlerPlayerStatus[i] = 0;
			if (m_user_status[i] == 0) continue;

			//������Ҫ����Ĳ����֣�â����âע����һ����ע��
			if (GetRealNextPos(m_dang_player_pos) == i)
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 3 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 10 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 20 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 5 + 1;
			}
			else
			{
				if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 1 + 1;
				else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 5 + 1;
				else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 2 + 1;
			}

			//����������ֵ�Ƿ����Ԥ��ֵ
			cheack_player_coins_warn(i);

			//��������Ƿ���Ҫ����â�ı�־
			//m_is_need_add_shouMang[i] = m_isShouMang ? 1 : 0;

			t_needAddMangScore = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];

			/*
			//�������ʣ��Ĳ����� + ������󲧲��� < ��Ҫ��â��������
			if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore || m_user_status[i] == 2)
			{
				t_standUpStatus[i] = 1;
				//m_user_status[i] = 2;
				//m_desk->m_handlerPlayerStatus[i] = 2;

				//�����ֲ���,�Ѿ�վ��
				m_player_stand_up[i] = 2;
				m_desk->m_standUpPlayerSeatDown[i] = 2;

				LLOG_ERROR("CheXuanGameHandler::finish_round() Run..., Player has no enought bo score, So stand up, deskId = [%d], pos = [%d], needAddMang=[%d], totalBoScore=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, i, t_needAddMangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i]);
			}
			else
			{
				m_desk->m_handlerPlayerStatus[i] = 1;
			}
			*/
			if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore ||		//�ֲ����¾���â����
				m_user_status[i] == 2 ||														//�Ѿ����ƾ���������ս��
				m_player_stand_up[i] == 1 ||													//�����Ѿ�����վ���
				m_player_stand_up[i] == 2 ||													//�Ѿ�վ�����ҵ�
				m_player_stand_up[i] == 3
				)
			{
				t_standUpStatus[i] = 1;
				//m_user_status[i] = 2;
			}
		}

		//���Ӿ���
		increase_round();

		add_round_log(m_player_score, t_maxWinnerPos);

		//��¼��һ��Ƥ�غ�â��
		m_per_pi_pool = m_pi_pool;
		m_per_mang_pool = m_mang_pool;


		//���ƹ�ս
		if (m_isLookOnDarkResult)
		{
			//���Ʋ���ʾ��
			if (m_isQiPaiNotShowCards)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] == 0) continue;

					CheXuanS2CDrawResult rest;
					rest.m_nextRoundTime = m_def_time_finish;
					rest.m_firstSendCardsPos = m_frist_send_card;
					rest.m_dangPlayerPos = m_dang_player_pos;
					rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
					rest.m_curCircle = m_round_offset;
					rest.m_curMaxCircle = m_round_limit;
					rest.m_piPool = m_pi_pool;
					rest.m_mangPool = m_mang_pool;
					rest.m_state = m_finish_round_type;
					rest.m_isFinish = 0;
					rest.m_lastGetYuMang = m_last_get_yu_mang;
					memcpy(rest.m_userStatus, m_user_status, sizeof(rest.m_userStatus));
					memcpy(rest.m_qiPaiStatus, m_qipai_status, sizeof(rest.m_qiPaiStatus));
					memcpy(rest.m_standUpStatus, t_standUpStatus, sizeof(rest.m_standUpStatus));
					memcpy(rest.m_handCardsCount, m_hand_cards_count, sizeof(rest.m_handCardsCount));
					memcpy(rest.m_playerAddScore, t_longXinFeiScore, sizeof(rest.m_playerAddScore));
					memcpy(rest.m_playerPiScore, m_player_pi_score, sizeof(rest.m_playerPiScore));
					memcpy(rest.m_playerMangScore, m_player_mang_score, sizeof(rest.m_playerMangScore));
					memcpy(rest.m_curBoBoScore, m_total_bo_score, sizeof(rest.m_curBoBoScore));
					memcpy(rest.m_score, m_player_score, sizeof(rest.m_score));
					memcpy(rest.m_totleScore, m_accum_score, sizeof(rest.m_totleScore));
					memcpy(rest.m_cardsType, m_hand_cards_type, sizeof(rest.m_cardsType));
					memcpy(rest.m_lossOrWin, t_loseOrWin, sizeof(rest.m_lossOrWin));

					for (Lint j = 0; j < m_player_count; ++j)
					{
						if (m_user_status[j] == 0) continue;

						if (m_desk && m_desk->m_user[j])
						{
							rest.m_userIds[j] = m_desk->m_user[j]->GetUserDataId();
							rest.m_userNames[j] = m_desk->m_user[j]->GetUserData().m_nike;
							rest.m_headUrl[j] = m_desk->m_user[j]->GetUserData().m_headImageUrl;
						}

						if (m_qipai_status[j] == 0 || m_qipai_status[j] == 2 || i == j)
						{
							memcpy(rest.m_handCards[j], m_hand_cards[j], sizeof(Lint) * m_hand_cards_count[j]);
						}
						else
						{
							for (Lint k = 0; k < m_hand_cards_count[j]; ++k)
							{
								if (k == 0 || k == 1)
								{
									rest.m_handCards[j][k] = 0;
								}
								else
								{
									rest.m_handCards[j][k] = m_hand_cards[j][k];
								}
							}
						}
					}
					
					notify_user(rest, i);
				}
			}
			else
			{
				CheXuanS2CDrawResult rest;
				rest.m_nextRoundTime = m_def_time_finish;
				rest.m_firstSendCardsPos = m_frist_send_card;
				rest.m_dangPlayerPos = m_dang_player_pos;
				rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
				rest.m_curCircle = m_round_offset;
				rest.m_curMaxCircle = m_round_limit;
				rest.m_piPool = m_pi_pool;
				rest.m_mangPool = m_mang_pool;
				rest.m_state = m_finish_round_type;
				rest.m_isFinish = 0;
				rest.m_lastGetYuMang = m_last_get_yu_mang;
				memcpy(rest.m_userStatus, m_user_status, sizeof(rest.m_userStatus));
				memcpy(rest.m_qiPaiStatus, m_qipai_status, sizeof(rest.m_qiPaiStatus));
				memcpy(rest.m_standUpStatus, t_standUpStatus, sizeof(rest.m_standUpStatus));
				memcpy(rest.m_handCardsCount, m_hand_cards_count, sizeof(rest.m_handCardsCount));
				memcpy(rest.m_playerAddScore, t_longXinFeiScore, sizeof(rest.m_playerAddScore));
				memcpy(rest.m_playerPiScore, m_player_pi_score, sizeof(rest.m_playerPiScore));
				memcpy(rest.m_playerMangScore, m_player_mang_score, sizeof(rest.m_playerMangScore));
				memcpy(rest.m_curBoBoScore, m_total_bo_score, sizeof(rest.m_curBoBoScore));
				memcpy(rest.m_score, m_player_score, sizeof(rest.m_score));
				memcpy(rest.m_totleScore, m_accum_score, sizeof(rest.m_totleScore));
				memcpy(rest.m_cardsType, m_hand_cards_type, sizeof(rest.m_cardsType));
				memcpy(rest.m_lossOrWin, t_loseOrWin, sizeof(rest.m_lossOrWin));
				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] == 0) continue;

					memcpy(rest.m_handCards[i], m_hand_cards[i], sizeof(Lint) * m_hand_cards_count[i]);

					if (m_desk && m_desk->m_user[i])
					{
						rest.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
						rest.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
						rest.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
					}
				}
				notify_desk_playing_user(rest);
				notify_desk_seat_no_playing_user(rest);
			}

			//�㲥��ս��Ұ���
			CheXuanS2CDrawResult gz_rest;
			gz_rest.m_nextRoundTime = m_def_time_finish;
			gz_rest.m_firstSendCardsPos = m_frist_send_card;
			gz_rest.m_dangPlayerPos = m_dang_player_pos;
			gz_rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
			gz_rest.m_curCircle = m_round_offset;
			gz_rest.m_curMaxCircle = m_round_limit;
			gz_rest.m_piPool = m_pi_pool;
			gz_rest.m_mangPool = m_mang_pool;
			gz_rest.m_state = m_finish_round_type;
			gz_rest.m_isFinish = 0;
			gz_rest.m_lastGetYuMang = m_last_get_yu_mang;
			memcpy(gz_rest.m_userStatus, m_user_status, sizeof(gz_rest.m_userStatus));
			memcpy(gz_rest.m_qiPaiStatus, m_qipai_status, sizeof(gz_rest.m_qiPaiStatus));
			memcpy(gz_rest.m_standUpStatus, t_standUpStatus, sizeof(gz_rest.m_standUpStatus));
			memcpy(gz_rest.m_handCardsCount, m_hand_cards_count, sizeof(gz_rest.m_handCardsCount));
			memcpy(gz_rest.m_playerAddScore, t_longXinFeiScore, sizeof(gz_rest.m_playerAddScore));
			memcpy(gz_rest.m_playerPiScore, m_player_pi_score, sizeof(gz_rest.m_playerPiScore));
			memcpy(gz_rest.m_playerMangScore, m_player_mang_score, sizeof(gz_rest.m_playerMangScore));
			memcpy(gz_rest.m_curBoBoScore, m_total_bo_score, sizeof(gz_rest.m_curBoBoScore));
			memcpy(gz_rest.m_score, m_player_score, sizeof(gz_rest.m_score));
			memcpy(gz_rest.m_totleScore, m_accum_score, sizeof(gz_rest.m_totleScore));
			memcpy(gz_rest.m_lossOrWin, t_loseOrWin, sizeof(gz_rest.m_lossOrWin));
			
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 0) continue;

				if (m_desk && m_desk->m_user[i])
				{
					gz_rest.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
					gz_rest.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
					gz_rest.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
				}
			}

			notify_lookon(gz_rest);

			if (m_isQiPaiNotShowCards)
			{
				notify_desk_seat_no_playing_user(gz_rest);
			}
		}
		//������սģʽ
		else
		{
			//���Ʋ���ʾ��
			if (m_isQiPaiNotShowCards)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] == 0) continue;

					CheXuanS2CDrawResult rest;
					rest.m_nextRoundTime = m_def_time_finish;
					rest.m_firstSendCardsPos = m_frist_send_card;
					rest.m_dangPlayerPos = m_dang_player_pos;
					rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
					rest.m_curCircle = m_round_offset;
					rest.m_curMaxCircle = m_round_limit;
					rest.m_piPool = m_pi_pool;
					rest.m_mangPool = m_mang_pool;
					rest.m_state = m_finish_round_type;
					rest.m_isFinish = 0;
					rest.m_lastGetYuMang = m_last_get_yu_mang;
					memcpy(rest.m_userStatus, m_user_status, sizeof(rest.m_userStatus));
					memcpy(rest.m_qiPaiStatus, m_qipai_status, sizeof(rest.m_qiPaiStatus));
					memcpy(rest.m_standUpStatus, t_standUpStatus, sizeof(rest.m_standUpStatus));
					memcpy(rest.m_handCardsCount, m_hand_cards_count, sizeof(rest.m_handCardsCount));
					memcpy(rest.m_playerAddScore, t_longXinFeiScore, sizeof(rest.m_playerAddScore));
					memcpy(rest.m_playerPiScore, m_player_pi_score, sizeof(rest.m_playerPiScore));
					memcpy(rest.m_playerMangScore, m_player_mang_score, sizeof(rest.m_playerMangScore));
					memcpy(rest.m_curBoBoScore, m_total_bo_score, sizeof(rest.m_curBoBoScore));
					memcpy(rest.m_score, m_player_score, sizeof(rest.m_score));
					memcpy(rest.m_totleScore, m_accum_score, sizeof(rest.m_totleScore));
					memcpy(rest.m_cardsType, m_hand_cards_type, sizeof(rest.m_cardsType));
					memcpy(rest.m_lossOrWin, t_loseOrWin, sizeof(rest.m_lossOrWin));

					for (int j = 0; j < m_player_count; ++j)
					{
						if (m_user_status[j] == 0) continue;

						if (m_desk && m_desk->m_user[j])
						{
							rest.m_userIds[j] = m_desk->m_user[j]->GetUserDataId();
							rest.m_userNames[j] = m_desk->m_user[j]->GetUserData().m_nike;
							rest.m_headUrl[j] = m_desk->m_user[j]->GetUserData().m_headImageUrl;
						}

						if (m_qipai_status[j] == 0 || m_qipai_status[j] == 2 || i == j)
						{
							memcpy(rest.m_handCards[j], m_hand_cards[j], sizeof(Lint) * m_hand_cards_count[j]);
						}
						else
						{
							for (Lint k = 0; k < m_hand_cards_count[j]; ++k)
							{
								if (k == 0 || k == 1)
								{
									rest.m_handCards[j][k] = 0;
								}
								else
								{
									rest.m_handCards[j][k] = m_hand_cards[j][k];
								}
							}
						}
					}
					notify_user(rest, i);
				}

				//��ս���
				CheXuanS2CDrawResult gz_rest;
				gz_rest.m_nextRoundTime = m_def_time_finish;
				gz_rest.m_firstSendCardsPos = m_frist_send_card;
				gz_rest.m_dangPlayerPos = m_dang_player_pos;
				gz_rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
				gz_rest.m_curCircle = m_round_offset;
				gz_rest.m_curMaxCircle = m_round_limit;
				gz_rest.m_piPool = m_pi_pool;
				gz_rest.m_mangPool = m_mang_pool;
				gz_rest.m_state = m_finish_round_type;
				gz_rest.m_isFinish = 0;
				gz_rest.m_lastGetYuMang = m_last_get_yu_mang;
				memcpy(gz_rest.m_userStatus, m_user_status, sizeof(gz_rest.m_userStatus));
				memcpy(gz_rest.m_qiPaiStatus, m_qipai_status, sizeof(gz_rest.m_qiPaiStatus));
				memcpy(gz_rest.m_standUpStatus, t_standUpStatus, sizeof(gz_rest.m_standUpStatus));
				memcpy(gz_rest.m_handCardsCount, m_hand_cards_count, sizeof(gz_rest.m_handCardsCount));
				memcpy(gz_rest.m_playerAddScore, t_longXinFeiScore, sizeof(gz_rest.m_playerAddScore));
				memcpy(gz_rest.m_playerPiScore, m_player_pi_score, sizeof(gz_rest.m_playerPiScore));
				memcpy(gz_rest.m_playerMangScore, m_player_mang_score, sizeof(gz_rest.m_playerMangScore));
				memcpy(gz_rest.m_curBoBoScore, m_total_bo_score, sizeof(gz_rest.m_curBoBoScore));
				memcpy(gz_rest.m_score, m_player_score, sizeof(gz_rest.m_score));
				memcpy(gz_rest.m_totleScore, m_accum_score, sizeof(gz_rest.m_totleScore));
				memcpy(gz_rest.m_cardsType, m_hand_cards_type, sizeof(gz_rest.m_cardsType));
				memcpy(gz_rest.m_lossOrWin, t_loseOrWin, sizeof(gz_rest.m_lossOrWin));

				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] == 0) continue;

					if (m_desk && m_desk->m_user[i])
					{
						gz_rest.m_userIds[i] = m_desk->m_user[i]->GetUserDataId();
						gz_rest.m_userNames[i] = m_desk->m_user[i]->GetUserData().m_nike;
						gz_rest.m_headUrl[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
					}

					if (m_qipai_status[i] == 0 || m_qipai_status[i] == 2)
					{
						memcpy(gz_rest.m_handCards[i], m_hand_cards[i], sizeof(Lint) * m_hand_cards_count[i]);
					}
					else
					{
						for (Lint k = 0; k < m_hand_cards_count[i]; ++k)
						{
							if (k == 0 || k == 1)
							{
								gz_rest.m_handCards[i][k] = 0;
							}
							else
							{
								gz_rest.m_handCards[i][k] = m_hand_cards[i][k];
							}
						}
					}
				}

				//�㲥��ս���
				notify_lookon(gz_rest);

				//�㲥�᳤��ս
				//notify_club_ower_lookon(gz_rest);
			}
			else
			{
				CheXuanS2CDrawResult rest;
				rest.m_nextRoundTime = m_def_time_finish;
				rest.m_firstSendCardsPos = m_frist_send_card;
				rest.m_dangPlayerPos = m_dang_player_pos;
				rest.m_type = m_round_offset == m_round_limit ? 1 : 0;
				rest.m_curCircle = m_round_offset;
				rest.m_curMaxCircle = m_round_limit;
				rest.m_piPool = m_pi_pool;
				rest.m_mangPool = m_mang_pool;
				rest.m_state = m_finish_round_type;
				rest.m_isFinish = 0;
				rest.m_lastGetYuMang = m_last_get_yu_mang;
				memcpy(rest.m_userStatus, m_user_status, sizeof(rest.m_userStatus));
				memcpy(rest.m_qiPaiStatus, m_qipai_status, sizeof(rest.m_qiPaiStatus));
				memcpy(rest.m_standUpStatus, t_standUpStatus, sizeof(rest.m_standUpStatus));
				memcpy(rest.m_handCardsCount, m_hand_cards_count, sizeof(rest.m_handCardsCount));
				memcpy(rest.m_playerAddScore, t_longXinFeiScore, sizeof(rest.m_playerAddScore));
				memcpy(rest.m_playerPiScore, m_player_pi_score, sizeof(rest.m_playerPiScore));
				memcpy(rest.m_playerMangScore, m_player_mang_score, sizeof(rest.m_playerMangScore));
				memcpy(rest.m_curBoBoScore, m_total_bo_score, sizeof(rest.m_curBoBoScore));
				memcpy(rest.m_score, m_player_score, sizeof(rest.m_score));
				memcpy(rest.m_totleScore, m_accum_score, sizeof(rest.m_totleScore));
				memcpy(rest.m_cardsType, m_hand_cards_type, sizeof(rest.m_cardsType));
				memcpy(rest.m_lossOrWin, t_loseOrWin, sizeof(rest.m_lossOrWin));

				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] == 0) continue;

					memcpy(rest.m_handCards[i], m_hand_cards[i], sizeof(Lint) * m_hand_cards_count[i]);

					if (m_desk && m_desk->m_user[i])
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
				//notify_club_ower_lookon(rest);
			}
		}
		
		set_desk_state(DESK_WAIT);

		m_desk->HanderGameOver(1);


		//��ҳ���һ����Ϸ��˭���Բμ�
		if (m_desk && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame)
		{
			memset(m_player_bobo_add_di_pi, 0, sizeof(m_player_bobo_add_di_pi));
			for (int i = 0; i < m_player_count; ++i)
			{
				m_desk->m_handlerPlayerStatus[i] = 0;

				if (m_user_status[i] == 0) continue;

				//��������Ƿ���Ҫ����â�ı�־
				//m_is_need_add_shouMang[i] = m_isShouMang ? 1 : 0;

				//������Ҫ����Ĳ����֣�â����âע����һ����ע��
				if (GetNextPos(m_dang_player_pos) == i)
				{
					if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 3 + 1;
					else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 10 + 1;
					else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 20 + 1;
					else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 5 + 1;
				}
				else
				{
					if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 1 + 1;
					else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 5 + 1;
					else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 5 + 1;
					else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 2 + 1;
				}

				t_needAddMangScore = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];

				//�������ʣ��Ĳ����� + ������󲧲��� < ��Ҫ��â��������
				if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
				{
					m_user_status[i] = 2;
					//����desk�ϵĿ��Խ�����һ�ֵ���ұ��
					m_desk->m_handlerPlayerStatus[i] = 2;
					//���������ֵ����
					m_desk->m_standUpPlayerSeatDown[i] = 2;
				}
				else
				{
					m_desk->m_handlerPlayerStatus[i] = 1;
				}

				LLOG_ERROR("CheXuanGameHandler::finish_round() next round player status..., deskId = [%d], pos=[%d], status=[%d] needAddMangScore=[%d], mangSocre=[%d], totalBoScore=[%d], deskStandUp=[%d], deskPlayerStatus=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, i, m_user_status[i], t_needAddMangScore, m_mangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i], m_desk->m_standUpPlayerSeatDown[i], m_desk->m_handlerPlayerStatus[i]);
			}
		}
		//�ǽ�ҳ��������������������
		else
		{
			memset(m_player_bobo_add_di_pi, 0, sizeof(m_player_bobo_add_di_pi));
			for (int i = 0; i < m_player_count; ++i)
			{
				m_desk->m_handlerPlayerStatus[i] = 0;

				if (m_user_status[i] == 0) continue;

				//������Ҫ����Ĳ����֣�â����âע����һ����ע��
				if (GetRealNextPos(m_dang_player_pos) == i)
				{
					if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 3 + 1;
					else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 10 + 1;
					else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 20 + 1;
					else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 5 + 1;
				}
				else
				{
					if (m_diPi == 0) m_player_bobo_add_di_pi[i] = 1 + 1;
					else if (m_diPi == 1) m_player_bobo_add_di_pi[i] = 5 + 1;
					else if (m_diPi == 2) m_player_bobo_add_di_pi[i] = 5 + 1;
					else if (m_diPi == 3) m_player_bobo_add_di_pi[i] = 2 + 1;
				}

				//��������Ƿ���Ҫ����â�ı�־
				//m_is_need_add_shouMang[i] = m_isShouMang ? 1 : 0;

				t_needAddMangScore = m_is_need_add_shouMang[i] * m_shouMangCount + m_is_need_add_xiuMang[i] * m_xiuMangCount + m_is_need_add_zouMang[i] * m_zouMangCount + m_player_bobo_add_di_pi[i];
			
				//��ұ��ֵ�������
				if (m_player_stand_up[i] == 1)
				{
					//����Ϊ��վ��״̬
					m_player_stand_up[i] = 2;

					//m_user_status[i] = 2;
					//����desk�ϵĿ��Խ�����һ�ֵ���ұ��
					m_desk->m_handlerPlayerStatus[i] = 2;
					//���������ֵ���� / �������ɹ���������ʱ������
					m_desk->m_standUpPlayerSeatDown[i] = 3;
				}
				//��ұ��ֵ�����µ�
				else if (m_player_stand_up[i] == 3)
				{
					//�������ʣ��Ĳ����� + ������󲧲��� < ��Ҫ��â��������
					if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
					{
						//����ʧ��
						m_player_stand_up[i] = 2;

						//m_user_status[i] = 2;
						//����desk�ϵĿ��Խ�����һ�ֵ���ұ��
						m_desk->m_handlerPlayerStatus[i] = 2;
						//���������ֵ����
						m_desk->m_standUpPlayerSeatDown[i] = 2;
					}
					else
					{
						//���³ɹ�
						m_player_stand_up[i] = 0;

						//m_user_status[i] = 1;
						m_desk->m_handlerPlayerStatus[i] = 1;
						m_desk->m_standUpPlayerSeatDown[i] = 1;
					}
				}
				//����δ������
				else
				{
					//�������ʣ��Ĳ����� + ������󲧲��� < ��Ҫ��â��������
					if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] < t_needAddMangScore)
					{
						//ֱ��վ��
						m_player_stand_up[i] = 2;

						//m_user_status[i] = 2;
						//����desk�ϵĿ��Խ�����һ�ֵ���ұ��
						m_desk->m_handlerPlayerStatus[i] = 2;
						//���������ֵ����
						m_desk->m_standUpPlayerSeatDown[i] = 2;
					}
					else if (m_total_bo_score[i] + m_rest_can_start_bo_score[i] > t_needAddMangScore &&
						(m_user_status[i] == 2 || m_player_stand_up[i] == 2))
					{
						//ֱ��վ��
						m_player_stand_up[i] = 2;

						//m_user_status[i] = 2;
						//����desk�ϵĿ��Խ�����һ�ֵ���ұ��
						m_desk->m_handlerPlayerStatus[i] = 2;
						//���������ֵ����
						m_desk->m_standUpPlayerSeatDown[i] = 3;
					}
					else
					{
						m_desk->m_handlerPlayerStatus[i] = 1;
						//m_user_status[i] = 1;
					}
				}

				LLOG_ERROR("CheXuanGameHandler::finish_round() next round player status..., deskId = [%d], pos=[%d], status=[%d] needAddMangScore=[%d], mangSocre=[%d], totalBoScore=[%d], deskStandUp=[%d], deskPlayerStatus=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, i, m_user_status[i], t_needAddMangScore, m_mangScore, m_total_bo_score[i] + m_rest_can_start_bo_score[i], m_desk->m_standUpPlayerSeatDown[i], m_desk->m_handlerPlayerStatus[i]);
			}
		}
	}

};

/* ������Ϸ������ */
struct CheXuanGameHandler : CheXuanGameCore
{
	/* ���캯�� */
	CheXuanGameHandler()
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

	/* �㲥���������������������Ϣ */
	void notify_desk(LMsg &msg) override
	{
		if (NULL == m_desk)
			return;
		m_desk->BoadCast(msg);
	}

	/* �㲥�������ﵱǰ������� */
	void notify_desk_playing_user(LMsg &msg) override
	{
		if (!m_desk) return;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1 && m_desk->m_user[i])
			{
				m_desk->m_user[i]->Send(msg);
			}
		}
	}

	//�㲥����ǰ��������û�в�����Ϸ���
	void notify_desk_seat_no_playing_user(LMsg &msg) override
	{
		if (!m_desk) return;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 2 && m_desk->m_user[i])
			{
				m_desk->m_user[i]->Send(msg);
			}
		}
		m_desk->MHBoadCastDeskSeatingUser(msg);
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

	//�㲥����̬�������
	void notify_desk_seating_user(LMsg &msg)
	{
		if (NULL == m_desk)
		{
			return;
		}
		m_desk->MHBoadCastDeskSeatingUser(msg);
	}

	/* �㲥���ֲ��᳤���ƹ�ս */
	/*void notify_club_ower_lookon(LMsg& msg) override
	{
	if (NULL == m_desk)
	{
	return;
	}
	if (m_desk->m_clubOwerLookOn == 1 && m_desk->m_clubOwerLookOnUser)
	{
	m_desk->MHBoadCastClubOwerLookOnUser(msg);
	}
	}*/

	/* ÿ�ֿ�ʼ*/
	bool startup(Desk *desk) 
	{
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
		m_baseTimes = m_playtype.CheXuanBaseTimes();

		//Сѡ��ס�Ƥ  0��1 / 3     1��5 / 1(5 / 10)     2��5 / 2(5 / 20)
		m_diPi = m_playtype.CheXuanDiPi();

		//Сѡ�����������  0:500   1 : 1000   2 : 2000
		m_maxBoBo = m_playtype.CheXuanMaxBoBo();

		//Сѡ��Ƿ���ר��������
		m_playCount = m_playtype.CheXuanPlayerCount();

		//Сѡ��Ƿ�����̬����  true:����   false:������
		m_isDynamicIn = m_playtype.CheXuanDynamicIn();

		//Сѡ���������â
		m_isZouMang = m_playtype.CheXuanIsZouMang();

		//Сѡ��Ƿ�������â
		m_isXiuMang = m_playtype.CheXuanIsXiuMang();

		//Сѡ��Ƿ���������â
		m_isShouMang = m_playtype.CheXuanIsShouShouMang();

		//Сѡ��Ƿ������â����
		m_isDaMangFanBei = m_playtype.CheXuanIsLianXuDaMangFanBei();

		//Сѡ��Ƿ������Ƿ񷭱��ⶥ
		m_isFanBeiFengDing = m_playtype.CheXuanIsFanBeiFengDing();

		//Сѡ��Ƿ�����ÿ�����Ƥ��â���� 0�������   1�����
		m_isClearPool = m_playtype.CheXuanIsClearPool();

		//Сѡ��Ƿ��ǹ�ս��Ұ��ƽ���  0������   1�����ƽ���
		m_isLookOnDarkResult = m_playtype.CheXuanLookOnDarkResult();

		//Сѡ��Ƿ�Ϊ��Ϸ��ʼ���ֹ���뷿��  0������   1����ʼ���ֹ���뷿��
		m_isStartNotInRoom = m_playtype.CheXuanStartNotInRoom();

		//Сѡ��Ƿ�����Ĭ��  false:������  true:����
		m_isMoFen = m_playtype.CheXuanMoFen();

		//Сѡ��Ƿ�ѡ���׼ӷִ���â��
		m_isFirstAddBigMang = m_playtype.IsFirstAddBigMang();

		//Сѡ��Ƿ�ѡ������ؾ���
		m_isAllowDiJiu = m_playtype.IsAllowDiJiu();

		//Сѡ��Ƿ�ʹ�ü���ģʽ
		m_isFastMode = m_playtype.IsFastMode();

		//Сѡ��Ƿ��ܿ��������Ƶ���
		m_isQiPaiNotShowCards = m_playtype.IsQiPaiNotShowCards();

		//Сѡ��Ƿ���������ķ�Ϊ����ѡ��  false:������   true:����
		m_isLongXinFeiMaxScore = m_playtype.IsLongXinFeiMaxScore();

		//�������𲧲�����
		//Lint t_playerMaxBoBo = 500;

		//����һЩȫ�ֱ���
		if (0 == m_diPi)		//1/3
		{
			m_scoreBase = 3;
			m_scoreTimes = 1;
			m_commonMangCount = 5;
			if (m_isShouMang) m_shouMangCount = 5;
			m_mangScore = 1;

			/*if (m_maxBoBo == 0) t_playerMaxBoBo = 500;
			else if (m_maxBoBo == 1) t_playerMaxBoBo = 1000;
			else if (m_maxBoBo == 2) t_playerMaxBoBo = 2000;*/
		}
		else if (1 == m_diPi)	//5/1
		{
			m_scoreBase = 10;
			m_scoreTimes = 5;
			m_commonMangCount = 10;
			if (m_isShouMang) m_shouMangCount = 10;
			m_mangScore = 5;

			/*if (m_maxBoBo == 0) t_playerMaxBoBo = 1000;
			else if (m_maxBoBo == 1) t_playerMaxBoBo = 2000;
			else if (m_maxBoBo == 2) t_playerMaxBoBo = 3000;*/
		}
		else if (2 == m_diPi)	//5/2
		{
			m_scoreBase = 20;
			m_scoreTimes = 5;
			m_commonMangCount = 20;
			if (m_isShouMang) m_shouMangCount = 20;
			m_mangScore = 5;

			/*if (m_maxBoBo == 0) t_playerMaxBoBo = 2000;
			else if (m_maxBoBo == 1) t_playerMaxBoBo = 4000;
			else if (m_maxBoBo == 2) t_playerMaxBoBo = 6000;*/
		}
		else if (3 == m_diPi)
		{
			m_scoreBase = 5;
			m_scoreTimes = 2;
			m_commonMangCount = 10;
			if (m_isShouMang) m_shouMangCount = 10;
			m_mangScore = 2;
		}

		//����ÿ���˿�����Ĳ�����
		for (int i = 0; i < m_player_count; ++i)
		{
			if (m_desk && m_desk->m_vip && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame == 1)
			{
				//m_rest_can_start_bo_score[i] = m_desk->m_vip->m_coins[i];	//��������ֵ
			}
			else
			{
				m_rest_can_start_bo_score[i] = m_maxBoBo;
				m_record_bobo_score[i] = m_maxBoBo;
			}
		}
		//�����Ƿ����������
		if (m_isAllowDiJiu)
		{
			m_gamelogic.setDiJiu(m_isAllowDiJiu);
		}

		//���ø����׶�Ĭ��ʱ��
		if (m_isFastMode)
		{
			m_def_time_qibobo = DEF_TIME_FAST_QIBOBO;					//�𲧲�
			m_def_time_optscore = DEF_TIME_FAST_OPTSCORE;				//ǰ������ע
			m_def_time_3rd_optscore = DEF_TIME_FAST_3RD_OPTSCORE;		//��������ע
			m_def_time_cuopai = DEF_TIME_FAST_CUOPAI;					//����
			m_def_time_chepai = DEF_TIME_FAST_CHEPAI;					//����
			m_def_time_finish = DEF_TIME_FAST_FINISH;					//�Զ���һ��ʱ�䣬desk�ϵ�ʱ��Ҫͬ����
		}
		else
		{
			m_def_time_qibobo = DEF_TIME_QIBOBO;						//�𲧲�
			m_def_time_optscore = DEF_TIME_OPTSCORE;					//ǰ������ע
			m_def_time_3rd_optscore = DEF_TIME_3RD_OPTSCORE;			//��������ע
			m_def_time_cuopai = DEF_TIME_CUOPAI;						//����
			m_def_time_chepai = DEF_TIME_CHEPAI;						//����
			m_def_time_finish = DEF_TIME_FINISH;						//�Զ���һ��ʱ�䣬desk�ϵ�ʱ��Ҫͬ����
		}
		//m_video.FillDeskBaseInfo(m_desk ? m_desk->GetDeskId() : 0, 0, m_round_limit, l_playtype);
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
		LLOG_DEBUG("CheXuanGameHandler::MHSetDeskPlay() Run... deskId=[%d], play_user_count=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, play_user_count);

		if (!m_desk || !m_desk->m_vip)
		{
			LLOG_ERROR("CheXuanGameHandler::MHSetDeskPlay() Error!!!!  m_desk or m_desk->m_vip is NULL");
			return;
		}

		m_player_count = play_user_count;

		//���õ�һ�����뷿������Ϊ��һ�����Ƶ����
		if (m_round_offset == 0)
		{
			Lint t_first_send_card = CHEXUAN_INVALID_POS;
			for (size_t i = 0; i < m_player_count; ++i)
			{
				if (m_desk->m_user[i] && m_desk->m_playStatus[i] == 1)
				{
					t_first_send_card = i;
					break;
				}

			}
			if (t_first_send_card == CHEXUAN_INVALID_POS)
			{
				return;
			}
			m_frist_send_card = t_first_send_card;
		}

		start_round(player_status);
	}

	/* ��ʱ�� */
	void Tick(LTime& curr)
	{
		//���Ӷ���Ϊ��
		if (m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::Tick() Error... This is desk NULL... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}

		//�����ɢ�У���ʱ����������
		if (m_desk && m_desk->m_resetTime > 0 && m_desk->m_resetUserId > 0)
		{
			if (m_rest_remain_time == -1)
			{
				m_rest_remain_time = curr.Secs() - m_play_status_time.Secs();
			}
			//ˢ�¼�¼ʱ��Ϊ��ǰʱ��
			m_play_status_time.Now();

			LLOG_ERROR("CheXuanGameHandler::Tick() The desk is reseting... So tick do nothing...", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}
		//��ɢ����ʧ�ܴ��¼���ʣ��ʱ��
		if (m_rest_remain_time != -1)
		{
			m_play_status_time.SetSecs(m_play_status_time.Secs() - m_rest_remain_time);
			//���ý�ɢ���붨ʱ����ʱ��
			m_rest_remain_time = -1;
		}

		LLOG_DEBUG("CheXuanGameHandler::Tick() Run... deskId=[%d], playStatus=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_play_status);

		//�������������ֵ����Ԥ��ֵ��ÿ��30s֪ͨһ�����
		if (m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame)
		{
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 0) continue;
				else if (m_is_coins_low[i] != 1) continue;

				//ÿ��30s֪ͨһ�¿ͻ���
				if (curr.Secs() >= m_is_coins_low_time[i].Secs() + DEF_TIME_COINS_LOW)
				{
					//֪ͨ�������ֵ����
					LMsgS2CWarnPointLow warnPoint;
					warnPoint.m_msgType = 0;
					if (m_desk->m_user[i])
					{
						warnPoint.m_userId = m_desk->m_user[i]->GetUserDataId();
						warnPoint.m_nike = m_desk->m_user[i]->m_userData.m_nike;
						warnPoint.m_point = m_rest_can_start_bo_score[i] + m_total_bo_score[i];
						warnPoint.m_warnPoint = m_desk->m_warnScore;
						m_desk->m_user[i]->Send(warnPoint);
					}

					//ˢ�¶�ʱ��
					m_is_coins_low_time[i].Now();
				}
			}
		}

		switch (m_play_status)
		{
		case GAME_PLAY_QIBOBO:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_def_time_qibobo)
			{
				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_user_status[i] != 1 || m_qibobo_status[i] != 0) continue;

					LLOG_ERROR("CheXuanGameHandler::Tick() Qi Bo Bo AutoRun...desk_id=[%d], pos=[%d], GAME_PLAY_QIBOBO time over!",
						m_desk ? m_desk->GetDeskId() : 0, i);
					if (m_is_need_add_bo[i] == 1)
					{
						on_event_player_select_bobo(i, m_min_qi_bo_score[i]);
					}
					else
					{
						on_event_player_select_bobo(i, 0);
					}
				}
			}

			break;
		}
		//��ע�׶�
		case GAME_PLAY_ADD_SCORE:
		{
			//����Զ�����
			if (m_playerAutoOpt[m_curPos] != 0 && m_user_status[m_curPos] == 1 && m_qipai_status[m_curPos] == 0)
			{
				LLOG_DEBUG("AutoPlay() Run... Current player auto play, deskId=[%d], curPos=[%d], autoPlayStatus=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, m_curPos, m_playerAutoOpt[m_curPos]);
				//�Զ����ƣ��������������������������������
				if (m_playerAutoOpt[m_curPos] == 1)
				{
					//����
					if (m_allow_opt_type[m_curPos][ADD_OPT_SANHUA] == ADD_OPT_SANHUA)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_SANHUA, 0);
					}
					//����
					else
					{
						Lint t_shuaiScore = 0;
						if (m_turn_count == 0 && m_total_add_score[m_curPos] == 0)
						{
							if (m_diPi == 1) t_shuaiScore = 5;
							else if (m_diPi == 2) t_shuaiScore = 5;
							else if (m_diPi == 3) t_shuaiScore = 2;
							else t_shuaiScore = 1;
						}

						if (t_shuaiScore > m_total_bo_score[m_curPos])
						{
							t_shuaiScore = m_total_bo_score[m_curPos];
						}

						on_event_player_select_opt(m_curPos, ADD_OPT_SHUAI, t_shuaiScore);
					}
				}
				//�Զ��ݻ�:��������  ������������  �������
				else if (m_playerAutoOpt[m_curPos] == 2)
				{
					//��
					if (m_allow_opt_type[m_curPos][ADD_OPT_XIU] == ADD_OPT_XIU)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_XIU, 0);
					}
					//����
					else if (m_allow_opt_type[m_curPos][ADD_OPT_SANHUA] == ADD_OPT_SANHUA)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_SANHUA, 0);
					}
					//����
					else
					{
						Lint t_shuaiScore = 0;
						if (m_turn_count == 0 && m_total_add_score[m_curPos] == 0)
						{
							if (m_diPi == 1) t_shuaiScore = 5;
							else if (m_diPi == 2) t_shuaiScore = 5;
							else if (m_diPi == 3) t_shuaiScore = 2;
							else t_shuaiScore = 1;
						}

						if (t_shuaiScore > m_total_bo_score[m_curPos])
						{
							t_shuaiScore = m_total_bo_score[m_curPos];
						}

						on_event_player_select_opt(m_curPos, ADD_OPT_SHUAI, t_shuaiScore);
					}
				}
				//�Զ���
				else if (m_playerAutoOpt[m_curPos] == 3)
				{
					//��
					if (m_allow_opt_type[m_curPos][ADD_OPT_FIRST_QIAO] == ADD_OPT_FIRST_QIAO)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_FIRST_QIAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO]);
					}
					//��
					else if (m_allow_opt_type[m_curPos][ADD_OPT_GEN] == ADD_OPT_GEN)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_GEN, m_allow_opt_min_score[m_curPos][ADD_OPT_GEN]);
					}
					//��
					else if (m_allow_opt_type[m_curPos][ADD_OPT_FA_ZHAO] == ADD_OPT_FA_ZHAO)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_FA_ZHAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FA_ZHAO]);
					}
					//��
					else
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_QIAO_BO, m_allow_opt_min_score[m_curPos][ADD_OPT_QIAO_BO]);
					}
				}
			}
			//��ʱ�Զ�����
			else
			{
				Lint t_defTime = (Lint)m_def_time_optscore;
				if (m_turn_count == 2)
				{
					t_defTime = (Lint)m_def_time_3rd_optscore;
				}
				else if (m_turn_count == 0 && m_turn_opt_type[m_turn_count][m_curPos] == ADD_OPT_INVALID && m_curPos == m_turn_first_speak_pos)
				{
					t_defTime = (Lint)m_def_time_optscore + (m_first_shou_mang_time ? 1 : 0) + (m_first_xiu_mang_time ? 1 : 0) + (m_first_zou_mang_time ? 1 : 0);
				}

				if (m_turn_count != 0 && m_after_turn_shuai_delay)
				{
					t_defTime += m_after_turn_shuai_delay;
				}
				
				if (curr.Secs() >= (m_play_status_time.Secs() + t_defTime) && m_curPos != CHEXUAN_INVALID_POS && m_turn_round_opt[m_curPos] == 1 && m_user_status[m_curPos] == 1)
				{
					LLOG_ERROR("CheXuanGameHandler::Tick() Add Score AutoRun...desk_id=[%d], pos=[%d], GAME_PLAY_ADD_SCORE time over!",
						m_desk ? m_desk->GetDeskId() : 0, m_curPos);

					//�Ƿ�Ϊ��һ�ֵ��ҵ�һ�β���
					//bool t_firstTurnSpeak = m_turn_count == 0 && m_curPos == m_turn_first_speak_pos && m_turn_opt_type[m_turn_count][m_curPos] == ADD_OPT_INVALID;
					bool t_firstTurnSpeak = m_allow_opt_type[m_curPos][ADD_OPT_FIRST_QIAO] == ADD_OPT_FIRST_QIAO;

					//�Ƿ�������������
					bool t_isCanSanHua = m_allow_opt_type[m_curPos][ADD_OPT_SANHUA] == ADD_OPT_SANHUA;

					//���ҵ�һ�β���
					if (t_firstTurnSpeak || (m_isMoFen && m_turn_count == 0 && m_curPos == m_dang_player_pos && m_turn_opt_type[m_turn_count][m_curPos] == ADD_OPT_INVALID))
					{
						//ѡ�Ĭ��
						if (!m_isMoFen)
						{
							on_event_player_select_opt(m_curPos, ADD_OPT_FIRST_QIAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO]);
						}
						//ѡ��Ĭ�֣�ѡ�񲻿���
						else if (m_isMoFen && m_selectMoFen == 0)
						{
							on_event_player_select_opt(m_curPos, ADD_OPT_FIRST_QIAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO]);
						}
						//ѡ��Ĭ�֣�ѡ����
						else if (m_isMoFen && m_selectMoFen == 1)
						{
							on_event_player_select_opt(m_curPos, ADD_OPT_FIRST_QIAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO]);
						}
						//ѡ��Ĭ�֣�û�в��� ����/������
						else if (m_isMoFen && m_selectMoFen == -1)
						{
							on_event_player_opt_mo_fen(m_dang_player_pos, 1);
							on_event_player_select_opt(m_curPos, ADD_OPT_FIRST_QIAO, m_allow_opt_min_score[m_curPos][ADD_OPT_FIRST_QIAO]);
						}
					}
					else if (t_isCanSanHua)
					{
						on_event_player_select_opt(m_curPos, ADD_OPT_SANHUA, 0);
					}
					else
					{
						Lint t_shuaiScore = 0;
						//if (m_turn_count == 0 && m_turn_opt_score[0][m_curPos] == 0)
						if (m_turn_count == 0 && m_total_add_score[m_curPos] == 0)
						{
							if (m_diPi == 1) t_shuaiScore = 5;
							else if (m_diPi == 2) t_shuaiScore = 5;
							else if (m_diPi == 3) t_shuaiScore = 2;
							else t_shuaiScore = 1;
						}

						if (t_shuaiScore > m_total_bo_score[m_curPos])
						{
							t_shuaiScore = m_total_bo_score[m_curPos];
						}

						on_event_player_select_opt(m_curPos, ADD_OPT_SHUAI, t_shuaiScore);
					}
				}
			}
			break;
		}
		//���ƽ׶�
		case GAME_PLAY_CUO_PAI:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_def_time_cuopai)
			{
				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

					if (m_cuopai_status[i] == 0 && m_user_status[i] == 1 && m_qipai_status[i] == 0)
					{
						LLOG_ERROR("CheXuanGameHandler::Tick() Cuo Pai AutoRun...desk_id=[%d], pos=[%d], GAME_PLAY_CUO_PAI time over!",
							m_desk ? m_desk->GetDeskId() : 0, i);
						on_event_player_cuo_pai(i);
					}
				}
			}
			break;
		}

		//���ƽ׶�
		case GAME_PLAY_CHE_PAI:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_def_time_chepai)
			{

				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

					if (m_chepai_status[i] == 0 && m_user_status[i] == 1)
					{
						LLOG_ERROR("CheXuanGameHandler::Tick() Che Pai AutoRun...desk_id=[%d], pos=[%d], GAME_PLAY_CHE_PAI time over!",
							m_desk ? m_desk->GetDeskId() : 0, i);
						on_event_player_che_pai(i, m_hand_cards[i], 4);
					}
				}
			}
			break;
		}
		default:
			break;
		}
	}

	/* ������Ҷ������� */
	void OnUserReconnect(User* pUser)
	{
		if (NULL == pUser || NULL == m_desk)
		{
			LLOG_ERROR("CheXuanGameHandler::OnUserReconnect() Error!!! pUser or m_desk Is NULL...");
			return;
		}

		Lint pos = GetUserPos(pUser);

		if (CHEXUAN_INVALID_POS == pos && !(m_desk->MHIsLookonUser(pUser) || m_desk->m_clubOwerLookOnUser == pUser))
		{
			LLOG_ERROR("CheXuanGameHandler::OnUserReconnect() Error!!! Reconnect Pos is invial... desk_id=[%d], userId=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId());
			return;
		}

		//Desk �Ѿ�����
		notify_desk_match_state();

		LLOG_ERROR("CheXuanGameHandler::OnUserReconnect() Run... deskId=[%d], userId=[%d], pos=[%d], curPos=[%d], zhuangPos=[%d], playStatus=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), pos, m_curPos, m_zhuangPos, m_play_status);
		LTime t_currTime;
		t_currTime.Now();

		CheXuanS2CReConnect reconn;
		reconn.m_pos = pos;
		reconn.m_curPos = m_curPos;
		reconn.m_dangPos = GetNextPos(m_frist_send_card);
		reconn.m_curStatus = m_play_status;
		reconn.m_turnCount = m_turn_count;
		reconn.m_firstSendCardPos = m_frist_send_card;
		reconn.m_turnFirstSpeakPos = m_turn_first_speak_pos;
		reconn.m_selectMoFen = m_selectMoFen;
		memcpy(reconn.m_userMustQiBoBo, m_is_need_add_bo, sizeof(reconn.m_userMustQiBoBo));
		memcpy(reconn.m_qiBoBoStatus, m_qibobo_status, sizeof(reconn.m_qiBoBoStatus));
		memcpy(reconn.m_userStatus, m_user_status, sizeof(reconn.m_userStatus));
		memcpy(reconn.m_qiPaiStatus, m_qipai_status, sizeof(reconn.m_qiPaiStatus));
		memcpy(reconn.m_xiuStatus, m_xiu_status, sizeof(reconn.m_xiuStatus));
		memcpy(reconn.m_qiaoStatus, m_qiao_status, sizeof(reconn.m_qiaoStatus));
		memcpy(reconn.m_cuoPaiStatus, m_cuopai_status, sizeof(reconn.m_cuoPaiStatus));
		memcpy(reconn.m_chePaiStatus, m_chepai_status, sizeof(reconn.m_chePaiStatus));
		memcpy(reconn.m_playerLastOpt, m_player_last_opt, sizeof(reconn.m_playerLastOpt));
		reconn.m_piPool = m_pi_pool;
		reconn.m_mangPool = m_mang_pool;

		memcpy(reconn.m_lastBoScore, m_last_bo_score, sizeof(reconn.m_lastBoScore));
		memcpy(reconn.m_playerTotalBoScore, m_total_bo_score, sizeof(reconn.m_playerTotalBoScore));
		memcpy(reconn.m_playerTotalAddScore, m_total_add_score, sizeof(reconn.m_playerTotalAddScore));
		memcpy(reconn.m_playerTotalAddMang, m_total_add_mang_score, sizeof(reconn.m_playerTotalAddMang));
		memcpy(reconn.m_turnOptType, m_turn_opt_type[m_turn_count], sizeof(reconn.m_turnOptType));
		memcpy(reconn.m_turnOptScore, m_turn_opt_score[m_turn_count], sizeof(reconn.m_turnOptScore));
		memcpy(reconn.m_playerAutoOpt, m_playerAutoOpt, sizeof(reconn.m_playerAutoOpt));
		memcpy(reconn.m_playerStandUp, m_player_stand_up, sizeof(reconn.m_playerStandUp));

		memcpy(reconn.m_handCardsCount, m_hand_cards_count, sizeof(reconn.m_handCardsCount));

		//���������������ֵ������ʾ
		if (pos != CHEXUAN_INVALID_POS && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame)
		{
			if (m_user_status[pos] != 0 && m_is_coins_low[pos] == 1)
			{
				reconn.m_isCoinsLow = 1;
				//������������ֵ���͵Ķ�ʱ��
				m_is_coins_low_time[pos].Now();
			}

			if (m_user_status[pos] == 2 && m_rest_can_start_bo_score[pos] + m_total_bo_score[pos] > m_desk->m_inDeskMinCoins)
			{
				reconn.m_isStandShowSeat = 1;
			}
		}

		switch (m_play_status)
		{

		//�𲧲��׶�
		case GAME_PLAY_QIBOBO:
		{
			//��ǰ������Ϣ�㲥�������û�
			pUser->Send(reconn);

			//������û��ǵ�ǰ������Ϸ����� && ��Ҫ�𲧲������ٷ����𲧲���Ϣ�������
			if (pos != CHEXUAN_INVALID_POS && m_user_status[pos] == 1 && m_qibobo_status[pos] == 0)
			{
				//֪ͨ����𲧲�
				CheXuanS2CNotifyPlayerQiBoBo qiBoBo;
				qiBoBo.m_pos = pos;
				qiBoBo.m_restTime = m_def_time_qibobo - (m_rest_remain_time == -1 ? (t_currTime.Secs() - m_play_status_time.Secs()) : m_rest_remain_time);
				qiBoBo.m_dangPos = GetNextPos(m_frist_send_card);
				qiBoBo.m_piPool = m_pi_pool;
				qiBoBo.m_mangPool = m_mang_pool;
				qiBoBo.m_minSelectBoScore = m_min_qi_bo_score[pos];
				qiBoBo.m_maxSelectBoScore = m_max_qi_bo_score[pos];
				memcpy(qiBoBo.m_userStatus, m_user_status, sizeof(qiBoBo.m_userStatus));
				memcpy(qiBoBo.m_userMustQiBoBo, m_is_need_add_bo, sizeof(qiBoBo.m_userMustQiBoBo));
				memcpy(qiBoBo.m_playerStandUp, m_player_stand_up, sizeof(qiBoBo.m_playerStandUp));
				notify_user(qiBoBo, pos);
			}

			break;
		}

		//��â���׶�
		case GAME_PLAY_ADD_MANGGUO:
		{
			pUser->Send(reconn);

			break;
		}

		//���ƽ׶� && ��ע
		case GAME_PLAY_SEND_CARD:
		case GAME_PLAY_ADD_SCORE:
		{
			Lint t_defTime = (Lint)m_def_time_optscore;
			if (m_turn_count == 2)
			{
				t_defTime = (Lint)m_def_time_3rd_optscore;
			}
			else if (pos == m_turn_first_speak_pos && m_turn_count == 0 && m_turn_opt_type[m_turn_count][pos] == ADD_OPT_INVALID)
			{
				t_defTime = (Lint)m_def_time_optscore + 2 + (m_first_shou_mang_time ? 1 : 0) + (m_first_xiu_mang_time ? 1 : 0) + (m_first_zou_mang_time ? 1 : 0);
			}
			
			if(!m_isLookOnDarkResult || (pos != CHEXUAN_INVALID_POS && m_user_status[pos] == 1))
			{
				for (int i = 0; i < m_player_count; ++i)
				{
					for (int j = 0; j < m_hand_cards_count[i]; ++j)
					{
						if (i == pos)
						{
							reconn.m_handCards[i][j] = m_hand_cards[i][j];
						}
						else if (j == 2)
						{
							reconn.m_handCards[i][j] = m_hand_cards[i][j];
						}
						else if (j == 3)
						{
							reconn.m_handCards[i][j] = m_hand_cards[i][j];
						}
					}
				}
			}

			pUser->Send(reconn);

			if (!m_isMoFen || m_selectMoFen != -1)
			{
				//������������Ĳ���
				Lint t_isMeiDaDong = 0;
				calc_opt_score();

				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = t_defTime - (m_rest_remain_time == -1 ? (t_currTime.Secs() - m_play_status_time.Secs()) : m_rest_remain_time);
				notifyOpt.m_isMeiDaDong = t_isMeiDaDong;
				notifyOpt.m_hasPlayerQiao = check_has_player_qiao();
				for (int i = 0; i < 10; ++i)
				{
					notifyOpt.m_optType[i] = m_allow_opt_type[m_curPos][i];
					notifyOpt.m_optMinScore[i] = m_allow_opt_min_score[m_curPos][i];
					notifyOpt.m_optMaxScore[i] = m_allow_opt_max_score[m_curPos][i];
				}
				pUser->Send(notifyOpt);
			}
			else
			{
				CheXuanS2CNotifyPlayerOpt notifyOpt;
				notifyOpt.m_pos = m_curPos;
				notifyOpt.m_restTime = t_defTime - (m_rest_remain_time == -1 ? (t_currTime.Secs() - m_play_status_time.Secs()) : m_rest_remain_time);
				notifyOpt.m_isMoFen = 1;
				pUser->Send(notifyOpt);
			}

			break;
		}

		//���ƽ׶�
		case GAME_PLAY_CUO_PAI:
		{
			if (!m_isLookOnDarkResult || (pos != CHEXUAN_INVALID_POS && m_user_status[pos] == 1))
			{
				//��ʾ������ʾ����
				for (int i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

					reconn.m_handCards[i][2] = m_hand_cards[i][2];
					if (m_cuopai_status[i] == 1)
					{
						reconn.m_handCards[i][3] = m_hand_cards[i][3];
					}
				}
			}
			//���������ǲ�����Ϸ��ң������Ʒ��͸������
			if (pos != CHEXUAN_INVALID_POS && m_qipai_status[pos] == 0)
			{
				memcpy(reconn.m_handCards[pos], m_hand_cards[pos], sizeof(Lint) * m_hand_cards_count[pos]);
			}
			pUser->Send(reconn);

			if (pos != CHEXUAN_INVALID_POS && m_cuopai_status[pos] == 0 && m_qipai_status[pos] == 0)
			{
				CheXuanS2CNotifyCuoPai cuoPai;
				cuoPai.m_restTime = m_def_time_cuopai - (m_rest_remain_time == -1 ? (t_currTime.Secs() - m_play_status_time.Secs()) : m_rest_remain_time);
				memcpy(cuoPai.m_userStatus, m_user_status, sizeof(cuoPai.m_userStatus));
				memcpy(cuoPai.m_cuoPaiStatus, m_cuopai_status, sizeof(cuoPai.m_cuoPaiStatus));
				memcpy(cuoPai.m_qiaoStatus, m_qiao_status, sizeof(cuoPai.m_qiaoStatus));
				memcpy(cuoPai.m_qiPaiStatus, m_qipai_status, sizeof(cuoPai.m_qiPaiStatus));

				pUser->Send(cuoPai);
			}
			break;
		}

		//���ƽ׶�
		case GAME_PLAY_CHE_PAI:
		{
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1 || m_qipai_status[i] != 0) continue;

				reconn.m_handCards[i][2] = m_hand_cards_backup[i][2];
				reconn.m_handCards[i][3] = m_hand_cards_backup[i][3];
				
				if (i == pos)
				{
					memcpy(reconn.m_handCards[pos], m_hand_cards[pos], sizeof(Lint) * m_hand_cards_count[pos]);
					memcpy(reconn.m_cardsType[pos], m_hand_cards_type[pos], sizeof(Lint) * 2);
				}
			}
			pUser->Send(reconn);

			if (pos != CHEXUAN_INVALID_POS && m_chepai_status[pos] == 0)
			{
				CheXuanS2CNotifyChePai chePai;
				chePai.m_restTime = m_def_time_chepai - (m_rest_remain_time == -1 ? (t_currTime.Secs() - m_play_status_time.Secs()) : m_rest_remain_time);
				memcpy(chePai.m_userStatus, m_user_status, sizeof(chePai.m_userStatus));
				memcpy(chePai.m_chePaiStatus, m_chepai_status, sizeof(chePai.m_chePaiStatus));
				memcpy(chePai.m_qiPaiStatus, m_qipai_status, sizeof(chePai.m_qiPaiStatus));
				memcpy(chePai.m_qiaoStatus, m_qiao_status, sizeof(chePai.m_qiaoStatus));
				pUser->Send(chePai);
			}
			break;
		}

		//
		default:
			pUser->Send(reconn);
			break;
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
	C->S ���ţ���������𲧲�
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401
	*/
	bool HandlerPlayerSelectBoBo(User* pUser, CheXuanC2SPlayerSelectBoBo* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectBoBo() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_QIBOBO != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectBoBo() Play Status Error!!! Current Play Status is not GAME_PLAY_QIBOBO... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectBoBo() Error!!! Select BoBo Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectBoBo() Run... deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

		//����ִ��
		return on_event_player_select_bobo(GetUserPos(pUser), msg->m_selectBoScore);
	}

	/*
	���ţ�C->S �������ѡ�����
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 622408
	*/
	bool HandlerPlayerSelectOpt(User* pUser, CheXuanC2SPlayerSelectOpt* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectOpt() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_ADD_SCORE != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectOpt() Play Status Error!!! Current Play Status is not GAME_PLAY_ADD_SCORE... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectOpt() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectOpt() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

		//����ִ�������ע
		return on_event_player_select_opt(GetUserPos(pUser), msg->m_selectType, msg->m_selectScore);
	}

	/*
	���ţ�C->S ������ҳ��Ʋ���
	MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 622411
	*/
	bool HandlerPlayerChePai(User* pUser, CheXuanC2SPlayerChePai* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerChePai() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_CHE_PAI != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerChePai() Play Status Error!!! Current Play Status is not GAME_PLAY_CHE_PAI... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerChePai() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		Lint t_handCards[4];
		t_handCards[0] = msg->m_handCard1;
		t_handCards[1] = msg->m_handCard2;
		t_handCards[2] = msg->m_handCard3;
		t_handCards[3] = msg->m_handCard4;

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerChePai() Run..., deskId=[%d], userId=[%d], userPos=[%d], handCards=[%x, %x, %x, %x]",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser), t_handCards[0], t_handCards[1], t_handCards[2], t_handCards[3]);

		//����ִ��
		return on_event_player_che_pai(GetUserPos(pUser), t_handCards, 4);
	}

	/*
	C->S ��Ҵ��Ʋ���
	MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417
	*/
	bool HandlerPlayerCuoPai(User* pUser, CheXuanC2SPlayerCuoPai* msg)
	{
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerCuoPai() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_CUO_PAI != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerCuoPai() Play Status Error!!! Current Play Status is not GAME_PLAY_CUO_PAI... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerCuoPai() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerCuoPai() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

		return on_event_player_cuo_pai(msg->m_pos);
	}

	//��ȡ�ƾ�����ҵĲ�����
	Lint GetPlayerBoBoScore(User* pUser)
	{
		if (!pUser)
		{
			return -100000;
		}

		Lint t_pos = GetUserPos(pUser);
		if (t_pos != CHEXUAN_INVALID_POS)
		{
			return m_total_bo_score[t_pos];
		}
		else
		{
			return -100000;
		}
	}

	//������ҿ��𲧲�����
	bool UpdatePlayerAccumCoins(User* pUser, Lint changeCoins)
	{
		if (!pUser)
		{
			return false;
		}

		if (!(m_desk && m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame == 1))
		{
			return false;
		}

		Lint t_pos = GetUserPos(pUser);
		if (t_pos == CHEXUAN_INVALID_POS)
		{
			return false;
		}

		m_rest_can_start_bo_score[t_pos] += changeCoins;
		m_record_bobo_score[t_pos] += changeCoins;

		LMsgS2CStandPlayerShowSeat addCoins;
		addCoins.m_pos = t_pos;
		addCoins.m_addCoins = changeCoins;
		addCoins.m_isShowSeat = 0;
		notify_desk(addCoins);
		notify_lookon(addCoins);

		//���������ܳ���Ԥ��ֵ������Ԥ���ͻ���
		if (m_rest_can_start_bo_score[t_pos] + m_total_bo_score[t_pos] > m_desk->m_warnScore)
		{
			m_is_coins_low[t_pos] = 0;
		}

		//����ֵ�������ң���������ֵ������ͽ��뷿��ֵ����ʾ��ҿ�������
		if (m_user_status[t_pos] == 2 && m_rest_can_start_bo_score[t_pos] + m_total_bo_score[t_pos] > m_desk->m_inDeskMinCoins)
		{
			//����ҷ��ͼ�����Ϸ��ť
			LMsgS2CStandPlayerShowSeat showSeat;
			showSeat.m_pos = t_pos;
			showSeat.m_addCoins = 0;
			showSeat.m_isShowSeat = 1;
			if (m_desk&&m_desk->m_standUpPlayerSeatDown[t_pos] == 2)
			{
				pUser->Send(showSeat);
				m_desk->m_standUpPlayerSeatDown[t_pos] = 3;
			}
		}
		return true;
	}

	//����ƾ��ڲ鿴ս��
	bool HandlerPlayerRequestRecord(User* pUser, CheXuanC2SPlayerRequestRecord* msg)
	{
		if (!pUser || !m_desk)
		{
			return false;
		}

		CheXuanS2CPlayerSendRecord sendRecord;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 0 || !m_desk->m_user[i]) continue;
			sendRecord.m_userId[i] = m_desk->m_user[i]->GetUserDataId();
			sendRecord.m_userHead[i] = m_desk->m_user[i]->GetUserData().m_headImageUrl;
			sendRecord.m_userNike[i] = m_desk->m_user[i]->GetUserData().m_nike;
			sendRecord.m_nowScore[i] = m_rest_can_start_bo_score[i] + m_total_bo_score[i];
			sendRecord.m_inScore[i] = m_record_bobo_score[i];
			sendRecord.m_changeScore[i] = sendRecord.m_nowScore[i] - sendRecord.m_inScore[i];
		}

		//��ս�����Ϣ
		if (!m_desk->m_desk_Lookon_user.empty())
		{
			auto lookIter = m_desk->m_desk_Lookon_user.begin();
			for (; lookIter != m_desk->m_desk_Lookon_user.end(); lookIter++)
			{
				if (*lookIter == NULL) continue;

				++sendRecord.m_lookOnCount;
				sendRecord.m_lookOnId.push_back((*lookIter)->GetUserDataId());
				sendRecord.m_lookOnNike.push_back((*lookIter)->GetUserData().m_nike);
				sendRecord.m_lookOnHead.push_back((*lookIter)->GetUserData().m_headImageUrl);
			}
		}

		if (m_desk->m_clubOwerLookOnUser)
		{
			++sendRecord.m_lookOnCount;
			sendRecord.m_lookOnId.push_back(m_desk->m_clubOwerLookOnUser->GetUserDataId());
			sendRecord.m_lookOnNike.push_back(m_desk->m_clubOwerLookOnUser->GetUserData().m_nike);
			sendRecord.m_lookOnHead.push_back(m_desk->m_clubOwerLookOnUser->GetUserData().m_headImageUrl);
		}
		pUser->Send(sendRecord);
		return true;
	}

	/*
	C->S �ͻ��˲���Ĭ��
	MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422
	*/
	bool HandlerPlayerOptMoFen(User* pUser, CheXuanC2SPlayerOptMoFen* msg) 
	{ 
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerOptMoFen() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GAME_PLAY_ADD_SCORE != m_play_status)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerOptMoFen() Play Status Error!!! Current Play Status is not GAME_PLAY_ADD_SCORE... deskId=[%d], userId=[%d], currStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerOptMoFen() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerOptMoFen() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

		return on_event_player_opt_mo_fen(GetUserPos(pUser),msg->m_optMo);
	}

	/*
	C->S �ͻ���ѡ���Զ�����
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424
	*/
	bool HandlerPlayerSelectAutoOpt(User* pUser, CheXuanC2SPlayerSelectAutoOpt* msg) 
	{ 
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectAutoOpt() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectAutoOpt() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerSelectAutoOpt() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));


		return on_event_player_select_auto_opt(GetUserPos(pUser), msg->m_autoOptType);
	}

	/*
	C->S �ͻ���ѡ��վ��
	MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427
	*/
	bool HandlerPlayerStandOrSeat(User* pUser, CheXuanC2SPlayerSelectStandOrSeat* msg) 
	{ 
		if (NULL == pUser || NULL == msg || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerStandOrSeat() Null Ptr Error!!! pUser or msg or m_desk Is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerStandOrSeat() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		if (msg->m_standOrSeat == 0)
		{
			LLOG_ERROR("CheXuanGameHandler::HandlerPlayerStandOrSeat() Error!!! This methord only stand, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HandlerPlayerStandOrSeat() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));


		return on_event_player_stand_or_seat(GetUserPos(pUser), msg->m_standOrSeat);
	}

	//�����������,desk�ϵ���
	bool HanderPlayerSeat(User* pUser, Lint seat)
	{
		if (NULL == pUser || m_desk == NULL)
		{
			LLOG_ERROR("CheXuanGameHandler::HanderPlayerSeat() Null Ptr Error!!! pUser or m_desk Is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == CHEXUAN_INVALID_POS)
		{
			LLOG_ERROR("CheXuanGameHandler::HanderPlayerSeat() Error!!! Select Opt Pos is invial, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		if (seat == 1)
		{
			LLOG_ERROR("CheXuanGameHandler::HanderPlayerSeat() Error!!! This methord only stand, deskId=[%d], userId=[%d] userPos=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		LLOG_ERROR("CheXuanGameHandler::HanderPlayerSeat() Run..., deskId=[%d], userId=[%d] userPos=[%d] ",
			m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));


		return on_event_player_stand_or_seat(GetUserPos(pUser), seat);
	}
};


DECLARE_GAME_HANDLER_CREATOR(111, CheXuanGameHandler);

/*
//���ŵĴ�������������(����ĺ���ɵ�ͬ��������)
class GameHandlerCreator_CheXuan : public GameHandlerCreator
{
public:

	//���캯���������Ŵ�������������ע�ᵽ������
	GameHandlerCreator_CheXuan()
	{
		GameHandlerFactory::getInstance()->registerCreator(CheXuan, (GameHandlerCreator*)this);
	}

	//���������������ŵĴ�����������ӹ������Ƴ�
	~GameHandlerCreator_CheXuan()
	{
		GameHandlerFactory::getInstance()->unregisterCreator(CheXuan);
	}

	//��������ĳ��Ŵ���������
	GameHandler* create()
	{
		return new CheXuanGameHandler();
	}
};

//��̬����������ʼʱ�Զ����أ�ִ�и�����Զ�ע�ᵽ������
static GameHandlerCreator_CheXuan sGameHandlerCreator_CheXuan;

*/