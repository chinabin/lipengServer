#ifndef GameHandler_h
#define GameHandler_h
#include "LBase.h"
#include "User.h"
//#include "Card.h"
#include "LIni.h"
#include <list>

class Desk;

class GameHandler
{
public:
    virtual bool startup(Desk *game) { return false; }
    virtual void shutdown(void) {}
    virtual void gameStart(void) {}

	virtual void Tick(LTime& curr){}

	 virtual void			HanderUserPlayCard(User* pUser,LMsgC2SUserPlay* msg){}
	 virtual void			HanderUserStartHu(User* pUser,LMsgC2SUserStartHuSelect* msg){}
	 virtual void			HanderUserEndSelect(User*	pUser,LMsgC2SUserEndCardSelect* msg){}		//��Ҵ�������
	 virtual void			HanderUserOperCard(User* pUser,LMsgC2SUserOper* msg){}
	 virtual void			OnUserReconnect(User* pUser){}
	 virtual void			LookUserReconnectOnDeskWait(User* pUser) {}
	// virtual void			OnGameOver(Lint result, Lint winpos, Lint bombpos, Card* winCard){}
	 virtual void			SetDeskPlay(){}
	 virtual void    		ProcessRobot(Lint pos, User * pUser){}
	 virtual void           ProcessAutoPlay(Lint pos, User * pUser){}
	 virtual void           HanderUserSpeak(User* pUser, LMsgC2SUserSpeak* msg){}

	 virtual void           HanderUserTangReq(User* pUser,LMsgC2STangCard* msg) {}

	 // ��һ���
	 virtual void			HanderUserChange(User* pUser, LMsgC2SUserChange* msg){}

	 virtual void			SetPlayType(std::vector<Lint>& playtype){}

	 /////////////////////////////////
	 virtual void			MHSetDeskPlay(Lint PlayUserCount) {}
	 //Ϊ��֧�ֶ�̬��Ա���֣�����Ϸ��ʼ��ʱ���ٴζ���Ϸ������ֵ
	 virtual void			MHSetDeskPlay(Lint PlayUserCount, Lint* PlayerStatus, Lint PlayerCount) {}
	 //��������ƾ��е�����ֵ
	 virtual bool			UpdatePlayerAccumCoins(User* pUser, Lint changeCoins) { return true; }
	 //��ȡ����ƾ��е�����ֵ
	 virtual Lint			GetPlayerAccumCoins(User* pUser) { return -100000; }
	 //��ȡ����ƾ��еĲ�����
	 virtual Lint			GetPlayerBoBoScore(User* pUser) { return -100000; }
	 //��ȡ����ƾ��п���Ĳ�������
	 virtual Lint			GetPlayerCanStartBoBo(User* pUser) { return -100000; }
	 //��ȡ�ƾ��в�����Ϸ���������
	 virtual Lint			GetRealPlayerCount() { return 0; }
	 //��ɢʧ�ܺ�������ҵ���ʱ
	 virtual void			ResetRemainTime() { return; }

	 ///////////////////////////////////
	 //������Ϸ����--(��Ϸ��ʼ-����֮�����Ϸ�йص�����)
	 virtual bool			HanderGameMessage(User* pUser, LMsgC2SGameCommon *msg) { return true; }
	 virtual bool			HanderYingSanZhangAddScore(User* pUser, YingSanZhangC2SAddScore*msg) { return true; }
	 virtual bool           HanderYingSanZhangUserSelectBigSmall(User* pUser, YingSanZhangC2SUserSelectBigSmall* msg) { return true; }


	 //��Ͳ��
	 virtual bool			HanderTuiTongZiAddScore(User* pUser, TuiTongZiC2SScore*msg) { return true; }
	 virtual bool        HanderTuiTongZiSelectZhuang(User* pUser, TuiTongZiC2SSelectZhuang*msg) { return true; }
	 virtual bool        HanderTuiTongZiDoShaiZi(User* pUser, TuiTongZiC2SDoShaiZi*msg) { return true; }
	 virtual bool        HanderTuiTongOpenCard(User* pUser, TuiTongZiC2SOpenCard*msg) { return true; }

	//ţţ
	 /* ţţ������ѡׯģʽ�£���ҷ���ѡׯ��Ϣ */
	 virtual bool        HanderNiuNiuSelectZhuang(User* pUser, NiuNiuC2SSelectZhuang* msg) { return true; }

