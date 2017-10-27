#include "Board.h"

lab309::Vector<int> lab309::mapPixelToGrid (const Window &window, const Vector<int> &pixel) {
	Vector<int> v(2);
	int gridWidth = window.getWidth()/BOARD_COLUMS;
	int gridHeight = window.getHeight()/BOARD_COLUMS;
	
	v[COORDINATE_X] = (pixel[COORDINATE_X] - pixel[COORDINATE_X] % gridWidth)/gridWidth;
	v[COORDINATE_Y] = (window.getHeight() - (pixel[COORDINATE_Y] - pixel[COORDINATE_Y] % gridHeight) - gridHeight)/gridHeight;
	
	return v;
}

//Calculation for a complete matrix would be i*colums + j;
//In this case there's half as many colums per line, so i*(colums/2)
//The colums are offseted in odd lines: R:j -> j*2; so j/2
//The colums are offseted in even lines: R:j -> j*2+1; so (j-1)/2
//~j to flip the least significant bit which determines if a number is odd or even then &0x1 which will result in 0 when j is odd and 1 otherwise
int lab309::mapGridToChecker (const Vector<int> &pos) {
	return pos[COORDINATE_Y]*(BOARD_COLUMS/2) + (pos[COORDINATE_X] - (~pos[COORDINATE_Y]&0x1)) / 2;
}

lab309::Vector<int> lab309::mapGridToPixel (const Window &window, const Vector<int> &pos) {
	Vector<int> v(2);
	int gridWidth = window.getWidth()/BOARD_COLUMS;
	int gridHeight = window.getHeight()/BOARD_COLUMS;
	
	v[COORDINATE_X] = pos[COORDINATE_X]*gridWidth;
	v[COORDINATE_Y] = (BOARD_LINES - pos[COORDINATE_Y] - 1) * gridHeight;
	
	return v;
}

lab309::Vector<int> lab309::mapCheckerToGrid (int i) {
	Vector<int> v(2);
	
	v[COORDINATE_Y] = i/(BOARD_COLUMS/2);
	v[COORDINATE_X] = (i%(BOARD_COLUMS/2)*2 + (~v[COORDINATE_Y]&0x1));
	
	return v;
}

/*DIRECTION*/
lab309::Direction::Direction (int directionMap) {
	this->offset = Vector<int>(2);
	
	this->offset[COORDINATE_X] = (directionMap&RIGHT) ? 1 : -1;
	this->offset[COORDINATE_Y] = (directionMap&FORWARDS) ? 1 : -1;
	
}

bool lab309::Direction::isForwards (void) const {
	return this->offset[COORDINATE_Y] > 0;
}

bool lab309::Direction::isRight (void) const {
	return this->offset[COORDINATE_X] > 0;
}

bool lab309::Direction::inboundsFor (int checker) const {
	Vector<int> v = this->offset + mapCheckerToGrid(checker);
	return v[COORDINATE_X] >= 0 && v[COORDINATE_X] < BOARD_COLUMS && v[COORDINATE_Y] >= 0 && v[COORDINATE_Y] < BOARD_LINES;
}

//returns the position for a checker that's moved in this direction
int lab309::Direction::operator+ (int checker) const {
	Vector<int> v = mapCheckerToGrid(checker) + this->offset;
	return mapGridToChecker(v);
}

lab309::Direction lab309::Direction::operator* (int scalar) const {
	Direction d = *this;
	d.offset = scalar*d.offset;
	return d;
}

/*BOARD*/

const lab309::Direction lab309::Board::moveDirections[POSSIBLE_DIRECTIONS] = { FORWARDS|LEFT, FORWARDS|RIGHT, BACKWARDS|LEFT, BACKWARDS|RIGHT };

/*CONSTRUCTORS*/
//initially the first lines have white checkers and the last lines have black checkers
lab309::Board::Board (void) {
	int i;
	for (i = 0; i < OCCUPIED_INITIAL_LINES*BOARD_COLUMS/2; i++) {
		this->checkers[i] = WHITE_CHECKER;
	}
	
	while (i < BOARD_COLUMS*(BOARD_LINES-OCCUPIED_INITIAL_LINES)/2) {
		this->checkers[i] = EMPTY_SQUARE;
		i++;
	}
	
	while (i < BOARD_COLUMS*BOARD_LINES/2) {
		this->checkers[i] = BLACK_CHECKER;
		i++;
	}
	
	this->toggledChecker = -1;
	
	this->turn = 0;
}

