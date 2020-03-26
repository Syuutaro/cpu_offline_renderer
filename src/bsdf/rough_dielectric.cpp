#include "bsdf/rough_dielectric.hpp"
#include "bsdf/bsdf_core.hpp"

RoughDielectric::RoughDielectric(const vec3& ref,
                                 FLOAT ieta,
                                 FLOAT oeta,
                                 FLOAT ani_x,
                                 FLOAT ani_y,
                                 const Texture* ref_tex,
                                 const Texture* ani_x_tex,
                                 const Texture* ani_y_tex,
                                 const Texture* tan_tex)
{
    m_ref = ref;
    m_ieta = ieta;
    m_oeta = oeta;
    m_ani_x = ani_x;
    m_ani_y = ani_y;
    m_ref_tex = ref_tex;
    m_ani_x_tex = ani_x_tex;
    m_ani_y_tex = ani_y_tex;
    m_tan_tex = tan_tex;
}

void RoughDielectric::Sample(vec3& wgt,Ray& ray_o,const Ray& ray_i,const Hitpoint& hp,Random& random) const{
    //sample reflectance,anisotropy
    vec3 ref = (m_ref_tex == nullptr)? m_ref:m_ref_tex->SampleNormalizedRGB(hp.uv);
    FLOAT ani_x = (m_ani_x_tex == nullptr)? m_ani_x:m_ani_x_tex->SampleNormalizedScalar(hp.uv);
    FLOAT ani_y = (m_ani_y_tex == nullptr)? m_ani_y:m_ani_y_tex->SampleNormalizedScalar(hp.uv);

    //check ray's inside hit or outside hit
    bool is_outside_hit = (dot(ray_i.dir,hp.normal) < 0)? true:false;

    //eta
    FLOAT eta_i,eta_o;
    if (is_outside_hit){
        eta_i = m_ieta;
        eta_o = m_oeta;
    }else{
        eta_i = m_oeta;
        eta_o = m_ieta;
    }

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

    //correct tangent space
    if (!is_outside_hit){
        normal *= -1;
        tangent *= -1;
    }

    //transformation matrix
    const mat3& A = mat3({tangent,binormal,normal});

    //omega_o
    const vec3& omega_o = transpose(A)*(ray_i.dir*(-1));

    //sample omage_m from GGX VNDF
    const vec3& omega_m = sample_ggx_vndf(omega_o,ani_x,ani_y,random);

    //choose one of reflection,refraction and total reflection
    if (!is_total_reflection(omega_m,omega_o,eta_o,eta_i)){
        //temporary omega_i(refraction direction)
        const vec3& refra_dir = sample_specular_refraction(omega_m,omega_o,eta_o,eta_i);

        //fresnel term
        FLOAT Fr = fresnel_term(omega_m,refra_dir,omega_o,eta_i,eta_o);

        //chose reflection or refraction
        if (random.GetNormalizedFloat() < Fr){
            //reflection
            //omega_i
            const vec3& omega_i = sample_specular_reflection(omega_m,omega_o);

            //weight
            wgt = ref*(smith_ggx_G2(omega_i,omega_o,ani_x,ani_y)/smith_ggx_G1(omega_o,ani_x,ani_y));

            //outgoing ray
            //transform omega_i to world coordinate
            ray_o.ori = hp.xyz+normal*EPSILON;
            ray_o.dir = A*omega_i;
        }else{
            //refraction
            //omega_i
            const vec3& omega_i = refra_dir;

            //weight
            wgt = ref*(smith_ggx_G2(omega_i,omega_o,ani_x,ani_y)/smith_ggx_G1(omega_o,ani_x,ani_y));

            //outgoing ray
            //transform omega_i to world coordinate
            ray_o.ori = hp.xyz+normal*(-1)*EPSILON;
            ray_o.dir = A*omega_i;
        }
    }else{
        //total reflection
        //omega_i
        const vec3& omega_i = sample_specular_reflection(omega_m,omega_o);

        //weight
        wgt = ref*(smith_ggx_G2(omega_i,omega_o,ani_x,ani_y)/smith_ggx_G1(omega_o,ani_x,ani_y));

        //outgoing ray
        //transform omega_i to world coordinate
        ray_o.ori = hp.xyz+normal*EPSILON;
        ray_o.dir = A*omega_i;
    }
}
