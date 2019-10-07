#include "RoomVip.h"
#include "Work.h"
#include "LLog.h"
#include "UserManager.h"
#include "Config.h"
//#include "RobotManager.h"
#include "RLogHttp.h"
#include "../mhmessage/mhmsghead.h"

//���Ժ꣬���ƾ�����0:����������1:���Ծ���
#define _MH_DEBUG_D 0

enum {
	FEAT_STARTUP_SCORE = 0, // ÿ�ֳ�ʼ��1000��
};

LVipLogItemForQiPai::LVipLogItemForQiPai() : m_id("")
{
	m_time = 0;
	m_state = 0;	//
	m_deskId = 0;	//��ע
	m_maxCircle = 0;//��Ȧ��
	m_curCircle = 0;//��ǰȦ��
	memset(m_posUserId, 0, sizeof(m_posUserId));
	memset(m_playerState, 0, sizeof(m_playerState));
	m_curZhuangPos = 0;
	m_checkTing[0] = 0;
	m_checkTing[1] = 0;
	m_checkTing[2] = 0;
	m_checkTing[3] = 0;
	//m_score[0] = 0;
	//m_score[1] = 0;
	//m_score[2] = 0;
	//m_score[3] = 0;
	memset(m_score, 0x00, sizeof(m_score));
	memset(m_coins, 0, sizeof(m_coins));
	memset(m_qiangCount, 0, sizeof(m_qiangCount));
	memset(m_zhuangCount, 0, sizeof(m_zhuangCount));
	memset(m_tuiCount, 0, sizeof(m_tuiCount));
	m_reset = 0;
	m_playtype.clear();
	m_curCircleReal = 0;
	m_isInsertDB = 0;
	//m_iPlayerCapacity = MAX_CHAIR_COUNT;  
}


LVipLogItemForQiPai::~LVipLogItemForQiPai()
{
	for (Lsize i = 0; i < m_log.size(); ++i)
	{
		delete m_log[i];
	}
}

Lstring LVipLogItemForQiPai::ToString()
{
	std::stringstream ss;

	ss << "LVipLogItemForQiPai";

	return ss.str();
}

Lstring  LVipLogItemForQiPai::ScoreToString()
{
	std::stringstream ss;
	for (int i = 0; i < m_log.size(); i++)
	{
		ss << m_log[i]->ScoreToString();
		if(i <m_log.size()-1)
			ss << "|";
		if (m_log.size() == 1)
			ss << "|";
	}
	
	return ss.str();

}

void LVipLogItemForQiPai::FromString(const Lstring& str)
{
	
}

Lstring LVipLogItemForQiPai::PlayTypeToStrint()
{
	std::stringstream ss;
	for (Lsize i = 0; i < m_playtype.size(); ++i)
	{
		ss << m_playtype[i] << ";";
	}
	return ss.str();
}

void LVipLogItemForQiPai::PlayTypeFromString(const Lstring& str)
{
	std::vector<Lstring> strSplid;

	L_ParseString(str, strSplid, ";");
	if (strSplid.size() > 0)
	{
		for (size_t j = 0; j < strSplid.size(); ++j)
		{
			m_playtype.push_back(atoi(strSplid[j].c_str()));
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

VipLogItem::VipLogItem()
{
	m_desk = NULL;
}

VipLogItem::~VipLogItem()
{

}

Lint VipLogItem::GetOwerId()
{
	return m_posUserId[0];
}

//void VipLogItem::AddLog(User** user, Lint* gold, HuInfo* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting)
//{
//	VipDeskLogForQiPai* log = new VipDeskLogForQiPai();
//	log->m_videoId = videoId;
//	log->m_zhuangPos = zhuangPos;
//	log->m_time = gWork.GetCurTime().Secs();
//	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
//	{
//		log->m_gold[i] = gold[i];
//		m_score[i] += gold[i];
//		
//	}
//	m_log.push_back(log);
//
//	// ����ϵ㣺�ֽ�����ʱ�򣬻�ûˢ�¿ͻ��ˣ���һ�ζϵ�ʱ��m_curCircleRealΪ0��m_curCircleReal���ڸõ����
//	m_curCircleReal += 1;
//	//m_curCircle = m_curCircle; // �ϵ�鿴��
//	m_curZhuangPos = zhuangPos;
//
//	if(m_curCircleReal == 1)
//	{
//		InsertToDb();
//	}
//	else
//	{
//		UpdateToDb();
//	}
//	//ÿȦ�������Ȧ��ͳ�ƺ���Ӯ״��
//	/*
//	for (Lint i = 0; i < m_iPlayerCapacity; i++)
//	{
//		if (user[i] != NULL)
//		{
//			user[i]->m_userData.m_customInt[0]++;
//			LMsgL2LMGAddUserCircleCount msg;
//			msg.m_userid = user[i]->m_userData.m_id;
//			msg.m_strUUID = user[i]->m_userData.m_unioid;
//			gWork.SendToLogicManager(msg);
//		}
//	}
//	*/
//
//	//�ڰ˾ִ���Ȧ��
//	// ��ׯ���Ӿ���Ҳû���⣬12345677777778 ֻ�����һ��8
//	if (m_curCircle == m_maxCircle)
//	{
//		for (Lint i = 0; i < m_iPlayerCapacity; i ++)
//		{
//			if (user[i] != NULL)
//			{
//				user[i]->m_userData.m_totalplaynum ++;
//				LMsgL2LMGAddUserPlayCount msg;
//				msg.m_userid = user[i]->m_userData.m_id;
//				msg.m_strUUID = user[i]->m_userData.m_unioid;
//				gWork.SendToLogicManager(msg);
//			}
//		}
//	}
//	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);
//}

//void VipLogItem::AddLog(User** user, Lint* gold, std::vector<HuInfo>* huinfo, Lint zhuangPos, Lint* agang, Lint* mgang, Lstring& videoId, Lint* ting)
//{
//	VipDeskLogForQiPai* log = new VipDeskLogForQiPai();
//	log->m_videoId = videoId;
//	log->m_zhuangPos = zhuangPos;
//	log->m_time = gWork.GetCurTime().Secs();
//	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
//	{
//		log->m_gold[i] = gold[i];
//		m_score[i] += gold[i];
//		
//	}
//	m_log.push_back(log);
//	
//	// ����ϵ㣺�ֽ�����ʱ�򣬻�ûˢ�¿ͻ��ˣ���һ�ζϵ�ʱ��m_curCircleRealΪ0��m_curCircleReal���ڸõ����
//	m_curCircleReal += 1;
//	//m_curCircle = m_curCircle; // �ϵ�鿴��
//	m_curZhuangPos = zhuangPos;
//
//	if(m_curCircleReal == 1)
//	{
//		InsertToDb();
//	}
//	else
//	{
//		UpdateToDb();
//	}
//
//	//�ڰ˾ִ���Ȧ��
//	// ��ׯ���Ӿ���Ҳû���⣬12345677777778 ֻ�����һ��8
//	if (m_curCircle == m_maxCircle)
//	{
//		for (Lint i = 0; i < m_iPlayerCapacity; i ++)
//		{
//			if (user[i] != NULL)
//			{
//				user[i]->m_userData.m_totalplaynum ++;
//				LMsgL2LMGAddUserPlayCount msg;
//				msg.m_userid = user[i]->m_userData.m_id;
//				msg.m_strUUID = user[i]->m_userData.m_unioid;
//				gWork.SendToLogicManager(msg);
//			}
//		}
//	}
//	LLOG_DEBUG("VipLogItem::addLog %s:%d:%d", m_id.c_str(), m_log.size(), m_curCircle);
//}

/* �����������ֵ */
void VipLogItem::UpdatePlayerCoins(Lint* changeCoins, Lint playerCount)
{
	Lint t_playerCount = playerCount <= m_iPlayerCapacity ? playerCount : m_iPlayerCapacity;
	for (Lint i = 0; i < t_playerCount; ++i)
	{
		m_coins[i] += changeCoins[i];
	}
}

//����ţţ���ÿ��������ͳ��
void VipLogItem::UpdateNiuNiuOptCount(Lint* qiangCount, Lint* zhuangCount, Lint* tuiCount, Lint playerCount)
{
	Lint t_playerCount = m_iPlayerCapacity <= playerCount ? m_iPlayerCapacity : playerCount;
	for (size_t i = 0; i < t_playerCount; ++i)
	{
		m_qiangCount[i] = qiangCount[i];
		m_zhuangCount[i] = zhuangCount[i];
		m_tuiCount[i] = tuiCount[i];
	}
}

//ţţ��Ҳ���ͳ�����л������1��ׯ���������2��ׯ����...|���1��ׯ���������2��ׯ����...|���1��ע���������2�˳�������
Lstring VipLogItem::NiuNiuOptCountToString()
{
	Lint t_playerCount = m_iPlayerCapacity;
	std::stringstream ss;
	for (size_t i = 0; i < t_playerCount; ++i)
	{
		ss << m_qiangCount[i];
		ss << ",";
		ss << m_zhuangCount[i];
		ss << ",";
		ss << m_tuiCount[i];

		if (i < t_playerCount - 1)
		{
			ss << ";";
		}
	}
	return ss.str();
}

void VipLogItem::AddLogForQiPai(User** user, Lint* gold, Lint * playerState, Lint zhuangPos, Lstring& videoId,const  std::vector<std::string> & srcPaiXing)
{
	VipDeskLogForQiPai* log = new VipDeskLogForQiPai();
	log->m_videoId = videoId;
	log->m_zhuangPos = zhuangPos;
	log->m_time = gWork.GetCurTime().Secs();
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		//���������Ϸ״̬
		m_playerState[i] = playerState[i];

		log->m_gold[i] = gold[i];  //����ֻ���
		m_score[i] += gold[i];     //�ۼӳ�����
		m_coins[i] += gold[i];		//�ۼӳ�����ֵ
	}
	m_log.push_back(log);

	// ����ϵ㣺�ֽ�����ʱ�򣬻�ûˢ�¿ͻ��ˣ���һ�ζϵ�ʱ��m_curCircleRealΪ0��m_curCircleReal���ڸõ����
	m_curCircleReal += 1;	
	m_curZhuangPos = zhuangPos;

	if (m_curCircleReal == 1)
	{
		if (m_reset == 0)
		{
			InsertToDb();  //�����һ�ֻ���
		}
	}
	else
	{
		UpdateToDb();  //�����ܻ��ֺͽ�ɢ��־
	}

	// �����¼�ֻ���
	if (m_reset == 0)
	{
		LTime curTime;
		//std::vector<Lint> card = srcPaiXing;
		for (int i = 0; i < m_iPlayerCapacity; i++)
		{
			if (m_posUserId[i] > 0 && m_playerState[i] == 1)
			{
				//����ÿ���û��þֵĳɼ�			 	 
				char buf[256];
				sprintf_s(buf, "%08d%d", m_posUserId[i], curTime.Secs());
				Lstring str_user_draw_id(buf);		
				if(i< srcPaiXing.size())
					this->InsertUserScoreToDB(i, str_user_draw_id, 0, gold[i], srcPaiXing[i]);
				else
					this->InsertUserScoreToDB(i, str_user_draw_id, 0, gold[i], "");

				this->m_mapUserIdToDrawId[m_posUserId[i]] = str_user_draw_id;
			}
		}
	}

	if (m_reset) //�����ɢ
	{
		if (m_desk && m_desk->m_bIsDissmissed && m_curCircle == 1) //��1���м��ɢ
		{
			//��ʱ�������Ӯ������
			m_desk->m_bIsDissmissed = false;
		}
		else
		{
			//�����Ӯ�ҵ����ݵ����ݿ�
			this->InsertDeskWinnerInfo();
		}

	}
	else if (this->isNormalEnd()) //�������ν���
	{
		//�����Ӯ�ҵ����ݵ����ݿ�
		this->InsertDeskWinnerInfo();
	}
	
	//�ڰ˾ִ���Ȧ��
	// ��ׯ���Ӿ���Ҳû���⣬12345677777778 ֻ�����һ��8
	bool isSend = true;  //
	if (m_curCircle == m_maxCircle && isSend)
	{
		MHLMsgL2LMGNewAgencyActivityUpdatePlayCount msgAgency;
		Lint i=0;
		for (; i<m_iPlayerCapacity; i++)
		{
			if (user[i])break;
		}
		if (user[i] && m_reset == 0)
		{
			msgAgency.m_openID = user[i]->m_userData.m_openid;
			msgAgency.m_userid = user[i]->GetUserDataId();
			msgAgency.m_strUUID = user[i]->m_userData.m_unioid;
			msgAgency.m_deskId = this->m_deskId;
			if (this->m_desk&&this->m_desk->m_clubInfo.m_clubId != 0 && this->m_desk->m_clubInfo.m_showDeskId != 0)
				msgAgency.m_deskId = this->m_desk->m_clubInfo.m_showDeskId;
			msgAgency.m_playType = m_state;
			msgAgency.m_flag = 1;
			msgAgency.m_playerCount = m_iPlayerCapacity;
			for (int i = 0; i < m_iPlayerCapacity; i++)
			{
				msgAgency.m_playerId[i] = m_posUserId[i];
				msgAgency.m_score[i] = m_score[i];
			}
			msgAgency.m_maxCircle = m_maxCircle;
			if (m_desk)
			{
				//char tempTime[32 + 1] = {};
				//strftime(tempTime, sizeof(tempTime) - 1, "%Y-%m-%d %H:%M", localtime(&(m_desk->m_gameStartTime)));
				msgAgency.m_startTime = m_desk->m_gameStartTime;// std::string(tempTime);
			}
			LLOG_DEBUG("LOGWYZ  SendToLogicManager  msgAgency ");
			gWork.SendToLogicManager(msgAgency);
		}

	}
	LLOG_DEBUG("VipLogItem::AddLog qipai %s:%d:%d desk:%d", m_id.c_str(), m_log.size(), m_curCircle, m_deskId);
}


bool VipLogItem::ExiestUser(Lint id)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (id == m_posUserId[i])
			return true;
	}
	return false;
}

Lint VipLogItem::GetCurrentPlayerCount()
{
	Lint count = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_posUserId[i] > 0 && m_playerState[i] == 1)
			count++;
	}
	return count;
}

