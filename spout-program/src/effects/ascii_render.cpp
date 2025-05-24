#include <math.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "ascii_render.h"
#include "util/util.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include "tracy/public/tracy/Tracy.hpp"
#include <regex>

std::string ParseComputeShader(std::string&& input, unsigned int x, unsigned int y, unsigned int z);

ascii_render::ascii_render() : 
	m_fullscreenQuad(Quad()), 
	m_prevDimensions({ 0, 0 }), 
	m_config({
		.charSize{10},
		.numChars{16},
		.bgColor{0.0, 0.0, 0.0, 0.0},
		.charColor{1.0, 1.0, 1.0, 1.0},
		.epsilon{0.015f},
		.phi{200.0},
		.sigma{0.083f},
		.k{2.26f},
		.p{1.00f}
		}),
	m_charset(m_charSets[m_config.numChars]) 
{
	fontLoader.LoadFace("res/fonts/roboto/Roboto-Regular.ttf");
	m_face = fontLoader.GetFace();
	LoadCharacterData(m_config.charSize);

	std::string computeShaderSource = ParseComputeShader(SpoutProgram::Util::ReadFile("res/shaders/compute.cs"), m_config.charSize, m_config.charSize, 1);
	computeShader.AddShader(GL_COMPUTE_SHADER, computeShaderSource);
	computeShader.CompileShader();
	GLCall(glGenTextures(1, &m_computeShaderOutput));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_computeShaderOutput));
	GLCall(glTextureStorage2D(m_computeShaderOutput, 1, GL_RGBA8UI, 80, 60));
	GLCall(glBindImageTexture(0, m_computeShaderOutput, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	sobelShader.AddShader(GL_VERTEX_SHADER, SpoutProgram::Util::ReadFile("res/shaders/edge.vs"));
	sobelShader.AddShader(GL_FRAGMENT_SHADER, SpoutProgram::Util::ReadFile("res/shaders/edge.fs"));
	sobelShader.CompileShader();
	dGaussianShader.AddShader(GL_VERTEX_SHADER, SpoutProgram::Util::ReadFile("res/shaders/edge.vs"));
	dGaussianShader.AddShader(GL_FRAGMENT_SHADER, SpoutProgram::Util::ReadFile("res/shaders/gaussian.fs"));
	dGaussianShader.CompileShader();

	shader.AddShader(GL_VERTEX_SHADER, SpoutProgram::Util::ReadFile("res/shaders/text.vs"));
	shader.AddShader(GL_FRAGMENT_SHADER, SpoutProgram::Util::ReadFile("res/shaders/text.fs"));
	shader.CompileShader();
	shader.Bind();
	GLCall(glUniform4f(shader.GetUniform("bgColor"), m_config.bgColor[0], m_config.bgColor[1], m_config.bgColor[2], m_config.bgColor[3]));
	GLCall(glUniform4f(shader.GetUniform("charColor"), m_config.charColor[0], m_config.charColor[1], m_config.charColor[2], m_config.charColor[3]));
	GLCall(glUniform1i(shader.GetUniform("charSize"), m_config.charSize));
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	shader.Unbind();

	GLCall(glGenFramebuffers(1, &m_FBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	GLCall(glGenTextures(1, &m_outTex));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_outTex, 0));
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	GLCall(glDrawBuffers(1, DrawBuffers));
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		exit(1);
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	GLCall(glGenFramebuffers(1, &m_intermediateFBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFBO));
	GLCall(glGenTextures(1, &m_intermediate));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_intermediate));
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
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, 80, 60, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, 0));
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
	GLCall(glBindVertexArray(0));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

