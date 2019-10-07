#pragma once
#ifndef _CLUB_PLAY_H_
#define _CLUB_PLAY_H_

#include "LMsgS2S.h"
#include "SafeResourceLock.h"

////////////////////////////////////////////////////////////////////////////////////////////
struct ClubPlayInfo :public CResourceLock 
{
	/* ���ֲ��淨���� */
	Lstring           m_playTypeString;

	/* ���ֲ��淨���� */
	playTypeDetail    m_detail;

	ClubPlayInfo() {}

	~ClubPlayInfo(){}

	/* ����=���������ṹ�帳ֵ */
	void copy(playTypeDetail& detail)
	{
		m_detail = detail;
	}

	Lstring getPlayString()
	{
		return m_playTypeString;
	}

	void updatePlayString(Lstring str)
	{
		m_playTypeString = str;
	}

	

	/* ����Json��ʽ���ֲ��淨���� */
	bool     parsePlayType(Lint playTypeId, const Lstring playType);

	/* ƴдJson��ʽ���ֲ��淨���� */
	Lstring  composePlayType();

	Lstring spec(Lstring str)
	{
		return "\"" + str + "\""+":";
	}

	/*
	���ݿ�club_playtypes.PlayType�ֶ�����
	{
		"play_type_name":"",
		"play_type":111,
		"play_rule":[1,0,500,5,0,0,0,0,0,0,0,1,1,0],
		"user_count":5,"round_count":1,"is_vip":0,
		"gps_limit":0,
		"allow_look":0,
		"club_ower_look":0,
		"join_score":0,
		"rob_score":0,
		"can_minus":0,
		"warn_score":0,
		"club_id":12356
	}
	*/

	/* ��ȡ�淨���� */
	Lstring get_play_type_name()
	{
		Lstring str;
		if (m_detail.m_name.empty())
			str = spec("play_type_name") + "\"\"";
		else
			str = spec("play_type_name") + m_detail.m_name;
		return  str;
	}
	Lstring get_play_type()
	{
		return spec("play_type") + std::to_string(m_detail.m_state);
	}
	Lstring get_play_rule()
	{
		Lstring str("[");
		for (auto It = m_detail.m_playtype.begin(); It != m_detail.m_playtype.end(); It++)
		{
			str.append(std::to_string(*It));
			if (It + 1 != m_detail.m_playtype.end())
				str.append(",");
		}
		str.append("]");

		return spec("play_rule") + str;
	}
	Lstring get_user_count()
	{
		return spec("user_count") + std::to_string(m_detail.m_Greater2CanStart);
	}
	Lstring get_round_count()
	{
		return  spec("round_count") + std::to_string(m_detail.m_flag);
	}
	Lstring get_is_vip()
	{
		return  spec("is_vip") + std::to_string(m_detail.m_cheatAgainst);
	}
	Lstring get_gps_limit()
	{
		return  spec("gps_limit") + std::to_string(m_detail.m_GpsLimit);
	}
	Lstring get_allow_look()
	{
		return  spec("allow_look") + std::to_string(m_detail.m_allowLookOn);
	}
	Lstring get_club_ower_look()
	{
		return  spec("club_ower_look") + std::to_string(m_detail.m_clubOwerLookOn);
	}
	Lstring get_join_score()
	{
		return  spec("join_score") + std::to_string(m_detail.m_minPointInRoom);
	}
	Lstring get_rob_score()
	{
		return  spec("rob_score") + std::to_string(m_detail.m_minPointXiaZhu);
	}
	Lstring get_can_minus()
	{
		return  spec("can_minus") + std::to_string(m_detail.m_smallZero);
	}
	Lstring get_warn_score()
	{
		return  spec("warn_score") + std::to_string(m_detail.m_warnScore);
	}
};
////////////////////////////////////////////////////////////////////////////////////////////
class ClubPlayManager :public LSingleton<ClubPlayManager>
{
public:
	virtual	bool Init();
	virtual	bool Final();
public:

	/* �����淨ID����ȡ�������淨��ϸ��Ϣ�����ָ�� */
	boost::shared_ptr<CSafeResourceLock<ClubPlayInfo>> getClubPlayInfoByClubPlayId(Lint clubPlayId);

	/* ����淨��Ϣ */
	bool addClubPlay(clubPlayType& aClubPlayType, Lint beforePlayId);

	/* �����淨��ϢID��ɾ���淨��ϸ��Ϣ */
	void delClubPlay(Lint clubPlayId);

	/* �����淨ID����ȡ�淨��ϸ��Ϣ�ṹ���� */
	playTypeDetail  getClubPlayInfo(Lint clubPlayId);

	/* �����淨��ϸ��Ϣ*/
	bool updatePlayType(LMsgC2SUpdatePlay* msg, Lint playId);

	/* �����淨��ϸ��Ϣ�н��뷿����������С����ֵ����������*/
	bool updatePlayType(Lint playId, Lint value, Lint type);

	/* �����淨��ϢID����ȡ�淨��ϸ��Ϣ�е�������� */
	Lint  getPlayMaxUser(Lint playId);

private:

	/* �����淨��ϸ��Ϣ�����ݿ� */
	bool updatePlayTypeToDB(boost::shared_ptr<ClubPlayInfo> playInfo);
	
private:

	/* �����淨��Ϣ����� */
	boost::mutex m_mutexClubPlayQueue;

	/* �����淨��ϸ��Ϣ��map��map<�淨ID���淨��ϸ��Ϣ>*/
	std::map<Lint, boost::shared_ptr<ClubPlayInfo>> m_mapId2ClubPlayInfo;
};

#define gClubPlayManager ClubPlayManager::Instance()

#endif
