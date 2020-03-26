#include "utility/file.hpp"

void read_file(std::string& text,const std::string& path){
    //open file
    std::FILE* fp = std::fopen(path.c_str(),"r");
    if (fp == NULL){
        std::cout << "file.cpp:failed to open" << path << "\n";
        std::terminate();
    }

    //file size
    std::fseek(fp,0,SEEK_END);
    size_t size = std::ftell(fp);

    //allocate memory
    char* src = new char[size+1];

    //read file
    std::fseek(fp,0,SEEK_SET);
    std::fread(src,1,size,fp);
    src[size] = '\0';

    //copy file source
    text = src;

    //free memory
    delete[] src;

    //close file
    std::fclose(fp);
}

void write_file(const std::string& text,const std::string& path){
    //open file
    std::FILE* fp = std::fopen(path.c_str(),"w");
    if (fp == NULL){
        std::cout << "file.cpp:failed to open" << path << "\n";
        std::terminate();
    }

    //write file
    std::fseek(fp,0,SEEK_SET);
    std::fwrite(text.c_str(),1,text.size(),fp);

    //close file
    std::fclose(fp);
}
