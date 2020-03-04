#include "algos.h" 
#include <vector>
#include <algorithm>
#include <queue> // priority queue
#include <cassert>
#include <unordered_map>
#include <iostream>
#include <fstream>
using namespace std;
using std::vector;
using std::min;


struct NodeAndPrio {
    int node;
    /// we very likeley need labels 
    int prio;

    NodeAndPrio(int node = 0, int prio = 0) {
        this->node = node;
        this->prio = prio;
    }

    bool operator < (const NodeAndPrio &other) const {
        return prio > other.prio; // we want a min heap, thus the sign here needs to be 'greater'. (prio queue defaults to max heap)
    }
};

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

void dijkstra(vector<int> f, STNU *stnu){
    vector<bool> dijkstra_done(stnu->N,false);
    vector<bool> dijkstra_queue(stnu->N,false);
    for (int j = 0; j < stnu->K; j++){
        CaseEdge C = stnu->lcEdgesList[j];

        //the priority queue takes in node X and priority (key)
        priority_queue<NodeAndPrio> q;
        q.push(NodeAndPrio(C.B,0));
        while (!q.empty()) {
            int TPnode = q.top().node;
            int TPval = q.top().prio;
            //NodeAndPrio TP = q.pop();
            //if dijkstra_done[TP.node] = true, TP was processed previously,
            //so we pop off the node and continue without doing anything to it 
            if (dijkstra_done[TPnode]){
                q.pop();
                continue;
            }
            else{
                dijkstra_done[TPnode] = true;
                int rpl = TPval + f[TPnode] - f[C.B];
                if (rpl<0){
                    stnu->addEdge(OrdEdge(C.A, C.value+rpl, TPnode));
                }
                else {
                    //for each ordinary successor edge
                    for (auto &succ : stnu->lcNeighbours[C.B]){
                        if (succ.c = "o"){
                            //get OrdEdge o from the list of ordinary edges
                            OrdEdge o = stnu->ordEdgesList[succ.index];
                            int nn_delta = f[TPnode] + o.value + f[o.B];
                            if (!dijkstra_queue[o.B] || TPval + nn_delta < o.value){
                                //decrease the key value of osucc-->
                                //add it to the queue?
                                q.push(NodeAndPrio(o.B, TPval + nn_delta));
                                dijkstra_queue[o.B] = true;
                            }
                        }
                        else if (succ.c = "u"){
                            CaseEdge u = stnu->ucEdgesList[succ.index];
                            int nn_delta = f[TPnode] + u.value - f[u.B];
                            int nn_path = TPval + nn_delta;
                            int rpl = nn_path + f[u.B] - f[C.B];

                            int min = stnu->lcEdges[u.B][u.A];
                            if (rpl >= (min*-1)){
                                if (!dijkstra_queue[u.B] || nn_path < u.value){
                                    q.push(NodeAndPrio(u.B, nn_path));
                                    dijkstra_queue[u.B]=true;
                                }
                                else {
                                    stnu->addEdge(CaseEdge(C.A, C.value+rpl, u.B));
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    //add all edges to the graph
    stnu->updateAllLazyEdges();
}


// side effect: modifies the graph by adding new edges
bool is_dinamically_controllable(STNU *stnu) {
    for(int rep = 1; rep <= stnu->K; ++rep) { 
        vector<int> f = bellman_ford(stnu); // potential function

        if(stnu->has_negative_cycle) {
            return false;
        }

        //for(auto &lc_edge : stnu->lcEdgesList) {
        dijkstra(f, stnu);
        // run dijkstra with the potential function
        //}
    }

    // if no negative cycle is found
    return true;
}
