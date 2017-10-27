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
#define LEFT 0x2
#define RIGHT 0
#define OVER 0x4	//Will be used internally by the functions, do not use

#define POSSIBLE_DIRECTIONS 4

//Scores used for the heuristic
#define SCORE_WHITE_CHECKER 3
#define SCORE_BLACK_CHECKER -3
#define SCORE_PROMOTED_MULTIPLIER 2
#define SCORE_WHITE_COVER 1
#define SCORE_BLACK_COVER -1

namespace lab309 {
	
	Vector<int> mapPixelToGrid (const Window &window, const Vector<int> &pos);	//maps a pixel to a board coordinate in a given window. Board contains 8 colums and 8 lines
	int mapGridToChecker (const Vector<int> &pos);	//maps a board position to a checker position in the attribute checkers[] within the Board class
	
	class Direction {
		private:
			int directionMap;
			int offset;
			
		public:
			Direction (int directionMap);
			
			bool isForwards (void) const;
			inline bool isBackwards (void) const { return !this->isForwards(); }
			bool isLeft (void) const;
			inline bool isRight (void) const { return !this->isLeft(); }
			bool inboundsFor (int checker) const;	//checks if a movement in this direction is within bounds of a checker
			inline int getMap (void) const { return this->directionMap; }
			
			inline operator int (void) const { return this->offset; }
	}
	
	class Board : public State {
		
		private:
			static const Direction moveDirections[POSSIBLE_DIRECTIONS] = { FORWARDS|LEFT, FORWARDS|RIGHT, BACKWARDS|LEFT, BACKWARDS|RIGHT };
				
			int checkers[BOARD_COLUMS*BOARD_LINES/2];
			int toggledChecker;
			unsigned int turn;
			
			int& getCheckerAt (const Vector &pixel);
			
			//getters
			bool hasEmptySquareAt (int i) const;
			bool hasWhiteCheckerAt (int i) const;
			bool hasBlackCheckerAt (int i) const;
			bool hasPromotedCheckerAt (int i) const;
			
		public:
			Board (void);
			Board (const Board &board);
			
			//getters
			bool checkerCanCapture (const Direction &direction) const;
			bool checkerCanMove (const Direction &direction) const;	//does not check for capture rules
			
			//methods
			void toggleCheckerAt (const Vector<int> &pixel);
			bool moveChecker (const Direction &direction);
			
			float evaluate (void) const;
			std::list<State*> nextStates (void) const;
			bool isFinal (void) const;
			
	};
};

#endif
