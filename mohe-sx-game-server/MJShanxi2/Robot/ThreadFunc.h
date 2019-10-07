#pragma once
#include "Net_Server_Inside.h"
#include "ClientManager.h"

void Net_Thread_Join() 
{
	gServer_InsideNet.Join();
}

void Net_Thread_Connect() 
{
	while (1)
	{
		gServer_UserManager().Connect();
		Sleep(1);
	}
}

void Net_Thread_Update() 
{
	while (1)
	{
		gServer_UserManager().Update();
		Sleep(1);
	}
}

void Net_Thread_Print() 
{
	while (1)
	{
		static int aaa=0;
		static int bbb=0;
		if (aaa!=Msg_Count || bbb!=Msg_Count_Send)
		{
			aaa=Msg_Count;
			bbb=Msg_Count_Send;
			std::cout
				<<"���ӣ�"				<<gServer_UserManager().m_ConnectCount
				<<"	���ͣ�"				<<Msg_Count_Send
				<<"	���أ�"				<<Msg_Count
				<<"	δ���أ�"			<<Msg_Count_Send-Msg_Count
				<<"	���ݴ���"			<<Msg_Count_Err
				<<std::endl;
		}
		Sleep(1000);
	}
}