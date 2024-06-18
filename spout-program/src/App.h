#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "SpoutLibrary.h"
#include "ascii_render.h"

class App {
public:
	App(GLFWwindow*);
	void DrawGUI();
private:
	void RunLogic();
	GLFWwindow* m_window = nullptr;
	SPOUTLIBRARY* m_receiver = nullptr;
	SPOUTLIBRARY* m_sender = nullptr;
	unsigned int m_spout_img_in = 0;
	unsigned char* m_spout_img_in_data = nullptr;
	unsigned int m_image_w, m_image_h = 0;
	ImGuiIO& m_ImGuiIO;
    ascii_render m_ascii;

	//App State editable in the GUI
	float m_charSize = 10.0f;
	int m_charRes = 100;
};

//cv::Mat GetImageFromTexture(const GLuint, const unsigned int, const unsigned int);
