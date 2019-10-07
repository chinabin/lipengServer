#ifndef GAME_LOGIC_LINFEN_HEAD_FILE
#define GAME_LOGIC_LINFEN_HEAD_FILE

#pragma once

#include "Stdafx.h"
#include "CMD_Game_Linfen.h"

//////////////////////////////////////////////////////////////////////////////////

//��������
#define ST_ORDER					1									//��С����
#define ST_COUNT					2									//��Ŀ����
#define ST_CUSTOM					3									//�Զ�����

//////////////////////////////////////////////////////////////////////////////////

extern struct tagAnalyseResult;
//�����ṹ
//struct tagAnalyseResult
//
//	BYTE 							cbBlockCount[4];					//�˿���Ŀ
//	BYTE							cbCardData[4][MAX_COUNT_LINFEN];			//�˿�����
//;
/*
//���ƽ��
struct tagOutCardResult
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbResultCard[MAX_COUNT_LINFEN];			//����˿�
};

//�ֲ���Ϣ
struct tagDistributing
{
	BYTE							cbCardCount;						//�˿���Ŀ
	BYTE							cbDistributing[15][6];				//�ֲ���Ϣ
};

//�������
struct tagSearchCardResult
{
	BYTE							cbSearchCount;						//�����Ŀ
	BYTE							cbCardCount[MAX_COUNT_LINFEN];				//�˿���Ŀ
	BYTE							cbResultCard[MAX_COUNT_LINFEN][MAX_COUNT_LINFEN];	//����˿�
};
*/
//////////////////////////////////////////////////////////////////////////////////

#define FULL_COUNT_LINFEN				52									//ȫ����Ŀ

//��Ϸ�߼���
class CGameLogicLinfen
{
	//��������
protected:
	static const BYTE				m_cbCardData[FULL_COUNT_LINFEN];			//�˿�����

	//��������
public:
	//���캯��
	CGameLogicLinfen();
	//��������
	virtual ~CGameLogicLinfen();

	//���ͺ���
public:
	//��ȡ����
	BYTE GetCardType(const BYTE cbCardData[], BYTE cbCardCount);
	//��ȡ��ֵ
	BYTE GetCardValue(BYTE cbCardData) { return cbCardData&MASK_VALUE; }
	//��ȡ��ɫ
	BYTE GetCardColor(BYTE cbCardData) { return cbCardData&MASK_COLOR; }

	//���ƺ���
public:
	//�����˿�
	VOID RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//�����˿�
	VOID SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortType);
	//�����˿�
	VOID SortOutCardList(BYTE cbCardData[], BYTE cbCardCount);
	//ɾ���˿�
	bool RemoveCardList(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);
	//ɾ���˿�
	bool RemoveCard(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//�߼�����
public:
	//�߼���ֵ
	BYTE GetCardLogicValue(BYTE cbCardData);
	//�Ա��˿�
	bool CompareCard(const BYTE cbFirstCard[], const BYTE cbNextCard[], BYTE cbFirstCount, BYTE cbNextCount);
	////��������
	//BYTE SearchOutCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, const BYTE cbTurnCardData[], BYTE cbTurnCardCount, 
	//	tagSearchCardResult *pSearchCardResult );
	////ͬ������
	//BYTE SearchSameCard( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCardCount,
	//	tagSearchCardResult *pSearchCardResult );
	////��������
	//BYTE SearchLineCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbBlockCount, BYTE cbLineCount,
	//	tagSearchCardResult *pSearchCardResult );
	////������������(����һ���Ĵ�һ��)
	//BYTE SearchTakeCardType( const BYTE cbHandCardData[], BYTE cbHandCardCount, BYTE cbReferCard, BYTE cbSameCount, BYTE cbTakeCardCount, 
	//	tagSearchCardResult *pSearchCardResult );
	////�����ɻ�
	//BYTE SearchThreeTwoLine( const BYTE cbHandCardData[], BYTE cbHandCardCount, tagSearchCardResult *pSearchCardResult );

	//�ڲ�����
public:
	////�����˿�
	//BYTE MakeCardData(BYTE cbValueIndex, BYTE cbColorIndex);
	//�����˿�
	VOID AnalysebCardData(const BYTE cbCardData[], BYTE cbCardCount, tagAnalyseResult & AnalyseResult);
	//�����ֲ�
	//VOID AnalysebDistributing(const BYTE cbCardData[], BYTE cbCardCount, tagDistributing & Distributing);

	bool  HaveSpecCard(const BYTE cbCardData[], BYTE cbCardCount);
};

//////////////////////////////////////////////////////////////////////////////////

#endif