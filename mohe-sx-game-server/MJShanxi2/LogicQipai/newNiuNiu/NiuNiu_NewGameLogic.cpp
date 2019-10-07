#include "StdAfx.h"
#include "NiuNiu_NewGameLogic.h"

/* ���α���ѡ�� */
static enum TYEP_TIME
{
	TYPE_TIME_NORMAL = 0,		//��ͨ����
	TYPE_TIME_BOMB = 1,			//ɨ�ױ���
	TYPE_TIME_X4 = 2,			//�¼ӱ���ţţX4
};


//////////////////////////////////////////////////////////////////////////

//�˿����ݣ�������С����
BYTE NNGameLogic::m_cbCardListData[52] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� (0)  A(1)  - K(13)
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� (16) A(17) - K(29)
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� (32) A(33) - K(45)
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//���� (48) A(49) - K(61)
};

//�˿����ݣ�����С����
BYTE NNGameLogic::m_cbCardListData_LaiZi[54] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� (0)  A(1)  - K(13)
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� (16) A(17) - K(29)
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� (32) A(33) - K(45)
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//���� (48) A(49) - K(61)
	0x4E,0x4F															//С��������
};

//////////////////////////////////////////////////////////////////////////

//���캯��
NNGameLogic::NNGameLogic()
{
	//��ʼ��ţţ����ѡ��
	this->m_niuNiuTimes = 1;

	//��ʼ���������
	this->m_isAllowLaiZi = false;

	//��ʼ���������������
	this->m_isAllowKuaiLe = false;
	this->m_isAllowZhaDan = false;
	this->m_isAllowHuLu = false;
	this->m_isAllowTongHua = false;
	this->m_isAllowShunZi = false;
	this->m_isAllowWuHua = false;
	this->m_isAllowBiYi = false;
}

//��������
NNGameLogic::~NNGameLogic()
{
}

//����ţţ����ѡ��
void NNGameLogic::SetNiuNiuTimes(int niuNiuTimes)
{
	this->m_niuNiuTimes = niuNiuTimes;
}

//�����Ƿ��������
void NNGameLogic::SetAllowLaiZi(bool isAllowLaiZi)
{
	this->m_isAllowLaiZi = isAllowLaiZi;
}

//�����������������
void NNGameLogic::SetAllowSecialType(bool isAllowBiYi, bool isAllowShunZi, bool isAllowWuHua, bool isAllowTongHua, bool isAllowHuLu, bool isAllowZhaDan, bool isAllowWuXiao, bool isAllowKuaiLe)
{
	this->m_isAllowBiYi = isAllowBiYi;
	this->m_isAllowShunZi = isAllowShunZi;
	this->m_isAllowWuHua = isAllowWuHua;
	this->m_isAllowTongHua = isAllowTongHua;
	this->m_isAllowHuLu = isAllowHuLu;
	this->m_isAllowZhaDan = isAllowZhaDan;
	this->m_isAllowWuXiao = isAllowWuXiao;
	this->m_isAllowKuaiLe = isAllowKuaiLe;
}

//�ж����ƺ�������
int NNGameLogic::GetHandCardsLaoZiCount(const BYTE handCards[], BYTE handCardsCount)
{
	int t_laiZiCount = 0;
	if (!m_isAllowLaiZi)
	{
		return 0;
	}
	for (int i = 0; i < handCardsCount; ++i)
	{
		if (handCards[i] == 0x4E || handCards[i] == 0x4F)
		{
			++t_laiZiCount;
		}
	}
	return t_laiZiCount;
}

