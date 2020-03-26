#build script for MacOSX + GNU Compiler Collection

#compile
cpp_files=$(find ./src -name \*.cpp)
g++ -std=c++11 -c ${cpp_files} -I./ext -I./src

#link and generate execution file
object_files=$(find . -name \*.o)
g++ ${object_files} -o app
