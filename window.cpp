#include "window.h"
#include <SDL2/SDL_image.h>
#include <iostream>

lab309::Window::Window (const char *title, unsigned int width, unsigned int hight, unsigned int frameLimit) {
	IMG_Init(IMG_INIT_JPG|IMG_INIT_PNG);
	this->window = SDL_CreateWindow (title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, hight, SDL_WINDOW_SHOWN);
	this->surface = SDL_GetWindowSurface (this->window);
	this->lastUpdate = SDL_GetTicks();
	this->frameLimit = frameLimit;
}

lab309::Window::~Window (void) {
	IMG_Quit();
	SDL_DestroyWindow(this->window);
}

int lab309::Window::getWidth (void) const {
	return this->surface->w;
}

int lab309::Window::getHeight (void) const {
	return this->surface->h;
}

double lab309::Window::getTimeDelta (void) const {
	return (SDL_GetTicks() - this->lastUpdate)/1000.0;
}

SDL_Surface* lab309::Window::loadTexture (const char *imgPath) {
	SDL_Surface *loaded;
	SDL_Surface *optimized;
	
	loaded = IMG_Load(imgPath);
	
	optimized = SDL_ConvertSurface (loaded, this->surface->format, 0);
	SDL_FreeSurface(loaded);
	
	return optimized;
}

void lab309::Window::update (void) {
	double previousUpdate = this->lastUpdate;
	this->lastUpdate = SDL_GetTicks();
	if (this->lastUpdate - previousUpdate < this->frameLimit) {
		SDL_Delay(this->frameLimit-this->lastUpdate+previousUpdate);
	}
	//std::cout << (SDL_GetTicks() - previousUpdate)/1000.0 << std::endl;	//debug
	SDL_UpdateWindowSurface(this->window);
	SDL_FillRect(this->surface, NULL, 0);
}
