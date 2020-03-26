#include "scene.hpp"

Scene::Scene(){
    m_camera = nullptr;
    m_acceleration = nullptr;
    m_integrator = nullptr;
}

Scene::~Scene(){
    for (auto ite = m_models.begin();ite != m_models.end();++ite){
        delete ite->second;
    }
    for (auto ite = m_textures.begin();ite != m_textures.end();++ite){
        delete ite->second;
    }
    for (auto ite = m_bsdfs.begin();ite != m_bsdfs.end();++ite){
        delete ite->second;
    }
    for (auto ite = m_lights.begin();ite != m_lights.end();++ite){
        delete ite->second;
    }
    for (size_t i = 0;i < m_primitives.size();i++){
        delete m_primitives[i];
    }
    delete m_camera;
    delete m_acceleration;
    delete m_integrator;
}

void Scene::Create(const std::string& asset_dir_path){
    //set asset dir path
    if (m_asset_dir_path.size() == 0){
        m_asset_dir_path = asset_dir_path;
    }else{
        return;
    }
    
    //read scene file
    JSON::Node* root_node = JSON::read(asset_dir_path+"/scene.json");

    //create scene
    CreateModel(root_node);
    CreateTexture(root_node);
    CreateBSDF(root_node);
    CreateLight(root_node);
    CreateObject(root_node);
    CreateCamera(root_node);
    CreateAcceleration(root_node);
    CreateIntegrator(root_node);
    CreateRendering(root_node);

    //release json node
    JSON::release(root_node);
}

static double reinhard(double x){
    return x/(x+1);
}

/*
static void render_on_multithreads(std::vector<vec3>& contributions,
                                   const std::vector<Primitive*>& primitives,
                                   const Camera* camera,
                                   const Integrator* integrator,
                                   Random& random,
                                   size_t iteration_count,
                                   size_t begin,
                                   size_t end)
{
    const size_t width = camera->GetSensorWidth();
    size_t count = 0;
    while (count < iteration_count){
        for (size_t i = begin;i <= end;i++){
            //camera pixel(row,column)
            const size_t column = i%width;
            const size_t row = (i-column)/width;
            
            //create ray from camera
            Ray ray;
            camera->CreateRay(ray,row,column,random);
            
            //integrate
            vec3 c;
            integrator->Calculate(c,ray,primitives,random);
            
            //add contribution
            contributions[i] += c;
        }
        count++;
    }
}
*/

static void render_on_multithreads(std::vector<vec3>& contributions,
                                   const Camera* camera,
                                   const Acceleration* acceleration,
                                   const Integrator* integrator,
                                   Random& random,
                                   size_t iteration_count,
                                   size_t begin,
                                   size_t end)
{
    const size_t width = camera->GetSensorWidth();
    size_t count = 0;
    while (count < iteration_count){
        for (size_t i = begin;i <= end;i++){
            //camera pixel(row,column)
            const size_t column = i%width;
            const size_t row = (i-column)/width;
            
            //create ray from camera
            Ray ray;
            camera->CreateRay(ray,row,column,random);
            
            //integrate
            vec3 c;
            integrator->Calculate(c,ray,acceleration,random);
            
            //add contribution
            contributions[i] += c;
        }
        count++;
    }
}



