/*************************************************************************
	> File Name: RLogHttp.h
	> Author: 
	> Mail: 
	> Created Time: Mon 24 Oct 2016 02:00:03 PM PDT
 ************************************************************************/

#ifndef _RLOGHTTP_H
#define _RLOGHTTP_H

#include <libcurl/curl.h>
#include <sstream>
#include "LBase.h"
#include "LSingleton.h"

// ������һ��\n ����
#define RLOG(type, content)  do { \
	std::basic_ostringstream<char> bo; \
	bo << type; \
	bo << "|"; \
	bo << content; \
	bo << '\n'; \
	gRLT.Push( new Lstring(bo.str()) ); \
} while(0)

class API_LBASESX RLogHttp : public LSingleton<RLogHttp>, public LRunnable
{
public:
	RLogHttp();
	virtual ~RLogHttp();

	virtual bool Init();
	virtual bool Final();

	bool Init(const std::string& url, size_t max_cached_log_size, size_t max_cached_log_num);

	void SetOpen( bool bopen ) { bopenrlog = bopen; }
	void SetURL( const std::string& url );

	virtual void Push(void* msg);

private:
	virtual void Clear() {};

	virtual void Run(void);

	int rlog(const Lstring& log);
	int sendLog();

	bool bopenrlog;	// ��¼�Ƿ���Զ����־����

	CURL* curl;
	curl_slist* list;

	// ������Ϣ
	std::string m_url;
	size_t m_max_cached_log_size;
	size_t m_max_cached_log_num;

	// ��������
	char* m_buff;

	// ��¼��ǰ����ĳ��Ⱥ�����
	size_t m_cached_size;
	size_t m_cached_log_num;
};

#define gRLT RLogHttp::Instance()

#endif
