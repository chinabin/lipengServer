#pragma once

#ifndef _SPEC_ACTIVE_H
#define _SPEC_ACTIVE_H
#include "LSingleton.h"
#include "LMsgS2S.h"

class SpecActive : public LSingleton<SpecActive>
{
public:
	SpecActive();
	~SpecActive();

	virtual	bool		Init();
	virtual	bool		Final();

	
	void sendToManager(Lint manageId,Lint id = 0);    //id=0��ȫ������ id !=0 ,��ָ��id��
	bool loadData();

private:

	void addSpecActiveItem(boost::shared_ptr<SpecActiveItem> specItem);      //��Ӻ͸��£����ڸ��£������ڸ���

	std::map<Lint, boost::shared_ptr<SpecActiveItem>>   m_mapId2SpecActive;
};

#define gSpecActive SpecActive::Instance()








#endif
