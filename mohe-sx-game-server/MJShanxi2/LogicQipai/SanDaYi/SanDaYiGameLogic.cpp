#include "SanDaYiGameLogic.h"
#include "LLog.h"


//�˿�����
const int SDYGameLogic::m_allCardsData[SANDAYI_CELL_PACK] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� A - K
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� A - K
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� A - K
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D,	//���� A - K
	0x4E,0x4F,
};

/* ���캯�� */
SDYGameLogic::SDYGameLogic()
{
	//Ĭ�ϣ�һ���˿�
	m_packCount = SANDAYI_PACK_COUNT;

	//Ĭ�ϣ���������Ϊ4��
	m_baseCardsCount = SANDAYI_BASE_CARDS_COUNT;

	//Ĭ�ϣ���������Ϊ10��
	m_sendCardsCount = SANDAYI_SEND_CARD_COUNT;

	//Ĭ�ϣ��޳���
	m_commMainValue = VALUE_ERROR;

	//Ĭ�ϣ�������ɫ
	m_mainColor = COLOR_ERROR;
}

/* �������� */
SDYGameLogic::~SDYGameLogic()
{

}


/* �����Ƹ��� */
bool SDYGameLogic::SetPackCount(int packCount)
{
	if (packCount <= 0 || packCount > SANDAYI_PACK_COUNT)
	{
		LLOG_ERROR("SDYGameLogic::SetPackCount() Error... Set Pack Count Is Not Valid...");
		return false;
	}

	//�����Ƹ���
	m_packCount = packCount;

	return true;
}

/* ��ȡ�Ƹ��� */
int SDYGameLogic::GetPackCount()
{
	return m_packCount;
}

/* ���õ������� */
bool SDYGameLogic::SetBaseCardsCount(int baseCardsCount)
{
	if (baseCardsCount <= 0 || baseCardsCount > SANDAYI_BASE_CARDS_COUNT)
	{
		LLOG_ERROR("SDYGameLogic::SetBaseCardsCount() Error... Set Base Cards Count Is Not Valid...");
		return false;
	}

	//���õ�������
	m_baseCardsCount = baseCardsCount;
}

/* ��ȡ�������� */
int SDYGameLogic::GetBaseCardsCount()
{
	return m_baseCardsCount;
}

/* ���ó�����ֵ */
bool SDYGameLogic::SetCommMainValue(int commMainValue)
{
	if (false)
	{
		LLOG_ERROR("SDYGameLogic::SetCommMainValue() Error... Set Common Cards Is Not Valid...");
		return false;
	}

	//���ó���
	m_commMainValue = commMainValue;

	return true;
}

/* ��ȡ������ֵ */
int SDYGameLogic::GetCommMainValue()
{
	return m_commMainValue;
}

/* ���÷��������� */
bool SDYGameLogic::SetSendCardsCount(int sendCardsCount)
{
	if (sendCardsCount <= 0)
	{
		LLOG_ERROR("SDYGameLogic::SetSendCardsCount() Error... Set Send Cards Count Is Not Valid...");
		return false;
	}

	m_sendCardsCount = sendCardsCount;

	return true;
}

/* ��ȡ���������� */
int SDYGameLogic::GetSendCardsCount()
{
	return m_sendCardsCount;
}

/* ��������ɫ */
bool SDYGameLogic::SetMainColor(int mainColor)
{
	if (mainColor < COLOR_FANG_KUAI || mainColor > COLOR_HEI_TAO)
	{
		LLOG_ERROR("SDYGameLogic::SetMainColor() Error... Set Main Color Is Not Valid...");
		return false;
	}

	m_mainColor = mainColor;

	//���»�ɫȨλ
	UpDataSortRight();

	return true;
}

/* ��ȡ����ɫ */
int SDYGameLogic::GetMainColor()
{
	return m_mainColor;
}

/* ��ȡ������ֵ */
int SDYGameLogic::GetCardValue(int cardData)
{
	return cardData & LOGIC_MASK_VALUE;
}

