#ifndef IDEAL_DIELECTRIC_HPP
#define IDEAL_DIELECTRIC_HPP

#include "bsdf/bsdf_abstract.hpp"

class IdealDielectric:public BSDF{
private:
    vec3 m_ref;
    FLOAT m_ieta;
    FLOAT m_oeta;
    const Texture* m_ref_tex;
public:
    IdealDielectric(const vec3& ref,FLOAT ieta,FLOAT oeta,const Texture* ref_tex);
    void Sample(vec3& wgt,Ray& ray_o,const Ray& ray_i,const Hitpoint& hp,Random& random) const override;
};


#endif // IDEAL_DIELECTRIC_HPP
