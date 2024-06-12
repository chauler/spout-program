#include "GL/glew.h"
#include "Renderer.h"
#include "Character.h"
#include "ft2build.h"
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
	GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	
	FT_Set_Pixel_Sizes(face, 0, 500);
	unsigned int glyph_index = FT_Get_Char_Index(face, 'A');
	FT_Load_Glyph(face, glyph_index, FT_LOAD_RENDER);

	GLCall(glGenTextures(1, &m_texture));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_texture));
	GLCall(glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RED,
		face->glyph->bitmap.width,
		face->glyph->bitmap.rows,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		face->glyph->bitmap.buffer
	));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
