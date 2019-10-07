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
	Lint    GetLoginFlag(); //0:�Ǹ߷���1:�߷�

	Lstring GetInsideIp();
	Lshort	GetInsidePort();

	Lstring GetOutsideIp();
	Lshort	GetOutsidePort();

	Lstring GetCenterIp();
	Lshort	GetCenterPort();
private:
	LIniConfig	m_ini;
};

#define gConfig Config::Instance()

#endif