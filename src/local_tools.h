
# ifndef LTOOLS_H
# define LTOOLS_H

#include <functional>
#include <ctime>
#include <random>
#include <utility>

double rand_double(double ranger=1)
{
    double mxranger = ((1ULL << 32) - 1) * ((1ULL << 32) - 1);
    std::mt19937 mt_rand((time(nullptr) * 23333ULL + 1231321LL));
    return double(mt_rand()) * double(mt_rand()) / mxranger * ranger;
}

double rand_double(double lranger, double rranger)
{
    return lranger + rand_double(rranger - lranger);
}

template<typename GenType, typename generator1Type, typename generator2Type>
std::pair<GenType, GenType> generator_functor(generator1Type generator1, generator2Type generator2)
{
    return std::make_pair(generator1(), generator2());
}


template<typename GenType>
auto bi_vari_generator(double mu1, double varx1, double mu2, double varx2) ->
    decltype(std::bind(
        generator_functor<
            GenType, 
            decltype(
                std::bind(
                    std::normal_distribution<GenType>(mu1, varx1),
                    std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
                )
            ),
            decltype(
                std::bind(
                    std::normal_distribution<GenType>(mu2, varx2),
                    std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
                )
            )
        >,
        std::bind(
            std::normal_distribution<GenType>(mu1, varx1),
            std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
        ),
        std::bind(
            std::normal_distribution<GenType>(mu2, varx2),
            std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
        )
    ))
{
    return std::bind(
        generator_functor<
            GenType, 
            decltype(
                std::bind(
                    std::normal_distribution<GenType>(mu1, varx1),
                    std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
                )
            ),
            decltype(
                std::bind(
                    std::normal_distribution<GenType>(mu2, varx2),
                    std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
                )
            )
        >,
        std::bind(
            std::normal_distribution<GenType>(mu1, varx1),
            std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
        ),
        std::bind(
            std::normal_distribution<GenType>(mu2, varx2),
            std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
        )
    );
}

# endif