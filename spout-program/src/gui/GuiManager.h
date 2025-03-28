#pragma once
#include <GLFW\glfw3.h>
#include "imgui/imgui.h"
#include <map>
#include <string>

class GuiManager {
public:
	GuiManager(GLFWwindow*);
	~GuiManager();
	GuiManager(const GuiManager& other) = delete;
	GuiManager& operator=(const GuiManager& other) = delete;
	ImFont* LoadFont(const std::string& label, const std::string& path, float fontSize);
private:
	std::map<std::string, ImFont*> m_fonts;
};