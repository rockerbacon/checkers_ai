#include "sprite.h"
#include "Board.h"
#include "window.h"
#include <iostream>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 640

using namespace lab309;

int highlightedSquareIndex[POSSIBLE_DIRECTIONS] = { -1, -1, -1, -1 };

bool handleInput (const Window *window, Board &board) {
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
				int click = mapGridToChecker(mapPixelToGrid(*window, {event.button.x, event.button.y}));
				if (board.hasWhiteCheckerAt(click) && board.isWhiteTurn() || board.hasBlackCheckerAt(click) && board.isBlackTurn()) {
					board.toggleCheckerAt(click);
					//std::cout << mapPixelToGrid(*window, {event.button.x, event.button.y})[COORDINATE_X] << ", " << mapPixelToGrid(*window, {event.button.x, event.button.y})[COORDINATE_Y] << std::endl;	//debug
				} else {
					//determine click direction
					int d = 0;
					while (d < POSSIBLE_DIRECTIONS) {
						if (click == Board::moveDirections[d]+board.getToggled() || click == Board::moveDirections[d]*2+board.getToggled() && board.checkerCanCapture(Board::moveDirections[d])) {
							break;
						}
						d++;
					}
					if (d != POSSIBLE_DIRECTIONS) {
						board.moveChecker(Board::moveDirections[d]);
					}	
				}
				
			}
			
			//highlight possible movements
			for (int i = 0; i < POSSIBLE_DIRECTIONS; i++) {
				//std::cout << Board::moveDirections[i]+board.getToggled() << std::endl;	//debug
				if (board.checkerCanMove(Board::moveDirections[i])) {
					highlightedSquareIndex[i] = Board::moveDirections[i]+board.getToggled();
					//std::cout << "Can move at direction " << i << std::endl;	//debug
				} else if (board.checkerCanCapture(Board::moveDirections[i])) {
					highlightedSquareIndex[i] = Board::moveDirections[i]*2+board.getToggled();
					//std::cout << "Can capture at direction " << i << std::endl;	//debug
				} else {
					highlightedSquareIndex[i] = -1;
				}
				//std::cout << highlightedSquareIndex[i] << std::endl;	//debug
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
				*texture_blackChecker,
				*texture_highlightedSquare,
				*texture_promotedWhiteChecker,
				*texture_promotedBlackChecker;
	Sprite	*emptySquare[2],
			*whiteChecker,
			*blackChecker,
			*highlightedSquare,
			*promotedWhiteChecker,
			*promotedBlackChecker;
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
	texture_highlightedSquare = window->loadTexture("img/highlighted_square.png");
	texture_promotedWhiteChecker = window->loadTexture("img/promotedWhite_checker.png");
	texture_promotedBlackChecker = window->loadTexture("img/promotedBlack_checker.png");
	
	emptySquare[0] = new Sprite(texture_whiteSquare, texture_whiteSquare->w, texture_whiteSquare->h, window->getWidth()/BOARD_COLUMS, window->getHeight()/BOARD_LINES);
	emptySquare[1] = new Sprite(texture_blackSquare, texture_blackSquare->w, texture_blackSquare->h, window->getWidth()/BOARD_COLUMS, window->getHeight()/BOARD_LINES);
	whiteChecker = new Sprite(texture_whiteChecker, texture_whiteChecker->w, texture_whiteChecker->h, window->getWidth()/BOARD_COLUMS, window->getHeight()/BOARD_LINES);
	blackChecker = new Sprite(texture_blackChecker, texture_blackChecker->w, texture_blackChecker->h, window->getWidth()/BOARD_COLUMS, window->getHeight()/BOARD_LINES);
	highlightedSquare = new Sprite(texture_highlightedSquare, texture_highlightedSquare->w, texture_highlightedSquare->h, window->getWidth()/BOARD_COLUMS, window->getHeight()/BOARD_LINES);
	promotedWhiteChecker = new Sprite(texture_promotedWhiteChecker, texture_promotedWhiteChecker->w, texture_promotedWhiteChecker->h, window->getWidth()/BOARD_COLUMS, window->getHeight()/BOARD_LINES);
	promotedBlackChecker = new Sprite(texture_promotedBlackChecker, texture_promotedBlackChecker->w, texture_promotedBlackChecker->h, window->getWidth()/BOARD_COLUMS, window->getHeight()/BOARD_LINES);
	
	running = true;
	
	while (running) {
		int drawSquare = 0;
		
		//draw board
		for (int i = 0; i < BOARD_LINES; i++) {
			for (int j = 0; j < BOARD_COLUMS; j++) {
				emptySquare[drawSquare]->setPos((Vector<float>)mapGridToPixel(*window, {i, j}));
				//std::cout << emptySquare[drawSquare]->getXPos() << ", " << emptySquare[drawSquare]->getYPos() << ", " << drawSquare << std::endl;	//debug
				emptySquare[drawSquare]->blitTo(*window);
				drawSquare = (drawSquare+1)%2;
			}
			drawSquare = (drawSquare+1)%2;
		}
		
		//draw checkers
		for (int i = 0; i < BOARD_LINES*BOARD_COLUMS/2; i++) {
			//std::cout << mapCheckerToGrid(i)[COORDINATE_X] << ", " << mapCheckerToGrid(i)[COORDINATE_Y] << std::endl;	//debug
			if (board.hasWhiteCheckerAt(i)) {
				if (board.hasPromotedCheckerAt(i)) {
					promotedWhiteChecker->setPos((Vector<float>)mapGridToPixel(*window, mapCheckerToGrid(i)));
					promotedWhiteChecker->blitTo(*window);
				} else {
					whiteChecker->setPos((Vector<float>)mapGridToPixel(*window, mapCheckerToGrid(i)));
					whiteChecker->blitTo(*window);				
				}
				//std::cout << whiteChecker->getXPos() << ", " << whiteChecker->getYPos() << std::endl;	//debug
			} else if (board.hasBlackCheckerAt(i)) {
				if (board.hasPromotedCheckerAt(i)) {
					promotedBlackChecker->setPos((Vector<float>)mapGridToPixel(*window, mapCheckerToGrid(i)));
					promotedBlackChecker->blitTo(*window);
				} else {
					blackChecker->setPos((Vector<float>)mapGridToPixel(*window, mapCheckerToGrid(i)));
					blackChecker->blitTo(*window);				
				}
			}
		}
		
		//draw highlights
		for (int i = 0; i < POSSIBLE_DIRECTIONS; i++) {
			if (highlightedSquareIndex[i] != -1) {
				highlightedSquare->setPos((Vector<float>)mapGridToPixel(*window, mapCheckerToGrid(highlightedSquareIndex[i])));
				highlightedSquare->blitTo(*window);
			}
		}
		
		window->update();
		
		running = handleInput(window, board);
	}
	

EXIT:
	delete(window);
	
	//free textures
	SDL_FreeSurface(texture_whiteSquare);
	SDL_FreeSurface(texture_blackSquare);
	SDL_FreeSurface(texture_whiteChecker);
	SDL_FreeSurface(texture_blackChecker);
	SDL_FreeSurface(texture_highlightedSquare);
	
	//free sprites
	delete(emptySquare[0]);
	delete(emptySquare[1]);
	delete(whiteChecker);
	delete(blackChecker);
	delete(highlightedSquare);
		
	SDL_Quit();
	return 0;
}
