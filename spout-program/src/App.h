#pragma once
#include "SpoutLibrary/SpoutLibrary.h"
#include "effects/ascii_render.h"
#include "Texture2D.h"
#include "sources/ISource.h"
#include "outputs/IOutput.h"
#include <memory>
#include "sources/SpoutSource.h"

enum class SourceType: unsigned int { None, Spout, Webcam };
enum class OutputType : unsigned int { None, Spout, VirtualCam };

class App {
public:
	App(GLFWwindow*);
	void DrawGUI();
	void RunLogic();
	static void SetIconified(int iconified) { m_iconified = iconified; }
private:
	inline static int m_iconified = 0;

	//Window management
	GLFWwindow* m_window;
	ImGuiIO& m_ImGuiIO;
    ascii_render m_ascii;
	std::unique_ptr<ISource> m_source;
	std::unique_ptr<IOutput> m_sender;
	SourceType m_sourceType;
	OutputType m_outputType;

	//GUI State
	unsigned int spoutSourceID = 0;
	std::string currentSourceName{};
	Texture2D m_spoutSource{};
	bool popupOpen = false;
	SpoutOutTex outputTex{};

	//App State editable in the GUI
	int m_charSize = 10;
	int m_cols = 100, m_rows = 100;
	float Epsilon = 0.015;
	float Phi = 200.0;
	float Sigma = 0.083;//0.156;
	float k = 2.26;
	float p = 1.00;
};
