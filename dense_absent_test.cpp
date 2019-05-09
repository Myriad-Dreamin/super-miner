
#define DEBUG
#define DONOTPRINT

#include "./src/miner_tester.h"
#include "./alter/tools.h"
#include "./src/miner.h"

int main ()
{
	auto absent_gen = AbsentMinerMapGenerator<int>();
	auto god_miner = GodMiner<int>("god.csv");
	auto absent_miner = AbsentMiner<int>("god.csv");
	auto absent_ave = AbsentMiner<int>("ave.csv");
	auto absent_ave2 = AbsentMiner<int>("ave2.csv");
	auto absent_gauss = AbsentMiner<int>("gauss.csv");
	auto absent_gauss2 = AbsentMiner<int>("gauss2.csv");
	auto absent_gauss3 = AbsentMiner<int>("gauss3.csv");
	auto absent_median = AbsentMiner<int>("median.csv");
    god_miner.show_map();
    auto ave = 1.0/9;
    KernalMatrix<double>::simple_matrix33 ker_ave;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            ker_ave[i][j] = ave;
        }
    }

    auto ave2 = 1.5/9;
    KernalMatrix<double>::simple_matrix33 ker_ave2;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            ker_ave2[i][j] = ave2;
        }
    }

    KernalMatrix<double>::simple_matrix33 ker_gauss;
    ker_gauss[0][0] = 0.0625; ker_gauss[0][1] = 0.125;  ker_gauss[0][2] = 0.0625;
    ker_gauss[1][0] = 0.125;  ker_gauss[1][1] = 0.25;   ker_gauss[1][2] = 0.125;
    ker_gauss[2][0] = 0.0625; ker_gauss[2][1] = 0.125;  ker_gauss[2][2] = 0.0625;

    KernalMatrix<double>::simple_matrix33 ker_gauss2;
    ker_gauss2[0][0] = ker_gauss[0][0] * 1.1; ker_gauss2[0][1] = ker_gauss[0][1] * 1.1;  ker_gauss2[0][2] = ker_gauss[0][2] * 1.1;
    ker_gauss2[1][0] = ker_gauss[1][0] * 1.1; ker_gauss2[1][1] = ker_gauss[1][1] * 1.1;  ker_gauss2[1][2] = ker_gauss[1][2] * 1.1;
    ker_gauss2[2][0] = ker_gauss[2][0] * 1.1; ker_gauss2[2][1] = ker_gauss[2][1] * 1.1;  ker_gauss2[2][2] = ker_gauss[2][2] * 1.1;

    KernalMatrix<double>::simple_matrix33 ker_gauss3;
    ker_gauss3[0][0] = ker_gauss[0][0] * 4; ker_gauss3[0][1] = ker_gauss[0][1] * 4;  ker_gauss3[0][2] = ker_gauss[0][2] * 4;
    ker_gauss3[1][0] = ker_gauss[1][0] * 4; ker_gauss3[1][1] = ker_gauss[1][1] * 4;  ker_gauss3[1][2] = ker_gauss[1][2] * 4;
    ker_gauss3[2][0] = ker_gauss[2][0] * 4; ker_gauss3[2][1] = ker_gauss[2][1] * 4;  ker_gauss2[2][2] = ker_gauss[2][2] * 4;

    auto self_ave_convolutor = map_guesser::self_convolution<double, int>(ker_ave);
    auto self_ave2_convolutor = map_guesser::self_convolution<double, int>(ker_ave2);
    auto self_gauss_convolutor = map_guesser::self_convolution<double, int>(ker_gauss);
    auto self_gauss2_convolutor = map_guesser::self_convolution<double, int>(ker_gauss2);
    auto self_gauss3_convolutor = map_guesser::self_convolution<double, int>(ker_gauss3);
    auto self_median_convolutor = map_guesser::self_convolution_nonlinear<int>(filter_33::median_filter<int>);
    
    for (int map_row = 1000; map_row >= 100; map_row -= 100) {
        double c = 0.9;
        int map_col = map_row + 500;
        auto gen = BiNormMinerMapGenerator<int>(map_row, map_col, 3000, 7000, 1, 4);
        auto mp = gen.gen();
        god_miner.read_map(mp, map_row, map_col);
        absent_miner.read_map(mp, map_row, map_col);
        absent_ave.read_map(mp, map_row, map_col);
        absent_ave2.read_map(mp, map_row, map_col);
        absent_gauss.read_map(mp, map_row, map_col);
        absent_gauss2.read_map(mp, map_row, map_col);
        absent_gauss3.read_map(mp, map_row, map_col);
        absent_median.read_map(mp, map_row, map_col);
        printf("map_row\n", map_row);
        absent_miner.read_absent_map(absent_gen.gen(gen, c, 0), map_row, map_col);
        god_miner.run();
        absent_miner.run();
        auto absent_a = std::move(AbsentMinerMapGenerator<int>(absent_gen));
        for (int guess_cnt = 0; guess_cnt < 3; guess_cnt++) {
            absent_ave.read_absent_map(absent_a.guess(self_ave_convolutor), map_row, map_col);
            absent_ave.show_absent_map();
            absent_ave.run();
        }
        puts("------------------------");
        auto absent_b = std::move(AbsentMinerMapGenerator<int>(absent_gen));
        for (int guess_cnt = 0; guess_cnt < 3; guess_cnt++) {
            absent_ave2.read_absent_map(absent_b.guess(self_ave2_convolutor), map_row, map_col);
            absent_ave2.show_absent_map();
            absent_ave2.run();
        }
        puts("------------------------");
        auto absent_c = std::move(AbsentMinerMapGenerator<int>(absent_gen));
        for (int guess_cnt = 0; guess_cnt < 3; guess_cnt++) {
            absent_gauss.read_absent_map(absent_c.guess(self_gauss_convolutor), map_row, map_col);
            absent_gauss.show_absent_map();
            absent_gauss.run();
        }
        puts("------------------------");
        auto absent_d = std::move(AbsentMinerMapGenerator<int>(absent_gen));
        for (int guess_cnt = 0; guess_cnt < 3; guess_cnt++) {
            absent_gauss2.read_absent_map(absent_d.guess(self_gauss2_convolutor), map_row, map_col);
            absent_gauss2.show_absent_map();
            absent_gauss2.run();
        }
        puts("------------------------");
        auto absent_e = std::move(AbsentMinerMapGenerator<int>(absent_gen));
        for (int guess_cnt = 0; guess_cnt < 3; guess_cnt++) {
            absent_gauss3.read_absent_map(absent_d.guess(self_gauss3_convolutor), map_row, map_col);
            absent_gauss3.show_absent_map();
            absent_gauss3.run();
        }
        puts("------------------------");
        auto absent_f = std::move(AbsentMinerMapGenerator<int>(absent_gen));
        for (int guess_cnt = 0; guess_cnt < 3; guess_cnt++) {
            absent_median.read_absent_map(absent_e.guess(self_median_convolutor), map_row, map_col);
            absent_median.show_absent_map();
            absent_median.run();
        }
        puts("---------------~~~~~~~~~~~~~~~~---------------");
        // absent_miner.read_absent_map_and_guess(absent_mp, map_row, map_col);
    }
	
}