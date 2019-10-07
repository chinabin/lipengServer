#ifndef _ACTIVE_MANAGER_H
#define _ACTIVE_MANAGER_H

#include "LSingleton.h"
#include "LMsgS2S.h"
#include "LRedisClient.h"

// ת��
#define ActivityDraw_Config "ActivityDraw_Config"						// ����
#define ActivityDraw_RemainTotalCount_ "ActivityDraw_RemainTotalCount_"	// ʣ��������
#define ActivityDraw_RemainTotalDate "ActivityDraw_RemainTotalDate"		// ��¼ʣ��������

#define ActivityDraw_WinUsers "ActivityDraw_WinUsers"	// �е��н���¼key
#define ActivityDraw_WinUsersCount	300					// �ܵ��н� ��¼����
#define ActivityDraw_Win_ "ActivityDraw_Win_"			// �����н���¼key
#define ActivityDraw_WinCount	100						// ���� ��¼������

// ����
#define ActivityShare_Config "ActivityShare_Config"

struct Activity
{
	Activity()
	{
		m_ActiveID = 0;
		m_BeginTime = -1;
		m_EndTime = -1;
	}

	Lint	m_ActiveID;		// ActivityID����
	Lstring	m_ActiveName;
	Lstring	m_Desc;
	Lint  m_BeginTime;
	Lint  m_EndTime;

	Lstring m_strConfig;		// ��̨�����ļ�
};

struct ActivityDrawGift
{
	ActivityDrawGift()
	{
		m_GiftIndex = -1;
		m_GiftType = 0;
		m_GiftCount = 0;
		m_GiftPercent = 0;
		m_GiftTotalCount = 0;
		m_GiftRemainTotalCount = 0;
		m_GiftEnsure = false;
		m_GiftResumeNextDay = false;
	}
	Lint m_GiftIndex;			// �������� �Զ�����
	Lstring m_GiftName;			// ��������
	Lint m_GiftType;			// �������� 1��������λ���� 2���ѣ���λԪ�� 3 ���ӵ��ӣ���λ���� 4 IPad min4����λ���� 5 iPhone7 plus����λ����
	Lint m_GiftCount;			// ��������
	Lint m_GiftPercent;			// ����Ȩֵ
	Lint m_GiftTotalCount;		// ���õĿ�� -1��ʾ����
	Lint m_GiftRemainTotalCount;// ʣ��Ŀ��  -1��ʾ����
	bool m_GiftEnsure;			// �������Ϊ0ʱ ȷ���еĽ�Ʒ
	bool m_GiftResumeNextDay;	// �����Ƿ�ָ����
};

struct ActivityDraw : public Activity
{
	ActivityDraw()
	{
		m_SpendType = 0;
		m_SpendCount = 0;
		m_GiftTotalDate = 0;
		m_GiftTime = 0;
		m_TotalGiftPercent = 0;
	}

	Lint m_SpendType;			// �������� 0��� 1��������λ����
	Lint m_SpendCount;			// ��������
	Lint m_GiftTotalDate;		// �����־ ���ڸ��ջָ�
	Lint m_GiftTime;			// ��ʾ�齱�Ĵ���
	std::vector<ActivityDrawGift> m_Gift;
	Lint m_TotalGiftPercent;	// �н��ܵ�Ȩֵ
};

struct ActivityShare : public Activity
{
	ActivityShare()
	{
		m_GiftType = 0;
		m_GiftCount = 0;
	}

	Lstring m_GiftName;			// ��������
	Lint m_GiftType;			// ������ 1��������λ����
	Lint m_GiftCount;			// ��������
};

class ActiveManager:public LSingleton<ActiveManager>
{
public:
	virtual	bool		Init();
	virtual	bool		Final();

	// ת�̽ӿ�====================================================
	// ����ת������ �ڲ�����������ͱ���
	void SetDrawActiveConfig(const Lstring& strSet,int id);
	// ���ת������
	void DrawSetClear();
	// ����ת������ �ڲ�����
	void _UpdateDrawActiveSet(const Lstring& strSet,int id);
	// �����ȡת������
	void SaveDrawActiveSet();
	void ReadDrawActiveSet();
	// �Ƿ���ת��
	bool CheckDrawActiveValid();
	// ת��ID
	int GetDrawActiveId();
	int GetDrawBeginTime();
	int GetDrawEndTime();
	// ת������
	int GetDrawSpendType();
	int GetDrawSpendCount();
	// ����ת���н� fGift��ʾҪ���ɵļ�����
	ActivityDrawGift GetDrawGift( ActivityDrawGift& fGift );

	// ����ӿ�
	void UpdateShareActiveSet(const Lstring& strSet,int id);
	Lstring GetShareActiveSet(); 
	int GetShareActiveId();

	ActivityShare GetShareSet();

	bool CheckShareActiveValid();
	bool CheckShareActiveStart();
	void ShareSetClear();

	inline CRedisClient & GetRedisClient() { return m_RedisClient; }

private:
	boost::recursive_mutex m_Mutex;
	CRedisClient m_RedisClient;

	ActivityDraw m_Draw;
	ActivityShare m_Share;
};

#define gActiveManager ActiveManager::Instance()



#endif