//��ȡ����
BYTE NNGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL)
	{
		return 0xFF;
	}

	//�����Ӽ�����������
	if (!m_isAllowLaiZi || GetHandCardsLaoZiCount(cbCardData, cbCardCount) == 0)
	{
		return CalcCardType(cbCardData, cbCardCount, cbSortOriginalCards);
	}
	//1�����
	else if (GetHandCardsLaoZiCount(cbCardData, cbCardCount) == 1)
	{
		BYTE t_cbSortOriginalCards[NIU_NIU_POKER_MAX_COUNT];

		//������ͨ�ƺ������
		BYTE t_nomalCards[4];
		int t_nomaluCardsCount = 0;
		BYTE t_laiZiCard = 0;

		int t_maxType = -1;
		BYTE t_maxCards[NIU_NIU_POKER_MAX_COUNT];

		int t_currType = 0;
		BYTE t_currCards[NIU_NIU_POKER_MAX_COUNT];
		
		for (int i = 0; i < cbCardCount; ++i)
		{
			if (cbCardData[i] == 0x4E || cbCardData[i] == 0x4F)
			{
				t_laiZiCard = cbCardData[i];
			}
			else
			{
				t_nomalCards[t_nomaluCardsCount++] = cbCardData[i];
			}
		}
		
		BYTE t_color = GetCardColor(t_nomalCards[0]);
		for (BYTE i = (t_color + 1); i < t_color + 14; ++i)
		{
			//����������
			/*if (i == t_nomalCards[0] || i == t_nomalCards[1] || i == t_nomalCards[2] || i == t_nomalCards[3])
			{
				continue;
			}*/

			memcpy(t_currCards, t_nomalCards, sizeof(t_nomalCards));
			t_currCards[4] = i;

			t_currType = CalcCardType(t_currCards, cbCardCount, t_cbSortOriginalCards);

			if (t_currType < t_maxType)
			{
				continue;
			}
			else if (t_currType > t_maxType || 
				(t_currType == t_maxType && t_maxType != -1 && CompareCard(t_currCards, t_maxCards, cbCardCount, t_currType, t_maxType)))
			{
				t_maxType = t_currType;
				memcpy(t_maxCards, t_currCards, sizeof(t_currCards));

				//���������滻�����
				if (cbSortOriginalCards != NULL)
				{
					//�滻�������
					int t_laiZiIndex = -1;
					int t_laiZiReplaceCard = 0;
					
					for (int n = cbCardCount - 1; n >= 0; --n)
					{
						if (t_cbSortOriginalCards[n] == i)
						{
							t_laiZiReplaceCard = t_cbSortOriginalCards[n];
							t_laiZiIndex = n;
							t_cbSortOriginalCards[n] = t_laiZiCard;
							break;
						}
					}
					
					for (int n = t_laiZiIndex + 1; n < cbCardCount - 1; ++n)
					{
						if (GetCardValue(t_laiZiReplaceCard) != GetCardValue(t_cbSortOriginalCards[n]))
						{
							break;
						}

						if (GetCardValue(t_laiZiReplaceCard) == GetCardValue(t_cbSortOriginalCards[n]) && 
							GetCardValue(t_laiZiReplaceCard) != GetCardValue(t_cbSortOriginalCards[n + 1]))
						{
							for (int m = t_laiZiIndex; m < n; ++m)
							{
								t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
							}
							t_cbSortOriginalCards[n] = t_laiZiCard;
							break;
						}
						else if (GetCardValue(t_laiZiReplaceCard) == GetCardValue(t_cbSortOriginalCards[n + 1]) && 
							n + 1 == cbCardCount - 1)
						{
							for (int m = t_laiZiIndex; m < n + 1; ++m)
							{
								t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
							}
							t_cbSortOriginalCards[n + 1] = t_laiZiCard;
							break;
						}
					}

					memcpy(cbSortOriginalCards, t_cbSortOriginalCards, sizeof(t_cbSortOriginalCards));
				}
			}
		}
		return t_maxType;
	}
	//2�����
	else if (GetHandCardsLaoZiCount(cbCardData, cbCardCount) == 2)
	{
		BYTE t_cbSortOriginalCards[NIU_NIU_POKER_MAX_COUNT];

		//������ͨ�ƺ������
		BYTE t_nomalCards[3];
		int t_nomaluCardsCount = 0;
		BYTE t_laiZiCard[2] = { 0 };

		int t_maxType = -1;
		BYTE t_maxCards[NIU_NIU_POKER_MAX_COUNT];

		int t_currType = 0;
		BYTE t_currCards[NIU_NIU_POKER_MAX_COUNT];

		for (int i = 0; i < cbCardCount; ++i)
		{
			if (cbCardData[i] == 0x4F)
			{
				t_laiZiCard[0] = cbCardData[i];
			}
			else if (cbCardData[i] == 0x4E)
			{
				t_laiZiCard[1] = cbCardData[i];
			}
			else
			{
				t_nomalCards[t_nomaluCardsCount++] = cbCardData[i];
			}
		}

		BYTE t_color = GetCardColor(t_nomalCards[0]);
		for (int i = t_color + 1; i < t_color + 14; ++i)
		{
			for (int j = i; j < t_color + 14; ++j)
			{
				memcpy(t_currCards, t_nomalCards, sizeof(BYTE) * 3);
				t_currCards[3] = i;
				t_currCards[4] = j;

				t_currType = CalcCardType(t_currCards, cbCardCount, t_cbSortOriginalCards);

				if (t_currType < t_maxType)
				{
					continue;
				}
				else if (t_currType > t_maxType ||
					(t_currType == t_maxType && t_maxType != -1 && CompareCard(t_currCards, t_maxCards, cbCardCount, t_currType, t_maxType)))
				{
					t_maxType = t_currType;
					memcpy(t_maxCards, t_currCards, sizeof(t_currCards));

					//���������滻�������
					if (cbSortOriginalCards != NULL)
					{
						//���е�������ʹ��С���ʹ����滻����
						int t_daWangCount = 1, t_xiaoWangCount = 1;
						int t_daWangIndex = -1, t_xiaoWangIndex = -1;
						int t_tmpDaWangCard = 0, t_tmpXiaoWangCard = 0;
						int t_daWangReplaceCard = 0, t_xiaoWangReplaceCard = 0;

						//���Ҵ����滻����λ��
						for (int n = 0; n < cbCardCount; ++n)
						{
							if (t_cbSortOriginalCards[n] == i)
							{
								t_daWangIndex = n;
							}
						}
						//����С���滻����λ��
						for (int n = 0; n < cbCardCount; ++n)
						{
							if (t_cbSortOriginalCards[n] == j && n != t_daWangIndex)
							{
								t_xiaoWangIndex = n;
							}
						}
						//����������С��ǰ��
						if (t_daWangIndex > t_xiaoWangIndex)
						{
							int t_index = t_daWangIndex;
							t_daWangIndex = t_xiaoWangIndex;
							t_xiaoWangIndex = t_index;
						}
						//�滻Ϊ����
						t_daWangReplaceCard = t_cbSortOriginalCards[t_daWangIndex];
						t_cbSortOriginalCards[t_daWangIndex] = 0x4F;
						t_tmpDaWangCard = t_cbSortOriginalCards[t_daWangIndex];
						//�滻ΪС��
						t_xiaoWangReplaceCard = t_cbSortOriginalCards[t_xiaoWangIndex];
						t_cbSortOriginalCards[t_xiaoWangIndex] = 0x4E;
						t_tmpXiaoWangCard = t_cbSortOriginalCards[t_xiaoWangIndex];

						//��С���䵱��ȵ���
						if (t_daWangReplaceCard == t_xiaoWangReplaceCard)
						{
							for (int n = t_daWangIndex + 2; n < cbCardCount - 1; ++n)
							{
								if (GetCardValue(t_daWangReplaceCard) != GetCardValue(t_cbSortOriginalCards[n]))
								{
									break;
								}
								if (GetCardValue(t_daWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n]) &&
									GetCardValue(t_daWangReplaceCard) != GetCardValue(t_cbSortOriginalCards[n + 1]))
								{
									for (int m = t_daWangIndex; m < n-1; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 2];
									}
									t_cbSortOriginalCards[n-1] = t_tmpDaWangCard;
									t_cbSortOriginalCards[n] = t_tmpXiaoWangCard;
									break;
								}
								else if (GetCardValue(t_daWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n + 1]) &&
									n + 1 == cbCardCount - 1)
								{
									for (int m = t_daWangIndex; m < n; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 2];
									}
									t_cbSortOriginalCards[n] = t_tmpDaWangCard;
									t_cbSortOriginalCards[n + 1] = t_tmpXiaoWangCard;
									break;
								}
							}
						}
						else
						{
							//�������
							for (int n = t_daWangIndex + 1; n < cbCardCount - 1; ++n)
							{
								if (GetCardValue(t_daWangReplaceCard) != GetCardValue(t_cbSortOriginalCards[n]))
								{
									break;
								}

								if (GetCardValue(t_daWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n]) &&
									GetCardValue(t_daWangReplaceCard) != GetCardValue(t_cbSortOriginalCards[n + 1]))
								{
									for (int m = t_daWangIndex; m < n; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
									}
									t_cbSortOriginalCards[n] = t_tmpDaWangCard;
									break;
								}
								else if (GetCardValue(t_daWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n + 1]) &&
									n + 1 == cbCardCount - 1)
								{
									for (int m = t_daWangIndex; m < n + 1; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
									}
									t_cbSortOriginalCards[n + 1] = t_tmpDaWangCard;
									break;
								}
							}

							//����С��
							for (int n = t_xiaoWangIndex + 1; n < cbCardCount - 1; ++n)
							{
								if (GetCardValue(t_xiaoWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n]))
								{
									break;
								}

								if (GetCardValue(t_xiaoWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n]) &&
									GetCardValue(t_xiaoWangReplaceCard) != GetCardValue(t_cbSortOriginalCards[n + 1]))
								{
									for (int m = t_xiaoWangIndex; m < n; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
									}
									t_cbSortOriginalCards[n] = t_tmpXiaoWangCard;
								}
								else if (GetCardValue(t_xiaoWangReplaceCard) == GetCardValue(t_cbSortOriginalCards[n + 1]) &&
									n + 1 == cbCardCount - 1)
								{
									for (int m = t_xiaoWangIndex; m < n + 1; ++m)
									{
										t_cbSortOriginalCards[m] = t_cbSortOriginalCards[m + 1];
									}
									t_cbSortOriginalCards[n + 1] = t_tmpXiaoWangCard;
								}
							}
						}

						memcpy(cbSortOriginalCards, t_cbSortOriginalCards, sizeof(t_cbSortOriginalCards));
					}
				}
			}
		}
		return t_maxType;
	}
}

