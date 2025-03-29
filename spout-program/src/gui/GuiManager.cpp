#include "GuiManager.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

GuiManager::GuiManager(GLFWwindow* window) : defaultFont(), m_fonts() {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 430");
}

GuiManager::~GuiManager() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

ImFont* GuiManager::LoadFont(const std::string& label, const std::string& path, float fontSize) {
    const ImGuiIO& io = ImGui::GetIO();

    ImFont* font = io.Fonts->AddFontFromFileTTF(path.c_str(), fontSize);
    if (!font) {
        return nullptr;
    }

    if (!defaultFont) {
        defaultFont = font;
    }

    m_fonts.insert({ label, font });
    return font;
}

ImFont* GuiManager::GetFont(const std::string& label) {
    auto entryIt = m_fonts.find(label);
    //Font was found
    if (entryIt != m_fonts.end()) {
        return entryIt->second;
    }
    else {
        return nullptr;
    }
}
