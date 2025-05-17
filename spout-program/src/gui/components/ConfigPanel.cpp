#include <memory>
#include <functional>
#include "util/types.h"
#include "sources/SpoutSource.h"
#include "effects/ascii_render.h"
#include "effects/Edges.h"
#include "effects/Invert.h"
#include "gui/EffectListItem.h"
#include "ConfigPanel.h"
#include "gui/components/SourceCombo.h"
#include "sources/CamSource.h"
#include "outputs/SpoutSender.h"
#include "outputs/VirtualCamera.h"

struct InputTextCallback_UserData {
	std::string* str;
};

static int InputTextCallback(ImGuiInputTextCallbackData* data) {
	auto userData = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
		std::string* str = userData->str;
		str->resize(data->BufTextLen);
		data->Buf = str->data();
	}
    return 0;
}

static inline void DrawSelectableBg(ImU32 color)
{
    ImVec2 p_min = ImGui::GetItemRectMin();
    ImVec2 p_max = ImGui::GetItemRectMax();
    ImGui::GetWindowDrawList()->AddRectFilled(p_min, p_max, color);
}

ConfigPanel::ConfigPanel(GuiManager* const gui, std::weak_ptr<std::unique_ptr<ISource>> source, std::weak_ptr<std::unique_ptr<IOutput>> output, std::vector<EffectListItem>& effects) :
    m_gui(gui),
    m_currentSourceName(),
	m_currentOutputName(),
    m_sourceType(SourceType::None),
    m_outputType(OutputType::None),
    m_source(source),
    m_sender(output),
    m_effects(effects)
{

}

template<Source T>
void ConfigPanel::DrawInputComponent(const std::string& label, const SourceType componentType) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->ChannelsSplit(2);
    drawList->ChannelsSetCurrent(1);
    //Selectable returns true if clicked that frame. Filter for only when *changing* selection
    if (ImGui::Selectable(label.c_str(), m_sourceType == componentType) && m_sourceType != componentType) {
        m_sourceType = componentType;
        m_currentSourceName = "";
        if (auto source = m_source.lock()) {
            *source = std::make_unique<T>(m_currentSourceName);
        }
    }

    //Selectables are transparent if not hovered/selected/etc. We'll draw our own background.
    if (!ImGui::IsItemHovered() && m_sourceType != componentType) {
        drawList->ChannelsSetCurrent(0);
        DrawSelectableBg(ImGui::GetColorU32(ImGuiCol_FrameBg));
    }
    drawList->ChannelsMerge();

    if (m_sourceType == componentType) {
        if (auto source = m_source.lock()) {
            SourceCombo(label, m_currentSourceName, (*source).get());
        }
    }
}

template<Output T>
void ConfigPanel::DrawOutputComponent(const std::string& label, const OutputType componentType) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->ChannelsSplit(2);
    drawList->ChannelsSetCurrent(1);
    if (ImGui::Selectable(label.c_str(), m_outputType == componentType) && m_outputType != componentType) {
        m_outputType = componentType;
        if (auto sender = m_sender.lock()) {
            (*sender) = std::make_unique<T>("Spout Effects");
        }
    }
    if (!ImGui::IsItemHovered() && m_outputType != componentType) {
        drawList->ChannelsSetCurrent(0);
        DrawSelectableBg(ImGui::GetColorU32(ImGuiCol_FrameBg));
    }
    drawList->ChannelsMerge();

	if (m_outputType == componentType) {
		// Display editable text box for the output name
		ImGuiInputTextFlags flags = ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_CallbackResize;
        InputTextCallback_UserData data = {&m_currentOutputName};
        if (ImGui::InputText("Name:##OutputName", m_currentOutputName.data(), m_currentOutputName.capacity() + 1, flags, InputTextCallback, &data)) {
			std::cout << "Output name changed to: " << m_currentOutputName << std::endl;
			// If the name is changed, update the output name
            if (auto sender = m_sender.lock()) {
				if (m_currentOutputName.empty()) {
					m_currentOutputName = "Spout Effects";
				}
                (*sender)->SetTargetName(m_currentOutputName);
            }
        }
	}
}

