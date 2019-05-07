
# ifndef SUPER_MINER_H
# define SUPER_MINER_H


#include <iostream>
#include <cstring>
#include <stdexcept>
#include <functional>
#include <iomanip>
#include "miner.h"
#include "../alter/generate.h"
#include "../alter/tester.h"
#include "local_tools.h"


template<typename ArrType>
class BiNormMinerMapGenerator
{
private:
    int map_row, map_col;
    int mine_cnt, mine_siz;
    double sigl, sigr;
    bivari::functor<int>::functory gen_functory;
    ArrType **space;
public:
    BiNormMinerMapGenerator()
    {
        map_row = map_col = 0;
        mine_cnt = mine_siz = 0;
        sigl = sigr = 0;
        space = nullptr;
    }

    BiNormMinerMapGenerator(
        const int row, const int col,
        const int mine_count, const int mine_size,
        const double sigmaL, const double sigmaR
    ) {
        map_row = row;
        map_col = col;
        mine_cnt = mine_count;
        mine_siz = mine_size;
        sigl = sigmaL;
        sigr = sigmaR;

        gen_functory = bivari::normal_distribution::functory_range<int>(sigmaL, sigmaR);

        space = new ArrType*[map_row];
        for (int i = 0; i < map_row; i++) {
            space[i] = new ArrType[map_col];
        }
    }

    BiNormMinerMapGenerator(const BiNormMinerMapGenerator &rg) {
        map_row = rg.map_row;
        map_col = rg.map_col;
        mine_cnt = rg.mine_cnt;
        mine_siz = rg.mine_siz;
        sigl = rg.sigl;
        sigr = rg.sigr;

        gen_functory = bivari::normal_distribution::functory_range<int>(rg.sigl, rg.sigr);

        space = new ArrType*[rg.map_row];
        for (int i = 0; i < rg.map_row; i++) {
            space[i] = new ArrType[rg.map_col];
        }
    }

    BiNormMinerMapGenerator(BiNormMinerMapGenerator &&rg) {
        map_row = rg.map_row;
        map_col = rg.map_col;
        mine_cnt = rg.mine_cnt;
        mine_siz = rg.mine_siz;
        sigl = rg.sigl;
        sigr = rg.sigr;

        gen_functory = std::move(rg.gen_functory);

        space = rg.space;
        rg.space = nullptr;
    }

    ~BiNormMinerMapGenerator()
    {
        if (space == nullptr) {
            return;
        }

        for (int i = 0; i < map_row; i++) {
            delete[] space[i];
        }
        delete[] space;
    }

    const ArrType **gen(std::function<ArrType()> rand_generator)
    {
        if (space == nullptr) {
            return nullptr;
        }

        int x,y,z;
        std::pair<int, int> ret;

        for (int i = 0; i < map_row; i++) {
            memset(space[i], 0, sizeof(ArrType) * map_col);
        }

        for (int i = 0; i < mine_cnt; i++) {
            x = rand_uint() % map_row, y = rand_uint() % map_col, z = rand_uint() % mine_siz;
            auto gen = gen_functory(x, y);
            while(z--) {
                ret = gen();
                if (
                    (0 <= ret.first && ret.first < map_row) &&
                    (0 <= ret.second && ret.second < map_col)
                ) {
                    space[ret.first][ret.second] = rand_generator();
                }
            }
        }
        return space;
    }

    const ArrType **gen()
    {
        if (space == nullptr) {
            return nullptr;
        }

        int x, y, z;
        std::pair<int, int> ret;

        for (int i = 0; i < map_row; i++) {
            memset(space[i], 0, sizeof(ArrType) * map_col);
        }

        for (int i = 0; i < mine_cnt; i++) {
            x = rand_uint() % map_row;
            y = rand_uint() % map_col;
            // std::cout << "randcenter " << x << " " << y << std::endl;
            z = rand_uint() % mine_siz;
            auto gen = gen_functory(x, y);
            while(z--) {
                ret = gen();
                // std::cout << "randgen " << ret.first << " " << ret.second << std::endl;
                if (
                    (0 <= ret.first && ret.first < map_row) &&
                    (0 <= ret.second && ret.second < map_col)
                ) {
                    space[ret.first][ret.second]++;
                }
            }
        }
        return const_cast<const ArrType **>(space);
    }
};


template<typename ArrType>
class Miner: public Tester
{
protected:
    int map_row, map_col;
    const ArrType **space;
    std::string self_path;
public:
    Miner(): Tester()
    {
        self_path = "";
        map_row = map_col = 0;
        ArrType ***as_assign = const_cast<ArrType ***>(&space);
        *as_assign = nullptr;
    }

