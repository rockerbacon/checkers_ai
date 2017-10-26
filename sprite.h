#ifndef LAB309SDL_SPRITE_H
#define LAB309SDL_SPRITE_H

#include <SDL2/SDL.h>
#include "Lab309_ADT_Matrix.h"
#include "window.h"

namespace lab309 {
	class Sprite {
		friend class Window;
		friend int collision (const Sprite &a, const Sprite &b);
		protected:
			/*ATTRIBUTES*/
			Vector<float> pos;
			SDL_Surface *texture;
			SDL_Rect rect;
			SDL_Rect displayRect;
			
		public:
			/*CONSTRUCTORS*/
			Sprite (SDL_Surface *texture, int rectWidth, int rectHeight, float size);
			
			/*GETTERS*/
			Vector<float> getSpritePos (void) const;
			Vector<float> getPos (void) const;
			int getXPos (void) const;
			int getYPos (void) const;
			Vector<float> getCenter (void) const;
			int getDisplayWidth (void) const;
			int getDisplayHeight (void) const;
			
			/*SETTERS*/
			void setSpritePos (const Vector<float> &pos);
			void setPos (const Vector<float> &pos);
			
			/*METHODS*/
			void translate (const Vector<float> &offset);
			void blitTo (const Window &window);
	};
	
	int collision (const Sprite &a, const Sprite &b);
};

#endif
