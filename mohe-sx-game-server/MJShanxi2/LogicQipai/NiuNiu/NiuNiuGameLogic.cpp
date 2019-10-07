#include "StdAfx.h"
#include "NiuNiuGameLogic.h"

/* ���α���ѡ�� */
static enum TYEP_TIME
{
	TYPE_TIME_NORMAL = 0,		//��ͨ����
	TYPE_TIME_BOMB = 1,			//ɨ�ױ���
	TYPE_TIME_X4 = 2,			//�¼ӱ���ţţX4
};

//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//////////////////////////////////////////////////////////////////////////

//�˿�����
BYTE NGameLogic::m_cbCardListData[52] =
{
	0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,	//���� (0)  A(1)  - K(13)
	0x11,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,	//÷�� (16) A(17) - K(29)
	0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2A,0x2B,0x2C,0x2D,	//���� (32) A(33) - K(45)
	0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3A,0x3B,0x3C,0x3D	//���� (48) A(49) - K(61)
};

//////////////////////////////////////////////////////////////////////////

//���캯��
NGameLogic::NGameLogic()
{
}

//��������
NGameLogic::~NGameLogic()
{
}

//��ȡ����
BYTE NGameLogic::GetCardType(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec)
{

	BYTE bTemp[NIU_NIU_POKER_MAX_COUNT];
	//�洢��ֵ��A = 1��
	BYTE bCardsTemp[NIU_NIU_POKER_MAX_COUNT];
	//�洢��ֵ��A = 14��
	BYTE bShunCardTemp[NIU_NIU_POKER_MAX_COUNT];
	BYTE bSum = 0;
	for (BYTE i = 0; i<cbCardCount; i++)
	{
		bTemp[i] = GetCardLogicValue(cbCardData[i]);
		bCardsTemp[i] = GetCardValue(cbCardData[i]);
		bShunCardTemp[i] = GetCardValue(cbCardData[i]) == 1 ? 14 : GetCardValue(cbCardData[i]);
		bSum += bTemp[i];
	}
	SortCardList(bTemp, cbCardCount);
	SortCardList(bCardsTemp, cbCardCount);
	SortCardList(bShunCardTemp, cbCardCount);

	//�����淨��������
	if (cbIsSpec)
	{
		//˳�������ж�
		bool isPlusOne = TRUE;    //˳�����α�־��true:˳�ӣ�false������˳��
		BYTE bLast = cbCardCount - 1;
		for (BYTE i = 0; i < bLast; i++)
		{
			if (bCardsTemp[i] - bCardsTemp[i + 1] != 1)
			{
				isPlusOne = FALSE;
			}
		}
		if (bCardsTemp[0] == 13 && bCardsTemp[1] == 12 && bCardsTemp[2] == 11 && bCardsTemp[3] == 10 && bCardsTemp[4] == 1)
		{
			isPlusOne = TRUE;
		}
		// һ��������
		//if (bTemp[0] == 5 && isPlusOne) return OX_DRAGON;

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

		//ը�������ж�
		BYTE cardValueCount[13] = { 0 };      //���ε������飬������
		BYTE maxSameCount = 1;              //��������������,�����ж�ը���ͺ�«ţ,4��ը��ţ��3����«ţ
		BOOL twoSameCount = FALSE;
		for (BYTE i = 0; i < cbCardCount; ++i)
		{
			++cardValueCount[GetCardValue(cbCardData[i]) - 1];
		}
		for (BYTE i = 0; i < 13; ++i)
		{
			if (cardValueCount[i] > maxSameCount)
			{
				maxSameCount = cardValueCount[i];
			}
			if (2 == cardValueCount[i])
			{
				twoSameCount = TRUE;
			}
		}

		////ը������
		//BYTE cTemp[NIU_NIU_POKER_MAX_COUNT];
		//for (BYTE i = 0; i < cbCardCount; i++)
		//{
		//	cTemp[i] = GetCardValue(cbCardData[i]);
		//}
		//BYTE bSameCount = 1;
		//for (BYTE i = 0; i < 2; ++i)
		//{
		//	for (BYTE j = i + 1; j < cbCardCount; ++j)
		//	{
		//		if (cTemp[i] == cTemp[j])
		//			bSameCount++;
		//	}
		//	if (bSameCount == 4)break;
		//	bSameCount = 1;
		//}
		//if (bSameCount == 4) return OX_FOUR_SAME;


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

		//��ţ����ţ�����ж�
		/*BYTE bKingCount = 0, bTenCount = 0;
		bool isJinNiu = FALSE, isYinNiu = FALSE;
		for (BYTE i = 0; i < cbCardCount; i++)
		{
		if (GetCardValue(cbCardData[i])>10)
		{
		bKingCount++;
		}
		else if (GetCardValue(cbCardData[i]) == 10)
		{
		bTenCount++;
		}
		}
		if (bKingCount == NIU_NIU_POKER_MAX_COUNT) isJinNiu = TRUE;
		else if (bKingCount + bTenCount == NIU_NIU_POKER_MAX_COUNT) isYinNiu = TRUE;*/

		/*if (bKingCount == NIU_NIU_POKER_MAX_COUNT) return OX_FIVEKING;
		else if (bKingCount + bTenCount == NIU_NIU_POKER_MAX_COUNT) return OX_ALLTEN;*/
		//else if(bKingCount==NIU_NIU_POKER_MAX_COUNT-1 && bTenCount==1) return OX_FOURKING;

		////��«����
		//if(bSameCount==3)
		//{
		//	if((bSecondValue!=GetCardValue(cbCardData[3]) && GetCardValue(cbCardData[3])==GetCardValue(cbCardData[4]))
		//	||(bSecondValue!=GetCardValue(cbCardData[1]) && GetCardValue(cbCardData[1])==GetCardValue(cbCardData[0])))
		//		return OX_THREE_SAME;
		//}


		//�����жϣ��������δ�С����

		//ͬ��˳
		if (TRUE == isPlusOne && TRUE == isSameColor) return OX_TONGHUASHUN;
		//��Сţ
		else if (TRUE == isWuXiaoNiu) return OX_FIVESMALL;
		//ը��
		else if (4 == maxSameCount) return OX_FOUR_SAME;
		//��«ţ
		else if (3 == maxSameCount && TRUE == twoSameCount) return OX_THREE_SAME;
		//ͬ��ţ
		//else if (TRUE == isSameColor && FALSE == isPlusOne) return OX_COLOR_SAME;
		//˳��ţ
		else if (TRUE == isPlusOne && FALSE == isSameColor) return OX_SHUNZINIU;
		//�廨ţ
		else if (TRUE == isWuHuaNiu) return OX_WUHUANIU;
		//��ţ
		//else if (TRUE == isJinNiu) return OX_FIVEKING;
		//��ţ
		//else if (TRUE == isYinNiu) return OX_FOURKING;

	}
	//�ǻ����淨��������
	else
	{
		//TODO����Ҫ�ж���Сţ
		if (bSum <= 10) return OX_FIVESMALL;

		//ը�������ж�
		BYTE cardValueCount[13] = { 0 };      //���ε������飬������
		BYTE maxSameCount = 1;              //��������������,�����ж�ը���ͺ�«ţ,4��ը��ţ��3����«ţ
		for (BYTE i = 0; i < cbCardCount; ++i)
		{
			++cardValueCount[GetCardValue(cbCardData[i]) - 1];
		}
		for (BYTE i = 0; i < 13; ++i)
		{
			if (cardValueCount[i] > maxSameCount)
			{
				maxSameCount = cardValueCount[i];
			}
		}
		//ը��
		if (4 == maxSameCount) return OX_FOUR_SAME;

		//TODO����Ҫ�жϽ�ţ ��ţ
		BYTE bKingCount = 0, bTenCount = 0;
		for (BYTE i = 0; i < cbCardCount; i++)
		{
			if (GetCardValue(cbCardData[i])>10)
			{
				bKingCount++;
			}
			else if (GetCardValue(cbCardData[i]) == 10)
			{
				bTenCount++;
			}
		}
		//��ţ
		if (bKingCount == NIU_NIU_POKER_MAX_COUNT) return OX_FIVEKING;
		//��ţ
		else if (bKingCount + bTenCount == NIU_NIU_POKER_MAX_COUNT) return OX_ALLTEN;
	}

	//�ж�ţţ������ͨ����
	for (BYTE i = 0; i<cbCardCount - 1; i++)
	{
		for (BYTE j = i + 1; j<cbCardCount; j++)
		{
			if ((bSum - bTemp[i] - bTemp[j]) % 10 == 0)
			{
				return ((bTemp[i] + bTemp[j])>10) ? (bTemp[i] + bTemp[j] - 10) : (bTemp[i] + bTemp[j]);
			}
		}
	}

	return OX_VALUE0;
}