void Scene::Render(){
    //width,height
    const size_t width = m_camera->GetSensorWidth();
    const size_t height = m_camera->GetSensorHeight();
    
    //initialize contributions
    std::vector<vec3> contributions(width*height);
    for (size_t i = 0;i < contributions.size();i++){
        contributions[i] = vec3();
    }
    
    //thread count
    size_t thread_count = std::thread::hardware_concurrency();
    if (thread_count > m_thread_count){
        thread_count = m_thread_count;
    }
    std::vector<std::thread> threads(thread_count);
    
    //block size of one thread
    size_t block_size = (contributions.size()-contributions.size()%thread_count)/thread_count;
    
    //initialize randoms
    std::vector<Random> randoms(thread_count);
    for (size_t i = 0;i < randoms.size();i++){
        randoms[i] = Random(i);
    }

    //rendering
    size_t current_spp = 0;
    std::time_t start_time = std::time(NULL);
    while (1){
        //render on multithreads
        for (size_t i = 0;i < thread_count;i++){
            /*
            if (i != thread_count-1){
                threads[i] = std::thread(render_on_multithreads,
                                         std::ref(contributions),
                                         std::ref(m_primitives),
                                         m_camera,
                                         m_integrator,
                                         std::ref(randoms[i]),
                                         m_rendering_interval,
                                         block_size*i,
                                         block_size*(i+1)-1);
            }else{
                threads[i] = std::thread(render_on_multithreads,
                                         std::ref(contributions),
                                         std::ref(m_primitives),
                                         m_camera,
                                         m_integrator,
                                         std::ref(randoms[i]),
                                         m_rendering_interval,
                                         block_size*i,
                                         contributions.size()-1);
            }
            */
            
            if (i != thread_count-1){
                threads[i] = std::thread(render_on_multithreads,
                                         std::ref(contributions),
                                         m_camera,
                                         m_acceleration,
                                         m_integrator,
                                         std::ref(randoms[i]),
                                         m_rendering_interval,
                                         block_size*i,
                                         block_size*(i+1)-1);
            }else{
                threads[i] = std::thread(render_on_multithreads,
                                         std::ref(contributions),
                                         m_camera,
                                         m_acceleration,
                                         m_integrator,
                                         std::ref(randoms[i]),
                                         m_rendering_interval,
                                         block_size*i,
                                         contributions.size()-1);
            }
            
        }
        for (size_t i = 0;i < thread_count;i++){
            threads[i].join();
        }
        current_spp += m_rendering_interval;

        //output spp,time
        std::time_t current_time = std::time(NULL)-start_time;
        std::time_t ts = current_time%60;
        std::size_t tm = (current_time-ts)/60;
        std::cout << "current sample per pixel = " << current_spp
                  << ":current time = "            << tm
                  << "m"                           << ts
                  << "s"                           << "\n";

        //save image with gamma correction
        Texture result(width,height);
        for (size_t i = 0;i < width;i++){
            for (size_t j = 0;j < height;j++){
                size_t idx = j*width+i;
                RGB rgb;
                rgb.r = 255*std::pow(reinhard(contributions[idx][0]/current_spp),1/GAMMA);
                rgb.g = 255*std::pow(reinhard(contributions[idx][1]/current_spp),1/GAMMA);
                rgb.b = 255*std::pow(reinhard(contributions[idx][2]/current_spp),1/GAMMA);
                result.SetRGB(rgb,width-1-i,height-1-j);
            }
        }
        result.Save(m_asset_dir_path+"/result/spp"+std::to_string(current_spp)+".png");

        //break
        if (current_spp >= m_sample_per_pixel){
            break;
        }
        if (current_time >= m_rendering_time){
            break;
        }
    }
}





void Scene::CreateModel(const JSON::Node* root_node){
    //model
    if (!root_node->members.count("model")){
        return;
    }

    //create model
    const JSON::Node* model = root_node->members.at("model");
    for (size_t i = 0;i < model->elements.size();i++){
        const JSON::Node* model_i = model->elements[i];
        //id
        std::string id;
        if (model_i->members.count("id")){
            id = model_i->members.at("id")->data;
        }else{
            std::cout << "scene file:model:id must exist" << "\n";
            std::terminate();
        }

        //file name
        std::string file_name;
        if (model_i->members.count("file_name")){
            file_name = model_i->members.at("file_name")->data;
        }else{
            std::cout << "scene file:model:file_name must exist" << "\n";
            std::terminate();
        }

        //read obj file
        m_models[id] = new OBJ::Model();
        OBJ::read(*(m_models[id]),m_asset_dir_path+"/model/"+file_name);
    }
}

