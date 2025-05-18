#include "Renderer.h"
#include "VirtualCamera.h"
#include "SpoutGL/SpoutCopy.h"

SpoutEffects::VirtualCamera::VirtualCamera(std::string name) : 
    m_w(4),
    m_h(4),
    m_camera(scCreateCamera(m_w, m_h, 60)),
    m_data(m_w * m_h * 4),
    m_convertedData(m_w* m_h * 3)
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
    int real_w = 0, real_h = 0, row_padding = 0, col_padding = 0;
    GLCall(glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_WIDTH, &real_w));
    GLCall(glGetTextureLevelParameteriv(id, 0, GL_TEXTURE_HEIGHT, &real_h));
    real_w = (real_w + 3) & ~0x03;
    real_h = (real_h + 3) & ~0x03;
    GLCall(glPixelStorei(GL_PACK_ALIGNMENT, 4));
    GLCall(glPixelStorei(GL_PACK_ROW_LENGTH, real_w));
    row_padding = real_w - width;
    col_padding = real_h - height;

    if (m_w != real_w || m_h != real_h) {
        scDeleteCamera(m_camera);
        m_w = real_w;
        m_h = real_h;
        m_camera = scCreateCamera(m_w, m_h, 60);
        m_data = std::vector<unsigned char>(m_w * m_h * 4);
        m_convertedData = std::vector<unsigned char>(m_w * m_h * 3);
    }

    //We have set up OpenGL to read in chunks of 4, so if we have less than a 4x4 input (e.g. no input), just fill zeros to avoid read access errors.
    if (width < 4 || height < 4) {
        std::fill(m_data.begin(), m_data.end(), 0);
    }
    else {
        GLCall(glBindTexture(GL_TEXTURE_2D, id));
        GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_data.data()));
        GLCall(glBindTexture(GL_TEXTURE_2D, 0));
        spoutCopy copier{};
        copier.rgba2bgr(m_data.data(), m_convertedData.data(), real_w, real_h, true);
    }
    scSendFrame(m_camera, m_convertedData.data());
}

void SpoutEffects::VirtualCamera::SetTargetName(const std::string& name) {
	if (m_name == name) {
		return;
	}

    m_name = name;
	RegisterSoftcam(m_name);
}

std::string SpoutEffects::VirtualCamera::GetTargetName() const {
	return m_name;
}

int SpoutEffects::VirtualCamera::ExecSoftcamInstaller(const std::string& args) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    if (!CreateProcessA(
        "C:\\Windows\\system32\\cmd.exe", // Path to the executable
        const_cast<LPSTR>(args.c_str()),           // Command line arguments
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        CREATE_NO_WINDOW,// No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)            // Pointer to PROCESS_INFORMATION structure
        )
    {
        std::cerr << "CreateProcess failed (" << GetLastError() << ")\n";
        return 1;
    }

    std::cout << args << std::endl;
    std::cout << "Process created with ID: " << pi.dwProcessId << std::endl;

    // Optionally wait for the process to finish
    WaitForSingleObject(pi.hProcess, INFINITE);

    // Close process and thread handles
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
	return 0;
}

#if defined(DEBUG)
#define DLL_PATH "softcamd.dll"
#else
#define DLL_PATH "softcam.dll"
#endif

int SpoutEffects::VirtualCamera::RegisterSoftcam(const std::string name) {
    return ExecSoftcamInstaller("cmd /c SoftcamInstaller.exe register " DLL_PATH " \"" + name + "\"");
}

int SpoutEffects::VirtualCamera::DeregisterSoftcam() {
    return ExecSoftcamInstaller("cmd /c SoftcamInstaller.exe unregister " DLL_PATH);
}
