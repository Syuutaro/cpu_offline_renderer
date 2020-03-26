#include "acceleration/aabb.hpp"

AABB::AABB(){
    m_range_x[0] = 0;
    m_range_x[1] = 0;
    m_range_y[0] = 0;
    m_range_y[1] = 0;
    m_range_z[0] = 0;
    m_range_z[1] = 0;
}

bool AABB::Hit(const Ray& ray) const{
    //tx range of o+t*d
    FLOAT range_tx[2];
    if (ray.dir[0] == 0){
        if (ray.ori[0] >= m_range_x[0] && ray.ori[0] <= m_range_x[1]){
            range_tx[0] = 0;
            range_tx[1] = FLT_MAX;
        }else{
            return false;
        }
    }else{
        FLOAT t1 = (m_range_x[0]-ray.ori[0])/ray.dir[0];
        FLOAT t2 = (m_range_x[1]-ray.ori[0])/ray.dir[0];
        range_tx[0] = std::min(t1,t2);
        range_tx[1] = std::max(t1,t2);
    }

    //ty range of o+t*d
    FLOAT range_ty[2];
    if (ray.dir[1] == 0){
        if (ray.ori[1] >= m_range_y[0] && ray.ori[1] <= m_range_y[1]){
            range_ty[0] = 0;
            range_ty[1] = FLT_MAX;
        }else{
            return false;
        }
    }else{
        FLOAT t1 = (m_range_y[0]-ray.ori[1])/ray.dir[1];
        FLOAT t2 = (m_range_y[1]-ray.ori[1])/ray.dir[1];
        range_ty[0] = std::min(t1,t2);
        range_ty[1] = std::max(t1,t2);
    }

    //tz range of o+t*d
    FLOAT range_tz[2];
    if (ray.dir[2] == 0){
        if (ray.ori[2] >= m_range_z[0] && ray.ori[2] <= m_range_z[1]){
            range_tz[0] = 0;
            range_tz[1] = FLT_MAX;
        }else{
            return false;
        }
    }else{
        FLOAT t1 = (m_range_z[0]-ray.ori[2])/ray.dir[2];
        FLOAT t2 = (m_range_z[1]-ray.ori[2])/ray.dir[2];
        range_tz[0] = std::min(t1,t2);
        range_tz[1] = std::max(t1,t2);
    }

    //common range of t ranges
    FLOAT range_t[2];
    range_t[0] = std::max(range_tx[0],std::max(range_ty[0],range_tz[0]));
    range_t[1] = std::min(range_tx[1],std::min(range_ty[1],range_tz[1]));
    if (range_t[1] > 0 && range_t[0] <= range_t[1]){
        return true;
    }else{
        return false;
    }
}

void AABB::Merge(const AABB& aabb1,const AABB& aabb2){
    FLOAT aabb1_range_x[2],aabb1_range_y[2],aabb1_range_z[2];
    FLOAT aabb2_range_x[2],aabb2_range_y[2],aabb2_range_z[2];
    aabb1.GetRange(aabb1_range_x,aabb1_range_y,aabb1_range_z);
    aabb2.GetRange(aabb2_range_x,aabb2_range_y,aabb2_range_z);
    m_range_x[0] = std::min(aabb1_range_x[0],aabb2_range_x[0]);
    m_range_x[1] = std::max(aabb1_range_x[1],aabb2_range_x[1]);
    m_range_y[0] = std::min(aabb1_range_y[0],aabb2_range_y[0]);
    m_range_y[1] = std::max(aabb1_range_y[1],aabb2_range_y[1]);
    m_range_z[0] = std::min(aabb1_range_z[0],aabb2_range_z[0]);
    m_range_z[1] = std::max(aabb1_range_z[1],aabb2_range_z[1]);
}

void AABB::CalCenter(FLOAT center[3]) const{
    center[0] = 0.5*(m_range_x[0]+m_range_x[1]);
    center[1] = 0.5*(m_range_y[0]+m_range_y[1]);
    center[2] = 0.5*(m_range_z[0]+m_range_z[1]);
}

void AABB::CalSize(FLOAT size[3]) const{
    size[0] = m_range_x[1]-m_range_x[0];
    size[1] = m_range_y[1]-m_range_y[0];
    size[2] = m_range_z[1]-m_range_z[0];
}

void AABB::GetRange(FLOAT range_x[2],FLOAT range_y[2],FLOAT range_z[2]) const{
    range_x[0] = m_range_x[0];
    range_x[1] = m_range_x[1];
    range_y[0] = m_range_y[0];
    range_y[1] = m_range_y[1];
    range_z[0] = m_range_z[0];
    range_z[1] = m_range_z[1];
}

void AABB::SetRange(FLOAT range_x[2],FLOAT range_y[2],FLOAT range_z[2]){
    m_range_x[0] = range_x[0];
    m_range_x[1] = range_x[1];
    m_range_y[0] = range_y[0];
    m_range_y[1] = range_y[1];
    m_range_z[0] = range_z[0];
    m_range_z[1] = range_z[1];
}

FLOAT AABB::CalSurfaceArea() const{
    FLOAT lx = m_range_x[1]-m_range_x[0];
    FLOAT ly = m_range_y[1]-m_range_y[0];
    FLOAT lz = m_range_z[1]-m_range_z[0];
    return 2*(lx*ly+ly*lz+lz*lx);
}


