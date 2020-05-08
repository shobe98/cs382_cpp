/* --------------------
 * CMPU-382, Spring2020
 * Authors:  Andrei Stanciu, Abigail Ren
 * File:     algos.h
 * --------------------
 * In namespace Morris2006:
 *  Declares methods bellman_ford, is_dynamically_controllable, dijkstra,
 * getRPL, morris2006
 *
 * In namespace Morris2014:
 *  Defines Structs: NodeAndPrio
 *  Declares Functions: unsuitable, DCBackprop, morris2014
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

namespace Morris2014 {

const int kInf = 0x3f3f3f3f;

bool DCBackprop(int source, Morris2014::STNU *stnu, bool debug);
bool morris2014(string filename, bool debug);
inline bool unsuitable(const Edge &e, const int &starting_label);

} // namespace Morris2014

#endif // _ALGOS_H_
