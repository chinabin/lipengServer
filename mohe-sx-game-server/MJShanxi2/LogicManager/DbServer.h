#ifndef _DB_SERVER_H_
#define _DB_SERVER_H_

#include "LDBSession.h"
#include "LRunnable.h"

//db������
class DbServer : public LRunnable
{
public:
	DbServer();

protected:
	virtual bool		Init();
	virtual bool		Final();

	virtual void		Run();
	virtual void		Clear();

	void				Excute( const std::string& sql );

private:
	LDBSession* m_dbsession;
};

#endif