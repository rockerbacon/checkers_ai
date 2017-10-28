#include "Minimax.h"
#include <math.h>
#include <cstddef>
#include <iostream>

namespace lab309 {

	//node of the search tree
	class Node {
		public:
			const State *state;
			float value;
			Node *father;
			
			Node (void) = default;
			Node (const State *state, float value, Node *father) {
				this->state = state;
				this->value = value;
				this->father = father;
			}
	};
	
	//class to define function to be used on search (min or max)
	class DecisionFunction {
		public:
			virtual std::list<Node*>::iterator f (std::list<Node*> &stateList) const = 0;
			virtual float g (float value, float threshold) const = 0;
	};
	
	class Min : public DecisionFunction {
		public:
			std::list<Node*>::iterator f (std::list<Node*> &stateList) const { 
				std::list<Node*>::iterator min = stateList.begin();
				if (!stateList.empty()) {
					for (std::list<Node*>::iterator i = stateList.begin()++; i != stateList.end(); ++i) {
						if ((*i)->value < (*min)->value) {
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
			std::list<Node*>::iterator f (std::list<Node*> &stateList) const {
				std::list<Node*>::iterator max = stateList.begin();
				if (!stateList.empty()) {
					for (std::list<Node*>::iterator i = stateList.begin()++; i != stateList.end(); ++i) {
						if ((*i)->value > (*max)->value) {
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
	
	Node* minimaxSearch (Node *currentNode, const DecisionFunction &pickBestIn, const DecisionFunction &pickWorstIn, float alpha, float beta, unsigned int depthLimit) {
		if (currentNode->state->isFinal() || depthLimit == 0) {
			currentNode->value = currentNode->state->evaluate();
			return currentNode;
		} else {
			std::list<State*> nextStates;
			std::list<Node*> childNodes;
			std::list<Node*>::iterator bestPlay;
			
			//create list with next valid nodes from current node
			//std::cout << "generating states" << std::endl;	//debug
			nextStates = currentNode->state->nextStates();
			//std::cout << "states generated: " << nextStates.size() << std::endl;	//debug
			for (State *s : nextStates) {
				childNodes.push_back(new Node(s, pickWorstIn.g(-INFINITY, INFINITY), currentNode));
			}
			
			//analyze nodes recursively
			for (std::list<Node*>::iterator n = childNodes.begin(); n != childNodes.end(); n++) {
				*n = minimaxSearch (*n, pickWorstIn, pickBestIn, beta, alpha, depthLimit-1);	//invert decision functions, beta and alpha to switch playing sides
				
				//prune in case a opponent play is already better than what was found in a previous path
				if (pickBestIn.g((*n)->value, beta) == (*n)->value) {
					goto RETURN;
				}
				
				alpha = pickBestIn.g((*n)->value, alpha);	
				
			}
			
		RETURN:
			bestPlay = pickBestIn.f(childNodes);
			
			//free memory of unused nodes
			//std::cout << "cleaning memory" << std::endl;	//debug
			for (std::list<Node*>::iterator i = childNodes.begin(); i != childNodes.end(); i++) {
				if (i != bestPlay) {
					delete((*i)->state);
					delete(*i);
				}
			}
			//std::cout << "memory cleaned" << std::endl;	//debug
			
			return *bestPlay;
		}
	} 
};

std::list<const lab309::State*> lab309::minimax (const State &currentState, unsigned int maxDepth) {
	std::list<const State*> list;
	float alpha = -INFINITY, beta = INFINITY;
	//std::cout << "Cpu is thinking" << std::endl;	//debug
	Node* bestPlay = minimaxSearch(new Node(&currentState, -INFINITY, NULL), Max(), Min(), alpha, beta, maxDepth);
	//std::cout << "Cpu decided something" << std::endl;	//debug
	Node *n;
	Node *next;
	
	//create list with the expected state path
	n = bestPlay;
	while (n->father != NULL) {
		next = n->father;
		list.push_front(n->state);
		delete(n);
		n = next;
	}
	delete(n);
	
	return list;
}

std::list<const lab309::State*> lab309::maximin (const State &currentState, unsigned int maxDepth) {
	std::list<const State*> list;
	float alpha = -INFINITY, beta = INFINITY;
	//std::cout << "Cpu is thinking" << std::endl;	//debug
	Node* bestPlay = minimaxSearch(new Node(&currentState, INFINITY, NULL), Min(), Max(), beta, alpha, maxDepth);
	//std::cout << "Cpu decided something" << std::endl;	//debug
	Node *n;
	Node *next;
	
	//create list with the expected state path
	n = bestPlay;
	while (n->father != NULL) {
		next = n->father;
		list.push_front(n->state);
		delete(n);
		n = next;
	}
	delete(n);
	
	return list;
}
