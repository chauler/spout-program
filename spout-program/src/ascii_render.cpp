#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ascii_render.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

void ascii_render::Draw() {
	std::string output = PixelsToString();
	UpdateProjection();
	shader.Bind();
	GLCall(glUniformMatrix4fv(shader.GetUniform("projection"), 1, NULL, glm::value_ptr(m_projection)));
	for (int i = 0; i < output.length(); i++) {
		char character = output[i];
		auto data = m_characterData[character];
		m_character.UpdateData(data, i);
		m_character.Draw();
		//break;
	}
	m_character.ResetPosition(m_win_w, m_win_h);
	shader.Unbind();
}

void ascii_render::UpdateImage(const cv::Mat image)
{
	m_inputImage = image;
	m_img_w = image.cols;
	m_img_h = image.rows;
}

void ascii_render::UpdateProjection() {
	GLCall(glfwGetWindowSize(window, &m_win_w, &m_win_h));
	m_projection = glm::ortho(0.0f, static_cast<float>(m_win_w), 0.0f, static_cast<float>(m_win_h));
}

std::string ascii_render::PixelsToString()
{
	unsigned char* pixelPtr = m_inputImage.data;
	int channels = m_inputImage.channels();
	unsigned char pixel = '\0';
	std::string output{};
	for (int i = 0; i < m_inputImage.rows; i++) {
		for (int j = 0; j < m_inputImage.cols; j++) {
			pixel = pixelPtr[i*m_inputImage.cols + j];
			unsigned int index = (unsigned int)pixel / (255 / m_charset.length());
			output+=m_charset[index];
		}
	}
	return output;
}

void ascii_render::LoadCharacterData()
{
	for (char character : m_charset) {
		FT_Set_Pixel_Sizes(m_face, 0, 100);
		unsigned int glyph_index = FT_Get_Char_Index(m_face, character);
		FT_Load_Glyph(m_face, glyph_index, FT_LOAD_RENDER);
		CharacterData data{};
		GLCall(glGenTextures(1, &data.textureID));
		GLCall(glBindTexture(GL_TEXTURE_2D, data.textureID));
		GLCall(glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RED,
			m_face->glyph->bitmap.width,
			m_face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			m_face->glyph->bitmap.buffer
		));
		data.size = glm::ivec2(m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows);
		data.bearing = glm::ivec2(m_face->glyph->bitmap_left, m_face->glyph->bitmap_top);
		data.advance = m_face->glyph->advance.x;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
		m_characterData.insert(std::pair<char, CharacterData>(character, data));
	}
}
