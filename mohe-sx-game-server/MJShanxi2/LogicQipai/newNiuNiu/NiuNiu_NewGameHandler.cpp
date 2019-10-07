#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "NiuNiu_NewGameLogic.h"
#include "LMsgL2C.h"

//���԰浹��ʱ�ӳ�
#define DEBUG_DELAY_TIME 0

/*
*  �°�ţţDebug����ʹ��
*  cbRandCard �����������
*  m_desk Desk����
*  conFileName �����ļ�
*  playerCount �������
*  handCardsCount �����������
*  cellPackCount ������
*/
static bool debugSendCards_NiuNiu_New(BYTE* cbRandCard, Desk* m_desk, const char* conFileName, int playerCount, int handCardsCount, int cellPackCount)
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

		memcpy(cbRandCard, t_cbRandCard, sizeof(BYTE) * cellPackCount);

		if (t_cbRandCard) delete(t_cbRandCard);
		t_cbRandCard = NULL;

		if (t_sendCount) delete(t_cbRandCard);
		t_sendCount = NULL;

		if (fp) fclose(fp);
		fp = NULL;
		return true;
	}

	if (fp) fclose(fp);
	fp = NULL;

	return false;
}


/* ţţ���淨��ǩ��ö�٣�*/
enum GAME_MAIN_MODE
{
	MAIN_MODE_NIUNIU_SHANG_ZHUANG = 1,		//ţţ��ׯ
	MAIN_MODE_ZI_YOU_QIANG_ZHUANG = 2,		//������ׯ
	MAIN_MODE_MING_PAI_QIANG_ZHUANG = 3,	//������ׯ
	MAIN_MODE_BA_REN_MING_PAI = 4,			//��������
	MAIN_MODE_GONG_PAI_NIU_NIU = 5,			//����ţţ
};

/* �淨ģʽ��ö�٣�*/
enum GAME_PLAY_MODE
{
	PLAY_MODE_COMMON_1 = 0,  //��ͨģʽ1
	PLAY_MODE_COMMON_2 = 1,  //��ͨģʽ2
	PLAY_MODE_BOMB = 2,      //ɨ��ģʽ
	PLAY_MODE_MINGSCORE = 3, //������ע
	PLAY_MODE_ANSCORE = 4,   //������ע
	PLAY_MODE_X1 = 5,		 //����ӱ���1
};

/* ��Ϸ�еĸ����׶Σ�ö�٣� */
enum GAME_PLAY_STATE
{
	GAME_PLAY_BEGIN = -1,
	GAME_PLAY_SELECT_ZHUANG = 0,
	GAME_PLAY_MAI_MA = 1,
	GAME_PLAY_ADD_SCORE = 2,
	GAME_PLAY_SEND_CARD = 3,
	GAME_PLAY_CUO_PAI = 4,
	GAME_PLAY_END = 5,
};

/* �����׶εĳ�ʱʱ�䣨ö�٣� */
enum PLAY_STATUS_DELAY_TIME
{
	DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU = 8 + DEBUG_DELAY_TIME,   //��ׯ
	DELAY_TIME_ADD_SCORE = 10 + DEBUG_DELAY_TIME,                   //��ע
	DELAY_TIME_OPEN_CARD = 15 + DEBUG_DELAY_TIME,                   //����
};
/* �����׶γ�ʱʱ�䣨����ģʽ��*/
enum PLAY_STATUS_DELAY_TIME_FAST
{
	DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU_FAST = 5 + DEBUG_DELAY_TIME,   //��ׯ
	DELAY_TIME_ADD_SCORE_FAST = 5 + DEBUG_DELAY_TIME,                   //��ע
	DELAY_TIME_OPEN_CARD_FAST = 8 + DEBUG_DELAY_TIME,                   //����
};

//����й�ѡ��
struct TuoGuanInfo
{
	Lint m_pos;
	Lint m_tuoGuanStatus;		//�й�״̬ 0��δ�й�  1�����й�
	Lint m_zhuangScore;			//��ׯ���� 0������   1:1��    2:2��    81��91��101��ţ��1����ţ��1����ţţ1�� ...  82��92��102��2��    83��93��103��3��    84��94��104
	Lint m_addScore;			//��ע���� 1:1��   2:2��    82��ţ������2��    92��ţ������2��   102��ţţ����2��
	Lint m_tuiScore;			//��ע���� 0������ע   81��ţ��������ע     91��ţ��������ע    101��ţţ������ע

	TuoGuanInfo()
	{
		memset(this, 0, sizeof(this));
		m_pos = NIUNIU_INVAILD_POS;
	}
};

//�м�Ӯ����Ϣ�ṹ
struct PlayerWinInfo
{
	Lint m_index;
	Lint m_oxNum;
	Lint m_oxTimes;
	Lint m_playerScore;

	PlayerWinInfo()
	{
		memset(this, 0, sizeof(this));
		m_index = NIUNIU_INVAILD_POS;
	}

	void operator= (PlayerWinInfo& info)
	{
		this->m_index = info.m_index;
		this->m_oxNum = info.m_oxNum;
		this->m_oxTimes = info.m_oxTimes;
		this->m_playerScore = info.m_playerScore;
	}
};


/* ţţ��ϷСѡ��(�ṹ��) */
struct PlayType
{
protected:
	std::vector<Lint>	m_playtype;  //��ϷСѡ��
									 //Index ------- 0: ѡׯģʽ 1:ɨ��ģʽ 2:�й�ģʽ 3:�����淨 4:����ׯ

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
	* ��������   NiuNiu_NewGetMainMode()
	* ������     ��ȡ�ͻ���ѡ���ţţ���淨��ǩ
	* ��ϸ��     ���淨��ǩ��1ţţ��ׯ��2������ׯ��3������ׯ��4ʮ�����ơ�5����ţţ
	* ���أ�     (Lint)�ͻ���ѡ���ţţ���淨��ǩ
	********************************************************/
	Lint NiuNiu_NewGetMainMode() const;

	/******************************************************
	* ��������   NiuNiu_NewGetPlayerNum()
	* ������     ��ȡ�ͻ�������֧�֣�6�ˡ�8�˻�10��
	* ���أ�     (int)������
	********************************************************/
	Lint NiuNiu_NewGetPlayerNum() const;

	/******************************************************
	* ��������   NiuNiu_NewQiangZhuangTimes()
	* ������     ��ȡţţ��ׯ����
	* ��ϸ��     ��ׯ���ʣ�1-1����2-2����3-3����4-4��
	* ���أ�     (Lint)ѡ�����ׯ����
	********************************************************/
	Lint NiuNiu_NewQiangZhuangTimes() const;

	/******************************************************
	* ��������   NiuNiu_NewAddScoreTimes()
	* ������     ��ȡţţ��ע�׷֣���
	* ��ϸ��     ��ע���ʣ�1-1/2  2-2/4  3-3/6  4-4/8  5-5/10
	* ���أ�     (Lint)ѡ�����ע����
	********************************************************/
	Lint NiuNiu_NewAddScoreTimes() const;

	/******************************************************
	* ��������   NiuNiu_NewAutoStartGame()
	* ������     ��ȡ�ͻ���ѡ���Զ���ʼ
	* ��ϸ��     �Զ���ʼ��1-����������ʼ��Ϸ��4-��4�˿���5-��5�˿���6-��6�˿�, 11-������ʼ��Ϸ
	* ���أ�     (Lint)�ͻ���ѡ����Զ���ʼ
	********************************************************/
	Lint NiuNiu_NewAutoStartGame() const;

	/******************************************************
	* ��������	NiuNiu_NewGetTuiZhu()
	* ������	ţţ��עѡ��
	* ��ϸ��	��ע������ 0������ע��5��5����ע��10��10����ע��15:15����ע��20:20����ע
	* ���أ�	(Lint)��ע����
	*******************************************************/
	Lint NiuNiu_NewGetTuiZhu() const;

	/******************************************************
	* ��������   NiuNiu_NewGetDynamicIn()
	* ������     ��ȡ�ͻ����Ƿ�����̬����ѡ��
	* ��ϸ��     ��̬���룺false-������true-����
	* ���أ�     (bool)�Ƿ�̬����
	********************************************************/
	bool NiuNiu_NewGetDynamicIn() const;

	/******************************************************
	* ��������   NiuNiu_NewGetAutoPlay()
	* ������     ��ȡ�ͻ��˳�ʱ�Զ�����
	* ��ϸ��     �йܣ�false-��������true-����
	* ���أ�     (bool)�ͻ���ѡ��ʱ�Զ�����
	********************************************************/
	bool NiuNiu_NewGetAutoPlay() const;

	/******************************************************
	* ��������   NiuNiu_NewAllowLaiZi()
	* ������     ��ȡ�ͻ��Ƿ����������
	* ��ϸ��     ����ӣ�false-������true-����
	* ���أ�     (bool)
	********************************************************/
	bool NiuNiu_NewAllowLaiZi() const;

	/******************************************************
	* ��������   NiuNiu_NewAllowMaiMa()
	* ������     ��ȡ�ͻ��Ƿ���������
	* ��ϸ��     ����ӣ�false-������true-����
	* ���أ�     (bool)
	********************************************************/
	bool NiuNiu_NewAllowMaiMa() const;

	/******************************************************
	* ��������   NiuNiu_NewGetCuoPai()
	* ������     ��ȡ�ͻ��Ƿ��������
	* ��ϸ��     ���ƣ�false-������true-����
	* ���أ�     (bool)
	********************************************************/
	bool NiuNiu_NewGetCuoPai() const;

	/******************************************************
	* ��������   NiuNiu_NewGetConstAddScore()
	* ������     ��ȡ�ͻ��Ƿ���ע����
	* ��ϸ��     ��ע���ƣ�false-�����ƣ�true-����
	* ���أ�     (bool)
	********************************************************/
	bool NiuNiu_NewGetConstAddScore() const;

	/******************************************************
	* ��������   NiuNiu_NewGetConstTuiZhu()
	* ������     ��ȡ�ͻ��Ƿ���ע����
	* ��ϸ��     ��ע���ƣ�false-�����ƣ�true-����
	* ���أ�     (bool)
	********************************************************/
	bool NiuNiu_NewGetConstTuiZhu() const;

	/******************************************************
	* ��������   NiuNiu_NewGetConstAddScoreDouble()
	* ������     ��ȡ�ͻ��Ƿ�����ӱ�
	* ��ϸ��     �Ƿ�����ӱ���false-������true-����
	* ���أ�     (bool)
	********************************************************/
	bool NiuNiu_NewGetConstAddScoreDouble() const;

	/******************************************************
	* ��������	NiuNiu_NewTypeTimes()
	* ������	ţţ���ͱ���
	* ��ϸ��	1��ţţx3  ţ��x2  ţ��x2  ţ��-��ţx1
	2��ţţx4  ţ��x3  ţ��x2  ţ��x2  ţ��-��ţx1
	3��...
	* ���أ�	(int) ��ѡ��ѡ��أ�0�� ѡ���ѡ��� 1
	*******************************************************/
	Lint NiuNiu_NewTypeTimes() const;

	/******************************************************
	* ��������	NiuNiu_NewAllowTypeBiYi()
	* ������	ţţ�������ͣ�����ţ4��
	* ��ϸ��	����ţ��false-������true-����
	* ���أ�	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeBiYi() const;

	/******************************************************
	* ��������	NiuNiu_NewAllowTypeShunZi()
	* ������	ţţ�������ͣ�˳��ţ5��
	* ��ϸ��	˳��ţ��false-������true-����
	* ���أ�	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeShunZi() const;

	/******************************************************
	* ��������	NiuNiu_NewAllowTypeWuHua()
	* ������	ţţ�������ͣ��廨ţ5��
	* ��ϸ��	�廨ţ��false-������true-����
	* ���أ�	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeWuHua() const;

	/******************************************************
	* ��������	NiuNiu_NewAllowTypeTongHua()
	* ������	ţţ�������ͣ�ͬ��ţ6��
	* ��ϸ��	ͬ��ţ��false-������true-����
	* ���أ�	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeTongHua() const;

	/******************************************************
	* ��������	NiuNiu_NewAllowTypeHuLu()
	* ������	ţţ�������ͣ���«ţ7��
	* ��ϸ��	��«ţ��false-������true-����
	* ���أ�	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeHuLu() const;

	/******************************************************
	* ��������	NiuNiu_NewAllowTypeZhaDan()
	* ������	ţţ�������ͣ�ը��ţ8��
	* ��ϸ��	ը��ţ��false-������true-����
	* ���أ�	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeZhaDan() const;

	/******************************************************
	* ��������	NiuNiu_NewAllowTypeWuXiao()
	* ������	ţţ�������ͣ���Сţ9��
	* ��ϸ��	��Сţ��false-������true-����
	* ���أ�	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeWuXiao() const;

	/******************************************************
	* ��������	NiuNiu_NewAllowTypeKuaiLe()
	* ������	ţţ�������ͣ�����ţ10��
	* ��ϸ��	����ţ��false-������true-����
	* ���أ�	(bool)
	*******************************************************/
	bool NiuNiu_NewAllowTypeKuaiLe() const;

	/******************************************************
	* ��������	NiuNiu_NewFastStyle()
	* ������	ţţ����ģʽ
	* ��ϸ��	����ģʽ��false-��������true-����
	* ���أ�	(bool)
	*******************************************************/
	bool NiuNiu_NewFastStyle() const;

};


/*****************************************************************************
* ��������   NiuNiu_NewGetMainMode()
* ������     ��ȡ�ͻ���ѡ���ţţ���淨��ǩ
* ��ϸ��     ׯ�ҹ���1ţţ��ׯ��2������ׯ��3������ׯ��4ʮ�����ơ�5����ţţ
* ���أ�     (Lint)�ͻ���ѡ���ţţ���淨��ǩ
******************************************************************************/
Lint PlayType::NiuNiu_NewGetMainMode() const
{
	if (m_playtype.empty())
	{
		return 1;
	}

	if (m_playtype[0] <= 0 || m_playtype[0] > 5)
	{
		return 1;
	}

	return m_playtype[0];
}

/******************************************************
* ��������   NiuNiu_NewGetPlayerNum()
* ������     ��ȡ�ͻ�������֧�֣�6�ˡ�8�˻�10��
* ���أ�     (Lint)�Ƿ�̬����
********************************************************/
Lint PlayType::NiuNiu_NewGetPlayerNum() const
{
	if (m_playtype.size() < 2)
	{
		return 6;
	}
	if (m_playtype[1] != 6 || m_playtype[1] != 8 || m_playtype[1] != 10)
	{
		return 6;
	}
	return m_playtype[1];
}

/******************************************************
* ��������   NiuNiu_NewQiangZhuangTimes()
* ������     ��ȡţţ��ׯ����
* ��ϸ��     ��ׯ���ʣ�1-1����2-2����3-3����4-4��
* ���أ�     (Lint)ѡ�����ׯ����
********************************************************/
Lint PlayType::NiuNiu_NewQiangZhuangTimes() const
{
	if (m_playtype.size() < 3)
	{
		return 1;
	}

	//������ׯ���������ơ�����ţţ����������ׯ����
	if (m_playtype[0] == MAIN_MODE_MING_PAI_QIANG_ZHUANG
		|| m_playtype[0] == MAIN_MODE_GONG_PAI_NIU_NIU
		|| m_playtype[0] == MAIN_MODE_BA_REN_MING_PAI)
	{
		return m_playtype[2];
	}

	return 1;
}

/******************************************************
* ��������   NiuNiu_NewAddScoreTimes()
* ������     ��ȡţţ��ע�׷֣���
* ��ϸ��     ��ע���ʣ�1-1/2  2-2/4  3-3/6  4-4/8  5-5/10
* ���أ�     (Lint)ѡ�����ע����
********************************************************/
Lint PlayType::NiuNiu_NewAddScoreTimes() const
{
	if (m_playtype.size() < 4)
	{
		return 1;
	}

	if (m_playtype[3] < 1 || m_playtype[3] > 5)
	{
		return 1;
	}

	return m_playtype[3];
}

/******************************************************
* ��������   NiuNiu_NewAutoStartGame()
* ������     ��ȡ�ͻ���ѡ���Զ���ʼ
* ��ϸ��     �Զ���ʼ��1-����������ʼ��Ϸ��4-��4�˿���5-��5�˿���6-��6�˿�, 11-������ʼ��Ϸ
* ���أ�     (Lint)�ͻ���ѡ����Զ���ʼ
********************************************************/
Lint PlayType::NiuNiu_NewAutoStartGame() const
{
	if (m_playtype.size() < 5)
	{
		return 1;
	}

	if (m_playtype[4] == 1)
	{
		return 1;
	}

	if (m_playtype[4] >= 4 && m_playtype[4] <= m_playtype[1])
	{
		return m_playtype[4];
	}

	return 1;
}

/******************************************************
* ��������	NiuNiu_NewGetTuiZhu()
* ������	ţţ��עѡ��
* ��ϸ��	��ע������ 0������ע��5��5����ע��10��10����ע��15:15����ע��20:20����ע
* ���أ�	(Lint)��ע����
*******************************************************/
Lint PlayType::NiuNiu_NewGetTuiZhu() const
{
	if (m_playtype.size() < 6)
	{
		return 0;
	}
	return m_playtype[5];
}

/******************************************************
* ��������   NiuNiu_NewGetDynamicIn()
* ������     ��ȡ�ͻ����Ƿ�����̬����ѡ��
* ��ϸ��     ����ׯ��false-������true-����
* ���أ�     (Lint)�Ƿ�̬����
********************************************************/
bool PlayType::NiuNiu_NewGetDynamicIn() const
{
	if (m_playtype.size() < 7)
	{
		return false;
	}
	return m_playtype[6] == 0 ? false : true;
}

/******************************************************
* ��������   NiuNiu_NewGetAutoPlay()
* ������     ��ȡ�ͻ��˳�ʱ�Զ�����
* ��ϸ��     ɨ��ģʽ��false-��������true-����
* ���أ�     (bool)�ͻ���ѡ��ʱ�Զ�����
********************************************************/
bool PlayType::NiuNiu_NewGetAutoPlay() const
{
	if (m_playtype.size() < 8)
	{
		return false;
	}
	return m_playtype[7] == 0 ? false : true;
}

/******************************************************
* ��������   NiuNiu_NewAllowLaiZi()
* ������     ��ȡ�ͻ��Ƿ����������
* ��ϸ��     ����ӣ�false-������true-����
* ���أ�     (bool)
********************************************************/
bool PlayType::NiuNiu_NewAllowLaiZi() const
{
	if (m_playtype.size() < 9)
	{
		return false;
	}
	return m_playtype[8] == 0 ? false : true;
}

/******************************************************
* ��������   NiuNiu_NewAllowMaiMa()
* ������     ��ȡ�ͻ��Ƿ���������
* ��ϸ��     ����ӣ�false-������true-����
* ���أ�     (bool)
********************************************************/
bool PlayType::NiuNiu_NewAllowMaiMa() const
{
	if (m_playtype.size() < 10)
	{
		return false;
	}
	return m_playtype[9] == 0 ? false : true;
}

