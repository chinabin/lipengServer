#pragma once


/*
error C4996 : 'std::_Copy_backward' : Function call with parameters that may be unsafe -
this call relies on the caller to check that the passed values are correct.
To disable this warning, use - D_SCL_SECURE_NO_WARNINGS.See documentation on how to use Visual C++ 'Checked Iterators'
*/
#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS
#endif
// ��������д�������ڱ�׼�����֮ǰʹ�ã�������ʱ���׼���Ѿ��ڸ��ļ�֮ǰ��������
#pragma warning(disable:4996)



#include <algorithm>
#include <vector>
#include <new>
#include <sstream>

#include <cstring>
#include <cstdlib>
#include <cstddef> // ptrdiff_t


// ע�� HuUnit ����Ҫͬ���޸�
typedef unsigned long long FANXING;

// ��������Ͳ����Լ�ȥ�˶Գ���ֵ�ˣ��Զ�Ψһ
// ������ enum����Ϊ enum ֻ��32λ���ֲ����� C++11
static FANXING const FANXING_COUNTER_BASE = __COUNTER__ + 1;
#define FANXING_DEF(name) static FANXING const name = 1ull << (__COUNTER__ - FANXING_COUNTER_BASE)
#define FANXING_DEF_BY(name, value) static FANXING const name = value

static FANXING const FAN_NONE = 0;


FANXING_DEF(FAN_PingHu); // ƽ��
FANXING_DEF(FAN_ShiSanYao); // ʮ����
FANXING_DEF(FAN_HaoHuaQiXiaoDui); // ������С��
FANXING_DEF(FAN_QiXiaoDui); // ��С��
FANXING_DEF(FAN_YiTiaoLong); // һ����
FANXING_DEF(FAN_PengPengHu); // ������
FANXING_DEF(FAN_FengYiSe); // ��һɫ
FANXING_DEF(FAN_QingYiSe); // ��һɫ
FANXING_DEF(FAN_CouYiSe); // ��һɫ
FANXING_DEF(FAN_ZiYiSe); // ��һɫ���ͷ�һɫ����һ������ͬ�ط��з���һ��

// �ϲ����ⷬ�ͺ����Ͻз�
FANXING_DEF(FAN_QingLong); // ����
FANXING_DEF(FAN_QingQiDui); // ���߶�
FANXING_DEF(FAN_QingHaoHuaQiXiaoDui); // �������С��

FANXING_DEF(FAN_QueMen); // ȱ��
FANXING_DEF(FAN_MenQing); // ����
FANXING_DEF(FAN_DuanYao); // ���ۣ�����ʱ����û��1��9�ͷ���
FANXING_DEF(FAN_KanHu); // ����
FANXING_DEF(FAN_BianZhang); // ����
FANXING_DEF(FAN_KanZhang); // ����
FANXING_DEF(FAN_DiaoZhang); // ����
FANXING_DEF(FAN_DanDiao); // ����
// ����ΪӲ�������������Ų�����һ��Ӯ��Ϊ����

// ȱ�ţ�ȱһ��1�죬ȱ����3�죬ȱ���Ű���һɫ��
// ȱ��ֻ������ƣ����������Ƶ�
FANXING_DEF(FAN_QueYiMen); // ȱһ��
FANXING_DEF(FAN_QueLiangMen); // ȱ����
// �½���1�죬ֻ��һ������һ�Խ�������з��ײ��㣬���е�Ͳ�������򲻹���һ������ֻ��һ���ƾͽй½�
FANXING_DEF(FAN_GuJiang); // �½�
FANXING_DEF(FAN_YiZhangYing); // һ��Ӯ���������ڴ�СΪһ����Ҫ�����ڼ���ʱ�����
// ���磺1��1�졢����3�졢����10�죬���ⲻһ����3�����Ϊһ��
FANXING_DEF(FAN_SanFeng_X1); // ����X1
FANXING_DEF(FAN_SanFeng_X2); // ����X2
FANXING_DEF(FAN_SanFeng_X3); // ����X3
FANXING_DEF(FAN_SanFeng_X4); // ����X4
// ��Ԫ�����С������ס���һ�ųɵ�˳��һ��������1�죬һ��������2�죻����������5���죬����������7���죻3��������50�죬3��������50���졣
// ���ֺ�������������13�ź�14�ŵ�����13�����֣�������һ�ž���14����
// ������2���С�2������1���ף��������һ���ף����ˣ������2�����������ֻ�������һ������û���з��׾Ϳ�����
FANXING_DEF(FAN_SanYuan_X1); // ��ԪX1
FANXING_DEF(FAN_SanYuan_X2); // ��ԪX2
FANXING_DEF(FAN_SanYuan_X3); // ��ԪX3
FANXING_DEF(FAN_SanYuan_X4); // ��ԪX4
FANXING_DEF(FAN_SanYuanZaiShou); // ��Ԫ���֣���������Ʋ����з���

FANXING_DEF_BY(FAN_JUDGE_QueMen, FAN_QueMen | FAN_QueYiMen | FAN_QueLiangMen | FAN_FengYiSe | FAN_ZiYiSe);
FANXING_DEF_BY(FAN_BianKanDiao, FAN_BianZhang | FAN_KanZhang | FAN_DiaoZhang);
FANXING_DEF_BY(FAN_JUDGE_BianKanDiao, FAN_BianKanDiao);
FANXING_DEF_BY(FAN_JUDGE_SanFeng, FAN_SanFeng_X1 | FAN_SanFeng_X2 | FAN_SanFeng_X3 | FAN_SanFeng_X4);
FANXING_DEF_BY(FAN_JUDGE_SanYuan, FAN_SanYuan_X1 | FAN_SanYuan_X2 | FAN_SanYuan_X3 | FAN_SanYuan_X4 | FAN_SanYuanZaiShou);

//�����ӣ���**Ren 2017-11-29��
FANXING_DEF_BY(FAN_HUCHI_QueMen, FAN_QingYiSe | FAN_CouYiSe | FAN_QingLong | FAN_ZiYiSe);

FANXING_DEF(FAN_YiWuJiu); // һ���

FANXING_DEF(FAN_ZiMo); // ����
FANXING_DEF(FAN_Zhuang); // ׯ��

// �ٵ�����һ����Ҫ������ʱҪ�����������3������ʱҪ�����������6
// last_tileΪ0��ʾ����̵�����������
FANXING_DEF(FAN_MUST_KouDian); // �ٵ����һ���Ƶ���Ҫ��
// ����Ҫ�����ȱ�Ų��ܺ�
FANXING_DEF(FAN_MUST_QueMen); // ����ȱ�Ų��ܺ�
// ������Ҫ�������һ���ƹ�8�Ż������Ϸ��ɺ���
// Ŀǰ��ʵ��δ����������
FANXING_DEF(FAN_MUST_YiMenGouBaZhang); // ����һ�Ź����Ų��ܺ�
FANXING_DEF(FAN_MUST_BianKanDiao); // �����б��š����š����ߵ��Ų��ܺ�

FANXING_DEF(FAN_HongTong_MustQueLiangMen);
// �鶴����¥��¥�ķ��ͣ�Ҫ����������1�����������з������е�1��
FANXING_DEF(FAN_HongTong_LouShangLou); // �鶴¥��¥
FANXING_DEF(FAN_HongTong_YingKou);     // �鶴Ӳ��
FANXING_DEF(FAN_HongTong_DiaoWang);    // �鶴����

// ��������ׯ��
FANXING_DEF(FAN_LianZhuang);

// �괨�齫�������淨����������2�����Ƽ�һ�����ԣ�ʮ����û���������
FANXING_DEF(FAN_LingChuan_Must_KaoBaZhang);

