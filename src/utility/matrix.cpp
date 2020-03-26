#include "utility/matrix.hpp"

template <>
float determinant<2>(const Matrix<float,2>& m){
    return m.GetComponent(0,0)*m.GetComponent(1,1)
          -m.GetComponent(0,1)*m.GetComponent(1,0);
}

template <>
double determinant<2>(const Matrix<double,2>& m){
    return m.GetComponent(0,0)*m.GetComponent(1,1)
          -m.GetComponent(0,1)*m.GetComponent(1,0);
}
