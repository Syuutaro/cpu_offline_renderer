#ifndef PT_HPP
#define PT_HPP


#include "integrator/integrator_abstract.hpp"

//path tracing
class PT:public Integrator{
public:
    void Calculate(vec3& contribution,
                   const Ray& ray,
                   const std::vector<Primitive*>& primitives,
                   Random& random) const override;
    void Calculate(vec3& contribution,
                   const Ray& ray,
                   const Acceleration* acceleration,
                   Random& random) const override;
};


#endif // PT_HPP
