#ifndef _USER_MESSAGE_H_DB
#define _USER_MESSAGE_H_DB

#include "LBase.h"
#include "LRunnable.h"
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMemoryRecycle.h"
#include "LDBSession.h"
#include "LTime.h"
#include "LRedisClient.h"

// #define  RUSER	"LDB_USER_10001"
// 
// Lstring record = "{"time":13456,"userid":"user1#user2#user3#user4","deskid":"12455"}";

class CUserMessage : public LRunnable
{
public:
	CUserMessage();
	virtual ~CUserMessage();
public:
	virtual bool		Init();
	virtual bool		Final();

	virtual void Clear();
	virtual void Run();

	virtual void Start();
	virtual	void Stop();
	virtual	void Join();
protected:
	void	HanderMsg(LMsg* msg);

	bool	Excute( const std::string& sql );

protected:
	void	HanderLogicReqVideo(LMsgL2LDBReqVideo* msg);

	void	HanderLogicReqVipLog(LMsgL2LBDReqVipLog* msg);

	void	HanderLogicReqPokerLog(LMsgL2LBDReqVipLog* msg);

	void	HanderLogicReqCRELog(LMsgL2LBDReqCRELog* msg);

	void    HanderLogicDelCRELog(LMsgL2LDBDEL_GTU* msg);

	void    HanderLogicReqSaveLog(LMsgL2LDBSaveLogItem* msg);

	void    HanderLogicReqSaveVideo(LMsgL2LDBSaveVideo* msg);

	void	HanderLogicReqRoomLog(LMsgL2LDBReqRoomLog * msg);

	void    HanderLogicSaveCRELog(LMsgL2LDBSaveCRELog* msg);

	void    HanderUserLogin(LMsgLMG2LdbUserLogin* msg);

	// ����
	void    HanderActivityPhone(LMsgC2SActivityPhone *msg);
	void    HanderActivityRequestLog(LMsgC2SActivityRequestLog *msg);
	void    HanderActivityRequestDrawOpen(LMsgC2SActivityRequestDrawOpen *msg);
	void    HanderActivityRequestDraw(LMsgC2SActivityRequestDraw *msg);

	//���� ���
	void    HanderActivityRequestShare(LMsgC2SActivityRequestShare *msg);

	bool    _initRedisConnect();

	//logic����ĳ����¼��
	void			HanderLogicReqShareVideo(LMsgL2LDBGetShareVideo* msg);

	//logic������ҷ����¼��
	void			HanderLogicGetShareVideo(LMsgL2LDBGetSsharedVideo* msg);

	//logic������һ�ȡĳ����¼��Id
	void			HanderLogicGetShareVideoId(LMsgL2LDBGGetShareVideoId* msg);

	//����share video id
	void			HanderLogicSaveShareVideoId(LMsgL2LDBSaveShareVideoId* msg);

	//loigc ͨ��Userid���������ͳ������
	void			HanderLogicGetUserPlayData(LMsgL2LDBGetUserPlayData* msg);

	void            HanderLogicUpdateUserPlayData(LMsgL2LDBUpdatePlayerScore* msg);

	//logic ͨ��ShareId2���ҷ���¼��
	void           HandlerLogicQueryShareVideoByShareId2(MHLMsgL2LDBQueryShareVideoByShareId * msg);
	 	
	void           InsertShareVideoRecordByVideoRecord(Lstring & str_videoid, Lstring & str_shareid);

	//�˿˻ط�
	void          HanderLogicQiPaiReqRoomLog(LMsgL2LDBReqRoomLog * msg);
	void          HanderLogicQiPaiReqVideo(LMsgL2LDBReqVideo* msg);

protected:
	void    UpdateUserInfo(const LUser& usert);
	MYSQL * GetMySQL();
private:
	CMemeoryRecycle m_memoryRecycle;
	LTime		m_tickTimer;

	LDBSession* m_dbsession;
	LDBSession* m_dbsessionBK;

private:
	CRedisClient	m_RedisClient;
};

#endif