	 /* ţţ����������� */

	 virtual bool		 HanderNiuNiuMaiMa(User* pUser, NiuNiuC2SMaiMa* msg) { return true; }

	 /* ţţ������ҷ�������ע��Ϣ */
	 virtual bool        HanderNiuNiuAddScore(User* pUser, NiuNiuC2SAddScore* msg) { return true; }

	 /* ţţ������ҷ����Ŀ�����Ϣ */
	 virtual bool        HanderNiuNiuOpenCard(User* pUser, NiuNiuC2SOpenCard* msg) { return true; }

	 /*
	 ţţ�����ѡ���й���Ϣ
	 MSG_C_2_S_NIUNIU_TUO_GUAN = 62084
	 */
	 virtual bool		HandNiuNiuTuoGuan(User* pUser, NiuNiuC2STuoGuan* msg) { return true; }

	 /*
	 ţţ�����ȡ���й���Ϣ
	 MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN = 62085
	 */
	 virtual bool		HandNiuNiuCancelTuoGuan(User* pUser, NiuNiuC2SCancelTuoGuan* msg) { return true; }

	 /*
	 ţţ����Ҵ깫��
	 MSG_C_2_S_NIUNIU_CUO_GONG_PAI = 62088
	 */
	 virtual bool		HandNiuNiuCuoGongPai(User* pUser, NiuNiuC2SCuoGongPai* msg) { return true; }

	 /*
	 �ͻ���֪ͨ��������ʼ��ʱ
	 MSG_C_2_S_START_TIME = 61198
	 */
	 virtual bool		HandNiuNiuStartTime(User* pUser, NiuNiuC2SStartTime* msg) { return true; }


	 // ��������Ϸ��Ϣ
	 virtual bool       HandleDouDiZhuGameMessage(User * pUser, MHLMsgDouDiZhuC2SGameMessage *msg) { return true; }

	 /*
	 ˫����C->S ��ұ�������
	 MSG_C_2_S_SHUANGSHENG_SELECT_ZHU = 62202
	 */
	 virtual bool		HandleShuangShengSelectZhu(User* pUser, ShuangShengC2SSelectZhu* msg) { return true; }

	 /*
	 ˫����C->S ��ҷ�������
	 MSG_C_2_S_SHUANGSHENG_FAN_ZHU = 62204
	 */
	 virtual bool		HandleShangShengFanZhu(User* pUser, ShuangShengC2SFanZhu* msg) { return true; }
	 
	 /*
	 ˫����C->S ��Ҹǵ��Ʋ���
	 MSG_C_2_S_SHUANGSHENG_BASE_CARDS = 62206
	 */
	 virtual bool		HandleShuangShengBaseCards(User* pUser, ShuangShengC2SBaseCards* msg) { return true; }

	 /*
	 ˫����C->S ��ҳ��Ʋ���
	 MSG_C_2_S_SHUANGSHENG_OUT_CARDS = 62208
	 */
	 virtual bool		HandleShuangShengOutCards(User* pUser, ShuangShengC2SOutCards* msg) { return true; }

	 /*
	 3��2��C->S ���ѡ����Ʒ�
	 MSG_C_2_S_SANDAER_SELECT_SCORE = 62253
	 */
	 virtual bool		HandleSanDaErSelectScore(User* pUser, SanDaErC2SSelectScore* msg) { return true; }

	 /*
	 3��2��C->S ���ѡ����
	 MSG_C_2_S_SANDAER_SELECT_MAIN = 62255
	 */
	 virtual bool		HandleSanDaErSelectMain(User* pUser, SanDaErC2SSelectMain* msg) { return true; }

	 /*
	 3��2��C->S ������
	 MSG_C_2_S_SANDAER_BASE_CARD = 62257
	 */
	 virtual bool		HandleSanDaErBaseCard(User* pUser, SanDaErC2SBaseCard* msg) { return true; }

	 /*
	 3��2��C->S ���ѡ�Լ���
	 MSG_C_2_S_SANDAER_SELECT_FRIEND = 62259
	 */
	 virtual bool		HandleSanDaErSelectFriend(User* pUser, SanDaErC2SSelectFriend* msg) { return true; }

	 /*
	 3��2��C->S ��ҳ���
	 MSG_C_2_S_SANDAER_OUT_CARD = 62261
	 */
	 virtual bool		HandleSanDaErOutCard(User* pUser, SanDaErC2SOutCard* msg) { return true; }

	 /*
	 3��2��C->S ��һظ��мҵ÷���105��,�Ƿ�ֹͣ��Ϸ
	 MSG_C_2_S_SANDAER_SCORE_105_RET = 62266,
	 */
	 virtual bool HandleSanDaErScore105Ret(User* pUser, SanDaErC2SScore105Ret* msg) { return true; }

	 /*
	 3��2��C->S ׯ���������
	 MSG_C_2_S_USER_GET_BASECARDS = 61027,		//
	 */
	 virtual bool HandleSanDaErGetBaseCards(User* pUser, SanDaErC2SGetBaseCards* msg) { return true; }

	 /*3��2��C->S �ͻ���ׯ��ѡ����
	 MSG_C_2_S_SANDAER_ZHUANG_JIAO_PAI = 62368
	 */
	 virtual bool HandleSanDaErZhuangRequestJiaoPai(User* pUser, SanDaErC2SZhuangJiaoPai* msg) { return true; }

	 /*
	 C->S ��ׯ��  �ͻ���ѡ��ׯ�ҽ��Ƶ�����
	 MSG_C_2_S_SANDAER_XIAN_SELECT_JIAO_PAI = 62370
	 */
	 virtual bool HandleSanDaErXianSelectJiaoPai(User* pUser, SanDaErC2SXianSelectJiaoPai* msg) { return true; }

	 /*
	 ���˰ٷ֣�C->S ���ѡ����Ʒ�
	 MSG_C_2_S_WURENBAIFEN_SELECT_SCORE = 62353
	 */
	 virtual bool HandleWuRenBaiFenSelectScore(User* pUser, WuRenBaiFenC2SSelectScore* msg) { return true; }

	 /*
	 ���˰ٷ֣�C->S ���ѡ����
	 MSG_C_2_S_WURENBAIFEN_SELECT_MAIN = 62355
	 */
	 virtual bool HandleWuRenBaiFenSelectMain(User* pUser, WuRenBaiFenC2SSelectMain* msg) { return true; }

	 /*
	 ���˰ٷ֣�C->S ������
	 MSG_C_2_S_WURENBAIFEN_BASE_CARD = 62357
	 */
	 virtual bool HandleWuRenBaiFenBaseCard(User* pUser, WuRenBaiFenC2SBaseCard* msg) { return true; }

	 /*
	 ���˰ٷ֣�C->S ���ѡ�Լ���
	 MSG_C_2_S_WURENBAIFEN_SELECT_FRIEND = 62359
	 */
	 virtual bool HandleWuRenBaiFenSelectFriend(User* pUser, WuRenBaiFenC2SSelectFriend* msg) { return true; }

	 /*
	 ���˰ٷ֣�C->S ��ҳ���
	 MSG_C_2_S_WURENBAIFEN_OUT_CARD = 62361
	 */
	 virtual bool HandleWuRenBaiFenOutCard(User* pUser, WuRenBaiFenC2SOutCard* msg) { return true; }

	 /*
	 ���˰ٷ֣�C->S ��һظ��мҵ÷���105��,�Ƿ�ֹͣ��Ϸ
	 MSG_C_2_S_WURENBAIFEN_SCORE_105_RET = 62366,
	 */
	 virtual bool HandleWuRenBaiFenScore105Ret(User* pUser, WuRenBaiFenC2SScore105Ret* msg) { return true; }

	 //����
	 virtual bool HandleWuRenBaiFenZhuangRequestJiaoPai(User* pUser, WuRenBaiFenC2SZhuangJiaoPai* msg) {return true;}
	 virtual bool HandleWuRenBaiFenXianSelectJiaoPai(User* pUser, WuRenBaiFenC2SXianSelectJiaoPai* msg) { return true; }


	 /*
	 3��1��C->S ���ѡ����Ʒ�
	 MSG_C_2_S_SANDAYI_SELECT_SCORE = 62253
	 */
	 virtual bool HandleSanDaYiSelectScore(User* pUser, SanDaYiC2SSelectScore* msg) { return true; }

	 /*
	 3��1��C->S ���ѡ����
	 MSG_C_2_S_SANDAYI_SELECT_MAIN = 62255
	 */
	 virtual bool HandleSanDaYiSelectMain(User* pUser, SanDaYiC2SSelectMain* msg) { return true; }

