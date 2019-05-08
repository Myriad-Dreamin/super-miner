
# ifndef SUPER_MINER_H
# define SUPER_MINER_H


#include <iostream>
#include <cstring>
#include <stdexcept>
#include <functional>
#include <utility>
#include <iomanip>
#include <cassert>
#include <algorithm>
#include "miner.h"
#include "../alter/generate.h"
#include "../alter/tester.h"

# define MAKE_RUN(INSERTED_FUNCTION, EXTEND_INFO) \
    do {\
        ArrType ret = 0;\
        ArrType **dp = require_two_dimensional_space<ArrType>(Miner<ArrType>::map_row, Miner<ArrType>::map_col);\
        ArrType **sig = require_two_dimensional_space<ArrType>(Miner<ArrType>::map_row, Miner<ArrType>::map_col);\
        INSERTED_FUNCTION;\
        Tester::show();\
        Tester::block(k);\
        Tester::block(Tester::used_time());\
        Tester::block(ret);\
        EXTEND_INFO;\
        Tester::newline();\
        Tester::settle_clock();\
        release_two_dimensional_space<ArrType>(&dp, Miner<ArrType>::map_row);\
        release_two_dimensional_space<ArrType>(&sig, Miner<ArrType>::map_row);\
    } while(0)\


template<typename ArrType>
class BiMapGenerator
{
protected:
    int map_row, map_col;
    ArrType **space;
public:

    BiMapGenerator<ArrType>()
    {
        map_row = map_col = 0;
        space = nullptr;
    }

    BiMapGenerator<ArrType>(const int row, const int col)
    {
        map_row = row;
        map_col = col;
        space = nullptr;
    }

    const int row_size() const {return map_row;}
    const int col_size() const {return map_col;}
    const ArrType **exported_space() const {return const_cast<const ArrType **>(space);}
};


template<typename ArrType>
class BiNormMinerMapGenerator: public BiMapGenerator<ArrType>
{
private:
    int mine_cnt, mine_siz;
    double sigl, sigr;
    bivari::functor<int>::functory gen_functory;
public:
    BiNormMinerMapGenerator(): BiMapGenerator<ArrType>()
    {
        mine_cnt = mine_siz = 0;
        sigl = sigr = 0;
    }

    BiNormMinerMapGenerator(
        const int row, const int col,
        const int mine_count, const int mine_size,
        const double sigmaL, const double sigmaR
    ) : BiMapGenerator<ArrType>(row, col)
    {
        mine_cnt = mine_count;
        mine_siz = mine_size;
        sigl = sigmaL;
        sigr = sigmaR;

        gen_functory = bivari::normal_distribution::functory_range<int>(sigmaL, sigmaR);

        this->space = new ArrType*[this->map_row];
        for (int i = 0; i < this->map_row; i++) {
            this->space[i] = new ArrType[this->map_col];
        }
    }

    BiNormMinerMapGenerator(const BiNormMinerMapGenerator &rg) {
        if (this == &rg) {
            return;
        }
        
        if (this->space != nullptr && this->map_row != rg.map_row && this->map_col != rg.map_col) {
            for (int i = 0; i < this->map_row; i++) {
                delete[] this->space[i];
            }
            delete[] this->space;
            this->space = nullptr;
        }

        this->map_row = rg.map_row;
        this->map_col = rg.map_col;


        if (this->space == nullptr) {
            this->space = new ArrType*[rg.map_row];
            for (int i = 0; i < rg.map_row; i++) {
                this->space[i] = new ArrType[rg.map_col];
            }
        }
        for (int i = 0; i < rg.map_row; i++) {
            memcpy(this->space[i], rg.space[i], sizeof(ArrType) * rg.map_col);
        }
    }

    BiNormMinerMapGenerator(BiNormMinerMapGenerator &&rg) {
        if (this == &rg) {
            return;
        }

        if (this->space != nullptr) {
            for (int i = 0; i < this->map_row; i++) {
                delete[] this->space[i];
            }
            delete[] this->space;
        }

        this->map_row = rg.map_row;
        this->map_col = rg.map_col;

        this->space = rg.space;
        rg.space = nullptr;
    }