SpoutOutTex ascii_render::Draw(unsigned int imageID) {
	ZoneScoped;

	unsigned int cols, rows;
	GLCall(glBindTexture(GL_TEXTURE_2D, imageID));
	GLCall(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&cols));
	GLCall(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&rows));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	//If incoming texture is invalid, don't update anything and just return the previous result
	if (cols == 0 || rows == 0) {
		return { m_outTex, (unsigned int)cols, (unsigned int)rows };
	}

	UpdateImage(imageID);

	//This buffer contains the layer each position will sample its texture from
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_iVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, (rows / m_config.charSize) * (cols / m_config.charSize) * sizeof(InstanceData), m_positions, GL_DYNAMIC_DRAW));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFBO));
	GLCall(glViewport(0, 0, cols, rows));
	glClear(GL_COLOR_BUFFER_BIT);
	dGaussianShader.Bind();
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, imageID));
	m_fullscreenQuad.Draw();
	dGaussianShader.Unbind();
	//return { m_intermediate, (unsigned int)cols, (unsigned int)rows };

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_intermediateFBO2));
	GLCall(glViewport(0, 0, cols, rows));
	glClear(GL_COLOR_BUFFER_BIT);
	sobelShader.Bind();
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_intermediate));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, imageID));
	GLCall(glUniform2i(sobelShader.GetUniform("outputSize"), cols, rows));
	m_fullscreenQuad.Draw();
	sobelShader.Unbind();
	//return { m_intermediate2, (unsigned int)cols, (unsigned int)rows };

	computeShader.Bind();
	GLCall(glBindImageTexture(0, m_computeShaderOutput, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8UI));
	GLCall(glBindImageTexture(1, m_intermediate2, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8UI));
	GLCall(glDispatchCompute(ceil(cols / m_config.charSize), ceil(rows / m_config.charSize), 1));
	GLCall(glMemoryBarrier(GL_ALL_BARRIER_BITS));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	GLCall(glViewport(0, 0, m_outputW, m_outputH));
	shader.Bind();
	GLCall(glUniform2f(shader.GetUniform("imgDims"), ceilf(cols / m_config.charSize), ceilf(rows / m_config.charSize)));
	GLCall(glUniform1i(shader.GetUniform("numChars"), m_config.numChars));
	GLCall(glBindVertexArray(m_VAO));
	glClear(GL_COLOR_BUFFER_BIT);
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textArray));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, imageID));
	GLCall(glActiveTexture(GL_TEXTURE2));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_computeShaderOutput));
	GLCall(glActiveTexture(GL_TEXTURE3));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_edgeArray));
	GLCall(glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, cols / m_config.charSize * rows / m_config.charSize));
	GLCall(glBindVertexArray(0));
	shader.Unbind();
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, 0));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	return { m_outTex, (unsigned int)m_outputW, (unsigned int)m_outputH };
}

void ascii_render::UpdateImage(unsigned int imageID) {
	ZoneScoped;

	unsigned int cols, rows;
	GLCall(glBindTexture(GL_TEXTURE_2D, imageID));
	GLCall(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&cols));
	GLCall(glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&rows));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));

	//New img is different size to previously allocated.
	//(Check for nullptr to not delete when uninitialized)
	if (cols != m_prevDimensions.x || rows != m_prevDimensions.y) {
		m_prevDimensions.x = cols;
		m_prevDimensions.y = rows;
		unsigned int outCols = cols / m_config.charSize;
		unsigned int outRows = rows / m_config.charSize;
		unsigned int leftoverX = cols % m_config.charSize;
		unsigned int leftoverY = rows % m_config.charSize;
		m_outputW = cols - leftoverX;
		m_outputH = rows - leftoverY;
		if (m_positions != nullptr) {
			delete(m_positions);
		}
		m_positions = new InstanceData[outCols * outRows];
		for (int row = 0; row < outRows; row++) {
			for (int col = 0; col < outCols; col++) {
				m_positions[row * outCols + col] = { (float)col, (float)row, 31.0f };
			}
		}
		GLCall(glDeleteTextures(1, &m_computeShaderOutput));
		GLCall(glGenTextures(1, &m_computeShaderOutput));
		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_computeShaderOutput));
		GLCall(glTextureStorage2D(m_computeShaderOutput, 1, GL_RGBA8UI, outCols, outRows));
		GLCall(glBindImageTexture(0, m_computeShaderOutput, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA8UI));

		GLCall(glActiveTexture(GL_TEXTURE0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_outTex));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_outputW, m_outputH, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));

		GLCall(glBindTexture(GL_TEXTURE_2D, m_intermediate));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, cols, rows, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0));
		GLCall(glBindTexture(GL_TEXTURE_2D, m_intermediate2));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, cols, rows, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, 0));
	}
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	GLCall(glActiveTexture(GL_TEXTURE0));
}

