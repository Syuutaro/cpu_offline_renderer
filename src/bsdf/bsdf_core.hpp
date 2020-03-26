#ifndef BSDF_CORE_HPP
#define BSDF_CORE_HPP

#include "bsdf/bsdf_abstract.hpp"

vec3 sample_diffuse_reflection(const vec3& normal,Random& random);
vec3 sample_specular_reflection(const vec3& normal,const vec3& omega_i);
vec3 sample_specular_refraction(const vec3& normal,const vec3& omega_i,FLOAT eta_i,FLOAT eta_o);
bool is_total_reflection(const vec3& normal,const vec3& omega_i,FLOAT eta_i,FLOAT eta_o);
FLOAT fresnel_term(const vec3& normal,const vec3& omega_i,const vec3& omega_o,FLOAT eta_i,FLOAT eta_o);
vec3 sample_ggx_vndf(const vec3& Ve,FLOAT ani_x,FLOAT ani_y,Random& random);
FLOAT smith_ggx_lambda(const vec3& omega,FLOAT ani_x,FLOAT ani_y);
FLOAT smith_ggx_G1(const vec3& omega,FLOAT ani_x,FLOAT ani_y);
FLOAT smith_ggx_G2(const vec3& omega_i,const vec3& omega_o,FLOAT ani_x,FLOAT ani_y);


#endif // BSDF_CORE_HPP