void ConfigPanel::Render() {
    ImGui::SetNextWindowPos(pos);
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
    ImDrawList* drawList = ImGui::GetWindowDrawList();

    ImGui::PushFont(m_gui->GetFont("Heading"));
    ImGui::Text("Inputs");
    ImGui::PopFont();

    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    ImGui::PushID("Inputs");
    DrawInputComponent<SpoutSource>("Spout", SourceType::Spout);
    DrawInputComponent<CamSource>("Webcam", SourceType::Webcam);
    ImGui::PopID();
    ImGui::Dummy(ImVec2(0.0f, 40.0f));

    const float buttonHeight = ImGui::GetTextLineHeight() + ImGui::GetStyle().FramePadding.y * 2.0f;
    const float listBoxHeight = m_effects.size() > 5 ? 5 * buttonHeight : (m_effects.size() + 1) * buttonHeight;
    ImGui::BeginTable("##Effects", 2, ImGuiTableFlags_SizingFixedFit);
    ImGui::TableSetupColumn("Label", ImGuiTableColumnFlags_WidthFixed);
    ImGui::TableSetupColumn("List", ImGuiTableColumnFlags_WidthStretch);
    ImGui::TableNextRow();
    ImGui::TableNextColumn();
    ImGui::PushFont(m_gui->GetFont("Heading"));
    ImGui::Text("Effects");
    ImGui::PopFont();
    ImGui::TableNextColumn();
    const ImVec2 listBoxDims{ ImGui::GetContentRegionAvail().x, listBoxHeight };
    if (ImGui::BeginListBox("##Effects", listBoxDims)) {
        ImGui::PushFont(m_gui->GetFont("ListEntries"));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0.0f, 0.0f });
        for (unsigned int i = 0; i < m_effects.size(); i++) {
            ImGuiStyle& style = ImGui::GetStyle();
            std::string label = m_effects[i].label;
            ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { style.FramePadding.x, 0.0f });
            ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, { 0.0f, 0.0f });
            ImGui::PushID(i);

            ImGui::SetNextItemAllowOverlap();
            const ImVec2 initialPos = ImGui::GetCursorPos();
            const ImVec2 buttonDims{ ImGui::GetContentRegionAvail().x, 0 };
            //We must be careful when deleting the current effect. Decrement i and skip the event handling.
            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ style.ItemSpacing.x, style.CellPadding.y * 2 });
            if (ImGui::Selectable(label.c_str(), m_selectedEffect == i, NULL, buttonDims)) {
                if (m_selectedEffect == i) {
                    m_selectedEffect = -1;
                }
                else {
                    m_selectedEffect = i;
                }
            }

            if (ImGui::BeginDragDropTarget()) {
                const ImVec2 screenPos = ImGui::GetCursorScreenPos();
                const float separatorWidth = 2.0f;
                if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Effect", ImGuiDragDropFlags_AcceptPeekOnly)) {
                    ImGui::GetForegroundDrawList()->AddLine({ screenPos.x - style.FramePadding.x, screenPos.y - separatorWidth }, { screenPos.x + listBoxDims.x - style.FramePadding.x, screenPos.y - separatorWidth }, ImGui::GetColorU32(ImGuiCol_DragDropTarget), separatorWidth);
                    if (payload->IsDelivery()) {
                        EffectListItem* effect;
                        memcpy(&effect, payload->Data, payload->DataSize);
                        m_effects.insert(m_effects.begin() + i + 1, *effect);
                    }
                }
                else if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("EffectList", ImGuiDragDropFlags_AcceptPeekOnly)) {
                    unsigned int incomingPos = *(unsigned int*)payload->Data;
                    float heightOffset = 0;
                    if (i < incomingPos) {
                        heightOffset = buttonHeight;
                    }
                    ImGui::GetForegroundDrawList()->AddLine({ screenPos.x - style.FramePadding.x, screenPos.y - heightOffset - separatorWidth }, { screenPos.x + listBoxDims.x - style.FramePadding.x, screenPos.y - heightOffset - separatorWidth }, ImGui::GetColorU32(ImGuiCol_DragDropTarget), separatorWidth);
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
                ImGui::Text(label.c_str());
                ImGui::EndDragDropSource();
            }

            ImGui::SetCursorPos({ initialPos.x + buttonDims.x * 4 / 5, initialPos.y });
            ImGui::SetNextItemAllowOverlap();
            const ImVec2 delButtonDims{ buttonDims.x / 5, ImGui::GetFrameHeight() - style.FramePadding.y * 2.0f };
            if (ImGui::Button("X", delButtonDims)) {
                //We are deleting the currently selected effect
                if (m_selectedEffect == i) {
                    m_selectedEffect = -1;
                }
                m_effects.erase(m_effects.begin() + i, m_effects.begin() + i + 1);
                i--;
                ImGui::PopID();
                ImGui::PopStyleVar(4);
                continue;
            }

            ImGui::PopID();
            ImGui::PopStyleVar(4);
        }
        ImGui::Dummy({ 0.0f, buttonHeight });
        ImGui::PopStyleVar();
        ImGui::PopFont();
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
    ImGui::EndTable();

    //We know to skip if we set the index to -1.
    //Still, check to make sure that the index which was set is valid before accessing.
    if (m_selectedEffect != -1) {
        try {
            EffectListItem& effect = m_effects.at(m_selectedEffect);
            effect.effect->DisplayGUIComponent();
        }
        catch (const std::out_of_range& e) {
            m_selectedEffect = -1;
        }
    }

    ImGui::Dummy(ImVec2(0.0f, 40.0f));

    ImGui::PushFont(m_gui->GetFont("Heading"));
    ImGui::Text("Outputs");
    ImGui::PopFont();
    ImGui::Dummy(ImVec2(0.0f, 5.0f));
    DrawOutputComponent<SpoutEffects::SpoutSender>("Spout", OutputType::Spout);
    DrawOutputComponent<SpoutEffects::VirtualCamera>("Virtual Camera", OutputType::VirtualCam);
    
    ImGui::End();
}