#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ascii_render.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "tracy/public/tracy/Tracy.hpp"

ascii_render::ascii_render(GLFWwindow* window) : window(window), m_charsetSize(16), m_charset(m_charSets[m_charsetSize]) {
	glfwGetWindowSize(window, &m_winW, &m_winH);
	fontLoader.LoadFace("res/fonts/Roboto-Regular.ttf");
	m_face = fontLoader.GetFace();
	LoadCharacterData();
	m_asciiShader.AddShader(GL_VERTEX_SHADER, "res/shaders/text.vs");
	m_asciiShader.AddShader(GL_FRAGMENT_SHADER, "res/shaders/text.fs");
	m_asciiShader.CompileShader();
	m_asciiShader.Bind();
	GLCall(glUniformMatrix4fv(m_asciiShader.GetUniform("projection"), 1, NULL, glm::value_ptr(glm::ortho(0.0f, static_cast<float>(m_winW), 0.0f, static_cast<float>(m_winH)))));
	GLCall(glUniform4f(m_asciiShader.GetUniform("bgColor"), m_bgColor[0], m_bgColor[1], m_bgColor[2], m_bgColor[3]));
	GLCall(glUniform4f(m_asciiShader.GetUniform("charColor"), m_charColor[0], m_charColor[1], m_charColor[2], m_charColor[3]));
	GLCall(glUniform1f(m_asciiShader.GetUniform("charsetSize"), (float)m_charsetSize));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	m_asciiShader.Unbind();
	m_sobelShader.AddShader(GL_VERTEX_SHADER, "res/shaders/edge.vs");
	m_sobelShader.AddShader(GL_FRAGMENT_SHADER, "res/shaders/edge.fs");
	m_sobelShader.CompileShader();
	m_sobelShader.Bind();

	GLCall(glGenFramebuffers(1, &m_outputFBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_outputFBO));
	GLCall(glGenTextures(1, &m_outTex));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_winW, m_winH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_outTex, 0));
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	GLCall(glDrawBuffers(1, DrawBuffers));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		exit(1);
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glGenTextures(1, &m_inputTex));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_inputTex));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_winW, m_winH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glGenBuffers(1, &m_VBO));
	GLCall(glGenBuffers(1, &m_EBO));
	GLCall(glGenVertexArrays(1, &m_VAO));
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GenericGlyphData), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glGenBuffers(1, &m_iVBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(m_positions), m_positions, GL_DYNAMIC_DRAW));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribDivisor(1, 1));
	GLCall(glVertexAttribDivisor(2, 1));
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

}

SpoutOutTex ascii_render::Draw() {
	ZoneScoped;
	GLfloat vertices[] = {
		/*  xy            uv */
			-1.0,  1.0,   0.0, 1.0,
			 -1.0, -1.0,   0.0, 0.0,
			1.0, -1.0,   1.0, 0.0,
			1.0, 1.0,   1.0, 1.0,
	};

	m_intermediate.Allocate(GL_RGBA, m_inputImage.w, m_inputImage.h, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	m_intermediate.StartDrawing();
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW));
	GLCall(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
	GLCall(glEnableVertexAttribArray(0));
	static GLuint ebo = 0;
	glGenBuffers(1, &ebo);
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW));
	m_sobelShader.Bind();
	GLCall(glUniform2i(m_sobelShader.GetUniform("outputSize"), m_inputImage.w, m_inputImage.h));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_inputTex));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glActiveTexture(GL_TEXTURE0));
	m_sobelShader.Unbind();
	m_intermediate.EndDrawing();
	GLCall(glDisableVertexAttribArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
	//return { m_intermediate.GetID(), m_inputImage.w, m_inputImage.h};
	
	//Update column and row numbers for each vertex
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, m_inputImage.h * m_inputImage.w * sizeof(InstanceData), m_positions, GL_DYNAMIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	UpdateProjection();
	
	//Update uniforms to prep for drawing
	m_asciiShader.Bind();
	GLCall(glUniformMatrix4fv(m_asciiShader.GetUniform("projection"), 1, NULL, glm::value_ptr(glm::ortho(0.0f, static_cast<float>(m_charSize * m_inputImage.w), 0.0f, static_cast<float>(m_charSize * m_inputImage.h)))));
	GLCall(glUniform2i(m_asciiShader.GetUniform("imgDims"), m_inputImage.w, m_inputImage.h));
	GLCall(glUniform1i(m_asciiShader.GetUniform("charSize"), m_charSize));
	GLCall(glUniform1f(m_asciiShader.GetUniform("charsetSize"), m_charsetSize));
	GLCall(glUniform2i(m_asciiShader.GetUniform("outputSize"), m_charSize * m_inputImage.w, m_charSize * m_inputImage.h));
	
	//Set up vertex data
	GLCall(glBindVertexArray(m_VAO));
	
	//Allocate space for output == (charSize * cols, charSize * rows) and set viewport
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_outputFBO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
	glClear(GL_COLOR_BUFFER_BIT);
	unsigned int outputW = m_charSize * m_inputImage.w, outputH = m_charSize * m_inputImage.h;
	GLCall(glViewport(0, 0, outputW, outputH));
	
	//Set up samplers and draw
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textArray));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_inputTex));
	GLCall(glActiveTexture(GL_TEXTURE2));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_intermediate.GetID()));
	GLCall(glActiveTexture(GL_TEXTURE3));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_edgeArray));
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_inputImage.h * m_inputImage.w));
	//Clean up and unbind everything
	GLCall(glBindVertexArray(0));
	m_asciiShader.Unbind();
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	
	return { m_outTex, outputW, outputH };
}

