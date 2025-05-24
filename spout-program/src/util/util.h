#pragma once
#include <string>
#include <iostream>

namespace SpoutProgram::Util {
	std::string ReadFile(const std::string& filepath);

	//Gives a scale factor that can be used to scale a texture to fit within a given width and height.
	double ComputeScaleFactor(int srcW, int srcH, int maxW, int maxH);
}
