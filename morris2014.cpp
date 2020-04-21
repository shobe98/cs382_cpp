#include <algorithm>
#include <cassert>
#include <fstream>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include "graph2014.h"
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

// STNU representation
//int N, M, K;

//vector<Edge> InEdges[kMaxLabels];
/*
unordered_map<string, int> labelsToNum;
vector<string> numsToLabel;

bool is_negative_node[kMaxLabels];
bool in_rec_stack[kMaxLabels];
bool done[kMaxLabels];
*/
bool unsuitable(const Edge &e, int u, int source, STNU *stnu) {
  // If the starting node was a special node (i.e. a node that has exactly one
  // ingoing UC edge)
  if (stnu->InEdges[source].size() == 1 && stnu->InEdges[source][0].type == 'u') {
    if (e.type == 'l') {
      // Unsuitable if same labels.
      return e.C == stnu->InEdges[source][0].C;
    }
  }
  // Any other edge is suitable. Yay!
  return false; // all edges are suitable for now
}

// for comodity we don't edit nodes int the heap, but keep track of wether we're
// done with a node or not, and thus w ecan have a node appear multiple times in
// the queue, I've done this multiple times with dijkstra, and the time
// difference is not significant enough to make it reasonable to implement a
// heap from scratch.
bool DCBackprop(int source, STNU *stnu) {
  if (stnu->in_rec_stack[source]) {
    return false;
  }
  if (stnu->done[source]) {
    return true;
  }

  DEBUG && (cerr << "Currently running DCBackprop from " << stnu->numsToLabel[source]
                 << endl);

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

    cerr << endl << "Expanding node " << stnu->numsToLabel[u] << endl;

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
      //
      // TODO(astanciu): The label removal rule only works if the label we
      // started with is different than u. Should be checked, righ? @Prof
      // Hunsberger ?
      stnu->addEdge(Edge(u, source, dist[u], 'o'));
      //stnu->InEdges[source].push_back(Edge(u, source, dist[u], 'o'));
      DEBUG && (cerr << "Added ord edge " << stnu->numsToLabel[u] << ' '
                     << stnu->numsToLabel[source] << ' ' << dist[u] << endl);
      continue;
    }

    if (stnu->is_negative_node[u]) {
      cerr << "about to enter recursion in " << stnu->numsToLabel[u] << endl;
      for (int i = 0; i < stnu->N; ++i) {
        if (dist[i] != kInf) {
          cerr << stnu->numsToLabel[i] << ":" << dist[i] << ' ';
        }
      }
      cerr << endl;

      if (DCBackprop(u,stnu) == false) {
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

      int v = edge.A;
      int new_dist = dist[u] + edge.value;
      if (new_dist < dist[v]) {
        dist[v] = new_dist;
        Q.push(NodeAndPrio(v, new_dist));
      }
    }
    for (int i = 0; i < stnu->N; ++i) {
      if (dist[i] != kInf) {
        cerr << stnu->numsToLabel[i] << ":" << dist[i] << ' ';
      }
    }
    cerr << endl;

    cerr << endl;
  }

  DEBUG &&cerr << stnu->numsToLabel[source] << "'s propagation done" << endl;
  for (int i = 0; i < stnu->N; ++i) {
    if (dist[i] != kInf) {
      DEBUG &&cerr << stnu->numsToLabel[i] << ":" << dist[i] << ' ';
    }
  }
  DEBUG &&cerr << endl;

  stnu->done[source] = true;
  stnu->in_rec_stack[source] = false;

  // Andrei's addition to the algorithm.
  // It is mentioned in the paper but nor present in the pseudocode.
  if (dist[source] < 0) {
    return false;
  }
  return true;
}

// TODO(anybody): According to section 3 (right before 3.1) the STNU should be
// in normal form.
// TODO(anybody): We should also have a function that transforms the graph.

// Abi's code
STNU *parse() {
  cerr << "PARSING!!" << endl;
  string str;
  getline(cin, str);
  // typenet reads in type of network
  string typenet;
  cin >> typenet;
  getline(cin, str);
  getline(cin, str);
  
  // get n and instantiate STNU *G
  int n;
  cin >> n;
  STNU *G = new STNU(n);
  getline(cin, str);
  getline(cin, str);
  
  //Get M and K and save to STNU graph G
  cin >> G->M;
  getline(cin, str);
  getline(cin, str);
  cin >> G->K;
  getline(cin, str);
  getline(cin, str);

  cerr << "Read some stuff!!" << endl;
  // read in TPs from file
  for (int i = 0; i < G->N; i++) {
    string TP;
    cin >> TP;
    // pushback TP name onto numsToLabel
    G->numsToLabel.push_back(TP);
    // save index of TP in labelsToNum
    G->labelsToNum[TP] = i;
  }
  getline(cin, str);
  getline(cin, str);

  for (int i = 0; i < G->M; i++) {
    int A, B, value;
    string edge1, edge2;
    // reads in string names for edges and value
    cin >> edge1 >> value >> edge2;
    // finds index for each edge and saves to o
    A = G->labelsToNum.find(edge1)->second;
    B = G->labelsToNum.find(edge2)->second;
    
    // adds incoming ord. edge to STNU graph
    G->addEdge(Edge(A,B,value,'o'));

    getline(cin, str);
  }
  getline(cin, str);

  // reads in Cont. Link Edges
  for (int i = 0; i < G->K; i++) {
    int A, B, low, high;
    string edge1, edge2;
    cin >> edge1 >> low >> high >> edge2;
    A = G->labelsToNum.find(edge1)->second;
    B = G->labelsToNum.find(edge2)->second;

    //adds incoming cont. link edges to graph
    //todo : andrei add cont. link edge
    G->addEdge(Edge(B, A, -high, 'u', B));
    G->addEdge(Edge(A, B, low, 'l', B));

    // TODO(andrei): Implement the transformation.
    // As soon as we implement the transformation of the graph this will have to
    // go somewhere else. Basically, since for every negative node we either
    // have one negative UC edge -> where we need to keep track of its label or
    // only oridnary edges, some negative -> where we don't have any label. This
    // map keeps track of those nodes that have a label. Maps node index to UC
    // label (also an index).

    getline(cin, str);
  }
  return G;
}

int main(int argc, char *argv[]) {
  if (argc == 2) {
    if (string(argv[1]) == "--verbose") {
      DEBUG = 1;
    } else {
      cout << "Usage: ./morris2014  [--verbose]" << endl;
      exit(1);
    }
  }

  STNU *Graph = parse();

  cerr << "Done reading!" << endl;

  for (int i = 0; i < Graph->N; ++i) {
    for (auto &edge : Graph->InEdges[i]) {
      if (edge.value < 0) {
        Graph->is_negative_node[i] = true;
        break;
      }
    }
  }

  cerr << "Done finding negative nodes!" << endl;

  for (int i = 0; i < Graph->N; ++i) {
    if (Graph->is_negative_node[i]) {
      if (!DCBackprop(i,Graph)) {
        cout << "Not DC!" << endl;
        return 0;
      }
    }
  }

  cout << "DC!" << endl;

  return 0;
}
