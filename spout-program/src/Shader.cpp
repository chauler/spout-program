#include "Shader.h"
#include "GL/glew.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "Renderer.h"

Shader::Shader()  {
	GLCall(m_programID = glCreateProgram());
}

void Shader::AddShader(GLuint type, const std::string& source)
{
	GLCall(unsigned int id = glCreateShader(type));
	const char* sourceRaw = source.c_str();
	GLCall(glShaderSource(id, 1, &sourceRaw, NULL));
	GLCall(glCompileShader(id));
	int  success;
	char infoLog[512];
	glGetShaderiv(id, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	shaders.push_back(id);
}

void Shader::CompileShader() {
	for (unsigned int id : shaders) {
		GLCall(glAttachShader(m_programID, id));
	}
	GLCall(glLinkProgram(m_programID));
	int  success;
	char infoLog[512];
	glGetShaderiv(m_programID, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(m_programID, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}
	for (unsigned int id : shaders) {
		GLCall(glDeleteShader(id));
	}
	m_valid = true;
	shaders.clear();
}

std::string Shader::ParseShader(const std::string& filepath) {
	std::ifstream inputFile(filepath);
	std::stringstream buffer;
	buffer << inputFile.rdbuf();
	return buffer.str();
}

void Shader::Bind() {
	GLCall(glUseProgram(m_programID));
}

void Shader::Unbind() {
	GLCall(glUseProgram(0));
}

GLint Shader::GetUniform(std::string name) {
	auto it = m_uniforms.find(name);
	unsigned int uniformID = 0;
	//Uniform position already stored
	if (it != m_uniforms.end()) {
		return m_uniforms[name];
	}
	else { //Uniform position not yet accessed and stored
		uniformID = glGetUniformLocation(m_programID, name.c_str());
		m_uniforms.insert(std::pair<std::string, unsigned int>(name, uniformID));
		return uniformID;
	}
}