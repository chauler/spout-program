#pragma once
#include "IOutput.h"
#include <SpoutGL\SpoutSender.h>
#include <memory>

namespace SpoutEffects {
	class SpoutSender: public IOutput {
	public:
		SpoutSender(const std::string&);
		~SpoutSender() { m_sender->ReleaseSender(); }
		void SendTexture(unsigned int textureID, unsigned int width, unsigned int height) override;
		void SetTargetName(const std::string& name) override;
	private:
		std::unique_ptr<::SpoutSender> m_sender;
	};
}