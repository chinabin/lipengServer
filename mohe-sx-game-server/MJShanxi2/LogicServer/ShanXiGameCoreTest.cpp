
#include "ShanXiFeatureToggles.h"

#include <cassert>
#include <iostream>



/*
���һ�ֲ��Ե�д������� ShanXiGameCore �����齫�淨ʵ�ֵĲ���
������һ�����ּ��ɱ�ʾ����һ�����ԣ���Ҫ��ʾ�淨��Χ��Сѡ�Χ�����ơ��߷������㷬�͡��������

11-19�� 21-29Ͳ 31-39�� 41-47��
0-3���λ��
290 0�� 291 1��
180 0���ܻ��߲���
280 0����
171 1��
263 3��
150 0������
250 0���ں�
330 0�ķ����ͷ��� -1��β
310 0�ķ���
666 ��������Զ��ߣ�ֱ����ǰ��ҿ��Բ���
999 ��������Զ��ߣ�ֱ������
444 ������ʼ�������кŲ���
441 ������Щ�淨 -1��β
440 �ų���Щ�淨 -1��β
451 ѡ����Щѡ�� -1��β
450 ��ѡ����Щѡ�� -1��β
449 ������ֹ��
880 0����  881 1����  884���ӵ��� 885���ӵ��� ����ָ���Զ���β


*/
struct ShanXiGameCoreTestParser {
	enum { MAX_PLAYER_COUNT = 4 };

	struct ShanXiGameCoreTestCase {
		std::vector<tile_t> player_assigned[MAX_PLAYER_COUNT];
		std::vector<tile_t> desk_assigned;
		std::vector<tile_t> haozi_assigned;
	};

	std::vector<ShanXiGameCoreTestCase> tests;
	ShanXiGameCoreTestCase *test;
	int const *par_p;
	int const *par_q;

	template <size_t N>
	ShanXiGameCoreTestParser(int const (&data)[N]) {
		parse_data(data, N);
	}
	void parse_data(int const *p, size_t n) {
		par_p = p;
		par_q = p + n;
		try {
			for (;;) {
				if (par_p >= par_q) break;
				int v = *par_p++;
				on_parser_op(v);
			}
		} catch (char const *err) {
			std::cout << "parser error: " << err << std::endl;
		}
	}
	void on_parser_op(int op) {
		switch (op) {
		case 444: return parser_op_new_test();
		case 449: return parser_op_finish_test();
		case 880: case 881: case 882: case 883: return parser_op_assign_player(op % 10);
		}
	}

	void parser_op_new_test() {
		if (test) throw "cannot new test before finish current test";
		tests.push_back(ShanXiGameCoreTestCase());
		test = &tests.back();
	}
	void parser_op_finish_test() {
		if (NULL == test) throw "there is no test to finish";
		test = NULL;
	}
	void check_test_available() {
		if (NULL == test) throw "there is no test to build";
	}
	void read_tiles(std::vector<tile_t> &out) {

	}
	void parser_op_assign_player(int pos) {
		check_test_available();
		std::vector<tile_t> &out = test->player_assigned[pos];
		read_tiles(out);
	}
};


#if defined(_WIN32)
struct ShanXiGameCoreTestStartup {
	ShanXiGameCoreTestStartup() {
		std::cout << "Hello ShanXiGameCoreTestStartup" << std::endl;
		static int const data[] = {
			444, __LINE__, 444, __LINE__,
		};
		ShanXiGameCoreTestParser par(data);
	}
};
static ShanXiGameCoreTestStartup g_ShanXiGameCoreTestStartup;
#endif

