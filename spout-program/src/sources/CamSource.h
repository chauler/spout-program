#pragma once
#include <vector>
#include <string>
#include "ISource.h"
#include "escapi/escapi.h"

class CamSource : public ISource {
public:
	CamSource(std::string name = "");
	~CamSource();
	void GetNextFrame(GLuint id, GLuint textureTarget) override;
	void SetTargetName(std::string) override;
	std::vector<std::string> EnumerateTargets() override;
private:
	int IDFromDeviceName(std::string);
	bool m_open = false;
	std::unique_ptr<int[]> m_data = nullptr;
	SimpleCapParams m_captureParams;
	int m_deviceID = -1;
};