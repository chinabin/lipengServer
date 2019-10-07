

#pragma warning(push, 4)
// warning C4100 : ��conf��: δ���õ��β�
#pragma warning(disable:4100)

#include "malgo.h"
#include "..\mhmessage\mhmsghead.h"



static FANXING plus_common_fans(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {
	FANXING fans = FAN_NONE;
	return fans;
}

static FANXING plus_pinghu_fans(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {
	FANXING fans = FAN_NONE;
	return fans;
}


FANXING malgo_can_pinghu(size_t num_huns, TileZoneDivider &saved_stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch) {
	TileZoneDivider stats(saved_stats);
	if (0 == num_huns) return malgo_unhun_can_pinghu(stats, last_tile, conf, selected_branch);
	else return malgo_hun_can_pinghu(num_huns, stats, last_tile, conf, selected_branch);
}

// ���粻����5������
static bool has_invalid_count(TileZoneDivider const &stats) {
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned const *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
			if (a[i] > 4) return true;
		}
	}
	return false;
}

struct CardCount
{
	unsigned count;
	unsigned index;
	CardCount() : count(0), index(0)
	{}
	CardCount(unsigned c, unsigned i) : count(c), index(i) {}
};

bool comp(const CardCount & a, const CardCount & b)
{
	return a.count > b.count;
}

bool hongtong_search_SanFengShun(unsigned * p, unsigned * pIndex)
{
	std::vector<CardCount> vCardCount;
	for (unsigned i = 0; i < 4; i++)
	{
		vCardCount.push_back(CardCount(p[i], i));
	}
	std::sort(vCardCount.begin(), vCardCount.end(), comp);
	
	unsigned nCount = 0;	 
	//MHLOG("********���������:");
	//MHLOG("Count: %d %d, %d %d", vCardCount[0].count, vCardCount[1].count, vCardCount[2].count, vCardCount[3].count);
	//MHLOG("Index: %d %d, %d %d", vCardCount[0].index, vCardCount[1].index, vCardCount[2].index, vCardCount[3].index);
	if (vCardCount[1].count == 1 && vCardCount[2].count == 1 && vCardCount[3].count == 1)
	{ 
		pIndex[0] = vCardCount[1].index;
		pIndex[1] = vCardCount[2].index;
		pIndex[2] = vCardCount[3].index;
		return true;
	}
	else
	{
		for (int i = 0; i < 4; i++)
		{
			if (vCardCount[i].count > 0) pIndex[nCount++] = vCardCount[i].index;
			if (nCount == 3)
			{
				return true;
			}
		}
	}
	return false;	 
}
// �鶴������������˳, ����������ɵ�˳��
unsigned hongtongwanpai_spec_remove_SanFengShun(TileZoneDivider & stats, tile_t last_tile, HuBranch & selected_branch)
{
	unsigned shun_count = 0;
	unsigned * p = stats.count_by_color_by_number[TILE_COLOR_ZI - TILE_COLOR_MIN];
	unsigned * pByColor = stats.count_by_color;

	// �������������������˳��
	 
	unsigned pI[3] = { 0 };
	bool bHasShun = hongtong_search_SanFengShun(p, pI);	
	 
	while (bHasShun)
	{
		//MHLOG("******************�������Ʋ���: bHasHun = %d, index1=%d, index2=%d, index3=%d", bHasShun, pI[0], pI[1], pI[2]);
		p[pI[0]] -= 1; p[pI[1]] -= 1; p[pI[2]] -= 1;  // ������
		pByColor[TILE_COLOR_ZI - TILE_COLOR_MIN] -= 3;
		stats.total -= 3;
		shun_count += 1;
		HunItem item(HUN0_SHUN_SanFeng, last_tile);
		selected_branch.add(item);	 
		bHasShun = hongtong_search_SanFengShun(p, pI);
	}
	if (p[0] < 1) stats.mask &= ~TILE_TO_MASK(41);
	if (p[1] < 1) stats.mask &= ~TILE_TO_MASK(42);
	if (p[2] < 1) stats.mask &= ~TILE_TO_MASK(43);
	if (p[3] < 1) stats.mask &= ~TILE_TO_MASK(44);
	
	return shun_count;
}

// �鶴���������з��׵�˳��
unsigned hongtongwanpai_spec_remove_SanYuanShun(TileZoneDivider & stats, tile_t last_tile, HuBranch & selected_branch)
{ 
	unsigned shun_count = 0;
	unsigned * p = stats.count_by_color_by_number[TILE_COLOR_ZI - TILE_COLOR_MIN];
	unsigned * pByColor = stats.count_by_color;
	while ( (p[4] > 0) && (p[5] > 0) && (p[6] > 0) )     
	{
		p[4] -= 1; p[5] -= 1; p[6] -= 1;  // ������
		pByColor[TILE_COLOR_ZI - TILE_COLOR_MIN] -= 3;
		stats.total -= 3;

		shun_count += 1;
		HunItem item(HUN0_SHUN_SanYuan, last_tile);
		selected_branch.add(item);
	}
	if (p[4] < 1) stats.mask &= ~TILE_TO_MASK(45);
	if (p[5] < 1) stats.mask &= ~TILE_TO_MASK(46);
	if (p[6] < 1) stats.mask &= ~TILE_TO_MASK(47);
	return shun_count;
}

FANXING malgo_can_hu(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf
	, tile_mask_t pg_mask, pg_tile_count_by_color_t sum_pg_tile_count_by_color, HuBranch &selected_branch, HuBranch &selected_branch_hongtong) {
	if (has_invalid_count(stats)) return FAN_NONE;	 
	if (conf & FAN_MUST_QueMen) {
		if (!PlayerState::is_quemen(stats.mask | pg_mask)) return FAN_NONE;

		// Ȼ��2�������Ƶ���ʱ����ȱ�ž����ˣ����϶����Բ���ȱ�Ű����������ֻ���ж��ܲ��ܺ�
	}
	// �鶴������ȱ2�ŵ��淨
	if (conf & FAN_HongTong_MustQueLiangMen)
	{
		int que_men_count = 0;
		if (0 == ((stats.mask | pg_mask)  & TILE_MASK_WAN)) ++que_men_count;
		if (0 == ((stats.mask | pg_mask) & TILE_MASK_TONG)) ++que_men_count;
		if (0 == ((stats.mask | pg_mask) & TILE_MASK_TIAO)) ++que_men_count;
		if (que_men_count < 2)
		{
			return FAN_NONE;
		}
	}

	if (conf & FAN_MUST_YiMenGouBaZhang) {
		bool has_big_men = false;
		for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
			unsigned zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];
			zone_size += sum_pg_tile_count_by_color[tc - TILE_COLOR_MIN];			 
			if (zone_size >= 8) {
				has_big_men = true;
				break;
			}
		}
		if (!has_big_men) return FAN_NONE;
	}
	if (last_tile) {
		// ���һ���Ʋ���������
		if (conf & FAN_MUST_KouDian) {
			unsigned min_points = (conf & FAN_ZiMo) ? 3 : 6; // �ٵ�������ƣ���������3�����ڵ���6
			if (tile_to_points(last_tile) < min_points) return FAN_NONE;
			// Ȼ����������Ǿ����ˣ��Ͳ�Ӧ����Ҫ��������
			conf = (FANXING)(conf & ~FAN_MUST_KouDian);
		}
	} else {
		// ����������ƣ����� FAN_MUST_KouDian ����Ҫ�������
	}
	 
	FANXING fans = malgo_judge_big_fans(num_huns, stats, last_tile, conf, selected_branch);	
	if (fans&FAN_PengPengHu) fans |= FAN_PingHu;     //���У�����������ƽ��

	// ԭʼ�ж�ƽ���߼����д�������������һ�����ı߿����ͼ�ⲻ�����ˣ�
	if (FAN_NONE == fans && (conf & FAN_PingHu)) {
		
		fans = malgo_can_pinghu(num_huns, stats, last_tile, conf, selected_branch);
	}

	// ����жϱ߿�������Ҫ��������һ��ƽ�����ж�(**Ren 2017-12-5)
	if ((conf & FAN_BianKanDiao) && (fans & FAN_YiTiaoLong))
	{
		TileZoneDivider temp_stats_biankadiao(stats);
		HuBranch temp_branch_biankadiao;
		malgo_can_pinghu(num_huns, temp_stats_biankadiao, last_tile, conf, temp_branch_biankadiao);
		fans |= judge_biankandiao(temp_branch_biankadiao, last_tile, conf);	
	}
	
	FANXING fan_hongtong_diaowang = FAN_NONE;
	if ( fans && (conf & FAN_HongTong_DiaoWang))
	{
		if (num_huns > 0)
		{
			TileZoneDivider temp_stats(stats);
			HuBranch temp_branch;
			fan_hongtong_diaowang = hongtong_judge_diaowang(num_huns, temp_stats, last_tile, conf, temp_branch);			
		}
	}


	// ������ˣ��ͰѶ�����Ҳ������
	if (fans) {
		fans |= plus_common_fans(num_huns, stats, last_tile, conf);
		if (fans & FAN_PingHu) fans |= plus_pinghu_fans(num_huns, stats, last_tile, conf);
		fans |= fan_hongtong_diaowang;
	}

	return (FANXING)fans;
}



