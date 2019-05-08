
#define DEBUG
#include "./src/miner_tester.h"
#include "./alter/tools.h"
#include "./src/miner.h"

int main ()
{
	int map_row = 200, map_col = 300;
	auto gen = BiNormMinerMapGenerator<int>(map_row, map_col, 100, 10000, 1, 4);
	auto mp = gen.gen();
	auto snake_miner = SnakeMiner<int>("./snake.csv");
	auto image_miner = ImageMiner<int>("./image.csv");
	auto god_miner = GodMiner<int>("god.csv");
	snake_miner.read_map(mp, map_row, map_col);
	image_miner.read_map(mp, map_row, map_col);
	god_miner.read_map(mp, map_row, map_col);
	// miner.show_map();

	for (int k = 0; k <= map_row + map_col; k+=50) {
		snake_miner.run(k);
	}

	for (int k = 0; k <= map_row + map_col; k+=50) {
		for (double c = 0; c <= 0.6; c += 0.05) {
			image_miner.run(k, c);
		}
	}

	god_miner.run();
}