/******************************************************
* ��������   NiuNiu_NewGetCuoPai()
* ������     ��ȡ�ͻ��Ƿ��������
* ��ϸ��     ���ƣ�false-������true-����
* ���أ�     (bool)
********************************************************/
bool PlayType::NiuNiu_NewGetCuoPai() const
{
	if (m_playtype.size() < 11)
	{
		return false;
	}
	return m_playtype[10] == 0 ? false : true;
}

/******************************************************
* ��������   NiuNiu_NewGetConstAddScore()
* ������     ��ȡ�ͻ��Ƿ���ע����
* ��ϸ��     ��ע���ƣ�false-�����ƣ�true-����
* ���أ�     (bool)
********************************************************/
bool PlayType::NiuNiu_NewGetConstAddScore() const
{
	if (m_playtype.size() < 12)
	{
		return false;
	}
	return m_playtype[11] == 0 ? false : true;
}

/******************************************************
* ��������   NiuNiu_NewGetConstTuiZhu()
* ������     ��ȡ�ͻ��Ƿ���ע����
* ��ϸ��     ��ע���ƣ�false-�����ƣ�true-����
* ���أ�     (bool)
********************************************************/
bool PlayType::NiuNiu_NewGetConstTuiZhu() const
{
	if (m_playtype.size() < 13)
	{
		return false;
	}
	return m_playtype[12] == 0 ? false : true;
}

/******************************************************
* ��������   NiuNiu_NewGetConstAddScoreDouble()
* ������     ��ȡ�ͻ��Ƿ�����ӱ�
* ��ϸ��     �Ƿ�����ӱ���false-������true-����
* ���أ�     (bool)
********************************************************/
bool PlayType::NiuNiu_NewGetConstAddScoreDouble() const
{
	if (m_playtype.size() < 14)
	{
		return false;
	}
	return m_playtype[13] == 0 ? false : true;
}

/******************************************************
* ��������	NiuNiu_NewTypeTimes()
* ������	ţţ���ͱ���
* ��ϸ��	1��ţţx3  ţ��x2  ţ��x2  ţ��-��ţx1
2��ţţx4  ţ��x3  ţ��x2  ţ��x2  ţ��-��ţx1
3��...
* ���أ�	(int)
*******************************************************/
Lint PlayType::NiuNiu_NewTypeTimes() const
{
	if (m_playtype.size() < 15)
	{
		return 1;
	}
	if (m_playtype[14] < 1 || m_playtype[14] > 2)
	{
		return 1;
	}

	return m_playtype[14];
}

/******************************************************
* ��������	NiuNiu_NewAllowTypeBiYi()
* ������	ţţ�������ͣ�����ţ4��
* ��ϸ��	����ţ��false-������true-����
* ���أ�	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeBiYi() const
{
	if (m_playtype.size() < 16)
	{
		return false;
	}
	return m_playtype[15] == 0 ? false : true;
}

/******************************************************
* ��������	NiuNiu_NewAllowTypeShunZi()
* ������	ţţ�������ͣ�˳��ţ5��
* ��ϸ��	˳��ţ��false-������true-����
* ���أ�	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeShunZi() const
{
	if (m_playtype.size() < 17)
	{
		return false;
	}
	return m_playtype[16] == 0 ? false : true;
}

/******************************************************
* ��������	NiuNiu_NewAllowTypeWuHua()
* ������	ţţ�������ͣ��廨ţ5��
* ��ϸ��	�廨ţ��false-������true-����
* ���أ�	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeWuHua() const
{
	if (m_playtype.size() < 18)
	{
		return false;
	}
	return m_playtype[17] == 0 ? false : true;
}

/******************************************************
* ��������	NiuNiu_NewAllowTypeTongHua()
* ������	ţţ�������ͣ�ͬ��ţ6��
* ��ϸ��	ͬ��ţ��false-������true-����
* ���أ�	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeTongHua() const
{
	if (m_playtype.size() < 19)
	{
		return false;
	}
	return m_playtype[18] == 0 ? false : true;
}

/******************************************************
* ��������	NiuNiu_NewAllowTypeHuLu()
* ������	ţţ�������ͣ���«ţ7��
* ��ϸ��	��«ţ��false-������true-����
* ���أ�	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeHuLu() const
{
	if (m_playtype.size() < 20)
	{
		return false;
	}
	return m_playtype[19] == 0 ? false : true;
}

/******************************************************
* ��������	NiuNiu_NewAllowTypeZhaDan()
* ������	ţţ�������ͣ�ը��ţ8��
* ��ϸ��	ը��ţ��false-������true-����
* ���أ�	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeZhaDan() const
{
	if (m_playtype.size() < 21)
	{
		return false;
	}
	return m_playtype[20] == 0 ? false : true;
}

/******************************************************
* ��������	NiuNiu_NewAllowTypeWuXiaoNiu()
* ������	ţţ�������ͣ���Сţ9��
* ��ϸ��	��Сţ��false-������true-����
* ���أ�	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeWuXiao() const
{
	if (m_playtype.size() < 22)
	{
		return false;
	}
	return m_playtype[21] == 0 ? false : true;
}

/******************************************************
* ��������	NiuNiu_NewAllowTypeKuaiLe()
* ������	ţţ�������ͣ�����ţ10��
* ��ϸ��	����ţ��false-������true-����
* ���أ�	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewAllowTypeKuaiLe() const
{
	if (m_playtype.size() < 23)
	{
		return false;
	}
	return m_playtype[22] == 0 ? false : true;
}

/******************************************************
* ��������	NiuNiu_NewFastStyle()
* ������	ţţ����ģʽ
* ��ϸ��	����ģʽ��false-��������true-����
* ���أ�	(bool)
*******************************************************/
bool PlayType::NiuNiu_NewFastStyle() const
{
	if (m_playtype.size() < 24)
	{
		return false;
	}
	return m_playtype[23] == 0 ? false : true;
}



/* ţţÿ�ֲ����ʼ���ֶΣ��ṹ�壩*/
struct NiuNiu_NewRoundState__c_part
{
	Lint		 m_curPos;						                    // ��ǰ�������
	Lint         m_play_status;                                     // �ƾ�״̬
	bool         m_tick_flag;                                       // ��ʱ������
	Lint		 m_player_tui_score[NIUNIU_PLAY_USER_COUNT][4];	// ��ҿ�����ע�ķ��������4����עѡ��

	Lint         m_user_status[NIUNIU_PLAY_USER_COUNT];             // �û�״̬����λ�����Ƿ������ˣ�ʵ����Чλ�ã�
	Lint         m_play_add_score[NIUNIU_PLAY_USER_COUNT];          // �����ע����
	Lint		 m_noQiangZhuang[NIUNIU_PLAY_USER_COUNT];			//����ֵ������������ׯ
	Lint         m_play_qiang_zhuang[NIUNIU_PLAY_USER_COUNT];       // �����ׯ������ Ĭ�ϣ�-1����Чλ��δ������ 0������ׯ�� >0��ׯ����
	bool         m_isOpenCard[NIUNIU_PLAY_USER_COUNT];              // ����Ƿ��ƣ�false-δ���ƣ�true-�ѿ���,��ʼΪ��false
	BYTE         m_player_hand_card[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];   //�û�����	
	BYTE		 m_show_hand_card[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];	 //��ұ�����Ӯ����
	Lint         m_player_score[NIUNIU_PLAY_USER_COUNT];            // ��ұ��ֵ÷�
	Lint		 m_player_coins[NIUNIU_PLAY_USER_COUNT];			// ��ұ���ʵ����Ӯ����ֵ
	Lint		 m_accum_coins[NIUNIU_PLAY_USER_COUNT];				//�������ֵ��¼
	Lint         m_player_oxnum[NIUNIU_PLAY_USER_COUNT];            // ���ţ��
	Lint         m_player_oxtimes[NIUNIU_PLAY_USER_COUNT];			//���ţţ����;
	Lint		 m_player_coins_status[NIUNIU_PLAY_USER_COUNT];		//���������ֵ����ʱ�Ƿ���  0������  1��������
																	//BYTE		 m_cbOxCard[NIUNIU_PLAY_USER_COUNT];				// ţţ����
	Lint		 m_player_maima[NIUNIU_PLAY_USER_COUNT][2];			// ������������0-���������1������λ��
	Lint		 m_canXiaMinZhu[NIUNIU_PLAY_USER_COUNT];			//��ע���ƣ���ׯ�������û����������ң���ע��ʱ��������С��  0:������ע��С��  1����������ע��С��
	Lint		 m_canAddScoreDouble[NIUNIU_PLAY_USER_COUNT];		//�Ƿ���ע�������ֶ�������ׯ�ұ����ߵ�û������ׯ�ҵ������עʱ������ʹ�üӱ����ܡ�  0�����ܷ���   1�����Է���
	Lint		 m_add_score_double_status[NIUNIU_PLAY_USER_COUNT]; //�Ƿ�ӱ�״̬�� 0���û�мӱ�   1����Ҽӱ�
	BYTE		 m_gong_card;										//����
	Lint		 m_open_gong_pos;									//�깫�Ƶ����λ��
	Lstring		 m_cuo_pai;

	Lint		 m_real_player_count;								//�������ʵ�ʲ�������
	Lint		 m_qiang_zhuang_count;								//��������ׯ�������
	Lint		 m_next_round_cartoon_time;							//���ֽ�������ʱ��
	Lint		 m_add_score_delay_time;							//��ׯ��ʱʱ��


	/* ����ÿ���ֶ� */
	void clear_round()
	{
		memset(this, 0, sizeof(*this));
		m_open_gong_pos = NIUNIU_INVAILD_POS;

		//ĳЩ������Ҫ�������ʼֵ
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			m_noQiangZhuang[i] = 1;
			m_play_qiang_zhuang[i] = -1;
			m_player_oxnum[i] = -1;
			m_isOpenCard[i] = false;
			m_player_maima[i][0] = -1;
			m_player_maima[i][1] = NIUNIU_INVAILD_POS;
		}
	}
};

/* ţţÿ�ֻ��ʼ���ֶΣ��ṹ�壩*/
struct NiuNiu_NewRoundState__cxx_part
{
	LTime m_play_status_time;
	/* ����ÿ���ֶ� */
	void clear_round()
	{
		this->~NiuNiu_NewRoundState__cxx_part();
		new (this) NiuNiu_NewRoundState__cxx_part;
	}
};

/* ţţÿ�������ʼ�����ֶΣ��ṹ�壩*/
struct NiuNiu_NewMatchState__c_part
{
	Desk*         m_desk;           // ���Ӷ���
	Lint          m_round_offset;   // ��ǰ����
	Lint          m_round_limit;    // ��������ѡ���������
	Lint          m_accum_score[NIUNIU_PLAY_USER_COUNT];	//����ּܷ�¼
	
	Lint		  m_per_round_score[NIUNIU_PLAY_USER_COUNT];  //��һ�ӵķ���
	bool          m_dismissed;
	int           m_registered_game_type;
	int           m_player_count;   // �淨���Ĵ�����ʹ�õ���������ֶ�
	Lint          m_zhuangPos;      // ׯ��λ��
	Lint		  m_carToon;		// �Ƿ�ѡׯ�Ķ���  0������Ҫ   1��ׯ��û��û������ֵ�������ѡׯ����
	bool          m_isFirstFlag;    // �׾���ʾ��ʼ��ť,true�����׾�
	bool		  m_canTuiZhu[NIUNIU_PLAY_USER_COUNT];  // �Ƿ������ע 0 ��������ע��1������ע
	TuoGuanInfo   m_tuo_guan_info[NIUNIU_PLAY_USER_COUNT];  //����й���Ϣ

	Lint		  m_qiangCount[NIUNIU_PLAY_USER_COUNT];		//�����ׯ����
	Lint		  m_zhuangCount[NIUNIU_PLAY_USER_COUNT];	//�����ׯ����
	Lint		  m_tuiCount[NIUNIU_PLAY_USER_COUNT];		//�����ע����

	//Ĭ�ϵ���ʱ
	Lint		  m_time_qiang_zhuang;
	Lint		  m_time_add_score;
	Lint		  m_time_open_card;
	
	/////�淨Сѡ��
	Lint          m_mainMode;				// Сѡ��[0]�����淨��ǩ��1ţţ��ׯ��2������ׯ��3������ׯ��4ʮ�����ơ�5����ţţ
	Lint          m_playerNum;              // Сѡ��[1]������6�� 8�� 10��
	Lint          m_qiangZhuangTimes;       // Сѡ��[2]����ׯ���ʣ�1-1����2-2����3-3����4-4��
	Lint          m_addScoreTimes;          // Сѡ��[3]����ע���ʣ�1-1/2  2-2/4  3-3/6  4-4/8  5-5/10
	Lint		  m_autoStart;				// Сѡ��[4]���Զ���ʼ��1-��2����ʱ��ʼ��4-��4�˿���5-��5�˿���6-��6�˿���4-������ע
	Lint		  m_tuiZhuTimes;		    // Сѡ��[5]����ע������ 0������ע��5��5����ע��10��10����ע��15:15����ע��20:20����ע
	bool          m_isDynamicIn;            // Сѡ��[6]���Ƿ񿪾ֺ��ֹ���뷿�䣺false-���ֺ���Լ��뷿�䣬true-���ֺ󲻿ɼ��뷿��
	bool          m_isAutoPlay;             // Сѡ��[7]���Ƿ������йܣ�false-������true-����
	bool		  m_isLaiZi;				// Сѡ��[8]���Ƿ�������ӣ�false-������true-����
	bool		  m_isMaiMa;				// Сѡ��[9]���Ƿ��������룺false-������true-����
	bool		  m_isCuoPai;				// Сѡ��[10]���Ƿ�������ƣ�false-������true-����
	bool		  m_isConstAddScore;		// Сѡ��[11]���Ƿ���ע���ƣ�false-�����ƣ�true-����
	bool		  m_isConstTuiZhu;			// Сѡ��[12]���Ƿ���ע���ƣ�false-�����ƣ�true-����
	bool		  m_isAddScoreDouble;		// Сѡ��[13]���Ƿ���ע�ӱ���false-������ӱ���true-����ӱ�
	Lint		  m_niuNiuTimes;			// Сѡ��[14]��1��ţţx3  ţ��x2  ţ��x2  ţ��-��ţx1   2��ţţx4  ţ��x3  ţ��x2  ţ��x2  ţ�� - ��ţx1
	bool		  m_isBiYi;					// Сѡ��[15]���Ƿ��������ţ��false-������true-����
	bool		  m_isShunZi;				// Сѡ��[16]���Ƿ�����˳��ţ��false-������true-����
	bool		  m_isWuHua;				// Сѡ��[17]���Ƿ������廨ţ��false-������true-����
	bool		  m_isTongHua;				// Сѡ��[18]���Ƿ�����ͬ��ţ��false-������true-����
	bool		  m_isHuLu;					// Сѡ��[19]���Ƿ������«ţ��false-������true-����
	bool		  m_isZhaDan;				// Сѡ��[20]���Ƿ�����ը��ţ��false-������true-����
	bool		  m_isWuXiao;				// Сѡ��[21]���Ƿ�������Сţ��false-������true-����
	bool		  m_isKuaiLe;				// Сѡ��[22]���Ƿ��������ţ��false-������true-����
	bool		  m_fastStyle;				// Сѡ��[23]���Ƿ��Ǽ���ģʽ��false-��ͨģʽ�� true-����ģʽ



	/* ����ṹ���ֶ� */
	void clear_match()
	{
		memset(this, 0, sizeof(*this));
		m_zhuangPos = NIUNIU_INVAILD_POS;
		m_isFirstFlag = true;
		m_qiangZhuangTimes = 1;  //Ĭ����ׯ����1��
		m_playerNum = 6;
		m_niuNiuTimes = 1;
	}
};

/* ţţÿ�����ʼ�����ֶΣ��ṹ�壩*/
struct NiuNiu_NewMatchState__cxx_part
{
	NNGameLogic    m_gamelogic;     // ��Ϸ�߼�
	PlayType       m_playtype;	    // �����淨Сѡ��

									/* ���ÿ���ṹ�� */
	void clear_match()
	{
		this->~NiuNiu_NewMatchState__cxx_part();
		new (this) NiuNiu_NewMatchState__cxx_part;
	}
};

/*
*  ţţÿ������Ҫ�������ֶΣ��ṹ�壩
*  �̳� ��NiuNiuRoundState__c_pard, NiuNiuRoundState_cxx_part
*/
struct NiuNiu_NewRoundState : NiuNiu_NewRoundState__c_part, NiuNiu_NewRoundState__cxx_part
{
	void clear_round()
	{
		NiuNiu_NewRoundState__c_part::clear_round();
		NiuNiu_NewRoundState__cxx_part::clear_round();
	}
};

/*
*  ţţÿ������Ҫ�������ֶΣ��ṹ�壩
*  �̳У�NiuNiuMatchState__c_pard, NiuNiuMatchState_cxx_pard
*/
struct NiuNiu_NewMatchState : NiuNiu_NewMatchState__c_part, NiuNiu_NewMatchState__cxx_part
{
	void clear_match()
	{
		NiuNiu_NewMatchState__c_part::clear_match();
		NiuNiu_NewMatchState__cxx_part::clear_match();
	}
};

/*
*  ţţ����״̬���ṹ�壩
*  �̳У�NiuNiuRoundState, NiuNiuMatchState
*/
struct NiuNiu_NewDeskState : NiuNiu_NewRoundState, NiuNiu_NewMatchState
{

	/* ����ÿ������ */
	void clear_round()
	{
		NiuNiu_NewRoundState::clear_round();
	}

	/* ����ÿ������*/
	void clear_match(int player_count)
	{
		NiuNiu_NewMatchState::clear_match();
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
		updateFirstFlag();

		if (play_status >= GAME_PLAY_SELECT_ZHUANG && play_status <= GAME_PLAY_END)
		{
			m_play_status = play_status;
		}
	}

	/* ��ȡ������������ϵ�λ�� */
	Lint GetUserPos(User* pUser)
	{
		if (NULL == pUser)
		{
			return NIUNIU_INVAILD_POS;
		}
		Lint pos = NIUNIU_INVAILD_POS;
		if (m_desk)
		{
			pos = m_desk->GetUserPos(pUser);
		}
		if (pos >= m_player_count)
		{
			pos = NIUNIU_INVAILD_POS;
		}
		return pos;
	}

	/* ��ȡ��һ����Чλ�� */
	Lint GetNextPos(Lint pos)
	{
		if (m_player_count == 0) return 0; // ����������	
		Lint nextPos = (pos + 1) % m_player_count;
		for (Lint i = 0; (i < m_player_count) && (m_user_status[nextPos] != 1); i++)
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
		return 0 <= pos && pos < m_player_count;
	}

	void updateFirstFlag()
	{
		if (m_desk)
		{
			m_desk->m_finish_first_opt = 0;
		}
	}
};

/* ţţ¼����(�ṹ��) */
struct NiuNiu_NewVideoSupport : NiuNiu_NewDeskState
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
};