	 /*
	 3��1��C->S ������
	 MSG_C_2_S_SANDAYI_BASE_CARD = 62257
	 */
	 virtual bool HandleSanDaYiBaseCard(User* pUser, SanDaYiC2SBaseCard* msg) { return true; }

	 /*
	 3��1��C->S ��ҳ���
	 MSG_C_2_S_SANDAYI_OUT_CARD = 62261
	 */
	 virtual bool HandleSanDaYiOutCard(User* pUser, SanDaYiC2SOutCard* msg) { return true; }

	 /*
	 3��1��C->S ��һظ��мҵ÷���105��,�Ƿ�ֹͣ��Ϸ
	 MSG_C_2_S_SANDAYI_SCORE_105_RET = 62266,
	 */
	 virtual bool HandleSanDaYiScore105Ret(User* pUser, SanDaYiC2SScore105Ret* msg) { return true; }

	 /*
	 ���ţ�C->S ��������𲧲�
	 MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401
	 */
	 virtual bool HandlerPlayerSelectBoBo(User* pUser, CheXuanC2SPlayerSelectBoBo* msg) { return true; }

	 /*
	 ���ţ�C->S �������ѡ�����
	 MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 622408
	 */
	 virtual bool HandlerPlayerSelectOpt(User* pUser, CheXuanC2SPlayerSelectOpt* msg) { return true; }

	 /*
	 ���ţ�C->S ������ҳ��Ʋ���
	 MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 622411
	 */
	 virtual bool HandlerPlayerChePai(User* pUser, CheXuanC2SPlayerChePai* msg) { return true; }

	 /*
	 C->S ��Ҵ��Ʋ���
	 MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417
	 */
	 virtual bool HandlerPlayerCuoPai(User* pUser, CheXuanC2SPlayerCuoPai* msg) { return true; }

	 /*
	 C->S �ͻ����ƾ�������ս��
	 MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD = 62420
	 */
	 virtual bool HandlerPlayerRequestRecord(User* pUser, CheXuanC2SPlayerRequestRecord* msg) { return true; }

	 /*
	 C->S �ͻ��˲���Ĭ��
	 MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422
	 */
	 virtual bool HandlerPlayerOptMoFen(User* pUser, CheXuanC2SPlayerOptMoFen* msg) { return true; }

	 /*
	 C->S �ͻ���ѡ���Զ�����
	 MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424
	 */
	 virtual bool HandlerPlayerSelectAutoOpt(User* pUser, CheXuanC2SPlayerSelectAutoOpt* msg) { return true; }

	 /*
	 C->S �ͻ���ѡ��վ�������
	 MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427
	 */
	 virtual bool HandlerPlayerStandOrSeat(User* pUser, CheXuanC2SPlayerSelectStandOrSeat* msg) { return true; }

	 //�����������,desk�ϵ���
	 virtual bool HanderPlayerSeat(User* pUser, Lint seat) { return true; }


	 virtual void			OnGameOver(Lint result, Lint winpos, Lint bombpos) {}
	 virtual void			OnResetDeskEnd() {} // ���ӽ�ɢ�����¼�


	 /////////////////////////////////

protected:
	 //PlayType m_playtype;	// ���ӵ��淨
	 
};

class GameHandlerCreator
{
public:
    virtual GameHandler* create(void) = 0;
};

class GameHandlerFactory
{
public:
    GameHandlerFactory(void);
    ~GameHandlerFactory(void);

    static GameHandlerFactory* getInstance(void);

    GameHandler* create(GameType gameType);
    void destroy(GameType gameType, GameHandler *gameHandler);

    void registerCreator(GameType gameType, GameHandlerCreator *creator);
    void unregisterCreator(GameType gameType);

private:
    std::map<GameType, GameHandlerCreator*> mCreators;
    std::map<GameType, std::list<GameHandler*> > mFreeHandlers;
};


