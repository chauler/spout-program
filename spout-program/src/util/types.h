#pragma once
#include <string>

enum class SourceType : unsigned int { None, Spout, Webcam };
enum class OutputType : unsigned int { None, Spout, VirtualCam };

struct iVec2 {
	int x;
	int y;
};

struct uVec2 {
	unsigned int x;
	unsigned int y;
};

struct SpoutOutTex {
	unsigned int id;
	unsigned int w;
	unsigned int h;
};
