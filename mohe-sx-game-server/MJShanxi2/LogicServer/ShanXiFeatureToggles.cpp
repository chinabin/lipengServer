
#include "malgo.h"

#include "ClientConstants.h"

#include "ShanXiFeatureToggles.h"
#define SUIJUN_LOG LLOG_ERROR
#include "..\mhmessage\mhmsghead.h"


void ShanXiFeatureToggles::setup_feature_toggles(int game_type) {
	// m_registered_game_type ָ��Ϊ GameHandler ע����淨��ţ������ǿͻ��˴������䴫����淨���
	m_registered_game_type = game_type;
	// FEAT_GAME_TYPE ���� ShanXiGameCore �ڲ�ʹ�õ��淨��ţ������ż�����ע���ŵ��Ӽ�

	// ���˿ٵ��������ǰ󶨶��˺Ϳٵ���������Ϣ����˼
	switch (game_type) {
	
	case ErRenKouDian:
	case SanRenKouDian:game_type = KouDian; break;
	case ErRenTuiDaoHu:
	case SanRenTuiDaoHu:
		game_type = TuiDaoHu;
		break;
	case ErRenJinZhong:
	case SanRenJinZhong:
		game_type = JinZhong;
		break;	
	case ErRenYingSanZui:
	case SanRenYingSanZui:
		game_type = YingSanZui;
		break;
	case ErRenHongTongWangPai:
	case SanRenHongTongWangPai:
		game_type = HongTongWangPai;
		break;
	case ErRenLinFenYiMenPai:
	case SanRenLinFenYiMenPai:
		game_type = LinFenYiMenPai;
		break;
	default:break;
	}

	FEAT_GAME_TYPE = game_type;

	switch (FEAT_GAME_TYPE) {

	case TuiDaoHu:
		break;

	case KouDian:
		break;

	case LiSi:
		break;

	case JinZhong:
		break;

	case TieJin:
		break;

	case GuaiSanJiao:
		break;

	}

	switch (FEAT_GAME_TYPE) {

	case TuiDaoHu:
		FEAT_DaiFeng = 0; // �Ƶ���Ĭ�ϲ����磬��Ҫ�����Сѡ������
		FEAT_DianGangBaoGang = 1; // ��ܰ��ܣ����ܡ����ܲ��ǵ�ܣ������Ĳ��������ѡ��
		FEAT_TingPaiBuBaoDianGang = 1; // ���Ʋ������---------���ϰ汾�齫�ĳ�һ��
		FEAT_YiPaoDuoXiang = 1; // һ�ڶ���
		FEAT_QiangGangHu = 1; // ���ܺ�
		SUIJUN_LOG("*******�����Ƶ����淨FeatureToggles game_type = %d", game_type);
		break;

	case KouDian:
		FEAT_BaoTing = 1; // ǿ����Ҫ����
		FEAT_DianGangBaoGang = 1; // ��ܰ��ܣ����ܡ����ܲ��ǵ�ܣ������Ĳ��������ѡ��
		FEAT_TingPaiBuBaoDianGang = 1; // ���Ʋ������
		FEAT_DealerRiskScore = 10; // ׯ���շ�
		FEAT_DealerRiskScoreDisabled = 1; // �ٵ���ͨ��Сѡ��������ׯ��ֵ�
		//FEAT_YiPaoDuoXiang = 1; // һ�ڶ���
		FEAT_QiangGangHu = 1; // ���ܺ�
		//FEAT_WeiShangTingGangBuSuanFen = 1; // δ�����ܲ����
		MHLOG("*******����۵��淨FeatureToggles game_type = %d", game_type);
		break;

	case LiSi:
		FEAT_BaoTing = 1; // ǿ����Ҫ����
		FEAT_SUO_COUNT = 4; // ����Ҫ����4����
		FEAT_ZhiYouHuPaiWanJiaGangSuanFen = 1; // ֻ�к��Ҳ��ܻ�øܷ֣������������ѡ����
		FEAT_FangPaoExtraLoss = 1; // �����߶��һ��
		FEAT_DealerRiskScore = 1; // ׯ���շ�
		FEAT_TingPaiHouBuNengGuoHu = 1; // ���ƺ��ܹ���
		break;

	case JinZhong:
		//  A ���ⷬ��Ĭ��֧�֣� ����ʾ
		//	B.��ׯ�ĸ� �� ������Ҳ���ʾ��Ĭ��֧��
		//	C.���ƿɸܲ���ʾ�� Ĭ��֧��
		//	D.���ܿɼ� ��ʾ��Ĭ��ѡ��֧��
		//	E.����ֻ������ ��ʾ��Ĭ�ϲ�ѡ��
		// 2018.1.13 ���ƺ󲻿ɸ���
		// 2018.1.13 ������ҵ��ڲ������1��
		// 2018.1.13 δ������ҵ��ڰ���ܷ�
		// 2018.1.13 ׯ+1�֣���������
		// 2018.1.13 ���������������Ʊ�����ڵ���5�ţ�1��ֻ���㵥��
		// 2018.1.15 ȥ�������߶ԣ���Ϊ7С��
		FEAT_BaoTing = 1; // ǿ����Ҫ����
		FEAT_TingPaiKeGang = 0; // �����ܸ���
		FEAT_ZhiYouHuPaiWanJiaGangSuanFen = 1; // ������ң�Ĭ��ѡ��
		FEAT_OnlyQueMenCanBaoTing = 1;
		FEAT_FangPaoExtraLoss = 1; // ��ǰ�ڳ�1��
		FEAT_FangPaoExtraLoss_TingHou = 0; // ������ڲ�����
		FEAT_DealerRiskScore = 1; // ׯ���շ�
		FEAT_DealerRiskScoreDisabled = 0; // ����ׯ���շ�
		FEAT_DisableZiMoDoubleScore = 0;  // ׯ��ַ���
		FEAT_WeiShangTingDianPaoBaoGang = 1; // δ�������ڰ���
		FEAT_QiangGangHu = 1;
		break;

	case TieJin:
		// ��ׯ�ĸ����Ĭ�ϵģ���ܰ����迪�������ܲ�������Ĭ�ϵ�
		// δ�������ڰ��⣬�������ڡ��������ҿ۷֣�����Ĭ�ϵ�
		// ��ׯ�����֣�����ׯ�������Ķ�����
		// 1��3��2��9��3����27�֣�������ַⶥ��������Ȼ�����Ͻ�
		// ���ƺ�����������ұ��������ͻ��˲�����������ť
		// ���˳��Ľ��Ʋ������͸�
		// �Լ�����Ľ���Ҳ���ܰ���
		FEAT_BaoTing = 1; // ǿ����Ҫ����
		FEAT_TingPaiKeGang = 0; // �������ƺ��ܸ���
		FEAT_DianGangBaoGang = 1; // ��ܰ��ܣ����ܡ����ܲ��ǵ�ܣ������Ĳ��������ѡ��
		FEAT_HunPaiBuKeGang = 1; // ���Ʋ��ɸ�
		FEAT_TingPaiHouBuNengGuoHu = 1; // ���ƺ��ܹ���
		break;

	case GuaiSanJiao:
		FEAT_DaiFeng = 0; // ������Ĭ�ϲ����磬��Ҫ�����Сѡ������
		// FEAT_DianGangBaoGang = 1; // ��ܰ��ܣ����ܡ����ܲ��ǵ�ܣ������Ĳ��������ѡ��
		FEAT_DianPaoDanRenKouFen = 1; // ���ڵ��˿۷�
		FEAT_DianGangDanRenKouFen = 1; // ��ܵ��˿۷�
		FEAT_QiangGangHu = 1;
		break;

	case PingLu:
		FEAT_DianGangBaoGang = 1; // ��ܰ��ܣ����ܡ����ܲ��ǵ�ܣ������Ĳ��������ѡ��
		FEAT_GuoHuChuPaiQianKeShouPao = 1; // ��������ǰ������
		break;

		//// �ٷ�һ����  by wyz
	case LinFenYiMenPai:
		MHLOG("*******�����ٷ�һ����FeatureToggles game_type = %d", game_type);
	case YingSanZui:
		FEAT_BaoTing = 1; // ǿ����Ҫ����
		FEAT_GuoHuChuPaiQianKeShouPao = 1; // ��������ǰ������
		FEAT_ZhiKeZiMoHuPai = 1; // ֻ���������ƣ��������ں���
		FEAT_DisableZiMoDoubleScore = 1; // ׯ���ʱ��ֹ����˫��
		FEAT_ZhiYouHuPaiWanJiaGangSuanFen = 1; // ֻ�к��Ҳ��ܻ�øܷ�
		FEAT_DealerRiskScore = 1; // ׯ���շ�
		FEAT_DealerRiskScoreDisabled = 0; // Ӳ������Ĭ��ׯһ�֣�����ͨ��Сѡ��ѡ��2��		
		MHLOG("*******����Ӳ����FeatureToggles game_type = %d", game_type);
		break;

	case BianKaDiao:
		FEAT_DaiFeng = 0; // �Ƶ���Ĭ�ϲ����磬��Ҫ�����Сѡ������
		FEAT_DianGangBaoGang = 1; // ��ܰ��ܣ����ܡ����ܲ��ǵ�ܣ������Ĳ��������ѡ��
		break;
	//�鶴���ƣ�
	case HongTongWangPai:	  // Suijun�鶴�����淨
		FEAT_BaoTing = 1; // ǿ����Ҫ����
		FEAT_ZhiKeZiMoHuPai = 1; // ֻ���������ƣ��������ں���
		FEAT_TingPaiKeGang = 1;  // ���ƿɸ�
		FEAT_OnlyQueMenCanBaoTing = 1; // ȱ�ű���
		FEAT_DealerRiskScore = 1; // ׯ���շ�
		FEAT_DealerRiskScoreDisabled = 0; // Ĭ�Ͽ���
		FEAT_DisableZiMoDoubleScore = 1; // ׯ���ʱ��ֹ����˫��	
		MHLOG("*******����鶴����FeatureToggles game_type = %d", game_type);
		break;

	//���ݿ۵㣺ǿ�Ʊ����������ܺ������ƿɸܣ������ܸı����ڣ���**Ren 2017-11-28��
	case XinZhouKouDian:
		FEAT_DaiFeng = 1;        // �������
		FEAT_BaoTing = 1;        // ǿ����Ҫ����
		FEAT_QiangGangHu = 1;    // ���ܺ�
		FEAT_TingPaiKeGang = 1;  // ���ƿɸ�
		FEAT_TingPaiBuBaoDianGang = 1; //���Ʋ�����ܣ����ܡ����ܲ����ܣ�
		m_remain_card_count = 12;// ������ʣ��12�������ׯ
		MHLOG("*******�������ݿ۵�FeatureToggles game_type = %d", game_type);
		break;

	//�����ӣ�
	case NianZhongZi:
		FEAT_DaiFeng = 1;        // �������
		FEAT_BaoTing = 1;        // ǿ����Ҫ����
		FEAT_QiangGangHu = 1;    // ���ܺ�
		FEAT_TingPaiKeGang = 1;  // ���ƿɸ�
		break;
	case LingChuanMaJiang:
		FEAT_BaoTing = 0; //ǿ�Ʊ���
		FEAT_DaiFeng = 1; //����
		FEAT_QiangGangHu = 1; //���ܺ�
		FEAT_FangPaoExtraLoss = FEAT_FangPaoExtraLoss_TingHou = 2; //���ڶ���2��
		FEAT_TingPaiKeGang = 1;  // ���ƿɸ�
		FEAT_YiPaoDuoXiang = 0;  // ����һ�ڶ���
		m_remain_card_count = 12;// ������ʣ��12�������ׯ
		MHLOG("*******�����괨FeatureToggles game_type = %d", game_type);
		break;
	case JinChengMaJiang:
		FEAT_BaoTing = 1; //ǿ�Ʊ���
		FEAT_DaiFeng = 0; //����
		FEAT_QiangGangHu = 1; //���ܺ�
		FEAT_FangPaoExtraLoss = FEAT_FangPaoExtraLoss_TingHou = 2; //���ڶ���2��
		FEAT_TingPaiKeGang = 1;  // ���ƿɸ�
		FEAT_YiPaoDuoXiang = 0;  // ����һ�ڶ���
		FEAT_WeiShangTingDianPaoBaoGang = 1; //δ��������
		MHLOG("*******�������*����FeatureToggles game_type = %d", game_type);
		break;
	}
}

