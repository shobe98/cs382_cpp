#include "graph.h"

#include<vector>
using std::vector;
using std::min;
using std::max;

void STNU::addEdge(const OrdEdge& e) {
    lazyOrdEdges.push_back(e);
}

void STNU::addEdge(const ContLinkEdge& e) {
    // page 80/182 in notes 
    lazyUcEdges.push_back(CaseEdge(e.B, e.A, e.B, -e.high));
    lazyLcEdges.push_back(CaseEdge(e.A, e.B, e.B, e.low));
}


void STNU::updateAllLazyEdges() {
    // TODO(somebody): refactor this duplicated code in a function call
    for(auto &e : lazyOrdEdges) {
        // if edge wasn't present add it to the list and keep track of the index in the matrix
        if(ordEdges[e.A][e.B] == kNaN) {
            ordEdges[e.A][e.B] = ordEdgesList.size();
            ordEdgesList.push_back(e);
            
            ordNeighbours[e.A].push_back(Neighbour(e.B, 'o')); // ordinary edge 
        }
        // if edge is present keep the smaller value for it
        else {
            ordEdgesList[ordEdges[e.A][e.B]].value = min(ordEdgesList[ordEdges[e.A][e.B]].value, e.value);
        }
    }

    for(auto &e : lazyUcEdges) {
        // if edge wasn't present add it to the list and keep track of the index in the matrix
        if(ucEdges[e.A][e.B] == kNaN) {
            ucEdges[e.A][e.B] = ucEdgesList.size();
            ucEdgesList.push_back(e);

            ucNeighbours[e.A].push_back(Neighbour(e.B, 'u'));
        }
        // if edge is present keep the smaller value for it
        else {
            assert(ucEdgesList[ucEdges[e.A][e.B]].C == e.C); // If this ever fails it means that we need to support multiple distinct uc edges between two nodes. 
            // TODO(!!!!!!!)
            ucEdgesList[ucEdges[e.A][e.B]].value = min(ucEdgesList[ucEdges[e.A][e.B]].value, e.value);
        }
    }


    for(auto &e : lazyLcEdges) {
        // if edge wasn't present add it to the list and keep track of the index in the matrix
        if(lcEdges[e.A][e.B] == kNaN) {
            lcEdges[e.A][e.B] = lcEdgesList.size();
            lcEdgesList.push_back(e);
            
            lcNeighbours[e.A].push_back(Neighbour(e.B, 'l'));
        }
        // if edge is present keep the *larger* value for it
        else {
            lcEdgesList[lcEdges[e.A][e.B]].value = max(lcEdgesList[lcEdges[e.A][e.B]].value, e.value);
        }
    }

    lazyOrdEdges.clear();
    lazyUcEdges.clear();
    lazyLcEdges.clear();

}