/* ţţ��Ϸ���崦���߼����ṹ�壩*/
struct NiuNiu_NewGameCore : GameHandler, NiuNiu_NewVideoSupport
{
	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
	}

	/* �㲥����������������Ϸ�����*/
	virtual void notify_seat_playing(LMsg& msg) {}

	/* �㲥������������ս����� */
	virtual void notify_seat_lookon(LMsg& msg) {}

	virtual void notify_lookon(LMsg &msg) {
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

		//�㲥����ս���
		notify_lookon(msg);
	}
	/*****************************************************
	* ��������    notify_zhuang()
	* ������      ֪ͨ�ͻ��������������ׯ��λ��
	* ������
	*  @ pos      ׯ��λ��
	* ���أ�      (void)
	******************************************************/
	void notify_zhuang(Lint select_zhuang_pos, Lint allPlayerNoQiang, Lint score_times = 1)
	{
		if (select_zhuang_pos < 0 || select_zhuang_pos >m_player_count)
		{
			LLOG_ERROR("desk_id=[%d]send_zhuang_cmd pos error!!!", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}
		if (select_zhuang_pos >= 0 &&
			select_zhuang_pos < m_player_count &&
			m_user_status[select_zhuang_pos] == 1)
		{
			m_zhuangPos = select_zhuang_pos;

			calc_tui_zhu_score();

			//�طż�¼��������
			m_video.setCommonInfo(gWork.GetCurTime().Secs(), m_round_offset, m_player_count, m_desk->m_vip->m_posUserId, m_zhuangPos);

			NiuNiuS2CStartGame send_start;
			send_start.m_remainTime = m_time_add_score;
			send_start.m_zhuangPos = select_zhuang_pos;
			send_start.m_scoreTimes = score_times;
			send_start.m_maiMa = (m_isMaiMa && calc_vaild_player_count() > 2) ? 1 : 0;
			send_start.m_isAllNoQiang = allPlayerNoQiang;
			//��ע����ҿ���ע״̬
			memcpy(send_start.m_isCanTui, m_canTuiZhu, sizeof(send_start.m_isCanTui));
			memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
			memcpy(send_start.m_playerXiaScore, m_canXiaMinZhu, sizeof(send_start.m_playerXiaScore));
			memcpy(send_start.m_playerAddScoreDouble, m_canAddScoreDouble, sizeof(send_start.m_playerAddScoreDouble));
			memcpy(send_start.m_playerStatus, m_user_status, sizeof(m_user_status));
			memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
			if (m_mainMode != MAIN_MODE_NIUNIU_SHANG_ZHUANG && m_mainMode != MAIN_MODE_ZI_YOU_QIANG_ZHUANG)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1) continue;

					for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; ++j)
					{
						send_start.m_playerHandCard[j] = m_player_hand_card[i][j];
					}
					notify_user(send_start, i);
				}
				notify_seat_lookon(send_start);
				//�㲥��ս��ң���Ϸ��ʼ
				notify_lookon(send_start);
			}
			else
			{
				notify_desk(send_start);
				//�㲥��ս��ң���Ϸ��ʼ
				notify_lookon(send_start);
			}

			//��¼��ׯ����
			++m_zhuangCount[m_zhuangPos];
		}
	}

	///////////////////////////////////////////////////////////////////////////////

	/* ������������ж�������������Чλ�ã�*/
	Lint calc_vaild_player_count()
	{
		Lint tmp_player_count = 0;
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_user_status[i] == 1)
			{
				++tmp_player_count;
			}
		}
		return tmp_player_count;
	}

	/* ȷ����С����ע���� */
	Lint min_add_score(Lint pos = NIUNIU_INVAILD_POS)
	{
		if (pos == NIUNIU_INVAILD_POS)
		{
			return 0;
		}
		// ��ע���� && ������С��ע && ѡ��ӱ�
		if (m_isConstAddScore && m_canXiaMinZhu[pos] == 1 && m_add_score_double_status[pos] == 1)
		{
			return 4 * m_addScoreTimes;
		}
		//��ע���� && ������С��ע
		else if (m_isConstAddScore && m_canXiaMinZhu[pos] == 1)
		{
			return 2 * m_addScoreTimes;
		}
		//ѡ����ע�ӱ�
		else if (m_add_score_double_status[pos] == 1)
		{
			return 2 * m_addScoreTimes;
		}
		//������ע
		else
		{
			return m_addScoreTimes;
		}
	}

	/* �жϿ���ѡׯ�淨���Ƿ���������Ѿ���ѡׯ������� */
	bool is_select_zhuang_over()
	{
		Lint userSelectZhuang = 0;
		for (Lint i = 0; i < m_player_count; i++)
		{

			if (m_play_qiang_zhuang[i] != -1)
			{
				++userSelectZhuang;
			}
		}

		if (userSelectZhuang == calc_vaild_player_count())
		{
			//��������ڹ涨ʱ���ڲ�����ɣ��ر���ׯ����ʱ
			m_tick_flag = false;
			return true;
		}
		return false;
	}

	/* �ж��Ƿ��������������� */
	bool is_mai_ma_over()
	{
		Lint userMaiMaCount = 0;
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_player_maima[i][0] != -1 && m_user_status[i] == 1)
			{
				++userMaiMaCount;
			}
		}

		if (userMaiMaCount == calc_vaild_player_count() - 1)
		{
			return true;
		}
		return false;
	}

	/* �ж��Ƿ�������Ҷ�����ע�� */
	bool is_add_score_over()
	{
		Lint userSelectScore = 0;
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_play_add_score[i] != 0 && m_user_status[i] == 1)
			{
				++userSelectScore;
			}
		}

		if (userSelectScore == calc_vaild_player_count())
		{
			//���������ע��ɣ������ƾ�״̬Ϊ����
			set_play_status(GAME_PLAY_SEND_CARD);
			return true;
		}

		return false;
	}

	/* �ж���������Ƿ񶼵㿪���� */
	bool is_open_card_over()
	{
		Lint tmp_user_open = 0;
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (true == m_isOpenCard[i])
			{
				++tmp_user_open;
			}
		}

		if (tmp_user_open == calc_vaild_player_count())
		{
			//��ʾ������Ҷ��Ѿ�ѡ����
			set_play_status(GAME_PLAY_END);
			return true;
		}
		return false;
	}

	/* �����ӳ�ʱ�� */
	Lint calc_delay_time(GAME_PLAY_STATE state)
	{
		double t_delayTime = 0.00;
		Lint t_realPlayCount = calc_vaild_player_count();
		if (GAME_PLAY_ADD_SCORE == state || GAME_PLAY_MAI_MA == state)
		{
			if (MAIN_MODE_NIUNIU_SHANG_ZHUANG == m_mainMode)
			{
				if (m_round_offset == 0 || m_carToon)
				{
					t_delayTime = 0.2 * 4 * t_realPlayCount;
				}
			}
			else
			{
				if (m_qiang_zhuang_count == 0)
				{
					t_delayTime = 0.2 * 4 * t_realPlayCount;
				}
				else if (m_qiang_zhuang_count > 1)
				{
					t_delayTime = 0.2 * 4 * m_qiang_zhuang_count;
				}
				else if (m_qiang_zhuang_count == 1)
				{
					t_delayTime = 0.00;
				}
			}
		}
		//���ֽ�������ʱ��
		else if (GAME_PLAY_END == state)
		{
			//����ʱ��
			t_delayTime = 1.5 * t_realPlayCount;

			//��������ʱ��
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1) continue;

				//����ţ
				if (m_player_oxnum[i] == 100) t_delayTime += 1.0;
				//˳��ţ
				else if (m_player_oxnum[i] == 102) t_delayTime += 1.0;
				//�廨ţ
				else if (m_player_oxnum[i] == 103) t_delayTime += 1.0;
				//ͬ��ţ
				else if (m_player_oxnum[i] == 107) t_delayTime += 1.0;
				//��«ţ
				else if (m_player_oxnum[i] == 108) t_delayTime += 1.0;
				//ը��ţ
				else if (m_player_oxnum[i] == 109) t_delayTime += 1.0;
				//��Сţ
				else if (m_player_oxnum[i] == 110) t_delayTime += 1.0;
				//ը��ţ
				else if (m_player_oxnum[i] == 111) t_delayTime += 1.0;
			}
			
			//ׯ��ȫӮʱ��
			t_delayTime += 1.0;
		}
		
		//����ʱ��
		if (t_delayTime == 0.00) return 0;
		else if (t_delayTime == (double)((int)t_delayTime)) return (int)t_delayTime;
		else return (int)(t_delayTime + 1);
	}

	///////////////////////////////////////////////////////////////////////////////////

	//�й���ׯ����
	int tuoguan_qiang_zhuang(Lint pos)
	{
		//ţţ��ׯ��������ׯ������ţţ ��ׯ�й�Ϊ����
		if (MAIN_MODE_NIUNIU_SHANG_ZHUANG == m_mainMode || MAIN_MODE_ZI_YOU_QIANG_ZHUANG == m_mainMode || MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
		{
			return 0;
		}
		//������ׯ����������
		if (m_tuo_guan_info[pos].m_zhuangScore == 0)
		{
			return 0;
		}
		int t_4cardsOxNum = m_gamelogic.GetCardsOxNumber(m_player_hand_card[pos], 4);
		int t_shiWeiNum = m_tuo_guan_info[pos].m_zhuangScore / 10;
		int t_geWeiNum = m_tuo_guan_info[pos].m_zhuangScore % 10;

		if (t_shiWeiNum < 8 || t_geWeiNum < 1 || t_geWeiNum > 4 || t_4cardsOxNum < t_shiWeiNum)
		{
			return 0;
		}

		return t_geWeiNum;
	}

	//�й��Զ��·�
	int tuoguan_add_score(Lint pos)
	{
		if (m_tuo_guan_info[pos].m_addScore / m_addScoreTimes == 2)
		{
			return 2 * m_addScoreTimes;
		}
		else if (m_tuo_guan_info[pos].m_addScore / m_addScoreTimes == 1)
		{
			return m_addScoreTimes;
		}

		if (MAIN_MODE_NIUNIU_SHANG_ZHUANG == m_mainMode || MAIN_MODE_ZI_YOU_QIANG_ZHUANG == m_mainMode || MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
		{
			return m_addScoreTimes;
		}

		int t_4cardsOxNum = m_gamelogic.GetCardsOxNumber(m_player_hand_card[pos], 4);
		int t_shiWeiNum = m_tuo_guan_info[pos].m_addScore / 10;
		int t_geWeiNum = m_tuo_guan_info[pos].m_addScore % 10;

		if (t_shiWeiNum < 8 || t_geWeiNum != 2 || t_4cardsOxNum < t_shiWeiNum)
		{
			return m_addScoreTimes;
		}
		else
		{
			return 2 * m_addScoreTimes;
		}


	}

	//�й���ע
	int tuoguan_tui_score(Lint pos)
	{
		if (MAIN_MODE_NIUNIU_SHANG_ZHUANG == m_mainMode || MAIN_MODE_ZI_YOU_QIANG_ZHUANG == m_mainMode || MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
		{
			return 0;
		}

		if (m_tuo_guan_info[pos].m_tuiScore == 0)
		{
			return 0;
		}

		int t_4cardsOxNum = m_gamelogic.GetCardsOxNumber(m_player_hand_card[pos], 4);
		int t_shiWeiNum = m_tuo_guan_info[pos].m_addScore / 10;
		int t_geWeiNum = m_tuo_guan_info[pos].m_addScore % 10;

		if (t_shiWeiNum < 8 || t_geWeiNum != 1 || t_4cardsOxNum < t_shiWeiNum)
		{
			return 0;
		}
		else
		{
			return m_player_tui_score[pos][0];
		}
	}

	/*���������ע����*/
	void calc_tui_zhu_score()
	{
		LLOG_ERROR("NiuNiu_NewGameHandler::start_game() Run... calc palyer tui zhu score deskId=[%d]",
			m_desk ? m_desk->GetDeskId() : 0);

		int t_tuiScore = m_tuiZhuTimes;

		//�����ע����
		int t_maxTuiZhuScore = 0;

		//�ĸ���ע����
		int t_tuiZhuScore[4];
		memset(t_tuiZhuScore, 0, sizeof(t_tuiZhuScore));

		int t_tuiGread = 0;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			memset(t_tuiZhuScore, 0, sizeof(t_tuiZhuScore));
			if (m_canTuiZhu[i])
			{
				t_maxTuiZhuScore = (m_per_round_score[i] + m_addScoreTimes * 2) < (m_addScoreTimes * m_tuiZhuTimes) ? (m_per_round_score[i] + m_addScoreTimes * 2) : (m_addScoreTimes * m_tuiZhuTimes);
				t_tuiGread = t_maxTuiZhuScore / (5 * m_addScoreTimes);

				for (Lint j = 0; j <= t_tuiGread; ++j)
				{
					if (t_maxTuiZhuScore == 5 * j * m_addScoreTimes)
					{
						break;
					}
					if (j == t_tuiGread)
					{
						if (t_maxTuiZhuScore > 2 * m_addScoreTimes)
						{
							t_tuiZhuScore[j] = t_maxTuiZhuScore;
						}
						break;
					}
					t_tuiZhuScore[j] = (j + 1)*(5 * m_addScoreTimes);
				}
				memcpy(m_player_tui_score[i], t_tuiZhuScore, sizeof(int) * 4);

			}
			else
			{
				memset(m_player_tui_score[i], 0, sizeof(int) * 4);
			}
			LLOG_ERROR("NiuNiu_NewGameHandler::start_game() Run... deskId=[%d], m_plauer_tui_score[%d]=[%d | %d | %d | %d]",
				m_desk ? m_desk->GetDeskId() : 0, i, m_player_tui_score[i][0], m_player_tui_score[i][1], m_player_tui_score[i][2], m_player_tui_score[i][3]);
		}
	}
	/********************************************************************
	* ��������      send_user_card()
	* ������        ����ҷ��� �� ����
	* ������
	*  @ pos ���λ��
	*  @ show_card �Ƿ�Ϊ���� Ĭ��Ϊfalse
	* ���أ�       (bool)
	*********************************************************************/
	bool send_user_card(Lint pos, bool show_card = false)
	{
		if (pos == NIUNIU_INVAILD_POS) return false;

		//����
 		if (show_card)
		{
			if (pos != m_zhuangPos)
			{
				NiuNiuS2COpenCard open_card;
				open_card.m_remainTime = m_time_open_card;
				open_card.m_pos = pos;
				//��Ϣ���Ϊ����
				open_card.m_show = 1;
				//����ÿ����ҵ�״̬
				memcpy(open_card.m_playerStatus, m_user_status, sizeof(open_card.m_playerStatus));
				open_card.m_oxNum = m_player_oxnum[pos];
				open_card.m_oxTimes = m_player_oxtimes[pos];
				//������λ���Ǹող�����λ�ã�������Ƹ��Ƹ������
				for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
				{
					open_card.m_playerHandCard[j] = (Lint)m_player_hand_card[pos][j];
					open_card.m_showHandCard[j] = (Lint)m_show_hand_card[pos][j];
				}
				notify_desk(open_card);
				notify_lookon(open_card);
			}
			else
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					NiuNiuS2COpenCard open_card;
					open_card.m_remainTime = m_time_open_card;
					open_card.m_pos = pos;
					//��Ϣ���Ϊ����
					open_card.m_show = 1;
					//����ÿ����ҵ�״̬
					memcpy(open_card.m_playerStatus, m_user_status, sizeof(open_card.m_playerStatus));
					if (i == m_zhuangPos)
					{
						open_card.m_oxNum = m_player_oxnum[pos];
						open_card.m_oxTimes = m_player_oxtimes[pos];
						//������λ���Ǹող�����λ�ã�������Ƹ��Ƹ������
						for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
						{
							open_card.m_playerHandCard[j] = (Lint)m_player_hand_card[pos][j];
							open_card.m_showHandCard[j] = (Lint)m_show_hand_card[pos][j];
						}
					}
					notify_user(open_card, i);
				}

				//�㲥��ս���
				NiuNiuS2COpenCard gz_open_card;
				gz_open_card.m_remainTime = m_time_open_card;
				gz_open_card.m_pos = pos;
				//��Ϣ���Ϊ����
				gz_open_card.m_show = 1;
				//����ÿ����ҵ�״̬
				memcpy(gz_open_card.m_playerStatus, m_user_status, sizeof(gz_open_card.m_playerStatus));

				notify_seat_lookon(gz_open_card);
				notify_lookon(gz_open_card);
			}
		}
		//����
		else
		{
			//����ţţ����
			m_player_oxnum[pos] = m_gamelogic.GetCardType(m_player_hand_card[pos], NIUNIU_HAND_CARD_MAX, m_show_hand_card[pos]);
			//����ţţ����
			m_player_oxtimes[pos] = m_gamelogic.GetTimes(m_player_hand_card[pos], NIUNIU_HAND_CARD_MAX);

			NiuNiuS2COpenCard send_card;
			send_card.m_pos = pos;

			//��Ϣ���Ϊ����
			send_card.m_show = 0;
			send_card.m_gongCard = m_gong_card;
			send_card.m_openGongPos = m_open_gong_pos;
			//����ÿ����ҵ�״̬
			memcpy(send_card.m_playerStatus, m_user_status, sizeof(m_user_status));

			//������ҷ�����
			for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX; i++)
			{
				send_card.m_playerHandCard[i] = m_player_hand_card[pos][i];
				send_card.m_showHandCard[i] = (Lint)m_show_hand_card[pos][i];
			}
			send_card.m_oxNum = m_player_oxnum[pos];
			send_card.m_oxTimes = m_player_oxtimes[pos];

			//����֪ͨ���
			notify_user(send_card, pos);

			if (MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1 || i == pos)
					{
						continue;
					}
					NiuNiuS2COpenCard go_send_card;
					go_send_card.m_pos = i;

					//��Ϣ���Ϊ����
					go_send_card.m_show = 0;
					go_send_card.m_gongCard = m_gong_card;
					go_send_card.m_openGongPos = m_open_gong_pos;
					//����ÿ����ҵ�״̬
					memcpy(go_send_card.m_playerStatus, m_user_status, sizeof(go_send_card.m_playerStatus));

					//������ҷ�����
					for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
					{
						go_send_card.m_playerHandCard[j] = m_player_hand_card[i][j];
					}

					//����֪ͨ���
					notify_user(go_send_card, pos);
				}
			}

			if (MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
			{
				//�㲥��ս��ң���ҷ���
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1)
					{
						continue;
					}
					NiuNiuS2COpenCard gz_goSendCard;
					gz_goSendCard.m_pos = i;

					//��Ϣ���Ϊ����
					gz_goSendCard.m_show = 0;
					gz_goSendCard.m_gongCard = m_gong_card;
					gz_goSendCard.m_openGongPos = m_open_gong_pos;
					//����ÿ����ҵ�״̬
					memcpy(gz_goSendCard.m_playerStatus, m_user_status, sizeof(gz_goSendCard.m_playerStatus));

					//������ҷ�����
					for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
					{
						gz_goSendCard.m_playerHandCard[j] = m_player_hand_card[i][j];
					}
					notify_seat_lookon(gz_goSendCard);
					notify_lookon(gz_goSendCard);
				}
			}
			else if(MAIN_MODE_GONG_PAI_NIU_NIU != m_mainMode)
			{
				//�㲥��ս��ң���ҷ���
				NiuNiuS2COpenCard gz_sendCard;
				gz_sendCard.m_pos = pos;
				gz_sendCard.m_show = 0;
				memcpy(gz_sendCard.m_playerStatus, m_user_status, sizeof(m_user_status));
				notify_seat_lookon(gz_sendCard);
				notify_lookon(gz_sendCard);
			}
			
		}
		return true;
	}

	void add_round_log(Lint* accum_score, Lint win_pos)
	{
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->UpdateNiuNiuOptCount(m_qiangCount, m_zhuangCount, m_tuiCount, m_player_count);
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, accum_score, m_user_status, win_pos, m_video.m_Id);
		}
	}

	//���ֲ����
	void calc_club_score()
	{
		//�����֣�������Ӯ
		if (m_desk->m_isAllowCoinsNegative)
		{
			memcpy(m_player_coins, m_player_score, sizeof(m_player_coins));
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] != 1)
				{
					continue;
				}

				m_accum_coins[i] += m_player_score[i];
			}
			return;
		}

		Lint t_zhuangWinScore = 0;
		Lint t_xianCoins = 0;
		Lint t_playerWinInfoCount = 0;
		//��¼ÿ����ұ��ֿ�ʼʱ�������ֵ����������Ӯȡ��ֵ
		Lint t_playerCanWinMaxCoins[NIUNIU_PLAY_USER_COUNT];
		memcpy(t_playerCanWinMaxCoins, m_accum_coins, sizeof(t_playerCanWinMaxCoins));
		for (Lint i = 0; i < m_player_count; ++i)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::calc_club_score() Run... deskId=[%d], player[%d] before [coins | playScore | oxNum | oxTimes]=[%d | %d | %d | %d]",
				m_desk ? m_desk->GetDeskId() : 0, i, m_accum_coins[i], m_player_score[i], m_player_oxnum[i], m_player_oxtimes[i]);
		}

		PlayerWinInfo t_playerWinInfo[NIUNIU_PLAY_USER_COUNT];
		for (Lint i = 0; i < m_player_count; ++i)
		{
			t_zhuangWinScore = 0;
			if (i == m_zhuangPos || m_user_status[i] != 1) continue;

			//�м���ķ���
			if (m_player_score[i] <= 0)
			{
				t_xianCoins = m_accum_coins[i];
				//t_zhuangWinScore = -m_player_score[i];
				t_zhuangWinScore = (t_playerCanWinMaxCoins[m_zhuangPos] < (-1 * m_player_score[i])) ? t_playerCanWinMaxCoins[m_zhuangPos] : (-1 * m_player_score[i]);

				if (!m_desk->m_isAllowCoinsNegative && t_zhuangWinScore > t_xianCoins)
				{
					m_player_coins[m_zhuangPos] += t_xianCoins;
					m_accum_coins[m_zhuangPos] += t_xianCoins;

					m_player_coins[i] -= t_xianCoins;
					m_accum_coins[i] -= t_xianCoins;

					m_player_coins_status[i] = 1;
				}
				else
				{
					m_player_coins[m_zhuangPos] += t_zhuangWinScore;
					m_accum_coins[m_zhuangPos] += t_zhuangWinScore;

					m_player_coins[i] -= t_zhuangWinScore;
					m_accum_coins[i] -= t_zhuangWinScore;
				}
			}
			//�м�Ӯ�ģ���Ҫ����ṹ��������ټ������
			else if (m_player_score[i] > 0)
			{
				t_playerWinInfo[t_playerWinInfoCount].m_index = i;
				t_playerWinInfo[t_playerWinInfoCount].m_oxNum = m_player_oxnum[i];
				t_playerWinInfo[t_playerWinInfoCount].m_oxTimes = m_player_oxtimes[i];
				t_playerWinInfo[t_playerWinInfoCount].m_playerScore = m_player_score[i];
				++t_playerWinInfoCount;
			}
		}

		//��ţţ��С����Ӯׯ�ҵ��м�
		PlayerWinInfo t_tmp;
		for (Lint i = 0; i < t_playerWinInfoCount - 1; ++i)
		{
			for (Lint j = 0; j < t_playerWinInfoCount - 1 - i; ++j)
			{
				if (t_playerWinInfo[j].m_oxNum < t_playerWinInfo[j + 1].m_oxNum)
				{
					t_tmp = t_playerWinInfo[j];
					t_playerWinInfo[j] = t_playerWinInfo[j + 1];
					t_playerWinInfo[j + 1] = t_tmp;
				}
				else if (t_playerWinInfo[j].m_oxNum == t_playerWinInfo[j + 1].m_oxNum)
				{
					if (!m_gamelogic.CompareCard(m_player_hand_card[t_playerWinInfo[j].m_index], m_player_hand_card[t_playerWinInfo[j + 1].m_index], 5, t_playerWinInfo[j].m_oxNum, t_playerWinInfo[j + 1].m_oxNum))
					{
						t_tmp = t_playerWinInfo[j];
						t_playerWinInfo[j] = t_playerWinInfo[j + 1];
						t_playerWinInfo[j + 1] = t_tmp;
					}
				}
			}
		}
		//ׯ����ķ���
		Lint t_zhuangCoins = m_accum_coins[m_zhuangPos];
		Lint t_zhuangLoseScore = 0;
		Lint t_xianPos = NIUNIU_INVAILD_POS;
		for (Lint i = 0; i < t_playerWinInfoCount; ++i)
		{
			t_zhuangCoins = m_accum_coins[m_zhuangPos];
			t_xianPos = t_playerWinInfo[i].m_index;
			//t_zhuangLoseScore = t_playerWinInfo[i].m_playerScore;
			t_zhuangLoseScore = (t_playerCanWinMaxCoins[t_xianPos] < t_playerWinInfo[i].m_playerScore) ? t_playerCanWinMaxCoins[t_xianPos] : t_playerWinInfo[i].m_playerScore;

			if (!m_desk->m_isAllowCoinsNegative && t_zhuangLoseScore > t_zhuangCoins)
			{
				m_player_coins[m_zhuangPos] -= t_zhuangCoins;
				m_accum_coins[m_zhuangPos] -= t_zhuangCoins;

				m_player_coins[t_xianPos] += t_zhuangCoins;
				m_accum_coins[t_xianPos] += t_zhuangCoins;

				m_player_coins_status[m_zhuangPos] = 1;
			}
			else
			{
				m_player_coins[m_zhuangPos] -= t_zhuangLoseScore;
				m_accum_coins[m_zhuangPos] -= t_zhuangLoseScore;

				m_player_coins[t_xianPos] += t_zhuangLoseScore;
				m_accum_coins[t_xianPos] += t_zhuangLoseScore;
			}
		}

		memcpy(m_player_score, m_player_coins, sizeof(m_player_score));
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;
			LLOG_ERROR("NiuNiu_NewGameHandler::calc_club_score() Run... deskId=[%d], player[%d] after [coins | playScore | oxNum | oxTimes]=[%d | %d | %d | %d]",
				m_desk ? m_desk->GetDeskId() : 0, i, m_accum_coins[i], m_player_score[i], m_player_oxnum[i], m_player_oxtimes[i]);
		}
	}

	//���ֲ����1������
	void calc_club_score1(const Lint* winCount, const Lint* winTimes)
	{
		Lint t_zhuangWinScore = 0;
		Lint t_zhuangWinMaiScore = 0;
		Lint t_xianCoins = 0;
		Lint t_playerWinInfoCount = 0;
		PlayerWinInfo t_playerWinInfo[NIUNIU_PLAY_USER_COUNT];
		for (Lint i = 0; i < m_player_count; ++i)
		{
			t_zhuangWinScore = 0;
			if (i == m_zhuangPos || m_user_status[i] != 1) continue;

			//�м�Ӯ�ģ���Ҫ����ṹ��������ټ������
			if (winCount[i] > 0)
			{
				t_playerWinInfo[t_playerWinInfoCount].m_index = i;
				t_playerWinInfo[t_playerWinInfoCount].m_oxNum = m_player_oxnum[i];
				t_playerWinInfo[t_playerWinInfoCount].m_oxTimes = m_player_oxtimes[i];
				++t_playerWinInfoCount;
			}
			//ׯ��Ӯ��
			else
			{
				t_xianCoins = m_desk->m_user[i]->m_userData.m_coins;
				t_zhuangWinScore = m_play_add_score[m_zhuangPos] * m_play_add_score[i] * winTimes[m_zhuangPos];

				if (!m_desk->m_isAllowCoinsNegative && t_zhuangWinScore > t_xianCoins)
				{
					m_player_score[m_zhuangPos] += t_xianCoins;
					m_player_score[i] -= t_xianCoins;
				}
				else
				{
					m_player_score[m_zhuangPos] += t_zhuangWinScore;
					m_player_score[i] -= t_zhuangWinScore;
				}

				//�м�����������
				for (Lint m = 0; m < m_player_count; ++m)
				{
					if (m_zhuangPos == m || m_player_maima[m][1] == m_zhuangPos) continue;

					if (m_player_maima[m][1] == i)
					{
						t_xianCoins = m_desk->m_user[m]->m_userData.m_coins;
						t_zhuangWinMaiScore = m_play_add_score[m_zhuangPos] * m_player_maima[m][0] * winTimes[m_zhuangPos];
						if (!m_desk->m_isAllowCoinsNegative && t_zhuangWinMaiScore > t_xianCoins)
						{
							m_player_score[m_zhuangPos] += t_xianCoins;
							m_player_score[m] -= t_xianCoins;
						}
						else
						{
							m_player_score[m_zhuangPos] += t_zhuangWinMaiScore;
							m_player_score[m] -= t_zhuangWinMaiScore;
						}
					}
				}
			}
		}

		//��ţţ��С����Ӯׯ�ҵ��м�
		PlayerWinInfo t_tmp;
		for (Lint i = 0; i < m_player_count - 1; ++i)
		{
			for (Lint j = 0; j < m_player_count - 1 - j; ++j)
			{
				if (t_playerWinInfo[j].m_oxNum < t_playerWinInfo[j + 1].m_oxNum)
				{
					t_tmp = t_playerWinInfo[j];
					t_playerWinInfo[j] = t_playerWinInfo[j + 1];
					t_playerWinInfo[j + 1] = t_tmp;
				}
				else if (t_playerWinInfo[j].m_oxNum == t_playerWinInfo[j + 1].m_oxNum)
				{
					if (!m_gamelogic.CompareCard(m_player_hand_card[t_playerWinInfo[j].m_index], m_player_hand_card[t_playerWinInfo[j + 1].m_index], 5, t_playerWinInfo[j].m_oxNum, t_playerWinInfo[j + 1].m_oxNum))
					{
						t_tmp = t_playerWinInfo[j];
						t_playerWinInfo[j] = t_playerWinInfo[j + 1];
						t_playerWinInfo[j + 1] = t_tmp;
					}
				}
			}
		}
		//ׯ����ķ���
		Lint t_zhuangCoins = m_desk->m_user[m_zhuangPos]->m_userData.m_coins;
		Lint t_zhuangLoseScore = 0;
		Lint t_xianPos = NIUNIU_INVAILD_POS;
		for (Lint i = 0; i < t_playerWinInfoCount; ++i)
		{
			t_zhuangCoins = m_desk->m_user[m_zhuangPos]->m_userData.m_coins;
			t_xianPos = t_playerWinInfo[i].m_index;
			t_zhuangLoseScore = m_play_add_score[m_zhuangPos] * m_play_add_score[t_xianPos] * winTimes[t_xianPos];
			if (!m_desk->m_isAllowCoinsNegative && t_zhuangLoseScore > t_zhuangCoins)
			{
				m_player_score[m_zhuangPos] -= t_zhuangCoins;
				m_player_score[i] += t_zhuangCoins;
			}
			else
			{
				m_player_score[m_zhuangPos] -= t_zhuangLoseScore;
				m_player_score[i] += t_zhuangLoseScore;
			}
		}
		//ׯ������м������
		for (Lint i = 0; i < t_playerWinInfoCount; ++i)
		{
			t_xianPos = t_playerWinInfo[i].m_index;
			for (Lint m = 0; m < m_player_count; ++m)
			{
				if (m_user_status[m] != 1 || m == m_zhuangPos || m_player_maima[m][1] == m_zhuangPos) continue;

				if (m_player_maima[m][1] == t_xianPos)
				{
					t_zhuangCoins = m_desk->m_user[m_zhuangPos]->m_userData.m_coins;
					t_zhuangLoseScore = m_play_add_score[m_zhuangPos] * m_player_maima[m][0] * winTimes[t_xianPos];
				}

				if (!m_desk->m_isAllowCoinsNegative && t_zhuangLoseScore > t_zhuangCoins)
				{
					m_player_score[m_zhuangPos] -= t_zhuangCoins;
					m_player_score[m] += t_zhuangCoins;
				}
				else
				{
					m_player_score[m_zhuangPos] -= t_zhuangLoseScore;
					m_player_score[m] += t_zhuangLoseScore;
				}
			}
		}
	}

	//С����
	void finish_round(Lint jiesan = 0)
	{
		LLOG_DEBUG("NiuNiu_NewGameHandler::finish_round() Run... This round is finished...deskId=[%d], round=[%d/%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit);

		if (0 != jiesan)
		{
			//�����ͻ���С����
			NiuNiuS2CResult jiesan_result;
			jiesan_result.m_isShowFee = m_round_offset == 0 ? 1 : 0;
			for (Lint i = 0; i < m_player_count; i++)
			{
				if (m_user_status[i] != 1)
				{
					continue;
				}
				for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
				{
					jiesan_result.m_playerHandCard[i][j] = (Lint)m_player_hand_card[i][j];
				}
			}
			memcpy(jiesan_result.m_oxNum, m_player_oxnum, sizeof(jiesan_result.m_oxNum));
			memcpy(jiesan_result.m_oxTimes, m_player_oxtimes, sizeof(jiesan_result.m_oxTimes));
			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				memcpy(jiesan_result.m_totleScore, m_desk->m_vip->m_coins, sizeof(jiesan_result.m_totleScore));
			}
			else
			{
				memcpy(jiesan_result.m_totleScore, m_desk->m_vip->m_score, sizeof(jiesan_result.m_totleScore));
			}
			notify_desk(jiesan_result);
			//�㲥��ս��ң�С����
			notify_lookon(jiesan_result);

			LLOG_ERROR("NiuNiu_NewGameHandler::finish_round() ... This has is dissolved... desk_id=[%d], desk_status=[%d], jie_san game over",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status);

			set_desk_state(DESK_WAIT);

			//������Ϸ
			if (m_desk) m_desk->HanderGameOver(jiesan);
			return;
		}

		increase_round();
		//�û����Ƹ���
		BYTE tmp_player_hand_card[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];
		//�����Ӯ����
		Lint tmp_win_times[NIUNIU_PLAY_USER_COUNT];
		//���ţţ����������ÿ�ֲ����Զ���ʼ���ж�����
		//Lint tmp_win_oxnum[NIUNIU_PLAY_USER_COUNT];
		//�����Ӯ����
		Lint tmp_win_count[NIUNIU_PLAY_USER_COUNT];
		//��ʼ������
		memset(tmp_player_hand_card, 0x00, sizeof(tmp_player_hand_card));
		memset(tmp_win_times, 0, sizeof(tmp_win_times));
		//memset(tmp_win_oxnum, 0, sizeof(tmp_win_oxnum));
		memset(tmp_win_count, 0, sizeof(tmp_win_count));

		//��ÿ����ҵ����Ƹ���һ������������ļ���
		CopyMemory(tmp_player_hand_card, m_player_hand_card, sizeof(tmp_player_hand_card));

		//ׯ�ұ���
		tmp_win_times[m_zhuangPos] = m_gamelogic.GetTimes(tmp_player_hand_card[m_zhuangPos], NIUNIU_HAND_CARD_MAX);
		LLOG_DEBUG("desk_id = [%d], *****ceshi***** tmp_win_times[m_zhuangPos] = %d", m_desk ? m_desk->GetDeskId() : 0, tmp_win_times[m_zhuangPos]);

		//�Ա����
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (i == m_zhuangPos || m_user_status[i] != 1) continue;

			//��ȡ����
			tmp_win_times[i] = m_gamelogic.GetTimes(tmp_player_hand_card[i], NIUNIU_HAND_CARD_MAX);
			if (m_gamelogic.CompareCard(tmp_player_hand_card[i], tmp_player_hand_card[m_zhuangPos], NIUNIU_HAND_CARD_MAX, m_player_oxnum[i], m_player_oxnum[m_zhuangPos]))
			{
				++tmp_win_count[i];
				LLOG_DEBUG("desk_id = [%d], *****ceshi***** tmp_win_times[%d] = %d", m_desk ? m_desk->GetDeskId() : 0, i, tmp_win_times[i]);
			}
			else
			{
				++tmp_win_count[m_zhuangPos];
			}
		}

		//ͳ����ҵ÷�
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (i == m_zhuangPos || m_user_status[i] != 1) continue;

			if (tmp_win_count[i] > 0)
			{
				//�м�Ӯ
				m_player_score[i] = m_play_add_score[m_zhuangPos] * m_play_add_score[i] * tmp_win_times[i];

				//��ע���м�Ӯ�����Ͼ�û����ע���򱾾ֿ�����ע��**Ren 2018-5-19��
				//if (m_tuiZhuTimes)
				//{
				//	//m_per_round_score[i] = m_player_score[i];
				//	m_canTuiZhu[i] = !m_canTuiZhu[i];
				//}

				LLOG_ERROR("desk_id = [%d] *****ceshi,Xian_Win *****,m_play_add_score[m_zhuangPos] = %d, m_play_add_score[%d] = %d, tmp_win_times[%d] = %d",
					m_desk ? m_desk->GetDeskId() : 0, m_play_add_score[m_zhuangPos], i, m_play_add_score[i], i, tmp_win_times[i]);
			}
			else
			{
				//ׯ��Ӯ
				m_player_score[i] = (-1) * m_play_add_score[m_zhuangPos] * m_play_add_score[i] * tmp_win_times[m_zhuangPos];

				//��ע��ׯ��Ӯ�ˣ��м��¾ֿ϶�û����עȨ��
				/*if (m_tuiZhuTimes)
				{
					m_canTuiZhu[i] = false;
				}*/

				LLOG_ERROR("desk_id = [%d] *****ceshi,Zhuang_Win *****,m_play_add_score[m_zhuangPos] = %d, m_play_add_score[%d] = %d, tmp_win_times[m_zhuangPos] = %d",
					m_desk ? m_desk->GetDeskId() : 0, m_play_add_score[m_zhuangPos], i, m_play_add_score[i], tmp_win_times[m_zhuangPos]);
			}

			//��������÷�
			if (m_player_maima[i][0] > 0)
			{
				if (tmp_win_count[m_player_maima[i][1]] > 0)
				{
					m_player_score[i] += m_play_add_score[m_zhuangPos] * m_player_maima[i][0] * tmp_win_times[m_player_maima[i][1]];
				}
				else
				{
					m_player_score[i] += (-1) * m_play_add_score[m_zhuangPos] * m_player_maima[i][0] * tmp_win_times[m_zhuangPos];
				}
			}


			m_player_score[m_zhuangPos] -= m_player_score[i];


		}

		//������ҽ����Ӯ
		if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
		{
			calc_club_score();
		}

		//�ҳ�����Ӯ��
		Lint wWinner = m_zhuangPos;
		Lint maxScore = m_player_score[m_zhuangPos];
		for (Lint i = 0; i < m_player_count; i++)
		{
			m_accum_score[i] += m_player_score[i];
			if (m_player_score[i] > maxScore)
			{
				maxScore = m_player_score[i];
				wWinner = i;
			}
		}

		//��ע��¼�Ͼ���ҵ÷�
		if (m_tuiZhuTimes)
		{
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && m_zhuangPos != i && m_player_score[i] > 0)
				{
					m_canTuiZhu[i] = !m_canTuiZhu[i];
					m_per_round_score[i] = m_player_score[i];
				}
				else
				{
					m_canTuiZhu[i] = false;
				}
			}
		}

		//�������[0-4] + ��������[5] + ���ͱ���[6] + ��ׯ[7] + ��ע[8]
		std::vector<Lint> t_handCards[NIUNIU_PLAY_USER_COUNT];

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] != 1) continue;

			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; ++j)
			{
				t_handCards[i].push_back(m_player_hand_card[i][j]);
			}
			t_handCards[i].push_back(m_player_oxnum[i]);
			t_handCards[i].push_back(tmp_win_times[i]);
			t_handCards[i].push_back(m_play_qiang_zhuang[i]);
			t_handCards[i].push_back(m_play_add_score[i]);
			t_handCards[i].push_back(((m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0 && m_accum_coins[i] <= 0) ? 1 : 0));
		}

		//�������� + �������� + ���Ʊ���
		m_video.setHandCards(m_player_count, t_handCards);

		//�طű������ұ��ֵ÷�
		m_video.setRoundScore(m_player_count, m_player_score);

		//����ط�
		//m_video.addUserScore(m_accum_score);
		if (m_desk && m_desk->m_vip && m_desk->m_vip->m_reset == 0)
		{
			VideoSave();
		}

		//�����ͻ���С����
		NiuNiuS2CResult send_result;
		//һ�ֽ���д��־
		add_round_log(m_player_score, wWinner);

		send_result.m_zhuangPos = m_zhuangPos;
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_user_status[i] != 1)
			{
				continue;
			}
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
			{
				send_result.m_playerHandCard[i][j] = (Lint)m_player_hand_card[i][j];
				send_result.m_showHandCard[i][j] = (Lint)m_show_hand_card[i][j];
			}
		}
		if (calc_vaild_player_count() > 2)
		{
			send_result.m_isZhuangAllWin = 1;
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_zhuangPos == i) continue;

				if (m_player_score[i] > 0)
				{
					send_result.m_isZhuangAllWin = 0;
					break;
				}
			}
			/*if (tmp_win_count[m_zhuangPos] == calc_vaild_player_count() - 1)
			{
				send_result.m_isZhuangAllWin = 1;
			}
			else if (tmp_win_count[m_zhuangPos] == 0)
			{
				send_result.m_isZhuangAllWin = 2;
			}*/
		}
		memcpy(send_result.m_playScore, m_player_score, sizeof(send_result.m_playScore));
		memcpy(send_result.m_oxNum, m_player_oxnum, sizeof(send_result.m_oxNum));
		memcpy(send_result.m_oxTimes, tmp_win_times, sizeof(send_result.m_oxTimes));
		memcpy(send_result.m_playerCoinsStatus, m_player_coins_status, sizeof(send_result.m_playerCoinsStatus));
		if (m_desk && m_desk->m_vip)
		{
			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				//���ֲ�������ֵ�����ˣ�����Ϊ������ս״̬
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (!m_desk->m_user[i]) continue;

					if (m_desk->m_vip->m_coins[i] <= 0)
					{
						m_user_status[i] = 2;
					}
					else
					{
						m_user_status[i] = 1;
					}
				}

				m_desk->SetHandlerPlayerStatus(m_user_status, m_player_count);
				memcpy(send_result.m_totleScore, m_accum_coins, sizeof(send_result.m_totleScore));
			}
			else
			{
				memcpy(send_result.m_totleScore, m_desk->m_vip->m_score, sizeof(send_result.m_totleScore));
			}

			LLOG_DEBUG("desk_id=[%d], send.m_TotalScore[%d %d %d %d %d %d]", m_desk ? m_desk->GetDeskId() : 0, send_result.m_totleScore[0],
				send_result.m_totleScore[1], send_result.m_totleScore[2],
				send_result.m_totleScore[3], send_result.m_totleScore[4],
				send_result.m_totleScore[5]);
		}
		else
		{
			LLOG_ERROR("m_desk or  m_desk->m_vip is null ");
		}
		memcpy(send_result.m_playerStatus, m_user_status, sizeof(send_result.m_playerStatus));

		LLOG_DEBUG("desk_id=[%d], GAME_END m_ZhuangPos=[%d]", m_desk ? m_desk->GetDeskId() : 0, send_result.m_zhuangPos);
		for (Lint i = 0; i < m_player_count; i++)
		{
			LLOG_DEBUG("GAME_END playerPos = %d", i);
			LLOG_ERROR("DeskId=[%d],GAME_END player%dHandCard = [%d,%d,%d,%d,%d]", m_desk ? m_desk->GetDeskId() : 0, i, m_player_hand_card[i][0], m_player_hand_card[i][1], m_player_hand_card[i][2], m_player_hand_card[i][3], m_player_hand_card[i][4]);
			LLOG_DEBUG("GAME_END m_Score[%d]", send_result.m_playScore[i]);
		}

		notify_desk(send_result);
		//�㲥��ս��ң�С����
		notify_lookon(send_result);
		set_desk_state(DESK_WAIT);

		if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
		{
			//���ֲ�������ֵ�����ˣ�����Ϊ������ս״̬
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (!m_desk->m_user[i]) continue;

				//ÿ�ֿ�ʼ��������ƾ���������ֵ����
				m_accum_coins[i] = m_desk->m_vip->m_coins[i];

				if (m_accum_coins[i] <= 0)
				{
					m_user_status[i] = 2;
				}
				else
				{
					m_user_status[i] = 1;
				}
			}
			m_desk->SetHandlerPlayerStatus(m_user_status, m_player_count);
			//���ֻ��һ������������ֱ�ӽ�ɢ����
			if (calc_vaild_player_count() <= 1)
			{
				LLOG_ERROR("NiuNiu_NewGameHandler::finish()... Player count <= 1, Game end... deskId=[%d], startRound=[%d/%d],  playerCount=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

				m_desk->HanderGameOver(2);
				return;
			}
		}
		//ţţ��ׯ��ȷ���¾�ׯ��
		if (m_mainMode == MAIN_MODE_NIUNIU_SHANG_ZHUANG)
		{
			//���Ŷ�����Ǹ�λ
			m_carToon = 0;

			Lint t_maxOxPos = m_zhuangPos;
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_player_oxnum[i] != 10 || i == m_zhuangPos || m_user_status[i] != 1)
				{
					continue;
				}

				if (m_player_oxnum[t_maxOxPos] != 10)
				{
					t_maxOxPos = i;
				}
				else if (m_gamelogic.CompareCard(tmp_player_hand_card[i], tmp_player_hand_card[t_maxOxPos], NIUNIU_HAND_CARD_MAX, m_player_oxnum[i], m_player_oxnum[t_maxOxPos]))
				{
					t_maxOxPos = i;
				}
			}
			m_zhuangPos = t_maxOxPos;

			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0 && m_user_status[m_zhuangPos] != 1)
			{
				Lint tmp_rand_zhuang_pos = 0;
				//����Ƿ�����Ч���������λ���Ƿ�����Чλ��
				do
				{
					tmp_rand_zhuang_pos = rand() % m_player_count;
					if (m_user_status[tmp_rand_zhuang_pos] == 1)
					{
						break;
					}
				} while (true);

				m_zhuangPos = tmp_rand_zhuang_pos;
				m_carToon = 1;
			}
		}

		
		//if (m_desk) m_desk->setDynamicToPlay();
		LLOG_ERROR("desk_id=[%d], desk_status=[%d], round_finish game over", m_desk ? m_desk->GetDeskId() : 0, m_play_status);
		//������Ϸ
		if (m_desk) m_desk->HanderGameOver(1);

	}

	///////////////////////////////////////////////////////////////////////////////

	/*********************************************************
	* ��������    on_event_user_select_zhuang()
	* ������      ����ѡׯ�淨�£��ͻ��˷����������ׯ����
	* ������
	*  @ pos ���λ��
	*  @ zhuang_score �����ׯ����
	* ���أ�      (void)
	**********************************************************/
	bool on_event_user_select_zhuang(Lint pos, Lint zhuang_score)
	{
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_select_zhuang() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d], qiangScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos], m_play_qiang_zhuang[pos]);
			return false;
		}
		if (m_play_qiang_zhuang[pos] != -1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_select_zhuang() Error!!! Player has opt qiang zhuang... deskId=[%d], pos=[%d], qiangScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_qiang_zhuang[pos]);
			return false;
		}
		LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_select_zhuang() Run... Player Qiang Zhuang... deskId=[%d], pos=[%d], qiangScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, zhuang_score);

		//��¼�������ׯ����; -1����Чλ�ã�0������ׯ��>0����ׯ����
		m_play_qiang_zhuang[pos] = zhuang_score;

		// ֪ͨ��ׯ��� 
		NiuNiuS2CQiangZhuangNotify send;
		send.m_pos = pos;
		send.m_qiangZhuangScore = zhuang_score;
		notify_desk(send);

		//��¼��ׯ����
		if (m_play_qiang_zhuang[pos] > 0)
		{
			++m_qiangCount[pos];

			//��¼������ׯ�������
			++m_qiang_zhuang_count;
		}

		//�㲥��ս���
		notify_lookon(send);

		if (is_select_zhuang_over())
		{
			LLOG_DEBUG("NiuNiu_NewGameHandler::on_event_user_select_zhuang() Run... All Player Has Qiang ZhuangOver, Zhen Go to Next Status Add Score... deskId=[%d]",
				m_desk ? m_desk->GetDeskId() : 0);
			
			Lint t_allPlayerNoQiang = 0;				//�Ƿ�������Ҷ�û����ׯ   1��������Ҷ�û����ׯ    0���������ׯ
			Lint tmp_max = m_play_qiang_zhuang[0];     //������ʱ���ֵ
			Lint maxNum = 1;                           //�����ׯ����������
			Lint tmp_score_max[NIUNIU_PLAY_USER_COUNT];
			ZeroMemory(tmp_score_max, sizeof(tmp_score_max));
			tmp_score_max[0] = 0;

			for (Lint i = 1; i < m_player_count; i++)
			{
				Lint ret = m_play_qiang_zhuang[i] - tmp_max;
				if (ret > 0)
				{
					tmp_max = m_play_qiang_zhuang[i];
					if (maxNum >= 1)
					{
						ZeroMemory(tmp_score_max, sizeof(tmp_score_max));
						tmp_score_max[0] = i;
						maxNum = 1;
					}
				}
				else if (ret < 0)
				{
					continue;
				}
				else
				{
					tmp_score_max[maxNum++] = i;
				}
			}

			// if ��߷��ж����ң��������ȡһ��Ϊׯ
			if (maxNum > 1)
			{
				m_zhuangPos = tmp_score_max[rand() % maxNum];
			}
			else
			{
				m_zhuangPos = tmp_score_max[0];
			}

			//���������ׯģʽ���û�ȫ���������
			if (m_play_qiang_zhuang[m_zhuangPos] == 0)
			{
				t_allPlayerNoQiang = 1;
				m_play_qiang_zhuang[m_zhuangPos] = 1;
			}

			//��¼ѡׯ����
			m_play_add_score[m_zhuangPos] = m_play_qiang_zhuang[m_zhuangPos];

			//��ע��ׯ��ȷ���ˣ�ׯ��û����ע��Ȩ��(**Ren 2018-5-19)
			if (m_tuiZhuTimes)
			{
				m_canTuiZhu[m_zhuangPos] = false;
				memset(m_player_tui_score[m_zhuangPos], 0, sizeof(Lint) * 4);
			}

			//��ע���ƣ��ֶ���ׯû��������Ҳ�������С�֣�û������ׯ����Ҳ�����ע
			if (m_isConstAddScore)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (i == m_zhuangPos) continue;

					//������ׯ
					if (m_mainMode == MAIN_MODE_ZI_YOU_QIANG_ZHUANG || m_mainMode == MAIN_MODE_NIUNIU_SHANG_ZHUANG)
					{
						if (m_user_status[i] == 1 && m_play_qiang_zhuang[i] > 0 )
						{
							m_canXiaMinZhu[i] = 1;
						}
						if (m_user_status[i] == 1 && m_tuiZhuTimes && m_play_qiang_zhuang[i] <= 0)
						{
							m_canTuiZhu[i] = false;
						}
					}
					//������ׯ���������ơ�ʮ�����ơ�����ţţ
					else
					{
						if (m_user_status[i] == 1 && m_play_qiang_zhuang[i] > 0 && m_play_qiang_zhuang[i] == m_play_qiang_zhuang[m_zhuangPos])
						{
							m_canXiaMinZhu[i] = 1;
						}
						if (m_user_status[i] == 1 && m_tuiZhuTimes && (m_play_qiang_zhuang[i] <= 0 || m_play_qiang_zhuang[i] < m_play_qiang_zhuang[m_zhuangPos]))
						{
							m_canTuiZhu[i] = false;
						}
					}
				}
			}
			//��ע����:�ֶ�������ׯû����ׯ�ҵ������עʱ����ѡ��С�֣���ֻ���ע������ׯ����Ҳ�����ע
			else if (m_isConstTuiZhu && m_tuiZhuTimes)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (i == m_zhuangPos) continue;

					//������ׯ
					if (m_mainMode == MAIN_MODE_ZI_YOU_QIANG_ZHUANG || m_mainMode == MAIN_MODE_NIUNIU_SHANG_ZHUANG)
					{
						if (m_user_status[i] != 1 || m_play_qiang_zhuang[i] <= 0)
						{
							m_canTuiZhu[i] = false;
						}
					}
					//������ׯ���������ơ�ʮ�����ơ�����ţţ
					else
					{
						if (m_user_status[i] == 1 && (m_play_qiang_zhuang[i] <= 0 || m_play_qiang_zhuang[i] < m_play_qiang_zhuang[m_zhuangPos]))
						{
							m_canTuiZhu[i] = false;
						}
					}
				}
			}

			//��ע�������ֶ�������ׯ�ұ����ߵ�û������ׯ�ҵ������עʱ������ʹ�üӱ����ܡ�
			if (m_isAddScoreDouble)
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (i == m_zhuangPos) continue;
					if (m_user_status[i] == 1 && m_play_qiang_zhuang[i] > 0 && m_play_qiang_zhuang[i] == m_play_qiang_zhuang[m_zhuangPos])
					{
						m_canAddScoreDouble[i] = 1;
					}
				}
			}

			//ȷ��ׯ�Һ�㲥ѡׯ���
			notify_zhuang(m_zhuangPos, t_allPlayerNoQiang, m_play_qiang_zhuang[m_zhuangPos]);

			//ѡׯ����������Ϸ״̬����Ϊ��ע������
			if (m_isMaiMa && calc_vaild_player_count() > 2)
			{
				set_play_status(GAME_PLAY_MAI_MA);
				m_add_score_delay_time = m_time_add_score + calc_delay_time(GAME_PLAY_MAI_MA);
			}
			else
			{
				set_play_status(GAME_PLAY_ADD_SCORE);
				m_add_score_delay_time =  m_time_add_score + calc_delay_time(GAME_PLAY_ADD_SCORE);
			}

			//��Ϸ��ʼ����ע����ʱ
			m_play_status_time.Now();
			m_tick_flag = true;

			return true;
		}
		return false;
	}

	/***************************************************
	* ��������    on_event_user_mai_ma()
	* ������      ��������������
	* ������
	*  @ pos ����������ϵ�λ��
	*  @ maiPos ��������λ��
	*  @ maiScore ���
	* ���أ�      (bool)
	****************************************************/
	bool on_event_user_mai_ma(Lint pos, Lint maiPos, Lint maiScore)
	{
		//У������
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_mai_ma() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d], maiScore=[%d], maiPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos], maiScore, maiPos);
			return false;
		}
		//���ׯ������ �� ��ׯ�ҵ��룬������
		if (pos == m_zhuangPos || maiPos == m_zhuangPos)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_mai_ma() error!!! mai ma player pos invalid!!! deskId=[%d], pos=[%d], maiScore=[%d], maiPos=[%d], zhuangPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, maiScore, maiPos, m_zhuangPos);
			return false;
		}

		//����ķ�������Ϊ����
		if (maiScore < 0)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_mai_ma() error!!! mai ma score invalid!!! deskId=[%d], pos=[%d], maiScore=[%d], maiPos=[%d], zhuangPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, maiScore, maiPos, m_zhuangPos);
			return false;
		}

		//�����ظ�����
		if (m_player_maima[pos][0] != -1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_mai_ma() Error!!!  Player Has Mai Ma!!! deskId=[%d], pos=[%d], maiPos=[%d], maiScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_player_maima[pos][1], m_player_maima[pos][0]);
			return false;
		}

		LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_mai_ma() Run... Player Mai Ma... deskId=[%d], pos=[%d], maiScore=[%d], maiPos=[%d], zhuangPos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, maiScore, maiPos, m_zhuangPos);

		m_player_maima[pos][0] = maiScore;
		m_player_maima[pos][1] = maiPos;

		NiuNiuS2CMaiMaBC maiMabc;
		maiMabc.m_pos = pos;
		maiMabc.m_maiPos = maiPos;
		maiMabc.m_maiScore = maiScore;
		memcpy(maiMabc.m_maiMa, m_player_maima, sizeof(m_player_maima));
		notify_desk(maiMabc);
		notify_lookon(maiMabc);

		if (is_mai_ma_over())
		{
			//�����������ע����ʱ�������������עʱ�����棩
			//m_play_status_time.Now();
			//m_tick_flag = true;

			//֪ͨ�ͻ��������������ʼ��ע
			set_play_status(GAME_PLAY_ADD_SCORE);

			NiuNiuS2CMaiMaBC maiMabc;
			memcpy(maiMabc.m_maiMa, m_player_maima, sizeof(m_player_maima));
			notify_desk(maiMabc);
			notify_lookon(maiMabc);
		}
		return true;
	}

	/***************************************************
	* ��������    on_event_user_add_score()
	* ������      ���������ע
	* ������
	*  @ pos ����������ϵ�λ��
	*  @ score �����ע����
	*  @ addDouble �Ƿ�ѡ��ӱ�
	*  @ tuiZhu �Ƿ�Ϊ��ע��Ϣ   0:������ע   1����ע��ע
	* ���أ�      (bool)
	****************************************************/
	bool on_event_user_add_score(Lint pos, Lint score, Lint addDouble, Lint tuiZhu)
	{
		//У������
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_add_score() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos]);
			return false;
		}

		//���Ǽӱ�ѡ������ע����Ϊ0
		if (0 == score && 0 == addDouble)
		{
			LLOG_ERROR("NiuNiu_NewGameLogic::on_event_user_add_score() error!!! Player add score invalid!!! desk_id=[%d], pos[%d], addScore=[%d], zhuangPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, score, m_zhuangPos);
			return false;
		}
		//ׯ�Ҳ�����ע
		if (pos == m_zhuangPos)
		{
			LLOG_ERROR("NiuNiu_NewGameLogic::on_event_user_add_score() error!!!, zhuang can not add score!!! deskId=[%d], pos=[%d], zhuang=[%d] addScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_zhuangPos, score);
			return false;
		}
		//����ظ���ע
		if (m_play_add_score[pos] > 0)
		{
			LLOG_ERROR("NiuNiu_NewGameLogic::on_event_user_add_score() error!!!, this player has add score!!! deskId=[%d], pos=[%d], hasAddScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_add_score[pos]);
			return false;
		}

		if (m_canTuiZhu[pos] == 0 && tuiZhu == 1)
		{
			LLOG_ERROR("NiuNiu_NewGameLogic::on_event_user_add_score() error!!!, this player can not tui zhu!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		LLOG_ERROR("NiuNiu_NewGameLogic::on_event_user_add_score() Run... desk_id=[%d], pos[%d], zhuangPos=[%d], addScore=[%d], addDouble=[%d], tuiZhu=[%d], canTuiZhu=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, m_zhuangPos, score, addDouble, tuiZhu, m_canTuiZhu[pos]);

		//��ע
		if (addDouble == 0 && score > 0)
		{
			m_play_add_score[pos] = score;
			
			//��¼��ע����
			if (tuiZhu == 1 && m_canTuiZhu[pos] == 1)
			{
				++m_tuiCount[pos];
			}
			//���ֲ���ע�¾ֿ���������ע
			else if (tuiZhu == 0 && m_canTuiZhu[pos] == 1)
			{
				m_canTuiZhu[pos] = 0;
			}
		}
		//�ӱ�
		else
		{
			m_add_score_double_status[pos] = 1;
		}

		//�����ע��ɺ�㲥��ע����
		NiuNiuS2CAddScore score_notify;
		score_notify.m_pos = pos;
		score_notify.m_score = score;
		memcpy(score_notify.m_addScoreDoubleStatus, m_add_score_double_status, sizeof(score_notify.m_addScoreDoubleStatus));
		notify_desk(score_notify);
		notify_lookon(score_notify);

		//�㲥��ս��ң������ע
		notify_lookon(score_notify);

		if (is_add_score_over())
		{

			//������ע��ɣ��޸���Ϸ״̬Ϊ����״̬
			set_play_status(GAME_PLAY_SEND_CARD);

			//��ӡ��־��
			LLOG_ERROR("desk_id=[%d], deskStatus=[%d],clent add score over", m_desk ? m_desk->GetDeskId() : 0, m_play_status);

			//����ǹ���ţţ����ȷ���깫�Ƶ����λ��
			/*
			if (MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
			{
				Lint tmp_rand_zhuang_pos = 0;
				//����Ƿ�����Ч���������λ���Ƿ�����Чλ��
				do
				{
					tmp_rand_zhuang_pos = rand() % m_player_count;
					if (m_user_status[tmp_rand_zhuang_pos])
					{
						break;
					}
				} while (true);

				m_open_gong_pos = tmp_rand_zhuang_pos;
			}*/
			if (MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
			{
				Lint t_addMaxScore = 0;
				Lint t_maxScoreCount = 0;
				Lint t_playAddScore[NIUNIU_PLAY_USER_COUNT];
				memcpy(t_playAddScore, m_play_add_score, sizeof(t_playAddScore));
				Lint t_maxScoreIndex[NIUNIU_PLAY_USER_COUNT];
				memset(t_maxScoreIndex, 0, sizeof(t_maxScoreIndex));
				Lint t_maxXianScore = 0;
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (i == m_zhuangPos) continue;
					if (t_playAddScore[i] > t_maxXianScore)
					{
						t_maxXianScore = t_playAddScore[i];
					}
				}
				if (t_playAddScore[m_zhuangPos] > t_maxXianScore)
				{
					t_playAddScore[m_zhuangPos] = 0;
				}

				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_user_status[i] != 1) continue;

					if (t_playAddScore[i] > t_addMaxScore)
					{
						t_addMaxScore = t_playAddScore[i];
						memset(t_maxScoreIndex, 0, sizeof(t_maxScoreIndex));
						t_maxScoreIndex[0] = i;
						t_maxScoreCount = 1;
					}
					else if (t_playAddScore[i] < t_addMaxScore)
					{
						continue;
					}
					else
					{
						t_maxScoreIndex[++t_maxScoreCount] = i;
					}
				}

				if (t_maxScoreCount == 1)
				{
					m_open_gong_pos = t_maxScoreIndex[0];
				}
				else
				{
					m_open_gong_pos = m_zhuangPos;
				}
			}

			//��������ע��ɣ���ÿ����ҷ���
			for (Lint i = 0; i < m_player_count; i++)
			{
				if (m_user_status[i] != 1)continue;
				send_user_card(i, false);
			}

			//�����ע��ɣ��������ƽ׶Σ��������ơ�����ʱ�䣩
			m_play_status_time.Now();
			m_tick_flag = true;
		}
		return true;
	}


	/***************************************************
	* ��������    on_event_user_open_card()
	* ������      ��ҵ�����ư�ť
	* ������
	*  @ pos ����������ϵ�λ��
	* ���أ�      (bool)
	****************************************************/
	bool on_event_user_open_card(Lint pos)
	{
		if (GAME_PLAY_SEND_CARD != m_play_status)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_open_card() Error..., status not GAME_PLAY_SEND_CARD, deskId=[%d], pos[%d], palyStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_play_status);

			return false;
		}
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_open_card() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos]);
			return false;
		}

		LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_open_card() Run... Player Open Card... deskId=[%d], pos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos);

		//��Ǹ��û�Ϊ�ѿ���
		m_isOpenCard[pos] = true;

		if (m_mainMode == MAIN_MODE_GONG_PAI_NIU_NIU)
		{
			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (m_user_status[i] == 1 && pos != i)
				{
					m_isOpenCard[i] = true;
				}
			}
		}

		//�㲥���û����ƽ��
		send_user_card(pos, true);

		//������������
		//m_player_oxnum[pos] = m_gamelogic.GetCardType(m_player_hand_card[pos], NIUNIU_HAND_CARD_MAX);

		if (is_open_card_over() || (m_mainMode == MAIN_MODE_GONG_PAI_NIU_NIU && m_isOpenCard[pos]))
		{
			set_play_status(GAME_PLAY_END);
			//�㲥ÿ��������
			//����
			finish_round();
		}
		return true;
	}

	//������Ҵ깲��
	bool on_event_user_cuo_gong_card(Lint pos, Lstring cuoPai)
	{
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_cuo_gong_card() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos]);
			return false;
		}

		if (pos != m_open_gong_pos)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_cuo_gong_card() Error!!! This palyer is not cuo gong pai pos!!! deskId=[%d], pos=[%d], cuoGongPaiPos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_open_gong_pos);
			return false;
		}
		m_cuo_pai = cuoPai;
		NiuNiuS2CCuoGongPaiBC send;
		send.m_cuoPai = cuoPai;
		notify_desk(send);
		notify_lookon(send);

		return true;
	}

	//��������й�
	bool on_event_user_tuo_guan(Lint pos, Lint zhuangScore, Lint addScore, Lint tuiScore)
	{
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_tuo_guan() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos]);
			return false;
		}

		if (m_tuo_guan_info[pos].m_tuoGuanStatus == 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_tuo_guan() Error!!! This palyer has in tuo guan status!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		if ((MAIN_MODE_NIUNIU_SHANG_ZHUANG == m_mainMode || MAIN_MODE_ZI_YOU_QIANG_ZHUANG == m_mainMode || MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode) &&
			(addScore > 2 || addScore < 1 || zhuangScore != 0 || tuiScore != 0))
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_tuo_guan() Error!!! Tuo Guan operators is invalid!!! deskId=[%d], pos=[%d], mainMode=[%d], zhuangScore=[%d], addScore=[%d], tuiScore=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_mainMode, zhuangScore, addScore, tuiScore);
			return false;
		}


		LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_tuo_guan() Run... deskId=[%d], pos=[%d], tuoZhuangScore=[%d], tuoAddScore=[%d], tuoTuiScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, zhuangScore, addScore, tuiScore);

		m_tuo_guan_info[pos].m_pos = pos;
		m_tuo_guan_info[pos].m_tuoGuanStatus = 1;
		m_tuo_guan_info[pos].m_zhuangScore = zhuangScore;
		m_tuo_guan_info[pos].m_addScore = addScore;
		m_tuo_guan_info[pos].m_tuiScore = tuiScore;

		return true;
	}

	//����ȡ���й�
	bool on_event_user_cancel_tuo_guan(Lint pos)
	{
		if (m_user_status[pos] != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_cancel_tuo_guan() Error!!! Player status can not opt... deskId=[%d], pos=[%d], userStates=[&d]",
				m_desk ? m_desk->GetDeskId() : 0, pos, m_user_status[pos]);
			return false;
		}

		if (m_tuo_guan_info[pos].m_tuoGuanStatus != 1)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_cancel_tuo_guan() Error!!! This palyer is not in tuo guan status!!! deskId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}

		LLOG_ERROR("NiuNiu_NewGameHandler::on_event_user_cancel_tuo_guan() Run... deskId=[%d], pos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos);

		m_tuo_guan_info[pos].m_pos = NIUNIU_HAND_CARD_MAX;
		m_tuo_guan_info[pos].m_tuoGuanStatus = 0;
		m_tuo_guan_info[pos].m_zhuangScore = 0;
		m_tuo_guan_info[pos].m_addScore = 0;
		m_tuo_guan_info[pos].m_tuiScore = 0;

		return true;
	}

	//����ʼ��ʱ
	bool on_event_start_time(Lint pos)
	{
		//������ׯ��ʼ��ʱ����ׯ�׶�
		m_play_status_time.Now();
		m_tick_flag = true;
		return true;
	}

	/* ţţ��ׯ */
	void niuniu_shang_zhuang()
	{
		//��һ�����ׯ��
		if (m_round_offset == 0)
		{
			Lint tmp_rand_zhuang_pos = 0;
			//����Ƿ�����Ч���������λ���Ƿ�����Чλ��
			do
			{
				tmp_rand_zhuang_pos = rand() % m_player_count;
				if (m_user_status[tmp_rand_zhuang_pos] == 1)
				{
					break;
				}
			} while (true);

			m_zhuangPos = tmp_rand_zhuang_pos;
		}
		//�Ժ�ÿ��ׯ�Ҷ���ֱ��ȷ����
		else
		{
			//��ע��ׯ��ȷ���ˣ�ׯ��û����ע��Ȩ��(**Ren 2018-5-19)
			if (m_tuiZhuTimes)
			{
				m_canTuiZhu[m_zhuangPos] = false;
				memset(m_player_tui_score[m_zhuangPos], 0, sizeof(Lint) * 4);
			}
		}

		//�طż�¼��������
		m_video.setCommonInfo(gWork.GetCurTime().Secs(), m_round_offset, m_player_count, m_desk->m_vip->m_posUserId, m_zhuangPos);
		
		m_play_qiang_zhuang[m_zhuangPos] = 1;

		NiuNiuS2CStartGame send_start;
		send_start.m_remainTime = m_time_add_score;
		send_start.m_zhuangPos = m_zhuangPos;
		send_start.m_scoreTimes = 1;  //����ׯģʽ��Ĭ�ϱ���Ϊһ��
		send_start.m_maiMa = (m_isMaiMa && calc_vaild_player_count() > 2) ? 1 : 0;
		send_start.m_carToon = m_carToon;
		//��ע����ҿ���ע״̬
		memcpy(send_start.m_isCanTui, m_canTuiZhu, sizeof(send_start.m_isCanTui));
		memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
		memcpy(send_start.m_playerAddScoreDouble, m_canAddScoreDouble, sizeof(send_start.m_playerAddScoreDouble));
		memcpy(send_start.m_playerStatus, m_user_status, sizeof(send_start.m_playerStatus));
		//memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
		m_play_add_score[m_zhuangPos] = 1;
		notify_desk(send_start);

		//�㲥��ս���
		notify_lookon(send_start);

		//��¼��ׯ����
		++m_zhuangCount[m_zhuangPos];

		//ѡׯ������������Ϸ״̬Ϊ�������ע״̬
		if (m_isMaiMa && calc_vaild_player_count() > 2)
		{
			set_play_status(GAME_PLAY_MAI_MA);
			m_add_score_delay_time = m_time_add_score + calc_delay_time(GAME_PLAY_MAI_MA);
		}
		else
		{
			set_play_status(GAME_PLAY_ADD_SCORE);
			m_add_score_delay_time = m_time_add_score + calc_delay_time(GAME_PLAY_ADD_SCORE);
		}
		//���Ŷ�����Ǹ�λ
		//m_carToon = 0;
		//ţţ��ׯ����Ҫ��ׯ��ֱ�ӵ���ע
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* ������ׯ */
	void free_zhuang_pos()
	{
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_user_status[i] != 1)
			{
				continue;
			}
			//62072:֪ͨ�ͻ�����ׯ
			NiuNiuS2CSelectZhuang qiang_zhuang;
			qiang_zhuang.m_remainTime = m_time_qiang_zhuang;
			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				if (m_accum_coins[i] < m_desk->m_qiangZhuangMinCoins)
				{
					//on_event_user_select_zhuang(i, 0);
					m_noQiangZhuang[i] = 0;
					qiang_zhuang.m_noQiangZhuang = 0;
				}
			}
			qiang_zhuang.m_pos = i;
			qiang_zhuang.m_qingZhuang = 0;  //����Ϊ��ׯģʽ
			qiang_zhuang.m_qiangTimes = 1;  //ֻ�����벻��
			memcpy(qiang_zhuang.m_isCanTui, m_canTuiZhu, sizeof(qiang_zhuang.m_isCanTui));
			memcpy(qiang_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
			notify_user(qiang_zhuang, i);

			if (m_noQiangZhuang[i] == 0)
			{
				on_event_user_select_zhuang(i, 0);
			}
		}

		//�㲥��ս���
		NiuNiuS2CSelectZhuang gz_zhuang;
		gz_zhuang.m_remainTime = m_time_qiang_zhuang;
		gz_zhuang.m_pos = m_player_count;
		gz_zhuang.m_qingZhuang = 1;
		gz_zhuang.m_qiangTimes = 1;  //ֻ�����벻��
		gz_zhuang.m_noQiangZhuang = 0;
		memcpy(gz_zhuang.m_isCanTui, m_canTuiZhu, sizeof(gz_zhuang.m_isCanTui));
		memcpy(gz_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
		notify_seat_lookon(gz_zhuang);
		notify_lookon(gz_zhuang);

		//ѡׯ������������Ϸ״̬Ϊ�������ע״̬
		/*if (m_isMaiMa)
		{
		set_play_status(GAME_PLAY_MAI_MA);
		}
		else
		{
		set_play_status(GAME_PLAY_ADD_SCORE);
		}*/

		//������ׯ��ʼ��ʱ����ׯ�׶�
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* ������ׯ */
	void qiang_zhuang_pos()
	{
		LLOG_DEBUG("NiuNiu_NewGameHandler::qiang_zhuang_pos() Run... This mode is QiangZhuang... deskId=[%d]", m_desk ? 0 : m_desk->GetDeskId());
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_user_status[i] != 1)
			{
				continue;
			}
			
			//62072:֪ͨ�ͻ�����ׯ
			NiuNiuS2CSelectZhuang qiang_zhuang;
			qiang_zhuang.m_remainTime = m_time_qiang_zhuang;
			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				if (m_accum_coins[i] < m_desk->m_qiangZhuangMinCoins)
				{
					//on_event_user_select_zhuang(i, 0);
					m_noQiangZhuang[i] = 0;
					qiang_zhuang.m_noQiangZhuang = 0;
				}
			}
			qiang_zhuang.m_pos = i;         //���λ��
			qiang_zhuang.m_qingZhuang = 1;  //����Ϊ��ׯģʽ
			memcpy(qiang_zhuang.m_isCanTui, m_canTuiZhu, sizeof(qiang_zhuang.m_isCanTui));
			//��ÿ����ҵ�����д�뿴��ѡׯ����Ϣ��,ÿ�������ֻ��4����
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
			{
				qiang_zhuang.m_playerHandCard[j] = m_player_hand_card[i][j];
			}
			memcpy(qiang_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
			notify_user(qiang_zhuang, i);

			if (m_noQiangZhuang[i] == 0)
			{
				on_event_user_select_zhuang(i, 0);
			}
		}

		//�㲥��ս���
		NiuNiuS2CSelectZhuang gz_zhuang;
		gz_zhuang.m_remainTime = m_time_qiang_zhuang;
		gz_zhuang.m_pos = m_player_count;
		gz_zhuang.m_qingZhuang = 1;
		gz_zhuang.m_noQiangZhuang = 0;
		memcpy(gz_zhuang.m_isCanTui, m_canTuiZhu, sizeof(gz_zhuang.m_isCanTui));
		memcpy(gz_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
		notify_seat_lookon(gz_zhuang);
		notify_lookon(gz_zhuang);

		LLOG_DEBUG("NiuNiu_NewGameHandler::qiang_zhuang_pos() Run... Has Notify Client Start Qiang Zhuang... deskId=[%d]", m_desk ? 0 : m_desk->GetDeskId());

		//������ׯ��ʼ��ʱ����ׯ�׶�
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* ѡׯ */
	void select_zhuang()
	{
		switch (m_mainMode)
		{

			//ţţ��ׯ
		case MAIN_MODE_NIUNIU_SHANG_ZHUANG:
		{
			niuniu_shang_zhuang();
			break;
		}

		//������ׯ
		case MAIN_MODE_ZI_YOU_QIANG_ZHUANG:
		{
			free_zhuang_pos();
			break;
		}

		//������ׯ
		case MAIN_MODE_MING_PAI_QIANG_ZHUANG:
		{
			//֪ͨ�����ע��ׯ
			qiang_zhuang_pos();
			break;
		}

		//��������
		case MAIN_MODE_BA_REN_MING_PAI:
		{
			//֪ͨ�����ע��ׯ
			qiang_zhuang_pos();
			break;
		}

		//����ţţ
		case MAIN_MODE_GONG_PAI_NIU_NIU:
		{
			//֪ͨ�����ע��ׯ
			qiang_zhuang_pos();

			//����
			Lint tmp_gong_pos = 0;
			//����Ƿ�����Ч���������λ���Ƿ�����Чλ��
			do
			{
				tmp_gong_pos = rand() % m_player_count;
				if (m_user_status[tmp_gong_pos] == 1)
				{
					break;
				}
			} while (true);

			m_gong_card = m_player_hand_card[tmp_gong_pos][4];

			for (Lint i = 0; i < m_player_count; ++i)
			{
				if (tmp_gong_pos == i)
				{
					continue;
				}
				m_player_hand_card[i][4] = m_player_hand_card[tmp_gong_pos][4];
			}
			break;
		}
		} //switch end
	}

	/********************************************************************************************
	* ��������      start_game()
	* ������        ÿһ����Ϸ��ʼ�ľ������
	* ���أ�        (void)
	*********************************************************************************************/
	void start_game()
	{
		LLOG_DEBUG("NiuNiu_NewGameHandler::start_game() Run... Game Start... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);

		//�������˿���
		BYTE cbRandCard[NIUNIU_MAX_POKER_COUNT];

		//�����˿ˣ��������
		if (m_isLaiZi)
		{
			//�����˿�
			m_gamelogic.RandCardList(cbRandCard, NIUNIU_LAIZI_POKER_COUNT);
			debugSendCards_NiuNiu_New(cbRandCard, m_desk, "card_niuniu_new.ini", NIUNIU_PLAY_USER_COUNT, NIUNIU_HAND_CARD_MAX, 54);
		}
		//�����˿ˣ��������
		else
		{
			//�����˿�
			m_gamelogic.RandCardList(cbRandCard, NIUNIU_NORMAL_POKER_COUNT);
			debugSendCards_NiuNiu_New(cbRandCard, m_desk, "card_niuniu_new.ini", NIUNIU_PLAY_USER_COUNT, NIUNIU_HAND_CARD_MAX, 52);
		}

		//��ÿ����ҷ���
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			memcpy(&m_player_hand_card[i], &cbRandCard[i*NIUNIU_HAND_CARD_MAX], sizeof(BYTE)*NIUNIU_HAND_CARD_MAX);
		}

		//ÿ��������ҿ�����ע�ķ���
		if (m_tuiZhuTimes)
		{
			calc_tui_zhu_score();
		}

		//������Ϸ״̬Ϊѡׯ״̬
		set_play_status(GAME_PLAY_SELECT_ZHUANG);

		//ѡׯ����
		select_zhuang();

	}

	/*********************************************************************************************
	* ��������     start_round()
	* ������       ��Ϸÿһ�ֵ��ܿ�ʼ����
	* ������
	*  @ *player_status ���״̬�б���ʾ��λ�����Ƿ�����ң�0����ʼ��1���þֲμӣ�2���¼��� OR �þֲ�������Ϸ(����)
	* ���أ�       (void)
	**********************************************************************************************/
	void start_round(Lint* player_status)
	{
		LLOG_DEBUG("NiuNiu_NewGameHandler::start_round() Run... Game Start Round........................... deskId=[%d]",
			m_desk ? m_desk->GetDeskId() : 0);

		clear_round();
		notify_desk_match_state();
		set_desk_state(DESK_PLAY);
		memcpy(m_user_status, player_status, sizeof(m_user_status));

		if (m_desk && m_desk->m_vip && m_desk->m_clubInfo.m_clubId != 0)
		{
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);

			if(m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				//���ֲ�������ֵ�����ˣ�����Ϊ������ս״̬
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (!m_desk->m_user[i]) continue;

					//ÿ�ֿ�ʼ��������ƾ���������ֵ����
					m_accum_coins[i] = m_desk->m_vip->m_coins[i];

					LLOG_ERROR("NiuNiu_NewGameHandler::start_round() Run... deskId=[%d], player[%d] roundStartCoins=[%d]",
						m_desk ? m_desk->GetDeskId() : 0, i, m_accum_coins[i]);

					if (m_accum_coins[i] <= 0)
					{
						m_user_status[i] = 2;
					}
					else
					{
						m_user_status[i] = 1;
					}

					LMsgS2CUpdateCoins updateCoins;
					updateCoins.m_pos = i;
					updateCoins.m_userId = m_desk->m_user[i]->m_userData.m_id;
					updateCoins.m_coins = m_accum_coins[i];
					notify_desk(updateCoins);
					notify_lookon(updateCoins);
					
				}

				m_desk->SetHandlerPlayerStatus(m_user_status, m_player_count);

				//���ֻ��һ������������ֱ�ӽ�ɢ����
				if (calc_vaild_player_count() <= 1)
				{
					LLOG_ERROR("NiuNiu_NewGameHandler::start_round()... Player count <= 1, Game end... deskId=[%d], startRound=[%d/%d],  playerCount=[%d]",
						m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

					finish_round(2);
					return;
				}
			}
		}

		//���㱾����ʵ�ʲ����������
		m_real_player_count = calc_vaild_player_count();

		LLOG_ERROR("NiuNiu_NewGameHandler::start_round()... Game Round Info... deskId=[%d], startRound=[%d/%d],  playerCount=[%d], realPlayerCount=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count, m_real_player_count);
		
		//��Ϸ��ʼ���
		start_game();
	}
};


