

#include "malgo.h"




#define TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES 1
//���ڼ�����С���Ƶ�˳�����
struct CombinItem {
	int card[3];
	CombinItem* parent;
	CombinItem* child;
	int nKePos;
	int huns;


	CombinItem() {
		card[3] = { 0 };
		parent = NULL;
		child = NULL;
		nKePos = 999;
		huns = 0;
	}
};

struct minHuns {
	CombinItem* curMinHunsItem = NULL;
	std::vector<CombinItem*> List;
	std::vector<CombinItem*> allList;
	int time;
	unsigned nKePos;

	minHuns() {
		time = 0;
		nKePos = 999;
	}

	~minHuns()
	{
		for (int i = 0; i < allList.size(); i++) {
			if (allList[i]!= NULL) {
				delete allList[i];
				allList[i] = NULL;
			}
		}

		allList.clear();
		List.clear();
	}

	//��ȡ���Ƶ�����
	int  getHunsNum(unsigned* arrFengCard, int arr_len, int& kePos)
	{
		int huns = 0;
		for (int i = 0; i < arr_len; i++)
		{
			if (arrFengCard[i] == 2) {
				huns++;
			}
			if (arrFengCard[i] == 4) {
				huns = huns + 999;
			}

			if (arrFengCard[i] == 1) {
				huns = huns + 2;
			}
			if (arrFengCard[i] == 3) {
				kePos = i;
			}
		}
		return huns;
	}

	//��ȡ˳����ϵ�����
	void  countShunsNumAndMinHunNum(int& hunNum, int& shunNum)
	{
		//��ȡ˳����ϵ�����
		int size = List.size();
		int random = 0;
		if (size > 1) {
			random = rand() % (size - 1);
		};
		hunNum = List[random]->huns;
		CombinItem* parent = List[random]->parent;
		while (parent) {
			shunNum++;
			parent = parent->parent;
		}
		nKePos = List[random]->nKePos;
	}

	//ʣ�µ����Ƿ�����ٻ�ȡ˳���
	bool judgeCanDoCombine(unsigned* arrFengCard, int num)
	{
		int leftFengSum = 0;  //ʣ�¿�����ϵ�����
		for (int i = 0; i < 4; i++) {
			if (arrFengCard[i] > 0)
				leftFengSum++;
		}
		return (num > leftFengSum ? 0 : 1);
	}