void Scene::CreateTexture(const JSON::Node* root_node){
    //texture
    if (!root_node->members.count("texture")){
        return;
    }

    //create texture
    const JSON::Node* texture = root_node->members.at("texture");
    for (size_t i = 0;i < texture->elements.size();i++){
        const JSON::Node* texture_i = texture->elements[i];
        //id
        std::string id;
        if (texture_i->members.count("id")){
            id = texture_i->members.at("id")->data;
        }else{
            std::cout << "scene file:texture:id must exist" << "\n";
            std::terminate();
        }

        //file name
        std::string file_name;
        if (texture_i->members.count("file_name")){
            file_name = texture_i->members.at("file_name")->data;
        }else{
            std::cout << "scene file:texture:file_name must exist" << "\n";
            std::terminate();
        }

        //read texture file
        m_textures[id] = new Texture();
        m_textures[id]->Load(m_asset_dir_path+"/texture/"+file_name);
    }
}

void Scene::CreateBSDF(const JSON::Node* root_node){
    //bsdf
    if (!root_node->members.count("bsdf")){
        std::cout << "scene file:bsdf must exist" << "\n";
        std::terminate();
    }

    //create bsdf
    const JSON::Node* bsdf = root_node->members.at("bsdf");
    for (size_t i = 0;i < bsdf->elements.size();i++){
        const JSON::Node* bsdf_i = bsdf->elements[i];

        //id
        std::string id;
        if (bsdf_i->members.count("id")){
            id = bsdf_i->members.at("id")->data;
        }else{
            std::cout << "scene file:bsdf:id must exist" << "\n";
            std::terminate();
        }

        //type
        std::string type;
        if (bsdf_i->members.count("type")){
            type = bsdf_i->members.at("type")->data;
        }else{
            std::cout << "scene file:bsdf:type must exist" << "\n";
            std::terminate();
        }

        //reflectance
        vec3 ref;
        if (bsdf_i->members.count("reflectance")){
            if (bsdf_i->members.at("reflectance")->elements.size() == 3){
                ref[0] = std::stof(bsdf_i->members.at("reflectance")->elements[0]->data);
                ref[1] = std::stof(bsdf_i->members.at("reflectance")->elements[1]->data);
                ref[2] = std::stof(bsdf_i->members.at("reflectance")->elements[2]->data);
            }else{
                std::cout << "scene file:bsdf:reflectance must have 3 components" << "\n";
                std::terminate();
            }
        }

        //eta
        FLOAT ieta,oeta;
        if (bsdf_i->members.count("inside_eta")){
            ieta = std::stof(bsdf_i->members.at("inside_eta")->data);
        }
        if (bsdf_i->members.count("outside_eta")){
            oeta = std::stof(bsdf_i->members.at("outside_eta")->data);
        }

        //anisotropy
        FLOAT ani_x,ani_y;
        if (bsdf_i->members.count("anisotropy_x")){
            ani_x = std::stof(bsdf_i->members.at("anisotropy_x")->data);
        }
        if (bsdf_i->members.count("anisotropy_y")){
            ani_y = std::stof(bsdf_i->members.at("anisotropy_y")->data);
        }

        //reflectance texture
        const Texture* ref_tex = nullptr;
        if (bsdf_i->members.count("reflectance_texture")){
            const std::string& ref_tex_id = bsdf_i->members.at("reflectance_texture")->data;
            if (m_textures.count(ref_tex_id)){
                ref_tex = m_textures[ref_tex_id];
            }else{
                std::cout << "scene file:bsdf:texture " << ref_tex_id << " does not exist" << "\n";
                std::terminate();
            }
        }

        //anisotropy_x texture
        const Texture* ani_x_tex = nullptr;
        if (bsdf_i->members.count("anisotropy_x_texture")){
            const std::string& ani_x_tex_id = bsdf_i->members.at("anisotropy_x_texture")->data;
            if (m_textures.count(ani_x_tex_id)){
                ani_x_tex = m_textures[ani_x_tex_id];
            }else{
                std::cout << "scene file:bsdf:texture " << ani_x_tex_id << " does not exist" << "\n";
                std::terminate();
            }
        }

        //anisotropy_y texture
        const Texture* ani_y_tex = nullptr;
        if (bsdf_i->members.count("anisotropy_y_texture")){
            const std::string& ani_y_tex_id = bsdf_i->members.at("anisotropy_y_texture")->data;
            if (m_textures.count(ani_y_tex_id)){
                ani_y_tex = m_textures[ani_y_tex_id];
            }else{
                std::cout << "scene file:bsdf:texture " << ani_y_tex_id << " does not exist" << "\n";
                std::terminate();
            }
        }

        //tangent texture
        const Texture* tan_tex = nullptr;
        if (bsdf_i->members.count("tangent_texture")){
            const std::string& tan_tex_id = bsdf_i->members.at("tangent_texture")->data;
            if (m_textures.count(tan_tex_id)){
                tan_tex = m_textures[tan_tex_id];
            }else{
                std::cout << "scene file:bsdf:texture " << tan_tex_id << " does not exist" << "\n";
                std::terminate();
            }
        }

        //create bsdf
        if (type == "ideal_diffuse"){
            m_bsdfs[id] = new IdealDiffuse(ref,ref_tex);
        }else if (type == "ideal_conductor"){
            m_bsdfs[id] = new IdealConductor(ref,ref_tex);
        }else if (type == "ideal_dielectric"){
            m_bsdfs[id] = new IdealDielectric(ref,ieta,oeta,ref_tex);
        }else if (type == "rough_diffuse"){
            m_bsdfs[id] = new RoughDiffuse(ref,ani_x,ani_y,ref_tex,ani_x_tex,ani_y_tex,tan_tex);
        }else if (type == "rough_conductor"){
            m_bsdfs[id] = new RoughConductor(ref,ani_x,ani_y,ref_tex,ani_x_tex,ani_y_tex,tan_tex);
        }else if (type == "rough_dielectric"){
            m_bsdfs[id] = new RoughDielectric(ref,ieta,oeta,ani_x,ani_y,ref_tex,ani_x_tex,ani_y_tex,tan_tex);
        }else{
            std::cout << "scene file:bsdf:type " << type << " is not supported" << "\n";
            std::terminate();
        }
    }
}

