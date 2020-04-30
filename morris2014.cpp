#include "graph2014.h"
#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
using namespace std;

static int DEBUG = 0;

const int kMaxLabels = 1000;
const int kNaN = -1; // -1 for now
const int kInf = 0x3f3f3f3f;

struct NodeAndPrio {
  int node;
  /// we very likeley need labels
  int prio;

  NodeAndPrio(int node = 0, int prio = 0) {
    this->node = node;
    this->prio = prio;
  }

  bool operator<(const NodeAndPrio &other) const {
    return prio > other.prio; // we want a min heap, thus the sign here needs to
                              // be 'greater'. (prio queue defaults to max heap)
  }
};

// Returns true if edge is unsuitable
inline bool unsuitable(const Edge &e, const int &starting_label) {
  // For now an edge is unsuitable iff it is a lower case edge and it  has the
  // same label as the initial uc edge
  // I'll leave this as a function in case we need to add anithing else here
  return e.type != 'u' && starting_label == e.C;
}

// for comodity we don't edit nodes int the heap, but keep track of wether we're
// done with a node or not, and thus w ecan have a node appear multiple times in
// the queue, I've done this multiple times with dijkstra, and the time
// difference is not significant enough to make it reasonable to implement a
// heap from scratch.
bool DCBackprop(int source, STNU *stnu, bool debug = false) {
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
  vector<char> type(stnu->N, 'o'); // default to ordinary edges
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

    cerr << endl;
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

bool morris2014(string filename, bool debug = false) {
  STNU *Graph = new STNU(filename, debug);

  debug &&cerr << "Done reading!" << endl;

  for (int i = 0; i < Graph->N; ++i) {
    for (auto &edge : Graph->InEdges[i]) {
      if (edge.value < 0) {
        Graph->is_negative_node[i] = true;
        break;
      }
    }
  }

  debug &&cerr << "Done finding negative nodes!" << endl;

  for (int i = 0; i < Graph->N; ++i) {
    if (Graph->is_negative_node[i]) {
      cerr << "Going to call DCBackprop from main!" << endl;
      if (!DCBackprop(i, Graph)) {
        debug &&cout << "Not DC!" << endl;
        return false;
      }
    }
  }

  debug &&cout << "DC!" << endl;
  return true;
}

int main(int argc, char *argv[]) {
  if (argc >= 3) {
    if (string(argv[1]) == "--verbose=true") {
      DEBUG = 1;

    } else if (string(argv[1]) != "--verbose=false") {
      cout << "Usage: ./morris2014  --verbose={true/false} testfile "
              "[testfiles..]"
           << endl;
      exit(1);
    }
  }

  for (int i = 2; i < argc; ++i) {
    cout << argv[i] << " Morris 2014: " << morris2014(string(argv[i]), DEBUG)
         << endl;
  }

  return 0;
}
