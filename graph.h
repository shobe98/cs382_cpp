#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <vector>
#include <unordered_map>
#include <string>
#include <cassert>
using std::vector;
using std::unordered_map;
using std::string;

struct Neighbour {
    int ne; // the index of the neighbouring label -> one can get the edge from the edge matrix and edge vector. it's v messy for now :(
    char c; // type of edge, o - ordinary, l - lower case, u - upper case
    
    Neighbour(int ne = 0, char c = 0) {
        this->ne = ne;
        this->c = c;
    }
};

// this might become unused later on, ignore for now
struct ContLink {
    int bound;
    int contingentLabel;

    ContLink(int bound = 0, int contingentLabel = 0) {
        this->bound = bound;
        this->contingentLabel = contingentLabel;
    }
};


// I am using the following two structs to represent the edges
struct OrdEdge {
    int A, B; // labels
    int value;

    OrdEdge(int A = 0, int value = 0, int B = 0) {
        this->A = A;
        this->value = value;
        this->B = B;
    }
};

struct ContLinkEdge {
    int A, B; // labels
    int low, high;

    ContLinkEdge(int A = 0, int low = 0, int high = 0, int B = 0) {
        this->A = A;
        this->low = low;
        this->high = high;
        this->B = B;
    }
};

struct CaseEdge {
    int A, B, C; // labels and Case 
    int value;
    
    // from, to, label, value 
    CaseEdge(int A = 0, int B = 0, int C = 0, int value = 0) {
        this->A = A;
        this->C = C;
        this->value = value;
        this->B = B;

    }
}; // there will be a vector of lower case edges and a vector of upper case edges


class STNU {
public:
    // since the algorithm is O(n^4) running on more than 1000 would crash the computer, so for now let the max number of labels be 1000
    static const int kMaxLabels = 1000;
    
    // -INF, a value we use to code missing values, edges etc
    static const int kNaN = -1; // -1 for now
    
    // Labels, Ord Edges, Cont Links
    int N, M, K;
    

    bool has_negative_cycle = false;

    // mappings between labels and ints and back
    // TODO(abi): Can you implement a function that takes a vector of strings, copies it into numsToLabel and generates the labelsToNum map?
    unordered_map<string, int> labelsToNum;
    vector<string> numsToLabel;

    // These  matrices are necessary to make sure we don't save duplicated edges (that would fuck up the complexity - say if an edge gets updated n times, we would otherwise have ncopies of it)
    // I know this makes the code slightly messier, but should be fine for now
    // index to the edge in the vector of edges or -1
    int ordEdges[kMaxLabels][kMaxLabels];
    int lcEdges[kMaxLabels][kMaxLabels];
    int ucEdges[kMaxLabels][kMaxLabels];

    // to get the allmax edges iterate first through the ordinary edges and then through the uc edges, assigning high to each contlink
    vector<OrdEdge> ordEdgesList;
    vector<CaseEdge> ucEdgesList;
    vector<CaseEdge> lcEdgesList;


    // in these two vectors we save the edges we want to add to the graph. 
    vector<OrdEdge> lazyOrdEdges;
    vector<CaseEdge> lazyUcEdges;
    vector<CaseEdge> lazyLcEdges;

    // Lists of neighbours -> basically the matrices prof Hunsberger showed us.
    vector<Neighbour> ordNeighbours[kMaxLabels];
    vector<Neighbour> lcNeighbours[kMaxLabels];
    vector<Neighbour> ucNeighbours[kMaxLabels];


    STNU() {
        for(int i = 0; i < kMaxLabels; ++i) { 
            for(int j = 0; j < kMaxLabels; ++j) {
                ordEdges[i][j] = ucEdges[i][j] = lcEdges[i][j] = kNaN;
            }
        }
    }

    void addEdge(const OrdEdge& e);    
    void addEdge(const ContLinkEdge& e);

    void updateAllLazyEdges();
};

#endif // _GRAPH_H_
