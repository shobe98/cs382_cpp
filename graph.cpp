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
void STNU::addEdge(const OrdEdge &e) {
  bufferOrdEdges.push_back(e);

  DEBUG && (cerr << "Added ord edge " << e.A << ' ' << e.B << ' ' << e.value
                 << endl);
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
void STNU::addEdge(const ContLinkEdge &e) {
  // page 80/182 in notes
  bufferUcEdges.push_back(CaseEdge(e.B, e.A, e.B, -e.high));

  lcEdges[e.A][e.B] = lcEdgesList.size();
  lcEdgesList.push_back(CaseEdge(e.A, e.B, e.B, e.low));
}

/* Input: An upper case edge stored in a CaseEdge struct
 *  Effect: Saves the edge in the buffer vector, without modifying the graph.
 */
void STNU::addUpperCaseEdge(const CaseEdge &e) {
  bufferUcEdges.push_back(e);
  DEBUG && (cerr << "Added UC edge " << e.A << ' ' << e.B << ' ' << e.C << ':'
                 << e.value << endl);
}

/* Effect: Clears all the buffer edges while adding (or updating) the edges to
 * the graph.
 */
void STNU::updateAllBufferedEdges() {
  // TODO(somebody): refactor this duplicated code in a function call
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

  DEBUG && (cerr << "Done updating the graph!" << endl);
}
