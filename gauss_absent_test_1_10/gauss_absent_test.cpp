
#define DEBUG
#define DONOTPRINT

typedef int test_type;

#include "../src/miner_tester.h"
#include "../alter/tools.h"
#include "../src/miner.h"
#include <string>

const int GUESS_CNT = 2;
const int repeat_times = 10;
const double unknown_rate = 0.9;
const std::string comment = "gauss_absent_test_mutiple_1_10_guess2times";

int main ()
{

    auto god_miner = GodMiner<test_type>("god_" + comment + ".csv");
    auto absent_miner = AbsentMiner<test_type>("donothing_" + comment + ".csv");
    auto absent_gauss = AbsentMiner<test_type>("gauss_" + comment + ".csv");

    auto absent_gen = AbsentMinerMapGenerator<test_type>();

    KernalMatrix<double>::simple_matrix33 ker_gauss_standard;
    ker_gauss_standard[0][0] = 0.0625; ker_gauss_standard[0][1] = 0.125;  ker_gauss_standard[0][2] = 0.0625;
    ker_gauss_standard[1][0] = 0.125;  ker_gauss_standard[1][1] = 0.25;   ker_gauss_standard[1][2] = 0.125;
    ker_gauss_standard[2][0] = 0.0625; ker_gauss_standard[2][1] = 0.125;  ker_gauss_standard[2][2] = 0.0625;


    long double ret_standard[12] = {0};
    test_type ret_do_nothing[12] = {0};
    test_type ret_gauss[12][12] = {0};

    for (int siz = 0; siz < 10; siz++) {
        int map_row = 10 + siz * 10;
        int map_col = 10 + siz * 10;
        for (int cc = 0; cc < repeat_times; cc++) {
            printf("repeating %d case\n", cc);

            auto gen = BiNormMinerMapGenerator<test_type>(map_row, map_col, 200, 10000, 4, 100);
            auto mp = gen.gen();

            god_miner.read_map(mp, map_row, map_col);
            absent_miner.read_map(mp, map_row, map_col);
            absent_gauss.read_map(mp, map_row, map_col);

            god_miner.show_map();
            ret_standard[siz] += god_miner.run();

            absent_miner.read_absent_map(absent_gen.gen(gen, unknown_rate, 0), map_row, map_col);
            ret_do_nothing[siz] += absent_miner.run();

            for (int gauss_multiple_fac = 0; gauss_multiple_fac <= 10; gauss_multiple_fac++) {
                KernalMatrix<double>::simple_matrix33 ker_gauss;
                ker_gauss[0][0] = ker_gauss_standard[0][0] * (1 + 0.1 * gauss_multiple_fac); ker_gauss[0][1] = ker_gauss_standard[0][1] * (1 + 0.1 * gauss_multiple_fac);  ker_gauss[0][2] = ker_gauss_standard[0][2] * (1 + 0.1 * gauss_multiple_fac);
                ker_gauss[1][0] = ker_gauss_standard[1][0] * (1 + 0.1 * gauss_multiple_fac); ker_gauss[1][1] = ker_gauss_standard[1][1] * (1 + 0.1 * gauss_multiple_fac);  ker_gauss[1][2] = ker_gauss_standard[1][2] * (1 + 0.1 * gauss_multiple_fac);
                ker_gauss[2][0] = ker_gauss_standard[2][0] * (1 + 0.1 * gauss_multiple_fac); ker_gauss[2][1] = ker_gauss_standard[2][1] * (1 + 0.1 * gauss_multiple_fac);  ker_gauss[2][2] = ker_gauss_standard[2][2] * (1 + 0.1 * gauss_multiple_fac);
                auto self_gauss_convolutor = map_guesser::self_convolution<double, test_type>(ker_gauss);

                auto absent_tmp = std::move(AbsentMinerMapGenerator<test_type>(absent_gen));
                for (int guess_cnt = 0; guess_cnt < GUESS_CNT; guess_cnt++) {
                    absent_gauss.read_absent_map(absent_tmp.guess(self_gauss_convolutor), map_row, map_col);
                    absent_gauss.show_absent_map();
                }
                ret_gauss[siz][gauss_multiple_fac] += absent_gauss.run();
                puts("------------------------");
            }
        }
    }

    absent_gauss.block(0);
    for (int gauss_multiple_fac = 0; gauss_multiple_fac <= 10; gauss_multiple_fac++) {
        absent_gauss.block(1 + 0.1 * gauss_multiple_fac);
    }
    absent_gauss.newline();

    for (int siz = 0; siz < 10; siz++) {
        int map_row = 10 + siz * 10;
        int map_col = 10 + siz * 10;
        absent_gauss.block(map_row * map_col);
        absent_gauss.block(ret_do_nothing[siz]/ret_standard[siz]);
        for (int gauss_multiple_fac = 0; gauss_multiple_fac <= 10; gauss_multiple_fac++) {
            absent_gauss.block(ret_gauss[siz][gauss_multiple_fac]/ret_standard[siz]);
        }
        absent_gauss.newline();
    }
}
