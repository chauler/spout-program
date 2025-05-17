#pragma once
#include <string>

class IOutput {
public:
	IOutput() {}
	virtual ~IOutput() {}
	virtual void SendTexture(unsigned int textureID, unsigned int width, unsigned int height) = 0;
	virtual void SetTargetName(const std::string&) = 0;
	virtual std::string GetTargetName() const = 0;
};