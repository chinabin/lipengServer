

#include "malgo.h"



// ����Եһ�����ܲ��ܺͺ�����Ƴ�Ϊ˳�ӣ��еĻ����޳����˳��
static bool malgo_can_zheng__remove_shunzi(TILE_COLOR tc, unsigned *a, HuBranch &branch, unsigned i) {
	if (i + 2 >= TILE_NUMBER_COUNT) return false;
	if (a[i + 1] == 0 || a[i + 2] == 0) return false;
	a[i] -= 1;
	a[i + 1] -= 1;
	a[i + 2] -= 1;
	branch.add(HunItem(HUN0_SHUN, tile_make(tc, i + 1)));
	return true;
}

// �Ѿ�֪����Ե3�����ǿ��ӣ��޳�����
static void malgo_can_zheng__remove_kezi(TILE_COLOR tc, unsigned *a, HuBranch &branch, unsigned i) {
	a[i] -= 3;
	branch.add(HunItem(HUN0_KE, tile_make(tc, i + 1)));
}

static bool malgo_can_zheng__shupai(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i);

// �ߵ������˵��Ե��1����ͬ����
static bool malgo_can_zheng__on_1(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	return malgo_can_zheng__shupai(tc, a, branch, conf, i);
}

// �ߵ������˵��Ե��2����ͬ����
static bool malgo_can_zheng__on_2(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	return malgo_can_zheng__shupai(tc, a, branch, conf, i);
}

static bool malgo_can_zheng__on_3__try_shunzi(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	return malgo_can_zheng__shupai(tc, a, branch, conf, i);
}

// �ߵ������˵��Ե��3����ͬ����
static bool malgo_can_zheng__on_3(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	// ����Ҫ��֧��ʱ���������
	unsigned saved_a[TILE_NUMBER_COUNT];
	memcpy(saved_a, a, sizeof(saved_a));
	malgo_can_zheng__remove_kezi(tc, a, branch, i);
	// �ѱ�Ե3����Ϊ�����ã�ʣ�µ��ƺ����˵Ļ����Ǳ�Ե9�ŵ�3��˳���ÿ϶�Ҳ������
	// ���ֻ��Ҫ���һ�־�����
	// ����������˳�ӣ��������ѵ����Ž�
	bool kezi_checked = false;
	bool shunzi_checked = false;
	if (conf & FAN_BianKanDiao) goto check_shunzi;
check_kezi:
	if (malgo_can_zheng__shupai(tc, a, branch, conf, i + 1)) return true;
	kezi_checked = true;
	if (shunzi_checked) return false;
check_shunzi:
	if (malgo_can_zheng__on_3__try_shunzi(tc, saved_a, branch, conf, i)) return true;
	shunzi_checked = true;
	if (!kezi_checked) goto check_kezi;
	return false;
}

// �ߵ������˵��Ե��4����ͬ����
static bool malgo_can_zheng__on_4(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	if (!malgo_can_zheng__remove_shunzi(tc, a, branch, i)) return false;
	return malgo_can_zheng__on_3(tc, a, branch, conf, i);
}

// �ܲ��ܰ�����(���ӡ�˳��)�������
// ����confָ��������˳��ѡ��һ��·�����
// ����ܹ����������������ϣ�����ʼ�˿�ʼ�жϣ������ܺ�����ǰ�����ˣ��п��ܺ��ͼ��������ж�
// �õݹ�ʵ�ֵ������������
static bool malgo_can_zheng__shupai(TILE_COLOR tc, unsigned *a, HuBranch &branch, FANXING conf, unsigned i) {
	// ����ǿյģ�˵���ܰ������������
	while (i < TILE_NUMBER_COUNT && 0 == a[i]) ++i;
	if (i >= TILE_NUMBER_COUNT) return true;
	unsigned count = a[i];
	// �ж�3n�����Ƿ������˳�ӻ����ǰ��̣���ͨ���жϱ�Ե�Ƶ�������
	// (1) �����1�Ż�2�ţ�Ҫ���ƵĻ���������ͺ���������˳�ӣ����Բ��ܺ���
	// (3) �����3�ţ�Ҫ���������ֿ��ܣ�һ���ǰ�3�ŵ����ǰ��̣���һ������3�Ŷ������������˳�ӡ�
	//	     ���ֻ���жϺ������ƣ�����������һ�ֶ���һ���ģ����Ҫ�����������Ҫ�Լ��ֿ����ˡ�
	// (4) �����4�ţ�Ҫ���ƵĻ�����1��Ҫ�ͺ���������˳�ӣ�ʣ�µ�3�����ֿ��Լ���(3)�Ĳ�����
	switch (count) {
	case 1: return malgo_can_zheng__on_1(tc, a, branch, conf, i);
	case 2: return malgo_can_zheng__on_2(tc, a, branch, conf, i);
	case 3: return malgo_can_zheng__on_3(tc, a, branch, conf, i);
	case 4: return malgo_can_zheng__on_4(tc, a, branch, conf, i);
	}
	return false;
}

static bool malgo_can_zheng__FengPai(tile_t last_tile, unsigned *a, HuBranch &branch) {
	for (unsigned i = 0; i < 4; ++i) {
		if (a[i] % 3 != 0) return false;
		if (a[i]) branch.add(HunItem(HUN0_KE, tile_make(TILE_COLOR_ZI, i + 1)));
	}
	return true;
}

