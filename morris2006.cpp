#include <vector>
#include <algorithm>
#include <queue>
#include <cassert>
using namespace std;

const int kMaxLabels = 1000;
const int kNaN = -1; // -1 for now
const int kInf = 0x3f3f3f3f;

struct NodeAndPrio {
    int node;
    /// we very likeley need labels 
    int prio;

    NodeAndPrio(int node = 0, int prio = 0) {
        this->node = node;
        this->prio = prio;
    }

    bool operator < (const NodeAndPrio &other) {
        return prio > other.prio; // we want a min heap, thus the sign here needs to be 'greater'. (prio queue defaults to max heap)
    }
};

struct Edge {
    int A, B, value;
    int C; // optional, if edge 
    char type; // 'o', 'u', 'l'

    Edge(int A = 0, int B = 0, int value = 0, char type = 0, int C = 0) {
        this->A = A;
        this->B = B;
        this->value = value;
        this->type = type;
        this->C = C;
        if(this->type == 'o') {
            assert(this->C == 0);
        }
    };

    // Edge(0, 1, 100, 'o') 
    // or
    // Edge(0, 1, 100, 'l', 1) // lower case edge towards 1, the label on it is 1 
};


bool unsuitable(const Edge& e, int u, int source) {
    //idk what unsuitable edge means 
    return false; // all edges are suitable for now
}

vector<Edge> InEdges[kMaxLabels];

// for comodity we don't edit nodes int the heap, but keep track of wether we're done with a node or not, and thus w ecan have a node appear multiple times in the queue,
// I've done this multiple times with dijkstra, and the time difference is not significant enough to make it reasonable to implement a heap from scratch.
bool DCBackprop(int source) {
    if(in_rec_stack[source]) {
        return false;
    }
    if(done[source]) {
        return true;
    }
    in_rec_stack[source] = true;

    // change with N
    vector<int> dist(kMaxLabels, kInf);
    vector<bool> done_dijkstra(kMaxLabels, false);

    dist[source] = 0;

    priority_queue<NodeAndPrio> Q;

    for (auto &edge : InEdges[source]) {
        dist[n1] = edge.value; // maybe labels

        Q.push(NodeAndPrio(n1, dist[n1]));
    }
    done_dijkstra[source] = true;

    while(!Q.empty()) {
        // popping
        int u = Q.top().node;
        Q.pop();

        if(done_dijkstra[u]) {
            continue; // in case u was in the queue multiple times and we already processed it.
        }
        done_dijkstra[u] = true;
      
        //done popping

        if(dist[u] >= 0) {
            // add new edge u->source
            
            // some bad memory management here. TODO(astanciu): change to a mtarix that keeps track of edges
            // actually, it's probably not needed, it's just a constant optimisation but it doesn't change the complexity.
            // at most N^2 new edges are added (N per negative node), so the final complexity is still N^3 (or N^3log(N) with binary heap)
            InEdges[source].push_back(Edge(u, source, dist[u], 'o'))
`           continue;
        }

        if(is_negative_node[u]) {
            if(DCBackprop(u) == false) {
                return false;
            }
        }

        for(auto &edge : InEdges[u]) {
            if(edge.value < 0) {
                continue;
            }


            if(unsuitable(edge, u, source)) {
                continue;
            }

            int v = edge.A;
            int new_dist = dist[u] + edge.value;
            if (new_dist < dist[v]) {
                dist[v] = new_dist;
                Q.push(NodeAndPrio(v, new_dist));
            }
        }
    }
    

    return true;
}
