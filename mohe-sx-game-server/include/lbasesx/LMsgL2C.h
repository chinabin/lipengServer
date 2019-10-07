#pragma once



#include "LMsg.h"


/*
LMsgL2C.h ���ڴ�Ž����� LogicServer ���� Client ����Ϣ��������Ϣ���������ص㣺
1���������� Server ����
2������Ҫע�ᵽ LMsgFactory

*/



// ���� msgpack ���л� 4 �� int ������
struct DeskIntArray {
	int data[4];
	DeskIntArray() { memset(this, 0, sizeof(*this)); }
	int &operator [](int pos) { return data[pos & 0x3]; } // ��ȫ��������Խ��
	operator int *() { return data; } // ����ʽת��ָ��
	MSGPACK_DEFINE_ARRAY(data[0], data[1], data[2], data[3]);
};


struct TingChuKouUnit {
	unsigned char m_outTile;
	// �� std::vector<unsigned char> �ᱻ�Զ�����ַ���
	std::vector<unsigned short> m_tingKou;
	MSGPACK_DEFINE_MAP(m_outTile, m_tingKou);
	TingChuKouUnit() : m_outTile(0) {}
};

struct LMsgS2COutCard :public LMsgSC {
	virtual LMsg *Clone() { return new LMsgS2COutCard(); }
	virtual bool Write(msgpack::packer<msgpack::sbuffer> &packer) { packer.pack(*this); return true; }

	MSGPACK_DEFINE_MAP(m_msgId, m_pos, m_time, m_dCount, m_color, m_number, m_flag, m_tingCards
		, m_think, m_ting, m_gang, m_end, m_bOnlyHu, m_tingChuKou);

	Lint		m_pos;//��ǰ�����е����
	Lint		m_time;//���Ƶ���ʱ
	Lint		m_dCount;//����ʣ�����
	Lint		m_flag;//��ǰ�Ƿ���Ҫ���ƣ�0-��Ҫ��1-����Ҫ
	Lint		m_gang;//�Ƿ�ܹ���0-û�У�1-�ܹ�
	Lint		m_end;//�Ƿ񺣵� 0-���ǣ�1-��
	// ���������ƣ����ֻ�� m_pos == mypos m_flag == 0 ��ʱ�����
	// ֮�����������ֶΣ�����Ϊ֮ǰ�Կͻ��˵�Э���������
	//CardValue	m_curCard;//���������ƣ����ֻ�� m_pos == mypos m_flag == 0 ��ʱ�����
	Lint m_color;
	Lint m_number;
	void SetCurCard(CardValue const &card) {
		m_color = card.m_color;
		m_number = card.m_number;
	}
	CardValue GetCurCard() const {
		return CardValue(m_color, m_number);
	}
	bool		m_bOnlyHu;		// ��������к��غ�
	std::vector<CardValue> m_tingCards;//����״̬�¿ɳ�����
	std::vector<ThinkData> m_think;//˼��������
	Lint		m_ting;//�Ƿ���״̬��0-������1-��
	Lint        m_check_ting;  //�������Ƶ��淨������Ӳ������ܣ�0������Ҫ������1����Ҫ������ע�ⲻ���������ƣ���**Ren 2017-12-13��
	std::vector<TingChuKouUnit> m_tingChuKou; // �����ڣ���ʾ�յ��ˡ�������ť�󣬿��Գ����Ƶļ���

	LMsgS2COutCard() : LMsgSC(MSG_S_2_C_GET_CARD) {
		m_pos = INVAILD_POS;
		m_time = 0;
		m_dCount = 0;
		m_flag = 0;
		m_gang = 0;
		m_end = 0;
		m_check_ting = 0;
		m_bOnlyHu = false;
	}

};



//////////////////////////////////////////////////////////////////////////
//�������㲥��Ϸ����
struct LMsgS2CGameOver : LMsgSC {
	virtual LMsg *Clone() { return new LMsgS2CGameOver(); }

	Lint		 m_result;//0-������1-���ڣ�2-��ׯ��4-���ܿ���
	DeskIntArray m_win;//4�Һ��������0-û����1-������2-���ڣ�3-����, 7-�� ��
	DeskIntArray m_score;//�� �� �� �� ������Ӯ���֣�������Ҫ������һ���
	DeskIntArray m_coins;//���
	DeskIntArray m_totalcoins;//��ҽ�������ܽ��
	DeskIntArray m_agang;//��������
	DeskIntArray m_mgang;//�������� ���˵��
	DeskIntArray m_mbgang;//�͸�(����)������
	DeskIntArray m_dgang;//����˸ܵ�����
	DeskIntArray m_gsgang;//���ָ�����
	DeskIntArray m_startHu;//���ֺ���
	DeskIntArray m_ting;//�Ƿ�ͣ��
	std::vector<Lint> &m_hu(int pos) { return (&m_hu1)[pos]; } // ������ҵĺ��Ʒ���
	std::vector<Lint> m_hu1;
	std::vector<Lint> m_hu2;
	std::vector<Lint> m_hu3;
	std::vector<Lint> m_hu4;

	std::vector<Lint> &m_dian(int pos) { return (&m_dian1)[pos]; } // ��������
	std::vector<Lint> m_dian1;
	std::vector<Lint> m_dian2;
	std::vector<Lint> m_dian3;
	std::vector<Lint> m_dian4;


	std::vector<Lint>       m_playType;     //��������
	std::vector<Lint>		m_huWay[4];		//���Ʒ�ʽ

