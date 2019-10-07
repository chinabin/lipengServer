#include "CheXuanGameLogic.h"

/* �˿˶��� */
int CXGameLogic::m_cardsPool[CHEXUAN_CELL_PACK] =
{
	0x0C,0x2C, 0x02,0x22, 0x08,0x28, 0x04,0x24,				/* ��(2��Q) > ��(2��2) > ��(2��8) > ��(2��4) */
	0x1A,0x3A, 0x14,0x34, 0x16,0x36,						/* ÷ʮ(2��10) = ���(2��4) = ����(2��6) */
	0x1B,0x3B, 0x0A,0x2A, 0x06,0x26, 0x07,0x27,				/* ��ͷ(2��J) = ��ʮ(2��10) = èè(2��6) = ��ҩ(2��7) */
	0x19,0x39, 0x18,0x38, 0x17,0x37, 0x15,0x35, 0x23, 0x4F	/* ������(2��9��2��8��2��7��2��5) = ����(1����3) = ����(1����) */
};

/* ���캯�� */
CXGameLogic::CXGameLogic()
{
	m_is_allow_di_jiu = false;
}

/* �������� */
CXGameLogic::~CXGameLogic()
{

}

/* ��ȡ�ؾ���ѡ�� */
bool CXGameLogic::getDiJiu()
{
	return m_is_allow_di_jiu;
}

/* ���õؾ���ѡ�� */
void CXGameLogic::setDiJiu(bool diJiu)
{
	m_is_allow_di_jiu = diJiu;
}

/* ��ȡ�Ƶ���ֵ */
int CXGameLogic::getCardValue(int card)
{
	return card & LOGIC_MASK_VALUE;
}

/* ��ȡ�ƵĻ�ɫ */
int CXGameLogic::getCardColor(int card)
{
	return card & LOGIC_MASK_COLOR;
}

/* ��ȡ��ɫ���ͣ�����&���� = ��  ÷��&���� = �� */
int CXGameLogic::getCardColorType(int card)
{
	int t_color = getCardColor(card);

	if (COLOR_FANG_KUAI == t_color || COLOR_HONG_TAO == t_color)
	{
		return COLOR_HONG;
	}

	else if (COLOR_MEI_HUA == t_color || COLOR_HEI_TAO == t_color)
	{
		return COLOR_HEI;
	}

	else
	{
		return -1;
	}
}

/* �Ƿ�Ϊ��Ч��ֵ */
bool CXGameLogic::isValidCard(int card)
{
	return false;
}

/* ��ȡ�����߼�ֵ */
SINGLE_TYPE CXGameLogic::getLogicCardValue(int card)
{
	if (card == 0) return STYTE_ERROR;

	//��ɫ
	int t_color = getCardColor(card);

	//��ɫ����
	int t_color_type = getCardColorType(card);

	//��ֵ
	int t_value = getCardValue(card);


	//�������ͣ�����Q  ����Q
	if (12 == t_value && COLOR_HONG == t_color_type)
	{
		return STYPE_TIAN;
	}

	//�������ͣ�����2  ����2
	if (2 == t_value && COLOR_HONG == t_color_type)
	{
		return STYPE_DI;
	}

	//�������ͣ�����8  ����8
	if (8 == t_value && COLOR_HONG == t_color_type)
	{
		return STYPE_REN;
	}

	//�������ͣ�����4  ����4
	if (4 == t_value && COLOR_HONG == t_color_type)
	{
		return STYPE_HE;
	}

	//÷ʮ��÷��10  ����10����ʣ�÷��4  ����4��������÷��6  ����6
	if (COLOR_HEI == t_color_type &&
		(10 == t_value || 4 == t_value || 6 == t_value))
	{
		return STYPE_MEI_BAN_SAN;
	}

	//��ͷ��÷��J  ����J ����ʮ������10  ����10 ��èè������6  ����6 �� ��ҩ������7  ����7
	if ((11 == t_value && COLOR_HEI == t_color_type) ||
		(COLOR_HONG == t_color_type && (10 == t_value || 6 == t_value || 7 == t_value)))
	{
		return STYPE_FU_SHI_MAO_GAO;
	}

	//������ ��9  ��8  ��7  ��5  ����3  ����
	return STYPE_XIA_SI_LAN;
}

