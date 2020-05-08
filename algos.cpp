/* ------------------------------
 * CMPU-382, Spring 2020
 * Authors:  Andrei Stanciu, Abigail Ren
 * File:     algos.cpp
 * ------------------------------
 * Implements
 * Defines Structs: NodeAndPrio
 * Defines Functions: bellman_ford, dijkstra, is_dynamically_contrallable
 */
#include "algos.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <queue> // priority queue
#include <unordered_map>
#include <vector>
using namespace std;
using std::min;
using std::vector;

const int kInf = 0x3f3f3f3f;

struct NodeAndPrio {
  int node;
  int prio;

  NodeAndPrio(int node = 0, int prio = 0) {
    this->node = node;
    this->prio = prio;
  }

  bool operator<(const NodeAndPrio &other) const {
    // we want a min heap, thus the sign here needs to be 'greater'.
    // (prio queue defaults to max heap)
    return prio > other.prio;
  }
};

/* Runs the unmodified bellman ford algorithm on the STNU.
 * Input: Any STNU
 * Output: a potential function
 * Side effect: Will set the has_negative_cycle flag in the STNU if one is
 * found.
 */
vector<int> bellman_ford(STNU *stnu, bool debug=false) {
  debug && (cout << "RUNNING BELLMAAAAAAAAAAAAAN"
      << endl);
  vector<int> distance(stnu->N,
      0); // this will be the potential function as well

  vector<int> parent(stnu->N, -1);

  // for best performance of bellman this can be optimized by implementing the
  // queue version
  for (int i = 1; i < stnu->N; ++i) {
    for (auto &edge : stnu->ordEdgesList) {
      if (distance[edge.A] + edge.value < distance[edge.B]) {
        distance[edge.B] = min(distance[edge.B], distance[edge.A] + edge.value);
        parent[edge.B] = edge.A;
      }
    }
    for (auto &edge : stnu->ucEdgesList) {
      if (distance[edge.A] + edge.value < distance[edge.B]) {
        distance[edge.B] = min(distance[edge.B], distance[edge.A] + edge.value);
        parent[edge.B] = edge.A;
      }
    }
  }

  int start = -1;
  int end = -1;
  // check for negative cycles
  for (auto &edge : stnu->ordEdgesList) {
    if (distance[edge.B] > distance[edge.A] + edge.value) {
      stnu->has_negative_cycle = true;
      start = edge.B;
      end = edge.A;
    }
  }
  for (auto &edge : stnu->ucEdgesList) {
    if (distance[edge.B] > distance[edge.A] + edge.value) {
      stnu->has_negative_cycle = true;
      start = edge.B;
      end = edge.A;
    }
  }

  if (stnu->has_negative_cycle) {
    vector<int> cycle;
    vector<bool> visited(stnu->N, false);
    while (end != start && !visited[end]) {
      visited[end] = true;
      cycle.push_back(end);
      end = parent[end];
      assert(end != parent[end]);
    }

    if (debug){
      cout << "Cycle found!" << endl;
      reverse(cycle.begin(), cycle.end());
      for (auto it : cycle) {
        cout << stnu->numsToLabel[it] << " ";
      }
      cout << endl;
    }
  }

  return distance;
}

/* Contract: getRPL(x, y , dijkstraDistance, f) -> int
 * Input: x,y indices of time points
 *        dijkstraDistance is the min dist. from x to y 
 *        f is the potential function
 * Calculates the real path length from x to y
 */
int getRPL(int x, int y, int dijkstraDistance, vector<int> &f) {
  return dijkstraDistance + f[y] - f[x];
}

/* Contract: dijkstra(lc_Edge, f, *stnu) -> void
 * Input: CaseEdge lc_Edge is the starting lower case edge,
 *        vector<int> f is the potential function f
 *        STNU *stnu is any STNU graph
 *        debug is boolean flag for printing debug statements
 * Starts from lower case edge lc_Edge and traverses STNU graph
 * until the function is able to reduce away the lower case edge
 */