//��ȡ����
BYTE NGameLogic::GetTimes_normal(BYTE cbCardData[], BYTE cbCardCount, bool isSpecPlay)
{
	if (cbCardCount != NIU_NIU_POKER_MAX_COUNT)return 0;

	BYTE bTimes = GetCardType(cbCardData, NIU_NIU_POKER_MAX_COUNT, isSpecPlay);
	// ��ţ - ţ��: 1��
	if (bTimes <= 6)return 1;
	// ţ�� - ţ��: 2��
	else if (bTimes >= 7 && bTimes <= 9) return 2;
	// ţţ: 3��
	else if (bTimes == 10)return 3;
	// ��ţ 4��
	else if (bTimes == OX_ALLTEN)return 4;
	// ��ţ 5��
	else if (bTimes == OX_FIVEKING)return 5;
	//�廨ţ 5��
	else if (bTimes == OX_WUHUANIU) return 5;
	//˳��ţ��6��
	else if (bTimes == OX_SHUNZINIU) return 6;
	//��«ţ��7��
	else if (bTimes == OX_THREE_SAME) return 7;
	//ͬ��ţ��8��
	//else if (bTimes == OX_COLOR_SAME) return 8;
	// ը�� 8��
	else if (bTimes == OX_FOUR_SAME)return 8;
	//��Сţ��10��
	else if (bTimes == OX_FIVESMALL)return 10;
	//ͬ��˳ţ��12��
	else if (bTimes == OX_TONGHUASHUN)return 12;

	return 0;
}

