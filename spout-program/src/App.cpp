#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "App.h"
#include <SpoutLibrary.h>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

cv::Mat GetImageFromTexture(const GLuint texID, const unsigned int width, const unsigned int height) {
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
    cv::resize(image, image, cv::Size(width, height));
    cv::cvtColor(image, image, cv::COLOR_RGBA2GRAY);
    return image;
}

App::App(GLFWwindow* window): m_window(window), m_ImGuiIO(ImGui::GetIO()), m_ascii(m_window) {
    m_receiver = GetSpout();
    m_receiver->SetReceiverName("VTubeStudioSpout");
    m_sender = GetSpout();
    m_sender->SetSenderName("SpoutProgram");
    glGenTextures(1, &m_spout_img_in);
    glBindTexture(GL_TEXTURE_2D, m_spout_img_in);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1920, 1017, 0, GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*)malloc(1920 * 1017 * 4 * sizeof(unsigned char)));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void App::DrawGUI() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    int window_w, window_h;
    glfwGetWindowSize(m_window, &window_w, &window_h);
    ImGui::SetNextWindowPos(ImVec2());
    ImGui::SetNextWindowSize(ImVec2(window_w, window_h));
    ImGui::Begin("Spout Feed", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar);
    if (m_receiver->ReceiveTexture()) {
        // Bind to get access to the shared texture
        if (m_receiver->BindSharedTexture()) {
            // Get the shared texture ID and do something with it
            GLuint sharedTexID = m_receiver->GetSharedTextureID();
            // copy from the shared texture
            glBindTexture(GL_TEXTURE_2D, m_spout_img_in);
            //Re-allocate texture buffer if incoming image is different dimensions
            if (m_image_w != m_receiver->GetSenderWidth() || m_image_h != m_receiver->GetSenderHeight()) {
                if (m_spout_img_in_data != nullptr) {
                    delete m_spout_img_in_data;
                }
                m_image_w = m_receiver->GetSenderWidth();
                m_image_h = m_receiver->GetSenderHeight();
                m_spout_img_in_data = new unsigned char[m_image_w * m_image_h * 4];
            }
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_image_w, m_image_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_spout_img_in_data);
            m_receiver->CopyTexture(sharedTexID, GL_TEXTURE_2D,
                m_spout_img_in,
                GL_TEXTURE_2D,
                m_image_w, m_image_h);
            glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_spout_img_in_data);
            glBindTexture(GL_TEXTURE_2D, 0);

            // Un-bind to release access to the shared texture
            m_receiver->UnBindSharedTexture();
            ImGui::Image((void*)(intptr_t)m_spout_img_in, ImVec2(m_image_w, m_image_h));
            ImGui::Text("size = %d x %d", m_image_w, m_image_h);
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / m_ImGuiIO.Framerate, m_ImGuiIO.Framerate);
        }
    }
    ImGui::End();
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(m_window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    ImVec4 bg_color = ImVec4(1.00f, 0.55f, 0.60f, 0.00f);
    glClearColor(bg_color.x * bg_color.w, bg_color.y * bg_color.w, bg_color.z * bg_color.w, bg_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    cv::Mat mat = GetImageFromTexture(m_spout_img_in, 500, 500);
    //cv::imshow("Display window", mat);
    m_ascii.UpdateImage(mat);
    unsigned int outputTex = m_ascii.Draw();
    m_sender->SendTexture(outputTex, GL_TEXTURE_2D, display_w, display_h);
    glfwSwapBuffers(m_window);
}