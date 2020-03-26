#ifndef JSON_HPP
#define JSON_HPP

#include <iostream>
#include <exception>
#include <vector>
#include <string>
#include <map>
#include "utility/file.hpp"


namespace JSON{

enum TokenType{
    PUNCTUATOR_TOKEN,
    NUMBER_TOKEN,
    STRING_TOKEN,
    BOOLEAN_TOKEN,
    NULL_TOKEN,
};

enum NodeType{
    OBJECT_NODE,
    ARRAY_NODE,
    NUMBER_NODE,
    STRING_NODE,
    BOOLEAN_NODE,
    NULL_NODE,
};

struct Token{
    TokenType type;
    std::string data;
};

struct Node{
    //for all
    NodeType type;

    //for object
    std::map<std::string,Node*> members;

    //for array
    std::vector<Node*> elements;

    //for number,string,boolean
    std::string data;
};



Node* read(const std::string& path);
void release(Node* node);
void print(const Node* node);


}// namespace JSON

#endif // JSON_HPP
