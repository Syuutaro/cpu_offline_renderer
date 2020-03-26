#include "bsdf/ideal_dielectric.hpp"
#include "bsdf/bsdf_core.hpp"

IdealDielectric::IdealDielectric(const vec3& ref,FLOAT ieta,FLOAT oeta,const Texture* ref_tex){
    m_ref = ref;
    m_ieta = ieta;
    m_oeta = oeta;
    m_ref_tex = ref_tex;
}

void IdealDielectric::Sample(vec3& wgt,Ray& ray_o,const Ray& ray_i,const Hitpoint& hp,Random& random) const{
    //check ray's inside hit or outside hit
    bool is_outside_hit = (dot(ray_i.dir,hp.normal) < 0)? true:false;

    //normal
    vec3 normal;
    if (is_outside_hit){
        normal = hp.normal;
    }else{
        normal = hp.normal*(-1);
    }

    //eta
    FLOAT eta_i,eta_o;
    if (is_outside_hit){
        eta_i = m_ieta;
        eta_o = m_oeta;
    }else{
        eta_i = m_oeta;
        eta_o = m_ieta;
    }

    //omega_o
    const vec3& omega_o = ray_i.dir*(-1);

    //choose one of reflection,refraction and total reflection
    if (!is_total_reflection(normal,omega_o,eta_o,eta_i)){
        //temporary omega_i(refraction direction)
        const vec3& refra_dir = sample_specular_refraction(normal,omega_o,eta_o,eta_i);

        //fresnel term
        FLOAT Fr = fresnel_term(normal,refra_dir,omega_o,eta_i,eta_o);

        //chose reflection or refraction
        if (random.GetNormalizedFloat() < Fr){
            //reflection
            const vec3& omega_i = sample_specular_reflection(normal,omega_o);

            //weight
            wgt = (m_ref_tex == nullptr)? m_ref:m_ref_tex->SampleNormalizedRGB(hp.uv);

            //outgoing ray
            ray_o.ori = hp.xyz+normal*EPSILON;
            ray_o.dir = omega_i;
        }else{
            //refraction
            const vec3& omega_i = refra_dir;

            //weight
            wgt = (m_ref_tex == nullptr)? m_ref:m_ref_tex->SampleNormalizedRGB(hp.uv);
            wgt *= (eta_o*eta_o)/(eta_i*eta_i);

            //outgoing ray
            ray_o.ori = hp.xyz+normal*(-1)*EPSILON;
            ray_o.dir = omega_i;
        }
    }else{
        //total reflection
        const vec3& omega_i = sample_specular_reflection(normal,omega_o);

        //weight
        wgt = (m_ref_tex == nullptr)? m_ref:m_ref_tex->SampleNormalizedRGB(hp.uv);

        //outgoing ray
        ray_o.ori = hp.xyz+normal*EPSILON;
        ray_o.dir = omega_i;
    }
}
