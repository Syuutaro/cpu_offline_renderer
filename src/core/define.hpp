#ifndef DEFINE_HPP
#define DEFINE_HPP

#include "utility/matrix.hpp"

//typedef
typedef double FLOAT;
typedef Vector<FLOAT,2> vec2;
typedef Vector<FLOAT,3> vec3;
typedef Vector<FLOAT,4> vec4;
typedef Matrix<FLOAT,2> mat2;
typedef Matrix<FLOAT,3> mat3;
typedef Matrix<FLOAT,4> mat4;

//constant
const FLOAT PI = 3.14159;
const FLOAT EPSILON = 1e-5;
const size_t MAX_TRACE_COUNT = 10;
const FLOAT GAMMA = 2.2;

#endif // DEFINE_HPP

