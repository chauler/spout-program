#include "GL/glew.h"
#include "Renderer.h"
#include "Character.h"
#include "ft2build.h"
#include "glm/gtc/matrix_transform.hpp"
#include FT_FREETYPE_H

void Character::Draw()
{
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texture));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glBindVertexArray(0));
}

void Character::Init(FT_Face face)
{
	GLCall(glGenBuffers(1, &m_VBO));
	GLCall(glGenBuffers(1, &m_EBO));
	GLCall(glGenVertexArrays(1, &m_VAO));
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Character::UpdateData(const CharacterData& charData, int index) {
	if (index%100 == 0) {
		m_vertices[0] = { 0.0f,  m_vertices[0].y - 12.0f};
		m_vertices[1] = { 0.0f, m_vertices[1].y - 12.0f};
		m_vertices[2] = { 10.0f, m_vertices[2].y - 12.0f};
		m_vertices[3] = { 10.0f,  m_vertices[3].y - 12.0f};
	}
	else {
		m_vertices[0].x += 12.0f;
		m_vertices[1].x += 12.0f;
		m_vertices[2].x += 12.0f;
		m_vertices[3].x += 12.0f;
	}
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertices), m_vertices));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	m_texture = charData.textureID;
}

void Character::ResetPosition(int width, int height) {
	m_vertices[0] = { 0.0f,  (float)height};
		m_vertices[1] = { 0.0f, (float)height-10};
		m_vertices[2] = { 10.0f, (float)height-10};
		m_vertices[3] = { 10.0f,  (float)height};
}