void VipLogItem::InsertToDb()
{
	if (m_desk==NULL)return;
	m_isInsertDB = 1;
	LTime t;
	std::stringstream ss;
	ss << "INSERT INTO ";	
	ss << "qipaiplayinfo ";
	ss << "(Id, ServerId, ClubId, ClubPlayTypeId, DeskId, RealDeskId, IsCoinsGame, DeskIsDymic, CreateUserId, DeskFlag, DeskPlayType, CurCircle, MaxCircle, ";
	ss << "IsDismiss,DismissUserId,DeskFeeType,DeskFeeTotal,PlayerCount,Pos1,Pos2,Pos3,Pos4,Pos5,Pos6,Pos7,Pos8,Pos9,Pos10, ";
	ss << "WinnerCount, Score1, Score2, Score3, Score4, Score5, Score6, Score7, Score8, Score9, Score10, ctime, utime, Time, ScoreData, OptData)";
	ss << "VALUES (";
	ss << "'" << m_id << "',";
	ss << "'" << gConfig.GetServerID() << "',";
	ss << "'" << m_desk->m_clubInfo.m_clubId<<"',";
	ss << "'" << m_desk->m_clubInfo.m_playTypeId<<"',";
	ss << "'" << (m_desk->m_clubInfo.m_clubId ? m_desk->m_clubInfo.m_showDeskId : m_deskId) << "',";
	ss << "'" << m_deskId << "',";
	ss << "'" << ((m_desk->m_clubInfo.m_clubId && m_desk->m_isCoinsGame == 1) ? 1 : 0) << "',";
	ss << "'" << m_desk->m_Greater2CanStart << "',";
	ss << "'" << m_desk->m_creatUserId << "',";
	ss << "'" << m_state << "',";
	ss << "'" << this->PlayTypeToStrint() << "',";
	ss << "'" << m_curCircle << "',";
	ss << "'" << m_maxCircle << "',";
	ss << "'" << m_reset << "',";
	ss << "'" << m_desk->m_resetUserId << "',";
	ss << "'" << m_desk->m_feeType << "',";
	ss << "'" << 0 << "',";
	ss << "'" << GetCurrentPlayerCount() << "',"; //player count
	for (int i = 0; i < 10; i++)
	{
		ss << "'" << m_posUserId[i] << "',";
	}
	ss << "'" << 0 << "',"; // winner count
	for (int i = 0; i < 10; i++) //score
	{
		ss << "'" << m_score[i] << "',";
	} 
	ss << "FROM_UNIXTIME(";
	ss << m_time << "), NOW(), '" <<m_time << "',";
	ss << "'" << this->ScoreToString() << "',";
	ss << "'" << this->NiuNiuOptCountToString() << "'";		//ţţ����ܵ���ׯ����ׯ����ע����
	ss << ")";

	LMsgL2LDBSaveLogItem send;
	send.m_userId = m_posUserId[0];
	send.m_type = 0;
	send.m_sql = ss.str();
	gWork.SendMsgToDb(send);
}

bool VipLogItem::IsBegin()
{
	return m_curCircle != 0;
}

void VipLogItem::RemoveUser(Lint id)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (id == m_posUserId[i])
			m_posUserId[i] = 0;
	}
}

void VipLogItem::UpdateToDb()
{
	std::stringstream ss;
	ss << "UPDATE";	
	ss << " qipaiplayinfo  ";
	ss << "SET CurCircle='" << m_curCircle << "',";
	ss << "IsDismiss='" << m_reset << "',";
	ss << "DismissUserId='" << m_desk->m_resetUserId << "',";
	ss << "PlayerCount='" << this->GetCurrentPlayerCount() << "',";
	ss << "ScoreData='" << this->ScoreToString() << "',";
	ss << "OptData='" << this->NiuNiuOptCountToString() << "',";
	for (int i = 0; i < m_iPlayerCapacity; i++)
	{
		if (m_playerState[i] == 1)
		{
			ss << "Pos" << (i + 1) << "='" << m_posUserId[i] << "',";
			ss << "Score" << (i + 1) << "='" << m_score[i] << "',";
		}
	}	
	ss << "Time='" << time(NULL) << "',";
	ss << "utime=FROM_UNIXTIME(" << time(NULL) << ")  WHERE Id=";
	//ss << "utime=NOW()"  << " WHERE Id=";
	ss << "'" << m_id << "'";

	LMsgL2LDBSaveLogItem send;

	send.m_userId = m_posUserId[0];
	send.m_type = 1;
	send.m_sql = ss.str();	
	gWork.SendMsgToDb(send);
}

bool VipLogItem::IsFull(User* user)
{
	Lint count = 0;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (m_posUserId[i] == 0 || user->GetUserDataId() == m_posUserId[i])
			return false;
	}

	return true;
}

// ����Ȧ����Ϣ
void VipLogItem::SendInfo()
{
	LMsgS2CVipInfo info;
	info.m_curCircle = m_curCircle ;
	info.m_curMaxCircle = m_maxCircle ;
	info.m_playtype = m_playtype;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		User* user = gUserManager.GetUserbyDataId(m_posUserId[i]);
		if (user)
			user->Send(info);
	}

	//����ս�������Ȧ����Ϣ(**Ren 2018-5-18)
	if (m_desk == NULL)
	{
		LLOG_ERROR("RoomVip::SendInfo() desk is NULL.... currCircle info to all look on user...");
		return;
	}
	//�㲥����ս���
	m_desk->MHBoadCastDeskLookonUser(info);

	//�㲥�����ֲ��᳤���ƹ�ս
	m_desk->MHBoadCastClubOwerLookOnUser(info);
}
/*
void VipLogItem::SendEnd()
{
	LLOG_DEBUG("VipLogItem::SendEnd()");
 	LMsgS2CVipEnd msg;
 	User* user[DESK_USER_COUNT] = { NULL };
 	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
 	{
 		user[i] = gUserManager.GetUserbyDataId(m_posUserId[i]);
 		if (user[i])
 		{
 			msg.m_nike[i] = user[i]->m_userData.m_nike;
 		}
		msg.m_id[i] = m_posUserId[i];
 	}
	LTime cur;
	Llong time = cur.MSecs();
	memset(msg.m_gold,0,sizeof(msg.m_gold));
	memset(msg.m_zimo,0,sizeof(msg.m_zimo));
	memset(msg.m_win,0,sizeof(msg.m_win));
	memset(msg.m_bomb,0,sizeof(msg.m_bomb));
	memset(msg.m_dzimo,0,sizeof(msg.m_dzimo));
	memset(msg.m_dwin,0,sizeof(msg.m_dwin));
	memset(msg.m_dbomb,0,sizeof(msg.m_dbomb));
	memset(msg.m_agang,0,sizeof(msg.m_agang));
	memset(msg.m_mgang,0,sizeof(msg.m_mgang));
	memset(msg.m_CheckTing,0,sizeof(msg.m_CheckTing));

 	for (Lsize i = 0; i < m_log.size(); ++i)
 	{
 		for(Lint  j = 0; j < m_iPlayerCapacity; ++j)
 		{
 			msg.m_gold[j] += m_log[i]->m_gold[j];
			msg.m_mgang[j] += m_log[i]->m_mgang[j];
			msg.m_agang[j] += m_log[i]->m_angang[j];
			msg.m_CheckTing[j] += m_log[i]->m_checkTing[j];

			msg.m_zimo[j] += m_log[i]->m_zimo[j];
			msg.m_bomb[j] += m_log[i]->m_bomb[j];
			msg.m_win[j] += m_log[i]->m_win[j];
			msg.m_dzimo[j] += m_log[i]->m_dzimo[j];
			msg.m_dbomb[j] += m_log[i]->m_dbomb[j];
			msg.m_dwin[j] += m_log[i]->m_dwin[j];
 		}
 	}
	msg.m_time = time;
	msg.m_creatorId = m_desk->m_creatUserId;
	msg.m_creatorNike = m_desk->m_creatUserNike;
	msg.m_creatorHeadUrl = m_desk->m_creatUserUrl;
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (user[i])
		{
			LLOG_DEBUG("VipLogItem::SendEnd()  send userid [%d]",user[i]->GetUserDataId());
			user[i]->Send(msg);
		}
	}

	// Զ����־
	RLOG("bswf", LTimeTool::GetLocalTimeInString()
		<< "|" << 15001
		<< "|" << m_state
		<< "|" << PlayTypeToString4RLog()
		<< "|" << m_maxCircle
		<< "|" << m_curCircle
		<< "|" << m_reset
		<< "|" << m_posUserId[0]
		<< ";" << m_posUserId[1]
		<< ";" << m_posUserId[2]
		<< ";" << m_posUserId[3]
		<< "|" << gConfig.GetInsideIp()
		<< "|" << gConfig.GetInsidePort()
		<< "|" << gConfig.GetServerID()
		);
}
*/

//���ν����������
void VipLogItem::SendEnd()
{
	//LLOG_DEBUG("VipLogItem::SendEnd() Run... deskId=[%d], m_iPlayerCapacity=[%d]", m_deskId, m_iPlayerCapacity);
	//LLOG_DEBUG("");
	MHLMsgL2LMGNewAgencyActivityUpdatePlayCount updatePoint;
	updatePoint.m_flag = 4;
	updatePoint.m_playerCount = m_desk->m_iPlayerCapacity;

	LMsgS2CGameEndResult msg;
	if(m_isInsertDB==1)
		msg.m_playId = m_id;
	msg.m_EffectiveUserCount=m_iPlayerCapacity;
	msg.m_zhuangPos = m_curZhuangPos;
	msg.m_maxCircle = m_maxCircle;

	//�û�״̬
	if (m_desk)
	{
		for (int i=0; i<m_iPlayerCapacity; i++)
		{
			//msg.m_UserState[i]=m_desk->m_playStatus[i];
			if (m_desk->m_user[i])
			{
				msg.m_UserState[i] = 1;
				msg.m_qiangCount[i] = m_qiangCount[i];
				msg.m_zhuangCount[i] = m_zhuangCount[i];
				msg.m_tuiCount[i] = m_tuiCount[i];
			}
		}
	}

	//�û�ͷ���ǳ�,id
	User* user[MAX_CHAIR_COUNT]={ NULL };
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		user[i]=gUserManager.GetUserbyDataId(m_posUserId[i]);
		if (user[i]&&msg.m_UserState[i])
		{
			msg.m_Nikes[i]=user[i]->m_userData.m_nike;
			msg.m_HeadUrls[i]=user[i]->m_userData.m_headImageUrl;
			msg.m_UserIds[i]=m_posUserId[i];

			updatePoint.m_playerId[i] = m_posUserId[i];
		}
	}


	LTime cur;
	Llong time=cur.MSecs();
	for (Lint j=0; j < m_iPlayerCapacity; ++j)
	{
		msg.m_Score[j]=m_score[j];
		updatePoint.m_score[j] = m_score[j];
		//LLOG_DEBUG("Logwyz . [%d]=[%d]",j,m_score[j]);
	}

	//���͸�manager���·���
	if (m_desk&& m_desk->m_clubInfo.m_clubId != 0 && m_desk->m_isCoinsGame == 1)
	{
		updatePoint.m_clubId = m_desk->m_clubInfo.m_clubId;
		LLOG_DEBUG("SendEnd()  TO MANAGER clubId[%d]", updatePoint.m_clubId);
		gWork.SendToLogicManager(updatePoint);
	}

	msg.m_Time=time;
	msg.m_CreatorId=m_desk->m_creatUserId;
	msg.m_CreatorNike=m_desk->m_creatUserNike;
	msg.m_CreatorHeadUrl=m_desk->m_creatUserUrl;
	for (Lint i=0; i < m_iPlayerCapacity; ++i)
	{
		if (user[i])
		{
			LLOG_DEBUG("VipLogItem::SendEnd() Send End Msg to Client... deskId=[%d], send userid [%d]", m_deskId, user[i]->GetUserDataId());
			user[i]->Send(msg);
		}
	}

	//// Զ����־
	//RLOG("bswf", LTimeTool::GetLocalTimeInString()
	//	<<"|"<<15001
	//	<<"|"<<m_state
	//	<<"|"<<PlayTypeToString4RLog()
	//	<<"|"<<m_maxCircle
	//	<<"|"<<m_curCircle
	//	<<"|"<<m_reset
	//	<<"|"<<m_posUserId[0]
	//	<<";"<<m_posUserId[1]
	//	<<";"<<m_posUserId[2]
	//	<<";"<<m_posUserId[3]
	//	<<"|"<<gConfig.GetInsideIp()
	//	<<"|"<<gConfig.GetInsidePort()
	//	<<"|"<<gConfig.GetServerID()
	//);
}

Lstring VipLogItem::PlayTypeToString4RLog()
{
	std::stringstream ss;
	if (!m_playtype.empty())
	{
		ss << m_playtype.front();
	}

	for (Lsize i = 1; i < m_playtype.size(); ++i)
	{
		ss << ";" << m_playtype[i];
	}
	return ss.str();
}

bool VipLogItem::isEnd()
{
	return m_maxCircle <= m_curCircle || m_reset == 1;
}

bool VipLogItem::isNormalEnd()
{
	return m_maxCircle == m_curCircle;
}

Lint VipLogItem::GetUserPos(User* user)
{
	if (!user)
	{
		return INVAILD_POS_QIPAI;
	}
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (user->GetUserDataId() == m_posUserId[i])
			return i;
	}

	return INVAILD_POS_QIPAI;
}

Lint VipLogItem::GetUserScore(User* user)
{
	Lint pos = GetUserPos(user);
	if (pos != INVAILD_POS_QIPAI)
	{
		return m_score[pos];
	}
	return 0;
}

bool VipLogItem::AddUser(User* user)
{
	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (user->GetUserDataId() == m_posUserId[i])
			return true;
		
		if (m_posUserId[i] == 0)
		{
			m_posUserId[i] = user->GetUserDataId();
			return true;
		}
	}

	return false;
}