//��ȡһ����ӵ����ͣ�cbSortCalcCards[] Ϊ������Ž����ͣ�
BYTE NNGameLogic::GetOneLaiZiCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE& bestLaiZiCard, BYTE cbSortCalcCards[])
{
	BYTE t_bestLaiZiCard = 0;
	BYTE t_cbSortOriginalCards[NIU_NIU_POKER_MAX_COUNT];

	//������ͨ�ƺ������
	BYTE t_nomalCards[4];
	int t_nomaluCardsCount = 0;
	BYTE t_laiZiCard = 0;

	int t_maxType = -1;
	BYTE t_maxCards[NIU_NIU_POKER_MAX_COUNT];

	int t_currType = 0;
	BYTE t_currCards[NIU_NIU_POKER_MAX_COUNT];

	for (int i = 0; i < cbCardCount; ++i)
	{
		if (cbCardData[i] == 0x4E || cbCardData[i] == 0x4F)
		{
			t_laiZiCard = cbCardData[i];
		}
		else
		{
			t_nomalCards[t_nomaluCardsCount++] = cbCardData[i];
		}
	}

	BYTE t_color = GetCardColor(t_nomalCards[0]);
	for (BYTE i = (t_color + 1); i < t_color + 14; ++i)
	{
		//����������
		/*if (i == t_nomalCards[0] || i == t_nomalCards[1] || i == t_nomalCards[2] || i == t_nomalCards[3])
		{
		continue;
		}*/

		memcpy(t_currCards, t_nomalCards, sizeof(t_nomalCards));
		t_currCards[4] = i;

		t_currType = CalcCardType(t_currCards, cbCardCount, t_cbSortOriginalCards);

		if (t_currType < t_maxType)
		{
			continue;
		}
		else if (t_currType > t_maxType ||
			(t_currType == t_maxType && t_maxType != -1 && CompareCard_Sample(t_currCards, t_maxCards, cbCardCount, t_currType, t_maxType)))
		{
			t_bestLaiZiCard = i;
			t_maxType = t_currType;
			memcpy(t_maxCards, t_currCards, sizeof(t_currCards));	
		}
	}

	bestLaiZiCard = t_bestLaiZiCard;
	if (cbSortCalcCards)
	{
		memcpy(cbSortCalcCards, t_maxCards, sizeof(t_maxCards));
	}
	return t_maxType;
}

