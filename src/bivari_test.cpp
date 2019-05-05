
#include <iostream>
#include "local_tools.h"

int main ()
{
    auto gen = bi_vari_generator<double>(1, 0, 1, 0);
    auto p = gen();
    std::cout << p.first << p.second << std::endl;
    auto p = gen();
    std::cout << p.first << p.second << std::endl;
    auto p = gen();
    std::cout << p.first << p.second << std::endl;
}