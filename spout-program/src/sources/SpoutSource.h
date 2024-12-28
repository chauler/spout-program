#pragma once
#include <string>
#include <SpoutLibrary/SpoutLibrary.h>
#include "ISource.h"

class SpoutSource : public ISource {
public:
	SpoutSource(std::string name = "");
	~SpoutSource() { m_receiver->ReleaseReceiver(); }
	void GetNextFrame(GLuint id, GLuint textureTarget) override;
	void SetTargetName(std::string) override;
	std::vector<std::string> EnumerateTargets() override;
private:
	SPOUTLIBRARY* m_receiver = nullptr;
};