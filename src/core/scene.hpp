#ifndef SCENE_HPP
#define SCENE_HPP

#include <vector>
#include <string>
#include <map>
#include <cmath>
#include <ctime>
#include <thread>
#include "parser/json.hpp"
#include "parser/obj.hpp"
#include "core/define.hpp"
#include "utility/matrix.hpp"
#include "utility/texture.hpp"
#include "utility/random.hpp"
#include "bsdf/bsdf.hpp"
#include "light/light.hpp"
#include "primitive/primitive.hpp"
#include "acceleration/acceleration.hpp"
#include "camera/camera.hpp"
#include "integrator/integrator.hpp"


class Scene{
private:
    std::string m_asset_dir_path;
    std::map<std::string,OBJ::Model*> m_models;
    std::map<std::string,Texture*> m_textures;
    std::map<std::string,BSDF*> m_bsdfs;
    std::map<std::string,Light*> m_lights;
    std::vector<Primitive*> m_primitives;
    Acceleration* m_acceleration;
    Camera* m_camera;
    Integrator* m_integrator;
    size_t m_sample_per_pixel;
    size_t m_rendering_time;
    size_t m_rendering_interval;
    size_t m_thread_count;
public:
    Scene();
    ~Scene();
    void Create(const std::string& asset_dir_path);
    void Render();
private:
    void CreateModel(const JSON::Node* root_node);
    void CreateTexture(const JSON::Node* root_node);
    void CreateBSDF(const JSON::Node* root_node);
    void CreateLight(const JSON::Node* root_node);
    void CreateObject(const JSON::Node* root_node);
    void CreateCamera(const JSON::Node* root_node);
    void CreateAcceleration(const JSON::Node* root_node);
    void CreateIntegrator(const JSON::Node* root_node);
    void CreateRendering(const JSON::Node* root_node);
};



#endif // SCENE_HPP
