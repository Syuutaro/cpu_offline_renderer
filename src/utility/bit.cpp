#include "utility/bit.hpp"


bool bit_number(uint64_t x,uint8_t i){
    return x << (63-i) >> 63;
}

uint8_t bit_length(uint64_t x){
    uint8_t count = 0;
    for (int8_t i = 63;i >= 0;i--){
        if (bit_number(x,i) == 0){
            count++;
        }else{
            break;
        }
    }
    return 64-count;
}

uint8_t common_bit_length(uint64_t x1,uint64_t x2){
    uint8_t count = 0;
    for (int8_t i = 63;i >= 0;i--){
        if (bit_number(x1,i) == bit_number(x2,i)){
            count++;
        }else{
            break;
        }
    }
    return count;
}

uint32_t morton_code(uint16_t x1,uint16_t x2){
    uint32_t n = 0;
    for (uint8_t i = 0;i < 16;i++){
        uint32_t bx1 = bit_number(x1,i);
        uint32_t bx2 = bit_number(x2,i);
        n += (bx1 << (2*i+1))+(bx2 << (2*i+0));
    }
    return n;
}

uint64_t morton_code(uint16_t x1,uint16_t x2,uint16_t x3){
    uint64_t n = 0;
    for (uint8_t i = 0;i < 16;i++){
        uint64_t bx1 = bit_number(x1,i);
        uint64_t bx2 = bit_number(x2,i);
        uint64_t bx3 = bit_number(x3,i);
        n += (bx1 << (3*i+2))+(bx2 << (3*i+1))+(bx3 << (3*i+0));
    }
    return n;
}
