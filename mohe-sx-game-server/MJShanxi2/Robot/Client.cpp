// Client.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "Net_Server_Inside.h"
#include "ClientManager.h"
#include "Public_func.h"

int _tmain(int argc, _TCHAR* argv[])
{
	SetLog();
	SetNet();
	NetJoin();
	return 0;
}

