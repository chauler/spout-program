#pragma once
#include "ft2build.h"
#include <glm/ext/vector_int2.hpp>
#include "glm/gtc/matrix_transform.hpp"
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
	void UpdateData(const CharacterData&, int);
	void ResetPosition(int, int);
private:
	unsigned int m_texture, m_VBO, m_VAO, m_EBO = 0;
	vertex m_vertices[4] = {
		{100.0f,  1320.0f},
		{100.0f, 210.0f},
		{610.0f, 210.0f},
		{610.0f,  1320.0f}
	};
	unsigned int indices[6] = { 0, 1, 3, 1, 2, 3 };

};
