#pragma once


#include <cstring>
#include <vector>
#include <algorithm>
#include "mhmsghead.h"
/*
����������Ϸ���ͱ�ţ����Զ�ע��Ϊ GameHandler���Զ��� Desk �趨����

C �� LMsgC2SCreateDesk �� LM
LM �� LMsgLMG2LCreateDesk �� L
L ���� CreateVipDesk ��������
m_state ������ԭ���� C ת���� L �ģ��� LM Ҳ��ʹ�����ֵ

ƴ���� ��� ����
*/
/*
	by wyz 20171128 ����˿�����Ϸ����Ϸ�����101-999
*/

#define POKER_GAME_TYPE_MAP(XX) \
	XX(DouDiZhu			, 101, 3) \
	XX(YingSanZhang			, 102, 5) \
	XX(NiuNiu			, 103, 10) \
    XX(TuiTongZi			, 104, 8) \
	XX(ShuangSheng          , 106, 4) \
	XX(SanDaEr 				, 107, 5) \
	XX(NiuNiu_New			, 108, 10) \
	XX(SanDaYi				, 109, 4) \
    XX(WuRenBaiFen          , 110, 5) \
	XX(CheXuan              , 111, 8) \
	/*END*/


// GameType
enum {
#define XX(k, v, player_count) k = v,
	POKER_GAME_TYPE_MAP(XX)
#undef XX
};

//enum  PLAYTYPE
//{
//	PT_NONE = 0,
//	PT_BaoTing = 1, // ����
//	PT_DaiFeng = 2, // ����
//	PT_ZhiKeZiMoHu = 3, // ֻ��������
//	PT_QingYiSeJiaFan = 4, // ��һɫ�ӷ�
//	PT_YiTiaoLongJiaFan = 5, // һ�����ӷ�
//	PT_ZhuoHaoZi = 6, // ׽����
//
//	PT_GuoHuZhiKeZiMo = 7, // ����ֻ������
//	PT_QiangGangHu = 8, // ���ܺ�
//	PT_HuangZhuangBuHuangGang = 9, // �ܲ��������������ׯ���ĸ�
//	PT_WeiShangTingGangBuSuanFen = 10, // δ�����ܲ����
//	PT_YiPaoDuoXiang = 11, // һ�ڶ���
//
//	// ����Сѡ��������С�ԡ���һɫ����һɫ����һɫ������ֻ����������ׯ���ĸܡ����ƿɸܡ�ֻ�к�����Ҹ����
//	// ����������Сѡ��
//	PT_TingPaiKeGang = 12, // ���ƿɸ�
//	PT_QiXiaoDui = 13, // ��С��
//	PT_ZhiYouHuPaiWanJiaGangSuanFen = 14, // ֻ�к�����Ҹ���֣��������������
//	PT_FengYiSe = 15, // ��һɫ
//	PT_QingYiSe = 16, // ��һɫ��������һɫ�ӷ�
//	PT_CouYiSe = 17, // ��һɫ
//	PT_AnGangKeJian = 18, // ���ܿɼ�
//
//	// ����Сѡ��������С�ԡ�һ��������һɫ��ʮ���ۡ�δ�Ͻ���ֻ��������4��8��
//	// ����������Сѡ��
//	PT_ShiSanYao = 19, // ʮ����
//	PT_YiTiaoLong = 20, // һ����
//	PT_SiJin = 21, // 4��
//	PT_BaJin = 22, // 8��
//	PT_ShangJinShaoZheZhiKeZiMo = 23, // �Ͻ�����ֻ������
//
//	// ������Сѡ��������С�ԡ�ʮ���ۡ�һ�ڶ��죨Ĭ��ѡ�У�
//
//	// 2017-3-14: �Ƶ�������Сѡ��
//	PT_BianKanDiao = 24, // �߿���
//
//	// ��ƽ³����Сѡ����
//	PT_WeiShangTingDianPaoBaoPei = 25, // δ�������ڰ���
//	PT_YiWuJiu = 26, // һ���
//	PT_DongLingTou = 27, // ����ͷ
//
//	// ��Ӳ��������Сѡ����
//  //// by wyz 20170906 PT_ZhuangJiaYiFen ������˵����ׯ��һ�ֵ���˼�����趨��ׯ�ҷ��շֻ����ϼ�һ�֣���ׯ�ҷ��շֲ�Ϊ0��FEAT_DealerRiskScore+1��
//	////Ϊ�˴���Ĭ�����ׯ��һ�֣��û�����ѡ�����ֵ����,����ʵ��get_DealerRiskScore()
//	////�Ƿ�����ׯ�ҷ��շ֣�������FEAT_DealerRiskScoreDisabled����,����Сѡ��PT_ZhuangSuanFen����
//	PT_ZhuangJiaYiFen = 28, // ׯ��1�֣��͡�ׯ��֡�������ͬ������ʾ���ø��淨��ׯ���
//	PT_ZiYiSe = 29, // ��һɫ
//	PT_QingLong = 30, // ����
//	PT_QingQiXiaoDui = 31, // ����С��
//	PT_HaoHuaQiXiaoDui = 32, // ������С��
//
//	// ���鶴��������Сѡ����
//	PT_GangBuSuiHu = 33, // �ܲ����
//	PT_QueLiangMen = 34, // ȱ����
//	PT_ZiPai = 35, // ����
//	PT_SanYuanZiMo = 36, // ��Ԫ����
//	PT_DiaoWang = 37, // ����
//	PT_QingHaoHuaQiXiaoDui = 38, // �������С��
//
//	// ���ٷ�һ��������Сѡ����
//	PT_ShuYe = 39, // ��ҳ
//	PT_YiZhangYing = 40, // һ��Ӯ
//	PT_SanYuan = 41, // ��Ԫ
//	PT_ZiYiSeQiDaDui = 42, // ��һɫ�ߴ��
//	PT_ShuangHaoHuaQiXiaoDui = 43, // ˫������С��
//	PT_SanFeng = 44, // ����
//
//	// 2017-3-20: �ٵ㲹��Сѡ��
//	PT_ShuangHaoZi = 45, // ˫����
//	PT_ZhuangSuanFen = 46, // ׯ���
//
//	// 2017-3-24: �ٵ㲹��Сѡ��
//	PT_YiTiaoLongKeHuSanSiWu = 47, // һ�����ɺ�345
//	// �ٵ㣺���ڼ���ʱ��������Щ���Ʊ��붼��3�����ϵ�
//	// ���û��6�����ϵĺ��ƣ�������ھ�����
//	// �����أ����ĳ�������Ǻ���һ�����������Ǻ�345���ͱ������������Ȼ�����ڲ�����
//	// ֮���أ�Ҳֻ�������������ܱ����˵�345��
//	PT_HongTongBuKePengPai = 48, // �鶴���Ʋ�����С��	
//
//	// 2017-10-8: �۵㲹��Сѡ��
//	MH_PT_FengHaoZi = 49, // �۵�����������
//
//	// 2017-11-3: ���������Сѡ��
//	MH_PT_YingBaZhang = 50,              // Ӳ���ţ�����ʱ������һ����������8��
//	MH_PT_LianZhuangJiaFen = 51,         // ��ׯ�ӷ֣�ׯ���������Ƽ���ׯ��
//	MH_PT_HuangZhuangLunZhuang = 52,      // ��ׯ��ׯ ��ׯ���¼���ׯСѡ� һ������Ҫ
//
//	PT_MAX = 63, // ���� GameOptionSet
//
//	// ��������������̫���ˣ������Ƶ���
//	PT_PingHu = 10001, // С���淨��Ҳ������ʾ��ƽ��������
//	PT_DaHu = 10002,   // 
//
//};
//
//bool shanxi_game_option_validate_by_game_type(int v, int game_type);
//
//struct GameOptionSet {
//	typedef char CHECK_PT_MAX[PT_MAX <= 63 ? 1 : -1];
//	unsigned long long mask;
//	bool operator [](int pt) const { return 0 != (mask & (1ull << pt)); }
//	void set(int pt) { mask |= 1ull << pt; }
//
//	bool b_37Jia; //37��
//	bool b_baoTing;//����
//	bool b_daiFeng;//����
//	bool b_zimoHu;//ֻ��������
//	bool b_HaoZi;//׽����
//	bool b_QingYiSeJiaFan;//��һɫ
//	bool YiTiaoLongJiaFan;//��һɫ����һ����
//	bool DaHu;
//	bool XiaoHu;
//
//	GameOptionSet() {
//		clear();
//	}
//	void clear() {
//		memset(this, 0, sizeof(*this));
//	}
//
//	// �����Ƶ����Ϳ۵㣬���������޶��淨Сѡ����͹ر�
//	void mh_fill_default_playtype_by_GameType(int game_type)
//	{
//		switch (game_type)
//		{
//		case TuiDaoHu:
//			set(PT_QiangGangHu);
//			//set(PT_HuangZhuangBuHuangGang);
//			//set(PT_YiPaoDuoXiang);
//			break;
//		case KouDian:
//			set(PT_QiangGangHu);
//			//set(PT_HuangZhuangBuHuangGang);
//			set(PT_QiXiaoDui);			
//#ifdef _MH_ENABLE_LOG_DEBUG
//			for (int i = 0; i < PT_MAX; i++)
//			{
//				if( (*this)[i])
//				   MHLOG("*******������Ĭ�ϴ�ѡ�� option= %d, game_type=%d", i,  game_type);
//			}
//#endif // _MH_ENABLE_LOG_DEBUG			
//			break;
//		case GuaiSanJiao:
//			//set(MH_PT_YingBaZhang);
//			break;
//		default:
//			break;
//		}
//	}
//
//	bool mh_is_default_playtype_by_GameType(int game_type, int play_type_value)
//	{
//		// �����Ƶ����Ϳ۵㣬���������޶��淨Сѡ����͹ر�
//		switch (game_type)
//		{
//		case TuiDaoHu:
//			if (PT_QiangGangHu == play_type_value )
//				return true;
//			break;
//		case KouDian:
//			if (PT_QiangGangHu == play_type_value || PT_QiXiaoDui == play_type_value)
//				return true;
//			break;
//		default:
//			break;
//		}
//		return false;
//	}
//	
//
//	void init_playtype_info(std::vector<int>& l_play_type, int game_type) {
//
//		MHLOG("******�����淨Сѡ�� playtype.size() = %d", l_play_type.size());
//		for (int i = 0; i < l_play_type.size(); i++)
//		{
//			MHLOG("*****Сѡ�� i = %d value = %d", i, l_play_type[i]);
//		}
//		clear();
//		for (size_t x = 0; x < l_play_type.size(); x++) {
//			int v = l_play_type[x];
//			// û��ͨ��У���Сѡ��򵥺���
//			if (!shanxi_game_option_validate_by_game_type(v, game_type)) continue;
//			MHLOG("**********��֤Сѡ��ͨ��  value = %d", v);
//			if (v >= 0 && v <= PT_MAX) set(v);
//			switch (v) {
//			case PT_BaoTing: b_baoTing = true; break;
//			case PT_DaiFeng: b_daiFeng = true; break;
//			case PT_ZhiKeZiMoHu: b_zimoHu = true; break;
//			case PT_ZhuoHaoZi:
//			case MH_PT_FengHaoZi:       // ��ӿ۵�����
//				b_HaoZi = true; break;
//			case PT_QingYiSeJiaFan: b_QingYiSeJiaFan = true; break;
//			case PT_YiTiaoLongJiaFan: YiTiaoLongJiaFan = true; break;
//			case PT_PingHu: XiaoHu = true; break;
//			case PT_DaHu: DaHu = true; break;
//			}
//		}
//		// ���ӷ���Ĭ��֧�ֵ��淨ѡ��
//		mh_fill_default_playtype_by_GameType(game_type);
//	}
//
//	void fill_vector_for_client(std::vector<int> &a, int game_type) {
//		for (int i = 0; i <= PT_MAX; ++i) {
//			if (mh_is_default_playtype_by_GameType(game_type, i)) // ������Ĭ��֧�ֵ�ѡ����͸��ͻ���
//			{
//				MHLOG("****������Ĭ�ϴ�ѡ������͸��ͻ��� option=%d, game_type=%d", i, game_type);
//				continue;
//			}
//			if ((*this)[i]) a.push_back(i);
//		}
//		if (XiaoHu) a.push_back(PT_PingHu);
//		if (DaHu) a.push_back(PT_DaHu);
//	}
//
//
//};
//
//
//// ���������Ƴ�
//typedef GameOptionSet PlayTypeInfo;
//
//
////��������
//enum HU_PAI_LEIXING
//{
//	HU_NULL = 0,					//û��
//	HU_XIAOHU = 1,					//С��
//
//
//	HU_SPECIAL_XIAOQIDUI = 2,			//���߶�
//	HU_XIAOQIDUI = 3,					//��С��
//
//	HU_QINGYISE = 4,					//��һɫ
//	HU_JIANGJIANGHU = 5,				//������
//	HU_PENGPENGHU = 6,				//�ԶԺ�
//	HU_QUANQIUREN = 7,				//ȫ����
//	HU_GANG1 = 8,						//���Ͽ���
//	HU_GANG2 = 9,						//������
//	HU_END1 = 10,						//������(ɨ�׺�)
//	HU_END2 = 11,						//������
//	HU_QIANGGANGHU = 12,				//������
//	HU_QISHOUFOURLAN = 13,			//�����ĸ�����
//	HU_DOUBLESPECIAL_XIAOQIDUI = 14, //˫������С��
//	HU_TIANHU = 15,					//���
//	HU_DIHU = 16,						//�غ�
//	HU_DANDIAO = 17,				//����
//	HU_JINGOUDIAO = 18,				//�𹳵�
//
//	HU_QINGDUI = 19,				//���
//	HU_QINGQIDUI = 20,				//���߶�
//	HU_QINGLONGQIDUI = 21,			//�����߶�
//
//	HU_JIANGDUI = 22,               //����
//	HU_JIANGDUI_7 = 23,              //��7��
//	HU_ONENINE = 24,                 //ȫ�۾�
//	HU_MENQING = 25,				//����
//	HU_NOONENINE = 26,				//���۾� ����
//
//	HU_CA2TIAO = 27,                //4��2�� ��2��
//
//	HU_CA5XIN = 28,                 //����5
//	HU_ONE2NINE = 29,               //һ����
//	HU_SISTERPAIRS = 30,            //��ö�
//	HU_TRIPLESPECIAL_XIAOQIDUI = 31,//����������С��
//
//	HU_GOLDGOUPAO = 41,             //�˱� ����
//	HU_NOGOLDCARD = 42,             //������
//	HU_MIDGOLD = 43,             //����
//	HU_TING = 44,				//����
//
//	HU_QINGSANDA = 80,				//������
//	HU_QIANSI = 81,				//ǰ��
//	HU_HOUSI = 82,				//����
//
//	HU_BADCARD = 101,                //���� �ֲ��ظ� ��ɫ�Ƹ�������
//	HU_QIXIN = 102,				    //���� ���������з��׸�һ��������
//	HU_YAOCARD = 103,				//���� ȫ�ֺ�19��ɵ���
//	HU_JIAXINWU = 104,				//�����壨ֻ�����壩4��
//	HU_HUNYISE = 105,				//��һɫ
//	HU_DASANYUAN = 106,				//����Ԫ
//	HU_XIAOSANYUAN = 107,			//С��Ԫ
//	HU_SHIFENG = 108,				//ʮ��
//	HU_SHISANYAO_FEIXIAOJI = 109,	//ʮ����-���Ϸ�С��
//	HU_LONGZHUABEI = 110,			//��צ��
//	HU_SIYAOJI = 111,				//���ۼ�
//	HU_GANGSHANGWUMEIHUA = 112,     //������÷��
//	HU_WUJI = 113,					//�޼�
//	HU_XIAOJIGUIWEI = 114,			//С����λ
//	HU_DOUBLEGANG = 115,			//����
//
//	HU_ALLTYPE = 149,				//�ܷ���		 --��֤���
//
//	GANG_MINGGANG = 150,			//����
//	GANG_ANGANG = 151,			//����
//	GANG_BUGANG = 152,			//����
//	GANG_ZHUANYI = 153,			//����ת��
//	GANG_HANBAOG = 154,          //������
//
//	HU_Qixiaodui = 1003, // 7С��
//	HU_DaQixiaodui = 1004, // ����7С��
//	HU_QingYiSe = 1005, // ��һɫ
//	HU_YiTiaoLong = 1006, // һ����
//	HU_ShiSanYao = 1007, // ʮ����
//
//	HU_PingHu = 10001, // �� PT_xiaoHu ȡֵ��ͬ
//
//	// ��������������
//	HU_QUEMEN = 1010, // ȱ��
//	HU_BIANZHANG = 1011, // ����
//	HU_KANZHANG = 1012, // ����
//	HU_DIAOZHANG = 1013, // ����
//
//	// ��������������
//	HU_CouYiSe = 1021, // ��һɫ
//	HU_FengYiSe = 1022, // ��һɫ
//	HU_MenQing = 1023, // ����
//	HU_DuanYao = 1024, // ����
//	HU_PengPengHu = 1025, // ������
//
//	// ����������������
//	HU_KanHu = 1026, // ����
//
//	// ��Ӳ������������
//	HU_QingLong = 1027, // ����
//	HU_QingQiDui = 1028, // ���߶�
//	HU_ZiYiSe = 1029, // ��һɫ
//	HU_QingHaoHuaQiXiaoDui = 1030, // �������С��
//	HU_QueYiMen = 1031, // ȱһ��
//	HU_QueLiangMen = 1032, // ȱ����
//	HU_GuJiang = 1033, // �½�
//	HU_YiZhangYing = 1034, // һ��Ӯ
//	HU_DanDiao = 1035, // ����
//	HU_SanFeng_X1 = 1041, // ����X1
//	HU_SanFeng_X2 = 1042, // ����X2
//	HU_SanFeng_X3 = 1043, // ����X3
//	HU_SanFeng_X4 = 1044, // ����X4
//	HU_SanYuan_X1 = 1051, // ��ԪX1
//	HU_SanYuan_X2 = 1052, // ��ԪX2
//	HU_SanYuan_X3 = 1053, // ��ԪX3
//	HU_SanYuan_X4 = 1054, // ��ԪX4
//
//    // �鶴���Ʒ���
//	HU_HongTong_LouShangLou = 1055, // ¥��¥
//	HU_HongTong_YingKou = 1056,
//	HU_HongTong_DiaoWang = 1057, //����
//
//	// ��������ׯ 
//	HU_LianZhuang = 1058
//};



