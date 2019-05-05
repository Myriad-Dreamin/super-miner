
# ifndef SUPER_MINER_H
# define SUPER_MINER_H



#include <iostream>
#include <cstring>
#include <stdexcept>
#include "../alter/generate.h"
#include "../alter/tester.h"
#include "local_tools.h"


template<typename ArrType>
class MinerMapGenerator
{
private:
    int map_row, map_col;
    int mne_cnt, mne_siz;
    double map_sil, map_sir;
    BiDistributionGenerate<ArrType> gen;
    ArrType **space;
public:
    MinerMapGenerator(
        const int row, const int col,
        const int mine_count, const int mine_size,
        const double sigmaL, const double sigmaR,
        const unsigned long long your_seed
    )
    {
        map_row = row;
        map_col = col;
        mne_cnt = mine_count;
        mne_siz = mine_size;
        map_sil = sigmaL;
        map_sir = sigmaR;


        gen = BiDistributionGenerate<int>(mine_count, your_seed);
        space = new *ArrType[row];
        for (int i = 0; i < row; i++) {
            space[i] = new ArrType[col]
        }
    }

    ~MinerMapGenerator()
    {
        for (int i = 0; i < map_row; i++) {
            delete[] space[i];
        }
        delete[] space;
    }

    const ArrType **gen()
    {
        for (int i = 0; i < map_row; i++) {
            memset(space[i], 0, sizeof(ArrType) * map_col);
        }

        for (int i = 0; i < map_cnt; i++) {
            int x = rand_uint() % map_row, y = rand_uint() % map_col, z = rand_uint() % mine_size;

            while(z--) {

            }
        }
        return space;
    }
};


class Miner {
private:

public:
    Miner() {
        
    }
};


# endif