/* ��ȡ���滨ɫ */
int SDYGameLogic::GetCardColor(int cardData)
{
	return cardData & LOGIC_MASK_COLOR;
}

/* ��Ч��ֵ�ж� */
bool SDYGameLogic::IsValidCard(int cardData)
{
	//��ȡ����
	int cardColor = GetCardColor(cardData);
	int cardValue = GetCardValue(cardData);

	//��С����Ч
	if ((cardData == 0x4E) || (cardData == 0x4F))
	{
		return true;
	}

	//��ͨ����Ч
	if ((cardColor <= 0x30) && (cardColor >= 0x00) && (cardValue >= 0x01) && (cardValue <= 0x0D))
	{
		return true;
	}

	return false;
}

/* �ж��Ƿ�Ϊ��÷��ڵĻ�ɫ */
bool SDYGameLogic::IsValidColor(int cardColor)
{
	int t_cardColor = GetCardColor(cardColor);

	if (t_cardColor == COLOR_FANG_KUAI ||
		t_cardColor == COLOR_MEI_HUA ||
		t_cardColor == COLOR_HONG_TAO ||
		t_cardColor == COLOR_HEI_TAO)
	{
		return true;
	}
	return false;
}

/* ��ȡ�Ƶ�����ֵ */
int SDYGameLogic::GetCardSortValue(int cardData)
{
	if (!IsValidCard(cardData)) return 0;

	//�߼���ֵ
	int t_cardColor = GetCardColor(cardData);
	int t_cardValue = GetCardValue(cardData);

	//����
	if ((cardData == 0x4E) || (cardData == 0x4F))
	{
		int t_sortValue = t_cardValue + 11;
		return t_sortValue + m_sortRight[4];
	}

	//2����
	if (t_cardValue == m_commMainValue)
	{
		int t_sortValue = (t_cardColor >> 4) + 15;
		if (t_cardColor == m_mainColor)
			t_sortValue = 19;
		return t_sortValue + m_sortRight[4];
	}

	//��ɫ�任
	int t_sortColor = t_cardColor;
	if (t_cardColor == m_mainColor) t_sortColor = 0x40;

	//A
	int t_sortValue = t_cardValue;
	if (t_cardValue == 1) t_sortValue += 13;

	return t_sortValue + m_sortRight[t_sortColor >> 4];
}

/* ��ȡ�Ƶ��߼�ֵ */
int SDYGameLogic::GetCardLogicValue(int cardData)
{
	if (!IsValidCard(cardData)) return 0;

	//�߼���ֵ
	int t_cardColor = GetCardColor(cardData);
	int t_cardValue = GetCardValue(cardData);

	//���ó���Ȩλ����2����2ռ������λ�ã�
	int t_commMainRight = (m_commMainValue == VALUE_ERROR) ? 0 : 4;

	//С�����ˣ�(2������С��20)��(2���ǳ�����С��16)
	if (cardData == 0x4E)
	{
		int t_logicValue = t_cardValue + 1 + t_commMainRight;
		return t_logicValue + m_sortRight[4];
	}

	//�������ˣ�(2����������22)��(2���ǳ���������18)
	if (cardData == 0x4F)
	{
		BYTE t_logicValue = t_cardValue + 2 + t_commMainRight;
		return t_logicValue + m_sortRight[4];
	}

	//2�������� ��2��18  ��2��16
	if (t_cardValue == m_commMainValue)
	{
		int t_logicValue = (t_cardColor == m_mainColor) ? 18 : 16;
		return t_logicValue + m_sortRight[4];
	}

	//����ɫ�任
	int t_logicColor = t_cardColor;
	if (t_cardColor == m_mainColor) t_logicColor = 0x40;

	//Aת���任Ϊ14
	int t_logicValue = t_cardValue;
	if (t_cardValue == 1) t_logicValue += 13;

	return t_logicValue + m_sortRight[t_logicColor >> 4];
}

