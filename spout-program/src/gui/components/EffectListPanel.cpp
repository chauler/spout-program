#include "imgui/imgui.h"
#include "effects/ascii_render.h"
#include "effects/Edges.h"
#include "effects/Invert.h"
#include "gui/EffectListItem.h"
#include "EffectListPanel.h"

EffectListPanel::EffectListPanel(GuiManager* const gui) : m_gui(gui)
{

}

template <typename T>
static void DrawComponent(const std::string& label) {
    ImGui::Button(label.c_str());
    if (ImGui::IsItemHovered()) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    }
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID)) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
        EffectListItem* effect = new EffectListItem{ .effect{new T()}, .label{label.c_str()}};
        ImGui::SetDragDropPayload("Effect", &effect, sizeof(effect));
        ImGui::Text(effect->label.c_str());
        ImGui::EndDragDropSource();
    }
}

void EffectListPanel::Render() {
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
	//Make the background translucent gray
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.2f, 0.2f, 0.2f, 0.8f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	//Round the Window borders
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 2.0f);
    ImGui::Begin("Effects", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
    ImGui::PushFont(m_gui->GetFont("Heading"));
    ImGui::Text("Effects");
    ImGui::PopFont();
    ImGui::PushID("EffectList");

    DrawComponent<ascii_render>("ASCII");
    DrawComponent<Edges>("Edges");
    DrawComponent<Invert>("Invert");

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
    ImGui::PopID();
    ImGui::End();
    //ImGui::GetForegroundDrawList()->AddLine({ pos.x + initialCursorPos.x + imageDims.x + gap, initialCursorPos.y }, { pos.x + initialCursorPos.x + imageDims.x + gap, initialCursorPos.y + imageDims.y }, ImGui::ColorConvertFloat4ToU32({ 1.0, 0.0, 0.0, 1.0 }), gap);
}
