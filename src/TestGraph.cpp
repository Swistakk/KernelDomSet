#include <iostream>
#include "Graph.hpp"
using namespace std;
int main() {
  //srand(clock());
  srand(0);
  int sum_sz_n = 0, sum_or_n = 0;
  int sum_or_dom = 0, sum_sz_gr = 0;
  int sum_black = 0;
  int completed_passes = 0;
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
    //debug(es);
    for (auto e : es) {
      gr.AddEdge(e.st, e.nd);
    }
    int sz_gr = gr.GreedyDomsetBlack().size();
    int or_n = n, or_m = m;
    int or_domset = gr.SolveBrut();
    Graph last_graph;
    pair<int, int> pr;
    pair<int, int> last_info = gr.GetVerticesStats();
    for (int passes = 1; ; passes++) {
      //last_graph = gr;
      gr.TryFindingIrrelevantDominatee();
//       if (last_graph.SolveBrut() != gr.SolveBrut()) {
//         last_graph.Debug();
//         gr.Debug();
//         debug(last_graph.SolveBrut(), gr.SolveBrut());
//         assert(false);
//       }
      for (int i = 1; i <= gr.n; i++) {
        if (gr.alive.count(i) == 0) { continue; }
        gr.CheckRemoveWhiteEdgesOne(i);
        gr.CheckWhiteNoBlack(i);
        gr.CheckWhiteSubBlack(i);
        gr.CheckRecolorSuperset(i);
        gr.CheckLeaf(i);
        gr.AlberOne(i);
        for (int j = 1; j < i; j++) {
//           last_graph = gr;
          if (gr.AlberPair(i, j)) {
//             pr = {i, j};
//             int aft = gr.SolveBrut();
//             if (aft != or_domset) {
//               debug(aft, or_domset, or_n, or_m);
//               goto Crash;
//             }
          }
        }
      }
      completed_passes++;
      pair<int, int> cur_info = gr.GetVerticesStats();
      if (cur_info == last_info) {
        break;
      }
      last_info = cur_info;
    }
    
//     Crash: ; 
//     debug(last_graph.SolveBrut(), gr.SolveBrut());
//     debug(last_graph.alive, gr.alive, pr);
//     debug(last_graph.domset);
//     for (auto v : last_graph.alive) {
//       debug(v, last_graph.black_neis[v],last_graph.white_neis[v]);
//     }
//     debug(gr.domset);
    int aft_n = gr.alive.size(), aft_m = 0;
    for (auto v : gr.alive) {
      aft_m += gr.all_neis[v].size();
      if (!gr.is_white[v]) {
        sum_black++;
      }
    }
    aft_m /= 2;
    int aft_domset = gr.SolveBrut();
    debug(or_n, or_m, aft_n, aft_m, or_domset, aft_domset, sz_gr, gr.n);
    sum_or_n += or_n;
    sum_sz_n += aft_n;
    sum_or_dom += or_domset;
    sum_sz_gr += sz_gr;
    assert(or_domset == aft_domset);
    assert(sz_gr >= or_domset);
    vector<int> A;
//     for (int i = 1; i <= gr.n; i++) {
//       if (gr.alive.count(i) == 0) { continue; }
//       if (rand() % 3 == 0) {
//         A.PB(i);
//       }
//     }
    //debug(A);
    //debug(gr.Count3Projections(A, ), gr.Count3Projections2(A));
    assert(gr.CountProjections(A, 3) == gr.CountProjections2(A, 3));
  }
  debug(sum_or_n, sum_sz_n, sum_black, sum_or_dom, sum_sz_gr, completed_passes);
}   