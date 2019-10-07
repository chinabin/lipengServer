#ifndef	_TUI_TONG_ZI_GAME_LOGIC_H_
#define _TUI_TONG_ZI_GAME_LOGIC_H_
#pragma once

#include "mhgamedef.h"
#include "Stdafx.h"
#include <cstdlib>
#include <time.h>

//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

/* �ϵ� */
#define UP_DAO 8

/* ������Ӯ�����ö�٣�*/
enum DAO_RESULT
{
	DOWN_LOSE = 0,  //δ�ϵ���
	DOWN_WIN =  2,  //δ�ϵ�Ӯ
	UP_LOSE =   3,  //�ϵ���
	UP_WIN =    4   //�ϵ�Ӯ
};

/* ���ÿ�������Լ���Ӯ�Ľ�����ṹ�壩*/
struct PlayerState
{
	BYTE        handCards[TUITONGZI_HAND_CARD_MAX];   //�������
	bool        ya_result;                            //��ҡ�Ѻ���ϵ���Ӯ���
	DAO_RESULT  dao_result;                           //��ҡ������ϵ���Ӯ���
	bool        ruan_result;                          //��ҡ����ϵ���Ӯ���
	bool        hong_result;                          //��ҡ��족�ϵ���Ӯ���

	/* ���캯�� */
	PlayerState();

	/* �������� */
	~PlayerState();

	void clear();
};

/* ��Ͳ�ӻ����㷨���ࣩ*/
class TGameLogic
{
private:

	static const BYTE m_cbCardListData[36];  //��Ͳ��ʹ��1-9�㣬ÿ��4��
	static const BYTE m_cbCardListDataBaiBan[40];
	BYTE m_randCardList[8];            //�洢4����ҵ�8����

public:
	TGameLogic()
	{}

	~TGameLogic()
	{}

public:
	/***********************************************
	* ��������   sordHandCards()
	* ������     �������ƣ���С����ǰ��
	* ������
	*  @ handCards ����������飨�����ƣ�
	* ���أ�     (void)
	************************************************/
	void sordHandCards(BYTE* handCards);

	/******************************************************************
	* ��������   compareCardByYa()
	* ������     �мҺ�ׯ�ұȽ��Ƶĵ�����Ѻ��
	* ������
	*  @ zhuang_handCards ׯ���������飨�����ƣ�
	*  @ xian_handCards �м��������飨�����ƣ�
	* ���أ�     (int) -1�������д�0��ׯ��Ӯ��1��ׯ����
	*******************************************************************/
	int compareCardByYa(BYTE* zhuang_handCards, BYTE* xian_handCards);

	/***********************************
	* ��������    isDuiZi()
	* ������      �ж������Ƿ�Ϊ����
	* ������
	*  @ handCards ����������飨�����ƣ�
	* ���أ�      (bool) ���ӷ���true
	************************************/
	bool isDuiZi(BYTE* handCards);

	/*********************************************
	* ��������    calcDianShu()
	* ������      �������Ƶ����ͣ�������ֵ��ӣ�
	* ������
	*  @ handCards ����������飨�����ƣ�
	* ���أ�      (BYTE) ������
	**********************************************/
	BYTE calcDianShu(BYTE* handCards);

	/***********************************
	* ��������    isUpDao()
	* ������      �ж������Ƿ��ϵ�
	* ������
	*  @ handCards �����������
	* ���أ�      (bool) �ϵ�����true
	************************************/
	bool isUpDao(BYTE* handCards);

	/******************************************************
	* ��������     isHandCards()
	* ������       ��������Ƿ�Ϸ�
	* ������
	*  @ handCards ����������飨�����ƣ�
	* ���أ�       (bool)�Ϸ�����true
	*******************************************************/
	bool isHandCardsValid(BYTE* handCards);

	/******************************************************
	* ��������     randCardList()
	* ������       ��һ�����������ȡ8���ƣ�׼������ÿ�����
	* ���أ�       (void)
	*******************************************************/
	void randCardList();

	/******************************************************
	* ��������     randStart()
	* ������       ������
	* ���أ�       (void)
	*******************************************************/
	int randStart();



	//������
	void RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount, BYTE isBaiBan=0);
	void RandCardList1and9(BYTE cbCardBuffer[], BYTE cbBufferCount);
	void RandCardListBaiBan(BYTE cbCardBuffer[], BYTE cbBufferCount);
	//��������
	bool  isDui(BYTE* handCards);
	BYTE getBigCard(BYTE* handCards);
	BYTE getLogicCard(BYTE* handCards);
	bool   isDao(BYTE* handCards);
	//����
	int CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount);

};


#endif _TUI_TONG_ZI_GAME_LOGIC_H_