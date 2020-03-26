#include "bsdf/ideal_diffuse.hpp"
#include "bsdf/bsdf_core.hpp"

IdealDiffuse::IdealDiffuse(const vec3& ref,const Texture* ref_tex){
    m_ref = ref;
    m_ref_tex = ref_tex;
}
void IdealDiffuse::Sample(vec3& wgt,Ray& ray_o,const Ray& ray_i,const Hitpoint& hp,Random& random) const{
    //hit back side
    if (dot(ray_i.dir,hp.normal) >= 0){
        wgt = vec3();
        return;
    }

    //weight
    wgt = (m_ref_tex == nullptr)? m_ref:m_ref_tex->SampleNormalizedRGB(hp.uv);

    //outgoing ray
    ray_o.ori = hp.xyz+hp.normal*EPSILON;
    ray_o.dir = sample_diffuse_reflection(hp.normal,random);
}
