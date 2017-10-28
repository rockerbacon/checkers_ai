/*
 * This header contains all the modeling of the checkers game
 *
 *		Vitor Andrade dos Santos, 2017
 */

#ifndef LAB309_BOARD_H
#define LAB309_BOARD_H

#include "Lab309_ADT_Matrix.h"
#include "window.h"
#include "Minimax.h"
#include <list>
#include <iostream>
#include <sstream>

#define EMPTY_SQUARE 0
#define WHITE_CHECKER 0x1
#define BLACK_CHECKER 0x2
#define PROMOTED_CHECKER 0x4

#define BOARD_LINES 8
#define BOARD_COLUMS 8
#define OCCUPIED_INITIAL_LINES 3

//Possible movement directions, used for creating a direction map
#define FORWARDS 0x1
#define BACKWARDS 0
#define RIGHT 0x2
#define LEFT 0

#define POSSIBLE_DIRECTIONS 4

//Scores used for the heuristic
#define SCORE_WHITE_CHECKER 3
#define SCORE_BLACK_CHECKER -3
#define SCORE_PROMOTED_MULTIPLIER 2
#define SCORE_WHITE_COVER 1
#define SCORE_BLACK_COVER -1

#define WHITE_CANNOT_MOVE 0x1
#define BLACK_CANNOT_MOVE 0x2

namespace lab309 {
	
	Vector<int> mapPixelToGrid (const Window &window, const Vector<int> &pos);	//maps a pixel to a board coordinate in a given window. Board contains 8 colums and 8 lines
	int mapGridToChecker (const Vector<int> &pos);	//maps a board position to a checker position in the attribute checkers[] within the Board class
	
	//inverse of the functions above
	Vector<int> mapGridToPixel (const Window &window, const Vector<int> &pos);
	Vector<int> mapCheckerToGrid (int i);
	
	class Direction {
		private:
			Vector<int> offset;
			
		public:
			Direction (int directionMap);
			
			bool isForwards (void) const;
			inline bool isBackwards (void) const { return !this->isForwards(); }
			bool isRight (void) const;
			inline bool isLeft (void) const { return !this->isRight(); }
			bool inboundsFor (int checker) const;	//checks if a movement in this direction is within bounds of a checker
			int operator+ (int checker) const;
			Direction operator* (int scalar) const;
	};
	
	class Board : public State {
		
		private:
			int checkers[BOARD_COLUMS/2*BOARD_LINES];
			mutable int toggledChecker;
			unsigned int turn;
			
		public:
			static const Direction moveDirections[POSSIBLE_DIRECTIONS];
		
			Board (void);
			Board (const Board &board);
			
			//getters
			bool hasEmptySquareAt (int i) const;
			bool hasWhiteCheckerAt (int i) const;
			bool hasBlackCheckerAt (int i) const;
			bool hasPromotedCheckerAt (int i) const;
			bool isWhiteTurn (void) const;
			bool isBlackTurn (void) const;
			int getToggled (void) const;
			bool whiteHasCapture (void) const;
			bool blackHasCapture (void) const;
			
			bool checkerCanCapture (const Direction &direction) const;
			bool checkerCanMove (const Direction &direction) const;	//does not check for capture rules
			
			//methods
			void toggleCheckerAt (int i) const;
			bool moveChecker (const Direction &direction);
			
			float evaluate (void) const;
			std::list<State*> nextStates (void) const;
			bool isFinal (void) const;
			
			bool operator== (const Board &b) const;
			inline bool operator!= (const Board &b) const { return !(*this == b); }
			
			std::string toString (void) const;
			
	};
};

#endif
