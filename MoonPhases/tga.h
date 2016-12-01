/*Published under The MIT License (MIT)

See LICENSE.TXT*/

// Ryan Pridgeon COM2032 rp00091

#ifndef RYAN_TGA_H
#define RYAN_TGA_H

#ifdef _WIN32
#include <Windows.h>
#endif
#ifdef __APPLE__
#include <OpenGL/gl.h>
#include <GLUT/glut.h>
#else
#include <GL/gl.h>
#include <GL/glut.h>
#endif

// This is a class that loads TGA files into opengl textures
class TGA
{
private:
	// the handle for the texture in opengl
	GLuint textureHandle;
public:
	// Constructs and loads a TGA into opengl from the given image file path
	TGA(char* imagePath);

	// Returns the handle to the texture created from the image, for binding to opengl
	GLuint getTextureHandle(void);
};

#endif
