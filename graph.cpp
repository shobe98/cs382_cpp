/* --------------------------------
 * CMPU-382, Spring 2020
 * Authors:  Andrei Stanciu, Abigail Ren
 * File:     graph.cpp
 * --------------------------------
 * Implements STNU class methods defined in graph.h
 */

#include "graph.h"

#include <vector>
using std::cerr;
using std::endl;
using std::max;
using std::min;
using std::vector;
// Control flag to turn debugging on or off. could be passed as a user parameter
// eventually.
#define DEBUG 1

/* Input: An OrdEdge
 *  Effect: Saves an ordinary edge in a buffer vector, without modifying the
 * graph.
 */
void Morris2006::STNU::addEdge(const OrdEdge &e) {
  bufferOrdEdges.push_back(e);

  debug && (cerr << "Added ord edge " << numsToLabel[e.A] << ' '
                 << numsToLabel[e.B] << ' ' << e.value << endl);
}

/* Input: A ContLinkEdge
 *  Effect: Saves a contlink edge as an upper case edge (to a buffer of uc
 * edges) and a lower case edge that gets added straight to the graph (list of
 * lc edges).
 * This is function is a mess right now due to refactoring the code while trying
 * to minimize changes to external functionality. Basically, new lc edges never
 * get added to the graph and old ones never get updated, so to reduce memory
 * waste, I'm adding them straight to the graph, bypassing the buffer.
 * Also, this function is only used when reading data. The one that adds an
 * ordinary edge and the one that adds an uc edge are necessary for updating the
 * graph after each iteration of the main algo.
 */
void Morris2006::STNU::addEdge(const ContLinkEdge &e) {
  // page 80/182 in notes
  bufferUcEdges.push_back(CaseEdge(e.B, e.A, e.B, -e.high));

  lcEdges[e.A][e.B] = lcEdgesList.size();
  lcEdgesList.push_back(CaseEdge(e.A, e.B, e.B, e.low));
}

/* Input: An upper case edge stored in a CaseEdge struct
 *  Effect: Saves the edge in the buffer vector, without modifying the graph.
 */
void Morris2006::STNU::addUpperCaseEdge(const CaseEdge &e) {
  bufferUcEdges.push_back(e);
  debug && (cerr << "Added UC edge " << numsToLabel[e.A] << " to "
                 << numsToLabel[e.B] << " with label " << numsToLabel[e.C]
                 << ':' << e.value << endl);
}

/* Effect: Clears all the buffer edges while adding (or updating) the edges to
 * the graph.
 */
void Morris2006::STNU::updateAllBufferedEdges() {
  for (auto &e : bufferOrdEdges) {
    // if edge wasn't present add it to the list and keep track of the index in
    // the matrix
    if (ordEdges[e.A][e.B] == kNaN) {
      ordEdges[e.A][e.B] = ordEdgesList.size();
      ordNeighbours[e.A].push_back(
          Neighbour(ordEdgesList.size(), 'o')); // ordinary edge

      ordEdgesList.push_back(e);
    }
    // if edge is present keep the smaller value for it
    else {
      ordEdgesList[ordEdges[e.A][e.B]].value =
          min(ordEdgesList[ordEdges[e.A][e.B]].value, e.value);
    }
  }

  for (auto &e : bufferUcEdges) {
    // if edge wasn't present add it to the list and keep track of the index in
    // the matrix
    if (ucEdges[e.A][e.B] == kNaN) {
      ucEdges[e.A][e.B] = ucEdgesList.size();
      ucNeighbours[e.A].push_back(Neighbour(ucEdgesList.size(), 'u'));

      ucEdgesList.push_back(e);
    }
    // if edge is present keep the smaller value for it
    else {
      assert(ucEdgesList[ucEdges[e.A][e.B]].C ==
             e.C); // If this ever fails it means that we need to support
                   // multiple distinct uc edges between two nodes.
      // TODO(!!!!!!!)
      ucEdgesList[ucEdges[e.A][e.B]].value =
          min(ucEdgesList[ucEdges[e.A][e.B]].value, e.value);
    }
  }

  bufferOrdEdges.clear();
  bufferUcEdges.clear();

  debug && (cerr << "Done updating the graph!" << endl);
}

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
