#include "UserMessage.h"
#include "LTime.h"
#include "LMemoryRecycle.h"
#include "UserManager.h"
#include "Work.h"
#include "DeskManager.h"
#include "Config.h"
#include "RobotManager.h"

CUserMessage::CUserMessage()
{
}

CUserMessage::~CUserMessage()
{

}

bool CUserMessage::Init()
{
	return true;
}

bool CUserMessage::Final()
{
	return true;
}

void CUserMessage::Clear()
{

}

void CUserMessage::Run()
{
	if(!this->Init())
	{
		LLOG_ERROR("CUserMessage init Failed!!!");
		return;
	}

	LTime msgTime;
	int iMsgCount = 0;
	int iElapseTime = 0;
	while(!GetStop())
	{
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 1)
		{
			m_tickTimer = cur;
		}

		LMsg* msg = (LMsg*)Pop();
		if(msg == NULL)
		{
			Sleep(1);
			iElapseTime++;
			continue;
		}
		else
		{
			HanderMsg(msg);
			m_memoryRecycle.recycleMsg(msg);

			//ÿһ������������ʱ��
			iMsgCount++;
			if(iMsgCount == 10000)
			{
				LTime now;
				LLOG_ERROR("User Logic: 10000 package needs %ld time", now.MSecs() - msgTime.MSecs() - iElapseTime);

				iMsgCount = 0;
				iElapseTime = 0;

				msgTime = now;
			}
		}
	}

	this->Final();
}

void CUserMessage::Start()
{
	m_memoryRecycle.Start();
	LRunnable::Start();
}

void CUserMessage::Stop()
{
	m_memoryRecycle.Stop();
	LRunnable::Stop();
}

void CUserMessage::Join()
{
	m_memoryRecycle.Join();
	LRunnable::Join();
}

void CUserMessage::HanderMsg(LMsg* msg)
{
	switch(msg->m_msgId)
	{
	case MSG_LMG_2_CN_ENTER_COIN_DESK:
		HanderEnterCoinDesk((LMsgLMG2CNEnterCoinDesk*)msg);
		break;
	case MSG_G_2_L_USER_MSG:
		HanderUserMsg((LMsgG2LUserMsg*)msg);
		break;
	case MSG_G_2_L_USER_OUT_MSG:
		HanderUserOutMsg((LMsgG2LUserOutMsg*)msg);
		break;
	case MSG_LMG_2_CN_USER_LOGIN:
		HanderUserLogin((LMsgLMG2CNUserLogin*)msg);
		break;
	case MSG_LMG_2_CN_GET_COINS:
		HanderGetCoins((LMsgLMG2CNGetCoins*)msg);
	case MSG_L_2_LMG_MODIFY_USER_STATE:
		HanderModifyUserState((LMsgL2LMGModifyUserState*)msg);
		break;
	default:
		break;
	}
}

