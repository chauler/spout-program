#pragma once
#include "SpoutLibrary/SpoutLibrary.h"
#include "effects/ascii_render.h"
#include "Texture2D.h"
#include "sources/ISource.h"
#include "outputs/IOutput.h"
#include <memory>
#include "sources/SpoutSource.h"

enum class SourceType: unsigned int { SpoutSource, CamSource, None };

class App {
public:
	App(GLFWwindow*);
	void DrawGUI();
	void RunLogic();
	static void SetIconified(int iconified) { m_iconified = iconified; }
private:
	GLFWwindow* m_window = nullptr;
	SPOUTLIBRARY* m_receiver = nullptr;
	SPOUTLIBRARY* m_sender = nullptr;
	Texture2D m_spoutSource{};
	std::shared_ptr<unsigned char[]> m_spoutSourceData{};
	unsigned int m_imageW = 0, m_imageH = 0;
	ImGuiIO& m_ImGuiIO;
    ascii_render m_ascii;
	std::unique_ptr<ISource> m_source;
	SourceType sourceType = SourceType::None;
	inline static int m_iconified = 0;

	IOutput* test;

	//GUI State
	unsigned int spoutSourceID = 0;
	std::string currentSourceName{};
	bool popupOpen = false;
	SpoutOutTex outputTex{};

	//App State editable in the GUI
	int m_charSize = 10;
	int m_cols = 100, m_rows = 100;
	float m_bgColor[4]{1.0f, 1.0f, 1.0f, 0.0f};
	float m_charColor[4]{1.0f, 1.0f, 1.0f, 1.0f};
	float Epsilon = 0.015;
	float Phi = 200.0;
	float Sigma = 0.083;//0.156;
	float k = 2.26;
	float p = 1.00;
};
