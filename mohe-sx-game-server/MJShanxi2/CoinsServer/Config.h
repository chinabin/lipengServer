#ifndef _CONFIG_H_
#define _CONFIG_H_


#include "LIni.h"
#include "LSingleton.h"

class Config :public LSingleton<Config>
{
public:
	virtual	bool	Init();

	virtual	bool	Final();


	Lint	GetLogLevel();

	Lstring GetCoinsKey();
	Lstring GetInsideIp();
	Lshort	GetInsidePort();

	Lstring GetLogicManagerIp();
	Lshort	GetLogicManagerPort();

	// ��һ��Ƶ���ʱ
	Lint	GetChangeOutTime();

	// ��Ҳ�������ʱ ��λ��
	Lint	GetOpOutTime();

	// ��ҳ���������
	Lint	GetMaxUserCount();

	// ÿ�����ͽ�ҵĴ���
	Lint	GetMaxGiveCount();

	// ÿ�����ͽ�ҵ�����
	Lint	GetGiveCoinsCount();

	// �Ƿ�������������ͽ��
	bool	GetIsGiveCoins();

	// ���뷿����С�Ľ����
	Lint	GetMinCoins();

	// �׷�
	Lint	GetBaseScore();

	// �Ƿ����ģʽ
	Lint	GetDebugModel();

private:
	boost::mutex m_mutex;
	LIniConfig	m_ini;
};

#define gConfig Config::Instance()

#endif