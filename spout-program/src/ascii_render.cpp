#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ascii_render.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

unsigned int ascii_render::Draw() {
	std::string output = PixelsToString();
	for (int i = 0; i < output.length(); i++) {
		m_positions[i].texArrayIndex = m_charset.find(output[i]);
	}
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, m_img_h*m_img_w*sizeof(vertex), m_positions, GL_DYNAMIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	UpdateProjection();
	shader.Bind();
	GLCall(glUniformMatrix4fv(shader.GetUniform("projection"), 1, NULL, glm::value_ptr(m_projection)));
	GLCall(glUniform2f(shader.GetUniform("windowDims"), m_win_w, m_win_h));
	GLCall(glUniform2f(shader.GetUniform("imgDims"), m_img_w, m_img_h));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_win_w, m_win_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	glClear(GL_COLOR_BUFFER_BIT);
	GLCall(glViewport(0, 0, m_win_w, m_win_h));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textArray));
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, output.length()));
	GLCall(glBindVertexArray(0));
	shader.Unbind();
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	return m_outTex;
}

void ascii_render::UpdateImage(const cv::Mat image)
{
	m_inputImage = image;
	//New img is different size to previously allocated.
	//(Check for nullptr to not delete when uninitialized)
	if (m_positions != nullptr && (image.cols != m_img_w || image.rows != m_img_h)) {
		delete(m_positions);
	}
	m_img_w = image.cols;
	m_img_h = image.rows;
	m_positions = new vertex[m_img_w * m_img_h];
	for (int row = 0; row < m_img_h; row++) {
		for (int col = 0; col < m_img_w; col++) {
			m_positions[row * m_img_w + col] = { (float)col, (float)row, 0.0f };
		}
	}
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
	assert(output.length() == m_img_h * m_img_w);
	return output;
}

void ascii_render::LoadCharacterData()
{
	unsigned int textSize = 100;
	GLCall(glGenTextures(1, &m_textArray));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textArray));
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLCall(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_R8, 90, 90, m_charset.length()));
	for (int i = 0; i < m_charset.length(); i++) {
		char character = m_charset[i];
		FT_Set_Pixel_Sizes(m_face, 0, textSize);
		unsigned int glyph_index = FT_Get_Char_Index(m_face, character);
		FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_NORMAL);
		CharacterData data{};
		GLCall(glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
			0,
			0,
			0,
			i,
			m_face->glyph->bitmap.width,
			m_face->glyph->bitmap.rows,
			1,
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
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
}
