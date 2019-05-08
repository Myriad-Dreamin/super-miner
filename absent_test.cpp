
#define DEBUG
#include "./src/miner_tester.h"
#include "./alter/tools.h"
#include "./src/miner.h"

int main ()
{
	int map_row = 1500, map_col = 2000;
	auto gen = BiNormMinerMapGenerator<int>(map_row, map_col, 100, 10000, 1, 4);
	auto absent_gen = AbsentMinerMapGenerator<int>();
    auto mp = gen.gen();
	auto god_miner = GodMiner<int>("god.csv");
	auto absent_miner = AbsentMiner<int>("absent.csv");
	god_miner.read_map(mp, map_row, map_col);
    absent_miner.read_map(mp, map_row, map_col);
    for (double c = 0; c <= 1.01; c += 0.1) {
        auto absent_mp = absent_gen.gen(gen, c, -1);
        absent_miner.read_absent_map_and_guess(absent_mp, map_row, map_col);
        absent_miner.run(c);
    }
	
}