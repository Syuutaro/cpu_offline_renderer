#include "utility/random.hpp"


Random::Random(uint32_t seed){
    if (seed == 0){
        m_x = 123456789;
        m_y = 362436069;
        m_z = 521288629;
        m_w = 88675123;
    }else{
        m_x = 123456789+123456789%seed;
        m_y = 362436069+362436069%seed;
        m_z = 521288629+521288629%seed;
        m_w = 88675123+88675123%seed;
    }
}


uint32_t Random::GetInteger(uint32_t min,uint32_t max){
    Update();
    return min+1.0*m_w/UINT32_MAX*(max-min);
}

double Random::GetFloat(double min,double max){
    Update();
    return min+1.0*m_w/UINT32_MAX*(max-min);
}

double Random::GetNormalizedFloat(){
    Update();
    return 1.0*m_w/UINT32_MAX;
}

void Random::Update(){
    uint32_t t;
    t = m_x^(m_x << 11);
    m_x = m_y;
    m_y = m_z;
    m_z = m_w;
    m_w = (m_w^(m_w >> 19))^(t^(t >> 8));
}
