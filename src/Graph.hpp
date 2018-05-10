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
  Graph(int n_) {
    n = n_;
    all_neis.resize(n + 1);
    black_neis.resize(n + 1);
    white_neis.resize(n + 1);
    is_white.resize(n + 1);
    for (int i = 1; i <= n; i++) {
      alive.insert(i);
    }
  }
  void AddEdge(int a, int b) {
    black_neis[a].insert(b);
    black_neis[b].insert(a);
    all_neis[a].insert(b);
    all_neis[b].insert(a);
  }
  void ColorWhite(int v) {
    debug("to white ", v);
    for (auto nei : all_neis[v]) {
      black_neis[nei].erase(v);
      white_neis[nei].insert(v);
    }
    is_white[v] = 1;
  }
  void Kill(int v) {
    debug("kill ", v);
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
    debug("rem edge", a, b);
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
        debug("here1", nei);
        Kill(v);
        return;
      }
    }
    for (auto nei : black_neis[v]) {
      for (auto neinei : all_neis[nei]) {
        if (checked.count(neinei) || neinei == v) { continue; }
        checked.insert(neinei);
        if (AreBlackContained(v, neinei)) {
          debug("here2", neinei);
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
        debug("superset ", u, " by ", nei);
        ColorWhite(u);
        return;
      }
    }
  }
  void PutDomset(int u) {
    debug("put ", u);
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
    assert(nxt_ind <= 25);
    vector<int> dp(1 << nxt_ind, n);
    dp[0] = 0;
    for (auto v : alive) {
      int covered = 0;
      for (auto nei : black_neis[v]) {
        covered += 1 << shrink_black_ind[nei];
      }
      if (!is_white[v]) {
        covered += 1 << shrink_black_ind[v];
      }
      for (int prv_mask = 0; prv_mask < 1 << nxt_ind; prv_mask++) {
        dp[prv_mask | covered] = min(dp[prv_mask | covered], dp[prv_mask] + 1);
      }
    }
    return dp.back() + domset.size();
  }
};