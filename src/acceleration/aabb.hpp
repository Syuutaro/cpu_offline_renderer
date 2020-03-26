#ifndef AABB_HPP
#define AABB_HPP

#include <cfloat>
#include <algorithm>
#include "core/define.hpp"
#include "core/ray.hpp"

class AABB{
private:
    FLOAT m_range_x[2];
    FLOAT m_range_y[2];
    FLOAT m_range_z[2];
public:
    AABB();
    bool Hit(const Ray& ray) const;
    void Merge(const AABB& aabb1,const AABB& aabb2);
    void CalCenter(FLOAT center[3]) const;
    void CalSize(FLOAT size[3]) const;
    void GetRange(FLOAT range_x[2],FLOAT range_y[2],FLOAT range_z[2]) const;
    void SetRange(FLOAT range_x[2],FLOAT range_y[2],FLOAT range_z[2]);
    FLOAT CalSurfaceArea() const;
};


#endif // AABB_HPP



