#pragma once
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "SpoutLibrary/SpoutLibrary.h"
#include "effects/ascii_render.h"
#include "Texture2D.h"
#include "sources/ISource.h"
#include <memory>

enum class SourceType: unsigned int { SpoutSource, CamSource };

class App {
public:
	App(GLFWwindow*);
	void DrawGUI();
	void RunLogic();
	static void SetIconified(int iconified) { m_iconified = iconified; }
private:
	GLFWwindow* m_window = nullptr;
	SPOUTLIBRARY* m_receiver = nullptr;
	SPOUTLIBRARY* m_sender = nullptr;
	Texture2D m_spoutSource{}, test{};
	std::shared_ptr<unsigned char[]> m_spoutSourceData{};
	unsigned int m_imageW = 0, m_imageH = 0;
	ImGuiIO& m_ImGuiIO;
    ascii_render m_ascii;
	std::unique_ptr<ISource> m_source;
	SourceType sourceType = SourceType::SpoutSource;
	inline static int m_iconified = 0;


	//App State editable in the GUI
	float m_charSize = 10.0f;
	int m_cols = 100, m_rows = 100;
	float m_bgColor[4]{1.0f, 1.0f, 1.0f, 0.0f};
	float m_charColor[4]{1.0f, 1.0f, 1.0f, 1.0f};
};

//cv::Mat GetImageFromTexture(const GLuint, const unsigned int, const unsigned int);
