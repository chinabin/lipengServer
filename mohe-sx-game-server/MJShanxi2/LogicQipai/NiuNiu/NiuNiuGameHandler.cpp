#include "Desk.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "NiuNiuGameLogic.h"
#include "LMsgL2C.h"



static int LoadPlayerCards(std::vector<BYTE> & cards, int playerIndex)
{
	std::string fileName("card.ini");
	LIniConfig config;
	config.LoadFile("card.ini");

	char szKey[32] = { 0 };
	if (playerIndex > 0 && playerIndex < 10)
	{
		sprintf(szKey, "player%dcards", playerIndex);
	}

	std::string cardsString = config.GetString(Lstring(szKey), "");
	std::vector<std::string> cardsStringVec = splitString(cardsString, ",", true);
	if (cardsStringVec.size() > 0)
	{
		cards.clear();
		for (auto it = cardsStringVec.begin(); it < cardsStringVec.end(); it++)
		{
			BYTE nValue = atoi((*it).c_str());
			MHLOG("Card: %d  %s", nValue, (*it).c_str());
			cards.push_back(nValue);
		}
	}

	return 0;
}


/* ׯ��ģʽ��ö�٣�*/
enum GAME_ZHUANG_MODE
{
	ZHUANG_MODE_LUN = 0,    //��ׯ
							//ZHUANG_MODE_RUND = 1,   //���ׯ
							ZHUANG_MODE_QIANG = 1,  //������ׯ
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
	GAME_PLAY_SELECT_ZHUANG,
	GAME_PLAY_ADD_SCORE,
	GAME_PLAY_SEND_CARD,
	GAME_PLAY_END,
};

/* �����׶εĳ�ʱʱ�䣨ö�٣� */
enum PLAY_STATUS_DELAY_TIME
{
	//DELAY_TIME_SELECT_ZHUANG = 10,               //ѡׯ
	////DELAY_TIME_SELECT_QIANG_ZHUANG = 30,  //��ׯ
	////DELAY_TIME_ADD_SCORE = 30,                      //��ע
	////DELAY_TIME_OPEN_CARD = 20,                      //����
	//DELAY_TIME_SELECT_QIANG_ZHUANG = 15,  //��ׯ
	//DELAY_TIME_ADD_SCORE = 15,                      //��ע
	//DELAY_TIME_OPEN_CARD = 10,                      //����

	DELAY_TIME_SELECT_ZHUANG = 5,               //ѡׯ
	DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU = 7,  //��ׯ
	DELAY_TIME_ADD_SCORE = 5,                      //��ע
	DELAY_TIME_OPEN_CARD = 4,                      //����
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
	* ��������   NiuNiuGetZhuangMode()
	* ������     ��ȡ�ͻ��˶����ׯ�ҹ���
	* ��ϸ��     ׯ�ҹ���0-����ׯ��1��������ׯ
	* ���أ�     (Lint)�ͻ���ѡ���ׯ�ҹ���
	********************************************************/
	Lint NiuNiuGetZhuangMode() const;

	/******************************************************
	* ��������   NiuNiuGetPlayMode()
	* ������     ��ȡ�ͻ���ѡ����淨ģʽ
	* ��ϸ��     �淨ģʽ��0-��ͨģʽ1��1-��ͨģʽ2��2-ɨ��ģʽ��3-������ע��4-������ע
	* ���أ�     (Lint)�ͻ���ѡ����淨ģʽ
	********************************************************/
	Lint NiuNiuGetPlayMode() const;

	/******************************************************
	* ��������   NiuNiuGetSpecPlay()
	* ������     ��ȡ�ͻ��˶���Ļ����淨ģʽ
	* ��ϸ��     ����ģʽ��false-��������true-����
	* ���أ�     (BOOL)�ͻ���ѡ��Ļ���ģʽ
	********************************************************/
	bool NiuNiuGetSpecPlay() const;

	/******************************************************
	* ��������   NiuNiuGetAutoPlay()
	* ������     ��ȡ�ͻ��˳�ʱ�Զ�����
	* ��ϸ��     ɨ��ģʽ��false-��������true-����
	* ���أ�     (bool)�ͻ���ѡ��ʱ�Զ�����
	********************************************************/
	bool NiuNiuGetAutoPlay() const;

	/******************************************************
	* ��������   NiuNiuGetDynamicIn()
	* ������     ��ȡ�ͻ����Ƿ�����̬����ѡ��
	* ��ϸ��     ����ׯ��false-������true-����
	* ���أ�     (bool)�Ƿ�̬����
	********************************************************/
	bool NiuNiuGetDynamicIn() const;

	/******************************************************
	* ��������   NiuNiuGetPlayerNum()
	* ������     ��ȡ�ͻ�������֧�֣�6�˻�10��
	* ���أ�     (int)������
	********************************************************/
	Lint NiuNiuGetPlayerNum() const;

	/******************************************************
	* ��������   NiuNiuQiangZhuangTimes()
	* ������     ��ȡţţ��ׯ���ʣ���ׯ����Ϊ1��
	* ��ϸ��     ��ׯ���ʣ�1-1����2-2����3-3����4-4��
	* ���أ�     (Lint)ѡ�����ׯ����
	********************************************************/
	Lint NiuNiuQiangZhuangTimes() const;

	/******************************************************
	* ��������   NiuNiuAddScoreTimes()
	* ������     ��ȡţţ��ע���ʣ���
	* ��ϸ��     ��ׯ���ʣ�0-С����1-�б���2-��
	* ���أ�     (Lint)ѡ�����ע����
	********************************************************/
	Lint NiuNiuAddScoreTimes() const;

	/******************************************************
	* ��������	NiuNiuTypeTimes()
	* ������	ţţ�������α���ţţX4
	* ��ϸ��	ţ��-ţ�ˣ�2����ţ�ţ�3����ţţ��4��...
	* ���أ�	(int) ��ѡ��ѡ��أ�0�� ѡ���ѡ��� 1
	*******************************************************/
	Lint NiuNiuTypeTimes() const;

	/******************************************************
	* ��������	NiuNiuTuiZhu()
	* ������	ţţ��עѡ��
	* ��ϸ��	��ע������ 0������ע��5��5����ע��10��10����ע
	* ���أ�	(Lint)��ע����
	*******************************************************/
	Lint NiuNiuTuiZhu() const;
};



/******************************************************
* ��������   NiuNiuGetZhuangMode()
* ������     ��ȡ�ͻ��˶����ׯ�ҹ���
* ��ϸ��     ׯ�ҹ���0-����ׯ��1-������ׯ
* ���أ�     (Lint)�ͻ���ѡ���ׯ�Ҷ���
********************************************************/
Lint PlayType::NiuNiuGetZhuangMode() const
{
	if (m_playtype.empty())
	{
		return 0;
	}
	return m_playtype[0];
}

/******************************************************
* ��������   NiuNiuGetPlayMode()
* ������     ��ȡ�ͻ���ѡ����淨ģʽ
* ��ϸ��     �淨ģʽ��0-��ͨģʽ1��1-��ͨģʽ2��2-ɨ��ģʽ��3-������ע��4-������ע
* ���أ�     (Lint)�ͻ���ѡ����淨ģʽ
********************************************************/
Lint PlayType::NiuNiuGetPlayMode() const
{
	if (m_playtype.size() < 2)
	{
		//������ׯ��Ĭ����ͨģʽ1
		if (m_playtype[0] == 1) return 0;

		//������ׯ��Ĭ�ϰ�����ע
		else if (m_playtype[0] == 0) return 4;
	}

	////������ׯ�淨��
	//if (m_playtype[0] == 1 && (m_playtype[1] < 0 || m_playtype[1] > 2))
	//{
	//	return 0;
	//}
	////��ׯ�淨��
	//else if(m_playtype[0] == 0 && (m_playtype[1] < 2 || m_playtype[1] > 4))
	//{
	//	return 4;
	//}
	return m_playtype[1];
}

/******************************************************
* ��������   NiuNiuGetZhuangMode()
* ������     ��ȡ�ͻ��˶���Ļ����淨ģʽ
* ��ϸ��     ����ģʽ��false-��������true-����
* ���أ�     (bool)�ͻ���ѡ��Ļ���ģʽ
********************************************************/
bool PlayType::NiuNiuGetSpecPlay() const
{
	if (m_playtype.size() < 3)
	{
		return false;
	}
	return m_playtype[2] == 0 ? false : true;
}

