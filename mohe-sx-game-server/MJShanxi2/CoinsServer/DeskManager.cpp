#include "DeskManager.h"
#include "Work.h"
#include "LLog.h"
#include "UserManager.h"
#include "Config.h"
#include "RobotManager.h"

bool DeskManager::Init()
{
	m_LastRequestTime = 0;
	_initTickTime();
	return true;
}

bool DeskManager::Final()
{
	return true;
}

void DeskManager::Run()
{
	Init();
	while(!GetStop())
	{
		//���ﴦ����Ϸ��ѭ��
		LTime cur;
		if(cur.MSecs() - m_tickTimer.MSecs() > 100) // 0.1����һ��
		{
			m_tickTimer = cur;
			Tick();	
		}

		if (cur.MSecs() - m_1SceTick > 1000)
		{
			m_1SceTick = cur.MSecs();
			TickOneSecond();
		}

		//5��ѭ��һ��
		if (cur.MSecs() - m_5SceTick > 5000)
		{
			m_5SceTick = cur.MSecs();
			TickFiveSecond();
		}
		Sleep(1);
	}
	Final();
}

void DeskManager::Tick()
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);
	// û�п��е����� ����
	if ( m_deskWaitList.size() > 0 && m_LastRequestTime == 0 )
	{
		std::map< GameType, bool > req;	// ��ʱ��¼ĳ���淨�Ƿ�����������
		for ( auto it = m_deskWaitList.begin(); it != m_deskWaitList.end(); ++it )
		{
			auto deskit = m_freeDeskList.find( (*it)->m_gameType );
			if ( !(deskit == m_freeDeskList.end() || deskit->second.size() == 0) )
			{
				// �п������� ����Ҫ����
				continue;
			}
			
			auto breq = req.find((*it)->m_gameType);
			if ( breq == req.end() || !breq->second )
			{
				req[(*it)->m_gameType] = true;
				LLOG_ERROR( "DeskManager::Tick DeskReq gametype=%d", (*it)->m_gameType );
				m_LastRequestTime = 3;
				LMsgCN2LMGFreeDeskReq req;
				req.m_gameType = (*it)->m_gameType;
				req.m_count = FREE_DESK_REQ;
				gWork.SendToLogicManager( req );
			}
		}
	}
}

void DeskManager::TickOneSecond()
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);
	if ( m_LastRequestTime > 0 )
	{
		m_LastRequestTime--;
	}
	// ���û��׼�������
	for ( auto it = m_deskWaitList.begin(); it != m_deskWaitList.end(); ++it )
	{
		(*it)->CheckReadyOutTime();
	}

	int startdeskcount = 0;
	// ��鿪ʼ����
	for ( auto it = m_deskWaitList.begin(); it != m_deskWaitList.end(); )
	{
		// ����Ƿ��п��е�����
		auto deskit = m_freeDeskList.find( (*it)->m_gameType );
		if ( deskit == m_freeDeskList.end() || deskit->second.size() == 0 )
		{
			++it;
			continue;
		}

		if ( !(*it)->CheckGameStart() )
		{
			++it;
			continue;
		}

		// ��һ�����Ӻ� ͬʱ����������ڷ�����������״̬
		CoinsDesk desk = _getCoinsDesk( (*it)->m_gameType );
		if ( desk.m_id == 0 )	// û���ҵ�����id
		{
			++it;
			continue;
		}

		// ���Ϳ�����Ϣ
		LMsgCN2LCreateCoinDesk send;
		for ( auto userit = (*it)->m_users.begin(); userit != (*it)->m_users.end(); ++userit )
		{
			boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId( userit->m_userID );
			if(safeUser && safeUser->isValid())
			{
				UserPtr user = safeUser->getResource();
				send.m_users.push_back( LMsgCN2LCreateCoinDesk::User() );
				send.m_users.back().m_gateId = user->GetUserGateID();
				send.m_users.back().m_ip = user->GetIp();
				send.m_users.back().m_usert = user->GetUserData();
				send.m_users.back().m_pos = userit->m_pos;

				// ������ҵ�����
				user->SetDeskID( desk.m_id );
			}
			else
			{
				// �����ϲ������������
				LLOG_ERROR( "DeskManager::TickOneSecond not find user, userid=%d", userit->m_userID );
			}
		}

		send.m_deskId = desk.m_id;
		send.m_state = (Lint)(*it)->m_gameType;
		send.m_robotNum = (*it)->m_robotNum;
		memcpy( send.m_cardValue, (*it)->m_cardValue, sizeof(send.m_cardValue) );
		send.m_playType = (*it)->m_playType;
		send.m_baseScore = gConfig.GetBaseScore();
		send.m_changeOutTime = gConfig.GetChangeOutTime();
		send.m_opOutTime = gConfig.GetOpOutTime();

		gWork.SendToLogic( desk.m_logicID, send );

		LLOG_INFO( "DeskManager::TickOneSecond GameStart deskid=%d waitsize=%d playsize=%d", desk.m_id, m_deskWaitList.size(), m_deskPlayList.size() );

		// �������Ӻ� �л��б�
		(*it)->m_coinDesk = desk;
		m_deskPlayList[desk.m_id] = (*it);
		m_deskWaitList.erase( it++ );
		startdeskcount++;
		if ( startdeskcount >= 256 )	// 1���������256��
		{
			LLOG_ERROR( "DeskManager::TickOneSecond DeskStart too much, more than 256" );
			break;
		}
	}

	static int lognum = 0;
	lognum++;
	if ( lognum >= 10 )
	{
		lognum = 0;
		LLOG_ERROR( "DeskManager::TickOneSecond desk waitsize=%d, playsize=%d, usersize=%d", m_deskWaitList.size(), m_deskPlayList.size(), gUserManager.GetUsersCount() );
	}
}

