

#include "malgo.h"

// �鶴���Ƶ�7С�Բ������ƣ�ֻ��ԭ������ʹ��
static FANXING hongtongwangpai_check_qixiaodui(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {
	// ��С�Ե�14���ƣ��иܡ������е�
	if (num_huns + stats.total != 14) return FAN_NONE;
	if (num_huns % 2 != 0) return FAN_NONE;
	bool has4 = false; // ��������С����
	bool ge6_or_zi_has_hun = false;
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];
		if (zone_size % 2) return FAN_NONE;
		unsigned const *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
			if (a[i] % 2) {			
				return FAN_NONE;			 
			}
		
		}
	}
	
	// ��С�ӿ϶��ǵ����������Զ������ϣ����治��Ҫ�Ļ��ᱻ����������	
	return FAN_QiXiaoDui | FAN_DiaoZhang;
}

static FANXING check_shisanyao(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {
	// ʮ���۵�14���ƣ��иܡ������е�
	if (num_huns + stats.total != 14) return FAN_NONE;
	tile_mask_t mask = stats.mask;
	tile_mask_t intersection = mask & TILE_MASK_SHISANYAO;
	tile_mask_t diff = mask & ~TILE_MASK_SHISANYAO;
	if (mjalgo_popcnt(intersection) + num_huns >= 13 && diff == 0) {
		if (conf & FAN_MUST_KouDian) {
			// last_tile Ϊ 0 ˵�����һ���������ƣ�Ҳ���� num_huns �϶��ô��� 0
			if (last_tile == 0) {
				tile_mask_t koudian_hupai = (conf & FAN_ZiMo) ? TILE_MASK_KouDian_ZiMo : TILE_MASK_KouDian_ShouPao;
				// ȱ���Ƶ���������ȥ����������ⲿ���Ʋ����Ͽٵ��Ǹ�Ҫ�󣬿ٵ�ͺ�������
				tile_mask_t lack = TILE_MASK_SHISANYAO & ~intersection;
				if ((lack & koudian_hupai) == 0) return FAN_NONE;
			}
		}
		return FAN_ShiSanYao;
	}
	return FAN_NONE;
}


static FANXING check_qixiaodui(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {
	// ��С�Ե�14���ƣ��иܡ������е�
	if (num_huns + stats.total != 14) return FAN_NONE;
	bool has4 = false; // ��������С����
	bool ge6_or_zi_has_hun = false;
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned zone_size = stats.count_by_color[tc - TILE_COLOR_MIN];
		if (zone_size % 2 && 0 == num_huns) return FAN_NONE;
		unsigned const *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
			if (a[i] % 2) {
				if (0 == num_huns) return FAN_NONE;
				--num_huns;
				if (conf & FAN_MUST_KouDian) {
					unsigned min_limit = (conf & FAN_ZiMo) ? 2 : 5; // ����3�㣬����6��
					if (tc == TILE_COLOR_ZI || i >= min_limit) ge6_or_zi_has_hun = true;
				}
			}
			if (a[i] == 4 || a[i] == 3) has4 = true;
		}
	}
	if (conf & FAN_MUST_KouDian) {
		// ���һ���������ƣ�����û��ʣ����������ˣ�����ǰ���������Ҳû�ܷ��䵽�ߵ�����ȥ
		if (last_tile == 0 && 0 == num_huns && ge6_or_zi_has_hun == false) return FAN_NONE;
	}
	// ��С�ӿ϶��ǵ����������Զ������ϣ����治��Ҫ�Ļ��ᱻ����������
	if (has4) return FAN_HaoHuaQiXiaoDui | FAN_DiaoZhang;
	return FAN_QiXiaoDui | FAN_DiaoZhang;
}

