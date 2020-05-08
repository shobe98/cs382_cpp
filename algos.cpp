/* ------------------------------
 * CMPU-382, Spring 2020
 * Authors:  Andrei Stanciu, Abigail Ren
 * File:     algos.cpp
 * ------------------------------
 * Defines Structs: NodeAndPrio
 * Implements methods declared in algos.h
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
vector<int> Morris2006::bellman_ford(Morris2006::STNU *stnu,
                                     bool debug = false) {
  debug && (cout << "RUNNING BELLMAAAAAAAAAAAAAN" << endl);
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
    // Negative cycle detection is correct.
    // Anyway, printing out the actual cycle (for debugging purposes) is
    // unreliable, and thus I suggest you to ignore this code
    if (debug) {
      vector<int> cycle;
      vector<bool> visited(stnu->N, false);
      while (end != start && !visited[end]) {
        visited[end] = true;
        cycle.push_back(end);
        end = parent[end];
        assert(end != parent[end]);
      }

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
int Morris2006::getRPL(int x, int y, int dijkstraDistance, vector<int> &f) {
  return dijkstraDistance + f[y] - f[x];
}

/* Contract: dijkstra(lc_Edge, f, *stnu) -> void
 * Input: CaseEdge lc_Edge is the starting lower case edge,
 *        vector<int> f is the potential function f
 *        Morris2006::STNU *stnu is any STNU graph
 *        debug is boolean flag for printing debug statements
 * Starts from lower case edge lc_Edge and traverses STNU graph
 * until the function is able to reduce away the lower case edge
 */