void DeskManager::TickFiveSecond()
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);

	// ����û����ҵ�����
	for ( auto it = m_deskWaitList.begin(); it != m_deskWaitList.end(); )
	{
		if ( (*it)->m_users.size() == 0 )
		{
			m_allDeskList.erase( (*it)->m_runID );
			m_deskWaitList.erase( it++ );
		}
		else
		{
			it++;
		}
	}

	// ���FreeDeskID���� ����һЩ
	LMsgCN2LMGRecycleDesk send;
	for ( auto it = m_freeDeskList.begin(); it != m_freeDeskList.end(); ++it )
	{
		if ( it->second.size() > FREE_DESK_MAX )
		{
			send.m_deskid.push_back( it->second.front().m_id );
			it->second.pop();
		}
	}
	if ( send.m_deskid.size() > 0 )
	{
		LLOG_ERROR( "DeskManager::Tick5 RecycleDesk %d", send.m_deskid.size() );
		gWork.SendToLogicManager( send );
	}
}

int DeskManager::GetALLDeskIDCount()
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);
	return (int)m_freeDeskList.size() + m_deskPlayList.size();
}

void DeskManager::AddFreeDeskID( GameType type, const std::vector<CoinsDesk>& deskid )
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);
	m_LastRequestTime = 0;
	auto deskit = m_freeDeskList.find( type );
	if ( deskit == m_freeDeskList.end() )
	{
		for ( auto it = deskid.begin(); it != deskid.end(); ++it )
		{
			m_freeDeskList[type].push( *it );
		}
	}
	else
	{
		for ( auto it = deskid.begin(); it != deskid.end(); ++it )
		{
			deskit->second.push( *it );
		}
	}
}

void DeskManager::ClearDesk()
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);
// 	for ( auto it = m_deskWaitList.begin(); it != m_deskWaitList.end(); ++it )
// 	{
// 		it->second->DismissDesk();
// 	}
// 	for ( auto it = m_deskPlayList.begin(); it != m_deskPlayList.end(); ++it )
// 	{
// 		it->second->DismissDesk();
// 	}
	m_freeDeskList.clear();
	m_allDeskList.clear();
	m_deskWaitList.clear();
	m_deskPlayList.clear();
}

