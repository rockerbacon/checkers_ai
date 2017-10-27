#include "Board.h"

Vector<int> lab309::mapPixelToGrid (const Window &window, const Vector<int> &pixel) {
	Vector<int> v(2);
	int gridWidth = window.getWidth()/BOARD_COLUMS;
	int gridHeight = window.getHeight()/BOARD_COLUMS;
	
	v[COORDINATE_X] = (pixel[COORDINATE_X] - pixel[COORDINATE_X] % gridWidth)/gridWidth;
	v[COORDINATE_Y] = ((pixel[COORDINATE_Y]+window.getHeight())%window.getHeight() - pixel[COORDINATE_Y] % gridHeight)/gridHeight;
	
	return v;
}

//Calculation for a complete matrix would be i*colums + j;
//In this case there's half as many colums per line, so i*(colums/2)
//The colums are offseted in odd lines: R:j -> j*2; so j/2
//The colums are offseted in even lines: R:j -> j*2+1; so (j-1)/2
//j^0x1 to flip the least significant bit which determines if a number is odd or even then &0x1 which will result in 0 when j is odd and 1 otherwise
int lab309::mapGridToChecker (const Vector<int> &pos) {
	return pos[COORDINATE_Y]*(BOARD_COLUMS/2) + (pos[COORDINATE_X] - ((pos[COORDINATE_Y]^0x1)&0x1)) / 2;
}

Vector<int> mapGridToPixel (const Window &window, const Vector<int> &pos) {
	Vector<int> v(2);
	int gridWidth = window.getWidth()/BOARD_COLUMS;
	int gridHeight = window.getHeight()/BOARD_COLUMS;
	
	v[COORDINATE_X] = pos[COORDINATE_X]*gridWidth;
	v[COORDINATE_Y] = ((pos[COORDINATE_Y]+BOARD_LINES)%BOARD_LINES))*gridHeight;
	
	return v;
}

Vector<int> mapCheckerToGrid (int i) {
	Vector<int> v(2);
	
	v[COORDINATE_Y] = i/(BOARD_COLUMS/2);
	v[COORDINATE_X] = (i%(BOARD_COLUMS/2) + ((v[COORDINATE_Y]^0x1)&0x1)) * 2;
	
	return v;
}

/*DIRECTION*/
//The map determines the offset in the checkers[] attribute 
lab309::Direction::Direction (int directionMap) {
	int	over = (directionMap & OVER)/OVER,
		vertical = (directionMap & FORWARDS) ? 1+over : -1-over,
		horizontal = (directionMap & LEFT) ? -over : 1;	//right is always 1, left is -1 when over ~= 0 and 0 otherwise
	
	this->directionMap = directionMap;
	
	this->offset = vertical*BOARD_COLUMS/2 + horizontal;
}

bool lab309::Direction::isForwards (void) const {
	return this->directionMap & FORWARDS;
}

bool lab309::Direction::isLeft (void) const {
	return this->directionMap & LEFT;
}

bool lab309::Direction::inboundsFor (int checker) const {
	bool result;
	const Direction &d;
	
	if (this->directionMap & OVER) {
		d = Direction(this->directionMap^OVER);
		checker = checker + d;
	} else {
		d = *this;
	}
	
	if (this->isLeft()) {
		result = checker % BOARD_COLUMS == 0 || checker % (BOARD_COLUMS/2);	//can only move left if the checker is not in the far left colum
	} else {
		result = checker % BOARD_COLUMS-1 == 0 || checker % (BOARD_COLUMS/2-1);	//can only move right if the checker is not in the far right colum
	}
	
	if (this->isForwards()) {
		//can only move forwards if checker is not at the far up line
		result = result && checker < (BOARD_LINES-1)*BOARD_COLUMS/2;
	} else {
		//can only move backwards if checker is not at the far down line
		result = result && checker >= BOARD_COLUMS/2;
	}
	
	return result;
}

int& lab309::Board::getCheckerAt (const Vector<int> &pixel) {
	return this->checkers[mapGridToChecker(mapPixelToGrid(pixel))];
}

/*BOARD*/
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
		this->checkers = BLACK_CHECKER;
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