void Morris2006::dijkstra(CaseEdge lc_Edge, vector<int> &f,
                          Morris2006::STNU *stnu, bool debug = false) {
  debug &&
      (cerr << "Dijkstra from " << stnu->numsToLabel[lc_Edge.B]
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
    if (debug) {

      cerr << endl << "Expanding node " << stnu->numsToLabel[TPnode] << endl;
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

    if (debug) {
      cerr << "After: ";
      for (int i = 0; i < stnu->N; ++i) {
        if (dist[i] != kInf) {
          cerr << stnu->numsToLabel[i] << ":"
               << getRPL(lc_Edge.B, i, dist[i], f) << ' ';
        }
      }
      cerr << endl;

      cerr << "\nThis is the distance vector at the end of dijkstra: " << endl;
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
 * Input: Morris2006::STNU *stnu is any STNU graph
 * Returns whether the given graph *stnu is dynamically
 * controllable or not by running bellman-ford and
 * dijkstra.
 * side effect: modifies the graph by adding new edges
 */
bool Morris2006::is_dynamically_controllable(Morris2006::STNU *stnu) {
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

/* Contract: morris2006(filename, debug)
 * Input:    filename is name of input file
 *           debug boolean flag for printing debug statements
 * Calls is_dynamically_controllable and determines whether
 * STNU graph described in the filename input is DC or not
 */

bool Morris2006::morris2006(string filename, bool debug = false) {
  Morris2006::STNU *Graph = new Morris2006::STNU(filename, debug);

  debug &&cerr << "Done reading!" << endl;

  if (is_dynamically_controllable(Graph)) {
    debug &&cout << "DC" << endl;
    return true;
  } else {
    debug &&cout << "Negative cycle found!" << endl;
    return false;
  }
}

// Returns true if edge is unsuitable
// starting_label is the label on the UC edge that started the backpropagation
// or -1 if the backpropagation has been started from a node that contains no UC
// edges.
inline bool Morris2014::unsuitable(const Edge &e, const int &starting_label) {
  // For now an edge is unsuitable iff it is a lower case edge and it  has the
  // same label as the initial uc edge
  // I'll leave this as a function in case we need to add anithing else here
  return e.type != 'u' && starting_label == e.C;
}

// Note about Dijkstra implementation:
// for comodity we don't edit nodes int the heap, but keep track of wether we're
// done with a node or not, and thus w ecan have a node appear multiple times in
// the queue, I've done this multiple times with dijkstra, and the time
// difference is not significant enough to make it reasonable to implement a
// heap from scratch.
//

/* Contract: DCBackprop(source, stnu, debug)
 * Input:    Source is the index of the negative node to start backpropagation
 * from. stnu is the representation in memory of an stnu debug boolean flag for
 * printing debug statements Effects: Will add ordinary positive edges to the
 * STNU. Output: true if there is no negative cycle containing source
 *
 * DCBackprop is a recursive function, and it keeps track of previous calls of
 * DCBackprop(source). If DCBackprop is called with a source already in the
 * recursive stack, a negative cycle has been found. If DCBackprop has ran
 * successfully before, it returns true without repeating work.
 *
 * It runs a dijkstra propagation starting from source. It stops whenever the
 * distance from source to a node is greater than 0, when it adds the
 * coresponding edge to the graph. If during dijkstra propagation it encounters
 * a negative node, it does a recursive call of DCBackprop on that node, and
 * after it returns it uses the newly generated positive edges to continue the
 * propagation.
 *
 */

bool Morris2014::DCBackprop(int source, Morris2014::STNU *stnu,
                            bool debug = false) {
  debug && (cerr << "Currently running DCBackprop from "
                 << stnu->numsToLabel[source] << endl);
  if (stnu->in_rec_stack[source]) {
    return false;
  }
  if (stnu->done[source]) {
    debug &&cerr << "Node was processed before!" << endl;
    return true;
  }

  stnu->in_rec_stack[source] = true;

  // change with N
  vector<int> dist(stnu->N, kInf);
  vector<int> label(stnu->N, 0);
  vector<bool> done_dijkstra(stnu->N, false);

  int starting_label = -1;

  dist[source] = 0;

  priority_queue<NodeAndPrio> Q;

  for (auto &edge : stnu->InEdges[source]) {
    // Only initialize the preds of the negative node connected by negative
    // edges
    if (edge.value < 0) {
      dist[edge.A] = edge.value;
      Q.push(NodeAndPrio(edge.A, dist[edge.A]));

      if (edge.type == 'u') {
        assert(starting_label == -1);
        starting_label = edge.C;
      }
    }
  }
  done_dijkstra[source] = true;

  while (!Q.empty()) {
    // popping
    int u = Q.top().node;
    Q.pop();

    if (done_dijkstra[u]) {
      continue; // in case u was in the queue multiple times and we already
                // processed it.
    }
    done_dijkstra[u] = true;

    // done popping

    debug &&cerr << endl << "Expanding node " << stnu->numsToLabel[u] << endl;

    if (dist[u] >= 0) {
      // add new edge u->source

      // some bad memory management here. TODO(astanciu): change to a mtarix
      // that keeps track of edges actually, it's probably not needed, it's just
      // a constant optimisation but it doesn't change the complexity. at most
      // N^2 new edges are added (N per negative node), so the final complexity
      // is still N^3 (or N^3log(N) with binary heap)
      //
      // As mentioned in the paper, we only add ordinary edges "by virtue of
      // label removal rule" (where applicable).
      stnu->addEdge(Edge(u, source, dist[u], 'o'));
      // stnu->InEdges[source].push_back(Edge(u, source, dist[u], 'o'));
      debug && (cerr << "Added ord edge " << stnu->numsToLabel[u] << ' '
                     << stnu->numsToLabel[source] << ' ' << dist[u] << endl);
      continue;
    }

    if (stnu->is_negative_node[u]) {
      if (debug) {
        cerr << "about to enter recursion in " << stnu->numsToLabel[u] << endl;
        for (int i = 0; i < stnu->N; ++i) {
          if (dist[i] != kInf) {
            cerr << stnu->numsToLabel[i] << ":" << dist[i] << ' ';
          }
        }
        cerr << endl;
      }

      if (DCBackprop(u, stnu) == false) {
        return false;
      }
    }

    // WARN: If an edge gets added inside this forloop, iterators can get
    // invalidated. Safe for now.
    for (auto &edge : stnu->InEdges[u]) {
      if (edge.value < 0) {
        // we can only get to this case if node u has been processed (first if
        // statement before the loop) so all the relevant edges have been added
        // to u's predecessor list
        continue;
      }

      if (unsuitable(edge, starting_label)) {
        continue;
      }

      if (debug) {
        cerr << "Processing edge ";
        stnu->printEdge(edge);
        cerr << endl;
      }

      int v = edge.A;
      int new_dist = dist[u] + edge.value;
      if (new_dist < dist[v]) {
        dist[v] = new_dist;
        Q.push(NodeAndPrio(v, new_dist));
      }
    }
    if (debug) {
      for (int i = 0; i < stnu->N; ++i) {
        if (dist[i] != kInf) {
          cerr << stnu->numsToLabel[i] << ":" << dist[i] << ' ';
        }
      }
      cerr << endl;
    }

    debug &&cerr << endl;
  }

  if (debug) {
    debug &&cerr << stnu->numsToLabel[source] << "'s propagation done" << endl;
    for (int i = 0; i < stnu->N; ++i) {
      if (dist[i] != kInf) {
        debug &&cerr << stnu->numsToLabel[i] << ":" << dist[i] << ' ';
      }
    }
    debug &&cerr << endl;
  }
  stnu->done[source] = true;
  stnu->in_rec_stack[source] = false;

  // Andrei's addition to the algorithm.
  // It is mentioned in the paper but nor present in the pseudocode.
  if (dist[source] < 0) {
    return false;
  }
  return true;
}

/* Contract: morris2014(string filename, bool debug) -> bool
 * Input: string filename: Formatted txt file with stnu
 *        bool debug: Flag to toggle debugging messages.
 * Output: True if STNU is DC.
 * Parses the graph, marks the negative nodes and runs DCBackprop on each
 * negative node.
 */
bool Morris2014::morris2014(string filename, bool debug = false) {
  Morris2014::STNU Graph(filename, debug);

  debug &&cerr << "Done reading!" << endl;

  for (int i = 0; i < Graph.N; ++i) {
    for (auto &edge : Graph.InEdges[i]) {
      if (edge.value < 0) {
        Graph.is_negative_node[i] = true;
        break;
      }
    }
  }

  debug &&cerr << "Done finding negative nodes!" << endl;

  for (int i = 0; i < Graph.N; ++i) {
    if (Graph.is_negative_node[i]) {
      debug &&cerr << "Going to call DCBackprop from main!" << endl;
      if (!DCBackprop(i, &Graph)) {
        debug &&cout << "Not DC!" << endl;
        return false;
      }
    }
  }

  debug &&cout << "DC!" << endl;
  return true;
}
