#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ascii_render.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

ascii_render::ascii_render(GLFWwindow* window) : window(window), m_charset(" .',:;clxokXdO0KN") {
	glfwGetWindowSize(window, &m_win_w, &m_win_h);
	fontLoader.LoadFace("res/fonts/arial.ttf");
	m_face = fontLoader.GetFace();
	LoadCharacterData();
	shader.AddShader(GL_VERTEX_SHADER, "res/shaders/text.vs");
	shader.AddShader(GL_FRAGMENT_SHADER, "res/shaders/text.fs");
	shader.CompileShader();
	shader.Bind();
	GLCall(glUniformMatrix4fv(shader.GetUniform("projection"), 1, NULL, glm::value_ptr(glm::ortho(0.0f, static_cast<float>(m_win_w), 0.0f, static_cast<float>(m_win_h)))));
	float textColor[3] = { 1.0f, 1.0f, 1.0f };
	GLCall(glUniform3f(glGetUniformLocation(shader.GetProgram(), "textColor"), textColor[0], textColor[1], textColor[2]));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader.Unbind();

	GLCall(glGenFramebuffers(1, &m_FBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	GLCall(glGenTextures(1, &m_outTex));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_win_w, m_win_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_outTex, 0));
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	GLCall(glDrawBuffers(1, DrawBuffers));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		exit(1);
	GLCall(glGenBuffers(1, &m_VBO));
	GLCall(glGenBuffers(1, &m_EBO));
	GLCall(glGenVertexArrays(1, &m_VAO));
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glGenBuffers(1, &m_iVBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(m_positions), m_positions, GL_DYNAMIC_DRAW));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribDivisor(1, 1));
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

}

SpoutOutTex ascii_render::Draw() {
	//Process image. Textures in same order as m_charset, so index == layer to sample from
	std::string output = PixelsToString();
	for (int i = 0; i < output.length(); i++) {
		m_positions[i].texArrayIndex = m_charset.find(output[i]);
	}

	//This buffer contains the layer each position will sample its texture from
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, m_img_h*m_img_w*sizeof(vertex), m_positions, GL_DYNAMIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	UpdateProjection();

	//Update uniforms to prep for drawing
	shader.Bind();
	//GLCall(glUniformMatrix4fv(shader.GetUniform("projection"), 1, NULL, glm::value_ptr(glm::ortho(0.0f, static_cast<float>(m_win_w), 0.0f, static_cast<float>(m_win_h)))));
	GLCall(glUniformMatrix4fv(shader.GetUniform("projection"), 1, NULL, glm::value_ptr(glm::ortho(0.0f, static_cast<float>(m_charSize * m_img_w), 0.0f, static_cast<float>(m_charSize * m_img_h)))));
	GLCall(glUniform2f(shader.GetUniform("windowDims"), m_win_w, m_win_h));
	GLCall(glUniform2f(shader.GetUniform("imgDims"), m_img_w, m_img_h));
	GLCall(glUniform1f(shader.GetUniform("charSize"), m_charSize));
	
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));

	//Allocate space for output == (charSize * cols, charSize * rows)
	GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
	glClear(GL_COLOR_BUFFER_BIT);
	//Set viewport == texture size
	GLCall(glViewport(0, 0, m_charSize * m_img_w, m_charSize * m_img_h));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textArray));
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, output.length()));
	GLCall(glBindVertexArray(0));
	shader.Unbind();
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	return { m_outTex, (int)m_charSize * m_img_w, (int)m_charSize * m_img_h };
}

void ascii_render::UpdateImage(const cv::Mat image)
{
	m_inputImage = image;
	//New img is different size to previously allocated.
	//(Check for nullptr to not delete when uninitialized)
	if (image.cols != m_img_w || image.rows != m_img_h) {
		if (m_positions != nullptr) {
			delete(m_positions);
		}
		m_positions = new vertex[image.cols * image.rows];
		for (int row = 0; row < image.rows; row++) {
			for (int col = 0; col < image.cols; col++) {
				m_positions[row * image.cols + col] = { (float)col, (float)row, 0.0f };
			}
		}
		m_img_w = image.cols;
		m_img_h = image.rows;
		GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
		//GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_win_w, m_win_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_charSize * m_img_w, m_charSize * m_img_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	}
}

void ascii_render::UpdateState(float charSize, int charRes) {
	if (charSize != m_charSize) {
		m_charSize = charSize;
		m_vertices[0].y = m_charSize;
		m_vertices[2].x = m_charSize;
		m_vertices[3].x = m_charSize;
		m_vertices[3].y = m_charSize;
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertices), m_vertices););
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_charSize * m_img_w, m_charSize * m_img_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	}
	if (charRes != m_charRes) {
		m_charRes = charRes;
		LoadCharacterData(m_charRes);
	}

}

void ascii_render::UpdateProjection() {
	GLCall(glfwGetWindowSize(window, &m_win_w, &m_win_h));
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

void ascii_render::LoadCharacterData(int textSize)
{
	//Function may be called to re-allocate textures. Only gen texture if this is first call.
	if (m_textArray == 0) {
		GLCall(glGenTextures(1, &m_textArray));
	}
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textArray));
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	//GLCall(glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_R8, 90, 90, m_charset.length()));
	unsigned char* buffer = new unsigned char[textSize * textSize * m_charset.length()]{};
	GLCall(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, textSize, textSize, m_charset.length(), 0, GL_RED, GL_UNSIGNED_BYTE, buffer));
	for (int i = 0; i < m_charset.length(); i++) {
		char character = m_charset[i];
		FT_Set_Pixel_Sizes(m_face, 0, textSize);
		unsigned int glyph_index = FT_Get_Char_Index(m_face, character);
		FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_NORMAL);
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
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	}
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
	delete[] buffer;
}
