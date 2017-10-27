#include "sprite.h"
#include "board.h"
#include "window.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 640

bool handleInput (void) {
	SDL_Event event;
	
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			return false;
		} else if (event.type == SDL_KEYUP) {
			switch (event.key.keysym.sym) {
				case SDLK_RIGHT:
				break;
			}
		} else if (event.type == SDL_MOUSEBUTTONUP) {
			if (event.button.button == SDL_BUTTON_LEFT) {
			
			}
		}
	}
	
	return true;
	
}

int main (int argc, char **args) {

	Window *window;
	SDL_Surface	*texture_whiteSquare,
				*texture_blackSquare,
				*texture_whiteChecker,
				*texture_blackChecker;
	Sprite	*emptySquare[2],
			*whiteChecker,
			*blackChecker;
	Board board;
	bool running;

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	
	//initialize window
	window = new Window("Checkers", WINDOW_WIDTH, WINDOW_HEIGHT, LIMIT_30FPS);
	
	//load textures
	texture_whiteSquare = window->loadTexture("img/white_square.png");
	texture_blackSquare = window->loadTexture("img/black_square.png");
	texture_whiteChecker = window->loadTexture("img/white_checker.png");
	texture_blackChecker = window->loadTexture("img/black_checker.png");
	
	emptySquare[0] = new Sprite(texture_whiteSquare, texture_whiteSquare->w, texture_whiteSquare->h, window->getWidth()/BOARD_COLUMS, window->getHeight()/BOARD_LINES);
	emptySquare[1] = new Sprite(texture_blackSquare, texture_blackSquare->w, texture_blackSquare->h, window->getWidth()/BOARD_COLUMS, window->getHeight()/BOARD_LINES);
	whiteChecker = new Sprite(texture_whiteChecker, texture_whiteChecker->w, texture_whiteChecker->h, window->getWidth()/BOARD_COLUMS, window->getHeight()/BOARD_LINES);
	blackChecker = new Sprite(texture_blackChecker, texture_blackChecker->w, texture_blackChecker->h, window->getWidth()/BOARD_COLUMS, window->getHeight()/BOARD_LINES);
	
	running = true;
	
	while (running) {
		int drawSquare = 0;
		
		//draw board
		for (int i = 0; i < BOARD_LINES; i++) {
			for (int j = 0; j < BOARD_COLUMS; j++) {
				emptySquare[drawSquare]->setPos(mapGridToPixel({i, j});
				emptySquare[drawSquare]->blitTo(*window);
				drawSquare = (drawSquare+1)%2;
			}
		}
		
		//draw checkers
		for (int i = 0; i < BOARD_LINES*BOARD_COLUMS/2; i++) {
			if (board.hasWhiteCheckerAt(i)) {
				whiteChecker->setPos(mapGridToPixel(mapCheckerToGrid(i)));
				whiteChecker->blitTo(*window);
			} else if (board.hasBlackCheckerAt(i)) {
				blackChecker->setPos(mapGridToPixel(mapCheckerToGrid(i)));
				blackChecker->blitTo(*window);
			}
		}
		
		window->update();
		
		running = handleInput();
	}
	

EXIT:
	delete(window);
	
	//free textures
	SDL_FreeSurface(texture_whiteSquare);
	SDL_FreeSurface(texture_blackSquare);
	SDL_FreeSurface(texture_whiteChecker);
	SDL_FreeSurface(texture_blackChecker);
	
	//free sprites
	delete(emptySquare[0]);
	delete(emptySquare[1]);
	delete(whiteChecker);
	delete(blackChecker);
		
	SDL_Quit();
	return 0;
}
