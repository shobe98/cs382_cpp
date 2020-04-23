#include "graph2014.h"

#include <vector>
using std::cerr;
using std::endl;
using std::max;
using std::min;
using std::vector;
// Control flag to turn debugging on or off. could be passed as a user parameter
// eventually.
#define DEBUG 1

/*  Input: An Edge
 *  Effect: Saves an edge in the InEdges vector containing all edges
 *          or updates the edge if it already exists.
 */
void STNU::addEdge(const Edge &e) {
  // if an edge does not exist at TP e->B in the InEdge vector
  if (indexEdges[e.A][e.B] == -1) {
    // add edge at e->B
    indexEdges[e.A][e.B] = InEdges[e.B].size();
    InEdges[e.B].push_back(e);
  }
  // else, if it does exists
  else {
    // update the edge in InEdges
    InEdges[e.B][indexEdges[e.A][e.B]] = e;
  }
  cerr << "Added edge: ";
  printEdge(e);
  cerr << endl;
}

void STNU::printEdge(const Edge &e) {
  DEBUG && (cerr << numsToLabel[e.A] << ' ' << numsToLabel[e.B] << ' '
                 << e.value << ' ' << e.type);
}

void STNU::addContLink(string &label1, int low, int high, string &label2,
                       int cont_link_index) {
  int A = labelsToNum.find(label1)->second;
  int B = labelsToNum.find(label2)->second;

  // First transform the edge into "normal" form. I.e. A =(lo, hi)=> B becomes
  // A <--lo--> AN =(0, hi-lo)=> B. Thus, add the ordinary edge.

  int AN;
  string AN_label = label1 + to_string(cont_link_index);
  AN = labelsToNum[AN_label] = numsToLabel.size();
  numsToLabel.push_back(AN_label);

  cerr << "We have the labels!" << endl;

  addEdge(Edge(A, AN, low, 'o'));
  addEdge(Edge(AN, A, -low, 'o'));

  cerr << "Added ordinary edges!" << endl;

  // - (high - low) = low - high
  addEdge(Edge(B, AN, low - high, 'u', B));
  addEdge(Edge(AN, B, 0, 'l', B));

  cerr << "Added normalized contingent link!" << endl;
}
