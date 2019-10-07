

#include "malgo.h"



// ��һɫ��ָ���ơ����ơ����ƶ���һ����ɫ��
// ȫ���򣬻���ȫ��Ͳ������ȫ������ȫ���ֲ���
static FANXING judge_qingyise(tile_mask_t mask) {
	//mask &= TILE_MASK_SHUPAI; // ���Ƴ���
	if (0 == (mask & ~TILE_MASK_WAN)) return FAN_QingYiSe;
	if (0 == (mask & ~TILE_MASK_TONG)) return FAN_QingYiSe;
	if (0 == (mask & ~TILE_MASK_TIAO)) return FAN_QingYiSe;
	return FAN_NONE;
}

// ��һɫ��ָ��Ͳ�������ұ���ֻ����һ�֣����������ƣ�û�����ƾ�����һɫ��
static FANXING judge_couyise(tile_mask_t mask) {
	// û�����Ʋ���
	if (0 == (mask & TILE_MASK_ZIPAI)) return FAN_NONE;
	// û�����Ʋ���
	if (0 == (mask & TILE_MASK_SHUPAI)) return FAN_NONE;
	// Ȼ���ų����ƣ�ʣ�µ�����һɫ������
	mask &= TILE_MASK_SHUPAI; // ���Ƴ���
	return judge_qingyise(mask) ? FAN_CouYiSe : FAN_NONE;
}

// ����ʱ����û��1��9�ͷ���
static FANXING judge_duanyao(tile_mask_t mask) {
	return 0 == (mask & ~TILE_MASK_DuanYao) ? FAN_DuanYao : FAN_NONE;
}

// ȱ�ţ��������������⣩ <= 2
static int calc_quemen_count(tile_mask_t mask) {
	int sum = 0;
	if (0 == (mask & TILE_MASK_WAN)) ++sum;
	if (0 == (mask & TILE_MASK_TONG)) ++sum;
	if (0 == (mask & TILE_MASK_TIAO)) ++sum;
	return sum;
}

// ��Щ�ж�����һЩ���ͱ�ǣ������ջ��� conf ȡ����������ÿ�ζ��� conf ȡ����
static FANXING judge_quemen(tile_mask_t all_mask) {
	// ����ȱ����Ŀ���ô����е���ֵ���ϣ����������ܵ�
	int n = calc_quemen_count(all_mask);
	switch (n) {
	case 1: return FAN_QueMen | FAN_QueYiMen;
	case 2: return FAN_QueMen | FAN_QueLiangMen;
	case 3: return FAN_QueMen | FAN_ZiYiSe | FAN_FengYiSe;
	}
	return FAN_NONE;
}

static FANXING judge_gujiang(TileZoneDivider const &stats, tile_mask_t pg_mask) {
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		// �½����㲻��������
		if (tc == TILE_COLOR_ZI) continue;
		// ���е�Ͳ�������򲻹���һ������ֻ��һ���ƾͽй½�
		// ע���������Ƿ������ƣ��������ܡ���
		// ע����������Ѿ������ˣ��Ǻ��ƺ���еĶ����ж�
		unsigned zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];
		// �½��ÿ���������
		if (zone_size == 2 && (pg_mask & tile_color_to_mask(tc)) == 0) return FAN_GuJiang;
	}
	return FAN_NONE;
}

// �����ԣ�ֻ֧�ֲ��������Ƶ�
FANXING judge_biankandiao(HuBranch const &b, tile_t last_tile, FANXING conf) {
	FANXING fans = FAN_NONE;
	TILE_COLOR last_color = tile_to_color(last_tile);
	unsigned last_tn = tile_to_number(last_tile);
	unsigned target_bian = 99;
	if (last_tn == 3) target_bian = tile_make(last_color, 1);
	else if (last_tn == 7) target_bian = last_tile;
	for (HunItem const *p = b.begin(); p != b.end(); ++p) {
		// �߿����ǻ����
		// ���ţ�ֻ��3��7�����ҵ���12��3��89��7
		if ((conf & FAN_BianZhang) && HUN0_SHUN == p->kind && p->tile == target_bian) fans |= FAN_BianZhang;
		// ���ţ�Ҫ��������˳���м����
		else if ((conf & FAN_KanZhang) && HUN0_SHUN == p->kind && last_tile == p->tile + 1) fans |= FAN_KanZhang;
		// ���ţ������������ڽ�����
		else if ((conf & FAN_DiaoZhang) && HUN0_JIANG == p->kind && p->tile == last_tile) fans |= FAN_DiaoZhang;
	}
	// ���߿��������ȼ�����
	FANXING a1 = fans & FAN_BianZhang;
	FANXING a2 = fans & FAN_KanZhang;
	FANXING a3 = fans & FAN_DiaoZhang;
	if (fans & FAN_BianZhang) return FAN_BianZhang;
	if (fans & FAN_KanZhang) return FAN_KanZhang;
	if (fans & FAN_DiaoZhang) return FAN_DiaoZhang;

	return FAN_NONE;
}

