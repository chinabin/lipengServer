#ifndef _GAME_DEFINE_H_
#define _GAME_DEFINE_H_

#include "LBase.h"

#if defined(DEBUG) || defined(_DEBUG)
// ����������ʾ �� �����������
//#define UNCHECKHEARTBEAT
#endif

#define BASE_CARD_COUNT 36
#define LAI_ZI_COUNT 4
#define CARD_COUNT 136
#define WIND_CARD_COUNT 28
#define MAX_HANDCARD_COUNT 13

enum CARD_COLOR
{
	CARD_COLOR_WAN = 1, // ��
	CARD_COLOR_TUO = 2,	// ��
	CARD_COLOR_SUO = 3, // ��
	CARD_COLOR_ZI  = 4, // ���������з���
};

//���˼������
enum THINK_OPERATOR
{
	THINK_OPERATOR_NULL = 0,
	THINK_OPERATOR_OUT = 1,
	THINK_OPERATOR_BOMB = 2,//��
	THINK_OPERATOR_AGANG = 3,//����
	THINK_OPERATOR_MGANG = 4,//����
	THINK_OPERATOR_PENG = 5,//��
	THINK_OPERATOR_CHI = 6,//��
	THINK_OPERATOR_TING = 7,//��
	THINK_OPERATOR_MBU = 8, // ����

	THINK_OPERATOR_DINGQUE = -3,
	THINK_OPERATOR_CHANGE = -2,
	THINK_OPERATOR_REOUT = -1, // ���������ٴθ�����Ҵ��ȥ����
	//THINK_OPERATOR_NULL  = 0, 
	//THINK_OPERATOR_OUT   = 1,
	//THINK_OPERATOR_BOMB  = 2, // ��
	//THINK_OPERATOR_AGANG = 3, // ����	Ŀǰû�õ�
	//THINK_OPERATOR_MGANG = 4, // ����
	//THINK_OPERATOR_PENG  = 5, // ��
	//THINK_OPERATOR_CHI   = 6, // ��     Ŀǰû�õ�
	THINK_OPERATOR_ABU = 12, // ����
	THINK_OPERATOR_DIANGANGPAO = 14,// ��ҵ�ܺ�����������ں�����
	THINK_OPERATOR_QIANGANGH = 15, //���ܺ�
	THINK_OPERATOR_TINGCARD = 16,  //����
	THINK_OPERATOR_FLY = 17,   //��
	THINK_OPERATOR_RETURN = 18,   //��
	THINK_OPERATOR_MUSTHU = 19,   //�����
	THINK_OPERATOR_HONGTONG_YINGKOU = 20, //�鶴����Ӳ��ѡ��
	THINK_OPERATOR_HONGTONG_YINGKOU_NOT = 21, //�鶴���Ʋ�Ӳ�۵�ѡ��
	THINK_OPERATOR_HONGTONG_XUANWANG=22  //�鶴����˫��ѡ����**Ren 2017-12-12��

};
#define DESK_USER_COUNT 4	//һ���������

#define INVAILD_POS		4     //��Ч��λ��
#define INVAILD_POS_QIPAI 21  //��Ч��λ��
#define INVAILD_POS_MANAGER_START -1    //������ʼ��Ϸ

#define	PLAY_CARD_TIME		10  //����ʱ��
#define WAIT_READY_TIME		15  //׼��ʱ��

//���ӵ�ǰ״̬
enum DESK_STATE
{
	DESK_FREE =		0,	//���ӿ���״̬
	DESK_WAIT,			//���ӵȴ�״̬
	DESK_PLAY,			//������Ϸ��
	//	DESK_COUNT_RESULT,	//���ڽ�����
	//	DESK_RESET_ROOM		//���������ɢ������
};

