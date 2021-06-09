#ifndef UTILS_RANDOM_HPP_
#define UTILS_RANDOM_HPP_

#include <vector>
#include <string>

unsigned int random_int();
void rand_buf(char* buf, unsigned int size);
void vector_shuffle(std::vector<std::string>& a);

#endif