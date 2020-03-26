#ifndef SPHERE_HPP
#define SPHERE_HPP

#include "primitive/primitive_abstract.hpp"

class Sphere:public Primitive{
private:
    FLOAT m_radius;
    vec3 m_origin;//local origin in world coordinate
    vec3 m_ax;//local axis x in world coordinate
    vec3 m_ay;//local axis y in world coordinate
    vec3 m_az;//local axis z in world coordinate
    const BSDF* m_bsdf;
    const Light* m_light;
public:
    Sphere(FLOAT radius,
           const vec3& origin,
           const vec4& attitude,
           const BSDF* bsdf,
           const Light* light);
    bool Hit(Hitpoint& hp,const Ray& ray) const override;
    void CreateAABB(AABB& aabb) const override;
};


#endif // SPHERE_HPP
