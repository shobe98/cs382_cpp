/* --------------------
 * CMPU-382, Spring2020
 * Authors:  Andrei Stanciu, Abigail Ren
 * File:     algos.h
 * --------------------
 * Declares methods bellman_ford, is_dynamically_controllable, dijkstra, getRPL,
 * morris2006
 */

#ifndef _ALGOS_H_
#define _ALGOS_H_

#include "graph.h"
#include <vector>
using std::vector;

namespace Morris2006 {

const int kInf = 0x3f3f3f3f;

vector<int> bellman_ford(STNU *stnu, bool debug);
bool is_dynamically_controllable(STNU *stnu);
void dijkstra(CaseEdge lc_Edge, vector<int> &f, STNU *stnu, bool debug);
int getRPL(int x, int y, int dijkstraDistance, vector<int> &f);
bool morris2006(string filename, bool debug);

} // namespace Morris2006
#endif // _ALGOS_H_
