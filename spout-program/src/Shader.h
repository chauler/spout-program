#pragma once
#include <string>
#include <GL/glew.h>
#include <vector>

class Shader {
public:
	Shader();
	void AddShader(GLuint, const std::string&);
	void CompileShader();
	void Bind();
	void Unbind();
	unsigned int GetProgram() { return m_programID; }
private:
	unsigned int m_programID;
	bool m_valid = false;
	std::vector<unsigned int> shaders;
	std::string ParseShader(const std::string&);
};