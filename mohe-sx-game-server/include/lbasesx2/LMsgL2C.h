#pragma once



#include "LMsg.h"


/*
LMsgL2C.h ���ڴ�Ž����� LogicServer ���� Client ����Ϣ��������Ϣ���������ص㣺
1���������� Server ����
2������Ҫע�ᵽ LMsgFactory

*/



// ���� msgpack ���л� 4 �� int ������
struct DeskIntArray {
	int kData[4];
	DeskIntArray() { memset(this, 0, sizeof(*this)); }
	int &operator [](int pos) { return kData[pos & 0x3]; } // ��ȫ��������Խ��
	operator int *() { return kData; } // ����ʽת��ָ��
	MSGPACK_DEFINE_ARRAY(kData[0], kData[1], kData[2], kData[3]);
};


struct TingChuKouUnit {
	unsigned char kOutTile;
	// �� std::vector<unsigned char> �ᱻ�Զ�����ַ���
	std::vector<unsigned short> kTingKou;
	MSGPACK_DEFINE_MAP(kOutTile, kTingKou);
	TingChuKouUnit() : kOutTile(0) {}
};

// �޸Ŀͻ�����Ϣ����
struct LMsgS2COutCard :public LMsgSC {
	virtual LMsg *Clone() { return new LMsgS2COutCard(); }
	virtual bool Write(msgpack::packer<msgpack::sbuffer> &packer) { packer.pack(*this); return true; }

	MSGPACK_DEFINE_MAP(kMId, kPos, kTime, kDCount, kColor, kNumber, kFlag, kTingCards
		, kThink, kTing, /*kCheckTing,*/ kGang, kEnd, kBOnlyHu, kTingChuKou);
	Lint        kMId;
	Lint		kPos;//��ǰ�����е����
	Lint		kTime;//���Ƶ���ʱ
	Lint		kDCount;//����ʣ�����
	Lint		kFlag;//��ǰ�Ƿ���Ҫ���ƣ�0-��Ҫ��1-����Ҫ
	Lint		kGang;//�Ƿ�ܹ���0-û�У�1-�ܹ�
	Lint		kEnd;//�Ƿ񺣵� 0-���ǣ�1-��
	// ���������ƣ����ֻ�� m_pos == mypos m_flag == 0 ��ʱ�����
	// ֮�����������ֶΣ�����Ϊ֮ǰ�Կͻ��˵�Э���������
	//CardValue	m_curCard;//���������ƣ����ֻ�� m_pos == mypos m_flag == 0 ��ʱ�����
	Lint kColor;
	Lint kNumber;
	void SetCurCard(CardValue const &card) {
		kColor = card.kColor;
		kNumber = card.kNumber;
	}
	CardValue GetCurCard() const {
		return CardValue(kColor, kNumber);
	}
	bool		kBOnlyHu;		// ��������к��غ�
	std::vector<CardValue> kTingCards;//����״̬�¿ɳ�����
	std::vector<ThinkData> kThink;//˼��������
	Lint		kTing;//�Ƿ���״̬��0-������1-��
	//Lint        kCheckTing;  //���ڲ��ñ����淨�в�����**Ren 2017-*12-15��(��ʱ������)
	std::vector<TingChuKouUnit> kTingChuKou; // �����ڣ���ʾ�յ��ˡ�������ť�󣬿��Գ����Ƶļ���

	LMsgS2COutCard() : LMsgSC(MSG_S_2_C_GET_CARD) {
		kPos = INVAILD_POS;
		kTime = 0;
		kDCount = 0;
		kFlag = 0;
		kGang = 0;
		kEnd = 0;
		//kCheckTing = 0;
		kBOnlyHu = false;
		kMId = m_msgId;
	}

};



//////////////////////////////////////////////////////////////////////////
//�������㲥��Ϸ����
struct LMsgS2CGameOver : LMsgSC {
	virtual LMsg *Clone() { return new LMsgS2CGameOver(); }

	Lint		kResult;//0-������1-���ڣ�2-��ׯ
	DeskIntArray kWin;//4�Һ��������0-û����1-������2-���ڣ�3-����, 7-�� ��
	DeskIntArray kScore;//�� �� �� �� ������Ӯ���֣�������Ҫ������һ���
	DeskIntArray kCoins;//���
	DeskIntArray kTotalcoins;//��ҽ�������ܽ��
	DeskIntArray kAgang;//��������
	DeskIntArray kMgang;//�������� ���˵��
	DeskIntArray kMbgang;//�͸�(����)������
	DeskIntArray kDgang;//����˸ܵ�����
	DeskIntArray kGsgang;//���ָ�����
	DeskIntArray kStartHu;//���ֺ���
	DeskIntArray kTing;//�Ƿ�ͣ��
	std::vector<Lint> &m_hu(int pos) { return (&kHu1)[pos]; } // ������ҵĺ��Ʒ���
	std::vector<Lint> kHu1;
	std::vector<Lint> kHu2;
	std::vector<Lint> kHu3;
	std::vector<Lint> kHu4;

