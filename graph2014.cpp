/* -----------------------------
 * CMPU-382, Spring 2020
 * Authors:  Andrei Stanciu, Abigail Ren
 * File:     graph2014.cpp
 * -----------------------------
 * Implements STNU class methods in graph2014.h
 */

#include "graph2014.h"

#include <vector>
using std::cerr;
using std::endl;
using std::max;
using std::min;
using std::vector;
// Control flag to turn debugging on or off. could be passed as a user parameter
// eventually.

/*  Contract: addEdge(e)
 *  Input: An Edge
 *  Effect: Saves an edge in the InEdges vector containing all edges
 *          or updates the edge if it already exists.
 */
void Morris2014::STNU::addEdge(const Edge &e) {
  assert(e.A < indexEdges.size());
  assert(e.B < indexEdges[e.A].size());
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
  if (debug) {
    cerr << "Added edge: ";
    printEdge(e);
    cerr << endl;
  }
}

/* Contract: printEdge(e)
 * Input:   e, an Edge
 * prints edge
 */
void Morris2014::STNU::printEdge(const Edge &e) {
  // If this method is called, it always prints, regardless of debug flag
  // check of the debug flag should be done before calling this function
  cerr << numsToLabel[e.A] << ' ' << numsToLabel[e.B] << ' ' << e.value << ' '
       << e.type;
}

/* Contract: addContLink(label1, low, high, label2, cont_link_index)
 * Input:    label1, label2 are labels of timepoints
             low, high are values of cont link edges
             cont_link_index is the index of the cont linke edge in the graph
 * adds ContLink Edge to graph
 */
void Morris2014::STNU::addContLink(string &label1, int low, int high,
                                   string &label2, int cont_link_index) {
  int A = labelsToNum.find(label1)->second;
  int B = labelsToNum.find(label2)->second;

  // First transform the edge into "normal" form. I.e. A =(lo, hi)=> B becomes
  // A <--lo--> AN =(0, hi-lo)=> B. Thus, add the ordinary edge.

  int AN;
  string AN_label = label1 + to_string(cont_link_index);
  AN = labelsToNum[AN_label] = numsToLabel.size();
  numsToLabel.push_back(AN_label);

  debug &&cerr << "We have the labels!" << endl;

  addEdge(Edge(A, AN, low, 'o'));
  addEdge(Edge(AN, A, -low, 'o'));

  debug &&cerr << "Added ordinary edges!" << endl;

  // - (high - low) = low - high
  addEdge(Edge(B, AN, low - high, 'u', B));
  addEdge(Edge(AN, B, 0, 'l', B));

  debug &&cerr << "Added normalized contingent link!" << endl;
}