bool  VipLogItem::ResetUser(User *user[] )
{

	for (Lint i = 0; i < m_iPlayerCapacity; ++i)
	{
		if (user[i])
			m_posUserId[i] = user[i]->GetUserDataId();
		else
			m_posUserId[i] = 0;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////
// Add New log table
void VipLogItem::CreateTableId() // ������Id
{
	char buf[256];
	sprintf_s(buf, "%d%d%d", gConfig.GetServerID(), m_desk->GetDeskId(), LTime().Secs());
	m_tableId = buf;
	LLOG_DEBUG("***generate table id: %s", buf);
}

void VipLogItem::SaveDeskInfoToDB()      // �洢�����ݿ�
{
	if (!m_desk || m_tableId.empty() || m_id.empty())
	{
		LLOG_ERROR("erorr desk state: %d: %s: %s", m_deskId, m_tableId.c_str(), m_id.c_str());
	}
	std::stringstream ss;
	ss << "INSERT INTO playinfo ";
	ss << "(Id, LogId, ServerId, ClubId, ClubPlayTypeId, DeskId, DeskIsDymic, CreateUserId, DeskFlag, DeskPlayType, CurCircle, MaxCircle, IsDismiss, DismissUserId, DeskFeeType, DeskFeeTotal, PlayerCount, Pos1, Pos2, Pos3, Pos4, WinnerCount, TotalScore1, TotalScore2, TotalScore3, TotalScore4, ctime, utime) VALUES (";
	ss << "'" << m_tableId << "',";
	ss << "'" << m_id << "',";
	ss << "'" << gConfig.GetServerID() << "',";
	ss << "'" << 0 << "',";
	ss << "'" << 0 << "',";
	ss << "'" << m_deskId << "',";
	ss << "'" << m_desk->m_Greater2CanStart << "',";
	ss << "'" << m_desk->m_creatUserId << "',";
	ss << "'" << m_desk->m_state << "',";
	ss << "'" << PlayTypeToStrint() << "',";
	ss << "'" << m_curCircle << "',";
	ss << "'" << m_maxCircle << "',";
	ss << "'" << 0 << "',";
	ss << "'" << 0 << "',";
	ss << "'" << m_desk->m_feeType << "',";
	ss << "'" << 0 << "',";
	ss << "'" << m_desk->m_desk_user_count << "',";
	ss << "'" << m_posUserId[0] << "',";
	ss << "'" << m_posUserId[1] << "',";
	ss << "'" << m_posUserId[2] << "',";
	ss << "'" << m_posUserId[3] << "',";
	ss << "'" << 0 << "',";
	ss << "'" << m_score[0] << "',";
	ss << "'" << m_score[1] << "',";
	ss << "'" << m_score[2] << "',";
	ss << "'" << m_score[3] << "',";
	ss << "FROM_UNIXTIME(";
	ss << m_time << "), NOW())";
	LMsgL2LDBSaveLogItem send;

	send.m_userId = m_posUserId[0];
	send.m_type = 0;
	send.m_sql = ss.str();
	gWork.SendMsgToDb(send);
	LLOG_DEBUG("***Insert to playinfo: sql=%s", send.m_sql.c_str());

}


void VipLogItem::UpdateDeskTotalFeeValueToDB(Lint nFeeValue)
{
	std::stringstream ss;
	ss << "UPDATE qipaiplayinfo";
	ss << " SET DeskFeeTotal = '" << nFeeValue << "'";
	ss << " WHERE Id = '" << m_id << "'";

	LMsgL2LDBSaveLogItem send;
	send.m_userId = m_posUserId[0];
	send.m_type = 1;
	send.m_sql = ss.str();
	//gWork.SendToCenter(send);
	gWork.SendMsgToDb(send);
	LLOG_ERROR("***Update desk fee to playinfo: sql=%s", send.m_sql.c_str());
}

void VipLogItem::InsertUserScoreToDB(Lint pos, Lstring &drawId, Lint isWin, Lint score, const std::string & srcPaiXing)
{
	if (pos < 0 || pos >= m_iPlayerCapacity)
	{
		LLOG_ERROR("erorr pos: %d:pos:%d", m_deskId, pos);
		return;
	}
	if (!m_desk || m_id.empty())
	{
		LLOG_ERROR("erorr desk state: %d: %s: %s", m_deskId, m_id.c_str());
		return;
	}
	
	std::stringstream ss;
	ss << "INSERT INTO qipaidrawinfo ";
	ss << "(Id, PlayId, UserId, ServerId, GameType, DeskId, ClubId, UserPos, CurCircle, Score, WinCard, FeeValue, ctime, utime) VALUES (";
	ss << "'" << drawId << "',";
	ss << "'" << m_id << "',";
	ss << "'" << m_posUserId[pos] << "',";
	ss << "'" << gConfig.GetServerID() << "',";
	ss << "'" << m_state << "',";
	ss << "'" << m_deskId << "',";
	ss << "'" << (m_desk ? m_desk->m_clubInfo.m_clubId : 0) << "',";
	ss << "'" << pos << "',";
	ss << "'" << m_curCircle << "',";
	ss << "'" << score << "',";
	ss << "'" << srcPaiXing << "',";
	ss << "'" << 0 << "',";	
	ss << " NOW(), NOW())";


	LMsgL2LDBSaveLogItem send;
	send.m_userId = m_posUserId[pos];
	send.m_type = 0;
	send.m_sql = ss.str();	
	gWork.SendMsgToDb(send);
	LLOG_ERROR("***Insert to qipaidrawinfo: sql=%s", send.m_sql.c_str());

}

void VipLogItem::UpdateUserFeeValueToDB(Lint userId, Lint feeValue)
{
	std::map<Lint, Lstring>::iterator it = m_mapUserIdToDrawId.find(userId);
	if (it == m_mapUserIdToDrawId.end())
	{
		return;
	}

	std::stringstream ss;
	ss << "UPDATE qipaidrawinfo ";
	ss << " SET FeeValue = '" << feeValue << "'";
	ss << " WHERE Id = '" << m_mapUserIdToDrawId[userId] << "'";

	LMsgL2LDBSaveLogItem send;
	send.m_userId = m_posUserId[0];
	send.m_type = 1;
	send.m_sql = ss.str();

	gWork.SendMsgToDb(send);
	LLOG_ERROR("*****Update user fee to qipaidrawinfo: sql=%s", send.m_sql.c_str());
}

// ����ĳ���Ӯ�ң����浽���ݿ�
void VipLogItem::InsertDeskWinnerInfo()
{
	std::vector<MHUserScore> score_vec(6);
	std::vector<MHUserScore> win_user_vec(6);
	score_vec.clear();
	win_user_vec.clear();

	for (int i = 0; i < this->m_iPlayerCapacity; i++)
	{
		if (m_posUserId[i] > 0)
		{
			score_vec.push_back(MHUserScore(i, m_posUserId[i], m_score[i]));
		}
	}
	std::sort(score_vec.begin(), score_vec.end());
	int  size = (int)score_vec.size();
	for (int i = size - 1; i > 0; i--)
	{
		if (true) // ���ж�0�ˣ�ÿ��������ֻ���ѡ��1��
		{
			win_user_vec.push_back(score_vec[i]);
			if (score_vec[i] > score_vec[i - 1]) // ǰ��λ��
			{
				break;
			}
		}
	}
	if (win_user_vec.empty())return;
	if (win_user_vec[win_user_vec.size()-1].m_score == 0)return;

	char record_id[48];
	LTime curTime;
	LLOG_DEBUG("***** Desk:%d  winner_count:%d", m_deskId, win_user_vec.size());

	LMsgL2LDBSaveLogItem send;
	std::stringstream ss;
	ss << "UPDATE qipaiplayinfo SET ";
	ss << " WinnerCount = '" << 1 << "',";
	ss << " utime = NOW() ";
	ss << " WHERE Id = '" << m_id << "'";
	send.m_userId = m_posUserId[0];
	send.m_type = 1;
	send.m_sql = ss.str();
	gWork.SendMsgToDb(send);
	LLOG_DEBUG("***Update winner count: sql: ��%s��", send.m_sql.c_str());

	int nIndex = 0;
	// ���ѡ��һ�����Ϊ��Ӯ��
	if (win_user_vec.size() > 0)
	{
		nIndex = curTime.Secs() % win_user_vec.size();
	}
	if (nIndex < win_user_vec.size())
	{
		std::stringstream ss;
		sprintf_s(record_id, 48, "%08d%d", win_user_vec[nIndex].m_userId, curTime.Secs());
		ss << "INSERT INTO qipaiwinnerinfo ";
		ss << "(Id, PlayId, UserId, ClubId,DeskFlag, Score, ctime, utime) VALUES (";
		ss << "'" << Lstring(record_id) << "',";
		ss << "'" << m_id << "',";
		ss << "'" << win_user_vec[nIndex].m_userId << "',";
		ss << "'" << (m_desk ? m_desk->m_clubInfo.m_clubId : 0) << "',";
		ss << "'" << (m_desk ? m_desk->m_state : 0) << "',";
		ss << "'" << win_user_vec[nIndex].m_score << "',";
		ss << "" << "NOW()" << ",";
		ss << "NOW())";

		LMsgL2LDBSaveLogItem send;
		send.m_userId = win_user_vec[nIndex].m_userId;
		send.m_type = 0;
		send.m_sql = ss.str();
		gWork.SendMsgToDb(send);
		LLOG_DEBUG("***Insert into winnerinfo: sql: ��%s��", send.m_sql.c_str());

		if (m_desk&& m_desk->m_clubInfo.m_clubId != 0)
		{
			SendBigWinnerInfo(m_desk->m_clubInfo.m_clubId, m_desk->m_clubInfo.m_showDeskId, win_user_vec[nIndex].m_userId, win_user_vec[nIndex].m_pos < 4 ? m_desk->m_user[(win_user_vec[nIndex].m_pos)]->m_userData.m_nike : std::string(""), m_desk->m_state, 10000);
		}

	}

}

//////////////////////////////////////////////////////////////////////////
bool VipLogMgr::Init()
{
	m_id = 1;
	m_videoId = 1;
	return true;
}

bool VipLogMgr::Final()
{
	return true;
}

void VipLogMgr::SetVipId(Lint id)
{
	m_id = id;
}

Lstring VipLogMgr::GetVipId()
{
	++m_id;
	m_id = m_id % 5000;
	char mVipLogID[64] = { 0 };
	sprintf_s(mVipLogID, sizeof(mVipLogID)-1, "%d%d%d", gConfig.GetServerID(), gWork.GetCurTime().Secs(), m_id);
	return mVipLogID;
}

void VipLogMgr::SetVideoId(Lint id)
{
	m_videoId = id;
}

Lstring VipLogMgr::GetVideoId()
{
	++m_videoId;
	m_videoId = m_videoId % 50000;
	char mVipVideoID[64] = { 0 };
	sprintf_s(mVipVideoID, sizeof(mVipVideoID)-1, "%d%d%d", gConfig.GetServerID(), gWork.GetCurTime().Secs(), m_videoId);
	return mVipVideoID;
}

VipLogItem*	 VipLogMgr::GetNewLogItem(Lint card,Lint usrId)
{
	VipLogItem* item = new VipLogItem();
	item->m_id = GetVipId();
	item->m_maxCircle = card;
	item->m_time = gWork.GetCurTime().Secs();
	m_item[item->m_id]= item;
	return item;
}

VipLogItem*	VipLogMgr::GetLogItem(Lstring& logId)
{
	if (m_item.count(logId))
	{
		return m_item[logId];
	}

	return NULL;
}

VipLogItem*	VipLogMgr::FindUserPlayingItem(Lint id)
{
	auto it = m_item.begin();
	for(; it != m_item.end(); ++it)
	{
		VipLogItem* item = it->second;
		if(!item->isEnd() && item->ExiestUser(id))
		{
			return item;
		}
	}
	return NULL;
}

void VipLogMgr::AddPlayingItem(VipLogItem* item)
{
	m_item[item->m_id] = item;
}

void VipLogMgr::Tick()
{
	auto it = m_item.begin();
	for (; it != m_item.end();)
	{
		if (it->second->isEnd())
		{
			LLOG_INFO("VipLogMgr::Tick vip end %s",it->first.c_str());
			delete it->second;
			m_item.erase(it++);
		}
		else
		{
			it++;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

bool RoomVip::Init()
{

	return true;
}

bool RoomVip::Final()
{
	return true;
}

void RoomVip::Tick(LTime& cur)
{
	DeskMap::iterator it = m_deskMap.begin();
	for (; it != m_deskMap.end(); ++it)
	{
		it->second->Tick(cur);
	}
}

/* Ѱ��һ�������ӣ�����ʼ������ID ����Ϸ���淨*/
Desk* RoomVip::GetFreeDesk(Lint nDeskID, QiPaiGameType gameType)
{
	Desk* ret = NULL;
	
	LLOG_DEBUG("RoomVip::m_deskMap size=%d", m_deskMap.size());

	//����һ�����ڿ��е����Ӷ���
	DeskMap::iterator it = m_deskMap.begin();
	for (; it != m_deskMap.end(); ++it)
	{
		if (it->second->getDeskState() == DESK_FREE)
		{
			ret = it->second;
			m_deskMap.erase(it);
			break;
		}
	}
	
	if(ret == NULL)
	{
		ret = new Desk();
	}

	//��ʼ�����Ӷ���
	if(!ret->initDesk(nDeskID, gameType))
	{
		delete ret;
		ret = NULL;
		LLOG_ERROR("RoomVip::GetFreeDesk -- Fail to init desk");
	}
	else
	{
		m_deskMap[nDeskID] = ret;
		ret->SetDeskWait();
		LLOG_ERROR("RoomVip::GetFreeDesk size=%d", m_deskMap.size());
	}
	
	return ret;
}

/* ��������ID����ȡ���Ӷ���*/
Desk* RoomVip::GetDeskById(Lint id)
{
	if(m_deskMap.count(id))
	{
		return m_deskMap[id];
	}

	return NULL;
}

Lint RoomVip::CreateVipDesk(LMsgLMG2LCreateDesk* pMsg, User* pUser)
{
	if (!pUser || !pMsg)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk() Error!!! pUser or pMsg is NULL Error!!!");
		return -1;
	}
	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode = 0;

	//���֮ǰ�Ѿ�������������
	if (pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::CreateVipDesk() Error!!! Create user has in desk Error!!! userId=[%d], createDeskId=[%d], userHasDeskId=[%d]", 
			pUser->GetUserDataId(), pMsg->m_deskID, pUser->m_desk->GetDeskId());

		ret.m_errorCode = 2;	//�����������
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//����ID�Ѿ���ʹ��
	if (GetDeskById(pMsg->m_deskID) != NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk() Error!!! DeskId is in useing Error!!! userId=[%d], deskId=[%d]", 
			pUser->GetUserDataId(), pMsg->m_deskID);

		ret.m_errorCode = 3;	//���Ӻ��ѱ�ʹ��
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//����һ����������
	Desk* desk = GetFreeDesk(pMsg->m_deskID, (QiPaiGameType)pMsg->m_state);
	//û���ҵ���������
	if (desk == NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk() Error!!! Create Desk Failed Error!!! userId=[%d], deskId=%d, gametype=[%d]", 
			pUser->GetUserDataId(), pMsg->m_deskID, pMsg->m_state);

		ret.m_errorCode = 4;	//û���ҵ��������ӣ���������ʧ��
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	/*
	if (pMsg->m_playType.empty())
	{
		LLOG_ERROR("RoomVip::CreateVipDesk() Error!!! PlayType is empty Error!!! userId=[%d], deskId=[%d]", 
			pUser->GetUserDataId(), pMsg->m_deskID);

		ret.m_errorCode = 5;	//��������
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	*/

	MHLOG("********************* RoomVip::CreateVipDesk(LMsgLMG2LCreateDesk* pMsg, User* pUser)");
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_gateId %d", pMsg->m_gateId);
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_flag %d", pMsg->m_flag);
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_gold %d", pMsg->m_gold);
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_strUUID %s", pMsg->m_strUUID.c_str());
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_userid %d", pMsg->m_userid);
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg->m_robotNum %d", pMsg->m_robotNum);
	MHLOG("********************* LMsgLMG2LCreateDesk pMsg-m_playType.size() = %d", pMsg->m_playType.size());;
	for (int i = 0; i < pMsg->m_playType.size(); i++)
	{
		LLOG_DEBUG("********************* LMsgLMG2LCreateDesk i = %d pMsg-m_playType[i] = %d", i, pMsg->m_playType[i]);;
	} 

	//�������Ӳ���
	Lint itemId = 0;

	//ȷ�����Ӿ���
	Lint circle = 0;
	circle = getGameDrawCount(pMsg->m_state, pMsg->m_flag);

	//ȷ�������������
	Lint userMax = desk->m_desk_user_count;
	if (getDeskUserMax(pMsg->m_state, pMsg->m_playType, &userMax))
	{
		desk->setDeskUserMax(userMax);
	}

	LLOG_ERROR("RoomVip::CreateVipDesk() Run... Create desk sucess^_^ deskId=[%d], gameType=[%d], createUserId=[%d], circleMax=[%d], userMax[%d], cost=[%d], even=[%d]", 
		pMsg->m_deskID, pMsg->m_state, pUser->GetUserDataId(), circle, userMax, pMsg->m_cost, pMsg->m_even);
		
	desk->m_creatUser = pUser;
	desk->m_creatUserId = pUser->GetUserDataId();
	desk->m_creatUserNike = pUser->m_userData.m_nike;
	desk->m_creatUserUrl = pUser->m_userData.m_headImageUrl;
	desk->m_unioid = pUser->m_userData.m_unioid;
	desk->m_cheatAgainst = pMsg->m_cheatAgainst;
	desk->m_feeType = pMsg->m_feeType;
	desk->m_cellscore = pMsg->m_cellscore;
	desk->m_state = pMsg->m_state;
	desk->m_flag = pMsg->m_flag;
	desk->m_cost = pMsg->m_cost;
	desk->m_even = pMsg->m_even;
	desk->MHSetGreater2CanStartGame(pMsg->m_Greater2CanStart, pMsg->m_playType);
	desk->m_free = pMsg->m_free;
	desk->m_startTime = pMsg->m_startTime;
	desk->m_endTime = pMsg->m_endTime;
	desk->m_Gps_Limit=pMsg->m_GpsLimit;
	desk->m_allowLookOn = pMsg->m_allowLookOn;
	desk->m_clubOwerLookOn = pMsg->m_clubOwerLookOn;

	//��������VipLog����
	VipLogItem* log = gVipLogMgr.GetNewLogItem(circle, pUser->GetUserDataId());
	desk->SetCreatType(pMsg->m_deskType);
	log->m_desk = desk;
	log->m_curCircle = 0;
	log->m_curCircleReal = 0;
	log->m_maxCircle = circle;
	log->m_state = pMsg->m_state;
	log->m_deskId = log->m_desk->GetDeskId();
	auto iPlayType = pMsg->m_playType.begin();
	for (; iPlayType != pMsg->m_playType.end(); iPlayType ++)
	{
		log->m_playtype.push_back( *iPlayType );
	}
	log->m_iPlayerCapacity = desk->GetPlayerCapacity();
	if (FEAT_STARTUP_SCORE) {
		for (int x = 0; x < DESK_USER_COUNT; ++x) log->m_score[x] = FEAT_STARTUP_SCORE;
	}
	log->m_desk->SetVip(log);
	log->AddUser(pUser);

	//������������
	desk->SetIsAllowLookOn();
	desk->SetIsStartNoInRoom();
	desk->SetIsAllowDynamicIn();
	desk->SetIsManagerStart();
	desk->SetDeskCostEven();

	//֪ͨ����������Ҵ���������
	ret.m_deskId = log->m_desk->GetDeskId();
	pUser->Send(ret);

	//�������뷿��
	log->m_desk->OnUserInRoomLookOn(pUser);

	/*
	//������������䣬��ս���������
	if (SanDaEr == log->m_desk->m_state || SanDaYi == log->m_desk->m_state || WuRenBaiFen == log->m_desk->m_state)
	{
		LLOG_ERROR("RoomVip::Creater creat look on desk, then go to OnUserInRoomLookOn()...deskId=[%d], creatUserId=[%d]",
			log->m_desk->GetDeskId(), pUser->GetUserDataId());
		//ִ�й�ս��ҽ��뷿��
		log->m_desk->OnUserInRoomLookOn(pUser);
	}
	//���Ŵ������䣬��ս���������
	else if (CheXuan == log->m_desk->m_state)
	{
		LLOG_ERROR("RoomVip::Creater creat look on desk, then go to OnUserInRoomLookOn()...deskId=[%d], creatUserId=[%d]",
			log->m_desk->GetDeskId(), pUser->GetUserDataId());
		//ִ�й�ս��ҽ��뷿��
		log->m_desk->OnUserInRoomLookOn(pUser);
	}
	//���������Ҵ�������ţţ�淨�����߹�ս���������**Ren 2018-5-18��
	else if (log->m_desk->m_state == NiuNiu || log->m_desk->m_state == NiuNiu_New)
	{
		LLOG_ERROR("RoomVip::Creater creat look on desk, then go to OnUserInRoomLookOn()...deskId=[%d], creatUserId=[%d]",
			log->m_desk->GetDeskId(), pUser->GetUserDataId());
		//ִ�й�ս��ҽ��뷿��
		log->m_desk->OnUserInRoomLookOn(pUser);
	}
	else  //û�й�ս�淨��������
	{
		log->m_desk->OnUserInRoom(pUser);
	}
	*/

	//�ж��Ƿ���Ҫ�ӵ��ԣ� ���ӵ���
	if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
	{
		/*for (Lint i = 0; i < CARD_COUNT; i ++)
		{
			log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
			log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
		}*/

//		for (Lint i = 0; i < 13; i++)
//		{
//			log->m_desk->m_player1Card[i].m_color = pMsg->m_Player0CardValue[i].m_color;
//			log->m_desk->m_player1Card[i].m_number = pMsg->m_Player0CardValue[i].m_number;
//		}
//		for (Lint i = 0; i < 13; i++)
//		{
//			log->m_desk->m_player2Card[i].m_color = pMsg->m_Player1CardValue[i].m_color;
//			log->m_desk->m_player2Card[i].m_number = pMsg->m_Player1CardValue[i].m_number;
//		}
//		for (Lint i = 0; i < 13; i++)
//		{
//			log->m_desk->m_player3Card[i].m_color = pMsg->m_Player2CardValue[i].m_color;
//			log->m_desk->m_player3Card[i].m_number = pMsg->m_Player2CardValue[i].m_number;
//		}
//		for (Lint i = 0; i < 13; i++)
//		{
//			log->m_desk->m_player4Card[i].m_color = pMsg->m_Player3CardValue[i].m_color;
//			log->m_desk->m_player4Card[i].m_number = pMsg->m_Player3CardValue[i].m_number;
//		}
//		for (Lint i = 0; i < 84; i++)
//		{
//			log->m_desk->m_SendCard[i].m_color = pMsg->m_SendCardValue[i].m_color;
//			log->m_desk->m_SendCard[i].m_number = pMsg->m_SendCardValue[i].m_number;
//		}
//		for (Lint i = 0; i < 2; i++)
//		{
//			log->m_desk->m_HaoZiCard[i].m_color = pMsg->m_HaoZiCardValue[i].m_color;
//			log->m_desk->m_HaoZiCard[i].m_number = pMsg->m_HaoZiCardValue[i].m_number;
//		}
		
		if (pMsg->m_robotNum > 0 && pMsg->m_robotNum < 4)
		{
			//gRobotManager.AddRobotToDesk(log->m_deskId, pMsg->m_robotNum);
		}

	}

	return ret.m_errorCode;
}

Lint RoomVip::CreateVipDeskForOther(LMsgLMG2LCreateDeskForOther* pMsg, User* pUser)
{
	if (!pUser || !pMsg)
	{
		return -1;
	}
	LLOG_INFO("Logwyz.................RoomVip::CreateVipDeskForOther   pMsg->m_Greater2CanStart=[%d]", pMsg->m_Greater2CanStart);

	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode = 0;

	//if(pMsg->m_playType.empty())
	//{
	//	LLOG_ERROR("RoomVip::CreateVipDesk play type is empty, userid=%d deskid=%d", pUser->GetUserDataId(), pMsg->m_deskID);
	//	ret.m_errorCode = 5;
	//	pUser->Send(ret);
	//	return ret.m_errorCode;
	//}

	//if (pMsg->m_flag < CARD_TYPE_4 || pMsg->m_flag > CARD_TYPE_16)
	//{
	//	pMsg->m_flag = CARD_TYPE_8;
	//}

	Lint itemId = 0;
	Lint circle = 0;
	circle = getGameDrawCount(pMsg->m_state, pMsg->m_flag);
//#ifndef _MH_DEBUG
//	switch (pMsg->m_flag)
//	{
//	case CARD_TYPE_4:
//		circle = 8;
//		break;
//	case CARD_TYPE_8:
//		circle = 16;
//		break;
//	case CARD_TYPE_16:
//		circle = 16;
//		break;
//	}
//#else
//	switch (pMsg->m_flag)
//	{
//	case CARD_TYPE_4:
//		circle = 1;
//		break;
//	case CARD_TYPE_8:
//		circle = 2;
//		break;
//	case CARD_TYPE_16:
//		circle = 16;
//		break;
//	}
//#endif

	if (GetDeskById(pMsg->m_deskID) != NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk deskID is in Use, userid=%d deskid=%d", pUser->GetUserDataId(), pMsg->m_deskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//��������ʧ��
	Desk* desk = GetFreeDesk(pMsg->m_deskID, (QiPaiGameType)pMsg->m_state);
	if (desk == NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDesk create Desk Failed! userid=%d deskid=%d gametype=%d", pUser->GetUserDataId(), pMsg->m_deskID, pMsg->m_state);
		ret.m_errorCode = 4;
		pUser->Send(ret);
		return ret.m_errorCode;
	}
	LLOG_DEBUG("RoomVip::CreateVipDesk userid=%d deskid=%d gametype=%d pMsg->m_deskType = %d  cost = %d even = %d", pUser->GetUserDataId(), pMsg->m_deskID, pMsg->m_state, pMsg->m_deskType, pMsg->m_cost, pMsg->m_even);

	Lint userMax = desk->m_desk_user_count;
	if (getDeskUserMax(pMsg->m_state, pMsg->m_playType, &userMax))
	{
		desk->setDeskUserMax(userMax);
	}

	desk->SetSelectSeat(true);  //����Ŀǰ����ѡ��״̬
	desk->SetCreatType(pMsg->m_deskType);
	desk->m_creatUser = pUser;
	desk->m_creatUserId = pUser->GetUserDataId();
	desk->m_creatUserNike = pUser->m_userData.m_nike;
	desk->m_creatUserUrl = pUser->m_userData.m_headImageUrl;
	desk->m_unioid = pUser->m_userData.m_unioid;
	desk->m_cheatAgainst = pMsg->m_cheatAgainst;
	desk->m_feeType = pMsg->m_feeType;
	desk->m_cellscore = pMsg->m_cellscore;
	desk->m_state = pMsg->m_state;
	desk->m_flag = pMsg->m_flag;
	desk->m_cost = pMsg->m_cost;
	desk->m_even = pMsg->m_even;
	desk->MHSetGreater2CanStartGame(pMsg->m_Greater2CanStart,pMsg->m_playType);
	desk->m_free = pMsg->m_free;

	desk->m_Gps_Limit=pMsg->m_GpsLimit;

	//////////////////////////////////////////////////////////////////////////
	VipLogItem* log = gVipLogMgr.GetNewLogItem(circle, pUser->GetUserDataId());
	log->m_desk = desk;
	log->m_curCircle = 0;
	log->m_curCircleReal = 0;
	log->m_maxCircle = circle;
	log->m_state = pMsg->m_state;
	log->m_deskId = log->m_desk->GetDeskId();
	auto iPlayType = pMsg->m_playType.begin();
	for (; iPlayType != pMsg->m_playType.end(); iPlayType++)
	{
		log->m_playtype.push_back(*iPlayType);
	}
	log->m_iPlayerCapacity = desk->GetPlayerCapacity();
	if (FEAT_STARTUP_SCORE) {
		for (int x = 0; x < DESK_USER_COUNT; ++x) log->m_score[x] = FEAT_STARTUP_SCORE;
	}

	log->m_desk->SetVip(log);

	ret.m_deskId = log->m_desk->GetDeskId();
	pUser->Send(ret);

	
	LMsgL2LMGModifyUserCreatedDesk  sendLMData;
	sendLMData.m_strUUID = pMsg->m_strUUID;
	sendLMData.m_userid = pMsg->m_userid;
	sendLMData.m_deskId = desk->GetDeskId();
	if (desk->m_feeType == 0) 
	{
		sendLMData.m_cost = desk->m_cost;
	}
	gWork.SendToLogicManager(sendLMData);
	

	//MHLOG("******************gConfig.GetDebugModel() = %d , gConfig.GetIfAddRobot()= %d", gConfig.GetDebugModel(),  gConfig.GetIfAddRobot());
	//MHLOG("******************pMsg->m_robotNum = %d , log->m_deskId= %d", pMsg->m_robotNum, log->m_deskId);
	//�ж��Ƿ���Ҫ�ӵ��ԣ� ���ӵ���
	
	if (gConfig.GetDebugModel() && gConfig.GetIfAddRobot())
	{
		//for (Lint i = 0; i < CARD_COUNT; i ++)
		//{
		//log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
		//log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
		//}

//		for (Lint i = 0; i < 13; i++)
//		{
//			log->m_desk->m_player1Card[i].m_color = pMsg->m_Player0CardValue[i].m_color;
//			log->m_desk->m_player1Card[i].m_number = pMsg->m_Player0CardValue[i].m_number;
//		}
//		for (Lint i = 0; i < 13; i++)
//		{
//			log->m_desk->m_player2Card[i].m_color = pMsg->m_Player1CardValue[i].m_color;
//			log->m_desk->m_player2Card[i].m_number = pMsg->m_Player1CardValue[i].m_number;
//		}
//		for (Lint i = 0; i < 13; i++)
//		{
//			log->m_desk->m_player3Card[i].m_color = pMsg->m_Player2CardValue[i].m_color;
//			log->m_desk->m_player3Card[i].m_number = pMsg->m_Player2CardValue[i].m_number;
//		}
//		for (Lint i = 0; i < 13; i++)
//		{
//			log->m_desk->m_player4Card[i].m_color = pMsg->m_Player3CardValue[i].m_color;
//			log->m_desk->m_player4Card[i].m_number = pMsg->m_Player3CardValue[i].m_number;
//		}
//		for (Lint i = 0; i < 84; i++)
//		{
//			log->m_desk->m_SendCard[i].m_color = pMsg->m_SendCardValue[i].m_color;
//			log->m_desk->m_SendCard[i].m_number = pMsg->m_SendCardValue[i].m_number;
//		}
//		for (Lint i = 0; i < 2; i++)
//		{
//			log->m_desk->m_HaoZiCard[i].m_color = pMsg->m_HaoZiCardValue[i].m_color;
//			log->m_desk->m_HaoZiCard[i].m_number = pMsg->m_HaoZiCardValue[i].m_number;
//		}

		if (_MH_ROBOT_PERF_TEST)
		{
			//gRobotManager.AddRobotToDesk(log->m_deskId, desk->GetPlayerCapacity());
		}
		else if (pMsg->m_robotNum > 0 && pMsg->m_robotNum < 4)
		{
			//gRobotManager.AddRobotToDesk(log->m_deskId, pMsg->m_robotNum);
		}

	}
	
	return ret.m_errorCode;
}

Lint RoomVip::CreateVipCoinDesk(LMsgCN2LCreateCoinDesk*pMsg,User* pUsers[])
{
	if(!pMsg) 
		return -1;

	Lint circle = 1;

	//��������ʧ��
	Desk* desk = GetFreeDesk(pMsg->m_deskId, (QiPaiGameType)pMsg->m_state);
	if (desk == NULL)
	{
		return -1;
	}

	desk->SetDeskType( DeskType_Coins );   //��ҳ�����
	desk->m_baseScore = pMsg->m_baseScore;
	desk->m_autoChangeOutTime = pMsg->m_changeOutTime;
	desk->m_autoPlayOutTime = pMsg->m_opOutTime;
	
	VipLogItem* log = gVipLogMgr.GetNewLogItem(circle, pUsers[0]->GetUserDataId());
	log->m_desk = desk;
	log->m_curCircle = 0;
	log->m_curCircleReal = 0;
	log->m_maxCircle = circle;
	log->m_state = pMsg->m_state;
	log->m_deskId = log->m_desk->GetDeskId();
	auto iPlayType = pMsg->m_playType.begin();
	for (; iPlayType != pMsg->m_playType.end(); iPlayType ++)
	{
		log->m_playtype.push_back( *iPlayType );
	}
	log->m_iPlayerCapacity = desk->GetPlayerCapacity();
	log->m_desk->SetVip(log);

	for(Lint i = 0; i < DESK_USER_COUNT; ++i)
	{
		log->AddUser(pUsers[i]);
	}

	// �Ƿ�����
	if (gConfig.GetDebugModel())
	{
		for (Lint i = 0; i < CARD_COUNT; i ++)
		{
			//log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
			//log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
		}
	}

	log->m_desk->OnUserInRoom(pUsers);

	return 0;
}

Lint RoomVip::AddToVipDesk(User* pUser, Lint nDeskID)
{
	if (!pUser)
	{
		return -1;
	}
	LMsgS2CAddDeskRet ret;
	ret.m_deskId = nDeskID;
	ret.m_errorCode = 0;

	if(pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk has desk, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Desk* desk = GetDeskById(nDeskID);

	if (!desk )
	{
		LLOG_ERROR("RoomVip::AddToVipDesk not find desk, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	// ȥ���������ƣ�����Ҳ�������
	//if (!desk->GetVip() || desk->GetVip()->IsBegin()) 
	if (!desk->GetVip())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk already begin, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (desk->m_feeType == 1 && pUser->m_userData.m_numOfCard2s < desk->m_even)  //������÷�̯ʱ ��Ҳ���
	{
		LLOG_ERROR("RoomVip::AddToVipDesk coin is not enough, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 4;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//ȥ�����ӵȴ�״̬���κ�ʱ�����û�����
	//if (desk->GetVip()->IsFull(pUser) || desk->GetUserCount() == desk->GetPlayerCapacity() || desk->getDeskState() != DESK_WAIT)
	if (desk->GetVip()->IsFull(pUser) || desk->GetUserCount() == desk->GetPlayerCapacity())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk full, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//����û���VIP��
	if ( !desk->GetVip()->AddUser(pUser) )
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk adduser fail, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	LLOG_INFO("RoomVip::AddToVipDesk userid=%d deskid=%d deskusercount=%d", pUser->GetUserDataId(), nDeskID, desk->GetUserCount());
	desk->OnUserInRoom(pUser);

	pUser->Send(ret);
	return ret.m_errorCode;
}

//��Ҽ��뷿��
Lint RoomVip::AddToVipDesk(User* pUser, LMsgLMG2LAddToDesk* msg)
{
	if (!pUser || !msg)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk() Error!!! pUser or pMsg is NULL Error!!!");
		return -1;
	}

	LMsgS2CAddDeskRet ret;
	ret.m_deskId = msg->m_deskID;
	ret.m_errorCode = 0;

	if (pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::CreateVipDesk() Error!!! Add desk user has in desk Error!!! userId=[%d], addDeskId=[%d], userHasDeskId=[%d]",
			pUser->GetUserDataId(), msg->m_deskID, pUser->m_desk->GetDeskId());

		ret.m_errorCode = 3;	//���������������
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Desk* desk = GetDeskById(msg->m_deskID);
	if (!desk)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk() Error!!! Not find desk Error!!! deskId=[%d] userId=[%d]", 
			msg->m_deskID, pUser->GetUserDataId());

		ret.m_errorCode = 2;	//���䲻����
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (!desk->GetVip())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk() Error!!! Not find desk vip Error!!! deskId=[%d], userId=[%d]", 
			msg->m_deskID, pUser->GetUserDataId());

		ret.m_errorCode = 2;	//����û��VipLog����
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Lint nFree = 0;
	LTime cur;
	Lint curTime = cur.Secs();
	if (curTime > msg->m_startTime && curTime < msg->m_endTime)
	{
		nFree = 1;
	}

	if (desk->m_state != 100010 && desk->m_state != 100008 && desk->m_state != 100009) 
	{
		nFree = 0;
	}

	if (desk->m_feeType == 1 && pUser->m_userData.m_numOfCard2s - msg->m_creatDeskCost < desk->m_even && nFree == 0)  //������÷�̯ʱ ��Ҳ���
	{
		LLOG_ERROR("RoomVip::AddToVipDesk coin is not enough, userid=%d deskid=%d free = %d", pUser->GetUserDataId(), msg->m_deskID, msg->m_Free);
		ret.m_errorCode = 4;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	desk->m_startTime = msg->m_startTime;
	desk->m_endTime = msg->m_endTime;

	//GPS��Ϣ���  ����GPS��� && �������ս && ������᳤���ƹ�ս
	if (desk->m_Gps_Limit == 1 && !desk->m_allowLookOn)
	{
		if (pUser->m_gps_lng==0.0f||pUser->m_gps_lat==0.0f) //GPS������Ч
		{
			LLOG_ERROR("RoomVip::AddToVipDesk GPS Error, userid:%d, gps:%s- [%f:%f]", 
				pUser->GetUserDataId(), pUser->GetUserData().m_customString2.c_str(), pUser->m_gps_lng, pUser->m_gps_lat);

			ret.m_errorCode = LMsgS2CAddDeskRet::Err_GPS_INVALID;		//9�����GPS������Ч
			pUser->Send(ret);
			return ret.m_errorCode;
		}

		// GPS������
		Lstring user_gps_list;
		if (!desk->MHCheckUserGPSLimit(pUser, user_gps_list))
		{
			LLOG_ERROR("RoomVip::AddToVipDesk user GPS is forbitten, userid=%d deskid=%d userlist=%s",
				pUser->GetUserDataId(), desk->GetDeskId(), user_gps_list.c_str());

			ret.m_errorCode = LMsgS2CAddDeskRet::Err_GPS_Limit;		//10�����GPS�������
			ret.m_userGPSList = user_gps_list;
			pUser->Send(ret);
			return ret.m_errorCode;
		}
	}

	//��Ϸ��ʼ && ��ʼ���ֹ���뷿��
	if (desk->MHIsDeskMatchBegin() && desk->m_isStartNoInRoom)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk() Error!!! Game has start, not start in room Error!!!, deskId=[%d], userId=[%d]",
			desk->GetDeskId(), pUser->GetUserDataId());

		ret.m_errorCode = 3;		//��Ϸ�Ѿ���ʼ����ֹ���뷿��
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	//���뷿��
	if (desk->m_allowLookOn)
	{
		desk->OnUserInRoomLookOn(pUser);
		return ret.m_errorCode;
	}

	/*
	//ţţ��ս���뷿���һ��  **Ren 2018-05-15
	if ((NiuNiu == desk->m_state || NiuNiu_New == desk->m_state) && desk && desk->GetVip() && 1 == msg->m_addDeskFlag)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk() Player First Into Room, So Is Look On Player... deskId=[%d], userId=[%d]",
			desk->GetDeskId(), pUser->GetUserDataId());

		//����Ҽ����������
		desk->OnUserInRoomLookOn(pUser);
		return ret.m_errorCode;
	}

	//��������뷿���һ��
	else if ((SanDaEr == desk->m_state || SanDaYi == desk->m_state || WuRenBaiFen == desk->m_state) && 
		desk && desk->GetVip() && 1 == msg->m_addDeskFlag && 
		(desk->m_allowLookOn || !desk->MHIsDeskMatchBegin() || (desk->m_clubOwerLookOn && pUser->GetUserDataId() == desk->m_creatUserId)))
	{
		LLOG_ERROR("RoomVip::AddToVipDesk() Player First Into Room, So Is Look On Player... deskId=[%d], userId=[%d]",
			desk->GetDeskId(), pUser->GetUserDataId());

		//����Ҽ����������
		desk->OnUserInRoomLookOn(pUser);
		return ret.m_errorCode;
	}

	//���ż��뷿���һ��
	else if (CheXuan == desk->m_state && desk && desk->GetVip() && 1 == msg->m_addDeskFlag &&
		(desk->m_allowLookOn || !desk->MHIsDeskMatchBegin() || (desk->m_clubOwerLookOn && pUser->GetUserDataId() == desk->m_creatUserId)))
	{
		LLOG_ERROR("RoomVip::AddToVipDesk() Player First Into Room, So Is Look On Player... deskId=[%d], userId=[%d]",
			desk->GetDeskId(), pUser->GetUserDataId());

		//����Ҽ����������
		desk->OnUserInRoomLookOn(pUser);
		return ret.m_errorCode;
	}
	*/

	//֧�ֿ����Ժ��û������
	if (desk && desk->m_isAllowDynamicIn)
	{
		if (!desk->GetVip() || desk->GetVip()->IsFull(pUser)||desk->GetUserCount()==desk->GetPlayerCapacity())
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk full, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);

			ret.m_errorCode=1;
			pUser->Send(ret);
			return ret.m_errorCode;
		}

		//��ɢ�����У���ֹ��ҽ���
		if (desk && desk->m_resetTime > 0 && desk->m_resetUserId > 0)
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk is in dissmiss, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);
			ret.m_errorCode = 11; // �������11
			pUser->Send(ret);
			return ret.m_errorCode;
		}


		if (desk && desk->GetVip() && msg->m_addDeskFlag == 1 && (desk->GetVip()->IsBegin() || (!desk->GetVip()->IsBegin() && desk->m_deskState == DESK_PLAY)))
		{
			LLOG_DEBUG("LMsgS2CJoinRoomCheck");
			LMsgS2CJoinRoomCheck send;
			send.m_errorCode = 0;
			desk->fillJoinRoomCheck(send, pUser->GetUserDataId());
			pUser->Send(send);
			return -1;
		}

		if (!desk->GetVip()->AddUser(pUser))
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk adduser fail, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);
			ret.m_errorCode=1;
			pUser->Send(ret);

			//������Ҷ�̬����������Ϸ
			if (NiuNiu == desk->m_state || NiuNiu_New == desk->m_state || CheXuan == desk->m_state)
			{
				LMsgS2CLookOnPlayerFill fill;
				fill.m_errorCode = 1;
				fill.m_state = desk->m_state;
				for (auto t_listIter = desk->m_desk_Lookon_user.begin(); t_listIter != desk->m_desk_Lookon_user.end(); t_listIter++)
				{
					if ((*t_listIter) != NULL)
						(*t_listIter)->Send(fill);
				}
			}

			return ret.m_errorCode;
		}
	}
	else
	{
		if (!desk->GetVip()||desk->GetVip()->IsBegin() || desk->m_deskState==DESK_PLAY)
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk already begin, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);
			ret.m_errorCode=3;
			pUser->Send(ret);
			return ret.m_errorCode;
		}

		if (desk->GetVip()->IsFull(pUser) || desk->GetUserCount() == desk->GetPlayerCapacity() || desk->getDeskState() != DESK_WAIT)
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk full, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);

			ret.m_errorCode = 1;
			pUser->Send(ret);
			return ret.m_errorCode;
		}

		if (!desk->GetVip()->AddUser(pUser))
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk adduser fail, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);
			ret.m_errorCode = 1;
			pUser->Send(ret);

			//������Ҷ�̬����������Ϸ
			if (NiuNiu == desk->m_state || NiuNiu_New == desk->m_state || CheXuan == desk->m_state)
			{
				LMsgS2CLookOnPlayerFill fill;
				fill.m_errorCode = 1;
				fill.m_state = desk->m_state;
				for (auto t_listIter = desk->m_desk_Lookon_user.begin(); t_listIter != desk->m_desk_Lookon_user.end(); t_listIter++)
				{
					if ((*t_listIter) != NULL)
						(*t_listIter)->Send(fill);
				}
			}

			return ret.m_errorCode;
		}
	}

	LLOG_INFO("RoomVip::AddToVipDesk userid=%d deskid=%d deskusercount=%d", pUser->GetUserDataId(), msg->m_deskID, desk->GetUserCount());
	desk->OnUserInRoom(pUser);

	pUser->Send(ret);
	return ret.m_errorCode;

}


