#pragma once
#include "Headers.hpp"

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
      for (auto u : N3) {
        Kill(u);
      }
      for (auto u : N2) {
        if (all_neis[v].count(u) && all_neis[w].count(u)) {
          Kill(u);
        }
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
};