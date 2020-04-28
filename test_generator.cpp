#include <cstdlib>
#include <ctime>
#include <fstream>
using namespace std;

int main() {
  srand(382);

  for (int test = 0; test < 100; ++test) {
    int n, m, k;
    n = rand() % 10 + 5;
    m = rand() % 15;
    k = rand() % 10;

    ofstream fout(string("tests/random/test") + to_string(test) +
                  string(".stnu"));

    fout << "# KIND OF NETWORK\nSTNU\n# Num Time-Points\n"
         << n << "\n"
         << "# Num Ordinary Edges\n"
         << m << "\n"
         << "# Num Contingent Links\n"
         << k << "\n"
         << "#Time - Point Names" << endl;

    for (int i = 0; i < n; ++i) {
      fout << (char)('A' + i) << " ";
    }
    fout << "\n# Ordinary Edges" << endl;

    for (int i = 0; i < m; ++i) {
      char A, B;
      int val;
      A = 'A' + rand() % n;
      B = 'A' + rand() % n;
      val = rand() % 40 - 20; // [-20:20]

      fout << A << " " << val << " " << B << '\n';
    }
    fout << "# Contingent Links" << endl;
    for (int i = 0; i < k; ++i) {
      char A, B;
      int lo, range;
      A = 'A' + rand() % n;
      B = 'A' + rand() % n;
      lo = rand() % 20;
      range = rand() % 10;

      fout << A << " " << lo << " " << lo + range << " " << B << "\n";
    }
    fout << endl;
    fout.close();
  }
}
