#ifndef BSDF_ABSTRACT_HPP
#define BSDF_ABSTRACT_HPP

#include <cmath>
#include <algorithm>
#include "core/define.hpp"
#include "core/ray.hpp"
#include "core/hitpoint.hpp"
#include "utility/matrix.hpp"
#include "utility/texture.hpp"
#include "utility/orthonormal_basis.hpp"
#include "utility/random.hpp"

//ray incident direction = photon outgoing direction(omega_o)
//ray outgoing direction = photon incident direction(omega_i)
//ray_i = incident ray
//ray_o = outgoing ray
//eta_i = refractive index of incident photon side
//eta_o = rafractive index of outgoing photon side
//snell's law
    //0 <= theta_i,theta_o <= PI/2
    //eta_i*sin_theta_i = eta_o*sin_theta_o
    //sin_theta_o = eta_i/eta_o*sin_theta_i
//total reflection
    // 0 <= sin_theta_o <= 1
    //<->
    // 0 <= eta_i/eta_o*sin_theta_i <= 1
    //<->
    // 0 <= eta_i*sin_theta_i <= eta_o


class BSDF{
public:
    virtual ~BSDF(){}
    virtual void Sample(vec3& wgt,Ray& ray_o,const Ray& ray_i,const Hitpoint& hp,Random& random) const = 0;
};

#endif // BSDF_ABSTRACT_HPP