void CUserMessage::HanderEnterCoinDesk(LMsgLMG2CNEnterCoinDesk* msg)
{
	if (msg == NULL)
	{
		return;
	}

	LMsgS2CEnterCoinsDeskRet send;
	Lint reenteruserid = 0;
	// ��ֹuser���� ���������̵߳�desk�ڲ�����userʱ��������
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserUUID(msg->m_strUUID);
		if(safeUser && safeUser->isValid())
		{
			UserPtr user = safeUser->getResource();

			if ( user->GetDeskRunID() == 0 )	// ���û�н����� ɾ����� �����������߼�
			{
				LLOG_INFO("Work::HanderEnterCoinDesk user already in desk %d:%d:%s", user->GetUserDataId(), user->GetUserGateID(),user->GetIp().c_str());
				gUserManager.DelUser( user->GetUserDataId() );
			}
			else if ( user->GetDeskRunID() != 0 && (user->GetDeskID() == 0 || user->GetUserLogicID() == 0) ) // ������½�������  GetUserLogicID = 0 ������Logic��������ס��
			{
				reenteruserid = user->GetUserDataId();
			}
			else
			{
				// ��ʱ����ڽ�������ڴ��� Ӧ���߶��ߵ��߼�
				LLOG_ERROR( "CUserMessage::HanderEnterCoinDesk in coins desk playing, userid=%d", msg->m_usert.m_id );
				send.m_errorCode = 2;
				user->Send( send );
				return;
			}
		}
	}

	if ( reenteruserid != 0 )
	{
		if ( gDeskManager.UserReEnterDesk( reenteruserid ) )
		{
			LLOG_INFO("Work::HanderEnterCoinDesk UserReEnterDesk %d:%d:%s usernum=%d", reenteruserid, msg->m_gateId, msg->m_ip.c_str(), gUserManager.GetUsersCount());
			send.m_errorCode = 0;
			boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserUUID(msg->m_strUUID);
			if(safeUser && safeUser->isValid())
			{
				UserPtr user = safeUser->getResource();
				user->Send( send );
			}
			return;
		}
		else
		{
			// ���û�����½����ɹ� ��ɾ����� �����������߼�
			LLOG_ERROR("CUserMessage::HanderEnterCoinDesk user reenterdesk fail, userid=%d", reenteruserid);
			_userOut( reenteruserid );
		}
	}
	

	UserPtr user(new User(msg->m_usert, msg->m_gateId, msg->m_ip));

	//����̫��
	if ( gUserManager.GetUsersCount() > gConfig.GetMaxUserCount() )
	{
		LLOG_ERROR( "CUserMessage::HanderEnterCoinDesk user too much, userid=%d", msg->m_usert.m_id );
		send.m_errorCode = 6;
		user->Send( send );
		return;
	}

	// �Ƿ�Ҫ���ý��
	if ( gConfig.GetDebugModel() && msg->m_coins > 0 )
	{
		Lint coins = msg->m_coins - msg->m_usert.m_coins;
		if ( coins > 0 )
		{
			user->AddCoinsCount( coins, COINS_OPER_TYPE_DEBUG );
		}
		else
		{
			user->DelCoinsCount( -coins, COINS_OPER_TYPE_DEBUG );
		}
	}

	// �жϽ������
	bool bgivecoins = false;	// ����Ƿ����ͽ�� ������� �ȸ���ҷ��ͽ���Ȼ���ڷ������͵���Ϣ
	if ( user->GetUserData().m_coins < gConfig.GetMinCoins() )
	{
		if ( gConfig.GetIsGiveCoins() && gUserManager.GetGiveCount(msg->m_usert.m_id) < gConfig.GetMaxGiveCount() )
		{
			LLOG_INFO( "CUserMessage::HanderEnterCoinDesk coins not enough, give userid=%d", msg->m_usert.m_id );

			user->AddCoinsCount( gConfig.GetGiveCoinsCount(), COINS_OPER_TYPE_FREE_GIVE );
			gUserManager.IncreaseGiveCount( msg->m_usert.m_id );

			bgivecoins = true;
		}
		else
		{
			LLOG_DEBUG("Work::HanderEnterCoinDesk coins not enough userid=%d coins=%d", user->GetUserDataId(), user->GetUserData().m_coins);
			send.m_errorCode = 1;
			Lint remaincount = gConfig.GetMaxGiveCount() - gUserManager.GetGiveCount(msg->m_usert.m_id);
			if ( remaincount < 0 )	// ����ʱ�޸����ûᵼ��������
			{
				remaincount = 0;
			}
			send.m_remainCount = remaincount;
			user->Send( send );
			return;
		}
	}

	send.m_errorCode = 0;
	gUserManager.AddUser(user);	// ������

	LLOG_INFO("Work::HanderEnterCoinDesk %d:%d:%s usernum=%d", user->GetUserDataId(), user->GetUserGateID(),user->GetIp().c_str(), gUserManager.GetUsersCount());

	// ��Ҽ��뿪���б�
	Lint ret = gDeskManager.UserEnterDesk( user->GetUserDataId(), (GameType)msg->m_state, msg->m_playType, msg->m_robotNum, msg->m_cardValue );
	if ( ret != 0 )
	{
		send.m_errorCode = ret;
		gUserManager.DelUser(user->GetUserDataId());
	}

	if ( bgivecoins )
	{
		// ������������
		LMsgS2CGiveCoins send;
		send.m_reasonCode = 1;
		send.m_coins = gConfig.GetGiveCoinsCount();
		Lint remaincount = gConfig.GetMaxGiveCount() - gUserManager.GetGiveCount(user->GetUserDataId());
		if ( remaincount < 0 )	// ����ʱ�޸����ûᵼ��������
		{
			remaincount = 0;
		}
		send.m_remainCount = remaincount;
		user->Send( send );
	}

	user->Send( send );

	// �޸���ҵ�½״̬
	user->Login();
}

