#ifndef OBJ_HPP
#define OBJ_HPP

#include <iostream>
#include <exception>
#include <vector>
#include <string>
#include <array>
#include "utility/file.hpp"

namespace OBJ{

typedef double FLOAT;

enum TokenType{
    PUNCTUATOR_TOKEN,
    IDENTIFIER_TOKEN,
    NUMBER_TOKEN,
};

struct Face{
    std::vector<size_t> v_idx;
    std::vector<size_t> vt_idx;
    std::vector<size_t> vn_idx;
};

struct Data{
    std::vector<std::array<FLOAT,3>> v;
    std::vector<std::array<FLOAT,2>> vt;
    std::vector<std::array<FLOAT,3>> vn;
    std::vector<Face> f;
};

struct Triangle{
    std::array<FLOAT,3> xyz1;
    std::array<FLOAT,3> xyz2;
    std::array<FLOAT,3> xyz3;
    std::array<FLOAT,2> uv1;
    std::array<FLOAT,2> uv2;
    std::array<FLOAT,2> uv3;
};

struct Model{
    std::vector<Triangle> triangles;
};

struct Token{
    TokenType type;
    std::string data;
};

void read(Data& data,const std::string& path);
void read(Model& model,const std::string& path);
void write(const Data& data,const std::string& path);
void print(const Data& data);



}//namespace OBJ

#endif // OBJ_HPP