//��������Ƶ�����
BYTE NNGameLogic::CalcCardType(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL)
	{
		return 0xFF;
	}

	//�洢ԭʼ���Ƹ���
	BYTE t_CardData[NIU_NIU_POKER_MAX_COUNT];
	memcpy(t_CardData, cbCardData, sizeof(t_CardData));

	//�洢�Ƶ��߼�ֵ(J\Q\K = 10)
	BYTE t_logicValue[NIU_NIU_POKER_MAX_COUNT];
	//��t_logicValue˳����ͬ����������
	BYTE t_sortCardByLogic[NIU_NIU_POKER_MAX_COUNT];

	//�洢��ֵ��A = 1��J = 11��Q = 12, K = 13��
	BYTE t_cardValue[NIU_NIU_POKER_MAX_COUNT];
	//��t_cardValue˳����ͬ������˳��
	BYTE t_sortCardByValue[NIU_NIU_POKER_MAX_COUNT];

	//�߼���ֵ�ĺ�
	BYTE bSum = 0;
	for (BYTE i = 0; i < cbCardCount; ++i)
	{
		t_logicValue[i] = GetCardLogicValue(t_CardData[i]);
		t_sortCardByLogic[i] = t_CardData[i];
		bSum += t_logicValue[i];

		t_cardValue[i] = GetCardValue(t_CardData[i]);
		t_sortCardByValue[i] = t_CardData[i];
	}
	SortCardByCardData(t_logicValue, cbCardCount, t_sortCardByLogic);
	SortCardByCardData(t_cardValue, cbCardCount, t_sortCardByValue);


	//˳�������ж�
	bool isPlusOne = TRUE;    //˳�����α�־��true:˳�ӣ�false������˳��
	BYTE bLast = cbCardCount - 1;
	for (BYTE i = 0; i < bLast; i++)
	{
		if (t_cardValue[i] - t_cardValue[i + 1] != 1)
		{
			isPlusOne = FALSE;
		}
	}
	if (t_cardValue[0] == 13 && t_cardValue[1] == 12 && t_cardValue[2] == 11 && t_cardValue[3] == 10 && t_cardValue[4] == 1)
	{
		isPlusOne = TRUE;

		//�����AKQJ10
		BYTE t_cards = t_cardValue[4];
		for (int n = 3; n >= 0; --n)
		{
			t_sortCardByValue[n + 1] = t_sortCardByValue[n];
		}
		t_sortCardByValue[0] = t_cards;
	}

	// һ��������
	//if (t_logicValue[0] == 5 && isPlusOne) return OX_DRAGON;

	//ͬ�������ж�
	bool isSameColor = true;   //ͬ�����α�־��true:ͬ����false������ͬ��
	BYTE firstCardColor = GetCardColor(cbCardData[0]);
	for (BYTE i = 1; i < cbCardCount; ++i)
	{
		if (GetCardColor(cbCardData[i]) != firstCardColor)
		{
			isSameColor = false;
			break;
		}
	}

	//ը�������жϡ���«������(ͳ��ÿ���Ƶ�����)
	//���ε������飬������
	int cardValueCount[14];      
	memset(cardValueCount, 0, sizeof(cardValueCount));
	//��������������,�����ж�ը���ͺ�«ţ,4��ը��ţ��3����«ţ
	int maxSameCount = 0;
	//���ض�������
	int twoSameCount = 0;

	for (BYTE i = 0; i < cbCardCount; ++i)
	{
		++cardValueCount[t_cardValue[i]];
	}

	for (BYTE i = 1; i < 14; ++i)
	{
		if (cardValueCount[i] > maxSameCount)
		{
			maxSameCount = cardValueCount[i];
		}
		if (2 == cardValueCount[i])
		{
			++twoSameCount;
		}
	}

	// ��Сţ�����ж�
	bool isWuXiaoNiu = FALSE;
	if (bSum <= 10)
	{
		isWuXiaoNiu = TRUE;
	}

	//�廨ţ�ж�
	bool isWuHuaNiu = true;
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		if (GetCardValue(cbCardData[i]) < 10)
		{
			isWuHuaNiu = false;
			break;
		}
	}

	//�����жϣ��������δ�С����

	//ͬ��˳��10����
	if (m_isAllowKuaiLe && (TRUE == isPlusOne && TRUE == isSameColor))
	{
		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		
		return OX_TONGHUASHUN;
	}

	//��Сţ��9���� 
	else if (m_isAllowWuHua && isWuXiaoNiu)
	{
		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		return OX_FIVESMALL;
	}

	//ը����8����
	else if (m_isAllowZhaDan && 4 <= maxSameCount)
	{
		if (t_cardValue[0] != t_cardValue[1])
		{
			BYTE t_singleCard = t_sortCardByValue[0];
			for (int n = 0; n < cbCardCount - 1; ++n)
			{
				t_sortCardByValue[n] = t_sortCardByValue[n + 1];
			}
			t_sortCardByValue[4] = t_singleCard;
		}

		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		
		return OX_FOUR_SAME;
	}

	//��«ţ��7����
	else if (m_isAllowHuLu && (3 == maxSameCount && 1 == twoSameCount))
	{
		if (t_cardValue[2] != t_cardValue[0])
		{
			BYTE t_towCard[2];
			t_towCard[0] = t_sortCardByValue[0];
			t_towCard[1] = t_sortCardByValue[1];
			for (int n = 0; n < cbCardCount - 2; ++n)
			{
				t_sortCardByValue[n] = t_sortCardByValue[n + 2];
			}
			t_sortCardByValue[3] = t_towCard[0];
			t_sortCardByValue[4] = t_towCard[1];
		}

		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		
		return OX_THREE_SAME;
	}

	//ͬ��ţ��6����
	else if (m_isAllowTongHua && (TRUE == isSameColor))
	{
		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		return OX_COLOR_SAME;
	}

	//�廨ţ��5����
	else if (m_isAllowWuHua && TRUE == isWuHuaNiu)
	{
		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		return OX_WUHUANIU;
	}

	//˳��ţ��5����
	else if (m_isAllowShunZi && (TRUE == isPlusOne))
	{
		if (cbSortOriginalCards != NULL)
		{
			memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
		}
		return OX_SHUNZINIU;
	}


	//�ж�ţţ������ͨ���Σ�����ţ�ڴ��жϣ�
	int t_oxNum = OX_VALUE0;
	for (int i = cbCardCount - 1; i >= 1; --i)
	{
		for (int j = i - 1; j >= 0; --j)
		{
			if ((bSum - t_logicValue[i] - t_logicValue[j]) % 10 == 0)
			{
				t_oxNum = ((t_logicValue[i] + t_logicValue[j]) > 10) ? (t_logicValue[i] + t_logicValue[j] - 10) : (t_logicValue[i] + t_logicValue[j]);

				//ţ������
				if (cbSortOriginalCards != NULL)
				{
					int t_oxCount = 0;
					for (int n = 0; n < cbCardCount; ++n)
					{
						if (n == j)
						{
							cbSortOriginalCards[3] = t_sortCardByLogic[j];
						}
						else if (n == i)
						{
							cbSortOriginalCards[4] = t_sortCardByLogic[i];
						}
						else
						{
							cbSortOriginalCards[t_oxCount++] = t_sortCardByLogic[n];
						}
					}
				}

				//����ţ��ţţ���ͣ��������� + һ�ŵ���
				if (t_oxNum == 10 && m_isAllowBiYi && twoSameCount == 2)
				{
					return OX_BIYINIU;
				}

				return t_oxNum;
			}
		}
	}
	/*
	for (BYTE i = 0; i<cbCardCount - 1; i++)
	{
		for (BYTE j = i + 1; j<cbCardCount; j++)
		{
			if ((bSum - t_logicValue[i] - t_logicValue[j]) % 10 == 0)
			{
				t_oxNum = ((t_logicValue[i] + t_logicValue[j]) > 10) ? (t_logicValue[i] + t_logicValue[j] - 10) : (t_logicValue[i] + t_logicValue[j]);

				//ţ������
				if (cbSortOriginalCards != NULL)
				{
					int t_oxCount = 0;
					for (int n = 0; n < cbCardCount; ++n)
					{
						if (n == i)
						{
							cbSortOriginalCards[3] = t_sortCardByLogic[i];
						}
						else if (n == j)
						{
							cbSortOriginalCards[4] = t_sortCardByLogic[j];
						}
						else
						{
							cbSortOriginalCards[t_oxCount++] = t_sortCardByLogic[n];
						}
					}
				}

				//����ţ��ţţ���ͣ��������� + һ�ŵ���
				if (t_oxNum == 10 && m_isAllowBiYi && twoSameCount == 2)
				{
					return OX_BIYINIU;
				}

				return t_oxNum;
			}
		}
	}
	*/
	if (cbSortOriginalCards != NULL)
	{
		memcpy(cbSortOriginalCards, t_sortCardByValue, sizeof(BYTE) * cbCardCount);
	}
	return OX_VALUE0;
}

//��ȡ����
BYTE NNGameLogic::GetTimes(BYTE cbCardData[], BYTE cbCardCount)
{
	if (cbCardCount != NIU_NIU_POKER_MAX_COUNT)
	{
		return 0;
	}

	//����ţţ����
	BYTE bTimes = GetCardType(cbCardData, NIU_NIU_POKER_MAX_COUNT);

	//1��ţţx3  ţ��x2  ţ��x2  ţ��-��ţx1
	if (m_niuNiuTimes == 1)
	{
		return GetTimes_x3(cbCardData, cbCardCount, bTimes);
	}

	//2��ţţx4  ţ��x3  ţ��x2  ţ��x2  ţ�� - ��ţx1
	else if (m_niuNiuTimes == 2)
	{
		return GetTimes_x4(cbCardData, cbCardCount, bTimes);
	}

	return 0;
}

