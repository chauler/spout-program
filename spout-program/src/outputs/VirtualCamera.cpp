#include "Renderer.h"
#include "VirtualCamera.h"
#include "SpoutGL/SpoutCopy.h"

SpoutEffects::VirtualCamera::VirtualCamera(std::string name) : 
    m_camera(scCreateCamera(1920, 1084, 60)),
    m_data(1920* 1084 *4),
    m_convertedData(1920* 1084 *3)
{
    if (!m_camera)
    {
        std::printf("failed to create camera\n");
    }
    std::printf("Ok, Softcam is now active.\n");
}

SpoutEffects::VirtualCamera::~VirtualCamera() {
    scDeleteCamera(m_camera);
}

void SpoutEffects::VirtualCamera::SendTexture(unsigned int id, unsigned int width, unsigned int height) {
    
    GLCall(glBindTexture(GL_TEXTURE_2D, id));
    GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data.data()));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
    spoutCopy copier{};
    copier.rgba2bgr(m_data.data(), m_convertedData.data(), width, height, true);

    scSendFrame(m_camera, m_convertedData.data());
}

void SpoutEffects::VirtualCamera::SetTargetName(const std::string& name) {
    return;
}