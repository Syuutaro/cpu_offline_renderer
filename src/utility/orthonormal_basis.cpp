#include "utility/orthonormal_basis.hpp"

void orthonormal_basis(vec3& ax,vec3& ay,const vec3& az){
    if (az == vec3({0,0,1})){
        ax = vec3({1,0,0});
        ay = vec3({0,1,0});
    }else if (az == vec3({0,0,-1})){
        ax = vec3({1,0,0});
        ay = vec3({0,-1,0});
    }else{
        ax = normalize(cross(vec3({0,0,1}),az));
        ay = cross(az,ax);
    }
}
