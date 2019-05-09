
#define DEBUG
#define DONOTPRINT

typedef double test_type;

#include "../src/miner_tester.h"
#include "../alter/tools.h"
#include "../src/miner.h"
#include <string>

const int GUESS_CNT = 2;
const int repeat_times = 10;
const double unknown_rate_l = 0.8, unknown_rate_r = 1.01, step = 0.02;
const std::string comment = "guess2times_map_siz_100_10000_rate_8e-2_1";

int main ()
{

    auto god_miner = GodMiner<test_type>();
    auto absent_miner = AbsentMiner<test_type>("do_nothing_" +comment + ".csv");
    auto absent_ave = AbsentMiner<test_type>("ave_" + comment + ".csv");

    auto absent_gen = AbsentMinerMapGenerator<test_type>();

    KernalMatrix<double>::simple_matrix33 ker_ave;
    ker_ave[0][0] = 0.1111111111111; ker_ave[0][1] = 0.1111111111111;  ker_ave[0][2] = 0.1111111111111;
    ker_ave[1][0] = 0.1111111111111; ker_ave[1][1] = 0.1111111111111;  ker_ave[1][2] = 0.1111111111111;
    ker_ave[2][0] = 0.1111111111111; ker_ave[2][1] = 0.1111111111111;  ker_ave[2][2] = 0.1111111111111;


    long double ret_standard[12][12] = {0};
    test_type ret_do_nothing[12][12] = {0};
    test_type ret_ave[12][12] = {0};

    for (int siz = 0; siz < 10; siz++) {
        printf("size %d case\n", siz);
        int map_row = 10 + siz * 10;
        int map_col = 10 + siz * 10;
        for (int rate_fac = 0; rate_fac < 10; rate_fac++) {
            printf("rate %d case\n", rate_fac);
            double unknown_rate = unknown_rate_l + rate_fac * step;
            for (int cc = 0; cc < repeat_times; cc++) {
                printf("repeating %d case\n", cc);

                auto gen = BiNormMinerMapGenerator<test_type>(map_row, map_col, 20, 10000, 10, 100);
                auto mp = gen.gen();


                god_miner.read_map(mp, map_row, map_col);
                absent_miner.read_map(mp, map_row, map_col);
                absent_ave.read_map(mp, map_row, map_col);

                god_miner.show_map();
                printf("show");
                ret_standard[siz][rate_fac] += god_miner.run();

                absent_miner.read_absent_map(absent_gen.gen(gen, unknown_rate, 0), map_row, map_col);
                ret_do_nothing[siz][rate_fac] += absent_miner.run();

                auto self_ave_convolutor = map_guesser::self_convolution<double, test_type>(ker_ave);

                auto absent_tmp = std::move(AbsentMinerMapGenerator<test_type>(absent_gen));
                for (int guess_cnt = 0; guess_cnt < GUESS_CNT; guess_cnt++) {
                    absent_ave.read_absent_map(absent_tmp.guess(self_ave_convolutor), map_row, map_col);
                    absent_ave.show_absent_map();
                }
                ret_ave[siz][rate_fac] += absent_ave.run();
                puts("------------------------");
            }
        }
    }

    absent_miner.block("");
    for (int gauss_multiple_fac = 0; gauss_multiple_fac <= 10; gauss_multiple_fac++) {
        absent_miner.block(unknown_rate_l + step * gauss_multiple_fac);
    }
    absent_miner.newline();

    absent_ave.block("");
    for (int gauss_multiple_fac = 0; gauss_multiple_fac <= 10; gauss_multiple_fac++) {
        absent_ave.block(unknown_rate_l + step * gauss_multiple_fac);
    }
    absent_ave.newline();

    for (int siz = 0; siz < 10; siz++) {
        int map_row = 10 + siz * 10;
        int map_col = 10 + siz * 10;
        absent_miner.block(map_row * map_col);
        for (int gauss_multiple_fac = 0; gauss_multiple_fac <= 10; gauss_multiple_fac++) {
            absent_miner.block(ret_do_nothing[siz][gauss_multiple_fac]/ret_standard[siz][gauss_multiple_fac]);
        }
        absent_miner.newline();
    }

    for (int siz = 0; siz < 10; siz++) {
        int map_row = 10 + siz * 10;
        int map_col = 10 + siz * 10;
        absent_ave.block(map_row * map_col);
        for (int gauss_multiple_fac = 0; gauss_multiple_fac <= 10; gauss_multiple_fac++) {
            absent_ave.block(ret_ave[siz][gauss_multiple_fac]/ret_standard[siz][gauss_multiple_fac]);
        }
        absent_ave.newline();
    }
}