	void combine_increase(unsigned* arrFengCard, int start, CombinItem* pItem, int count, const int NUM, const int arr_len)
	{
		int i = 0;
		for (i = start; i < arr_len + 1 - count; i++)
		{
			if (arrFengCard[i] <= 0) {
				continue;
			}
			pItem->card[count - 1] = i;
			if (count - 1 == 0)
			{
				unsigned arrFengCardC[4] = {0,0,0,0};

				for (int k = 0; k < arr_len; k++) {    //������
					arrFengCardC[k] = arrFengCard[k];
				}

				//���ݵõ�����ϼ���ÿ�ַ��Ƶ�����
				for (int m = 0; m < NUM; m++) {
					arrFengCardC[pItem->card[m]]--;
				}

				CombinItem* resultC = new CombinItem();
				allList.push_back(resultC);
				resultC->parent = pItem;
				bool doNext = false;
				doNext = judgeCanDoCombine(arrFengCardC, 3);
				if (doNext) {
					time++;
					combine_increase(arrFengCardC, 0, resultC, 3, 3, arr_len);
				}
				else {
					//�õ����Ƶ�ֵ�����Ҹ������ٻ��Ƽ�¼
					int kePos = 999;
					int huns = getHunsNum(arrFengCardC, arr_len, kePos);
					resultC->huns = huns;
					resultC->nKePos = kePos;
					if (curMinHunsItem == NULL)
					{
						curMinHunsItem = resultC;
						List.push_back(resultC);
					}
					else {
						if (curMinHunsItem->huns > resultC->huns)
						{
							curMinHunsItem = resultC;
							List.clear();
							List.push_back(resultC);
						}
						else if (curMinHunsItem->huns == resultC->huns)
						{
							List.push_back(resultC);
						}
					}
				}
			}
			else
				combine_increase(arrFengCard, i + 1, pItem, count - 1, NUM, arr_len);
		}
		/*
		if (curMinHunsItem == NULL) {
			//�õ����Ƶ�ֵ�����Ҹ������ٻ��Ƽ�¼
			int kePos = 999;
			int huns = getHunsNum(arrFengCard, arr_len, kePos);
			pItem->huns = huns;
			pItem->nKePos = kePos;

			curMinHunsItem = pItem;
			List.push_back(pItem);

		}*/


		//�õ����Ƶ�ֵ�����Ҹ������ٻ��Ƽ�¼
		int kePos = 999;
		int huns = getHunsNum(arrFengCard, arr_len, kePos);
		pItem->huns = huns;
		pItem->nKePos = kePos;
		if (curMinHunsItem != NULL && huns < curMinHunsItem->huns) 
		{
			curMinHunsItem = pItem;
			List.clear();
			List.push_back(pItem);
		}

		if (curMinHunsItem == NULL) 
		{
			curMinHunsItem = pItem;
			List.push_back(pItem);
		}


	}
    //��ȡ���������˳������
	void getShunsNumAndMinHunNum(unsigned* arrFengCard, int size, int& hunNum, int& shunNum) {
		int num = 3;
		CombinItem* curCombinItem = new CombinItem();
		allList.push_back(curCombinItem);
		combine_increase(arrFengCard, 0, curCombinItem, num, num, size);
		//�����������εĴ�����2,3��3����С��Ӧ����1
		unsigned ArrFengCardCopy[4] = { 0,0,0,0 };
		for (int k = 0; k < size; k++) {    //������
			ArrFengCardCopy[k] = arrFengCard[k];
		}

		for (int i = 0; i < size; i++) {
			if (ArrFengCardCopy[i] > 3) {
				ArrFengCardCopy[i] = ArrFengCardCopy[i] - 3;

				unsigned tempArrFengCard[4] = { 0,0,0,0 };
				for (int k = 0; k < size; k++) {    //������
					tempArrFengCard[k] = ArrFengCardCopy[k];
				}

				CombinItem* tempCurCombinItem = new CombinItem();
				allList.push_back(tempCurCombinItem);
				combine_increase(tempArrFengCard, 0, tempCurCombinItem, num, num, size);
			}
		}

		//������������˳������
		countShunsNumAndMinHunNum(hunNum, shunNum);
	}
};


// ����ֻ�ܳɿ��ӣ����Ժܼ�
static unsigned tile_block_min_huns__zipai(unsigned const p[9], unsigned head, unsigned tail) {
	unsigned min_hun = 0;
	for (unsigned i = head; i <= tail; ++i) {
		// 4��ͬ�������ƣ�������˳�ӣ�����һ�����ӣ���ʣһ�ţ�������Ҳ������ȥȡ���ֵ�����Կ϶�������
		if (TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES && p[i] == 4) return 999; 
		min_hun += (3 - (p[i] % 3)) % 3;
	 
	}
	return min_hun;
}

static int  tile_min_huns_num_for_HongTong(unsigned const p[9], int& hunsNum, int& shunNum, unsigned jiang_pos, tile_t last_tile, HuBranch& branch);
// �鶴���Ƽ���˳����Ԫ��������С��������
// ����jiang_pos �ǳ����佫�Ե�λ�ã��������������
unsigned tile_min_huns_for_hongtongwangpai_zipai_sanyuan_sanfeng(unsigned const p[9], unsigned head, unsigned tail, unsigned jiang_pos, tile_t last_tile, HuBranch & branch) {
	unsigned min_hun = 0;
	for (unsigned i = head; i <= tail; ++i) {
		// 4��ͬ�������ƣ�������˳�ӣ�����һ�����ӣ���ʣһ�ţ�������Ҳ������ȥȡ���ֵ�����Կ϶�������
		if (TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES && p[i] >= 5) return 999;		
	}
	int shunNum;			
	int hunsNum;
	tile_min_huns_num_for_HongTong(p, hunsNum, shunNum, jiang_pos, last_tile, branch);
	min_hun = hunsNum;
	return min_hun;
}



