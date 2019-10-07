#ifndef _D_USER_H_
#define _D_USER_H_


#include "LUser.h"

/************************************************************************************************
LUser �ֶ�˵��
1.  m_customInt[0]
2.  m_customIni[1]
3.  m_customInt[2]
4.  m_customInt[3] user -block �Ƿ���
5. m_customInit[4] user_info - level �û�����
6. m_customInit[5] userid, -��¼�ͻ��˱����userid����Ӯ������ʱ����--Ӧ���ڼ���userid  
7. m_customInit[6] userid, -��¼�ͻ��˱����userid����Ӯ������ʱ����--ϵͳ�ڼ���userid

//����ƽ��  5��  11-15
m_customInt[10]  --�ۼƷ���
m_customInt[11]  --ƽ�����
m_customInt[12]  --��������


1. m_customString1  IP
2. m_customString2 gps
3. m_customString3
4. m_customString4  phoneUUID,-��¼�ͻ����ϴ����ֻ�������,
5. m_customString5   ��¼�ͻ��˱����userid, ���userId �����ã���Ӯ������ʱ����   �ֻ�app�ڼ�¼userid
6. m_customString6  ��¼�ͻ��˱����userid, ���userId �����ã���Ӯ������ʱ����    �ֻ�ϵͳ�ڼ�¼userid
7. m_customString7  ��¼�ͻ���¼�õ��ֻ��ͺ�

8.m_customString8   phoneUUID��ǰ
9.m_customString9   ��¼�ͻ���¼�õ��ֻ��ͺ�  ��ǰ

************************************************************************************************/


class CSafeLock
{
public:
	CSafeLock()
	{
		m_uLockedSum = 0;
	}
	virtual ~CSafeLock(){}
protected:
	boost::thread::id m_lockedThreadId;
	unsigned int m_uLockedSum;
};

class DUser : public CSafeLock
{
	friend class CSafeUser;
public:
	DUser();
	virtual ~DUser();

	// ��ӷ���
	void	AddCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin);
	//ɾ������
	void	DelCardCount(Lint cardType, Lint count, Lint operType, const Lstring& admin);

	// ��ӽ��
	void	AddCoinsCount(Lint count, Lint operType);
	// ɾ�����
	void	DelCoinsCount(Lint count, Lint operType);

	void    ExchangeCardCoin(Lint add,Lint del,Lint operType,Lint cardType,const Lstring& admin);

	void    AddCreditValue();   //ֻ�������

	// ������Ҵ��ƵĴ���
	void	AddPlayCount();

	// �޸�new״̬ value��LMsgL2CeModifyUserNew�е�ע��
	void	ModifyNew( Lint value );
	
public:
	LUser m_usert;
	Lint  m_lastReqTime;	//���һ�������¼��ʱ��
	Lint  m_logicID;		//��ҵ�ǰ���ڷ�����Index
};

class CSafeUser
{
public:
	CSafeUser(boost::shared_ptr<DUser> user);
	~CSafeUser();
public:
	boost::shared_ptr<DUser>& getUser();
	bool isValid();
protected:
	void _lockUser();
	void _unlockUser();
private:
	boost::shared_ptr<DUser> m_User;
	bool m_bLocked;
private:
	static boost::recursive_mutex m_mutexLockUser;
private:
	CSafeUser(){}
};

#endif