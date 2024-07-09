#include <iostream>
#include "Texture2D.h"
#include "Renderer.h"

Texture2D::Texture2D(GLenum texUnit): m_texUnit(texUnit) {
	GLCall(glGenFramebuffers(1, &m_FBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	GLCall(glGenTextures(1, &m_texture));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texture));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture, 0));
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	GLCall(glDrawBuffers(1, DrawBuffers));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cout << "Framebuffer setup error" << std::endl;
		exit(1);
	}
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glActiveTexture(GL_TEXTURE0));
}

Texture2D::~Texture2D() {
	GLCall(glDeleteTextures(1, &m_texture));
}

void Texture2D::Bind() const
{
	GLCall(glActiveTexture(m_texUnit));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texture));
}

void Texture2D::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glActiveTexture(GL_TEXTURE0));
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

void Texture2D::StartDrawing()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texture));
	glClear(GL_COLOR_BUFFER_BIT);
	//GLCall(glViewport(0, 0, m_w, m_h));
}

void Texture2D::EndDrawing()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
