#pragma once

namespace SpoutProgram::Gui {
    template<typename Fn>
    inline void LabeledWidget(const char* label, Fn widgetFunc, float labelWidth = -1.0f) {
        // Reserve space for label
        ImGui::PushID(label);
        ImGui::AlignTextToFramePadding();

        // You can tweak labelWidth to whatever fixed width you like (or compute it via CalcTextSize)
        ImGui::PushItemWidth(ImGui::CalcTextSize(label).x);
        // Draw the label
        ImGui::TextUnformatted(label);
        ImGui::PopItemWidth();

        ImGui::SameLine();

        // Draw the actual widget (with its own width settings, if needed)
        widgetFunc();

        ImGui::PopID();
    }
}