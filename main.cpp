#include <iostream>
#include <fstream>
#include "algos.h" // include graph.cpp
using namespace std;

//takes in a STNU graph and parses file to initiate graph
void parse(STNU& G) {
    string str;
    getline(cin,str); 
    //typenet reads in type of network
    string typenet;
    cin >> typenet;
    getline(cin,str); 
    getline(cin,str); 
    //get N, M, and K
    cin >> G.N;
    getline(cin,str); 
    getline(cin,str); 
    cin >> G.M;
    getline(cin,str); 
    getline(cin,str); 
    cin >> G.K;
    getline(cin,str); 
    getline(cin,str); 
    //read in TPs from file
    for (int i=0;i<G.N;i++){
        string TP;
        cin >> TP;
        //pushback TP name onto numsToLabel
        G.numsToLabel.push_back(TP);
        //save index of TP in labelsToNum
        G.labelsToNum[TP] = i;
    }
    getline(cin,str); 
    getline(cin,str); 
    
    for (int i=0;i<G.M;i++){
        OrdEdge o;
        string edge1, edge2;
        //reads in string names for edges and value
        cin >> edge1 >> o.value >> edge2;
        //finds index for each edge and saves to o 
        o.A = G.labelsToNum.find(edge1)->second;
        o.B = G.labelsToNum.find(edge2)->second;
        G.addEdge(o);
        getline(cin,str); 
    }
    getline(cin,str); 

    //reads in Cont. Link Edges
    for (int i=0;i<G.K;i++){
        ContLinkEdge c;
        string edge1, edge2;
        cin >> edge1 >> c.low >> c.high >> edge2;
        c.A = G.labelsToNum.find(edge1)->second;
        c.B = G.labelsToNum.find(edge2)->second;
        G.addEdge(c);
        getline(cin,str); 
    
    }

    G.updateAllLazyEdges();
}

int main() {
    STNU* Graph = new STNU();
    parse(*Graph);
    
    if(is_dinamically_controllable(Graph)) {
        cout << "DC" << endl;
    }
    else {
        cout << "Negative cycle found!" << endl;
    }
    
    return 0;
}

