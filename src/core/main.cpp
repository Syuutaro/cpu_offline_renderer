#include <iostream>
#include "core/scene.hpp"


int main(){
    std::cout << "Input absolute path to asset directory" << "\n";
    std::string path;
    std::cin >> path;

    Scene scene;
    scene.Create(path);
    scene.Render();
}
