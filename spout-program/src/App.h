#pragma once
#include <memory>
#include "SpoutLibrary/SpoutLibrary.h"
#include "Texture2D.h"
#include "sources/ISource.h"
#include "outputs/IOutput.h"
#include "effects/IEffect.h"
#include "effects/Invert.h"
#include "gui/GuiManager.h"
#include "gui/components/EffectListPanel.h"
#include "gui/components/PreviewPanel.h"
#include "gui/components/ConfigPanel.h"
#include "gui/EffectListItem.h"

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
	GuiManager m_gui;

    std::vector<EffectListItem> m_effects;
	//These are pointers in the first palce for the runtime polymorphism. We want this instance to have ownership (source and output will never be
	//deleted elsewhere) but we want copies of the shared ptr to be able to assign a different object and have that be recognized here.
	std::shared_ptr<std::unique_ptr<ISource>> m_source;
	std::shared_ptr<std::unique_ptr<IOutput>> m_sender;

	//GUI State
	unsigned int spoutSourceID = 0;
	std::string currentSourceName{};
	Texture2D m_spoutSource{};
	bool popupOpen = false;
	SpoutOutTex outputTex{};
	EffectListPanel m_listPanel;
	SpoutProgram::Gui::PreviewPanel m_previewPanel;
	SpoutProgram::Gui::ConfigPanel m_configPanel;
	Invert m_builtInInversion;
};
