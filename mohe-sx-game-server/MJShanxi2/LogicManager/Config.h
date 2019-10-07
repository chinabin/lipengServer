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

	Lstring GetCenterIp();
	Lshort GetCenterPort();
	Lstring GetCenterKey();


	Lstring GetDBIp();
	Lshort GetDBPort();
	Lstring GetDBKey();

	Lstring	GetDbHost();
	Lstring	GetDbUser();
	Lstring	GetDbPass();
	Lstring	GetDbName();
	Lshort	GetDbPort();


	Lstring GetInsideIp();
	Lshort GetInsidePort();
	bool	GetDebugModel();
	Lint	GetServerID();
	Lstring	GetServerName();

	Lint    GetTime();

	// �ϴ���־���
	Lstring GetRemoteLogUrl();
	Lint GetMaxCachedLogSize();
	Lshort GetMaxCachedLogNum();

	// ������������
	Lint GetLogicLimitDeskCount();
private:
	LIniConfig	m_ini;
private:
	Lshort		m_inPort;
	Lshort		m_outPort;

	Lstring		m_inIp;
	Lstring		m_outIp;
	Lint		m_DebugMod;
	Lint		m_ServerID;						//������ID          
};

#define gConfig Config::Instance()

#endif