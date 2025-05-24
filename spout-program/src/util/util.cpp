#include <string>
#include <fstream>
#include <sstream>
#include "util.h"

std::string SpoutProgram::Util::ReadFile(const std::string& filepath) {
	std::ifstream inputFile(filepath);
	std::stringstream buffer;
	buffer << inputFile.rdbuf();
	return buffer.str();
}

double SpoutProgram::Util::ComputeScaleFactor(int srcW, int srcH, int maxW, int maxH) {
    // Compute the scaling factors along each axis, take the minimum.
    double scaleX = static_cast<double>(maxW) / static_cast<double>(srcW);
    double scaleY = static_cast<double>(maxH) / static_cast<double>(srcH);

    double scale = std::min({ scaleX, scaleY, 1.0 });

    return scale;
}