static int  tile_min_huns_num_for_HongTong(unsigned const p[9], int& hunsNum, int& shunNum, unsigned jiang_pos, tile_t last_tile, HuBranch & branch)
{
	unsigned arrFengCard[4] = { 0 };
	for (int i = 0; i < 4; i++) {
		arrFengCard[i] = p[i];
	}


	minHuns tempMinHuns1;
	int fengHunsNum = 0;
	int fengShunNum = 0;
	//LLOG_DEBUG(" jiang_pos = %d ", jiang_pos);
	//LLOG_DEBUG(" arrFengCard = %d  %d  %d  %d ", arrFengCard[0], arrFengCard[1], arrFengCard[2], arrFengCard[3]);
	if (jiang_pos<4 && jiang_pos >=0 && arrFengCard[jiang_pos] == 3) {
		fengHunsNum = 999;
		//LLOG_DEBUG("����λ�ò�����3�� �ı���ֵ fengHunsNum = %d  ", fengHunsNum);
	}
	else {
		tempMinHuns1.getShunsNumAndMinHunNum(arrFengCard, 4, fengHunsNum, fengShunNum);
	}
	//LLOG_DEBUG("fengHunsNum = %d   fengShunNum = %d ", fengHunsNum, fengShunNum);



	minHuns tempMinHuns2;
	int yunHunsNum = 0;
	int yunShunNum = 0;
	unsigned arrYunCard[3] = { 0 };
	for (int i = 0; i < 3; i++) {
		arrYunCard[i] = p[4+i];
	}

	//LLOG_DEBUG(" arrYunCard = %d  %d  %d ", arrYunCard[0], arrYunCard[1], arrYunCard[2]);

	if (jiang_pos != 999 && jiang_pos >= 4 && p[jiang_pos] >= 3)
	{
		yunHunsNum = 999;
	}
	else {
		tempMinHuns2.getShunsNumAndMinHunNum(arrYunCard, 3, yunHunsNum, yunShunNum);
	}

	//LLOG_DEBUG(" yunHunsNum = %d  yunShunNum = %d  ", yunHunsNum, yunShunNum);


	hunsNum = fengHunsNum + yunHunsNum;
	shunNum = fengShunNum + yunShunNum;
	for (int i = 0; i < fengShunNum;i++)
	{
		branch.add(HunItem(HUN0_SHUN_SanFeng, 41));
	}
	for (int i = 0; i < yunShunNum; i++)
	{
		branch.add(HunItem(HUN0_SHUN_SanYuan, last_tile));
	}
	//LLOG_DEBUG("branch.size = %d ",branch.size());

	return hunsNum;
}




