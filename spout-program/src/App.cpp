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
#include <memory>
#include "sources/SpoutSource.h"
#include "sources/CamSource.h"
#include "outputs/VirtualCamera.h"
#include "gui/SourceCombo.h"
#include <outputs\SpoutSender.h>

void IconifyCallback(GLFWwindow* window, int iconified) {
    App::SetIconified(iconified);
}

App::App(GLFWwindow* window): 
    m_window(window),
    m_ImGuiIO(ImGui::GetIO()),
    m_ascii(),
    m_source(nullptr),
    m_sender(nullptr),
    m_sourceType(SourceType::None),
    m_outputType(OutputType::None) 
{
    glfwSetWindowIconifyCallback(window, IconifyCallback);
    m_spoutSource.Allocate(GL_RGBA, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE,  0);
    m_spoutSource.SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_spoutSource.SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_spoutSource.SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_spoutSource.SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void App::DrawGUI() {
    ZoneScoped;
    if (m_iconified) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    //ImGui::ShowDemoWindow();

    int window_w, window_h;
    glfwGetWindowSize(m_window, &window_w, &window_h);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(window_w / 5, window_h));
    
    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    
    ImGui::Text("Inputs");
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    //Selectable returns true if clicked that frame. Filter for only when *changing* selection
    if (ImGui::Selectable("Spout##Input", m_sourceType == SourceType::Spout) && m_sourceType != SourceType::Spout) {
        m_sourceType = SourceType::Spout;
        currentSourceName = "";
        m_source = std::make_unique<SpoutSource>(currentSourceName);
    }

    if (m_sourceType == SourceType::Spout) {
        SourceCombo("Spout Sources", currentSourceName, m_source.get());
    }

    if (ImGui::Selectable("Webcam", m_sourceType == SourceType::Webcam)) {
        if (m_sourceType != SourceType::Webcam) {
            ImGui::OpenPopup("Warning");
            m_sourceType = SourceType::Webcam;
            currentSourceName = "";
            m_source = std::make_unique<CamSource>();
        }
    }

    if (m_sourceType == SourceType::Webcam) {
        if (ImGui::BeginPopupModal("Warning", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Camera support is experimental - your webcam may not work properly.");
            if (ImGui::Button("OK", ImVec2(120, 0))) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        SourceCombo("Camera Sources", currentSourceName, m_source.get());
    }
    
    ImGui::Dummy(ImVec2(0.0f, 40.0f));

    ImGui::Text("Outputs");
    ImGui::Dummy(ImVec2(0.0f, 5.0f));

    if (ImGui::Selectable("Spout##Output", m_outputType == OutputType::Spout) && m_outputType != OutputType::Spout) {
        m_outputType = OutputType::Spout;
        m_sender = std::make_unique<SpoutEffects::SpoutSender>("Spout Effects");
    }

    if (ImGui::Selectable("Virtual Camera", m_outputType == OutputType::VirtualCam) && m_outputType != OutputType::VirtualCam) {
        m_outputType = OutputType::VirtualCam;
        m_sender = std::make_unique<SpoutEffects::VirtualCamera>("Spout Effects");
    }

    ImGui::SliderInt("Char Size", &m_charSize, 8, 32, "%d", ImGuiSliderFlags_AlwaysClamp);

    ImGui::Dummy(ImVec2(0.0f, 40.0f));
    ImGui::Text("Edge Detection Parameters");
    ImGui::SliderFloat("Epsilon", &Epsilon, 0.0f, 0.5f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SliderFloat("Phi", &Phi, 100.0f, 500.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SliderFloat("Sigma", &Sigma, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SliderFloat("k", &k, 0.0f, 3.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SliderFloat("p", &p, 0.0f, 1.0f, "%.3f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::End();

    if (m_source && m_sender) {
        ImGui::SetNextWindowPos(ImVec2(window_w / 5, 0));
        ImGui::SetNextWindowSize(ImVec2(ImGui::GetContentRegionAvail()), ImGuiCond_Once);
        ImGui::Begin("Spout Feed", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
        ImGui::Text("size = %d x %d \t | \t %.3f ms/frame (%.1f FPS)", m_source->GetWidth(), m_source->GetHeight(), 1000.0f / m_ImGuiIO.Framerate, m_ImGuiIO.Framerate);
        auto contentSpace = ImGui::GetContentRegionAvail();
        auto initialCursorPos = ImGui::GetCursorPos();
        ImGui::Image((void*)(intptr_t)m_spoutSource.GetID(), ImVec2(contentSpace.x/2, contentSpace.y));
        ImGui::SetCursorPos(ImVec2(initialCursorPos.x+ contentSpace.x / 2, initialCursorPos.y));
        ImGui::Image((void*)(intptr_t)outputTex.id, ImVec2(contentSpace.x / 2, contentSpace.y));
        ImGui::End();
    }

    //ImGui::ShowMetricsWindow();

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
    if (!m_source || !m_sender) {
        return;
    }

    m_source->GetNextFrame(m_spoutSource.GetID(), GL_TEXTURE_2D);
    
    m_ascii.UpdateState(m_charSize, 16,
        { 0.0, 0.0, 0.0, 0.0 },
        { 0.0, 0.0, 0.0, 0.0 },
        Epsilon, Phi, Sigma, k, p
    );
    outputTex = m_ascii.Draw(m_spoutSource.GetID());

    m_sender->SendTexture(outputTex.id, outputTex.w, outputTex.h);
}