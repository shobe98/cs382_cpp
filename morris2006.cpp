#include <vector>
#include <algorithm>
#include <queue>
using namespace std;

const int kMaxLabels = 1000;
const int kNaN = -1; // -1 for now
const int kInf = 0x3f3f3f3f;

struct NodeAndPrio {
    int node;
    int prio;

    NodeAndPrio(int node = 0, int prio = 0) {
        this->node = node;
        this->prio = prio;
    }

    bool operator < (const NodeAndPrio &other) {
        return prio > other.prio; // we want a min heap, thus the sign here needs to be 'greater'. (prio queue defaults to max heap)
    }
};

bool DCBackprop(int source) {
    if(in_rec_stack[source]) {
        return false;
    }
    if(done[source]) {
        return true;
    }
    in_rec_stack[source] = true;

    vector<int> dist(kMaxLabels, kInf);

    dist[source] = 0;

    priority_queue<NodeAndPrio> Q;

}
