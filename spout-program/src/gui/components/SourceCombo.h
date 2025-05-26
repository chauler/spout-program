#pragma once
#include <string>
#include "sources/ISource.h"

namespace SpoutProgram::Gui {
	void SourceCombo(const std::string&, std::string&, ISource*);
}