void Scene::CreateLight(const JSON::Node* root_node){
    //light
    if (!root_node->members.count("light")){
        std::cout << "scene file:light must exist" << "\n";
        std::terminate();
    }

    //create light
    const JSON::Node* light = root_node->members.at("light");
    for (size_t i = 0;i < light->elements.size();i++){
        const JSON::Node* light_i = light->elements[i];

        //id
        std::string id;
        if (light_i->members.count("id")){
            id = light_i->members.at("id")->data;
        }else{
            std::cout << "scene file:light:id must exist" << "\n";
            std::terminate();
        }

        //radiance
        vec3 rad;
        if (light_i->members.count("radiance")){
            if (light_i->members.at("radiance")->elements.size() == 3){
                rad[0] = std::stof(light_i->members.at("radiance")->elements[0]->data);
                rad[1] = std::stof(light_i->members.at("radiance")->elements[1]->data);
                rad[2] = std::stof(light_i->members.at("radiance")->elements[2]->data);
            }else{
                std::cout << "scene file::light:radiance must have 3 components" << "\n";
                std::terminate();
            }
        }

        //scale
        FLOAT scale;
        if (light_i->members.count("scale")){
            scale = std::stof(light_i->members.at("scale")->data);
        }

        //radiance texture
        const Texture* rad_tex = nullptr;
        if (light_i->members.count("radiance_texture")){
            const std::string& rad_tex_id = light_i->members.at("radiance_texture")->data;
            if (m_textures.count(rad_tex_id)){
                rad_tex = m_textures[rad_tex_id];
            }else{
                std::cout << "scene file:light:texture " << rad_tex_id << " does not exist" << "\n";
                std::terminate();
            }
        }

        //create light
        m_lights[id] = new Light(rad,scale,rad_tex);
    }
}