static FANXING check_yitiaolong_in_color(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch, TILE_COLOR tc) {
	if (tc == TILE_COLOR_ZI) return FAN_NONE;
	bool ge6_or_zi_has_hun = false;
	unsigned *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
	for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
		if (a[i]) {
			--a[i];
			--stats.total;
			--stats.count_by_color[tc - TILE_COLOR_MIN];
			// stats.mask ���ô��������ƽ���ж��ò���
		} else {
			if (0 == num_huns) return FAN_NONE;
			// ȱ���û��Ʋ�
			--num_huns;
			if (conf & FAN_MUST_KouDian) {
				unsigned min_limit = (conf & FAN_ZiMo) ? 2 : 5; // ����3�㣬����6��
				if (tc == TILE_COLOR_ZI || i >= min_limit) ge6_or_zi_has_hun = true;
			}
		}
	}
	// ��ǰ�������������ƣ�Ҫ6�����ϵ��������������ˣ��Ͳ���ҪҪ��������
	if (last_tile == 0 && ge6_or_zi_has_hun == true) conf = (FANXING)(conf & ~FAN_MUST_KouDian);
	HuBranch branch;
	FANXING fans = malgo_can_pinghu(num_huns, stats, last_tile, conf, branch);
	if (fans) {
		selected_branch.extend(branch);
		fans |= FAN_YiTiaoLong;
	}
	return fans;
}

static FANXING check_yitiaolong(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch) {
	// (123)(456)(789)
	// ֻ��ͬһ��ɫ��123456789����
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		// һ���������޸�״̬��������Ҫһ����ʱ����
		TileZoneDivider tmp_stats(stats);
		FANXING fans = check_yitiaolong_in_color(num_huns, tmp_stats, last_tile, conf, selected_branch, tc);
		if (fans) return fans;
	}
	return FAN_NONE;
}

static FANXING check_pengpenghu(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf) {

	//��������5����
	if (stats.total < 5) 
	{
		return FAN_NONE;
	}
	// ȫ�ǿ���
	++num_huns; // ��һ�������ƣ��������԰ѽ��ӵ��ɿ��������
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_MAX; tc = (TILE_COLOR)(tc + 1)) {
		unsigned const *a = stats.count_by_color_by_number[tc - TILE_COLOR_MIN];
		for (unsigned i = 0; i < TILE_NUMBER_COUNT; ++i) {
			size_t needed_num_huns = (3 - a[i] % 3) % 3;
			if (num_huns < needed_num_huns) return FAN_NONE;
			num_huns -= needed_num_huns;
		}
	}

	//���һ���Ʊ�����ɿ��ӣ������ǽ���
	if (stats.count_by_color_by_number[tile_color_index(last_tile)][tile_to_number(last_tile) - 1] < 3)
		return FAN_NONE;
	return FAN_PengPengHu;
}

FANXING malgo_judge_big_fans(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch) {
	if (conf & FAN_ShiSanYao) {
		FANXING fans = check_shisanyao(num_huns, stats, last_tile, conf);
		if (fans) return fans;
	}
	if ( (conf & FAN_QiXiaoDui) && (conf & FAN_HongTong_YingKou))
	{
		FANXING fans = hongtongwangpai_check_qixiaodui(num_huns, stats, last_tile, conf);
		if (fans) return fans;
	}
	else
	{
		if (conf & FAN_QiXiaoDui) {
			FANXING fans = check_qixiaodui(num_huns, stats, last_tile, conf);
			if (fans) return fans;
		}
	}
	// �������Ƶ�һ�������ŵ�ƽ���㷨������Ѽ��㣬�������ɶ�������ǰ���
	if (conf & FAN_YiTiaoLong) {
		FANXING fans = check_yitiaolong(num_huns, stats, last_tile, conf, selected_branch);
		if (fans) return fans;
	}
	// ������Ҫ�����������ӣ����ʧ�ܣ���Ӱ��߿����ļ�⣬���Զ���������ǰ���
	if (conf & FAN_PengPengHu) {
		FANXING fans = check_pengpenghu(num_huns, stats, last_tile, conf);
		if (fans) return fans;
	}
	return FAN_NONE;
}





