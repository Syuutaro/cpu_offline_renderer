#ifndef INTEGRATOR_ABSTRACT_HPP
#define INTEGRATOR_ABSTRACT_HPP

#include <vector>
#include "core/define.hpp"
#include "core/ray.hpp"
#include "core/hitpoint.hpp"
#include "utility/matrix.hpp"
#include "utility/random.hpp"
#include "primitive/primitive.hpp"
#include "bsdf/bsdf.hpp"
#include "light/light.hpp"
#include "acceleration/acceleration.hpp"

class Integrator{
public:
    virtual ~Integrator(){}
    virtual void Calculate(vec3& contribution,
                           const Ray& ray,
                           const std::vector<Primitive*>& primitives,
                           Random& random) const = 0;
    virtual void Calculate(vec3& contribution,
                           const Ray& ray,
                           const Acceleration* acceleration,
                           Random& random) const = 0;
};


#endif // INTEGRATOR_ABSTRACT_HPP