void Scene::CreateObject(const JSON::Node* root_node){
    //object
    if (!root_node->members.count("object")){
        std::cout << "scene file:object must exist" << "\n";
        std::terminate();
    }

    const JSON::Node* object = root_node->members.at("object");
    for (size_t i = 0;i < object->elements.size();i++){
        const JSON::Node* object_i = object->elements[i];

        //id
        std::string id;
        if (object_i->members.count("id")){
            id = object_i->members.at("id")->data;
        }else{
            std::cout << "scene file:object:id must exist" << "\n";
            std::terminate();
        }

        //type
        std::string type;
        if (object_i->members.count("type")){
            type = object_i->members.at("type")->data;
        }else{
            std::cout << "scene file:object:type must exist" << "\n";
            std::terminate();
        }

        //bsdf
        const BSDF* bsdf = nullptr;
        if (object_i->members.count("bsdf")){
            const std::string& bsdf_id = object_i->members.at("bsdf")->data;
            if (m_bsdfs.count(bsdf_id)){
                bsdf = m_bsdfs[bsdf_id];
            }else{
                std::cout << "scene file:object:bsdf " << bsdf_id << " does not exist" << "\n";
                std::terminate();
            }
        }else{
            std::cout << "scene file:object:bsdf must exist" << "\n";
            std::terminate();
        }

        //light
        const Light* light = nullptr;
        if (object_i->members.count("light")){
            const std::string& light_id = object_i->members.at("light")->data;
            if (m_lights.count(light_id)){
                light = m_lights[light_id];
            }else{
                std::cout << "scene file:object:light " << light_id << " does not exist" << "\n";
                std::terminate();
            }
        }else{
            std::cout << "scene file:object:light must exist" << "\n";
            std::terminate();
        }

        //model
        const OBJ::Model* model = nullptr;
        if (object_i->members.count("model")){
            const std::string& model_id = object_i->members.at("model")->data;
            if (m_models.count(model_id)){
                model = m_models[model_id];
            }else{
                std::cout << "scene file:object:model " << model_id << " does not exist" << "\n";
                std::terminate();
            }
        }else{
            if (type == "mesh"){
                std::cout << "scene file:object:mesh type object must have model" << "\n";
                std::terminate();
            }
        }

        //origin
        vec3 origin;
        if (object_i->members.count("origin")){
            if (object_i->members.at("origin")->elements.size() == 3){
                origin[0] = std::stof(object_i->members.at("origin")->elements[0]->data);
                origin[1] = std::stof(object_i->members.at("origin")->elements[1]->data);
                origin[2] = std::stof(object_i->members.at("origin")->elements[2]->data);
            }else{
                std::cout << "scene file:object:origin must have 3 components" << "\n";
                std::terminate();
            }
        }

        //scale
        vec3 scale;
        if (object_i->members.count("scale")){
            if (object_i->members.at("scale")->elements.size() == 3){
                scale[0] = std::stof(object_i->members.at("scale")->elements[0]->data);
                scale[1] = std::stof(object_i->members.at("scale")->elements[1]->data);
                scale[2] = std::stof(object_i->members.at("scale")->elements[2]->data);
            }else{
                std::cout << "scene file:object:scale must have 3 components" << "\n";
                std::terminate();
            }
        }

        //attitude(idx 0~2 is axis,idx 3 is degree measure angle)
        vec4 attitude;
        if (object_i->members.count("attitude")){
            if (object_i->members.at("attitude")->elements.size() == 4){
                attitude[0] = std::stof(object_i->members.at("attitude")->elements[0]->data);
                attitude[1] = std::stof(object_i->members.at("attitude")->elements[1]->data);
                attitude[2] = std::stof(object_i->members.at("attitude")->elements[2]->data);
                attitude[3] = std::stof(object_i->members.at("attitude")->elements[3]->data);
            }else{
                std::cout << "scene file:object:attitude must have 4 components" << "\n";
                std::terminate();
            }
        }

        //local coordinate position xyz
        vec3 xyz1,xyz2,xyz3;
        if (object_i->members.count("xyz1")){
            if (object_i->members.at("xyz1")->elements.size() == 3){
                xyz1[0] = std::stof(object_i->members.at("xyz1")->elements[0]->data);
                xyz1[1] = std::stof(object_i->members.at("xyz1")->elements[1]->data);
                xyz1[2] = std::stof(object_i->members.at("xyz1")->elements[2]->data);
            }else{
                std::cout << "scene file:object:xyz1 must have 3 components" << "\n";
                std::terminate();
            }
        }
        if (object_i->members.count("xyz2")){
            if (object_i->members.at("xyz2")->elements.size() == 3){
                xyz2[0] = std::stof(object_i->members.at("xyz2")->elements[0]->data);
                xyz2[1] = std::stof(object_i->members.at("xyz2")->elements[1]->data);
                xyz2[2] = std::stof(object_i->members.at("xyz2")->elements[2]->data);
            }else{
                std::cout << "scene file:object:xyz2 must have 3 components" << "\n";
                std::terminate();
            }
        }
        if (object_i->members.count("xyz3")){
            if (object_i->members.at("xyz3")->elements.size() == 3){
                xyz3[0] = std::stof(object_i->members.at("xyz3")->elements[0]->data);
                xyz3[1] = std::stof(object_i->members.at("xyz3")->elements[1]->data);
                xyz3[2] = std::stof(object_i->members.at("xyz3")->elements[2]->data);
            }else{
                std::cout << "scene file:object:xyz3 must have 3 components" << "\n";
                std::terminate();
            }
        }

        //texture coodinate position uv
        vec2 uv1,uv2,uv3;
        if (object_i->members.count("uv1")){
            if (object_i->members.at("uv1")->elements.size() == 2){
                uv1[0] = std::stof(object_i->members.at("uv1")->elements[0]->data);
                uv1[1] = std::stof(object_i->members.at("uv1")->elements[1]->data);
            }else{
                std::cout << "scene file:object:uv1 must have 2 components" << "\n";
                std::terminate();
            }
        }
        if (object_i->members.count("uv2")){
            if (object_i->members.at("uv2")->elements.size() == 2){
                uv2[0] = std::stof(object_i->members.at("uv2")->elements[0]->data);
                uv2[1] = std::stof(object_i->members.at("uv2")->elements[1]->data);
            }else{
                std::cout << "scene file:object:uv2 must have 2 components" << "\n";
                std::terminate();
            }
        }
        if (object_i->members.count("uv3")){
            if (object_i->members.at("uv3")->elements.size() == 2){
                uv3[0] = std::stof(object_i->members.at("uv3")->elements[0]->data);
                uv3[1] = std::stof(object_i->members.at("uv3")->elements[1]->data);
            }else{
                std::cout << "scene file:object:uv3 must have 2 components" << "\n";
                std::terminate();
            }
        }

        //radius
        FLOAT radius;
        if (object_i->members.count("radius")){
            radius = std::stof(object_i->members.at("radius")->data);
        }

        //create primitive
        if (type == "triangle"){
            m_primitives.push_back(new Triangle(xyz1,xyz2,xyz3,uv1,uv2,uv3,origin,scale,attitude,bsdf,light));
        }else if (type == "parallelogram"){
            m_primitives.push_back(new Parallelogram(xyz1,xyz2,xyz3,uv1,uv2,uv3,origin,scale,attitude,bsdf,light));
        }else if (type == "sphere"){
            m_primitives.push_back(new Sphere(radius,origin,attitude,bsdf,light));
        }else if (type == "mesh"){
            for (size_t j = 0;j < model->triangles.size();j++){
                Primitive* p = new Triangle(vec3(model->triangles[j].xyz1),
                                            vec3(model->triangles[j].xyz2),
                                            vec3(model->triangles[j].xyz3),
                                            vec2(model->triangles[j].uv1),
                                            vec2(model->triangles[j].uv2),
                                            vec2(model->triangles[j].uv3),
                                            origin,scale,attitude,bsdf,light);
                m_primitives.push_back(p);
            }
        }else{
            std::cout << "scene file:object:type " << type << " is not supported" << "\n";
            std::terminate();
        }
    }
}

