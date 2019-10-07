#ifndef _CARD_KOUDIAN_H_
#define _CARD_KOUDIAN_H_

#include "CardBase.h"

class ShanXiCardKouDian : public CardBase
{
public:
	ShanXiCardKouDian();
	virtual ~ShanXiCardKouDian(){};

	static	ShanXiCardKouDian*	Instance()
	{
		if (!mInstance)
		{
			mInstance = new ShanXiCardKouDian();
		}
		return mInstance;
	}

	//���˳��� ����������
	virtual ThinkVec	CheckOutCardOperator(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard,OperateState& gameInfo);

	//�Լ����� ����������
	virtual ThinkVec	CheckGetCardOperator(CardVector& handcard, CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard,Card* getCard,OperateState& gameInfo);
	
	//����Ƿ����
	virtual bool		CheckCanHu(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Hu_type>& vec);

	void				CheckCanHuNormal(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec);
	void				CheckCanHuHun(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, Card* outCard, OperateState& gameInfo,std::vector<Lint>& vec);
	
	// ���� ������������3n+1 ���ط�ֵ����winCards���� -1��ʾû����
	virtual Lint		CheckTing(CardVector& handcard,CardVector& pengCard,CardVector& agangCard,CardVector& mgangCard,CardVector& eatCard, OperateState& gameInfo, std::vector<TingCard>& winCards, const Card* ignore = NULL);

	virtual TingVec		CheckGetCardTing( CardVector& handcard, CardVector& pengCard, CardVector& agangCard, CardVector& mgangCard, CardVector& eatCard, Card* getCard, OperateState& gameInfo );

private:
	static	ShanXiCardKouDian* mInstance;
};

#define gCardKouDian ShanXiCardKouDian::Instance()

#endif

