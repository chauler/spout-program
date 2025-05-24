#pragma once
#include <string>
#include "imgui/imgui.h"

namespace SpoutProgram::Gui {
	struct InputTextCallback_UserData {
		std::string* str;
	};

	static int InputTextCallback(ImGuiInputTextCallbackData* data);

	bool TextBox(const char* label, char* buf, size_t buf_size, ImGuiInputTextFlags flags, void* user_data);
}