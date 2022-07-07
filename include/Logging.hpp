#pragma once
#include <iostream>

#ifdef DEBUG
    #define LOG(MESSAGE) std::cerr << __FILE__ << "(" << __LINE__ << "):" << MESSAGE << std::endl
#else
    #define LOG(MESSAGE) void(0)
#endif
