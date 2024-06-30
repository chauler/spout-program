#pragma once
#include <memory>
#include <GL/glew.h>

class ISource {
public:
	virtual void GetNextFrame(GLuint id, GLuint textureTarget) = 0;
	virtual std::shared_ptr<unsigned char[]> GetFrameData() = 0;
	virtual ~ISource() { }
	unsigned int GetWidth() { return m_w; }
	unsigned int GetHeight() { return m_h; }
protected:
	std::shared_ptr<unsigned char[]> m_data{};
	unsigned int m_w = 0, m_h = 0;
};