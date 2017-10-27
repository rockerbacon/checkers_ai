#include "Minimax.h"
#include <math.h>
#include <cstddef>

namespace lab309 {

	//node of the search tree
	class Node {
		public:
			const State *state;
			float value;
			Node *child;
			
			Node (void) = default;
			Node (const State *state, float value, Node *child) {
				this->state = state;
				this->value = value;
				this->child = child;
			}
	};
	
	//class to define function to be used on search (min or max)
	class DecisionFunction {
		public:
			virtual std::list<Node>::iterator f (std::list<Node> &stateList) const = 0;
			virtual float g (float value, float threshold) const = 0;
	};
	
	class Min : public DecisionFunction {
		public:
			std::list<Node>::iterator f (std::list<Node> &stateList) const { 
				std::list<Node>::iterator min = stateList.begin();
				if (!stateList.empty()) {
					for (std::list<Node>::iterator i = stateList.begin()++; i != stateList.end(); i++) {
						if (i->value < min->value) {
							min = i;
						}
					}
				}
			
				return min;
			}
		
			float g (float value, float threshold) const {
				return (value < threshold) ? value : threshold;
			}
	};
	
	class Max : public DecisionFunction {
		public:
			std::list<Node>::iterator f (std::list<Node> &stateList) const {
				std::list<Node>::iterator max = stateList.begin();
				if (!stateList.empty()) {
					for (std::list<Node>::iterator i = stateList.begin()++; i != stateList.end(); i++) {
						if (i->value < max->value) {
							max = i;
						}
					}
				}
			
				return max;
			}
		
			float g (float value, float threshold) const {
				return (value > threshold) ? value : threshold;
			}
	};
	
	Node minimaxSearch (Node currentNode, const DecisionFunction &pickBestIn, const DecisionFunction &pickWorstIn, float alpha, float beta, unsigned int depthLimit) {
		if (currentNode.state->isFinal() || depthLimit == 0) {
			currentNode.value = currentNode.state->evaluate();
			return currentNode;
		} else {
			std::list<State*> nextStates;
			std::list<Node> childNodes;
			std::list<Node>::iterator bestPlay;
			
			//create list with next valid nodes from current node
			nextStates = currentNode.state->nextStates();
			for (State *s : nextStates) {
				childNodes.push_back(Node(s, pickWorstIn.g(-INFINITY, INFINITY), NULL));
			}
			
			//analyze nodes recursively
			for (std::list<Node>::iterator n = childNodes.begin(); n != childNodes.end(); n++) {
				*n = minimaxSearch (*n, pickWorstIn, pickBestIn, beta, alpha, depthLimit-1);	//invert decision functions, beta and alpha to switch playing sides
				
				//prune in case a opponent play is already better than what was found in a previous path
				if (pickBestIn.g(n->value, beta) == n->value) {
					goto RETURN;
				}
				
				alpha = pickBestIn.g(n->value, alpha);
			}
			
		RETURN:
			bestPlay = pickBestIn.f(childNodes);
			currentNode.child = new Node(bestPlay->state, bestPlay->value, bestPlay->child);
			
			//free memory of unused nodes
			for (std::list<Node>::iterator i = childNodes.begin(); i != childNodes.end(); i++) {
				if (i != bestPlay) {
					Node *child = i->child, *next;
					while (child != NULL) {
						next = child;
						delete(i->child->state);
						delete(child);
						child = next;
					}
					delete(i->state);
				}
			}
			
			return *bestPlay;
		}
	} 
};

std::list<const lab309::State*> lab309::minimax (const State &currentState, unsigned int maxDepth) {
	std::list<const State*> list;
	Node bestPlay = minimaxSearch(Node(&currentState, -INFINITY, NULL), Max(), Min(), -INFINITY, INFINITY, maxDepth);
	Node *n;
	Node *next;
	
	//create list with the expected state path
	list.push_back(bestPlay.state);
	n = bestPlay.child;
	while (n != NULL) {
		next = n->child;
		list.push_back(n->state);
		delete(n);
		n = next;
	}
	
	return list;
}
