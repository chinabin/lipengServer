

#include "ClientConstants.h"





// ��������淨�����Сѡ��

// ����Сѡ�����󣺰��ܿɼ�����С�ԡ���һɫ����һɫ����һɫ������ֻ����������ׯ���ĸܡ����ƿɸܡ�ֻ�к�����Ҹ����
static int const ShanXiGameOptions_JinZhong[] = {
	PT_AnGangKeJian, 
	PT_GuoHuZhiKeZiMo, PT_TingPaiKeGang, PT_HuangZhuangBuHuangGang, PT_ZhiYouHuPaiWanJiaGangSuanFen, 
	PT_QiXiaoDui, PT_FengYiSe, PT_QingYiSe, PT_CouYiSe, 
};

// ƽ³�����ܿɼ���δ�������ڰ��⡢�������ơ�һ�ڶ��졢��С�ԡ�һ���(Ĭ��)������ͷ(Ĭ��)����һɫ(Ĭ��)��һ����(Ĭ��)
static int const ShanXiGameOptions_PingLu[] = {
	PT_AnGangKeJian,
	PT_WeiShangTingDianPaoBaoPei, PT_BaoTing, PT_YiPaoDuoXiang, 
	PT_QiXiaoDui, PT_YiWuJiu, PT_DongLingTou, PT_QingYiSe, PT_YiTiaoLong, 
};

// Ӳ���죺���ܿɼ���һ�ڶ��졢ׯ��1��(Ĭ��)����С��(Ĭ��)����һɫ(Ĭ��)����һɫ(Ĭ��)������(Ĭ��)������С��(Ĭ��)��������С��(Ĭ��)��һ����(Ĭ��)
static int const ShanXiGameOptions_YingSanZui[] = {
	PT_AnGangKeJian,
	PT_YiPaoDuoXiang, PT_ZhuangJiaYiFen,
	PT_QiXiaoDui, PT_QingYiSe, PT_ZiYiSe, PT_QingLong, PT_QingQiXiaoDui, PT_HaoHuaQiXiaoDui, PT_YiTiaoLong,
};

// �鶴���ƣ����ܿɼ����ܲ������ȱ���š�����(Ĭ��)����Ԫ����(Ĭ��)������(Ĭ��)���������С��(Ĭ��)
static int const ShanXiGameOptions_HongTongWangPai[] = {
	PT_AnGangKeJian, PT_CouYiSe,
	PT_GangBuSuiHu, PT_QueLiangMen, PT_ZiPai, PT_SanYuanZiMo, PT_DiaoWang, 
	PT_QingHaoHuaQiXiaoDui, PT_HongTongBuKePengPai,PT_HuangZhuangBuHuangGang
};

// �ٷ�һ���ƣ����ܿɼ�����ҳ��ׯ��1�֡�һ��Ӯ(Ĭ��)����һɫ(Ĭ��)����һɫ(Ĭ��)����Ԫ(Ĭ��)����һɫ�ߴ��(Ĭ��)��˫������С��(Ĭ��)������(Ĭ��)
static int const ShanXiGameOptions_LinFenYiMenPai[] = {
	PT_AnGangKeJian,
	PT_ShuYe, PT_ZhuangJiaYiFen, PT_YiZhangYing, PT_QingYiSe, PT_ZiYiSe, PT_SanYuan, 
	PT_ZiYiSeQiDaDui, PT_ShuangHaoHuaQiXiaoDui, PT_SanFeng,  MH_PT_HuangZhuangLunZhuang
};





bool shanxi_game_option_validate_by_game_type(int v, int game_type) {
	switch (game_type) {
#define CASE(NAME) \
		case NAME: {\
			int const *first = ShanXiGameOptions_##NAME;\
			int const *last = first + sizeof(ShanXiGameOptions_##NAME)/sizeof(ShanXiGameOptions_##NAME)[0];\
			return std::find(first, last, v) != last; /* found */\
		}
		// ������Щ�淨��Сѡ�������У��
		CASE(JinZhong)
#undef CASE
	default: return true; // û��������У����Ƶ��淨����������
	}
}




