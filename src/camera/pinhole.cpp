#include "camera/pinhole.hpp"

PinholeCamera::PinholeCamera(size_t sensor_width,
                             size_t sensor_height,
                             const vec3& sensor_origin,
                             const vec3& sensor_target,
                             FLOAT rotation,
                             FLOAT sensor_pinhole_length)
{
    m_sensor_width = sensor_width;
    m_sensor_height = sensor_height;
    m_sensor_origin = sensor_origin;
    m_sensor_pinhole_length = sensor_pinhole_length;

    //camera axis(right handed system)
    m_az = normalize(sensor_target-sensor_origin)*(-1);
    orthonormal_basis(m_ax,m_ay,m_az);
    FLOAT angle = (PI/180.0)*rotation;
    m_ax = m_ax*std::cos(angle)+m_ay*std::sin(angle);
    m_ay = cross(m_az,m_ax);
}

void PinholeCamera::CreateRay(Ray& ray,size_t row,size_t column,Random& random) const{
    //sensor size
    size_t sensor_size = std::max(m_sensor_width,m_sensor_height);
    
    //sensor position 2d
    vec2 sensor_pos_2d;
    sensor_pos_2d[0] = (column+random.GetNormalizedFloat())*(1.0/sensor_size);
    sensor_pos_2d[1] = (m_sensor_height-1-row+random.GetNormalizedFloat())*(1.0/sensor_size);
    sensor_pos_2d[0] = 2*sensor_pos_2d[0]-1.0*m_sensor_width/sensor_size;
    sensor_pos_2d[1] = 2*sensor_pos_2d[1]-1.0*m_sensor_height/sensor_size;

    //sensor position 3d
    const vec3& sensor_pos_3d = m_sensor_origin+m_ax*sensor_pos_2d[0]+m_ay*sensor_pos_2d[1];

    //pinhole position
    const vec3& pinhole_pos = m_sensor_origin+m_az*(-m_sensor_pinhole_length);

    //create ray
    ray.ori = sensor_pos_3d;
    ray.dir = normalize(pinhole_pos-sensor_pos_3d);
}