void CUserMessage::HanderUserMsg(LMsgG2LUserMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	Lint gateId = gWork.getGateIdBySp(msg->m_sp);
	if(gateId == INVALID_GATEID)
	{
		LLOG_ERROR("%s have not gate", msg->m_strUUID.c_str());
		return;
	}

	if (msg->m_userMsg)
	{
		switch ( msg->m_userMsgId )
		{
		case MSG_C_2_S_LEAVE_ROOM:
			{
				HanderUserLeaveDesk((LMsgC2SLeaveDesk*)msg->m_userMsg, msg->m_strUUID, gateId);
			}
			break;
		case MSG_C_2_S_READY:
			{
				HanderUserReady((LMsgC2SUserReady*)msg->m_userMsg, msg->m_strUUID);
			}
			break;
		case MSG_C_2_S_RESET_ROOM:
			{
				HanderResetDesk((LMsgC2SResetDesk*)msg->m_userMsg, msg->m_strUUID);
			}
			break;
		case MSG_C_2_S_GOON_COINS_ROOM:
			{
				HanderGoOnCoinsDesk((LMsgC2SGoOnCoinsDesk*)msg->m_userMsg, msg->m_strUUID, gateId);
			}
			break;
		case MSG_C_2_S_GET_COINS:
			{
				HanderGetCoins((LMsgC2SGetCoins*)msg->m_userMsg, msg->m_strUUID, gateId);
			}
			break;
		default:
			{
				LLOG_DEBUG("CUserMessage::HanderUserMsg msg error %s:%d", msg->m_strUUID.c_str(), msg->m_userMsgId);
			}
			break;
		}
	}
	else
	{
		LLOG_DEBUG("CUserMessage::HanderUserMsg msg error %s:%d", msg->m_strUUID.c_str(), msg->m_userMsgId);
	}
}

void CUserMessage::HanderUserOutMsg(LMsgG2LUserOutMsg* msg)
{
	if(msg == NULL)
	{
		return;
	}

	Lint gateId = gWork.getGateIdBySp(msg->m_sp);
	if(gateId == INVALID_GATEID)
	{
		LLOG_ERROR("CUserMessage::HanderUserOutMsg have not gate, gateid=%d uuid=%s", gateId, msg->m_strUUID.c_str());
		return;
	}

	Lint deluserid = 0;
	// ��ֹuser���� ���������̵߳�desk�ڲ�����userʱ��������
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserUUID(msg->m_strUUID);
		if(safeUser && safeUser->isValid())
		{
			UserPtr user = safeUser->getResource();
			if ( user->GetUserGateID() != gateId )
			{
				// ��ҿ��ܴ�������gate�ϵ�½�� ���������Ϣ
				return;
			}

			LLOG_DEBUG("CUserMessage::HanderUserOutMsg, ID:%d, State:%d, deskID:%d, serverID: %d", user->GetUserDataId(), user->GetUserState(), user->GetDeskID(), user->GetUserLogicID());

			if( user->GetDeskID() == 0 )	// ��Ҳ���������
			{
				// ɾ�����
				deluserid = user->GetUserDataId();
			}
			else
			{
				// ������ڴ���
				user->SetOnline( false );
			}
		}
		else
		{
			LLOG_DEBUG("CUserMessage::HanderUserOutMsg user not find uuid=%s", msg->m_strUUID.c_str());
		}
	}
	
	if ( deluserid != 0 )
	{
		_userOut(deluserid);
	}
}

void CUserMessage::HanderUserLeaveDesk(LMsgC2SLeaveDesk* msg, const std::string& uuid, Lint gateid)
{
	LMsgS2CLeaveDesk send;
	send.m_errorCode = 0;

	Lint deluserid = 0;
	// ��ֹuser���� ���������̵߳�desk�ڲ�����userʱ��������
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserUUID(uuid);
		if(safeUser && safeUser->isValid())
		{
			UserPtr user = safeUser->getResource();

			LLOG_INFO("CUserMessage::HanderUserLeaveDesk, ID:%d, State:%d, deskID:%d, serverID: %d", user->GetUserDataId(), user->GetUserState(), user->GetDeskID(), user->GetUserLogicID());

			if( user->GetDeskID() == 0 ) // ��Ҳ���������
			{
				deluserid = user->GetUserDataId();
			}
			else
			{
				LLOG_ERROR("CUserMessage::HanderUserLeaveDesk, user in playing, not leave, userid=%d", user->GetUserDataId());
				// ������ڴ��� �����˳�
				send.m_errorCode = 1;
			}
			user->Send(send);
		}
		else
		{
			LLOG_ERROR("CUserMessage::HanderUserLeaveDesk user not find uuid=%s", uuid.c_str());
			// û�ҵ����Ҳ����������뿪����Ϣ
			LMsgL2GUserMsg usermsg;
			usermsg.m_strUUID = uuid;
			usermsg.m_dataBuff = send.GetSendBuff();
			gWork.SendToGate(gateid, usermsg);
		}
	}
	if ( deluserid != 0 )
	{
		_userOut( deluserid );
	}
}

