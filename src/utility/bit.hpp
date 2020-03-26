#ifndef BIT_HPP
#define BIT_HPP

#include <cstdint>

//bit function
bool bit_number(uint64_t x,uint8_t i);
uint8_t bit_length(uint64_t x);
uint8_t common_bit_length(uint64_t x1,uint64_t x2);

//morton code
uint32_t morton_code(uint16_t x1,uint16_t x2);
uint64_t morton_code(uint16_t x1,uint16_t x2,uint16_t x3);


#endif // BIT_HPP
