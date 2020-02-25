#include "graph.cpp"
#include <vector>
#include <queue> // priority queue
using namespace std;

vector<int> bellman_ford(STNU *stnu) {
    vector<int> distance(stnu->N, 0); // this will be the potential function as well

    // for best performance of bellman this can be optimized by implementing the queue version
    for(int i = 1; i < stnu->N; ++i) {
        for(auto &edge : stnu->ordEdgesList) {
            distance[edge.B] = min(distance[edge.B], distance[edge.A] + edge.value);
        }
        for(auto &edge : stnu->ucEdgesList) {
            distance[edge.B] = min(distance[edge.B], distance[edge.A] + edge.value);
        }
    }

    // check for negative cycles
    for(auto &edge : stnu->ordEdgesList) {
        if(distance[edge.B] > distance[edge.A] + edge.value) {
            stnu->has_negative_cycle = true;
        }
    }
    for(auto &edge : stnu->ucEdgesList) {
        if(distance[edge.B] > distance[edge.A] + edge.value) {
            stnu->has_negative_cycle = true;
        }
    }

    return distance;
}

// side effect: modifies the graph by adding new edges
bool is_dinamically_controllable(STNU *stnu) {
    for(int rep = 1; rep <= stnu->K; ++rep) { 
        vector<int> f = bellman_ford(stnu); // potential function

        if(stnu->has_negative_cycle) {
            return false;
        }

        for(auto &lc_edge : stnu->lcEdgesList) {
            
            // run dijkstra with the potential function
        }
    }

    // if no negative cycle is found
    return true;
}

