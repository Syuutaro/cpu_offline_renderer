#include "primitive/sphere.hpp"

Sphere::Sphere(FLOAT radius,
               const vec3& origin,
               const vec4& attitude,
               const BSDF* bsdf,
               const Light* light)
{
    m_radius = radius;
    m_origin = origin;
    m_bsdf = bsdf;
    m_light = light;

    //local axis in world coordinate
    m_az = normalize(vec3(attitude));
    orthonormal_basis(m_ax,m_ay,m_az);

    //rotate ax
    FLOAT angle = (PI/180.0)*attitude[3];
    m_ax = m_ax*std::cos(angle)+m_ay*std::sin(angle);
    m_ay = cross(m_az,m_ax);
}

bool Sphere::Hit(Hitpoint& hp,const Ray& ray) const{
    //t
    //(o+t*d-c,o+t*d-c) <= r^2
    //k1 = (d,o-c),k2 = (o-c,o-c)
    //t^2+2*k1*t+k2 <= r^2
    FLOAT k1 = dot(ray.dir,ray.ori-m_origin);
    FLOAT k2 = dot(ray.ori-m_origin,ray.ori-m_origin);
    FLOAT D = m_radius*m_radius+k1*k1-k2;
    if (D < 0){return false;}
    FLOAT root_D = std::sqrt(D);
    FLOAT t_min = -k1-root_D;
    FLOAT t_max = -k1+root_D;
    FLOAT t;
    if (t_min > 0){
        t = t_min;
    }else{
        if (t_max > 0){
            t = t_max;
        }else{
            return false;
        }
    }

    //world xyz
    const vec3& world_xyz = ray.ori+ray.dir*t;

    //local xyz for sampling uv
    //world_xyz = origin+mat3(ax,ay,az)*local_xyz
    const mat3& A = mat3({m_ax,m_ay,m_az});
    const vec3& local_xyz = transpose(A)*(world_xyz-m_origin);

    //spherical coordinate (phi,theta)
    //pos = (cos_phi*sin_theta,sin_phi*sin_theta,cos_theta)
    const vec3& pos = local_xyz/m_radius;
    FLOAT cos_theta = std::max(std::min(pos[2],1.0),-1.0);
    FLOAT cos_phi = std::max(std::min(normalize(vec2({pos[0],pos[1]}))[0],1.0),-1.0);
    FLOAT theta = std::acos(cos_theta);
    FLOAT phi = std::acos(cos_phi);

    //uv
    vec2 uv;
    uv[0] = (pos[1] > 0)? phi/(2*PI):(-phi+2*PI)/(2*PI);
    uv[1] = 1-theta/PI;

    //tangent space in local coordinate
    vec3 normal,tangent,binormal;
    normal = pos;
    orthonormal_basis(tangent,binormal,normal);

    //tangent space in world coordinate
    normal = A*normal;
    tangent = A*tangent;
    binormal = A*binormal;

    //result
    hp.t = t;
    hp.xyz = world_xyz;
    hp.uv = uv;
    hp.normal = normal;
    hp.tangent = tangent;
    hp.binormal = binormal;
    hp.bsdf = m_bsdf;
    hp.light = m_light;
    return true;
}

void Sphere::CreateAABB(AABB& aabb) const{
    FLOAT range_x[2],range_y[2],range_z[2];
    range_x[0] = m_origin[0]-m_radius;
    range_x[1] = m_origin[0]+m_radius;
    range_y[0] = m_origin[1]-m_radius;
    range_y[1] = m_origin[1]+m_radius;
    range_z[0] = m_origin[2]-m_radius;
    range_z[1] = m_origin[2]+m_radius;
    aabb.SetRange(range_x,range_y,range_z);
}
