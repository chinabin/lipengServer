#ifndef _CHE_XUAN_H_
#define _CHE_XUAN_H_
#pragma once

#include "mhgamedef.h"
#include <cstdlib>
#include <time.h>
#include <vector>
#include <algorithm>

//����ά��
#define CountArray(Array) (sizeof(Array)/sizeof(Array[0]))

//��Ч����
#define CT_ERROR					0									//��������
#define VALUE_ERROR					0x00								//��ֵ����
#define COLOR_ERROR					0xFF								//��ɫ����

//��ɫ����
#define COLOR_NT					0x40								//��ɫ���壺����
#define COLOR_HEI_TAO				0x30								//��ɫ���壺����
#define COLOR_HONG_TAO				0x20								//��ɫ���壺����
#define COLOR_MEI_HUA				0x10								//��ɫ���壺÷��
#define COLOR_FANG_KUAI				0x00								//��ɫ���壺����

#define COLOR_HONG					0									//��ɫ�죺���� & ����
#define COLOR_HEI					1									//��ɫ�ڣ�÷�� & ����

//��ֵ����
#define	LOGIC_MASK_COLOR			0xF0								//��ɫ����
#define	LOGIC_MASK_VALUE			0x0F								//��ֵ����

/* ������Ϣ��������ṹ */
struct CalcHandCardsInfo
{
	/* ���ƣ��Ż��������ѡ����Ϻ�ǰ���߼���С���򣬺����߼���С���� */
	int m_handCards[CHEXUAN_HAND_CARDS_COUNT];

	/* �������� */
	int m_handCardsCount;

	/* �����߼�ֵ��������ͬ��ˢ�� */
	int m_handCardsLogic[CHEXUAN_HAND_CARDS_COUNT];

	/* �������ͣ�ǰ���������ͣ������������� */
	int m_handCardsType[2];

	/* ������ǰ����������������*/
	int m_handCardsNum[2];
};

/*
���ţ��������ͣ��߼�ֵ��
��С���ϵ������У�ͬһ�鲻�ִ�С
�죺����Q  ����Q
�أ�����2  ����2
�ˣ�����8  ����8
�ͣ�����4  ����4
÷������
÷��÷ʮ����÷��10  ����10
�壨��ʣ���÷��4   ����4
������������÷��6   ����6

��ʮè�ࣺ
������ͷ����÷��J   ����J
ʮ����ʮ��������10  ����10
è��èè��������6   ����6
�ࣨ��ҩ��������7   ����7
�����ģ�
÷��9   ����9
÷��8   ����8
÷��7   ����7
÷��5   ����5
����������3
���ʣ�����
��ֵ��Ĵ�
*/
enum SINGLE_TYPE
{
	STYPE_TIAN = 7,					/* �죺��Q */
	STYPE_DI = 6,					/* �أ���2 */
	STYPE_REN = 5,					/* �ˣ���8 */
	STYPE_HE = 4,					/* �ͣ���4 */
	STYPE_MEI_BAN_SAN = 3,			/* ÷��������10 | ��4 | ��6 */
	STYPE_FU_SHI_MAO_GAO = 2,		/* ��ʮè�ࣺ��J | ��10 | ��6 | ��7 */
	STYPE_XIA_SI_LAN = 1,			/* �����ģ���9 | ��8 | ��7 | ��5 | ����3(����) | ����(����) */
	STYTE_ERROR = -1,				/* ��Ч���� */
};

/* ���ţ���������*/
enum DOUBLE_TYPE
{
	DTYPE_DINGERHUANG = 6,			/* �����ʣ�����3 + ���� */
	DTYPE_DUIZI = 5,				/* ���ӣ�ͬɫ��ֵ�������� */
	DTYPE_WANG = 4,					/* �������� + ��9 */
	DTYPE_TIANGANG = 3,				/* ��ܣ����� + ��/��8 */
	DTYPE_DIGANG = 2,				/* �ظܣ����� + ��/��8 */
	DYTPE_DIAN = 1,					/* �㣺����������������ͳ�Ϊ�㣬�ȵ���������12�㣬��ͷ11�㣬���6�㣬�����Ϊ�䱾�����*/
	DYTPE_ERROR = -1,				/* */
};

enum DOUBLE_TYPE_VALUE
{
	DTV_MIN_INVALID = -1,

	DTV_DIAN_LING,					//��㲻�ִ�С

