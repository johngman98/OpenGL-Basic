#pragma once
#include <string>
#include <iostream>
#include <fstream>

//my read file func
//cant read .bin file properly
std::string getFileContent(const std::string& filePath);

//Victor's function
std::string get_file_contents(const char* filename);