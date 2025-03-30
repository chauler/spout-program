#pragma once
#include <functional>
#include "gui/GuiManager.h"
#include "outputs/IOutput.h"
#include "gui/EffectListItem.h"
#include "Window.h"

class ConfigPanel : public Window {
public:
	ConfigPanel(GuiManager* const gui, std::weak_ptr<std::unique_ptr<ISource>> source, std::weak_ptr<std::unique_ptr<IOutput>> output, std::vector<EffectListItem>& effects);
	void Render();
private:
	template <typename T>
	void DrawInputComponent(const std::string&, const SourceType componentType);
	template <typename T>
	void DrawOutputComponent(const std::string&, const OutputType componentType);
	GuiManager* const m_gui;
	SourceType m_sourceType;
	OutputType m_outputType;
	std::string m_currentSourceName;
	bool m_asciiEnabled = false;
	bool m_edgesEnabled = false;
	bool m_invertEnabled = false;
	int m_selectedEffect = -1;
	std::vector<EffectListItem>& m_effects;
	std::weak_ptr<std::unique_ptr<ISource>> m_source;
	std::weak_ptr<std::unique_ptr<IOutput>> m_sender;
};