	DTV_DIAN_DING_BA_YI,			//����һ������3��������+ ��8
	DTV_DIAN_FU_SHI_YI,				//��ʮһ����J����ͷ��+ ��10����ʮ��|| ��6��èè��+ ��5
	DTV_DIAN_MEI_SHI_YI,			//÷ʮһ����10��÷ʮ��+ ��J����ͷ��|| ��4����ʣ�+ ���7 || ��6��������+ ��5
	DTV_DIAN_HE_SHI_YI,				//��ʮһ����4�����ƣ�+ ���7
	DTV_DIAN_REN_SHI_YI,			//��ʮһ����8�����ƣ�+ ����3��������
	DTV_DIAN_DI_JIU_YI,				//�ؾ�һ����2�����ƣ�+ ��9

	DTV_DIAN_DING_JIU_ER,			//���Ŷ�������3��������+ ��9 || ��7 + ��5
	DTV_DIAN_GAO_YAO_ER,			//��ҩ������7����ҩ��+ ��5
	DTV_DIAN_SI_BA_ER,				//�İ˶�����4����ʣ�+ ��8 || ��6��������+ ��6��èè��
	DTV_DIAN_HE_SHI_ER,				//��ʮ������4�����ƣ�+ ��8
	DTV_DIAN_REN_SHI_ER,			//��ʮ������8�����ƣ�+ ���4
	DTV_DIAN_DI_SHI_ER,				//��ʮ������2�����ƣ�+ ���10
	DTV_DIAN_TIAN_SHI_ER,			//��ʮ������Q�����ƣ�+ ���10

	DTV_DIAN_WU_BA_SAN,				//���������8 + ��5
	DTV_DIAN_TIAO_SHI_SAN,			//��ʮ������10����ʮ��+ ����3��������|| ��6��èè��+ ���7
	DTV_DIAN_MEI_SHI_SAN,			//÷ʮ������10��÷ʮ��+ ����3��������|| ��4����ʣ�+ ��9 || ��6��������+ ���7
	DTV_DIAN_HE_SHI_SAN,			//��ʮ������4�����ƣ�+ ��9
	DTV_DIAN_REN_SHI_SAN,			//��ʮ������8�����ƣ�+ ��5
	DTV_DIAN_DI_FU_SAN,				//�ظ�������2�����ƣ�+ ��J����ͷ��
	DTV_DIAN_TIAN_FU_SAN,			//�츫������Q�����ƣ�+ ��J����ͷ��

	DTV_DIAN_WU_JIU_SI,				//����ģ���9 + ��5
	DTV_DIAN_DING_FU_SI,			//�����ģ�����3��������+ ��J����ͷ��|| ��6��èè��+ ��8 || ��7����ҩ��+ ��7
	DTV_DIAN_MEI_SHI_SI,			//÷ʮ�ģ���10��÷ʮ��+ ��4����ʣ�|| ��6��������+ ��8
	DTV_DIAN_HE_SHI_SI,				//��ʮ�ģ���4�����ƣ�+ ���10
	DTV_DIAN_REN_SHI_SI,			//��ʮ�ģ���8�����ƣ�+ ���6
	DTV_DIAN_TIAN_DI_SI,			//��ع����ģ���Q�����ƣ�+ ��2�����ƣ�

	DTV_DIAN_QI_BA_WU,				//�߰��壺��7 + ��8
	DTV_DIAN_TIAO_SHI_WU,			//��ʮ�壺��10����ʮ��+ ��5 || ��6��èè��+ ��9 || ��7����ҩ��+ ��8
	DTV_DIAN_MEI_SHI_WU,			//÷ʮ�壺��10��÷ʮ��+ ��5 || ��4����ʣ�+ ��J����ͷ��|| ��6��������+ ��9
	DTV_DIAN_HE_FU_WU,				//�͸��壺��4�����ƣ�+ ��J����ͷ��
	DTV_DIAN_REN_SHI_WU,			//��ʮ�壺��8�����ƣ�+ ���7
	DTV_DIAN_DI_GUAN_WU,			//�ض��壺��2�����ƣ�+ ����3
	DTV_DIAN_TIAN_GUAN_WU,			//�춡�壺��Q�����ƣ�+ ����3

