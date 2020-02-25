#include <iostream>
#include <fstream>
#include "algos.cpp" // include graph.cpp

//takes in a STNU graph and parses file to initiate graph
void parse(STNU& G) {
    string str;
    ifstream data;
    data.open("anything.stnu");
    getline(data,str); 
    //typenet reads in type of network
    string typenet;
    data >> typenet;
    cout << typenet << endl;
    getline(data,str); 
    getline(data,str); 
    //get N, M, and K
    data >> G.N;
    cout << G.N << endl;
    getline(data,str); 
    getline(data,str); 
    data >> G.M;
    cout << G.M << endl;
    getline(data,str); 
    getline(data,str); 
    data >> G.K;
    cout << G.K << endl;
    getline(data,str); 
    getline(data,str); 
    //read in TPs from file
    for (int i=0;i<G.N;i++){
        string TP;
        data >> TP;
        //pushback TP name onto numsToLabel
        G.numsToLabel.push_back(TP);
        //save index of TP in labelsToNum
        G.labelsToNum[TP] = i;
        cout << TP << endl;
    }
    for (string t : G.numsToLabel)
        cout << "TPS: " << t << endl;
    for (auto x : G.labelsToNum)
        cout << "map: " << x.first << " " << x.second << endl;
    getline(data,str); 
    getline(data,str); 
    
    for (int i=0;i<G.M;i++){
        OrdEdge o;
        string edge1, edge2;
        //reads in string names for edges and value
        data >> edge1 >> o.value >> edge2;
        //finds index for each edge and saves to o 
        o.A = G.labelsToNum.find(edge1)->second;
        o.B = G.labelsToNum.find(edge2)->second;
        cout << edge1 << ": " << o.A << " " <<  o.value << " " << edge2 << ": " << o.B <<endl;
        G.addEdge(o);
        getline(data,str); 
    }
    getline(data,str); 

    //reads in Cont. Link Edges
    for (int i=0;i<G.K;i++){
        ContLinkEdge c;
        string edge1, edge2;
        data >> edge1 >> c.low >> c.high >> edge2;
        c.A = G.labelsToNum.find(edge1)->second;
        c.B = G.labelsToNum.find(edge2)->second;
        cout << c.A << ":" << edge1 << " " << c.low << c.high  << c.B << ":"<<  edge2<<endl;
        G.addEdge(c);
        getline(data,str); 
    
    }

    //This is seg faulting and I don't know why
    G.updateAllLazyEdges();
    data.close();
}

int main() {
    STNU* Graph = new STNU();
    parse(*Graph);
    return 0;
}

