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
#include <algorithm>
#include "sources/SpoutSource.h"
#include "sources/CamSource.h"
#include "outputs/VirtualCamera.h"
#include "gui/SourceCombo.h"
#include <outputs/SpoutSender.h>
#include "effects/ascii_render.h"
#include "effects/Edges.h"
#include "effects/Invert.h"

void IconifyCallback(GLFWwindow* window, int iconified) {
    App::SetIconified(iconified);
}

App::App(GLFWwindow* window): 
    m_window(window),
    m_ImGuiIO(ImGui::GetIO()),
    m_effects(std::vector<EffectListItem>{}),
    m_source(nullptr),
    m_sender(nullptr),
    m_sourceType(SourceType::None),
    m_outputType(OutputType::None),
    m_builtInInversion()
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
    ImGui::ShowDemoWindow();

    int window_w, window_h;
    glfwGetWindowSize(m_window, &window_w, &window_h);

    const ImVec2 mainPanelPos{0.0f, 0.0f};
    const ImVec2 mainPanelDims{ window_w / 5.0f, static_cast<float>(window_h)};
    ImGui::SetNextWindowPos(mainPanelPos);
    ImGui::SetNextWindowSize(mainPanelDims);
    
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

    if (ImGui::BeginListBox("Effects", ImVec2(0, (m_effects.size() + 1) * ImGui::GetTextLineHeightWithSpacing()))) {
        for (unsigned int i = 0; i < m_effects.size(); i++) {
            std::string label = m_effects[i].label;
            ImGui::Text(label.c_str());

            if (ImGui::BeginDragDropTarget()) {
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Effect")) {
                    EffectListItem* effect;
                    memcpy(&effect, payload->Data, payload->DataSize);
                    m_effects.insert(m_effects.begin()+i+1, *effect);
                }
                else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EffectList", ImGuiDragDropFlags_AcceptPeekOnly)) {
                    unsigned int incomingPos = *(unsigned int*)payload->Data;
                    ImVec2 screenPos = ImGui::GetCursorScreenPos();
                    float heightOffset = 0;
                    if (i < incomingPos) {
                        heightOffset = ImGui::GetTextLineHeight();
                    }
                    float separatorWidth = 2.0f;
                    ImGui::GetForegroundDrawList()->AddLine({ screenPos.x, screenPos.y - heightOffset - separatorWidth}, {screenPos.x + ImGui::CalcTextSize(label.c_str()).x, screenPos.y - heightOffset - separatorWidth}, ImGui::ColorConvertFloat4ToU32({1.0, 0.0, 0.0, 1.0}), separatorWidth);
                    //We run the whole block whenever the mouse hovers. Then, on mouse release, we run this.
                    if (payload->IsDelivery()) {
                        //Incoming item is being moved earlier in the list, shift right
                        if (i < incomingPos) {
                            std::rotate(m_effects.begin() + i, m_effects.begin() + incomingPos, m_effects.begin() + incomingPos + 1);
                        }
                        else { //Shift left
                            std::rotate(m_effects.begin() + incomingPos, m_effects.begin() + incomingPos + 1, m_effects.begin() + i + 1);
                        }
                    }
                }
                ImGui::EndDragDropTarget();
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
                ImGui::SetDragDropPayload("EffectList", &i, sizeof(i));
                ImGui::EndDragDropSource();
            }
        }
        ImGui::EndListBox();
    }

    if (ImGui::BeginDragDropTarget()) {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Effect")) {
            EffectListItem* effect;
            memcpy(&effect, payload->Data, payload->DataSize);
            m_effects.push_back(*effect);
        }
        ImGui::EndDragDropTarget();
    }
    
    for (int i = 0; const auto & effect : m_effects) {
        ImGui::PushID(i);
        effect.effect->DisplayGUIComponent();
        ImGui::PopID();
        i++;
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

    ImGui::End();

    ImVec2 listPanelPos{mainPanelDims.x, 0.0f};
    ImVec2 listPanelDims{ std::max(100.f, window_w * 0.1f), static_cast<float>(window_h)};
    ImGui::SetNextWindowPos(listPanelPos);
    ImGui::SetNextWindowSize(listPanelDims);
    ImGui::Begin("Effects", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);
    ImGui::Text("Effects");
    ImGui::PushID("EffectList");

    ImGui::Button("ASCII");
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        EffectListItem* effect = new EffectListItem{ .effect{new ascii_render()}, .label{"ASCII"} };
        ImGui::SetDragDropPayload("Effect", &effect, sizeof(effect));
        ImGui::EndDragDropSource();
    }

    ImGui::Button("Edges");
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        EffectListItem* effect = new EffectListItem{ .effect{new Edges()}, .label{"Edges"} };
        ImGui::SetDragDropPayload("Effect", &effect, sizeof(effect));
        ImGui::EndDragDropSource();
    }

    ImGui::Button("Invert");
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None)) {
        EffectListItem* effect = new EffectListItem{ .effect{new Invert()}, .label{"Invert"} };
        ImGui::SetDragDropPayload("Effect", &effect, sizeof(effect));
        ImGui::EndDragDropSource();
    }
    ImGui::PopID();
    ImGui::End();

    if (m_source && m_sender) {
        const ImVec2 previewPanelPos{listPanelPos.x + listPanelDims.x, 0.0f};
        const ImVec2 previewPanelDims{ static_cast<float>(window_w) - previewPanelPos.x, static_cast<float>(window_h) };
        ImGui::SetNextWindowPos(previewPanelPos);
        ImGui::SetNextWindowSize(previewPanelDims);
        ImGui::Begin("Spout Feed", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
        ImGui::Text("size = %d x %d \t | \t %.3f ms/frame (%.1f FPS)", m_source->GetWidth(), m_source->GetHeight(), 1000.0f / m_ImGuiIO.Framerate, m_ImGuiIO.Framerate);
        auto contentSpace = ImGui::GetContentRegionAvail();
        auto initialCursorPos = ImGui::GetCursorPos();
        ImGui::Image((void*)(intptr_t)m_spoutSource.GetID(), ImVec2(contentSpace.x/2, contentSpace.y));
        ImGui::SetCursorPos(ImVec2(initialCursorPos.x+ contentSpace.x / 2, initialCursorPos.y));
        ImGui::Image((void*)(intptr_t)outputTex.id, ImVec2(contentSpace.x / 2, contentSpace.y));
        ImGui::End();
    }

    

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
    
    outputTex.id = m_spoutSource.GetID();
    uVec2 dims = m_spoutSource.GetDimensions();
    outputTex.w = dims.x;
    outputTex.h = dims.y;

    for (const auto& effect : m_effects) {
        outputTex = effect.effect->Draw(outputTex.id);
    }

    //Our texture is inverted on the y axis when we receive it from Spout, correct that
    outputTex = m_builtInInversion.Draw(outputTex.id);
    m_sender->SendTexture(outputTex.id, outputTex.w, outputTex.h);
}