#ifndef NIU_NIU_GAME_LOGIC_HEAD_FILE
#define NIU_NIU_GAME_LOGIC_HEAD_FILE

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
#define OX_FOURKING					101									//��������
#define OX_WUHUANIU                 102									//�廨ţ�������Ƶĵ��� >= 10��
#define OX_ALLTEN					103									//  ��ţ�������ƶ���С��10-��ţ�������Ƶ��� >= 10 / �廨ţ
#define OX_FIVEKING					104									//  ��ţ����������-��ţ �������Ƶĵ��� >= J
#define OX_SHUNZINIU                105									//˳��ţ��
#define OX_THREE_SAME				106									//��«ţ���������� / ��«ţ
#define OX_DRAGON				    107									//һ������һ��������
#define OX_COLOR_SAME               108									//ͬ��ţ��
#define OX_FOUR_SAME				109									//ը��ţ����������-ը��
#define OX_FIVESMALL				110									//��Сţ���������ܺ�<=10-��Сţ
#define OX_TONGHUASHUN              111									//ͬ��˳ţ��




#define NIU_NIU_POKER_MAX_COUNT						5									//�����Ŀ

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
class NGameLogic
{
	//��������
private:
	static BYTE						m_cbCardListData[52];				//�˿˶���

																		//��������
public:
	//���캯��
	NGameLogic();
	//��������
	virtual ~NGameLogic();

	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec = false);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&LOGIC_MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&LOGIC_MASK_COLOR; }

	//��ȡţţ���α���
	BYTE GetTimes(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec, BYTE cbSweep);
	//��ͨ����
	BYTE GetTimes_normal(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec = false);
	//��������X1
	BYTE GetTimes_x4(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec = false);


	//��ȡţţ
	bool GetOxCard(BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ����
	bool IsIntValue(BYTE cbCardData[], BYTE cbCardCount);

	//���ƺ���
public:
	//�����˿�
	void SortCardList(BYTE cbCardData[], BYTE cbCardCount);
	//�����˿�
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);

	//���ܺ���
public:
	//�߼���ֵ��J,Q,K ���ص�Ϊ10��
	BYTE GetCardLogicValue(BYTE cbCardData);

	//�Ա��˿�
	bool CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BOOL FirstOX, BOOL NextOX, BOOL isSpecPlay);
};

//////////////////////////////////////////////////////////////////////////

#endif