/* ��ȡ�Ƶ��߼���ɫ */
int SDYGameLogic::GetCardLogicColor(int cardData)
{
	//�߼���ֵ
	BYTE cardColor = GetCardColor(cardData);
	BYTE cardValue = GetCardValue(cardData);

	//���������ɫ
	if (cardColor == m_mainColor)
		return COLOR_NT;

	//����ǳ�������Ϊ����ɫ
	if (cardValue == m_commMainValue)
		return COLOR_NT;

	//����Ǵ�С������Ϊ����ɫ
	if (cardColor == 0x40)
		return COLOR_NT;

	return cardColor;
}

/* �Ƿ�ͬ�� */
bool SDYGameLogic::IsSameColor(int firstCard, int secondCard)
{
	//��ȡ��ɫ
	BYTE firstLogicColor = GetCardLogicColor(firstCard);
	BYTE secondLogicColor = GetCardLogicColor(secondCard);

	//ͬ���ж�
	return (firstLogicColor == secondLogicColor);
}

/* �ж�ѡ�Լҵ����Ƿ����Ҫ�� */
bool SDYGameLogic::IsValidFirendCard(int selectCard, int selectScore)
{
	Lint t_color = GetCardColor(selectCard);
	Lint t_value = GetCardValue(selectCard);

	//ׯ�ҽ��Ʒ�100�֣���ѡ��С�������е�A��K��Q��J
	if (selectScore == 100)
	{
		if (selectCard == 0x4E || selectCard == 0x4F || t_value == 1 || (t_value >= 10 && t_value <= 13))
		{
			return true;
		}
		return false;
	}
	else
	{
		if (selectCard == 0x4E || selectCard == 0x4F)
			return false;
		if (t_color == m_mainColor)
			return false;
		if (t_value > 1 && t_value < 10)
			return false;
		return true;
	}
}