//������X1
BYTE NGameLogic::GetTimes_x4(BYTE cbCardData[], BYTE cbCardCount, bool isSpecPlay)
{
	if (cbCardCount != NIU_NIU_POKER_MAX_COUNT)return 0;

	BYTE bTimes = GetCardType(cbCardData, NIU_NIU_POKER_MAX_COUNT, isSpecPlay);
	// ��ţ - ţ��: 1��
	if (bTimes <= 6)return 1;
	// ţ�� - ţ��: 2��
	else if (bTimes >= 7 && bTimes <= 8) return 2;
	// ţ�ţ�3��
	else if (bTimes == 9) return 3;
	// ţţ: 4��
	else if (bTimes == 10)return 4;
	// ��ţ 5��
	else if (bTimes == OX_ALLTEN)return 5;
	// ��ţ 6��
	else if (bTimes == OX_FIVEKING)return 6;
	//�廨ţ 6��
	else if (bTimes == OX_WUHUANIU) return 6;
	//˳��ţ��7��
	else if (bTimes == OX_SHUNZINIU) return 7;
	//��«ţ��8��
	else if (bTimes == OX_THREE_SAME) return 8;
	//ͬ��ţ��9��
	//else if (bTimes == OX_COLOR_SAME) return 9;
	// ը�� 10��
	else if (bTimes == OX_FOUR_SAME)return 10;
	//��Сţ��12��
	else if (bTimes == OX_FIVESMALL)return 12;
	//ͬ��˳ţ��14��
	else if (bTimes == OX_TONGHUASHUN)return 14;

	return 0;
}

BYTE NGameLogic::GetTimes(BYTE cbCardData[], BYTE cbCardCount, bool cbIsSpec, BYTE cbSweep)
{
	//��ɨ��ģʽ��(��ͨ����)
	if (cbSweep == TYPE_TIME_NORMAL)
	{
		return GetTimes_normal(cbCardData, cbCardCount, cbIsSpec);
	}
	//��������ţţX4
	else if (cbSweep == TYPE_TIME_X4)
	{
		return GetTimes_x4(cbCardData, cbCardCount, cbIsSpec);
	}
	//ɨ��ģʽ��
	else
	{
		if (cbCardCount != NIU_NIU_POKER_MAX_COUNT)return 0;

		BYTE bTimes = GetCardType(cbCardData, NIU_NIU_POKER_MAX_COUNT, cbIsSpec);
		//��ţ��1��
		if (bTimes == 0) return 1;
		//ţһ - ţţ��1-10��
		else if (bTimes >= 1 && bTimes <= 10)return bTimes;
		// ��ţ 12��
		else if (bTimes == OX_ALLTEN)return 12;
		// ��ţ 15��
		else if (bTimes == OX_FIVEKING)return 15;
		//�廨ţ 15��
		//else if (bTimes == OX_WUHUANIU) return 15;
		//��«ţ��18��
		//else if (bTimes == OX_THREE_SAME) return 18;
		//˳��ţ��20��
		//else if (bTimes == OX_SHUNZINIU) return 20;
		//ͬ��ţ��22��
		//else if (bTimes == OX_COLOR_SAME) return 22;
		// ը�� 18��
		else if (bTimes == OX_FOUR_SAME)return 18;
		//��Сţ��20��
		else if (bTimes == OX_FIVESMALL)return 20;
		//ͬ��˳ţ��30��
		//else if (bTimes == OX_DRAGON)return 30;

		return 0;
	}
}


