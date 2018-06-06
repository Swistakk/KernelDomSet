#pragma once
#include "Headers.hpp"
#include "Independent.hpp"
#include "CommonGraph.hpp"
#include "UQW.hpp"

struct Graph {
  vector<set<int>> all_neis;
  vector<set<int>> black_neis;
  vector<set<int>> white_neis;
  set<int> alive;
  vector<bool> is_white;
  vector<int> domset;
  int n;
  Graph() {
    n = -1;
  }
  Graph(int n_) {
    n = n_;
    ResizeVectors(n);
    for (int i = 1; i <= n; i++) {
      alive.insert(i);
    }
  }
  void ResizeVectors(int k) {
    all_neis.resize(k + 1);
    black_neis.resize(k + 1);
    white_neis.resize(k + 1);
    is_white.resize(k + 1);
  }
  void AddEdge(int a, int b) {
    for (int rnds = 0; rnds < 2; rnds++) {
      if (!is_white[b]) {
        black_neis[a].insert(b);
      } else {
        white_neis[a].insert(b);
      }
      all_neis[a].insert(b);
      swap(a, b);
    }
  }
  void ColorWhite(int v) {
    //debug("to white ", v);
    for (auto nei : all_neis[v]) {
      black_neis[nei].erase(v);
      white_neis[nei].insert(v);
    }
    is_white[v] = 1;
  }
  void Kill(int v) {
    //debug("kill ", v);
    assert(alive.count(v));
    for (auto nei : all_neis[v]) {
      all_neis[nei].erase(v);
      black_neis[nei].erase(v);
      white_neis[nei].erase(v);
    }
//     all_neis[v].clear();
//     black_neis[v].clear();
//     white_neis[v].clear();
    alive.erase(v);
  }
  void RemoveEdge(int a, int b) {
    //debug("rem edge", a, b);
    for (int i = 0; i < 2; i++) {
      all_neis[a].erase(b);
      white_neis[a].erase(b);
      black_neis[a].erase(b);
      swap(a, b);
    }
  } 
  void CheckRemoveWhiteEdgesOne(int v) { // 2 (a)
    if (alive.count(v) == 0) { return; }
    if (!is_white[v]) { return; }
    vector<int> to_remove(white_neis[v].begin(), white_neis[v].end());
    for (auto nei : to_remove) {
      RemoveEdge(v, nei);
    }
  }
  void CheckWhiteNoBlack(int v) { // 2 (b)
    if (alive.count(v) == 0) { return; }
    if (is_white[v] && black_neis[v].empty()) {
      Kill(v);
    }
  }
  bool AreBlackContained(int u, int v) { // check if black neis of v contain black neis of u
    for (auto nei : black_neis[u]) {
      if (black_neis[v].count(nei) == 0 && nei != v) {
        return false;
      }
    }
    return true;
  }
  void CheckWhiteSubBlack(int v) { // 2 (c) (superset of 2b)
    // easy to code it probably faster by intersecting neighbourhoods of black_neis[v]
    if (alive.count(v) == 0) { return; }
    if (!is_white[v]) { return; }
    set<int> checked;
    for (auto nei : all_neis[v]) {
      checked.insert(nei);
      if (AreBlackContained(v, nei)) {
        //debug("here1", nei);
        Kill(v);
        return;
      }
    }
    for (auto nei : black_neis[v]) {
      for (auto neinei : all_neis[nei]) {
        if (checked.count(neinei) || neinei == v) { continue; }
        checked.insert(neinei);
        if (AreBlackContained(v, neinei)) {
          //debug("here2", neinei);
          Kill(v);
          return;
        }
      }
    }
  }
  // If e(u, v), N(u) \superset N(v) and v is black then u can be colored white
  void CheckRecolorSuperset(int u) {
    if (alive.count(u) == 0) { return; }
    if (is_white[u]) { return; }
    for (auto nei : black_neis[u]) {
      bool fail = false;
      for (auto neinei : all_neis[nei]) {
        if (neinei != u && all_neis[u].count(neinei) == 0) {
          fail = true;
          break;
        }
      }
      if (!fail) {
        //debug("superset ", u, " by ", nei);
        ColorWhite(u);
        return;
      }
    }
  }
  void PutDomset(int u) {
    //debug("put ", u);
    domset.PB(u);
    vector<int> to_recolor;
    for (auto nei : black_neis[u]) {
      if (!is_white[nei]) {
        to_recolor.PB(nei);
      }
    }
    for (auto nei_to_recolor : to_recolor) {
      ColorWhite(nei_to_recolor);
    }
    Kill(u);
  }
  void CheckLeaf(int u) { // 2 (d)
    if (alive.count(u) == 0) { return; }
    if (all_neis[u].empty()) {
      if (!is_white[u]) {
        PutDomset(u);
      } else {
        Kill(u);
      }
    }
    if ((int)all_neis[u].size() == 1) {
      if (!is_white[u]) {
        PutDomset(*all_neis[u].begin());
      }
      Kill(u);
    }
  }
  int SolveBrut() {
    vector<int> shrink_black_ind(n + 1);
    int nxt_ind = 0;
    for (auto v : alive) {
      if (!is_white[v]) {
        shrink_black_ind[v] = nxt_ind;
        nxt_ind++;
      }
    }
    //debug(n, nxt_ind, shrink_black_ind);
    assert(nxt_ind <= 25);
    vector<int> dp(1 << nxt_ind, n);
    dp[0] = 0;
    for (auto v : alive) {
      int covered = 0;
      for (auto nei : black_neis[v]) {
        covered += 1 << shrink_black_ind[nei];
      }
      //debug(v, black_neis[v], is_white[v]);
      if (!is_white[v]) {
        covered += 1 << shrink_black_ind[v];
      }
      //debug(covered);
      for (int prv_mask = 0; prv_mask < 1 << nxt_ind; prv_mask++) {
        dp[prv_mask | covered] = min(dp[prv_mask | covered], dp[prv_mask] + 1);
      }
    }
    return dp.back() + domset.size();
  }
  void AlberOne(int v) {
    if (alive.count(v) == 0) { return; }
    set<int> N1;
    for (auto nei : all_neis[v]) {
      for (auto neinei : all_neis[nei]) {
        if (neinei != v && all_neis[v].count(neinei) == 0) {
          N1.insert(nei);
          break;
        }
      }
    }
    set<int> N2;
    for (auto nei : all_neis[v]) {
      if (N1.count(nei)) { continue; }
      for (auto neinei : all_neis[nei]) {
        if (N1.count(neinei)) {
          N2.insert(nei);
          break;
        }
      }
    }
    set<int> N3;
    for (auto nei : all_neis[v]) {
      if (N1.count(nei) || N2.count(nei)) { continue; }
      N3.insert(nei);
    }
    for (auto nei : N3) {
      if (!is_white[nei]) {
        PutDomset(v);
        return;
      }
    }
  }
  bool AlberPair(int v, int w) {
    assert(v != w);
    if (!alive.count(v) || !alive.count(w)) { return false; }
    set<int> nvw = all_neis[v];
    nvw.insert(all_neis[w].begin(), all_neis[w].end());
    nvw.erase(v);
    nvw.erase(w);
    set<int> N1;
    for (auto u : nvw) {
      for (auto nei : all_neis[u]) {
        if (nei != v && nei != w && nvw.count(nei) == 0) {
          N1.insert(u);
          break;
        }
      }
    }
    set<int> N2, N3, N3_black;
    for (auto u : nvw) {
      if (N1.count(u)) { continue; }
      bool got = false;
      for (auto nei : all_neis[u]) {
        if (N1.count(nei)) {
          N2.insert(u);
          got = true;
          break;
        }
      }
      if (!got) {
        N3.insert(u);
        if (!is_white[u]) {
          N3_black.insert(u);
        }
      }
    }
    if ((int)N3.size() <= 1) { return false; }
    set<int> N23 = N2;
    N23.insert(N3.begin(), N3.end());
    for (auto u : N23) {
      bool any_non_dominated = false;
      for (auto t : N3_black) {
        if (all_neis[u].count(t) == 0 && t != u) {
          any_non_dominated = true;
          break;
        }
      }
      if (!any_non_dominated) { return false; }
    }
    int dom_v = true, dom_w = true;
    for (auto u : N3_black) {
      if (all_neis[v].count(u) == 0) { dom_v = false; }
      if (all_neis[w].count(u) == 0) { dom_w = false; }
    }
    // GOSCIE Z N3 MUSZA BYC CZARNI ABY WYMUSIC TE REGULE
    if (dom_w && !dom_v) {
      swap(v, w);
      swap(dom_w, dom_v);
    }
    if (!dom_w && !dom_v) {
      PutDomset(v);
      PutDomset(w);
      for (auto u : N23) {
        Kill(u);
      }
    } else if (!dom_w) {
      PutDomset(v);
      for (auto u : N3) {
        Kill(u);
      }
      for (auto u : N2) {
        if (all_neis[v].count(u)) {
          Kill(u);
        }
      }
    } else {
      assert(dom_w && dom_v);
      set<int> to_kill(N3.begin(), N3.end());
//       for (auto u : N3) {
//         Kill(u);
//       }
      for (auto u : N2) {
        if (all_neis[v].count(u) && all_neis[w].count(u)) {
          to_kill.insert(u);
          //Kill(u);
        }
      }
      if (to_kill.size() <= 2) {
        return false;
      }
      for (auto u : to_kill) {
        Kill(u);
      }
      ResizeVectors(n + 2);
      for (int new_v = n + 1; new_v <= n + 2; new_v++) {
        alive.insert(new_v);
        AddEdge(v, new_v);
        AddEdge(w, new_v);
      }
      n += 2;
    }
    return true;
  }
  