/******************************************************
* ��������   NiuNiuGetAutoPlay()
* ������     ��ȡ�ͻ��˳�ʱ�Զ�����
* ��ϸ��     ɨ��ģʽ��false-��������true-����
* ���أ�     (bool)�ͻ���ѡ��ʱ�Զ�����
********************************************************/
bool PlayType::NiuNiuGetAutoPlay() const
{
	if (m_playtype.size() < 4)
	{
		return false;
	}
	return m_playtype[3] == 0 ? false : true;
}

/******************************************************
* ��������   NiuNiuGetDynamicIn()
* ������     ��ȡ�ͻ����Ƿ�����̬����ѡ��
* ��ϸ��     ����ׯ��false-������true-����
* ���أ�     (Lint)�Ƿ�̬����
********************************************************/
bool PlayType::NiuNiuGetDynamicIn() const
{
	if (m_playtype.size() < 5)
	{
		return false;
	}
	return m_playtype[4] == 0 ? false : true;
}

/******************************************************
* ��������   NiuNiuGetPlayerNum()
* ������     ��ȡ�ͻ�������֧�֣�6�˻�10��
* ���أ�     (Lint)�Ƿ�̬����
********************************************************/
Lint PlayType::NiuNiuGetPlayerNum() const
{
	if (m_playtype.size() < 6)
	{
		return 6;
	}
	if (m_playtype[5] != 6 || m_playtype[5] != 10)
	{
		return 6;
	}
	return m_playtype[5];
}

/******************************************************
* ��������   NiuNiuQiangZhuangTimes()
* ������     ��ȡţţ��ׯ���ʣ���ׯ����Ϊ1��
* ��ϸ��     ��ׯ���ʣ�1-1����2-2����3-3����4-4��
* ���أ�     (Lint)ѡ�����ׯ����
********************************************************/
Lint PlayType::NiuNiuQiangZhuangTimes() const
{
	if (m_playtype.size() < 7)
	{
		return 1;
	}
	//��ׯ��������ׯ����Ϊ1��
	if (m_playtype[6] < 0 || m_playtype[6] > 3 || 0 == m_playtype[0])
	{
		return 1;
	}
	return m_playtype[6] + 1;
}

/******************************************************
* ��������   NiuNiuAddScoreTimes()
* ������     ��ȡţţ��ע���ʣ���
* ��ϸ��     ��ׯ���ʣ�0-С����1-�б���2-��
* ���أ�     (Lint)ѡ�����ע����
********************************************************/
Lint PlayType::NiuNiuAddScoreTimes() const
{
	if (m_playtype.size() < 8)
	{
		return 0;
	}
	//��ͨģʽ1��ɨ��ģʽ��ֻ��С��
	if (m_playtype[7] < 0 || m_playtype[7] > 2)
	{
		return 0;
	}
	return m_playtype[7];
}

/******************************************************
* ��������	NiuNiuTypeTimes()
* ������	ţţ�������α���ţţX4
* ��ϸ��	ţ��-ţ�ˣ�2����ţ�ţ�3����ţţ��4��...
* ���أ�	(int) ��ѡ��ѡ��أ�0�� ѡ���ѡ��� 1
*******************************************************/
Lint PlayType::NiuNiuTypeTimes() const
{
	if (m_playtype.size() < 9)
	{
		return 0;
	}
	return m_playtype[8];
}

/******************************************************
* ��������	NiuNiuTuiZhu()
* ������	ţţ��עѡ��
* ��ϸ��	��ע������ 0������ע��5��5����ע��10��10����ע
* ���أ�	(Lint)��ע����
*******************************************************/
Lint PlayType::NiuNiuTuiZhu() const
{
	if (m_playtype.size() < 10)
	{
		return 0;
	}
	return m_playtype[9];
}


/* ţţÿ�ֲ����ʼ���ֶΣ��ṹ�壩*/
struct NiuNiuRoundState__c_part
{
	Lint		 m_curPos;						                    // ��ǰ�������
	Lint         m_play_status;                                     // �ƾ�״̬
	BOOL         m_tick_flag;                                       // ��ʱ������
	Lint		 m_player_tui_score[NIUNIU_PLAY_USER_COUNT][4];		// ��ҿ�����ע�ķ���

	Lint         m_user_status[NIUNIU_PLAY_USER_COUNT];             // �û�״̬����λ�����Ƿ������ˣ�ʵ����Чλ�ã�
	Lint         m_play_add_score[NIUNIU_PLAY_USER_COUNT];          // �����ע����
	Lint         m_play_qiang_zhuang[NIUNIU_PLAY_USER_COUNT];       // �����ׯ������ Ĭ�ϣ�-1����Чλ��δ������ 0������ׯ�� >0��ׯ����
	bool         m_isOpenCard[NIUNIU_PLAY_USER_COUNT];              // ����Ƿ��ƣ�false-δ���ƣ�true-�ѿ���,��ʼΪ��false
	BYTE         m_player_hand_card[NIUNIU_PLAY_USER_COUNT][NIUNIU_HAND_CARD_MAX];   //�û�����																			 //��ұ�����Ӯ����
	Lint         m_player_score[NIUNIU_PLAY_USER_COUNT];            // ��ұ��ֵ÷�
	Lint         m_player_oxnum[NIUNIU_PLAY_USER_COUNT];            // ���ţ��

																	//BYTE         m_cbDynamicJoin[NIUNIU_PLAY_USER_COUNT];           // ��̬����
																	//BYTE         m_cbLeaveGame[NIUNIU_PLAY_USER_COUNT];             // ��;�볡
																	//BYTE		 m_cbPlayStatus[NIUNIU_PLAY_USER_COUNT];			// ��Ϸ״̬
																	//BYTE		 m_cbCallStatus[NIUNIU_PLAY_USER_COUNT];			// ��ׯ״̬
	BYTE		 m_cbOxCard[NIUNIU_PLAY_USER_COUNT];				// ţţ����
																	//BYTE		 m_cbFirest[NIUNIU_PLAY_USER_COUNT];				// ѡ��ţ��

																	/* ����ÿ���ֶ� */
	void clear_round()
	{
		memset(this, 0, sizeof(*this));

		//ĳЩ������Ҫ�������ʼֵ
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			m_play_qiang_zhuang[i] = -1;
			m_player_oxnum[i] = -1;
			m_isOpenCard[i] = false;
		}
	}
};

/* ţţÿ�ֻ��ʼ���ֶΣ��ṹ�壩*/
struct NiuNiuRoundState__cxx_part
{
	LTime m_play_status_time;
	/* ����ÿ���ֶ� */
	void clear_round()
	{
		this->~NiuNiuRoundState__cxx_part();
		new (this) NiuNiuRoundState__cxx_part;
	}
};

/* ţţÿ�������ʼ�����ֶΣ��ṹ�壩*/
struct NiuNiuMatchState__c_part
{
	Desk*         m_desk;           // ���Ӷ���
	Lint          m_round_offset;   // ��ǰ����
	Lint          m_round_limit;    // ��������ѡ���������
	Lint          m_accum_score[NIUNIU_PLAY_USER_COUNT];
	BOOL          m_dismissed;
	int           m_registered_game_type;
	int           m_player_count;   // �淨���Ĵ�����ʹ�õ���������ֶ�
	Lint          m_zhuangPos;      // ׯ��λ��
	bool          m_isFirstFlag;    // �׾���ʾ��ʼ��ť,true�����׾�
	bool		  m_canTuiZhu[NIUNIU_PLAY_USER_COUNT];  // �Ƿ������ע 0 ��������ע��1������ע
														/////�淨Сѡ��
	Lint          m_selectZhuangMode;      // Сѡ�ѡׯ����0-��ׯ��1-������ׯ
	Lint          m_playMode;              // Сѡ��淨ģʽ��0-��ͨģʽ1����������1-��ͨģʽ2����������2-ɨ��ģʽ��ͨ�ã���3-������ע����ׯ����4-������ע����ׯ��
	BOOL          m_isSpacePlay;           // Сѡ��Ƿ��������淨
	BOOL          m_isAutoPlay;            // Сѡ��Ƿ������й�
	BOOL          m_isDynamicIn;           // Сѡ��Ƿ�֧�ֶ�̬����
	Lint          m_playerNum;             // Сѡ�����6�˻�10��
	Lint          m_qiangZhuangTimes;      // Сѡ���ׯ���ʣ�1-1����2-2����3-3����4-4��
	Lint          m_addScoreTimes;         // Сѡ���ע���ʣ�0-С����1-�б���2-��
	Lint          m_scoreTimes;			   // Сѡ�������ֱ��� 0����ͨ����  1��ɨ��ģʽ����  2������ţţX4���ʣ�**Ren 2018-05-17��
	Lint		  m_tuiZhuTimes;		   // Сѡ���ע������**Ren 2018-05-19��
										   //BOOL          m_isSweepMode;           // Сѡ��Ƿ���ɨ��ģʽ
										   //BOOL          m_isMingPaiAddScore;     // Сѡ��Ƿ�������ע