// �������ԣ��о���һЩ���ӣ����ֻ��ǵõݹ�ȥ�⣬ȡ��Сֵ
static unsigned tile_block_min_huns__shupai(unsigned p[9], unsigned head, unsigned tail, unsigned jiang_pos) {
	unsigned min_hun = 0;
	// ����ʼ�˿�ʼ��
	for (unsigned i = head; i <= tail; ++i) {
		// �Ȱ����λ���ϵ����еĿ�������
		// ���Է�֤Ϊʲô��ô����������Щ�����ܽ�ʡ�����ƣ������ǿ϶��Ǻͺ��������˳���ã����з����������
		//   ����1��˳�ӣ��鲻�鶼��Ҫһ�������ƣ�����Ļ��������������ƻ����ܸ����������˳����
		//   ����2��˳�ӣ���Ļ���������Ӿ�ȱ�ˣ�Ҳ�ò������ư�������Ļ�������Ļ����ܸ����������˳����
		//   ����3��˳�ӣ���Ҳ������3�����Ӱ���������Ҫ����������
		p[i] = p[i] % 3;
		// Ȼ�����p[i]����0��1��2���������
		if (0 == p[i]) continue; // ���λ�ÿ��ˣ�û����
		unsigned has1 = i + 1 <= tail ? p[i + 1] : 0;
		unsigned has2 = i + 2 <= tail ? p[i + 2] : 0;
		if (1 == p[i]) {
			// ��ʣһ���ƣ������ȥ��������˳�ӣ���Ȼ�ø�����������������
			// �˴�֮�䶼�ǽ�����ģ�������ĩ�˺���û���ˣ���Ȼ��ಹһ�������ƿ϶������˳��
			if (has1 && has2) {
				// ����˳�ӣ��Ǿ�������
				--p[i + 1];
				--p[i + 2];
			} else if (has1 || has2) {
				// ��һ�������ƾ�����˳����
				if (has1) --p[i + 1];
				else --p[i + 2];
				min_hun += 1;
			} else {
				// ��Ȼ���˷�������������
				p[i] = 0;
				min_hun += 2;
			}
		} else {
			// ��ʣ�����ƣ���1���������ܳɿ��ӣ�Ҳ���Ժͺ������2��˳�ӣ�Ҳ���Ժͺ������1��˳��
			// �����ֽ����ݹ��㣬ȡ��Сֵ
			// �ȿ�2��˳�����ַ�֧�ģ��л�����ǰ������
			if (has1 >= 2 && has2 >= 2) {
				unsigned tmp[9];
				memcpy(tmp, p, sizeof(tmp));
				tmp[i] = 0;
				tmp[i + 1] -= 2;
				tmp[i + 2] -= 2;
				unsigned needed_hun__shunzi2_branch = tile_block_min_huns__shupai(tmp, i + 1, tail, jiang_pos);
				if (needed_hun__shunzi2_branch <= 1) {
					min_hun += needed_hun__shunzi2_branch;
					return min_hun;
				}
				// �Լ���1�������Ƴɿ��Ӱ�
				p[i] = 0;
				unsigned needed_hun__kezi_brance;
				// �ڽ���λ�þͲ����������Ʋ�������
				if (TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES && i == jiang_pos) needed_hun__kezi_brance = 999;
				else needed_hun__kezi_brance = 1 + tile_block_min_huns__shupai(p, i + 1, tail, jiang_pos);
				min_hun += std::min(needed_hun__kezi_brance, needed_hun__shunzi2_branch);
				return min_hun;
			} else if (has1 >= 1 && has2 >= 1) {
				unsigned tmp[9];
				memcpy(tmp, p, sizeof(tmp));
				tmp[i] -= 1;
				tmp[i + 1] -= 1;
				tmp[i + 2] -= 1;
				unsigned needed_hun__shunzi1_branch = tile_block_min_huns__shupai(tmp, i, tail, jiang_pos);
				if (needed_hun__shunzi1_branch <= 1) {
					min_hun += needed_hun__shunzi1_branch;
					return min_hun;
				}
				// �Լ���1�������Ƴɿ��Ӱ�
				p[i] = 0;
				unsigned needed_hun__kezi_brance;
				// �ڽ���λ�þͲ����������Ʋ�������
				if (TILE_BLOCK_MIN_HUNS__CHECK_AVAILABLE_TILES && i == jiang_pos) needed_hun__kezi_brance = 999;
				else needed_hun__kezi_brance = 1 + tile_block_min_huns__shupai(p, i + 1, tail, jiang_pos);
				min_hun += std::min(needed_hun__kezi_brance, needed_hun__shunzi1_branch);
				return min_hun;
			} else {
				min_hun += 1;
				// �൱��β�ݹ�
			}
		}
	}
	return min_hun;
}



// ����Ԫ�ر�ʾ���λ�����м����ظ����ƣ�����1��9��9��λ��
unsigned malgo_min_huns(unsigned const p[9], bool is_zipai, unsigned jiang_pos) {
	// ���ƾͲ��÷ֿ�ȥ����
	if (is_zipai) return tile_block_min_huns__zipai(p, 0, 8);
	// ���ư�����ֿ���Ϊ�˽��ͷ�֧��ϣ��ɳ˷���ϵ��ɼӷ���ϵ
	unsigned min_hun = 0;
	// ��ֳ�block��block֮������Ҫ��2����λ
	for (unsigned i = 0; i <= 8; ) {
		if (0 == p[i]) {
			++i;
		} else {
			unsigned j = i;
			unsigned block[9] = { 0 };
			// �ҵ�block��ĩβ [i, j]
			for (;;) {
				block[j] = p[j];
				if (8 == j) break;
				if (0 == p[j + 1]) {
					if (7 == j) break;
					if (0 == p[j + 2]) break;
				}
				++j;
			}
			min_hun += tile_block_min_huns__shupai(block, i, j, jiang_pos);
			i = j + 3;
		}
	}
	return min_hun;
}

