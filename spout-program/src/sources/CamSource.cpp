#include "ISource.h"
#include "CamSource.h"
#include "Renderer.h"
#include <iostream>
#include "escapi/escapi.h"
#include "tracy/public/tracy/Tracy.hpp"

CamSource::CamSource(std::string name) {
	if (name == "") {
		m_w = 0;
		m_h = 0;
		m_data = nullptr;
		return;
	}

	m_w = 1920;
	m_h = 1080;
	m_data = std::make_unique<int[]>(m_w * m_h);
	m_captureParams = {
		.mTargetBuf = m_data.get(),
		.mWidth = (int)m_w,
		.mHeight = (int)m_h};

	m_deviceID = IDFromDeviceName(name);
	if (m_deviceID != -1) {
		m_open = initCapture(m_deviceID, &m_captureParams);
		doCapture(m_deviceID);
	}
}

CamSource::~CamSource() {
	if (m_deviceID != -1) {
		deinitCapture(m_deviceID);
	}
}

void CamSource::GetNextFrame(GLuint id, GLuint textureTarget) {
	ZoneScoped;
	if (m_deviceID == -1) {
		return;
	}

	while(!isCaptureDone(m_deviceID)) {
		
	}
	if (int error = getCaptureErrorLine(m_deviceID)) {
		std::cout << error << std::endl;
	}

	for (int i = 0; i < m_w * m_h; i++) {
		m_captureParams.mTargetBuf[i] = 
			(m_captureParams.mTargetBuf[i] & 0xff00ff00) | // 
			((m_captureParams.mTargetBuf[i] & 0xff) << 16) | //
			((m_captureParams.mTargetBuf[i] & 0xff0000) >> 16) | //
			0xff000000; //A
	}
	GLCall(glBindTexture(GL_TEXTURE_2D, id));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_w, m_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_captureParams.mTargetBuf));
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
	doCapture(m_deviceID);
	return;
}

void CamSource::SetTargetName(std::string targetName) {
	for (int i = 0; i < countCaptureDevices(); i++) {
		char deviceName[256] = {};
		getCaptureDeviceName(i, deviceName, 256);
		
		//We have a match
		if (!strcmp(targetName.c_str(), deviceName)) {
			//Release existing device if exists, request new device
			if (m_deviceID != -1) {
				deinitCapture(m_deviceID);
			}
			m_deviceID = i;
			m_w = 1920;
			m_h = 1080;
			m_captureParams.mWidth = m_w;
			m_captureParams.mHeight = m_h;
			m_data = std::make_unique<int[]>(m_w * m_h);
			m_captureParams.mTargetBuf = m_data.get();
			m_open = initCapture(m_deviceID, &m_captureParams);
			doCapture(m_deviceID);
			break;
		}
	}
}

std::vector<std::string> CamSource::EnumerateTargets() {
	std::vector<std::string> targets{};
	for (int i = 0; i < countCaptureDevices(); i++) {
		char name[256] = {};
		getCaptureDeviceName(i, name, 256);
		targets.push_back(name);
	}
	return targets;
}

int CamSource::IDFromDeviceName(std::string targetName) {
	for (int i = 0; i < countCaptureDevices(); i++) {
		char deviceName[256] = {};
		getCaptureDeviceName(i, deviceName, 256);
		if (!strcmp(deviceName, targetName.c_str())) {
			return i;
		}
	}
	return -1;
}