    Miner(const std::string &file_path): Tester(file_path)
    {
        self_path = file_path;
        map_row = map_col = 0;
        space = nullptr;
    }

    Miner(const Miner &rg)
    {
        if (this == &rg) {
            return;
        }
        Tester::close();
        if (rg.self_path != "") {
            Tester::open(rg.self_path);
        }

        map_row = rg.map_row;
        map_col = rg.map_col;
        self_path = rg.self_path;
        ArrType ***as_assign = const_cast<ArrType ***>(&space);
        *as_assign = const_cast<ArrType**>(rg.space);
    }

    Miner(Miner &&rg)
    {
        if (this == &rg) {
            return;
        }
        Tester::close();
        map_row = rg.map_row;
        map_col = rg.map_col;
        
        if (rg.self_path != "") {
            Tester::open(rg.self_path);
        }
        rg.close();
        self_path = std::move(rg.self_path);
        ArrType ***as_assign = const_cast<ArrType ***>(&space);
        *as_assign = const_cast<ArrType**>(rg.space);
    }
    
    void read_map(const ArrType **out_space, int map_row, int map_col)
    {
        map_row = map_row; map_col = map_col;
        ArrType ***as_assign = const_cast<ArrType ***>(&space);
        *as_assign = const_cast<ArrType **>(out_space);
    }

    void show_map()
    {
        for (int i = 0; i < map_row; i++) {
            for (int j = 0; j < map_col; j++) {
                std::cout << std::setw(2) << space[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }
};

template<typename ArrType>
class SnakeMiner: public Miner<ArrType>
{
public:
    SnakeMiner(): Miner<ArrType>() {}

    SnakeMiner(const std::string &file_path): Miner<ArrType>(file_path) {}
    
    void run(int k)
    {
        ArrType ret = 0;
        ArrType **dp = require_two_dimensional_space<ArrType>(Miner<ArrType>::map_row, Miner<ArrType>::map_col);
        ArrType **sig = require_two_dimensional_space<ArrType>(Miner<ArrType>::map_row, Miner<ArrType>::map_col);
        Tester::run(snake_run::bind_variables(
            ret, dp, sig,
            Miner<ArrType>::space,
            Miner<ArrType>::map_row,
            Miner<ArrType>::map_col,
            k
        ));
        Tester::show();
        Tester::settle_clock();
        release_two_dimensional_space<ArrType>(&dp, Miner<ArrType>::map_row);
        release_two_dimensional_space<ArrType>(&sig, Miner<ArrType>::map_row);
    }
};

template<typename ArrType>
class ImageMiner: public Miner<ArrType>
{
public:
    ImageMiner(): Miner<ArrType>() {}

    ImageMiner(const std::string &file_path): Miner<ArrType>(file_path) {}
    
    void run(int k, double c)
    {
        ArrType ret = 0;
        ArrType **dp = require_two_dimensional_space<ArrType>(Miner<ArrType>::map_row, Miner<ArrType>::map_col);
        ArrType **sig = require_two_dimensional_space<ArrType>(Miner<ArrType>::map_row, Miner<ArrType>::map_col);
        Tester::run(image_run::bind_variables(
            ret, dp, sig,
            Miner<ArrType>::space,
            Miner<ArrType>::map_row,
            Miner<ArrType>::map_col,
            k, c
        ));
        Tester::show();
        Tester::settle_clock();
        release_two_dimensional_space<ArrType>(&dp, Miner<ArrType>::map_row);
        release_two_dimensional_space<ArrType>(&sig, Miner<ArrType>::map_row);
    }
};

template<typename ArrType>
class GodMiner: public Miner<ArrType>
{
public:
    GodMiner(): Miner<ArrType>() {}

    GodMiner(const std::string &file_path): Miner<ArrType>(file_path) {}
    
    void run()
    {
        ArrType ret = 0;
        ArrType **dp = require_two_dimensional_space<ArrType>(Miner<ArrType>::map_row, Miner<ArrType>::map_col);
        ArrType **sig = require_two_dimensional_space<ArrType>(Miner<ArrType>::map_row, Miner<ArrType>::map_col);
        Tester::run(god_run::bind_variables(
            ret, dp, sig,
            Miner<ArrType>::space,
            Miner<ArrType>::map_row,
            Miner<ArrType>::map_col
        ));
        Tester::show();
        Tester::settle_clock();
        release_two_dimensional_space<ArrType>(&dp, Miner<ArrType>::map_row);
        release_two_dimensional_space<ArrType>(&sig, Miner<ArrType>::map_row);
    }
};


# endif