Lint DeskManager::UserEnterDesk( Lint userid, GameType type, const std::vector<Lint>& playtype, Lint robotnum, CardValue cardvlaue[CARD_COUNT] )
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userid);
	if(!safeUser || !safeUser->isValid())
	{
		return 3;
	}
	UserPtr user = safeUser->getResource();

	// �ȴӵȴ�����������в���
	DeskPtr desk;
	for ( auto it = m_deskWaitList.begin(); it != m_deskWaitList.end(); ++it )
	{
		if ( (*it)->m_gameType == type && (int)(*it)->m_users.size() < (*it)->m_deskMaxPlayer )
		{
			desk = (*it);
			break;
		}
	}

	if ( !desk )
	{
		int deskMaxPlayer = 4;
		if ( type == SiChuanMaJinagxlc )
		{
			deskMaxPlayer = 4;
		}
		else
		{
			LLOG_ERROR( "DeskManager::UserEnterDesk gametype error, type=%d, userid=%d", type, userid );
			return 5;
		}

		// ����Desk
		static Lint s_deskrunid = 0;
		s_deskrunid++;
		desk.reset( new Desk(s_deskrunid, type) );
		desk->m_deskMaxPlayer = deskMaxPlayer;
		m_deskWaitList.push_back( desk );
		m_allDeskList[s_deskrunid] = desk;

		LLOG_INFO( "DeskManager::UserEnterDesk create desk, waitsize=%d, playsize=%d", m_deskWaitList.size(), m_deskPlayList.size() );
	}

	// ����ǵ�һ����� ��������
	if ( desk->m_users.size() == 0 )
	{
		desk->m_robotNum = robotnum;
		memcpy( desk->m_cardValue, cardvlaue, sizeof(desk->m_cardValue) );
		desk->m_playType = playtype;
	}
	
	if ( desk->OnUserInRoom( user ) )
	{
		int freenum = desk->m_deskMaxPlayer - (int)desk->m_users.size();
		if ( robotnum > 0 && freenum > 0 )
		{
			int num = robotnum > freenum ? freenum : robotnum;
			for ( int i = 0; i < num; ++i )
			{
				RobotPtr robot = gRobotManager.CaptureRotot();
				if ( robot )
				{
					gUserManager.AddUser( robot );
					boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(robot->GetUserDataId());
					desk->OnUserInRoom( safeUser->getResource() );
					robot->Login();
				}
			}
		}
		return 0;
	}
	return 3;
}

bool DeskManager::UserLeaveDesk( Lint userid )
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userid);
	if(safeUser && safeUser->isValid())
	{
		UserPtr user = safeUser->getResource();

		// ���ֻ���ڵȴ���״̬���뿪����
		for ( auto it = m_deskWaitList.begin(); it != m_deskWaitList.end(); ++it )
		{
			if ( (*it)->m_runID == user->GetDeskRunID() )
			{
				return (*it)->OnUserOutRoom( user, true );
			}
		}
		LLOG_ERROR( "DeskManager::UserLeaveDesk not find desk, deskrunid=%d", user->GetDeskRunID() );
		return false;
	}
	return false;
}

bool DeskManager::UserReady( Lint deskrunid, Lint userid )
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);

	for ( auto it = m_deskWaitList.begin(); it != m_deskWaitList.end(); ++it )
	{
		if ( (*it)->m_runID == deskrunid )
		{
			boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId(userid);
			if(safeUser && safeUser->isValid())
			{
				UserPtr user = safeUser->getResource();
				return (*it)->OnUserReady( user );
			}
			return false;
		}
	}
	LLOG_ERROR( "DeskManager::UserReady not find desk, deskrunid=%d", deskrunid );
	return false;
}

void DeskManager::GameOver( Lint deskid, Lint score[4], const LBuffPtr& resultmsg )
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);

	auto it = m_deskPlayList.find( deskid );
	if ( it == m_deskPlayList.end() )
	{
		LLOG_ERROR( "DeskManager::GameOver not find desk, deskid=%d", deskid );
		return;
	}
	LLOG_INFO( "DeskManager::GameOver deskid=%d waitsize=%d playsize=%d", deskid, m_deskWaitList.size(), m_deskPlayList.size() );

	for ( auto userit = it->second->m_users.begin(); userit != it->second->m_users.end(); ++userit )
	{
		boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId( userit->m_userID );
		if(safeUser && safeUser->isValid())
		{
			UserPtr user = safeUser->getResource();
			// ������ҵ�����
			user->SetDeskID( 0 );

			// ���ͽ�����Ϣ
			user->Send( resultmsg );

			// �޸���ҵĽ��
			if ( userit->m_pos >= 0 && userit->m_pos < 4 )
			{
				if ( score[userit->m_pos] > 0 )
				{
					user->AddCoinsCount( score[userit->m_pos], COINS_OPER_TYPE_RESULT );
				}
				else if ( score[userit->m_pos] < 0 )
				{
					user->DelCoinsCount( -score[userit->m_pos], COINS_OPER_TYPE_RESULT );
				}
			}
		}
	}

	it->second->OnGameOver();
	// �л�����
	m_deskWaitList.push_back( it->second );
	m_freeDeskList[it->second->m_gameType].push( it->second->m_coinDesk );	// �������Ӻ�
	m_deskPlayList.erase( it );
}

