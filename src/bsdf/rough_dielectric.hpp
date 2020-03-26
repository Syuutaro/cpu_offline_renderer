#ifndef ROUGH_DIELECTRIC_HPP
#define ROUGH_DIELECTRIC_HPP

#include "bsdf/bsdf_abstract.hpp"

class RoughDielectric:public BSDF{
private:
    vec3 m_ref;
    FLOAT m_ieta;
    FLOAT m_oeta;
    FLOAT m_ani_x;
    FLOAT m_ani_y;
    const Texture* m_ref_tex;
    const Texture* m_ani_x_tex;
    const Texture* m_ani_y_tex;
    const Texture* m_tan_tex;
public:
    RoughDielectric(const vec3& ref,
                    FLOAT ieta,
                    FLOAT oeta,
                    FLOAT ani_x,
                    FLOAT ani_y,
                    const Texture* ref_tex,
                    const Texture* ani_x_tex,
                    const Texture* ani_y_tex,
                    const Texture* tan_tex);
    void Sample(vec3& wgt,Ray& ray_o,const Ray& ray_i,const Hitpoint& hp,Random& random) const override;
};

#endif // ROUGH_DIELECTRIC_HPP
