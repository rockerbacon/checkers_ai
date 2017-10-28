/*
 * Checkers game with AI using minimax with alpha-beta prunning as AI
 * The program call should be: ./<executable> <white player> <black player>
 * The players can be:
 *		"player" for a human player to play using the left mouse button to move the pieces
 *		"cpu" <difficulty> for a cpu to play using the minimax algorithm. The difficulty can be any value greater than 0
 *
 */

#include "sprite.h"
#include "Board.h"
#include "window.h"
#include <iostream>
#include "Minimax.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 640

#define CPU_DELAY 200

using namespace lab309;

Window *window;
int highlightedSquareIndex[POSSIBLE_DIRECTIONS] = { -1, -1, -1, -1 };
Board *board;
Vector<int> lastClick = {-1, -1};

class Player {
	public:
		virtual void play (void) const = 0;
};

class Human : public Player {
	public:
		void play (void) const {
			
			if (lastClick != Vector<int>({-1, -1})) {
				int click = mapGridToChecker(mapPixelToGrid(*window, lastClick));
				if (board->hasWhiteCheckerAt(click) && board->isWhiteTurn() || board->hasBlackCheckerAt(click) && board->isBlackTurn()) {
					board->toggleCheckerAt(click);
				} else {
					//determine click direction
					int d = 0;
					while (	d < POSSIBLE_DIRECTIONS && click != Board::moveDirections[d]+board->getToggled() &&
							!(click == Board::moveDirections[d]*2+board->getToggled() && board->checkerCanCapture(Board::moveDirections[d])) ) {
						d++;
					}
					if (d != POSSIBLE_DIRECTIONS) {
						board->moveChecker(Board::moveDirections[d]);
						//debug
						if (board->isWhiteTurn()) {
							std::cout << "White's turn" << std::endl;
						} else {
							std::cout << "Black's turn" << std::endl;
						}
						//std::cout << board->evaluate() << std::endl;	//debug	
					}	
				}
			
				//highlight possible movements
				for (int i = 0; i < POSSIBLE_DIRECTIONS; i++) {
					//std::cout << Board::moveDirections[i]+board->getToggled() << std::endl;	//debug
					if (board->checkerCanMove(Board::moveDirections[i])) {
						highlightedSquareIndex[i] = Board::moveDirections[i]+board->getToggled();
						//std::cout << "Can move at direction " << i << std::endl;	//debug
					} else if (board->checkerCanCapture(Board::moveDirections[i])) {
						highlightedSquareIndex[i] = Board::moveDirections[i]*2+board->getToggled();
						//std::cout << "Can capture at direction " << i << std::endl;	//debug
					} else {
						highlightedSquareIndex[i] = -1;
					}
					//std::cout << highlightedSquareIndex[i] << std::endl;	//debug
				}
				
				lastClick = {-1, -1};
			}
		}
};

class CPU : public Player {
	private:
		unsigned int maxDepth;
		
	public:
		CPU (unsigned int difficulty) {
			this->maxDepth = difficulty;
		}
		
		void play (void) const {
			std::cout << "The computer is thinking..." << std::endl;	//debug
			std::list<const State*> stateList;
			
			if (board->isWhiteTurn()) {
				stateList = minimax(*board, this->maxDepth);
			} else {
				stateList = maximin(*board, this->maxDepth);
			}
			//delete(board);
			//std::cout << stateList.size() << std::endl;	//debug
			board = (Board*)stateList.front();
			stateList.pop_front();
			
			//debug
			if (board->isWhiteTurn()) {
				std::cout << "White's turn" << std::endl;
			} else {
				std::cout << "Black's turn" << std::endl;
			}
			//std::cout << board->evaluate() << std::endl;	//debug
			
			//clear memory of states
			//std::cout << "cleaning memory" << std::endl;	//debug
			for (const State *s : stateList) {
				//std::cout << ((Board*)s)->toString() << std::endl;	//debug
				delete(s);
			}
			//std::cout << "memory cleaned" << std::endl;	//debug
			SDL_Delay(CPU_DELAY);
		}
};

bool handleInput (void) {
	SDL_Event event;
	
	while (SDL_PollEvent(&event) != 0) {
		if (event.type == SDL_QUIT) {
			return false;
		} else 	if (event.type == SDL_MOUSEBUTTONUP) {
			if (event.button.button == SDL_BUTTON_LEFT) {
				lastClick = { event.button.x, event.button.y };
			}
		}	
		
	}
	
	return true;
	
}

