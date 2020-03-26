#include "light/light.hpp"

Light::Light(const vec3& rad,FLOAT scale,const Texture* rad_tex){
    m_rad = rad;
    m_scale = scale;
    m_rad_tex = rad_tex;
}

void Light::Sample(vec3& rad,const Hitpoint& hp) const{
    if (m_rad_tex == nullptr){
        rad = m_rad*m_scale;
    }else{
        rad = m_rad_tex->SampleNormalizedRGB(hp.uv)*m_scale;
    }
}
