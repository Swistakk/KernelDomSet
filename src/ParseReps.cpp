#include "Headers.hpp"
#include "FilesOps.hpp"

const int batch_size = 100;
int main() {
  vector<string> sparsity_flags{"off", "on"};
  vector<string> alber_flags{"none", "one", "both"};
  vector<int> sizes{302, 450, 600, 2002, 3000, 4000};
  for (auto sz : sizes) {
    for (auto alber_flag : alber_flags) {
      for (auto sparsity_flag : sparsity_flags) {
        string filename = "rep" + to_string(sz) + "." + sparsity_flag + "." + alber_flag + ".txt";
        ifstream in;
        InitIfstream(in, filename);
        int sum_n = 0, sum_m = 0, sum_aft_n = 0, sum_aft_m = 0;
        for (int line = 1; line <= batch_size; line++) {
          int n, m, aft_n, aft_m;
          in>>n>>m>>aft_n>>aft_m;
          sum_n += n;
          sum_m += m;
          sum_aft_n += aft_n;
          sum_aft_m += aft_m;
        }
        cout<<sz<<" "<<sparsity_flag<<" "<<alber_flag<<" "<<1. * sum_aft_n / sum_n<<" "<<1. * sum_aft_m / sum_m<<endl;
        in.close();
      }
    }
  }
}