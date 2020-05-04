/* 
   main.cpp
   Abigail Ren, Andrei Stanciu
 */

#include "algos.h" // include graph.cpp
#include <fstream>
#include <iostream>
using namespace std;

static int DEBUG = 0;

/* Contract: *parse() -> STNU
 * Output:   returns an STNU graph
 * Parses a file to obtain data
 * and initiates a new STNU graph
 */
/*
   STNU *parse() {
   string str;
   getline(cin, str);

// typenet reads in type of network
string typenet;
cin >> typenet;

getline(cin, str);
getline(cin, str);

// get n
int n;
cin >> n;

// instantiate STNU *G as new STNU with n edges
STNU *G = new STNU(n);

getline(cin, str);
getline(cin, str);

//Get M and K and save to STNU graph G
cin >> G->M;

getline(cin, str);
getline(cin, str);

cin >> G->K;

getline(cin, str);
getline(cin, str);

// read in TPs from file
for (int i = 0; i < G->N; i++) {
string TP;
cin >> TP;
// pushback TP name onto numsToLabel
G->numsToLabel.push_back(TP);
// save index of TP in labelsToNum
G->labelsToNum[TP] = i;
}

getline(cin, str);
getline(cin, str);

// reads in Ord. Edges

for (int i = 0; i < G->M; i++) {
OrdEdge o;
string edge1, edge2;
// reads in string names for edges and value
cin >> edge1 >> o.value >> edge2;
// finds index for each edge and saves to o

o.A = G->labelsToNum.find(edge1)->second;
o.B = G->labelsToNum.find(edge2)->second;
G->addEdge(o);
getline(cin, str);
}
getline(cin, str);

// reads in Cont. Link Edges
for (int i = 0; i < G->K; i++) {
ContLinkEdge c;
string edge1, edge2;
cin >> edge1 >> c.low >> c.high >> edge2;
c.A = G->labelsToNum.find(edge1)->second;
c.B = G->labelsToNum.find(edge2)->second;
G->addEdge(c);
getline(cin, str);
}

//updates buffered edges and adds them to STNU graph
G->updateAllBufferedEdges();

return G;
}
*/

/*morris2006()

 */

bool morris2006(string filename, bool debug=false){
  cout << "Do i get here?";
  STNU *Graph = new STNU(filename,debug);

  debug &&cerr << "Done reading!" << endl;

  if (is_dynamically_controllable(Graph)) {
    cout << "DC" << endl;
    return true;
  } else {
    cout << "Negative cycle found!" << endl;
    return false;
  }
}

/* Contract: main()
 * Initiates graph by parsing file
 * Determines if the graph is dynamically_controllable
 * and prints result.
 */
int main(int argc, char *argv[]) {
  if (argc >=3) {
    if (string(argv[1])=="--verbose=true") {
      DEBUG = 1;

    }
    else if (string(argv[1]) !="--verbose=false") {
      cout << "Usage: ./main --verbose={true/false} testfile "
        "[testfiles..]"
        << endl;
      exit(1);
    }
  }

  for (int i=2; i < argc; ++i){
    cout << argv[i] << " Morris 2006: " 
      << morris2006(string(argv[i]),DEBUG)
      << endl;
  }
  /*
     STNU *Graph = new STNU(filename);

     if (is_dynamically_controllable(Graph)) {
     cout << "DC" << endl;
     } else {
     cout << "Negative cycle found!" << endl;
     }
   */
  return 0;
}