//�ƾֵ�ǰ״̬
enum DESK_PLAY_STATE
{
	DESK_PLAY_START_HU,		//���ֺ�״̬
	DESK_PLAY_GET_CARD,		//����
	DESK_PLAY_THINK_CARD,	//˼��
	DESK_PLAY_END_CARD,		//������
	DESK_PLAY_DINGQUE,		//��ȱ
	DESK_PLAY_CHANGE,		//����
	DESK_PLAY_FANPIGU,		//��ƨ�ɲ���
};

//�Һ��������0-û����1-������2-���ڣ�3-����
enum WIN_TYPE
{
	WIN_INVALID = -1,
	WIN_ZIMO = 0,	//����
	WIN_BOMB,		//����
	WIN_NONE,		//��ׯ
	WIN_DISS,       //��ɢ����
	WIN_MINGGANGHUA,//���ܿ��� ��4  �����ӣ����ܸ��Ͽ�������������**Ren 2017-12-01��
};

//���Ʒ�ʽ
enum WIN_TYPE_SUB
{
	WIN_SUB_NONE = 0,
	WIN_SUB_ZIMO = 1,		//����
	WIN_SUB_BOMB = 2,		//����
	WIN_SUB_ABOMB = 3,		//����

	WIN_SUB_DZIMO = 4,		//�������
	WIN_SUB_DBOMB = 5,		//�������
	WIN_SUB_DABOMB = 6,		//�������
	WIN_SUB_TINGCARD = 7,   //�н� 3/4Ѫս��2��
	WIN_SUB_MINGGANGHUA=8,//���ܿ�����8   �����ӣ����ܸ��Ͽ�������������**Ren 2017-12-01��
	WIN_SUB_ZIMOANGANGHUA=9,    //����(���ܿ���) //��Ӹ��Ͽ������ܣ�**Ren 2017-12-11��
	WIN_SUB_ZIMOMINGGANGHUA=10, //����(���ܿ���) //��Ӹ��Ͽ������ܣ�**Ren 2017-12-11��

};

//���ֺ�������
enum START_HU_INFO
{
	START_HU_NULL = 0,
	START_HU_QUEYISE=1,	//ȱһɫ
	START_HU_BANBANHU,		//����
	START_HU_DASIXI,		//����ϲ
	START_HU_LIULIUSHUN,	//����˳
};

//�齫����
typedef int GameType;
enum {
	SiChuanMaJiang		= 101,	//Ѫս����
	SiChuanMaJinagxl	= 102,	//Ѫ���ɺ�
	SiChuanMaJinagThr   = 103,  //3��Ѫս�齫
	SiChuanMaJinagZZ	= 2001,	//������
	SiChuanMaJiangFourTwo = 105,  //4��2��
	SiChuanMaJiangDeYang = 106,   //�����齫
	SiChuanMaJiangThrThr = 107,   //��������
	SiChuanMaJiangManYng = 108,  //����
	SiChuanMaJiangYiBin  = 109,  //�Ĵ��˱�
	SiChuanMaJiangWanZhou= 110,	// ����
	YunNanMajiang		= 201,	// �����齫
	KunMingMajiang		= 202,	// �����齫
	LiJiangKaXinWu		= 203,  // ����������
	YunNanFeiXiaoJi     = 204,  // ���Ϸ�С��
	SiChuanMaJinagxlc	= 1102,	//Ѫ���ɺ� ��ҳ�

};

enum HU_WAY
{
	HUWAY_ZiMo =1,
	HUWAY_MoBao =2,
	HUWAY_DuiBao =3,
};

//�����ɢ����
enum RESET_ROOM_TYPE
{
	RESET_ROOM_TYPE_WAIT,
	RESET_ROOM_TYPE_AGREE,
	RESET_ROOM_TYPE_REFLUSE,
};


enum ErrorCode
{
	ErrorNone = 0,
	UserNotExist = 1,
	TaskNotExist = 2,
	ExceedMaximum = 3,
	AlreadyBinded = 4,
	NotQualified = 5,
	TaskNotFinished = 6,
	Expired = 7,
	BindingSelf = 8,
	Unknown = 100
};



#endif