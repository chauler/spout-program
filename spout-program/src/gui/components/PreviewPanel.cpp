#include "PreviewPanel.h"
#include "imgui/imgui.h"

PreviewPanel::PreviewPanel(GuiManager* const gui, const Texture2D& sourceTex, const SpoutOutTex& outputTex) : m_gui(gui), m_sourceTex(sourceTex), m_outputTex(outputTex)
{

}

void PreviewPanel::Render() {
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Spout Feed", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    ImGui::PushFont(m_gui->GetFont("Heading"));
    ImGui::Text("Video Preview:"); ImGui::SameLine();
    if (m_sourceTex.GetID() && m_outputTex.id) {
        const float framerate = ImGui::GetIO().Framerate;
        ImGui::Text("size = %d x %d \t | \t %.3f ms/frame (%.1f FPS)", m_sourceTex.GetDimensions().x, m_sourceTex.GetDimensions().y, 1000.0f / framerate, framerate);
        auto contentSpace = ImGui::GetContentRegionAvail();
        auto initialCursorPos = ImGui::GetCursorPos();
        const float gap = 5.0f;
        const ImVec2 imageDims{ contentSpace.x / 2 - gap, contentSpace.y };
        ImGui::Image(m_sourceTex.GetID(), imageDims);
        ImGui::GetForegroundDrawList()->AddLine({ pos.x + initialCursorPos.x + imageDims.x + gap, initialCursorPos.y }, { pos.x + initialCursorPos.x + imageDims.x + gap, initialCursorPos.y + imageDims.y }, ImGui::GetColorU32(ImGuiCol_HeaderHovered), gap);
        ImGui::SetCursorPos(ImVec2(initialCursorPos.x + imageDims.x + gap * 2.0f, initialCursorPos.y));
        ImGui::Image(m_outputTex.id, imageDims, {0, 1}, {1, 0});
    }
    else {
        ImGui::Text("Select both an Input and an Output.");
    }
    ImGui::PopFont();
    ImGui::End();
}
