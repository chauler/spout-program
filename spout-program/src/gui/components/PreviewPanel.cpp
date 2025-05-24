#include "PreviewPanel.h"
#include "imgui/imgui.h"
#include <iostream>
#include "util/util.h"

SpoutProgram::Gui::PreviewPanel::PreviewPanel(GuiManager* const gui, const Texture2D& sourceTex, const SpoutOutTex& outputTex) : m_gui(gui), m_sourceTex(sourceTex), m_outputTex(outputTex)
{

}

void SpoutProgram::Gui::PreviewPanel::Render() {
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Spout Feed", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
    ImGui::PushFont(m_gui->GetFont("Heading"));
    ImGui::Text("Video Preview:"); ImGui::SameLine();
    //Replace GetID() with something else, all Texture2D objects generate a texture on construction.
    if (m_sourceTex.GetID() && m_outputTex.id) {
        const float framerate = ImGui::GetIO().Framerate;
        ImGui::Text("size = %d x %d \t | \t %.3f ms/frame (%.1f FPS)", m_sourceTex.GetDimensions().x, m_sourceTex.GetDimensions().y, 1000.0f / framerate, framerate);
        auto contentSpace = ImGui::GetContentRegionAvail();
        auto initialCursorPos = ImGui::GetCursorPos();
        const float gap = 5.0f;
        const ImVec2 maxDims{ contentSpace.x / 2 - gap, contentSpace.y }; //The max space allocated for each image.
		//Get each image to fit within the allocated space while maintaining the aspect ratio.
		uVec2 sourceDims = m_sourceTex.GetDimensions();
        ImVec2 imageDims{};
		double scaleFactor = Util::ComputeScaleFactor(sourceDims.x, sourceDims.y, maxDims.x, maxDims.y);
        if (scaleFactor < 1.0) {
            imageDims.x = sourceDims.x * scaleFactor;
            imageDims.y = sourceDims.y * scaleFactor;
        }
        ImGui::Image(m_sourceTex.GetID(), imageDims);
        ImGui::GetForegroundDrawList()->AddLine({ pos.x + initialCursorPos.x + maxDims.x + gap, initialCursorPos.y }, { pos.x + initialCursorPos.x + maxDims.x + gap, initialCursorPos.y + maxDims.y }, ImGui::GetColorU32(ImGuiCol_HeaderHovered), gap);
        ImGui::SetCursorPos(ImVec2(initialCursorPos.x + maxDims.x + gap * 2.0f, initialCursorPos.y));

		scaleFactor = Util::ComputeScaleFactor(m_outputTex.w, m_outputTex.h, maxDims.x, maxDims.y);
		if (scaleFactor < 1.0) {
			imageDims.x = m_outputTex.w * scaleFactor;
			imageDims.y = m_outputTex.h * scaleFactor;
		}
        ImGui::Image(m_outputTex.id, imageDims);
	}
    else {
        ImGui::Text("Select both an Input and an Output.");
    }
    ImGui::PopFont();
    ImGui::End();
}
