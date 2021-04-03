#ifndef METRICS_ASSERTS_HPP
#define METRICS_ASSERTS_HPP

#include <iostream>
#include <cassert>

#define ASSERT_W(expr)                                                                                      \
if(!expr)                                                                                                   \
{                                                                                                           \
std::cout << __FILE__ << ":" << __LINE__ << ": assertion failed in " << __PRETTY_FUNCTION__ << std::endl;   \
return EXIT_FAILURE;                                                                                        \
}

#define ASSERT_EQ(a, b)                                                                                     \
if(a != b)                                                                                                  \
{                                                                                                           \
std::cout << std::endl;                                                                                     \
std::cout << __FILE__ << ":" << __LINE__ << ": assertion failed in " << __PRETTY_FUNCTION__ << std::endl;   \
std::cout << "a" << " = " << a << std::endl;                                                                 \
std::cout << "b" << " = " << b << std::endl;                                                                 \
return EXIT_FAILURE;                                                                                        \
} else void(0)

#define TEST(test)                                                                                          \
if(test() != 0)                                                                                             \
    return EXIT_FAILURE;


#endif //METRICS_ASSERTS_HPP1