/* �����˿� */
void SDYGameLogic::SortCardList(int cardsData[], int cardsCount)
{
	if (cardsCount < 2)
		return;

	//���˿���ֵת��Ϊ����ֵ
	int cbSortValue[SANDAYI_HAND_CARDS_COUNT_MAX];
	for (int i = 0; i < cardsCount; i++)
	{
		cbSortValue[i] = GetCardSortValue(cardsData[i]);
	}

	//�������
	bool bSorted = true;
	int bTempData;
	int bLast = cardsCount - 1;
	do
	{
		bSorted = true;
		for (int i = 0; i < bLast; i++)
		{
			if ((cbSortValue[i] < cbSortValue[i + 1]) ||
				((cbSortValue[i] == cbSortValue[i + 1]) && (cardsData[i] < cardsData[i + 1])))
			{
				//����λ��
				bTempData = cardsData[i];
				cardsData[i] = cardsData[i + 1];
				cardsData[i + 1] = bTempData;
				bTempData = cbSortValue[i];
				cbSortValue[i] = cbSortValue[i + 1];
				cbSortValue[i + 1] = bTempData;
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//�����˿�
void SDYGameLogic::RandCardList(int cardsBuffer[], int bufferCount)
{
	//����׼��
	int cbCardData[SANDAYI_CELL_PACK * SANDAYI_PACK_COUNT];

	memcpy(cbCardData, m_allCardsData, sizeof(m_allCardsData));


	//�����˿�
	BYTE cbRandCount = 0, cbPosition = 0;
	do
	{
		cbPosition = rand() % (bufferCount - cbRandCount);
		cardsBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[bufferCount - cbRandCount];
	} while (cbRandCount < bufferCount);

	return;
}

/* �Ƿ����Ŀ����ֵ */
bool SDYGameLogic::IncludeCard(const int cardsData[], int cardsDataCount, int destCard)
{
	for (int i = 0; i < cardsDataCount; ++i)
	{
		if (cardsData[i] == destCard) return true;
	}

	return false;
}

//ɾ���˿�
bool SDYGameLogic::RemoveCard(const int removeCards[], int removeCount, int cardsData[], int cardsDataCount)
{
	//�������
	int t_deleteCount = 0;
	int t_cardsData[SANDAYI_HAND_CARDS_COUNT_MAX];

	//��������
	if (cardsDataCount > CountArray(t_cardsData))
	{
		return false;
	}
	memcpy(t_cardsData, cardsData, cardsDataCount * sizeof(cardsData[0]));

	//��������Ҫɾ�����˿�����
	for (int i = 0; i < removeCount; i++)
	{
		for (int j = 0; j < cardsDataCount; j++)
		{
			if (removeCards[i] == t_cardsData[j])
			{
				t_deleteCount++;
				t_cardsData[j] = 0;
				break;
			}
		}
	}
	if (t_deleteCount != removeCount) return false;

	//�����˿�
	int t_cardPos = 0;
	int t_invalidInde = cardsDataCount - removeCount;
	for (int i = 0; i < cardsDataCount; i++)
	{
		if (t_cardsData[i] != 0)
		{
			cardsData[t_cardPos++] = t_cardsData[i];
		}
		if (i >= t_invalidInde)
		{
			cardsData[i] = 0;  //**Ren ���
		}
	}

	return true;
}

/* ��ȡ���� */
int SDYGameLogic::GetCardsScore(const int cardsData[], int cardsDataCount)
{
	//��������
	int t_cardScore = 0;

	//�˿��ۼ�
	for (int i = 0; i < cardsDataCount; i++)
	{
		//��ȡ��ֵ
		int t_cardValue = GetCardValue(cardsData[i]);

		//�ۼƻ���
		if (t_cardValue == 5)
		{
			t_cardScore += 5;
			continue;
		}

		//�ۼƻ���
		if ((t_cardValue == 10) || (t_cardValue == 13))
		{
			t_cardScore += 10;
			continue;
		}
	}

	return t_cardScore;
}

/* ��ȡ���� */
int SDYGameLogic::GetScoreCards(const int cardsData[], int cardsDataCount, int scoreCards[], int maxCount)
{
	//��������
	int t_postion = 0;

	//�˿��ۼ�
	for (int i = 0; i < cardsDataCount; ++i)
	{
		//��ȡ��ֵ
		int t_cardValue = GetCardValue(cardsData[i]);

		//�ۼ��˿�
		if ((t_cardValue == 5) || (t_cardValue == 10) || (t_cardValue == 13))
		{
			scoreCards[t_postion++] = cardsData[i];
		}
	}

	return t_postion;
}

/* �Ա��˿��߼�ֵ */
bool SDYGameLogic::CompareCardData(int firstCard, int secondCard)
{
	//��ȡ��ɫ
	int t_logicColorNext = GetCardLogicColor(secondCard);
	int t_logicColorFirst = GetCardLogicColor(firstCard);   //�߼���ɫ

															//�Աȴ�С
	if ((t_logicColorNext == COLOR_NT) || (t_logicColorNext == t_logicColorFirst))
	{
		//��ȡ��С
		int t_logicValueNext = GetCardLogicValue(secondCard);
		int t_logicValueFirst = GetCardLogicValue(firstCard);

		if (GetCommMainValue() != VALUE_ERROR &&
			t_logicValueNext == t_logicValueFirst  &&
			t_logicValueNext == GetCommMainValue() &&
			t_logicValueFirst == GetCommMainValue()) // 2�ǳ��� �����˳��Ķ���2
		{
			if (GetCardColor(secondCard) == GetMainColor())  //����ڶ����˳�������2,��ڶ����˵��ƴ�
			{
				return true;
			}
		}

		//��С�ж�
		if (t_logicValueNext > t_logicValueFirst)
			return true;
	}
	else if (t_logicColorNext != t_logicColorFirst)
	{
		if (t_logicColorNext == m_mainColor)
		{
			return true;
		}
	}

	return false;
}

/* �Աȳ��� */
int SDYGameLogic::CompareOutCardArray(const int outCardArr[], int firstOutPos, int outPlayerCount)// ����ÿ����ҳ�����   ��һ�����Ƶ����  5�����
{
	//��������
	int t_winnerIndex = firstOutPos;

	//ѭ���Ƚ��Ѿ����Ƶ���
	//for (int i = t_winnerIndex; (i < SANDAER_PLAYER_COUNT + t_winnerIndex - 1) && (i < t_winnerIndex + outPlayerCount - 1); i++)
	for (int i = firstOutPos; (i < SANDAYI_PLAYER_COUNT + firstOutPos - 1) && (i < firstOutPos + outPlayerCount - 1); i++)
	{
		int t_userIndex = (i + 1) % SANDAYI_PLAYER_COUNT;

		//������Ч��ֵ
		if (!IsValidCard(outCardArr[t_userIndex]))
		{
			continue;
		}

		//�Աȴ�С
		if (CompareCardData(outCardArr[t_winnerIndex], outCardArr[t_userIndex]))
		{
			t_winnerIndex = t_userIndex;
		}
	}

	return t_winnerIndex;
}

//�������߼���ɫ��ȡ��ͬ�߼���ɫ�˿�
//���� ��ǰ�������  �����ϰѳ�����
int SDYGameLogic::DistillCardByColor(const int cardsData[], int cardsDataCount, int destCard, int resultCards[])
{
	//����Ҫ����Ƶļ���
	int t_resultCardsCount = 0;

	//Ŀ���Ƶ��߼���ɫ
	int t_destCardLogicColor = GetCardLogicColor(destCard);  //�����ϰѳ��Ļ�ɫ  COLOR_NT  ����ɫ

															 //����ɫ��ȡ�˿�
	for (int i = 0; i < cardsDataCount; i++)
	{
		//��ɫ�ж�
		if (GetCardLogicColor(cardsData[i]) == t_destCardLogicColor)
		{
			resultCards[t_resultCardsCount++] = cardsData[i];
			continue;
		}

	}

	return t_resultCardsCount;
}

/* У�������ҳ��� */
// ��ǰ�������  ����  ��������  �����ϰѳ�����  
bool SDYGameLogic::EfficacyOutCard(int outCard, const int handCards[], int handCardsCount, int firstOutCard)
{
	//�߼���ɫ
	int t_outLogicColor = GetCardLogicColor(outCard);
	int t_firstLogicColor = GetCardLogicColor(firstOutCard);

	//ͬ��ɫ��������
	if (t_outLogicColor == t_firstLogicColor)
		return true;

	//����������Ƿ���ͬ��ɫ����
	int t_resultCards[SANDAYI_HAND_CARDS_COUNT];

	//������û��ͬ��ɫ��������  ���� ��ǰ�������  �����ϰѳ�����
	//if (DistillCardByColor(handCards, outCard, firstOutCard, t_resultCards) == 0) 
	if (DistillCardByColor(handCards, handCardsCount, firstOutCard, t_resultCards) == 0)	//�������0,��û����ͬ�Ļ�ɫ
	{
		//�������û����ͬ�Ļ�ɫ,�������������,��Ҳ����
		//if (GetCardLogicColor(outCard) == COLOR_NT)
		return true;
	}

	return false;
}

//����Ȩλ
bool SDYGameLogic::UpDataSortRight()
{
	//����Ȩλ
	for (BYTE i = 0; i < CountArray(m_sortRight); i++)
	{
		m_sortRight[i] = i * COLOR_RIGHT;
	}

	//����Ȩλ
	if (!IsValidColor(m_mainColor))
	{
		return false;
	}

	m_sortRight[m_mainColor >> 4] = 4 * COLOR_RIGHT;

	//����Ȩλ
	switch (m_mainColor)
	{
	case COLOR_MEI_HUA:		//÷��
	{
		m_sortRight[COLOR_HEI_TAO >> 4] = COLOR_RIGHT;
		break;
	}
	case COLOR_HONG_TAO:	//����
	{
		m_sortRight[COLOR_FANG_KUAI >> 4] = 2 * COLOR_RIGHT;
		break;
	}
	}

	return true;
}