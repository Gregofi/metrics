//
// Created by filip on 3/26/21.
//

#ifndef METRICS_LOGGING_HPP
#define METRICS_LOGGING_HPP


#ifdef DEBUG
    #include <iostream>
    #define LOG(MESSAGE) std::cout << __FILE__ << "(" << __LINE__ << "):" << MESSAGE << std::endl
#else
    #include <iostream>
    #define LOG(MESSAGE) std::cout << __FILE__ << "(" << __LINE__ << "):" << MESSAGE << std::endl
#endif

#endif //METRICS_LOGGING_HPP

