#ifndef _SAN_DA_YI_GAME_LOGIC_H_
#define _SAN_DA_YI_GAME_LOGIC_H_

#pragma once

#include "mhgamedef.h"
#include <cstdlib>
#include <time.h>
#include <vector>
#include <algorithm>


////////////////////////////////////////// �궨�� ////////////////////////////////////////////

//���鳤��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//��Ч����
#define CT_ERROR					0									//��������
#define VALUE_ERROR					0x00								//��ֵ����
#define COLOR_ERROR					0xFF								//��ɫ����

//��ɫ����
#define COLOR_NT					0x40								//��ɫ����
#define COLOR_HEI_TAO				0x30								//��ɫ����
#define COLOR_HONG_TAO				0x20								//��ɫ����
#define COLOR_MEI_HUA				0x10								//��ɫ����
#define COLOR_FANG_KUAI				0x00								//��ɫ����

//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

//��������
#define COLOR_RIGHT					40									//����Ȩλ




class SDYGameLogic
{

	//���Ա���
protected:

	/* �Ƹ��� */
	int	m_packCount;

	/* �������� */
	int m_baseCardsCount;

	/* �������� */
	int	m_sendCardsCount;

	//�߼�����
protected:

	/* ����ɫ */
	int	m_mainColor;

	/* ������ֵ */
	int	m_commMainValue;

	/* ����Ȩλ */
	int m_sortRight[5];

	/* һ���Ƶ���ֵ */
	static const int m_allCardsData[SANDAYI_CELL_PACK];


	//��������
public:

	/* ���캯�� */
	SDYGameLogic();

	/* �������� */
	virtual ~SDYGameLogic();

	//����״̬
public:

	/* �����Ƹ��� */
	bool SetPackCount(int packCount);

	/* ��ȡ�Ƹ��� */
	int GetPackCount();

	/* ���õ������� */
	bool SetBaseCardsCount(int baseCardsCount);

	/* ��ȡ�������� */
	int GetBaseCardsCount();

	/* ���ó�����ֵ */
	bool SetCommMainValue(int commMainValue);

	/* ��ȡ������ֵ */
	int GetCommMainValue();

	/* ���÷��������� */
	bool SetSendCardsCount(int sendCardsCount);

	/* ��ȡ���������� */
	int GetSendCardsCount();

	//��Ϣ����
public:

	/* ��������ɫ */
	bool SetMainColor(int mainColor);

	/* ��ȡ����ɫ */
	int GetMainColor();

	//�߼�����
public:

	/* ��ȡ������ֵ */
	int GetCardValue(int cardData);

	/* ��ȡ���滨ɫ */
	int GetCardColor(int cardData);

	/* ��ȡ�Ƶ�����ֵ */
	int GetCardSortValue(int cardData);

	/* ��ȡ�Ƶ��߼�ֵ */
	int GetCardLogicValue(int cardData);

	/* ��ȡ�Ƶ��߼���ɫ */
	int GetCardLogicColor(int cardData);

	/* ��Ч��ֵ�ж� */
	bool IsValidCard(int cardData);

	/* �ж��Ƿ�Ϊ��÷��ڵĻ�ɫ */
	bool IsValidColor(int cardColor);

	/* �Ƿ�ͬ�� */
	bool IsSameColor(int firstCard, int secondCard);

	/* �ж�ѡ�Լҵ����Ƿ����Ҫ�� */
	bool IsValidFirendCard(int selectCard, int selectScore);

	//���ƺ���
public:

	/* �����˿� */
	void SortCardList(int cardsData[], int cardsCount);

	/* �����˿� */
	void RandCardList(int cardsBuffer[], int bufferCount);

	/* �Ƿ����Ŀ����ֵ */
	bool IncludeCard(const int cardsData[], int cardsDataCount, int destCard);

	/* ɾ���˿� */
	bool RemoveCard(const int removeCards[], int removeCount, int cardsData[], int cardsDataCount);

	/* ��ȡ���� */
	int GetCardsScore(const int cardsData[], int cardsDataCount);

	/* ��ȡ���� */
	int GetScoreCards(const int cardsData[], int cardsDataCount, int scoreCards[], int maxCount = 12);

	/* �Ա��˿��߼�ֵ */
	bool CompareCardData(int firstCard, int secondCard);

	/* �ҳ������������߼���ֵ��Ӧ��λ�� */
	int GetMaxCardIndexByLogicValue(const int cardsData[], int cardsCount);

	/* �Աȳ��� */
	int CompareOutCardArray(const int outCardArr[], int firstOutPos, int outPlayerCount = 5);

	/* �����ƻ�ɫ��ȡ�˿� */
	int DistillCardByColor(const int cardsData[], int cardsDataCount, int destCard, int resultCards[]);

	/* У�������ҳ��� */
	bool EfficacyOutCard(int outCard, const int handCards[], int handCardsCount, int firstOutCard);

	/* ����Ȩλ */
	bool UpDataSortRight();
};



#endif // !_SAN_DA_YI_GAME_LOGIC_H_