//1��ţţx3  ţ��x2  ţ��x2  ţ��-��ţx1
BYTE NNGameLogic::GetTimes_x3(BYTE cbCardData[], BYTE cbCardCount, BYTE bTimes)
{
	// ��ţ - ţ��: 1��
	if (bTimes <= 7)return 1;
	// ţ�� - ţ��: 2��
	else if (bTimes >= 8 && bTimes <= 9) return 2;
	// ţţ: 3��
	else if (bTimes == 10)return 3;
	//����ţ��4��
	else if (bTimes == OX_BIYINIU) return 4;
	//˳��ţ��5��
	else if (bTimes == OX_SHUNZINIU) return 5;
	//�廨ţ 5��
	else if (bTimes == OX_WUHUANIU) return 5;
	//ͬ��ţ��6��
	else if (bTimes == OX_COLOR_SAME) return 6;
	//��«ţ��7��
	else if (bTimes == OX_THREE_SAME) return 7;
	//ը��ţ��8��
	else if (bTimes == OX_FOUR_SAME)return 8;
	//��Сţ��9��
	else if (bTimes == OX_FIVESMALL) return 9;
	//ͬ��˳ţ��10��
	else if (bTimes == OX_TONGHUASHUN)return 10;

	return 0;
}

//2��ţţx4  ţ��x3  ţ��x2  ţ��x2  ţ�� - ��ţx1
BYTE NNGameLogic::GetTimes_x4(BYTE cbCardData[], BYTE cbCardCount, BYTE bTimes)
{
	// ��ţ - ţ��: 1��
	if (bTimes <= 6)return 1;
	// ţ�� - ţ��: 2��
	else if (bTimes >= 7 && bTimes <= 8) return 2;
	// ţ�ţ�3��
	else if (bTimes == 9) return 3;
	// ţţ: 4��
	else if (bTimes == 10) return 4;
	//����ţ��4��
	else if (bTimes == OX_BIYINIU) return 4;
	//˳��ţ��5��
	else if (bTimes == OX_SHUNZINIU) return 5;
	//�廨ţ 5��
	else if (bTimes == OX_WUHUANIU) return 5;
	//ͬ��ţ��6��
	else if (bTimes == OX_COLOR_SAME) return 6;
	//��«ţ��7��
	else if (bTimes == OX_THREE_SAME) return 7;
	//ը��ţ��8��
	else if (bTimes == OX_FOUR_SAME) return 8;
	//��Сţ��9��
	else if (bTimes == OX_FIVESMALL) return 9;
	//ͬ��˳ţ��10��
	else if (bTimes == OX_TONGHUASHUN)return 10;

	return 0;
}

//��ȡţţ
bool NNGameLogic::GetOxCard(BYTE cbCardData[], BYTE cbCardCount)
{
	//���ñ���
	BYTE bTemp[NIU_NIU_POKER_MAX_COUNT], bTempData[NIU_NIU_POKER_MAX_COUNT];
	CopyMemory(bTempData, cbCardData, sizeof(bTempData));
	BYTE bSum = 0;
	for (BYTE i = 0; i<cbCardCount; i++)
	{
		bTemp[i] = GetCardLogicValue(cbCardData[i]);
		bSum += bTemp[i];
	}

	//����ţţ
	for (BYTE i = 0; i<cbCardCount - 1; i++)
	{
		for (BYTE j = i + 1; j<cbCardCount; j++)
		{
			if ((bSum - bTemp[i] - bTemp[j]) % 10 == 0)
			{
				BYTE bCount = 0;
				for (BYTE k = 0; k<cbCardCount; k++)
				{
					if (k != i && k != j)
					{
						cbCardData[bCount++] = bTempData[k];
					}
				}

				cbCardData[bCount++] = bTempData[i];
				cbCardData[bCount++] = bTempData[j];

				return true;
			}
		}
	}

	return false;
}

//��ȡ����
bool NNGameLogic::IsIntValue(BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE sum = 0;
	for (BYTE i = 0; i<cbCardCount; i++)
	{
		sum += GetCardLogicValue(cbCardData[i]);
	}

	return (sum % 10 == 0);
}

//�ж�4���Ƶ�ţţ��
int NNGameLogic::GetCardsOxNumber(const BYTE handCards[], int handCardsCount)
{
	if (handCards == NULL || handCardsCount < 4)
	{
		return -1;
	}

	BYTE bTemp[NIU_NIU_POKER_MAX_COUNT];
	BYTE bSum = 0;
	for (BYTE i = 0; i< handCardsCount; i++)
	{
		bTemp[i] = GetCardLogicValue(handCards[i]);
		bSum += bTemp[i];
	}
	SortCardList(bTemp, handCardsCount);


	int t_oxNum = OX_VALUE0;
	for (BYTE i = 0; i < handCardsCount - 1; i++)
	{
		for (BYTE j = i + 1; j < handCardsCount; j++)
		{
			if ((bSum - bTemp[i] - bTemp[j]) % 10 == 0)
			{
				t_oxNum = ((bTemp[i] + bTemp[j]) > 10) ? (bTemp[i] + bTemp[j] - 10) : (bTemp[i] + bTemp[j]);
				return t_oxNum;
			}
		}
	}
	return t_oxNum;
}

