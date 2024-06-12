#pragma once
#include <vector>
#include "ft2build.h"
#include FT_FREETYPE_H

class FontLoader {
public:
	FT_Library library;
	FontLoader();
	void LoadFace(const char*);
	FT_Face GetFace();
private:
	FT_Face m_face;
};