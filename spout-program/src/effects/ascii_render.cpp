#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ascii_render.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "tracy/public/tracy/Tracy.hpp"

unsigned int quadVAO = 0;
unsigned int quadVBO;

ascii_render::ascii_render(GLFWwindow* window) : window(window), m_charset(m_charSets[32]) {
	glfwGetWindowSize(window, &m_winW, &m_winH);
	fontLoader.LoadFace("res/fonts/Roboto-Regular.ttf");
	m_face = fontLoader.GetFace();
	LoadCharacterData();

	computeShader.AddShader(GL_COMPUTE_SHADER, "res/shaders/compute.cs");
	computeShader.CompileShader();
	GLCall(glGenTextures(1, &m_computeShaderOutput));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_computeShaderOutput));
	GLCall(glTextureStorage2D(m_computeShaderOutput, 1, GL_RGBA8, 80, 60));
	GLCall(glBindImageTexture(0, m_computeShaderOutput, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	screenRenderShader.AddShader(GL_VERTEX_SHADER, "res/shaders/sample.vs");
	screenRenderShader.AddShader(GL_FRAGMENT_SHADER, "res/shaders/sample.fs");
	screenRenderShader.CompileShader();
	sobelShader.AddShader(GL_VERTEX_SHADER, "res/shaders/edge.vs");
	sobelShader.AddShader(GL_FRAGMENT_SHADER, "res/shaders/edge.fs");
	sobelShader.CompileShader();
	dGaussianShader.AddShader(GL_VERTEX_SHADER, "res/shaders/edge.vs");
	dGaussianShader.AddShader(GL_FRAGMENT_SHADER, "res/shaders/gaussian.fs");
	dGaussianShader.CompileShader();

	shader.AddShader(GL_VERTEX_SHADER, "res/shaders/text.vs");
	shader.AddShader(GL_FRAGMENT_SHADER, "res/shaders/text.fs");
	shader.CompileShader();
	shader.Bind();
	GLCall(glUniformMatrix4fv(shader.GetUniform("projection"), 1, NULL, glm::value_ptr(glm::ortho(0.0f, static_cast<float>(m_winW), 0.0f, static_cast<float>(m_winH)))));
	GLCall(glUniform4f(shader.GetUniform("bgColor"), m_bgColor[0], m_bgColor[1], m_bgColor[2], m_bgColor[3]));
	GLCall(glUniform4f(shader.GetUniform("charColor"), m_charColor[0], m_charColor[1], m_charColor[2], m_charColor[3]));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader.Unbind();

	GLCall(glGenFramebuffers(1, &m_FBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
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

	GLCall(glGenFramebuffers(1, &m_intermediateFBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFBO));
	GLCall(glGenTextures(1, &m_intermediate));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_intermediate));
	//GLCall(glTextureStorage2D(m_intermediate, 1, GL_RGBA8, 80, 60));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 80, 60, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_intermediate, 0));
	GLCall(glDrawBuffers(1, DrawBuffers));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		exit(1);

	GLCall(glGenFramebuffers(1, &m_intermediateFBO2));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFBO2));
	GLCall(glGenTextures(1, &m_intermediate2));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_intermediate2));
	//GLCall(glTextureStorage2D(m_intermediate, 1, GL_RGBA8, 80, 60));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 80, 60, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_intermediate2, 0));
	GLCall(glDrawBuffers(1, DrawBuffers));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		exit(1);
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glActiveTexture(GL_TEXTURE0));

	GLCall(glGenBuffers(1, &m_VBO));
	GLCall(glGenBuffers(1, &m_EBO));
	GLCall(glGenVertexArrays(1, &m_VAO));
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GenericGlyphData), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glGenBuffers(1, &m_iVBO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(m_positions), m_positions, GL_DYNAMIC_DRAW));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribDivisor(1, 1));
	GLCall(glGenBuffers(1, &m_iVBO2));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO2));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW));
	GLCall(glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)0));
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribDivisor(2, 1));
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	float vertices[] = {
		// positions          // colors           // texture coords
		 1.0f,  1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		 1.0f, -1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		-1.0f, -1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		-1.0f,  1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
	};
	unsigned int indices[] = {
		0, 1, 3, // first triangle
		1, 2, 3  // second triangle
	};
	glGenBuffers(1, &testEBO);
	glGenBuffers(1, &testVBO);
	glEnableVertexAttribArray(0);
	GLCall(glGenVertexArrays(1, &testVAO));
	GLCall(glBindVertexArray(testVAO));
	glBindBuffer(GL_ARRAY_BUFFER, testVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, testEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

SpoutOutTex ascii_render::Draw() {
	ZoneScoped;
	//Process image. Textures in same order as m_charset, so index == layer to sample from
	//CalculateCharsFromLuminance();

	//This buffer contains the layer each position will sample its texture from
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, m_inputImage.rows * m_inputImage.cols * sizeof(InstanceData), m_positions, GL_DYNAMIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO2));
	GLCall(glBufferData(GL_ARRAY_BUFFER, m_inputImage.rows * m_inputImage.cols * sizeof(InstanceData), m_colors, GL_DYNAMIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

	/*GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFBO));
	GLCall(glViewport(0, 0, m_inputImage.image.cols, m_inputImage.image.rows));
	glClear(GL_COLOR_BUFFER_BIT);
	sobelShader.Bind();
	GLCall(glBindVertexArray(testVAO));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_inputTex));
	GLCall(glUniform2i(sobelShader.GetUniform("outputSize"), m_inputImage.image.cols, m_inputImage.image.rows));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	GLCall(glBindVertexArray(0));
	sobelShader.Unbind();*/
	//return { m_intermediate, (unsigned int)m_inputImage.image.cols, (unsigned int)m_inputImage.image.rows };

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFBO2));
	GLCall(glViewport(0, 0, m_inputImage.image.cols, m_inputImage.image.rows));
	glClear(GL_COLOR_BUFFER_BIT);
	sobelShader.Bind();
	GLCall(glBindVertexArray(testVAO));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_inputTex));
	GLCall(glUniform2i(sobelShader.GetUniform("outputSize"), m_inputImage.image.cols, m_inputImage.image.rows));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	GLCall(glBindVertexArray(0));
	sobelShader.Unbind();
	//return { m_intermediate2, (unsigned int)m_inputImage.image.cols, (unsigned int)m_inputImage.image.rows };

	computeShader.Bind();
	GLCall(glBindImageTexture(0, m_computeShaderOutput, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8));
	GLCall(glBindImageTexture(1, m_intermediate2, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8));
	GLCall(glDispatchCompute(ceil(m_inputImage.cols), ceil(m_inputImage.rows), 1));
	GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));
	return { m_computeShaderOutput, (unsigned int)m_inputImage.cols, (unsigned int)m_inputImage.rows };

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	GLCall(glViewport(0, 0, m_inputImage.image.cols, m_inputImage.image.rows));
	shader.Bind();
	GLCall(glUniform2f(shader.GetUniform("imgDims"), m_inputImage.cols, m_inputImage.rows));
	GLCall(glBindVertexArray(m_VAO));
	glClear(GL_COLOR_BUFFER_BIT);
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textArray));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_inputTex));
	GLCall(glActiveTexture(GL_TEXTURE2));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_computeShaderOutput));
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, m_inputImage.cols * m_inputImage.rows));
	GLCall(glBindVertexArray(0));
	shader.Unbind();
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	/*glClear(GL_COLOR_BUFFER_BIT);
	screenRenderShader.Bind();
	GLCall(glBindVertexArray(testVAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	GLCall(glBindVertexArray(0));
	screenRenderShader.Unbind();*/

	//return { m_computeShaderOutput, (unsigned int)m_inputImage.cols, (unsigned int)m_inputImage.rows };
	return { m_outTex, (unsigned int)m_inputImage.image.cols, (unsigned int)m_inputImage.image.rows };
}