//���˿��߼�ֵ�����˿�
void NNGameLogic::SortCardByLogicValue(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL || cbCardCount < 2)
	{
		return;
	}
	//�洢�Ƶĵ���
	BYTE cbLogicValue[NIU_NIU_POKER_MAX_COUNT];
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		cbLogicValue[i] = GetCardLogicValue(cbCardData[i]);
	}

	//�������
	bool bSorted = true;
	BYTE cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i < bLast; i++)
		{
			if ((cbLogicValue[i] < cbLogicValue[i + 1]) ||
				((cbLogicValue[i] == cbLogicValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1])))
			{
				//����λ��
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbTempData;
				cbTempData = cbLogicValue[i];
				cbLogicValue[i] = cbLogicValue[i + 1];
				cbLogicValue[i + 1] = cbTempData;

				if (cbSortOriginalCards != NULL)
				{
					cbTempData = cbSortOriginalCards[i];
					cbSortOriginalCards[i] = cbSortOriginalCards[i + 1];
					cbSortOriginalCards[i + 1] = cbTempData;
				}
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//���˿���ֵ�����˿ˣ�J=11  Q=12  K=13��
void NNGameLogic::SortCardByCardValue(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL || cbCardCount < 2)
	{
		return;
	}
	//�洢�Ƶĵ���
	BYTE cbCardValue[NIU_NIU_POKER_MAX_COUNT];
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		cbCardValue[i] = GetCardValue(cbCardData[i]);
	}

	//�������
	bool bSorted = true;
	BYTE cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i < bLast; i++)
		{
			if ((cbCardValue[i] < cbCardValue[i + 1]) ||
				((cbCardValue[i] == cbCardValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1])))
			{
				//����λ��
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbTempData;
				cbTempData = cbCardValue[i];
				cbCardValue[i] = cbCardValue[i + 1];
				cbCardValue[i + 1] = cbTempData;

				if (cbSortOriginalCards != NULL)
				{
					cbTempData = cbSortOriginalCards[i];
					cbSortOriginalCards[i] = cbSortOriginalCards[i + 1];
					cbSortOriginalCards[i + 1] = cbTempData;
				}
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//����cbCardDataֵ�����˿�
void NNGameLogic::SortCardByCardData(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL || cbCardCount < 2)
	{
		return;
	}

	//�������
	bool bSorted = true;
	BYTE cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i < bLast; i++)
		{
			if (cbCardData[i] < cbCardData[i + 1] ||
				(cbCardData[i] == cbCardData[i + 1] && cbSortOriginalCards && GetCardValue(cbSortOriginalCards[i]) < GetCardValue(cbSortOriginalCards[i + 1])) ||
				(cbCardData[i] == cbCardData[i + 1] && cbSortOriginalCards && GetCardValue(cbSortOriginalCards[i]) == GetCardValue(cbSortOriginalCards[i + 1]) && cbSortOriginalCards[i] < cbSortOriginalCards[i + 1]))
			{
				//����λ��
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbTempData;

				if (cbSortOriginalCards != NULL)
				{
					cbTempData = cbSortOriginalCards[i];
					cbSortOriginalCards[i] = cbSortOriginalCards[i + 1];
					cbSortOriginalCards[i + 1] = cbTempData;
				}
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//�����˿�
void NNGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount, BYTE cbSortOriginalCards[])
{
	if (cbCardData == NULL || cbCardCount < 2)
	{
		return;
	}

	//�洢�Ƶĵ���
	BYTE cbCardValue[NIU_NIU_POKER_MAX_COUNT];
	for (BYTE i = 0; i < cbCardCount; i++)
	{
		cbCardValue[i] = GetCardValue(cbCardData[i]);
	}

	//�������
	bool bSorted = true;
	BYTE cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i < bLast; i++)
		{
			if ((cbCardValue[i] < cbCardValue[i + 1]) ||
				((cbCardValue[i] == cbCardValue[i + 1]) && (cbCardData[i] < cbCardData[i + 1])))
			{
				//����λ��
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbTempData;
				cbTempData = cbCardValue[i];
				cbCardValue[i] = cbCardValue[i + 1];
				cbCardValue[i + 1] = cbTempData;

				if (cbSortOriginalCards != NULL)
				{
					cbTempData = cbSortOriginalCards[i];
					cbSortOriginalCards[i] = cbSortOriginalCards[i + 1];
					cbSortOriginalCards[i + 1] = cbTempData;
				}
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//ϴ��
void NNGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	if (m_isAllowLaiZi || cbBufferCount == 54)
	{
		RandCardList_laizi(cbCardBuffer, cbBufferCount);
	}
	else
	{
		RandCardList_normal(cbCardBuffer, cbBufferCount);
	}
}

//ϴ��-������С��
void NNGameLogic::RandCardList_normal(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardListData)];
	CopyMemory(cbCardData, m_cbCardListData, sizeof(m_cbCardListData));

	//�����˿�
	BYTE bRandCount = 0, bPosition = 0;
	do
	{
		bPosition = rand() % (CountArray(m_cbCardListData) - bRandCount);
		cbCardBuffer[bRandCount++] = cbCardData[bPosition];
		cbCardData[bPosition] = cbCardData[CountArray(m_cbCardListData) - bRandCount];
	} while (bRandCount < cbBufferCount);

	return;
}

//ϴ��-����С��
void NNGameLogic::RandCardList_laizi(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//����׼��
	BYTE cbCardData[CountArray(m_cbCardListData_LaiZi)];
	CopyMemory(cbCardData, m_cbCardListData_LaiZi, sizeof(m_cbCardListData_LaiZi));

	//�����˿�
	BYTE bRandCount = 0, bPosition = 0;
	do
	{
		bPosition = rand() % (CountArray(m_cbCardListData_LaiZi) - bRandCount);
		cbCardBuffer[bRandCount++] = cbCardData[bPosition];
		cbCardData[bPosition] = cbCardData[CountArray(m_cbCardListData_LaiZi) - bRandCount];
	} while (bRandCount < cbBufferCount);

	return;
}