void Scene::CreateCamera(const JSON::Node* root_node){
    //camera
    if (!root_node->members.count("camera")){
        std::cout << "scene file:camera must exist" << "\n";
        std::terminate();
    }
    const JSON::Node* camera = root_node->members.at("camera");

    //type
    std::string type;
    if (camera->members.count("type")){
        type = camera->members.at("type")->data;
    }else{
        std::cout << "scene file:camera:type must exist" << "\n";
        std::terminate();
    }

    //sensor size
    size_t sensor_width,sensor_height;
    if (camera->members.count("sensor_width") && camera->members.count("sensor_height")){
        sensor_width = std::stof(camera->members.at("sensor_width")->data);
        sensor_height = std::stof(camera->members.at("sensor_height")->data);
    }else{
        std::cout << "scene file:camera:sensor_width,sensor_height must exist" << "\n";
        std::terminate();
    }

    //sensor origin
    vec3 sensor_origin;
    if (camera->members.count("sensor_origin")){
        if (camera->members.at("sensor_origin")->elements.size() == 3){
            sensor_origin[0] = std::stof(camera->members.at("sensor_origin")->elements[0]->data);
            sensor_origin[1] = std::stof(camera->members.at("sensor_origin")->elements[1]->data);
            sensor_origin[2] = std::stof(camera->members.at("sensor_origin")->elements[2]->data);
        }else{
            std::cout << "scene file:camera:sensor_origin must have 3 components" << "\n";
            std::terminate();
        }
    }else{
        std::cout << "scene file:camera:sensor_origin must exist" << "\n";
        std::terminate();
    }

    //sensor target
    vec3 sensor_target;
    if (camera->members.count("sensor_target")){
        if (camera->members.at("sensor_target")->elements.size() == 3){
            sensor_target[0] = std::stof(camera->members.at("sensor_target")->elements[0]->data);
            sensor_target[1] = std::stof(camera->members.at("sensor_target")->elements[1]->data);
            sensor_target[2] = std::stof(camera->members.at("sensor_target")->elements[2]->data);
        }else{
            std::cout << "scene file:camera:sensor_target must have 3 components" << "\n";
            std::terminate();
        }
    }else{
        std::cout << "scene file:camera:sensor_target must exist" << "\n";
        std::terminate();
    }

    //rotation
    FLOAT rotation;
    if (camera->members.count("rotation")){
        rotation = std::stof(camera->members.at("rotation")->data);
    }else{
        std::cout << "scene file:camera:rotation must exist" << "\n";
        std::terminate();
    }

    //sensor_pinhole_length
    FLOAT sensor_pinhole_length;
    if (camera->members.count("sensor_pinhole_length")){
        sensor_pinhole_length = std::stof(camera->members.at("sensor_pinhole_length")->data);
    }

    //sensor_lens_length
    FLOAT sensor_lens_length;
    if (camera->members.count("sensor_lens_length")){
        sensor_lens_length = std::stof(camera->members.at("sensor_lens_length")->data);
    }

    //lens radius
    FLOAT lens_radius;
    if (camera->members.count("lens_radius")){
        lens_radius = std::stof(camera->members.at("lens_radius")->data);
    }

    //create camera
    if (type == "pinhole"){
        m_camera = new PinholeCamera(sensor_width,
                                     sensor_height,
                                     sensor_origin,
                                     sensor_target,
                                     rotation,
                                     sensor_pinhole_length);
    }else if (type == "thinlens"){
        m_camera = new ThinlensCamera(sensor_width,
                                      sensor_height,
                                      sensor_origin,
                                      sensor_target,
                                      rotation,
                                      sensor_lens_length,
                                      lens_radius);
    }else{
        std::cout << "scene file:camera:type " << type << " is not supported" << "\n";
        std::terminate();
    }
}

