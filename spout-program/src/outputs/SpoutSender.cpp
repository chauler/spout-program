#include <memory>
#include <GL\glew.h>
#include "SpoutSender.h"

SpoutEffects::SpoutSender::SpoutSender(const std::string& name) : m_sender(new ::SpoutSender()) {
	SetTargetName(name.c_str());
}

void SpoutEffects::SpoutSender::SendTexture(unsigned int textureID, unsigned int width, unsigned int height) {
	m_sender->SendTexture(textureID, GL_TEXTURE_2D, width, height);
}

void SpoutEffects::SpoutSender::SetTargetName(const std::string& name) {
	m_sender->ReleaseSender();
	m_sender = std::make_unique<::SpoutSender>();
	m_sender->SetSenderName(name.c_str());
}

std::string SpoutEffects::SpoutSender::GetTargetName() const {
	return m_sender->GetName();
}