  vector<int> GreedyDomsetBlack() {
    vector<int> black_vertices;
    for (int i = 1; i <= n; i++) {
      if (!is_white[i] && alive.count(i)) {
        black_vertices.PB(i);
      }
    }
    return GreedyDomsetOfArbitrarySubset(black_vertices);
  }
  
  // lazily coded in n^2, this won't be a bottleneck anyway
  // assumes that all vertices in to_dominate are black
  vector<int> GreedyDomsetOfArbitrarySubset(vector<int> to_dominate) { 
    vector<bool> need_domination(n + 1);
    int cnt_need = (int)to_dominate.size();
    for (auto v : to_dominate) {
      assert(!is_white[v]);
      need_domination[v] = true;
    }
    vector<bool> need_domination_copy = need_domination;
    vector<int> res;
    while (cnt_need) {
      int best_guy = 0;
      int best_amount = 0;
      for (auto v : alive) {
        int here_amount = 0;
        if (need_domination[v]) {
          here_amount++;
        }
        for (auto nei : black_neis[v]) {
          if (need_domination[nei]) {
            here_amount++;
          }
        }
        if (here_amount > best_amount) {
          best_amount = here_amount;
          best_guy = v;
        }
      }
      res.PB(best_guy);
      cnt_need -= best_amount;
      need_domination[best_guy] = false;
      for (auto nei : black_neis[best_guy]) {
        need_domination[nei] = false;
      }
    }
    
    // begin checks
    assert(cnt_need == 0);
    set<int> domset_set(res.begin(), res.end());
    for (int i = 1; i <= n; i++) {
      if (!need_domination_copy[i] || alive.count(i) == 0) { continue; }
      if (domset_set.count(i)) { continue; }
      bool succ = false;
      for (auto nei : all_neis[i]) {
        if (domset_set.count(nei)) {
          succ = true;
          break;
        }
      }
      assert(succ);
    }
    // end checks
    
    return res;
  }
  
