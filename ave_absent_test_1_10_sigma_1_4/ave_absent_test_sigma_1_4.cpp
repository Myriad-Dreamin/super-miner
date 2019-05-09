
#define DEBUG
#define DONOTPRINT

typedef double test_type;

#include "../src/miner_tester.h"
#include "../alter/tools.h"
#include "../src/miner.h"
#include <string>

const int GUESS_CNT = 2;
const int repeat_times = 10;
const double unknown_rate = 0.9;
const std::string comment = "ave_absent_test_mutiple_1_10_guess2times__";

int main ()
{

    auto god_miner = GodMiner<test_type>("god_" + comment + ".csv");
    auto absent_miner = AbsentMiner<test_type>("donothing_" + comment + ".csv");
    auto absent_ave = AbsentMiner<test_type>("ave_" + comment + ".csv");

    auto absent_gen = AbsentMinerMapGenerator<test_type>();

    KernalMatrix<double>::simple_matrix33 ker_ave_standard;
    ker_ave_standard[0][0] = 1.0/9; ker_ave_standard[0][1] = 1.0/9;  ker_ave_standard[0][2] = 1.0/9;
    ker_ave_standard[1][0] = 1.0/9; ker_ave_standard[1][1] = 1.0/9;  ker_ave_standard[1][2] = 1.0/9;
    ker_ave_standard[2][0] = 1.0/9; ker_ave_standard[2][1] = 1.0/9;  ker_ave_standard[2][2] = 1.0/9;


    long double ret_standard[12] = {0};
    test_type ret_do_nothing[12] = {0};
    test_type ret_ave[12][12] = {0};

    for (int siz = 0; siz < 10; siz++) {
        int map_row = 10 + siz * 10;
        int map_col = 10 + siz * 10;
        for (int cc = 0; cc < repeat_times; cc++) {
            printf("repeating %d case\n", cc);

            auto gen = BiNormMinerMapGenerator<test_type>(map_row, map_col, 200, 10000, 1, 4);
            auto mp = gen.gen();

            god_miner.read_map(mp, map_row, map_col);
            absent_miner.read_map(mp, map_row, map_col);
            absent_ave.read_map(mp, map_row, map_col);

            god_miner.show_map();
            ret_standard[siz] += god_miner.run();

            absent_miner.read_absent_map(absent_gen.gen(gen, unknown_rate, 0), map_row, map_col);
            ret_do_nothing[siz] += absent_miner.run();

            for (int ave_multiple_fac = 0; ave_multiple_fac <= 10; ave_multiple_fac++) {
                KernalMatrix<double>::simple_matrix33 ker_ave;
                ker_ave[0][0] = ker_ave_standard[0][0] * (1 + 0.1 * ave_multiple_fac); ker_ave[0][1] = ker_ave_standard[0][1] * (1 + 0.1 * ave_multiple_fac);  ker_ave[0][2] = ker_ave_standard[0][2] * (1 + 0.1 * ave_multiple_fac);
                ker_ave[1][0] = ker_ave_standard[1][0] * (1 + 0.1 * ave_multiple_fac); ker_ave[1][1] = ker_ave_standard[1][1] * (1 + 0.1 * ave_multiple_fac);  ker_ave[1][2] = ker_ave_standard[1][2] * (1 + 0.1 * ave_multiple_fac);
                ker_ave[2][0] = ker_ave_standard[2][0] * (1 + 0.1 * ave_multiple_fac); ker_ave[2][1] = ker_ave_standard[2][1] * (1 + 0.1 * ave_multiple_fac);  ker_ave[2][2] = ker_ave_standard[2][2] * (1 + 0.1 * ave_multiple_fac);
                auto self_ave_convolutor = map_guesser::self_convolution<double, test_type>(ker_ave);

                auto absent_tmp = std::move(AbsentMinerMapGenerator<test_type>(absent_gen));
                for (int guess_cnt = 0; guess_cnt < GUESS_CNT; guess_cnt++) {
                    absent_ave.read_absent_map(absent_tmp.guess(self_ave_convolutor), map_row, map_col);
                    absent_ave.show_absent_map();
                }
                ret_ave[siz][ave_multiple_fac] += absent_ave.run();
                puts("------------------------");
            }
        }
    }

    absent_ave.block(0);
    for (int ave_multiple_fac = 0; ave_multiple_fac <= 10; ave_multiple_fac++) {
        absent_ave.block(1 + 0.1 * ave_multiple_fac);
    }
    absent_ave.newline();

    for (int siz = 0; siz < 10; siz++) {
        int map_row = 10 + siz * 10;
        int map_col = 10 + siz * 10;
        absent_ave.block(map_row * map_col);
        absent_ave.block(ret_do_nothing[siz]/ret_standard[siz]);
        for (int ave_multiple_fac = 0; ave_multiple_fac <= 10; ave_multiple_fac++) {
            absent_ave.block(ret_ave[siz][ave_multiple_fac]/ret_standard[siz]);
        }
        absent_ave.newline();
    }
}
