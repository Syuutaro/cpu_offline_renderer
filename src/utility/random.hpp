#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <cstdint>

//xorshift128
class Random{
private:
    uint32_t m_x;
    uint32_t m_y;
    uint32_t m_z;
    uint32_t m_w;
public:
    Random(uint32_t seed = 0);
    uint32_t GetInteger(uint32_t min,uint32_t max);
    double GetFloat(double min,double max);
    double GetNormalizedFloat();
private:
    void Update();
};


#endif // RANDOM_HPP
