#include <iostream>
#include "Graph.hpp"
using namespace std;
int main() {
  //srand(clock());
  srand(0);
  for (int test_ind = 1; test_ind <= 1000; test_ind++) {
    int n = 1 + rand() % 20;
    int m = min(n * (n - 1) / 2, 1 + rand() % (3 * n));
    set<pair<int, int>> es;
    Graph gr(n);
    while ((int)es.size() < m) {
      int a = 1 + rand() % n;
      int b = 1 + rand() % n;
      if (a == b) { continue; }
      if (a > b) {
        swap(a, b);
      }
      es.insert({a, b});
    }
    debug(es);
    for (auto e : es) {
      gr.AddEdge(e.st, e.nd);
    }
    int or_n = n, or_m = m;
    int or_domset = gr.SolveBrut();
    for (int passes = 1; passes <= 5; passes++) {
      for (int i = 1; i <= n; i++) {
        if (gr.alive.count(i) == 0) { continue; }
        gr.CheckRemoveWhiteEdgesOne(i);
        gr.CheckWhiteNoBlack(i);
        gr.CheckWhiteSubBlack(i);
        gr.CheckRecolorSuperset(i);
        gr.CheckLeaf(i);
      }
    }
    int aft_n = gr.alive.size(), aft_m = 0;
    for (auto v : gr.alive) {
      aft_m += gr.all_neis[v].size();
    }
    aft_m /= 2;
    int aft_domset = gr.SolveBrut();
    debug(or_n, or_m, aft_n, aft_m, or_domset, aft_domset);
    assert(or_domset == aft_domset);
  }
}   