//�߼���ֵ
BYTE NNGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE bCardColor = GetCardColor(cbCardData);
	BYTE bCardValue = GetCardValue(cbCardData);

	//ת����ֵ
	return (bCardValue > 10) ? (10) : bCardValue;
}

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
bool NNGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BYTE XianOX, BYTE ZhuangOX)
{
	//�м���ţ�����м���
	/*if (XianOX == 0)
	{
		return false;
	}*/

	//����ţţ���Ͳ���ȣ�ֱ�ӿ��ԱȽϳ���С
	if (XianOX != ZhuangOX)
	{
		return (XianOX > ZhuangOX);
	}

	//�������ҵ����Ƶ������
	BYTE t_XianLaiZiCount = GetHandCardsLaoZiCount(cbFirstData, cbCardCount);
	BYTE t_ZhuangLaiZiCount = GetHandCardsLaoZiCount(cbNextData, cbCardCount);

	//��ͬ���ͣ�˭�����˭��
	if (XianOX == ZhuangOX && (t_XianLaiZiCount > 0 || t_ZhuangLaiZiCount > 0))
	{
		//���������ȣ�����ٵ���
		if (t_XianLaiZiCount != t_ZhuangLaiZiCount)
		{
			return t_XianLaiZiCount < t_ZhuangLaiZiCount;
		}

		//���������ͬ��ֻ����ÿ����һ�����
		BYTE t_xianLaiZi = 0, t_zhuangLaiZi = 0;			//����ƣ�������С����
		BYTE t_xianBestLaiZi = 0, t_zhuangBestLaiZi = 0;	//����滻��������ֵ�����Ǵ�С����
		int t_xianLaiZiIndex = 0, t_zhuangLaiZiIndex = 0;	//����ھ����������������е�λ�ã�0-4��
		BYTE t_xianBestCards[NIU_NIU_POKER_MAX_COUNT], t_zhuangBestCards[NIU_NIU_POKER_MAX_COUNT];  //��������
		memset(t_xianBestCards, 0, sizeof(t_xianBestCards));
		memset(t_zhuangBestCards, 0, sizeof(t_zhuangBestCards));

		//��������������ͣ��Լ��������ֵ
		GetOneLaiZiCardType(cbFirstData, cbCardCount, t_xianBestLaiZi, t_xianBestCards);
		GetOneLaiZiCardType(cbNextData, cbCardCount, t_zhuangBestLaiZi, t_zhuangBestCards);

		//��������������͵�����
		SortCardList(t_xianBestCards, cbCardCount);
		SortCardList(t_zhuangBestCards, cbCardCount);

		//�������������ֵ
		BYTE cbFirstMaxValue = GetCardValue(t_xianBestCards[0]);
		BYTE cbNextMaxValue = GetCardValue(t_zhuangBestCards[0]);

		//������ӵ�λ�ã��Լ�ׯ�� �мҵ������
		for (int i = 0; i < cbCardCount; ++i)
		{
			if (t_xianBestCards[i] == t_xianBestLaiZi)
			{
				t_xianLaiZiIndex = i;
			}
			if (t_zhuangBestCards[i] == t_zhuangBestLaiZi)
			{
				t_zhuangLaiZiIndex = i;
			}

			if (cbFirstData[i] == 0x4F || cbFirstData[i] == 0x4E)
			{
				t_xianLaiZi = cbFirstData[i];
			}
			if (cbNextData[i] == 0x4F || cbNextData[i] == 0x4E)
			{
				t_zhuangLaiZi = cbNextData[i];
			}
		}

		//�������λ�ò���ͬ
		if (t_zhuangLaiZiIndex != t_xianLaiZiIndex)
		{
			//�������ֵ��ֵ��ͬ
			if (cbFirstMaxValue != cbNextMaxValue)
			{
				return cbFirstMaxValue > cbNextMaxValue;
			}

			//��ֵ��ȣ���һ����һλ�������
			if (t_zhuangLaiZiIndex == 0 || t_xianLaiZiIndex == 0)
			{
				//ׯ�����Ƶ�һ������ӣ��м�Ӯ
				if (t_zhuangLaiZiIndex == 0) return true;
				else return false;
			}

			//�Ƚ���ɫ
			return GetCardColor(t_xianBestCards[0]) > GetCardColor(t_zhuangBestCards[0]);
		}

		//���������λ����ͬ
		//������Ӷ��ǵ�һ����
		if (t_zhuangLaiZiIndex == 0 && t_xianLaiZiIndex == 0)
		{
			if (t_xianLaiZi == 0x4F) return true;
			else return false;
		}
		
		//������Ӷ����ǵ�һ����
		if (cbFirstMaxValue != cbNextMaxValue)
		{
			return cbFirstMaxValue > cbNextMaxValue;
		}

		//�Ƚ���ɫ
		return GetCardColor(t_xianBestCards[0]) > GetCardColor(t_zhuangBestCards[0]);
	}

	//������ͬ������ӣ������ж�
	//������ֵ���ٻ�ɫ�������С
	BYTE bFirstTemp[NIU_NIU_POKER_MAX_COUNT], bNextTemp[NIU_NIU_POKER_MAX_COUNT];
	CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
	CopyMemory(bNextTemp, cbNextData, cbCardCount);
	SortCardList(bFirstTemp, cbCardCount);
	SortCardList(bNextTemp, cbCardCount);

	//ը������«���ͱȽ�
	if (ZhuangOX == OX_FOUR_SAME || ZhuangOX == OX_THREE_SAME)
	{
		return GetCardValue(bFirstTemp[NIU_NIU_POKER_MAX_COUNT / 2]) > GetCardValue(bNextTemp[NIU_NIU_POKER_MAX_COUNT / 2]);
	}

	//����ţ���ͱȽ�
	if (ZhuangOX == OX_BIYINIU)
	{
		//�ҳ����Ķ���
		int t_xianCardCountByValue[14];
		int t_zhuangCardCountByValue[14];
		int t_xianMaxCardValue = 0;
		int t_zhuangMaxCardValue = 0;
		int t_xianMaxCard = 0;
		int t_zhuangMaxCard = 0;
		memset(t_xianCardCountByValue, 0, sizeof(t_xianCardCountByValue));
		memset(t_zhuangCardCountByValue, 0, sizeof(t_zhuangCardCountByValue));
		for (size_t i = 0; i < NIU_NIU_POKER_MAX_COUNT; ++i)
		{
			++t_xianCardCountByValue[GetCardValue(bFirstTemp[i])];
			++t_zhuangCardCountByValue[GetCardValue(bNextTemp[i])];
		}

		for (size_t i = 0; i < 14; ++i)
		{
			if (t_xianCardCountByValue[i] == 2 && GetCardLogicValue(i) > t_xianMaxCardValue)
			{
				t_xianMaxCardValue = i;
			}
			if (t_zhuangCardCountByValue[i] == 2 && GetCardLogicValue(i) > t_zhuangMaxCardValue)
			{
				t_zhuangMaxCardValue = i;
			}
		}
		for (size_t i = 0; i < NIU_NIU_POKER_MAX_COUNT; ++i)
		{
			if (t_xianMaxCardValue == GetCardValue(bFirstTemp[i]) && bFirstTemp[i] > t_xianMaxCard)
			{
				t_xianMaxCard = bFirstTemp[i];
			}
			if (t_zhuangMaxCardValue == GetCardValue(bNextTemp[i]) && bNextTemp[i] > t_zhuangMaxCard)
			{
				t_zhuangMaxCard = bNextTemp[i];
			}
		}


		//�Ƚ���ֵ
		BYTE cbNextMaxValue = GetCardValue(t_zhuangMaxCard);
		BYTE cbFirstMaxValue = GetCardValue(t_xianMaxCard);
		if (cbNextMaxValue != cbFirstMaxValue)
		{
			return cbFirstMaxValue > cbNextMaxValue;
		}

		//�Ƚ���ɫ
		return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);

		return false;
	}

	//˳�ӡ�ͬ��˳���ͱȽ�
	if (ZhuangOX == OX_TONGHUASHUN || ZhuangOX == OX_SHUNZINIU)
	{
		BYTE bShunFirstTemp[NIU_NIU_POKER_MAX_COUNT], bShunNextTemp[NIU_NIU_POKER_MAX_COUNT];
		for (int i = 0; i < cbCardCount; ++i)
		{
			bShunFirstTemp[i] = GetCardValue(cbFirstData[i]) == 1 ? cbFirstData[i] + 13 : cbFirstData[i];
			bShunNextTemp[i] = GetCardValue(cbNextData[i]) == 1 ? cbNextData[i] + 13 : cbNextData[i];
		}
		SortCardList(bShunFirstTemp, cbCardCount);
		SortCardList(bShunNextTemp, cbCardCount);

		if (GetCardValue(bShunFirstTemp[0]) == 14 && (GetCardValue(bShunFirstTemp[0]) - GetCardValue(bShunFirstTemp[1]) == 1))
		{
			CopyMemory(bFirstTemp, bShunFirstTemp, cbCardCount);
		}
		if (GetCardValue(bShunNextTemp[0]) == 14 && (GetCardValue(bShunNextTemp[0]) - GetCardValue(bShunNextTemp[1]) == 1))
		{
			CopyMemory(bNextTemp, bShunNextTemp, cbCardCount);
		}
	}

	//�Ƚ���ֵ
	BYTE cbNextMaxValue = GetCardValue(bNextTemp[0]);
	BYTE cbFirstMaxValue = GetCardValue(bFirstTemp[0]);
	if (cbNextMaxValue != cbFirstMaxValue)
	{
		return cbFirstMaxValue > cbNextMaxValue;
	}

	//�Ƚ���ɫ
	return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);

	return false;
}

