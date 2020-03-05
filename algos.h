#ifndef _ALGOS_H_
#define _ALGOS_H_

#include <vector>
#include "graph.h"
using std::vector;

vector<int> bellman_ford(STNU *stnu);
bool is_dynamically_controllable(STNU *stnu); 

#endif // _ALGOS_H_