void CUserMessage::HanderUserReady(LMsgC2SUserReady*msg, const std::string& uuid)
{
	Lint userid = 0;
	Lint deskrunid = 0;
	// ��ֹuser���� ���������̵߳�desk�ڲ�����userʱ��������
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserUUID(uuid);
		if(safeUser && safeUser->isValid())
		{
			UserPtr user = safeUser->getResource();
			userid = user->GetUserDataId();
			deskrunid = user->GetDeskRunID();
		}
		else
		{
			return;
		}
	}
	if ( deskrunid != 0 )
	{
		if ( !gDeskManager.UserReady( deskrunid, userid ) )
		{
			LLOG_ERROR("CUserMessage::HanderUserReady error userid=%d deskrunid=%d", userid, deskrunid);
		}
	}
}

void CUserMessage::HanderResetDesk(LMsgC2SResetDesk* msg, const std::string& uuid)
{
	LLOG_ERROR("CUserMessage::HanderResetDesk no logic");
}

void CUserMessage::HanderGoOnCoinsDesk(LMsgC2SGoOnCoinsDesk* msg, const std::string& uuid, Lint gateid)
{
	LMsgS2CGoOnCoinsDeskRet ret;
	// ������Ϊ0
	ret.m_errorCode = 0;

	Lint userid = 0;
	Lint deskrunid = 0;
	// ��ֹuser���� ���������̵߳�desk�ڲ�����userʱ��������
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserUUID(uuid);
		if(safeUser.get() == NULL || !safeUser->isValid())
		{
			LLOG_ERROR("CUserMessage::HanderGoOnCoinsDesk user not in desk, uuid=%s", uuid.c_str());
			ret.m_errorCode = 2;
			LMsgL2GUserMsg send;
			send.m_strUUID = uuid;
			send.m_dataBuff = ret.GetSendBuff();
			gWork.SendToGate(gateid, send);
			return;
		}

		UserPtr user = safeUser->getResource();
		// �ж�����Ƿ�������Ϸ
		if( user->GetDeskID() != 0 )
		{
			// ����Ѿ�����������
			LLOG_ERROR("CUserMessage::HanderGoOnCoinsDesk user state error, ID:%d, State:%d, serverID: %d", user->GetUserDataId(), user->GetUserState(), user->GetUserLogicID());
			ret.m_errorCode = 2;
			user->Send( ret );
			return;
		}
		
		userid = user->GetUserDataId();
		deskrunid = user->GetDeskRunID();

		// �жϽ������
		if ( user->GetUserData().m_coins < gConfig.GetMinCoins() )
		{
			if ( gConfig.GetIsGiveCoins() && gUserManager.GetGiveCount(user->GetUserDataId()) < gConfig.GetMaxGiveCount() )
			{
				LLOG_INFO( "CUserMessage::HanderGoOnCoinsDesk coins not enough, give userid=%d", user->GetUserDataId() );

				user->AddCoinsCount( gConfig.GetGiveCoinsCount(), COINS_OPER_TYPE_FREE_GIVE );
				gUserManager.IncreaseGiveCount( user->GetUserDataId() );

				// ������������
				LMsgS2CGiveCoins send;
				send.m_reasonCode = 1;
				send.m_coins = gConfig.GetGiveCoinsCount();
				Lint remaincount = gConfig.GetMaxGiveCount() - gUserManager.GetGiveCount(user->GetUserDataId());
				if ( remaincount < 0 )	// ����ʱ�޸����ûᵼ��������
				{
					remaincount = 0;
				}
				send.m_remainCount = remaincount;
				user->Send( send );
			}
			else
			{
				LLOG_DEBUG("Work::HanderGoOnCoinsDesk coins not enough userid=%d coins=%d", user->GetUserDataId(), user->GetUserData().m_coins);
				ret.m_errorCode = 1; // ��ҽ�Ҳ���
				Lint remaincount = gConfig.GetMaxGiveCount() - gUserManager.GetGiveCount(user->GetUserDataId());
				if ( remaincount < 0 )	// ����ʱ�޸����ûᵼ��������
				{
					remaincount = 0;
				}
				ret.m_remainCount = remaincount;
			}
		}
	}

	if ( ret.m_errorCode == 0 )
	{
		if ( deskrunid == 0 )	// �������û��������
		{
			CardValue cardvlaue[CARD_COUNT];
			Lint rst = gDeskManager.UserEnterDesk( userid, (GameType)msg->m_state, msg->m_playType, 0, cardvlaue );
			if ( rst != 0 )
			{
				LLOG_ERROR("CUserMessage::HanderGoOnCoinsDesk enter desk error userid=%d", userid);
				ret.m_errorCode = rst;
			}
		}
		else
		{
			if ( !gDeskManager.UserReady( deskrunid, userid ) )
			{
				ret.m_errorCode = 3;
				LLOG_ERROR("CUserMessage::HanderGoOnCoinsDesk ready error userid=%d deskrunid=%d", userid, deskrunid);
			}
		}
	}

	// �ظ��û�
	LMsgL2GUserMsg send;
	send.m_strUUID = uuid;
	send.m_dataBuff = ret.GetSendBuff();
	gWork.SendToGate(gateid, send);

	// ��Ҽ����Ƿ�󲢲�ɾ����� �ͻ��ȷ�����ҳʱ �����뿪��Ϣ
