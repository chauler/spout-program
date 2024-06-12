#pragma once
#include "ft2build.h"
#include <glm/ext/vector_int2.hpp>
#include FT_FREETYPE_H

struct vertex {
	float x;
	float y;
	float texX;
	float texY;
};

struct CharacterData {
	unsigned int textureID;
	glm::ivec2 size;
	glm::ivec2 bearing;
	unsigned int advance;
};

class Character {
public:
	//Character();
	void Draw();
	void Init(FT_Face);
private:
	unsigned int m_texture, m_VBO, m_VAO, m_EBO = 0;
	vertex m_vertices[4] = {
		{1280.0f,  720.0f,  0.0f, 0.0f},
		{1280.0f, 0.0f,  0.0f, 1.0f},
		{0.0f, 0.0f, 1.0f, 1.0f},
		{0.0f,  720.0f, 1.0f, 0.0f}
	};
	unsigned int indices[6] = { 0, 1, 3, 1, 2, 3 };
};