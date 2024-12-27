#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "App.h"
#include "Texture2D.h"
#include <SpoutLibrary.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include "tracy/public/tracy/Tracy.hpp"
#include <memory>
#include "sources/SpoutSource.h"
#include "sources/CamSource.h"

//Converts OpenGL texture to OpenCV mat for easier processing. Resize mat to provided dimensions.
//Argument of 0 for one of the dimensions means we scale it to match other dimension with same aspect ratio as texture.
//Argument of 0 for both dimensions means we do not resize the mat.
cv::Mat GetImageFromTexture(const GLuint texID, const unsigned int width=0, const unsigned int height=0) {
    ZoneScoped;
    GLCall(glBindTexture(GL_TEXTURE_2D, texID));
    GLenum gl_texture_width, gl_texture_height;

    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, (GLint*)&gl_texture_width);
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, (GLint*)&gl_texture_height);
    cv::Mat image(gl_texture_height, gl_texture_width, CV_8UC4);
    glPixelStorei(GL_PACK_ALIGNMENT, 1);
    //Temporarily removed. Performance consideration, but it causes crash when texture shrinks for some reason.
    //glPixelStorei(GL_PACK_ROW_LENGTH, image.step[0] / image.elemSize());
    GLCall(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.data));
    GLCall(glBindTexture(GL_TEXTURE_2D, 0));
    //If both are zero, skip all resizing.
    if (width != 0 || height != 0) {
        //Calculate width that keeps input aspect ratio
        if (width == 0) {
            //Cast to float to avoid integer division and divide by 0 errors
            unsigned int newWidth = gl_texture_width / ((float)gl_texture_height / height);
            cv::resize(image, image, cv::Size(newWidth, height));
        } //Calculate height that keeps input aspect ratio
        else if (height == 0) {
            unsigned int newHeight = gl_texture_height / ((float)gl_texture_width / width);
            cv::resize(image, image, cv::Size(width, newHeight));
        } //User input fixed width and height
        else {
            cv::resize(image, image, cv::Size(width, height));
        }
    }
    //cv::cvtColor(image, image, cv::COLOR_RGBA2GRAY);
    return image;
}

void IconifyCallback(GLFWwindow* window, int iconified) {
    std::cout << iconified << std::endl;
    App::SetIconified(iconified);
}

App::App(GLFWwindow* window): m_window(window), m_ImGuiIO(ImGui::GetIO()), m_ascii() {
    glfwSetWindowIconifyCallback(window, IconifyCallback);
    m_source = std::make_unique<SpoutSource>("VTubeStudioSpout");
    //m_source = std::make_unique<CamSource>();
    m_sender = GetSpout();
    m_sender->SetSenderName("SpoutProgram");
    m_spoutSource.Allocate(GL_RGBA, 1920, 1017, GL_RGBA, GL_UNSIGNED_BYTE, std::make_unique<unsigned char[]>(1920 * 1017 * 4).get());
    m_spoutSource.SetParameter(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    m_spoutSource.SetParameter(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    m_spoutSource.SetParameter(GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    m_spoutSource.SetParameter(GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void App::DrawGUI() {
    ZoneScoped;
    if (m_iconified) {
        return;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    int window_w, window_h;
    glfwGetWindowSize(m_window, &window_w, &window_h);
    ImGui::SetNextWindowPos(ImVec2());
    ImGui::SetNextWindowSize(ImVec2(window_w / 5, window_h));
    
    ImGui::ShowDemoWindow();

    ImGui::Begin("Options", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
    //Selectable returns true if clicked that frame. Filter for only when *changing* selection
    if (ImGui::Selectable("Spout", sourceType == SourceType::SpoutSource) && sourceType != SourceType::SpoutSource) {
        sourceType = SourceType::SpoutSource;
        //m_source.reset();
        m_source = std::make_unique<SpoutSource>("VTubeStudioSpout");
    }
    if (ImGui::Selectable("Cam", sourceType == SourceType::CamSource) && sourceType != SourceType::CamSource) {
        sourceType = SourceType::CamSource;
        //m_source.reset();
        m_source = std::make_unique<CamSource>();
    }
    ImGui::SliderFloat("Char Size", &m_charSize, 1.0f, 100.0f, "%.1f", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SliderInt("Width", &m_cols, 0, 1000, "%d", ImGuiSliderFlags_AlwaysClamp);
    ImGui::SliderInt("Height", &m_rows, 0, 1000, "%d", ImGuiSliderFlags_AlwaysClamp);
    ImGui::ColorPicker4("Background Color", m_bgColor, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
    ImGui::ColorPicker4("Character Color", m_charColor, ImGuiColorEditFlags_DisplayRGB | ImGuiColorEditFlags_Float);
    ImGui::End();

    ImGui::SetNextWindowPos(ImVec2(window_w / 5, 0));
    ImGui::SetNextWindowSize(ImVec2(window_w / 10, window_h / 10), ImGuiCond_Once);
    ImGui::Begin("Spout Feed", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_HorizontalScrollbar);
    ImGui::Image((void*)(intptr_t)m_spoutSource.GetID(), ImVec2(m_source->GetWidth(), m_source->GetHeight()));
    ImGui::Text("size = %d x %d", m_source->GetWidth(), m_source->GetHeight());
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / m_ImGuiIO.Framerate, m_ImGuiIO.Framerate);
    ImGui::End();
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImVec4 bg_color = ImVec4(1.00f, 0.55f, 0.60f, 0.00f);
    glClearColor(bg_color.x * bg_color.w, bg_color.y * bg_color.w, bg_color.z * bg_color.w, bg_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void App::RunLogic() {
    ZoneScoped;
    m_source->GetNextFrame(m_spoutSource.GetID(), GL_TEXTURE_2D);
    m_spoutSourceData = m_source->GetFrameData();
    cv::Mat mat = GetImageFromTexture(m_spoutSource.GetID());
    m_ascii.UpdateState(m_charSize,
        { m_bgColor[0], m_bgColor[1], m_bgColor[2], m_bgColor[3] },
        { m_charColor[0], m_charColor[1], m_charColor[2], m_charColor[3] }
    );
    SpoutOutTex outputTex = m_ascii.Draw(mat);
    m_sender->SendTexture(outputTex.id, GL_TEXTURE_2D, outputTex.w, outputTex.h);
}