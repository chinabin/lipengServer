#ifndef _DESK_MANAGER_H
#define _DESK_MANAGER_H

#include "LBase.h"
#include "LSingleton.h"
#include "Desk.h"
#include "User.h"
#include "Robot.h"

#define FREE_DESK_MAX 256	// ����������ӵ�����
#define FREE_DESK_REQ 128	// һ���������ӵ�����

class DeskManager:public LSingleton<DeskManager>, public LRunnable
{
public:
	virtual	bool	Init();
	virtual	bool	Final();

	virtual void	Clear() {}
	virtual void	Run();
	// ÿ֡����
	void	Tick();

	// 1�����
	void	TickOneSecond();

	// 5�����
	void	TickFiveSecond();

public:
	// ���е����Ӻŵ�����
	int GetALLDeskIDCount();

	// ��ӽ������
	void	AddFreeDeskID( GameType type, const std::vector<CoinsDesk>& deskid );

	void	ClearDesk();

	// ����ҷ������� ��������ʱ����ID
	// �����ϢҲ����صĹ㲥��Ϣ
	Lint	UserEnterDesk( Lint userid, GameType type, const std::vector<Lint>& playtype, Lint robotnum, CardValue cardvlaue[CARD_COUNT] );
	bool	UserLeaveDesk( Lint userid );
	bool	UserReady( Lint deskrunid, Lint userid );
	void	GameOver( Lint deskid, Lint score[4], const LBuffPtr& resultmsg );
	void	DismissPlayDesk( Lint deskid );

	// ������½���
	bool	UserReEnterDesk( Lint userid );

	// �㲥��ҽ��
	void	CastUserCoins( Lint userid );

private:
	CoinsDesk _getCoinsDesk( GameType gameType );
	void	_initTickTime();
	LTime		m_tickTimer;
	Llong		m_1SceTick;//30��ѭ��һ��
	Llong		m_5SceTick;//15��ѭ��һ��

	boost::recursive_mutex		m_mutexDesk;
	std::map< GameType, std::queue<CoinsDesk> > m_freeDeskList;	// ��������б�

	Lint				m_LastRequestTime;	// ����Ƿ������������ӵ���ʱ ��ֹ�ڶ�ʱ���ڶ������

	// deskrunid : DeskPtr
	std::map< Lint, DeskPtr > m_allDeskList;	// ���е������б�

	std::list< DeskPtr > m_deskWaitList;	// ��������� ��û�����Ӻ�
	
	// deskid : DeskPtr
	std::map< Lint, DeskPtr > m_deskPlayList;	// ���ڴ��Ƶ������б� ռ�������Ӻ�
};

#define gDeskManager DeskManager::Instance()

#endif