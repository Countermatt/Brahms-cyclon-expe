#include "random.hpp"

#ifdef NATIVE
#include <algorithm> // for random_shuffle
#include <random> // for random generators, distribution and all
#include <mutex> // for call_once, to initialize the generators once

std::once_flag genInitFlag;
std::mt19937 gen;

void initGen() {
  // Seed with a real random value, if available
  std::random_device r;
  gen.seed(r());
}
#else // !NATIVE
#include "sgx_trts.h"
#endif

unsigned int random_int()
{
#ifdef NATIVE
  std::call_once(genInitFlag, initGen);
  return static_cast<unsigned int>(gen());
#else // !NATIVE
  uint32_t i;
  sgx_read_rand((unsigned char *) &i, 4);
  return (unsigned int)i;
#endif
}

void rand_buf(char* buf, unsigned int size)
{
#ifdef NATIVE
  std::call_once(genInitFlag, initGen);
  for(unsigned int i = 0; i < size; i++)
      buf[i] = static_cast<char>(gen());
#else // !NATIVE
  sgx_read_rand((unsigned char*) buf, size);
#endif
}

void vector_shuffle(std::vector<std::string>& a)
{
#ifdef NATIVE
  std::call_once(genInitFlag, initGen);
  std::shuffle(a.begin(), a.end(), gen);
#else // !NATIVE
  // random_shuffle is not supported in SGX - doing it Knuth's style.
  for(size_t i = a.size() - 1; i >= 1; i--)
  {
    size_t j = (random_int() % i) + 1; // where 0 <= j <= i
    std::swap(a[i], a[j]);
  }
#endif
}