										   /* ����ṹ���ֶ� */
	void clear_match()
	{
		memset(this, 0, sizeof(*this));
		m_zhuangPos = NIUNIU_INVAILD_POS;
		m_isFirstFlag = true;
		m_playMode = 4;          //Ĭ�ϰ�����ע
		m_qiangZhuangTimes = 1;  //Ĭ����ׯ����1��
		m_playerNum = 6;
	}
};

/* ţţÿ�����ʼ�����ֶΣ��ṹ�壩*/
struct NiuNiuMatchState__cxx_part
{
	NGameLogic     m_gamelogic;     // ��Ϸ�߼�
	PlayType       m_playtype;	    // �����淨Сѡ��

									/* ���ÿ���ṹ�� */
	void clear_match()
	{
		this->~NiuNiuMatchState__cxx_part();
		new (this) NiuNiuMatchState__cxx_part;
	}
};

/*
*  ţţÿ������Ҫ�������ֶΣ��ṹ�壩
*  �̳� ��NiuNiuRoundState__c_pard, NiuNiuRoundState_cxx_part
*/
struct NiuNiuRoundState : NiuNiuRoundState__c_part, NiuNiuRoundState__cxx_part
{
	void clear_round()
	{
		NiuNiuRoundState__c_part::clear_round();
		NiuNiuRoundState__cxx_part::clear_round();
	}
};

/*
*  ţţÿ������Ҫ�������ֶΣ��ṹ�壩
*  �̳У�NiuNiuMatchState__c_pard, NiuNiuMatchState_cxx_pard
*/
struct NiuNiuMatchState : NiuNiuMatchState__c_part, NiuNiuMatchState__cxx_part
{
	void clear_match()
	{
		NiuNiuMatchState__c_part::clear_match();
		NiuNiuMatchState__cxx_part::clear_match();
	}
};

/*
*  ţţ����״̬���ṹ�壩
*  �̳У�NiuNiuRoundState, NiuNiuMatchState
*/
struct NiuNiuDeskState : NiuNiuRoundState, NiuNiuMatchState
{

	/* ����ÿ������ */
	void clear_round()
	{
		NiuNiuRoundState::clear_round();
	}

	/* ����ÿ������*/
	void clear_match(int player_count)
	{
		NiuNiuMatchState::clear_match();
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
		if (play_status >= GAME_PLAY_SELECT_ZHUANG && play_status <= GAME_PLAY_END)
			m_play_status = play_status;
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
		if (pos >= NIUNIU_PLAY_USER_COUNT)
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
		for (int i = 0; (i<m_player_count) && (m_user_status[nextPos] != 1); i++)
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
		return 0 <= pos && pos < NIUNIU_INVAILD_POS;
	}
};

/* ţţ¼����(�ṹ��) */
struct NiuNiuVideoSupport : NiuNiuDeskState {
	VideoLog		m_video;

	void VideoSave()
	{
		if (NULL == m_desk) return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		m_video.m_playType = m_desk->getPlayType();
		//m_video.m_flag=m_registered_game_type;
		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		gWork.SendMsgToDb(video);
	}
};

