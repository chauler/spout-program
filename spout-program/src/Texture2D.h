#pragma once
#include <memory>
#include <GL/glew.h>
#include "util/types.h"

class Texture2D {
public:
	Texture2D(GLenum texUnit = GL_TEXTURE0);
	~Texture2D();
	void operator=(const Texture2D&) = delete; //If need to copy, implement this so that the existing texture ID is freed correctly.
	void Bind() const;
	void Unbind() const;
	unsigned int GetSize() const { return m_w * m_h * m_channels; }
	unsigned int GetID() const { return m_id; }
	void Allocate(GLint internalFormat, unsigned int w, unsigned int h, GLenum format, GLenum type, unsigned char* data);
	void SetParameter(GLenum parameter, GLint value);
	void GetPixelData(unsigned char* data) const;
	uVec2 GetDimensions() const;
private:
	unsigned int m_id = 0;
	unsigned int m_w = 0, m_h = 0, m_channels = 0;
	GLenum m_texUnit = GL_TEXTURE0, m_format = GL_RGBA, m_type = GL_UNSIGNED_BYTE;

};