#pragma once
#include <string>
#include <SpoutLibrary.h>
#include "ISource.h"

class SpoutSource : public ISource {
public:
	SpoutSource(std::string name);
	~SpoutSource() { m_receiver->ReleaseReceiver(); }
	void GetNextFrame(GLuint id, GLuint textureTarget) override;
	std::shared_ptr<unsigned char[]> GetFrameData() override;
private:
	SPOUTLIBRARY* m_receiver = nullptr;
};