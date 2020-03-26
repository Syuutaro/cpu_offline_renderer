#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>
#include "core/define.hpp"
#include "utility/matrix.hpp"

struct RGB{
    uint8_t r;
    uint8_t g;
    uint8_t b;
};


class Texture{
private:
    size_t m_width;
    size_t m_height;
    std::vector<RGB> m_rgb;
public:
    Texture();
    Texture(size_t width,size_t height);
    void Load(const std::string& path);
    void Save(const std::string& path);
    size_t GetWidth() const;
    size_t GetHeight() const;
    RGB GetRGB(size_t row,size_t column) const;
    void SetRGB(const RGB& rgb,size_t row,size_t column);
    vec3 SampleNormalizedRGB(const vec2& uv) const;
    FLOAT SampleNormalizedScalar(const vec2& uv) const;
    vec3 SampleNormalizedVector(const vec2& uv) const;
};



#endif // TEXTURE_HPP
