


#include "Desk.h"
#include "CardSx.h"
#include "LTime.h"
#include "LVideo.h"
#include "LLog.h"
#include "Config.h"
#include "RoomVip.h"
#include "Work.h"
#include "GameHandler.h"
#include "Utils.h"
#include "ShanXiFeatureToggles.h"
#include "LMsgL2C.h"

#define USE_NEW_DEALING 01
#define USE_NEW_TING    01 // ʹ���µ������߼�


// 11-19�� 21-29Ͳ 31-39�� 41-47���������з��� �ܹ�34����
struct HongTongWangPaiTest : public LSingleton<HongTongWangPaiTest>
{
	tile_t tile_cards[13] = { 11, 11, 11, 12, 12, 12, 13, 13, 13, 14,14,14,15 }; // 
	tile_t tile_4men[13] = { 11,11,11, 21,21,21, 31,31,31, 41,41,41, 12 }; //�����Ʋ�����
	tile_t tile_3men[13] = { 11,11,11, 21,21,21, 31,31,31, 12,12,12, 13 }; //�����Ʋ�����
	tile_t tile_2men[13] = { 11,11,11, 21,21,21, 12,12,12, 41,41,41, 13 }; //2���ƿ������ɺ�
	tile_t tile_feng[13] = { 11,11,11, 12,12,12, 13,13,13, 41,42,43, 14 }; // ��ɸ�
	tile_t tile_sanyan[13] = { 11,11,11, 12,12,12, 13,13,13, 45,46,47, 14 }; //�ֳɸ�
	tile_t tile_3men7xiaodui[13] = { 11,11,12, 12,21,21, 31,31,41, 41,45,45, 14 }; //������С��
	tile_t tile_2men7xiaodui[13] = { 11,11,12, 12,21,21, 22,22,41, 41,45,45, 14 }; //2����С��
	tile_t tile_yingsanzui[13] = { 12,13,14, 21,22,23, 41,41,42, 42,43,44, 19 }; // 
	virtual bool Init() { return true; }
	virtual bool Final() { return true; }
};

#define HTTestClass HongTongWangPaiTest::Instance()

std::vector<std::string> splitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored)
{
	std::vector<std::string> resultStringVector;
	std::replace_if(srcStr.begin(), srcStr.end(), [&](const char& c) {if (delimStr.find(c) != std::string::npos) { return true; } else { return false; }}/*pred*/, delimStr.at(0));//�����ֵ����зָ������滻��Ϊһ����ͬ���ַ����ָ����ַ����ĵ�һ����  
	size_t pos = srcStr.find(delimStr.at(0));
	std::string addedString = "";
	while (pos != std::string::npos) {
		addedString = srcStr.substr(0, pos);
		if (!addedString.empty() || !repeatedCharIgnored) {
			resultStringVector.push_back(addedString);
		}
		srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
		pos = srcStr.find(delimStr.at(0));
	}
	addedString = srcStr;
	if (!addedString.empty() || !repeatedCharIgnored) {
		resultStringVector.push_back(addedString);
	}
	return resultStringVector;
}

int LoadPlayerCards(std::vector<int> & cards, int playerIndex)
{
	std::string fileName("card.ini");
	LIniConfig config;
	config.LoadFile("card.ini");

	char szKey[32] = { 0 };
	if (playerIndex > 0 && playerIndex <= 4)
		sprintf(szKey, "player%dcards", playerIndex);
	else if( playerIndex == 5)
		strcpy(szKey, "rest");
	else if (playerIndex == 6)
	{
		strcpy(szKey, "haozi");
	}

	std::string cardsString = config.GetString(Lstring(szKey), "");
	std::vector<std::string> cardsStringVec = splitString(cardsString, ",", true);
	if (cardsStringVec.size() > 0)
	{
		cards.clear();
		for (auto it = cardsStringVec.begin(); it < cardsStringVec.end(); it++)
		{
			int nValue = atoi((*it).c_str());
			MHLOG("Card: %d  %s", nValue, (*it).c_str());
			cards.push_back(nValue);
		}
	}


	return 0;
}

enum {
	ShanXi_JiaDe1000FenZhiZaiTouXiangChuXianShi = 0,
};



struct LMsgS2CUserSpeakTest : LMsgSC {
	Lint		m_userId;//˵����id
	Lint		m_pos;//˵���˵�λ��
	Lint		m_type;//˵������
	Lint		m_id;//����id
	Lstring		m_musicUrl;//����url����
	Lstring		m_msg;//��������


	LMsgS2CUserSpeakTest(): LMsgSC(MSG_S_2_C_USER_SPEAK) {
		m_userId = 0;
		m_pos = INVAILD_POS;
		m_type = -1;
		m_id = 0;
	}


	MSGPACK_DEFINE_MAP(m_msgId, m_userId, m_pos, m_type, m_id, m_musicUrl, m_msg);

	virtual bool Write(msgpack::packer<msgpack::sbuffer> &pack) {
		pack.pack(*this);
		return true;
		WriteMap(pack, 7);
		WriteKeyValue(pack, NAME_TO_STR(m_msgId), m_msgId);
		WriteKeyValue(pack, NAME_TO_STR(m_userId), m_userId);
		WriteKeyValue(pack, NAME_TO_STR(m_pos), m_pos);
		WriteKeyValue(pack, NAME_TO_STR(m_type), m_type);
		WriteKeyValue(pack, NAME_TO_STR(m_id), m_id);
		WriteKeyValue(pack, NAME_TO_STR(m_musicUrl), m_musicUrl);
		WriteKeyValue(pack, NAME_TO_STR(m_msg), m_msg);
		return true;
	}

};

/*�����ṹ��*/
struct PosOp 
{
	// ������ƺ�����Ĳ���Ϊ�������������˳��ƺ��Լ������Ĳ���Ϊ��������
	bool active;
	// 0123 �ĸ�λ�õ�������Ĳ���
	int pos;
	// THINK_OPERATOR_ ö�٣����ܸ������ֵ���Ϳͻ��˹��õ�(1�����ƣ�2����...)
	int code;
	// ����һ����������
	tile_t tile;
	/*************************************************
	* ��������  PosOp()
	* ������    ��ʼ������
	* ������
	* /active   �Ƿ�Ϊ����������true:������false:����
	* /pos      ��������ҵ�λ��
	* /code     ��������THINK_OPERATOR_ ö�٣�1:���ƣ�2:��...
	* /tile     �������Ƶ���ֵ
	*************************************************/
	PosOp(bool active, int pos, int code, tile_t tile): active(active), pos(pos), code(code), tile(tile) {}
};

/*������������ṹ�壬�̳в����ṹ��*/
struct ActivePosOp: PosOp {
	ActivePosOp(int pos, int code, tile_t tile): PosOp(true, pos, code, tile) {}
};

/*��ұ��������ṹ�壬�̳в����ṹ��*/
struct PassivePosOp: PosOp {
	PassivePosOp(int pos, int code, tile_t tile): PosOp(false, pos, code, tile) {}
};


// �����Զ���ʼ�����ֶη����ÿ���Զ�����
struct ShanXiRoundState__c_part {

	Lint			 m_angang[DESK_USER_COUNT];//��������
	Lint			 m_minggang[DESK_USER_COUNT];//��������
	Lint			 m_diangang[DESK_USER_COUNT];//�������
	Lint			 m_adiangang[DESK_USER_COUNT];//���������
	Lint			 m_louHuCard[DESK_USER_COUNT];//�Ƿ���©����¼
	bool			 m_louZimohu[DESK_USER_COUNT];//����������ͱ���¼���������ڹ���ֻ������
	Lint			 m_kouCount[DESK_USER_COUNT];
	Lint			 m_knowCardCount[40];
	Lint             m_getChardFromMAGang[DESK_USER_COUNT];  //�����Ƿ���Ϊ���ܻ��ǰ��ܣ�0�����Ǹܺ����ƣ�1���������ƣ�2����������(Ĭ��Ϊ0)---------------- true:���ܣ�false:���� ��ʼΪ��true ��������ʹ�ã���**Ren 2017-11-30��
	Card             m_hongTongWangPai_ShuangWang[DESK_USER_COUNT];  //�鶴����˫����ÿ�����Լ�ѡ�����ƣ�**Ren 2017-12-12��

	Card*			 m_curOutCard;//��ǰ����������
	Card*			 m_curGetCard;//��ǰ��ȡ����
	GangThink		 m_GangThink;	//��ǰ�Ƿ���˼����
	Lint			 m_beforePos;					//֮ǰ������λ��
	Lint			 m_beforeType;					//֮ǰ����������
	Lint			 m_curPos;						//��ǰ�������
	bool			 m_needGetCard;
	DESK_PLAY_STATE  m_play_state; // �� m_desk->setDeskPlayState ��ȫͬ������ʼֵΪ-1

	Llong            m_tingTimeBegin[DESK_USER_COUNT];
	Llong            m_tingTimeEnd[DESK_USER_COUNT];
	Llong            m_getcardTimeBegin[DESK_USER_COUNT];
	Llong            m_getcardTimeEnd[DESK_USER_COUNT];

	void clear_round() {
		memset(this, 0, sizeof(*this));
		// �����Ǿ����㲻����Ҫ��Ļ�������д�Զ�������
		m_GangThink = GangThink_over;
		m_beforePos = INVAILD_POS;
		for (Lint i = 0; i< DESK_USER_COUNT; i++) {
			m_kouCount[i] = -1;
			//�����ӣ��������Ʊ��Ĭ��Ϊtrue(**Ren 2017-12-01)
			//m_getChardFromMAGang[i] = true;
		}
		m_play_state = (DESK_PLAY_STATE)-1;		 
	}
};

// �й��캯�����ֶη����ÿ���Զ�����
struct ShanXiRoundState__cxx_part {
	PlayerState     m_ps[DESK_USER_COUNT]; // ���ڼ�¼��������ʷ

	CardVector      m_handCard[DESK_USER_COUNT]; // ������ϵ���
	CardVector		m_suoHandCard[DESK_USER_COUNT];//����������
	CardVector		m_outCard[DESK_USER_COUNT];	//��ҳ�����
	CardVector		m_pengCard[DESK_USER_COUNT];//��������ƣ�
	CardVector		m_minggangCard[DESK_USER_COUNT];//������ܵ���
	
	CardVector		m_angangCard[DESK_USER_COUNT];//��Ұ��ܵ���
	CardVector		m_eatCard[DESK_USER_COUNT];//��ҳԵ���

	ThinkTool		m_thinkInfo[DESK_USER_COUNT];//��ǰ����˼��״̬
	ThinkTool       m_thinkInfoForYingKou[DESK_USER_COUNT]; // Ӳ��˼��״̬
	//ThinkTool       m_checkTingInfo[DESK_USER_COUNT];  //�����������������ݣ�**Ren 2017-12-27��
	ThinkUnit		m_thinkRet[DESK_USER_COUNT];//��ҷ���˼�����
	TingState		m_tingState[DESK_USER_COUNT];	//����״̬

	CardVector		m_deskCard;						//������ʣ�����
	CardVector      m_Cannot_gangCard_MH[DESK_USER_COUNT];  // ��¼�û����ܸܵ��ƣ����ڿ۵���Ƶ���
	CardVector      m_guoPengCard[DESK_USER_COUNT];         //������¼�������󲻹��Լ�����������������,���ݿ۵���ʹ�ã�**Ren 2017-11-25��

	typedef unsigned char desk_mask_t;
	SmallBitVec<desk_mask_t> m_winConfirmed; // ��ҿ��Ժ��ˣ������Ѿ����ˡ�������ť
	SmallBitVec<desk_mask_t> m_decideGuoHu; // ���������������� CheckThink �Ͳ���������ˣ������˳��ƾ����
	SmallBitVec<desk_mask_t> m_bHongTongYingKou; // ����ʱѡ���Ӳ��״̬: 0:��Ӳ�� 1:Ӳ��
	SmallBitVec<desk_mask_t> m_bInYingKouThink;  // ��ǰ�û��Ƿ���Ӳ��˼���� 1 �ǣ�2: ����
	SmallBitVec<desk_mask_t> m_bHongTongYingKouFirstOutCard;// ���ƺ��һ�γ��ƣ�

#if USE_NEW_DEALING
#else
	CardVector		m_3_laizi; // ������������ϴ�ƣ�ÿ�̶����õ�

	// ��4����������InitCard����ʱ״̬��Ӧ��Ų��InitCard����
	Card			wan_base_card[BASE_CARD_COUNT];
	Card			bing_base_card[BASE_CARD_COUNT];
	Card			tiao_base_card[BASE_CARD_COUNT];
	Card			zi_base_card[BASE_CARD_COUNT];
#endif

	void clear_round() {
		this->~ShanXiRoundState__cxx_part();
		new (this) ShanXiRoundState__cxx_part;
		for (Lint i = 0; i< DESK_USER_COUNT; i++) {
			m_thinkInfo[i].Reset();
			m_thinkRet[i].Clear();
			m_tingState[i].clear();
		}
	}
};

struct ShanXiMatchState__c_part {
	Desk *m_desk;
	int m_round_offset;
	int m_round_limit;
	int m_accum_score[DESK_USER_COUNT];
	bool m_dismissed;


	void clear_match() {
		memset(this, 0, sizeof(*this));
		// �����Ǿ����㲻����Ҫ��Ļ�������д�Զ�������
	}
};

struct ShanXiMatchState__cxx_part {


	void clear_match() {
		this->~ShanXiMatchState__cxx_part();
		new (this) ShanXiMatchState__cxx_part;
	}
};


struct ShanXiRoundState: ShanXiRoundState__c_part, ShanXiRoundState__cxx_part {
	// ��Ҫ��������ֶ�
	void clear_round() {
		ShanXiRoundState__c_part::clear_round();
		ShanXiRoundState__cxx_part::clear_round();
	}
};

// һ��Match���кü���Round
struct ShanXiMatchState: ShanXiFeatureToggles, ShanXiMatchState__c_part, ShanXiMatchState__cxx_part {
	// ��Ҫ��������ֶ�
	void clear_match() {
		ShanXiFeatureToggles::clear_match();
		ShanXiMatchState__c_part::clear_match();
		ShanXiMatchState__cxx_part::clear_match();
	}
};



// Ϊ�˼�����ǰ��Cardָ���ʾ��ֵ��д��
struct CardPool {
	Card black_hole;
	Card pool[TILE_MAX - TILE_MIN + 1];
	CardPool() {
		memset(this, 0, sizeof(*this));
		for (tile_t tile = TILE_MIN; tile <= TILE_MAX; ++tile) {
			if ((TILE_TO_MASK(tile) & TILE_MASK_VALID) == 0) continue;
			Card *card = (*this)[tile];
			card->m_color = tile_to_color(tile);
			card->m_number = tile_to_number(tile);
		}
	}

	Card *operator [](tile_t tile) {
		if (tile < TILE_MIN || tile > TILE_MAX) return &black_hole;
		return &pool[tile - TILE_MIN];
	}
};



struct ShanXiDeskState: ShanXiRoundState, ShanXiMatchState {
	OperateState	mGameInfo;
	CardPool card_pool;

	void clear_match(int player_count) {
		ShanXiMatchState::clear_match();
		clear_round();
		m_player_count = player_count;
	}

	void clear_round() {
		ShanXiRoundState::clear_round();
		mGameInfo.m_hCard.clear();
		mGameInfo.m_cards_guo_gang_MH.clear();		 
	}

	void setup_match(std::vector<Lint> &l_playtype, int round_limit) {
		m_playtype.init_playtype_info(l_playtype, FEAT_GAME_TYPE);
		m_round_limit = round_limit;
		// ����Сѡ���������ĺ��Ʒ���
		init_allowed_fans();
		mGameInfo.allowed_fans = m_allowed_fans;
		init_desk_card_remain_count();		
	}

	void increase_round() {
		++m_round_offset;
		if (m_desk && m_desk->m_vip) ++m_desk->m_vip->m_curCircle;
	}

	void set_play_state(DESK_PLAY_STATE v) {
		m_play_state = v;
		if (m_desk) m_desk->setDeskPlayState(v);
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

	Lint CheckPositionPrior(Lint CurPos, Lint first_pos, Lint second_pos) {
		// �� CurPos Ϊԭ�㣬���� first_pos �� second_pos �� CurPos ����ʱ�����
		Lint dif1 = first_pos - CurPos > 0 ? first_pos - CurPos : first_pos - CurPos + m_player_count;
		Lint dif2 = second_pos - CurPos > 0 ? second_pos - CurPos : second_pos - CurPos + m_player_count;
		// ѡȡ��ʱ�����̵��Ǹ�
		if (dif1 < dif2) return first_pos;
		else return second_pos;
	}

	int find_primary_winpos(int bombpos, int winpos) {
		if (bombpos == INVAILD_POS) return winpos;
		for (int d = 0; d < m_player_count; ++d) {
			int pos = (bombpos + d) % m_player_count;
			if (m_winConfirmed[pos]) return pos;
		}
		return winpos;
	}

	unsigned sync_hands_to_m_ps(int x, Card* winCard) {
		PlayerState &ps = m_ps[x];
		CardVector &handCards = m_handCard[x];
		unsigned num_huns = 0;
		for (size_t i = 0; i < handCards.size(); ++i) {
			//TODO:�˴���Ӻ鶴����˫���������жϣ�**Ren 2017-12-12����ʱע�͵���2018-01-08��
			/*if (HongTongWangPai == FEAT_GAME_TYPE && 
				m_playtype[MH_PT_HongTongWangPai_ShuangWang] &&
				IsHaozipai_HongTongShuangWang(x, handCards[i]))
			{
				++num_huns;
			}
			else if (IsHaozipai(handCards[i])) ++num_huns;  //ȥ�������if���ԲŸĳ�if
			*/
			if (IsHaozipai(handCards[i])) ++num_huns;
			else ps.hands.add(CardToTile(handCards[i]));
		}
		// ��������������14���ˣ�
		if (winCard && ps.hands.size() < 14) {
			if (IsHaozipai(winCard)) ++num_huns;
			else ps.hands.add(CardToTile(winCard));
		}
		return num_huns;
	}

	HuUnit const * hongtong_find_hu_unit(int winpos, Card * winCard, bool is_zimo)
	{
		if (NULL == winCard)
		{
			return NULL;
		}
		if (m_tingState[winpos].m_ting_state == Ting_waitHu && !m_tingState[winpos].hu_units.empty()) {
			const std::vector<HuUnit>& hu_units = m_tingState[winpos].hu_units;
			for (size_t k = 0; k < hu_units.size(); ++k) {
				if (ToTile(hu_units[k].hu_card) == ToTile((winCard))) return &hu_units[k];
			}
		}
		return NULL;
	}

	HuUnit const *find_hu_unit(int winpos, Card *winCard, bool is_zimo) {
		// Ӧ�ñ���winCard�ɣ�TODO: Ū���������һ��
		if (NULL == winCard) return 0;
		if (HongTongWangPai == FEAT_GAME_TYPE)
		{
		    return hongtong_find_hu_unit(winpos,winCard,is_zimo);
		}
		// �ٵ�������ƣ����ú�����ȡ��������
		// ���˴����������ֻ�ܵ���ֵ�������ã����ܵ���������
		if (IsHaozipai(winCard) && is_zimo) {
			// ����ж�Ӧ�ÿ϶������������㰲ȫ���
			if (m_tingState[winpos].m_ting_state == Ting_waitHu && !m_tingState[winpos].hu_units.empty()) {
				const std::vector<HuUnit>& hu_units = m_tingState[winpos].hu_units;

				struct SortUnit {
					int score;
					HuUnit const *hu_unit;
				};
				// ������֮ǰ�ð��ٵ�ĺ���Ҫ����˵������͵�
				std::vector<SortUnit> candidates;
				for (size_t k = 0; k < hu_units.size(); ++k) {
					tile_t tile = ToTile(hu_units[k].hu_card);
					SortUnit unit = { hu_units[k].score, &hu_units[k] };
					if (KouDian == FEAT_GAME_TYPE) {
						int points = tile_to_points(tile);
						int limit = is_zimo ? 3 : 6;
						if (points < limit) continue;
					}
					candidates.push_back(unit);
				}

				// �ڿٵ����ڼ���ʱ���ᱣ֤������һ��6�����ϵģ����Բ����������֧��
				if (candidates.empty()) return NULL;

				struct hufan_less_than {
					//bool operator ()(const HuUnit& lhs, const HuUnit& rhs) { return tile_to_points(CardToTile(lhs.hu_card)) < tile_to_points(CardToTile(rhs.hu_card)); }
					bool operator ()(const SortUnit& lhs, const SortUnit& rhs) { return lhs.score < rhs.score; }
				};
				std::vector<SortUnit>::const_iterator the_big = std::max_element(candidates.begin(), candidates.end(), hufan_less_than());
				return the_big->hu_unit;
			}
		} 
		else {
			if (m_tingState[winpos].m_ting_state == Ting_waitHu && !m_tingState[winpos].hu_units.empty()) {
				const std::vector<HuUnit>& hu_units = m_tingState[winpos].hu_units;
				for (size_t k = 0; k < hu_units.size(); ++k) {
					if (ToTile(hu_units[k].hu_card) == ToTile((winCard))) return &hu_units[k];
				}
			}
		}
		return NULL;
	}	

	void sync_to_m_ps(int win_result, Card* winCard, FANXING conf) {
		tile_t last_tile = CardToTile(winCard);
		for (int x = 0; x < m_player_count; x++) {
			PlayerState &ps = m_ps[x];
			if (m_winConfirmed[x]) {
				ps.num_huns = sync_hands_to_m_ps(x, winCard);
			}
			//ׯ��+ׯ�ҷ���
			ps.make_over_status(m_winConfirmed[x], x == m_zhuangpos);
			//�� ������ ��ť�����
			if (m_winConfirmed[x]) {
				ps.last_tile = CardToTile(winCard);
				if (IsHaozipai(winCard)) last_tile = 0;
				FANXING tmp_conf = conf;
				if (WIN_ZIMO == win_result) tmp_conf |= FAN_ZiMo;
				if (x == m_zhuangpos) tmp_conf |= FAN_Zhuang;
				
				FANXING hongtong_qixiaodui_fan = FAN_NONE;
				if (HongTongWangPai == FEAT_GAME_TYPE)
				{
					bool is_zimo = WIN_ZIMO == win_result;
					HuUnit const * pHuUnit = hongtong_find_hu_unit(x, winCard, is_zimo);
					if (pHuUnit && (pHuUnit->fans & FAN_QiXiaoDui))
					{
					    // �鶴��С�Եĺ������Ͳ��������ƴ���
						MHLOG("*******�鶴�����ҵ���С�Եĺ��ڷ���:%s", malgo_format_fans(pHuUnit->fans).c_str());
						hongtong_qixiaodui_fan = pHuUnit->fans;
					}
					//for (int i = 0; i < m_tingState[x].hu_units.size(); i++)
					//{
					//	MHLOG("***********i = %d m_tingState[x].hu_units[i].hu_card = %d, fans=%s", i, ToTile(m_tingState[x].hu_units[i].hu_card), 
					//		malgo_format_fans(m_tingState[x].hu_units[i].fans).c_str());
					//}
				}
				FANXING ps_conf_fans = tmp_conf;
				if (HongTongWangPai == FEAT_GAME_TYPE && hongtong_qixiaodui_fan)
				{
					ps_conf_fans  &= ~FAN_QiXiaoDui; //�鶴���Ʋ�����7С�ԣ������ڻ�ȡ
				}
				 
				MHLOG("********* ��ʼ������Ƽ���ķ��� ps_conf_fans = %s", malgo_format_fans(ps_conf_fans).c_str());
				MHLOG("********* ��ʼ������Ƽ���ķ��� hongtong_qixiaodui_fan = %d tmp_conf = %s, ",(bool)hongtong_qixiaodui_fan, malgo_format_fans(tmp_conf).c_str());
				FANXING fans = call_judge(ps, ps.num_huns, ps.hands, last_tile, ps_conf_fans
					, ps.make_pg_mask(), ps.sum_pg_tile_count_by_color());
				MHLOG("*********���Ƽ���ķ���%s", malgo_format_fans(fans).c_str());
				MHLOG("*********ps�ķ��ͷ��� %s ", malgo_format_fans(ps.fans).c_str());
				// �����ڵ�����Ʒ��ͣ��������� FAN_ZiMo FAN_Zhuang �⣬��֪������û������Ӱ�죬�������޶����ٵ�
				// Ӳ������뱨���ģ�Ҳ�����ڵ��� fans�����治�ü��һ��Ӯ�ˣ����ڼ���ʱ�����
				if (KouDian == FEAT_GAME_TYPE || YingSanZui == FEAT_GAME_TYPE || LinFenYiMenPai == FEAT_GAME_TYPE || 
					HongTongWangPai == FEAT_GAME_TYPE || XinZhouKouDian == FEAT_GAME_TYPE || NianZhongZi == FEAT_GAME_TYPE || 
					JinZhong == FEAT_GAME_TYPE) {
					bool is_zimo = WIN_ZIMO == win_result;
					HuUnit const *hu_unit = find_hu_unit(x, winCard, is_zimo);
					if (hu_unit) {
						fans = hu_unit->fans;
						ps.good_last_tile = ToTile(hu_unit->hu_card);						
					}
				}
				
				// �鶴���ƴ����ڵ���ķ���
				if (HongTongWangPai == FEAT_GAME_TYPE)
				{		
					// ��������С�ӵķ���
					if (hongtong_qixiaodui_fan)
					{
						fans |= hongtong_qixiaodui_fan;
					}
				}

				// ps ������һЩ���� mask ���ķ����ж�
				if (fans) fans |= ps.fans;

				// ����ǰ�����壬�����иܣ����ڼ��һ���ǲ������壬�����������Ͱ�����ķ���ȥ��
				// ����ܻ�Ӱ�����ڼ���ʱ�õ��� fans ��������Ҫ����һ��
				// �����Ӱ�죺����
				if ((fans & FAN_MenQing) && !ps.is_menqing()) {
					fans &= ~FAN_MenQing;
				}

				// �� judgement ͬ���� ps
				ps.fans = fans;

				if (ps.fans) {
					// ���պ���ʱ�����ڵ�����Ʒ��͵Ļ��ƣ��������� FAN_ZiMo FAN_Zhuang
					ps.fans &= ~(FAN_ZiMo | FAN_Zhuang);
					if (WIN_ZIMO == win_result) ps.fans |= FAN_ZiMo;
					if (x == m_zhuangpos) ps.fans |= FAN_Zhuang;

					// ���Կ��������ڴ�СΪ1
					if (m_tingState[x].hu_units.size() == 1) {
						ps.fans |= FAN_KanHu;
					}
					// �����ǲ�������ô�������ڴ�СΪ1��
					if ((m_allowed_fans & FAN_KanHu) && m_tingState[x].hu_units.size() == 0) {
						int pos = x;
						tile_mask_t pg_mask = calc_pg_mask(m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos]);
						std::vector<HuUnit> tingInfo;
						gCardMgrSx.calc_ting_kou(m_ps[pos], m_handCard[pos], pg_mask, mGameInfo, *this, tingInfo);
						if (tingInfo.size() == 1) {
							ps.fans |= FAN_KanHu;
						}
					}
					if (FEAT_GAME_TYPE == GuaiSanJiao)
					{
						if (ps.fans & FAN_KanHu) ps.fans &= ~FAN_PingHu;
						if ( (ps.fans&FAN_KanHu) && (ps.fans&(FAN_HaoHuaQiXiaoDui|FAN_QiXiaoDui)))
							ps.fans &= ~FAN_KanHu;
					}
				}

				ps.fans &= tmp_conf; // ���ͱ������޶���Χ��

			}
		}
	}