static FANXING calc_allowed_fans_by_game_type(int FEAT_GAME_TYPE) {
	// �����������淨�̶�ȷ���Ĳ���
	switch (FEAT_GAME_TYPE) {
	case TuiDaoHu: return FAN_BASIC_ALLOWED | FAN_ShiSanYao | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_YiTiaoLong | FAN_QingYiSe;
	// �ٵ��Сѡ���С�ԡ�ָ���ǣ��ܺ���С�ԣ�������С�Լӷ������Һ�����С�Է����ı�
	case KouDian: return FAN_BASIC_ALLOWED | FAN_MUST_KouDian | FAN_ShiSanYao | FAN_YiTiaoLong | FAN_QingYiSe
		//| FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui
		;
	case LiSi: return FAN_BASIC_ALLOWED | FAN_QueMen | FAN_BianKanDiao | FAN_YiTiaoLong | FAN_QingYiSe;
	case JinZhong: return FAN_BASIC_ALLOWED | FAN_YiTiaoLong | FAN_PengPengHu | FAN_BianKanDiao | FAN_FengYiSe | FAN_CouYiSe | FAN_QingYiSe | FAN_QiXiaoDui| FAN_HaoHuaQiXiaoDui
			| FAN_MenQing | FAN_DuanYao
			| FAN_MUST_QueMen // ���У�����ȱ�Ų��ܺ�, MH�޸ļ��� FAN_FengYiSe |FAN_CouYiSe | FAN_QingHaoHuaQiXiaoDui
			;
	case TieJin: return FAN_BASIC_ALLOWED 
		//| FAN_ShiSanYao | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_YiTiaoLong | FAN_QingYiSe
		;
	case GuaiSanJiao: return FAN_BASIC_ALLOWED | FAN_YiTiaoLong | FAN_QingYiSe | FAN_KanHu | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui |FAN_LianZhuang;
		// return FAN_BASIC_ALLOWED | FAN_MUST_YiMenGouBaZhang | FAN_YiTiaoLong | FAN_QingYiSe | FAN_KanHu ԭ���߼�
			//| FAN_ShiSanYao | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui
			 // ������ȥ����һ�Ű��ţ� Ĭ�ϼ�����С�ԣ�������С��
	case PingLu: return FAN_BASIC_ALLOWED | FAN_MUST_QueMen | FAN_ShiSanYao | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_YiTiaoLong | FAN_QingYiSe;
		//// �ٷ�һ����   by wyz
	case LinFenYiMenPai:

	case YingSanZui: return FAN_BASIC_ALLOWED
		| FAN_QueYiMen | FAN_QueLiangMen | FAN_GuJiang | FAN_YiZhangYing | FAN_DiaoZhang | FAN_DanDiao
		| FAN_JUDGE_SanFeng | FAN_JUDGE_SanYuan
		| FAN_MenQing
		| FAN_QingYiSe | FAN_ZiYiSe
		| FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_QingQiDui | FAN_QingHaoHuaQiXiaoDui
		| FAN_YiTiaoLong | FAN_QingLong
		;
	// SUIJUN��Ӻ鶴���Ʒ���
	case HongTongWangPai: return FAN_BASIC_ALLOWED | FAN_QiXiaoDui | FAN_YiTiaoLong | FAN_QingYiSe | FAN_CouYiSe | FAN_ZiYiSe | FAN_MUST_QueMen
		 | FAN_JUDGE_SanFeng | FAN_JUDGE_SanYuan | FAN_HongTong_LouShangLou | FAN_HongTong_YingKou | FAN_HongTong_DiaoWang;
	//�߿�����
	case BianKaDiao: return FAN_BASIC_ALLOWED | FAN_ShiSanYao | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_YiTiaoLong | FAN_QingYiSe;
		
	//���ݿ۵�:�۵���Ƶ�����һ��������һɫ����һɫ����С�ԡ����������߶ԡ����߶�  (**Ren 2017-11-22)
	case XinZhouKouDian:
		return FAN_BASIC_ALLOWED
			| FAN_MUST_KouDian
			| FAN_YiTiaoLong
			| FAN_HaoHuaQiXiaoDui
			| FAN_QiXiaoDui
			| FAN_QingYiSe
			| FAN_ZiYiSe
			| FAN_QingLong
			| FAN_QingQiDui
			| FAN_QingHaoHuaQiXiaoDui;

	//�����ӣ����塢ȱ�š����ۡ����š����š���������һɫ(��һɫ)����һɫ����һɫ��������ʮ���� ��**Ren 2017-11-28��
	case NianZhongZi:
		return FAN_BASIC_ALLOWED
			| FAN_MenQing
			| FAN_QueMen
			| FAN_DuanYao
			| FAN_KanZhang
			| FAN_BianZhang
			| FAN_DiaoZhang
			| FAN_CouYiSe
			| FAN_ZiYiSe
			| FAN_QingYiSe
			| FAN_YiTiaoLong
			| FAN_QingLong
			| FAN_ShiSanYao;
	case LingChuanMaJiang:
		return FAN_BASIC_ALLOWED | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_ShiSanYao | FAN_LingChuan_Must_KaoBaZhang;
		break;
	case JinChengMaJiang:
		return FAN_BASIC_ALLOWED | FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_ShiSanYao;
		break;
	default: return FAN_BASIC_ALLOWED;
	}
}
static FANXING revise_allowed_fans_by_game_options(FANXING fans, GameOptionSet const &m_playtype) {
	// ����������Сѡ��ȷ���Ĳ���
	// Сѡ��Ҫ��Сѡ����������ƿͻ������룬����������Լٶ���������Сѡ��ǺϷ���

	// �ٵ��׽�����淨û����С�Ժ�ʮ���ۣ���Сѡ�������С�Ժ�ʮ����
	if (m_playtype.b_HaoZi || m_playtype[PT_ShuangHaoZi]) fans = FAN_MUST_KouDian | FAN_BASIC_ALLOWED | FAN_YiTiaoLong | FAN_QingYiSe;

    // �°�ɽ���齫�п۵�������淨������С�ԣ�ʮ���۲��ܺ�
	if (m_playtype.b_HaoZi || m_playtype[PT_ShuangHaoZi] || m_playtype[MH_PT_FengHaoZi])
	{
		fans = FAN_MUST_KouDian | FAN_BASIC_ALLOWED | FAN_YiTiaoLong | FAN_QingYiSe | FAN_QiXiaoDui;
		fans = FAN_MUST_KouDian | FAN_BASIC_ALLOWED | FAN_QiXiaoDui;   // �����ӵ��淨ȥ��һ��������һɫ�ķ�
	}

	// ��Щ���Ǳ���������
	if (m_playtype[PT_QiXiaoDui]) fans |= FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui;
	if (m_playtype[PT_FengYiSe]) fans |= FAN_FengYiSe;
	if (m_playtype[PT_QingYiSe]) fans |= FAN_QingYiSe;
	if (m_playtype[PT_CouYiSe]) fans |= FAN_CouYiSe;
	if (m_playtype[PT_ShiSanYao]) fans |= FAN_ShiSanYao;
	if (m_playtype[PT_YiTiaoLong]) fans |= FAN_YiTiaoLong;
	if (m_playtype[PT_QueYiMen]) fans |= FAN_MUST_QueMen;  //���ݿ۵㣺ȱһ��ѡ�**Ren 2017-11-28��

	
	if (m_playtype[PT_QueLiangMen])
	{
		fans |= FAN_HongTong_MustQueLiangMen; // ȱ2�T��Ȼ��һɫ���Ͳ����@������
		//fans &= ~FAN_CouYiSe;               // ���Ҫ���������������ؿͻ��˵�ʱ����˵�
	}

	// ��Щ�������ڱ������ӵĺ��棬��ѡ����Ч��
	// �Ƶ�����С���淨ֻ��ƽ��
	if (m_playtype.XiaoHu) fans = FAN_BASIC_ALLOWED;
	// �Ƶ����ı߿����淨
	if (m_playtype[PT_BianKanDiao]) {
		fans |= FAN_MUST_BianKanDiao | FAN_BianKanDiao;
		fans &= ~(FAN_QiXiaoDui | FAN_HaoHuaQiXiaoDui | FAN_ShiSanYao);
	}

	// ��������Ӵ���Сѡ�����ʮ����
	if (m_playtype[PT_DaiFeng])
	{
		fans |= FAN_ShiSanYao;
	}
	if (m_playtype[MH_PT_YingBaZhang])
	{
		fans |= FAN_MUST_YiMenGouBaZhang;
	}
	// �����޶����ֵ
	return fans;
}
static FANXING exclude_allowed_fans_by_game_type(FANXING fans, int FEAT_GAME_TYPE) {
	// ��Щ�淨����С��ѡ�����������С�ӵ�
	switch (FEAT_GAME_TYPE) {
	case TuiDaoHu: return fans;
	//case TieJin: return fans & ~(FAN_HaoHuaQiXiaoDui);
	default: return fans;
	}
}
void ShanXiFeatureToggles::init_allowed_fans() {
	// �����������淨�̶�ȷ���Ĳ���
	m_allowed_fans = calc_allowed_fans_by_game_type(FEAT_GAME_TYPE);
	LLOG_DEBUG("****1****GAME_TYPE = %d �ܺ��ķ��� %s", FEAT_GAME_TYPE, malgo_format_fans(m_allowed_fans).c_str());
	// ����������Сѡ��ȷ���Ĳ���
	m_allowed_fans = revise_allowed_fans_by_game_options(m_allowed_fans, m_playtype);
	// ��Щ�淨����С��ѡ�����������С�ӵ�
	m_allowed_fans = exclude_allowed_fans_by_game_type(m_allowed_fans, FEAT_GAME_TYPE);
	LLOG_DEBUG("****2****GAME_TYPE = %d �ܺ��ķ��� %s", FEAT_GAME_TYPE, malgo_format_fans(m_allowed_fans).c_str());
}