static FANXING judge_sanfeng(HuBranch &b) {
	int sum = 0;
	for (HunItem const *p = b.begin(); p != b.end(); ++p) {
		if (HUN0_SHUN_SanFeng == p->kind) ++sum;
	}
	switch (sum) {
	case 1: return FAN_SanFeng_X1;
	case 2: return FAN_SanFeng_X2;
	case 3: return FAN_SanFeng_X3;
	case 4: return FAN_SanFeng_X4;
	default: return FAN_NONE;
	}
}

static FANXING judge_sanyuan(HuBranch &b, tile_t last_tile) {
	int sum = 0;
	for (HunItem const *p = b.begin(); p != b.end(); ++p) {
		if (HUN0_SHUN_SanYuan == p->kind) ++sum;
	}
	if (0 == sum) return FAN_NONE;
	FANXING fans = FAN_NONE;
	if (!tile_is_JianPai(last_tile)) fans |= FAN_SanYuanZaiShou;
	switch (sum) {
	case 1: return fans | FAN_SanYuan_X1;
	case 2: return fans | FAN_SanYuan_X2;
	case 3: return fans | FAN_SanYuan_X3;
	case 4: return fans | FAN_SanYuan_X4;
	default: return FAN_NONE;
	}
}

static FANXING merge_special_fans(FANXING fans, FANXING const conf) {
	if (conf & FAN_QingLong) {
		if ((fans & FAN_QingYiSe) && (fans & FAN_YiTiaoLong)) {
			fans &= ~(FAN_QingYiSe | FAN_YiTiaoLong);
			fans |= FAN_QingLong;
		}
	}
	if (conf & FAN_QingQiDui) {
		if ((fans & FAN_QingYiSe) && (fans & FAN_QiXiaoDui)) {
			fans &= ~(FAN_QingYiSe | FAN_QiXiaoDui);
			fans |= FAN_QingQiDui;
		}
	}
	if (conf & FAN_QingHaoHuaQiXiaoDui) {
		if ((fans & FAN_QingYiSe) && (fans & FAN_HaoHuaQiXiaoDui)) {
			fans &= ~(FAN_QingYiSe | FAN_HaoHuaQiXiaoDui);
			fans |= FAN_QingHaoHuaQiXiaoDui;
		}
	}
	return fans;
}


FANXING HuJudgement::later_stage(PlayerState &ps, FANXING fans) {
	if (conf & FAN_ZiMo) fans |= FAN_ZiMo;
	if (conf & FAN_QingYiSe) fans |= judge_qingyise(all_mask);
	if (conf & FAN_CouYiSe) fans |= judge_couyise(all_mask);
	if (conf & FAN_DuanYao) fans |= judge_duanyao(all_mask);
	if (conf & FAN_MenQing) fans |= ps.is_menqing() ? FAN_MenQing : FAN_NONE;
	if (conf & FAN_JUDGE_QueMen) fans |= judge_quemen(all_mask);
	if (conf & FAN_GuJiang) fans |= judge_gujiang(stats, pg_mask);
	if (conf & FAN_JUDGE_BianKanDiao) fans |= judge_biankandiao(selected_branch, last_tile, conf);
	if (conf & FAN_MUST_BianKanDiao) {
		if ((fans & FAN_BianKanDiao) == 0) return FAN_NONE;
	}

	
	if (conf & FAN_JUDGE_SanFeng) fans |= judge_sanfeng(selected_branch);
	if (conf & FAN_JUDGE_SanYuan) fans |= judge_sanyuan(selected_branch, last_tile);
	
	fans = merge_special_fans(fans, conf);
	return fans;
}