void dijkstra(CaseEdge lc_Edge, vector<int> &f, STNU *stnu, bool debug=false) {
  debug && (cerr << "Dijkstra from " << stnu->numsToLabel[lc_Edge.B]
      << " with lc edge from " << stnu->numsToLabel[lc_Edge.A] << " to "
      << stnu->numsToLabel[lc_Edge.B] << " with label "
      << stnu->numsToLabel[lc_Edge.C] << ":" << lc_Edge.value << endl);
  // The minimum distances from C (lc_edge.B) to all the nodes
  vector<int> dist(stnu->N, kInf);
  // Whether we're done processing a node or not.
  vector<bool> dijkstra_done(stnu->N, false);

  // the priority queue takes in node X and priority (key)
  priority_queue<NodeAndPrio> q;
  q.push(NodeAndPrio(lc_Edge.B, 0));

  dist[lc_Edge.B] = 0;

  // Since priority_queue does not support updating priorities the strategy is
  // to addd a node to the PQ every single time it gets updated with a lower
  // distance Thus, when a node gets popped off the queue, it is at it's minimum
  // distance among all its copies in the queue. After the node gets processed
  // all those copies become irrelevant. Marking the node as done after it gets
  // popped helps as identify the redundant copies and skip them.
  while (!q.empty()) {
    int TPnode = q.top().node;
    int TPval = q.top().prio;

    // if dijkstra_done[TP.node] = true, TP was processed previously,
    // so we pop off the node and continue without doing anything to it
    if (dijkstra_done[TPnode]) {
      q.pop();
      continue;
    }
    dijkstra_done[TPnode] = true;
    if (debug){

      cerr << endl << "Expanding node " 
        << stnu->numsToLabel[TPnode] << endl;
      cerr << "Before: ";
      for (int i = 0; i < stnu->N; ++i) {
        if (dist[i] != kInf) {
          cerr << stnu->numsToLabel[i] << ":" 
            << getRPL(lc_Edge.B, i, dist[i], f) << ' ';
        }
      }
      cerr << endl;
    }
    int rpl = TPval + f[TPnode] - f[lc_Edge.B];
    assert(rpl == getRPL(lc_Edge.B, TPnode, TPval, f));
    if (rpl < 0) {
      stnu->addEdge(OrdEdge(lc_Edge.A, lc_Edge.value + rpl, TPnode));
      continue;
    }

    // for each successor edge
    // ordinary
    for (auto &succ : stnu->ordNeighbours[TPnode]) {
      // get OrdEdge o from the list of ordinary edges
      OrdEdge orde = stnu->ordEdgesList[succ.index];
      // non negative edge value
      int nn_delta = f[TPnode] + orde.value - f[orde.B];
      if (TPval + nn_delta < dist[orde.B]) {
        dist[orde.B] = TPval + nn_delta;
        // Push instead of update. Explained above.
        q.push(NodeAndPrio(orde.B, TPval + nn_delta));
      }
    }
    // uc
    for (auto &succ : stnu->ucNeighbours[TPnode]) {
      CaseEdge ucedge = stnu->ucEdgesList[succ.index];
      int nn_delta = f[TPnode] + ucedge.value - f[ucedge.B];

      // This path is in fact an upper case edge generated by the upper case
      // rule applied to the ordinary edge (dijkstra path) from lc_edge.B to
      // ucedge.A and to ucedge.

      int nn_path = TPval + nn_delta;
      int rpl = nn_path + f[ucedge.B] - f[lc_Edge.B];

      // The lower case that came together (in a contingent link) with the
      // upper case edge ucedge
      const auto &lower_case_edge_aa_cc =
        stnu->lcEdgesList[stnu->lcEdges[ucedge.B][ucedge.C]];
      int minv = lower_case_edge_aa_cc.value;

      // The label removal rule.
      if (rpl >= -minv) {
        if (nn_path < dist[ucedge.B]) {
          q.push(NodeAndPrio(ucedge.B, nn_path));
        }
      }
      // If the label removal rule doesn't work, then reduce the lower case
      // edge
      //
      // Shouldn't we try to apply the cross case rule even if the label
      // removal rule works?
      //
      else if (lower_case_edge_aa_cc.C != lc_Edge.C) { // rpl < -min < 0
        // The cross-case rule. lc_edge + (lc_edge.B ~~~~dijkstra path~~~~~>
        // ucedge.A ---(C:value)---> ucedge.B)
        stnu->addUpperCaseEdge(CaseEdge(
              lc_Edge.A, ucedge.B, lower_case_edge_aa_cc.C, lc_Edge.value + rpl));
      }
    }

    if (debug){
      cerr << "After: ";
      for (int i = 0; i < stnu->N; ++i) {
        if (dist[i] != kInf) {
          cerr << stnu->numsToLabel[i] << ":" 
            << getRPL(lc_Edge.B, i, dist[i], f) << ' ';
        }
      }
      cerr << endl;


      cerr << "\nThis is the distance vector at the end of dijkstra: " 
        << endl;
      for (int i = 0; i < stnu->N; ++i) {
        if (dist[i] != kInf) {
          cerr << stnu->numsToLabel[i] << ":" 
            << getRPL(lc_Edge.B, i, dist[i], f) << ' ';
        }
      }
      cerr << endl;
    }
  }
}

/* Contract: is_dynamically_controllable(*stnu) -> bool
 * Input: STNU *stnu is any STNU graph
 * Returns whether the given graph *stnu is dynamically
 * controllable or not by running bellman-ford and
 * dijkstra.
 * side effect: modifies the graph by adding new edges
 */
bool is_dynamically_controllable(STNU *stnu) {
  for (int rep = 1; rep <= stnu->K; ++rep) {
    vector<int> f = bellman_ford(stnu); // potential function

    if (stnu->has_negative_cycle) {
      return false;
    }

    for (auto &lc_edge : stnu->lcEdgesList) {
      // run dijkstra with the potential function
      dijkstra(lc_edge, f, stnu);
    }
    // adds all edges to the graph
    stnu->updateAllBufferedEdges();
  }

  // Final check if we've generated a negative cycle in the OU graph
  bellman_ford(stnu); // we don't need the potential function

  if (stnu->has_negative_cycle) {
    return false;
  }

  // if no negative cycle is found
  return true;
}
