#ifndef THINLENS_HPP
#define THINLENS_HPP

#include "camera/camera_abstract.hpp"

class ThinlensCamera:public Camera{
private:
    FLOAT m_sensor_lens_length;
    FLOAT m_lens_plane_length;
    FLOAT m_lens_radius;
public:
    ThinlensCamera(size_t sensor_width,
                   size_t sensor_height,
                   const vec3& sensor_origin,
                   const vec3& sensor_target,
                   FLOAT rotation,
                   FLOAT sensor_lens_length,
                   FLOAT lens_radius);
    void CreateRay(Ray& ray,size_t row,size_t column,Random& random) const override;
};

#endif // THINLENS_HPP