int main (int argc, char **args) {

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
	bool running;
	Player *whitePlayer, *blackPlayer;
	std::string reader;
	int argi = 1;
	
	//read players
	if (argi >= argc) {
		std::cout << "Please identify the players" << std::endl;
		return 2;
	}
	
	reader = args[argi];
	if (reader.compare("cpu") == 0) {
		unsigned int difficulty;
		argi++;
		if (argi >= argc) {
			std::cout << "Please identify the difficulty for the CPU" << std::endl;
			return 2;
		}
		difficulty = std::stoul(args[argi]);
		whitePlayer = new CPU(difficulty);
	} else if (reader.compare("player") == 0) {
		whitePlayer = new Human();
	} else {
		std::cout << "Invalid 1st player, make sure all letters are lowercase" << std::endl;
		return 1;
	}
	argi++;
	
	reader = args[argi];
	std::cout << reader << std::endl;	//debug
	if (reader.compare("cpu") == 0) {
		unsigned int difficulty;
		argi++;
		if (argi >= argc) {
			std::cout << "Please identify the difficulty for the CPU" << std::endl;
			return 2;
		}
		difficulty = std::stoul(args[argi]);
		blackPlayer = new CPU(difficulty);
	} else if (reader.compare("player") == 0) {
		blackPlayer = new Human();
	} else {
		std::cout << "Invalid 2nd player, make sure all letters are lowercase" << std::endl;
		return 1;
	}

	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_PNG);
	
	//initialize window
	board = new Board();
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
	
	std::cout << "White begins" << std::endl;	//debug
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
		//std::cout << "board drawed" << std::endl;	//debug
		
		//draw checkers
		for (int i = 0; i < BOARD_LINES*BOARD_COLUMS/2; i++) {
			//std::cout << mapCheckerToGrid(i)[COORDINATE_X] << ", " << mapCheckerToGrid(i)[COORDINATE_Y] << std::endl;	//debug
			if (board->hasWhiteCheckerAt(i)) {
				if (board->hasPromotedCheckerAt(i)) {
					promotedWhiteChecker->setPos((Vector<float>)mapGridToPixel(*window, mapCheckerToGrid(i)));
					promotedWhiteChecker->blitTo(*window);
				} else {
					whiteChecker->setPos((Vector<float>)mapGridToPixel(*window, mapCheckerToGrid(i)));
					whiteChecker->blitTo(*window);				
				}
				//std::cout << whiteChecker->getXPos() << ", " << whiteChecker->getYPos() << std::endl;	//debug
			} else if (board->hasBlackCheckerAt(i)) {
				if (board->hasPromotedCheckerAt(i)) {
					promotedBlackChecker->setPos((Vector<float>)mapGridToPixel(*window, mapCheckerToGrid(i)));
					promotedBlackChecker->blitTo(*window);
				} else {
					blackChecker->setPos((Vector<float>)mapGridToPixel(*window, mapCheckerToGrid(i)));
					blackChecker->blitTo(*window);				
				}
			}
		}
		//std::cout << "checkers drawed" << std::endl;	//debug
		
		//draw highlights
		for (int i = 0; i < POSSIBLE_DIRECTIONS; i++) {
			if (highlightedSquareIndex[i] != -1) {
				highlightedSquare->setPos((Vector<float>)mapGridToPixel(*window, mapCheckerToGrid(highlightedSquareIndex[i])));
				highlightedSquare->blitTo(*window);
			}
		}
		//std::cout << "highlights drawed" << std::endl;	//debug
		
		window->update();
		running = handleInput();
		
		//std::cout << "window updated" << std::endl;	//debug
		
		//players
		if (board->isWhiteTurn()) {
			//std::cout << "white's turn" << std::endl;	//debug
			whitePlayer->play();
		} else {
			//std::cout << "black's turn" << std::endl;	//debug
			blackPlayer->play();
		}
		
		//check endgame
		if (board->isFinal()) {
			if (board->isWhiteTurn()) {
				std::cout << "BLACK WINS!" << std::endl;	//debug
			} else {
				std::cout << "WHITE WINS!" << std::endl;	//debug
			}
			goto EXIT;
		}
		
	}
	

EXIT:
	delete(window);
	delete(board);
	
	//free textures
	SDL_FreeSurface(texture_whiteSquare);
	SDL_FreeSurface(texture_blackSquare);
	SDL_FreeSurface(texture_whiteChecker);
	SDL_FreeSurface(texture_blackChecker);
	SDL_FreeSurface(texture_highlightedSquare);
	SDL_FreeSurface(texture_promotedWhiteChecker);
	SDL_FreeSurface(texture_promotedBlackChecker);
	
	delete(whitePlayer);
	delete(blackPlayer);
	
	//free sprites
	delete(emptySquare[0]);
	delete(emptySquare[1]);
	delete(whiteChecker);
	delete(blackChecker);
	delete(highlightedSquare);
	delete(promotedWhiteChecker);
	delete(promotedBlackChecker);
		
	SDL_Quit();
	return 0;
}