  vector<vector<int>> CountProjections(vector<int> A, int r) {
    sort(A.begin(), A.end());
    vector<int> in_A(n + 1);
    for (auto v : A) {
      in_A[v] = 1;
    }
    vector<int> last_vis(n + 1);
    vector<int> dis(n + 1);
    int cnt_bfs = 0;
    vector<vector<int>> projs(n + 1);
    for (auto root : A) {
      cnt_bfs++;
      vector<int> que{root};
      last_vis[root] = cnt_bfs;
      dis[root] = 0;
      for (int ii = 0; ii < (int)que.size(); ii++) {
        int v = que[ii];
        if (ii) {
          projs[v].PB(root);
        }
        if (dis[v] == r) { continue; }
        for (auto nei : all_neis[v]) {
          if (in_A[nei]) { continue; }
          if (last_vis[nei] == cnt_bfs) { continue; }
          last_vis[nei] = cnt_bfs;
          dis[nei] = dis[v] + 1;
          que.PB(nei);
        }
      }
    }
    return projs;
  }
  
  // coded for independent verification of correctness
  vector<vector<int>> CountProjections2(vector<int> A, int r) {
    vector<int> in_A(n + 1);
    for (auto v : A) {
      in_A[v] = 1;
    }
    vector<vector<int>> projs(n + 1);
    for (int root = 1; root <= n; root++) {
      if (alive.count(root) == 0) { continue; }
      if (in_A[root]) { continue; }
      vector<int> que{root};
      vector<int> dis(n + 1, n + 1);
      dis[root] = 0;
      for (int ii = 0; ii < (int)que.size(); ii++) {
        int v = que[ii];
        for (auto nei : all_neis[v]) {
          if (dis[nei] < n + 1) { continue; }
          dis[nei] = dis[v] + 1;
          if (in_A[nei]) {
            projs[root].PB(nei);
          } else if (dis[nei] < r) {
            que.PB(nei);
          }
        }
      }
      sort(projs[root].begin(), projs[root].end());
    }
    return projs;
  }
  
