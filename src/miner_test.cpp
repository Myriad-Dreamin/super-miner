
#include "miner_tester.h"
#include "local_tools.h"
#include "miner.h"

int main ()
{
	int map_row = 200, map_col = 300;
	auto gen = BiNormMinerMapGenerator<int>(map_row, map_col, 100, 10000, 1, 4);
	auto mp = gen.gen();
	auto miner = Miner<int>();
	miner.read_map(mp, map_row, map_col);
	// miner.show_map();

	for (int k = 1; k <= map_row + map_col; k+=10) {
		snake_run::solve<int>(mp, map_row, map_col, k);
	}

	for (int k = 1; k <= map_row + map_col; k+=10) {
		for (double c = 0; c <= 0.6; c += 0.05) {
			image_run::solve<int>(mp, map_row, map_col, k, c);
		}
	}

	auto dp = require_two_dimensional_space<int>(map_row, map_col);
	god_run::solve(mp, map_row, map_col, dp);
	release_two_dimensional_space<int>(&dp, map_row);
}