#ifndef UTILS_LOG_HPP_
#define UTILS_LOG_HPP_
#include <string>
#include "data/Data.hpp"

// Docs:
// https://www.cprogramming.com/reference/preprocessor/__LINE__.html
// https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html

void logMessage (bool log, const char* file, int line, const char* message);

std::string dataLog(int turn, Data data);

#define DEBUG( debug, message ) logMessage( (debug), __FILE__, __LINE__, (message) )
#define LOG( message ) logMessage( true, __FILE__, __LINE__, (message) )


#endif // UTILS_LOG_HPP_
