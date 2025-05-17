#include <memory>
#include <algorithm>
#include "string.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "App.h"
#include "Texture2D.h"
#include <SpoutLibrary/SpoutLibrary.h>
#include "tracy/public/tracy/Tracy.hpp"
#include "sources/CamSource.h"
#include "outputs/VirtualCamera.h"
#include "gui/components/SourceCombo.h"
#include <outputs/SpoutSender.h>
#include "effects/ascii_render.h"
#include "effects/Edges.h"
#include "effects/Invert.h"
#include "gui/EffectListItem.h"

void IconifyCallback(GLFWwindow* window, int iconified) {
    App::SetIconified(iconified);
}

App::App(GLFWwindow* window) :
    m_source(std::make_shared<std::unique_ptr<ISource>>()),
    m_sender(std::make_shared<std::unique_ptr<IOutput>>()),
    m_window(window),
    m_gui(window),
    m_listPanel(&m_gui),
    m_previewPanel(&m_gui, m_spoutSource, outputTex),
    m_configPanel(&m_gui, m_source, m_sender, m_effects),
    m_effects(std::vector<EffectListItem>{}),
    m_builtInInversion()
{
    glfwSetWindowIconifyCallback(window, IconifyCallback);
    m_spoutSource.Allocate(GL_RGBA, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,  0);
    m_spoutSource.SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_spoutSource.SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_spoutSource.SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_spoutSource.SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    m_gui.LoadFont("OpenSans", "res/fonts/opensans/OpenSans-Regular.ttf", 18.0f);
    m_gui.LoadFont("ListEntries", "res/fonts/opensans/OpenSans-Regular.ttf", 18.0f);
    m_gui.LoadFont("Heading", "res/fonts/opensans/OpenSans-Bold.ttf", 27.0f);
}

void App::DrawGUI() {
    ZoneScoped;
    if (m_iconified) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    int window_w, window_h;
    glfwGetWindowSize(m_window, &window_w, &window_h);

    m_configPanel.pos = { 0.0f, 0.0f };
    m_configPanel.size = { window_w / 5.0f, static_cast<float>(window_h) };
    m_configPanel.Render();

    m_listPanel.pos = {m_configPanel.size.x, 0.0f};
    m_listPanel.size = { std::max(100.f, window_w * 0.1f), static_cast<float>(window_h)};
    m_listPanel.Render();

    m_previewPanel.pos = {m_listPanel.pos.x + m_listPanel.size.x, 0.0f};
    m_previewPanel.size = {static_cast<float>(window_w) - m_previewPanel.pos.x, static_cast<float>(window_h)};
    m_previewPanel.Render();

    //ImGui::ShowDemoWindow();

    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImVec4 bg_color = ImVec4(1.00f, 0.55f, 0.60f, 0.00f);
    glClearColor(bg_color.x * bg_color.w, bg_color.y * bg_color.w, bg_color.z * bg_color.w, bg_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::RunLogic() {
    ZoneScoped;
    if (!(*m_source) || !(*m_sender)) {
        return;
    }

    (*m_source)->GetNextFrame(m_spoutSource.GetID(), GL_TEXTURE_2D);
    
    outputTex.id = m_spoutSource.GetID();
    uVec2 dims = m_spoutSource.GetDimensions();
    outputTex.w = dims.x;
    outputTex.h = dims.y;

    for (const auto& effect : m_effects) {
        outputTex = effect.effect->Draw(outputTex.id);
    }

    //Our texture is inverted on the y axis when we receive it from Spout, correct that
    outputTex = m_builtInInversion.Draw(outputTex.id);
    (*m_sender)->SendTexture(outputTex.id, outputTex.w, outputTex.h);
}