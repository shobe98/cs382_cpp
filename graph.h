/* --------------------------------
 *  CMPU-382, Spring 2020
 *  Authors:  Andrei Stanciu, Abigail Ren
 *  File:     graph.h
 * --------------------------------
 *  Implements STNU class
 *  Defines Structs: Neighbour, OrdEdge, ContLinkEdge, CaseEdge
 *  Abstract Functions: addEdge(OrdEdge), addEdge(ContLinkEdge),
 *                      addUpperCaseEdge(CaseEdge), updateAllBufferedEdges()
 */

#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
using std::string;
using std::unordered_map;
using std::vector;
using namespace std;

namespace Morris2006 {
struct Neighbour {
  int index; // the index of the neighbouring label -> one can get the edge from
  // the edge vector.
  char c; // type of edge, o - ordinary, l - lower case, u - upper case

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

  // True if debugging printfs are enabled;
  bool debug;

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
  /*
      Constructor: STNU(filename, debug)
      Parses information from filename and saves directly to class variables
  */
  STNU(string filename, bool _debug = false) {

    this->debug = _debug;
    ifstream fin(filename);

    debug &&cerr << "PARSING!!" << endl;
    string str;
    getline(fin, str);

    // typenet reads in type of network
    string typenet;
    fin >> typenet;

    getline(fin, str);
    getline(fin, str);

    // get n
    fin >> this->N;

    ordEdges = vector<vector<int>>(this->N, vector<int>(this->N, kNaN));
    ucEdges = vector<vector<int>>(this->N, vector<int>(this->N, kNaN));
    lcEdges = vector<vector<int>>(this->N, vector<int>(this->N, kNaN));

    ordNeighbours = vector<vector<Neighbour>>(this->N, vector<Neighbour>());
    ucNeighbours = vector<vector<Neighbour>>(this->N, vector<Neighbour>());

    getline(fin, str);
    getline(fin, str);

    // Get M and K and save to STNU graph
    fin >> this->M;

    getline(fin, str);
    getline(fin, str);

    fin >> this->K;

    getline(fin, str);
    getline(fin, str);

    // read in TPs from file
    for (int i = 0; i < this->N; i++) {
      string TP;
      fin >> TP;
      // pushback TP name onto numsToLabel
      this->numsToLabel.push_back(TP);
      // save index of TP in labelsToNum
      this->labelsToNum[TP] = i;
    }

    getline(fin, str);
    getline(fin, str);

    // reads in Ord. Edges

    for (int i = 0; i < this->M; i++) {
      OrdEdge o;
      string edge1, edge2;
      // reads in string names for edges and value
      fin >> edge1 >> o.value >> edge2;
      // finds index for each edge and saves to o

      o.A = this->labelsToNum.find(edge1)->second;
      o.B = this->labelsToNum.find(edge2)->second;
      this->addEdge(o);
      getline(fin, str);
    }
    getline(fin, str);

    // reads in Cont. Link Edges
    for (int i = 0; i < this->K; i++) {
      ContLinkEdge c;
      string edge1, edge2;
      fin >> edge1 >> c.low >> c.high >> edge2;
      c.A = this->labelsToNum.find(edge1)->second;
      c.B = this->labelsToNum.find(edge2)->second;
      this->addEdge(c);
      getline(fin, str);
    }

    // updates buffered edges and adds them to STNU graph
    this->updateAllBufferedEdges();
  }

  void addEdge(const OrdEdge &e);
  void addEdge(const ContLinkEdge &e);
  void addUpperCaseEdge(const CaseEdge &e);

  void updateAllBufferedEdges();
};
} // namespace Morris2006
#endif // _GRAPH_H_
