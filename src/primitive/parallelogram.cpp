#include "primitive/parallelogram.hpp"

Parallelogram::Parallelogram(const vec3& xyz1,
                             const vec3& xyz2,
                             const vec3& xyz3,
                             const vec2& uv1,
                             const vec2& uv2,
                             const vec2& uv3,
                             const vec3& origin,
                             const vec3& scale,
                             const vec4& attitude,
                             const BSDF* bsdf,
                             const Light* light)
{
    m_uv1 = uv1;
    m_uv2 = uv2;
    m_uv3 = uv3;
    m_bsdf = bsdf;
    m_light = light;

    //local axis in world coordinate
    vec3 ax,ay,az;
    az = normalize(vec3(attitude));
    orthonormal_basis(ax,ay,az);

    //rotate ax
    FLOAT angle = (PI/180.0)*attitude[3];
    ax = ax*std::cos(angle)+ay*std::sin(angle);
    ay = cross(az,ax);

    //scale axis
    ax *= scale[0];
    ay *= scale[1];
    az *= scale[2];

    //tranform local xyz to world coordinate
    m_xyz1 = origin+ax*xyz1[0]+ay*xyz1[1]+az*xyz1[2];
    m_xyz2 = origin+ax*xyz2[0]+ay*xyz2[1]+az*xyz2[2];
    m_xyz3 = origin+ax*xyz3[0]+ay*xyz3[1]+az*xyz3[2];

    //temporary tangent space
    m_normal = normalize(cross(m_xyz2-m_xyz1,m_xyz3-m_xyz1));
    m_tangent = normalize(m_xyz2-m_xyz1);
    m_binormal = cross(m_normal,m_tangent);

    //angle of uv21 and axis u
    const vec2& uv21 = normalize(m_uv2-m_uv1);
    FLOAT cos_theta = std::min(std::max(uv21[0],-1.0),1.0);
    FLOAT theta = (uv21[1] > 0)? acos(cos_theta):-acos(cos_theta);

    //correct tangent space
    m_tangent = m_tangent*std::cos(-theta)+m_binormal*std::sin(-theta);
    m_binormal = cross(m_normal,m_tangent);

}

bool Parallelogram::Hit(Hitpoint& hp,const Ray& ray) const{
    //solve linear equation
    //o+t*d = xyz1+x1*e1+x2*e2 <-> o-xyz1 = mat3(e1,e2,-d)*vec3(x1,x2,t)
    const vec3& e1 = m_xyz2-m_xyz1;
    const vec3& e2 = m_xyz3-m_xyz2;
    const vec3& x = cramer(mat3({e1,e2,ray.dir*(-1)}),ray.ori-m_xyz1);

    //check hit
    if (x[0] >= 0 && x[1] >= 0 && x[0] <= 1 && x[1] <= 1 && x[2] > 0){
        hp.t = x[2];
        hp.xyz = m_xyz1+e1*x[0]+e2*x[1];
        hp.uv = m_uv1+(m_uv2-m_uv1)*x[0]+(m_uv3-m_uv2)*x[1];
        hp.normal = m_normal;
        hp.tangent = m_tangent;
        hp.binormal = m_binormal;
        hp.bsdf = m_bsdf;
        hp.light = m_light;
        return true;
    }else{
        return false;
    }
}

void Parallelogram::CreateAABB(AABB& aabb) const{
    const vec3& xyz4 = m_xyz1+(m_xyz3-m_xyz2);
    FLOAT range_x[2],range_y[2],range_z[2];
    range_x[0] = std::min(xyz4[0],std::min(m_xyz1[0],std::min(m_xyz2[0],m_xyz3[0])));
    range_x[1] = std::max(xyz4[0],std::max(m_xyz1[0],std::max(m_xyz2[0],m_xyz3[0])));
    range_y[0] = std::min(xyz4[1],std::min(m_xyz1[1],std::min(m_xyz2[1],m_xyz3[1])));
    range_y[1] = std::max(xyz4[1],std::max(m_xyz1[1],std::max(m_xyz2[1],m_xyz3[1])));
    range_z[0] = std::min(xyz4[2],std::min(m_xyz1[2],std::min(m_xyz2[2],m_xyz3[2])));
    range_z[1] = std::max(xyz4[2],std::max(m_xyz1[2],std::max(m_xyz2[2],m_xyz3[2])));
    aabb.SetRange(range_x,range_y,range_z);
}
