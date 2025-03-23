#pragma once
#include <any>

struct SpoutOutTex {
	unsigned int id;
	unsigned int w;
	unsigned int h;
};

class IEffect {
public:
	virtual ~IEffect() {}
	virtual SpoutOutTex Draw(unsigned int) = 0;
	virtual void UpdateState(const std::any&) = 0;
	virtual void DisplayGUIComponent() = 0;
};
