#ifndef _LVIP_LOG_H
#define _LVIP_LOG_H

#include "LTool.h"
#include "LBuff.h"
#include "GameDefine.h"

struct API_LBASESX VipDeskLog
{
	Lstring	 m_videoId;//¼��id
	Lint	 m_gold[DESK_USER_COUNT];
	Lint	 m_zhuangPos;//��ׯ��λ��
	Lint	 m_time;//����ʱ��
	Lint	 m_angang[DESK_USER_COUNT];//����
	Lint	 m_mgang[DESK_USER_COUNT];//����
	Lint	 m_checkTing[DESK_USER_COUNT];	//����

	Lint	m_zimo[DESK_USER_COUNT];//��������
	Lint	m_bomb[DESK_USER_COUNT];//���ڴ���
	Lint	m_win[DESK_USER_COUNT];//���ڴ���
	Lint	m_dzimo[DESK_USER_COUNT];//�����������
	Lint	m_dbomb[DESK_USER_COUNT];//������ڴ���
	Lint	m_dwin[DESK_USER_COUNT];//������ڴ���

	VipDeskLog()
	{
		memset(m_gold, 0, sizeof(m_gold));
		memset(m_angang, 0, sizeof(m_angang));
		memset(m_mgang, 0, sizeof(m_mgang));
		memset(m_checkTing, 0, sizeof(m_checkTing));
		m_zhuangPos = INVAILD_POS;
		m_time = 0;
		m_videoId = "";

		memset(m_zimo, 0, sizeof(m_zimo));
		memset(m_bomb, 0, sizeof(m_bomb));
		memset(m_win, 0, sizeof(m_win));
		memset(m_dzimo, 0, sizeof(m_dzimo));
		memset(m_dbomb, 0, sizeof(m_dbomb));
		memset(m_dwin, 0, sizeof(m_dwin));
	}
};

struct API_LBASESX LVipLogItem
{
	Lstring			m_id;		//id
	Lint			m_time;		//ʱ��
	Lint			m_state;	//�����淨0-������1-���ڣ�2-����
	Lint			m_deskId;	//����id
	Lstring			m_secret;	//����
	Lint			m_maxCircle;//��Ȧ��
	Lint			m_curCircle;//��ǰȦ������ˢ���ͻ��˵ģ���ʾ���ơ���1/8�֡�����
	Lint			m_curCircleReal; // ʵ�ʵĵ�ǰȦ������Ϊ�е��淨��ׯ����Ȧ������ֵ�������жϵ�1��insert������update�����ݿ�
	Lint			m_posUserId[DESK_USER_COUNT];//0-3����λ���ϵ����id
	Lint			m_curZhuangPos;//��ǰׯ��
	Lint			m_score[DESK_USER_COUNT];// ����λ������Ļ���
	Lint			m_reset;//�Ƿ����
	std::vector<Lint> m_playtype;		//�淨�б�  1-�����ӵ� 2-�����ӷ�
	std::vector<VipDeskLog*> m_log;//ÿһ�ѵļ�¼
	Lint			m_checkTing[DESK_USER_COUNT];		//��������

	Lint			m_iPlayerCapacity;				//��������������ܳ���DESK_USER_COUNT

	LVipLogItem();
	virtual  ~LVipLogItem();
	virtual  Lstring		ToString();

	virtual  void			FromString(const Lstring& str);

	virtual  Lstring		PlayTypeToStrint();

	virtual  void			PlayTypeFromString(const Lstring& str);
};
#endif