void ShanXiFeatureToggles::vectorize_fans(FANXING fans, std::vector<Lint> &vec) {
	// ������С�Բ�����С��ͬʱ��ʾ
	if (fans & FAN_HaoHuaQiXiaoDui) fans &= ~FAN_QiXiaoDui;

	// ��Щ�淨ǿ�Ʋ���ʾƽ��
	switch (FEAT_GAME_TYPE) {
	//// �ٷ�һ����   by wyz
	case LinFenYiMenPai:
	case YingSanZui:
		fans &= ~FAN_PingHu;
		break;
	case HongTongWangPai:
		fans &= ~FAN_PingHu;
		fans &= ~FAN_ZiMo;
		if (m_playtype[PT_QueLiangMen])
		{
			fans &= ~FAN_CouYiSe;
		}
		break;
	//���ݿ۵㣺������С����ʾ��С�ԣ��������С����ʾ���߶ԣ�**Ren 2017-12-22��
	case XinZhouKouDian:
		if (fans & FAN_HaoHuaQiXiaoDui)
		{
			fans = fans & ~FAN_HaoHuaQiXiaoDui | FAN_QiXiaoDui;
		}
		if (fans & FAN_QingHaoHuaQiXiaoDui)
		{
			fans = fans & ~FAN_QingHaoHuaQiXiaoDui | FAN_QingQiDui;
		}
	//�����ӣ�����ʾƽ����**Ren 2017-11-29��
	case NianZhongZi:
		fans &= ~FAN_PingHu;
	}

#define A(fan, hu) if (fans & (fan)) vec.push_back((hu))
	// ����
	A(FAN_PingHu, HU_PingHu);
	A(FAN_QiXiaoDui, HU_Qixiaodui);
	A(FAN_HaoHuaQiXiaoDui, HU_DaQixiaodui);
	A(FAN_ShiSanYao, HU_ShiSanYao);
	// �������
	A(FAN_QingLong, HU_QingLong);
	A(FAN_QingQiDui, HU_QingQiDui);
	A(FAN_QingHaoHuaQiXiaoDui, HU_QingHaoHuaQiXiaoDui);
	// ͨ�õ���
	A(FAN_QingYiSe, HU_QingYiSe);
	A(FAN_CouYiSe, HU_CouYiSe);
	A(FAN_FengYiSe, HU_FengYiSe);
	A(FAN_ZiYiSe, HU_ZiYiSe);
	A(FAN_KanHu, HU_KanHu);
	A(FAN_MenQing, HU_MenQing);
	A(FAN_DuanYao, HU_DuanYao);
	A(FAN_QueMen, HU_QUEMEN);
	A(FAN_QueYiMen, HU_QueYiMen);
	A(FAN_QueLiangMen, HU_QueLiangMen);
	A(FAN_GuJiang, HU_GuJiang);
	A(FAN_YiZhangYing, HU_YiZhangYing);
	// ƽ������
	A(FAN_BianZhang, HU_BIANZHANG);
	A(FAN_KanZhang, HU_KANZHANG);
	A(FAN_DiaoZhang, HU_DIAOZHANG);
	A(FAN_DanDiao, HU_DanDiao);
	A(FAN_PengPengHu, HU_PengPengHu);
	A(FAN_YiTiaoLong, HU_YiTiaoLong);
	// Ӳ��������硢��Ԫ
	A(FAN_SanFeng_X1, HU_SanFeng_X1);
	A(FAN_SanFeng_X2, HU_SanFeng_X2);
	A(FAN_SanFeng_X3, HU_SanFeng_X3);
	A(FAN_SanFeng_X4, HU_SanFeng_X4);
	A(FAN_SanYuan_X1, HU_SanYuan_X1);
	A(FAN_SanYuan_X2, HU_SanYuan_X2);
	A(FAN_SanYuan_X3, HU_SanYuan_X3);
	A(FAN_SanYuan_X4, HU_SanYuan_X4);
	A(FAN_HongTong_LouShangLou, HU_HongTong_LouShangLou);
	A(FAN_HongTong_YingKou, HU_HongTong_YingKou);
	A(FAN_HongTong_DiaoWang, HU_HongTong_DiaoWang);
	A(FAN_LianZhuang, HU_LianZhuang);
#undef A


}


tile_mask_t ShanXiFeatureToggles::calc_hu_candidate_set(tile_mask_t mask) const {
	// ����4��3����Χû�ƣ��Ǿ͵�һ�����ӣ��������һ��˳��
	// ���ԾͰ�ÿ�����Լ���Χ��4���Ƽӽ���
	tile_mask_t dst = mask;
	dst |= (mask & TILE_MASK_SHUPAI39) >> 2; // ��2
	dst |= (mask & TILE_MASK_SHUPAI29) >> 1; // ��1
	dst |= (mask & TILE_MASK_SHUPAI18) << 1; // ��1
	dst |= (mask & TILE_MASK_SHUPAI17) << 2; // ��2
	if (m_allowed_fans & FAN_ShiSanYao) dst |= TILE_MASK_SHISANYAO;
	if (m_allowed_fans & FAN_JUDGE_SanFeng) {
		if (mask & TILE_MASK_Feng) dst |= TILE_MASK_Feng;
	}
	if (m_allowed_fans & FAN_JUDGE_SanYuan) {
		if (mask & TILE_MASK_Jian) dst |= TILE_MASK_Jian;
	}
	return dst;
}

FANXING ShanXiFeatureToggles::call_judge(PlayerState &ps, size_t num_huns, TileVec const &hands, tile_t last_tile, FANXING conf
	, tile_mask_t pg_mask, pg_tile_count_by_color_t const &sum_pg_tile_count_by_color) const {

	//MHLOG("*****************************���㷬��************************");
	// ����Ҫ��������һ�����⣺һ����淨�϶���С�Էָߣ����е��淨��С�Էָ߻���ƽ���ָ߲������ȷ������
	FANXING fans = FAN_NONE;
	switch (FEAT_GAME_TYPE) {
	//// �ٷ�һ����   by wyz
	case LinFenYiMenPai:
	case YingSanZui: {
		HuJudgement judgement1(ps, num_huns, hands, last_tile, conf & ~FAN_JUDGE_DaHu, pg_mask, sum_pg_tile_count_by_color);
		HuJudgement judgement2(ps, num_huns, hands, last_tile, conf & ~FAN_JUDGE_XiaoHu, pg_mask, sum_pg_tile_count_by_color);
		FANXING diaozhang1 = judgement1.fans & FAN_DiaoZhang;
		FANXING diaozhang2 = judgement2.fans & FAN_DiaoZhang;
		int score1 = calc_common_loss(judgement1.fans, last_tile);
		int score2 = calc_common_loss(judgement2.fans, last_tile);
		if (score1 > score2) fans = judgement1.fans;
		else fans = judgement2.fans;
		break;
	}
	default: {
		HuJudgement judgement(ps, num_huns, hands, last_tile, conf, pg_mask, sum_pg_tile_count_by_color);
		fans = judgement.fans;
	}
	}
	//MHLOG("***********fans = judgement.fans =%s", malgo_format_fans(fans).c_str());
	FANXING diaozhang3 = fans & FAN_DiaoZhang;
	// �鶴����¥��¥�����ڣ�����¥��¥�ķ��� -- �����������Ԫ����תΪ¥��¥����
	if (HongTongWangPai == FEAT_GAME_TYPE)
	{	
			if ((fans &(FAN_SanYuan_X2|FAN_SanYuan_X3|FAN_SanYuan_X4))			 
				&& (TILE_MASK_Jian & TILE_TO_MASK(last_tile)))
			{
				fans |= FAN_HongTong_LouShangLou;
			}

			if ((fans &(FAN_SanYuan_X1))
				&& (TILE_MASK_Jian & TILE_TO_MASK(last_tile)))
			{
				fans |= FAN_HongTong_YingKou;
			}	

			if (fans & FAN_QiXiaoDui)   
			{
				//for(int i = 0; i < ps.ha )
			}
		 
	}

	fans = reduce_fans(fans, conf);
	FANXING diaozhang4 = fans & FAN_DiaoZhang;	
	//MHLOG("***********FanValue=%lld", fans);
	//MHLOG("***********FanOuput=%s", malgo_format_fans(fans).c_str());
	return fans;
}

// �����۷ֵ��ӵķ���
FANXING ShanXiFeatureToggles::reduce_fans(FANXING fans, FANXING const conf) const {
	// ��������������һ�����⣺�󲿷��淨�Ĵ�������������塢������ЩС�ֵģ�����Щ�淨��Ӳ����ͱȽ�����
	switch (FEAT_GAME_TYPE) {
	//// �ٷ�һ����  by wyz
	case LinFenYiMenPai:
	case YingSanZui:
		// Ӳ���죺ֻ����С��ʱ�����忳������������һɫ��һ������������
		if (fans & FAN_JUDGE_QiDui) fans &= ~FAN_MenQing;
		// Ӳ���죺�������絥�����½�ʲô�ģ�Ҫ���죬��������
		break;
	case HongTongWangPai: // ���Ƽ��ٵ��� 
		//if (fans & FAN_QingYiSe)  fans &= ~(FAN_YiTiaoLong | FAN_QiXiaoDui | FAN_CouYiSe);
		//if (fans & FAN_ZiYiSe) fans &= ~(FAN_JUDGE_SanFeng | FAN_JUDGE_SanYuan | FAN_QiXiaoDui | FAN_HongTong_LouShangLou);
		if (fans & FAN_HongTong_LouShangLou) fans &= ~(FAN_QiXiaoDui | FAN_HongTong_DiaoWang);
		if (fans & FAN_HongTong_YingKou) fans &= ~(FAN_HongTong_DiaoWang | FAN_QiXiaoDui);
		if (fans & FAN_BIG_EFFECTS) fans &= ~FAN_SMALL_EFFECTS;
		break;
	case GuaiSanJiao:
		if (fans & FAN_KanHu) fans &= ~FAN_PingHu; // �����ǿ�������ƽ����
		if (fans & FAN_BIG_EFFECTS) fans &= ~FAN_SMALL_EFFECTS;
		break;
	//�����ӣ����塢�߿�����ȱ�š�һ����������Ҫ���ӵ�(**Ren 2017-11-29)
	case NianZhongZi:
		if (fans & FAN_HUCHI_QueMen) fans &= ~FAN_QueMen;
		if (fans & FAN_ShiSanYao) fans &= ~FAN_SMALL_EFFECTS;
		if (fans & FAN_BIG_EFFECTS) fans &= ~FAN_PingHu;
		break;
	//���У����塢�߿�����ȱ�š�һ����������Ҫ���ӵ�(**Ren 2017-11-29)
	case JinZhong:		
		if (fans & FAN_BIG_EFFECTS) fans &= ~FAN_PingHu;
		if (fans & FAN_PengPengHu) fans |= FAN_PingHu;
		if (fans & FAN_HaoHuaQiXiaoDui)
		{
			fans &= (~FAN_HaoHuaQiXiaoDui); //����û�к���7��
			fans |= FAN_QiXiaoDui;
		}
		if (fans & FAN_QiXiaoDui)
		{
			fans &= (~FAN_MenQing);   // 
			fans &= (~FAN_DiaoZhang); // ��С��ȥ�����ź�����
		}
		break;
	default:
		if (fans & FAN_BIG_EFFECTS) fans &= ~FAN_SMALL_EFFECTS;
		break;
	}
	// ���Ҫ������ķ���ȡ����
	return fans & conf;
}

FANXING ShanXiFeatureToggles::reduce_fans_in_TingKou_stage(FANXING fans, FANXING const conf) const {
	switch (FEAT_GAME_TYPE) {
	//// �ٷ�һ����  by wyz
	case LinFenYiMenPai:
	case YingSanZui:
		// Ӳ���죺������֮ǰ�淨�ĵ��Ų�ͬ������Ҫ��ֻ��һ���ƣ�������ʱ�����һ��Ӯ������
		if ((fans & FAN_DiaoZhang) && (fans & FAN_YiZhangYing)) {
			fans |= FAN_DanDiao;
			fans &= ~FAN_YiZhangYing;
		}
		fans &= ~FAN_DiaoZhang;
		// Ӳ���죺��Ԫ��������Ҫ��������һ��Ӯ�ķ���
		if (!(fans & FAN_SanYuanZaiShou) && (fans & FAN_JUDGE_SanYuan)) {
			fans &= ~FAN_YiZhangYing;
		}
		break;
	default:
		break;
	}
	// ���Ҫ������ķ���ȡ����
	return fans & conf;
}

int ShanXiFeatureToggles::calc_zui_score(FANXING fans) const {
	int score = 0;
	if (fans & FAN_QueYiMen) score += 1;
	if ((fans & FAN_QueLiangMen) || (fans & FAN_ZiYiSe)||(fans & FAN_FengYiSe)) score += 3;
	//�������ˣ������⺯���������������
	//if ( fans & FAN_ZiYiSe) score += 3;               // by wyz ���ȫ�������⣬��ȱ������
	//if (fans & FAN_FengYiSe) score += 3;             // by wyz ���ȫ�������⣬��ȱ������
	if (fans & FAN_GuJiang) score += 1;
	if (fans & FAN_YiZhangYing) score += 1;
	if (fans & FAN_DanDiao) score += 1;
	if (fans & FAN_SanFeng_X1) score += 1;
	if (fans & FAN_SanFeng_X2) score += 3;
	if (fans & FAN_SanFeng_X3) score += 10;
	if (fans & FAN_SanFeng_X4) score += 10;
	if (fans & FAN_SanYuan_X1) {
		if (fans & FAN_SanYuanZaiShou) score += 1;
		else score += 2;
	}
	if (fans & FAN_SanYuan_X2) {
		if (fans & FAN_SanYuanZaiShou) score += 5;
		else score += 7;
	}
	if (fans & FAN_SanYuan_X3) score += 50;
	if (fans & FAN_SanYuan_X4) score += 50;
	return score;
}

//// Ӳ���� �ں��ķ�����Ϊ��С�ԣ�����ļ��������ĺ���  by wyz
int ShanXiFeatureToggles::calc_zui_score_spec_qixiaodui(TileVec const &hands, tile_mask_t pg_mask) const {
	struct TileZoneDivider stats(hands.begin(), hands.size());
	tile_mask_t const  all_mask(stats.mask | pg_mask);

	int zuiNum = 1;   // ���1��ʱһ��Ӯ�ģ���С�Կ���ʱһ��Ӯ
					  //ȱ��
					  //int n = calc_quemen_count(all_mask);
	int sum = 0;
	if (0 == (all_mask & TILE_MASK_WAN)) ++sum;
	if (0 == (all_mask & TILE_MASK_TONG)) ++sum;
	if (0 == (all_mask & TILE_MASK_TIAO)) ++sum;
	switch (sum)
	{
	case 1:
		zuiNum++;
		break;
	case 2:
	case 3:
		zuiNum += 3;
		break;
	}
	MHLOG("Logwyz ... sum=[%d] zuiNum=[%d]", sum, zuiNum);
	//����
	std::vector<int> temp;
	for (TILE_COLOR tc = TILE_COLOR_MIN; tc <= TILE_COLOR_ZI; tc = (TILE_COLOR)(tc + 1))
		temp.push_back(stats.count_by_color_by_number[TILE_COLOR_ZI - TILE_COLOR_MIN][tc - TILE_COLOR_MIN]);
	std::sort(temp.begin(), temp.begin() + (TILE_COLOR_ZI - TILE_COLOR_MIN));
	switch (temp[1])
	{
	case 1:
		zuiNum++;
		break;
	case 2:
		zuiNum += 3;
		break;
	case 3:
		zuiNum += 10;
		break;
	}
	MHLOG("Logwyz ...ss temp[0]=[%d] temp[1]=[%d] temp[2]=[%d] temp[3]=[%d]zuiNum=[%d]", temp[0], temp[1], temp[2], temp[3], zuiNum);
	//��Ԫ
	temp.clear();
	for (TILE_COLOR tc = TILE_COLOR_ZI; tc <= 6; tc = (TILE_COLOR)(tc + 1))
		temp.push_back(stats.count_by_color_by_number[TILE_COLOR_ZI- TILE_COLOR_MIN][tc - TILE_COLOR_ZI]);
	std::sort(temp.begin(), temp.begin() + 3);
	switch (temp[0])
	{
	case 1:
		zuiNum++;
		break;
	case 2:
		zuiNum += 5;
		break;
	case 3:
		zuiNum += 50;
		break;
	}
	MHLOG("Logwyz ...ff temp[0]=[%d] temp[1]=[%d] temp[2]=[%d] zuiNum=[%d]", temp[0], temp[1], temp[2], zuiNum);
	return zuiNum;
}

