#ifndef CMD_GAME_LINFEN_HEAD_FILE
#define CMD_GAME_LINFEN_HEAD_FILE

#pragma once

#pragma pack(1)

#include <minwindef.h>
#include "../mhgamedef.h"



//////////////////////////////////////////////////////////////////////////////////
//�����


//�ٷ�ȥ����������  ������������
//#define SUB_S_LINFEN_GAME_START                    120           //��Ϸ��ʼ,����
//#define SUB_S_LINFEN_CALL_SCORE                      121            //�û��з�
//#define SUB_S_LINFEN_BANKER_INFO                   122           //ׯ����Ϣ
#define SUB_S_LINFEN_TIPAI                               123            //�û�����
#define SUB_S_LINFEN_GENTI                             124            //�û�����
#define SUB_S_LINFEN_HUIPAI                            125          //�û�����
#define SUB_S_LINFEN_BANKER_OUT_CARD   126           //ׯ�ҿ�ʼѡ�����
//#define SUB_S_LINFEN_OUT_CARD                        127           //�û�����
//#define SUB_S_LINFEN_PASS_CARD                        128           //�û�����
//#define SUB_S_LINFEN_GAME_CONCLUDE            129           //��Ϸ����

#define SUB_S_LINFEN_SCORE_FRESH                  130        //ˢ�±���
#define SUB_S_LINFEN_TIPAI_RESULT                         131      //�û����ƽ��
#define SUB_S_LINFEN_GENTI_RESULT                         132      //�û����߽��
#define SUB_S_LINFEN_HUIPAI_RESULT                         133      //�û����ƽ��
#define SUB_S_LINFEN_USERS_TI_RESULT                     134      //�����û���ti���

struct CMD_S_BankerOutCard
{
	int m_Pos;                                       //ׯ��λ��
};

struct CMD_S_NotifyUserTi
{
	int    m_pos;                     //���û�λ��
};

//�����û���TI���
struct CMD_S_UsersTiResult
{
	BYTE                             wUserStatue[DOUDIZHU_PLAY_USER_COUNT];                  //����Ƿ�ѡ��  0-û��ѡ��1-���ƣ�2-����,3-����
};

//TI�Ľ��
struct CMD_S_TiResult
{
	WORD							wActUser;							//�������
	BYTE                              wActSelect;                     // ����Ƿ�ѡ��
};

//ˢ�±���
struct CMD_S_ScoreFresh
{
	LONG							score;						//�Ƿ�ѡ��
};


//�û�����
struct CMD_S_TiPai_Linfen
{
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wCallScoreUser;						//�������
	BYTE							cbCurrentScore;						//��ǰ����
	BYTE							cbUserCallScore;					//�ϼ�����
};

//�û�����
struct CMD_S_HuiTi_Linfen
{
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wCallScoreUser;						//�������
	BYTE							cbCurrentScore;						//��ǰ����
	BYTE							cbUserCallScore;					//�ϼ�����
};


#define SUB_C_TIPAI                          5                                //�û�����
#define SUB_C_GENTI                        6                                 //�û�����
#define SUB_C_HUIPAI                      7                                 //�û�����
//
////�û�����,���ߣ�����
struct CMD_C_Ti
{
	BYTE							cbIsYes;						//�Ƿ�ѡ��
};

//////////////////////////////////////////////////////////////////////////////////

#pragma pack()

#endif