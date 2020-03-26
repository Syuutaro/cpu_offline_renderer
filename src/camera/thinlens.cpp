#include "camera/thinlens.hpp"

ThinlensCamera::ThinlensCamera(size_t sensor_width,
                               size_t sensor_height,
                               const vec3& sensor_origin,
                               const vec3& sensor_target,
                               FLOAT rotation,
                               FLOAT sensor_lens_length,
                               FLOAT lens_radius)
{
    m_sensor_width = sensor_width;
    m_sensor_height = sensor_height;
    m_sensor_origin = sensor_origin;
    m_sensor_lens_length = sensor_lens_length;
    m_lens_radius = lens_radius;
    m_lens_plane_length = length(sensor_target-sensor_origin)-sensor_lens_length;

    //camera axis(right handed system)
    m_az = normalize(sensor_target-sensor_origin)*(-1);
    orthonormal_basis(m_ax,m_ay,m_az);
    FLOAT angle = (PI/180.0)*rotation;
    m_ax = m_ax*std::cos(angle)+m_ay*std::sin(angle);
    m_ay = cross(m_az,m_ax);
}

void ThinlensCamera::CreateRay(Ray& ray,size_t row,size_t column,Random& random) const{
    //sensor size
    size_t sensor_size = std::max(m_sensor_width,m_sensor_height);
    
    //lens center position
    const vec3& lens_center = m_sensor_origin+m_az*(-m_sensor_lens_length);
    
    //sensor position 2d
    vec2 sensor_pos_2d;
    sensor_pos_2d[0] = (column+random.GetNormalizedFloat())*(1.0/sensor_size);
    sensor_pos_2d[1] = (m_sensor_height-1-row+random.GetNormalizedFloat())*(1.0/sensor_size);
    sensor_pos_2d[0] = 2*sensor_pos_2d[0]-1.0*m_sensor_width/sensor_size;
    sensor_pos_2d[1] = 2*sensor_pos_2d[1]-1.0*m_sensor_height/sensor_size;

    //sensor position 3d
    const vec3& sensor_pos_3d = m_sensor_origin+m_ax*sensor_pos_2d[0]+m_ay*sensor_pos_2d[1];

    //lens position
    FLOAT radius = m_lens_radius*std::sqrt(random.GetNormalizedFloat());
    FLOAT theta = 2*PI*random.GetNormalizedFloat();
    const vec3& lens_pos = lens_center+m_ax*radius*std::cos(theta)+m_ay*radius*std::sin(theta);

    //plane position
    const vec3& plane_pos = sensor_pos_3d
                            +(lens_center-sensor_pos_3d)
                            *(1+m_lens_plane_length/m_sensor_lens_length);

    //create ray
    ray.ori = lens_pos;
    ray.dir = normalize(plane_pos-lens_pos);
}
