#pragma once
#include <any>
#include "util/types.h"

class IEffect {
public:
	virtual ~IEffect() {}
	virtual SpoutOutTex Draw(unsigned int) = 0;
	virtual void UpdateState(const std::any&) {};
	virtual void DisplayGUIComponent() = 0;
};
