#include "Board.h"

//Scores used for the heuristic 1
#define SCORE1_WHITE_CHECKER 3
#define SCORE1_BLACK_CHECKER -3
#define SCORE1_PROMOTED_MULTIPLIER 2
#define SCORE1_WHITE_COVER 1.5
#define SCORE1_BLACK_COVER -1.5

//scores used for the heuristic 2
#define SCORE2_WHITE_CHECKER 4
#define SCORE2_BLACK_CHECKER -4
#define SCORE2_PROMOTED_MULTIPLIER 1.5
#define SCORE2_WHITE_COVER 0.1
#define SCORE2_BLACK_COVER -0.1

#define SCORE_WHITE_WINS 1000
#define SCORE_BLACK_WINS -1000

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

bool lab309::Board::whiteHasCapture (void) const {
	int previouslyToggled = this->toggledChecker;
	
	for (int i = 0; i < BOARD_COLUMS/2*BOARD_LINES; i++) {
		if (this->hasWhiteCheckerAt(i)) {
			this->toggledChecker = i;
			for (int j = 0; j < POSSIBLE_DIRECTIONS; j++) {
				if (this->checkerCanCapture(Board::moveDirections[j])) {
					this->toggledChecker = previouslyToggled;
					return true;
				}
			}
		}	
	}
	
	this->toggledChecker = previouslyToggled;
	return false;
}

bool lab309::Board::blackHasCapture (void) const {
	int previouslyToggled = this->toggledChecker;
	
	for (int i = 0; i < BOARD_COLUMS/2*BOARD_LINES; i++) {
		if (this->hasBlackCheckerAt(i)) {
			this->toggledChecker = i;
			for (int j = 0; j < POSSIBLE_DIRECTIONS; j++) {
				if (this->checkerCanCapture(Board::moveDirections[j])) {
					this->toggledChecker = previouslyToggled;
					return true;
				}
			}
		}	
	}
	
	this->toggledChecker = previouslyToggled;
	return false;
}


bool lab309::Board::checkerCanCapture (const Direction &direction) const {
	Direction over = direction*2;
	bool result;
	
	result = over.inboundsFor(this->toggledChecker) && this->hasEmptySquareAt(over+this->toggledChecker);	//can only capture if the square in that direction is whitin bounds and empty
	//piece can only move in its turn
	result = result && (this->hasWhiteCheckerAt(this->toggledChecker) && this->isWhiteTurn() || this->hasBlackCheckerAt(this->toggledChecker) && this->isBlackTurn());
	if (direction.isForwards()) {
		//can only move forwards if checker is a white or a promoted black
		result = result && (this->hasWhiteCheckerAt(this->toggledChecker) || this->hasPromotedCheckerAt(this->toggledChecker));
	} else {
		//can only move backwards if checker is a black or a promoted white
		result = result && (this->hasBlackCheckerAt(this->toggledChecker) || this->hasPromotedCheckerAt(this->toggledChecker));
	}
	
	//checker can capture if the movement is valid and it's a white with a black in its direction or a black with a white in its direction
	return result && (	this->hasWhiteCheckerAt(this->toggledChecker) && this->hasBlackCheckerAt(direction+this->toggledChecker) ||
						this->hasBlackCheckerAt(this->toggledChecker) && this->hasWhiteCheckerAt(direction+this->toggledChecker) );
	
}

