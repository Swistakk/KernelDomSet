// File for common graph operations

#include "CommonGraph.hpp"

// if performance critical and used many times - it can be rewritten with
// vectors instead of maps and sets
vector<int> RNei(vector<vector<int>>& graph, int root, int R, unordered_set<int>& forb) {
  unordered_map<int, int> dis;
  vector<int> que{root};
  dis[root] = 0;
  assert(forb.count(root) == 0);
  for (int ii = 0; ii < (int)que.size(); ii++) {
    int v = que[ii];
    if (dis[v] == R) { return que; }
    for (auto nei : graph[v]) {
      if (dis.count(nei)) { continue; }
      if (forb.count(nei)) { continue; }
      dis[nei] = dis[v] + 1;
      que.PB(nei);
    }
  }
  return que; // should happen only if whole graph is reachable in less than R steps
}

// Returns graph where u, v are connected iff dis(u, v) <= R
vector<vector<int>> PowerGraph(vector<vector<int>>& graph, int R, unordered_set<int>& forb) {
  int n = (int)graph.size() - 1;
  vector<vector<int>> pow_graph(n + 1);
  vector<int> last_vis(n + 1);
  vector<int> dis(n + 1);
  vector<int> where_in_order(n + 1);
  vector<int> is_forb;
  if (!forb.empty()) {
    is_forb.resize(n + 1);
    for (auto v : forb) {
      is_forb[v] = 1;
    }
  }
  for (int root = 1; root <= n; root++) {
    where_in_order[root] = -1; // hack, to make it think root is before everybody  in order
    pow_graph[root] = ComputeSingleCluster(graph, where_in_order, R, is_forb, last_vis, dis, root, root);
    where_in_order[root] = 0;
  }
  return pow_graph;
}

// why did I have to make it forb scat instead of scat forb :(?
vector<vector<int>> RDistanceProfile(vector<vector<int>>& graph, int R, vector<int>& forb, vector<int>& scat) {
  int n = (int)graph.size() - 1;
  //vector<int> scat_ind(n + 1, -1);
  vector<vector<int>> profiles((int)scat.size());
  vector<int> last_vis(n + 1);
  vector<int> dis(n + 1);
  for (auto root : forb) {
    vector<int> que{root};
    last_vis[root] = root;
    dis[root] = 0;
    for (int ii = 0; ii < (int)que.size(); ii++) {
      int cur_v = que[ii];
      if (dis[cur_v] == R) { break; }
      for (auto nei : graph[cur_v]) {
        if (last_vis[nei] != root) {
          last_vis[nei] = root;
          dis[nei] = dis[cur_v] + 1;
          que.PB(nei);
        }
      }
    }
    for (int ii = 0; ii < (int)scat.size(); ii++) {
      int v = scat[ii];
      if (last_vis[v] != root) {
        profiles[ii].PB(R + 1);
      } else {
        profiles[ii].PB(dis[v]);
      }
    }
  }
  return profiles;
}
    
int UQWScore(vector<vector<int>>& graph, int R, vector<int>& forb, vector<int>& scat) {
  map<vector<int>, int> cnt_profiles;
  vector<vector<int>> profiles = RDistanceProfile(graph, R, forb, scat);
  for (auto& profile : profiles) {
    cnt_profiles[profile]++;
  }
  int biggest_class = 0;
  for (auto& p : cnt_profiles) {
    if (p.nd > biggest_class) {
      biggest_class = p.nd;
    }
  }
  return biggest_class;
}

Solution::Solution(vector<vector<int>>& graph, int R, vector<int>& forb_, vector<int>& scat_) {
  forb = ReviveRedundantForb(graph, R, forb_, scat_);
  scat = scat_;
  score = UQWScore(graph, R, forb, scat);
}

Solution::Solution() {
  score = -1;
}

bool Solution::operator<(const Solution& oth) const {
  return score < oth.score;
}

bool Solution::operator==(const Solution& oth) const {
  return score == oth.score;
}