  void TryFindingIrrelevantDominatee() { // D is domset apx of current core
    vector<int> D = GreedyDomsetBlack();
    vector<int> in_D(n + 1);
    for (auto x : D) {
      in_D[x] = 1;
    }
    while (1) {
      vector<vector<int>> G_minus_D(n + 1);
      vector<int> A;
      for (int v = 1; v <= n; v++) {
        if (in_D[v] || alive.count(v) == 0) { continue; }
        for (auto nei : all_neis[v]) {
          if (in_D[nei] || alive.count(nei) == 0) { continue; }
          G_minus_D[v].PB(nei);
        }
        if (!is_white[v]) {
          A.PB(v);
        }
      }
      vector<int> S, scat;
      tie(S, scat) = UQWLeastDegreePower(G_minus_D, A, 2);
      // wrzuc S do D
      vector<int> in_D_copy = in_D;
      vector<int> Dprim(D);
      for (auto v : S) {
        Dprim.PB(v);
        in_D[v] = 1;
      }
      vector<vector<int>> projs = CountProjections(Dprim, 2);
      //debug(Dprim, projs[1]);
      map<pair<vector<int>, vector<int>>, vector<int>> classes;
      //for (int v = 1; v <= n; v++) {
      //  if (in_D[v] || alive.count(v) == 0 || is_white[v]) { continue; }
      for (auto v : scat) {
        vector<int> neis_in_Dprim;
        vector<int> black_proj2;
        for (auto at_proj : projs[v]) {
          if (all_neis[v].count(at_proj)) {
            neis_in_Dprim.PB(at_proj);
          } else if (!is_white[at_proj]) {
            black_proj2.PB(at_proj);
          } 
        }
        classes[{neis_in_Dprim, black_proj2}].PB(v);
      }
      
      // do greedy on something and find irr dominatee
      
      int bfs_count = 0;
      vector<int> dis(n + 1);
      vector<int> last_vis(n + 1);
      for (auto elt : classes) {
        vector<int> neis_in_Dprim, black_proj2;
        tie(neis_in_Dprim, black_proj2) = elt.st;
        vector<int> lambda = elt.nd; // hopefully it is big
        
        bfs_count++;
        vector<int> que = lambda;
        for (auto v : lambda) {
          last_vis[v] = bfs_count;
          dis[v] = 0;
        }
        vector<int> to_dominate;
        for (int ii = 0; ii < (int)que.size(); ii++) {
          int v = que[ii];
          if (!is_white[v]) {
            to_dominate.PB(v);
          }
          if (dis[v] == 2) { continue; }
          if (in_D[v]) { continue; }
          for (auto nei : all_neis[v]) {
            if (last_vis[nei] == bfs_count) { continue; }
            last_vis[nei] = bfs_count;
            dis[nei] = 1 + dis[v];
            que.PB(nei);
          } 
        }
        
        
        // zrob drugiego checka tutaj
        set<int> zioms;
        for (auto v : lambda) {
          zioms.insert(v);
          for (auto nei : all_neis[v]) {
            if (!is_white[nei]) {
              zioms.insert(nei);
            }
            if (in_D[nei]) { continue; }
            for (auto neinei : all_neis[nei]) {
              if (!is_white[neinei]) {
                zioms.insert(neinei);
              }
            }
          }
        }
        
        
        sort(to_dominate.begin(), to_dominate.end());
        vector<int> chk(zioms.begin(), zioms.end());
        //debug(to_dominate, chk, lambda, all_neis[lambda[0]]);
        assert(to_dominate == chk);
        vector<int> greedy = GreedyDomsetOfArbitrarySubset(to_dominate);
        if ((int)greedy.size() + 2 <= (int)lambda.size()) {
          int to_whiten_count = (int)lambda.size() - (int)greedy.size() - 1; 
          debug("SUCC", to_whiten_count);
          debug(D, Dprim, lambda, neis_in_Dprim, black_proj2);
          for (int ii = 0; ii < to_whiten_count; ii++) {
            ColorWhite(lambda[ii]);
          }
        }
      }
      
      int who_biggest_deg_D = -1;
      int biggest_deg_D = -1;
      for (int v = 1; v <= n; v++) {
        if (in_D[v] || alive.count(v) == 0) { continue; }
        int my_deg_D = 0;
        for (auto nei : all_neis[v]) {
          if (in_D[nei]) {
            my_deg_D++;
          }
        }
        if (my_deg_D > biggest_deg_D) {
          biggest_deg_D = my_deg_D;
          who_biggest_deg_D = v;
        }
      }
      if (biggest_deg_D <= 5) {
        break;
      }
      in_D = in_D_copy;
      D.PB(who_biggest_deg_D);
      in_D[who_biggest_deg_D] = 1;
    }
      
  }
    