void DeskManager::DismissPlayDesk( Lint deskid )
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);

	auto it = m_deskPlayList.find( deskid );
	if ( it == m_deskPlayList.end() )
	{
		LLOG_ERROR( "DeskManager::DismissPlayDesk not find desk, deskid=%d", deskid );
		return;
	}
	it->second->DismissDesk();
	m_freeDeskList[it->second->m_gameType].push( it->second->m_coinDesk );	// �������Ӻ�
	m_deskPlayList.erase( it );
}

bool DeskManager::UserReEnterDesk( Lint userid )
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId( userid );
	if(!(safeUser && safeUser->isValid()))
	{
		return false;
	}
	UserPtr user = safeUser->getResource();
	if ( user->GetDeskRunID() == 0 )
	{
		return false;
	}

	auto it = m_allDeskList.find( user->GetDeskID() );
	if ( it != m_allDeskList.end() )
	{
		return it->second->OnUserReInRoom( user );
	}
	return false;
}

void DeskManager::CastUserCoins( Lint userid )
{
	boost::recursive_mutex::scoped_lock l(m_mutexDesk);

	boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId( userid );
	if(!(safeUser && safeUser->isValid()))
	{
		return;
	}
	UserPtr user = safeUser->getResource();
	if ( user->GetDeskRunID() == 0 )
	{
		return;
	}

	auto it = m_allDeskList.find( user->GetDeskRunID() );
	if ( it != m_allDeskList.end() )
	{
		LMsgS2COtherCoins send;
		send.m_coins = user->GetUserData().m_coins;
		// ���ҵ���ҵ�λ��
		for ( auto userit = it->second->m_users.begin(); userit != it->second->m_users.end(); ++userit )
		{
			if ( userit->m_userID == user->GetUserDataId() )
			{
				send.m_pos = userit->m_pos;
				break;
			}
		}
		// �㲥
		for ( auto userit = it->second->m_users.begin(); userit != it->second->m_users.end(); ++userit )
		{
			if ( userit->m_userID != user->GetUserDataId() )
			{
				boost::shared_ptr<CSafeResourceLock<User> > safeUser = gUserManager.getUserbyUserId( userit->m_userID );
				if(safeUser && safeUser->isValid())
				{
					UserPtr user = safeUser->getResource();
					user->Send( send );
				}
			}
		}
	}
}

CoinsDesk DeskManager::_getCoinsDesk( GameType gameType )
{
	CoinsDesk desk;
	desk.m_id = 0;
	auto deskit = m_freeDeskList.find( gameType );
	if ( deskit == m_freeDeskList.end() )
	{
		return desk;
	}
	LMsgCN2LMGRecycleDesk rcymsg;
	while ( deskit->second.size() > 0 )
	{
		if ( !gWork.isLogicServerExist( deskit->second.front().m_logicID ) )
		{
			LLOG_ERROR( "DeskManager::TickOneSecond LogicServer not exist, logicserver=%d", deskit->second.front().m_logicID );
			// �����������ID
			rcymsg.m_deskid.push_back( deskit->second.front().m_id );
			deskit->second.pop();
			continue;
		}
		desk = deskit->second.front();
		deskit->second.pop();
		break;
	}
	if ( rcymsg.m_deskid.size() > 0 )
	{
		LLOG_ERROR( "DeskManager::GetCoinsDesk RecycleDesk %d", rcymsg.m_deskid.size() );
		gWork.SendToLogicManager( rcymsg );
	}
	return desk;
}

void DeskManager::_initTickTime()
{
	LTime cur;
	m_1SceTick = cur.MSecs();
	m_5SceTick = cur.MSecs();
}
