#include <vector>
#include <imgui\imgui.h>
#include "SourceCombo.h"

void SourceCombo(const std::string& label, std::string& currentSourceName, ISource* source) {
    if (ImGui::BeginCombo((label + " Sources").c_str(), currentSourceName.c_str())) {
        std::vector<std::string> spoutSourceList = source->EnumerateTargets();

        for (const std::string& sourceName : spoutSourceList) {
            if (sourceName == "Spout Effects") {
                continue;
            }
            //Option was just selected - update receiver
            if (ImGui::Selectable(sourceName.c_str(), sourceName == currentSourceName)) {
                //Execute once on click
                if (sourceName != currentSourceName) {
                    source->SetTargetName(sourceName.c_str());
                }
                currentSourceName = sourceName;
            }

            if (sourceName == currentSourceName) {
                ImGui::SetItemDefaultFocus();
            }
        }

        ImGui::EndCombo();
    }
}