/* ��ȡ�������� */
DOUBLE_TYPE CXGameLogic::getDoubleCardsType()
{
	return (DOUBLE_TYPE)0;
}

/* ��ȡ����ֵ */
DOUBLE_TYPE_VALUE CXGameLogic::getDoubleCardsTypeValue(int card1, int card2)
{
	//��ɫ
	int t_color1 = getCardColor(card1);
	int t_color2 = getCardColor(card2);

	//��ɫ����
	int t_color_type1 = getCardColorType(card1);
	int t_color_type2 = getCardColorType(card2);

	//��ֵ
	int t_value1 = getCardValue(card1);
	int t_value2 = getCardValue(card2);

	//�����ʣ�����3 + ����
	if ((card1 == 0x23 && card2 == 0x4F) || (card1 == 0x4F && card2 == 0x23)) return DTV_DING_ER_HUANG;

	//���ƶԣ�2�ź�Q
	else if ((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (12 == t_value1 && t_value1 == t_value2)) return DTV_TIAN_DUI;

	//���ƶԣ�2�ź�2
	else if ((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (2 == t_value1 && t_value1 == t_value2)) return DTV_DI_DUI;

	//���ƶԣ�2�ź�8
	else if ((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (8 == t_value1 && t_value1 == t_value2)) return DTV_REN_DUI;

	//���ƶԣ�2�ź�4
	else if ((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (4 == t_value1 && t_value1 == t_value2)) return DTV_HE_DUI;

	//÷�������ƣ���4�ԡ���6�ԡ���10��
	else if (
		(COLOR_HEI == t_color_type1 && t_color_type1 == t_color_type2) &&
		((4 == t_value1 && t_value1 == t_value2) || (6 == t_value1 && t_value1 == t_value2) || (10 == t_value1 && t_value1 == t_value2))
		)
	{
		return DTV_MEI_BAN_SAN_DUI;
	}

	//��ʮè��ԣ���J�ԡ���10�ԡ���6�ԡ���7��
	else if (
		((COLOR_HEI == t_color_type1 && t_color_type1 == t_color_type2) && (11 == t_value1 && t_value1 == t_value2)) ||
		((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (10 == t_value1 && t_value1 == t_value2)) ||
		((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (6 == t_value1 && t_value1 == t_value2)) ||
		((COLOR_HONG == t_color_type1 && t_color_type1 == t_color_type2) && (7 == t_value1 && t_value1 == t_value2))
		)
	{
		return DTV_FU_SHI_MAO_GAO_DUI;
	}

	//�����Ķԣ���9�ԡ���8�ԡ���7�ԡ���5��
	else if ((COLOR_HEI == t_color_type1 && t_color_type1 == t_color_type2) &&
		(
		(9 == t_value1 && t_value1 == t_value2) ||
			(8 == t_value1 && t_value1 == t_value2) ||
			(7 == t_value1 && t_value1 == t_value2) ||
			(5 == t_value1 && t_value1 == t_value2))
		)
	{
		return DTV_XIA_SI_LAN_DUI;
	}

	//���������Q�����ƣ� + ��9
	else if ((COLOR_HONG == t_color_type1 && 12 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 12 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1))
	{
		return DTV_TIAN_JIU_WANG;
	}

	//�ؾ�������2�����ƣ� + ��9 ����Ҫѡ���֧�֣�
	else if ( m_is_allow_di_jiu &&
		((COLOR_HONG == t_color_type1 && 2 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 2 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1))
		)
	{
		return DTV_DI_JIU_WANG;
	}

	//��ܣ���Q�����ƣ�+ ��8�����ƣ� ��  ��Q�����ƣ�+ ��8
	else if ((COLOR_HONG == t_color_type1 && 12 == t_value1 && 8 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 12 == t_value2 && 8 == t_value1))
	{
		return DTV_TIAN_GANG;
	}

	//�ظܣ���2�����ƣ�+ ��8�����ƣ�|| ��2�����ƣ�+ ��8
	else if ((COLOR_HONG == t_color_type1 && 2 == t_value1 && 8 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 2 == t_value2 && 8 == t_value1))
	{
		return DTV_DI_GANG;
	}

	//��پţ���Q�����ƣ�+ ���7
	else if ((COLOR_HONG == t_color_type1 && 12 == t_value1 && 7 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 12 == t_value2 && 7 == t_value1))
	{
		return DTV_DIAN_TIAN_GUAN_JIU;
	}

	//�عپţ���2�����ƣ�+ ���7
	else if ((COLOR_HONG == t_color_type1 && 2 == t_value1 && 7 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 2 == t_value2 && 7 == t_value1))
	{
		return DTV_DIAN_DI_GUAN_JIU;
	}

	//�˹پţ���8�����ƣ�+ ��J����ͷ��
	else if ((COLOR_HONG == t_color_type1 && 8 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 8 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1))
	{
		return DTV_DIAN_REN_GUAN_JIU;
	}

	//�͹پţ���4�����ƣ�+ ��5
	else if ((COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) ||
		(COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1))
	{
		return DTV_DIAN_HE_GUAN_JIU;
	}

	//÷ʮ�ţ���10��÷ʮ��+ ��9 || ��4����ʣ�+ ��5 || ����3��������+ ��6��������
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HEI == t_color_type2 && 6 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HEI == t_color_type1 && 6 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_JIU;
	}

	//�����ţ���J����ͷ��+ ��8 || ��10����ʮ��+ ��9 || ����3��������+ ��6��èè��
	else if (
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1) ||
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HONG == t_color_type2 && 6 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HONG == t_color_type1 && 6 == t_value1)
		)
	{
		return DTV_DIAN_WU_LONG_JIU;
	}

	//��ٰˣ���Q�����ƣ�+ ���6 || ��Q�����ƣ�+ ����
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && 6 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && 6 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_GUAN_BA;
	}

	//�عٰˣ���2�����ƣ�+ ���6 || ��2�����ƣ�+ ����
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && 6 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && 6 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_DI_GUAN_BA;
	}

	//�˹ٰˣ���8�����ƣ�+ ���10
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && 10 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && 10 == t_value1)
		)
	{
		return DTV_DIAN_REN_GUAN_BA;
	}

	//�͹ٰˣ���4�����ƣ�+ ��4����ʣ�
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 4 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 4 == t_value1)
		)
	{
		return DTV_DIAN_HE_GUAN_BA;
	}

	//÷ʮ�ˣ���10��÷ʮ��+ ��8
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_BA;
	}

	//���߰ˣ���J����ͷ��+ ���7 || ��10����ʮ��+ ��8
	else if (
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && 7 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && 7 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_FU_QI_BA;
	}

	//����ˣ�����3��������+ ��5
	else if (
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_DING_WU_BA;
	}

	//����ߣ���Q�����ƣ�+ ��5
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_GUAN_QI;
	}

	//�ع��ߣ���2�����ƣ�+ ��5
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_DI_GUAN_QI;
	}

	//�˹��ߣ���8�����ƣ�+ ��9
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1)
		)
	{
		return DTV_DIAN_REN_GUAN_QI;
	}

	//�͹��ߣ���4�����ƣ�+ ����3
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1)
		)
	{
		return DTV_DIAN_HE_GUAN_QI;
	}

	//÷ʮ�ߣ���10��÷ʮ�� + ���7 || ��4����ʣ� + ����3 || ��J����ͷ�� + ��6��������
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && 7 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && 7 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && COLOR_HEI == t_color_type2 && 6 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && COLOR_HEI == t_color_type1 && 6 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_QI;
	}

	//��è�ߣ���J����ͷ��+ ��6��èè��|| ��J����ͷ��+ ���� || ��10����ʮ��+ ���7
	else if (
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && COLOR_HONG == t_color_type2 && 6 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && COLOR_HONG == t_color_type1 && 6 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && 15 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && 7 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && 7 == t_value1)
		)
	{
		return DTV_DIAN_FU_MAO_QI;
	}

	//�����ߣ���9 + ��8
	else if (
		(COLOR_HEI == t_color_type1 && 9 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 9 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_FEI_ZHOU_QI;
	}

	//���������Q�����ƣ�+ ���4
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && 4 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && 4 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_GUAN_LIU;
	}

	//�ع�������2�����ƣ�+ ���4
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && 4 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && 4 == t_value1)
		)
	{
		return DTV_DIAN_DI_GUAN_LIU;
	}

	//�˹�������8�����ƣ�+ ��8
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_REN_GUAN_LIU;
	}

	//÷ʮ������10��÷ʮ��+ ���6 || ��10��÷ʮ��+ ���� || ��6��������+ ��10����ʮ��
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && 6 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && 6 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && 15 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && COLOR_HONG == t_color_type2 && 10 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && COLOR_HONG == t_color_type1 && 10 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_LIU;
	}

	//����������J����ͷ��+ ��5 || ��10����ʮ��+ ��6��èè��|| ��10����ʮ��+ ���� || ��7����ҩ��+ ��9
	else if (
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HONG == t_color_type2 && 6 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HONG == t_color_type1 && 6 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && 15 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1)
		)
	{
		return DTV_DIAN_FU_WU_LIU;
	}

	//�߾�������9 + ��7
	else if (
		(COLOR_HEI == t_color_type1 && 9 == t_value1 && COLOR_HEI == t_color_type2 && 7 == t_value2) || (COLOR_HEI == t_color_type2 && 9 == t_value2 && COLOR_HEI == t_color_type1 && 7 == t_value1)
		)
	{
		return DTV_DIAN_QI_JIU_LIU;
	}

	//�춡�壺��Q�����ƣ�+ ����3
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_GUAN_WU;
	}

	//�ض��壺��2�����ƣ�+ ����3
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1)
		)
	{
		return DTV_DIAN_DI_GUAN_WU;
	}

	//��ʮ�壺��8�����ƣ�+ ���7
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && 7 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && 7 == t_value1)
		)
	{
		return DTV_DIAN_REN_SHI_WU;
	}

	//�͸��壺��4�����ƣ�+ ��J����ͷ��
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1)
		)
	{
		return DTV_DIAN_HE_FU_WU;
	}

	//÷ʮ�壺��10��÷ʮ��+ ��5 || ��4����ʣ�+ ��J����ͷ��|| ��6��������+ ��9
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_WU;
	}

	//��ʮ�壺��10����ʮ��+ ��5 || ��6��èè��+ ��9 || ��7����ҩ��+ ��8
	else if (
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_TIAO_SHI_WU;
	}

	//�߰��壺��7 + ��8 || ��9 + ����
	else if (
		(COLOR_HEI == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 9 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 9 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_QI_BA_WU;
	}

	//��ع����ģ���Q�����ƣ�+ ��2�����ƣ�
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && COLOR_HONG == t_color_type2 && 2 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && COLOR_HONG == t_color_type1 && 2 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_DI_SI;
	}

	//��ʮ�ģ���8�����ƣ�+ ���6 || ��8�����ƣ�+ ����
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && 6 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && 6 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_REN_SHI_SI;
	}

	//��ʮ�ģ���4�����ƣ�+ ���10
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && 10 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && 10 == t_value1)
		)
	{
		return DTV_DIAN_HE_SHI_SI;
	}

	//÷ʮ�ģ���10��÷ʮ��+ ��4����ʣ�|| ��6��������+ ��8 || ��10 + ��4����ʣ�
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 4 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 4 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 4 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 4 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_SI;
	}

	//�����ģ�����3��������+ ��J����ͷ��|| ��6��èè��+ ��8 || ��7����ҩ��+ ��7
	else if (
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 7 == t_value2) || (COLOR_HONG == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 7 == t_value1)
		)
	{
		return DTV_DIAN_DING_FU_SI;
	}

	//����ģ���9 + ��5 || ��8 + ����
	else if (
		(COLOR_HEI == t_color_type1 && 9 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 9 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 8 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 8 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_WU_JIU_SI;
	}

	//�츫������Q�����ƣ�+ ��J����ͷ��
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_FU_SAN;
	}

	//�ظ�������2�����ƣ�+ ��J����ͷ��
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1)
		)
	{
		return DTV_DIAN_DI_FU_SAN;
	}

	//��ʮ������8�����ƣ�+ ��5
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_REN_SHI_SAN;
	}

	//��ʮ������4�����ƣ�+ ��9
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1)
		)
	{
		return DTV_DIAN_HE_SHI_SAN;
	}

	//÷ʮ������10��÷ʮ��+ ����3��������|| ��4����ʣ�+ ��9 || ��6��������+ ���7
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && 7 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && 7 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_SAN;
	}

	//��ʮ������10����ʮ��+ ����3��������|| ��6��èè��+ ���7 || ��7����ҩ�� + ����
	else if (
		(COLOR_HONG == t_color_type1 && 10 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HONG == t_color_type2 && 10 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 6 == t_value1 && 7 == t_value2) || (COLOR_HONG == t_color_type2 && 6 == t_value2 && 7 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 7 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 7 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_TIAO_SHI_SAN;
	}

	//���������8 + ��5 || ��7 + ����
	else if (
		(COLOR_HEI == t_color_type1 && 8 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 8 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 7 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 7 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_WU_BA_SAN;
	}

	//��ʮ������Q�����ƣ�+ ���10
	else if (
		(COLOR_HONG == t_color_type1 && 12 == t_value1 && 10 == t_value2) || (COLOR_HONG == t_color_type2 && 12 == t_value2 && 10 == t_value1)
		)
	{
		return DTV_DIAN_TIAN_SHI_ER;
	}

	//��ʮ������2�����ƣ�+ ���10
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && 10 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && 10 == t_value1)
		)
	{
		return DTV_DIAN_DI_SHI_ER;
	}

	//��ʮ������8�����ƣ�+ ���4
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && 4 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && 4 == t_value1)
		)
	{
		return DTV_DIAN_REN_SHI_ER;
	}

	//��ʮ������4�����ƣ�+ ��8
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1)
		)
	{
		return DTV_DIAN_HE_SHI_ER;
	}

	//�İ˶�����4����ʣ�+ ��8 || ��6��������+ ��6��èè��|| ��6��������+ ����
	else if (
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && COLOR_HONG == t_color_type2 && 6 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && COLOR_HONG == t_color_type1 && 6 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_SI_BA_ER;
	}

	//��ҩ������7����ҩ��+ ��5 || ��6��èè��+ ����
	else if (
		(COLOR_HONG == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 6 == t_value1 && 15 == t_value2) || (COLOR_HONG == t_color_type2 && 6 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_GAO_YAO_ER;
	}

	//���Ŷ�������3��������+ ��9 || ��7 + ��5
	else if (
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 7 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 7 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_DING_JIU_ER;
	}

	//�ؾ�һ����2�����ƣ�+ ��9
	else if (
		(COLOR_HONG == t_color_type1 && 2 == t_value1 && COLOR_HEI == t_color_type2 && 9 == t_value2) || (COLOR_HONG == t_color_type2 && 2 == t_value2 && COLOR_HEI == t_color_type1 && 9 == t_value1)
		)
	{
		return DTV_DIAN_DI_JIU_YI;
	}

	//��ʮһ����8�����ƣ�+ ����3��������
	else if (
		(COLOR_HONG == t_color_type1 && 8 == t_value1 && COLOR_HONG_TAO == t_color2 && 3 == t_value2) || (COLOR_HONG == t_color_type2 && 8 == t_value2 && COLOR_HONG_TAO == t_color1 && 3 == t_value1)
		)
	{
		return DTV_DIAN_REN_SHI_YI;
	}

	//��ʮһ����4�����ƣ�+ ���7
	else if (
		(COLOR_HONG == t_color_type1 && 4 == t_value1 && 7 == t_value2) || (COLOR_HONG == t_color_type2 && 4 == t_value2 && 7 == t_value1)
		)
	{
		return DTV_DIAN_HE_SHI_YI;
	}

	//÷ʮһ����10��÷ʮ��+ ��J����ͷ��|| ��4����ʣ�+ ���7 || ��6��������+ ��5
	else if (
		(COLOR_HEI == t_color_type1 && 10 == t_value1 && COLOR_HEI == t_color_type2 && 11 == t_value2) || (COLOR_HEI == t_color_type2 && 10 == t_value2 && COLOR_HEI == t_color_type1 && 11 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 4 == t_value1 && 7 == t_value2) || (COLOR_HEI == t_color_type2 && 4 == t_value2 && 7 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HEI == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_MEI_SHI_YI;
	}

	//��ʮһ����J����ͷ��+ ��10����ʮ��|| ��6��èè��+ ��5
	else if (
		(COLOR_HEI == t_color_type1 && 11 == t_value1 && COLOR_HONG == t_color_type2 && 10 == t_value2) || (COLOR_HEI == t_color_type2 && 11 == t_value2 && COLOR_HONG == t_color_type1 && 10 == t_value1) ||
		(COLOR_HONG == t_color_type1 && 6 == t_value1 && COLOR_HEI == t_color_type2 && 5 == t_value2) || (COLOR_HONG == t_color_type2 && 6 == t_value2 && COLOR_HEI == t_color_type1 && 5 == t_value1)
		)
	{
		return DTV_DIAN_FU_SHI_YI;
	}

	//����һ������3��������+ ��8 || ��5 + ����
	else if (
		(COLOR_HONG_TAO == t_color1 && 3 == t_value1 && COLOR_HEI == t_color_type2 && 8 == t_value2) || (COLOR_HONG_TAO == t_color2 && 3 == t_value2 && COLOR_HEI == t_color_type1 && 8 == t_value1) ||
		(COLOR_HEI == t_color_type1 && 5 == t_value1 && 15 == t_value2) || (COLOR_HEI == t_color_type2 && 5 == t_value2 && 15 == t_value1)
		)
	{
		return DTV_DIAN_DING_BA_YI;
	}

	else
	{
		return DTV_DIAN_LING;
	}

}

/* ����Ƿ�Ϊ�������� */
bool CXGameLogic::checkSanHuaType(const int* handCards, int handCardsCount)
{
	if (handCards == NULL || handCardsCount < 3)
	{
		return false;
	}
	int t_color_type = COLOR_NT;
	int t_value = 0;

	//�ж��Ƿ�Ϊ����ʮ����10����10����J
	int t_sanHuaShiCount = 0;
	int t_hongShiCount = 1;
	int t_heiShiCount = 1;
	int t_heiGouCount = 1;
	for (int i = 0; i < handCardsCount; ++i)
	{
		t_color_type = getCardColorType(handCards[i]);
		t_value = getCardValue(handCards[i]);
		//���Һ�10
		if (t_hongShiCount == 1 && t_color_type == COLOR_HONG && t_value == 10)
		{
			++t_sanHuaShiCount;
			t_hongShiCount = 0;
			continue;
		}
		//���Һ�10
		else if (t_heiShiCount == 1 && t_color_type == COLOR_HEI && t_value == 10)
		{
			++t_sanHuaShiCount;
			t_heiShiCount = 0;
			continue;
		}
		//���Һ�J
		else if (t_heiGouCount == 1 && t_color_type == COLOR_HEI && t_value == 11)
		{
			++t_sanHuaShiCount;
			t_heiGouCount = 0;
			continue;
		}
	}
	if (t_sanHuaShiCount == 3) return true;

	//�ж��Ƿ�Ϊ����������6����6������
	int t_sanHuaLiuCount = 0;
	int t_hongLiuCount = 1;
	int t_heiLiuCount = 1;
	int t_daWangCount = 1;
	for (int i = 0; i < handCardsCount; ++i)
	{
		t_color_type = getCardColorType(handCards[i]);
		t_value = getCardValue(handCards[i]);
		//���Һ�6
		if (t_hongLiuCount == 1 && t_color_type == COLOR_HONG && t_value == 6)
		{
			++t_sanHuaLiuCount;
			t_hongLiuCount = 0;
			continue;
		}
		//���Һ�6
		else if (t_heiLiuCount == 1 && t_color_type == COLOR_HEI && t_value == 6)
		{
			++t_sanHuaLiuCount;
			t_heiLiuCount = 0;
			continue;
		}
		//���Ҵ���
		else if (t_daWangCount == 1 && handCards[i] == 0x4F)
		{
			++t_sanHuaLiuCount;
			t_daWangCount = 0;
			continue;
		}
	}
	if (t_sanHuaLiuCount == 3) return true;

	return false;
}

/* ����Ƿ�Ϊ�����ɶ� */
bool CXGameLogic::checkSanHuaDouble(const int* handCards, int handCardsCount)
{
	if (handCards == NULL || handCardsCount != 4 && !checkSanHuaType(handCards, handCardsCount))
	{
		return false;
	}

	int t_color_type = COLOR_NT;
	int t_value = 0;

	//�ж��Ƿ�Ϊ����ʮ�ɶԣ���10����10����J
	int t_hongShiCount = 0;
	int t_heiShiCount = 0;
	int t_heiGouCount = 0;
	for (int i = 0; i < handCardsCount; ++i)
	{
		t_color_type = getCardColorType(handCards[i]);
		t_value = getCardValue(handCards[i]);
		//���Һ�10
		if (t_color_type == COLOR_HONG && t_value == 10)
		{
			++t_hongShiCount;
			continue;
		}
		//���Һ�10
		else if (t_color_type == COLOR_HEI && t_value == 10)
		{
			++t_heiShiCount;
			continue;
		}
		//���Һ�J
		else if (t_color_type == COLOR_HEI && t_value == 11)
		{
			++t_heiGouCount;
			continue;
		}
	}
	if (t_hongShiCount + t_heiShiCount + t_heiGouCount == 4)
	{
		return true;
	}

	//�ж��Ƿ�Ϊ�������ɶԣ���6����6������
	int t_hongLiuCount = 0;
	int t_heiLiuCount = 0;
	int t_daWangCount = 0;
	int t_hongSanCount = 0;
	for (int i = 0; i < handCardsCount; ++i)
	{
		t_color_type = getCardColorType(handCards[i]);
		t_value = getCardValue(handCards[i]);
		//���Һ�6
		if (t_color_type == COLOR_HONG && t_value == 6)
		{
			++t_hongLiuCount;
			continue;
		}
		//���Һ�6
		else if (t_color_type == COLOR_HEI && t_value == 6)
		{
			++t_heiLiuCount;
			continue;
		}
		//���Ҵ���
		else if (handCards[i] == 0x4F)
		{
			++t_daWangCount;
			continue;
		}
		//���Һ���3
		else if (handCards[i] == 0x23)
		{
			++t_hongSanCount;
		}
	}

	if (t_hongLiuCount + t_heiLiuCount + t_daWangCount + t_hongSanCount == 4)
	{
		return true;
	}

	return false;
}

/* У���������Ƿ�һ�� */
bool CXGameLogic::checkTwoHandCardsIsSame(const int* srcCards, const int* desCards, int cardCount)
{
	if (cardCount < 1) return false;

	bool t_flag = false;
	for (int i = 0; i < cardCount; ++i)
	{
		t_flag = false;
		for (int j = 0; j < cardCount; ++j)
		{
			if (srcCards[i] == desCards[j])
			{
				t_flag = true;
				break;
			}
		}
		if (!t_flag)
		{
			return false;
		}
	}
	return true;
}

/* ���������е������ƣ������ǰ */
bool CXGameLogic::sortHandCardsByDouble(int* handCards, int cardCount, int* handCardsType)
{
	if (handCards == NULL || cardCount != 4)
	{
		return false;
	}

	int t_perType = getDoubleCardsTypeValue(handCards[0], handCards[1]);
	int t_afterType = getDoubleCardsTypeValue(handCards[2], handCards[3]);
	handCardsType[0] = t_perType;
	handCardsType[1] = t_afterType;
	int t_card1 = 0;
	int t_card2 = 0;
	if (t_afterType > t_perType)
	{
		t_card1 = handCards[0];
		t_card2 = handCards[1];
		handCards[0] = handCards[2];
		handCards[1] = handCards[3];
		handCards[2] = t_card1;
		handCards[3] = t_card2;

		handCardsType[0] = t_afterType;
		handCardsType[1] = t_perType;
	}
	return true;
}

/* ���յڶ����ʹ�С�������в�����ҵ����ƣ��������Ƚ� */
bool CXGameLogic::sortAllPlayerCards(HandCardsTypeInfo info[], int infoCount, int firstDangPos, int playerCount)
{
	if (info == NULL || infoCount < 1 || playerCount <= 0)
	{
		return false;
	}

	for (int i = 0; i < infoCount - 1; ++i)
	{
		for (int j = 0; j < infoCount - i - 1; ++j)
		{
			//���ڶ���������
			if (info[j].m_secondType < info[j + 1].m_secondType)
			{
				HandCardsTypeInfo t_tmp = info[j];
				info[j] = info[j + 1];
				info[j + 1] = t_tmp;
			}
			//����ڶ�������ͬ������һ���ʹ�С����
			else if (info[j].m_secondType == info[j + 1].m_secondType && info[j].m_firstType < info[j + 1].m_firstType)
			{
				HandCardsTypeInfo t_tmp = info[j];
				info[j] = info[j + 1];
				info[j + 1] = t_tmp;
			}
			//����ڶ����ͺ͵�һ���Ͷ���ͬ�����뷢���˽����������ǰ��
			else if (info[j].m_secondType == info[j + 1].m_secondType && info[j].m_firstType == info[j + 1].m_firstType)
			{
				//�뵲�������λ��
				int t_dangJinPos = firstDangPos;
				for (int k = firstDangPos; k < infoCount + firstDangPos; ++k)
				{
					int t_pos = k % playerCount;
					if (t_pos == j)
					{
						t_dangJinPos = j;
						break;
					}
					if (t_pos == j + 1)
					{
						t_dangJinPos = j + 1;
						break;
					}
				}

				//j+1�뵲�ҽ�����j+1����ǰ��
				if (t_dangJinPos == j + 1)
				{
					HandCardsTypeInfo t_tmp = info[j];
					info[j] = info[j + 1];
					info[j + 1] = t_tmp;
				}
			}
		}
	}

	int t_sameBegin = 0;
	int t_sameCount = 0;
	for (int i = 0; i < infoCount - 1; ++i)
	{
		if (info[i].m_firstType == info[i + 1].m_firstType && info[i].m_secondType == info[i + 1].m_secondType)
		{
			t_sameBegin = i;
			t_sameCount = 0;

			//����֮���ּ�����ͬ��
			for (int j = t_sameBegin; j < infoCount - 1; ++j)
			{
				if (info[j].m_firstType == info[j + 1].m_firstType && info[j].m_secondType == info[j + 1].m_secondType)
				{
					++t_sameCount;
				}
				else
				{
					break;
				}
			}

			for (int j = t_sameBegin; t_sameCount >= 0; ++j)
			{
				info[j].m_isSameNext = t_sameCount + 1;
				--t_sameCount;
				++i;
			}
		}
	}
}

/* ��������ƶ�Ӧ��λ�� */
int CXGameLogic::getMaxCardPos(const int* srcCards, int cardCount, int playerCount, int firstSpeckPos)
{
	if (cardCount <= 0 || srcCards == NULL)
	{
		return -1;
	}
	int t_maxCard = 0;
	for (int i = 0; i < cardCount; ++i)
	{
		if (getLogicCardValue(srcCards[i]) > getLogicCardValue(t_maxCard))
		{
			t_maxCard = srcCards[i];
		}
	}

	for (int i = firstSpeckPos; i < playerCount + firstSpeckPos; ++i)
	{
		int t_pos = i % playerCount;
		if (getLogicCardValue(t_maxCard) == getLogicCardValue(srcCards[t_pos])) return t_pos;
	}
	return 0;
}

/* ���������Ƶĵ��� */
int CXGameLogic::getDoubleCardsNum()
{
	return 0;
}

/* ϴ�� */
void CXGameLogic::RandCardList(int cardBuffer[], int cardBufferCount)
{
	//����׼��
	int cbCardData[CHEXUAN_CELL_PACK];

	memcpy(cbCardData, m_cardsPool, sizeof(m_cardsPool));

	//�����˿�
	int cbRandCount = 0, cbPosition = 0;
	do
	{
		cbPosition = rand() % (cardBufferCount - cbRandCount);
		cardBuffer[cbRandCount++] = cbCardData[cbPosition];
		cbCardData[cbPosition] = cbCardData[cardBufferCount - cbRandCount];
	} while (cbRandCount < cardBufferCount);

	return;
}