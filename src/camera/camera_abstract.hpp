#ifndef CAMERA_ABSTRACT_HPP
#define CAMERA_ABSTRACT_HPP

#include <algorithm>
#include "core/define.hpp"
#include "core/ray.hpp"
#include "utility/matrix.hpp"
#include "utility/orthonormal_basis.hpp"
#include "utility/random.hpp"

class Camera{
protected:
    size_t m_sensor_width;
    size_t m_sensor_height;
    vec3 m_sensor_origin;
    vec3 m_ax;
    vec3 m_ay;
    vec3 m_az;
public:
    virtual ~Camera(){}
    virtual void CreateRay(Ray& ray,size_t row,size_t column,Random& random) const = 0;
    size_t GetSensorWidth() const{
        return m_sensor_width;
    }
    size_t GetSensorHeight() const{
        return m_sensor_height;
    }
};


#endif // CAMERA_ABSTRACT_HPP
