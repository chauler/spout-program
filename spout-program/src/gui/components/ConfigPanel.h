#pragma once
#include <functional>
#include "gui/GuiManager.h"
#include "outputs/IOutput.h"
#include "gui/EffectListItem.h"
#include "Window.h"

template <typename T>
concept Source = std::derived_from<T, ISource>;

template <typename T>
concept Output = std::derived_from<T, IOutput>;

class ConfigPanel : public Window {
public:
	ConfigPanel(GuiManager* const gui, std::weak_ptr<std::unique_ptr<ISource>> source, std::weak_ptr<std::unique_ptr<IOutput>> output, std::vector<EffectListItem>& effects);
	void Render();
private:
	template <Source T>
	void DrawInputComponent(const std::string&, const SourceType componentType);
	template <Output T>
	void DrawOutputComponent(const std::string&, const OutputType componentType);
	GuiManager* const m_gui;
	SourceType m_sourceType;
	OutputType m_outputType;
	std::string m_currentSourceName;
	std::string m_currentOutputName;
	bool m_asciiEnabled = false;
	bool m_edgesEnabled = false;
	bool m_invertEnabled = false;
	int m_selectedEffect = -1;
	std::vector<EffectListItem>& m_effects;
	std::weak_ptr<std::unique_ptr<ISource>> m_source;
	std::weak_ptr<std::unique_ptr<IOutput>> m_sender;
};
