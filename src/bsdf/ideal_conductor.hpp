#ifndef IDEAL_CONDUCTOR_HPP
#define IDEAL_CONDUCTOR_HPP

#include "bsdf/bsdf_abstract.hpp"

class IdealConductor:public BSDF{
private:
    vec3 m_ref;
    const Texture* m_ref_tex;
public:
    IdealConductor(const vec3& ref,const Texture* ref_tex);
    void Sample(vec3& wgt,Ray& ray_o,const Ray& ray_i,const Hitpoint& hp,Random& random) const override;
};


#endif // IDEAL_CONDUCTOR_HPP