lab309::Board::Board (const Board &board) {
	for (int i = 0; i < BOARD_LINES*BOARD_COLUMS/2; i++) {
		this->checkers[i] = board.checkers[i];
	}
	this->toggledChecker = board.toggledChecker;
	this->turn = board.turn;
}

/*GETTERS*/
bool lab309::Board::hasEmptySquareAt (int i) const {
	return this->checkers[i] == EMPTY_SQUARE;
}

bool lab309::Board::hasWhiteCheckerAt (int i) const {
	return this->checkers[i] & WHITE_CHECKER;
}

bool lab309::Board::hasBlackCheckerAt (int i) const {
	return this->checkers[i] & BLACK_CHECKER;
}

bool lab309::Board::hasPromotedCheckerAt (int i) const {
	return this->checkers[i] & PROMOTED_CHECKER;
}

bool lab309::Board::isWhiteTurn (void) const {
	return ~this->turn&0x1;
}

bool lab309::Board::isBlackTurn (void) const {
	return this->turn&0x1;
}

int lab309::Board::getToggled (void) const {
	return this->toggledChecker;
}

bool lab309::Board::checkerCanCapture (const Direction &direction) const {
	//checker can capture if the movement is valid and it's a white with a black in its direction or a black with a white in its direction
	return this->checkerCanMove(direction*2) && (	this->hasWhiteCheckerAt(this->toggledChecker) && this->hasBlackCheckerAt(direction+this->toggledChecker) ||
													this->hasBlackCheckerAt(this->toggledChecker) && this->hasWhiteCheckerAt(direction+this->toggledChecker) );
	
}

bool lab309::Board::checkerCanMove (const Direction &direction) const {
	bool result;
	
	result = direction.inboundsFor(this->toggledChecker) && this->hasEmptySquareAt(direction+this->toggledChecker);	//can only move if the square in that direction is whitin bounds and empty
	
	if (direction.isForwards()) {
		//can only move forwards if checker is a white or a promoted black
		result = result && (this->hasWhiteCheckerAt(this->toggledChecker) || this->hasPromotedCheckerAt(this->toggledChecker));
	} else {
		//can only move backwards if checker is a black or a promoted white
		result = result && (this->hasBlackCheckerAt(this->toggledChecker) || this->hasPromotedCheckerAt(this->toggledChecker));
	}
	
	return result;
}

/*METHODS*/
int lab309::Board::toggleCheckerAt (int i) const {
	if (this->hasWhiteCheckerAt(i) && this->isWhiteTurn() || this->hasBlackCheckerAt(i) && this->isBlackTurn()) {
		this->toggledChecker = i;
		//std::cout << "Checker at " << i << " selected" << std::endl;	//debug
	} else {
		this->toggledChecker = -1;
	}
	
	return this->toggledChecker;	
}

bool lab309::Board::moveChecker (const Direction &direction) {
	bool valid = false;
	int moved;
	
	if (this->checkerCanMove(direction)) {
		this->checkers[direction+this->toggledChecker] = this->checkers[this->toggledChecker];	//moves checker
		this->checkers[this->toggledChecker] = EMPTY_SQUARE;	//removes checker
		moved = direction+this->toggledChecker;
		this->toggledChecker = -1;
		this->turn++;
		valid = true;
	} else if (this->checkerCanCapture(direction)) {
		bool switchTurn = true;
		this->checkers[direction*2+this->toggledChecker] = this->checkers[this->toggledChecker];	//moves checker
		this->checkers[direction+this->toggledChecker] = EMPTY_SQUARE;	//removes opponent checker
		this->checkers[this->toggledChecker] = EMPTY_SQUARE;	//removes checker
		moved = direction*2+this->toggledChecker;
		
		//chain captures
		this->toggledChecker = direction*2+this->toggledChecker;
		for (int j = 0; j < POSSIBLE_DIRECTIONS; j++) {
			if (this->checkerCanCapture(Board::moveDirections[j])) {
				switchTurn = false;
				break;
			}
		}
		if (switchTurn) {
			this->turn++;
			this->toggledChecker = -1;	
		}
		valid = true;
	}
	
	//promote checker
	if (this->hasWhiteCheckerAt(moved) && moved >= BOARD_COLUMS/2*BOARD_LINES-BOARD_COLUMS/2 || this->hasBlackCheckerAt(moved) && moved < BOARD_COLUMS/2) {
		this->checkers[moved] |= PROMOTED_CHECKER;
	}
	
	//debug
	/*
	if (this->isWhiteTurn()) {
		std::cout << "White plays " << this->turn << std::endl;
	} else {
		std::cout << "Black plays " << this->turn << std::endl;
	}
	*/
	
	return valid;
}