void Scene::CreateAcceleration(const JSON::Node* root_node){
    //acceleration
    if (!root_node->members.count("acceleration")){
        std::cout << "scene file:acceleration must exist" << "\n";
        std::terminate();
    }
    const JSON::Node* acceleration = root_node->members.at("acceleration");

    //type
    std::string type;
    if (acceleration->members.count("type")){
        type = acceleration->members.at("type")->data;
    }else{
        std::cout << "scene file:acceleration:type must exist" << "\n";
        std::terminate();
    }

    //create acceleration
    if (type == "lbvh"){
        m_acceleration = new LBVH();
        m_acceleration->Construct(m_primitives);
    }else if (type == "sahbvh"){
        m_acceleration = new SAHBVH();
        m_acceleration->Construct(m_primitives);
    }else{
        std::cout << "scene file:acceleration:type " << type << " is not supported" << "\n";
        std::terminate();
    }

}

void Scene::CreateIntegrator(const JSON::Node* root_node){
    //integrator
    if (!root_node->members.count("integrator")){
        std::cout << "scene file:integrator must exist" << "\n";
        std::terminate();
    }
    JSON::Node* integrator = root_node->members.at("integrator");

    //type
    std::string type;
    if (integrator->members.count("type")){
        type = integrator->members.at("type")->data;
    }else{
        std::cout << "scene file:integrator:type must exist" << "\n";
        std::terminate();
    }

    //create integrator
    if (type == "pt"){
        m_integrator = new PT();
    }else{
        std::cout << "scene file:integrator:type " << type << " is not supported" << "\n";
        std::terminate();
    }
}