Lint RoomVip::SelectSeatInVipDesk(User* pUser, Lint nDeskID)
{
	if (!pUser)
	{
		return -1;
	}
	LMsgS2CAddDeskRet ret;
	ret.m_deskId = nDeskID;
	ret.m_errorCode = 0;

	if (pUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk has desk, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	Desk* desk = GetDeskById(nDeskID);

	if (!desk)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk not find desk, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 2;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (!desk->GetVip() || desk->GetVip()->IsBegin())
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk already begin, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 3;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (desk->GetVip()->IsFull(pUser) || desk->GetUserCount() == desk->GetPlayerCapacity() || desk->getDeskState() != DESK_WAIT)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk full, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	if (!desk->GetVip()->AddUser(pUser))
	{
		LLOG_ERROR("RoomVip::AddToVipDesk desk adduser fail, userid=%d deskid=%d", pUser->GetUserDataId(), nDeskID);
		ret.m_errorCode = 1;
		pUser->Send(ret);
		return ret.m_errorCode;
	}

	LLOG_INFO("RoomVip::AddToVipDesk userid=%d deskid=%d deskusercount=%d", pUser->GetUserDataId(), nDeskID, desk->GetUserCount());
	desk->OnUserInRoom(pUser);

	pUser->Send(ret);
	return ret.m_errorCode;
}


bool RoomVip::LeaveToVipDesk(LMsgC2SLeaveDesk* pMsg, User* pUser)
{
	if (!pMsg || !pUser)
	{
		LLOG_ERROR("RoomVip::LeaveToVipDesk() Error... The pMsg or pUser is NULL...");
		return false;
	}

	Desk* desk = pUser->GetDesk();
	if (desk == NULL)
	{
		LLOG_ERROR("RoomVip::LeaveToVipDesk() Error... Get user desk failure... userId=[%d]", pUser->GetUserDataId());
		return false;
	}

	LLOG_INFO("RoomVip::LeaveToVipDesk() Run... deskId=[%d], userId=[%d]",
		desk->GetDeskId(), pUser->GetUserDataId());

	//61017����������뿪������
	LMsgS2CLeaveDesk send;
	send.m_errorCode = 0;

	//����ǹ�������뿪���䣬���߻᳤���� ������
	if (desk->MHIsLookonUser(pUser) || pUser == desk->m_clubOwerLookOnUser)
	{
		LLOG_ERROR("RoomVip::LeaveToVipDesk() Look on user leave out desk... deskId=[%d], userId=[%d]",
			desk->GetDeskId(), pUser->GetUserDataId());

		//�ù�ս����Ƿ���
		if (desk->MHGetClubId() == 0 )
		{
			if (pUser->GetUserDataId() == desk->m_creatUserId)
			{
				gWork.HanderUserLogout(pUser->getUserGateID(), pUser->m_userData.m_unioid);
				//desk->MHEraseDeskSeatListUserByPuser(pUser);

			}
			else
			{
				desk->OnUserOutRoom(pUser, 2);
			}
		}
		//��ͨ��ս���
		else
		{
			//ɾ�����
			desk->OnUserOutRoom(pUser, 2);
		}

		//LMsgS2CLeaveDesk lookLeave;
		send.m_errorCode = 0;
		pUser->Send(send);

		return true;
	}

	if(desk == NULL || desk->getDeskState() != DESK_WAIT)
	{
		LLOG_ERROR("RoomVip::LeaveToVipDesk() fail... This desk status is not wait... deskId=[%d], userId=[%d] deskState=[%d]",
			desk ? desk->GetDeskId() : 0, pUser->GetUserDataId(), desk ? desk->getDeskState() : -1);

		send.m_errorCode = 1;

		//61017:��������뿪������
		pUser->Send(send);
		return false;
	}

	if(!desk->GetVip() || desk->GetVip()->IsBegin())
	{
		LLOG_ERROR("RoomVip::LeaveToVipDesk() fail... This desk status is not wait... deskId=[%d], userId=[%d] deskState=[%d]",
			desk ? desk->GetDeskId() : 0, pUser->GetUserDataId(), desk ? desk->getDeskState() : -1);
		send.m_errorCode = 1;
		pUser->Send(send);
		return false;
	}

	LLOG_DEBUG("RoomVip::LeaveToVipDesk() This user leave out desk secuss... deskId=[%d], leaveUserId=[%d]", desk->GetDeskId(), pUser->GetUserDataId());

	//61017:��������뿪������
	pUser->Send(send);


	//����
	if(desk->GetVip()->GetOwerId() ==  pUser->GetUserDataId() && desk->GetCreatType() == 0)
	{
		LLOG_INFO("RoomVip::LeaveToVipDesk() This Desk Ower Leave Out Desk, But It Desk Info Will Be Keeped... deskId=[%d], leaveUserId=[%d]",
			desk->GetDeskId(), pUser->GetUserDataId());

		//�ѿ�ʼ��ťλ�ÿ�,������������ˣ������ڵ��߼�������ѡ�����˳����ٻص����䣬������ԭ������λ��������ѡ�����˳����ٽ�����Ҫ����ѡ����
		//desk->MHSetStartButtonPos(INVAILD_POS_QIPAI);
		//���䱣���������κβ���
		gWork.HanderUserLogout(pUser->getUserGateID() , pUser->m_userData.m_unioid);
	}
	//�Ǵ����������뿪���䣬�򽫸���Ҵӷ����������/
	else
	{
		LLOG_INFO("RoomVip::LeaveToVipDesk() Common User Leave Out Desk, It Desk Info Will Be Delete... deskId=[%d], leaveUserId=[%d]",
			desk->GetDeskId(), pUser->GetUserDataId());

		desk->GetVip()->RemoveUser(pUser->GetUserDataId());
		desk->OnUserOutRoom(pUser, 2);

		//��������״̬����������뿪���䣬��֪ͨ����lookon��Ҳ���ʾ������ť��**Ren 2018-5-20��
		if (desk->GetUserCount() < desk->GetPlayerCapacity() && desk->GetUserCount() == desk->GetPlayerCapacity() - 1)
		{
			LLOG_DEBUG("RoomVip::LeaveToVipDesk() User Leave Full Desk, Then Look Can Seat Down...  deskId=[%d], leaveUserId=[%d]",
				desk->GetDeskId(), pUser->GetUserDataId());

			//62065��֪ͨ��ս��ң�������δ�������Լ���
			LMsgS2CLookOnPlayerFill notFull;
			notFull.m_errorCode = 0;  //0������δ����֪ͨ������ս���
			notFull.m_state = desk->m_state;

			//62065���㲥�����й�ս���
			desk->MHBoadCastDeskLookonUser(notFull);
		}
	}
	return true;
}

void RoomVip::MHPrintAllDeskStatus()
{
	Lint free = 0;
	Lint wait = 0;
	Lint play = 0;
	DeskMap::iterator it = m_deskMap.begin();	
	for (; it != m_deskMap.end(); it++)
	{
		if (it->second)
		{
			it->second->MHPrintDeskStatus();
			Lint state = it->second->getDeskState();
			switch (state)
			{
			case DESK_FREE:
				free += 1;
				break;
			case DESK_WAIT:
				wait += 1;
				break;
			case DESK_PLAY:
				play += 1;
				break;
			default:
				break;
			}
		}
	}
	MHLOG_DESK("***Total desk number:%d wait:%d, free:%d, play:%d", m_deskMap.size(), wait, free, play);
}

//���ֲ�  �����ʱû���ã�����������ӵ�
Lint RoomVip::CreateVipDeskForClub(LMsgLMG2LCreateDeskForClub* pMsg)
{
	LLOG_DEBUG("RoomVip::CreateVipDeskForClub");
	if (!pMsg)
	{
		return -1;
	}
	LLOG_DEBUG("RoomVip::CreateVipDeskForClub,   creatorId = [%d],m_deskCount=[%d]", pMsg->m_userid, pMsg->m_deskCount);

	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode=0;

	Lint itemId=0;
	Lint circle=0;
	circle = getGameDrawCount(pMsg->m_state, pMsg->m_flag);

	for (int i=0; i<pMsg->m_deskCount; i++)
	{
		if (i>pMsg->m_deskIds.size())break;

		Lint m_deskID=pMsg->m_deskIds[i];
		LLOG_DEBUG("RoomVip::CreateVipDeskForClub  [%d]=[%d]", i, m_deskID);
		if (GetDeskById(m_deskID)!=NULL)
		{
			LLOG_ERROR("RoomVip::CreateVipDeskForClub deskID is in Use, userid=%d deskid=%d", pMsg->m_userid, m_deskID);
			ret.m_errorCode=3;
			//pUser->Send(ret);
			break;
		}

		//��������ʧ��
		Desk* desk=GetFreeDesk(m_deskID, (QiPaiGameType)pMsg->m_state);
		if (desk==NULL)
		{
			LLOG_ERROR("RoomVip::CreateVipDeskForClub create Desk Failed! userid=%d deskid=%d gametype=%d", pMsg->m_userid, m_deskID, pMsg->m_state);
			ret.m_errorCode=4;
			//pUser->Send(ret);
			return ret.m_errorCode;
		}
		LLOG_DEBUG("RoomVip::CreateVipDeskForClub userid=%d deskid=%d gametype=%d pMsg->m_deskType = %d  cost = %d even = %d", pMsg->m_userid, m_deskID, pMsg->m_state, pMsg->m_deskType, pMsg->m_cost, pMsg->m_even);
		desk->SetSelectSeat(true);  //����Ŀǰ����ѡ��״̬
		desk->SetCreatType(pMsg->m_deskType);
		desk->m_creatUser=NULL;
		desk->m_creatUserId=pMsg->m_userid;
		//desk->m_creatUserNike=pUser->m_userData.m_nike;
		//desk->m_creatUserUrl=pUser->m_userData.m_headImageUrl;
		//desk->m_unioid=pUser->m_userData.m_unioid;
		desk->m_cheatAgainst=pMsg->m_cheatAgainst;
		desk->m_feeType=pMsg->m_feeType;
		desk->m_cellscore=pMsg->m_cellscore;
		desk->m_state=pMsg->m_state;
		desk->m_flag=pMsg->m_flag;
		desk->m_cost=pMsg->m_cost;
		desk->m_even=pMsg->m_even;
		desk->MHSetGreater2CanStartGame(pMsg->m_Greater2CanStart,pMsg->m_playType);
		desk->m_free=pMsg->m_free;
		//desk->MHSetClubInfo(pMsg->m_clubId, pMsg->m_playTypeId);

		//////////////////////////////////////////////////////////////////////////
		VipLogItem* log=gVipLogMgr.GetNewLogItem(circle, pMsg->m_userid);
		log->m_desk=desk;
		log->m_curCircle=0;
		log->m_curCircleReal=0;
		log->m_maxCircle=circle;
		log->m_state=pMsg->m_state;
		log->m_deskId=log->m_desk->GetDeskId();
		auto iPlayType=pMsg->m_playType.begin();
		for (; iPlayType!=pMsg->m_playType.end(); iPlayType++)
		{
			log->m_playtype.push_back(*iPlayType);
		}
		log->m_iPlayerCapacity=desk->GetPlayerCapacity();
		if (FEAT_STARTUP_SCORE) {
			for (int x=0; x<DESK_USER_COUNT; ++x) log->m_score[x]=FEAT_STARTUP_SCORE;
		}

		log->m_desk->SetVip(log);
	}

	//ret.m_deskId=log->m_desk->GetDeskId();
	//pUser->Send(ret);

	//�����Ҫ���� ����������
	//LMsgL2LMGModifyUserCreatedDesk  sendLMData;
	//sendLMData.m_strUUID=pMsg->m_strUUID;
	//sendLMData.m_userid=pMsg->m_userid;
	//sendLMData.m_deskId=desk->GetDeskId();
	//if (desk->m_feeType==0)
	//{
	//	sendLMData.m_cost=desk->m_cost;
	//}
	//gWork.SendToLogicManager(sendLMData);


	//MHLOG("******************gConfig.GetDebugModel() = %d , gConfig.GetIfAddRobot()= %d", gConfig.GetDebugModel(),  gConfig.GetIfAddRobot());
	//MHLOG("******************pMsg->m_robotNum = %d , log->m_deskId= %d", pMsg->m_robotNum, log->m_deskId);
	//�ж��Ƿ���Ҫ�ӵ��ԣ� ���ӵ���

	//if (gConfig.GetDebugModel()&&gConfig.GetIfAddRobot())
	//{
	//	//for (Lint i = 0; i < CARD_COUNT; i ++)
	//	//{
	//	//log->m_desk->m_specialCard[i].m_color = pMsg->m_cardValue[i].m_color;
	//	//log->m_desk->m_specialCard[i].m_number = pMsg->m_cardValue[i].m_number;
	//	//}
	//
	//	for (Lint i=0; i < 13; i++)
	//	{
	//		log->m_desk->m_player1Card[i].m_color=pMsg->m_Player0CardValue[i].m_color;
	//		log->m_desk->m_player1Card[i].m_number=pMsg->m_Player0CardValue[i].m_number;
	//	}
	//	for (Lint i=0; i < 13; i++)
	//	{
	//		log->m_desk->m_player2Card[i].m_color=pMsg->m_Player1CardValue[i].m_color;
	//		log->m_desk->m_player2Card[i].m_number=pMsg->m_Player1CardValue[i].m_number;
	//	}
	//	for (Lint i=0; i < 13; i++)
	//	{
	//		log->m_desk->m_player3Card[i].m_color=pMsg->m_Player2CardValue[i].m_color;
	//		log->m_desk->m_player3Card[i].m_number=pMsg->m_Player2CardValue[i].m_number;
	//	}
	//	for (Lint i=0; i < 13; i++)
	//	{
	//		log->m_desk->m_player4Card[i].m_color=pMsg->m_Player3CardValue[i].m_color;
	//		log->m_desk->m_player4Card[i].m_number=pMsg->m_Player3CardValue[i].m_number;
	//	}
	//	for (Lint i=0; i < 84; i++)
	//	{
	//		log->m_desk->m_SendCard[i].m_color=pMsg->m_SendCardValue[i].m_color;
	//		log->m_desk->m_SendCard[i].m_number=pMsg->m_SendCardValue[i].m_number;
	//	}
	//	for (Lint i=0; i < 2; i++)
	//	{
	//		log->m_desk->m_HaoZiCard[i].m_color=pMsg->m_HaoZiCardValue[i].m_color;
	//		log->m_desk->m_HaoZiCard[i].m_number=pMsg->m_HaoZiCardValue[i].m_number;
	//	}
	//
	//	if (_MH_ROBOT_PERF_TEST)
	//	{
	//		gRobotManager.AddRobotToDesk(log->m_deskId, desk->GetPlayerCapacity());
	//	}
	//	else if (pMsg->m_robotNum > 0&&pMsg->m_robotNum < 4)
	//	{
	//		gRobotManager.AddRobotToDesk(log->m_deskId, pMsg->m_robotNum);
	//	}
	//
	//}

	return ret.m_errorCode;
}

//���ֲ���������
Lint RoomVip::CreateVipDeskForClub(LMsgLMG2LCreateClubDeskAndEnter* pMsg, User* pJoinUser)
{
	if (!pMsg || pJoinUser == NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDeskForClub() Error!!! pUser or pMsg is NULL Error!!!");
		return -1;
	}

	LMsgS2CCreateDeskRet ret;
	ret.m_errorCode=0;

	Lint itemId=0;
	Lint circle=0;
	circle = getGameDrawCount(pMsg->m_state, pMsg->m_flag);

	ret.m_deskId=pMsg->m_clubDeskId;

	Lint m_deskID = pMsg->m_deskId;
	if (GetDeskById(m_deskID) != NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDeskForClub deskID is in Use, userid=%d deskid=%d", pMsg->m_userid, m_deskID);
		ret.m_errorCode = 3;
		//���ﲻ���������ݷ���ֵ�ϲ㺯������
		//pJoinUser->Send(ret);
		return ret.m_errorCode;
	}

	//��������ʧ��
	Desk* desk = GetFreeDesk(m_deskID, (QiPaiGameType)pMsg->m_state);
	if (desk == NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDeskForClub create Desk Failed! userid=%d deskid=%d gametype=%d", pMsg->m_userid, m_deskID, pMsg->m_state);
		ret.m_errorCode = 4;
		pJoinUser->Send(ret);
		return ret.m_errorCode;
	}
	
	Lint userMax = desk->m_desk_user_count;
	if (getDeskUserMax(pMsg->m_state, pMsg->m_playType, &userMax))
	{
		desk->setDeskUserMax(userMax);
	}

	LLOG_ERROR("RoomVip::CreateVipDeskForClub() Run... Create desk sucess^_^ deskId=[%d], gameType=[%d], createUserId=[%d], circleMax=[%d], userMax[%d], cost=[%d], even=[%d]",
		pMsg->m_deskId, pJoinUser->GetUserDataId(), pMsg->m_state, circle, userMax, pMsg->m_cost, pMsg->m_even);

	desk->SetSelectSeat(true);  //����Ŀǰ����ѡ��״̬
	desk->SetCreatType(pMsg->m_deskType);
	desk->m_creatUser=NULL;
	desk->m_creatUserId=pMsg->m_userid;
	desk->m_creatUserNike=pMsg->m_usert.m_nike;
	desk->m_creatUserUrl=pMsg->m_usert.m_headImageUrl;
	desk->m_unioid=pMsg->m_usert.m_unioid;
	desk->m_cheatAgainst=pMsg->m_cheatAgainst;
	desk->m_feeType=pMsg->m_feeType;
	desk->m_cellscore=pMsg->m_cellscore;
	desk->m_state=pMsg->m_state;
	desk->m_flag=pMsg->m_flag;
	desk->m_cost=pMsg->m_cost;
	desk->m_even=pMsg->m_even;
	desk->MHSetGreater2CanStartGame(pMsg->m_Greater2CanStart,pMsg->m_playType);
	desk->m_free=pMsg->m_free;
	desk->m_allowLookOn = pMsg->m_allowLookOn;
	desk->m_clubOwerLookOn = pMsg->m_clubOwerLookOn;
	desk->m_isCoinsGame = pMsg->m_clubType;
	desk->m_inDeskMinCoins = pMsg->m_minPointInRoom;
	desk->m_qiangZhuangMinCoins = pMsg->m_minPointXiaZhu;
	desk->m_isAllowCoinsNegative = pMsg->m_smallZero;
	desk->m_warnScore = pMsg->m_warnValue;
	desk->MHSetClubInfo(pMsg->m_clubId, pMsg->m_playTypeId, pMsg->m_clubDeskId, pMsg->m_showDeskId);
	//������ʱ��֧�־��ֲ���GPS������
	desk->m_Gps_Limit=pMsg->m_GpsLimit;

	desk->m_clubInfo.m_managerIdList.clear();
	auto iManager = pMsg->m_managerList.begin();
	for (; iManager != pMsg->m_managerList.end(); iManager++)
	{
		desk->m_clubInfo.m_managerIdList.push_back(*iManager);
	}
	desk->m_clubInfo.m_clubName = pMsg->m_clubName;

	//����VipLog����
	VipLogItem* log=gVipLogMgr.GetNewLogItem(circle, pMsg->m_userid);
	log->m_desk=desk;
	log->m_curCircle=0;
	log->m_curCircleReal=0;
	log->m_maxCircle=circle;
	log->m_state=pMsg->m_state;
	log->m_deskId=log->m_desk->GetDeskId();
	auto iPlayType=pMsg->m_playType.begin();
	for (; iPlayType!=pMsg->m_playType.end(); iPlayType++)
	{
		log->m_playtype.push_back(*iPlayType);
	}
	log->m_iPlayerCapacity=desk->GetPlayerCapacity();
	if (FEAT_STARTUP_SCORE) {
		for (int x=0; x<DESK_USER_COUNT; ++x) log->m_score[x]=FEAT_STARTUP_SCORE;
	}
	log->m_desk->SetVip(log);
	log->AddUser(pJoinUser);

	//���������ţ��޸�Сѡ��Ͷע����
	if (desk->m_clubInfo.m_clubId != 0 && desk->m_isCoinsGame == 1)
	{
		if (desk->m_state == CheXuan && log->m_playtype.size() >= 3 && desk->m_inDeskMinCoins >= 500)
		{
			log->m_playtype[2] = desk->m_inDeskMinCoins;
		}
	}

	//������������
	desk->SetIsAllowLookOn();
	desk->SetIsStartNoInRoom();
	desk->SetIsAllowDynamicIn();
	desk->SetIsManagerStart();
	desk->SetDeskCostEven();

	pJoinUser->Login();
	pJoinUser->setUserState(LGU_STATE_DESK);
	pJoinUser->ModifyUserState();

	//���뷿��
	log->m_desk->OnUserInRoomLookOn(pJoinUser);

	/*
	//���������Ҵ�������ţţ�淨�����߹�ս���������**Ren 2018-5-18��
	if (log->m_desk->m_state == NiuNiu || log->m_desk->m_state == NiuNiu_New)
	{
		LLOG_ERROR("RoomVip::Creater creat look on desk, then go to OnUserInRoomLookOn()...deskId=[%d], creatUserId=[%d]",
			log->m_desk->GetDeskId(), pJoinUser->GetUserDataId());
		//ִ�й�ս��ҽ��뷿��
		log->m_desk->OnUserInRoomLookOn(pJoinUser);
	}
	//��������뷿���һ��   ,������ֲ�ƥ�䴴���������߹�ս����
	else if ((SanDaEr == desk->m_state || SanDaYi == desk->m_state || WuRenBaiFen == desk->m_state) && desk->GetVip() && pMsg->m_type != 3)
	{
		LLOG_ERROR("RoomVip::Creater creat look on desk, then go to OnUserInRoomLookOn()...deskId=[%d], creatUserId=[%d]",
			log->m_desk->GetDeskId(), pJoinUser->GetUserDataId());

		//ִ�й�ս��ҽ��뷿��
		log->m_desk->OnUserInRoomLookOn(pJoinUser);
	}
	//���ż��뷿���һ��   ,������ֲ�ƥ�䴴���������߹�ս����
	else if (CheXuan == desk->m_state && desk->GetVip() && pMsg->m_type != 3)
	{
		LLOG_ERROR("RoomVip::Creater creat look on desk, then go to OnUserInRoomLookOn()...deskId=[%d], creatUserId=[%d]",
			log->m_desk->GetDeskId(), pJoinUser->GetUserDataId());

		//ִ�й�ս��ҽ��뷿��
		log->m_desk->OnUserInRoomLookOn(pJoinUser);
	}
	else  //û�й�ս�淨��������
	{
		log->m_desk->OnUserInRoom(pJoinUser);
	}
	*/
	return ret.m_errorCode;
}

Lint RoomVip::AddToVipClubDesk(LMsgLMG2LEnterClubDesk* pMsg, User* pJoinUser)
{
	if (!pMsg||pJoinUser==NULL)
	{
		LLOG_ERROR("RoomVip::CreateVipDeskForClub() Error!!! pUser or pMsg is NULL Error!!!");
		return -1;
	}
	LMsgS2CAddDeskRet ret;
	ret.m_deskId = pMsg->m_deskId;
	ret.m_errorCode = 0;

	if (pJoinUser->GetDesk())
	{
		LLOG_ERROR("RoomVip::CreateVipDesk() Error!!! Add desk user has in desk Error!!! userId=[%d], addDeskId=[%d], userHasDeskId=[%d]",
			pJoinUser->GetUserDataId(), pMsg->m_deskId, pJoinUser->m_desk->GetDeskId());

		//pJoinUser->GetDesk()->OnUserReconnect(pJoinUser);
		//return ret.m_errorCode;

		//�����������п��ܣ���������
		//LLOG_ERROR("RoomVip::AddToVipClubDesk has desk, userid=%d deskid=%d", pUser->GetUserDataId(), msg->m_deskID);
		//ret.m_errorCode=3;
		//pUser->Send(ret);
		//return ret.m_errorCode;
	}

	Desk* desk = GetDeskById(pMsg->m_deskId);

	if (!desk)
	{
		LLOG_ERROR("RoomVip::AddToVipClubDesk not find desk, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);
		ret.m_errorCode=2;
		pJoinUser->Send(ret);
		return ret.m_errorCode;
	}

	if (desk && ((desk->GetVip() == NULL) || (desk->m_deskState == DESK_FREE)))
	{
		LLOG_ERROR("RoomVip::AddToVipClubDesk not find desk, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);
		ret.m_errorCode = 2;
		pJoinUser->Send(ret);
		return ret.m_errorCode;
	}

	//GPS��Ϣ���
	if (desk->m_Gps_Limit == 1 && !desk->MHIsAllowLookOnMode(desk->m_state))
	{
		LLOG_DEBUG("RoomVip::AddToVipClubDesk GPS, userid:%d gps:%s value:[%f:%f]", pJoinUser->GetUserDataId(), pJoinUser->GetUserData().m_customString2.c_str(), pJoinUser->m_gps_lng, pJoinUser->m_gps_lat);
		if (pJoinUser->m_gps_lng == 0.0f || pJoinUser->m_gps_lat == 0.0f) //GPS������Ч
		{
			LLOG_ERROR("RoomVip::AddToVipClubDesk GPS Error, userid:%d, gps:%s- [%f:%f]", pJoinUser->GetUserDataId(), pJoinUser->GetUserData().m_customString2.c_str(), pJoinUser->m_gps_lng, pJoinUser->m_gps_lat);
			ret.m_errorCode = LMsgS2CAddDeskRet::Err_GPS_INVALID;
			pJoinUser->Send(ret);
			return ret.m_errorCode;
		}

		// GPS������
		Lstring user_gps_list;
		if (!desk->MHCheckUserGPSLimit(pJoinUser, user_gps_list))
		{
			ret.m_errorCode = LMsgS2CAddDeskRet::Err_GPS_Limit;
			ret.m_userGPSList = user_gps_list;
			pJoinUser->Send(ret);
			LLOG_ERROR("RoomVip::AddToVipDesk user GPS is forbitten, userid=%d deskid=%d userlist=%s", pJoinUser->GetUserDataId(), desk->GetDeskId(), ret.m_userGPSList.c_str());
			return ret.m_errorCode;
		}
	}

	//��Ϸ��ʼ && ��ʼ���ֹ���뷿��
	if (desk->MHIsDeskMatchBegin() && desk->m_isStartNoInRoom)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk() Error!!! Game has start, not start in room Error!!!, deskId=[%d], userId=[%d]",
			desk->GetDeskId(), pJoinUser->GetUserDataId());

		ret.m_errorCode = 3;		//��Ϸ�Ѿ���ʼ����ֹ���뷿��
		pJoinUser->Send(ret);
		return ret.m_errorCode;
	}

	//desk->m_startTime=msg->m_startTime;
	//desk->m_endTime=msg->m_endTime;

	//���뷿��
	if (desk->m_allowLookOn && pMsg->m_type != 3)
	{
		desk->OnUserInRoomLookOn(pJoinUser);
		return ret.m_errorCode;
	}

	/*
	//ţţ��ս���뷿���һ��  **Ren 2018-05-15
	if (desk && (NiuNiu == desk->m_state && desk->GetVip() || NiuNiu_New == desk->m_state && desk->GetVip()) && 1 == pMsg->m_addDeskFlag)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk() Player First Into Room, So Is Look On Player... deskId=[%d], userId=[%d]",
			desk->GetDeskId(), pJoinUser->GetUserDataId());

		//����Ҽ����������
		desk->OnUserInRoomLookOn(pJoinUser);
		return ret.m_errorCode;
	}

	//��������뷿���һ��
	else if (desk && (SanDaEr == desk->m_state || SanDaYi == desk->m_state || WuRenBaiFen == desk->m_state) && 
		desk->GetVip() && 1 == pMsg->m_addDeskFlag && pMsg->m_type != 3 && 
		(desk->m_allowLookOn || !desk->MHIsDeskMatchBegin() || (desk->m_clubOwerLookOn && pJoinUser->GetUserDataId() == desk->m_creatUserId))
		)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk() Player First Into Room, So Is Look On Player... deskId=[%d], userId=[%d]",
			desk->GetDeskId(), pJoinUser->GetUserDataId() && 1 == pMsg->m_addDeskFlag);

		//����Ҽ����������
		desk->OnUserInRoomLookOn(pJoinUser);
		return ret.m_errorCode;
	}
	//���ż��뷿���һ��
	else if (desk && CheXuan == desk->m_state &&
		desk->GetVip() && 1 == pMsg->m_addDeskFlag && pMsg->m_type != 3 &&
		(desk->m_allowLookOn || !desk->MHIsDeskMatchBegin() || (desk->m_clubOwerLookOn && pJoinUser->GetUserDataId() == desk->m_creatUserId))
		)
	{
		LLOG_ERROR("RoomVip::AddToVipDesk() Player First Into Room, So Is Look On Player... deskId=[%d], userId=[%d]",
			desk->GetDeskId(), pJoinUser->GetUserDataId() && 1 == pMsg->m_addDeskFlag);

		//����Ҽ����������
		desk->OnUserInRoomLookOn(pJoinUser);
		return ret.m_errorCode;
	}
	*/

	//֧�ֿ����Ժ��û������
	if (desk && desk->m_isAllowDynamicIn)
	{
		if (  (!desk->GetVip() &&desk->GetVip()->IsFull(pJoinUser) )  ||desk->GetUserCount()==desk->GetPlayerCapacity())
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk full, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);

			ret.m_errorCode=1;
			pJoinUser->Send(ret);
			return ret.m_errorCode;
		}

		//��ɢ�����У���ֹ��ҽ���
		if (desk && desk->m_resetTime > 0 && desk->m_resetUserId > 0)
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk is in dissmiss, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);
			ret.m_errorCode = 11; // �������11
			pJoinUser->Send(ret);
			return ret.m_errorCode;
		}

		if (desk&& desk->GetVip() && pMsg->m_addDeskFlag == 1 && (desk->GetVip()->IsBegin() || (!desk->GetVip()->IsBegin() && desk->m_deskState == DESK_PLAY)))
		{
			LLOG_DEBUG("LMsgS2CJoinRoomCheck");
			LMsgS2CJoinRoomCheck send;
			send.m_errorCode = 0;
			desk->fillJoinRoomCheck(send, pJoinUser->GetUserDataId());
			pJoinUser->Send(send);
			return -1;
		}

		if (!desk->GetVip()->AddUser(pJoinUser))
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk adduser fail, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);
			ret.m_errorCode=1;
			pJoinUser->Send(ret);
			return ret.m_errorCode;
		}
	}
	else
	{
		if (!desk->GetVip()||desk->GetVip()->IsBegin() || desk->m_deskState == DESK_PLAY)
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk already begin, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);
			ret.m_errorCode=3;
			pJoinUser->Send(ret);
			return ret.m_errorCode;
		}

		if (desk->GetVip()->IsFull(pJoinUser)||desk->GetUserCount()==desk->GetPlayerCapacity()||desk->getDeskState()!=DESK_WAIT)
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk full, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);

			ret.m_errorCode=1;
			pJoinUser->Send(ret);
			return ret.m_errorCode;
		}

		if (!desk->GetVip()->AddUser(pJoinUser))
		{
			LLOG_ERROR("RoomVip::AddToVipDesk desk adduser fail, userid=%d deskid=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId);
			ret.m_errorCode=1;
			pJoinUser->Send(ret);
			return ret.m_errorCode;
		}
	}

	LLOG_INFO("RoomVip::AddToVipClubDesk userid=%d deskid=%d deskusercount=%d", pJoinUser->GetUserDataId(), pMsg->m_deskId, desk->GetUserCount());
	desk->OnUserInRoom(pJoinUser);

	pJoinUser->Send(ret);
	return ret.m_errorCode;
}

