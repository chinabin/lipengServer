#pragma once

// �����㷨
#include "malgo.h"
// �淨���
#include "GameDefine.h"
// ���������Сѡ��
#include "ClientConstants.h"



// ��ͬ���淨�����Ƶ������ٵ㡢���Ĵ���һЩС���죬�����ڻ����и�һЩĬ��ֵ
// Ȼ����Щ�淨������ڹ��캯�����޸���ЩĬ��ֵ
struct ShanXiFeatureToggles {
	int m_registered_game_type;
	int FEAT_GAME_TYPE; // ��Ϸ�淨���Ƶ������ٵ㡢����
	int FEAT_DaiFeng : 1; // Ĭ�϶��Ǵ���ģ��Ƶ�����Ҫ����Զ���
	int FEAT_BaoTing : 1; // ǿ�Ʊ�����������Ҫ
	int FEAT_TingPaiKeGang : 1; // ���ƿɸ�
	int FEAT_OnlyQueMenCanBaoTing : 1; // ֻ��ȱ�Ų��ܱ�����������Ҫ
	int FEAT_ZhiYouHuPaiWanJiaGangSuanFen : 1; // ֻ�к�����Ҹ����
	int FEAT_DianGangBaoGang : 1; // ��ܰ�������Ĭ�ϲ�������
	int FEAT_HunPaiBuKeGang : 1; // ���Ʋ��ɸܣ���������
	int FEAT_LianZhuangJiaFen : 1; // ��ׯ�ӷ֣���������Ҫ
	int FEAT_GuoHuChuPaiQianKeShouPao : 1; // ��������ǰ�����ڣ�ƽ³��Ҫ
	int FEAT_ZhiKeZiMoHuPai : 1; // ֻ���������ƣ����ǲ������ڵ���˼
	int FEAT_TingPaiHouBuNengGuoHu : 1; // ���ƺ��ܹ�����Ŀǰ�ǿͻ��˴���ģ�����ʾ��������ť
	int FEAT_YiPaoDuoXiang : 1; // һ�ڶ��죬Ŀǰ����Сѡ�����õģ���û�й̶����õ�
	int FEAT_QiangGangHu : 1; // ���ܺ�
	int FEAT_WeiShangTingGangBuSuanFen : 1; // δ�����ܲ����
	int FEAT_TingPaiBuBaoDianGang : 1; // ���Ʋ������
	int FEAT_DianPaoDanRenKouFen : 1; // ���ڵ��˿۷�
	int FEAT_DealerRiskScoreDisabled : 1; // �ر�ׯ��֣�Ȼ���ͨ��Сѡ�����
	int FEAT_DisableZiMoDoubleScore : 1; // ׯ���ʱ��ֹ����˫��
	int FEAT_DianGangDanRenKouFen : 1; // ������ʹ�ã�������ֻ�۵��˷�
	int FEAT_WeiShangTingDianPaoBaoGang : 1;  // δ��������ҵ��ڰ���
	unsigned char FEAT_SUO_COUNT; // �������ƣ�������Ҫ
	unsigned char FEAT_FangPaoExtraLoss;      // δ�������ڶ���۷�
	unsigned char FEAT_FangPaoExtraLoss_TingHou; // ������ڶ���۷֣����������ǰ���������FEAT_FangPaoExtraLoss��������ֵͬ
	// ׯ���շ֣�Ӯ�˶�Ӯ�����˶��䣬����������Ӱ�죬����Ӯ�˶�Ӯ�������շ֣�����ͬ�������������ӱ�Ӱ��
	unsigned char FEAT_DealerRiskScore;
	// ������ǹ��ܿ��أ��ɹ��캯��ȷ��
	// ���¶��� MatchState���� clear_match ʱ���
	GameOptionSet m_playtype;
	FANXING m_allowed_fans; // �������ã���Ҫ�淨��Сѡ�ͬȷ��
	int m_lianzhuang_jiafen;
	int m_player_count; // �淨���Ĵ�����ʹ�õ���������ֶ�
	Lint m_zhuangpos; //ׯ��λ��
	Lint m_remain_card_count; // ������ʣ�¶��ُ�������f

	ShanXiFeatureToggles() {
		memset(this, 0, sizeof(*this));
		FEAT_DaiFeng = 1;
		FEAT_TingPaiKeGang = 1; // Ĭ�����ƿɸܣ�Ȼ����и�����
		FEAT_FangPaoExtraLoss = 0;
		FEAT_FangPaoExtraLoss_TingHou = 0;
	}

	void clear_match() {
		m_playtype.clear();
		m_lianzhuang_jiafen = 0;
		m_player_count = 0;
		m_zhuangpos = 0;
		m_remain_card_count = 0;
	}

	int GetNextPos(int pos) {
		if (m_player_count == 0) return 0; // ����������
		return (pos + 1) % m_player_count;
	}

	// �ϲ��淨�趨�Ĺ��ܿ����봴������ʱָ����Сѡ��
	// Ҳ���ܴ�����Сѡ��ָ��ͬһ���µ�����
	bool has_YiPaoDuoXiang() {
		return FEAT_YiPaoDuoXiang || m_playtype[PT_YiPaoDuoXiang];
	}
	bool has_QiangGangHu() {
		return FEAT_QiangGangHu || m_playtype[PT_QiangGangHu];
	}
	bool has_WeiShangTingGangBuSuanFen() {
		return FEAT_WeiShangTingGangBuSuanFen || m_playtype[PT_WeiShangTingGangBuSuanFen];
	}
	int get_DealerRiskScore() {

		if (FEAT_DealerRiskScore
			&&  !FEAT_DealerRiskScoreDisabled  && m_playtype[PT_ZhuangJiaYiFen]) {
			return FEAT_DealerRiskScore+1;
		}
		if (FEAT_DealerRiskScore
			&& (!FEAT_DealerRiskScoreDisabled || m_playtype[PT_ZhuangSuanFen] )) {
			return FEAT_DealerRiskScore;
		}
		return 0;
	}



	void setup_feature_toggles(int game_type);
	void init_allowed_fans();
	void vectorize_fans(FANXING fans, std::vector<Lint> &vec);
	tile_mask_t calc_hu_candidate_set(tile_mask_t mask) const;
	FANXING call_judge(PlayerState &ps, size_t num_huns, TileVec const &hands, tile_t last_tile, FANXING conf
		, tile_mask_t pg_mask, pg_tile_count_by_color_t const &sum_pg_tile_count_by_color) const;
	FANXING reduce_fans(FANXING fans, FANXING const conf) const;
	FANXING reduce_fans_in_TingKou_stage(FANXING fans, FANXING const conf) const;
	int calc_zui_score(FANXING fans) const;
	//// Ӳ���� �ں��ķ�����Ϊ��С�ԣ�����ļ��������ĺ���  by wyz
	int calc_zui_score_spec_qixiaodui(TileVec const &hands, tile_mask_t pg_mask) const;
	int calc_common_loss(FANXING fans, tile_t last_tile, unsigned max_zone_card_count = 0) const;
	int calc_loss_factor(FANXING fans);

	int calc_next_zhuangpos(int curpos, int winpos, int total_gangs);
	int init_desk_card_remain_count();

};













