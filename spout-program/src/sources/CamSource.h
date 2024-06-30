#pragma once
#include "ISource.h"
#include "opencv2/videoio.hpp"

class CamSource : public ISource {
public:
	CamSource();
	~CamSource();
	void GetNextFrame(GLuint id, GLuint textureTarget) override;
	std::shared_ptr<unsigned char[]> GetFrameData() override;
private:
	cv::VideoCapture m_cap;
};