#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_write.h"

#include "utility/texture.hpp"


Texture::Texture():m_width(0),m_height(0){}
Texture::Texture(size_t width,size_t height):m_width(width),m_height(height){
    m_rgb.resize(m_width*m_height);
}

void Texture::Load(const std::string& path){
    //read texture by using stb
    int w,h,n;
    unsigned char* data = stbi_load(path.c_str(),&w,&h,&n,3);
    if (data == NULL){
        std::cout << "failed to load texture:" << path << "\n";
        std::terminate();
    }
    m_width = w;
    m_height = h;
    m_rgb.resize(m_width*m_height);
    for (size_t i = 0;i < m_rgb.size();i++){
        RGB rgb;
        rgb.r = data[3*i+0];
        rgb.g = data[3*i+1];
        rgb.b = data[3*i+2];
        m_rgb[i] = rgb;
    }
    stbi_image_free(data);
}

void Texture::Save(const std::string& path){
    stbi_write_png(path.c_str(),m_width,m_height,STBI_rgb,m_rgb.data(),0);
}

size_t Texture::GetWidth() const{
    return m_width;
}

size_t Texture::GetHeight() const{
    return m_height;
}

RGB Texture::GetRGB(size_t row,size_t column) const{
    size_t index = m_width*column+row;
    if (index >= 0 && index < m_rgb.size()){
        return m_rgb[index];
    }else{
        return {0,0,0};
    }
}

void Texture::SetRGB(const RGB& rgb,size_t row,size_t column){
    size_t index = m_width*column+row;
    if (index >= 0 && index < m_rgb.size()){
        m_rgb[index] = rgb;
    }
}

vec3 Texture::SampleNormalizedRGB(const vec2& uv) const{
    size_t row = uv[0]*m_width;
    size_t column = (1-uv[1])*m_height;
    size_t idx = column*m_width+row;
    if (idx < m_rgb.size()){
        return vec3({m_rgb[idx].r/255.0,m_rgb[idx].g/255.0,m_rgb[idx].b/255.0});
    }else{
        return vec3();
    }
}

FLOAT Texture::SampleNormalizedScalar(const vec2& uv) const{
    size_t row = uv[0]*m_width;
    size_t column = (1-uv[1])*m_height;
    size_t idx = column*m_width+row;
    if (idx < m_rgb.size()){
        return m_rgb[idx].r/255.0;
    }else{
        return 0;
    }
}

vec3 Texture::SampleNormalizedVector(const vec2& uv) const{
    size_t row = uv[0]*m_width;
    size_t column = (1-uv[1])*m_height;
    size_t idx = column*m_width+row;
    if (idx < m_rgb.size()){
        return vec3({m_rgb[idx].r/255.0,m_rgb[idx].g/255.0,m_rgb[idx].b/255.0})*2-1;
    }else{
        return vec3();
    }
}
