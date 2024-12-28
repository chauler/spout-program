#include "util.h"
#include <string>
#include <fstream>
#include <sstream>

std::string ReadFile(const std::string& filepath) {
	std::ifstream inputFile(filepath);
	std::stringstream buffer;
	buffer << inputFile.rdbuf();
	return buffer.str();
}