/* -----------------------------
   CMPU-382, Spring 2020
   Authors:  Abigail Ren, Andrei Stanciu
   File:     main.cpp
   ------------------------------
   Defines Functions: main
 */

#include "algos.h" // include graph.cpp
#include <fstream>
#include <iostream>
using namespace std;

/* Contract: main()
 * Sets debug flag and calls morris2006
 */
int main(int argc, char *argv[]) {
  bool debug = false;
  if (argc >= 3) {
    if (string(argv[1]) == "--verbose=true") {
      debug = true;

    } else if (string(argv[1]) != "--verbose=false") {
      cout << "Usage: ./main --verbose={true/false} testfile "
              "[testfiles..]"
           << endl;
      exit(1);
    }
  }

  for (int i = 2; i < argc; ++i) {
    cout << argv[i]
         << " Morris 2006: " << Morris2006::morris2006(string(argv[i]), debug)
         << endl;
  }
  return 0;
}
