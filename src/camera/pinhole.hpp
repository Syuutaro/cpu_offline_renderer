#ifndef PINHOLE_HPP
#define PINHOLE_HPP

#include "camera/camera_abstract.hpp"

class PinholeCamera:public Camera{
private:
    FLOAT m_sensor_pinhole_length;
public:
    PinholeCamera(size_t sensor_width,
                  size_t sensor_height,
                  const vec3& sensor_origin,
                  const vec3& sensor_target,
                  FLOAT rotation,
                  FLOAT sensor_pinhole_length);
    void CreateRay(Ray& ray,size_t row,size_t column,Random& random) const override;
};

#endif // PINHOLE_HPP