void ascii_render::UpdateState(const std::any& input) {
	ZoneScoped;
	AsciiRenderState state;
	try {
		state = std::any_cast<AsciiRenderState>(input);
	}
	catch(const std::bad_any_cast& e) {
		std::cout << "Invalid state for AsciiRender; continuing previous state.\n";
		return;
	}

	if (state.bgColor != m_config.bgColor) {
		m_config.bgColor = state.bgColor;
		GLCall(glProgramUniform4f(shader.GetProgram(), shader.GetUniform("bgColor"), m_config.bgColor[0], m_config.bgColor[1], m_config.bgColor[2], m_config.bgColor[3]));
	}

	if (state.charColor != m_config.charColor) {
		m_config.charColor = state.charColor;
		GLCall(glProgramUniform4f(shader.GetProgram(), shader.GetUniform("charColor"), m_config.charColor[0], m_config.charColor[1], m_config.charColor[2], m_config.charColor[3]));
	}

	if (state.charSize != m_config.charSize) {
		m_config.charSize = state.charSize;
		GLCall(glProgramUniform1i(shader.GetProgram(), shader.GetUniform("charSize"), m_config.charSize));
		std::string computeShaderSource = ParseComputeShader(SpoutProgram::Util::ReadFile("res/shaders/compute.cs"), m_config.charSize, m_config.charSize, 1);
		Shader temp = Shader();
		temp.AddShader(GL_COMPUTE_SHADER, computeShaderSource);
		temp.CompileShader();
		computeShader = std::move(temp);
		LoadCharacterData(m_config.charSize);
		//TODO: MAKE A BETTER SOLUTION.
		//Need to reallocate m_positions and m_computeShaderOutput when charSize changes, but we can't know 
		//if charSize changes within UpdateImage(), where we currently reallocate. This prompts reallocation.
		m_prevDimensions.x = 0;
	}

	if (state.numChars != m_config.numChars) {
		try {
			m_config.numChars = state.numChars;
			m_charset = m_charSets.at(m_config.numChars);
			GLCall(glProgramUniform1f(shader.GetProgram(), shader.GetUniform("numChars"), m_config.numChars));
			LoadCharacterData(m_config.charSize);
		}
		catch (const std::out_of_range& e) {
			
		}
	}

	GLCall(glProgramUniform1f(dGaussianShader.GetProgram(), dGaussianShader.GetUniform("Epsilon"), state.epsilon));
	GLCall(glProgramUniform1f(dGaussianShader.GetProgram(), dGaussianShader.GetUniform("Phi"), state.phi));
	GLCall(glProgramUniform1f(dGaussianShader.GetProgram(), dGaussianShader.GetUniform("Sigma"), state.sigma));
	GLCall(glProgramUniform1f(dGaussianShader.GetProgram(), dGaussianShader.GetUniform("k"), state.k));
	GLCall(glProgramUniform1f(dGaussianShader.GetProgram(), dGaussianShader.GetUniform("p"), state.p));
}

