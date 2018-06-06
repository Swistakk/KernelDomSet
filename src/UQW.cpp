#include "UQW.hpp"

pair<vector<int>, vector<int>> UQWLeastDegreePower(vector<vector<int>>& graph, vector<int> oldA, int R) {
  unordered_set<int> S;
  vector<vector<int>> candsA;
  vector<int> candsAszs;
  vector<unordered_set<int>> candsS;
  vector<vector<int>> candsS_vec;
  vector<Solution> solutions;
  int n = (int)graph.size() - 1;
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
    if (!oldA.empty() && oldA.back() == to_delete) {
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