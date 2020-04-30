#ifndef _GRAPH2014_H_
#define _GRAPH2014_H_

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using std::string;
using std::to_string;
using std::unordered_map;
using std::vector;
using namespace std;
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

  Edge(int A = 0, int B = 0, int value = 0, char type = 0, int C = 0) {
    this->A = A;
    this->B = B;
    this->value = value;
    this->type = type;
    this->C = C;

    // if ord. edge, C=0
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

  // True if debugging printfs are enabled
  bool debug;

  // These  matrices are necessary to make sure we don't save duplicated edges
  // saves the index to the edge in the vector of edges or -1
  vector<vector<int>> indexEdges;

  // keeps track of the incoming edges in the graph
  // at InEdge[B], the edge saved is the edge going from A to B
  vector<vector<Edge>> InEdges;

  // Lists of neighbours -> basically the matrices prof Hunsberger showed us.

  STNU(int n, int m, int k) {}

  STNU(string filename, bool _debug = false) {
    this->debug = debug;
    ifstream fin(filename);

    debug &&cerr << "PARSING!!" << endl;
    string str;
    getline(fin, str);

    // typenet reads in type of network
    string typenet;
    fin >> typenet;

    getline(fin, str);
    getline(fin, str);

    // get n and instantiate STNU *G
    int n, m, k;
    fin >> n;
    getline(fin, str);
    getline(fin, str);

    // Get M and K and save to STNU graph G
    fin >> m;
    getline(fin, str);
    getline(fin, str);
    fin >> k;

    // Initialize constructor as we used to
    this->N = n + k;
    this->K = k;
    this->M = m;

    InEdges = vector<vector<Edge>>(N, vector<Edge>());
    indexEdges = vector<vector<int>>(N, vector<int>(N, kNaN));

    getline(fin, str);
    getline(fin, str);

    cout << n << ' ' << m << ' ' << k;

    // read in TPs from file
    for (int i = 0; i < n; i++) {
      string TP;
      fin >> TP;
      // pushback TP name onto numsToLabel
      this->numsToLabel.push_back(TP);
      // save index of TP in labelsToNum
      this->labelsToNum[TP] = i;
    }
    getline(fin, str);
    getline(fin, str);

    for (int i = 0; i < this->M; i++) {
      int A, B, value;
      string label1, label2;
      // reads in string names for edges and value
      fin >> label1 >> value >> label2;
      // finds index for each edge and saves to o
      A = this->labelsToNum.find(label1)->second;
      B = this->labelsToNum.find(label2)->second;

      // adds incoming ord. edge to STNU graph
      this->addEdge(Edge(A, B, value, 'o'));

      getline(fin, str);
    }
    getline(fin, str);

    // reads in Cont. Link Edges
    for (int i = 0; i < this->K; i++) {
      int low, high;
      string label1, label2;
      fin >> label1 >> low >> high >> label2;

      // adds incoming cont. link edges to graph
      this->addContLink(label1, low, high, label2, i);
      getline(fin, str);
    }
  }

  void addEdge(const Edge &e);

  // Adds a contingent link to the graph. It takes care of the normalisation
  // transformation. It adds a new node to the graph for each cont-link added.
  void addContLink(string &label1, int low, int high, string &label2,
                   int cont_link_index);

  void printEdge(const Edge &e);
};

#endif // _GRAPH2014_H_:
