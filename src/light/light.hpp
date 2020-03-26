#ifndef LIGHT_HPP
#define LIGHT_HPP

#include "core/define.hpp"
#include "core/hitpoint.hpp"
#include "utility/matrix.hpp"
#include "utility/texture.hpp"

class Light{
private:
    vec3 m_rad;
    FLOAT m_scale;
    const Texture* m_rad_tex;
public:
    Light(const vec3& rad,FLOAT scale,const Texture* rad_tex);
    void Sample(vec3& rad,const Hitpoint& hp) const;
};


#endif // LIGHT_HPP