//��manager�������Ӻ���managerͬ��������Ϣ
void RoomVip::SynchroDeskData(std::vector<LogicDeskInfo> & DeskInfo)
{
	LLOG_DEBUG("RoomVip::SynchroDeskData  deskSize=[%d]", m_deskMap.size());
	for (auto ItDesk=m_deskMap.begin(); ItDesk!=m_deskMap.end(); ItDesk++)
	{
		if (ItDesk->second==NULL)continue;
		LogicDeskInfo tempDeskInfo;
		tempDeskInfo.m_deskId=ItDesk->second->GetDeskId();

		tempDeskInfo.m_clubId=ItDesk->second->MHGetClubId();
		tempDeskInfo.m_playTypeId=ItDesk->second->MHGetPlayTypeId();
		tempDeskInfo.m_clubDeskId=ItDesk->second->MHGetClubDeskId();
		tempDeskInfo.m_showDeskId=ItDesk->second->MHGetClubShowDeskId();

		tempDeskInfo.m_roomFull=ItDesk->second->m_deskState==DESK_PLAY?1:0;   //����
		if (ItDesk->second->m_vip!=NULL)
		{
			tempDeskInfo.m_currCircle=ItDesk->second->m_vip->m_curCircle+1;
			tempDeskInfo.m_totalCircle=ItDesk->second->m_vip->m_maxCircle;
		}
		else
		{
			LLOG_ERROR("RoomVip::SynchroDeskData  deskId[%d] m_vip==NULL", ItDesk->first);
		}

		//�����û�
		for (int i=0; i<DESK_USER_COUNT; i++)
		{
			if (ItDesk->second->m_user[i])
			{
				LogicDeskUserInfo  tempDeskUserInfo;
				tempDeskUserInfo.m_userId=ItDesk->second->m_user[i]->GetUserDataId();
				tempDeskUserInfo.m_pos=i;
				tempDeskUserInfo.m_headUrl=ItDesk->second->m_user[i]->m_userData.m_headImageUrl;
				tempDeskUserInfo.m_nike=ItDesk->second->m_user[i]->m_userData.m_nike;
				tempDeskUserInfo.m_ip=ItDesk->second->m_user[i]->m_ip;
				tempDeskInfo.m_seatUser.push_back(tempDeskUserInfo);
			}
		}
		tempDeskInfo.m_seatUserCount=tempDeskInfo.m_seatUser.size();

		//if (ItDesk->second->m_deskState==DESK_WAIT)
		//{
		//	for (auto ItLookonUser=ItDesk->second->m_desk_Lookon_user.begin(); ItLookonUser!=ItDesk->second->m_desk_Lookon_user.end(); ItLookonUser++)
		//	{
		//		if ((*ItLookonUser)!=NULL)
		//		{
		//			LogicDeskUserInfo  tempDeskUserInfo;
		//			tempDeskUserInfo.m_userId=(*ItLookonUser)->GetUserDataId();
		//			tempDeskUserInfo.m_pos=INVAILD_POS_QIPAI;
		//			tempDeskUserInfo.m_headUrl=(*ItLookonUser)->m_userData.m_headImageUrl;
		//			tempDeskUserInfo.m_nike=(*ItLookonUser)->m_userData.m_nike;
		//			tempDeskUserInfo.m_ip=(*ItLookonUser)->m_ip;
		//			tempDeskInfo.m_noSeatUser.push_back(tempDeskUserInfo);
		//		}
		//	}
		//}

		tempDeskInfo.m_noSeatUserCount=tempDeskInfo.m_noSeatUser.size();

		DeskInfo.push_back(tempDeskInfo);
	}
}