void ascii_render::DisplayGUIComponent() {
	const unsigned int min = 8;
	const unsigned int max = 32;
	if (ImGui::SliderScalar("Char Size", ImGuiDataType_U32, &m_config.charSize, &min, &max, "%d", ImGuiSliderFlags_AlwaysClamp)) {
		GLCall(glProgramUniform1i(shader.GetProgram(), shader.GetUniform("charSize"), m_config.charSize));
		std::string computeShaderSource = ParseComputeShader(SpoutProgram::Util::ReadFile("res/shaders/compute.cs"), m_config.charSize, m_config.charSize, 1);
		Shader temp = Shader();
		temp.AddShader(GL_COMPUTE_SHADER, computeShaderSource);
		temp.CompileShader();
		computeShader = std::move(temp);
		LoadCharacterData(m_config.charSize);
		//TODO: MAKE A BETTER SOLUTION.
		//Need to reallocate m_positions and m_computeShaderOutput when charSize changes, but we can't know 
		//if charSize changes within UpdateImage(), where we currently reallocate. This prompts reallocation.
		m_prevDimensions.x = 0;
	}
	
	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	const auto updateCharset = [this]() {
		m_charset = m_charSets.at(m_config.numChars);
		GLCall(glProgramUniform1i(shader.GetProgram(), shader.GetUniform("numChars"), m_config.numChars));
		LoadCharacterData(m_config.charSize);
		};

	if (ImGui::RadioButton("8 Characters", &m_config.numChars, 8)) {
		updateCharset();
	}
	if (ImGui::RadioButton("16 Characters", &m_config.numChars, 16)) {
		updateCharset();
	}
	if (ImGui::RadioButton("32 Characters", &m_config.numChars, 32)) {
		updateCharset();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	ImGui::Text("Edge Detection Parameters");
	if (ImGui::SliderFloat("Epsilon", &m_config.epsilon, 0.0f, 0.5f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
		GLCall(glProgramUniform1f(dGaussianShader.GetProgram(), dGaussianShader.GetUniform("Epsilon"), m_config.epsilon));
	}
	if (ImGui::SliderFloat("Phi", &m_config.phi, 100.0f, 500.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp)) {
		GLCall(glProgramUniform1f(dGaussianShader.GetProgram(), dGaussianShader.GetUniform("Phi"), m_config.phi));
	}
	if (ImGui::SliderFloat("Sigma", &m_config.sigma, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
		GLCall(glProgramUniform1f(dGaussianShader.GetProgram(), dGaussianShader.GetUniform("Sigma"), m_config.sigma));
	}
	if (ImGui::SliderFloat("k", &m_config.k, 0.0f, 3.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp)) {
		GLCall(glProgramUniform1f(dGaussianShader.GetProgram(), dGaussianShader.GetUniform("k"), m_config.k));
	}
	if (ImGui::SliderFloat("p", &m_config.p, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp)) {
		GLCall(glProgramUniform1f(dGaussianShader.GetProgram(), dGaussianShader.GetUniform("p"), m_config.p));
	}
}

void ascii_render::LoadCharacterData(int textSize) {
	ZoneScoped;
	//Function may be called to re-allocate textures. Only gen texture if this is first call.
	if (m_textArray == 0) {
		GLCall(glGenTextures(1, &m_textArray));
		GLCall(glGenTextures(1, &m_edgeArray));
	}
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_edgeArray));
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	GLCall(glBindTexture(GL_TEXTURE_2D_ARRAY, m_textArray));
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
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
		wchar_t character = "|_\\/"[i];
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

std::string ParseComputeShader(std::string&& input, unsigned int x, unsigned int y, unsigned int z) {
	try {
		std::regex x_size_re("\\{local_size_x\\}");
		std::regex y_size_re("\\{local_size_y\\}");
		std::regex z_size_re("\\{local_size_z\\}");
		input = std::regex_replace(input, x_size_re, std::to_string(x));
		input = std::regex_replace(input, y_size_re, std::to_string(y));
		input = std::regex_replace(input, z_size_re, std::to_string(z));
	}
	catch (const std::regex_error& e) {
		std::cout << "regex_error: " << e.what() << std::endl;
	}
	return input;
}