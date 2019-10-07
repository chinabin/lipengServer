#pragma once

#ifndef _NEW_NIU_NIU_MSG_H_
#define _NEW_NIU_NIU_MSG_H_

#include "LTool.h"
#include "LMsgBase.h"
#include "..\mhgamedef.h"

//////////////////////////////////////////////////////////
// ��ţţ ��Ϣ��   
/////////////////////////////////////////////////////////
enum NEWNIUNIU_MSG_ID
{
	/////////////////////////////
	//ţţ  62071-62100
	MSG_C_2_S_NIUNIU_SELECT_ZHUANG = 62071,          //ѡׯ
	MSG_S_2_C_NIUNIU_SELECT_ZHUANG = 62072,

	MSG_C_2_S_NIUNIU_ADD_SCORE = 62073,           //��ע
	MSG_S_2_C_NIUNIU_ADD_SCORE = 62074,

	MSG_C_2_S_NIUNIU_OPEN_CARD = 62075,           //����
	MSG_S_2_C_NIUNIU_OPEN_CARD = 62076,

	MSG_S_2_C_NIUNIU_START_GAME = 62077,         //��Ϸ��ʼ

	MSG_S_2_C_NIUNIU_DRAW_RESULT = 62078,        //С����

	MSG_S_2_C_NIUNIU_RECON = 62079,              //��������
	MSG_S_2_C_NIUNIU_NOIFY_QIANG_ZHUNG = 62080,              //��ׯ֪ͨ

	MSG_S_2_C_NIUNIU_MING_PAI_ADD_SCORE = 62081,  //������ע

	MSG_C_2_S_NIUNIU_MAI_MA = 62082,			//�������
	MSG_S_2_C_NIUNIU_MAI_MA_BC = 62083,			//�������㲥�������

	MSG_C_2_S_NIUNIU_TUO_GUAN = 62084,			//���ţţ�й�
	MSG_C_2_S_NIUNIU_CANCEL_TUO_GUAN = 62085,			//���ȡ��ţţ�й�
	MSG_S_2_C_NIUNIU_TUO_GUAN_BC = 62086,		//ţţ�йܹ㲥
	MSG_S_2_C_NIUNIU_NOTIFY_CUO_GONG_PAI = 62087,		//֪ͨ��Ҵ깫��(��ʱ����)
	MSG_C_2_S_NIUNIU_CUO_GONG_PAI = 62088,				//��Ҵ깫��
	MSG_S_2_C_NIUNIU_CUO_GONG_PAI_BC = 62089,			//�㲥��Ҵ깫��

	MSG_S_2_C_UPDATE_CONINS = 61199,			//���������¿ͻ�����ҽ������
	MSG_S_2_C_LOOKON_PLAYER_FULL = 62065,   //�������㲥��ս��ң�������Ϸ�����������
	MSG_C_2_S_ADD_ROOM_SEAT_DOWN = 62066,    //�ͻ��˷��͹�ս״̬������
	MSG_S_2_C_ADD_ROOM_SEAT_DOWN = 62067,    	//���������ع�ս����������
	MSG_C_2_S_VIEW_LOOKON_LIST = 62068,			//�ͻ�������鿴��ս����б�
	MSG_S_2_C_RECT_LOOKON_LIST = 62069,			//���������ع�ս����б�
	MSG_S_2_C_SHOW_LOOKON_COUNT = 62070,		//���������ع�ս�������
	MSG_C_2_S_START_TIME = 61198,				// �ͻ���֪ͨ��������ʼ��ʱ

};
















#endif
