#pragma once
#include <vector>
#include <string>
#include "ISource.h"
#include "opencv2/videoio.hpp"

class CamSource : public ISource {
public:
	CamSource();
	~CamSource();
	void GetNextFrame(GLuint id, GLuint textureTarget) override;
	void SetTargetName(std::string) override;
	std::vector<std::string> EnumerateTargets() override;
private:
	cv::VideoCapture m_cap;
};