#pragma once
#include <string>
#include <GL/glew.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <vector>
#include <map>
#include <iostream>

class Shader {
public:
	Shader();
	~Shader();
	//Shader objects don't store shader source, so we can't copy the internal shader programs
	//and guarantee their lifetime, so only moves are allowed.
	Shader(const Shader&) = delete;
	Shader& operator=(const Shader& other) = delete;
	Shader(Shader&& other) noexcept;
	Shader& operator=(Shader&& other);
	void AddShader(GLuint, const std::string&);
	void CompileShader();
	void Bind();
	void Unbind();
	unsigned int GetProgram() { return m_programID; }
	GLint GetUniform(std::string);
private:
	unsigned int m_programID;
	bool m_valid = false;
	std::vector<unsigned int> shaders;
	std::string ParseShader(const std::string&);
	std::map<std::string, unsigned int> m_uniforms;
};
