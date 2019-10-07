#ifndef _L_CHARGE_H_
#define _L_CHARGE_H_

#include "LBuff.h"
#include "LLog.h"

//��������
enum CARD_TYPE
{
	CARD_TYPE_4 = 1,//4�ֿ�
	CARD_TYPE_8 = 2,//8�ֿ�
	CARD_TYPE_16 = 3,//16�ֿ�
	CARD_TYPE_Other = 100,   //����
};

// ����۷���������
// LogicServer LogicManager CenterServer �� DelCardCount ����������
inline int GetCardDelCount(int cardType, int count) {
	switch (cardType) {
	case CARD_TYPE_4: return 1;
	case CARD_TYPE_8: return 2;
	case CARD_TYPE_16: return 4;
	case CARD_TYPE_Other: return count;
	default: return 0;
	}
}
inline int DelCardCount(int balance, int cardType, int count) {
	int delCount = GetCardDelCount(cardType, count);
	LLOG_DEBUG("User::DelCardCount  delCount = %d", delCount);
	if (balance > delCount) balance -= delCount;
	else balance = 0;
	return balance;
}

enum CARDS_OPER_TYPE
{
	CARDS_OPER_TYPE_INIT = 0,//��������
	CARDS_OPER_TYPE_CHARGE = 1,//��ֵ�
	CARDS_OPER_TYPE_FREE_GIVE = 2,//�������
	CARDS_OPER_TYPE_CREATE_ROOM = 3,//��������۳�
	CARDS_OPER_TYPE_ACTIVE = 4,		//��ͷ���
	CARDS_OPER_TYPE_INVITED = 5,    //�������
	CARDS_OPER_TYPE_EXCHANGE = 6,   //�����
	CARDS_OPER_TYPE_ACTIVE_DRAW = 7,//ת������
};

enum COINS_OPER_TYPE
{
	COINS_OPER_TYPE_DEBUG = -1,//��������
	COINS_OPER_TYPE_CHARGE = 0,//��ֵ
	COINS_OPER_TYPE_FREE_GIVE = 1,//�������
	COINS_OPER_TYPE_RESULT = 2,//�������
	COINS_OPER_TYPE_SELF_GIVE = 3,	//����Լ���ȡ
	COINS_OPER_TYPE_CHANGE_CARD = 4,   //����
};

class API_LBASESX LCharge
{
public:
	LCharge();
	~LCharge();

	void	Read(LBuff& buff);
	void	Write(LBuff& buff);
public:
	Lint	m_time;
	Lint	m_cardType;
	Lint	m_cardNum;
	Lint	m_type;
	Lint	m_userId;
	Lstring	m_admin;
};
#endif