//��ȡţţ
bool NGameLogic::GetOxCard(BYTE cbCardData[], BYTE cbCardCount)
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
bool NGameLogic::IsIntValue(BYTE cbCardData[], BYTE cbCardCount)
{
	BYTE sum = 0;
	for (BYTE i = 0; i<cbCardCount; i++)
	{
		sum += GetCardLogicValue(cbCardData[i]);
	}

	return (sum % 10 == 0);
}

//�����˿�
void NGameLogic::SortCardList(BYTE cbCardData[], BYTE cbCardCount)
{
	//ת����ֵ
	BYTE cbLogicValue[NIU_NIU_POKER_MAX_COUNT];
	for (BYTE i = 0; i<cbCardCount; i++) cbLogicValue[i] = GetCardValue(cbCardData[i]);

	//�������
	bool bSorted = true;
	BYTE cbTempData, bLast = cbCardCount - 1;
	do
	{
		bSorted = true;
		for (BYTE i = 0; i<bLast; i++)
		{
			if ((cbLogicValue[i]<cbLogicValue[i + 1]) ||
				((cbLogicValue[i] == cbLogicValue[i + 1]) && (cbCardData[i]<cbCardData[i + 1])))
			{
				//����λ��
				cbTempData = cbCardData[i];
				cbCardData[i] = cbCardData[i + 1];
				cbCardData[i + 1] = cbTempData;
				cbTempData = cbLogicValue[i];
				cbLogicValue[i] = cbLogicValue[i + 1];
				cbLogicValue[i + 1] = cbTempData;
				bSorted = false;
			}
		}
		bLast--;
	} while (bSorted == false);

	return;
}

//�����˿�
void NGameLogic::RandCardList(BYTE cbCardBuffer[], BYTE cbBufferCount)
{
	//CopyMemory(cbCardBuffer,m_cbCardListData,cbBufferCount);
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
	} while (bRandCount<cbBufferCount);

	return;
}

//�߼���ֵ
BYTE NGameLogic::GetCardLogicValue(BYTE cbCardData)
{
	//�˿�����
	BYTE bCardColor = GetCardColor(cbCardData);
	BYTE bCardValue = GetCardValue(cbCardData);

	//ת����ֵ
	return (bCardValue>10) ? (10) : bCardValue;
}

//�Ա��˿�
bool NGameLogic::CompareCard(BYTE cbFirstData[], BYTE cbNextData[], BYTE cbCardCount, BOOL FirstOX, BOOL NextOX, BOOL isSpecPlay)
{
	if (FirstOX != NextOX)return (FirstOX>NextOX);

	//��ȡţţ����
	BYTE cbNextType = GetCardType(cbNextData, cbCardCount, isSpecPlay);
	BYTE cbFirstType = GetCardType(cbFirstData, cbCardCount, isSpecPlay);

	//�Ƚ�ţ��С
	if (FirstOX == TRUE)
	{
		//�����ж�
		if (cbFirstType != cbNextType) return (cbFirstType>cbNextType);

		//������ͬ�������ж�
		switch (cbNextType)
		{
		case OX_FOUR_SAME:		//ը������	
		case OX_THREE_SAME:		//��«����
		{
			//�����С
			BYTE bFirstTemp[NIU_NIU_POKER_MAX_COUNT], bNextTemp[NIU_NIU_POKER_MAX_COUNT];
			CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
			CopyMemory(bNextTemp, cbNextData, cbCardCount);
			SortCardList(bFirstTemp, cbCardCount);
			SortCardList(bNextTemp, cbCardCount);

			return GetCardValue(bFirstTemp[NIU_NIU_POKER_MAX_COUNT / 2])>GetCardValue(bNextTemp[NIU_NIU_POKER_MAX_COUNT / 2]);

			break;
		}
		}
	}

	//�����С
	BYTE bFirstTemp[NIU_NIU_POKER_MAX_COUNT], bNextTemp[NIU_NIU_POKER_MAX_COUNT];
	CopyMemory(bFirstTemp, cbFirstData, cbCardCount);
	CopyMemory(bNextTemp, cbNextData, cbCardCount);
	SortCardList(bFirstTemp, cbCardCount);
	SortCardList(bNextTemp, cbCardCount);

	//�������˳������ ����ͬΪͬ��˳
	if ((cbNextType == cbFirstType && (cbFirstType == OX_TONGHUASHUN || cbFirstType == OX_SHUNZINIU)))
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
	if (cbNextMaxValue != cbFirstMaxValue)return cbFirstMaxValue>cbNextMaxValue;

	//�Ƚ���ɫ
	return GetCardColor(bFirstTemp[0]) > GetCardColor(bNextTemp[0]);

	return false;
}

//////////////////////////////////////////////////////////////////////////
