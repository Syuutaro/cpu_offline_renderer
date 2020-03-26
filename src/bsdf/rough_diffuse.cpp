#include "bsdf/rough_diffuse.hpp"
#include "bsdf/bsdf_core.hpp"

RoughDiffuse::RoughDiffuse(const vec3& ref,
                           FLOAT ani_x,
                           FLOAT ani_y,
                           const Texture* ref_tex,
                           const Texture* ani_x_tex,
                           const Texture* ani_y_tex,
                           const Texture* tan_tex)
{
    m_ref = ref;
    m_ani_x = ani_x;
    m_ani_y = ani_y;
    m_ref_tex = ref_tex;
    m_ani_x_tex = ani_x_tex;
    m_ani_y_tex = ani_y_tex;
    m_tan_tex = tan_tex;
}

void RoughDiffuse::Sample(vec3& wgt,Ray& ray_o,const Ray& ray_i,const Hitpoint& hp,Random& random) const{
    //hit back side
    if (dot(ray_i.dir,hp.normal) >= 0){
        wgt = vec3();
        return;
    }

    //sample reflectance,anisotropy
    vec3 ref = (m_ref_tex == nullptr)? m_ref:m_ref_tex->SampleNormalizedRGB(hp.uv);
    FLOAT ani_x = (m_ani_x_tex == nullptr)? m_ani_x:m_ani_x_tex->SampleNormalizedScalar(hp.uv);
    FLOAT ani_y = (m_ani_y_tex == nullptr)? m_ani_y:m_ani_y_tex->SampleNormalizedScalar(hp.uv);

    //tangent space
    vec3 normal,tangent,binormal;
    if (m_tan_tex == nullptr){
        normal = hp.normal;
        tangent = hp.tangent;
        binormal = hp.binormal;
    }else{
        normal = hp.normal;
        tangent = mat3({hp.tangent,hp.binormal,hp.normal})*m_tan_tex->SampleNormalizedVector(hp.uv);
        binormal = cross(normal,tangent);
    }

    //transformation matrix
    const mat3& A = mat3({tangent,binormal,normal});

    //omega_o
    const vec3& omega_o = transpose(A)*(ray_i.dir*(-1));

    //sample omage_m from GGX VNDF
    const vec3& omega_m = sample_ggx_vndf(omega_o,ani_x,ani_y,random);

    //sample omega_i
    const vec3& omega_i = sample_diffuse_reflection(omega_m,random);

    //weight
    wgt = ref*(smith_ggx_G2(omega_i,omega_o,ani_x,ani_y)/smith_ggx_G1(omega_o,ani_x,ani_y));

    //outgoing ray
    //transform omega_i to world coordinate
    ray_o.ori = hp.xyz+normal*EPSILON;
    ray_o.dir = A*omega_i;
}
