#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <cassert>
#include <string>
#include <unordered_map>
#include <vector>
using std::string;
using std::unordered_map;
using std::vector;

struct Neighbour {
  int index; // the index of the neighbouring label -> one can get the edge from
             // the edge vector.
  char c;    // type of edge, o - ordinary, l - lower case, u - upper case

  Neighbour(int index = 0, char c = 0) {
    this->index = index;
    this->c = c;
  }
};

// I am using the following two structs to represent the edges
struct OrdEdge {
  int A, B; // labels
  int value;

  OrdEdge(int A = 0, int value = 0, int B = 0) {
    this->A = A;
    this->value = value;
    this->B = B;
  }
};

struct ContLinkEdge {
  int A, B; // labels
  int low, high;

  ContLinkEdge(int A = 0, int low = 0, int high = 0, int B = 0) {
    this->A = A;
    this->low = low;
    this->high = high;
    this->B = B;
  }
};

struct CaseEdge {
  int A, B, C; // labels and Case
  int value;

  // from, to, label, value
  CaseEdge(int A = 0, int B = 0, int C = 0, int value = 0) {
    this->A = A;
    this->C = C;
    this->value = value;
    this->B = B;
  }
}; // there will be a vector of lower case edges and a vector of upper case
   // edges

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

  // These  matrices are necessary to make sure we don't save duplicated edges
  // (that would fuck up the complexity - say if an edge gets updated n times,
  // we would otherwise have ncopies of it) I know this makes the code slightly
  // messier, but should be fine for now index to the edge in the vector of
  // edges or -1
  vector<vector<int>> ordEdges;
  vector<vector<int>> ucEdges;
  vector<vector<int>> lcEdges;

  // to get the allmax edges iterate first through the ordinary edges and then
  // through the uc edges, assigning high to each contlink
  vector<OrdEdge> ordEdgesList;
  vector<CaseEdge> ucEdgesList;
  vector<CaseEdge> lcEdgesList;

  // in these two vectors we save the edges we want to add to the graph.
  vector<OrdEdge> bufferOrdEdges;
  vector<CaseEdge> bufferUcEdges;

  // Lists of neighbours -> basically the matrices prof Hunsberger showed us.
  vector<vector<Neighbour>> ordNeighbours;
  vector<vector<Neighbour>> ucNeighbours;

  STNU(int n) {
    this->N = n;

    ordEdges = vector<vector<int>>(n, vector<int>(n, kNaN));
    ucEdges = vector<vector<int>>(n, vector<int>(n, kNaN));
    lcEdges = vector<vector<int>>(n, vector<int>(n, kNaN));

    ordNeighbours = vector<vector<Neighbour>>(n, vector<Neighbour>());
    ucNeighbours = vector<vector<Neighbour>>(n, vector<Neighbour>());
  }

  void addEdge(const OrdEdge &e);
  void addEdge(const ContLinkEdge &e);
  void addUpperCaseEdge(const CaseEdge &e);

  void updateAllBufferedEdges();
};

#endif // _GRAPH_H_
