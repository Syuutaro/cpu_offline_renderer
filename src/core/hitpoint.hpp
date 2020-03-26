#ifndef HITPOINT_HPP
#define HITPOINT_HPP

#include "core/define.hpp"
#include "utility/matrix.hpp"

class BSDF;
class Light;

struct Hitpoint{
    FLOAT t;
    vec3 xyz;
    vec2 uv;
    vec3 normal;
    vec3 tangent;
    vec3 binormal;
    const BSDF* bsdf;
    const Light* light;
};

#endif // HITPOINT_HPP
