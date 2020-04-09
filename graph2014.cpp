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

/* Input: An Edge
 *  Effect: Saves an edge in the InEdges vector containing all edges
 *          or updates the edge if it already exists.
 */
void STNU::addEdge(const Edge &e) {
  //if an edge does not exist at TP e->B in the InEdge vector
  if (indexEdges[e->A][e->B]==-1) {
    //add edge at e->B
    indexEdges[e->A][e->B] = InEdges[e->B].size();
    InEdges[e->B].push_back(e);
  }
  //else, if it does exists
  else {
    //update the edge in InEdges
    InEdges[e->B][indexEdges[e->A][e->b]] = e;
  }

  DEBUG && (cerr << "Added edge " << e.A << ' ' << e.B << ' ' << e.value
                 << ' ' << e.type << endl);
}

///DONT NEED THIS PART ANYMORE
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
