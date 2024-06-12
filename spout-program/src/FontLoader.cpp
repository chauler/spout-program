#include "FontLoader.h"
#include <iostream>

FontLoader::FontLoader()
{
	unsigned int error = FT_Init_FreeType(&library);
	if (error) {
		std::cout << "Error initializing freetype" << std::endl;
	}
}

void FontLoader::LoadFace(const char* filepath)
{
	unsigned int error = FT_New_Face(library, filepath, 0, &m_face);
	if (error == FT_Err_Unknown_File_Format) {
		std::cout << "Unsupported font format" << std::endl;
	}
	else if (error) {
		std::cout << "Error loading font face" << std::endl;
	}
}

FT_Face FontLoader::GetFace()
{
	return m_face;
}
