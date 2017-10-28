/*
 * This header provides abstractions for the usage of a minimax algorithm for any problem implementing it
 *
 * 		Vitor Andrade dos Santos, 2017
 */

#ifndef LAB309_MINIMAX_H
#define LAB309_MINIMAX_H

#include <list>

namespace lab309 {
	class State {
		public:
			//returns the result of an evaluation of the current state using an heuristic
			virtual float evaluate (void) const = 0;
			
			//returns all the next valid states from the current state. The states MUST BE ALLOCATED USING THE NEW() OPERATOR
			virtual std::list<State*> nextStates (void) const = 0;
			
			//returns 0 still has valid next states
			virtual bool isFinal (void) const = 0;
	};
	
	//returns an ordered list of the states in the path of a Minimax search result given the current state and a maximum depth for the search
	std::list<const State*> minimax (const State &currentState, unsigned int maxDepth);
	std::list<const State*> maximin (const State &currentState, unsigned int maxDepth);	//same as minimax but the lowest value is searched instead
};

#endif
