#include "Texture2D.h"
#include "Renderer.h"

Texture2D::Texture2D(GLenum texUnit): m_texUnit(texUnit) {
	GLCall(glGenTextures(1, &m_id));
}

Texture2D::~Texture2D() {
	GLCall(glDeleteTextures(1, &m_id));
}

void Texture2D::Bind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, m_id));
}

void Texture2D::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}

void Texture2D::Allocate(GLint internalFormat, unsigned int w, unsigned int h, GLenum format, GLenum type, unsigned char* data)
{
	Bind();
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, type, data));
	Unbind();
	m_format = format;
	m_type = type;
	m_w = w;
	m_h = h;
}

void Texture2D::SetParameter(GLenum parameter, GLint value)
{
	Bind();
	GLCall(glTexParameteri(GL_TEXTURE_2D, parameter, value));
	Unbind();
}

void Texture2D::GetPixelData(unsigned char* data) const
{
	Bind();
	GLCall(glGetTexImage(GL_TEXTURE_2D, 0, m_format, m_type, data));
	Unbind();
}

uVec2 Texture2D::GetDimensions() const {
	uVec2 dims{0, 0};
	GLCall(glBindTexture(GL_TEXTURE_2D, m_id));
	GLCall(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&dims.x));
	GLCall(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&dims.y));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	return dims;
}