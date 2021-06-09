#include "utils/time.hpp" 

#ifdef NATIVE
#include <ctime> // time(...) function not supported (replace w/ sgx_get_trusted_time)
#include <stdexcept>
#else
#include "sgx_tae_service.h"
#endif

Time Now() {
#ifdef NATIVE
  return (uint64_t) std::time(nullptr);
#else
  sgx_time_t trusted_time;
  sgx_time_source_nonce_t nonce = {0};
  sgx_get_trusted_time(&trusted_time, &nonce);
  return (uint64_t) trusted_time;
#endif
}

std::string TimeToString(Time t) {
  return std::to_string((uint64_t) t);
}

// bool operator<(const Time& t1, const Time& t2) {
//   return t1 < t2;
// }
// Time operator+(const Time& t, const Duration& d) {
//   return t + d;
// }
// Time operator-(const Time& t, const Duration& d) {
//   if (d > t)
//     throw std::runtime_error("Substraction would result in negative Time");

//   return t - d;
// }

// Time operator%(const Time& t, const Duration& d) {
//   return t % d;
// }

// Duration operator*(const Duration& d, const unsigned int& x) {
//   return d * x;
// }
// Duration operator*(const unsigned int& x, const Duration& d) {
//   return d * x;
// }