	DTV_DIAN_QI_JIU_LIU,			//�߾�������9 + ��7
	DTV_DIAN_FU_WU_LIU,				//����������J����ͷ��+ ��5 || ��10����ʮ��+ ��6��èè��|| ��7����ҩ��+ ��9
	DTV_DIAN_MEI_SHI_LIU,			//÷ʮ������10��÷ʮ��+ ���6 || ��6��������+ ��10����ʮ��
	DTV_DIAN_REN_GUAN_LIU,			//�˹�������8�����ƣ�+ ��8
	DTV_DIAN_DI_GUAN_LIU,			//�ع�������2�����ƣ�+ ���4
	DTV_DIAN_TIAN_GUAN_LIU,			//���������Q�����ƣ�+ ���4

	DTV_DIAN_FEI_ZHOU_QI,			//�����ߣ���9 + ��8
	DTV_DIAN_FU_MAO_QI,				//��è�ߣ���J����ͷ��+ ��6��èè�� || ��10����ʮ��+ ���7
	DTV_DIAN_MEI_SHI_QI,			//÷ʮ�ߣ���10��÷ʮ��+ ���7 || ��4����ʣ�+ ����3 || ��J����ͷ��+ ��6��������
	DTV_DIAN_HE_GUAN_QI,			//�͹��ߣ���4�����ƣ�+ ����3
	DTV_DIAN_REN_GUAN_QI,			//�˹��ߣ���8�����ƣ�+ ��9
	DTV_DIAN_DI_GUAN_QI,			//�ع��ߣ���2�����ƣ�+ ��5
	DTV_DIAN_TIAN_GUAN_QI,			//����ߣ���Q�����ƣ�+ ��5

	DTV_DIAN_DING_WU_BA,			//����ˣ�����3��������+ ��5
	DTV_DIAN_FU_QI_BA,				//���߰ˣ���J����ͷ��+ ���7 || ��10����ʮ��+ ��8
	DTV_DIAN_MEI_SHI_BA,			//÷ʮ�ˣ���10��÷ʮ��+ ��8
	DTV_DIAN_HE_GUAN_BA,			//�͹ٰˣ���4�����ƣ�+ ��4����ʣ�
	DTV_DIAN_REN_GUAN_BA,			//�˹ٰˣ���8�����ƣ�+ ���10
	DTV_DIAN_DI_GUAN_BA,			//�عٰˣ���2�����ƣ�+ ���6 || ��2�����ƣ�+ ����
	DTV_DIAN_TIAN_GUAN_BA,			//��ٰˣ���Q�����ƣ�+ ���6 || ��Q�����ƣ�+ ����

	DTV_DIAN_WU_LONG_JIU,			//�����ţ���J����ͷ��+ ��8 || ��10����ʮ��+ ��9 || ����3��������+ ��6��èè��
	DTV_DIAN_MEI_SHI_JIU,			//÷ʮ�ţ���10��÷ʮ��+ ��9 || ��4����ʣ�+ ��5 || ����3��������+ ��6��������
	DTV_DIAN_HE_GUAN_JIU,			//�͹پţ���4�����ƣ�+ ��5
	DTV_DIAN_REN_GUAN_JIU,			//�˹پţ���8�����ƣ�+ ��J����ͷ��
	DTV_DIAN_DI_GUAN_JIU,			//�عپţ���2�����ƣ�+ ���7
	DTV_DIAN_TIAN_GUAN_JIU,			//��پţ���Q�����ƣ�+ ���7

	DTV_DI_GANG,					//�ظܣ���2�����ƣ�+ ��8�����ƣ�|| ��2�����ƣ�+ ��8

	DTV_TIAN_GANG,					//��ܣ���Q�����ƣ�+ ��8�����ƣ�|| ��Q�����ƣ�+ ��8

	DTV_DI_JIU_WANG,				//�ؾ�������2�����ƣ�+ ��9

	DTV_TIAN_JIU_WANG,				//���������Q�����ƣ� + ��9

	DTV_XIA_SI_LAN_DUI,				//�����Ķԣ���9�ԡ���8�ԡ���7�ԡ���5��
	DTV_FU_SHI_MAO_GAO_DUI,			//��ʮè��ԣ���J�ԡ���10�ԡ���6�ԡ���7��
	DTV_MEI_BAN_SAN_DUI,			//÷�����ԣ���4�ԡ���6�ԡ���10��
	DTV_HE_DUI,						//���ƶԣ�����4 + ����4
	DTV_REN_DUI,					//���ƶԣ�����8 + ����8
	DTV_DI_DUI,						//���ƶԣ�����2 + ����2
	DTV_TIAN_DUI,					//���ƶԣ�����Q + ����Q

