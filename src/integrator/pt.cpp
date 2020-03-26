#include "integrator/pt.hpp"

void PT::Calculate(vec3& contribution,
                   const Ray& ray,
                   const std::vector<Primitive*>& primitives,
                   Random& random) const
{
    vec3 wgt = vec3({1,1,1});
    Ray ray_i = ray;
    Ray ray_o;
    contribution = vec3();
    for (size_t i = 0;i < MAX_TRACE_COUNT;i++){
        //search nearest primitive
        Hitpoint hp;
        hp.t = -1;
        for (size_t i = 0;i < primitives.size();i++){
            Hitpoint tmp;
            if (primitives[i]->Hit(tmp,ray_i)){
                if (hp.t != -1){
                    if (hp.t > tmp.t){
                        hp = tmp;
                    }
                }else{
                    hp = tmp;
                }
            }
        }
        if (hp.t == -1){
            return;
        }

        //sample light
        if (hp.light != nullptr){
            vec3 r;
            hp.light->Sample(r,hp);
            contribution += wgt*r;
        }else{
            return;
        }

        //sample bsdf
        if (hp.bsdf != nullptr){
            vec3 w;
            hp.bsdf->Sample(w,ray_o,ray_i,hp,random);
            wgt *= w;
        }else{
            return;
        }

        //update ray
        ray_i = ray_o;
    }
}


void PT::Calculate(vec3& contribution,
                   const Ray& ray,
                   const Acceleration* acceleration,
                   Random& random) const
{
    vec3 wgt = vec3({1,1,1});
    Ray ray_i = ray;
    Ray ray_o;
    contribution = vec3();
    for (size_t i = 0;i < MAX_TRACE_COUNT;i++){
        //search nearest primitive
        Hitpoint hp;
        if (!acceleration->Traversal(hp,ray_i)){
            return;
        }

        //sample light
        if (hp.light != nullptr){
            vec3 r;
            hp.light->Sample(r,hp);
            contribution += wgt*r;
        }else{
            return;
        }

        //sample bsdf
        if (hp.bsdf != nullptr){
            vec3 w;
            hp.bsdf->Sample(w,ray_o,ray_i,hp,random);
            wgt *= w;
        }else{
            return;
        }

        //update ray
        ray_i = ray_o;
    }
}

