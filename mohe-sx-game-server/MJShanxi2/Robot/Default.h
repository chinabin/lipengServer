#pragma once
#include <sstream>
#include <string>
#include "LBase.h"
#include "LVideo.h"
#include "LMsg.h"
#include "..\LogicServer\Card.h"

const Lint TimeSpace_Login			= 10;
const Lint TimeSpace_Reconnect		= 60;
const Lint TimeSpace_HeartbeatTime	= 5;
const Lint TimeSpace_Ready			= 0;
struct TimeSpace
{
	int Test;
	int	Login;
	int	HeartbeatTime;
	int	DeskOpTime;
	int	OutCard;
	int	OutThink;
	int Wait;
	void Init()
	{
		memset(this,0,sizeof(TimeSpace));
	}
};
#define TimeSpaceCheck(A,B)	if (time(NULL)< ((A) + (B)))\
								return;\
							A=time(NULL);


const Lint RoomAddRobot		= 0;
const Lint HardCardCount	= 14;


enum User_State
{
	User_State_Connect_Nothing = 0,	// ��ʼ״̬;
	User_State_Connect_Center,		// ����Center;
	User_State_Connect_Center_Wait,	// 
	User_State_Connect_Gete,		// ����Gate;
	User_State_Connect_Gete_Wait,	//
	User_State_Hall,				// ������;
	User_State_Game_GoReady,		// ��Ϸ�ȴ�������;
	User_State_Game_Wait,			// ��Ϸ�� - �ȴ�;
	User_State_Game_Get_Card,		// ��Ϸ�� - ����;
	User_State_Game_THINK,			// ��Ϸ�� - ˼��;
	User_State_Game_StartHu,		// ��Ϸ�� - ���ֺ�;
	User_State_Game_EndSelect,		// ��Ϸ�� - ������
	User_State_Desk_Exit,			// ����;
};

struct User_Play_Info
{
	Lint					m_deskId;	// ����;
	Lint					m_pos;		// �ҵ�Pos;
	Lint					m_ready;	// �Ƿ�׼����;
	Lint					m_CurPos;	// ��ǰ��������;
	std::vector<Card>		m_cardValue;
	std::vector<ThinkData>	m_think;//˼��������;
	User_State				m_state;
	bool					m_DeskBegin;
	bool					m_HaiDi;
	int						m_WaitCount;
	std::vector<StartHuInfo> m_startHu;	//���ֺ�����
	void Init()
	{
		Init_DeskOver();
		m_state = User_State_Connect_Center;
	}
	void Init_GameOver()
	{
		m_ready = 0;
		m_CurPos = 4;
		m_cardValue.clear();
		m_think.clear();
		m_startHu.clear();
		m_state = User_State_Game_GoReady;
		m_DeskBegin = false;
		m_HaiDi = false;
	}
	void Init_DeskOver()
	{
		Init_GameOver();
		m_deskId = -1;
		m_pos    = 4;
		m_state = User_State_Hall;
	}
};

struct DeskInfo
{
	Lint Index;
	std::vector<Lstring> UserLise;
};
