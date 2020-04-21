#ifndef _GRAPH2014_H_
#define _GRAPH2014_H_

#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using std::string;
using std::unordered_map;
using std::vector;


/*
 * Edge Struct
 * Member Variables: int A: start TP,
 *                   int B: end TP,
 *                   int value: value of edge,
 *                   int C: label of cont. link edge
 *                   char type: type of edge
 */
struct Edge {
  int A, B, value;
  int C;
  char type;

  Edge(int A=0, int B =0, int value = 0, char type = 0, int C = 0) {
    this->A = A;
    this->B = B;
    this->value = value;
    this->type = type;
    this->C = C;

    //if ord. edge, C=0
    if (this->type == 'o') {
      assert(this->C == 0);
    }
  };
};


class STNU {
public:
  // since the algorithm is O(n^4) running on more than 1000 would crash the
  // computer, so for now let the max number of labels be 1000
  static const int kMaxLabels = 1000;

  // -INF, a value we use to code missing values, edges etc
  static const int kNaN = -1; // -1 for now

  // Labels, Ord Edges, Cont Links
  int N, M, K;

  bool has_negative_cycle = false;
  
  // mappings between labels and ints and back
  unordered_map<string, int> labelsToNum;
  vector<string> numsToLabel;

  bool is_negative_node[kMaxLabels];
  bool in_rec_stack[kMaxLabels];
  bool done[kMaxLabels];
  
  // These  matrices are necessary to make sure we don't save duplicated edges
  // saves the index to the edge in the vector of edges or -1
  vector<vector<int>> indexEdges;

  // keeps track of the incoming edges in the graph
  // at InEdge[B], the edge saved is the edge going from A to B
  vector<vector<Edge>> InEdges;

  // Lists of neighbours -> basically the matrices prof Hunsberger showed us.

  STNU(int n) {
    this->N = n;
    
    InEdges = vector<vector<Edge>>(n, vector<Edge>());
    indexEdges = vector<vector<int>>(n, vector<int>(n, kNaN));

  }

  void addEdge(const Edge &e);

};

#endif // _GRAPH2014_H_:
