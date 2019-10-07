#pragma once
#ifndef NIU_NIU_NEW_GAME_LOGIC_HEAD_FILE
#define NIU_NIU_NEW_GAME_LOGIC_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include <cstdlib>

//////////////////////////////////////////////////////////////////////////
//�궨��

//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//�˿�����
#define OX_VALUE0					0									//�������
#define OX_BIYINIU					100									//����ţ
#define OX_FOURKING					101									//****��������
#define OX_SHUNZINIU                102									//˳��ţ����5����
#define OX_WUHUANIU                 103									//�廨ţ�������Ƶĵ��� >= 10�㣨5����
#define OX_ALLTEN					104									//  ****��ţ�������ƶ���С��10-��ţ�������Ƶ��� >= 10 / �廨ţ
#define OX_FIVEKING					105									//  ****��ţ����������-��ţ �������Ƶĵ��� >= J
#define OX_DRAGON				    106									//****һ������һ��������
#define OX_COLOR_SAME               107									//ͬ��ţ����ɫ��ͬ��6����
#define OX_THREE_SAME				108									//��«ţ������������ / ��«ţ��7����
#define OX_FOUR_SAME				109									//ը��ţ����������-ը����8����
#define OX_FIVESMALL				110									//��Сţ���������ܺ�<=10-��Сţ
#define OX_TONGHUASHUN              111									//����ţ��ͬ��˳ţ��10����


#define NIU_NIU_POKER_MAX_COUNT		5									//�����Ŀ

//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//////////////////////////////////////////////////////////////////////////

//�����ṹ
struct tagAnalyseResult
{
	BYTE 							cbFourCount;						//������Ŀ
	BYTE 							cbThreeCount;						//������Ŀ
	BYTE 							cbDoubleCount;						//������Ŀ
	BYTE							cbSignedCount;						//������Ŀ
	BYTE 							cbFourLogicVolue[1];				//�����б�
	BYTE 							cbThreeLogicVolue[1];				//�����б�
	BYTE 							cbDoubleLogicVolue[2];				//�����б�
	BYTE 							cbSignedLogicVolue[5];				//�����б�
	BYTE							cbFourCardData[NIU_NIU_POKER_MAX_COUNT];			//�����б�
	BYTE							cbThreeCardData[NIU_NIU_POKER_MAX_COUNT];			//�����б�
	BYTE							cbDoubleCardData[NIU_NIU_POKER_MAX_COUNT];		//�����б�
	BYTE							cbSignedCardData[NIU_NIU_POKER_MAX_COUNT];		//������Ŀ
};


//////////////////////////////////////////////////////////////////////////

//��Ϸ�߼���
class NNGameLogic
{
//��������
private:

	//�˿˶��壨������С����
	static BYTE						m_cbCardListData[52];				

	//�˿˶��壨����С����
	static BYTE						m_cbCardListData_LaiZi[54];

	/*
	ţţ����ѡ��
	1��ţţx3  ţ��x2  ţ��x2  ţ��-��ţx1
	2��ţţx4  ţ��x3  ţ��x2  ţ��x2  ţ�� - ��ţx1
	*/
	int m_niuNiuTimes;

	//���������
	int m_isAllowLaiZi;

	//������������
	bool m_isAllowBiYi;
	bool m_isAllowShunZi;
	bool m_isAllowWuHua;
	bool m_isAllowTongHua;
	bool m_isAllowHuLu;
	bool m_isAllowZhaDan;
	bool m_isAllowWuXiao;
	bool m_isAllowKuaiLe;

//��������
public:

	//���캯��
	NNGameLogic();

	//��������
	virtual ~NNGameLogic();

//���ͺ���
public:

	//����ţţ����ѡ��
	void SetNiuNiuTimes(int niuNiuTimes);

	//�����Ƿ��������
	void SetAllowLaiZi(bool isAllowLaiZi);

	//�����������������
	void SetAllowSecialType(bool isAllowBiYi, bool isAllowShunZi, bool isAllowWuHua, bool isAllowTongHua, bool isAllowHuLu, bool isAllowZhaDan, bool isAllowWuXiao, bool isAllowKuaiLe);

	//�ж������������
	int GetHandCardsLaoZiCount(const BYTE handCards[], BYTE handCardsCount);

	//��ȡ����
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//��ȡһ����ӵ����ͣ�cbSortCalcCards[] Ϊ������Ž����ͣ�
	BYTE GetOneLaiZiCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE& bestLaiZiCard, BYTE cbSortCalcCards[] = NULL);

	//��������Ƶ�����
	BYTE CalcCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }

	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//��ȡţţ���α���
	BYTE GetTimes(BYTE cbCardData[], BYTE cbCardCount);

	//1��ţţx3  ţ��x2  ţ��x2  ţ��-��ţx1
	BYTE GetTimes_x3(BYTE cbCardData[], BYTE cbCardCount, BYTE bTimes);

	//2��ţţx4  ţ��x3  ţ��x2  ţ��x2  ţ�� - ��ţx1
	BYTE GetTimes_x4(BYTE cbCardData[], BYTE cbCardCount, BYTE bTimes);

	//��ȡţţ
	bool GetOxCard(BYTE cbCardData[], BYTE cbCardCount);

	//��ȡ����
	bool IsIntValue(BYTE cbCardData[], BYTE cbCardCount);

	//�ж�4���Ƶ�ţţ��
	int GetCardsOxNumber(const BYTE handCards[], int handCardsCount);

//���ƺ���
public:

	//���˿��߼�ֵ�����˿ˣ�J/Q/K = 10��
	void SortCardByLogicValue(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//���˿���ֵ�����˿ˣ�J=11  Q=12  K=13��
	void SortCardByCardValue(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//����cbCardDataֵ�����˿�
	void SortCardByCardData(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//�����˿�
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[] = NULL);

	//ϴ��
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//ϴ��-������С��
	void RandCardList_normal(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//ϴ��-����С��
	void RandCardList_laizi(BYTE cbCardBuffer[], BYTE cbBufferCount);

//���ܺ���
public:

	//�߼���ֵ��J,Q,K ���ص�Ϊ10��
	BYTE GetCardLogicValue(BYTE cbCardData);

	/*
	*  �������Ա��˿�
	*  ������
	*   @cbFirstData[] �м�����
	*   @cbNextData[] ׯ������
	*   @cbCardCount ��������
	*   @XianOX �м�ţţ����
	*   @ZhuangOX ׯ��ţţ����
	*  ���أ�
	*   �м� > ׯ��  true
	*   �м� <= ׯ��  false
	*/
	bool CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BYTE FirstOX, BYTE NextOX);

	//��Ӳ������Ž���ʹ�õĺ���
	bool CompareCard_Sample(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BYTE XianOX, BYTE ZhuangOX);

};

//////////////////////////////////////////////////////////////////////////

#endif
