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
  //if an edge does not exist at TP e->B in the InEdge vector
  if (indexEdges[e.A][e.B]==-1) {
    //add edge at e->B
    indexEdges[e.A][e.B] = InEdges[e.B].size();
    InEdges[e.B].push_back(e);
  }
  //else, if it does exists
  else {
    //update the edge in InEdges
    InEdges[e.B][indexEdges[e.A][e.B]] = e;
  }

  DEBUG && (cerr << "Added edge " << e.A << ' ' << e.B << ' ' << e.value
                 << ' ' << e.type << endl);
}

