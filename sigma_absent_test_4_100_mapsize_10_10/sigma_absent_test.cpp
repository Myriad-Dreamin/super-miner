
#define DEBUG
#define DONOTPRINT

typedef int test_type;

#include "../src/miner_tester.h"
#include "../alter/tools.h"
#include "../src/miner.h"
#include <string>

const int GUESS_CNT = 2;
const int map_row = 10, map_col = 10;
const int repeat_times = 10;
const double unknown_rate = 0.9, sigma_l = 4, sigma_r = 100, step = 4;
const std::string comment = "sigma_between_4_100_guess2times";

int main ()
{

    auto god_miner = GodMiner<test_type>("god_" + comment + ".csv");
    auto absent_miner = AbsentMiner<test_type>("donothing_" + comment + ".csv");
    auto absent_ave = AbsentMiner<test_type>("ave_" + comment + ".csv");
    auto absent_ave2 = AbsentMiner<test_type>("ave2_" + comment + ".csv");
    auto absent_gauss = AbsentMiner<test_type>("gauss_" + comment + ".csv");
    auto absent_gauss2 = AbsentMiner<test_type>("gauss2_" + comment + ".csv");
    auto absent_median = AbsentMiner<test_type>("median_" + comment + ".csv");
    auto absent_gen = AbsentMinerMapGenerator<test_type>();

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

    for (double c = sigma_l; c <= sigma_r; c += step) {
        printf("c%lf\n",c);

        long double ret_standard = 0;
        test_type ret_do_nothing = 0;
        test_type ret_ave = 0;
        test_type ret_ave2 = 0;
        test_type ret_gauss = 0;
        test_type ret_gauss2 = 0;
        test_type ret_median = 0;

        for (int cc = 0; cc < repeat_times; cc++) {
            printf("repeating %d case\n", cc);
            auto gen = BiNormMinerMapGenerator<test_type>(map_row, map_col, 200, 10000, c, c * 4);
            auto mp = gen.gen();

            god_miner.read_map(mp, map_row, map_col);
            absent_miner.read_map(mp, map_row, map_col);
            absent_ave.read_map(mp, map_row, map_col);
            absent_ave2.read_map(mp, map_row, map_col);
            absent_gauss.read_map(mp, map_row, map_col);
            absent_gauss2.read_map(mp, map_row, map_col);
            absent_median.read_map(mp, map_row, map_col);

            god_miner.show_map();
            ret_standard += god_miner.run();

            absent_miner.read_absent_map(absent_gen.gen(gen, unknown_rate, 0), map_row, map_col);
            ret_do_nothing += absent_miner.run();

            auto absent_a = std::move(AbsentMinerMapGenerator<test_type>(absent_gen));
            for (int guess_cnt = 0; guess_cnt < GUESS_CNT; guess_cnt++) {
                absent_ave.read_absent_map(absent_a.guess(self_ave_convolutor), map_row, map_col);
                absent_ave.show_absent_map();
            }
            ret_ave += absent_ave.run();
            puts("------------------------");

            auto absent_b = std::move(AbsentMinerMapGenerator<test_type>(absent_gen));
            for (int guess_cnt = 0; guess_cnt < GUESS_CNT; guess_cnt++) {
                absent_ave2.read_absent_map(absent_b.guess(self_ave2_convolutor), map_row, map_col);
                absent_ave2.show_absent_map();
            }
            ret_ave2 += absent_ave2.run();
            puts("------------------------");

            auto absent_c = std::move(AbsentMinerMapGenerator<test_type>(absent_gen));
            for (int guess_cnt = 0; guess_cnt < GUESS_CNT; guess_cnt++) {
                absent_gauss.read_absent_map(absent_c.guess(self_gauss_convolutor), map_row, map_col);
                absent_gauss.show_absent_map();
            }
            ret_gauss += absent_gauss.run();
            puts("------------------------");

            auto absent_d = std::move(AbsentMinerMapGenerator<test_type>(absent_gen));
            for (int guess_cnt = 0; guess_cnt < GUESS_CNT; guess_cnt++) {
                absent_gauss2.read_absent_map(absent_d.guess(self_gauss2_convolutor), map_row, map_col);
                absent_gauss2.show_absent_map();
            }
            ret_gauss2 += absent_gauss2.run();
            puts("------------------------");

            auto absent_e = std::move(AbsentMinerMapGenerator<test_type>(absent_gen));
            for (int guess_cnt = 0; guess_cnt < GUESS_CNT; guess_cnt++) {
                absent_median.read_absent_map(absent_e.guess(self_median_convolutor), map_row, map_col);
                absent_median.show_absent_map();
            }
            ret_median += absent_median.run();
            puts("---------------~~~~~~~~~~~~~~~~---------------");
            // absent_miner.read_absent_map_and_guess(absent_mp, map_row, map_col);
        }
        absent_miner.block(c);
        absent_miner.block(ret_do_nothing/ret_standard);
        absent_miner.newline();

        absent_ave.block(c);
        absent_ave.block(ret_ave/ret_standard);
        absent_ave.newline();

        absent_ave2.block(c);
        absent_ave2.block(ret_ave2/ret_standard);
        absent_ave2.newline();

        absent_gauss.block(c);
        absent_gauss.block(ret_gauss/ret_standard);
        absent_gauss.newline();

        absent_gauss2.block(c);
        absent_gauss2.block(ret_gauss2/ret_standard);
        absent_gauss2.newline();

        absent_median.block(c);
        absent_median.block(ret_median/ret_standard);
        absent_median.newline();
    }

}