int ShanXiFeatureToggles::calc_common_loss(FANXING fans, tile_t last_tile, unsigned max_zone_card_count) const {
	if (fans == FAN_NONE) return 0;
	int loss = 1;
	switch (FEAT_GAME_TYPE) {

	case TuiDaoHu:
		// ���ﲻ���ǻ��⣬ֻ�������ۼӣ��û����ǰ�涼Ӧ�û���������
		// ���������С�Ӻ���С���ǻ���ģ���һɫ��ƽ���ǻ����
		loss = 0;
		if (fans & FAN_PingHu) loss += 1;
		if (fans & FAN_ShiSanYao) loss += 9;
		if (fans & FAN_HaoHuaQiXiaoDui) loss += 6;
		if (fans & FAN_QiXiaoDui) loss += 3;
		if (fans & FAN_YiTiaoLong) loss += 3;
		if (fans & FAN_QingYiSe) loss += 3;
		// ƽ������ʱ2�������ⷬ��������3��
		if (fans & FAN_ZiMo) loss *= (fans & FAN_PingHu) ? 2 : 3;
		break;

	case KouDian:
		loss = tile_to_points(last_tile);
		if (fans & FAN_ZiMo) loss *= 2;

		// ��������ʱ����һɫ�ӷ���һ�����ӷ���ѡ����ѡ��ֻӰ��Ʒ֣���Ӱ����ĩ�ɼ���ʾ
		if (m_playtype.b_HaoZi || m_playtype[PT_ShuangHaoZi]) {
			// �ٵ㣺�����ӡ�˫���ӣ�Ĭ����һɫ��һ�������ӷ�
			//if (fans & FAN_YiTiaoLong) loss *= 2;
			//if (fans & FAN_QingYiSe) loss *= 2;
			// �����ӵ��淨������������֧
			if (fans & (FAN_YiTiaoLong | FAN_QingYiSe))
			{
				LLOG_ERROR("********������� �۵㷬��fans & (FAN_YiTiaoLong | FAN_QingYiSe) = true**************");
			}
		} else {
			if (m_playtype.YiTiaoLongJiaFan && (fans & FAN_YiTiaoLong)) loss *= 2;
			if (m_playtype.b_QingYiSeJiaFan && (fans & FAN_QingYiSe)) loss *= 2;

			// �°�۵㲻�����Ӽӷ�
			if (fans & FAN_HaoHuaQiXiaoDui) loss *= 4;
			if (fans & FAN_QiXiaoDui) loss *= 2;
			if (fans & FAN_ShiSanYao) loss *= 2;
		}

		//if (fans & FAN_HaoHuaQiXiaoDui) loss *= 4;
		//if (fans & FAN_QiXiaoDui) loss *= 2;
		//if (fans & FAN_ShiSanYao) loss *= 2;
		break;

	case LiSi:
		if (fans & FAN_YiTiaoLong) loss += 10;
		if (fans & FAN_QingYiSe) loss += 10;
		if (fans & FAN_BianKanDiao) loss += 1;
		if (fans & FAN_QueMen) loss += 1;
		if (fans & FAN_ZiMo) loss *= 2;
		break;

	case JinZhong:
		loss = 2;
		if (fans & FAN_HaoHuaQiXiaoDui) loss += 18;
		if (fans & FAN_QiXiaoDui) loss += 9;
		if (fans & FAN_YiTiaoLong) loss += 10;
		if (fans & FAN_PengPengHu) loss += 5;
		if (fans & FAN_BianKanDiao) loss += 1;
		if (fans & FAN_FengYiSe) loss += 20;
		if (fans & FAN_QingYiSe) loss += 10;
		if (fans & FAN_CouYiSe) loss += 5;
		if (fans & FAN_MenQing) loss += 1;
		if (fans & FAN_DuanYao) loss += 1;
		//if (fans & FAN_QueMen) loss += 1;
		if (fans & FAN_ZiMo) loss *= 2;
		break;

	case TieJin:
		loss = 1;
		if (fans & FAN_ZiMo) loss *= 2;
		break;

	case GuaiSanJiao:
		// ���ﲻ���ǻ��⣬ֻ�������ۼӣ��û����ǰ�涼Ӧ�û���������
		// ���������С�Ӻ���С���ǻ���ģ���һɫ��ƽ���ǻ����
		// ������˭�л����
		loss = 0;
		if (m_playtype[MH_PT_GuaSanJiao_High_Score])
		{		
			// ���Ƹ߷�ֵ
			if (fans & FAN_PingHu) loss += 5;
			if (fans & FAN_KanHu) loss += 10;
			if (fans & FAN_ShiSanYao) loss += 15;
			if (fans & FAN_HaoHuaQiXiaoDui) loss += 30;
			if (fans & FAN_QiXiaoDui) loss += 15;
			if (fans & FAN_YiTiaoLong) loss += 15;
			if (fans & FAN_QingYiSe) loss += 15;			
		}
		else // ��ͬ���Ƶͷ�ֵ
		{
			if (fans & FAN_PingHu) loss += 1;
			if (fans & FAN_KanHu) loss += 2;
			if (fans & FAN_ShiSanYao) loss += 10;
			if (fans & FAN_HaoHuaQiXiaoDui) loss += 10;
			if (fans & FAN_QiXiaoDui) loss += 5;
			if (fans & FAN_YiTiaoLong) loss += 5;
			if (fans & FAN_QingYiSe) loss += 5;
		}
		// ��ׯ�ӷ�
		if (fans & FAN_Zhuang) loss += m_lianzhuang_jiafen;
		break;

	case BianKaDiao:
		// ���ﲻ���ǻ��⣬ֻ�������ۼӣ��û����ǰ�涼Ӧ�û���������
		// ���������С�Ӻ���С���ǻ���ģ���һɫ��ƽ���ǻ����
		loss = 0;
		if (fans & FAN_PingHu) loss += 1;
		if (fans & FAN_ShiSanYao) loss += 9;
		if (fans & FAN_HaoHuaQiXiaoDui) loss += 6;
		if (fans & FAN_QiXiaoDui) loss += 3;
		if (fans & FAN_YiTiaoLong) loss += 3;
		if (fans & FAN_QingYiSe) loss += 3;
		// ƽ������ʱ2�������ⷬ��������3��
		if (fans & FAN_ZiMo) loss *= (fans & FAN_PingHu) ? 2 : 3;
		break;

	//// �ٷ�һ����  by wyz
	case LinFenYiMenPai:
	case YingSanZui:
		loss = calc_zui_score(fans);
		MHLOG("Logwyz .. ����:[%d]", loss);
		if (fans & FAN_MenQing) loss += 1;
		if (fans & FAN_QiXiaoDui) loss += 10;
		if (fans & FAN_HaoHuaQiXiaoDui) loss += 25;
		if (fans & FAN_YiTiaoLong) loss += 10;
		if (fans & FAN_QingYiSe) loss += 10;
		if (fans & FAN_QingLong) loss += 50;
		if (fans & FAN_QingQiDui) loss += 50;
		if (fans & FAN_QingHaoHuaQiXiaoDui) loss += 50;
		if (fans & FAN_ZiYiSe) loss += 50;
		//�ж϶˹�
		if (m_playtype[MH_PT_Linfen_DuanGuo] && loss >= 50) loss = 50;
		break;
	case HongTongWangPai: //Suijun, ���ݷ��ͼ������		 
		loss = 1;		
		if (fans & FAN_SanFeng_X1) loss += 1;
		if (fans & FAN_SanYuan_X1) loss += 1;
		if (fans & FAN_SanFeng_X2) loss += 2;
		if (fans & FAN_SanYuan_X2) loss += 2;
		if (fans & FAN_SanFeng_X3) loss += 3;
		if (fans & FAN_SanYuan_X3) loss += 3;
		if (fans & FAN_SanFeng_X4) loss += 4;
		if (fans & FAN_SanYuan_X4) loss += 4;
		if (fans & FAN_CouYiSe) loss += 2;       //��һɫ��ɫ�ƣ����磬����
		if (fans & FAN_QiXiaoDui) loss += 10;    //��С�ԣ� ���Ƶ��������� 
		if (fans & FAN_YiTiaoLong) loss += 10;   //һ��������������   
		if (fans & FAN_HongTong_LouShangLou) loss += 10;	 //¥��¥
		if (fans & FAN_QingYiSe) loss += 20;     //��һɫ����һɫ�����֣��޷�
		if (fans & FAN_ZiYiSe) loss += 30;       //��һɫ����������Ͳ  
		if (fans & FAN_HongTong_YingKou) loss += 4; //Ӳ��
		if (fans & FAN_HongTong_DiaoWang) loss += 2;
		//MHLOG("*****************************HongTongWangPai���ݷ��ͼ����������  GameType=%d loss=%d", FEAT_GAME_TYPE, loss);
		//MHLOG("*****************************Fans= %s", malgo_format_fans(fans).c_str());
		break;
	//���ݿ۵㣺���ͼ��������**Ren 2017-11-28��
	case XinZhouKouDian:
		loss = tile_to_points(last_tile);
		if (fans & FAN_QingYiSe) loss += 10;
		if (fans & FAN_QiXiaoDui) loss += 10;
		if (fans & FAN_HaoHuaQiXiaoDui) loss += 10;     //��**Ren 2017-12-21��
		if (fans & FAN_YiTiaoLong) loss += 10;
		if (fans & FAN_ZiYiSe) loss += 10;
		if (fans & FAN_QingLong) loss += 20;
		if (fans & FAN_QingQiDui) loss += 20;
		if (fans & FAN_QingHaoHuaQiXiaoDui) loss += 20;  //��**Ren 2017-12-21��
		if (fans & FAN_ZiMo) loss *= 2;
		break;
	//�����ӣ����ͼ��������**Ren 2017-11-28��
	case NianZhongZi:
		loss = 0;
		if (fans & FAN_BianZhang) loss += 1;
		if (fans & FAN_KanZhang) loss += 1;
		if (fans & FAN_DiaoZhang) loss += 1;
		if (fans & FAN_DuanYao) loss += 1;
		if (fans & FAN_MenQing) loss += 1;
		if (fans & FAN_QueMen) loss += 1;
		if (fans & FAN_CouYiSe) loss += 5;
		if (fans & FAN_ZiYiSe) loss += 10;
		if (fans & FAN_YiTiaoLong) loss += 10;
		if (fans & FAN_QingLong) loss += 20;
		if (fans & FAN_QingYiSe) loss += 10;
		if (fans & FAN_ShiSanYao) loss += 50;
		if (fans & FAN_ZiMo) loss *= 2;
		break;
	case LingChuanMaJiang:
		loss = max_zone_card_count;		
		if (fans & FAN_ZiMo) loss *= 2;
		break;
	case JinChengMaJiang:
		if (fans & FAN_ZiMo) loss *= 2;      //���������ӱ�
		if (fans & (FAN_QiXiaoDui|FAN_HaoHuaQiXiaoDui|FAN_ShiSanYao)) loss *= 2; //�������ͷ����ӱ�
		break;
	}

	return loss;
}

