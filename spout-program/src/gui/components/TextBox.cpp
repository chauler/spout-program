#include "./TextBox.h"

static int SpoutProgram::Gui::InputTextCallback(ImGuiInputTextCallbackData* data) {
	auto userData = (InputTextCallback_UserData*)data->UserData;
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize) {
		std::string* str = userData->str;
		str->resize(data->BufTextLen);
		data->Buf = str->data();
	}
	return 0;
}

bool SpoutProgram::Gui::TextBox(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, void* user_data) {
	return ImGui::InputText(label, buf, buf_size, flags | ImGuiInputTextFlags_CallbackResize, InputTextCallback, user_data);
}
