#ifndef PARALLELOGRAM_HPP
#define PARALLELOGRAM_HPP

#include "primitive/primitive_abstract.hpp"

class Parallelogram:public Primitive{
private:
    vec3 m_xyz1;//world
    vec3 m_xyz2;//world
    vec3 m_xyz3;//world
    vec2 m_uv1;
    vec2 m_uv2;
    vec2 m_uv3;
    vec3 m_normal;
    vec3 m_tangent;
    vec3 m_binormal;
    const BSDF* m_bsdf;
    const Light* m_light;
public:
    Parallelogram(const vec3& xyz1,//local
                  const vec3& xyz2,//local
                  const vec3& xyz3,//local
                  const vec2& uv1,
                  const vec2& uv2,
                  const vec2& uv3,
                  const vec3& origin,
                  const vec3& scale,
                  const vec4& attitude,
                  const BSDF* bsdf,
                  const Light* light);
    bool Hit(Hitpoint& hp,const Ray& ray) const override;
    void CreateAABB(AABB& aabb) const override;
};


#endif // PARALLELOGRAM_HPP