    ~BiNormMinerMapGenerator()
    {
        if (this->space == nullptr) {
            return;
        }

        for (int i = 0; i < this->map_row; i++) {
            delete[] this->space[i];
        }
        delete[] this->space;
        this->space = nullptr;
    }

    const ArrType **gen(std::function<ArrType()> rand_generator)
    {
        if (this->space == nullptr) {
            return nullptr;
        }

        int x,y,z;
        std::pair<int, int> ret;

        for (int i = 0; i < this->map_row; i++) {
            memset(this->space[i], 0, sizeof(ArrType) * this->map_col);
        }

        for (int i = 0; i < mine_cnt; i++) {
            x = rand_uint() % this->map_row, y = rand_uint() % this->map_col, z = rand_uint() % mine_siz;
            auto gen = gen_functory(x, y);
            while(z--) {
                ret = gen();
                if (
                    (0 <= ret.first && ret.first < this->map_row) &&
                    (0 <= ret.second && ret.second < this->map_col)
                ) {
                    this->space[ret.first][ret.second] = rand_generator();
                }
            }
        }
        return this->exported_space();
    }

    const ArrType **gen()
    {
        if (this->space == nullptr) {
            return nullptr;
        }

        int x, y, z;
        std::pair<int, int> ret;

        for (int i = 0; i < this->map_row; i++) {
            memset(this->space[i], 0, sizeof(ArrType) * this->map_col);
        }

        for (int i = 0; i < mine_cnt; i++) {
            x = rand_uint() % this->map_row;
            y = rand_uint() % this->map_col;
            // std::cout << "randcenter " << x << " " << y << std::endl;
            z = rand_uint() % mine_siz;
            auto gen = gen_functory(x, y);
            while(z--) {
                ret = gen();
                // std::cout << "randgen " << ret.first << " " << ret.second << std::endl;
                if (
                    (0 <= ret.first && ret.first < this->map_row) &&
                    (0 <= ret.second && ret.second < this->map_col)
                ) {
                    this->space[ret.first][ret.second]++;
                }
            }
        }
        return this->exported_space();
    }
};


template<typename ArrType>
class AbsentMinerMapGenerator: public BiMapGenerator<ArrType>
{
public:
    AbsentMinerMapGenerator() : BiMapGenerator<ArrType>()
    {
        this->space = nullptr;
    }

    AbsentMinerMapGenerator(
        const int row, const int col
    ) : BiMapGenerator<ArrType>(row, col)
    {
        this->space = new ArrType*[this->map_row];
        for (int i = 0; i < this->map_row; i++) {
            this->space[i] = new ArrType[this->map_col];
        }
    }

    AbsentMinerMapGenerator(const AbsentMinerMapGenerator &rg) {
        if (this == &rg) {
            return;
        }
        
        if (this->space != nullptr && this->map_row != rg.map_row && this->map_col != rg.map_col) {
            for (int i = 0; i < this->map_row; i++) {
                delete[] this->space[i];
            }
            delete[] this->space;
            this->space = nullptr;
        }

        this->map_row = rg.map_row;
        this->map_col = rg.map_col;


        if (this->space == nullptr) {
            this->space = new ArrType*[rg.map_row];
            for (int i = 0; i < rg.map_row; i++) {
                this->space[i] = new ArrType[rg.map_col];
            }
        }

        for (int i = 0; i < rg.map_row; i++) {
            memcpy(this->space[i], rg.space[i], sizeof(ArrType) * rg.map_col);
        }
    }

    AbsentMinerMapGenerator(AbsentMinerMapGenerator &&rg) {
        this->map_row = rg.map_row;
        this->map_col = rg.map_col;

        this->space = rg.space;
        rg.space = nullptr;
    }

    ~AbsentMinerMapGenerator()
    {
        if (this->space == nullptr) {
            return;
        }

        for (int i = 0; i < this->map_row; i++) {
            delete[] this->space[i];
        }
        delete[] this->space;
    }

