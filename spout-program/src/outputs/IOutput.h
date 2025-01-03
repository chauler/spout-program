#pragma once

class IOutput {
public:
	virtual ~IOutput() {}
	virtual void SendTexture(unsigned int textureID, unsigned int width, unsigned int height) = 0;
};