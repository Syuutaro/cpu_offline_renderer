#ifndef PRIMITIVE_ABSTRACT_HPP
#define PRIMITIVE_ABSTRACT_HPP

#include <algorithm>
#include <cmath>
#include "utility/matrix.hpp"
#include "utility/orthonormal_basis.hpp"
#include "core/ray.hpp"
#include "core/hitpoint.hpp"
#include "acceleration/aabb.hpp"


class BSDF;
class Light;

class Primitive{
public:
    virtual ~Primitive(){}
    virtual bool Hit(Hitpoint& hp,const Ray& ray) const = 0;
    virtual void CreateAABB(AABB& aabb) const = 0;
};


#endif // PRIMITIVE_ABSTRACT_HPP