    const ArrType **gen(const BiMapGenerator<ArrType> &bimap, double pec, ArrType invalid_arr_element)
    {
        if (this->space != nullptr && this->map_col != bimap.col_size() && this->map_row != bimap.row_size()) {
            for (int i = 0; i < this->map_row; i++) {
                delete[] this->space[i];
            }
            delete[] this->space;
            this->space = nullptr;
        }
        
        if (this->space == nullptr) {
            this->space = new ArrType*[bimap.row_size()];
            for (int i = 0; i < bimap.row_size(); i++) {
                this->space[i] = new ArrType[bimap.col_size()];
            }
        }

        this->map_row = bimap.row_size();
        this->map_col = bimap.col_size();

        std::pair<int, int> *gen_range = new std::pair<int, int>[bimap.row_size() * bimap.col_size()];
        for (int i = 0; i < this->map_row; i++) {
            for (int j = 0; j < this->map_col; j++) {
                gen_range[i * this->map_col + j] = std::move(std::make_pair(i, j));
                this->space[i][j] = invalid_arr_element;
            }
        }
        std::random_shuffle(gen_range, gen_range + bimap.row_size() * bimap.col_size());
        for (int i = (1 - pec) * bimap.row_size() * bimap.col_size() - 1; i >=0; i--) {
            this->space[gen_range[i].first][gen_range[i].second]
            = bimap.exported_space()[gen_range[i].first][gen_range[i].second];
        }

        delete[] gen_range;

        return this->exported_space();
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
    
    void read_map(const ArrType **out_space, int row, int col)
    {
        map_row = row; map_col = col;
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
        auto &mp = Miner<ArrType>::space;
        auto &row = Miner<ArrType>::map_row;
        auto &col = Miner<ArrType>::map_col;
        MAKE_RUN(Tester::run(snake_run::bind_variables(
            &ret, dp, sig, mp, row, col, k
        ), [&ret, sig, mp, row, col]() mutable -> void {
            ArrType tmpret = 0;
            for (int curx = 0, cury = 0;;) {
                if (curx == row - 1) {
                    while (cury < col) {
                        tmpret = tmpret + mp[curx][cury];
                        cury++;
                    }
                    break;
                }
                if (cury == col - 1) {
                    while (curx < row) {
                        tmpret = tmpret + mp[curx][cury];
                        curx++;
                    }
                    break;
                }
                tmpret = tmpret + mp[curx][cury];
                if (sig[curx + 1][cury]) {
                    curx++;
                } else {
                    cury++;
                }
            }
            assert(tmpret == ret);
            return;
        }), {});
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
        auto &mp = Miner<ArrType>::space;
        auto &row = Miner<ArrType>::map_row;
        auto &col = Miner<ArrType>::map_col;
        MAKE_RUN(Tester::run(image_run::bind_variables(
            &ret, dp, sig, mp, row, col, k, c
        ), [&ret, sig, mp, row, col]() mutable -> void {
            ArrType tmpret = 0;
            for (int curx = 0, cury = 0;;) {
                if (curx == row - 1) {
                    while (cury < col) {
                        tmpret = tmpret + mp[curx][cury];
                        cury++;
                    }
                    break;
                }
                if (cury == col - 1) {
                    while (curx < row) {
                        tmpret = tmpret + mp[curx][cury];
                        curx++;
                    }
                    break;
                }
                tmpret = tmpret + mp[curx][cury];
                if (sig[curx + 1][cury]) {
                    curx++;
                } else {
                    cury++;
                }
            }
            assert(tmpret == ret);
            return;
        }),{
            Tester::block(c);
        });
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
        int k = 0;
        auto &mp = Miner<ArrType>::space;
        auto &row = Miner<ArrType>::map_row;
        auto &col = Miner<ArrType>::map_col;
        MAKE_RUN(Tester::run(god_run::bind_variables(
            &ret, dp, sig, mp, row, col
        ), [&ret, sig, mp, row, col]() mutable -> void {
            ArrType tmpret = 0;
            for (int curx = 0, cury = 0;;) {
                if (curx == row - 1) {
                    while (cury < col) {
                        tmpret = tmpret + mp[curx][cury];
                        cury++;
                    }
                    break;
                }
                if (cury == col - 1) {
                    while (curx < row) {
                        tmpret = tmpret + mp[curx][cury];
                        curx++;
                    }
                    break;
                }
                tmpret = tmpret + mp[curx][cury];
                if (sig[curx + 1][cury]) {
                    curx++;
                } else {
                    cury++;
                }
            }
            assert(tmpret == ret);
            return;
        }),{});
    }
};


template<typename ArrType>
class AbsentMiner: public Tester
{
protected:
    int map_row, map_col;
    const ArrType **space;
    const ArrType **absent_space;
    std::string self_path;
public:
    AbsentMiner(): Tester()
    {
        self_path = "";
        map_row = map_col = 0;
        ArrType ***as_assign = const_cast<ArrType ***>(&space);
        *as_assign = nullptr;
        as_assign = const_cast<ArrType ***>(&absent_space);
        *as_assign = nullptr;
    }