// ��С�Ժ�һ�����ǻ���ģ�����һ��������ƽ������С����
// �����ֶ��壬Ӳ������ܴ���ָߣ�Ҳ����С���ָߣ��������ֶ���⣬Ȼ��Ƚ�һ�·�ֵ
FANXING_DEF_BY(FAN_JUDGE_XiaoHu, FAN_PingHu);
FANXING_DEF_BY(FAN_JUDGE_QiDui, FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_QingQiDui | FAN_QingHaoHuaQiXiaoDui);
FANXING_DEF_BY(FAN_JUDGE_YiTiaoLong, FAN_YiTiaoLong | FAN_QingLong);
FANXING_DEF_BY(FAN_JUDGE_DaHu, FAN_JUDGE_QiDui | FAN_JUDGE_YiTiaoLong);

// ����ƽ����һɫ�Ͳ���ʾƽ��
FANXING_DEF_BY(FAN_BIG_EFFECTS, FAN_ShiSanYao | FAN_JUDGE_QiDui | FAN_JUDGE_YiTiaoLong | FAN_FengYiSe | FAN_QingYiSe | FAN_CouYiSe | FAN_ZiYiSe | FAN_PengPengHu | FAN_KanHu);
// ���ⷬ�ͱ�����С�ԡ���һɫʱ���͵����ε��߿����������
FANXING_DEF_BY(FAN_SMALL_EFFECTS, FAN_PingHu | FAN_BianKanDiao | FAN_QueMen | FAN_MenQing | FAN_DuanYao);
// �û�allowed_fans
FANXING_DEF_BY(FAN_BASIC_ALLOWED, FAN_PingHu);


#undef FANXING_DEF
#undef FANXING_DEF_BY


typedef char FANXING_ASSERT_MAX[__COUNTER__ - FANXING_COUNTER_BASE <= 64 ? 1 : -1];









#include "malgo_popcnt.h"
#include "malgo_tile.h"
#include "malgo_structs.h"




FANXING malgo_judge_big_fans(size_t num_huns, TileZoneDivider const &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch);
FANXING malgo_can_pinghu(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch);
FANXING malgo_can_hu(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf
	, tile_mask_t pg_mask, pg_tile_count_by_color_t sum_pg_tile_count_by_color, HuBranch &selected_branch);


FANXING malgo_unhun_can_pinghu(TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch &selected_branch);

bool malgo_can_zheng(TILE_COLOR tc, tile_t last_tile, unsigned *a, HuBranch &branch, FANXING conf);

unsigned malgo_min_huns(unsigned const p[9], bool is_zipai, unsigned jiang_pos = -1);

unsigned tile_min_huns_for_hongtongwangpai_zipai_sanyuan_sanfeng(unsigned const p[9], unsigned head, unsigned tail, unsigned jiang_pos, tile_t last_tile, HuBranch &branch);

FANXING malgo_hun_can_pinghu(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch & branch);
FANXING hongtong_judge_diaowang(size_t num_huns, TileZoneDivider &stats, tile_t last_tile, FANXING conf, HuBranch & branch);

std::string malgo_format_fans(FANXING fans);
//***Ren
FANXING judge_biankandiao(HuBranch const &b, tile_t last_tile, FANXING conf);



/*
����Լ����
1��last_tile Ϊ 0 ��ʾ�������������������
*/
struct HuJudgement: HuJudgementResult {
	// ����������Լ���������ȷ�����м�״̬
	size_t const num_huns;
	TileVec const hands;
	tile_t const last_tile;
	FANXING const conf;
	tile_mask_t const pg_mask;
	pg_tile_count_by_color_t const sum_pg_tile_count_by_color;
	TileZoneDivider const stats;
	tile_mask_t const all_mask;
	// �м�״̬
	HuBranch selected_branch;
	// �м�״̬2 -- �鶴����ʹ��
	HuBranch selected_branch_hongtong;

	// ִ�к���
	HuJudgement(PlayerState &ps, size_t num_huns, TileVec const &hands, tile_t last_tile, FANXING conf, tile_mask_t pg_mask
		, pg_tile_count_by_color_t const &sum_pg_tile_count_by_color);
	FANXING later_stage(PlayerState &ps, FANXING fans);
};


