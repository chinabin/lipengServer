#ifndef GAME_LOGIC_LAIZI_HEAD_FILE
#define GAME_LOGIC_LAIZI_HEAD_FILE

#pragma once

//////////////////////////////////////////////////////////////////////////


//扑克类型
#define CT_INVALID					0									//错误类型
#define CT_SINGLE					1									//单牌类型
#define CT_DOUBLE					2									//对牌类型
#define CT_THREE					3									//三条类型
#define CT_ONE_LINE					4									//单连类型
#define CT_DOUBLE_LINE				5									//对连类型
#define CT_THREE_LINE				6									//三连类型
#define CT_THREE_LINE_TAKE_ONE		7									//三带一单
#define CT_THREE_LINE_TAKE_DOUBLE	8									//三带一对
#define CT_FOUR_LINE_TAKE_ONE		9									//四带两单
#define CT_FOUR_LINE_TAKE_DOUBLE	10									//四带两对
#define CT_BOMB_SOFT				11									//软炸
#define CT_BOMB_CARD				12									//炸弹类型
#define CT_MISSILE_CARD				13									//火箭类型

//数值掩码
#define	LOGIC_MASK_COLOR			0xF0								//花色掩码
#define	LOGIC_MASK_VALUE			0x0F								//数值掩码

//分析结构
struct tagAnalyseResultLaizi
{
	BYTE 							bFourCount;							//四张数目
	BYTE 							bThreeCount;						//三张数目
	BYTE 							bDoubleCount;						//两张数目
	BYTE							bSignedCount;						//单张数目
	BYTE 							bFourLogicVolue[5];					//四张列表
	BYTE 							bThreeLogicVolue[7];				//三张列表
	BYTE 							bDoubleLogicVolue[10];				//两张列表
	BYTE							m_bSCardData[21];					//手上扑克
	BYTE							m_bDCardData[21];					//手上扑克
	BYTE							m_bTCardData[21];					//手上扑克
	BYTE							m_bFCardData[21];					//手上扑克
};

//////////////////////////////////////////////////////////////////////////

//游戏逻辑类
class CGameLogicLaizi
{
	//变量定义
protected:
	static const BYTE				m_bCardListData[55];				//扑克数据

	//函数定义
public:
	//构造函数
	CGameLogicLaizi();
	//析构函数
	virtual ~CGameLogicLaizi();

	//类型函数
public:
	//获取类型
	BYTE GetCardType(const BYTE bCardData[], BYTE bCardCount, bool isDui3Tong=false);
	//获取类型(有赖子)
	BYTE GetMagicCardType(const BYTE bCardData[], BYTE bCardCount);

	BYTE GetMagicCardType(const BYTE bCardData[], BYTE bCardCount, BYTE *bMagicCard, bool isDui3Tong = false);

	//获取类型(有赖子)
	BYTE GetMagicCardType(const BYTE bCardData[], BYTE bCardCount,BYTE *bMagicCard, bool isDui3Tong,bool isSpecl ,int speclCardType=0);
	//获取数值
	BYTE GetCardValue(BYTE bCardData) { return bCardData&LOGIC_MASK_VALUE; }
	//获取花色
	BYTE GetCardColor(BYTE bCardData) { return bCardData&LOGIC_MASK_COLOR; }

	//控制函数
public:
	//排列扑克
	void SortCardList(BYTE bCardData[], BYTE bCardCount);
	//混乱扑克
	void RandCardList(BYTE bCardBuffer[], BYTE bBufferCount);
	//删除扑克
	bool RemoveCard(const BYTE bRemoveCard[], BYTE bRemoveCount, BYTE bCardData[], BYTE bCardCount);
	//删除扑克
	bool RemoveCardList(const BYTE cbRemoveCard[], BYTE cbRemoveCount, BYTE cbCardData[], BYTE cbCardCount);

	//逻辑函数
public:
	//是否有赖子
	bool IsHadRoguishCard(const BYTE bCardData[],BYTE bCardCount);
	//逻辑数值
	BYTE GetCardLogicValue(BYTE bCardData);
	//对比扑克
	bool CompareCard(const BYTE bFirstList[], const BYTE bNextList[], BYTE bFirstCount, BYTE bNextCount, bool isDui3Tong = false);

	//内部函数
public:
	//分析扑克
	void AnalysebCardData(const BYTE bCardData[], BYTE bCardCount, tagAnalyseResultLaizi & AnalyseResult);
};

//////////////////////////////////////////////////////////////////////////

#endif