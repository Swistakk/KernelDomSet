#pragma once
#include "Headers.hpp"
#include "Independent.hpp"
#include "CommonGraph.hpp"

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
  
  // 
  vector<int> GreedyDomset() { // lazily coded in n^2, this won't be a bottleneck anyway
    vector<bool> need_domination(n + 1);
    int cnt_need = 0;
    for (int i = 1; i <= n; i++) {
      if (!is_white[i] && alive.count(i)) {
        need_domination[i] = true;
        cnt_need++;
      }
    }
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
      if (is_white[i] || alive.count(i) == 0) { continue; }
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
  
  vector<vector<int>> ClassifyBy3Projections(vector<int> A) {
    vector<vector<int>> projections = CountProjections(A, 3);
    vector<int> in_A(n + 1);
    for (auto u : A) {
      in_A[u] = 1;
    }
    map<vector<int>, vector<int>> classes;
    for (int i = 1; i <= n; i++) {
      if (alive.count(i) == 0) { continue; }
      if (in_A[i]) { continue; }
      classes[projections[i]].PB(i);
    }
    vector<vector<int>> res;
    for (auto p : classes) {
      res.PB(p.nd);
    }
    return res;
  }
  
  pair<vector<int>, vector<int>> UQWLeastDegreePower(vector<int> oldA, int R) {
    unordered_set<int> S;
    vector<vector<int>> candsA;
    vector<int> candsAszs;
    vector<unordered_set<int>> candsS;
    vector<vector<int>> candsS_vec;
    vector<Solution> solutions;
    vector<vector<int>> graph(n + 1);
    for (int i = 1; i <= n; i++) {
      if (alive.count(i) == 0) { continue; }
      graph[i] = vector<int>(all_neis[i].begin(), all_neis[i].end());
    }
    for (int phase = 0; phase < min(10, n); phase++) {
      vector<int> independent = IndependentRLeastDegreePow(graph, oldA, R, S);
      candsA.PB(independent);
      candsAszs.PB(independent.size());
      candsS.PB(S);
      candsS_vec.PB(vector<int>(S.begin(), S.end()));
      solutions.PB(Solution(graph, R, candsS_vec.back(), independent));
      int to_delete = 0;
      for (int v = 1; v <= n; v++) {
        if (S.count(v)) { continue; }
        if (to_delete == 0 || graph[to_delete].size() < graph[v].size()) {
          to_delete = v;
        }
      }
      S.insert(to_delete);
      for (auto& v : oldA) {
        if (v == to_delete) {
          swap(v, oldA.back());
          break;
        }
      }
      if (oldA.back() == to_delete) {
        oldA.pop_back();
      }
    }
    int who_biggest = 0;
    for (int ii = 1; ii < (int)candsA.size(); ii++) {
      if (solutions[who_biggest] < solutions[ii]) {
        who_biggest = ii;
      }
    }
    oldA = candsA[who_biggest];
    S = candsS[who_biggest];
    return {vector<int>(S.begin(), S.end()), oldA};
  }
  
    
};