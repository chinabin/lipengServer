

#include "malgo.h"



std::string malgo_format_fans(FANXING fans) {
	std::ostringstream s;
	s << "(FANXING)(0";
#define FANXING_DEF(name) if (fans & name) s << " | " #name; fans &= ~name
#define FANXING_DEF_BY(name, value)


	
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
FANXING_DEF(FAN_DuanYao); // ����
FANXING_DEF(FAN_KanHu); // ����
FANXING_DEF(FAN_BianZhang); // ����
FANXING_DEF(FAN_KanZhang); // ����
FANXING_DEF(FAN_DiaoZhang); // ���ţ�����Ϊ�е���

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
FANXING_DEF(FAN_HongTong_LouShangLou); // ¥��¥ - ����һ���з��ף�������һ���е�һ��
FANXING_DEF(FAN_HongTong_YingKou);     // Ӳ��  -  ����һ���з����е�1��
FANXING_DEF(FAN_HongTong_DiaoWang);



#undef FANXING_DEF
#undef FANXING_DEF_BY
	if (fans) s << std::hex << fans;
	s << ")";
	return s.str();
}
