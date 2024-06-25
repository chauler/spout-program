#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "SpoutLibrary.h"
#include "effects/ascii_render.h"

class App {
public:
	App(GLFWwindow*);
	void DrawGUI();
private:
	void RunLogic();
	GLFWwindow* m_window = nullptr;
	SPOUTLIBRARY* m_receiver = nullptr;
	SPOUTLIBRARY* m_sender = nullptr;
	unsigned int m_spoutSource = 0;
	unsigned char* m_spoutSourceData = nullptr;
	unsigned int m_imageW = 0, m_imageH = 0;
	ImGuiIO& m_ImGuiIO;
    ascii_render m_ascii;

	//App State editable in the GUI
	float m_charSize = 10.0f;
	int m_charRes = 100, m_cols = 100, m_rows = 100;
	float m_bgColor[4]{1.0f, 1.0f, 1.0f, 0.0f};
	float m_charColor[4]{1.0f, 1.0f, 1.0f, 1.0f};
};

//cv::Mat GetImageFromTexture(const GLuint, const unsigned int, const unsigned int);