// 	if ( ret.m_errorCode != 0 )
// 	{
// 		_userOut( userid );
// 	}
}

void CUserMessage::HanderGetCoins(LMsgLMG2CNGetCoins* msg)
{
	HanderGetCoins( msg->m_userid, msg->m_strUUID, msg->m_gateid );
}

void CUserMessage::HanderGetCoins(LMsgC2SGetCoins* msg, const std::string& uuid, Lint gateid)
{
	HanderGetCoins( msg->m_userid, uuid, gateid );
}

void CUserMessage::HanderGetCoins(Lint userid, const std::string& uuid, Lint gateid)
{
	Lint deskrunid = 0;
	LMsgS2CGetCoins ret;
	if ( gUserManager.GetGiveCount(userid) < gConfig.GetMaxGiveCount() )
	{
		LLOG_INFO( "CUserMessage::HanderGetCoins userid=%d", userid );

		// ��ȡ��ҵ�ʱ�� ��ҿ��ܲ��ڽ�ҳ���
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserUUID(uuid);
		if(safeUser && safeUser->isValid())
		{
			UserPtr user = safeUser->getResource();
			user->AddCoinsCount( gConfig.GetGiveCoinsCount(), COINS_OPER_TYPE_SELF_GIVE );
			deskrunid = user->GetDeskRunID();
		}
		else
		{
			User::AddCoinsCount( gConfig.GetGiveCoinsCount(), COINS_OPER_TYPE_SELF_GIVE, userid, uuid );
		}
		gUserManager.IncreaseGiveCount( userid );

		ret.m_result = 0;
		ret.m_coins = gConfig.GetGiveCoinsCount();
		Lint remaincount = gConfig.GetMaxGiveCount() - gUserManager.GetGiveCount(userid);
		if ( remaincount < 0 )	// ����ʱ�޸����ûᵼ��������
		{
			remaincount = 0;
		}
		ret.m_remainCount = remaincount;
	}
	else
	{
		ret.m_result = 1;
		LLOG_DEBUG( "CUserMessage::HanderGetCoins userid=%d, not have GiveCoins", userid );
	}

	LMsgL2GUserMsg send;
	send.m_strUUID = uuid;
	send.m_dataBuff = ret.GetSendBuff();
	gWork.SendToGate(gateid, send);

	if ( deskrunid != 0 )
	{
		// �㲥�������������
		gDeskManager.CastUserCoins( userid );
	}
}