float lab309::Board::evaluate (void) const {
	float score = 0;
	Direction	forwardsLeft = Direction(LEFT|FORWARDS),
				forwardsRight = Direction(RIGHT|FORWARDS),
				backwardsLeft = Direction(LEFT|BACKWARDS),
				backwardsRight = Direction(RIGHT|BACKWARDS);			
	
	for (int i = 0; i < BOARD_COLUMS/2*BOARD_LINES; i++) {
		if (this->hasWhiteCheckerAt(i)) {
			score += SCORE_WHITE_CHECKER * (this->hasPromotedCheckerAt(i) ? SCORE_PROMOTED_MULTIPLIER : 1);
			score += this->hasWhiteCheckerAt(forwardsLeft+i) ? SCORE_WHITE_COVER : 0;
			score += this->hasWhiteCheckerAt(forwardsRight+i) ? SCORE_WHITE_COVER : 0;
		} else if (this->hasBlackCheckerAt(i)) {
			score += SCORE_BLACK_CHECKER * (this->hasPromotedCheckerAt(i) ? SCORE_PROMOTED_MULTIPLIER : 1);
			score += this->hasBlackCheckerAt(backwardsLeft+i) ? SCORE_BLACK_COVER : 0;
			score += this->hasBlackCheckerAt(backwardsRight+i) ? SCORE_BLACK_COVER : 0;
		}
	}
	
	return score;
}

std::list<lab309::State*> lab309::Board::nextStates (void) const {
	Board *next;
	std::list<State*> states;
	for (int i = 0; i < BOARD_LINES*BOARD_COLUMS/2; i++) {
		if (!this->hasEmptySquareAt(i)) {
			this->toggledChecker = i;
			for (int j = 0; j < POSSIBLE_DIRECTIONS; j++) {
				if (this->checkerCanMove(Board::moveDirections[j]) || this->checkerCanCapture(Board::moveDirections[j])) {
					next = new Board(*this);
					next->moveChecker(Board::moveDirections[j]);
					states.push_back(next);
				}
			}
		}	
	}
	
	return states;
}

//game is over if there are no movements left for one of the players
int lab309::Board::isFinal (void) const {
	int i, j;
	int moves = WHITE_CANNOT_MOVE|BLACK_CANNOT_MOVE;
	
	for (i = 0; i < BOARD_LINES*BOARD_COLUMS/2; i++) {
		if (this->hasWhiteCheckerAt(i)) {
			this->toggledChecker = i;
			for (j = 0; j < POSSIBLE_DIRECTIONS; j++) {
				if (this->checkerCanMove(Board::moveDirections[j]) || this->checkerCanCapture(Board::moveDirections[j])) {
					moves &= ~WHITE_CANNOT_MOVE;
					break;
				}
			}
		}
	}
	
	for (i = 0; i < BOARD_LINES*BOARD_COLUMS/2; i++) {
		if (this->hasBlackCheckerAt(i)) {
			this->toggledChecker = i;
			for (j = 0; j < POSSIBLE_DIRECTIONS; j++) {
				if (this->checkerCanMove(Board::moveDirections[j]) || this->checkerCanCapture(Board::moveDirections[j])) {
					moves &= ~BLACK_CANNOT_MOVE;
					break;
				}
			}
		}
	}
	
	return moves;
}