/* ţţ��Ϸ���崦���߼����ṹ�壩*/
struct NiuNiuGameCore : GameHandler, NiuNiuVideoSupport
{
	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
	}
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
	void notify_zhuang(Lint select_zhuang_pos, Lint score_times = 1)
	{
		if (select_zhuang_pos < 0 || select_zhuang_pos >NIUNIU_PLAY_USER_COUNT)
		{
			LLOG_ERROR("desk_id=[%d]send_zhuang_cmd pos error!!!", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}
		if (select_zhuang_pos >= 0 &&
			select_zhuang_pos < NIUNIU_PLAY_USER_COUNT &&
			m_user_status[select_zhuang_pos] == 1)
		{
			m_zhuangPos = select_zhuang_pos;
			NiuNiuS2CStartGame send_start;
			send_start.m_zhuangPos = select_zhuang_pos;
			send_start.m_scoreTimes = score_times;
			//��ע����ҿ���ע״̬
			memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
			memcpy(send_start.m_playerStatus, m_user_status, sizeof(send_start.m_playerStatus));
			memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(send_start.m_qingScore));
			notify_desk(send_start);

			//�㲥��ս��ң���Ϸ��ʼ
			notify_lookon(send_start);
		}
	}

	///////////////////////////////////////////////////////////////////////////////

	/* ������������ж�������������Чλ�ã�*/
	Lint calc_vaild_player_count()
	{
		Lint tmp_player_count = 0;
		for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			if (m_user_status[i])
			{
				++tmp_player_count;
			}
		}
		return tmp_player_count;
	}

	/* ȷ����С����ע���� */
	Lint min_add_score()
	{
		//������ׯ && ɨ��ģʽ
		if (0 == m_selectZhuangMode && 2 == m_playMode)
		{
			switch (m_addScoreTimes)
			{
				//���ʣ�С����1 2 3��
			case 0:
			{
				return 1;
				break;
			}
			//���ʣ��󱶣�4 5 6��
			case 2:
			{
				return 4;
				break;
			}
			default:
			{
				return 1;
				break;
			}
			}
		}

		//��ͨģʽ1 �� ɨ��ģʽ
		else if (m_playMode == 0 || m_playMode == 2) return 1;

		//��ͨģʽ2 �� ������ע �� ������ע
		else if (m_playMode == 1 || m_playMode == 3 || m_playMode == 4)
		{
			//����
			switch (m_addScoreTimes)
			{
				//���ʣ�С����2��3��4��5��
			case 0:
			{
				return 2;
				break;
			}
			//���ʣ��б���6��9��12��15��
			case 1:
			{
				return 6;
				break;
			}
			//���ʣ��󱶣�5��10��20��30��
			case 2:
			{
				return 5;
				break;
			}
			default:
				return 2;
				break;
			}
		}

		else
		{
			return 1;
		}
	}

	/* �жϿ���ѡׯ�淨���Ƿ���������Ѿ���ѡׯ������� */
	bool is_select_zhuang_over()
	{
		Lint userSelectZhuang = 0;
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
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

	/* �ж��Ƿ�������Ҷ�����ע�� */
	bool is_add_score_over()
	{
		Lint userSelectScore = 0;
		for (int i = 0; i < m_player_count; i++)
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

	///////////////////////////////////////////////////////////////////////////////////

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
		if (pos >= 0 && pos < m_player_count)
		{
			//����
			if (show_card)
			{
				for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
				{
					if (m_user_status[i] != 1) continue;

					NiuNiuS2COpenCard send_card;
					send_card.m_pos = pos;
					//��Ϣ���Ϊ����
					send_card.m_show = 1;
					//����ÿ����ҵ�״̬
					memcpy(send_card.m_playerStatus, m_user_status, sizeof(m_user_status));
					if (i == pos)
					{
						//����ţţ����
						BYTE tmp_player_hand_card[NIUNIU_HAND_CARD_MAX];
						memcpy(tmp_player_hand_card, m_player_hand_card[pos], sizeof(m_player_hand_card[pos]));
						m_player_oxnum[pos] = m_gamelogic.GetCardType(tmp_player_hand_card, NIUNIU_HAND_CARD_MAX, m_isSpacePlay);
						send_card.m_oxNum = m_player_oxnum[pos];

						//������λ���Ǹող�����λ�ã�������Ƹ��Ƹ������
						for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
						{
							send_card.m_playerHandCard[j] = (Lint)m_player_hand_card[pos][j];
						}
					}
					//�Ǹ���Ҳ����ģ�����ϸ��Ϣ��������ң����Ǹ���ҵ���ֻ֪ͨ��ҿ���״̬��
					notify_user(send_card, i);
				}

				//�㲥��ս��ң���ҿ���
				NiuNiuS2COpenCard gz_opCard;
				gz_opCard.m_pos = pos;
				gz_opCard.m_show = 1;
				memcpy(gz_opCard.m_playerStatus, m_user_status, sizeof(m_user_status));
				notify_lookon(gz_opCard);
			}
			//����
			else
			{
				NiuNiuS2COpenCard send_card;
				send_card.m_pos = pos;
				//��Ϣ���Ϊ����
				send_card.m_show = 0;
				//����ÿ����ҵ�״̬
				memcpy(send_card.m_playerStatus, m_user_status, sizeof(m_user_status));
				//������ҷ�����
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX; i++)
				{
					send_card.m_playerHandCard[i] = m_player_hand_card[pos][i];
				}
				//����֪ͨ���
				notify_user(send_card, pos);

				//�㲥��ս��ң���ҷ���
				NiuNiuS2COpenCard gz_sendCard;
				gz_sendCard.m_pos = pos;
				gz_sendCard.m_show = 0;
				memcpy(gz_sendCard.m_playerStatus, m_user_status, sizeof(m_user_status));
				notify_lookon(gz_sendCard);
			}
			return true;
		}
		return false;
	}

	void add_round_log(Lint* accum_score, Lint win_pos)
	{
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLogForQiPai(m_desk->m_user, accum_score, m_user_status, win_pos, m_video.m_Id);
		}
	}

	//С����
	void finish_round(BOOL jiesan = false)
	{
		LLOG_DEBUG("NiuNiuHandler::finish_round() Run... This round is finished...deskId=[%d], round=[%d/%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit);

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
		CopyMemory(tmp_player_hand_card, m_player_hand_card, sizeof(m_player_hand_card));

		//ׯ�ұ���
		tmp_win_times[m_zhuangPos] = 1;
		if (true == m_cbOxCard[m_zhuangPos])
		{
			tmp_win_times[m_zhuangPos] = m_gamelogic.GetTimes(tmp_player_hand_card[m_zhuangPos], NIUNIU_HAND_CARD_MAX, m_isSpacePlay, m_scoreTimes);
		}
		LLOG_DEBUG("desk_id = [%d], *****ceshi***** tmp_win_times[m_zhuangPos] = %d", m_desk ? m_desk->GetDeskId() : 0, tmp_win_times[m_zhuangPos]);

		//�Ա����
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (i == m_zhuangPos || m_user_status[i] == 0) continue;

			if (m_gamelogic.CompareCard(tmp_player_hand_card[i], tmp_player_hand_card[m_zhuangPos], NIUNIU_HAND_CARD_MAX, m_cbOxCard[i], m_cbOxCard[m_zhuangPos], m_isSpacePlay))
			{
				++tmp_win_count[i];
				//��ȡ����
				tmp_win_times[i] = 1;
				if (m_cbOxCard[i] == true)
				{
					tmp_win_times[i] = m_gamelogic.GetTimes(tmp_player_hand_card[i], NIUNIU_HAND_CARD_MAX, m_isSpacePlay, m_scoreTimes);
				}
				LLOG_DEBUG("desk_id = [%d], *****ceshi***** tmp_win_times[%d] = %d", m_desk ? m_desk->GetDeskId() : 0, i, tmp_win_times[i]);
			}
			else
			{
				++tmp_win_count[m_zhuangPos];
			}
		}

		//��ȡţţ
		/*for (Lint i = 0; i < m_player_count; i++)
		{
		if (m_user_status[i] == 0) continue;
		tmp_win_oxnum[i] = m_gamelogic.GetCardType(tmp_player_hand_card[i], NIUNIU_HAND_CARD_MAX, m_isSpaceMode);
		}*/

		//ͳ����ҵ÷�
		for (Lint i = 0; i < m_player_count; i++)
		{
			if (i == m_zhuangPos || m_user_status[i] == 0) continue;

			if (tmp_win_count[i] > 0)
			{
				//�м�Ӯ
				m_player_score[i] = m_play_add_score[m_zhuangPos] * m_play_add_score[i] * tmp_win_times[i];

				//��ע���м�Ӯ�����Ͼ�û����ע���򱾾ֿ�����ע��**Ren 2018-5-19��
				m_canTuiZhu[i] = !m_canTuiZhu[i];

				LLOG_ERROR("desk_id = [%d] *****ceshi,Xian_Win *****,m_play_add_score[m_zhuangPos] = %d, m_play_add_score[%d] = %d, tmp_win_times[%d] = %d",
					m_desk ? m_desk->GetDeskId() : 0, m_play_add_score[m_zhuangPos], i, m_play_add_score[i], i, tmp_win_times[i]);
			}
			else
			{
				//ׯ��Ӯ
				m_player_score[i] = (-1) * m_play_add_score[m_zhuangPos] * m_play_add_score[i] * tmp_win_times[m_zhuangPos];

				//��ע��ׯ��Ӯ�ˣ��м��¾ֿ϶�û����עȨ��
				m_canTuiZhu[i] = false;

				LLOG_ERROR("desk_id = [%d] *****ceshi,Zhuang_Win *****,m_play_add_score[m_zhuangPos] = %d, m_play_add_score[%d] = %d, tmp_win_times[m_zhuangPos] = %d",
					m_desk ? m_desk->GetDeskId() : 0, m_play_add_score[m_zhuangPos], i, m_play_add_score[i], tmp_win_times[m_zhuangPos]);
			}
			m_player_score[m_zhuangPos] -= m_player_score[i];


		}
		//�ҳ�����Ӯ��
		Lint wWinner = m_zhuangPos;
		Lint maxScore = m_player_score[m_zhuangPos];
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			if (m_player_score[i] > maxScore)
			{
				maxScore = m_player_score[i];
				wWinner = i;
			}
		}

		//�����ͻ���С����
		NiuNiuS2CResult send_result;
		//һ�ֽ���д��־
		add_round_log(m_player_score, wWinner);

		if (m_desk && m_desk->m_vip)
		{
			memcpy(send_result.m_totleScore, m_desk->m_vip->m_score, sizeof(send_result.m_totleScore));
			LLOG_DEBUG("desk_id=[%d], send.m_TotalScore[%d %d %d %d %d %d]", m_desk ? m_desk->GetDeskId() : 0, send_result.m_totleScore[0],
				send_result.m_totleScore[1], send_result.m_totleScore[2],
				send_result.m_totleScore[3], send_result.m_totleScore[4],
				send_result.m_totleScore[5]);
		}
		else
		{
			LLOG_ERROR("m_desk or  m_desk->m_vip is null ");
		}
		if (true == jiesan)
		{
			set_desk_state(DESK_WAIT);
			LLOG_ERROR("NiuNiuGameHandler::finish_round() ... This has is dissolved... desk_id=[%d], desk_status=[%d], jie_san game over",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status);

			//������Ϸ
			if (m_desk) m_desk->HanderGameOver(1);
			return;
		}

		send_result.m_zhuangPos = m_zhuangPos;
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			if (m_user_status[i] == 0)
			{
				continue;
			}
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX; j++)
			{
				send_result.m_playerHandCard[i][j] = (Lint)m_player_hand_card[i][j];
			}
		}
		memcpy(send_result.m_playScore, m_player_score, sizeof(m_player_score));
		memcpy(send_result.m_oxNum, m_player_oxnum, sizeof(m_player_oxnum));
		memcpy(send_result.m_oxTimes, tmp_win_times, sizeof(tmp_win_times));

		LLOG_DEBUG("desk_id=[%d], GAME_END m_ZhuangPos=[%d]", m_desk ? m_desk->GetDeskId() : 0, send_result.m_zhuangPos);
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			LLOG_DEBUG("GAME_END playerPos = %d", i);
			LLOG_ERROR("DeskId=[%d],GAME_END player%dHandCard = [%d,%d,%d,%d,%d]", m_desk ? m_desk->GetDeskId() : 0, i, m_player_hand_card[i][0], m_player_hand_card[i][1], m_player_hand_card[i][2], m_player_hand_card[i][3], m_player_hand_card[i][4]);
			LLOG_DEBUG("GAME_END m_Score[%d]", m_accum_score[i]);
		}

		notify_desk(send_result);

		//�㲥��ս��ң�С����
		notify_lookon(send_result);

		set_desk_state(DESK_WAIT);
		//if (m_desk) m_desk->setDynamicToPlay();
		LLOG_ERROR("desk_id=[%d], desk_status=[%d], round_finish game over", m_desk ? m_desk->GetDeskId() : 0, m_play_status);
		//������Ϸ
		if (m_desk) m_desk->HanderGameOver(1);

	}

	///////////////////////////////////////////////////////////////////////////////

	/***************************************************
	* ��������    on_event_user_add_score()
	* ������      ���������ע
	* ������
	*  @ pos ����������ϵ�λ��
	*  @ score �����ע����
	* ���أ�      (bool)
	****************************************************/
	bool on_event_user_add_score(Lint pos, Lint score)
	{
		LLOG_ERROR("desk_id=[%d], on_event_user_add_score,pos[%d],score[%d]", m_desk ? m_desk->GetDeskId() : 0, pos, score);
		if (0 == score)
		{
			LLOG_DEBUG("desk_id=[%d], on_event_user_add_score error !!!!  score==0 error pos[%d]", m_desk ? m_desk->GetDeskId() : 0, pos);
			return false;
		}
		m_play_add_score[pos] = score;

		//�����ע��ɺ�㲥��ע����
		NiuNiuS2CAddScore score_notify;
		score_notify.m_pos = pos;
		score_notify.m_score = score;
		notify_desk(score_notify);

		//�㲥��ս��ң������ע
		notify_lookon(score_notify);

		if (is_add_score_over())
		{
			//������ע��ɣ��޸���Ϸ״̬Ϊ����״̬
			set_play_status(GAME_PLAY_SEND_CARD);

			//��ӡ��־��
			LLOG_ERROR("desk_id=[%d], deskStatus=[%d],clent add score over", m_desk ? m_desk->GetDeskId() : 0, m_play_status);

			//��������ע��ɣ���ÿ����ҷ���
			for (Lint i = 0; i < m_player_count; i++)
			{
				if (m_user_status[i] != 1)continue;
				send_user_card(i, false);
			}

			m_play_status_time.Now();
			m_tick_flag = true;
		}
		return true;
	}


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
		LLOG_DEBUG("NiuNiuGameHandler::on_event_user_select_zhuang() Run... Player Qiang Zhuang... deskId=[%d], pos=[%d], qiangScore=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos, zhuang_score);

		//��¼�������ׯ����; -1����Чλ�ã�0������ׯ��>0����ׯ����
		m_play_qiang_zhuang[pos] = zhuang_score;

		// ֪ͨ��ׯ��� 
		NiuNiuS2CQiangZhuangNotify   send;
		send.m_pos = pos;
		send.m_qiangZhuangScore = zhuang_score;
		notify_desk(send);

		//�㲥��ս���
		notify_lookon(send);

		if (is_select_zhuang_over())
		{
			LLOG_DEBUG("NiuNiuGameHandler::on_event_user_select_zhuang() Run... All Player Has Qiang ZhuangOver, Zhen Go to Next Status Add Score... deskId=[%d]",
				m_desk ? m_desk->GetDeskId() : 0);

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
				m_play_qiang_zhuang[m_zhuangPos] = 1;
			}

			//��¼ѡׯ����
			m_play_add_score[m_zhuangPos] = m_play_qiang_zhuang[m_zhuangPos];

			//��ע��ׯ��ȷ���ˣ�ׯ��û����ע��Ȩ��(**Ren 2018-5-19)
			m_canTuiZhu[m_zhuangPos] = false;
			memset(m_player_tui_score[m_zhuangPos], 0, sizeof(Lint) * 4);
			//m_player_tui_score[m_zhuangPos][0] = 0;

			//ȷ��ׯ�Һ�㲥ѡׯ���
			notify_zhuang(m_zhuangPos, m_play_qiang_zhuang[m_zhuangPos]);

			//ѡׯ����������Ϸ״̬����Ϊ��ע
			set_play_status(GAME_PLAY_ADD_SCORE);

			//��Ϸ��ʼ����ע����ʱ
			m_play_status_time.Now();
			m_tick_flag = true;

			return true;
		}
		return false;
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
		LLOG_DEBUG("NiuNiuGameHandler::on_event_user_open_card() Run... Player Open Card... deskId=[%d], pos=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, pos);

		//��Ǹ��û�Ϊ�ѿ���
		m_isOpenCard[pos] = true;

		//�㲥���û����ƽ��
		send_user_card(pos, true);

		//������������
		int OxNum = m_gamelogic.GetCardType(m_player_hand_card[pos], NIUNIU_HAND_CARD_MAX, m_isSpacePlay);
		if (OxNum < 0) OxNum = 0;
		m_cbOxCard[pos] = OxNum == 0 ? false : true;

		if (is_open_card_over())
		{
			//�㲥ÿ��������
			//����
			finish_round();
		}
		return true;
	}

	/* ��ׯ */
	void lun_zhuang_pos()
	{
		//��ׯ�淨�£���һ����Ҫ��ׯ��
		if (m_round_offset == 0)
		{
			/*if (m_desk)
			{
			m_zhuangPos = m_desk->MHSpecPersonPos();
			}*/
			for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
			{
				if (m_user_status[i] == 0)
				{
					continue;
				}
				NiuNiuS2CSelectZhuang qiang_zhuang;
				qiang_zhuang.m_pos = i;         //���λ��
				qiang_zhuang.m_qingZhuang = 2;  //����Ϊ��ׯ��һ����ׯ���ͻ���ֻ��ʾ��0-������1��ׯ
												//��ÿ����ҵ�����д�뿴��ѡׯ����Ϣ��,ÿ�������ֻ��4����
				for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1 && PLAY_MODE_MINGSCORE == m_playMode; j++)
				{
					qiang_zhuang.m_playerHandCard[j] = m_player_hand_card[i][j];
				}
				memcpy(qiang_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
				notify_user(qiang_zhuang, i);
			}

			//�㲥��ս���
			NiuNiuS2CSelectZhuang gz_zhuang;
			gz_zhuang.m_pos = m_player_count;
			gz_zhuang.m_qingZhuang = 2;
			memcpy(gz_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
			notify_lookon(gz_zhuang);

			//��ʱ����ʼ��ʱ
			m_play_status_time.Now();
			m_tick_flag = true;

			return;
		}
		else
		{
			m_zhuangPos = GetNextPos(m_zhuangPos);

			//��ע��ׯ��ȷ���ˣ�ׯ��û����ע��Ȩ��(**Ren 2018-5-19)
			m_canTuiZhu[m_zhuangPos] = false;
			memset(m_player_tui_score[m_zhuangPos], 0, sizeof(Lint) * 4);

			//������עģʽ
			if (PLAY_MODE_MINGSCORE == m_playMode)
			{
				for (Lint pos = 0; pos < NIUNIU_PLAY_USER_COUNT; pos++)
				{
					/*if (0 == m_user_status[pos])
					{
					continue;
					}
					NiuNiuS2CMingPaiAddScore send;
					send.m_pos = pos;
					for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
					{
					send.m_playerHandCard[j] = (Lint)m_player_hand_card[pos][j];
					}
					notify_user(send, pos);*/

					if (0 == m_user_status[pos])
					{
						continue;
					}


					NiuNiuS2CStartGame send_start;
					send_start.m_zhuangPos = m_zhuangPos;
					send_start.m_scoreTimes = 1;  //����ׯģʽ��Ĭ�ϱ���Ϊһ��
												  //��ע����ҿ���ע״̬
					memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
					memcpy(send_start.m_playerStatus, m_user_status, sizeof(send_start.m_playerStatus));
					for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
					{
						send_start.m_playerHandCard[j] = (Lint)m_player_hand_card[pos][j];
					}
					memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
					notify_user(send_start, pos);
				}
				//ׯ��Ĭ����ע����
				m_play_add_score[m_zhuangPos] = 1;

				//�㲥��ս���
				NiuNiuS2CStartGame gz_start;
				gz_start.m_zhuangPos = m_zhuangPos;
				gz_start.m_scoreTimes = 1;  //����ׯģʽ��Ĭ�ϱ���Ϊһ��
											//��ע����ҿ���ע״̬
				memcpy(gz_start.m_playerTuiScore, m_player_tui_score, sizeof(gz_start.m_playerTuiScore));
				memcpy(gz_start.m_playerStatus, m_user_status, sizeof(gz_start.m_playerStatus));
				memcpy(gz_start.m_qingScore, m_play_qiang_zhuang, sizeof(gz_start.m_qingScore));
				notify_lookon(gz_start);

			}
			//������ע
			else
			{
				NiuNiuS2CStartGame send_start;
				send_start.m_zhuangPos = m_zhuangPos;
				send_start.m_scoreTimes = 1;  //����ׯģʽ��Ĭ�ϱ���Ϊһ��
											  //��ע����ҿ���ע״̬
				memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
				memcpy(send_start.m_playerStatus, m_user_status, sizeof(m_user_status));
				memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
				m_play_add_score[m_zhuangPos] = 1;
				notify_desk(send_start);

				//�㲥��ս���
				notify_lookon(send_start);
			}
		}

		//ѡׯ������������Ϸ״̬Ϊ��ע״̬
		set_play_status(GAME_PLAY_ADD_SCORE);

		//ѡׯ������͸��ͻ��ˣ���Ϸ��ʼ��Ϣ
		//NiuNiuS2CStartGame send_start;
		//send_start.m_zhuangPos = m_zhuangPos;
		//send_start.m_scoreTimes = 1;  //����ׯģʽ��Ĭ�ϱ���Ϊһ��
		//��ע����ҿ���ע״̬
		//memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(m_player_tui_score));
		//memcpy(send_start.m_playerStatus, m_user_status, sizeof(m_user_status));
		//m_play_add_score[m_zhuangPos] = 1;
		//notify_desk(send_start);

		//��ʱ����ʼ��ʱ
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* ���ׯ */
	void rand_zhuang_pos()
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

		//��¼ׯ��λ��
		m_zhuangPos = tmp_rand_zhuang_pos;

		//��ע��ׯ��ȷ���ˣ�ׯ��û����ע��Ȩ��(**Ren 2018-5-19)
		m_canTuiZhu[m_zhuangPos] = false;
		memset(m_player_tui_score[m_zhuangPos], 0, sizeof(Lint) * 4);

		//ѡׯ������������Ϸ״̬Ϊ��ע״̬
		set_play_status(GAME_PLAY_ADD_SCORE);

		//ׯ�ұ����·�ȷ��
		m_play_add_score[m_zhuangPos] = 1;

		//ѡׯ������͸��ͻ��ˣ���Ϸ��ʼ��Ϣ
		NiuNiuS2CStartGame send_start;
		send_start.m_zhuangPos = m_zhuangPos;
		send_start.m_scoreTimes = 1;  //�ǿ���ѡׯĬ��Ϊһ��
									  //��ע����ҿ���ע״̬
		memcpy(send_start.m_playerTuiScore, m_player_tui_score, sizeof(send_start.m_playerTuiScore));
		memcpy(send_start.m_playerStatus, m_user_status, sizeof(send_start.m_playerStatus));
		memcpy(send_start.m_qingScore, m_play_qiang_zhuang, sizeof(send_start.m_qingScore));
		notify_desk(send_start);

		//��ʱ����ʼ��ʱ
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* ������ׯ */
	void qiang_zhuang_pos()
	{
		LLOG_DEBUG("NiuNiuGameHandler::qiang_zhuang_pos() Run... This mode is QiangZhuang... deskId=[%d]", m_desk ? 0 : m_desk->GetDeskId());
		for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
		{
			if (m_user_status[i] == 0)
			{
				continue;
			}
			//62072:֪ͨ�ͻ�����ׯ
			NiuNiuS2CSelectZhuang qiang_zhuang;
			qiang_zhuang.m_pos = i;         //���λ��
			qiang_zhuang.m_qingZhuang = 1;  //����Ϊ��ׯģʽ
											//��ÿ����ҵ�����д�뿴��ѡׯ����Ϣ��,ÿ�������ֻ��4����
			for (Lint j = 0; j < NIUNIU_HAND_CARD_MAX - 1; j++)
			{
				qiang_zhuang.m_playerHandCard[j] = m_player_hand_card[i][j];
			}
			memcpy(qiang_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
			notify_user(qiang_zhuang, i);
		}

		//�㲥��ս���
		NiuNiuS2CSelectZhuang gz_zhuang;
		gz_zhuang.m_pos = m_player_count;
		gz_zhuang.m_qingZhuang = 1;
		memcpy(gz_zhuang.m_playerStatus, m_user_status, sizeof(m_user_status));
		notify_lookon(gz_zhuang);

		LLOG_DEBUG("NiuNiuGameHandler::qiang_zhuang_pos() Run... Has Notify Client Start Qiang Zhuang... deskId=[%d]", m_desk ? 0 : m_desk->GetDeskId());

		//��ʱ����ʼ��ʱ
		m_play_status_time.Now();
		m_tick_flag = true;
	}

	/* ѡׯ */
	void select_zhuang()
	{
		switch (m_playtype.NiuNiuGetZhuangMode())
		{

			//��ׯ
		case ZHUANG_MODE_LUN:
		{
			lun_zhuang_pos();
			break;
		}

		//���ׯ
		/*case ZHUANG_MODE_RUND:
		{
		rand_zhuang_pos();
		break;
		}*/

		//������ׯ
		case ZHUANG_MODE_QIANG:
		{
			//֪ͨ�����ע��ׯ
			qiang_zhuang_pos();
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
		LLOG_DEBUG("NiuNiuGameHandler::start_game() Run... Game Start... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);

		//�����ļ����Ƿ������������
		bool can_assign = gConfig.GetDebugModel();
		//�������
		//if (can_assign)
		if (false)
		{
			std::vector<BYTE> handCard;
			handCard.resize(5);

			for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
			{
				if (m_user_status[i] != 1) continue;
				handCard.clear();
				LoadPlayerCards(handCard, i + 1);
				LLOG_DEBUG("player%dcards=[%2x,%2x,%2x,%2x,%2x]", i + 1, handCard[0], handCard[1], handCard[2], handCard[3], handCard[4]);
				//memcpy(m_player_hand_card[i], &handCard, sizeof(handCard));
				for (int j = 0; j < NIUNIU_HAND_CARD_MAX; ++j)
				{
					m_player_hand_card[i][j] = handCard[j];
				}
			}
		}
		//���������
		else
		{
			//ϴ��
			m_gamelogic.RandCardList(m_player_hand_card[0], sizeof(m_player_hand_card) / sizeof(m_player_hand_card[0][0]));
		}

		//ÿ��������ҿ�����ע�ķ���
		int t_tuiScore = m_tuiZhuTimes * min_add_score();
		LLOG_DEBUG("Start Game NiuNiu TuiZhu t_tuiScore=[%d]", t_tuiScore);
		for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; ++i)
		{
			if (m_canTuiZhu[i])
			{
				m_player_tui_score[i][0] = t_tuiScore;
			}
			LLOG_DEBUG("Start Game NiuNiu m_plauer_tui_score[%d]=[%d | %d]", i, m_player_tui_score[i][0]);
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
		LLOG_DEBUG("NiuNiuGameHandler::start_round() Run... Game Start Round........................... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);
		clear_round();
		notify_desk_match_state();

		set_desk_state(DESK_PLAY);
		memcpy(m_user_status, player_status, sizeof(m_user_status));

		if (m_desk && m_desk->m_vip&&m_desk->m_clubInfo.m_clubId != 0)
		{
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle + 1, m_desk->m_vip->m_maxCircle);
		}
		LLOG_ERROR("NiuNiuGameHandler::start_round()... Game Round Info... deskId=[%d], startRound=[%d/%d],  playerCount=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_round_offset, m_round_limit, m_player_count);

		LLOG_DEBUG("desk_id=[%d], NiuNiuGetZhuangMode=[%d], NiuNiuGetPlayMode=[%d], NiuNiuGetSpecMode=[%d], NiuNiuGetAutoMode=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_selectZhuangMode, m_playMode, m_isSpacePlay, m_isAutoPlay);

		//��Ϸ��ʼ���
		start_game();
	}
};


struct NiuNiuGameHandler : NiuNiuGameCore
{

	/* ���캯�� */
	NiuNiuGameHandler()
	{
		LLOG_DEBUG("NiuNiuGameHandler Init...");
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

	/* �㲥����ս��� */
	void notify_lookon(LMsg &msg) override
	{
		if (NULL == m_desk) return;
		m_desk->MHBoadCastDeskLookonUser(msg);
	}

	/* */
	bool startup(Desk *desk) {
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

		//Сѡ�ׯ�ҹ���
		m_selectZhuangMode = m_playtype.NiuNiuGetZhuangMode();

		//Сѡ��淨ģʽ
		m_playMode = m_playtype.NiuNiuGetPlayMode();

		//Сѡ������淨
		m_isSpacePlay = m_playtype.NiuNiuGetSpecPlay();

		//Сѡ��Ƿ������й�
		m_isAutoPlay = m_playtype.NiuNiuGetAutoPlay();

		//Сѡ���̬����
		m_isDynamicIn = m_playtype.NiuNiuGetDynamicIn();

		//Сѡ�6�˷� �� 10�˷�
		m_playerNum = m_playtype.NiuNiuGetPlayerNum();

		//Сѡ���ׯ����
		m_qiangZhuangTimes = m_playtype.NiuNiuQiangZhuangTimes();

		//Сѡ���ע����
		m_addScoreTimes = m_playtype.NiuNiuAddScoreTimes();

		//Сѡ���ע����
		m_tuiZhuTimes = m_playtype.NiuNiuTuiZhu();

		// Сѡ�������ֱ��� 0����ͨ����  1��ɨ��ģʽ����  2������ţţX4����
		m_scoreTimes = 0;

		//Сѡ����α���ţţX4
		int t_scoreTimes = m_playtype.NiuNiuTypeTimes();


		if (PLAY_MODE_BOMB == m_playMode)
		{
			m_scoreTimes = 1;    //��Ӧɨ��ģʽ
		}
		else
		{
			if (t_scoreTimes == 1)
			{
				m_scoreTimes = 2;   //��������
			}
			else
			{
				m_scoreTimes = 0;   //��ͨ����
			}
		}

		LLOG_ERROR("NiuNiuGameHandler::SetPlayType() Run... This Desk Play Type List ... deskIid=[%d], ׯ�ҹ���%d���淨ģʽ��%d�������淨��%d���Ƿ��йܣ�%d����̬���룺%d��������%d����ׯ���ʣ�%d����ע���ʣ�%d, TuiZhu=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_selectZhuangMode, m_playMode, m_isSpacePlay, m_isAutoPlay, m_isDynamicIn, m_playerNum, m_qiangZhuangTimes, m_addScoreTimes, m_tuiZhuTimes);

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
		LLOG_DEBUG("desk_id=[%d], NiuNiu---------------------MHSetDeskPlay(Lint play_user_count) = [%d]",
			m_desk ? m_desk->GetDeskId() : 0, play_user_count);

		if (!m_desk || !m_desk->m_vip)
		{
			LLOG_ERROR("MHSetDeskPlay  error !!!! m_desk or  m_desk->m_vip  null");
			return;
		}

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
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuSelsectZhuang error, user[%d] , status not GAME_PLAY_SELECT_ZHUANG, [%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);
			return false;
		}
		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuSelsectZhuang  user[%d] pos not ok [%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));
			return false;
		}
		//ֻ�п�����ׯ����淨�Ż������
		return on_event_user_select_zhuang(GetUserPos(pUser), msg->m_qingScore);
	}

	/* ����ţţ�����ע */
	bool HanderNiuNiuAddScore(User* pUser, NiuNiuC2SAddScore* msg)
	{
		if (NULL == pUser || NULL == msg || NULL == m_desk)
		{
			return false;
		}

		if (GAME_PLAY_ADD_SCORE != m_play_status)
		{
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuAddScore error, user[%d] , status not GAME_PLAY_ADD_SCORE, [%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);

			return false;
		}
		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuAddScore  user[%d] pos not ok [%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

			return false;
		}
		return on_event_user_add_score(GetUserPos(pUser), msg->m_score);

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
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuOpenCard error, user[%d] , status not GAME_PLAY_SEND_CARD, [%d]",
				m_desk ? m_desk->GetDeskId() : 0, pUser->GetUserDataId(), m_play_status);

			return false;
		}

		if (GetUserPos(pUser) == NIUNIU_INVAILD_POS)
		{
			LLOG_ERROR("desk_id=[%d], HanderNiuNiuOpenCard  user[%d] pos not ok [%d] ",
				m_desk ? m_desk->GetDeskId() : 0, pUser->m_userData.m_id, GetUserPos(pUser));

			return false;
		}

		return on_event_user_open_card(GetUserPos(pUser));
	}

	/* ������Ҷ������� */
	void OnUserReconnect(User* pUser)
	{
		if (NULL == pUser || NULL == m_desk)
		{
			LLOG_ERROR("NiuNiuGameHandler::OnUserReconnect() Error... The user or desk is NULL...");
			return;
		}

		LLOG_DEBUG("NiuNiuGameHandle::OnUserReconnect() Run... deskId=[%d], reconnUserId=[%d], curPos=[%d], playStatus=[%d]",
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


		//��ȡׯ�ҹ���
		Lint tmp_zhuangMode = m_playtype.NiuNiuGetZhuangMode();
		//��ȡ�淨ģʽ
		Lint tmp_playMode = m_playtype.NiuNiuGetPlayMode();

		Lint play_delay_time = 0;
		NiuNiuS2CRecon reconn;
		reconn.m_playStatus = m_play_status;
		memcpy(reconn.m_playerTuiScore, m_player_tui_score, sizeof(reconn.m_playerTuiScore));
		memcpy(reconn.m_user_status, this->m_user_status, sizeof(reconn.m_user_status));
		memcpy(reconn.m_addZhuang, m_play_qiang_zhuang, sizeof(reconn.m_addZhuang));

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
			LLOG_DEBUG("NiuNiuGameHandler::OnUserReconnect()  1 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
		}

		switch (m_play_status)
		{

			//ѡׯ�׶ζ���
		case GAME_PLAY_SELECT_ZHUANG:
		{
			//LLOG_DEBUG("****��������desk_id=[%d], OnUserReconnect:GAME_PLAY_SELECT_ZHUANG", m_desk ? m_desk->GetDeskId() : 0);
			reconn.m_hasSelectZhuang = 0;

			if (ZHUANG_MODE_LUN == tmp_zhuangMode && 0 != m_round_offset)
			{
				//��ׯģʽ���ҷǵ�һ��
				reconn.m_hasSelectZhuang = 1;
				reconn.m_zhuangPos = m_zhuangPos;
			}
			else if (ZHUANG_MODE_QIANG == tmp_zhuangMode || (ZHUANG_MODE_LUN == tmp_zhuangMode && 0 == m_round_offset))
			{
				//��ׯģʽ���� ��ׯģʽ��һ����ׯ
				//���û���δ����������ׯ
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
				play_delay_time = DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU;
			}

			//switch (m_playtype.NiuNiuGetZhuangMode())
			//{
			////��ׯ �� ���ׯ �ǲ���Ҫ���ѡׯ��
			//case ZHUANG_MODE_LUN:
			////case ZHUANG_MODE_RUND:
			//{
			//	reconn.m_hasSelectZhuang = 1;
			//	reconn.m_zhuangPos = m_zhuangPos;
			//	break;
			//}

			////������ׯ����Ҫ�������ׯ��
			//case ZHUANG_MODE_QIANG:
			//{
			//	//���û���δ����������ׯ
			//	reconn.m_hasSelectZhuang = 0;
			//	if (m_play_qiang_zhuang[pos] >= 0)
			//	{
			//		//���û���ׯѺע�׶��Ѿ���������
			//		reconn.m_hasSelectZhuang = 1;
			//	}
			//	for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1; i++)
			//	{
			//		reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
			//	}
			//	memcpy(reconn.m_addZhuang, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
			//	play_delay_time = DELAY_TIME_SELECT_QIANG_ZHUANG;
			//	break;
			//}

			//default:
			//	break;
			//}
			break;
		}
		//��ע�׶ζ���
		case GAME_PLAY_ADD_SCORE:
		{
			reconn.m_zhuangPos = m_zhuangPos;
			memcpy(reconn.m_addScore, m_play_add_score, sizeof(m_play_add_score));
			memcpy(reconn.m_addZhuang, m_play_qiang_zhuang, sizeof(m_play_qiang_zhuang));
			for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
			{
				if (m_play_add_score[i] != 0)
				{
					//��ʾ�Ѿ��¹�ע��
					reconn.m_hasScore[i] = 1;
				}
			}
			//�����ׯ������ ���� ������ע ��ҵ�ǰ4�����Ƹ������
			if (ZHUANG_MODE_QIANG == tmp_zhuangMode || PLAY_MODE_MINGSCORE == tmp_playMode)
			{
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1 && NIUNIU_INVAILD_POS > pos; i++)
				{
					reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
				}
			}
			play_delay_time = DELAY_TIME_ADD_SCORE;
			break;
		}
		//���� or ����
		case GAME_PLAY_SEND_CARD:
		{
			reconn.m_zhuangPos = m_zhuangPos;
			memcpy(reconn.m_addScore, m_play_add_score, sizeof(m_play_add_score));
			for (int i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
			{
				if (m_play_add_score[i] != 0)
				{
					//��ʾ�Ѿ��¹�ע��
					reconn.m_hasScore[i] = 1;
				}
			}
			//�����ׯ������ ���� ������ע ��ҵ�ǰ4�����Ƹ������
			if (ZHUANG_MODE_QIANG == tmp_zhuangMode || PLAY_MODE_MINGSCORE == tmp_playMode)
			{
				for (Lint i = 0; i < NIUNIU_HAND_CARD_MAX - 1 && NIUNIU_INVAILD_POS > pos; i++)
				{
					reconn.m_playerHandCard[pos][i] = m_player_hand_card[pos][i];
				}
			}

			for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
			{
				reconn.m_hasOpenCard[i] = m_isOpenCard[i] ? 1 : 0;
				if (pos == i) // �Լ�������
				{
					for (Lint card = 0; card < NIUNIU_HAND_CARD_MAX; card++)
					{
						reconn.m_playerHandCard[i][card] = (Lint)m_player_hand_card[i][card];
					}
				}

				if (m_isOpenCard[i]) // �����������
				{
					reconn.m_player_ox[i] = m_cbOxCard[i];
					reconn.m_player_oxnum[i] = m_cbOxCard[i] ? m_player_oxnum[i] : 0;
					//����������Ƶ����
					for (Lint card = 0; card < NIUNIU_HAND_CARD_MAX; card++)
					{
						reconn.m_playerHandCard[i][card] = (Lint)m_player_hand_card[i][card];
					}
				}
			}
			play_delay_time = DELAY_TIME_OPEN_CARD;
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
			/*if (time_remain > 0 && time_remain <= play_delay_time)
			{
			}
			else if (time_remain > play_delay_time)
			{
			time_remain = play_delay_time;
			}
			else
			{
			time_remain = 0;
			}*/
			reconn.m_remainTime = time_remain;
			LLOG_ERROR("****��������desk_id=[%d], OnUserReconnect play_status:%d , delay:%d , escaped:%d  remain:%d",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status, play_delay_time, time_escaped, time_remain);

		}
		if (NIUNIU_INVAILD_POS > pos)
		{
			LLOG_ERROR("NiuNiuGameHandler::OnUserReconnect()  2 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
			notify_user(reconn, pos);
		}
		else if (m_desk->MHIsLookonUser(pUser))
		{
			LLOG_ERROR("NiuNiuGameHandler::OnUserReconnect()  3 userId[%d],deskId[%d] m_lookin[%d]",
				pUser->GetUserDataId(), m_desk ? m_desk->GetDeskId() : 0, reconn.m_isLookOn);
			pUser->Send(reconn);
		}
	}

	/* ������Ϸ���� */
	void OnGameOver(Lint result, Lint winpos, Lint bombpos)
	{
		if (m_desk == NULL || m_desk->m_vip == NULL)
		{
			LLOG_ERROR("NiuNiuGameHandler::OnGameOver() Error... This desk is NULL");
			return;
		}

		m_dismissed = !!m_desk->m_vip->m_reset;
		MHLOG_PLAYCARD("****Desk game over. Desk:%d round_limit: %d round: %d dismiss: %d",
			m_desk ? m_desk->GetDeskId() : 0, m_round_limit, m_round_offset, m_dismissed);

		//��ɢ���䣬��Ϸ����
		finish_round(true);
	}

	/* ��ʱ�� */
	void Tick(LTime& curr)
	{
		//���Ӷ���Ϊ��
		if (m_desk == NULL)
		{
			LLOG_ERROR("NiuNiuGameHandler::Tick() Error... This is desk NULL... deskId=[%d]", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}

		//û�п�����ʱ�Զ�����ѡ��  ��  ��ʱ����Ч
		if (m_isAutoPlay == false || m_tick_flag == false)
		{
			return;
		}

		//�����ɢ�У���ʱ����������
		if (m_desk && m_desk->m_resetTime > 0 && m_desk->m_resetUserId > 0)
		{
			LLOG_DEBUG("NiuNiuGameHandler::Tick() The desk is reseting... So tick do nothing...", m_desk ? m_desk->GetDeskId() : 0);
			return;
		}

		LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... deskId=[%d], playStatus=[%d], autoPlay=[%d], tickFlag=[%d]",
			m_desk ? m_desk->GetDeskId() : 0, m_play_status, m_isAutoPlay, m_tick_flag);

		switch (m_play_status)
		{
			//ѡׯ�׶�
		case GAME_PLAY_SELECT_ZHUANG:
		{
			LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Tick in Select Zhuang Status... deskId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status, m_isAutoPlay, m_tick_flag);

			switch (m_selectZhuangMode)
			{
				//��ׯģʽ
			case ZHUANG_MODE_LUN:
			{
				if (m_round_offset == 0)
				{
					if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU)
					{
						LLOG_DEBUG("desk_id=[%d], GAME_MODE_QIANG_ZHUANG time over!", m_desk ? m_desk->GetDeskId() : 0);
						for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
						{
							if (m_play_qiang_zhuang[i] == -1 && m_user_status[i] == 1)
							{
								on_event_user_select_zhuang(i, 0);
							}
						}
						//ִ���굹��ʱ�����õ���ʱ
						LLOG_ERROR("desk_id=[%d], set m_tick_flag = 0 ....", m_desk ? m_desk->GetDeskId() : 0);
						//m_tick_flag = false;
					}
				}
				break;
			}
			//���ׯģʽ
			/*case ZHUANG_MODE_RUND:
			{
			break;
			}*/

			//��ׯģʽ
			case ZHUANG_MODE_QIANG:
			{
				LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Tick in Select Zhuang Status, Now is Qiang Zhuang... deskId=[%d], playStatus=[%d]",
					m_desk ? m_desk->GetDeskId() : 0, m_play_status);

				if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_SELECT_QIANG_ZHUANG_NIUNIU)
				{
					LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Qiang Zhuang Time over!!!, Auto Qiang Zhuang... deskId=[%d]",
						m_desk ? m_desk->GetDeskId() : 0);

					for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
					{
						if (m_play_qiang_zhuang[i] == -1 && m_user_status[i] == 1)
						{
							on_event_user_select_zhuang(i, 0);
						}
					}
					//ִ���굹��ʱ�����õ���ʱ
					LLOG_ERROR("desk_id=[%d], set m_tick_flag = 0 ....", m_desk ? m_desk->GetDeskId() : 0);
					//m_tick_flag = false;
				}
				break;
			}

			}
			break;
		}
		//��ע�׶�
		case GAME_PLAY_ADD_SCORE:
		{
			LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Tick in Add Score Status, Now is Add Score... deskId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status);
			if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_ADD_SCORE)
			{
				LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Add Score Time over!!!, Auto Add Score... deskId=[%d]",
					m_desk ? m_desk->GetDeskId() : 0);

				for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
				{
					if (m_play_add_score[i] == 0 && m_user_status[i] == 1)
					{
						on_event_user_add_score(i, min_add_score());
					}
				}
				//ִ���굹��ʱ�����õ���ʱ
				//m_tick_flag = false;
			}
			break;
		}
		//���ƽ׶�
		case GAME_PLAY_SEND_CARD:
		{
			LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Tick in Open Cards Status, Now is Open Cards... deskId=[%d], playStatus=[%d]",
				m_desk ? m_desk->GetDeskId() : 0, m_play_status);
			if (curr.Secs() >= m_play_status_time.Secs() + DELAY_TIME_OPEN_CARD)
			{
				LLOG_DEBUG("NiuNiuGameHandler::Tick() Run... Open Card Time over!!!, Auto Open Card... deskId=[%d]",
					m_desk ? m_desk->GetDeskId() : 0);

				for (Lint i = 0; i < NIUNIU_PLAY_USER_COUNT; i++)
				{
					if (m_isOpenCard[i] == 0 && m_user_status[i] == 1)
					{
						on_event_user_open_card(i);
					}
				}
				//ִ���굹��ʱ�����õ���ʱ
				//m_tick_flag = false;
			}
			break;
		}

		}//switch (m_play_status) end...
	}
};

DECLARE_GAME_HANDLER_CREATOR(103, NiuNiuGameHandler);