    AbsentMiner(const std::string &file_path): Tester(file_path)
    {
        self_path = file_path;
        map_row = map_col = 0;
        ArrType ***as_assign = const_cast<ArrType ***>(&space);
        *as_assign = nullptr;
        as_assign = const_cast<ArrType ***>(&absent_space);
        *as_assign = nullptr;
    }

    AbsentMiner(const AbsentMiner &rg)
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
        as_assign = const_cast<ArrType ***>(&absent_space);
        *as_assign = const_cast<ArrType**>(rg.absent_space);
    }

    AbsentMiner(AbsentMiner &&rg)
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
        as_assign = const_cast<ArrType ***>(&absent_space);
        *as_assign = const_cast<ArrType**>(rg.absent_space);
    }
    
    void read_map(const ArrType **out_space, int row, int col)
    {
        map_row = row; map_col = col;
        ArrType ***as_assign = const_cast<ArrType ***>(&space);
        *as_assign = const_cast<ArrType **>(out_space);
    }

    void read_absent_map_and_guess(const ArrType **out_space, int row, int col)
    {
        map_row = row; map_col = col;
        ArrType ***as_assign = const_cast<ArrType ***>(&absent_space);
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

    void show_absent_map()
    {
        for (int i = 0; i < map_row; i++) {
            for (int j = 0; j < map_col; j++) {
                std::cout << std::setw(2) << absent_space[i][j] << " ";
            }
            std::cout << std::endl;
        }
    }

    void run(double c)
    {
        int k = 0;
        auto &mp = space;
        auto &ab_mp = absent_space;
        auto &row = map_row;
        auto &col = map_col;

        ArrType ret = 0;
        ArrType **dp = require_two_dimensional_space<ArrType>(map_row, map_col);
        ArrType **sig = require_two_dimensional_space<ArrType>(map_row, map_col);
        Tester::run(god_run::bind_variables_absent(
            &ret, dp, sig, mp, ab_mp, row, col
        ), [&ret, sig, mp, row, col]() mutable -> void {
            ArrType tmpret = 0;
            for (int curx = 0, cury = 0;;) {
                if (curx == row - 1) {
                    while (cury < col) {
                        tmpret = tmpret + mp[curx][cury];
                        cury++;
                    }
                    break;
                }
                if (cury == col - 1) {
                    while (curx < row) {
                        tmpret = tmpret + mp[curx][cury];
                        curx++;
                    }
                    break;
                }
                tmpret = tmpret + mp[curx][cury];
                if (sig[curx + 1][cury]) {
                    curx++;
                } else {
                    cury++;
                }
            }
            assert(tmpret == ret);
            return;
        });
        Tester::show();
        Tester::block(k);
        Tester::block(Tester::used_time());
        Tester::block(ret);
        Tester::block(c);
        Tester::newline();
        Tester::settle_clock();
        release_two_dimensional_space<ArrType>(&dp, map_row);
        release_two_dimensional_space<ArrType>(&sig, map_row);
    }
};

# endif