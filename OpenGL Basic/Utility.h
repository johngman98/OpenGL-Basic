#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "VBO.h"

//my read file func
//cant read .bin file properly
std::string getFileContent(const std::string& filePath);

//Victor's function
std::string get_file_contents(const char* filename);

//load OBJ files
std::vector<Vertex> loadObj(const std::string& filePath);