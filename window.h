#ifndef LAB309_WINDOW_H
#define LAB309_WINDOW_H

#define LIMIT_60FPS 16
#define LIMIT_30FPS 33
#define LIMIT_NONE 0

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace lab309 {
	class Sprite;
	
	class Window {
		friend class Sprite;
		private:
			/*ATTRIBUTES*/
			SDL_Window *window;
			SDL_Surface *surface;
			unsigned int lastUpdate;
			unsigned int frameLimit;
			
		public:
			/*CONSTRUCTORS*/
			Window (const char *title, unsigned int width, unsigned int hight, unsigned int frameLimit);
			~Window (void);
			
			/*GETTERS*/
			int getWidth (void) const;
			int getHeight (void) const;
			double getTimeDelta (void) const;
			
			/*METHODS*/
			SDL_Surface* loadTexture (const char *imgPath);
			void update (void);
			
	};
};

#endif
