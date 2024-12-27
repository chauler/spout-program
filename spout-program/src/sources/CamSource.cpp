#include "ISource.h"
#include "CamSource.h"
#include "Renderer.h"
#include <iostream>
#include "opencv2/core/opengl.hpp"
#include <opencv2/imgproc.hpp>
#include "tracy/public/tracy/Tracy.hpp"

CamSource::CamSource(): m_cap(0)
{
	if (!m_cap.isOpened()) {
		std::cout << "Error opening webcam capture" << std::endl;
	}
	else {
		m_cap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_WIDTH, 1920);
		m_cap.set(cv::VideoCaptureProperties::CAP_PROP_FRAME_HEIGHT, 1080);
		m_cap.set(cv::VideoCaptureProperties::CAP_PROP_FPS, 60);
	}
}

CamSource::~CamSource() {
	std::cout << "released" << std::endl;
	if (m_cap.isOpened()) {
		m_cap.release();
	}
}

void CamSource::GetNextFrame(GLuint id, GLuint textureTarget)
{
	ZoneScoped;
	cv::Mat frame;
	m_cap.read(frame);
	if (frame.empty()) {
		std::cout << "Error retreiving frame" << std::endl;
		return;
	}
	cv::cvtColor(frame, frame, cv::COLOR_BGR2RGBA);
	m_w = frame.cols;
	m_h = frame.rows;
	size_t sizeInBytes = frame.step[0] * frame.rows;
	m_data = std::make_shared<unsigned char[]>(sizeInBytes);
	std::memcpy(m_data.get(), frame.data, sizeInBytes);
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_w, m_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, frame.data));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

std::shared_ptr<unsigned char[]> CamSource::GetFrameData()
{
	return m_data;
}