void Scene::CreateRendering(const JSON::Node* root_node){
    //rendering
    if (!root_node->members.count("rendering")){
        std::cout << "scene file:rendering must exist" << "\n";
        std::terminate();
    }
    JSON::Node* rendering = root_node->members.at("rendering");

    //sample per pixel
    if (rendering->members.count("sample_per_pixel")){
        int t = std::stoi(rendering->members.at("sample_per_pixel")->data);
        if (t > 0){
            m_sample_per_pixel = t;
        }else{
            std::cout << "scene file:rendering:sample_per_pixel must be positive integer" << "\n";
            std::terminate();
        }
    }else{
        std::cout << "scene file:rendering:sample_per_pixel must exist" << "\n";
        std::terminate();
    }

    //rendering time(second)
    if (rendering->members.count("rendering_time")){
        int t = std::stoi(rendering->members.at("rendering_time")->data);
        if (t > 0){
            m_rendering_time = t;
        }else{
            std::cout << "scene file:rendering:rendering_time must be positive integer" << "\n";
            std::terminate();
        }
    }else{
        std::cout << "scene file:rendering:rendering_time must exist" << "\n";
        std::terminate();
    }

    //rendering interval
    if (rendering->members.count("rendering_interval")){
        int t = std::stoi(rendering->members.at("rendering_interval")->data);
        if (t > 0){
            m_rendering_interval = t;
        }else{
            std::cout << "scene file:rendering:rendering_interval must be positive integer" << "\n";
            std::terminate();
        }
    }else{
        std::cout << "scene file:rendering:rendering_interval must exist" << "\n";
        std::terminate();
    }
    
    //thread count
    if (rendering->members.count("thread_count")){
        int t = std::stoi(rendering->members.at("thread_count")->data);
        if (t > 0){
            m_thread_count = t;
        }else{
            std::cout << "scene file:rendering:thread_count must be positive integer" << "\n";
            std::terminate();
        }
    }else{
        std::cout << "scene file:rendering:thread_count must exist" << "\n";
        std::terminate();
    }

}
