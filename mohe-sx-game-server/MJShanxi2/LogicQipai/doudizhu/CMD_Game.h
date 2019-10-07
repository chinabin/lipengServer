
#ifndef CMD_GAME_HEAD_FILE
#define CMD_GAME_HEAD_FILE

#include <minwindef.h>
#include "../mhgamedef.h"

#pragma pack(1)


//��ֵ����
#define	MASK_COLOR					0xF0								//��ɫ����
#define	MASK_VALUE					0x0F								//��ֵ����

//�߼�����
#define CT_ERROR					0									//��������
#define CT_SINGLE					1									//��������
#define CT_DOUBLE					2									//��������
#define CT_THREE					3									//��������
#define CT_SINGLE_LINE				4									//��������
#define CT_DOUBLE_LINE				5									//��������
#define CT_THREE_LINE				6									//��������
#define CT_THREE_TAKE_ONE			7									//����һ��
#define CT_THREE_TAKE_TWO			8									//����һ��
#define CT_FOUR_TAKE_ONE			9									//�Ĵ�����
#define CT_FOUR_TAKE_TWO			10									//�Ĵ�����
#define CT_BOMB_SOFT               11
#define CT_BOMB_CARD				12									//ը������
#define CT_MISSILE_CARD				13									//�������

//////////////////////////////////////////////////////////////////////////////////


//����״̬
struct CMD_S_StatusFree
{
	BYTE                            cbPlayStatus[DOUDIZHU_PLAY_USER_COUNT];          //�û�״̬
	//�淨����
	BYTE                      cbPlayType;                         //�������淨

	//��Ϸ����
	LONG							lCellScore;							//��������

	//ʱ����Ϣ
	BYTE							cbTimeOutCard;						//����ʱ��
	BYTE							cbTimeCallScore;					//�з�ʱ��
	BYTE							cbTimeStartGame;					//��ʼʱ��
	BYTE							cbTimeHeadOutCard;					//�׳�ʱ��

};

//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_S_GAME_START			100									//��Ϸ��ʼ
#define SUB_S_CALL_SCORE			101									//�û��з�
#define SUB_S_BANKER_INFO			102									//ׯ����Ϣ
#define SUB_S_OUT_CARD				103									//�û�����
#define SUB_S_PASS_CARD				104									//�û�����
#define SUB_S_GAME_CONCLUDE			105									//��Ϸ����
#define SUB_S_SET_BASESCORE			106									//���û���
#define SUB_S_CHEAT_CARD			107									//�����˿�
#define SUB_S_TRUSTEE				108									//�й�

//�����˿�
struct CMD_S_GameStart
{
	WORD							wStartUser;							    //��ʼ���
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbValidCardData;					  //�����˿�
	BYTE							cbValidCardIndex;					  //����λ��
	BYTE                           cbCardCount;                             // ��������
	BYTE							cbCardData[PLAY_TYPE_CARD_HAND_MAX];			//�˿��б�
	//int                              cbScore[DOUDIZHU_PLAY_USER_COUNT];            //��Ϸ����
	BYTE                         cbOnlyCall;                             //��ǰ���ֻ�ܽз�
};


//�û��з�
struct CMD_S_CallScore
{
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wCallScoreUser;						//�з����
	BYTE							cbCurrentScore;						//��ǰ�з�
	BYTE							cbUserCallScore;					//�ϴνз�
	BYTE                         cbOnlyCall;                             //��ǰ���ֻ�ܽз�
};

//ׯ����Ϣ
struct CMD_S_BankerInfo
{
	WORD				 			wBankerUser;						//ׯ�����
	WORD				 			wCurrentUser;						//��ǰ���
	BYTE							cbBankerScore;						//ׯ�ҽз�
	BYTE                           cbBankerCardCount;            //ׯ�ҵ�������
	//BYTE							cbBankerCard[3];					//ׯ���˿�
	BYTE							cbBankerCard[4];					//ׯ���˿ˣ� �����Ϊ4
};

//�û�����
struct CMD_S_OutCard
{
	BYTE							cbCardCount;						//������Ŀ
	WORD				 			wCurrentUser;						//��ǰ���
	WORD							wOutCardUser;						//�������
	//BYTE							cbCardData[MAX_COUNT];				//�˿��б�
	BYTE							cbCardData[21];    //�˿��б�,�����Ϊ21
};

//��������
struct CMD_S_PassCard
{
	BYTE							  cbTurnOver;							//һ�ֽ���
	WORD				 			wCurrentUser;						//��ǰ���
	WORD				 			wPassCardUser;						//�������
};

//��Ϸ����
struct CMD_S_GameConclude
{
	BYTE                          cbIsFinalDraw;              //�Ƿ������һ�֣��ﵽ���Ƶľ���
	BYTE                          cbBankerUser;             // ����λ��
	BYTE                               cbBankerCall;              //ׯ�ҽз�
	//���ֱ���
	LONG							lCellScore;							//��Ԫ����
	int							lGameScore[DOUDIZHU_PLAY_USER_COUNT];			//��Ϸ����

	//�����־
	BYTE							bChunTian;							//�����־
	BYTE							bFanChunTian;						//�����־

	//ը����Ϣ
	BYTE							cbBombCount;						//ը������
	BYTE							cbEachBombCount[DOUDIZHU_PLAY_USER_COUNT];		//ը������

	//��Ϸ��Ϣ
	BYTE							cbBankerScore[DOUDIZHU_PLAY_USER_COUNT];						//�з���Ŀ,ÿ��ֵ����ׯ�ҽзֵ���ֵ
	BYTE							cbCardCount[DOUDIZHU_PLAY_USER_COUNT];			               //�˿���Ŀ
	BYTE							cbHandCardData[DOUDIZHU_PLAY_USER_COUNT][PLAY_TYPE_CARD_HAND_MAX+ PLAY_TYPE_CARD_REST_MAX];		   //�˿��б�
	BYTE                         cbActSelect[DOUDIZHU_PLAY_USER_COUNT];              //0-���� ��1-��  2�� 3��
	//int                              cbScore[DOUDIZHU_PLAY_USER_COUNT];            //��Ϸ����
	BYTE                         cbRestCardData[PLAY_TYPE_CARD_REST_MAX];     //����
};



//////////////////////////////////////////////////////////////////////////////////
//�����

#define SUB_C_CALL_SCORE			1									//�û��з�
#define SUB_C_OUT_CARD			2									//�û�����
#define SUB_C_PASS_CARD			3									//�û�����
#define SUB_C_TRUSTEE				4									//�û��й�

//�û��з�
struct CMD_C_CallScore
{
	int						cbCallScore;						//   �з���Ŀ
};

//�û�����
struct CMD_C_OutCard
{
	BYTE							cbCardCount;						        //������Ŀ
	BYTE							cbCardData[PLAY_TYPE_CARD_HAND_MAX+PLAY_TYPE_CARD_REST_COUNT_4];		//�˿�����
};


//////////////////////////////////////////////////////////////////////////////////




#pragma pack()

#endif