#include <SpoutLibrary/SpoutLibrary.h>
#include "SpoutSource.h"
#include "Renderer.h"
#include "tracy/public/tracy/Tracy.hpp"

SpoutSource::SpoutSource(std::string name)
{
    m_receiver = GetSpout();
    m_receiver->SetReceiverName(name.c_str());
}

void SpoutSource::GetNextFrame(GLuint id, GLuint textureTarget)
{
    ZoneScoped;

    if (m_receiver->ReceiveTexture() && m_receiver->BindSharedTexture() /*&& m_receiver->IsFrameNew()*/) {
        // Bind to get access to the shared texture
        // Get the shared texture ID and do something with it
        GLuint sharedTexID = m_receiver->GetSharedTextureID();
        // copy from the shared texture
        GLCall(glBindTexture(GL_TEXTURE_2D, id));
        //Re-allocate texture buffer if incoming image is different dimensions
        if (m_w != m_receiver->GetSenderWidth() || m_h != m_receiver->GetSenderHeight()) {
            m_w = m_receiver->GetSenderWidth();
            m_h = m_receiver->GetSenderHeight();
            //m_data = std::make_shared<unsigned char[]>(m_w * m_h * 4);
        }
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_w, m_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        m_receiver->CopyTexture(sharedTexID, GL_TEXTURE_2D,
            id,
            GL_TEXTURE_2D,
            m_w, m_h);
        //GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data.get()));
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));

        // Un-bind to release access to the shared texture
        m_receiver->UnBindSharedTexture();
        return;
    }
}

std::shared_ptr<unsigned char[]> SpoutSource::GetFrameData()
{
	return m_data;
}
