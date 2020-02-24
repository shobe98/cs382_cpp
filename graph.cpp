#include <vector>
#include <unordered_map>
#include <string>
#include <assert.h>
using namespace std;

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



    void addEdge(const OrdEdge& e) {
        lazyOrdEdges.push_back(e);
    }

    void addEdge(const ContLinkEdge& e) {
        // page 80/182 in notes 
        lazyUcEdges.push_back(CaseEdge(e.B, e.A, e.B, -e.high));
        lazyLcEdges.push_back(CaseEdge(e.A, e.B, e.B, e.low));
    }


    void updateAllLazyEdges() {
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
};