	void add_hand_card(int pos, Card *card) {
		m_handCard[pos].push_back(card);
		gCardMgrSx.SortCard(m_handCard[pos]);
	}

	void erase_hand_card(int pos, Card *card, int n = 1) {
		gCardMgrSx.EraseCard(m_handCard[pos], card, n);
	}

	// ɾ�����ƣ��Զ��������ĵ�����
	void erase_suo_card(int pos, Card *card, int n = 1) {
		// m_suoHandCard��m_handCard��һ���֣�m_handCard����m_suoHandCard
		// ����ʱ���ȴ����Ƴ������ǲ��ܰ�����Ū����
		if (n <= 0) return;
		if (NULL == card) return;
		erase_hand_card(pos, card, n);
		// �������淨�����m_suoHandCard�ǿյ�
		if (!m_suoHandCard[pos].empty()) {
			gCardMgrSx.EraseCard(m_suoHandCard[pos], card, n);
			// ��Ϊ���ȴ���������ɾ������ֱ��ɾ���ǣ�ɾ���ˣ��Ͳ���һ����
			if (m_suoHandCard[pos].empty()) {
				m_suoHandCard[pos].push_back(card);
			}
		}
	}

	void InsertIntoKnowCard(Card* the_card, Lint num) {
		m_knowCardCount[gCardMgrSx.get_card_index(the_card)] += num;
	}

	void InsertIntoKnowCard(CardVector the_cards) {
		for (size_t x = 0; x < the_cards.size(); x++) {
			m_knowCardCount[gCardMgrSx.get_card_index(the_cards[x])] ++;
		}
	}


	bool IsHaozipai(const Card& card) { return std::find(mGameInfo.m_hCard.begin(), mGameInfo.m_hCard.end(), card) != mGameInfo.m_hCard.end(); }
	bool IsHaozipai(const Card* card) { return card && IsHaozipai(*card); }
	/**************************************************************************
	* ��������     IsHaozipai_HongTongShuangWang()
	* ������       �鶴����˫���淨�ж��Ƿ�Ϊ�����ƣ�ÿ����Һ����Ʋ�һ����
	* ������
	*  @ pos ���λ��
	*  @ card �жϵ���
	* ���أ�       (bool)
	***************************************************************************/
	bool IsHaozipai_HongTongShuangWang(int pos, const Card* card) 
	{
		unsigned char tmp_wang = m_hongTongWangPai_ShuangWang[pos].ToChar();
		if (tile_is_valid(tmp_wang) && tmp_wang == card->ToChar())
		{
			return true;
		}
		return false;
	}
};




struct ShanXiVideoSupport: ShanXiDeskState {
	VideoLog		m_video;						//¼��




	///////////////////////////////////////////////////////////////////
	// ��鼯�д�ź�¼���йصĺ���
	///////////////////////////////////////////////////////////////////
	void VideoSave() {
		if (NULL == m_desk) return;

		m_video.m_Id = gVipLogMgr.GetVideoId();
		//m_video.m_playType = m_desk->getPlayType();
		m_video.m_flag = m_registered_game_type;
		LMsgL2LDBSaveVideo video;
		video.m_type = 0;
		video.m_sql = m_video.GetInsertSql();
		gWork.SendMsgToDb(video);
	}

	void VideoAdd__pos_oper_cards(int pos, int oper, std::vector<CardValue>& cards) {
		if (NULL == m_desk) return;

		m_video.AddOper(oper, pos, cards);
	}

	void VideoAdd__deal_card() {
		m_video.Clear();
		Lint id[4] = { 0 };
		Lint score[4] = { 0 };
		std::vector<CardValue> vec[4];
		for (Lint i = 0; i < m_player_count; ++i)
		{
			int user_data_id = m_desk ? m_desk->m_user[i]->GetUserDataId() : i;
			id[i] = user_data_id;
			score[i] = m_accum_score[i];
			for (size_t j = 0; j < m_handCard[i].size(); ++j)
			{
				CardValue v;
				v.kColor = m_handCard[i][j]->m_color;
				v.kNumber = m_handCard[i][j]->m_number;
				vec[i].push_back(v);
			}
		}
		int desk_id = m_desk ? m_desk->GetDeskId() : 0;
		std::vector<int> game_options;
		m_playtype.fill_vector_for_client(game_options, FEAT_GAME_TYPE);
		// ���������֪����ʲô�õ�
		int desk_flag = m_desk ? m_desk->m_flag : 0;
		// ����ȥ�� score �ᱻ�浽���� m_score �ϣ���ʾ�����¼��ʱ�����ͷ������ʾ�ķ�����������1000���ͻ�����ʾ¼��ʱ���������1000
		m_video.DealCard(id, vec, gWork.GetCurTime().Secs(), m_zhuangpos, score, desk_id, m_round_offset, m_round_limit, desk_flag, game_options);

	}

	void VideoAdd__pos_oper_card_n(Lint pos, Lint oper, const Card* card, int n) {
		std::vector<CardValue> cards;
		if (n > 0) cards.insert(cards.begin(), n, ToCardValue(card));
		VideoAdd__pos_oper_cards(pos, oper, cards);
	}
	void VideoAdd__pos_oper_card(Lint pos, Lint oper, const Card* card) {
		VideoAdd__pos_oper_card_n(pos, oper, card, 1);
	}
	void VideoAdd__pos_oper(Lint pos, Lint oper) {
		VideoAdd__pos_oper_card_n(pos, oper, NULL, 0);
	}
	void VideoAdd__doing(PosOp const &op) {
		std::vector<CardValue> cards;
		CardValue card;
		card.kNumber = op.code;
		card.kColor = op.tile;
		cards.push_back(card);
		VideoAdd__pos_oper_cards(op.pos, VIDEO_OPEN_DOING, cards);
		MHLOG_PLAYCARD("****Desk:%d ��Ҳ��� pos=%d type:%d card:%d",m_desk ? m_desk->GetDeskId():0, op.pos, op.code, op.tile);
	}

	void VideoDoing(Lint op, Lint pos, Lint card_color, Lint card_number) {
		// ����Ǽ�¼���ִ�����ĸ�����
		std::vector<CardValue> cards;
		CardValue card;
		card.kNumber = op;
		card.kColor = card_color * 10 + card_number;
		cards.push_back(card);
		VideoAdd__pos_oper_cards(pos, VIDEO_OPEN_DOING, cards);
	}

	Lstring format_user_think(const ThinkVec & vect)
	{
		std::ostringstream ss;
		for (auto itr =  vect.begin(); itr != vect.end(); ++itr) {
			CardValue card;
			ss << '(' << itr->m_type << '-';
			if (itr->m_card.size() > 0)
				ss << (int)ToTile(itr->m_card[0]);	
			ss << ')' << ';';
		}
		return ss.str();
	}
	void VideoThink(Lint pos) {
		// ��ν���˼�����Ǹ���ҵ�������ť�Լ��������
		// ����ĳ��˼�����������ܲ������ƣ����ܹ���һ���ƣ�Ҳ���ܹ���������
		// �����ʵ�ְ�CardValue����ͨ�����ݽṹ�ˣ���m_number��thinkö�٣���m_color�����2����
		if (m_thinkInfo[pos].m_thinkData.size() > 0) {
			std::vector<CardValue> cards;
			for (auto itr = m_thinkInfo[pos].m_thinkData.begin(); itr != m_thinkInfo[pos].m_thinkData.end(); ++itr) {
				CardValue card;
				card.kNumber = itr->m_type;
				if (itr->m_card.size() > 0)
					card.kColor = itr->m_card[0]->m_color * 10 + itr->m_card[0]->m_number;
				if (itr->m_card.size() > 1)
					card.kColor = card.kColor * 1000 + itr->m_card[1]->m_color * 10 + itr->m_card[1]->m_number;
				cards.push_back(card);
			}
			VideoAdd__pos_oper_cards(pos, VIDEO_OPEN_THINK, cards);
			MHLOG_PLAYCARD("****Desk:%d Send ���˼��  pos=%d think:%s", m_desk ? m_desk->GetDeskId() : 0, pos, format_user_think(m_thinkInfo[pos].m_thinkData).c_str());
		}
	}

	void VideoAdd__HaoZi(std::vector<Card> const &huns) {
		// ¼�����
		std::vector<CardValue> cards;
		VecExtend(cards, huns);
		VideoAdd__pos_oper_cards(100, VIDEO_Oper_Haozi, cards);
	}

};


struct ShanXiGameCore: GameHandler, ShanXiVideoSupport {
	bool basic_can_hu_shoupao(int pos) {
		if (FEAT_ZhiKeZiMoHuPai) return false;
		if (m_decideGuoHu[pos]) return false;
		return true;
	}

	void add_gang_item(int pos, Card *card, int fire_pos, bool is_from_peng) {
		bool firer_has_ting = false;
		if (fire_pos != INVAILD_POS) firer_has_ting = m_tingState[fire_pos].m_ting_state == Ting_waitHu;		
		m_ps[pos].gangs.add(GangItem(ToTile(card), fire_pos, is_from_peng, firer_has_ting));		 
	}

	/******************************************************************
	* ��������    CheckYiMenPaiCanPG()
	* ������      ���һ�����������Ƿ���ƻ�һ����
	* ������
	*  @ pg_mask ��������
	*  @ outCard ��ǰ�������
	* ���أ�      (bool)
	*******************************************************************/
	bool CheckYiMenPaiCanPG(tile_mask_t pg_mask, const Card* curCard)
	{
		if (curCard == NULL)
		{
			return true;
		}
		tile_mask_t tmp_pgMask = pg_mask | TILE_TO_MASK(curCard->ToChar());
		int que_men_count = 0;
		if (0 == (tmp_pgMask & TILE_MASK_WAN)) ++que_men_count;
		if (0 == (tmp_pgMask & TILE_MASK_TONG)) ++que_men_count;
		if (0 == (tmp_pgMask & TILE_MASK_TIAO)) ++que_men_count;
		if (que_men_count >= 2)
		{
			return true;
		}
		return false;
	}

	// δ����״̬���Լ�����һ���ƣ�������Щ��ѡ����
	void CheckGetCardOperator(int pos, bool can_gang) {
		mGameInfo.b_CanHu = true;
		mGameInfo.b_CanTing = FEAT_BaoTing || m_playtype.b_baoTing;
		can_gang = can_gang && true;
		mGameInfo.b_CanAnGang = can_gang && true;
		mGameInfo.b_CanMingGang = can_gang && true;
		mGameInfo.b_CanDianGang = can_gang && false;
		mGameInfo.b_CanChi = false;

		mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
		mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
		mGameInfo.b_isHun = m_playtype.b_HaoZi;
		mGameInfo.m_GameType = FEAT_GAME_TYPE;
		mGameInfo.m_pos = pos;
		if (HongTongWangPai == FEAT_GAME_TYPE)
		{						
			if (m_playtype[PT_QueLiangMen]) // �鶴����ȱ2���淨�У���������������һ���ƣ������Ų�����ʾ����
			{				 
			}
			//�鶴����˫�����������е�˫������gameInfo.m_hCard (**Ren 2017-12-12) ��ʱע�͵���2018-01-08��
			/*if (m_playtype[MH_PT_HongTongWangPai_ShuangWang])
			{
				if (!is_invalid_card(m_hongTongWangPai_ShuangWang[pos]))
				{
					mGameInfo.m_hCard.push_back(m_hongTongWangPai_ShuangWang[pos]);
				}
			}*/
		}
		//����������Ŀ��Բ�������ʱ�����ã�
		/*if (!mGameInfo.b_CanTing)
		{
			m_checkTingInfo[pos].m_thinkData = gCardMgrSx.CheckCanTing_Check(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_suoHandCard[pos], m_curGetCard, mGameInfo, *this);
		}*/
		m_thinkInfo[pos].m_thinkData = gCardMgrSx.CheckGetCardOperator(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_suoHandCard[pos], m_curGetCard, mGameInfo, *this);
	}

	// δ����״̬�����˳���һ���ƣ��Լ�������Щ��ѡ����
	void CheckOutCardOperator(int pos) {
		// �÷�֧�����û��������״̬
		bool not_jin = TieJin != FEAT_GAME_TYPE || !IsHaozipai(m_curOutCard);
		// ���˳���һ���ƣ��������˿ɲ����Խ��ڣ�ֻ�����������淨�ǲ�������ڵģ���©�����Ҳ���ܽ��ڣ�������û�����ص����淨Ҫ����������Ҳ���ܽ��ڣ�
		mGameInfo.b_CanHu = basic_can_hu_shoupao(pos) && not_jin && !(m_playtype.b_zimoHu || m_louHuCard[pos] || (FEAT_BaoTing || m_playtype.b_baoTing));
		mGameInfo.b_CanTing = FEAT_BaoTing || m_playtype.b_baoTing;
		bool can_gang = not_jin;
		mGameInfo.b_CanAnGang = can_gang && false;
		mGameInfo.b_CanMingGang = can_gang && false;
		mGameInfo.b_CanDianGang = can_gang && m_GangThink != GangThink_qianggang;
		mGameInfo.b_CanPeng = not_jin && m_GangThink != GangThink_qianggang;		
		mGameInfo.b_CanChi = pos == GetNextPos(m_curPos) && m_GangThink != GangThink_qianggang;

		// ���ڹ�������Ҳ�����ʾ�ܺ���
		mGameInfo.b_CanDianGang = can_gang && (m_GangThink != GangThink_qianggang) && (!m_decideGuoHu[pos]);
		mGameInfo.b_CanPeng = not_jin && (m_GangThink != GangThink_qianggang) && (!m_decideGuoHu[pos]);

		mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
		mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
		mGameInfo.b_isHun = m_playtype.b_HaoZi;
		mGameInfo.m_GameType = FEAT_GAME_TYPE;

		//һ���Ƽ�������Ƿ���ƻ�һ���ƣ�**Ren 2017-12-23��
		if (LinFenYiMenPai == FEAT_GAME_TYPE)
		{
			if (false == CheckYiMenPaiCanPG(calc_pg_mask(m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos]), m_curOutCard))
			{
				mGameInfo.b_CanDianGang = false;
				mGameInfo.b_CanPeng = false;
			}
		}
		if (HongTongWangPai == FEAT_GAME_TYPE)
		{
			mGameInfo.b_CanPeng = !m_playtype[PT_HongTongBuKePengPai];
			mGameInfo.b_CanDianGang = !m_playtype[PT_HongTongBuKePengPai]; // �鶴���Ʋ������ƣ�Ҳ�������Ե��

			// �鶴����ȱ2���淨�У���������������һ���ƣ������Ų�����ʾ����
			if (m_playtype[PT_QueLiangMen])
			{
				tile_mask_t pg_mask = m_ps[pos].make_pg_mask();
				tile_mask_t out_tile_mask = TILE_TO_MASK(ToTile(m_curOutCard));

				//pg_mask &= ~TILE_TO_MASK(ToTile(mGameInfo.m_hCard[0])); // �ų�������(**Ren 2017-12-27)
				pg_mask &= ~TILE_MASK_ZIPAI;                       // �ų�����				
				if (VecHas(mGameInfo.m_hCard, m_curOutCard)) //������
				{
				}
				else
				{
					int quemen_count = m_ps[pos].get_quemen_count(pg_mask | out_tile_mask); // ����ȱ������					
					if (quemen_count < 2)      //����ȱ2�ţ������������͸�
					{
						mGameInfo.b_CanPeng = false;
						mGameInfo.b_CanDianGang = false;
					}
					else
					{
					}
				}
			}
		}
		// ���ݿ۵㣺�������Լ�����ǰ�������������� ��**Ren 2017-11-25��
		if (XinZhouKouDian == FEAT_GAME_TYPE && VecHas(m_guoPengCard[pos], m_curOutCard))
		{
			mGameInfo.b_CanPeng = false;
		}
		m_thinkInfo[pos].m_thinkData = gCardMgrSx.CheckOutCardOperator(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_suoHandCard[pos], m_curOutCard, mGameInfo, *this);
	}

	// ���ƺ��Լ����Ƴ��� ����������
	void CheckGetCardOpeartorAfterTing(int pos) {

		MHLOG("*******************************���ƺ��û�����***********���������û��ǲ��Ǻ��з���????");
		if (FEAT_GAME_TYPE == HongTongWangPai && m_bHongTongYingKouFirstOutCard[pos])
		{
			MHLOG("**********����Ӳ�۾��߹��ܣ��������û��ǲ��Ǻ��з���????");
			m_bHongTongYingKouFirstOutCard.clear();
		}
		mGameInfo.b_CanHu = true;
		// �ٵ㣺����ж��ƺ�û��Ҫ����ΪCardSx�Ǹ����Ѿ�����õ��������жϵģ��Ǹ������ǰ���������ģ�����������Ҫ�ж�
		if (KouDian == FEAT_GAME_TYPE) {
			if (!(m_curGetCard && (m_curGetCard->m_number >= 3 || m_curGetCard->m_color > 3 || IsHaozipai(m_curGetCard)))) mGameInfo.b_CanHu = false;
		}
		mGameInfo.b_CanTing = false;
		bool can_gang = FEAT_TingPaiKeGang || m_playtype[PT_TingPaiKeGang];
		mGameInfo.b_CanAnGang = can_gang && true;
		mGameInfo.b_CanMingGang = can_gang && true;
		mGameInfo.b_CanDianGang = can_gang && false;
		mGameInfo.b_CanChi = false;

		mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
		mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
		mGameInfo.m_GameType = FEAT_GAME_TYPE;

		//�����ӣ����ܸ��Ͽ�������� ��**Ren 2017-11-30��
		mGameInfo.b_getCardFromMAGang = true;
		if (NianZhongZi == FEAT_GAME_TYPE && 2 == m_getChardFromMAGang[pos])
		{
			mGameInfo.b_getCardFromMAGang = false;  //���Ϊ�������ƣ����Ͽ�����������
		}
		m_thinkInfo[pos].m_thinkData = gCardMgrSx.CheckGetCardOpeartorAfterTing(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_suoHandCard[pos], m_curGetCard, mGameInfo, *this);
		if (HongTongWangPai == FEAT_GAME_TYPE)
		{
			MHLOG("******Suijun print ThinkData pos = %d", pos);
			for (int i = 0; i < m_thinkInfo[pos].m_thinkData.size(); i++)
			{
				 
				if (m_thinkInfo[pos].m_thinkData[i].m_type == 2)
				{
					MHLOG("*****Suijun pos %d, has HU_THINK, Ҫ������ %d ", pos, 0xff);
					for (int j = 0; j < m_thinkInfo[pos].m_thinkData[i].m_hu.size(); j++)
					{
					   Hu_type const & hu = m_thinkInfo[pos].m_thinkData[i].m_hu[j];
					   MHLOG("*******************Suijun ���Ʒ���: hu.hu_type=%d ", hu.m_hu_type);
					}
					
				}
			}
		}
	}

	bool is_shangjin_shao(int pos) {
		// �����Ͻ�����ֻ������
		// �Ͻ��ٵı�׼�������������ϲ��Ҵ�����͵���һ��
		struct shangjin_lessthan {
			bool operator ()(PlayerState const &lhs, PlayerState const &rhs) { return lhs.shangjin < rhs.shangjin; }
		};
		int min_shangjin =  std::min_element(m_ps + 0, m_ps + m_player_count, shangjin_lessthan()) - m_ps;
		int max_shangjin =  std::max_element(m_ps + 0, m_ps + m_player_count, shangjin_lessthan()) - m_ps;
		int min_v = 999, max_v = 0, my_v = 0;
		for (int i = 0; i < m_player_count; ++i) {
			int v = m_ps[i].shangjin;
			if (min_v > v) min_v = v;
			if (max_v < v) max_v = v;
			if (i == pos) my_v = v;
		}
		return min_v != max_v && my_v == min_v;
	}

	// ���˳��ƣ�����Լ�����ʲô����
	void CheckOutCardOpeartorAfterTing(int pos) {		
		// �÷�֧����Ҵ�������״̬
		// ���𣺽��Ʋ�����������
		bool not_jin = TieJin != FEAT_GAME_TYPE || !IsHaozipai(m_curOutCard);
		// �����Ͻ�����ֻ�������������Ͻ��Ĳ�������
		bool shangjin_enough = TieJin != FEAT_GAME_TYPE || !m_playtype[PT_ShangJinShaoZheZhiKeZiMo] || !is_shangjin_shao(pos);
		// ���˳���һ���ƣ��������˿ɲ����Խ��ڣ�ֻ�����������淨�ǲ�������ڵģ���©�����Ҳ���ܽ��ڣ�
		mGameInfo.b_CanHu = basic_can_hu_shoupao(pos) && shangjin_enough && not_jin && !(m_playtype.b_zimoHu || m_louHuCard[pos] || (m_louZimohu[pos] && m_playtype[PT_GuoHuZhiKeZiMo]));
		//���ݿ۵㣺���˴������6�����ϲ�������ں�  (**Ren)
		if (KouDian == FEAT_GAME_TYPE || XinZhouKouDian == FEAT_GAME_TYPE) {
			// �ٵ㣺���˳���������ֻ�ܵ���ֵ����ʹ��
			bool koudian_can_hupai = m_curOutCard && (m_curOutCard->m_number >= 6 || m_curOutCard->m_color > 3);
			if (!koudian_can_hupai) mGameInfo.b_CanHu = false;
		}
		mGameInfo.b_CanTing = FEAT_BaoTing || m_playtype.b_baoTing;
		bool can_gang = not_jin && (FEAT_TingPaiKeGang || m_playtype[PT_TingPaiKeGang]);
		mGameInfo.b_CanAnGang = can_gang && true;
		mGameInfo.b_CanMingGang = can_gang && true;
		mGameInfo.b_CanDianGang = can_gang && true;
		mGameInfo.b_CanPeng = not_jin && false;
		mGameInfo.b_CanChi = false;
		// ���ڹ�������Ҳ�����ʾ�ܺ���
		mGameInfo.b_CanDianGang = can_gang && (m_GangThink != GangThink_qianggang) && (!m_decideGuoHu[pos]);
		if (FEAT_GAME_TYPE == HongTongWangPai)
		{
			mGameInfo.b_CanDianGang = !m_playtype[PT_HongTongBuKePengPai]; // �鶴���Ʋ������ƣ�Ҳ�������Ե��
		}		 

		mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
		mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
		mGameInfo.b_isHun = m_playtype.b_HaoZi;
		mGameInfo.m_GameType = FEAT_GAME_TYPE;
		m_thinkInfo[pos].m_thinkData = gCardMgrSx.CheckOutCardOpeartorAfterTing(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_suoHandCard[pos], m_curOutCard, mGameInfo, *this);
	}

#if USE_NEW_DEALING
#else
	void InitCard()
	{
		m_3_laizi.clear();
		//��
		Lint index = 0;
		for (Lint k = 0; k < 9; k++)  //9����
		{
			for (Lint i = 0; i < 4; ++i)		//ѭ�����Ĵ�
			{
				wan_base_card[index].m_color = 1;
				wan_base_card[index].m_number = k + 1;
				index++;
			}
		}
		//��
		index = 0;
		for (Lint k = 0; k < 9; k++)  //9����
		{
			for (Lint i = 0; i < 4; ++i)		//ѭ�����Ĵ�
			{

				bing_base_card[index].m_color = 2;
				bing_base_card[index].m_number = k + 1;
				index++;
			}
		}

		//��
		index = 0;
		for (Lint k = 0; k < 9; k++)  //9����
		{
			for (Lint i = 0; i < 4; ++i)		//ѭ�����Ĵ�
			{

				tiao_base_card[index].m_color = 3;
				tiao_base_card[index].m_number = k + 1;
				index++;
			}
		}
		for (Lint i = 0; i < BASE_CARD_COUNT; ++i)
		{
			m_3_laizi.push_back(&wan_base_card[i]);
			m_3_laizi.push_back(&bing_base_card[i]);
			m_3_laizi.push_back(&tiao_base_card[i]);
		}

		//��
		if (FEAT_DaiFeng || m_playtype.b_daiFeng)
		{
			index = 0;
			for (Lint k = 0; k < 7; k++)  //6����
			{
				for (Lint i = 0; i < 4; ++i)		//ѭ�����Ĵ�
				{

					zi_base_card[index].m_color = 4;
					zi_base_card[index].m_number = k + 1;
					index++;
				}
			}
			for (Lint i = 0; i < 28; ++i)
			{
				m_3_laizi.push_back(&zi_base_card[i]);
			}
		}
		gCardMgrSx.SortCard(m_3_laizi);
	}