void ascii_render::UpdateImage(const cv::Mat& image)
{
	ZoneScoped;
	m_inputImage.image = image;
	//New img is different size to previously allocated.
	//(Check for nullptr to not delete when uninitialized)
	if (image.cols != m_inputImage.w || image.rows != m_inputImage.h) {
		if (m_positions != nullptr) {
			delete(m_positions);
		}
		m_positions = new InstanceData[image.cols * image.rows];
		for (int row = 0; row < image.rows; row++) {
			for (int col = 0; col < image.cols; col++) {
				m_positions[row * image.cols + col] = { (float)col, (float)row };
			}
		}
		m_inputImage.w = image.cols;
		m_inputImage.h = image.rows;
		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_charSize * m_inputImage.w, m_charSize * m_inputImage.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	}
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_inputTex));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_inputImage.w, m_inputImage.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_inputImage.image.data));
	GLCall(glActiveTexture(GL_TEXTURE0));
}

void ascii_render::UpdateState(int charSize, int charRes, glm::vec4 bgColor, glm::vec4 charColor) {
	ZoneScoped;
	if (charSize != m_charSize) {
		m_charSize = charSize;
		m_vertices[0].y = (float)m_charSize;
		m_vertices[2].x = (float)m_charSize;
		m_vertices[3].x = (float)m_charSize;
		m_vertices[3].y = (float)m_charSize;
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
		GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertices), m_vertices););
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_charSize * m_inputImage.w, m_charSize * m_inputImage.h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	}
	if (charRes != m_charRes) {
		m_charRes = charRes;
		LoadCharacterData(m_charRes);
	}

	if (bgColor != m_bgColor) {
		m_bgColor = bgColor;
		m_asciiShader.Bind();
		GLCall(glUniform4f(m_asciiShader.GetUniform("bgColor"), m_bgColor[0], m_bgColor[1], m_bgColor[2], m_bgColor[3]));
		m_asciiShader.Unbind();
	}

	if (charColor != m_charColor) {
		m_charColor = charColor;
		m_asciiShader.Bind();
		GLCall(glUniform4f(m_asciiShader.GetUniform("charColor"), m_charColor[0], m_charColor[1], m_charColor[2], m_charColor[3]));
		m_asciiShader.Unbind();
	}
}

void ascii_render::UpdateProjection() {
	GLCall(glfwGetWindowSize(window, &m_winW, &m_winH));
}

void ascii_render::LoadCharacterData(int textSize)
{
	ZoneScoped;
	//Function may be called to re-allocate textures. Only gen texture if this is first call.
	if (m_textArray == 0) {
		GLCall(glGenTextures(1, &m_textArray));
		GLCall(glGenTextures(1, &m_edgeArray));
	}
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_edgeArray));
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textArray));
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	unsigned char* buffer = new unsigned char[textSize * textSize * m_charset.length()]{};
	GLCall(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, textSize, textSize, m_charset.length(), 0, GL_RED, GL_UNSIGNED_BYTE, buffer));
	//Load all normal glyphs from the charset
	for (int i = 0; i < m_charset.length(); i++) {
		wchar_t character = m_charset[i];
		FT_Set_Pixel_Sizes(m_face, 0, textSize);
		unsigned int glyph_index = FT_Get_Char_Index(m_face, character);
		FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_MONO);
		
		//Mono format above represents each pixel as 1 bit
		//Convert that to 1 byte for OpenGL. 1=white, 0=black
		unsigned char* glyphBuffer = new unsigned char[m_face->glyph->bitmap.width * m_face->glyph->bitmap.rows];
		int pitch = m_face->glyph->bitmap.pitch;
		for (int row = 0; row < m_face->glyph->bitmap.rows; row++) {
			for (int col = 0; col < m_face->glyph->bitmap.width; col++) {
				unsigned int rowStartIndex = row * pitch;
				unsigned char cValue = m_face->glyph->bitmap.buffer[rowStartIndex + (col >> 3)];
				glyphBuffer[row * m_face->glyph->bitmap.width + col] = ((cValue & (128 >> (col & 7))) != 0) * 255;
			}
		}
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
			glyphBuffer
			));
		delete[] glyphBuffer;
	}
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));

	unsigned char* newBuffer = new unsigned char[textSize * textSize * 4] {};
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_edgeArray));
	GLCall(glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_R8, textSize, textSize, 4, 0, GL_RED, GL_UNSIGNED_BYTE, newBuffer));
	//Load all edge glyphs
	for (int i = 0; i < 4; i++) {
		wchar_t character = "|_/\\"[i];
		FT_Set_Pixel_Sizes(m_face, 0, textSize);
		unsigned int glyph_index = FT_Get_Char_Index(m_face, character);
		FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT);
		FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_MONO);

		//Mono format above represents each pixel as 1 bit
		//Convert that to 1 byte for OpenGL. 1=white, 0=black
		unsigned char* glyphBuffer = new unsigned char[m_face->glyph->bitmap.width * m_face->glyph->bitmap.rows];
		int pitch = m_face->glyph->bitmap.pitch;
		for (int row = 0; row < m_face->glyph->bitmap.rows; row++) {
			for (int col = 0; col < m_face->glyph->bitmap.width; col++) {
				unsigned int rowStartIndex = row * pitch;
				unsigned char cValue = m_face->glyph->bitmap.buffer[rowStartIndex + (col >> 3)];
				glyphBuffer[row * m_face->glyph->bitmap.width + col] = ((cValue & (128 >> (col & 7))) != 0) * 255;
			}
		}
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
			glyphBuffer
		));
		delete[] glyphBuffer;
	}
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
	delete[] buffer;
}