  void Debug() {
    debug(n);
    debug(alive);
    for (int v = 1; v <= n; v++) {
      if (alive.count(v) == 0) { continue; }
      debug(v, is_white[v], black_neis[v], white_neis[v]);
    }
  }
  
  pair<int, int> GetVerticesStats() {
    int cnt_black = 0;
    for (auto x : alive) {
      if (!is_white[x]) {
        cnt_black++;
      }
    }
    return {(int)alive.size(), cnt_black};
  }
  
  void Reduce(bool sparsity_rule, bool alber_one, bool alber_pair) {
    //Graph last_graph;
    pair<int, int> last_info = GetVerticesStats();
    for (int passes = 1; ; passes++) {
      //last_graph = gr;
      if (sparsity_rule) {
        TryFindingIrrelevantDominatee();
      }
//       if (last_graph.SolveBrut() != SolveBrut()) {
//         last_graph.Debug();
//         Debug();
//         debug(last_graph.SolveBrut(), SolveBrut());
//         assert(false);
//       }
      for (int i = 1; i <= n; i++) {
        if (alive.count(i) == 0) { continue; }
        CheckRemoveWhiteEdgesOne(i);
        CheckWhiteNoBlack(i);
        CheckWhiteSubBlack(i);
        CheckRecolorSuperset(i);
        CheckLeaf(i);
        if (alber_one) {
          AlberOne(i);
        }
        if (alber_pair) {
          for (int j = 1; j < i; j++) {
  //           last_graph = gr;
            if (AlberPair(i, j)) {
  //             pr = {i, j};
  //             int aft = SolveBrut();
  //             if (aft != or_domset) {
  //               debug(aft, or_domset, or_n, or_m);
  //               goto Crash;
  //             }
            }
          }
        }
      }
      //completed_passes++;
      pair<int, int> cur_info = GetVerticesStats();
      if (cur_info == last_info) {
        break;
      }
      last_info = cur_info;
    }
  }
    
};