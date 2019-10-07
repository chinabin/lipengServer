#ifndef _BASE_COMMON_FUNC_
#define _BASE_COMMON_FUNC_

#include "LBuff.h"
#include "LSocket.h"
#include "LRunnable.h"
#include "LMsg.h"

#ifdef WIN32

API_LBASESX void disableConsoleCloseButton();

#endif

API_LBASESX Lstring caculateMd5ToHex(const void* pData, unsigned int uDataLen);

/*
�����ͻ��˷���������Ϣ

��Ϣ��ʽ��
|--2 bytes(��֤ͷ����)--|--N bytes(��֤ͷ)--|--2 bytes(��Ϣʵ�峤��)--|--1 byte(�Ƿ񾭹����)--|--N bytes(��Ϣʵ��)--|
*/
API_LBASESX bool parserClientMsgPack(LBuffPtr recv,
						 boost::shared_ptr<LMsgC2SVerifyHead>& msgVerifyHead, 
						 boost::shared_ptr<LMsg>& msgEntity,
						 boost::shared_ptr<LBuff>& msgOriginData);

//Format
// |--1 byte(isPacket)--|--N bytes(data)--|
API_LBASESX LMsg* parserMsgPack(LBuffPtr recv, LSocketPtr s);

API_LBASESX void disableIP(std::string strIP);

API_LBASESX unsigned int caculateHashKey(const char* pchData, int iDataLen);

//�����������
API_LBASESX void updateRandomFactor();

///////////////////////Function///////////////////////////////////////////////////

API_LBASESX std::string convertInt2String(int iValue);
API_LBASESX std::string convertUInt2String(unsigned int uValue);

#endif