void ascii_render::UpdateImage(const cv::Mat& image)
{
	ZoneScoped;
	//New img is different size to previously allocated.
	//(Check for nullptr to not delete when uninitialized)
	if (image.cols != m_inputImage.image.cols || image.rows != m_inputImage.image.rows) {
		m_inputImage.cols = image.cols / m_charSize;
		m_inputImage.rows = image.rows / m_charSize;
		if (m_positions != nullptr) {
			delete(m_positions);
		}
		if (m_colors != nullptr) {
			delete(m_colors);
		}
		m_positions = new InstanceData[m_inputImage.cols * m_inputImage.rows];
		m_colors = new InstanceData[m_inputImage.cols * m_inputImage.rows];
		for (int row = 0; row < m_inputImage.rows; row++) {
			for (int col = 0; col < m_inputImage.cols; col++) {
				m_positions[row * m_inputImage.cols + col] = { (float)col, (float)row, 31.0f };
				m_colors[row * m_inputImage.cols + col] = { 0.5, 0.5, 0.5 };
			}
		}
		GLCall(glDeleteTextures(1, &m_computeShaderOutput));
		GLCall(glGenTextures(1, &m_computeShaderOutput));
		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_computeShaderOutput));
		GLCall(glTextureStorage2D(m_computeShaderOutput, 1, GL_RGBA8, m_inputImage.cols, m_inputImage.rows));
		GLCall(glBindImageTexture(0, m_computeShaderOutput, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8));

		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

		GLCall(glBindTexture(GL_TEXTURE_2D, m_intermediate));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_intermediate2));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	}
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_inputTex));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.cols, image.rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data));
	GLCall(glActiveTexture(GL_TEXTURE0));
	m_inputImage.image = image;
}