#endif

	void SuijunPrintfFanxing()
	{
		MHLOG("********__COUNTER__ = %lld, FANXING_COUNTER_BASE = %lld ", FANXING_COUNTER_BASE - 1, FANXING_COUNTER_BASE);
		MHLOG("******* FAN_PingHu = %lld, FAN_ShiSanYao = %lld FAN_HaoHuaQiXiaoDui= %lld FAN_QiXiaoDui=%lld FAN_YiTiaoLong= %lld", FAN_PingHu, FAN_ShiSanYao, FAN_HaoHuaQiXiaoDui,
			FAN_QiXiaoDui, FAN_YiTiaoLong);
	}

	void start_round() {
		
		SuijunPrintfFanxing();
		clear_round();
		notify_desk_match_state();
		set_desk_state(DESK_PLAY);

		//֪ͨmanager�����仯
		if (m_desk && m_desk->m_vip&&m_desk->m_clubInfo.m_clubId!=0)
			m_desk->MHNotifyManagerDeskInfo(1, m_desk->m_vip->m_curCircle+1, m_desk->m_vip->m_maxCircle);

#if USE_NEW_DEALING
		DealCardByDesk();
#else
		DealCardByDesk();
		DealHaoZi();
#endif
		if(m_desk)
			m_desk->m_vip->ResetJinAnGangCount();
		CheckStartPlayCard();
		char buf[128] = {0};
		char buf2[128];
		MHLOG_PLAYCARD("****Desk %d start round %d/%d. flag=%d, player_count=%d", m_desk->GetDeskId(), m_round_offset, m_round_limit, FEAT_GAME_TYPE, m_player_count);
		for (int i = 0; i < m_player_count; i++) 
		{
			if (m_desk && m_desk->m_user[i])
			{
#ifdef WIN32
				strncpy_s((char*)buf, 128, m_desk->m_user[i]->GetUserData().m_nike.c_str(), 128);
				buf[127] = 0;
				MultiByteToWideChar(CP_UTF8, 0, buf, -1, (wchar_t*)buf2, 63);
				buf2[126] = buf2[127] = 0;
				WideCharToMultiByte(CP_ACP, 0, (wchar_t*)buf2, -1, buf, 127, NULL, NULL);
				buf[127] = 0;
#endif // WIN32	
				buf[127] = 0;
				MHLOG_PLAYCARD("****Desk %d user[pos:%d id:%d nike:%s]", m_desk->GetDeskId(), i, m_desk->m_user[i]->GetUserDataId(), buf);
			}
		}
	}

	int calc_haozi_count() {
		MHLOG("****Setup ��������****");
		if (KouDian == FEAT_GAME_TYPE) {
			if (m_playtype[PT_ShuangHaoZi]) return 2; // �ٵ�˫�����淨
			if (m_playtype.b_HaoZi) return 1; // �ٵ�׽�����淨
		} 
		else if (TieJin == FEAT_GAME_TYPE) {
			// ����Ĭ��4�����8��Сѡ�ѡ������8��
			if (m_playtype[PT_BaJin]) return 2; // ����8��
			else return 1; // ����4��
		}
		//�鶴���Ʋ���˫���������ѡ���ӣ�**Ren 2017-12-12��
		else if (HongTongWangPai == FEAT_GAME_TYPE /*&& !m_playtype[MH_PT_HongTongWangPai_ShuangWang]*/){
			return 1; // �鶴���ƣ����Ƶ����Ӵ���
		}
		// ���к���(**Ren2017-12-6)
		else if (m_playtype[MH_PT_HongZhongHaoZi]){
			return 1;
		}
		return 0;
	}
	int calc_haozi_style() {
		MHLOG("*****Enter setup ����ģʽ ****");
		switch (FEAT_GAME_TYPE) {
		//��ӿ۵�����
		case KouDian:  
			if (m_playtype[MH_PT_FengHaoZi]){
				return HAOAI_STYLE_KouDian_FengHaozi;
			}
			else{
				return HAOZI_STYLE_KouDian;
			}
			break;
		case TieJin: return HAOZI_STYLE_TieJin;
		case HongTongWangPai: return HAOZI_STYLE_HongTongWanPai;
		//�Ƶ������к���(**Ren 2017-12-6)
		case TuiDaoHu: 
			if (m_playtype[MH_PT_HongZhongHaoZi]) {
				return HAOZI_STYLE_HongZhongHaoZi;
			}
			return HAOZI_STYLE_NONE;
			break;
		default: return HAOZI_STYLE_NONE;
		}
	}

	TileDealing make_free_dealing() {
		// ϴ��  
		//���к��Ӳ����磬��Ӻ��е������У�**Ren 2017-12-18��
		tile_mask_t allowed_tile_set = (FEAT_DaiFeng || m_playtype[PT_DaiFeng]) ? TILE_MASK_VALID : TILE_MASK_SHUPAI;
		if (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi] && false == m_playtype[PT_DaiFeng])
		{
			allowed_tile_set |= TILE_TO_MASK(45);
		}
		TileDealing dealing(m_player_count, allowed_tile_set, 13, calc_haozi_count(), calc_haozi_style());
		return dealing;
	}

	void SuijunSetupPlayerCard()
	{
		MHLOG("****** SuijunSetupPlayerCard����*******");
		Card * pCard = m_desk->m_player1Card;
		//tile_t * pTile = NULL;
		// pTile = HTTestClass.tile_4men; //���Ų���������
		// pTile = HTTestClass.tile_3men; //���Ų���������
		// pTile = HTTestClass.tile_2men;
		// pTile = HTTestClass.tile_feng; //��ɸ�
		// pTile = HTTestClass.tile_sanyan; //�ֳ�
		// pTile = HTTestClass.tile_3men7xiaodui; // ����ȫ��С��
		//pTile = HTTestClass.tile_yingsanzui;


		std::vector<int> cardsVect;
		LoadPlayerCards(cardsVect, 1);	 
		for (int i = 0; i < 13; i++)
		{
			if (i < cardsVect.size())
			{
				pCard[i] = (tile_t)cardsVect[i];
			}
		}

		LoadPlayerCards(cardsVect, 2);
		pCard = m_desk->m_player2Card;
		for (int i = 0; i < 13; i++)
		{
			if (i < cardsVect.size())
			{
				pCard[i] = (tile_t)cardsVect[i];
			}
		}

		LoadPlayerCards(cardsVect, 3);
		pCard = m_desk->m_player3Card;
		for (int i = 0; i < 13; i++)
		{
			if (i < cardsVect.size())
			{
				pCard[i] = (tile_t)cardsVect[i];
			}
		}

		LoadPlayerCards(cardsVect, 4);
		pCard = m_desk->m_player4Card;
		for (int i = 0; i < 13; i++)
		{
			if (i < cardsVect.size())
			{
				pCard[i] = (tile_t)cardsVect[i];
			}
		}

		LoadPlayerCards(cardsVect, 5);
		for (int i = 0; i < 84; i++)
		{
			if (i < cardsVect.size())
			{
				m_desk->m_SendCard[i] = cardsVect[i];
			}
		}
		
		LoadPlayerCards(cardsVect, 6);
		for (int i = 0; i < 2; i++)
		{
			if (i <  cardsVect.size())
			{
				m_desk->m_HaoZiCard[i] = cardsVect[i];
			}
		}
	
	}

	TileDealing make_desk_dealing() {
		Lstring   strOut("");		
		MHLOG("****** Enter make_desk_dealing ��ʼϴ��");
		TileDealing dealing(make_free_dealing());
		bool can_assign = gConfig.GetDebugModel();
		MHLOG("******gConfig.GetDebugModel() = %d", can_assign);
		if (can_assign) {
			tile_t buf[34 * 4];
			size_t n;			
			SuijunSetupPlayerCard();
			n = SetTileArray(buf, m_desk->m_player1Card);
			Lint pos0 = m_desk->GetFirstZhuangPos();
			dealing.assign_to_player(pos0, buf, buf + n);
			n = SetTileArray(buf, m_desk->m_player2Card);
			Lint pos1 = (pos0 + 1) % 4;
			dealing.assign_to_player(pos1, buf, buf + n);
			n = SetTileArray(buf, m_desk->m_player3Card);
			Lint pos2 = (pos1 + 1) % 4;
			dealing.assign_to_player(pos2, buf, buf + n);
			n = SetTileArray(buf, m_desk->m_player4Card);
			Lint pos3 = (pos2 + 1) % 4;
			dealing.assign_to_player(pos3, buf, buf + n);
			n = SetTileArray(buf, m_desk->m_SendCard);
			dealing.assign_to_rest(buf, buf + n);
			dealing.finish();
			// ���� finish �� setup_haozi
			n = SetTileArray(buf, m_desk->m_HaoZiCard);
			dealing.setup_haozi(buf, buf + n);
		} else {
			dealing.finish();
			dealing.setup_haozi(NULL, NULL);
		}
		return dealing;
	}

	void DealCardByDesk() {
		MHLOG("**********Enter DealCardByDesk ����ϴ��---------------");
		// ϴ��
		TileDealing dealing(make_desk_dealing());

		dealing.DumpDeskInfo();
		dealing.DumpHaoziInfo();

		DealCard(dealing);
	}

	void DealCard(TileDealing &dealing) {
		// ���Է��֣�m_vip��m_curCircle��0��ʼ����ÿ�̽���ʱ���������һ�̽�����������m_maxCircle
		// ��m_vip->IsBegin()��ʵ�ֿ����������⣬���ж�m_curCircle�Ƿ�Ϊ0��������һ�ֿ�ʼ����false���ڶ��������ľֿ�ʼ����true
		// �����������⣬�����ʹ��û���⣬��һ�ַ���Ϊׯ���ڶ����Ժ����ﲻ�޸�ׯ
		if (m_round_offset > 0) {
			// �ڶ����Ժ��������޸�ׯ
		} else {
			// ��һ�ַ���Ϊׯ
			m_zhuangpos = m_desk->GetFirstZhuangPos();//L_Rand(0, m_player_count - 1);
		}
		m_curPos = m_zhuangpos;

#if USE_NEW_DEALING

		// �������ĵ�����
		for (int i = 0; i < dealing.player_count; ++i) {
			TileVec &hands = dealing.players[i];
			CardVector &suo_cards = m_suoHandCard[i];
			// ������δ���򣬺͵���˳��һ�£�ȡǰ������Ϊ����
			for (Lsize j = 0; j < FEAT_SUO_COUNT; ++j) {
				suo_cards.push_back(card_pool[hands[j]]);
			}
			// ����ɾ������Ϊ�ڷ����������ư�������
			//hands.pop_front(FEAT_SUO_COUNT);
		}

		// ��������
		// ���ﲻ�����򣬻�Ӱ�������ͻ��˴����ĵ��ƣ��ͻ��˻��Լ���������ģ����÷��������ź������
		//dealing.sort_hands();
		for (int i = 0; i < dealing.player_count; ++i) {
			CardVector &hand_cards = m_handCard[i];
			TileVec &hands = dealing.players[i];
			for (Lsize j = 0; j < hands.size(); ++j) {
				hand_cards.push_back(card_pool[hands[j]]);
			}
		}

		// ��������
		for (Lsize j = 0; j < dealing.rest.size(); ++j) {
			m_deskCard.push_back(card_pool[dealing.rest[j]]);
		}

		// ���ú���
		for (size_t j = 0; j < dealing.haozi_count; ++j) {
			mGameInfo.m_hCard.push_back(ToCard(dealing.haozi_tiles[j]));
		}

		if (mGameInfo.m_hCard.size() == 1) {
			int desk_id = m_desk ? m_desk->GetDeskId() : 0;
			Card haozi0 = mGameInfo.m_hCard[0];
			LLOG_ERROR("Desk %d create 1 haozi %d|%d", desk_id, haozi0.m_color, haozi0.m_number);
		} else if (mGameInfo.m_hCard.size() == 2) {
			int desk_id = m_desk ? m_desk->GetDeskId() : 0;
			Card haozi0 = mGameInfo.m_hCard[0];
			Card haozi1 = mGameInfo.m_hCard[1];
			LLOG_ERROR("Desk %d create 2 haozi %d|%d, %d|d", desk_id, haozi0.m_color, haozi0.m_number, haozi1.m_color, haozi1.m_number);
		}

		// ׯ�Ҷ෢һ����
		if (!m_deskCard.empty()) {
			Card *newCard = m_deskCard.back();
			m_deskCard.pop_back();
			// ��ط��Զ����������������
			add_hand_card(m_curPos, newCard);
			// Ҳ�� dealing ���£�������ͻ��˴���Ϣ��
			dealing.players[m_curPos].add(ToTile(newCard));
		}

#else
		InitCard();
		//����   
		if (gConfig.GetDebugModel() && (m_desk->m_player1Card[0].m_color>0 || m_desk->m_player2Card[0].m_color>0 || m_desk->m_player3Card[0].m_color>0 || m_desk->m_player4Card[0].m_color>0 || m_desk->m_SendCard[0].m_color>0)) {
			// ����ָ�����Ƶ��ƾ�
			gCardMgrSx.DealCard2(m_3_laizi, m_handCard, m_player_count, m_deskCard, m_desk->m_player1Card, m_desk->m_player2Card, m_desk->m_player3Card, m_desk->m_player4Card, m_desk->m_SendCard, m_desk->getGameType(), m_playtype);
		} else {
			// ����������Ƶ��ƾ�
			gCardMgrSx.DealCard(m_3_laizi, m_handCard, m_player_count, m_deskCard, m_desk->getGameType(), m_playtype);
		}

		// ׯ�Ҷ෢һ����
		Card *newCard = m_deskCard.back();
		m_deskCard.pop_back();
		add_hand_card(m_curPos, newCard);

		// �������ĵ�����
		for (Lint i = 0; i < m_player_count; ++i) {
			CardVector const &hand_cards = m_handCard[i];
			CardVector &suo_cards = m_suoHandCard[i];
			for (Lsize j = 0; j < FEAT_SUO_COUNT; ++j) {
				suo_cards.push_back(hand_cards[j]);
			}
		}
#endif

		// ��Ϊ���ƻᷢ���仯������ֱ���� mGameInfo.m_suoCard ָ�� m_suoHandCard
		mGameInfo.m_suoCard = m_suoHandCard;

		// ������Ϣ���ͻ���
		for (Lint i = 0; i < m_player_count; ++i) {
			LMsgS2CPlayStart msg;
			msg.m_zhuang = m_curPos;
			msg.m_pos = i;
			// ����������ҵ��ۼƷ���
			for (int x = 0; x < m_player_count; x++) {
				msg.m_score.push_back(m_accum_score[x] + ShanXi_JiaDe1000FenZhiZaiTouXiangChuXianShi);
			}
			CardVector const &hand_cards = m_handCard[i];
			// m_handCard �������ˣ��� dealing �ĸ��ͻ��˷�
			TileVec &hands = dealing.players[i];
			CardVector &suo_cards = m_suoHandCard[i];
			for (Lsize j = 0; j < hands.size(); ++j) {
				if (j < FEAT_SUO_COUNT) {
					// ���ͻ��˵�����
					msg.m_cardSuoValue[j] = ToCardValue(hands[j]);
				} else {
					// ���ͻ��˵����Ʋ���������
					Lint index = j - FEAT_SUO_COUNT;
					msg.m_cardValue[index] = ToCardValue(hands[j]);
				}
			}

			for (Lint j = 0; j < m_player_count; ++j) {
				msg.m_cardCount[j] = m_handCard[j].size();
			}
			msg.m_dCount = (Lint)m_deskCard.size();

			//�����¼������ֶ� by wyz
			msg.m_gamePlayerCount=m_player_count;
			if (m_desk!=NULL)   //������ѡ�����ﻹ��Ҫ��
			{
				for (Lint i=0; i<m_player_count; ++i)
				{
					if (m_desk->m_user[i])
						msg.m_posUserid[i]=m_desk->m_user[i]->GetUserDataId();
				}
			}
			// ����Ϣ����������
			notify_user(msg, i);
			MHLOG("**********Send to client ��Ϸ��ʼ LMsgS2CPlayStart chaird=%d", i);
		}

		//¼����
		VideoAdd__deal_card();

#if USE_NEW_DEALING
		// ����к��Ӿ�ˢ���ͻ��ˡ���¼¼��
		if (!mGameInfo.m_hCard.empty()) {
			MHLOG("************ Send to client ������ LLMsgS2CUserHaozi"); 
			notify_HaoZi();
			//���Է��ֺ�����Ϣ�����־�����Ϣ��Ӱ���2�ֿ�ʼʱ�ͻ��˽������־�����Ϣ����ʱ���ӳ��ٲ���
			if (HongTongWangPai == FEAT_GAME_TYPE)
			{
				//Sleep(250);
			}
			// ¼�����
			VideoAdd__HaoZi(mGameInfo.m_hCard);
		}
#endif
	}

	// ����8��ϵͳÿ���������2����Ϊ������Ƶ�8�Ŷ��ǽ𣬵��������Ʊ����Ǽ�����ڵġ�
	// ����3��5����4��3���4��5���ǽ�Ҳ����3��5Ͳ������������ʱ�������ָ���ɡ���ɫ�����ơ�
	// ����Ľ𣺶�������Ϊһ���ƣ��з���Ϊһ���ƣ���Ϊ8����������ԣ����аװ塢�������硢�Ϸ籱�綼��8�������Ĳ��С�
	std::pair<tile_t, tile_t> tiejin_generate_bajin() {
		// 7x3x3 + 3 = 66 �ֿ���
		int i = L_Rand(0, 65);
		tile_t jin1, jin2;
		if (i == 65) jin1 = 45, jin2 = 47; // ��
		else if (i == 64) jin1 = 42, jin2 = 44; // ��
		else if (i == 63) jin1 = 41, jin2 = 43; // ��
		else {
			int c2 = i / 21;
			i %= 21;
			int c1 = i / 7;
			i %= 7;
			jin1 = (tile_t)((c1+1)*10 + i);
			jin2 = (tile_t)((c2+1)*10 + i + 2);
		}
		return std::make_pair(jin1, jin2);
	}

	void DealHaoZi() {
		tile_t buf[2];
		size_t n = 0;
		bool can_assign = gConfig.GetDebugModel();
		if (can_assign) {
			n = SetTileArray(buf, m_desk->m_HaoZiCard);
		}
		DealHaoZiInternal(buf, n);
	}

	void DealHaoZiInternal(tile_t buf[], size_t n) {
		if (KouDian == FEAT_GAME_TYPE && m_playtype.b_HaoZi) {
			// �ٵ�׽�����淨
			goto haozi1;
		} else if (TieJin == FEAT_GAME_TYPE) {
			// ����Ĭ��4�����8��Сѡ�ѡ������8��
			if (m_playtype[PT_BaJin]) {
				// ����8��
				goto haozi2;
			} else {
				// ����4��
				goto haozi1;
			}
		}
haozi1:
		if (n) {
			mGameInfo.m_hCard.push_back(ToCard(buf[0]));
		} else {
			// L_Rand(begin, end) ����ĩ��
			int index = L_Rand(0, m_deskCard.size() - 1);
			mGameInfo.m_hCard.push_back(ToCardValue(m_deskCard[index]));
		}
		goto haozi0;
haozi2:
		if (n) {
			mGameInfo.m_hCard.push_back(ToCard(buf[0]));
			mGameInfo.m_hCard.push_back(ToCard(n >= 2 ? buf[1] : buf[0] + 2));
		} else {
			std::pair<tile_t, tile_t> bajin = tiejin_generate_bajin();
			mGameInfo.m_hCard.push_back(TileToCard(bajin.first));
			mGameInfo.m_hCard.push_back(TileToCard(bajin.second));
		}
		goto haozi0;
haozi0:

		// ����к��Ӿ�ˢ���ͻ��ˡ���¼¼��
		if (!mGameInfo.m_hCard.empty()) {
			notify_HaoZi();
			// ¼�����
			VideoAdd__HaoZi(mGameInfo.m_hCard);
		}
	}

	/*********************************************************************************
	* ��������     hongTongWangPaiShuangWang()
	* ������       ���������е����ż�������ͬ���Ƹ��ͻ���ѡ����������ThinkTool�ṹ
	* ������
	*  @ pos ���λ��
	*  @ hand_cards �������
	* ���أ�       (void)
	**********************************************************************************/
	void hongTongWangPaiShuangWang(int pos, const CardVector& hand_cards)
	{
		CardVector tmp_hand_cards(hand_cards);
		CardVector tmp_same;  //�洢�ظ�����
		gCardMgrSx.SortCard(tmp_hand_cards);
		if (tmp_hand_cards.empty())
		{
			return;
		}
		Lint color = tmp_hand_cards.front()->m_color;
		Lint number = tmp_hand_cards.front()->m_number;
		auto it = tmp_hand_cards.begin();
		auto it_end = tmp_hand_cards.end();
		for (; it != it_end;)
		{
			if (color == (*it)->m_color && number == (*it)->m_number && !(tmp_same.back() == *it))
			{
				tmp_same.push_back(*it);
			}
			else
			{
				color = (*it)->m_color;
				number = (*it)->m_number;
			}
			++it;
		}
		ThinkUnit think_wang;
		//ѡ��˼��
		think_wang.m_type = THINK_OPERATOR_HONGTONG_XUANWANG;
		VecExtend(think_wang.m_card, tmp_same);
		m_thinkInfo[pos].m_thinkData.push_back(think_wang);
	}

	/********************************************************
	* ��������   CheckStartPlayCard()
	* ������     ��Ϸ��ʼ
	* ���أ�     (void)
	*********************************************************/
	void CheckStartPlayCard()
	{
		//�鶴����˫����ѡ���Ĺ��̣�����ׯ���Ѿ��෢��һ����14���ƣ�**Ren 2017-12-12����ʱע�͵���2018-01-08��
		//if (HongTongWangPai == FEAT_GAME_TYPE && m_playtype[MH_PT_HongTongWangPai_ShuangWang])
		//{
		//	//����ÿ�������������ż��������ϵ���
		//	for (int pos = 0; pos < m_player_count; ++pos) 
		//	{
		//		hongTongWangPaiShuangWang(pos, m_handCard[pos]);
		//	}
		//	//����Ϣ���͸��ͻ��ˣ����û�ѡ��
		//	set_play_state(DESK_PLAY_THINK_CARD);
		//	SendThinkInfoToClient();

		//	//��������ѡ���Ľ�����͸��ͻ���
		//	notify_desk_xuanwang();
		//}

		SetPlayIng(m_curPos, false, true, true, true);
		m_curGetCard = m_handCard[m_curPos].back();
		//m_handCard[m_curPos].pop_back();
		m_needGetCard = true;
	}


	struct SetPlayIng__Options {
		bool needGetCard;
		bool needting;
		bool needotherthink;
		bool first_think;
		Lint pengPos;
		SetPlayIng__Options(): needGetCard(false), needting(false), needotherthink(false), first_think(false), pengPos(INVAILD_POS) {}
	};

	inline void SetPlayIng_SetTimeBegin(Lint pos)
	{
		LTime cur;
		m_getcardTimeBegin[pos] = cur.MSecs();
	}

	//����
	void SetPlayIng(Lint pos, bool needGetCard, bool needting, bool needotherthink, bool first_think = false) {
		SetPlayIng_SetTimeBegin(pos); //SUIJUN
		SetPlayIng__Options opt;
		opt.needGetCard = needGetCard;
		opt.needting = needting;
		opt.needotherthink = needotherthink;
		opt.first_think = first_think;
		SetPlayIng__opt(pos, opt);

		while(true)
		{
			LTime end;
			m_getcardTimeEnd[pos] = end.MSecs();
			LLOG_ERROR("****SetPlayIng Desk:%d pos:%d calc_time:%ld ms, ting_state:%d", m_desk ? m_desk->GetDeskId():-1, pos, m_getcardTimeEnd[pos] - m_getcardTimeBegin[pos], m_tingState[pos].m_ting_state == Ting_waitOutput ? 1 : 0);
			break;
		}
	}

	void SetPlayIng__opt(Lint pos, const SetPlayIng__Options& opt) {
		if (m_desk == NULL) {
			LLOG_DEBUG("HanderUserEndSelect NULL ERROR ");
			return;
		}
		if (pos < 0 || pos >= INVAILD_POS) {
			LLOG_ERROR("Desk::SetPlayIng pos error ��");
			return;
		}
		// �鶴ʣ�N14����
		if ((m_deskCard.size() <= m_remain_card_count && opt.needGetCard) )
		{
			finish_round(WIN_NONE, INVAILD_POS, INVAILD_POS, NULL);
			return;
		}
		// ���ݿ۵㣺ʣ��12���� (**Ren 2017-11-22)
		/*if (XinZhouKouDian == FEAT_GAME_TYPE && (m_deskCard.size() <= 12 && opt.needGetCard))
		{
			finish_round(WIN_NONE, INVAILD_POS, INVAILD_POS, NULL);
			return;
		}*/
		// ��ׯ
		if (m_deskCard.empty() && opt.needGetCard) {
			finish_round(WIN_NONE, INVAILD_POS, INVAILD_POS, NULL);
			return;
		}
		m_curPos = pos;
		m_thinkInfo[pos].m_thinkData.clear();
		set_play_state(DESK_PLAY_GET_CARD);
		m_needGetCard = false;
		if (opt.needGetCard) {
			// �Ѿ�������
			if (m_tingState[m_beforePos].m_ting_state == Ting_waitHu) {

			}
			m_needGetCard = true;
			m_curGetCard = m_deskCard.back();
			m_deskCard.pop_back();
			//¼��
			VideoAdd__pos_oper_card(pos, VIDEO_OPER_GET_CARD, m_curGetCard);
			m_tingState[pos].m_out_card = m_curGetCard;
		}
		if (opt.needotherthink) {
			if (m_tingState[pos].m_ting_state == Ting_waitHu) {
				CheckGetCardOpeartorAfterTing(pos);
			} else {
				CheckGetCardOperator(pos, true);
			}
			VideoThink(pos);
		} else if (opt.needting) {
			CheckGetCardOperator(pos, false);
			VideoThink(pos);
		}
		if (pos == opt.pengPos && opt.pengPos != INVAILD_POS && (FEAT_BaoTing || m_playtype.b_baoTing)) {
			// ���ƺ����Ƿ������
			mGameInfo.b_CanChi = false;
			mGameInfo.b_CanHu = true;
			mGameInfo.b_CanAnGang = false;
			mGameInfo.b_CanMingGang = false;
			mGameInfo.b_CanDianGang = false;
			mGameInfo.b_CanTing = FEAT_BaoTing || m_playtype.b_baoTing;

			mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
			mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
			mGameInfo.b_isHun = m_playtype.b_HaoZi;
			mGameInfo.m_GameType = FEAT_GAME_TYPE;
			mGameInfo.m_pos = pos;
			m_thinkInfo[pos].m_thinkData = gCardMgrSx.CheckAfterPeng(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_curOutCard, mGameInfo, *this);
			bool think = !m_thinkInfo[pos].m_thinkData.empty();
			if (think) {
				mGameInfo.b_CanChi = false;
				mGameInfo.b_CanHu = true;
				mGameInfo.b_CanAnGang = false;
				mGameInfo.b_CanMingGang = false;
				mGameInfo.b_CanDianGang = false;
				mGameInfo.b_CanTing = FEAT_BaoTing || m_playtype.b_baoTing;

				mGameInfo.b_ZiMoHu = m_playtype.b_zimoHu;
				mGameInfo.b_DaiFeng = m_playtype.b_daiFeng;
				mGameInfo.b_isHun = m_playtype.b_HaoZi;
				mGameInfo.m_GameType = FEAT_GAME_TYPE;
				VideoThink(pos);
			}
		}

		//���ڲ��������淨����Ӳ������ܣ�**Ren 2017-12-27������ʱ�����ã�
		/*if (pos == opt.pengPos && opt.pengPos != INVAILD_POS && !(FEAT_BaoTing || m_playtype.b_baoTing))
		{
			m_checkTingInfo[pos].m_thinkData = gCardMgrSx.CheckAfterPeng(m_ps[pos], m_tingState[pos], m_handCard[pos], m_pengCard[pos], m_angangCard[pos], m_minggangCard[pos], m_eatCard[pos], m_curOutCard, mGameInfo, *this);
		}*/

		if (m_needGetCard) {
			add_hand_card(pos, m_curGetCard);
			MHLOG_PLAYCARD("****Desk:%d ������� pos: %d, card=%d[%s]", m_desk ? m_desk->GetDeskId() : 0, pos, ToTile(m_curGetCard), (*m_curGetCard).ToString().c_str());
		}

		SetPlayIng__update_client(pos, opt);
	}

	// �к����Ƶ���������Ϻ����ƣ���Ӱ�����ڣ�ֻ�Ǹ��ͻ�����ʾ�����ƣ�
	void Extend_TingChuKou_Haozi(LMsgS2COutCard & msg)
	{
		//����Ƶ������к����ƣ�**Ren 2017-12-07��
		if (FEAT_GAME_TYPE != KouDian && !(FEAT_GAME_TYPE == TuiDaoHu && m_playtype[MH_PT_HongZhongHaoZi]))
			return;
		for (size_t m = 0; m < mGameInfo.m_hCard.size(); m++)
		{
			tile_t hu_tile = ToTile(mGameInfo.m_hCard[m]);
			for (size_t n = 0; n < msg.kTingChuKou.size(); n++)
			{
				if (!VecHas(msg.kTingChuKou[n].kTingKou, hu_tile))
				{
					msg.kTingChuKou[n].kTingKou.push_back(hu_tile);
				}
			}
		}
	}

	void SetPlayIng__update_client(Lint pos, const SetPlayIng__Options& opt) {
		for (Lint i = 0; i < m_player_count; ++i) {
			LMsgS2COutCard msg;
			MHLOG("SetPlayIng__update_client m_round_offset =  %d", m_round_offset);

			msg.kTime = 15;
			msg.kPos = pos;
			msg.kDCount = (Lint)m_deskCard.size();
			msg.kTing = m_tingState[i].m_ting_state == Ting_waitHu ? 1 : 0;
			msg.kFlag = 1;

			if (m_needGetCard) {
				msg.kFlag = 0;
			}

			if (pos == i) {
				if (m_needGetCard) {
					msg.SetCurCard(ToCardValue(m_curGetCard));
					if (msg.kTing) {
						//msg.m_tingCards.push_back(msg.GetCurCard());
						m_tingState[pos].m_out_card = m_curGetCard; // ��֪��ǰ������û�����������һ��
						LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
						//�۵������Ҫ���������
						if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
						{
							Extend_TingChuKou_Haozi(msg);
						}
					}
				}
				ThinkVec &units = m_thinkInfo[pos].m_thinkData;
				MHLOG_TING("**** pos = %d, m_thinkInfo[pos].m_thinkData.size() = %d", pos , m_thinkInfo[pos].m_thinkData.size());
				for (Lsize j = 0; j < units.size(); ++j) {
					ThinkData info;
					info.kType = units[j].m_type;
					if (opt.first_think && info.kType == THINK_OPERATOR_BOMB && !m_handCard[pos].empty()) {
						Card *back = m_handCard[pos].back();
						if (back) {
							units[j].m_card.push_back(back);
							msg.SetCurCard(ToCardValue(back));
							MHLOG_PLAYCARD("****Desk:%d  ���˼�� pos:%d hu_card:%d", m_desk ? m_desk->GetDeskId() : 0, pos, ToTile(back));
						}
					}
#ifdef USE_NEW_TING
					MHLOG_TING("****j = %d info.m_type = %d", j, info.kType);
					// ��������ʱ����ҵ����ڣ��������ݴ������
					if (info.kType == THINK_OPERATOR_TING) // SUIJUN���������߼�
					{
						MHLOG_TING("****SUIJUN���������߼�,��������ʱ����ҵ����ڣ��������ݴ������pos=%d msg.m_ting = %d m_ting_state = %d",pos, msg.m_ting, m_tingState[i].m_ting_state);
						m_tingState[pos].m_ting_state = Ting_waitOutput;
						this->LMsgS2COutCard_setup_TingChuKou_waitOutput(msg, units[j].ting_unit);
						MHLOG_TING("****units[j].ting_unit.size() = %d", units[j].ting_unit.size());
						for (int k = 0; k < units[j].ting_unit.size(); k++)
						{
							const TingUnit & tu = units[j].ting_unit[k];
							MHLOG_TING("****������k:%d, m_out_card: %d", k, ToTile(tu.out_card));
							for (int m = 0; m < tu.hu_units.size(); m++)
							{
								MHLOG_TING("*******����m:%d, hu_units[m].hu_card: %d", m, ToTile(tu.hu_units[m].hu_card));
							}
						}

						//�۵������Ҫ���������
						if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
						{	
							Extend_TingChuKou_Haozi(msg);
						}
						
					}
#endif
					VecExtend(info.kCard, units[j].m_card);				 
					//�۵������Ҫ���������
					msg.kThink.push_back(info);
				}
				//���������淨��Ӳ������ܣ�**Ren 2017-12-27������ʱ�����ã�
				/*ThinkVec &check_ting_units = m_checkTingInfo[pos].m_thinkData;
				for (Lsize j = 0; j < check_ting_units.size() && !(FEAT_BaoTing || m_playtype.b_baoTing); ++j)
				{
					msg.kCheckTing = 1;
					this->LMsgS2COutCard_setup_TingChuKou_waitOutput(msg, check_ting_units[j].ting_unit);
				}*/
			}
			notify_user(msg, i);
		}
	}


	// ��������
	void GetCard_AfterTing(Lint position, bool need_getCard = true) {
		if (m_desk == NULL) {
			LLOG_DEBUG("GetCard_AfterTing NULL ERROR ");
			return;
		}
		if (!is_pos_valid(position)) {
			LLOG_ERROR("Desk::GetCard_AfterTing pos error!");
			return;
		}
		// ���ݿ۵㣺ʣ��12���� (**Ren 2017-11-22)
		if (XinZhouKouDian == FEAT_GAME_TYPE && (m_deskCard.size() <= 12 && need_getCard))
		{
			finish_round(WIN_NONE, INVAILD_POS, INVAILD_POS, NULL);
			return;
		}
		// ��ׯ
		if (m_deskCard.empty() && need_getCard) {
			finish_round(WIN_NONE, INVAILD_POS, INVAILD_POS, NULL);
			return;
		}
		m_curPos = position;
		m_thinkInfo[position].m_thinkData.clear();
		set_play_state(DESK_PLAY_GET_CARD);
		m_needGetCard = false;
		if (need_getCard) {
			m_needGetCard = true;
			m_curGetCard = m_deskCard.back();
			m_deskCard.pop_back();
			// ¼��
			VideoAdd__pos_oper_card(position, VIDEO_OPER_GET_CARD, m_curGetCard);

			std::vector<TingUnit> &units = m_tingState[position].ting_unit;
			for (size_t x = 0; x < units.size(); x++) {
				if (!units[x].out_card) units[x].out_card = m_curGetCard;
			}

			CheckGetCardOpeartorAfterTing(position);

			VideoThink(position);
			add_hand_card(position, m_curGetCard);
		}
		MHLOG_TING("**********�����߼� �����ߵ�������߼�����GetCard_AfterTing******************* pos = %d", position);
		m_tingState[position].m_ting_state = Ting_waitOutput;

		for (Lint i = 0; i < m_player_count; ++i) {
			LMsgS2COutCard msg;
			msg.kTime = 15;
			msg.kPos = position;
			msg.kDCount = (Lint)m_deskCard.size();
			msg.kTing = 1;
			msg.kFlag = 1;
			if (m_needGetCard) {
				msg.kFlag = 0;
			}

			if (position == i) {
				int pos = position;
				if (m_needGetCard) {
					msg.SetCurCard(ToCardValue(m_curGetCard));
					//msg.m_tingCards.push_back(msg.GetCurCard());
					m_tingState[pos].m_out_card = m_curGetCard; // ��֪��ǰ������û�����������һ��
					LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
					//�۵������Ҫ���������
					if (KouDian == FEAT_GAME_TYPE ||(TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
					{
						Extend_TingChuKou_Haozi(msg);
					}

					ThinkVec const &units = m_thinkInfo[position].m_thinkData;
					for (Lsize j = 0; j < units.size(); ++j) {
						ThinkData info;
						info.kType = units[j].m_type;
						VecExtend(info.kCard, units[j].m_card);
						msg.kThink.push_back(info);
					}
				} else {
					m_tingState[pos].m_out_card = m_curGetCard; // ��֪��ǰ������û�����������һ��
					LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
					//�۵������Ҫ���������
					if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
					{
						Extend_TingChuKou_Haozi(msg);
					}
				}
			}
			notify_user(msg, i);
		}
	}


	void ThinkEnd(Lint hu_position = INVAILD_POS, Lint ting_position = INVAILD_POS)
	{
		if (!m_desk)
			return;
		for (int i = 0; i < m_player_count; i++)
		{
			if (m_thinkInfo[i].NeedThink())
			{
				VideoDoing(99, i, 0, 0);
			}
			m_thinkInfo[i].Reset();
		}
		if (m_GangThink == GangThink_qianggang)
		{
			m_GangThink = GangThink_gangshangpao;
		}
		else if (m_GangThink == GangThink_gangshangpao)
		{
			m_GangThink = GangThink_over;
		}
		//���˺�
		if (hu_position != INVAILD_POS)
		{
			Card* winCard = m_thinkRet[hu_position].m_card.empty() ? NULL : m_thinkRet[hu_position].m_card.front();
			finish_round(WIN_BOMB, hu_position, m_beforePos, winCard);
			return;
		}

		//������
		if (ting_position != INVAILD_POS)
		{
			//m_louHuCard[ting_position] = 0;
			notify_desk_passive_op(ting_position);
			for (int i = 0; i < m_player_count; i++)
			{
				m_thinkRet[i].Clear();
			}
			return;
		}

		Lint pengPos = INVAILD_POS;
		Lint gangPos = INVAILD_POS;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_thinkRet[i].m_type == THINK_OPERATOR_MGANG)
			{
				gangPos = i;
				m_minggang[gangPos] += 1;
			}
			else if (m_thinkRet[i].m_type == THINK_OPERATOR_PENG)
			{
				pengPos = i;
			}
		}

		//�ܣ����ܡ���ܣ�
		// ����ֻ�е�ܰɣ��������� HanderUserPlayCard
		if (gangPos != INVAILD_POS)
		{
			//m_louHuCard[gangPos] = 0;
			ThinkUnit gangUnitRet;
			notify_desk_passive_op(gangPos);
			erase_suo_card(gangPos, m_thinkRet[gangPos].m_card[0], 3);

			//¼��
			VideoAdd__pos_oper_card_n(gangPos, VIDEO_OPER_GANG, m_curOutCard, 4);
			for (int i = 0; i < 4; i++)
			{
				m_minggangCard[gangPos].push_back(m_curOutCard);
			}
			add_gang_item(gangPos, m_curOutCard, m_beforePos, false); // �������
			MHLOG_PLAYCARD("****Desk:%d �������pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, gangPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());

			m_diangang[gangPos] += 1;
			m_adiangang[m_beforePos] += 1;
			//��Ӹ��Ͽ������ܣ��޸ĸܺ����Ʊ��Ϊ���ܺ����ƣ�**Ren 2017-12-11��
			m_getChardFromMAGang[gangPos] = 2;

			//�鶴������Ҫʹ��˼����Ԫ��������ܺ��ڣ� For all type
			//if (FEAT_GAME_TYPE == HongTongWangPai) 
			{
				gangUnitRet = m_thinkRet[gangPos];
			}
			for (int i = 0; i < m_player_count; i++)
			{
				m_thinkRet[i].Clear();
			}
			//�������һ����
			m_GangThink = GangThink_gangshangpao;

			if (m_tingState[gangPos].m_ting_state == Ting_waitHu)
			{	
				//�鶴������������ܺ���, for all type
				//if (FEAT_GAME_TYPE == HongTongWangPai) 
				{
					MHLOG("************�鶴������������ܺ���FEAT_GAME_TYPE == HongTongWangPai**********************");
					if (gangUnitRet.m_type == THINK_OPERATOR_MGANG && gangUnitRet.ting_unit.size() > 0 && gangUnitRet.ting_unit[0].hu_units.size() > 0)
					{
						m_tingState[gangPos].hu_units.clear();
						m_tingState[gangPos].hu_units = gangUnitRet.ting_unit[0].hu_units;
					}
				}
				SetPlayIng(gangPos, true, false, true);
			}
			else {
				SetPlayIng(gangPos, true, true, true);
			}
			return;
		}

		if (pengPos != INVAILD_POS)
		{
			// pengPos�����Ƶ���
			//m_louHuCard[pengPos] = 0;
			notify_desk_passive_op(pengPos);
			erase_suo_card(pengPos, m_curOutCard, 2);
			// �������������е��ƣ�ɾ��2��ͬ���ģ�����3��һ���ĵ�m_knowCardCount
			InsertIntoKnowCard(m_curOutCard, 3);

			//¼��
			VideoAdd__pos_oper_card_n(pengPos, VIDEO_OPER_PENG_CARD, m_curOutCard, 3);
			for (int i = 0; i < 3; i++)
			{
				m_pengCard[pengPos].push_back(m_curOutCard);
			}

			// �۵��Ƶ���:������������Ȼ�е�ǰ�����ƣ���¼�������Ժ󶼲����ٸ�
			if (VecHas(m_handCard[pengPos], ToCard(m_curOutCard)))
			{
				m_Cannot_gangCard_MH[pengPos].push_back(m_curOutCard);
				mGameInfo.m_cards_guo_gang_MH.push_back(ToCard(m_curOutCard));
				MHLOG_PLAYCARD("****Desk:%d ����и����� pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, pengPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}

			// churunmin: m_beforePosӦ�þ�����ǰ���Ƶ��Ǹ���
			if (m_beforePos != INVAILD_POS) {
				m_ps[pengPos].pengs.add(PengItem(CardToTile(m_curOutCard), m_beforePos));
				MHLOG_PLAYCARD("****Desk:%d ������� pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, pengPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}

			for (int i = 0; i <m_player_count; ++i) {
				m_thinkRet[i].Clear();
			}

			//�����һ����
			m_curGetCard = NULL;
			SetPlayIng__Options opt;
			opt.pengPos = pengPos;
			SetPlayIng__opt(pengPos, opt);
			m_needGetCard = true;
			return;
		}

		//����û���˲���
		if (m_beforeType == THINK_OPERATOR_MGANG)
		{
			//¼��
			//int video_oper = m_beforeType == THINK_OPERATOR_MGANG ? VIDEO_OPER_MING_GANG : VIDEO_Oper_Ming1GangTing;
			int video_oper = VIDEO_OPER_MING_GANG;
			VideoAdd__pos_oper_card_n(m_beforePos, video_oper, m_curOutCard, 4);

			m_minggangCard[m_beforePos].push_back(m_curOutCard);
			CardVector::iterator it = m_pengCard[m_beforePos].begin();
			for (; it != m_pengCard[m_beforePos].end(); it += 3)
			{
				if (gCardMgrSx.IsSame(m_curOutCard, *it))
				{
					m_minggangCard[m_beforePos].insert(m_minggangCard[m_beforePos].end(), it, it + 3);
					m_pengCard[m_beforePos].erase(it, it + 3);
					break;
				}
			}
			PengItem const *peng_item = m_ps[m_beforePos].pengs.find_ptr(CardToTile((m_curOutCard)));
			if (peng_item) { // ��ȫ���
				add_gang_item(m_beforePos, m_curOutCard, peng_item->fire_pos, true); // ����������
				// ʵ���ϻ������������һ�����Լ�������һ�ţ�һ�����Լ���ǰ��������
				MHLOG_PLAYCARD("****Desk:%d ������Ƹ�pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, m_beforePos, peng_item->fire_pos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}
			//�޸����ܿ����߼�(**Ren2018-01-08)
			m_getChardFromMAGang[m_beforePos] = 2;
			//���ﴦ�� ���� ��������Ϣ
			notify_desk_BuGang(m_beforePos, m_beforeType);

			m_minggang[m_beforePos] += 1;

			if (m_beforeType == THINK_OPERATOR_MGANG)
				// ���Է��֣���ʱ gangPos ��4������pvs��ʾ�����������Խ���ǶԵ�
				//SetPlayIng(m_beforePos, true, m_tingState[gangPos].m_ting_state == Ting_Null, true);
				SetPlayIng(m_beforePos, true, m_tingState[m_beforePos].m_ting_state == Ting_Null, true);
			else
				GetCard_AfterTing(m_beforePos);
		}
		else
		{
			m_outCard[m_beforePos].push_back(m_curOutCard);
			InsertIntoKnowCard(m_curOutCard, 1);
			//m_louHuCard[GetNextPos(m_beforePos)] = 0;
			SetPlayIng(GetNextPos(m_beforePos), true, m_tingState[GetNextPos(m_beforePos)].m_ting_state == Ting_Null, true);
		}
	}

	void SetThinkIng()
	{
		bool think = false;
		if (!m_desk)
			return;
		MHLOG_PLAYCARD("****Desk:%d ��ҳ��� pos:%d, card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, m_curPos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
		// �������������������ҵ�˼��״̬
		for (int i = 0; i < m_player_count; ++i) {
			m_thinkRet[i].Clear();
			m_thinkInfo[i].Reset();
		}

		// ����ʱ��˳��ӵ�ǰ������ҵ��¼ҿ�ʼ��������������ǰ�������
		int origin = m_curPos;
		for (int d = 1; d < m_player_count; ++d) {
			int i = (origin + d) % m_player_count;

			if (m_tingState[i].m_ting_state == Ting_waitHu) {
				CheckOutCardOpeartorAfterTing(i);
			} else {
				CheckOutCardOperator(i);
			}

			if (m_thinkInfo[i].NeedThink()) {
				think = true;
				VideoThink(i);
			}

			// ������һ�ڶ���ʱ������ʱ��˳��ӵ�ǰ������ҵ��¼ҿ�ʼ��������������ɺ��ģ��Ͳ���ʣ�µ���
			// ���Ч�����ǰ����ȼ�˳��һ��һ�����ʸú����������		 
			if (!has_YiPaoDuoXiang() && m_thinkInfo[i].HasHu()) {
				//break;
			}
		}

		if (think) {
			set_play_state(DESK_PLAY_THINK_CARD);
			SendThinkInfoToClient();
		} else {
			ThinkEnd();
		}
	}

	void SendThinkInfoToClient() {
		for (Lint pos = 0; pos < m_player_count; ++pos) {
			// ���54����Ϣ����������������ť������51����Ϣ LMsgS2COutCard MSG_S_2_C_GET_CARD
			// �����Ϣ�������������İ�ť�ɣ��ܡ�����
			LMsgS2CThink think;
			think.m_time = 15;
			think.m_card = ToCardValue(m_curOutCard);
			if (m_thinkInfo[pos].NeedThink()) {
				think.m_flag = 1;
				ThinkVec const &units = m_thinkInfo[pos].m_thinkData;
				for (Lsize j = 0; j < units.size(); ++j) {
					ThinkData info;
					info.kType = units[j].m_type;
					VecExtend(info.kCard, units[j].m_card);
					think.m_think.push_back(info);
				}
			} else {
				think.m_flag = 0;
			}
			notify_user(think, pos);
		}
	}

	// �������Ϻü�����ѡ������������һ����ѡ����
	// m_thinkRet �����ѡ����ĳ������
	// m_thinkInfo ����ҿ�ѡ��Щ����
	void CheckThink()
	{
		if (!m_desk)
			return;
		bool hu = false;
		bool Peng = false;
		bool Gang = false;
		bool Ting = false;

		bool hu_New = false;
		bool Peng_New = false;
		bool Gang_New = false;
		bool Ting_New = false;

		Lint Hu_most_value_position = INVAILD_POS;
		Lint Ting_most_value_position = INVAILD_POS;

		// ������Ѿ���������ѡ������
		for (Lint i = 0; i < m_player_count; ++i) {
			int pos = i;
			if (m_beforePos != INVAILD_POS)
			{
				pos = (m_beforePos - i) < 0 ? (m_beforePos - i + m_player_count) : m_beforePos - i;
			}
			// �൱�ڴ� m_beforePos ��ʼ˳ʱ�������ң�������������ʱ��˳��
			// ���û�� m_beforePos �أ����൱����ʱ��������

			// ������˳��Ӱ�죬ѡȡһ�����Ⱥ������
			if (m_thinkRet[pos].m_type == THINK_OPERATOR_BOMB) Hu_most_value_position = pos;

			// ������û����ѡȡ��Щ����
			if (m_thinkRet[pos].m_type == THINK_OPERATOR_BOMB)			hu = true;
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_PENG)		Peng = true;
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_MGANG)	Gang = true;
		}

		// �������δ��������ѡ������
		for (Lint i = 0; i < m_player_count; ++i) {
			if (!m_thinkInfo[i].NeedThink()) continue;
			if (m_thinkInfo[i].HasHu()) {
				hu_New = true;
				if (hu) {
					// ѡȡ��m_beforePos����ʱ�������̵��Ǹ�
					if (has_YiPaoDuoXiang() || CheckPositionPrior(m_beforePos, i, Hu_most_value_position) == i) {
						hu_New = true;
						break;
					} else {
						hu_New = false;
					}
				}
			} else if (m_thinkInfo[i].HasTing() && !Ting_New) {
				Ting_New = true;
				if (Ting) {
					if (CheckPositionPrior(m_beforePos, i, Ting_most_value_position) == i) {
						Ting_New = true;
					} else {
						Ting_New = false;
					}
				}
			}
			else if (m_thinkInfo[i].HasPeng())		Peng_New = true;
			else if (m_thinkInfo[i].HasMGang())		Gang_New = true;
		}

		bool think = false;

		if (hu_New) think = true;
		else {
			if (!hu)//���˺�
			{
				if (Ting_New)
				{
					think = true;
				}
				else {
					if (!Ting)
					{
						if (Peng_New || Gang_New)
							think = true;
					}
				}
			}
		}

		if (!think) ThinkEnd(Hu_most_value_position, Ting_most_value_position);
	}







	void send_active_op(ActivePosOp op) {
		LMsgS2CUserPlay sendMsg;
		sendMsg.m_errorCode = 0;
		sendMsg.m_pos = op.pos;
		sendMsg.m_card.kType = op.code;
		sendMsg.m_card.kCard.push_back(ToCardValue(op.tile));
		notify_desk(sendMsg);
	}

	void send_effect_ChuPai_in_TingKou(ActivePosOp op) {
		LMsgS2CUserPlay sendMsg;
		sendMsg.m_errorCode = 0;
		sendMsg.m_pos = op.pos;
		sendMsg.m_card.kType = op.code;
		sendMsg.m_card.kCard.push_back(ToCardValue(op.tile));
		sendMsg.m_flag = 5;
		notify_desk(sendMsg);
	}

	/*************************************************************************************************************
	* ���������ܿ������㲥������
	*/
	void send_effect_MingGangHua(ActivePosOp op) {
		LMsgS2CUserOper send;
		send.m_pos = op.pos;
		send.m_errorCode = 0;
		send.m_think.kType = op.code;
		send.m_think.kCard.push_back(ToCardValue(op.tile));
		send.m_card = ToCardValue(op.tile);
		send.m_hu.push_back(VecFront(m_thinkRet[op.pos].m_hu).m_hu_type);
		CardVector &hand_cards = m_handCard[op.pos];
		send.m_cardCount = hand_cards.size();
		VecExtend(send.m_cardValue, hand_cards);
		notify_desk(send);
	}

	void send_effect_ZiMoHu(ActivePosOp op, tile_t winning_tile, std::vector<Hu_type> const &hu_types) {
		LMsgS2CUserPlay sendMsg;
		sendMsg.m_errorCode = 0;
		sendMsg.m_pos = op.pos;
		sendMsg.m_card.kType = op.code;
		sendMsg.m_card.kCard.push_back(ToCardValue(op.tile));

		sendMsg.m_huCard = ToCardValue(winning_tile);

		for (std::vector<Hu_type>::const_iterator p = hu_types.begin(); p != hu_types.end(); ++p) {
			sendMsg.m_hu.push_back(p->m_hu_type);
		}

		CardVector &hand_cards = m_handCard[op.pos];
		sendMsg.m_cardCount = hand_cards.size();
		VecExtend(sendMsg.m_cardValue, hand_cards);

		notify_desk(sendMsg);
	}

	void active_op_process_ZiMoHu(ActivePosOp op, ThinkUnit *unit) {
		int pos = op.pos;
		// ¼�񣺼�¼��ҵ���������
		VideoAdd__doing(op);
		// �㲥������
		if (NianZhongZi == FEAT_GAME_TYPE && 2 == m_getChardFromMAGang[op.pos])
		{
			//�����ӣ����ܸ��Ͽ����������� ��**Ren��
			//����һ˲�䷢�͸��ͻ��˺��˵���Ϣ����������ʾ�����������ӣ�
			send_effect_MingGangHua(op);  
		}
		else
		{
			send_effect_ZiMoHu(op, ToTile(m_curGetCard), unit->m_hu);
		}

		if (m_curGetCard) {
			erase_hand_card(pos, m_curGetCard);
		}

		// ���λ��0�� ThinkUnit һ�����������Ǹ� ThinkUnit ��
		m_thinkRet[m_curPos] = m_thinkInfo[m_curPos].m_thinkData[0];
		m_winConfirmed.set(m_curPos);
		finish_round(WIN_ZIMO, m_curPos, INVAILD_POS, m_curGetCard);
	}

	static void LMsgS2COutCard_add_TingChuKouUnit(LMsgS2COutCard &msg, Card *out_card, std::vector<HuUnit> const &hu_units) {
		msg.kTingCards.push_back(ToCardValue(out_card));
		// �����������ť����ǰֻ��Ҫ���ò��ɳ����Ƹ����ɳ�����
		// ��������һ�����󣺳���ĳ���ɳ����ƣ���ʾ����Ӧ������
		TingChuKouUnit &chu_unit = VecAdd(msg.kTingChuKou);
		// �������������
		chu_unit.kOutTile = ToTile(out_card);
		// ������ת������
		for (size_t k = 0; k < hu_units.size(); ++k) {
			tile_t hu_tile = ToTile(hu_units[k].hu_card);
			if (!hu_tile) continue; // ��ȫ���
			chu_unit.kTingKou.push_back(hu_tile);			
		}

		//�۵㷢������Ϣ����Ҫ���������
		//if(KouDian == FEAT_GAME_TYPE)
		//for (size_t m = 0; m < mGameInfo.m_hCards.size(); m++)
		//{
		//}


	}

	static void LMsgS2COutCard_setup_TingChuKou_waitOutput(LMsgS2COutCard &msg, std::vector<TingUnit> const &ting_units) {
		if (ting_units.empty()) return;

		for (size_t j = 0; j < ting_units.size(); ++j) {
			Card *out_card = ting_units[j].out_card;
			if (NULL == out_card) continue;
			LMsgS2COutCard_add_TingChuKouUnit(msg, out_card, ting_units[j].hu_units);
		}
	}



	static void LMsgS2COutCard_setup_TingChuKou(LMsgS2COutCard &msg, TingState &ting_state) {
		if (ting_state.m_ting_state == Ting_waitOutput) {
			std::vector<TingUnit> const &ting_units = ting_state.ting_unit;
			LMsgS2COutCard_setup_TingChuKou_waitOutput(msg, ting_units);
		} else if (ting_state.m_ting_state == Ting_waitHu) {
			Card *out_card = ting_state.m_out_card;
			if (out_card) {
				LMsgS2COutCard_add_TingChuKouUnit(msg, out_card, ting_state.hu_units);
			}
		}
	}

	void send_effect_TingKou(ActivePosOp op) {
		int pos = op.pos;
		LMsgS2COutCard msg;
		msg.kTime = 15;
		msg.kPos = pos;
		msg.kDCount = (Lint)m_deskCard.size();
		msg.kTing = 1;
		msg.kFlag = 1;

		LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
		//�۵������Ҫ���������
		if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
		{
			Extend_TingChuKou_Haozi(msg);
		}

		notify_user(msg, pos);
	}

	Lstring format_cards_list(const std::vector<HuUnit> & units)
	{
		std::ostringstream ss;
		size_t size = units.size();
		for (int i = 0; i < size; i++)
		{
			if (i < size - 1)
			{
				ss << (int)ToTile(units[i].hu_card) << ',';
			}
			else
				ss << (int)ToTile(units[i].hu_card);
		}
		return ss.str();
	}

	// �滻ԭ���������߼���ԭ������ֻ�Ƿ������ڣ������û��Ѿ�ȷ��������������ڳ�����
	// ����ͻ��������յ�2����Ϣ������ʱ��Ҫע��
	void active_op_process_Ting_new_version(ActivePosOp op, ThinkUnit * unit)
	{
		MHLOG_PLAYCARD("****Desk:%d ������� pos:%d out_card:%d*****",m_desk->GetDeskId(), op.pos, op.tile);
		MHLOG_TING("***�����߼� op.pos = %d, op.tile = %d, op.code = %d", op.pos, op.tile, op.code);	
		if (NULL == unit)
		{
			LLOG_ERROR("Error can not find user think unit in think list. out_card:%d", op.tile);
			return;
		}
		// У��ͻ��˲������������ǿյĿ϶�����
		if (op.tile == 0) return;
		//VideoAdd__doing(op);  //���¼���б���¼��**Ren 2018-01-23��
		CardValue out_card_value = ToCardValue(op.tile);
		size_t i = VecGetPos(unit->ting_unit, out_card_value);
		if (i == -1)
		{
			MHLOG_TING("***�����Ҳ�����Ӧ������ unit->ting_unit.size() = %d", unit->ting_unit.size());
			return;
		}		

		// �㲥������
		send_active_op(op);
        int pos = op.pos;

		m_tingState[pos].ting_unit = unit->ting_unit; //�������ڣ���������
		m_curPos = pos;
		m_thinkInfo[pos].m_thinkData.clear();		

		TingUnit const &ting_unit = m_tingState[pos].ting_unit[i];
		m_curOutCard = ting_unit.out_card;

		MHLOG_TING("***�ҵ�����Ԫ i = %d, ����ting_unit.out_card = %d", i, ToTile(ting_unit.out_card));
		for (int j = 0; j < ting_unit.hu_units.size(); j++)
		{
			MHLOG_TING("***�ҵ����� j = %d, ting_unit.hu_units[j].hu_card = %d", j, ToTile(ting_unit.hu_units[j].hu_card));
		}
		MHLOG_PLAYCARD("****Desk:%d ������� pos:%d out_card:%d hu_cards: %s*****", m_desk->GetDeskId(), op.pos, op.tile, format_cards_list(ting_unit.hu_units).c_str());
		// ����
		active_op_process_ChuPai__common(pos);		
		ActivePosOp out_card_op(pos, THINK_OPERATOR_OUT, op.tile);
		active_op_process_ChuPai__in_TingKou(out_card_op, ting_unit); // ������ڣ��������
	}

	// ��������֮ǰѯ���û��Ƿ�Ҫ����Ԫ���ͣ������������Ԫ�����ڣ����Ҽ�¼�û���ѡ�񣬺��Ƶ�ʱ����
	// ���о���
	void active_op_process_Ting(ActivePosOp op, ThinkUnit *unit) {

		MHLOG_TING("SUIJUN�޸������߼��������unit�����û������ڴ�����ƣ�����ȷ������!");
		int pos = op.pos;
		// �㲥������
		send_active_op(op);

		m_tingState[pos].ting_unit = unit->ting_unit;
		m_curPos = pos;
		m_thinkInfo[pos].m_thinkData.clear();
		set_play_state(DESK_PLAY_GET_CARD);
		m_needGetCard = false;
		m_tingState[pos].m_ting_state = Ting_waitOutput;

		// �����ڷ���������
		send_effect_TingKou(op);
		LTime cur;
		m_tingTimeEnd[pos] = cur.MSecs();
		LLOG_ERROR("***Ting time is %ld ms, user pos = %d", m_tingTimeEnd[pos] - m_tingTimeBegin[pos], pos);
		
	}

	void active_op_process_AnGang(ActivePosOp op, ThinkUnit *unit) {
		int pos = op.pos;
		// ¼�񣺼�¼��ҵ���������
		VideoAdd__doing(op);
		// �㲥������
		if (m_playtype[PT_AnGangKeJian] && HongTongWangPai == FEAT_GAME_TYPE)
		{
			send_active_op(op);
			//notify_desk_BuGang(pos, THINK_OPERATOR_MGANG);
		}
		else 
			send_active_op(op);
		

		erase_suo_card(pos, unit->m_card[0], 4);
		m_angang[pos] += 1;
		m_curOutCard = unit->m_card[0];
		// ¼��
		VideoAdd__pos_oper_card_n(pos, VIDEO_OPER_AN_GANG, unit->m_card[0], 4);

		for (int i = 0; i < 4; i++) {
			m_angangCard[pos].push_back(unit->m_card[0]);
		}
		add_gang_item(pos, unit->m_card[0], INVAILD_POS, false); // ����������
		MHLOG_PLAYCARD("****Desk:%d ��Ұ��� pos: %d, card: %d [%s]", m_desk ? m_desk->GetDeskId() : 0, pos, ToTile(unit->m_card[0]), unit->m_card[0]->ToString().c_str());
		m_beforeType = THINK_OPERATOR_AGANG;
		//��Ӹ��Ͽ���֧�֣��޸ĸܺ����Ʊ��Ϊ���ܺ����ƣ�**Ren 2017-12-11��
		m_getChardFromMAGang[pos] = 1;

		if (m_tingState[pos].m_ting_state == Ting_waitOutput) {
#ifndef USE_NEW_TING
			GetCard_AfterTing(pos, true);
#else
			//SUIJUN�޸������߼������¼�����
			MHLOG_TING("***�����߼������ܺ�����˼���������������߼�!");
			SetPlayIng(pos, true, true, true);
#endif // !USE_NEW_TING

		}
		else if (m_tingState[pos].m_ting_state == Ting_waitHu) {
			//�鶴������������ܺ��ڣ�for all type
			//if (FEAT_GAME_TYPE == HongTongWangPai) 
			{				
				if (unit->ting_unit.size() > 0)
				{
					m_tingState[pos].hu_units.clear();
					m_tingState[pos].hu_units = unit->ting_unit[0].hu_units;
				}
			}
			SetPlayIng(pos, true, false, true);
		} 
		else {
			SetPlayIng(pos, true, true, true);
		}
	}

	void active_op_process_BuGang(ActivePosOp op, ThinkUnit *unit) {
		int pos = op.pos;
		m_curOutCard = unit->m_card[0];
		erase_hand_card(pos, m_curOutCard);
		//�������˼��
		m_beforePos = pos;
		m_beforeType = THINK_OPERATOR_MGANG;
		m_GangThink = GangThink_qianggang;

		// ���ܺ������ڣ����������ѡ��������������ܵ���֮��û�ܳɹ�֮ǰ�����ɺ���ҵ㡰�����͡������İ�ť
		// �Ǹ���Ҿ�ѡ���ͺ��ˣ������ʧ�ܣ����������
		// �Ǹ����ѡ�����ܾͳɹ���
		// ֱ�ӵ���ThinkEnd()�����൱���Ǹ���ҵ��
		if (has_QiangGangHu())
		{
			SetThinkIng();
		}
		else
		{
			//��Ӹ��Ͽ�����ǣ��޸ĸܺ�����Ϊ���ܺ����ƣ�**Ren 2017-12-11��
			m_getChardFromMAGang[pos] = 2;		
			ThinkEnd();
		}
	}

	// �ͻ��˴������Ĳ���Ҫ����У�飬���У���������״̬��ͬ��У�鷽��Ҳ��ͬ��У���������
	void active_op_process_ChuPai__common(int pos) {
		// �����˳��ƾ��������������
		m_decideGuoHu.clear();
		mGameInfo.m_playerPos = pos; //����������λ��

		if (m_thinkInfo[pos].HasHu()) {
			m_louZimohu[pos] = true;
		}
		// ���ݿ۵㣺���ƺ���൱�ڹ����Լ��ˣ��ѹ�����¼��գ�**Ren:2017-11-25��
		if (XinZhouKouDian == FEAT_GAME_TYPE)
		{
			m_guoPengCard[pos].clear();
		}
		// churunmin: ����ϵ����ڳ�������֮�󣬳��ƽ�����ʾ֮ǰ
		// �Լ����ƺ�����Լ���©����ǣ������Ϳ���������
		// ���ݿ۵㣺�����˾Ͳ����ٺ���,m_louHuCard[pos]��������  ��**Ren:2017-11-22��
		if (m_playtype[PT_GuoHuZhiKeZiMo] || XinZhouKouDian == FEAT_GAME_TYPE) {
			// �����ѡ���ˡ�����ֻ��������ѡ���ô�͸�ʼ�ձ�����©�����
			// ���ڳ���ʱ���©����ǣ��������©������Ҿ�һֱ����������
		} else {
			m_louHuCard[pos] = 0;
		}

		// �����ۼ��Ͻ�
		if (IsHaozipai(m_curOutCard)) {
			++m_ps[pos].shangjin;
		}

		if (m_thinkInfo[pos].NeedThink()) {
			VideoDoing(99, pos, 0, 0);
		}
	}

	void active_op_process_ChuPai__before_Ting(ActivePosOp op) {
		int pos = op.pos;
		// �㲥������
		send_active_op(op);

		Lint size = m_handCard[pos].size();
		erase_hand_card(pos, m_curOutCard);
		m_beforePos = pos;
		m_beforeType = THINK_OPERATOR_OUT;

		// ¼��
		VideoAdd__pos_oper_card(pos, VIDEO_OPER_OUT_CARD, m_curOutCard);
		// �������˼��
		SetThinkIng();
	}

	void notify_YingkouThink(ActivePosOp op)
	{
		int pos = op.pos;
		if (pos != m_curPos)
		{
			LLOG_ERROR("Error think  pos %d", pos);
		}
		// �����û���ʼӲ��˼��
		m_bHongTongYingKou.clear(); //Ĭ�ϲ�Ӳ��
		m_bInYingKouThink.set(pos);
		ThinkTool &tt = m_thinkInfoForYingKou[pos];
		tt.Reset();
		ThinkUnit tUnit1, tUnit2;
		tUnit1.m_type = THINK_OPERATOR_HONGTONG_YINGKOU;
		tUnit1.m_card.push_back(m_curOutCard);
		tt.m_thinkData.push_back(tUnit1);

		tUnit2.m_type = THINK_OPERATOR_HONGTONG_YINGKOU_NOT;
		tUnit2.m_card.push_back(m_curOutCard);
		tt.m_thinkData.push_back(tUnit2);
		LMsgS2CThink think;
		think.m_time = 15;
		think.m_card = ToCardValue(m_curOutCard);
		if (tt.NeedThink()) {
			think.m_flag = 1;
			ThinkVec const &units = tt.m_thinkData;

			for (Lsize j = 0; j < units.size(); ++j) {
				ThinkData info;
				info.kType = units[j].m_type;
				VecExtend(info.kCard, units[j].m_card);
				think.m_think.push_back(info);
			}
		}
		else {
			think.m_flag = 0;
		}
		notify_user(think, op.pos);

		MHLOG("*******************����Ӳ�۵�ָ����ͻ��� Think think.m_card = %d m_think.size=%d", think.m_card, think.m_think.size());
		for (int i = 0; i < think.m_think.size(); i++)
		{
			ThinkData & tData = think.m_think[i];
			MHLOG("******************ThinkData i =%d m_think[i].m_type=%d m_think[i].m_card.size() = %d", i, tData.m_type, tData.m_card.size());
			for (int j = 0; j < tData.kCard.size(); j++)
			{
				MHLOG(" ******************** i = %d, j =%d Card: %d ", i, j, ToTile(tData.m_card[j]));
			}
		}
	}

	//�鶴����  �������Ҫ���ضϣ��ȴ��ͻ��˷���Ӳ��
	void active_op_process_ChuPai__in_TingKou_ForHongTong(ActivePosOp op, TingUnit const &unit) {

		MHLOG("********************** �鶴�������ڴ����� pos = %d, tile = %d", op.pos, op.tile);
		//m_curOutCard;		

		//����Ĵ�����Ҫ�û��ظ���Ӳ�ۺ����ִ��
		int pos = op.pos;
		// �㲥������
		send_effect_ChuPai_in_TingKou(op);

		erase_suo_card(pos, m_curOutCard, 1);
		m_kouCount[pos] = m_outCard[pos].size();
		m_beforePos = pos;
		m_beforeType = THINK_OPERATOR_OUT;

		//¼��
		VideoAdd__pos_oper_card(pos, VIDEO_OPER_OUT_CARD, m_curOutCard);

		m_tingState[pos].m_ting_state = Ting_waitHu;
		m_tingState[pos].hu_units = unit.hu_units;
		m_tingState[pos].ting_unit.clear();
		//�������˼��
		//SetThinkIng();
		m_outCard[m_beforePos].push_back(m_curOutCard);
		InsertIntoKnowCard(m_curOutCard, 1);


		notify_YingkouThink(op);
		//SetPlayIng(GetNextPos(m_beforePos), true, m_tingState[GetNextPos(m_beforePos)].m_ting_state == Ting_Null, true);
		

	}

	void SuijunPintfTIingKou(int pos)
	{
		MHLOG("************* ���������Ϣ pos = %d, m_tingStatus[pos] = %d", pos, m_tingState[pos]);
       // m_tin
	}

	//�鶴����  �������Ҫ���ضϣ��ȴ��ͻ��˷���Ӳ��
	void active_op_process_ChuPai__in_TingKou(ActivePosOp op, TingUnit const &unit) {
		
		MHLOG("**********************��� pos = %d �����ڳ��� %d, ���ڴ�С=%d", op.pos, op.tile, unit.hu_units.size());
		
		if (FEAT_GAME_TYPE == HongTongWangPai)
		{
			bool bYingKouHuType = false;
			int  ting_kou_count = 0;
			for (int i = 0; i < unit.hu_units.size(); i++)
			{
				HuUnit const & h = unit.hu_units[i];
			    MHLOG("*****************�ܺ�����:%d, unit.hu_units[i].fans = %s", ToTile(h.hu_card), malgo_format_fans(h.fans).c_str());	 
				 
				//return active_op_process_ChuPai__in_TingKou_ForHongTong(op, unit);				
			
				 if (h.fans & (FAN_HongTong_YingKou | FAN_HongTong_LouShangLou))
				 {
					 bYingKouHuType = true;
				 }
				 ting_kou_count += 1;				 
			}	
			MHLOG("**********************ting_kou_count = %d bYingKouHuType = %d", ting_kou_count, bYingKouHuType);
			if (ting_kou_count > 1 && bYingKouHuType)
			{
				return active_op_process_ChuPai__in_TingKou_ForHongTong(op, unit);
			}			 
		}

		int pos = op.pos;
		// �㲥������
		send_effect_ChuPai_in_TingKou(op);

		erase_suo_card(pos, m_curOutCard, 1);
		m_kouCount[pos] = m_outCard[pos].size();     //��������ʱ���ʵı���
		m_beforePos = pos;
		m_beforeType = THINK_OPERATOR_OUT;

		//¼��
		VideoAdd__pos_oper_card(pos, VIDEO_OPER_OUT_CARD, m_curOutCard);

		m_tingState[pos].m_ting_state = Ting_waitHu;
		m_tingState[pos].hu_units = unit.hu_units;
		m_tingState[pos].ting_unit.clear();

		if (FEAT_GAME_TYPE == HongTongWangPai)
		{
			//�������˼��
			SetThinkIng();
		}
		else
		{
			m_outCard[m_beforePos].push_back(m_curOutCard);
			InsertIntoKnowCard(m_curOutCard, 1);
			SetPlayIng(GetNextPos(m_beforePos), true, m_tingState[GetNextPos(m_beforePos)].m_ting_state == Ting_Null, true);
		}

	}

	void active_op_process_ChuPai__after_Ting(ActivePosOp op) {
		int pos = op.pos;
		// �㲥������
		send_active_op(op);
		Lint size = m_handCard[pos].size();
		erase_hand_card(pos, m_curOutCard);
		m_beforePos = pos;
		m_beforeType = THINK_OPERATOR_OUT;

		//¼��
		VideoAdd__pos_oper_card(pos, VIDEO_OPER_OUT_CARD, m_curOutCard);
		SetThinkIng();
	}

	void active_op_process_ChuPai(ActivePosOp op) {
		MHLOG_TING("****��ҳ��Ʋ�����pos = %d, card=%d", op.pos, op.tile);
		int pos = op.pos;
		// У��ͻ��˲������������ǿյĿ϶�����
		if (op.tile == 0) return;
		CardValue out_card_value = ToCardValue(op.tile);

		//char buf[1024];
		//sprintf(buf, "ChuPai %d", op.tile);
		//notify_chat_test(pos, buf);

#ifdef USE_NEW_TING
		if (m_tingState[pos].m_ting_state == Ting_waitOutput) { //SUIJUN�޸������߼��������ڽ���������ڳ�����
			m_tingState[pos].m_ting_state = Ting_Null;
			MHLOG_TING("****�����߼����ѡ���˹���pos = %d��Ҫ���ĳ��� %d", pos, ToTile(out_card_value));
			size_t i = VecGetPos(m_handCard[pos], out_card_value);
			if (i != -1) {
				// ����m_curOutCard��������ɾ�������ơ��������������ҳ����ˡ�¼���������˼��
				m_curOutCard = m_handCard[pos][i];
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__before_Ting(op);
				MHLOG_TING("****�����߼����ѡ���˹���pos = %d�����Ƴɹ� %d", pos, ToTile(out_card_value));
			}
		}
		else if (m_tingState[pos].m_ting_state == Ting_waitHu) {
			if (m_tingState[pos].m_out_card == out_card_value) {
				m_curOutCard = m_tingState[pos].m_out_card;
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__after_Ting(op);
			}
		}
		else {
			//SUIJUN�޸������߼�����ҹ�������ͨ���ƣ���ʱ��Ҫ��������ڳ��Ʊ�־
			size_t i = VecGetPos(m_handCard[pos], out_card_value);
			if (i != -1) {
				// ����m_curOutCard��������ɾ�������ơ��������������ҳ����ˡ�¼���������˼��
				m_curOutCard = m_handCard[pos][i];
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__before_Ting(op);
			}
		}

#else // USE_NEW_TING		
		if (m_tingState[pos].m_ting_state == Ting_waitOutput) { //SUIJUN�޸������߼��������ڽ���������ڳ�����
			size_t i = VecGetPos(m_tingState[pos].ting_unit, out_card_value);
			if (i != -1) {
				TingUnit const &unit = m_tingState[pos].ting_unit[i];
				m_curOutCard = unit.out_card;
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__in_TingKou(op, unit);
			}
		} else if (m_tingState[pos].m_ting_state == Ting_waitHu) {
			if (m_tingState[pos].m_out_card == out_card_value) {
				m_curOutCard = m_tingState[pos].m_out_card;
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__after_Ting(op);
			}
		} else {
			//SUIJUN�޸������߼�����ҹ�������ͨ���ƣ���ʱ��Ҫ��������ڳ��Ʊ�־
			size_t i = VecGetPos(m_handCard[pos], out_card_value);
			if (i != -1) {
				// ����m_curOutCard��������ɾ�������ơ��������������ҳ����ˡ�¼���������˼��
				m_curOutCard = m_handCard[pos][i];
				active_op_process_ChuPai__common(pos);
				active_op_process_ChuPai__before_Ting(op);
			}
		}
#endif
	}


	void active_op_process(ActivePosOp op) {
		int pos = op.pos;
		if (op.code == THINK_OPERATOR_TING)
		{
			LTime cur;
			m_tingTimeBegin[pos] = cur.MSecs();
		}
		// ��֤λ�ò�����Ч
		if (!is_pos_valid(pos)) {
			LLOG_DEBUG("active_op_process pos error: %d", pos);
			return;
		}
		// ���������״̬�����ǵȴ�����ҽ�����������
		if (pos != m_curPos) {
			LLOG_ERROR("active_op_process not my pos: pos=%d m_curPos=%d", pos, m_curPos);
			return;
		}

		//���Ͽ���������������ƺ�ѡ���������ոܺ����Ƶı�ǣ�**Ren 2017-12-11��
		if (THINK_OPERATOR_BOMB != op.code)
		{
			//�ܺ����Ƶı���������ֵΪ0��**Ren 2017-12-11��
			m_getChardFromMAGang[op.pos] = 0;
		}

		if (op.code == THINK_OPERATOR_HONGTONG_YINGKOU || op.code == THINK_OPERATOR_HONGTONG_YINGKOU_NOT)
		{
			if (THINK_OPERATOR_HONGTONG_YINGKOU == op.code)
			{
				MHLOG("****�յ��ͻ�����ϢTHINK_OPERATOR_HONGTONG_YINGKOU tile = %d, ����������з���Ԫ�ĺ���", op.tile);
				m_bHongTongYingKou.set(pos);
				std::vector<HuUnit> & units =  m_tingState[pos].hu_units;
				std::vector<HuUnit>::iterator it = units.begin();
				while ( it != units.end())
				{
					if ((*it).fans & (FAN_HongTong_LouShangLou | FAN_HongTong_YingKou))
					{
					 
						MHLOG("****��������: hucard = %d, fans= %s ", ToTile(it->hu_card), malgo_format_fans(it->fans).c_str());
						++it;
					}
					else
					{
						MHLOG("****ɾ������: hucard = %d, fans= %s ", ToTile(it->hu_card), malgo_format_fans(it->fans).c_str());
						units.erase(it);
						it = units.begin();
					}					 
				}
			/*	for (it = units.begin(); it < units.end(); it++)
				{
					if ((*it).fans & (FAN_HongTong_LouShangLou | FAN_HongTong_YingKou))
					{
						MHLOG("****************��������: hucard = %d, fans= %s ", ToTile(it->hu_card), malgo_format_fans(it->fans).c_str());
					}
					else
					{
						MHLOG("****************ɾ������: hucard = %d, fans= %s ", ToTile(it->hu_card), malgo_format_fans(it->fans).c_str());					
						units.erase(it);
					}
				}*/
				MHLOG("****ʣ�µĺ������� count = %d ",  units.size());
				for (std::vector<HuUnit>::iterator it = units.begin(); it < units.end(); it++)
				{
					MHLOG("****����: hucard = %d, fans= %s ", ToTile(it->hu_card), malgo_format_fans(it->fans).c_str());			 
					 
				}
			}
			else if(THINK_OPERATOR_HONGTONG_YINGKOU_NOT == op.code)
			{
				MHLOG("****�յ��ͻ�����ϢTHINK_OPERATOR_HONGTONG_YINGKOU_NOT, tile = %d", op.tile);
				m_bHongTongYingKou.reset(pos);
			}
			for (int i = 0; i < m_thinkInfoForYingKou[op.pos].m_thinkData.size(); i++)
			{
				MHLOG("****m_thinkInfoForYingKou[op.pos].m_thinkData[i].m_type = %d m_thinkInfoForYingKou[op.pos].m_thinkData[i].m_card.size()=%d", 
					m_thinkInfoForYingKou[op.pos].m_thinkData[i].m_type,
				m_thinkInfoForYingKou[op.pos].m_thinkData[i].m_card.size());				 
				
			}
			m_bInYingKouThink.reset(pos);
			SetThinkIng();
			//SetPlayIng(GetNextPos(m_beforePos), true, m_tingState[GetNextPos(m_beforePos)].m_ting_state == Ting_Null, true);
			return;
		}

		//��������Ż������������������иܣ�ѡ���˹��ܾͲ�����ʾ��
		// ��ʱע�͵����ܵļ�⣬��Ҹܲ��˲�������
		if (op.code == THINK_OPERATOR_OUT)
		{
			/*
			Card * pAnGangCard = NULL;
			if (m_thinkInfo[pos].HasAnGang(pAnGangCard))
			{
				if (pAnGangCard)
				{
					LLOG_ERROR("****�����¼��ҹ��ܵ���***out_tile = %d gang_card=%d", op.tile, ToTile(pAnGangCard));
					mGameInfo.m_cards_guo_gang_MH.push_back(*pAnGangCard);
				}
			}
			*/
			//���ݿ۵㣺��������ֻ������ ��** Ren 2017-11-27��
			if (m_thinkInfo[pos].HasHu() && XinZhouKouDian == FEAT_GAME_TYPE)
			{
				m_louHuCard[pos] = 1;
			}
		}

		if (op.code == THINK_OPERATOR_OUT) return active_op_process_ChuPai(op);
		// ���ƺ���Һ���Ԫ
		ThinkUnit *unit = find_think_unit(op);
		if (NULL == unit) {
			// ����������������ҵ㡰�������ָᷢ�����������Ϊ���滹���г��Ʋ���
			LLOG_ERROR("active_op_process invalid op: pos=%d code=%d", pos, op.code);
			return;
		}

		switch (op.code) {
		case THINK_OPERATOR_BOMB: return active_op_process_ZiMoHu(op, unit);
#ifndef USE_NEW_TING
		case THINK_OPERATOR_TING: return active_op_process_Ting(op, unit);
#else
		case THINK_OPERATOR_TING: return active_op_process_Ting_new_version(op, unit);
#endif
		case THINK_OPERATOR_AGANG: return active_op_process_AnGang(op, unit);
		case THINK_OPERATOR_MGANG: return active_op_process_BuGang(op, unit);
		case THINK_OPERATOR_HONGTONG_YINGKOU:
		case THINK_OPERATOR_HONGTONG_YINGKOU_NOT:
		{
		 
			 
		}
		break;
		}


	}



	void send_passive_op_error(PassivePosOp const &op, int err) {
		LMsgS2CUserOper sendMsg;
		sendMsg.m_pos = op.pos;
		sendMsg.m_think.kType = op.code;
		sendMsg.m_think.kCard.push_back(ToCardValue(op.tile));
		sendMsg.m_errorCode = err;
		notify_user(sendMsg, op.pos);
	}

	bool passive_op_is_valid_Guo(PassivePosOp op) {
		ThinkVec const &whitelist = m_thinkInfo[op.pos].m_thinkData;
		// û�к����ܡ������ԣ�Ҳ��û�й�
		if (whitelist.empty()) return false;
		if (op.code != THINK_OPERATOR_NULL) return false;
		return true;
	}

	/*******************************************************************************
	* �������ƣ�  find_think_unit(PosOp op)
	* ������      ���Һ���Ԫ
	* ������
	*  /op        ���˳��ƣ�������ұ����Ĳ���
	* ���أ�      ThinkUnit ����Ԫ
	*/
	ThinkUnit *find_think_unit(PosOp op) {
		ThinkVec &whitelist = m_thinkInfo[op.pos].m_thinkData;
		for (ThinkVec::iterator p = whitelist.begin(); p != whitelist.end(); ++p) {
			if (op.code == p->m_type) {
				// ������û����������������֮������Ǹ������Ĳ���
				if (op.code == THINK_OPERATOR_TING)
				{
					//SUIJUN�޸������߼��������ȡ���û�ȷ��TING��ѡ����Ҫ���ش������				 
#ifdef USE_NEW_TING
					MHLOG_TING("SUIJUN�޸������߼��������ȡ���û�ȷ��TING��ѡ����Ҫ���ش������");
					if (VecHas(p->m_card, ToCardValue(op.tile))) // �û�������Ʊ�����ϵͳ�����
						return &*p;
#else
					return &*p;
#endif // USE_NEW_TING
				}
				// ��ҪУ������������������ǲ���ϵͳ��������ţ��ܡ�������
				if (op.tile == ToTile(VecFront(p->m_card))) return &*p;
			}
		}
		return NULL;
	}

	bool is_pos_valid(int pos) {
		if (NULL == m_desk) return false;
		return 0 <= pos && pos < m_player_count;
	}

	void passive_op_process_Guo(PassivePosOp op) {
		int pos = op.pos;
		m_thinkRet[pos].m_type = THINK_OPERATOR_NULL;
		if (m_thinkInfo[pos].HasHu()) {
			// ����Ҿ�������
			m_decideGuoHu.set(pos);
			// ��Ǹ����©�����ں�
			m_louHuCard[pos] = 1;
			// �е��淨��ƽ³�������Լ��ٳ���ǰ��Ȼ��������
			if (FEAT_GuoHuChuPaiQianKeShouPao) m_louHuCard[pos] = 0;
		}
		//���ݿ۵㣺�����󲻹��Լ����Ʋ��������������� ��**Ren 2017-11-25��
		if (XinZhouKouDian == FEAT_GAME_TYPE && m_thinkInfo[pos].HasPeng())
		{
			m_guoPengCard[pos].push_back(m_curOutCard);
		}
	}

	void send_effect_ShouPaoHu(PassivePosOp op) {
		LMsgS2CUserOper send;
		send.m_pos = op.pos;
		send.m_errorCode = 0;
		send.m_think.kType = op.code;
		send.m_think.kCard.push_back(ToCardValue(op.tile));
		send.m_card = ToCardValue(op.tile);
		send.m_hu.push_back(VecFront(m_thinkRet[op.pos].m_hu).m_hu_type);
		CardVector &hand_cards = m_handCard[op.pos];
		send.m_cardCount = hand_cards.size();
		VecExtend(send.m_cardValue, hand_cards);
		notify_desk(send);
	}

	void passive_op_process_ShouPaoHu(PassivePosOp op) {
		send_effect_ShouPaoHu(op);
		m_winConfirmed.set(op.pos);
	}

	//�°汾�Ĵ���
	void ThinkEnd_new_version(Lint hu_position = INVAILD_POS, Lint ting_position = INVAILD_POS)
	{
		if (!m_desk)
			return;
		for (int i = 0; i < m_player_count; i++)
		{
			if (m_thinkInfo[i].NeedThink())
			{
				VideoDoing(99, i, 0, 0);
			}
			m_thinkInfo[i].Reset();
		}
		if (m_GangThink == GangThink_qianggang)
		{
			m_GangThink = GangThink_gangshangpao;
		}
		else if (m_GangThink == GangThink_gangshangpao)
		{
			m_GangThink = GangThink_over;
		}
		//���˺�
		if (hu_position != INVAILD_POS)
		{
			Card* winCard = m_thinkRet[hu_position].m_card.empty() ? NULL : m_thinkRet[hu_position].m_card.front();
			PassivePosOp op(hu_position, THINK_OPERATOR_BOMB, ToTile(winCard));
			passive_op_process_ShouPaoHu(op);

			//ÿ��ȷ�Ϻ�λ��
			if (has_YiPaoDuoXiang())
			{
				for (int i = 0; i < m_player_count; i++)
				{
					if (i == hu_position)
						continue;
					if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB)
					{
						Card* winCard2 = m_thinkRet[i].m_card.empty() ? NULL : m_thinkRet[i].m_card.front();
						PassivePosOp op2(i, THINK_OPERATOR_BOMB, ToTile(winCard2));
						passive_op_process_ShouPaoHu(op2);
					}
				}
			}


			finish_round(WIN_BOMB, hu_position, m_beforePos, winCard);
			return;
		}

		//������
		if (ting_position != INVAILD_POS)
		{
			//m_louHuCard[ting_position] = 0;
			notify_desk_passive_op(ting_position);
			for (int i = 0; i < m_player_count; i++)
			{
				m_thinkRet[i].Clear();
			}
			return;
		}

		Lint pengPos = INVAILD_POS;
		Lint gangPos = INVAILD_POS;

		for (Lint i = 0; i < m_player_count; ++i)
		{
			if (m_thinkRet[i].m_type == THINK_OPERATOR_MGANG)
			{
				gangPos = i;
				m_minggang[gangPos] += 1;
			}
			else if (m_thinkRet[i].m_type == THINK_OPERATOR_PENG)
			{
				pengPos = i;
			}
		}

		//�ܣ����ܡ���ܣ�
		// ����ֻ�е�ܰɣ��������� HanderUserPlayCard
		if (gangPos != INVAILD_POS)
		{
			//m_louHuCard[gangPos] = 0;
			ThinkUnit gangUnitRet;
			notify_desk_passive_op(gangPos);
			erase_suo_card(gangPos, m_thinkRet[gangPos].m_card[0], 3);

			//¼��
			VideoAdd__pos_oper_card_n(gangPos, VIDEO_OPER_GANG, m_curOutCard, 4);
			for (int i = 0; i < 4; i++)
			{
				m_minggangCard[gangPos].push_back(m_curOutCard);
			}
			add_gang_item(gangPos, m_curOutCard, m_beforePos, false); // �������
			MHLOG_PLAYCARD("****Desk:%d �������pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, gangPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());

			m_diangang[gangPos] += 1;
			m_adiangang[m_beforePos] += 1;
			//��Ӹ��Ͽ������ܣ��޸ĸܺ����Ʊ��Ϊ���ܺ����ƣ�**Ren 2017-12-11��
			m_getChardFromMAGang[gangPos] = 2;

			//�鶴������Ҫʹ��˼����Ԫ��������ܺ��ڣ� For all type
			//if (FEAT_GAME_TYPE == HongTongWangPai) 
			{
				gangUnitRet = m_thinkRet[gangPos];
			}
			for (int i = 0; i < m_player_count; i++)
			{
				m_thinkRet[i].Clear();
			}
			//�������һ����
			m_GangThink = GangThink_gangshangpao;

			if (m_tingState[gangPos].m_ting_state == Ting_waitHu)
			{
				//�鶴������������ܺ���, for all type
				//if (FEAT_GAME_TYPE == HongTongWangPai) 
				{
					MHLOG("************�鶴������������ܺ���FEAT_GAME_TYPE == HongTongWangPai**********************");
					if (gangUnitRet.m_type == THINK_OPERATOR_MGANG && gangUnitRet.ting_unit.size() > 0 && gangUnitRet.ting_unit[0].hu_units.size() > 0)
					{
						m_tingState[gangPos].hu_units.clear();
						m_tingState[gangPos].hu_units = gangUnitRet.ting_unit[0].hu_units;
					}
				}
				SetPlayIng(gangPos, true, false, true);
			}
			else {
				SetPlayIng(gangPos, true, true, true);
			}
			return;
		}

		if (pengPos != INVAILD_POS)
		{
			// pengPos�����Ƶ���
			//m_louHuCard[pengPos] = 0;
			notify_desk_passive_op(pengPos);
			erase_suo_card(pengPos, m_curOutCard, 2);
			// �������������е��ƣ�ɾ��2��ͬ���ģ�����3��һ���ĵ�m_knowCardCount
			InsertIntoKnowCard(m_curOutCard, 3);

			//¼��
			VideoAdd__pos_oper_card_n(pengPos, VIDEO_OPER_PENG_CARD, m_curOutCard, 3);
			for (int i = 0; i < 3; i++)
			{
				m_pengCard[pengPos].push_back(m_curOutCard);
			}

			// �۵��Ƶ���:������������Ȼ�е�ǰ�����ƣ���¼�������Ժ󶼲����ٸ�
			if (VecHas(m_handCard[pengPos], ToCard(m_curOutCard)))
			{
				m_Cannot_gangCard_MH[pengPos].push_back(m_curOutCard);
				mGameInfo.m_cards_guo_gang_MH.push_back(ToCard(m_curOutCard));
				MHLOG_PLAYCARD("****Desk:%d ����и����� pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, pengPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}

			// churunmin: m_beforePosӦ�þ�����ǰ���Ƶ��Ǹ���
			if (m_beforePos != INVAILD_POS) {
				m_ps[pengPos].pengs.add(PengItem(CardToTile(m_curOutCard), m_beforePos));
				MHLOG_PLAYCARD("****Desk:%d ������� pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, pengPos, m_beforePos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}

			for (int i = 0; i <m_player_count; ++i) {
				m_thinkRet[i].Clear();
			}

			//�����һ����
			m_curGetCard = NULL;
			SetPlayIng__Options opt;
			opt.pengPos = pengPos;
			SetPlayIng__opt(pengPos, opt);
			m_needGetCard = true;
			return;
		}

		//����û���˲���
		if (m_beforeType == THINK_OPERATOR_MGANG)
		{
			//¼��
			//int video_oper = m_beforeType == THINK_OPERATOR_MGANG ? VIDEO_OPER_MING_GANG : VIDEO_Oper_Ming1GangTing;
			int video_oper = VIDEO_OPER_MING_GANG;
			VideoAdd__pos_oper_card_n(m_beforePos, video_oper, m_curOutCard, 4);

			m_minggangCard[m_beforePos].push_back(m_curOutCard);
			CardVector::iterator it = m_pengCard[m_beforePos].begin();
			for (; it != m_pengCard[m_beforePos].end(); it += 3)
			{
				if (gCardMgrSx.IsSame(m_curOutCard, *it))
				{
					m_minggangCard[m_beforePos].insert(m_minggangCard[m_beforePos].end(), it, it + 3);
					m_pengCard[m_beforePos].erase(it, it + 3);
					break;
				}
			}
			PengItem const *peng_item = m_ps[m_beforePos].pengs.find_ptr(CardToTile((m_curOutCard)));
			if (peng_item) { // ��ȫ���
				add_gang_item(m_beforePos, m_curOutCard, peng_item->fire_pos, true); // ����������
																					 // ʵ���ϻ������������һ�����Լ�������һ�ţ�һ�����Լ���ǰ��������
				MHLOG_PLAYCARD("****Desk:%d ������Ƹ�pos:%d, firePos: %d card:%d [%s]", m_desk ? m_desk->GetDeskId() : 0, m_beforePos, peng_item->fire_pos, ToTile(m_curOutCard), m_curOutCard->ToString().c_str());
			}
			//���ﴦ�� ���� ��������Ϣ
			notify_desk_BuGang(m_beforePos, m_beforeType);

			m_minggang[m_beforePos] += 1;

			if (m_beforeType == THINK_OPERATOR_MGANG)
				// ���Է��֣���ʱ gangPos ��4������pvs��ʾ�����������Խ���ǶԵ�
				//SetPlayIng(m_beforePos, true, m_tingState[gangPos].m_ting_state == Ting_Null, true);
				SetPlayIng(m_beforePos, true, m_tingState[m_beforePos].m_ting_state == Ting_Null, true);
			else
				GetCard_AfterTing(m_beforePos);
		}
		else
		{
			m_outCard[m_beforePos].push_back(m_curOutCard);
			InsertIntoKnowCard(m_curOutCard, 1);
			//m_louHuCard[GetNextPos(m_beforePos)] = 0;
			SetPlayIng(GetNextPos(m_beforePos), true, m_tingState[GetNextPos(m_beforePos)].m_ting_state == Ting_Null, true);
		}
	}

	void CheckThink_new_version()
	{
		if (!m_desk)
			return;
		bool hu = false;
		bool Peng = false;
		bool Gang = false;
		bool Ting = false;

		bool hu_New = false;
		bool Peng_New = false;
		bool Gang_New = false;
		bool Ting_New = false;

		Lint Hu_most_value_position = INVAILD_POS;
		Lint Ting_most_value_position = INVAILD_POS;

		// ������Ѿ���������ѡ������
		for (Lint i = 0; i < m_player_count; ++i) {
			int pos = i;
			if (m_beforePos != INVAILD_POS)
			{
				pos = (m_beforePos - i) < 0 ? (m_beforePos - i + m_player_count) : m_beforePos - i;
			}
			// �൱�ڴ� m_beforePos ��ʼ˳ʱ�������ң�������������ʱ��˳��
			// ���û�� m_beforePos �أ����൱����ʱ��������

			// ������˳��Ӱ�죬ѡȡһ�����Ⱥ������
			if (m_thinkRet[pos].m_type == THINK_OPERATOR_BOMB) Hu_most_value_position = pos;

			// ������û����ѡȡ��Щ����
			if (m_thinkRet[pos].m_type == THINK_OPERATOR_BOMB)			hu = true;
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_PENG)		Peng = true;
			else if (m_thinkRet[pos].m_type == THINK_OPERATOR_MGANG)	Gang = true;
		}
		LLOG_ERROR("--------has_YiPaoDuoXiang() :%d", has_YiPaoDuoXiang());
		// �������δ��������ѡ������
		for (Lint i = 0; i < m_player_count; ++i) {
			if (!m_thinkInfo[i].NeedThink()) continue;
			if (m_thinkInfo[i].HasHu()) {
				hu_New = true;
				if (hu) {
					// ѡȡ��m_beforePos����ʱ�������̵��Ǹ�
					if (has_YiPaoDuoXiang() || (CheckPositionPrior(m_beforePos, i, Hu_most_value_position) == i)) {
						hu_New = true;
						break;
					}
					else {
						hu_New = false;
					}
				}
				LLOG_ERROR("��δѡ��״̬ hu_New:%d", hu_New);
			}
			else if (m_thinkInfo[i].HasTing() && !Ting_New) {
				Ting_New = true;
				if (Ting) {
					if (CheckPositionPrior(m_beforePos, i, Ting_most_value_position) == i) {
						Ting_New = true;
					}
					else {
						Ting_New = false;
					}
				}
			}
			else if (m_thinkInfo[i].HasPeng())		Peng_New = true;
			else if (m_thinkInfo[i].HasMGang())		Gang_New = true;
		}

		bool think = false;
		LLOG_ERROR("�����״̬hu:%d ting:%d peng:%d gang:%d hu_New:%d ting_new:%d peng_new:%d gang_new:%d",
			hu, Ting, Peng, Gang, hu_New, Ting_New, Peng_New, Gang_New);
		if (hu_New) think = true;
		else {
			if (!hu)//���˺�,����Ҳû�п����ٺ���
			{
				if (Ting_New) // ������������ߵ�
				{
					think = true;
				}
				else {
					if (!Ting)
					{
						if (Peng_New || Gang_New)
							think = true;
					}
				}
			}
		}

		if (!think) ThinkEnd_new_version(Hu_most_value_position, Ting_most_value_position);
	}

	void passive_op_process_new_version(PassivePosOp op) {
		int pos = op.pos;
		// ��֤λ�ò�����Ч
		if (!is_pos_valid(op.pos)) return;
		MHLOG_PLAYCARD("Desk:%d �յ���Ҿ��� code=%d pos=%d,tile=%d", m_desk?m_desk->GetDeskId():0, op.code, op.pos, op.tile);
		//�鶴����ѡ������(**Ren 2017-12-12)
		if (HongTongWangPai == FEAT_GAME_TYPE &&
			m_playtype[MH_PT_HongTongWangPai_ShuangWang] &&
			THINK_OPERATOR_HONGTONG_XUANWANG == op.code &&
			!is_invalid_card(op.tile))
		{
			m_hongTongWangPai_ShuangWang[op.pos] = TileToCard(op.tile);
			//TODO���Ƿ���Ҫ��ӷ��ظ��ͻ���ѡ�������������Ϣ
			return;
		}

		// ������ҵ�ġ�������ť��Ҫ��������ʶ�����⴦��
		if (passive_op_is_valid_Guo(op)) {
			MHLOG_PLAYCARD("Desk:%d �յ���ҹ���ѡ�� pos=%d,tile=%d", m_desk ? m_desk->GetDeskId():0, op.pos, op.tile);
			// ��ҵ�ġ�������ť			 
			passive_op_process_Guo(op);
		}
		else {
			// ÿ����ҵ�ǰ����һ�� ThinkUnit �б�����ʶ������ѡ���ĸ�
			// ���˳��Ƽ���Լ�����Ԫ
			ThinkUnit *unit = find_think_unit(op);
			if (NULL == unit) {
				// ����Ժ����������ȣ�һ���û����˳ԣ�����״̬�ͱ��ˣ���һ���û���������������п����ܵ������֧
				// ���������ҷ�ָ��Ҳ���ܵ������֧
				// �ƺ���Ӧ�÷��ش����룬Ӧ��ֱ�Ӻ��ԣ�
				return send_passive_op_error(op, 1);
			}
			// ��������ҵ�ѡ�񣬺������ܷ���
			m_thinkRet[op.pos] = *unit;
		}

		// ����ֱ�Ӿ���ʾ����˺��ˣ��ƺ���̫�԰�
		// ���������˿������ڣ�һ�������ȼ�����ѡ���ڣ�һ�������ȼ��ĺ�ѡ���ڣ������ȼ��Ĳ��ɹ�ȴ��ʾ������Ч
		//if (op.code == THINK_OPERATOR_BOMB) passive_op_process_ShouPaoHu(op);
		// ¼�񣺱��˳��ƺ󣬸���ҵ����ĸ���ť
		//VideoAdd__doing(op);
		// ��ո���ҵĿ�ѡ��������ʾ������Ѿ�����ѡ����
		m_thinkInfo[op.pos].Reset();
		CheckThink_new_version();

		//if (!has_YiPaoDuoXiang() && m_decideGuoHu[pos]) {
		// �����ѡ��Ĺ����������·���˼��
		//	SetThinkIng();
		//} else {
		// ���ܷ���
		//	CheckThink();
		//}
	}

	void passive_op_process(PassivePosOp op) {
		
		return passive_op_process_new_version(op);
		int pos = op.pos;
		// ��֤λ�ò�����Ч
		if (!is_pos_valid(op.pos)) return;

		//�鶴����ѡ������(**Ren 2017-12-12) ��ʱע�͵���2018-01-08��
		//if (HongTongWangPai == FEAT_GAME_TYPE && 
		//	m_playtype[MH_PT_HongTongWangPai_ShuangWang] &&
		//	THINK_OPERATOR_HONGTONG_XUANWANG == op.code &&
		//	!is_invalid_card(op.tile))
		//{
		//	m_hongTongWangPai_ShuangWang[op.pos] = TileToCard(op.tile);
		//	//TODO���Ƿ���Ҫ��ӷ��ظ��ͻ���ѡ�������������Ϣ
		//	return;
		//}

		// ������ҵ�ġ�������ť��Ҫ��������ʶ�����⴦��
		if (passive_op_is_valid_Guo(op)) {
			// ��ҵ�ġ�������ť
			passive_op_process_Guo(op);
		} 
		else {
			// ÿ����ҵ�ǰ����һ�� ThinkUnit �б�����ʶ������ѡ���ĸ�
			// ���˳��Ƽ���Լ�����Ԫ
			ThinkUnit *unit = find_think_unit(op);
			if (NULL == unit) {
				// ����Ժ����������ȣ�һ���û����˳ԣ�����״̬�ͱ��ˣ���һ���û���������������п����ܵ������֧
				// ���������ҷ�ָ��Ҳ���ܵ������֧
				// �ƺ���Ӧ�÷��ش����룬Ӧ��ֱ�Ӻ��ԣ�
				return send_passive_op_error(op, 1);
			}
			// ��������ҵ�ѡ�񣬺������ܷ���
			m_thinkRet[op.pos] = *unit;
		}

		// ����ֱ�Ӿ���ʾ����˺��ˣ��ƺ���̫�԰�
		// ���������˿������ڣ�һ�������ȼ�����ѡ���ڣ�һ�������ȼ��ĺ�ѡ���ڣ������ȼ��Ĳ��ɹ�ȴ��ʾ������Ч
		if (op.code == THINK_OPERATOR_BOMB) passive_op_process_ShouPaoHu(op);
		// ¼�񣺱��˳��ƺ󣬸���ҵ����ĸ���ť
		VideoAdd__doing(op);
		// ��ո���ҵĿ�ѡ��������ʾ������Ѿ�����ѡ����
		m_thinkInfo[op.pos].Reset();

		if (!has_YiPaoDuoXiang() && m_decideGuoHu[pos]) {
			// �����ѡ��Ĺ����������·���˼��
			SetThinkIng();
		} else {
			// ���ܷ���
			CheckThink();
		}
	}

	void PrintCard(int pos, std::string &text, int nCount, CardValue cards[])
	{
		std::ostringstream str;
		for (int i = 0; i < nCount; i++)
		{
			str << (int)ToTile(cards[i]) << ',';
		}
		MHLOG_PLAYCARD("pos:%d %s: cards_size:%d cards:%s", pos, text.c_str(), nCount, str.str().c_str());
	}

	void OnUserReconnect(User *pUser) {
		if (pUser == NULL || m_desk == NULL) {
			return;
		}

		// ���͵�ǰȦ����Ϣ
		// ΪʲôҪ�������ӣ�������ֻ�������������������ң�
		notify_desk_match_state();

		Lint pos = m_desk->GetUserPos(pUser);
		if (!is_pos_valid(pos)) {
			LLOG_ERROR("Desk::OnUserReconnect pos error %d", pUser->GetUserDataId());
			return;
		}
		MHLOG_PLAYCARD("*******��������desk:%d userid:%d pos:%d m_curPos=%d, m_needGetCard:%d m_curGetCard:%d, m_curOutCard:%d",
			m_desk->GetDeskId(), pUser->GetUserDataId(), pos, m_curPos, m_needGetCard, m_curGetCard?ToTile(m_curGetCard):-1, m_curOutCard?ToTile(m_curOutCard):-1);
			//m_needGetCard��m_curGetCard?ToTile(m_curGetCard):0, m_curOutCard?ToTile(m_outCard):0);
		Lint nCurPos = m_curPos;
		// �����һ�̽����Ǹ��ط����������Ǹ�ʲô�����
		Lint nDeskPlayType = m_play_state;
		LLOG_DEBUG("Desk::OnUserReconnect send out");
		LMsgS2CDeskState reconn;
		reconn.m_user_count = m_player_count;
		reconn.m_state = get_desk_state();
		reconn.m_pos = nCurPos;
		reconn.m_time = 15;
		reconn.m_zhuang = m_zhuangpos;
		reconn.m_myPos = pos;
		reconn.m_deskCreatType = m_desk->GetCreatType();
		if (nCurPos != pos) {
			reconn.m_flag = 0;
		} else {
			reconn.m_flag = 1; // ��֪���Բ���
		}
		reconn.m_dCount = m_deskCard.size();

		for (int x = 0; x < m_player_count; x++) {
#ifdef USE_NEW_TING
			reconn.m_TingState.push_back(m_tingState[x].m_ting_state >= Ting_waitHu ? 1 : 0);
#else
			reconn.m_TingState.push_back(m_tingState[x].m_ting_state >= Ting_waitOutput ? 1 : 0);
#endif			
			reconn.m_iTing.push_back(m_tingState[x].m_ting_state);
			MHLOG_TING("*********�����B���͸��ͻ��˵���״̬ pos = %d, m_tingState[x].m_ting_state= %d, reconn.m_iTing %d", x, m_tingState[x].m_ting_state, reconn.m_iTing[x]);
			if ( x== pos && m_tingState[x].m_ting_state > Ting_waitOutput) {
				MHLOG_TING("***** x== pos && m_tingState[x].m_ting_state > Ting_waitOutput = true, pos = %d, m_tingState[x].hu_units.size() = %d", x, m_tingState[x].hu_units.size());
				for (Lint i = 0; i < m_tingState[x].hu_units.size(); i++) {

					HuUnit temp = m_tingState[x].hu_units[i];
					reconn.m_iTingHuCard.push_back(*temp.hu_card);
					MHLOG_TING("********* hu_cards: i=%d m_tingState[x].hu_units[i].hu_card = %d", i,  ToTile(*temp.hu_card));
				}

				//�۵���Ҫ���������
				if (FEAT_GAME_TYPE == KouDian)
				{
					for (size_t m = 0; m < mGameInfo.m_hCard.size(); m++)
					{						
						if (!VecHas(reconn.m_iTingHuCard, ToCardValue(mGameInfo.m_hCard[m])))
						{
							reconn.m_iTingHuCard.push_back(ToCardValue(mGameInfo.m_hCard[m]));
						}
					}
				}
			}
		}

		
		for (Lint i = 0; i < m_player_count; ++i) {
			//reconn.m_cardCount[i] = m_handCard[i].size() - m_suoHandCard[i].size();
			reconn.m_cardCount[i] = m_handCard[i].size();
			// �����Լ����ƣ�Ҫ����������Ŀ�����ڱ��˵��ƣ�����������Ŀ�������ƾ�����ʾ�м��ţ���������ֵ
			if (pos == i) reconn.m_cardCount[i] -= m_suoHandCard[i].size();
			reconn.m_oCount[i] = m_outCard[i].size();
			reconn.m_aCount[i] = m_angangCard[i].size();
			reconn.m_mCount[i] = m_minggangCard[i].size();
			reconn.m_pCount[i] = m_pengCard[i].size();
			reconn.m_eCount[i] = m_eatCard[i].size();
			reconn.m_score.push_back(m_accum_score[i] + ShanXi_JiaDe1000FenZhiZaiTouXiangChuXianShi);
			reconn.m_kouCount[i] = m_kouCount[i];
			VecExtend(reconn.m_oCard[i], m_outCard[i]);
			VecExtend(reconn.m_aCard[i], m_angangCard[i]);
			VecExtend(reconn.m_mCard[i], m_minggangCard[i]);

			for (Lint m = 0; m < m_minggangCard[i].size(); m++) {
				tile_t m_tile = ToTile(m_minggangCard[i][m]);
				for (Lint n = 0; n <= m_ps[i].gangs.size(); n++)
				{
					if (m_ps[i].gangs[n].tile == m_tile) {
						reconn.m_mCardFirePos[i][m] = m_ps[i].gangs[n].fire_pos;
						break;
					}
				}
			}
		
			////////////////////////////////////////////////////////////
			VecExtend(reconn.m_pCard[i], m_pengCard[i]);

			for (Lint m = 0; m < m_pengCard[i].size(); m++) {
				tile_t m_tile = ToTile(m_pengCard[i][m]);
				for (Lint n = 0; n <= m_ps[i].pengs.size(); n++)
				{
					if (m_ps[i].pengs[n].tile == m_tile) {
						reconn.m_pCardFirePos[i][m] = m_ps[i].pengs[n].fire_pos;
						break;
					}
				}
			}
	
			//////////////////////////////////////////////////////////

		}

		// ����������Щ���ط����ͻ���
		//�鶴����˫�����淨��ÿ���˺����ƶ���һ������ÿ���˵�������Ϣ���͸��ͻ��ˣ�**Ren 2017-12-13����ʱע�͵���2018-01-08��
		//if (HongTongWangPai == FEAT_GAME_TYPE && m_playtype[MH_PT_HongTongWangPai_ShuangWang])
		//{
		//	for (int i = 0; i < m_player_count; i++)
		//	{
		//		/*reconn.m_hongTongShuangWang[i].kColor = m_hongTongWangPai_ShuangWang[i].m_color;
		//		reconn.m_hongTongShuangWang[i].kNumber = m_hongTongWangPai_ShuangWang[i].m_number;*/
		//	}
		//}
		//else if (!mGameInfo.m_hCard.empty()) {  //ע����Ϊע�������if�����Ը�else if Ϊif
		if (!mGameInfo.m_hCard.empty()) {
			VecExtend(reconn.m_hunCard, mGameInfo.m_hCard);
		}

		// �����ƴ��������ų��������ͻ���
		CardVector tmpHand = m_handCard[pos];
		gCardMgrSx.SortCard(tmpHand);
		for(Lsize j = 0; j < m_suoHandCard[pos].size(); ++j) {
			if(VecHas(tmpHand, m_suoHandCard[pos][j])) {
				gCardMgrSx.EraseCard(tmpHand, m_suoHandCard[pos][j], 1);
			}
		}
		// �ҵ��ƣ��ͻ���������֮ǰĪ���Ƶ������ó���������
		if (nDeskPlayType == DESK_PLAY_GET_CARD && m_needGetCard && pos == nCurPos) {
			if (m_curGetCard) { 
				reconn.m_cardCount[pos] -= 1;
				gCardMgrSx.EraseCard(tmpHand, m_curGetCard);
				MHLOG_PLAYCARD("***Desk:%d �������� �ҵ��ƣ��ͻ���������֮ǰĪ���Ƶ������ó��������� pos:%d userid:%d card=%d", m_desk ? m_desk->GetDeskId() : -1,
					pos, pUser ? pUser->GetUserDataId() : -1, ToTile(m_curGetCard));
			}
			VecExtend(reconn.m_cardValue, tmpHand);
		} else {
			VecExtend(reconn.m_cardValue, tmpHand);
		}
		VecExtend(reconn.m_cardSuoValue, m_suoHandCard[pos]);

		// �ó��Ƶ���ң��෢һ���ƣ����ڴ��ȥ��
		if (m_needGetCard && nDeskPlayType == DESK_PLAY_THINK_CARD) {
			if (m_curOutCard && pos != nCurPos) {
				// ����ط�ֻ��������1��ȴû��ȥ���ƣ���Ϊʲô��
				reconn.m_cardCount[nCurPos] ++;
			} else if (m_curOutCard && pos == m_beforePos && m_beforeType == THINK_OPERATOR_OUT) {
				// �޸������ܣ������ͻ��˶��߶��Ƶ�����
				//reconn.m_cardCount[pos]++;
				//reconn.m_oCount[pos]--;
				//reconn.m_cardValue[reconn.m_cardCount[pos] - 1] = ToCardValue(m_curOutCard);
				MHLOG_PLAYCARD("***Desk:%d �ó��Ƶ���ң��෢һ���ƣ����ڴ��ȥ pos:%d userid:%d card=%d", m_desk ? m_desk->GetDeskId() : -1,
					pos, pUser ? pUser->GetUserDataId() : -1, ToTile(m_curOutCard));
			}
		}

		for (int x = 0; x < m_player_count; x++) {
			 
			//LLOG_DEBUG("*****************222222222���͸��ͻ��˵���״̬ pos = %d, reconn.m_iTing %d", x, reconn.m_iTing[x]);
			if (x == pos)
			{
				PrintCard(x, std::string("hand_card"), reconn.m_cardCount[pos], reconn.m_cardValue);
			}
			PrintCard(x, std::string("out_card"), reconn.m_oCount[x], reconn.m_oCard[x]);
			PrintCard(x, std::string("angang_card"), reconn.m_aCount[x], reconn.m_aCard[x]);
			PrintCard(x, std::string("minggang_card"), reconn.m_mCount[x], reconn.m_mCard[x]);
			PrintCard(x, std::string("peng_card"), reconn.m_pCount[x], reconn.m_pCard[x]);
			 
		}
		notify_user(reconn, pos);

		// ��˼��
		if (nDeskPlayType == DESK_PLAY_THINK_CARD) {
			if (m_thinkInfo[pos].NeedThink()) {
				LMsgS2CThink think;
				think.m_time = 15;
				think.m_flag = 1;
				think.m_card = ToCardValue(m_curOutCard);
				ThinkVec const &think_units = m_thinkInfo[pos].m_thinkData;
				for (Lsize j = 0; j < think_units.size(); ++j) {
					ThinkData info;
					info.kType = think_units[j].m_type;
					VecExtend(info.kCard, think_units[j].m_card);
					think.m_think.push_back(info);
				}
				notify_user(think, pos);
			}
		}

		// �ҳ���
		if (nDeskPlayType == DESK_PLAY_GET_CARD && pos == nCurPos) {
			LMsgS2COutCard msg;
			msg.kTime = 15;
			msg.kPos = pos;
			msg.kDCount = (Lint)m_deskCard.size();
			msg.kFlag = (m_curGetCard && m_needGetCard) ? 0 : 1;
#ifndef USE_NEW_TING
			msg.m_ting = m_tingState[pos].m_ting_state >= Ting_waitOutput;
			if (m_needGetCard && m_curGetCard) {
				msg.SetCurCard(ToCardValue(m_curGetCard));
			}

			LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
			//�۵������Ҫ���������
			if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
			{
				Extend_TingChuKou_Haozi(msg);
			}

			ThinkVec const &think_units = m_thinkInfo[pos].m_thinkData;
			for (Lsize j = 0; j < think_units.size(); ++j) {
				ThinkData info;
				info.m_type = think_units[j].m_type;
				VecExtend(info.m_card, think_units[j].m_card);
				msg.m_think.push_back(info);
			}
#else 
			msg.kTing =  m_tingState[pos].m_ting_state == Ting_waitHu ? 1 : 0;
			if (m_needGetCard && m_curGetCard) {
				msg.SetCurCard(ToCardValue(m_curGetCard));
			}
			if (m_tingState[pos].m_ting_state == Ting_waitHu)
			{
				LMsgS2COutCard_setup_TingChuKou(msg, m_tingState[pos]);
				//�۵������Ҫ���������
				if (KouDian == FEAT_GAME_TYPE || (TuiDaoHu == FEAT_GAME_TYPE && m_playtype[MH_PT_HongZhongHaoZi]))
				{
					Extend_TingChuKou_Haozi(msg);
				}
			}
			ThinkVec const &think_units = m_thinkInfo[pos].m_thinkData;
			for (Lsize j = 0; j < think_units.size(); ++j) {
				ThinkData info;
				info.kType = think_units[j].m_type;				
				if (m_tingState[pos].m_ting_state == Ting_waitOutput)
				{
					MHLOG_TING("****��������SUIJUN���������߼�,��������ʱ����ҵ����ڣ��������ݴ������pos=%d msg.m_ting = %d m_ting_state = %d", pos, msg.m_ting, m_tingState[pos].m_ting_state);
					if (info.kType == THINK_OPERATOR_TING)
					{	
						this->LMsgS2COutCard_setup_TingChuKou_waitOutput(msg, think_units[j].ting_unit);
						MHLOG_TING("****think_units[j].ting_unit.size() = %d", think_units[j].ting_unit.size());
						for (int k = 0; k < think_units[j].ting_unit.size(); k++)
						{
							const TingUnit & tu = think_units[j].ting_unit[k];
							MHLOG_TING("****������k:%d, m_out_card: %d", k, ToTile(tu.out_card));
							for (int m = 0; m < tu.hu_units.size(); m++)
							{
								MHLOG_TING("*******����m:%d, hu_units[m].hu_card: %d", m, ToTile(tu.hu_units[m].hu_card));
							}
						}
					}					
				}
				VecExtend(info.kCard, think_units[j].m_card);
				msg.kThink.push_back(info);
			}
#endif // !USE_NEW_TING
			notify_user(msg, pos);
			MHLOG_PLAYCARD("***�ҳ���LMsgS2COutCard msg.pos:%d, msg.m_dcount:%d msg.flag:%d msg.color:%d msg.number:%d", msg.kPos, msg.kDCount, msg.kFlag, msg.kColor, msg.kNumber);
			//��˼��Ӳ��
			if (FEAT_GAME_TYPE == HongTongWangPai && m_tingState[pos].m_ting_state == Ting_waitHu && m_bInYingKouThink[pos] )
			{
				ActivePosOp op(pos, THINK_OPERATOR_OUT, ToTile(m_curOutCard));
				notify_YingkouThink(op);
			}
		}

		// �����ϵ������·�����ҵ�����
		if (m_needGetCard && nDeskPlayType == DESK_PLAY_THINK_CARD && m_GangThink != GangThink_qianggang) {
			if (m_curOutCard) {
				LMsgS2CUserPlay sendMsg;
				sendMsg.m_errorCode = 0;
				sendMsg.m_pos = nCurPos;
				sendMsg.m_card.kType = THINK_OPERATOR_OUT;
				sendMsg.m_card.kCard.push_back(ToCardValue(m_curOutCard));
				notify_user(sendMsg, pos);
			}
		}
	}



	void ProcessRobot(Lint pos, User *pUser) {
		if (!is_pos_valid(pos)) return;
		switch (m_play_state) {
		case DESK_PLAY_GET_CARD:
			// �����ȥ
			if (m_curPos == pos) {
				ActivePosOp op(pos, THINK_OPERATOR_OUT, ToTile(VecFront(m_handCard[pos])));
				active_op_process(op);
			}
			break;
		case DESK_PLAY_THINK_CARD:
			if (m_thinkInfo[pos].NeedThink()) {
				ThinkVec const &think_units = m_thinkInfo[pos].m_thinkData;
				for (size_t i = 0; i < think_units.size(); i++) {
					int code = think_units[i].m_type;
					tile_t tile = ToTile(VecFront(think_units[i].m_card));
					if (code == THINK_OPERATOR_BOMB || code == THINK_OPERATOR_TING) {
						if (code == THINK_OPERATOR_TING)
						{
							MHLOG_TING("********�����˵�������******* pos =%d", pos);
							for (int i = 0; i < m_handCard[pos].size(); i++)
							{
								MHLOG_TING("*******card i = %d, tile = %d", i, ToTile(m_handCard[pos][i]));
							}
						}
#ifdef USE_NEW_TING
						if (m_tingState[pos].m_ting_state == Ting_waitOutput)
						{
							m_tingState[pos].m_ting_state = Ting_Null;
						}
						ActivePosOp op(pos, THINK_OPERATOR_OUT, tile); //�����ȥ
						active_op_process(op);
						return;

#else
						PassivePosOp op(pos, THINK_OPERATOR_NULL, tile);
						passive_op_process(op);
#endif // USE_NEW_TING
						return;
					} else {
						PassivePosOp op(pos, code, tile);
						passive_op_process(op);
						// Ҳ��return
						return;
					}
				}
			}
			break;
		}
	}


	int calc_gang_loss(GangItem const &item) {
		int loss = 1;
		if (LingChuanMaJiang == FEAT_GAME_TYPE) loss = 0; //�괨�����Ÿ���������������ܷ�
		if (KouDian == FEAT_GAME_TYPE) loss = tile_to_points(item.tile);
		if (GuaiSanJiao == FEAT_GAME_TYPE && m_playtype[MH_PT_GuaSanJiao_High_Score]) loss = 5; // ���Ƹ߷֣���Ϊ�ͷ�
		//���ݿ۵㡢�۵㣺�����֣�**Ren 2017-11-28��
		if (XinZhouKouDian == FEAT_GAME_TYPE) loss = tile_to_points(item.tile);
		if (item.is_agang()) loss *= 2;
		return loss;
	}

	/*************************************************************************
	* ���������ݿ۵����ڴ���δ�������ڰ��ܰ��� ��**Ren 2017-11-23��
	* ������over ��Ϸ��������Ϣ
	* ������bombpos ����λ��
	* ���أ�int �ܵĸ���
	*/
	int balance_gang_score_baohubaogang(LMsgS2CGameOver& over, int bombpos)
	{
		int total_gangs = 0;
		for (int i = 0; i < m_player_count; ++i)
		{
			const GangVec& history = m_ps[i].gangs;
			for (int k = 0; k < history.size(); ++k)
			{
				const GangItem& item = history[k];
				Lint gang_score = calc_gang_loss(item);
				++total_gangs;
				over.kDgang[item.fire_pos]++;
				if (item.is_mgang()) over.kDgang[i]++;
				else over.kDgang[i]++;
				if (i == bombpos) continue;
				over.kGangScore[i] += gang_score * (m_player_count - 1);
				over.kGangScore[bombpos] -= gang_score * (m_player_count - 1);
			}
		}
		return total_gangs;
	}

	int balance_gang_score(LMsgS2CGameOver& over, bool is_win, int bombpos, bool is_unting_bomb) {
		int total_gangs = 0;
		// ������Ƶ÷�
		for (Lint i = 0; i < m_player_count; ++i) {
			// ���ֲ�����ֵܷģ����������ܲ������ѡ���Ҫ������
			bool normal_gang_score_enabled = is_win || m_playtype[PT_HuangZhuangBuHuangGang];
			// �������������ѡ�������Ǻ��ң��Ͳ����ܷ�
			if ((FEAT_ZhiYouHuPaiWanJiaGangSuanFen || m_playtype[PT_ZhiYouHuPaiWanJiaGangSuanFen]) && !m_winConfirmed[i]) {
				normal_gang_score_enabled = false;
			}
			// ����Сѡ�δ�����ܲ���֣��ٵ㣩����Ӱ��ܺ������
			if (has_WeiShangTingGangBuSuanFen()) {
				if (m_tingState[i].m_ting_state != Ting_waitHu) normal_gang_score_enabled = false;
			}
			// ���������ҵ����иܼ�¼
			const GangVec& history = m_ps[i].gangs;
			for (size_t k = 0; k < history.size(); ++k) {
				const GangItem& item = history[k];
				// �ٵ㣺�ܺ����淨
				bool is_ganghaozi = KouDian == FEAT_GAME_TYPE && IsHaozipai(TileToCard(item.tile));
				Lint gang_score;
				bool diangang_baogang_enabled;

				if (!is_ganghaozi) {
					if (!normal_gang_score_enabled) continue; // ���ֲ�������ͨ�ĸܷ֣�ֻ����ܺ��ӵĸܷ�
					gang_score = calc_gang_loss(item);
					diangang_baogang_enabled = FEAT_DianGangBaoGang && item.is_zhigang();
					// �������淨���������Ʋ�����ܣ����������Ҵ�������״̬���ͽ��õ�ܰ���
					if (FEAT_TingPaiBuBaoDianGang && item.firer_has_ting) diangang_baogang_enabled = false;
				} else {
					gang_score = 100; // �ܺ��ӱ�׼����100
					if (KouDian == FEAT_GAME_TYPE) gang_score = 40;  // �۵��°汾�𰵸ܱ�׼��40
					if (HongTongWangPai == FEAT_GAME_TYPE) gang_score = calc_gang_loss(item); // �鶴���ư���ͨ�ĸܴ���
					m_desk->m_vip->m_JinAnGangCount[k] += 1;  // ����ܺ�������
					diangang_baogang_enabled = item.is_mgang();
				}

				++total_gangs;
				if (diangang_baogang_enabled) {
					// ���ڵ�ܰ��ܵ��ˣ��ͻ�����ʾ����ܡ����������ǡ���ܰ��ܡ�����  �𣺡���ܡ�
					// ����û�е�ܰ��ܣ��Ͳ���ʾ�ɣ�  �𣺶�
					// ��ܵ�����˿��ܵ��˺ü��ҵĸܣ����ߵ���ͬһ�Һü����ܣ��������ͬһ��3���ܣ���ʾ�ɡ����X3����  �𣺵㼸����X��
					over.kDgang[item.fire_pos]++;
				}
				if (item.is_mgang()) over.kMgang[i]++;
				else over.kAgang[i]++;
				// ����ĳ����ҵ�ĳ���ܣ���������ҳ�֣������ɱ��˴��ͣ����ܣ�
				for (Lint j = 0; j < m_player_count; ++j) {
					if (j == i) continue; // �Լ������Լ����
					if (FEAT_DianGangDanRenKouFen && (j != item.fire_pos) && (item.fire_pos != INVAILD_POS)) continue; // ������ʹ�ã�����ֻ�����˵��˸ܷ�
					Lint tmp_j = j;
					// �����ܡ��ϼҴ���֮����������������Ÿ��Ǳ��������һ���ģ��Ϳ��ԡ����ơ������ָ��ƽ��������ܡ���
					// �����ܡ��������������������ͬ���ƣ�ȡ�����ƣ�����������ܡ���
					// ���Ӹܡ�������Ѿ������ˣ�ȴ��������һ����ͬ���ƣ�Ҳ���Խи��ơ����ָ��ƽ������Ӹܡ���
					// Ŀǰ��ʵ�֣��Ӹ�Ҳ����������
					// ��ܰ��ܣ��������������ͣ���δ��������û��ϵ��
					if (diangang_baogang_enabled) tmp_j = item.fire_pos;

					//���ڰ���
					if (FEAT_WeiShangTingDianPaoBaoGang && is_unting_bomb && (bombpos != INVAILD_POS)) tmp_j = bombpos;
					over.kGangScore[tmp_j] -= gang_score;
					over.kGangScore[i] += gang_score;
				}
			}
		}
		return total_gangs;
	}


	int winpos_to_xianjia_loss(int winpos, tile_t last_tile) {
		FANXING fans = m_ps[winpos].fans;
		return calc_common_loss(fans, last_tile, m_ps[winpos].lingchuan_loss_score) * calc_loss_factor(fans);
	}

	// �ٷ�һ���Ƽ�������Ķ��صĺ���
	int winpos_to_xianjia_loss_for_linfenyimenpai(int winpos, tile_t last_tile,Card * winCard) {
		FANXING fans = m_ps[winpos].fans;
		//return calc_common_loss(fans, last_tile) * calc_loss_factor(fans);
		if (winpos == m_zhuangpos) {
			MHLOG("Logwyz ... calc_ye_zhuang=%d,calc_common_loss=%d, calc_loss_factor=%d", calc_ye_zhuang(m_zhuangpos, winCard), calc_common_loss(fans, last_tile), calc_loss_factor(fans));
			return (calc_ye_zhuang(m_zhuangpos, winCard) + calc_common_loss(fans, last_tile)) * calc_loss_factor(fans);
		}
		else {
			MHLOG("Logwyz ... calc_common_loss=%d, calc_loss_factor=%d",  calc_common_loss(fans, last_tile), calc_loss_factor(fans));
			return calc_common_loss(fans, last_tile) * calc_loss_factor(fans);
		}
	}

	// һ����˵ malgo �ĺ����㷨�õ��ľ����������ͣ����ٵ�Ƚ����⣬�ǰ��������Ƶĵ�������ֵ�
	// malgo �ж���һ����������һ����ƽ���ָߵ�
	// ���Զ��ں����������Ƶ������ж�����Ӧ�û������ڽ��м���
	tile_t calc_last_tile_for_hu_score(int winpos, Card *winCard) {
		// �������Ū�����Ǹ��ٵ�����õ�
		return m_ps[winpos].good_last_tile;


		// Ӧ�ñ���winCard�ɣ�TODO: Ū���������һ��
		if (NULL == winCard) return 0;
		if (KouDian == FEAT_GAME_TYPE) {
			// �ٵ�������ƣ����ú�����ȡ��������
			if (IsHaozipai(winCard)) {
				// ����ж�Ӧ�ÿ϶������������㰲ȫ���
				if (m_tingState[winpos].m_ting_state == Ting_waitHu && !m_tingState[winpos].hu_units.empty()) {
					const std::vector<HuUnit>& hu_units = m_tingState[winpos].hu_units;
					struct hufan_less_than {
						//bool operator ()(const HuUnit& lhs, const HuUnit& rhs) { return tile_to_points(CardToTile(lhs.hu_card)) < tile_to_points(CardToTile(rhs.hu_card)); }
						bool operator ()(const HuUnit& lhs, const HuUnit& rhs) { return lhs.score < rhs.score; }
					};
					std::vector<HuUnit>::const_iterator the_big = std::max_element(hu_units.begin(), hu_units.end(), hufan_less_than());
					return CardToTile(the_big->hu_card);
				}
			}
		}
		return CardToTile(winCard);
	}

	int calc_dealer_risk_score(int winpos, int losepos, bool is_zimo) {
		int risk = get_DealerRiskScore();
		int score = 0;
		if (winpos == m_zhuangpos) score = risk;
		else if (losepos == m_zhuangpos) score = risk;
		if (is_zimo && !FEAT_DisableZiMoDoubleScore) score *= 2;
		MHLOG("*************** score = %d , is_zimo && !FEAT_DisableZiMoDoubleScore = %d FEAT_DisableZiMoDoubleScore = %d",
			score, is_zimo && !FEAT_DisableZiMoDoubleScore, FEAT_DisableZiMoDoubleScore);
		return score;
	}

	void balance_hu_score(LMsgS2CGameOver& over, int bombpos, bool is_unting_bomb, Card *winCard) {
		// ���ֲ��ܼ������
		for (int winpos = 0; winpos < m_player_count; ++winpos) {
			// �����ҿ��Ժ������ҵ��ˡ�������ť�ˣ������ǻᱻ���õ�
			// �������к�����ң��ֱ�Ӯ�־�����
			if (!m_winConfirmed[winpos]) continue;

			//�ٷ�һ���� ����   by wyz
			//�м���ķ�
			int common_loss = 0;
			if (FEAT_GAME_TYPE == LinFenYiMenPai && m_playtype[PT_ShuYe] && winpos == m_zhuangpos) {
				MHLOG("Logwyz ... ׯ��Ӯ,�ٷ�һ���ƽ���");
				common_loss = winpos_to_xianjia_loss_for_linfenyimenpai(winpos, calc_last_tile_for_hu_score(winpos, winCard), winCard);
			}
			else{
				 common_loss = winpos_to_xianjia_loss(winpos, calc_last_tile_for_hu_score(winpos, winCard));
			}
			int dian_pao_extra_loss = 0;
			if (bombpos != INVAILD_POS)
			{
				if (is_unting_bomb)
					dian_pao_extra_loss = FEAT_FangPaoExtraLoss;
				else
					dian_pao_extra_loss = FEAT_FangPaoExtraLoss_TingHou;
			}

			for (int x = 0; x < m_player_count; ++x) {
				if (x == winpos) continue; // �Լ������Լ����
				// ��������˵��ڵ��˿۷֣��������ǵ��ں������������Ҳ��ǵ��ڵģ��Ͳ��۷�
				if (FEAT_DianPaoDanRenKouFen && bombpos != INVAILD_POS && x != bombpos) continue;

				int this_loss = common_loss;
				//�괨�齫��ׯ����Ӯ2������
				if (FEAT_GAME_TYPE == LingChuanMaJiang && (winpos == m_zhuangpos || x == m_zhuangpos))
					this_loss *= 2;

				//�����齫��ׯ����Ӯ2������
				if (FEAT_GAME_TYPE == JinChengMaJiang && (winpos == m_zhuangpos || x == m_zhuangpos))
					this_loss *= 2;

				if (x == bombpos) this_loss += dian_pao_extra_loss; // �����߶���۷�


				FANXING fans = m_ps[winpos].fans;
				bool is_zimo = !!(fans & FAN_ZiMo);
				this_loss += calc_dealer_risk_score(winpos, x, is_zimo); // ��ׯ���շ�

			    

				
				//// �ٷ�һ���� ׯ���䣬����ׯ�ҵ�ҳ��  by wyz  
				//// ������ˣ�ׯ���䣬���ϵ���Ӯ�ҵ�ҳ���� by wyz 20170918
				if (FEAT_GAME_TYPE == LinFenYiMenPai && m_playtype[PT_ShuYe] &&  x == m_zhuangpos  && m_zhuangpos!= winpos  ) {
					this_loss += calc_ye_zhuang(winpos, winCard);
					MHLOG("Logwyz ׯ����,�ٷ�һ����,����Ӯ�ҵ�ҳ��[%d]", calc_ye_zhuang(winpos, winCard));
				}

				int paypos = x;
				if (is_unting_bomb) paypos = bombpos; // δ�������ڰ���
				over.kHuScore[paypos] -= this_loss;
				over.kHuScore[winpos] += this_loss;
			}
		}
	}

	int calc_jin_loss(int pos) {
		int shangjin = m_ps[pos].shangjin;
		// ��ఴ�Ͻ�3�����㣺1��3�֡�2��9�֡�3��27��
		if (shangjin >= 3) return 27;
		if (shangjin >= 2) return 9;
		if (shangjin >= 1) return 3;
		return 0;
	}

	void balance_jin_score(LMsgS2CGameOver& over, bool is_win, int bombpos, bool is_unting_bomb) {
		// ���𣺽�ּ���
		if (TieJin != FEAT_GAME_TYPE) return;
		// ��ׯ��û�н��
		if (!is_win) return;
		for (int winpos = 0; winpos < m_player_count; ++winpos) {
			// ���ƺ�����
			//if (!m_winConfirmed[winpos] && m_tingState[winpos].m_ting_state != Ting_waitHu) continue;
			// ������ߣ�ֻ�к�����Ҳ��ܵý��
			if (!m_winConfirmed[winpos]) continue;
			over.kJinNum[winpos] = m_ps[winpos].shangjin;
			int loss = calc_jin_loss(winpos);
			if (loss == 0) continue;
			// ������ҵĽ�ֻᱻ����
			// �����������ֲ�����
#if 0
			if (m_winConfirmed[winpos]) loss *= calc_loss_factor(m_ps[winpos].fans);
#endif
			for (int x = 0; x < m_player_count; ++x) {
				if (x == winpos) continue; // �Լ������Լ����
				int this_loss = loss;
				int paypos = x;
				if (is_unting_bomb) paypos = bombpos; // δ��������Ҳ�����
				over.kJinScore[paypos] -= this_loss;
				over.kJinScore[winpos] += this_loss;
			}
		}
	}

	//// ����Ӯ�ҵ�ҳ��   by wyz
	int calc_ye_zhuang(int zhuangpos, Card* winCard) {
		if (zhuangpos == 4)return 0;
		Lint zhuang_ye=0 , wan_num = 0 ,tuo_num = 0,suo_num = 0,zi_num = 0;
		Lint color = -1;
		CardVector* cards[5] = { &m_handCard[zhuangpos], &m_eatCard[zhuangpos], &m_pengCard[zhuangpos], &m_angangCard[zhuangpos], &m_minggangCard[zhuangpos] };
		for (int i = 0; i < 5; ++i)
		{
			int space = 1;
			// ������ֻ����һ���� �������Ա��������� ������Ʒ���������ĺ���
			if (i == 1 || i == 2)
			{
				space = 3;
			}
			else if (i == 3 || i == 4)
			{
				space = 4;
			}
			int size = (int)cards[i]->size();
			for (int k = 0; k < size; k += space)
			{
				if (!cards[i]->at(k))
				{
					return false;
				}
				int color_ = cards[i]->at(k)->m_color;
				switch (color_)
				{
				case CARD_COLOR_ZI:
					if (i == 1 || i == 2)zi_num += 3;
					else if (i == 3 || i == 4)zi_num += 4;
					else zi_num++;
					break;
				case CARD_COLOR_WAN:
					if (i == 1 || i == 2)wan_num += 3;
					else if (i == 3 || i == 4)wan_num += 4;
					else wan_num++;
					break;
				case CARD_COLOR_TUO:
					if (i == 1 || i == 2)tuo_num += 3;
					else if (i == 3 || i == 4)tuo_num += 4;
					else tuo_num++;
					break;
				case CARD_COLOR_SUO:
					if (i == 1 || i == 2)suo_num += 3;
					else if (i == 3 || i == 4)suo_num += 4;
					else suo_num++;
					break;
				default:
					break;

				}
				
			}
		}

		if (winCard != NULL)
		{
			switch (winCard->m_color)
			{
			case CARD_COLOR_ZI:
				zi_num++;
				break;
			case CARD_COLOR_WAN:
				wan_num++;
				break;
			case CARD_COLOR_TUO:
				tuo_num++;
				break;
			case CARD_COLOR_SUO:
				suo_num++;
				break;

			}
		}

		zhuang_ye = zi_num;
		if (zi_num < wan_num)zhuang_ye = wan_num;
		if (zi_num < tuo_num)zhuang_ye = tuo_num;
		if (zi_num < suo_num)zhuang_ye = suo_num;
		MHLOG("Logwyz ... zhuang_ye=%d", zhuang_ye);
		return zhuang_ye;
	}

	//�괨�齫����ׯ��2��
	void balance_liuju_score_for_lingchuan(LMsgS2CGameOver& over, bool is_win = false)
	{
		if (is_win) return;
		for (int pos = 0; pos < m_player_count; pos++)
		{
			if (pos != m_zhuangpos)
			{
				over.kHuScore[pos] += 2;
				over.kHuScore[m_zhuangpos] -= 2;
			}
		}
	}

	int calcScore(Lint result, Lint winpos, Lint bombpos, LMsgS2CGameOver& over, Card* winCard) {
		MHLOG("*****************************�������  GameType=%d winpos=%d bombpos=%d", FEAT_GAME_TYPE, winpos, bombpos);
		if (result == WIN_BOMB)
			if (bombpos < 0 || bombpos > m_player_count - 1)
				return 0;

		bool is_win = winpos != INVAILD_POS; // ����ʤ�ֺ����֣�����ҲҪ����ܺ��ӵĸܷ֣����ֲ����������ĸܷ�
		

		// δ�������������������ֻ������������
		// ��������µ�����ұȽϵ�ù
		bool is_unting_bomb = result == WIN_BOMB && m_tingState[bombpos].m_ting_state != Ting_waitHu;

		// m_gangScore �� m_huScore ����Ϣ��Ϊ��Ƴ� std::vector �ˣ�Ӧ��������[4]
		while (over.kGangScore.size() < 4) over.kGangScore.push_back(0);
		while (over.kHuScore.size() < 4) over.kHuScore.push_back(0);

		//�괨�齫���ֲ����
		if ((!is_win) && (FEAT_GAME_TYPE == LingChuanMaJiang))
		{
			//balance_liuju_score_for_lingchuan(over);
		}

		int total_gangs = 0;   //(**Ren 2017-11-23)
		//���ݿ۵� ��**Ren 2017-11-23��
		if (XinZhouKouDian == FEAT_GAME_TYPE && is_unting_bomb)
		{
			//���ݿ۵�����δ�������ڣ����������и� ��**Ren 2017-11-23��
			total_gangs = balance_gang_score_baohubaogang(over, bombpos);
		}
		else if (NianZhongZi == FEAT_GAME_TYPE)
		{
			//�����Ӹܲ����
		}
		else
		{
		    total_gangs = balance_gang_score(over, is_win, bombpos, is_unting_bomb);
        }
		balance_hu_score(over, bombpos, is_unting_bomb, winCard);
		balance_jin_score(over, is_win, bombpos, is_unting_bomb);

		// ���������ϵ׷�
		Lint desk_cell_score = 1;
		if (this->m_desk)
		{
			MHLOG("*****balance_hu_score  desk->cellsore = %d", m_desk->m_cellscore);
			if (m_desk->m_cellscore > 0) desk_cell_score = m_desk->m_cellscore;
		}
		// ���ֺܸܷͺ���
		for (int x = 0; x < m_player_count; x++) 	{
			over.kGangScore[x] *= desk_cell_score;
			over.kHuScore[x] *= desk_cell_score;
			over.kJinScore[x] *= desk_cell_score;
			over.kScore[x] = over.kGangScore[x] + over.kHuScore[x] + over.kJinScore[x];
			
			m_accum_score[x] += over.kScore[x];
		}
		for (int i = 0; i < m_player_count; i++)
		{
			MHLOG_PLAYCARD("Desk:%d -- Score >> pos:%d, gang_fen:%d hu_fen:%d jin_fen:%d total:%d m_accum_score:%d", m_desk->GetDeskId(), i, over.kGangScore[i], over.kHuScore[i], over.kJinScore[i], over.kScore[i], m_accum_score[i]);
		}
		return total_gangs;
	}

	void deal_zhuangpos(int bombpos, int winpos, int total_gangs) {
		// һ�ֽ����󣬼�����ׯλ�õı仯
		Lint zhuangPos = m_zhuangpos;
		int primary_winpos = find_primary_winpos(bombpos, winpos);
		m_zhuangpos = calc_next_zhuangpos(m_curPos, primary_winpos, total_gangs);
		m_desk->m_zhuangPos = m_zhuangpos;
		// ��������£�ׯ���䣬�Ͳ��Ӿ���
		//if (zhuangPos != m_zhuangpos) increase_round();
		// �κ�����¶��Ӿ���
		increase_round();
	}

	void add_round_log(LMsgS2CGameOver &over, int bombpos, int zhuangPos) {
		Lint mgang[4] = { 0 };
		for (Lint i = 0; i < m_player_count; ++i)
		{
			mgang[i] += m_minggang[i];
			mgang[i] += m_diangang[i];
		}

		// churunmin: AddLog�ƺ��ǴӺ�����������copy���ģ��ӿں�ɽ���Ĳ�һ��������һ��HuInfo����
		HuInfo huinfo[4];
		for (Lint i = 0; i < m_player_count; ++i) {
			WIN_TYPE_SUB type = (WIN_TYPE_SUB)over.kWin[i];
			Lint abomb = INVAILD_POS;
			// ������ڻ��ߴ�����ڣ��ͼ�¼һ�µ�����ҵ�λ��
			if (WIN_SUB_BOMB == type || WIN_SUB_DBOMB == type) abomb = bombpos;
			huinfo[i] = HuInfo(type, abomb);
			huinfo[i].hu_types = over.m_hu(i);
		}

		// ������	
		if (m_desk && m_desk->m_vip)
		{
			m_desk->m_vip->AddLog(m_desk->m_user, over.kScore, huinfo, zhuangPos, m_angang, mgang, m_video.m_Id, over.kTing);
			m_desk->m_vip->ResetJinAnGangCount(); // ʹ�ú������
		}

	}

	void finish_round(Lint result, Lint winpos, Lint bombpos, Card *winCard) {
		// �����ȼ�����ҿ�����ȷ���˺������û��һ�ڶ��죬���ȷ��Ӧ������Ч��
		if (!has_YiPaoDuoXiang()) {
			m_winConfirmed.clear();
			m_winConfirmed.set(winpos);
		}

		// ��Ӯȷ�����ȼ�������ׯ�֣�������ѵ�ׯ�Ҵ�ҪӮ�Ļ���Ҳ������ׯ��
		// ���ׯûӮ���͵ð���ׯ�ӷָ����ˣ�����Ʒ�Ҫ��
		// ���ׯ������ҪӮ����ҵĻ���Ҳ�ð���ׯ�ָ�����
		//if (!m_winConfirmed[m_zhuangpos]) m_lianzhuang_jiafen = 0;
		if (false) //
		{
			int primary_winpos = find_primary_winpos(bombpos, winpos);
			if (m_zhuangpos != primary_winpos) m_lianzhuang_jiafen = 0;
		}

		//�㲥���
		LMsgS2CGameOver over;
		over.kUser_count = m_player_count;

		//�����ӣ������ӵ����ܿ�������������**Ren 2017-12-11��
		if (NianZhongZi == FEAT_GAME_TYPE && 2 == m_getChardFromMAGang[winpos] && result == WIN_ZIMO)
		{
			result = WIN_MINGGANGHUA;
		}

		// churunmin: ͬ��������Ƶ�״̬�����ϣ����ں����Ʒ�
		FANXING allowed_fans = m_allowed_fans;
		sync_to_m_ps(result, winCard, allowed_fans);

		//�������
		int total_gangs = calcScore(result, winpos, bombpos, over, winCard);

		//������������ׯ����������ׯ�ӷ�
		if (FEAT_GAME_TYPE == GuaiSanJiao && (winpos != INVAILD_POS) 
			&& (winpos == m_zhuangpos) && (m_ps[winpos].fans & FAN_Zhuang)
			&& (m_lianzhuang_jiafen > 0))
		{
			m_ps[winpos].fans |= FAN_LianZhuang;
			over.kYbPiao[winpos] = m_lianzhuang_jiafen;
		}

		// һ�ֽ����󣬼�����ׯλ�õı仯
		Lint zhuangPos = m_zhuangpos;
		deal_zhuangpos(bombpos, winpos, total_gangs);

		// �ͻ�����ʾ����ҳ�棨¼��
		
		//��Ӹ��Ͽ������ܣ�**Ren 2017-12-11��
		if (result == WIN_ZIMO && winpos != INVAILD_POS && winCard) {
			//Ӯ����ʾ��������
			over.kWin[winpos] = WIN_SUB_ZIMO;
			//���ܺ����ƣ�Ӯ����ʾ�����������ܿ�����
			if (1 == m_getChardFromMAGang[winpos]) {
				over.kWin[winpos] = WIN_SUB_ZIMOANGANGHUA;  //9
			}
			//���ܺ����ƣ�Ӯ����ʾ�����������ܿ�����
			else if (2 == m_getChardFromMAGang[winpos]) {
				over.kWin[winpos] = WIN_SUB_ZIMOMINGGANGHUA;  //10
			}

			VideoAdd__pos_oper_card(winpos, VIDEO_OPER_ZIMO, winCard);
			MHLOG_PLAYCARD("****Desk:%d ������. pos: %d, win_card:%d", m_desk ? m_desk->GetDeskId() : 0, winpos, ToTile(winCard));
		} 
		else if (result == WIN_BOMB && winpos != INVAILD_POS && winCard) {
			for (int x = 0; x < m_player_count; x++) {
				if (m_winConfirmed[x])
				{
					over.kWin[x] = WIN_SUB_BOMB;				
				}
			}
			if (bombpos != INVAILD_POS) over.kWin[bombpos] = WIN_SUB_ABOMB;
			VideoAdd__pos_oper_card(winpos, VIDEO_OPER_SHOUPAO, winCard);
			MHLOG_PLAYCARD("****Desk:%d ���ں�. winpos:%d fire_pos:%d win_card:%d", m_desk ? m_desk->GetDeskId() : 0, winpos, bombpos, ToTile(winCard));
			for (int k = 1; k < m_player_count; k++)
			{
				int next_win_pos = (winpos + k) % m_player_count;
				if (m_winConfirmed[next_win_pos])
				{
					VideoAdd__pos_oper_card(next_win_pos, VIDEO_OPER_SHOUPAO, winCard);
					MHLOG_PLAYCARD("****Desk:%d ���ں�. winpos:%d fire_pos:%d win_card:%d", m_desk ? m_desk->GetDeskId() : 0, next_win_pos, bombpos, ToTile(winCard));
				}			
			}
		} 
		// �����ӣ�������ܿ����������� ��**Ren 2017-12-01��
		else if (result == WIN_MINGGANGHUA && winpos != INVAILD_POS && winCard){
			over.kWin[winpos] = WIN_SUB_MINGGANGHUA;  //8
			MHLOG_PLAYCARD("****Desk:%d ���ܿ���. pos: %d, win_card:%d", m_desk ? m_desk->GetDeskId() : 0, winpos, ToTile(winCard));
		}
		else {
			VideoAdd__pos_oper(m_curPos, VIDEO_OPER_HUANGZHUANG);
			MHLOG_PLAYCARD("****Desk:%d ��ׯ.", m_desk ? m_desk->GetDeskId() : 0);
		}

		// ����¼��
		VideoSave();

		over.kResult = result;  //�����ӣ�:������ܸ��ϻ�4 ��������ӣ�**Ren 2017-12-01��

		for (Lint i = 0; i < m_player_count; ++i) {
			// ������Ŀ
			// churunmin: ����ط�û����������Ŀ���ͻ���Ϊʲôû������⣿
			over.kCount[i] = m_handCard[i].size();
			// ����
			for (Lint j = 0; j < over.kCount[i]; ++j) {
				over.kCard[i][j].kColor = m_handCard[i][j]->m_color;
				over.kCard[i][j].kNumber = m_handCard[i][j]->m_number;
				over.m_hand_cards(i).push_back(ToCardValue(m_handCard[i][j]));
			}
			// ��������
			if (m_thinkRet[i].m_type == THINK_OPERATOR_BOMB) {				
				vectorize_fans(m_ps[i].fans, over.m_hu(i));
				MHLOG_PLAYCARD("Desk:%d ���Ʒ���- pos: %d hu_fans: %s", m_desk ? m_desk->GetDeskId() : 0, i, malgo_format_fans(m_ps[i].fans).c_str());
			}
			// ����״̬
			over.kTing[i] = m_tingState[i].m_ting_state == Ting_waitHu;
		}
		if (winCard && winpos != INVAILD_POS) {
			over.m_hucards(winpos).push_back(ToCardValue(winCard));
		}

		VecExtend(over.kHunCard, mGameInfo.m_hCard);

		set_desk_state(DESK_WAIT);

		add_round_log(over, bombpos, zhuangPos);

		// �Ƿ����һ��
		// ��ʱ m_round_offset �Ѿ��ǵ������˵�
		over.kEnd = m_dismissed || m_round_offset >= m_round_limit;
		// �����̵Ľ������㲥������
		notify_desk(over);

		if (m_desk) m_desk->HanderGameOver(1);
	}



	virtual void notify_user(LMsg &msg, int pos) {
	}

	virtual void notify_desk(LMsg &msg) {
	}

	void notify_desk_match_state() {
		LMsgS2CVipInfo msg;
		msg.m_curCircle = m_round_offset;
		msg.m_curMaxCircle = m_round_limit;
		m_playtype.fill_vector_for_client(msg.m_playtype, FEAT_GAME_TYPE);
		notify_desk(msg);
	}

	void notify_desk_passive_op(int pos) {
		LMsgS2CUserOper send;
		send.m_pos = pos;
		send.m_errorCode = 0;
		send.m_think.kType = m_thinkRet[pos].m_type;
		VecExtend(send.m_think.kCard, m_thinkRet[pos].m_card);
		send.m_card = ToCardValue(m_curOutCard);
		notify_desk(send);
	}

	/************************************************************************
	* ��������    notify_desk_xuanwang()
	* ������      �鶴�����������ѡ�����֪ͨ���ͻ���
	* ���أ�      (void)
	*************************************************************************/
	void notify_desk_xuanwang()
	{
		LMsgS2CUserOper send;
		send.m_xuanWang = 1;
		for (int i = 0; i < 4; ++i)
		{
			send.m_hongTongShuangWang[i] = m_hongTongWangPai_ShuangWang[i];
		}
		notify_desk(send);
	}

	void notify_desk_BuGang(int pos, int code) {
		LMsgS2CUserPlay sendMsg;
		sendMsg.m_errorCode = 0;
		sendMsg.m_pos = pos;
		sendMsg.m_card.kType = code;
		sendMsg.m_card.kCard.push_back(ToCardValue(m_curOutCard));
		notify_desk(sendMsg);
	}

	static bool is_invalid_card(Card card) {
		return !tile_is_valid(ToTile(card));
	}
	static bool is_invalid_card(CardValue card) {
		return !tile_is_valid(ToTile(card));
	}

	void notify_HaoZi() {

		MHLOG("*****Send to client LMsgS2CUserHaozi haozi_count=%d", mGameInfo.m_hCard.size());
		int index = 0; // �ƺ�ûɶ�ã���ǰ����Ǻ����������ϵ�����
		LMsgS2CUserHaozi msg;
		msg.m_type = 1;
		msg.m_pos = m_curPos;
		msg.m_randNum.push_back(index);
		VecExtend(msg.m_HaoZiCards, mGameInfo.m_hCard);

		if (msg.m_HaoZiCards.size() == 1) {
			int desk_id = m_desk ? m_desk->GetDeskId() : 0;
			CardValue haozi0 = msg.m_HaoZiCards[0];
			LLOG_ERROR("Desk %d notify 1 haozi %d|%d", desk_id, haozi0.kColor, haozi0.kNumber);
			if (is_invalid_card(haozi0)) {
				LLOG_ERROR("INVALID_CARD: desk %d notify 1 haozi %d|%d", desk_id, haozi0.kColor, haozi0.kNumber);
			}
		} else if (msg.m_HaoZiCards.size() == 2) {
			int desk_id = m_desk ? m_desk->GetDeskId() : 0;
			CardValue haozi0 = msg.m_HaoZiCards[0];
			CardValue haozi1 = msg.m_HaoZiCards[1];
			LLOG_ERROR("desk %d notify 2 haozi %d|%d, %d|%d", desk_id, haozi0.kColor, haozi0.kNumber, haozi1.kColor, haozi1.kNumber);
			if (is_invalid_card(haozi0) || is_invalid_card(haozi1)) {
				LLOG_ERROR("INVALID_CARD: desk %d notify 2 haozi %d|%d, %d|%d", desk_id, haozi0.kColor, haozi0.kNumber, haozi1.kColor, haozi1.kNumber);
			}
		}


		ThinkData think;
		think.kCard = msg.m_HaoZiCards;
		think.kType = THINK_OPERATOR_NULL;
		msg.m_think.push_back(think);
		notify_desk(msg);
	}

	void notify_chat_test(int pos, char const *msg) {
		LMsgS2CUserSpeakTest speak;
		speak.m_userId = m_desk->m_user[pos]->GetUserDataId();
		speak.m_pos = pos;
		speak.m_type = 2;
		speak.m_msg = msg;
		notify_desk(speak);
	}

};


struct ShanXiGameHandler : ShanXiGameCore {

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
	}

	void SetDeskPlay() {
		if (!m_desk || !m_desk->m_vip) {
			return;
		}		 
		start_round();
	}

	/*   //���ѡ����
	void MHSetDeskPlay(Lint PlayUserCount, Lint * PlayerStatus, Lint PlayerCount) {
		LLOG_INFO("Logwyz-------------MHSetDeskPlay(Lint PlayUserCount)=[%d]", PlayUserCount);
		if (!m_desk||!m_desk->m_vip) {
			return;
		}
		//��Ϸ����ֻ����2��3��4��
		if (PlayUserCount>1&&PlayUserCount<=4)
		{
			m_player_count=PlayUserCount;
			memset(m_players_status, 0x00, sizeof(m_players_status));
			memcpy(m_players_status, PlayerStatus, sizeof(m_players_status));
			LLOG_INFO("Logwyz-------------MHSetDeskPlay:PlayerStatus[%d][%d][%d][%d],PlayerCount=[%d]", PlayerStatus[0], PlayerStatus[1], PlayerStatus[2], PlayerStatus[3], PlayerCount);
			LLOG_INFO("Logwyz-------------MHSetDeskPlay:m_players_status[%d][%d][%d][%d]", m_players_status[0], m_players_status[1], m_players_status[2], m_players_status[3]);
		}
		else
			return;
		start_round();
	}
	*/

	void MHSetDeskPlay(Lint PlayUserCount) {
		LLOG_INFO("Logwyz-------------MHSetDeskPlay(Lint PlayUserCount)=[%d]", PlayUserCount);
		if (!m_desk||!m_desk->m_vip) {
			return;
		}
		//��Ϸ����ֻ����2��3��4��
		if (PlayUserCount>1&&PlayUserCount<=4)
		{
			m_player_count=PlayUserCount;
		}
		else
			return;
		start_round();
	}


	Lstring format_user_oper(Lint op)
	{
		switch (op)
		{

#define A(v)  case v: return #v
			A(THINK_OPERATOR_NULL);
 
			A(THINK_OPERATOR_OUT);
			A(THINK_OPERATOR_BOMB);
			A(THINK_OPERATOR_AGANG);
			A(THINK_OPERATOR_MGANG);// = 4,//����
			A(THINK_OPERATOR_PENG);// = 5,//��
			A(THINK_OPERATOR_CHI);// = 6,//��
			A(THINK_OPERATOR_TING);// = 7,//��
			A(THINK_OPERATOR_MBU);// = 8, // ����

			A(THINK_OPERATOR_DINGQUE);// = -3,
			A(THINK_OPERATOR_CHANGE);// = -2,
			A(THINK_OPERATOR_REOUT); //= -1, // ���������ٴθ�����Ҵ��ȥ����
									   //THINK_OPERATOR_NULL  = 0,
									   //THINK_OPERATOR_OUT   = 1,
									   //THINK_OPERATOR_BOMB  = 2, // ��
									   //THINK_OPERATOR_AGANG = 3, // ����	Ŀǰû�õ�
									   //THINK_OPERATOR_MGANG = 4, // ����
									   //THINK_OPERATOR_PENG  = 5, // ��
									   //THINK_OPERATOR_CHI   = 6, // ��     Ŀǰû�õ�
			A(THINK_OPERATOR_ABU);// = 12, // ����
			A(THINK_OPERATOR_DIANGANGPAO);// = 14,// ��ҵ�ܺ�����������ں�����
			A(THINK_OPERATOR_QIANGANGH);// = 15, //���ܺ�
			A(THINK_OPERATOR_TINGCARD); //= 16,  //����
			A(THINK_OPERATOR_FLY);// = 17,   //��
			A(THINK_OPERATOR_RETURN);// = 18,   //��
			A(THINK_OPERATOR_MUSTHU);// = 19,   //�����
			A(THINK_OPERATOR_HONGTONG_YINGKOU); //20
			A(THINK_OPERATOR_HONGTONG_YINGKOU_NOT); //21
#undef A

		default: 
			return "Unknow operator";
		}
	}
	// ��� HanderUserPlayCard �� HanderUserOperCard
	// ����������Ϣ�����ݲ��
	// LMsgC2SUserPlay: ThinkData m_thinkInfo;
	// LMsgC2SUserOper: ThinkData m_think;

	// �ͻ��˷�������ҳ��Ʋ������Լ���������Щ�ƣ�Ӧ�ù�����ֻ����һ����
	// ����ĳЩ˼������Ҳ���ܵ����
	void HanderUserPlayCard(User *pUser, LMsgC2SUserPlay *msg) {
		// ���������Ҫ�Կͻ��˴�������Ϣ���򵥵�ת����Ȼ���ض����ڲ�������
		if (m_desk == NULL || pUser == NULL || msg == NULL) {
			LLOG_DEBUG("HanderUserPlayCard NULL ERROR: %p %p %p", m_desk, pUser, msg);
			return;
		}
		MHLOG("****HanderUserPlayCard Get message LMsgC2SUserPlay msgid = %d op=%s", msg->m_msgId, format_user_oper(msg->m_thinkInfo.m_type).c_str());
		int pos = m_desk->GetUserPos(pUser);
		ActivePosOp op(pos, msg->m_thinkInfo.kType, ToTile(VecFront(msg->m_thinkInfo.kCard)));
		active_op_process(op);	 
	}


	// �ͻ��˷��������˼�������������ܡ������ԣ��Լ���������Щ��
	// ���Լ����Ƶģ����Ĺ����ͻ��˲�����Ϣ������˼���Ĺ�������Ϣ������
	void HanderUserOperCard(User *pUser, LMsgC2SUserOper *msg) {
		// ���������Ҫ�Կͻ��˴�������Ϣ���򵥵�ת����Ȼ���ض����ڲ�������
		if (m_desk == NULL || pUser == NULL || msg == NULL) {
			LLOG_DEBUG("HanderUserOperCard NULL ERROR: %p %p %p", m_desk, pUser, msg);
			return;
		}
		MHLOG("*****HanderUserPlayCard Get message LMsgC2SUserOper msgid = %d op=%s", msg->m_msgId, format_user_oper(msg->m_think.m_type).c_str());
		// �û����������������һ�˳��ƺ�ѯ�������˺������Թ�
		int pos = m_desk->GetUserPos(pUser);
		PassivePosOp op(pos, msg->kThink.kType, ToTile(VecFront(msg->kThink.kCard)));
		passive_op_process(op);
	}

	void OnGameOver(Lint result, Lint winpos, Lint bombpos, Card* winCard) {
		if (m_desk == NULL || m_desk->m_vip == NULL) {
			LLOG_DEBUG("OnGameOver NULL ERROR ");
			return;
		}

		m_dismissed = !!m_desk->m_vip->m_reset;
		MHLOG_PLAYCARD("****Desk game over. Desk:%d round_limit: %d round: %d dismiss: %d", m_desk ? m_desk->GetDeskId() : 0, m_round_limit, m_round_offset, m_dismissed);
		finish_round(result, winpos, bombpos, winCard);
	}


};





// �����淨�� ShanXiGameHandlerCreator ��Ϊȫ�ֱ��������Լ�ע�ᵽ������������ˣ����ù�ע��
// ����ʵ���Ͼ��Ǹ�ȫ�ֵ� GameType �� GameHandlerCreator ��ӳ��
// ���ֹ����� GameHandlerCreator ���������� GameHandler ������ɾ���ģ��������ٻ������
struct ShanXiGameHandlerCreator: GameHandlerCreator {
	GameType game_type;
	ShanXiGameHandlerCreator(GameType game_type): game_type(game_type) {
		GameHandlerFactory::getInstance()->registerCreator(game_type, this);
	}

	GameHandler *create() {
		ShanXiGameHandler *gh = new ShanXiGameHandler();
		gh->setup_feature_toggles(game_type);
		return gh;
	}
};

#define XX(k, v, player_count) ShanXiGameHandlerCreator g_ShanXiGameHandlerCreator__##k(k);
ShanXi_GAME_TYPE_MAP(XX)
#undef XX


//struct MajinagTuiTongZiGameHandler : GameHandler
//{

//};

//DECLARE_GAME_HANDLER_CREATOR(20001, MajinagTuiTongZiGameHandler);
