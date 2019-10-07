#ifndef _NEWUSERVERIFY_H_
#define _NEWUSERVERIFY_H_

#include "LDBSession.h"
#include "LRunnable.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LTime.h"

//db������
class NewUserVerify : public LRunnable
{
public:
	NewUserVerify();
	void SetDatabaseSource(bool  bUserMaster);

protected:
	virtual bool		Init();
	virtual bool		Final();

	virtual void		Run();
	virtual void		Clear();

	void				HanderMsg(LMsg* msg);

	void				ClearOpenids( const LTime& cur );	// ������ڵ�openid

	void                ReadConfig();

protected:
	void	HanderNewUserVerifyRequest(LMsgNewUserVerifyRequest* msg);
	void    MHHanderNewAgencyActivity(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg);
	void    MHHanderOneDrawActivity(MHLMsgL2LMGNewAgencyActivityUpdatePlayCount * msg);
	Lint	GetMaxUserIdFromDB();
	Lint GetUniqueIdByTimestamp();
	Lint	GetRandInsertIDFromDB();

private:
	bool m_bUseMasterSource;
	LDBSession* m_dbsession;

	LTime		m_tickTimer;
	Llong		m_60SceTick;//60��ѭ��һ��

	Llong		m_600SceTick;//5������ѭ��һ��

	Lshort        m_needWechatVerify;

	// ��ֹ��ͬ���û� �ڶ�ʱ�����ظ���¼
	std::map<std::string,LTime> m_openids;
};

#endif