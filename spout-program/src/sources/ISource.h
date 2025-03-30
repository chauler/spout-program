#pragma once
#include <memory>
#include <vector>
#include <string>
#include <GL/glew.h>

class ISource {
public:
	virtual void GetNextFrame(GLuint id, GLuint textureTarget) = 0;
	virtual ~ISource() { }
	unsigned int GetWidth() const { return m_w; }
	unsigned int GetHeight() const { return m_h; }
	virtual void SetTargetName(std::string) = 0;
	virtual std::vector<std::string> EnumerateTargets() = 0;
protected:
	unsigned int m_w = 0, m_h = 0;
};