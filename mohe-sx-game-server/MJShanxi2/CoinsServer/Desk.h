#ifndef _DESK_H_
#define _DESK_H_

#include "LBase.h"
#include "GameDefine.h"
#include "LTime.h"
#include "LVideo.h"
#include "LMsg.h"
#include "User.h"

struct DeskUser
{
	DeskUser( Lint userid, Lint pos )
		: m_userID( userid )
		, m_pos(pos)
		, m_bReady( false )
	{
	}
	//bool operator = ( const DeskUser& o ) const { return m_userID == o.m_userID; }

	Lint m_userID;
	Lint m_pos;
	bool m_bReady;
	LTime m_time;	// ״̬ʱ�� �������� ����ʱ����
};

struct Desk
{
	Desk( Lint runid, GameType type );

	Lint m_runID;				// ��������ʱID
	GameType m_gameType;
	CoinsDesk m_coinDesk;		// �������Ӻź���Ч
	Lint m_deskMaxPlayer;		// ������������
	std::list<DeskUser> m_users;	// �Ѿ��������ӵ����

	// �淨���
	Lint				m_robotNum;			// 0,���ӻ����ˣ�1��2��3�ӻ���������
	CardValue			m_cardValue[CARD_COUNT];
	std::vector<Lint>	m_playType;			//�淨

	// �����Ϸ�����Ƿ���Կ�ʼ
	bool CheckGameStart();

	bool OnUserInRoom( UserPtr& user );
	bool OnUserOutRoom( UserPtr& user, bool bremove );	// bremove ��ʾ�Ƿ��m_users��ɾ��
	bool OnUserReady( UserPtr& user );
	void OnGameOver();

	void CheckReadyOutTime();
	// ǿ�ƽ�ɢ����
	void DismissDesk();

	bool OnUserReInRoom( UserPtr& user );	// ������½���
};

typedef boost::shared_ptr<Desk> DeskPtr;

#endif