bool lab309::Board::checkerCanMove (const Direction &direction) const {
	bool result;
	
	result = direction.inboundsFor(this->toggledChecker) && this->hasEmptySquareAt(direction+this->toggledChecker);	//can only move if the square in that direction is whitin bounds and empty
	//piece can only move in its turn
	result = result && (this->hasWhiteCheckerAt(this->toggledChecker) && this->isWhiteTurn() || this->hasBlackCheckerAt(this->toggledChecker) && this->isBlackTurn());
	
	result = result && (this->hasWhiteCheckerAt(this->toggledChecker) && !this->whiteHasCapture() || this->hasBlackCheckerAt(this->toggledChecker) && !this->blackHasCapture());	//can only move if checker has no available capture
	
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
void lab309::Board::toggleCheckerAt (int i) const {
	this->toggledChecker = i;	
}

bool lab309::Board::moveChecker (const Direction &direction) {
	bool valid = false;
	int moved;
	bool switchTurn;
	
	if (this->checkerCanMove(direction)) {
		this->checkers[direction+this->toggledChecker] = this->checkers[this->toggledChecker];	//moves checker
		this->checkers[this->toggledChecker] = EMPTY_SQUARE;	//removes checker
		moved = direction+this->toggledChecker;
		switchTurn = true;
		valid = true;
	} else if (this->checkerCanCapture(direction)) {
		this->checkers[direction*2+this->toggledChecker] = this->checkers[this->toggledChecker];	//moves checker
		this->checkers[direction+this->toggledChecker] = EMPTY_SQUARE;	//removes opponent checker
		this->checkers[this->toggledChecker] = EMPTY_SQUARE;	//removes checker
		moved = direction*2+this->toggledChecker;
		
		//chain captures
		this->toggleCheckerAt(moved);
		switchTurn = true;
		for (int j = 0; j < POSSIBLE_DIRECTIONS; j++) {
			if (this->checkerCanCapture(Board::moveDirections[j])) {
				switchTurn = false;
				break;
			}
		}
		valid = true;
	}
	
	//promote checker
	if (this->hasWhiteCheckerAt(moved) && moved >= BOARD_COLUMS/2*BOARD_LINES-BOARD_COLUMS/2 || this->hasBlackCheckerAt(moved) && moved < BOARD_COLUMS/2) {
		this->checkers[moved] |= PROMOTED_CHECKER;
	}
	
	if (switchTurn) {
		this->turn++;
		this->toggledChecker = -1;	
	}
	
	return valid;
}

std::list<lab309::State*> lab309::Board::nextStates (void) const {
	Board *next;
	std::list<State*> nextStates;
	int toggled = this->toggledChecker;
	
	for (int i = 0; i < BOARD_COLUMS/2*BOARD_LINES; i++) {
		this->toggleCheckerAt(i);
		for (int j = 0; j < POSSIBLE_DIRECTIONS; j++) {
			if (this->checkerCanMove(Board::moveDirections[j]) || this->checkerCanCapture(Board::moveDirections[j])) {
				next = new Board(*this);
				next->moveChecker(Board::moveDirections[j]);
				nextStates.push_back(next);
			}
		}
	}
	
	//std::cout << nextStates.size() << std::endl;	//debug
	this->toggleCheckerAt(toggled);
	return nextStates;
}

//game is over if the current player cannot move
int lab309::Board::isFinal (void) const {
	int i, j;
	int toggled = this->toggledChecker;
	
	for (i = 0; i < BOARD_LINES*BOARD_COLUMS/2; i++) {
		this->toggleCheckerAt(i);
		for (j = 0; j < POSSIBLE_DIRECTIONS; j++) {
			if (this->checkerCanMove(Board::moveDirections[j]) || this->checkerCanCapture(Board::moveDirections[j])) {
				this->toggleCheckerAt(toggled);
				return 0;
			}
		}
	}

	this->toggleCheckerAt(toggled);
	return this->isWhiteTurn() ? BLACK_WINS : WHITE_WINS;
}

bool lab309::Board::operator== (const Board &board) const {
	for (int i = 0; i < BOARD_COLUMS/2*BOARD_LINES; i++) {
		if (this->checkers[i] != board.checkers[i]) {
			return false;
		}	
	}
	
	return true;
}

std::string lab309::Board::toString (void) const {
	std::ostringstream stream;
	
	int i;
	stream << "[";
	for (i = 0; i < BOARD_COLUMS/2*BOARD_LINES-1; i++) {
		stream << std::to_string(this->checkers[i]) << ", ";
	}
	stream << std::to_string(this->checkers[i]) << "]";
	
	return stream.str();
}

float lab309::evaluate1 (const State *state, int isFinal) {
	float score = 0;
	const Board *board = (Board*)state;
	Direction	forwardsLeft = Direction(LEFT|FORWARDS),
				forwardsRight = Direction(RIGHT|FORWARDS),
				backwardsLeft = Direction(BACKWARDS|LEFT),
				backwardsRight = Direction(BACKWARDS|RIGHT);			
	
	for (int i = 0; i < BOARD_COLUMS/2*BOARD_LINES; i++) {
		if (board->hasWhiteCheckerAt(i)) {
			score += SCORE1_WHITE_CHECKER * (board->hasPromotedCheckerAt(i) ? SCORE1_PROMOTED_MULTIPLIER : 1);
			score += (forwardsLeft.inboundsFor(i) && board->hasWhiteCheckerAt(forwardsLeft+i)) ? SCORE1_WHITE_COVER : 0;
			score += (forwardsRight.inboundsFor(i) && board->hasWhiteCheckerAt(forwardsRight+i)) ? SCORE1_WHITE_COVER : 0;
		} else if (board->hasBlackCheckerAt(i)) {
			score += SCORE1_BLACK_CHECKER * (board->hasPromotedCheckerAt(i) ? SCORE1_PROMOTED_MULTIPLIER : 1);
			score += (backwardsLeft.inboundsFor(i) && board->hasBlackCheckerAt(backwardsLeft+i)) ? SCORE1_BLACK_COVER : 0;
			score += (backwardsRight.inboundsFor(i) && board->hasBlackCheckerAt(backwardsRight+i)) ? SCORE1_BLACK_COVER : 0;
		}
	}

	if (isFinal == WHITE_WINS) {
		score += SCORE_WHITE_WINS;
	} else if (isFinal == BLACK_WINS) {
		score += SCORE_BLACK_WINS;
	}
	
	return score;
}

float lab309::evaluate2 (const State *state, int isFinal) {
	float score = 0;
	const Board *board = (Board*)state;
	Direction	forwardsLeft = Direction(LEFT|FORWARDS),
				forwardsRight = Direction(RIGHT|FORWARDS),
				backwardsLeft = Direction(BACKWARDS|LEFT),
				backwardsRight = Direction(BACKWARDS|RIGHT);			
	
	for (int i = 0; i < BOARD_COLUMS/2*BOARD_LINES; i++) {
		if (board->hasWhiteCheckerAt(i)) {
			score += SCORE2_WHITE_CHECKER * (board->hasPromotedCheckerAt(i) ? SCORE2_PROMOTED_MULTIPLIER : 1);
			//
			score += (forwardsLeft.inboundsFor(i) && board->hasWhiteCheckerAt(forwardsLeft+i)) ? SCORE2_WHITE_COVER : 0;
			score += (forwardsRight.inboundsFor(i) && board->hasWhiteCheckerAt(forwardsRight+i)) ? SCORE2_WHITE_COVER : 0;
		} else if (board->hasBlackCheckerAt(i)) {
			score += SCORE2_BLACK_CHECKER * (board->hasPromotedCheckerAt(i) ? SCORE2_PROMOTED_MULTIPLIER : 1);
			score += (backwardsLeft.inboundsFor(i) && board->hasBlackCheckerAt(backwardsLeft+i)) ? SCORE2_BLACK_COVER : 0;
			score += (backwardsRight.inboundsFor(i) && board->hasBlackCheckerAt(backwardsRight+i)) ? SCORE2_BLACK_COVER : 0;
		}
	}

	if (isFinal == WHITE_WINS) {
		score += SCORE_WHITE_WINS;
	} else if (isFinal == BLACK_WINS) {
		score += SCORE_BLACK_WINS;
	}
	
	return score;
}
