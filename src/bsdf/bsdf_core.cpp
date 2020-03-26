#include "bsdf_core.hpp"


vec3 sample_diffuse_reflection(const vec3& normal,Random& random){
    //hemisphere cosine sampling
    FLOAT theta = std::asin(std::sqrt(random.GetNormalizedFloat()));//caution!
    FLOAT phi = 2*PI*random.GetNormalizedFloat();
    FLOAT sin_theta = std::sin(theta);
    FLOAT cos_theta = std::cos(theta);
    FLOAT sin_phi = std::sin(phi);
    FLOAT cos_phi = std::cos(phi);
    const vec3& com = vec3({cos_phi*sin_theta,sin_phi*sin_theta,cos_theta});

    //orthonormal basis
    vec3 ax,ay;
    orthonormal_basis(ax,ay,normal);

    //direction
    return ax*com[0]+ay*com[1]+normal*com[2];
}

vec3 sample_specular_reflection(const vec3& normal,const vec3& omega_i){
    return normal*(2*dot(omega_i,normal))-omega_i;
}


vec3 sample_specular_refraction(const vec3& normal,const vec3& omega_i,FLOAT eta_i,FLOAT eta_o){
    FLOAT eta = eta_i/eta_o;
    FLOAT cos_i = dot(omega_i,normal);
    return (omega_i-normal*cos_i)*(-eta)-normal*std::sqrt(std::max(1-eta*eta*(1-cos_i*cos_i),0.0));
}

bool is_total_reflection(const vec3& normal,const vec3& omega_i,FLOAT eta_i,FLOAT eta_o){
    //sin_theta_o
    FLOAT cos_theta_i = dot(normal,omega_i);
    FLOAT sin_theta_i = std::sqrt(std::max(1-cos_theta_i*cos_theta_i,0.0));
    if (eta_i*sin_theta_i > eta_o){
        return true;
    }else{
        return false;
    }
}

FLOAT fresnel_term(const vec3& normal,const vec3& omega_i,const vec3& omega_o,FLOAT eta_i,FLOAT eta_o){
    FLOAT cos_theta_i = dot(normal,omega_i);
    FLOAT cos_theta_o = dot(normal,omega_o)*(-1);
    FLOAT eta_o_cos_theta_i = eta_o*cos_theta_i;
    FLOAT eta_i_cos_theta_o = eta_i*cos_theta_o;
    FLOAT eta_i_cos_theta_i = eta_i*cos_theta_i;
    FLOAT eta_o_cos_theta_o = eta_o*cos_theta_o;
    FLOAT rho1 = (eta_o_cos_theta_i-eta_i_cos_theta_o)/(eta_o_cos_theta_i+eta_i_cos_theta_o);
    FLOAT rho2 = (eta_i_cos_theta_i-eta_o_cos_theta_o)/(eta_i_cos_theta_i+eta_o_cos_theta_o);
    return 0.5*(rho1*rho1+rho2*rho2);
}

/*
FLOAT schlick_fresnel_term(const vec3& normal,const vec3& omega_i,FLOAT eta_i,FLOAT eta_o){
    FLOAT f0 = ((eta_i-eta_o)*(eta_i-eta_o))/((eta_i+eta_o)*(eta_i+eta_o));
    FLOAT cos = 1-dot(normal,omega_i);
    return f0+(1-f0)*cos*cos*cos*cos*cos;
}
*/

vec3 sample_ggx_vndf(const vec3& Ve,FLOAT ani_x,FLOAT ani_y,Random& random){
    //A = mat3(vec3(ani_x,0,0),vec3(0,ani_y,0),vec3(0,0,1))
    //ellipsoid is (A*A*x,x) = 1
    //coordinate transformation which transforms ellipsoid to sphere
    //transform Ve to Vh
    const mat3& A = mat3({vec3({ani_x,0,0}),vec3({0,ani_y,0}),vec3({0,0,1})});
    const vec3& Vh = normalize(A*Ve);

    //orthonormal basis [T1,T2,Vh]
    vec3 T1,T2;
    orthonormal_basis(T1,T2,Vh);

    //sample projected area of hemisphere
    FLOAT radius = std::sqrt(random.GetNormalizedFloat());
    FLOAT phi = 2*PI*random.GetNormalizedFloat();
    FLOAT t1 = radius*std::cos(phi);
    FLOAT t2 = radius*std::sin(phi);
    FLOAT s = 0.5*(1+dot(Vh,vec3({0,0,1})));
    t2 = (1-s)*std::sqrt(1-t1*t1)+s*t2;

    //sample a point Ph on hemisphere based on t1,t2
    const vec3& Ph = T1*t1+T2*t2+Vh*std::sqrt(std::max(0.0,1-t1*t1-t2*t2));

    //Pe = inverse(A)*Ph
    //Ne = normalize(A*A*Pe) = normalize(A*A*inverse(A)*Ph) = normalize(A*Ph)
    const vec3& Ne = normalize(A*Ph);
    return Ne;
}

FLOAT smith_ggx_lambda(const vec3& omega,FLOAT ani_x,FLOAT ani_y){
    FLOAT tx = ani_x*ani_x*omega[0]*omega[0];
    FLOAT ty = ani_y*ani_y*omega[1]*omega[1];
    FLOAT tz = omega[2]*omega[2];
    return 0.5*(-1+std::sqrt(1+(tx+ty)/tz));
}

FLOAT smith_ggx_G1(const vec3& omega,FLOAT ani_x,FLOAT ani_y){
    return 1.0/(1.0+smith_ggx_lambda(omega,ani_x,ani_y));
}

FLOAT smith_ggx_G2(const vec3& omega_i,const vec3& omega_o,FLOAT ani_x,FLOAT ani_y){
    return 1.0/(1.0+smith_ggx_lambda(omega_i,ani_x,ani_y)+smith_ggx_lambda(omega_o,ani_x,ani_y));
}