struct NiuNiu_NewGameHandler : NiuNiu_NewGameCore
{

	/* ���캯�� */
	NiuNiu_NewGameHandler()
	{
		LLOG_DEBUG("NiuNiu_NewGameHandler Init...");
	}

	/* ����ҷ���Ϣ */
	void notify_user(LMsg &msg, int pos) override
	{
		if (NULL == m_desk) return;
		if (pos < 0 || pos >= m_player_count) return;
		User *u = m_desk->m_user[pos];
		if (NULL == u) return;
		u->Send(msg);
	}

	/* �㲥��������������� */
	void notify_desk(LMsg &msg) override
	{
		if (NULL == m_desk) return;
		m_desk->BoadCast(msg);
	}

	/* �㲥����������������Ϸ�����*/
	void notify_seat_playing(LMsg& msg)
	{
		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 1)
			{
				notify_user(msg, i);
			}
		}
	}

	/* �㲥������������ս����� */
	void notify_seat_lookon(LMsg& msg)
	{
		for(Lint i = 0; i < m_player_count; ++i)
		{
			if (m_user_status[i] == 2 || m_desk->m_playStatus[i] == 2)
			{
				notify_user(msg, i);
			}
		}
	}

	/* �㲥����ս��� */
	void notify_lookon(LMsg &msg) override
	{
		if (NULL == m_desk) return;
		m_desk->MHBoadCastDeskLookonUser(msg);
	}

	/* */
	bool startup(Desk *desk)
	{
		if (NULL == desk) return false;
		clear_match(desk->m_desk_user_count);
		m_desk = desk;
		return true;
	}
	/* */
	void shutdown(void) {
		clear_match(0);
	}


	/* �����淨Сѡ���Desk::SetVip()���� */
	void SetPlayType(std::vector<Lint>& l_playtype)
	{
		// �� Desk �� SetVip ���ã���ʱ�ܵõ� m_vip->m_maxCircle
		int round_limit = m_desk && m_desk->m_vip ? m_desk->m_vip->m_maxCircle : 0;
		setup_match(l_playtype, round_limit);

		// Сѡ��[0]�����淨��ǩ��1ţţ��ׯ��2������ׯ��3������ׯ��4��ʮ�����ơ�5����ţţ
		m_mainMode = m_playtype.NiuNiu_NewGetMainMode();

		// Сѡ��[1]������6�� 8�� 10��
		m_playerNum = m_playtype.NiuNiu_NewGetPlayerNum();

		// Сѡ��[2]����ׯ���ʣ�1-1����2-2����3-3����4-4��
		m_qiangZhuangTimes = m_playtype.NiuNiu_NewQiangZhuangTimes();

		// Сѡ��[3]����ע���ʣ�1-1/2  2-2/4  3-3/6  4-4/8  5-5/10
		m_addScoreTimes = m_playtype.NiuNiu_NewAddScoreTimes();

		// Сѡ��[4]���Զ���ʼ��1-��2����ʱ��ʼ��4-��4�˿���5-��5�˿���6-��6�˿���4-������ע
		m_autoStart = m_playtype.NiuNiu_NewAutoStartGame();

		// Сѡ��[5]����ע������ 0������ע��5��5����ע��10��10����ע��15:15����ע��20:20����ע
		m_tuiZhuTimes = m_playtype.NiuNiu_NewGetTuiZhu();

		// Сѡ��[6]���Ƿ�֧�ֶ�̬���룺false-������true-����
		m_isDynamicIn = m_playtype.NiuNiu_NewGetDynamicIn();

		// Сѡ��[7]���Ƿ������йܣ�false-������true-����
		m_isAutoPlay = m_playtype.NiuNiu_NewGetAutoPlay();

		// Сѡ��[8]���Ƿ�������ӣ�false-������true-����
		m_isLaiZi = m_playtype.NiuNiu_NewAllowLaiZi();

		// Сѡ��[9]���Ƿ��������룺false-������true-����
		m_isMaiMa = m_playtype.NiuNiu_NewAllowMaiMa();

		// Сѡ��[10]���Ƿ�������ƣ�false-������true-����
		m_isCuoPai = m_playtype.NiuNiu_NewGetCuoPai();

		// Сѡ��[11]���Ƿ���ע���ƣ�false-�����ƣ�true-����
		m_isConstAddScore = m_playtype.NiuNiu_NewGetConstAddScore();

		// Сѡ��[12]���Ƿ���ע���ƣ�false-�����ƣ�true-����
		m_isConstTuiZhu = m_playtype.NiuNiu_NewGetConstTuiZhu();

		// Сѡ��[13]���Ƿ�����ӱ���false-������true-����
		m_isAddScoreDouble = m_playtype.NiuNiu_NewGetConstAddScoreDouble();

		// Сѡ��[14]��1��ţţx3  ţ��x2  ţ��x2  ţ��-��ţx1   2��ţţx4  ţ��x3  ţ��x2  ţ��x2  ţ�� - ��ţx1
		m_niuNiuTimes = m_playtype.NiuNiu_NewTypeTimes();

		// Сѡ��[15]���Ƿ��������ţ��false-������true-����
		m_isBiYi = m_playtype.NiuNiu_NewAllowTypeBiYi();

		// Сѡ��[16]���Ƿ�����˳��ţ��false-������true-����
		m_isShunZi = m_playtype.NiuNiu_NewAllowTypeShunZi();

		// Сѡ��[17]���Ƿ������廨ţ��false-������true-����
		m_isWuHua = m_playtype.NiuNiu_NewAllowTypeWuHua();

		// Сѡ��[18]���Ƿ�����ͬ��ţ��false-������true-����
		m_isTongHua = m_playtype.NiuNiu_NewAllowTypeTongHua();

		// Сѡ��[19]���Ƿ������«ţ��false-������true-����
		m_isHuLu = m_playtype.NiuNiu_NewAllowTypeHuLu();

		// Сѡ��[20]���Ƿ�����ը��ţ��false-������true-����
		m_isZhaDan = m_playtype.NiuNiu_NewAllowTypeZhaDan();

		// Сѡ��[21]���Ƿ�������Сţ��false-������true-����
		m_isWuXiao = m_playtype.NiuNiu_NewAllowTypeWuXiao();

		// Сѡ��[22]���Ƿ��������ţ��false-������true-����
		m_isKuaiLe = m_playtype.NiuNiu_NewAllowTypeKuaiLe();
		// Сѡ��[23]���Ƿ��Ǽ���ģʽ��false-��ͨģʽ�� true-����ģʽ
		m_fastStyle = m_playtype.NiuNiu_NewFastStyle();				


		//����ţţ����ѡ��
		m_gamelogic.SetNiuNiuTimes(m_niuNiuTimes);

		//�����Ƿ��������
		m_gamelogic.SetAllowLaiZi(m_isLaiZi);

		//�����������������
		m_gamelogic.SetAllowSecialType(m_isBiYi, m_isShunZi, m_isWuHua, m_isTongHua, m_isHuLu, m_isZhaDan, m_isWuXiao, m_isKuaiLe);

		if (m_fastStyle)
		{
			m_time_qiang_zhuang = DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU_FAST;
			m_time_add_score = DELAY_TIME_ADD_SCORE_FAST;
			m_time_open_card = DELAY_TIME_OPEN_CARD_FAST;
		}
		else
		{
			m_time_qiang_zhuang = DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU;
			m_time_add_score = DELAY_TIME_ADD_SCORE;
			m_time_open_card = DELAY_TIME_OPEN_CARD;
		}

		//��ӡ�����������ϷСѡ��
		std::stringstream ss;
		ss << "NiuNiu_NewGameHandler::SetPlayType() Run... This Desk Play Type List ... deskId=[%d], ";
		ss << "mainMode=[%d], playerNum=[%d], qiangZhuangTimes=[%d], addScoreTimes=[%d], autoStart=[%d], tuiZhuTimes=[%d], ";
		ss << "isDynamicIn=[%d], isAutoPlay=[%d], isLaiZi=[%d], isMaiMa=[%d], isCuoPai=[%d], isConstAddScore=[%d], isConstTuiZhu=[%d], ";
		ss << "niuNiuTimes=[%d], isKuaiLe=[%d], isZhaDan=[%d], isHuLu=[%d], isTongHua=[%d], isShunZi=[%d], isWuHua=[%d]";
		LLOG_ERROR(ss.str().c_str(), m_desk ? m_desk->GetDeskId() : 0,
			m_mainMode, m_playerNum, m_qiangZhuangTimes, m_addScoreTimes, m_autoStart, m_tuiZhuTimes,
			m_isDynamicIn, m_isAutoPlay, m_isLaiZi, m_isMaiMa, m_isCuoPai, m_isConstAddScore, m_isConstTuiZhu,
			m_niuNiuTimes, m_isKuaiLe, m_isZhaDan, m_isHuLu, m_isTongHua, m_isShunZi, m_isWuHua);
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
		if (!m_desk || !m_desk->m_vip)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::MHSetDeskPlay() error !!!! m_desk or  m_desk->m_vip  null");
			return;
		}

		LLOG_DEBUG("NiuNiu_NewGameHandler::MHSetDeskPlay() Run... deskId=[%d], playerCount=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, play_user_count);

		m_player_count = play_user_count;

		start_round(player_status);
	}

	/********************************************************************************
	* ��������      HanderNiuNiuSelectZhuang()
	* ������
	����ţţ��ҿ���ѡׯ */
	bool HanderNiuNiuSelectZhuang(User* pUser, NiuNiuC2SSelectZhuang* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			return false;
		}
		if (GAME_PLAY_SELECT_ZHUANG != m_play_status)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuSelsectZhuang() error!!!, status not GAME_PLAY_SELECT_ZHUANG!!!, deskId=[%d], userId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);

			return false;
		}
		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuSelsectZhuang() error!!! Player pos is invalid!!! deskId=[%d], userId=[%d], playStatus=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, m_play_status);

			return false;
		}
		//ֻ�п�����ׯ����淨�Ż������
		return on_event_user_select_zhuang(GetUserPos(pUser), msg->m_qingScore);
	}

	/* ����ţţ������� */
	bool HanderNiuNiuMaiMa(User* pUser, NiuNiuC2SMaiMa* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			return false;
		}

		if (GAME_PLAY_MAI_MA != m_play_status)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuMaiMa() Error!!! Game Status is not GAME_PLAY_MAI_MA!!! deskId=[%d], userId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuMaiMa() Error!!! Player Pos is invalid!!! deskId=[%d], userId=[%d], pos=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}

		return on_event_user_mai_ma(GetUserPos(pUser), msg->m_maiPos, msg->m_maiScore);

	}

	/* ����ţţ�����ע */
	bool HanderNiuNiuAddScore(User* pUser, NiuNiuC2SAddScore* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuAddScore() error!!! player pos is invalid!!! deskId=[%d], userId=[%d], playStatus=[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, m_play_status);

			return false;
		}

		if (GAME_PLAY_ADD_SCORE != m_play_status && !(GAME_PLAY_MAI_MA == m_play_status && m_player_maima[GetUserPos(pUser)][0] != -1))
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuAddScore() error!!! status not GAME_PLAY_ADD_SCORE!!! deskId=[%d], userId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);

			return false;
		}
		
		return on_event_user_add_score(GetUserPos(pUser), msg->m_score, msg->m_isAddScoreDouble, msg->m_isTuiScore);

	}

	/* ����ţţ������� */
	bool HanderNiuNiuOpenCard(User* pUser, NiuNiuC2SOpenCard* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			return false;
		}

		if (GAME_PLAY_SEND_CARD != m_play_status)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuOpenCard() Error..., status not GAME_PLAY_SEND_CARD, deskId=[%d], user[%d], palyStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);

			return false;
		}

		Lint t_pos = GetUserPos(pUser);
		if (t_pos == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuOpenCard() Error..., player pos is invalid, deskId=[%d], userId[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, m_play_status);

			return false;
		}
		if (m_mainMode == MAIN_MODE_GONG_PAI_NIU_NIU && t_pos != m_open_gong_pos)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HanderNiuNiuOpenCard() Error..., open gong pai player pos is invalid, deskId=[%d], userId[%d], pos=[%d], gongPos=[%d], mainMode=[&d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, t_pos, m_open_gong_pos, m_mainMode, m_play_status);
			return false;
		}

		return on_event_user_open_card(GetUserPos(pUser));
	}

	/*
	ţţ����Ҵ깫��
	MSG_C_2_S_NIUNIU_CUO_GONG_PAI = 62088
	*/
	bool HandNiuNiuCuoGongPai(User* pUser, NiuNiuC2SCuoGongPai* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuCuoGongPai() Error!!! The user or desk is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuCuoGongPai() Error!!! Player pos is invald!!!, deskId=[%d], pos[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, GetUserPos(pUser));
			return false;
		}

		return on_event_user_cuo_gong_card(GetUserPos(pUser), msg->m_cuoPai);
	}
	/*
	ţţ�����ѡ���й���Ϣ
	MSG_C_2_S_NIUNIU_TUO_GUAN = 62084
	*/
	bool HandNiuNiuTuoGuan(User* pUser, NiuNiuC2STuoGuan* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuTuoGuan() Error!!! The user or desk is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuTuoGuan() Error!!! Player pos is invald!!!, deskId=[%d], pos[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, GetUserPos(pUser));
			return false;
		}

		return on_event_user_tuo_guan(GetUserPos(pUser), msg->m_qiangScore, msg->m_addScore, msg->m_tuiScore);
	}

	/*
	ţţ�����ȡ���й���Ϣ
	MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN = 62085
	*/
	bool HandNiuNiuCancelTuoGuan(User* pUser, NiuNiuC2SCancelTuoGuan* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuTuoGuan() Error!!! The user or desk is NULL...");
			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuTuoGuan() Error!!! Player pos is invald!!!, deskId=[%d], pos[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, GetUserPos(pUser));
			return false;
		}
		return on_event_user_cancel_tuo_guan(GetUserPos(pUser));
	}

	/*
	�ͻ���֪ͨ��������ʼ��ʱ
	MSG_C_2_S_START_TIME = 61198
	*/
	bool HandNiuNiuStartTime(User* pUser, NiuNiuC2SStartTime* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuStartTime() Error!!! The user or desk is NULL...");
			return false;
		}
		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::HandNiuNiuStartTime() Error!!! Player pos is invald!!!, deskId=[%d], pos[%d] ",
				m_desk ? m_desk->GetDeskId() : 0, GetUserPos(pUser));
			return false;
		}

		return on_event_start_time(GetUserPos(pUser));
	}

	//��ȡ�ƾ��вμӵ���������
	Lint GetRealPlayerCount() 
	{ 
		return m_real_player_count; 
	}

	/* ������Ҷ������� */
	void OnUserReconnect(User* pUser)
	{
		if (NULL == pUser || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::OnUserReconnect() Error... The user or desk is NULL...");
			return;
		}

		LLOG_ERROR("NiuNiuGameHandle::OnUserReconnect() Run... deskId=[%d], reconnUserId=[%d], curPos=[%d], playStatus=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_curPos, m_play_status);

		//Desk �Ѿ�����
		notify_desk_match_state();

		Lint pos = m_desk->GetUserPos(pUser);

		if (INVAILD_POS_QIPAI == pos && !m_desk->MHIsLookonUser(pUser))
		{
			LLOG_ERROR("NiuNiuGameHandle::OnUserReconnect() Error... This Reconn User is not Seat User and not Look On User... deskId=[%d], reconnUser=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId());
			return;
		}

		/*LMsgS2CVipInfo msg;
		msg.m_curCircle = m_round_offset + 1;
		msg.m_curMaxCircle = m_round_limit;
		notify_user(msg, pos);*/


		Lint play_delay_time = 0;
		NiuNiuS2CRecon reconn;
		reconn.m_playStatus = m_play_status;
		memcpy(reconn.m_playerTuiScore, m_player_tui_score, sizeof(reconn.m_playerTuiScore));
		memcpy(reconn.m_user_status, this->m_user_status, sizeof(m_user_status));
		memcpy(reconn.m_addZhuang, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
		memcpy(reconn.m_addScore, m_play_add_score, sizeof(m_play_add_score));
		memcpy(reconn.m_playerAddScoreDouble, m_canAddScoreDouble, sizeof(reconn.m_playerAddScoreDouble));
		memcpy(reconn.m_addScoreDoubleStatus, m_add_score_double_status, sizeof(reconn.m_addScoreDoubleStatus));
		memcpy(reconn.m_playerXiaScore, m_canXiaMinZhu, sizeof(reconn.m_playerXiaScore));
		memcpy(reconn.m_isCanTui, m_canTuiZhu, sizeof(reconn.m_isCanTui));
		if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
		{
			reconn.m_isCoinsGame = 1;
			memcpy(reconn.m_totleScore, m_accum_coins, sizeof(reconn.m_totleScore));
			for (int i = 0; i < m_player_count; ++i)
			{
				if (m_desk->m_user[i] && m_desk->m_playStatus[i] == 2 && m_user_status[i] == 0)
				{
					reconn.m_totleScore[i] = m_desk->GetUserCoins(m_desk->m_user[i]);
				}
			}
		}

		if (NIUNIU_INVAILD_POS > pos)
		{
			//�¼��ֶ�
			if (m_user_status[pos] == 1)
				reconn.m_DynamicJoin = 0;
			else
				reconn.m_DynamicJoin = 1;
		}
		else if (m_desk->MHIsLookonUser(pUser))
		{
			reconn.m_isLookOn = 1;
			LLOG_DEBUG("NiuNiu_NewGameHandler::OnUserReconnect()  1 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
		}

		switch (m_play_status)
		{
			//ѡׯ�׶ζ���
		case GAME_PLAY_SELECT_ZHUANG:
		{
			reconn.m_noQiangZhuang = pos != INVAILD_POS_QIPAI ? m_noQiangZhuang[pos] : 0;

			switch (m_mainMode)
			{
				//ţţ��ׯ
			case MAIN_MODE_NIUNIU_SHANG_ZHUANG:
			{
				reconn.m_hasSelectZhuang = 1;
				reconn.m_zhuangPos = m_zhuangPos;
				break;
			}
			//������ׯ
			case MAIN_MODE_ZI_YOU_QIANG_ZHUANG:
			{
				reconn.m_hasSelectZhuang = 0;
				if (NIUNIU_INVAILD_POS > pos && m_play_qiang_zhuang[pos] >= 0)
				{
					//���û���ׯѺע�׶��Ѿ���������
					reconn.m_hasSelectZhuang = 1;
				}
				play_delay_time = m_time_qiang_zhuang;
				break;
			}
			//�������ơ�������ׯ������ţţ
			case MAIN_MODE_BA_REN_MING_PAI:
			case MAIN_MODE_MING_PAI_QIANG_ZHUANG:
			case MAIN_MODE_GONG_PAI_NIU_NIU:
			{
				reconn.m_hasSelectZhuang = 0;
				if (NIUNIU_INVAILD_POS > pos && m_play_qiang_zhuang[pos] >= 0)
				{
					//���û���ׯѺע�׶��Ѿ���������
					reconn.m_hasSelectZhuang = 1;
				}
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1 && NIUNIU_INVAILD_POS > pos; i++)
				{
					reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
				}
				//memcpy(reconn.m_addZhuang, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
				play_delay_time = m_time_qiang_zhuang;
				break;
			}

			}
			break;
		}

		//����׶ζ���
		case GAME_PLAY_MAI_MA:
		{
			reconn.m_zhuangPos = m_zhuangPos;
			for (Lint i = 0; i < m_player_count; ++i)
			{
				reconn.m_maiMaPos[i] = m_player_maima[i][1];
				reconn.m_maiMaScore[i] = m_player_maima[i][0];
				if (m_player_maima[i][0] != -1)
				{
					reconn.m_hasMaiMa[i] = 1;
				}
				if (m_play_add_score[i] != 0)
				{
					//��ʾ�Ѿ��¹�ע��
					reconn.m_hasScore[i] = 1;
				}
			}
			//�������ţţ��ׯ��������ׯ ��ҵ�ǰ4�����Ƹ������
			if (MAIN_MODE_NIUNIU_SHANG_ZHUANG != m_mainMode && MAIN_MODE_ZI_YOU_QIANG_ZHUANG != m_mainMode)
			{
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1 && NIUNIU_INVAILD_POS > pos; i++)
				{
					reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
				}
			}
			//�������ע����һ��ʱ��
			play_delay_time = m_add_score_delay_time;
			break;
		}

		//��ע�׶ζ���
		case GAME_PLAY_ADD_SCORE:
		{
			reconn.m_zhuangPos = m_zhuangPos;

			for (Lint i = 0; i < m_player_count; i++)
			{
				reconn.m_maiMaPos[i] = m_player_maima[i][1];
				reconn.m_maiMaScore[i] = m_player_maima[i][0];
				if (m_player_maima[i][0] != -1)
				{
					reconn.m_hasMaiMa[i] = 1;
				}

				if (m_play_add_score[i] != 0)
				{
					//��ʾ�Ѿ��¹�ע��
					reconn.m_hasScore[i] = 1;
				}
			}
			//�������ţţ��ׯ��������ׯ ��ҵ�ǰ4�����Ƹ������
			if (MAIN_MODE_NIUNIU_SHANG_ZHUANG != m_mainMode && MAIN_MODE_ZI_YOU_QIANG_ZHUANG != m_mainMode)
			{
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1 && NIUNIU_INVAILD_POS > pos; i++)
				{
					reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
				}
			}
			play_delay_time = m_add_score_delay_time;
			break;
		}
		//���� or ����
		case GAME_PLAY_SEND_CARD:
		{
			reconn.m_zhuangPos = m_zhuangPos;
			reconn.m_openGongPos = m_open_gong_pos;
			reconn.m_gongCard = m_gong_card;
			reconn.m_cuoPai = m_cuo_pai;
			if (pos != NIUNIU_INVAILD_POS)
			{
				reconn.m_player_ox[pos] = m_player_oxnum[pos];
				reconn.m_player_oxnum[pos] = m_player_oxnum[pos];
				reconn.m_oxTimes[pos] = m_player_oxtimes[pos];
			}

			for (Lint i = 0; i < m_player_count; i++)
			{
				reconn.m_maiMaPos[i] = m_player_maima[i][1];
				reconn.m_maiMaScore[i] = m_player_maima[i][0];
				if (m_player_maima[i][0] != -1)
				{
					reconn.m_hasMaiMa[i] = 1;
				}

				if (m_play_add_score[i] != 0)
				{
					//��ʾ�Ѿ��¹�ע��
					reconn.m_hasScore[i] = 1;
				}
			}

			for (Lint i = 0; i < m_player_count; i++)
			{
				if (m_user_status[i] != 1) continue;

				reconn.m_hasOpenCard[i] = m_isOpenCard[i] ? 1 : 0;
				if (pos == i) // �Լ�������
				{
					for (Lint card = 0; card < NIUNIU_HAND_CARD_MAX; card++)
					{
						reconn.m_playerHandCard[i][card] = (Lint)m_player_hand_card[i][card];
						reconn.m_showHandCard[i][card] = m_show_hand_card[i][card];
					}
				}

				if (m_isOpenCard[i]) // �����������
				{
					reconn.m_player_ox[i] = m_player_oxnum[i];
					reconn.m_player_oxnum[i] = m_player_oxnum[i];
					reconn.m_oxTimes[i] = m_player_oxtimes[i];
					//����������Ƶ����
					for (Lint card = 0; card < NIUNIU_HAND_CARD_MAX; card++)
					{
						reconn.m_playerHandCard[i][card] = (Lint)m_player_hand_card[i][card];
						reconn.m_showHandCard[i][card] = m_show_hand_card[i][card];
					}
				}
				else if (MAIN_MODE_GONG_PAI_NIU_NIU == m_mainMode)
				{
					for (Lint card = 0; card < NIUNIU_HAND_CARD_MAX - 1; card++)
					{
						reconn.m_playerHandCard[i][card] = (Lint)m_player_hand_card[i][card];
					}
				}
			}
			play_delay_time = m_time_open_card;
			break;
		}
		default:
			break;
		}

		if (play_delay_time && m_isAutoPlay && m_tick_flag)
		{
			LTime curTime;
			Lint time_escaped = curTime.Secs() - m_play_status_time.Secs();
			Lint time_remain = play_delay_time - time_escaped;
			if (time_remain < 0)
			{
				time_remain = 0;
			}
			else if (time_remain > play_delay_time)
			{
				time_remain = play_delay_time;
			}

			reconn.m_remainTime = time_remain;
			LLOG_ERROR("****��������desk_id=[%d], OnUserReconnect play_status:%d , delay:%d , escaped:%d  remain:%d",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status, play_delay_time, time_escaped, time_remain);

		}
		if (NIUNIU_INVAILD_POS > pos)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::OnUserReconnect()  2 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
			pUser->Send(reconn);
		}
		else if (m_desk->MHIsLookonUser(pUser))
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::OnUserReconnect()  3 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
			pUser->Send(reconn);
		}
	}

	void LookUserReconnectOnDeskWait(User* pUser) 
	{
		if (!pUser)
		{

			return;
		}
		NiuNiuS2CResult lookWaitResult;
		if (m_desk && m_desk->m_vip)
		{
			if (m_desk->m_isCoinsGame == 1 && m_desk->m_clubInfo.m_clubId != 0)
			{
				memcpy(lookWaitResult.m_totleScore, m_desk->m_vip->m_coins, sizeof(lookWaitResult.m_totleScore));
			}
			else
			{
				memcpy(lookWaitResult.m_totleScore, m_desk->m_vip->m_score, sizeof(lookWaitResult.m_totleScore));
			}
		}
		lookWaitResult.m_zhuangPos = m_zhuangPos;
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (m_user_status[i] != 1)
			{
				continue;
			}
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
			{
				lookWaitResult.m_playerHandCard[i][j] = (Lint)m_player_hand_card[i][j];
				lookWaitResult.m_showHandCard[i][j] = (Lint)m_show_hand_card[i][j];
			}
		}
		memcpy(lookWaitResult.m_playScore, m_player_score, sizeof(lookWaitResult.m_playScore));
		memcpy(lookWaitResult.m_oxNum, m_player_oxnum, sizeof(lookWaitResult.m_oxNum));
		memcpy(lookWaitResult.m_oxTimes, m_player_oxtimes, sizeof(lookWaitResult.m_oxTimes));
		memcpy(lookWaitResult.m_playerStatus, m_user_status, sizeof(lookWaitResult.m_playerStatus));

		pUser->Send(lookWaitResult);
	}

	Lint GetPlayerAccumCoins(User* pUser)
	{
		if (!pUser)
		{

			return -100000;
		}

		Lint t_pos = GetUserPos(pUser);
		if (t_pos != NIUNIU_INVAILD_POS)
		{
			return m_accum_coins[t_pos];
		}
		else
		{
			return pUser->m_userData.m_coins;
		}
	}
	/* ������Ϸ���� */
	void OnGameOver(Lint result, Lint winpos, Lint bombpos)
	{
		if (m_desk == NULL || m_desk->m_vip == NULL)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::OnGameOver() Error... This desk is NULL");
			return;
		}

		m_dismissed = !!m_desk->m_vip->m_reset;
		MHLOG_PLAYCARD("****Desk game over. Desk:%d round_limit: %d round: %d dismiss: %d",
			m_desk ? m_desk->GetDeskId() : 0, m_round_limit, m_round_offset, m_dismissed);

		//��ɢ���䣬��Ϸ����
		finish_round(1);
	}

	/* ��ʱ�� */
	void Tick(LTime& curr)
	{
		//���Ӷ���Ϊ��
		if (m_desk == NULL)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::Tick() Error... This is desk NULL... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}

		//û�п�����ʱ�Զ�����ѡ��  ��  ��ʱ����Ч
		/*if (m_isAutoPlay == false || m_tick_flag == false)
		{
		return;
		}*/

		//�����ɢ�У���ʱ����������
		if (m_desk && m_desk->m_resetTime > 0 && m_desk->m_resetUserId > 0)
		{
			LLOG_ERROR("NiuNiu_NewGameHandler::Tick() The desk is reseting... So tick do nothing...", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}

		LLOG_ERROR("NiuNiu_NewGameHandler::Tick() Run... deskId=[%d], playStatus=[%d], autoPlay=[%d], tickFlag=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_play_status, m_isAutoPlay, m_tick_flag);

		switch (m_play_status)
		{

			//ѡׯ�׶�
		case GAME_PLAY_SELECT_ZHUANG:
		{
			switch (m_mainMode)
			{
				//��ׯģʽ
			case MAIN_MODE_NIUNIU_SHANG_ZHUANG:
			{

				break;
			}
			//������ׯ��ʮ�����ơ�������ׯ������ţţ
			case MAIN_MODE_ZI_YOU_QIANG_ZHUANG:
			case MAIN_MODE_BA_REN_MING_PAI:
			case MAIN_MODE_MING_PAI_QIANG_ZHUANG:
			case MAIN_MODE_GONG_PAI_NIU_NIU:
			{
				LLOG_DEBUG("NiuNiu_NewGameHandler::Tick() Run... Tick in Select Zhuang Status, Now is Qiang Zhuang... deskId=[%d], playStatus=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, m_play_status);

				if (curr.Secs() >= m_play_status_time.Secs() + m_time_qiang_zhuang)
				{
					for (Lint i = 0; i < m_player_count; i++)
					{
						if (m_play_qiang_zhuang[i] == -1 && m_user_status[i] == 1)
						{
							LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TimeOverRun... Qiang Zhuang Time over!!!, Auto Qiang Zhuang... deskId=[%d], pos=[%d]",
								m_desk ? m_desk->GetDeskId() : 0, i);

							on_event_user_select_zhuang(i, 0);
						}
					}
					//ִ���굹��ʱ�����õ���ʱ
					//m_tick_flag = false;
				}
				//�й�ִ��
				else
				{
					for (Lint i = 0; i < m_player_count; ++i)
					{
						if (m_tuo_guan_info[i].m_tuoGuanStatus == 1 && m_play_qiang_zhuang[i] == -1 && m_user_status[i] == 1)
						{
							LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TuoGuanRun... TuoGuan Qiang Zhuang... deskId=[%d], pos=[%d]",
								m_desk ? m_desk->GetDeskId() : 0, i);

							on_event_user_select_zhuang(i, tuoguan_qiang_zhuang(i));
						}
					}
				}
				break;
			}

			}

			break;
		}

		//����׶�
		case GAME_PLAY_MAI_MA:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_add_score_delay_time - 1)
			{
				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_player_maima[i][0] == -1 && m_user_status[i] == 1)
					{
						LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TimeOverRun... Mai Ma Time over!!!, Auto Mai Ma... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);

						on_event_user_mai_ma(i, NIUNIU_INVAILD_POS, 0);
					}
				}
			}
			else
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_tuo_guan_info[i].m_tuoGuanStatus == 1 && m_player_maima[i][0] == -1 && m_user_status[i] == 1)
					{
						LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TuoGuanRun... TuoGuan Mai Ma... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);

						on_event_user_mai_ma(i, NIUNIU_INVAILD_POS, 0);
					}
				}
			}

			break;
		}

		//��ע�׶�
		case GAME_PLAY_ADD_SCORE:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_add_score_delay_time)
			{
				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_play_add_score[i] == 0 && m_user_status[i] == 1)
					{
						LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TimeOverRun... Add Score Time over!!!, Auto Add Score... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);

						on_event_user_add_score(i, min_add_score(i), 0, 0);
					}
				}
				//ִ���굹��ʱ�����õ���ʱ
				//m_tick_flag = false;
			}
			else
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_tuo_guan_info[i].m_tuoGuanStatus == 1 && m_play_add_score[i] == 0 && m_user_status[i] == 1)
					{
						LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TuoGuanRun... TuoGuan Add Score... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);
						int t_tuoGuanTuiScore = tuoguan_tui_score(i);
						int t_tuoGuanAddScore = tuoguan_add_score(i);
						int t_addScore = t_tuoGuanAddScore > t_tuoGuanTuiScore ? t_tuoGuanAddScore : t_tuoGuanTuiScore;
						int t_isTuiScore = t_tuoGuanAddScore > t_tuoGuanTuiScore ? 0 : 1;
						on_event_user_add_score(i, t_addScore, 0, t_isTuiScore);
					}
				}
			}

			break;
		}

		//���ƽ׶�
		case GAME_PLAY_SEND_CARD:
		{
			if (curr.Secs() >= m_play_status_time.Secs() + m_time_open_card)
			{
				for (Lint i = 0; i < m_player_count; i++)
				{
					if (m_play_status != GAME_PLAY_SEND_CARD) break;

					if (m_isOpenCard[i] == 0 && m_user_status[i] == 1)
					{
						LLOG_DEBUG("NiuNiu_NewGameHandler::Tick() TimeOverRun... Open Card Time over!!!, Auto Open Card... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);

						on_event_user_open_card(i);
					}
				}
			}
			else
			{
				for (Lint i = 0; i < m_player_count; ++i)
				{
					if (m_play_status != GAME_PLAY_SEND_CARD) break;

					if (m_tuo_guan_info[i].m_tuoGuanStatus == 1 && m_isOpenCard[i] == 0 && m_user_status[i] == 1)
					{
						LLOG_ERROR("NiuNiu_NewGameHandler::Tick() TuoGuanRun... TuoGuan Open Card... deskId=[%d], pos=[%d]",
							m_desk ? m_desk->GetDeskId() : 0, i);

						on_event_user_open_card(i);
					}
				}
			}

			break;
		}

		}//switch (m_play_status) end...
	}
};

DECLARE_GAME_HANDLER_CREATOR(108, NiuNiu_NewGameHandler);