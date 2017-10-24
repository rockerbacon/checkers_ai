#include "Minimax.h"
#include <math.h>

namespace lab309 {

	//node of the search tree
	class Node {
		public:
			State *state;
			float value;
			Node *child;
			
			Node (void) = default;
			Node (State *state, float value, Node *child) {
				this->state = state;
				this->value = value;
				this->child = child;
			}
	};
	
	//class to define function to be used on search (min or max)
	class DecisionFunction {
		public:
			virtual std::list<Node>::iterator f (const std::list<Node> &stateList) const = 0;
			virtual float g (float value, float threshold);
	};
	
	class Min : public DecisionFunction {
		public:
			std::list<Node>::iterator f (const std::list<Node> &stateList) {
				std::list<Node>::iterator min = stateList.begin();
				if (!stateList.empty()) {
					for (std::list<Node>::iterator i = stateList.begin()++; i != !stateList.end(); i++) {
						if ((*i).value < (*min).value) {
							min = i;
						}
					}
				}
			
				return min;
			}
		
			float g (float value, float threshold) {
				return (value < threshold) ? value : threshold;
			}
	};
	
	class Max : public DecisionFunction {
		public:
			std::list<Node>::iterator f (const std::list<Node> &stateList) {
				std::list<Node>::iterator max = stateList.begin();
				if (!stateList.empty()) {
					for (std::list<Node>::iterator i = stateList.begin()++; i != !stateList.end(); i++) {
						if ((*i).value < (*max).value) {
							max = i;
						}
					}
				}
			
				return max;
			}
		
			float g (float value, float threshold) {
				return (value > threshold) ? value : threshold;
			}
	};
	
	Node* minimaxSearch (State *currentState, const DecisionFunction &pickBestIn, const DecisionFunction &pickWorstIn, float alpha, float beta, unsigned int depthLimit) {
		//if state is final or the depth limit was reached end the search
		if (currentState.isFinal() || depthLimit == 0) {
			return new Node(currentState, currentState->evaluate(), NULL);
		} else {
			Node opponentPlay;
			Node bestPlay;
			//generate all valid states after the current state
			std::list<State*> nextStates = currentState->nextStates();
			std::list<Node> children;
			
			for (State *s : nextStates) {
				//analyze state recursively. In the next depth the opponent plays, so the decision functions and alpha/beta values are switched
				opponentPlay = minimaxSearch(s, pickWorstIn, pickBestIn, beta, alpha, depthLimit-1);
				children.push_back(opponentPlay);
				
				//prune in case the opponent already has a better play
				if (pickBestIn.g(opponentPlay.value, beta) == opponentPlay.value) {
					goto RETURN;
				}
				
				//update alpha in case a better play for the player is found
				alpha = pickBestIn.g(opponentPlay.value, alpha);
			}
			
		RETURN:
			bestPlay = pickBestIn.f(children);
			currentState.child = new Node((*bestPlay).state, (*bestPlay).value, (*bestPlay).child);
			//clear memory of all unused states
			for (std::list<Node>::iterator i = children.begin(); i != list.end(); i++) {
				if (i != bestPlay) {
					delete(i.state);
				}
			}
			
			return currentState.child;
			
		}
	} 
};

std::list<State*> lab309::minimax (const State &currentState, unsigned int maxDepth) {
	std::list<State*> list;
	Node *bestPlay = minimaxSearch(&currentState, Max(), Min(), -INFINITY, INFINITY, maxDepth);
	Node *next;
	
	//create list with the expected state path
	while (bestPlay != NULL) {
		list.push_back(bestPlay->state);
		next = bestPlay->child;
		delete(bestPlay);
		bestPlay = next;
	}
	
	return list;
}
