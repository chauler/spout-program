#pragma once
#include "SpoutLibrary/SpoutLibrary.h"
#include "Texture2D.h"
#include "sources/ISource.h"
#include "outputs/IOutput.h"
#include <memory>
#include "sources/SpoutSource.h"
#include <effects/IEffect.h>

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
    std::vector<IEffect*> m_effects;
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
	bool asciiEnabled = false;
};