void ascii_render::UpdateState(float charSize, int charRes, glm::vec4 bgColor, glm::vec4 charColor) {
	ZoneScoped;
	if (charRes != m_charRes) {
		m_charRes = charRes;
		LoadCharacterData(m_charRes);
	}

	if (bgColor != m_bgColor) {
		m_bgColor = bgColor;
		shader.Bind();
		GLCall(glUniform4f(shader.GetUniform("bgColor"), m_bgColor[0], m_bgColor[1], m_bgColor[2], m_bgColor[3]));
		shader.Unbind();
	}

	if (charColor != m_charColor) {
		m_charColor = charColor;
		shader.Bind();
		GLCall(glUniform4f(shader.GetUniform("charColor"), m_charColor[0], m_charColor[1], m_charColor[2], m_charColor[3]));
		shader.Unbind();
	}
}

void ascii_render::CalculateCharsFromLuminance() {
	ZoneScoped;
	unsigned char* pixelPtr = m_inputImage.image.data;
	int channels = m_inputImage.image.channels();
	if ((m_inputImage.image.type() & CV_MAT_DEPTH_MASK) != CV_8U || channels != 1 && channels != 4) {
		std::cout << "Unsupported Image type" << std::endl;
	}

	//Grayscale image
	if (channels == 1) {
		m_inputImage.image.forEach<unsigned char>([&](unsigned char& pixel, const int* pos) {
			//pos layout: [y, x]
			unsigned int index = pixel / ceil((float)255 / m_charset.length());
			m_positions[pos[0] * m_inputImage.image.cols + pos[1]].texArrayIndex = index;
			});
	}
	else { //RGB
		m_inputImage.image.forEach<cv::Vec4b>([&](cv::Vec4b& pixel, const int* pos) {
			//pos layout: [y, x]
			const int x = pos[1];
			const int y = pos[0];
			const int col = x / m_inputImage.image.cols * m_inputImage.cols;
			const int row = y / m_inputImage.image.rows * m_inputImage.rows;
			m_colors[row * m_inputImage.cols + col] = { (float)pixel[0] / 255, (float)pixel[1] / 255, (float)pixel[2] / 255};
			m_positions[row * m_inputImage.cols + col].texArrayIndex = (((float)pixel[0] + (float)pixel[1] + (float)pixel[2]) / 3) / ceil((float)255 / m_charset.length());
			});
	}
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
	delete[] buffer;
}