bool lab309::Board::checkerCanCapture (const Direction &direction) const {
	Direction overDirection = Direction(direction.getMap()|OVER);
	
	//checker can capture if the movement is valid and it's a white with a black in its direction or a black with a white in its direction
	return this->checkerCanMove(overDirection) && (	this->hasWhiteCheckerAt(this->toggledChecker) && this->hasBlackCheckerAt(this->toggledChecker+direction) ||
													this->hasBlackCheckerAt(this->toggledChecker) && this->hasWhiteCheckerAt(this->toggledChecker+direction);
	
}

bool lab309::Board::checkerCanMove (const Direction &direction) const {
	bool result;
	
	result = direction.inboundsFor(this->toggledChecker) && this->hasEmptySquareAt(this->toggledChecker+direction);	//can only move if the square in that direction is whitin bounds and empty
	
	if (direction.isForwards()) {
		//can only move forwards if checker is a white or a promoted black
		result = result && (this->hasWhiteCheckerAt(this->toggledChecker) || this->hasPromotedCheckerAt(this->toggledChecker));
	} else {
		//can only move backwards if checker is a black or a promoted white
		result = result && (this->hasBlackCheckerAt(this->toggledChecker) || this->hasPromotedCheckerAt(this->toggledChecker));
	}
	
	return result;
}

unsigned int lab309::Board::getTurn (void) const {
	return this->turn&0x1;
}

/*METHODS*/
void lab309::Board::toggleCheckerAt (const Vector<int> &pixel) {
	this->toggledChecker = mapGridToChecker(mapPixelToGrid(pixel));
}

bool lab309::Board::moveChecker (const Direction &direction) {
	bool valid = false;
	
	if (this->checkerCanMove(direction)) {
		this->checkers[this->toggledChecker+direction] = this->checkers[this->toggledChecker];	//moves checker
		this->checkers[this->toggledChecker] = EMPTY_SQUARE;	//removes checker
		this->toggledChecker = this->toggledChecker+direction;	//keeps track of last moved checker
		this->turn++;
		valid = true;
	} else if (this->checkerCanCapture(direction)) {
		bool switchTurn = true;
		this->checkers[this->toggledChecker+2*direction] = this->checkers[this->toggledChecker];	//moves checker
		this->checkers[this->toggledChecker+direction] = EMPTY_SQUARE;	//removes opponent checker
		this->checkers[this->toggledChecker] = EMPTY_SQUARE;	//removes checker
		this->toggledChecker = this->toggledChecker+2*direction;	//keeps track of last moved checker
		for (int j = 0; j < POSSIBLE_DIRECTIONS; j++) {
			if (this->checkerCanCapture(Board::moveDirections[j])) {
				switchTurn = false;
				break;
			}
		}
		if (switchTurn) { this->turn++; }
		valid = true;
	}
	
	return valid;
}

float lab309::Board::evaluate (void) const {
	float score = 0;
	Direction	forwardsLeft = Direction(LEFT|FORWARDS),
				forwardsRight = Direction(RIGHT|FORWARDS),
				backwardsLeft = Direction(LEFT|BACKWARDS),
				backwardsRight = Direction(RIGHT|BACKWARDS);			
	
	for (int i = 0; i < BOARD_COLUMS*BOARD_LINES/2; i++) {
		if (this->hasWhiteCheckerAt(i)) {
			score += SCORE_WHITE_CHECKER * (this->hasPromotedCheckerAt(i) ? SCORE_PROMOTED_MULTIPLIER : 1);
			score += this->hasWhiteCheckerAt(i+forwardsLeft) ? SCORE_WHITE_COVER : 0;
			score += this->hasWhiteCheckerAt(i+forwardsRight) ? SCORE_WHITE_COVER : 0;
		} else if (this->hasBlackCheckerAt(i)) {
			score += SCORE_BLACK_CHECKER * (this->hasPromotedCheckerAt(i) ? SCORE_PROMOTED_MULTIPLIER : 1);
			score += this->hasBlackCheckerAt(i+backwardsLeft) ? SCORE_BLACK_COVER : 0;
			score += this->hasBlackCheckerAt(i+backwardsRight) ? SCORE_BLACK_COVER : 0;
		}
	}
	
	return score;
}

std::list<State*> nextStates (void) const {
	Board *next;
	std::list<State*> states;
	for (int i = 0; i < BOARD_LINES*BOARD_COLUMS/2; i++) {
		if (!this->hasEmptyCheckerAt(i)) {
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