	std::vector<Lint> &m_myFan(int pos) { return (&m_myFan1)[pos]; } //4x4 Ӯ�����(����Ϊ����)(�˱�Ϊ����)
	std::vector<Lint> m_myFan1;
	std::vector<Lint> m_myFan2;
	std::vector<Lint> m_myFan3;
	std::vector<Lint> m_myFan4;


	DeskIntArray                    m_ybPiao;
	std::vector<CardValue>	m_hunCard;

	std::vector<Lint>		m_gangScore;
	std::vector<Lint>		m_huScore;//���Ƶ�����
									  //�ļ����ϵ���
	DeskIntArray		m_count;
	CardValue	m_card[4][14];

	std::vector<CardValue> &m_hand_cards(int pos) { return (&array0)[pos]; } // ������ҵ�����
	std::vector<CardValue> array0;
	std::vector<CardValue> array1;
	std::vector<CardValue> array2;
	std::vector<CardValue> array3;

	//������µ���
	DeskIntArray		m_TangNum;
	CardValue	m_TangCard[4][14];
	std::vector<CardValue> &m_tang_cards(int pos) { return (&m_TangCard0)[pos]; } // ������µ���
	std::vector<CardValue> m_TangCard0;
	std::vector<CardValue> m_TangCard1;
	std::vector<CardValue> m_TangCard2;
	std::vector<CardValue> m_TangCard3;

	std::vector<CardValue> &m_hucards(int pos) { return (&m_hucards1)[pos]; }
	std::vector<CardValue> m_hucards1;
	std::vector<CardValue> m_hucards2;
	std::vector<CardValue> m_hucards3;
	std::vector<CardValue> m_hucards4;

	Lint		m_end;
	DeskIntArray		m_fan;
	DeskIntArray		m_gen;
	DeskIntArray		m_winList;
	Lint		m_maxFan;	// �ⶥ����
	DeskIntArray		m_checkTing;		// ����
	DeskIntArray		m_rewardCardNum;		//���ƽ���������
	DeskIntArray		m_rewardHu;			//���Ʊ�����������
	DeskIntArray		m_changeScore;		//���Ƶķ�

	Lint		m_user_count;

	// ������������ֶ�
	DeskIntArray m_jinScore;
	DeskIntArray m_jinNum;


	LMsgS2CGameOver() :LMsgSC(MSG_S_2_C_GAME_OVER)
	{
		m_result = -1;
		for (int i = 0; i < 4; ++i)
		{
			m_win[i] = 0;
			m_score[i] = 0;
			m_coins[i] = 0;
			m_totalcoins[i] = 0;
			m_agang[i] = 0;
			m_mgang[i] = 0;
			m_mbgang[i] = 0;
			m_dgang[i] = 0;
			m_gsgang[i] = 0;
			m_startHu[i] = 0;
			m_count[i] = 0;

			m_fan[i] = 0;
			m_gen[i] = 0;
			m_winList[i] = 0;
			m_checkTing[i] = 0;
			m_TangNum[i] = 0;
			m_ybPiao[i] = 0;
			m_rewardCardNum[i] = 0;
			m_rewardHu[i] = 0;
			m_changeScore[i] = 0;
			m_ting[i] = 0;
		}
		m_end = 0;
		m_maxFan = 0;
	}

	/* ����̫�಻����ô���壬���֧��16����
	MSGPACK_DEFINE_MAP(m_msgId, m_result, m_end, m_gangScore, m_huScore, m_win
	, m_hu1, m_hu2, m_hu3, m_hu4
	, m_hucards1, m_hucards2, m_hucards3, m_hucards4
	, m_playType, m_agang, m_mgang, m_dgang, m_score
	, array0, array1, array2, array3
	, m_rewardCardNum, m_rewardHu
	, m_hunCard, m_ting
	);
	*/

	virtual bool Write(msgpack::packer<msgpack::sbuffer>& pack)
	{
		//pack.pack(*this);
		//return true;

		// map���ֶ���Ҫ�˶�׼ȷ����ɾ�ֶ�Ҫͬʱ�޸�����
		pack.pack_map(51 + 2);
#define A(name) pack.pack(#name).pack(name)
		//7
		A(m_msgId); A(m_result); A(m_end); A(m_gangScore); A(m_huScore); A(m_win);
		A(m_ybPiao);
		//17
		A(m_hu1); A(m_hu2); A(m_hu3); A(m_hu4);
		A(m_myFan1); A(m_myFan2); A(m_myFan3); A(m_myFan4);
		A(m_dian1); A(m_dian2); A(m_dian3); A(m_dian4);
		A(m_gsgang);
		A(m_hucards1); A(m_hucards2); A(m_hucards3); A(m_hucards4);
		//9
		A(m_playType); A(m_agang); A(m_startHu); A(m_mgang); A(m_mbgang); A(m_dgang); A(m_score);
		A(m_coins); A(m_totalcoins);
		//12
		// ��Щ���ֶ�Ҫ�Կͻ��˱��ּ��ݵ�
		A(array0); A(array1); A(array2); A(array3);
		A(m_TangCard0); A(m_TangCard1); A(m_TangCard2); A(m_TangCard3);
		A(m_fan); A(m_gen); A(m_winList); A(m_checkTing);
		//6
		A(m_rewardCardNum); A(m_rewardHu);
		A(m_hunCard); A(m_maxFan); A(m_changeScore); A(m_ting);
		//2
		A(m_jinScore);
		A(m_jinNum);
#undef A
		return true;

	}

};


