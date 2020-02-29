#include <vector>
#include <algorithm>
#include <queue>
#include <cassert>
#include <unordered_map>
#include <iostream>
#include <fstream>
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

    bool operator < (const NodeAndPrio &other) const {
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


// STNU representation
int N, M, K;

vector<Edge> InEdges[kMaxLabels];

unordered_map<string, int> labelsToNum;
vector<string> numsToLabel;

bool is_negative_node[kMaxLabels];
bool in_rec_stack[kMaxLabels];
bool done[kMaxLabels];

// for comodity we don't edit nodes int the heap, but keep track of wether we're done with a node or not, and thus w ecan have a node appear multiple times in the queue,
// I've done this multiple times with dijkstra, and the time difference is not significant enough to make it reasonable to implement a heap from scratch.
bool DCBackprop(int source) {
    if(in_rec_stack[source]) {
        return false;
    }
    if(done[source]) {
        return true;
    }
    
    cerr << "Negative node: " << source << endl;

    in_rec_stack[source] = true;

    // change with N
    vector<int> dist(kMaxLabels, kInf);
    vector<bool> done_dijkstra(kMaxLabels, false);

    dist[source] = 0;

    priority_queue<NodeAndPrio> Q;

    for (auto &edge : InEdges[source]) {
        dist[edge.A] = edge.value; // maybe labels

        Q.push(NodeAndPrio(edge.A, dist[edge.A]));
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
            InEdges[source].push_back(Edge(u, source, dist[u], 'o'));
            cerr << "Added edge " << u << "->" << source << ' ' << dist[u] << endl;
            continue;
        }

        if(is_negative_node[u]) {
            cerr << "about to enter recursion in " << u << endl;
            for(int x = 0; x < N; ++x) {
                cerr << dist[x] << " ";
            }
            cerr << endl;

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
    
    cerr << source << "'s propagation done" << endl;
    for(int x = 0; x < N; ++x) {
        cerr << dist[x] << " ";
    }
    cerr << endl;

    done[source] = true;
    in_rec_stack[source] = false;

    // Andrei's addition to the algorithm. Comment for original behavior. 
    if(dist[source] < 0) {
        return false;
    }
    return true;
}

// Abi's code
void parse() {
    string str;
    getline(cin,str); 
    //typenet reads in type of network
    string typenet;
    cin >> typenet;
    cout << typenet << endl;
    getline(cin,str); 
    getline(cin,str); 
    //get N, M, and K
    cin >> N;
    cout << N << endl;
    getline(cin,str); 
    getline(cin,str); 
    cin >> M;
    cout << M << endl;
    getline(cin,str); 
    getline(cin,str); 
    cin >> K;
    cout << K << endl;
    getline(cin,str); 
    getline(cin,str); 
    //read in TPs from file
    for (int i=0;i<N;i++){
        string TP;
        cin >> TP;
        //pushback TP name onto numsToLabel
        numsToLabel.push_back(TP);
        //save index of TP in labelsToNum
        labelsToNum[TP] = i;
        cout << TP << endl;
    }
    for (string t : numsToLabel)
        cout << "TPS: " << t << endl;
    for (auto x : labelsToNum)
        cout << "map: " << x.first << " " << x.second << endl;
    getline(cin,str); 
    getline(cin,str); 
    
    for (int i=0;i<M;i++){
        int A, B, value;
        string edge1, edge2;
        //reads in string names for edges and value
        cin >> edge1 >> value >> edge2;
        //finds index for each edge and saves to o 
        A = labelsToNum.find(edge1)->second;
        B = labelsToNum.find(edge2)->second;
        cout << edge1 << ": " << A << " " <<  value << " " << edge2 << ": " << B <<endl;
        
        InEdges[B].push_back(Edge(A, B, value, 'o'));
        
        getline(cin,str); 
    }
    getline(cin,str); 

    //reads in Cont. Link Edges
    for (int i=0;i<K;i++){
        int A, B, low, high;
        string edge1, edge2;
        cin >> edge1 >> low >> high >> edge2;
        A = labelsToNum.find(edge1)->second;
        B = labelsToNum.find(edge2)->second;
        cout << A << ":" << edge1 << " " << low << " " << high << " " << B << ":" <<  edge2 << endl;
        
        InEdges[A].push_back(Edge(B, A, -high, 'u', B));
        InEdges[B].push_back(Edge(A, B, low, 'l', B));
        
        getline(cin,str); 
    
    }

}


int main() {
    parse();

    for(int i = 0; i < N; ++i) {
        for(auto &edge : InEdges[i]) {
            if(edge.value < 0) {
                is_negative_node[i] = true;
                break;
            }
        }
    }

    for(int i = 0; i < N; ++i) {
        if(is_negative_node[i]) {
            if(!DCBackprop(i)) {
                cout << "Not DC!" << endl;
                return 0;
            }
        }
    }

    cout << "DC!" << endl;

    return 0;
}
