#ifndef _ROBOT_H
#define _ROBOT_H

#include "LBase.h"
#include "User.h"

class Robot : public User
{
public:
	Robot(const LUser& data);
	~Robot(void);
	
	virtual bool IsRobot() { return true; }

	virtual void	Login();
	// ����˳��������
	virtual void	Logout();

	// ֱ�ӷ����û�
	virtual void	Send(LMsg& msg);
	virtual void	Send(const LBuffPtr& buff);

	virtual void	AddCoinsCount(Lint count, Lint operType);
	virtual void	DelCoinsCount(Lint count, Lint operType);

private:
	
};

typedef boost::shared_ptr<Robot> RobotPtr; 

#endif
