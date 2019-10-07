#ifndef _COUNTER_H_
#define _COUNTER_H_

#include "LRunnable.h"
#include "LTime.h"
#include "CardBase.h"

class CounterUnit
{
public:
	CounterUnit()
		: m_finish(false)
	{

	}

	virtual ~CounterUnit() {}

	void Finish();
	void WaitFinish();
	virtual void Calc() = 0;

	bool m_finish;
protected:
	boost::mutex m_mutex;
	boost::condition_variable m_condition;
};

class CounterTing : public CounterUnit
{
public:
	virtual void Calc();

	CardBase* cardmgr;
	CardVector* handcard;
	CardVector* pengCard;
	CardVector* agangCard;
	CardVector* mgangCard;
	CardVector* eatCard;
	OperateState* gameInfo;

	int index;	// ��ʾ�ڼ��� �����ֶμ���
	TingVec result;
};

class CounterTingWithHun : public CounterTing
{
public:
	virtual void Calc();

	std::vector<CardValue> huntype;	// ��ʾ���Ƶ����� �磺���ֻ��һ���ۼ����� ֱ������һ���ۼ�����
};


//db������
class Counter : public LRunnable
{
public:
	Counter();

protected:
	virtual bool		Init();
	virtual bool		Final();

	virtual void		Run();
	virtual void		Clear();

public:
	void				Notify( CounterUnit* unit );

protected:
	CounterUnit* m_counter;
	boost::mutex m_mutex;
	boost::condition_variable m_condition;
};

#endif