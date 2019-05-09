
#define DEBUG
#include "./src/miner_tester.h"
#include "./alter/tools.h"
#include "./src/miner.h"

const int repeat_times = 10;
typedef int test_type;
int kstep = 20, kup = 10;

int main ()
{
	int map_row = 100, map_col = 100;
	auto snake_miner = SnakeMiner<test_type>("./miner.csv");
	auto image_miner = ImageMiner<test_type>("./miner.csv");
	auto god_miner = GodMiner<test_type>("./miner.csv");

	long double ret_standard = 0;
	test_type ret_snake[12] = {0};
	test_type ret_image[12][15] = {0};

	for (int cc = 0; cc < repeat_times; cc++) {
		auto gen = BiNormMinerMapGenerator<test_type>(map_row, map_col, 100, 10000, 1, 10000);
		auto mp = gen.gen();
		snake_miner.read_map(mp, map_row, map_col);
		image_miner.read_map(mp, map_row, map_col);
		god_miner.read_map(mp, map_row, map_col);
		// miner.show_map();
		ret_standard += god_miner.run();
		for (int k = 0; k <= kup; k++) {
			ret_snake[k] += snake_miner.run(k * kstep);
			for (int c = 0; c <= 12; c++) {
				ret_image[k][c] += image_miner.run(k * kstep, c * 0.05);
			}
		}

	}

	for (int k = 0; k <= kup; k++) {
		god_miner.block(k * kstep);
		god_miner.block(ret_snake[k]/ret_standard);
		for (int c = 0; c <= 12; c++) {
			god_miner.block(ret_image[k][c]/ret_standard);
		}
		god_miner.newline();
	}

}