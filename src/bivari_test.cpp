
#include <iostream>
#include "local_tools.h"

int main ()
{
	auto functory = bivari_normal_distribution_generator<int>(1.0, 0.1, 1.0, 0.1);
	auto p = functory();
	std::cout << p.first << " " << p.second << std::endl;
	p = functory();
	std::cout << p.first << " " << p.second << std::endl;

	auto functory_ex = bivari_normal_distribution_generator_factory<double>(0.1, 0.1);
	auto funcx = functory_ex(1, 2);
	p = funcx();
	std::cout << p.first << " " << p.second << std::endl;
	p = funcx();
	std::cout << p.first << " " << p.second << std::endl;

	auto funcy = functory_ex(1, 4);
	p = funcy();
	std::cout << p.first << " " << p.second << std::endl;
	p = funcy();
	std::cout << p.first << " " << p.second << std::endl;

}