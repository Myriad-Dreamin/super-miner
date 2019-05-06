
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

// template<typename GenType, typename generator1Type, typename generator2Type>
// std::pair<GenType, GenType> generator_functor(generator1Type generator1, generator2Type generator2)
// {
//     return std::make_pair(generator1(), generator2());
// }


// template<typename GenType>
// auto bi_vari_generator(double mu1, double varx1, double mu2, double varx2) ->
//     decltype(std::bind(
//         generator_functor<
//             GenType, 
//             decltype(
//                 std::bind(
//                     std::normal_distribution<GenType>(mu1, varx1),
//                     std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
//                 )
//             ),
//             decltype(
//                 std::bind(
//                     std::normal_distribution<GenType>(mu2, varx2),
//                     std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
//                 )
//             )
//         >,
//         std::bind(
//             std::normal_distribution<GenType>(mu1, varx1),
//             std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
//         ),
//         std::bind(
//             std::normal_distribution<GenType>(mu2, varx2),
//             std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
//         )
//     ))
// {
//     return std::bind(
//         generator_functor<
//             GenType, 
//             decltype(
//                 std::bind(
//                     std::normal_distribution<GenType>(mu1, varx1),
//                     std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
//                 )
//             ),
//             decltype(
//                 std::bind(
//                     std::normal_distribution<GenType>(mu2, varx2),
//                     std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
//                 )
//             )
//         >,
//         std::bind(
//             std::normal_distribution<GenType>(mu1, varx1),
//             std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
//         ),
//         std::bind(
//             std::normal_distribution<GenType>(mu2, varx2),
//             std::mt19937((time(nullptr) * 23333ULL + 1229483049421ULL))
//         )
//     );
// }



namespace bivari {

    template<typename GenType>
    struct functor {
        typedef std::function<std::pair<GenType, GenType>()> func;
        typedef std::function<func(double,double)> functory;
    };

namespace normal_distribution {
    template<typename GenType>
    typename functor<GenType>::func
    generator(double mu1, double varx1, double mu2, double varx2)
    {
        auto gen1 = std::bind(
            std::normal_distribution<double>(mu1, varx1),
            std::mt19937(time(nullptr) * 23333ULL + 1ULL)
        );
        auto gen2 = std::bind(
            std::normal_distribution<double>(mu2, varx2),
            std::mt19937(time(nullptr) * 23333ULL + 2ULL)
        );
        return [gen1, gen2]() mutable -> std::pair<GenType, GenType> {
            return std::make_pair(GenType(gen1()), GenType(gen2()));
        };
    }

    template<typename GenType>
    typename functor<GenType>::functory
    functory_fixed(double varx1, double varx2)
    {
        return [varx1, varx2](double mu1, double mu2)
            mutable -> std::function<std::pair<GenType, GenType>()>
        {
            auto gen1 = std::bind(
                std::normal_distribution<double>(mu1, varx1),
                std::mt19937(time(nullptr) * 23333ULL + 1ULL)
            );
            auto gen2 = std::bind(
                std::normal_distribution<double>(mu2, varx2),
                std::mt19937(time(nullptr) * 23333ULL + 2ULL)
            );
            return [gen1, gen2]() mutable -> std::pair<GenType, GenType> {
                return std::make_pair(GenType(gen1()), GenType(gen2()));
            };
        };
    }

    template<typename GenType>
    typename functor<GenType>::functory
    functory_range(double varxl, double varxr)
    {
        return [varxl, varxr

            ](double mu1, double mu2)
            mutable -> std::function<std::pair<GenType, GenType>()>
        {
            auto gen1 = std::bind(
                std::normal_distribution<double>(mu1, rand_double(varxl, varxr)),
                std::mt19937(time(nullptr) * 23333ULL + 1ULL)
            );
            auto gen2 = std::bind(
                std::normal_distribution<double>(mu2, rand_double(varxl, varxr)),
                std::mt19937(time(nullptr) * 23333ULL + 2ULL)
            );
            return [gen1, gen2]() mutable -> std::pair<GenType, GenType> {
                return std::make_pair(GenType(gen1()), GenType(gen2()));
            };
        };
    }
} // end of namespace normal distribution
} // end of bivari

# endif