#pragma once
#include <string>
#include <vector>
#include "IOutput.h"
#include "softcam.h"

class VirtualCamera : public IOutput {
public:
	VirtualCamera(std::string name);
	~VirtualCamera() override;
	void SendTexture(unsigned int id, unsigned int width, unsigned int height) override;

private:
	scCamera m_camera;
	std::vector<unsigned char> m_data;
	std::vector<unsigned char> m_convertedData;
};