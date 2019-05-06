
#include "miner_tester.h"

int main ()
{
	int map_row = 18, map_col = 20;
	auto gen = BiNormMinerMapGenerator<int>(map_row, map_col, 2, 1000, 1, 4);
	auto mp = gen.gen();
	auto miner = Miner<int>();
	miner.read_map(mp, map_row, map_col);
	miner.show_map();
}