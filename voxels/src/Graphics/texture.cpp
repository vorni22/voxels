#include "texture.h"


// IMAGE CLASS ==================================

Image::Image(std::string path) {
	data = LoadImageSrc(path);
	if (data) {
		loaded = true;
	}
	else {
		std::cout << "Error [Image::LoadImageSrc()]: Could not load image! Image data will be null '" << path << "' " << std::endl;
		loaded = false;
	}
}

unsigned char* Image::getData() {
	return data;
}

void Image::delData() {
	if (!loaded) return;
	stbi_image_free(data);
	loaded = false;
}

unsigned char* Image::LoadImageSrc(std::string path) {
	stbi_set_flip_vertically_on_load(true);
	return stbi_load(path.c_str(), &width, &height, &nChannels, 0);
}

int Image::getWidth() {
	return width;
}

int Image::getHeight() {
	return height;
}

int Image::getNChannels() {
	return nChannels;
}

bool Image::isLoaded(){
	return loaded;
}


// TEXTURE CLASS ======================================

Texture::Texture(Image *_image, GLenum _type, GLenum _warp_s, GLenum _warp_t, GLenum _magFilter, GLenum _minFiler, GLenum _colorType){
	loaded = false;
	
	// generate a new texture
	glGenTextures(1, &id);

	image = _image;
	type = _type;
	setParameters(_warp_s, _warp_t, _magFilter, _minFiler, _colorType);

	// load an image to the texture
	LoadImageToTexture(image);
}

void Texture::setParameters(GLenum _warp_s, GLenum _warp_t, GLenum _magFilter, GLenum _minFiler, GLenum _colorType){
	warp_s = _warp_s;
	warp_t = _warp_t;
	magFilter = _magFilter;
	minFilter = _minFiler;
	colorType = _colorType;
}

void Texture::LoadImageToTexture(Image *newImage){
	image = newImage;

	// place the texture in the work unit (0 - work unit)
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(type, id);

	glTexParameteri(type, GL_TEXTURE_WRAP_S, warp_s);
	glTexParameteri(type, GL_TEXTURE_WRAP_T, warp_t);
	glTexParameteri(type, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(type, GL_TEXTURE_MIN_FILTER, minFilter);

	// checks if the data was loaded
	if (image->isLoaded()) {
		glTexImage2D(type, 0, colorType, image->getWidth(), image->getWidth(), 0, GL_RGB, GL_UNSIGNED_BYTE, image->getData());
		glGenerateMipmap(type);
		loaded = true;
	}
	else {
		std::cout << "Error [Texture.LoadImageToTexture()]: The image data was null!" << std::endl;
		loaded = false;
	}
}

void Texture::bindToUnit(int i){
	if (!isLoaded()) return;

	if (i > 15) i = 15;
	if (i < 0) i = 0;
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(type, id);
}

unsigned int Texture::GetId(){
	return id;
}

const char* Texture::getPath(){
	return path.c_str();
}

bool Texture::isLoaded(){
	return loaded;
}
