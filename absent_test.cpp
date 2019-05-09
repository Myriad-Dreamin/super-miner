
#define DEBUG
#define DONOTPRINT

#include "./src/miner_tester.h"
#include "./alter/tools.h"
#include "./src/miner.h"

int main ()
{
	int map_row = 1000, map_col = 1500;

    auto god_miner = GodMiner<int>("god.csv");
    auto absent_miner = AbsentMiner<int>("god.csv");
    auto absent_ave = AbsentMiner<int>("ave.csv");
    auto absent_ave2 = AbsentMiner<int>("ave2.csv");
    auto absent_gauss = AbsentMiner<int>("gauss.csv");
    auto absent_gauss2 = AbsentMiner<int>("gauss2.csv");
    auto absent_median = AbsentMiner<int>("median.csv");
    auto absent_gen = AbsentMinerMapGenerator<int>();
    for(int cc = 0; cc < 10; cc++) {
        auto gen = BiNormMinerMapGenerator<int>(map_row, map_col, 200, 100000, 1, 4);
        auto mp = gen.gen();
        god_miner.read_map(mp, map_row, map_col);
        absent_miner.read_map(mp, map_row, map_col);
        absent_ave.read_map(mp, map_row, map_col);
        absent_ave2.read_map(mp, map_row, map_col);
        absent_gauss.read_map(mp, map_row, map_col);
        absent_gauss2.read_map(mp, map_row, map_col);
        absent_median.read_map(mp, map_row, map_col);
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

        auto self_ave_convolutor = map_guesser::self_convolution<double, int>(ker_ave);
        auto self_ave2_convolutor = map_guesser::self_convolution<double, int>(ker_ave2);
        auto self_gauss_convolutor = map_guesser::self_convolution<double, int>(ker_gauss);
        auto self_gauss2_convolutor = map_guesser::self_convolution<double, int>(ker_gauss2);
        auto self_median_convolutor = map_guesser::self_convolution_nonlinear<int>(filter_33::median_filter<int>);
        double standard_ret = god_miner.run();
        for (double c = 0.9; c <= 1.01; c += 0.01) {
                printf("c%lf\n",c);
                absent_miner.read_absent_map(absent_gen.gen(gen, c, 0), map_row, map_col);
                absent_miner.block(absent_miner.run()/standard_ret);
                auto absent_a = std::move(AbsentMinerMapGenerator<int>(absent_gen));
                absent_ave.block(c);
                for (int guess_cnt = 0; guess_cnt < 3; guess_cnt++) {
                    absent_ave.read_absent_map(absent_a.guess(self_ave_convolutor), map_row, map_col);
                    absent_ave.show_absent_map();
                    absent_ave.block(absent_ave.run()/standard_ret);
                }
                absent_ave.newline();
                puts("------------------------");
                absent_ave2.block(c);
                auto absent_b = std::move(AbsentMinerMapGenerator<int>(absent_gen));
                for (int guess_cnt = 0; guess_cnt < 3; guess_cnt++) {
                    absent_ave2.read_absent_map(absent_b.guess(self_ave2_convolutor), map_row, map_col);
                    absent_ave2.show_absent_map();
                    absent_ave2.block(absent_ave2.run()/standard_ret);
                }
                absent_ave2.newline();
                puts("------------------------");
                absent_gauss.block(c);
                auto absent_c = std::move(AbsentMinerMapGenerator<int>(absent_gen));
                for (int guess_cnt = 0; guess_cnt < 3; guess_cnt++) {
                    absent_gauss.read_absent_map(absent_c.guess(self_gauss_convolutor), map_row, map_col);
                    absent_gauss.show_absent_map();
                    absent_gauss.block(absent_gauss.run()/standard_ret);
                }
                absent_gauss.newline();
                puts("------------------------");
                absent_gauss2.block(c);
                auto absent_d = std::move(AbsentMinerMapGenerator<int>(absent_gen));
                for (int guess_cnt = 0; guess_cnt < 3; guess_cnt++) {
                    absent_gauss2.read_absent_map(absent_d.guess(self_gauss2_convolutor), map_row, map_col);
                    absent_gauss2.show_absent_map();
                    absent_gauss2.block(absent_gauss2.run()/standard_ret);
                }
                absent_gauss2.newline();
                puts("------------------------");
                absent_median.block(c);
                auto absent_e = std::move(AbsentMinerMapGenerator<int>(absent_gen));
                for (int guess_cnt = 0; guess_cnt < 3; guess_cnt++) {
                    absent_median.read_absent_map(absent_e.guess(self_median_convolutor), map_row, map_col);
                    absent_median.show_absent_map();
                    absent_median.block(absent_median.run()/standard_ret);
                }
                absent_median.newline();
                puts("---------------~~~~~~~~~~~~~~~~---------------");
                // absent_miner.read_absent_map_and_guess(absent_mp, map_row, map_col);
        }
    }
	
}