/* ���ݴ��淨�����淨����ѡ��ȷ������ */
Lint  RoomVip::getGameDrawCount(Lint game_state, Lint no)
{
	if (_MH_DEBUG_D)
	{
		if (no == 1) return 1;
		else if (no == 2) return 2;
		else if (no == 3) return 3;
	}

	switch (game_state)
	{
	case YingSanZhang:
	{
		if (no == 1)return 10;
		else if (no == 2)return 20;
		else if (no == 3)return 16;
		else
		{
			LLOG_ERROR("getGameDrawCount error! game_state=[%d],no=[%d]", game_state, no);
			return 10;
		}
		break;
	}
	//ţţ����ѡ��
	case NiuNiu:
	{
		if (no == 1)return 10;
		else if (no == 2)return 20;
		else
		{
			LLOG_ERROR("getGameDrawCount error! game_state=[%d],no=[%d]", game_state, no);
			return 10;
		}

		break;
	}
	//�°�ţţ����ѡ��
	case NiuNiu_New:
	{
		if (no == 1) return 15;
		else if (no == 2) return 20;
		else if (no = 3) return 30;
		else
		{
			LLOG_ERROR("getGameDrawCount error! game_state=[%d],no=[%d]", game_state, no);
			return 10;
		}

		break;
	}

	case TuiTongZi:
	{
		if (no == 1)return 10;
		else if (no == 2)return 20;
		else
		{
			LLOG_ERROR("getGameDrawCount error! game_state=[%d],no=[%d]", game_state, no);
			return 10;
		}

		break;
	}
	case DouDiZhu:
	{
		if (no == 1)  return 6;
		else if (no == 2) return 9;
		else if (no == 3) return 18;
		else return 9;
		break;
	}
	//˫������ѡ��
	case ShuangSheng:
	{
		if (no == 1)return 3;
		else if (no == 2)return 6;
		else if (no == 3)return 9;
		else
		{
			LLOG_ERROR("getGameDrawCount error! game_state=[%d],no=[%d]", game_state, no);
			return 6;
		}
		break;
	}

	//���������ѡ��107
	case SanDaEr:
	{
		if (no == 1) return 8;
		else if (no == 2) return 12;
		else if (no == 3) return 20;
		else
		{
			LLOG_ERROR("RoomVip::getGameDrawCount() SanDaEr Get Game Draw Count Error!!! no=[%d]", no);
			return 6;
		}
		break;
	}

	//����һ����ѡ��109
	case SanDaYi:
	{
		if (no == 1) return 8;
		else if (no == 2) return 12;
		else if (no == 3) return 20;
		else
		{
			LLOG_ERROR("RoomVip::getGameDrawCount() WuRenBaiFen Get Game Draw Count Error!!! no=[%d]", no);
			return 6;
		}
		break;
	}

	//���˰ٷ־���ѡ��110
	case WuRenBaiFen:
	{
		if (no == 1) return 8;
		else if (no == 2) return 12;
		else if (no == 3) return 20;
		else
		{
			LLOG_ERROR("RoomVip::getGameDrawCount() WuRenBaiFen Get Game Draw Count Error!!! no=[%d]", no);
			return 6;
		}
		break;
	}

	//���ž���ѡ��111
	case CheXuan:
	{
		if (no == 1) return 8;
		else if (no == 2) return 12;
		else if (no == 3) return 20;
		else if (no == 4) return 40;
		else
		{
			LLOG_ERROR("RoomVip::getGameDrawCount() CheXuan Get Game Draw Count Error!!! no=[%d]", no);
			return 8;
		}
		break;
	}

	default:
	{
		break;
	}
	}
	return 10;
}


