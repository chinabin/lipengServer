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

	Lstring GetLogicDBIp();
	Lshort	GetLogicDBPort();
	Lstring GetLogicDBKey();

	Lstring GetRedisIp();
	Lshort	GetRedisPort();

	Lstring	GetDbHost();
	Lstring	GetDbUser();
	Lstring	GetDbPass();
	Lstring	GetDbName();
	Lshort	GetDbPort();

	Lstring	 GetBKDbHost();
	Lstring	 GetBKDbUser();
	Lstring	 GetBKDbPass();
	Lshort GetBKDbPort();

private:
	LIniConfig	m_ini;
};

#define gConfig Config::Instance()

#endif