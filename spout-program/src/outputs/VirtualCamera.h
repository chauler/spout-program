#pragma once
#include <string>
#include <vector>
#include "IOutput.h"
#include "softcam.h"

namespace SpoutEffects {
	class VirtualCamera : public IOutput {
	public:
		VirtualCamera(std::string name);
		~VirtualCamera() override;
		void SendTexture(unsigned int id, unsigned int width, unsigned int height) override;
		void SetTargetName(const std::string&) override;
		std::string GetTargetName() const override;
	private:
		int ExecSoftcamInstaller(const std::string& args);
		int RegisterSoftcam(const std::string name = "\"DirectShow Softcam\"");
		int DeregisterSoftcam();

		scCamera m_camera;
		std::string m_name;
		std::vector<unsigned char> m_data;
		std::vector<unsigned char> m_convertedData;
		int m_w = 0, m_h = 0;
	};
}