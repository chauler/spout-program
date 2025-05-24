#pragma once
#include <GL/glew.h>
#include "gui/GuiManager.h"
#include "Texture2D.h"
#include "gui/components/Window.h"
#include "util/types.h"

namespace SpoutProgram::Gui {
class PreviewPanel : public Window {
public:
	PreviewPanel(GuiManager* const gui, const Texture2D& sourceTex, const SpoutOutTex& m_outputTex);
	void Render();
private:
	GuiManager* const m_gui;
	const Texture2D& m_sourceTex;
	const SpoutOutTex& m_outputTex;
};
}