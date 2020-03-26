#ifndef ACCELERATION_ABSTRACT_HPP
#define ACCELERATION_ABSTRACT_HPP

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <exception>
#include <cstdint>
#include "core/ray.hpp"
#include "core/hitpoint.hpp"
#include "utility/bit.hpp"
#include "primitive/primitive.hpp"
#include "acceleration/aabb.hpp"

class Acceleration{
public:
    virtual ~Acceleration(){}
    virtual void Construct(const std::vector<Primitive*>& primitives) = 0;
    virtual bool Traversal(Hitpoint& hp,const Ray& ray) const = 0;
};

#endif // ACCELERATION_ABSTRACT_HPP