//�����������ã���delimStr���ŷָ�ssrcStrΪvector<std::string>��
static std::vector<std::string> splitString(std::string srcStr, std::string delimStr, bool repeatedCharIgnored)
{
	std::vector<std::string> resultStringVector;
	std::replace_if(srcStr.begin(), srcStr.end(), [&](const char& c) {if (delimStr.find(c) != std::string::npos) { return true; } else { return false; }}/*pred*/, delimStr.at(0));//�����ֵ����зָ������滻��Ϊһ����ͬ���ַ����ָ����ַ����ĵ�һ����  
	size_t pos = srcStr.find(delimStr.at(0));
	std::string addedString = "";
	while (pos != std::string::npos) {
		addedString = srcStr.substr(0, pos);
		if (!addedString.empty() || !repeatedCharIgnored) {
			resultStringVector.push_back(addedString);
		}
		srcStr.erase(srcStr.begin(), srcStr.begin() + pos + 1);
		pos = srcStr.find(delimStr.at(0));
	}
	addedString = srcStr;
	if (!addedString.empty() || !repeatedCharIgnored) {
		resultStringVector.push_back(addedString);
	}
	return resultStringVector;
}

/*
*  �����������ã�����������ƣ�����conFileName�ļ���playerIndex����ҵ����Ƶ�cards��
*  ע��������ʽ�ֶ�Ҫƥ��
*  ������ʽ��
*  player1cards = 0x01, 0x02, 0x03...
*  player2cards = 0x01, 0x02, 0x03...
*  player3cards = 0x01, 0x02, 0x03...
*  ......
*/
static int LoadPlayerCards(std::vector<BYTE>& cards, int playerIndex, const char* conFileName)
{

	if (conFileName == NULL || std::strlen(conFileName) == 0)
	{
		return -1;
	}

	std::string fileName(conFileName);
	LIniConfig config;
	config.LoadFile(conFileName);

	char szKey[32] = { 0 };
	if (playerIndex > 0 && playerIndex < 10)
	{
		sprintf(szKey, "player%dcards", playerIndex);
	}

	std::string cardsString = config.GetString(Lstring(szKey), "");
	std::vector<std::string> cardsStringVec = splitString(cardsString, ",", true);
	if (cardsStringVec.size() > 0)
	{
		cards.clear();
		for (auto it = cardsStringVec.begin(); it < cardsStringVec.end(); it++)
		{
			char *str;
			int i = (int)strtol((*it).c_str(), &str, 16);
			BYTE nValue = (BYTE)i;

			cards.push_back(nValue);
		}
	}
	return 0;
}


/*
*  �����������ã��������ƣ�����conFileName�ļ��е��Ƶ�baseCards��
*  ע��������ʽ�ֶ�Ҫƥ��
*  ������ʽ��
*  basecards = 0x01, 0x02, 0x03...
*/
static int LoadBaseCards(std::vector<BYTE> & baseCards, const char* conFileName)
{
	if (conFileName == NULL || std::strlen(conFileName) == 0)
	{
		return -1;
	}

	std::string fileName(conFileName);
	LIniConfig config;
	config.LoadFile(conFileName);

	std::string cardsString = config.GetString(Lstring("basecards"), "");
	std::vector<std::string> cardsStringVec = splitString(cardsString, ",", true);
	if (cardsStringVec.size() > 0)
	{
		baseCards.clear();
		for (auto it = cardsStringVec.begin(); it < cardsStringVec.end(); it++)
		{
			char *str;
			int i = (int)strtol((*it).c_str(), &str, 16);
			BYTE nValue = (BYTE)i;
			baseCards.push_back(nValue);
		}
	}
	return 0;
}



#define DECLARE_GAME_HANDLER_CREATOR(gameType, className)                           \
class GameHandlerCreator_##className : public GameHandlerCreator                    \
{                                                                                   \
public:                                                                             \
    GameHandlerCreator_##className(void)                                            \
    {                                                                               \
        GameHandlerFactory::getInstance()->registerCreator(gameType,                \
            (GameHandlerCreator*)this);                                             \
    }                                                                               \
    ~GameHandlerCreator_##className(void)                                           \
    {                                                                               \
        GameHandlerFactory::getInstance()->unregisterCreator(                       \
            gameType);                                                              \
    }                                                                               \
    GameHandler* create(void)                                                       \
    {                                                                               \
        return new className();                                                     \
    }                                                                               \
};                                                                                  \
static GameHandlerCreator_##className sGameHandlerCreator_##className;
#endif


//#undef DECLARE_GAME_HANDLER_CREATOR
//#define DECLARE_GAME_HANDLER_CREATOR(gameType, className)
