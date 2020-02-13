#include <vector>
#include <unordered_map>
#include <string>
using namespace std;

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

 

class STNU {
public:
    // since the algorithm is O(n^4) running on more than 1000 would crash the computer, so for now let the max number of labels be 1000
    static const int kMaxLabels = 1000;
    
    // -INF, a value we use to code missing values, edges etc
    static const int kNaN = ~0;
    
    // Labels, Ord Edges, Cont Links
    int N, M, K;
    

    // mappings between labels and ints and back
    // TODO(abi): Can you implement a function that takes a vector of strings, copies it into numsToLabel and generates the labelsToNum map?
    unordered_map<string, int> labelsToNum;
    vector<string> numsToLabel;

    int ordEdges[kMaxLabels][kMaxLabels];
    
 
    ContLink contLinks[kMaxLabels][kMaxLabels];


    // to get the allmax edges iterate first through the ordinary edges and then through the contlinks, assigning high to each contlink
    vector<OrdEdge> ordEdgesList;
    vector<ContLinkEdge> contLinksList;


    // in these two vectors we save the edges we want to add to the graph. 
    vector<OrdEdge> lazyOrdEdges;
    vector<ContLinkEdge> lazyContLinks;




    void addEdge(const OrdEdge& e) {
        lazyOrdEdges.push_back(e);
    }

    void addEdge(const ContLinkEdge& e) {
        lazyContLinks.push_back(e);
    }

    void updateAllLazyEdges() {
        // TODO(astanciu): implement a function that adds all the new edges to the graph and clears the buffers (lazy edges vectors)
    }
};