HuJudgement::HuJudgement(PlayerState &ps, size_t num_huns, TileVec const &hands, tile_t last_tile, FANXING conf, tile_mask_t pg_mask
	, pg_tile_count_by_color_t const &sum_pg_tile_count_by_color)
	: num_huns(num_huns), hands(hands), last_tile(last_tile), conf(conf)
	, pg_mask(pg_mask), sum_pg_tile_count_by_color(sum_pg_tile_count_by_color)
	, stats(hands.begin(), hands.size()), all_mask(stats.mask | pg_mask) {

	//for (int i = 0; i < hands.size(); i++)
	//{
		//LLOG_DEBUG("*******************hand card i=%d, card = %d", i, hands[i]);
	//}
	TileZoneDivider tmp_stats(stats);
	fans = malgo_can_hu(num_huns, tmp_stats, last_tile, conf, pg_mask, sum_pg_tile_count_by_color, selected_branch, selected_branch_hongtong);	
	//MHLOG("*******************malgo_can_hu output fans: %lld lasttile= %d, num_huns = %d", fans, last_tile, num_huns);

	//�괨�齫���ƺ�������(�������) ���������Ƿ����㿿��������������������������������ܺ���
	if ( (conf & FAN_LingChuan_Must_KaoBaZhang) && (fans)) {
		if (fans & FAN_ShiSanYao)
		{
			ps.lingchuan_loss_score = 14; // ʮ���۷����̶�Ϊ14��
		}
		else
		{
			pg_tile_count_by_color_t pg_count_by_color = ps.sum_pg_tile_count_by_color_for_lingchuanmajiang();
			TILE_COLOR tc = TILE_COLOR_MIN;
			unsigned zone_size = 0;
			bool has_big_men = false;
			for (; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
				zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];				
				if ((zone_size + pg_count_by_color[tc - TILE_COLOR_MIN]) >= 8) {
					has_big_men = true;
					break;
				}
			}
			if (has_big_men)
			{
				ps.calc_last_score_for_lingchuanmajiang(tc,zone_size);
			}
			else      //�����㿿���ŵ����������ܺ���
			{
				fans = FAN_NONE;
			}
		}
	}
	if (fans) fans = later_stage(ps, fans);	
}





#pragma warning(pop)