void CUserMessage::HanderUserLogin(LMsgLMG2CNUserLogin* msg)
{
	Lint deluserid = 0;
	Lint reenteruserid = 0;
	// ��ֹuser���� ���������̵߳�desk�ڲ�����userʱ��������
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserUUID(msg->m_strUUID);
		if(safeUser && safeUser->isValid())
		{
			UserPtr user = safeUser->getResource();
			user->SetUserData( msg->m_usert );
			user->SetOnline( true );
			user->SetUserGateID( msg->m_gateId );
			user->SetIp( msg->m_ip );

			LLOG_INFO("CUserMessage::HanderUserLogin ID:%d, State:%d, serverID: %d", user->GetUserDataId(), user->GetUserState(), user->GetUserLogicID());
			// ������������������ �߳���ң� ����������ڲ����� �ͻ��˻��߶��ߵ��߼�
			if ( user->GetDeskID() == 0 )
			{
				LLOG_ERROR("CUserMessage::HanderUserLogin user not in desk, kick out, ID:%d, State:%d, serverID: %d", user->GetUserDataId(), user->GetUserState(), user->GetUserLogicID());
				deluserid = user->GetUserDataId();

				// �߳����
				LMsgS2CKickCoinsDesk kick;
				kick.m_reasonCode = 3;
				user->Send( kick );
			}
			else if ( user->GetUserLogicID() == 0 )	// ����ʱ����Ϣ�϶��ǿ�ס�� �Ȱ����������
			{
				reenteruserid = user->GetUserDataId();
				user->Login();	// ��Ҫ���޸���ҵ�״̬
			}
		}
	}
	
	if ( reenteruserid != 0 )
	{
		if ( !gDeskManager.UserReEnterDesk( reenteruserid ) )
		{
			LLOG_ERROR("CUserMessage::HanderUserLogin user reenterdesk fail, userid=%d", reenteruserid);
			deluserid = reenteruserid;
		}
	}

	if ( deluserid != 0 )
	{
		_userOut( deluserid );
	}
}

void CUserMessage::HanderModifyUserState(LMsgL2LMGModifyUserState* msg)
{
	if(msg == NULL)
	{
		return;
	}

	Lint deluserid = 0;
	// ��ֹuser���� ���������̵߳�desk�ڲ�����userʱ��������
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(msg->m_userid);
		if(safeUser.get() == NULL || !safeUser->isValid())
		{
			return;
		}

		UserPtr user = safeUser->getResource();

		LLOG_DEBUG("Work::HanderModifyUserState ID:%d, State:%d, serverID: %d", user->GetUserDataId(), msg->m_userstate, msg->m_logicID);

		user->SetUserState( (LGUserMsgState)msg->m_userstate );
		user->SetUserLogicID(msg->m_logicID);

		// Ŀǰ��ҷ�������ҵ�״ֻ̬������ �������߳����
		if ( !(msg->m_userstate == LGU_STATE_COIN || msg->m_userstate == LGU_STATE_COINDESK) )
		{
			LLOG_ERROR("Work::HanderModifyUserState user state error, kick out, ID:%d, State:%d, serverID: %d", user->GetUserDataId(), msg->m_userstate, msg->m_logicID);
			// �߳����
			LMsgS2CKickCoinsDesk kick;
			kick.m_reasonCode = 3;
			user->Send( kick );

			deluserid = user->GetUserDataId();
		}
	}
	if ( deluserid != 0 )
	{
		_userOut( deluserid );
	}
}

void CUserMessage::_userOut( Lint userid )
{
	Lint deskrunid = 0;
	// ��ֹuser���� ���������̵߳�desk�ڲ�����userʱ��������
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userid);
		if(safeUser.get() == NULL || !safeUser->isValid())
		{
			return;
		}

		UserPtr user = safeUser->getResource();
		deskrunid = user->GetDeskRunID();
	}
	if ( deskrunid != 0 )
	{
		if ( !gDeskManager.UserLeaveDesk( userid ) )
		{
			// �����ʲô������ ������
			LLOG_ERROR("CUserMessage::_userOut user can not leave desk, userid=%d", userid);
			return;
		}
	}

	// �޸���ҵ�½״̬
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userid);
		if(safeUser && safeUser->isValid())
		{
			UserPtr user = safeUser->getResource();
			user->Logout();

			if ( user->IsRobot() )
			{
				gRobotManager.ReleaseRotot( userid );
			}
		}
	}
	// ɾ�����
	gUserManager.DelUser( userid );
}