	DTV_DING_ER_HUANG,				//�����ʣ�����3 + ����

	DTV_MAX_INVALID,
};


struct HandCardsTypeInfo
{
	int m_userIndex;						//���λ��
	DOUBLE_TYPE_VALUE m_firstType;			//ǰ�����Ƶ��߼�ֵ
	DOUBLE_TYPE_VALUE m_secondType;			//�������Ƶ��߼�ֵ
	int m_pretHandCards[2];					//ǰ������
	int m_nextHandCards[2];					//��������
	int m_isSameNext;						//�Ƿ����һ������������

	HandCardsTypeInfo()
	{
		memset(this, 0, sizeof(this));
		m_userIndex = CHEXUAN_INVALID_POS;
	}

	bool operator > (HandCardsTypeInfo& right)
	{
		return this->m_secondType > right.m_secondType;
	}

	bool operator == (HandCardsTypeInfo& right)
	{
		return this->m_secondType == right.m_secondType;
	}

	bool operator < (HandCardsTypeInfo& right)
	{
		return this->m_secondType < right.m_secondType;
	}

	void operator = (HandCardsTypeInfo& right)
	{
		this->m_userIndex = right.m_userIndex;
		this->m_firstType = right.m_firstType;
		this->m_secondType = right.m_secondType;
		this->m_pretHandCards[0] = right.m_pretHandCards[0];
		this->m_pretHandCards[1] = right.m_pretHandCards[1];
		this->m_nextHandCards[0] = right.m_nextHandCards[0];
		this->m_nextHandCards[1] = right.m_nextHandCards[1];
		this->m_isSameNext = right.m_isSameNext;
	}

};

/* ��Ϸ�߼������� */
class CXGameLogic
{
private:

	/* �˿˶��� ֻʹ�����е�32���˿� */
	static int m_cardsPool[CHEXUAN_CELL_PACK];

	/* �Ƿ�����ؾ���(Ĭ�ϣ�������) */
	bool m_is_allow_di_jiu;

public:

	/* ���캯�� */
	CXGameLogic();

	/* �������� */
	virtual ~CXGameLogic();

public:

	/* ��ȡ�ؾ���ѡ�� */
	bool getDiJiu();

	/* ���õؾ���ѡ�� */
	void setDiJiu(bool diJiu);

	/* ��ȡ�Ƶ���ֵ */
	int getCardValue(int card);

	/* ��ȡ�ƵĻ�ɫ */
	int getCardColor(int card);

	/* ��ȡ��ɫ���ͣ�����&���� = ��  ÷��&���� = �� */
	int getCardColorType(int card);

	/* �Ƿ�Ϊ��Ч��ֵ */
	bool isValidCard(int card);

	/* ��ȡ�����߼�ֵ */
	SINGLE_TYPE getLogicCardValue(int card);

	/* ��ȡ�������� */
	DOUBLE_TYPE getDoubleCardsType();

	/* ��ȡ����ֵ */
	DOUBLE_TYPE_VALUE getDoubleCardsTypeValue(int card1, int card2);

	/* ����Ƿ�Ϊ�������� */
	bool checkSanHuaType(const int* handCards, int handCardsCount);

	/* ����Ƿ�Ϊ�����ɶ� */
	bool checkSanHuaDouble(const int* handCards, int handCardsCount);

	/* У���������Ƿ�һ�� */
	bool checkTwoHandCardsIsSame(const int* srcCards, const int* desCards, int cardCount);

	/* ���������е������ƣ������ǰ */
	bool sortHandCardsByDouble(int* handCards, int cardCount, int* handCardsType);

	/* ���յڶ����ʹ�С�������в�����ҵ����ƣ��������Ƚ� */
	bool sortAllPlayerCards(HandCardsTypeInfo info[], int infoCount, int firstDangPos, int playerCount);

	/* ��������ƶ�Ӧ��λ�� */
	int getMaxCardPos(const int* srcCards, int cardCount, int playerCount, int firstSpeckPos);

	/* ���������Ƶĵ��� */
	int getDoubleCardsNum();

	/* ϴ�� */
	void RandCardList(int cardBuffer[], int cardBufferCount);
};

#endif // !_CHE_XUAN_H_



