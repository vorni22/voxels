#ifndef TEXTURE_H
#define TEXTURE_H

#include <stb/stb_image.h>
#include <glad/glad.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Image {
public:
	Image(std::string _path);

	unsigned char* getData();
	void delData();

	unsigned char* LoadImageSrc(std::string path);

	int getWidth();
	int getHeight();
	int getNChannels();
	bool isLoaded();

private:
	unsigned char* data;

	int height, width, nChannels;

	bool loaded;
};

class Texture {
public:
	Texture(Image *_image, GLenum _type, GLenum _warp_s, GLenum _warp_t, GLenum _magFilter, GLenum _minFiler, GLenum _colorType);

	void setParameters(GLenum _warp_s, GLenum _warp_t, GLenum _magFilter, GLenum _minFiler, GLenum _colorType);

	void LoadImageToTexture(Image *newImage);

	void bindToUnit(int i);

	// Accesors
	unsigned int GetId();

	const char* getPath();

	bool isLoaded();

private:
	unsigned int id;

	Image* image;

	std::string path;

	GLenum type, warp_s, warp_t, magFilter, minFilter, colorType;

	bool loaded;
};

#endif