static bool malgo_can_zheng__JianPai(tile_t last_tile, unsigned *a, HuBranch &branch) {
	for (unsigned i = 4; i < 7; ++i) {
		if (a[i] % 3 != 0) return false;
		if (a[i]) branch.add(HunItem(HUN0_KE, tile_make(TILE_COLOR_ZI, i + 1)));
	}
	return true;
}

static bool all_have(unsigned *a, unsigned n, unsigned count) {
	for (unsigned i = 0; i < n; ++i) {
		if (a[i] < count) return false;
	}
	return true;
}
static void all_decrease(unsigned *a, unsigned n, unsigned count) {
	for (unsigned i = 0; i < n; ++i) a[i] -= count;
}

static int malgo_can_zheng__SanFeng__select_maximized_branch(tile_t last_tile, unsigned *a, HuBranch &branch, unsigned i, unsigned max_left);
static int malgo_can_zheng__SanFeng__go_this_branch(tile_t const last_tile, unsigned *a, HuBranch &branch, unsigned const i, unsigned const n, unsigned const max_left) {
	// ���嵽ĳ��λ�õ�ĳ��������������Ƿ���ڣ���������ڣ�˵�������֧��Ч�������������� -999
	if (n) {
		// ��Ŀ��λ��������n���磬����ķ��Ƿ���n��
		a[i] += n;
		if (all_have(a, 4, n)) {
			all_decrease(a, 4, n);
		} else {
			a[i] -= n;
			return -999;
		}
	}
	// ����һ��λ���ϵ�5��������������
	// ���û����һ��λ���ˣ��Ϳ�ʣ������ܲ�������
	// ���û��ʣ�����������ȥ�����ˣ�ҲҪ��ʣ������ܲ�������
	unsigned total = n;
	if (i + 1 >= 4 || n == max_left) {
		// ʣ��������оͺã���Ȼ�ͷ���һ����������ʾ�����֧��Ч
		if (!malgo_can_zheng__FengPai(last_tile, a, branch)) return -999;
	} else {
		// ����������жϣ�������϶�����ʣ����������ɹ����ԣ��Ǿͼ�������һ��λ��
		total += malgo_can_zheng__SanFeng__select_maximized_branch(last_tile, a, branch, i + 1, max_left - n);
	}

	// ȷ�������֧����Ч�ģ����������磬����� HuBranch
	if (total > 0) {
		branch.add_n(n, HunItem(HUN0_SHUN_SanFeng, tile_make(TILE_COLOR_ZI, i + 1)));
	}
	return total;
}

static int malgo_can_zheng__SanFeng__select_maximized_branch(tile_t last_tile, unsigned *a, HuBranch &branch, unsigned i, unsigned max_left) {
	int counts[5];
	HuBranch branches[5];
	for (unsigned n = 0; n <= max_left; ++n) {
		unsigned a_copy[4];
		memcpy(a_copy, a, sizeof(a[0]) * 4);
		counts[n] = malgo_can_zheng__SanFeng__go_this_branch(last_tile, a_copy, branches[n], i, n, max_left);
	}
	size_t n = std::max_element(counts, counts + max_left + 1) - counts;
	int count = counts[n];
	if (count >= 0)	branch.extend(branches[n]);
	return count;
}

static bool malgo_can_zheng__SanFeng(tile_t last_tile, unsigned *a, HuBranch &branch) {
	// ���ⲻһ����3�����Ϊһ�죬����һ��˳�ӣ��ܺ����õ�
	// 4��λ�ã�ÿ��λ����01234��������5�ֿ����ԣ��ܵ������������4��������Ŀ����������Ŀ���
	int count = malgo_can_zheng__SanFeng__select_maximized_branch(last_tile, a, branch, 0, 4);
	// ��ֵ��ʾ�������У���ֵ��ʾ�����֧���м�������
	return count >= 0;
}

static bool malgo_can_zheng__SanYuan(tile_t last_tile, unsigned *a, HuBranch &branch) {
	while (all_have(a + 4, 3, 1)) {
		all_decrease(a + 4, 3, 1);
		branch.add(HunItem(HUN0_SHUN_SanYuan, 45));
	}
	return malgo_can_zheng__JianPai(last_tile, a, branch);
}

typedef bool(*ZiPaiHandler)(tile_t last_tile, unsigned *a, HuBranch &branch);
static ZiPaiHandler GetFengPaiHandler(FANXING conf) {
	if (conf & FAN_JUDGE_SanFeng) return malgo_can_zheng__SanFeng;
	return malgo_can_zheng__FengPai;
}
static ZiPaiHandler GetJianPaiHandler(FANXING conf) {
	if (conf & FAN_JUDGE_SanYuan) return malgo_can_zheng__SanYuan;
	return malgo_can_zheng__JianPai;
}

bool malgo_can_zheng(TILE_COLOR tc, tile_t last_tile, unsigned *a, HuBranch &branch, FANXING conf) {
	if (tc == TILE_COLOR_ZI) {
		// ���Ʒ粿�ͼ�������������˵�����޹صģ����Էֿ�����Ҳ��û�����
		return GetFengPaiHandler(conf)(last_tile, a, branch) && GetJianPaiHandler(conf)(last_tile, a, branch);
	}
	return malgo_can_zheng__shupai(tc, a, branch, conf, 0);
}




