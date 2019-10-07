#pragma once

#ifndef _CHE_XUAN_MSG_H_
#define _CHE_XUAN_MSG_H_

#include "LTool.h"
#include "LMsgBase.h"
#include "..\mhgamedef.h"


//////////////////////////////////////////////////////////
// ���� ��Ϣ��   
/////////////////////////////////////////////////////////
enum CHEXUAN_MSG_ID
{
	//////////////////////////////////////////////////////////////////////////
	//������Ϣ�Ź滮 62400-62449

	//S->C ������֪ͨ����𲧲�
	MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_QI_BOBO = 62400,

	//C->S ����𲧲�
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_BOBO = 62401,

	//S->C �㲥����𲧲����
	MSG_S_2_C_CHEXUAN_PLAYER_SELECT_BOBO_BC = 62402,

	//S->C �����֪ͨ�����â��
	MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_ADD_MANGGUO = 62403,

	//C->S �����â��
	MSG_C_2_S_CHEXUAN_PLAYER_ADD_MANGGUO = 62404,

	//S->C �㲥�����â��
	MSG_S_2_C_CHEXUAN_PLAYER_ADD_MANGGUO_BC = 62405,

	//S->C ����������ҷ���
	MSG_S_2_C_CHEXUAN_SEND_CARD = 62406,

	//S->C ������֪ͨ��Ҳ���
	MSG_S_2_C_CHEXUAN_NOTIFY_PLAYER_OPT = 62407,

	//C->S ���ѡ�����
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_OPT = 62408,

	//S->C �������㲥��Ҳ������
	MSG_S_2_C_CHEXUAN_PLAYER_SELECT_OPT_BC = 62409,

	//S->C ������֪ͨ��ҿ�ʼ����
	MSG_S_2_C_CHEXUAN_NOTIFY_CHE_PAI = 62410,

	//C->S ��ҳ��Ʋ���
	MSG_C_2_S_CHEXUAN_PLAYER_CHE_PAI = 62411,

	//S->C ������ҳ��ƽ������㲥���
	MSG_S_2_C_CHEXUAN_PLAYER_CHE_PAI_BC = 62413,

	//S->C С������Ϣ
	MSG_S_2_C_CHEXUAN_DRAW_RESULT_BC = 62414,

	//S->C ���������Ͷ�������
	MSG_S_2_C_CHEXUAN_RECON = 62415,

	//S->C ������֪ͨ�ͻ��˿�ʼ����
	MSG_S_2_C_CHEXUAN_NOTIFY_CUO_PAI = 62416,

	//C->S ��ҳ��Ʋ���
	MSG_C_2_S_CHEXUAN_PLAYER_CUO_PAI = 62417,

	//S->C ������ҳ��ƽ������㲥���
	MSG_S_2_C_CHEXUAN_PLAYER_CUO_PAI_BC = 62418,

	//C->S �ͻ�����âע���
	MSG_C_2_S_CHEXUAN_PLAYER_ADD_MANG_SCORE_BC = 62419,

	//C->S �ͻ����ƾ�������ս��
	MSG_C_2_S_CHEXUAN_PLAYER_REQUEST_RECORD = 62420,

	//S->C �����������ƾ���ս�����
	MSG_S_2_C_CHEXUAN_PLAYER_SEND_RECORD = 62421,

	//C->S �ͻ��˲���Ĭ��
	MSG_C_2_S_CHEXUAN_PLAYER_OPT_MO_FEN = 62422,

	//S->C ����˷��ؿͻ��˲���Ĭ��
	MSG_S_2_C_CHEXUAN_PLAYER_OPT_MO_FEN = 62423,

	//C->S �ͻ���ѡ���Զ�����
	MSG_C_2_S_CHEXUAN_PLAYER_SELECT_AUTO_OPT = 62424,

	//S->C �������������ѡ����Զ��������
	MSG_S_2_C_CHEXUAN_PLAYER_SELECT_AUTO_OPT_RET = 62425,

	//C->S �ͻ���ѡ��վ�������
	MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT = 62427,

	//S->C �������������վ������½��
	MSG_C_2_S_CHEXUAN_PLAYER_STAND_OR_SEAT_RET = 62428

};







#endif