//������Ž�ʹ�õıȽϺ���
bool NNGameLogic::CompareCard_Sample(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BYTE XianOX, BYTE ZhuangOX)
{
	//�м���ţ�����м���
	/*if (XianOX == 0)
	{
	return false;
	}*/

	//����ţţ���Ͳ���ȣ�ֱ�ӿ��ԱȽϳ���С
	if (XianOX != ZhuangOX)
	{
		return (XianOX > ZhuangOX);
	}

	//������ͬ�������ж�
	//������ֵ���ٻ�ɫ�������С
	BYTE bFirstTemp[NIU_NIU_POKER_MAX_COUNT], bNextTemp[NIU_NIU_POKER_MAX_COUNT];
	CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
	CopyMemory(bNextTemp, cbNextData, cbCardCount);
	SortCardList(bFirstTemp, cbCardCount);
	SortCardList(bNextTemp, cbCardCount);

	//ը������«���ͱȽ�
	if (ZhuangOX == OX_FOUR_SAME || ZhuangOX == OX_THREE_SAME)
	{
		return GetCardValue(bFirstTemp[NIU_NIU_POKER_MAX_COUNT / 2]) > GetCardValue(bNextTemp[NIU_NIU_POKER_MAX_COUNT / 2]);
	}

	//����ţ���ͱȽ�
	if (ZhuangOX == OX_BIYINIU)
	{
		//�ҳ����Ķ���
		int t_xianCardCountByValue[14];
		int t_zhuangCardCountByValue[14];
		int t_xianMaxCardValue = 0;
		int t_zhuangMaxCardValue = 0;
		int t_xianMaxCard = 0;
		int t_zhuangMaxCard = 0;
		memset(t_xianCardCountByValue, 0, sizeof(t_xianCardCountByValue));
		memset(t_zhuangCardCountByValue, 0, sizeof(t_zhuangCardCountByValue));
		for (size_t i = 0; i < NIU_NIU_POKER_MAX_COUNT; ++i)
		{
			++t_xianCardCountByValue[GetCardValue(bFirstTemp[i])];
			++t_zhuangCardCountByValue[GetCardValue(bNextTemp[i])];
		}

		for (size_t i = 0; i < 14; ++i)
		{
			if (t_xianCardCountByValue[i] == 2 && GetCardLogicValue(i) > t_xianMaxCardValue)
			{
				t_xianMaxCardValue = i;
			}
			if (t_zhuangCardCountByValue[i] == 2 && GetCardLogicValue(i) > t_zhuangMaxCardValue)
			{
				t_zhuangMaxCardValue = i;
			}
		}
		for (size_t i = 0; i < NIU_NIU_POKER_MAX_COUNT; ++i)
		{
			if (t_xianMaxCardValue == GetCardValue(bFirstTemp[i]) && bFirstTemp[i] > t_xianMaxCard)
			{
				t_xianMaxCard = bFirstTemp[i];
			}
			if (t_zhuangMaxCardValue == GetCardValue(bNextTemp[i]) && bNextTemp[i] > t_zhuangMaxCard)
			{
				t_zhuangMaxCard = bNextTemp[i];
			}
		}


		//�Ƚ���ֵ
		BYTE cbNextMaxValue = GetCardValue(t_zhuangMaxCard);
		BYTE cbFirstMaxValue = GetCardValue(t_xianMaxCard);
		if (cbNextMaxValue != cbFirstMaxValue)
		{
			return cbFirstMaxValue > cbNextMaxValue;
		}

		//�Ƚ���ɫ
		return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);

		return false;
	}

	//˳�ӡ�ͬ��˳���ͱȽ�
	if (ZhuangOX == OX_TONGHUASHUN || ZhuangOX == OX_SHUNZINIU)
	{
		BYTE bShunFirstTemp[NIU_NIU_POKER_MAX_COUNT], bShunNextTemp[NIU_NIU_POKER_MAX_COUNT];
		for (int i = 0; i < cbCardCount; ++i)
		{
			bShunFirstTemp[i] = GetCardValue(cbFirstData[i]) == 1 ? cbFirstData[i] + 13 : cbFirstData[i];
			bShunNextTemp[i] = GetCardValue(cbNextData[i]) == 1 ? cbNextData[i] + 13 : cbNextData[i];
		}
		SortCardList(bShunFirstTemp, cbCardCount);
		SortCardList(bShunNextTemp, cbCardCount);

		if (GetCardValue(bShunFirstTemp[0]) == 14 && (GetCardValue(bShunFirstTemp[0]) - GetCardValue(bShunFirstTemp[1]) == 1))
		{
			CopyMemory(bFirstTemp, bShunFirstTemp, cbCardCount);
		}
		if (GetCardValue(bShunNextTemp[0]) == 14 && (GetCardValue(bShunNextTemp[0]) - GetCardValue(bShunNextTemp[1]) == 1))
		{
			CopyMemory(bNextTemp, bShunNextTemp, cbCardCount);
		}
	}

	//�Ƚ���ֵ
	BYTE cbNextMaxValue = GetCardValue(bNextTemp[0]);
	BYTE cbFirstMaxValue = GetCardValue(bFirstTemp[0]);
	if (cbNextMaxValue != cbFirstMaxValue)
	{
		return cbFirstMaxValue > cbNextMaxValue;
	}

	//�Ƚ���ɫ
	return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);

	return false;
}

//////////////////////////////////////////////////////////////////////////
