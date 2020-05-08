/* ----------------------------- 
   CMPU-382, Spring 2020
   Authors:  Abigail Ren, Andrei Stanciu
   File:     main.cpp
   ------------------------------
   Defines Functions: morris2006, main
 */

#include "algos.h" // include graph.cpp
#include <fstream>
#include <iostream>
using namespace std;

/* Contract: morris2006(filename, debug)
 * Input:    filename is name of input file
 *           debug boolean flag for printing debug statements
 * Calls is_dynamically_controllable and determines whether
 * STNU graph described in the filename input is DC or not
 */

bool morris2006(string filename, bool debug=false){
  STNU *Graph = new STNU(filename,debug);

  debug &&cerr << "Done reading!" << endl;

  if (is_dynamically_controllable(Graph)) {
    debug && cout << "DC" << endl;
    return true;
  } else {
    debug && cout << "Negative cycle found!" << endl;
    return false;
  }
}

/* Contract: main()
 * Sets debug flag and calls morris2006
 */
int main(int argc, char *argv[]) {
  bool debug = false;
  if (argc >=3) {
    if (string(argv[1])=="--verbose=true") {
      debug = true;

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
      << morris2006(string(argv[i]),debug)
      << endl;
  }
  return 0;
}
