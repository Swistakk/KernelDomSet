#include "Independent.hpp"

vector<int> Independent1LeastDegree(vector<vector<int>>& graph) {
  set<pair<int, int>> que;
  int n = (int)graph.size() - 1;
  vector<int> degree(n + 1);
  vector<bool> alive(n + 1);
  for (int v = 1; v <= n; v++) {
    degree[v] = (int)graph[v].size();
    alive[v] = true;
    que.insert({degree[v], v});
  }
  auto DecDegree = [&](int v) {
    que.erase({degree[v], v});
    degree[v]--;
    que.insert({degree[v], v});
  };
  auto Remove = [&](int v) {
    que.erase({degree[v], v});
    assert(alive[v]);
    alive[v] = false;
    for (auto nei : graph[v]) {
      if (alive[nei]) {
        DecDegree(nei);
      }
    }
  };
  vector<int> independent;
  while (!que.empty()) {
    auto least_degree_it = que.begin();
    pair<int, int> least_degree = *least_degree_it;
    int v = least_degree.nd;
    independent.PB(v);
    Remove(v);
    for (auto nei : graph[v]) {
      if (alive[nei]) {
        Remove(nei);
      }
    }
  }
  return independent;
}

vector<int> IndependentRLeastDegreePow(vector<vector<int>>& orig_graph, vector<int> A, int R, unordered_set<int>& forb) {
  if (A.empty()) { return {}; }
  vector<vector<int>> pow_graph = PowerGraph(orig_graph, R, forb);
  int n = (int)orig_graph.size() - 1;
  vector<int> forw_mapping(n + 1);
  vector<int> backw_mapping(n + 1);
  for (int ii = 0; ii < (int)A.size(); ii++) {
    forw_mapping[A[ii]] = ii + 1;
    backw_mapping[ii + 1] = A[ii];
  }
  vector<vector<int>> graph_A((int)A.size() + 1);
  for (int v = 1; v <= n; v++) {
    if (forw_mapping[v] == 0) { continue; }
    for (auto nei : pow_graph[v]) {
      if (forw_mapping[nei] == 0) { continue; }
      graph_A[forw_mapping[v]].PB(forw_mapping[nei]);
    }
  }
  vector<int> independent = Independent1LeastDegree(graph_A);
  for (auto& v : independent) {
    assert(backw_mapping[v] != 0);
    v = backw_mapping[v];
  }
  return independent;
}