/* ������Ϸ���淨 �� ��ϷСѡ�� ȷ�������������*/
bool RoomVip::getDeskUserMax(Lint game_state, std::vector<Lint>& playType, Lint * userMax)
{
	switch (game_state)
	{
	case YingSanZhang:
	{
		if (playType.size() >= 7 && playType[6] == 8)
		{
			LLOG_DEBUG("getDeskUserMax set user =8");
			*userMax =8 ;
			return true;
		}
		else
		{
			LLOG_DEBUG("getDeskUserMax set user =5");
			*userMax = 5;
			return true;
		}
		break;
	}
	case TuiTongZi:
	{
		if (playType.size() >= 6 && playType[5] == 1)
		{
			LLOG_DEBUG("getDeskUserMax set user =8");
			*userMax = 8;
			return true;
		}
		else
		{
			LLOG_DEBUG("getDeskUserMax set user =4");
			*userMax = 4;
			return true;
		}
		break;
	}

	case NiuNiu:
	{
		if(playType.size() >= 6 && playType[5] == 10)
		{ 
			LLOG_DEBUG("getDeskUserMax set user = 10");
			*userMax = 10;
			return true;
		}
		else
		{
			LLOG_DEBUG("getDeskUserMax set user = 6");
			*userMax = 6;
			return true;
		}
		break;
	}

	case ShuangSheng:
	{
		LLOG_DEBUG("RoomVip::getDeskUserMax() Run... ShuangSheng Get Desk Max User = 4");
		*userMax = 4;
		return true;
		break;
	}
	//�����107
	case SanDaEr:
	{
		LLOG_DEBUG("RoomVip::getDeskUserMax() Run... SanDaEr Get Desk Max User = 5");
		*userMax = 5;
		return true;
	}

	//�°�ţţ108
	case NiuNiu_New:
	{
		if (playType.size() >= 2 && playType[1] == 10)
		{
			LLOG_DEBUG("getDeskUserMax set user = 10");
			*userMax = 10;
			return true;
		}
		/*else if (playType.size() >= 2 && playType[1] == 8)
		{
			LLOG_DEBUG("getDeskUserMax set user = 8");
			*userMax = 8;
			return true;
		}*/
		else
		{
			LLOG_DEBUG("getDeskUserMax set user = 6");
			*userMax = 6;
			return true;
		}
		break;
	}

	//����һ109��
	case SanDaYi:
	{
		LLOG_DEBUG("RoomVip::getDeskUserMax() Run... SanDaYi Get Desk Max User = 4");
		*userMax = 4;
		return true;
	}

	//���˰ٷ�110��
	case WuRenBaiFen:
	{
		LLOG_DEBUG("RoomVip::getDeskUserMax() Run... WuRenBaiFen Get Desk Max User = 5");
		*userMax = 5;
		return true;
	}

	//����111��
	case CheXuan:
	{
		if (playType.size() >= 4 && playType[3] == 8)
		{
			LLOG_DEBUG("RoomVip::getDeskUserMax() Run... CheXuan Get Desk Max User = 8");
			*userMax = 8;
			return true;
		}
		else if (playType.size() >= 4 && playType[3] == 5)
		{
			LLOG_DEBUG("RoomVip::getDeskUserMax() Run... CheXuan Get Desk Max User = 5");
			*userMax = 5;
			return true;
		}
		else if (playType.size() >= 4 && playType[3] == 2)
		{
			LLOG_DEBUG("RoomVip::getDeskUserMax() Run... CheXuan Get Desk Max User = 2");
			*userMax = 2;
			return true;
		}
		else
		{
			LLOG_DEBUG("RoomVip::getDeskUserMax() Run... CheXuan Get Desk Max User = 8");
			*userMax = 8;
			return true;
		}
	}
	default:
	{
		return false;
	}
	}

	return false;
}


void  MHExtendVipLogItem::SendBigWinnerInfo(Lint clubId, Lint deskId, Lint userId, Lstring nike, Lint state, Lint type)
{
	std::vector<Lint> paiXing;
	paiXing.push_back(type);
	MHLMsgL2LMGNewAgencyActivityUpdatePlayCount bigWinner;
	bigWinner.m_flag = AgencyActivityUpdatePlayCountType_BigWin;
	bigWinner.m_clubId = clubId;
	bigWinner.m_deskId = deskId;
	bigWinner.m_userId = userId;
	bigWinner.m_userNike = nike;
	bigWinner.m_playType = state;
	bigWinner.m_paiXing = paiXing;
	bigWinner.m_paiXingCount = bigWinner.m_paiXing.size();
	bigWinner.m_userCount = 4;
	if (!bigWinner.m_paiXing.empty())
	{
		gWork.SendToLogicManager(bigWinner);
	}

}