	std::vector<Lint> &m_dian(int pos) { return (&kDian1)[pos]; } // ��������
	std::vector<Lint> kDian1;
	std::vector<Lint> kDian2;
	std::vector<Lint> kDian3;
	std::vector<Lint> kDian4;


	std::vector<Lint>       kPlayType;     //��������
	std::vector<Lint>		kHuWay[4];		//���Ʒ�ʽ

	std::vector<Lint> &m_myFan(int pos) { return (&kMyFan1)[pos]; } //4x4 Ӯ�����(����Ϊ����)(�˱�Ϊ����)
	std::vector<Lint> kMyFan1;
	std::vector<Lint> kMyFan2;
	std::vector<Lint> kMyFan3;
	std::vector<Lint> kMyFan4;


	DeskIntArray                    kYbPiao;
	std::vector<CardValue>	kHunCard;

	std::vector<Lint>		kGangScore;
	std::vector<Lint>		kHuScore;//���Ƶ�����
									  //�ļ����ϵ���
	DeskIntArray		kCount;
	CardValue	kCard[4][14];

	std::vector<CardValue> &m_hand_cards(int pos) { return (&kArray0)[pos]; } // ������ҵ�����
	std::vector<CardValue> kArray0;
	std::vector<CardValue> kArray1;
	std::vector<CardValue> kArray2;
	std::vector<CardValue> kArray3;

	//������µ���
	DeskIntArray		kTangNum;
	CardValue	kTangCard[4][14];
	std::vector<CardValue> &m_tang_cards(int pos) { return (&kTangCard0)[pos]; } // ������µ���
	std::vector<CardValue> kTangCard0;
	std::vector<CardValue> kTangCard1;
	std::vector<CardValue> kTangCard2;
	std::vector<CardValue> kTangCard3;

	std::vector<CardValue> &m_hucards(int pos) { return (&kHucards1)[pos]; }
	std::vector<CardValue> kHucards1;
	std::vector<CardValue> kHucards2;
	std::vector<CardValue> kHucards3;
	std::vector<CardValue> kHucards4;

	Lint		kEnd;
	DeskIntArray		kFan;
	DeskIntArray		kGen;
	DeskIntArray		kWinList;
	Lint		kMaxFan;	// �ⶥ����
	DeskIntArray		kCheckTing;		// ����
	DeskIntArray		kRewardCardNum;		//���ƽ���������
	DeskIntArray		kRewardHu;			//���Ʊ�����������
	DeskIntArray		kChangeScore;		//���Ƶķ�

	Lint		kUser_count;

	// ������������ֶ�
	DeskIntArray kJinScore;
	DeskIntArray kJinNum;


	LMsgS2CGameOver() :LMsgSC(MSG_S_2_C_GAME_OVER)
	{
		kResult = -1;
		for (int i = 0; i < 4; ++i)
		{
			kWin[i] = 0;
			kScore[i] = 0;
			kCoins[i] = 0;
			kTotalcoins[i] = 0;
			kAgang[i] = 0;
			kMgang[i] = 0;
			kMbgang[i] = 0;
			kDgang[i] = 0;
			kGsgang[i] = 0;
			kStartHu[i] = 0;
			kCount[i] = 0;

			kFan[i] = 0;
			kGen[i] = 0;
			kWinList[i] = 0;
			kCheckTing[i] = 0;
			kTangNum[i] = 0;
			kYbPiao[i] = 0;
			kRewardCardNum[i] = 0;
			kRewardHu[i] = 0;
			kChangeScore[i] = 0;
			kTing[i] = 0;
		}
		kEnd = 0;
		kMaxFan = 0;
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
		pack.pack("kMId"); pack.pack(m_msgId);
		A(kResult); A(kEnd); A(kGangScore); A(kHuScore); A(kWin);
		//A(m_msgId); A(m_result); A(m_end); A(m_gangScore); A(m_huScore); A(m_win);
		A(kYbPiao);
		//17
		A(kHu1); A(kHu2); A(kHu3); A(kHu4);
		A(kMyFan1); A(kMyFan2); A(kMyFan3); A(kMyFan4);
		A(kDian1); A(kDian2); A(kDian3); A(kDian4);
		A(kGsgang);
		A(kHucards1); A(kHucards2); A(kHucards3); A(kHucards4);
		//9
		A(kPlayType); A(kAgang); A(kStartHu); A(kMgang); A(kMbgang); A(kDgang); A(kScore);
		A(kCoins); A(kTotalcoins);
		//12
		// ��Щ���ֶ�Ҫ�Կͻ��˱��ּ��ݵ�
		A(kArray0); A(kArray1); A(kArray2); A(kArray3);
		A(kTangCard0); A(kTangCard1); A(kTangCard2); A(kTangCard3);
		A(kFan); A(kGen); A(kWinList); A(kCheckTing);
		//6
		A(kRewardCardNum); A(kRewardHu);
		A(kHunCard); A(kMaxFan); A(kChangeScore); A(kTing);
		//2
		A(kJinScore);
		A(kJinNum);
#undef A
		return true;

	}

};


