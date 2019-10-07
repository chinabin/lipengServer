#pragma once
#include "LMsg.h"
#include "LMsgS2S.h"
#include "LMsgL2C.h"
#include "LLog.h"
#include "..\LogicQipai\LMsgPoker.h"
#include "..\LogicServer\malgo_tile.h"
//#define _MH_DEBUG              // �������԰汾, �޸��������Ȧ��Ϊ1��2
#define _MH_ROBOT_PERF_TEST 0    // ����Ϊ1�������������Զ���������˽��з�����ѹ�����ܲ���

//#define _MH_ENABLE_LOG_DEBUG
//#define _MH_ENABLE_LOG_INFO
#define _MH_ENABLE_LOG_PLAYCARD
#define _MH_ENABLE_LOG_DESK
//#define _MH_ENABLE_LOG_TING


//#define _MH_USE_FIX_HAO_ZI
#define MH_HAOZI_1 31
#define MH_HAOZI_2 32

#ifdef _MH_ENABLE_LOG_DEBUG
#define  MHLOG LLOG_DEBUG              //������Ե�LOG����
#else
#define MHLOG(v1,v2,v3,v4,v5,v6) (void*)0;
#endif

#ifdef _MH_ENABLE_LOG_INFO
#define  MHLOG_INFO LLOG_INFO           //������Ե�LOG����
#else
#define MHLOGINFO(v1,v2,v3,v4,v5,v6) (void*)0;
#endif

#ifdef _MH_ENABLE_LOG_PLAYCARD
#define  MHLOG_PLAYCARD LLOG_ERROR           //������Ե�LOG����
#else
#define MHLOG_PLAYCARD(v1,v2,v3,v4,v5,v6) (void*)0;
#endif

#ifdef _MH_ENABLE_LOG_TING
#define  MHLOG_TING LLOG_DEBUG           //������Ե�LOG����
#else
#define MHLOG_TING(v1,v2,v3,v4,v5,v6) (void*)0;
#endif


#ifdef _MH_ENABLE_LOG_DESK
#define  MHLOG_DESK LLOG_ERROR          //������Ե�LOG����
#else
#define  MHLOG_DESK(v1,v2,v3,v4,v5,v6) (void*)0;
#endif


#define  _MH_ALERT_URL_ROOM_NOT_ENOUGH "http://api.ry.haoyunlaiyule.com/server/alert?type=10"
#define  _MH_ALERT_URL_SERVER_CRASH "http://api.ry.haoyunlaiyule.com/server/alert?type=11"
#define _MH_DEFALUT_MAX_DESK_COUNT_LIMIT_FOR_LOGIC_SERVER 100                 //������Ĭ����󷿼���������
#define _MH_DEFALUT_MAX_DESK_COUNT_FOR_OTHER 8                                //������������������� 
#define _MH_GPS_DISTANCE_LIMIT        200.0f                                 //��СGPS��������
 
void MHInitFactoryMessage();
void MH_InitFactoryMessage_ForMapMessage();
double convert_string2float(const Lstring &strValue);
void get_gps_pair_values(const Lstring & strGPS, double& value1, double& value2);
double calc_gps_distance(double lat1, double lng1, double lat2, double lng2);
Lstring string_replace(Lstring &src, Lstring &replace, Lstring &value);