int ShanXiFeatureToggles::calc_loss_factor(FANXING fans) {
	int factor = 1;
	if (TieJin == FEAT_GAME_TYPE) {
		// ��С�Ժͺ�����С���ں����㷨�о��ǻ���ģ���������ֱ�ӱ���д������
		if (fans & FAN_QiXiaoDui) factor *= 2;
		if (fans & FAN_HaoHuaQiXiaoDui) factor *= 4;
		if (fans & FAN_YiTiaoLong) factor *= 2;
		if (fans & FAN_QingYiSe) factor *= 2;
		if (fans & FAN_ShiSanYao) factor *= 2;
	}
	MHLOG("*****���ݷ��ͼ���Factor  GameType=%d factor=%d", FEAT_GAME_TYPE, factor);
	return factor;
}


// ��ׯ����
// ��һ�ڶ���ʱ����ׯ�ǰ���ҪӮ����Ҽ���
int ShanXiFeatureToggles::calc_next_zhuangpos(int curpos, int winpos, int total_gangs) {

	MHLOG("*****************************����ׯ��λ��************************");
	if (winpos == INVAILD_POS) { // ����Ӯ
		if (TieJin == FEAT_GAME_TYPE) {
#if 1
			return GetNextPos(m_zhuangpos); // ׯ�ҵ���һ����ׯ
#else
			// ���������иܻ�ׯ���¼���ׯ��û�и�����ׯ������ǰ����ƵĹ���д�ġ�
			if (total_gangs) return GetNextPos(m_zhuangpos); // ׯ�ҵ���һ����ׯ
			else return m_zhuangpos;
#endif
		}
		//if (GuaiSanJiao == FEAT_GAME_TYPE) return curpos; // ���ƹ����ǣ�����ׯ���������һ���Ƶ�����ׯ
		if (GuaiSanJiao == FEAT_GAME_TYPE) { 
			++m_lianzhuang_jiafen;     // ���ƹ����ǻ�ׯҲ��������ׯ��
			return m_zhuangpos; }
		if (LinFenYiMenPai == FEAT_GAME_TYPE && m_playtype[MH_PT_HuangZhuangLunZhuang])
		{
			return GetNextPos(m_zhuangpos);
		}
		if (YingSanZui == FEAT_GAME_TYPE && m_playtype[MH_PT_HuangZhuangLunZhuang])
		{
			return GetNextPos(m_zhuangpos);
		}

		switch (FEAT_GAME_TYPE) {
		case TuiDaoHu:
			return curpos; // ����ׯ���������һ���Ƶ�����ׯ
		case PingLu:
		case YingSanZui:
		case HongTongWangPai:
		//// �ٷ�һ����  by wyz
		case LinFenYiMenPai:
		case LingChuanMaJiang:
		case JinChengMaJiang:
			return m_zhuangpos; // ��ׯʱׯ�Ҽ�����ׯ
		}
		// ��ׯʱ�Ķ�ׯ����Ĭ��ׯ�ҵ���һ����ׯ
		return GetNextPos(m_zhuangpos); // ׯ�ҵ���һ����ׯ
	} else if (m_zhuangpos == winpos) { // ׯӮ
		// ׯӮ��һ����Ӯ���㣬��ׯ����
		++m_lianzhuang_jiafen;
		// ׯӮʱ�Ķ�ׯ����Ĭ��ׯ�Ҽ�����ׯ
		return winpos;
	} else { // ��Ӯ
		// �����ׯ�Ʒ�
		m_lianzhuang_jiafen = 0;
		// ��Ӯʱ�Ķ�ׯ����Ĭ���ǻ�ʤ�м���ׯ
		switch (FEAT_GAME_TYPE) {
		case JinZhong:
		case TieJin:
		//case GuaiSanJiao: ������˭Ӯ˭��ׯ
		case YingSanZui:
		case HongTongWangPai:
		//// �ٷ�һ����  by wyz
		case LinFenYiMenPai:
		case LingChuanMaJiang:
		case JinChengMaJiang:
			return GetNextPos(m_zhuangpos); // ׯ�ҵ���һ����ׯ
		default:
			return winpos; // ��Ҫ�Ļ�ʤ�м�������ׯ
		}
	}
}

int ShanXiFeatureToggles::init_desk_card_remain_count()
{
	switch (FEAT_GAME_TYPE)
	{
	case LingChuanMaJiang:
		m_remain_card_count = 12;
		break;
	case XinZhouKouDian:
		m_remain_card_count = 12;
		break;
	default:
		m_remain_card_count = 0;
		break;
	}
	if (FEAT_GAME_TYPE == HongTongWangPai && (!m_playtype[PT_HongTongBuKePengPai]))
	{
		m_remain_card_count = 14;
	}
	return m_remain_card_count;
}

