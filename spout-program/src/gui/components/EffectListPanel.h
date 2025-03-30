#pragma once
#include "gui/GuiManager.h"
#include "gui/components/Window.h"

class EffectListPanel : public Window {
public:
	EffectListPanel(GuiManager* const);
	void Render();
private:
	GuiManager* const m_gui;
};