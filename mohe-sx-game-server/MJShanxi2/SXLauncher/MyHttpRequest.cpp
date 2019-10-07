// MyHttpRequest.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SXLauncher.h"
#include "MyHttpRequest.h"


// CMyHttpRequest

CMyHttpRequest::CMyHttpRequest()
{
	
}

CMyHttpRequest::~CMyHttpRequest()
{
}

size_t httpWriteCallBack(char *ptr, size_t size, size_t nmemb, void *data)
{
	size_t res_size = size * nmemb;
	if (res_size > 1024 * 64)
		return 0;
	char *res_buf = (char*)(data);
	memcpy(res_buf, ptr, res_size);
	return res_size;
}

// CMyHttpRequest ��Ա����
bool CMyHttpRequest::InvokeGet(const std::string& httpurl, std::string& result, bool isHttps, int timeout)
{
	CURL *curl = curl_easy_init();
	if (curl == NULL)
	{		
		return false;
	}

	curl_easy_setopt(curl, CURLOPT_URL, httpurl.c_str());
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout);	//���ó�ʱ
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);        //���������ź�
	if (isHttps)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
	}
	char recvBuff[1024 * 64];
	memset(recvBuff, 0, sizeof(recvBuff));
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, httpWriteCallBack);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, recvBuff);
	CURLcode res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);

	if (res != CURLE_OK)
	{		 
		return false;
	}
	else
	{
		recvBuff[sizeof(recvBuff) - 1] = 0;
		result = recvBuff;
		return true;
	}
	return true;
}
