#include "Headers.hpp"
#include "ReadTxt.hpp"
#include "FilesOps.hpp"
#include "ComputeWReach.hpp"

vector<vector<int>> ComputeAllWReach(vector<vector<int>>& graph,
                                     vector<int>& where_in_order,
                                     int R,
                                     vector<int> is_forb) {
  int n = graph.size() - 1;
  vector<int> last_vis(n + 1, -1);
  vector<int> dis(n + 1);
  vector<vector<int>> res(n + 1);
  for (int root = 1; root <= n; root++) {
    vector<int> cluster = ComputeSingleCluster(graph, where_in_order, R, is_forb, last_vis, dis, root, root);
    for (auto v : cluster) {
      res[v].PB(root);
    }
  }
  return res;
}

// this is needed for huge graphs to omit unnecessary O(n^2) memory
// if wcol is all we need
int ComputeWcol(vector<vector<int>>& graph, vector<int>& where_in_order, int R) {
  int n = graph.size() - 1;
  vector<int> last_vis(n + 1, -1);
  vector<int> dis(n + 1);
  vector<int> wreach_sz(n + 1);
  vector<int> is_forb;
  for (int root = 1; root <= n; root++) {
    vector<int> cluster = ComputeSingleCluster(graph, where_in_order, R, is_forb, last_vis, dis, root, root);
    for (auto v : cluster) {
      wreach_sz[v]++;
    }
  }
  int res = 0;
  for (auto x : wreach_sz) {
    res = max(x, res);
  }
  return res;
}

vector<int> ComputeSingleCluster(vector<vector<int>>& graph,
                                 vector<int>& where_in_order,
                                 int R,
                                 vector<int>& is_forb,
                                 vector<int>& last_vis,
                                 vector<int>& dis,
                                 int root,
                                 int phase_id) {
  vector<int> res;
  if (!is_forb.empty() && is_forb[root]) { return {}; }
  last_vis[root] = phase_id;
  dis[root] = 0;
  vector<int> que{root};
  for (int ii = 0; ii < (int)que.size(); ii++) {
    int cur_v = que[ii];
    res.PB(cur_v);
    if (dis[cur_v] == R) { continue; }
    for (auto nei : graph[cur_v]) {
      if (last_vis[nei] != phase_id && where_in_order[nei] > where_in_order[root] && (is_forb.empty() || !is_forb[nei])) {
        last_vis[nei] = phase_id;
        que.PB(nei);
        dis[nei] = dis[cur_v] + 1;
      }
    }
  }
  return res;
}
  
vector<vector<int>> ComputeClustersFromWReach(vector<vector<int>>& wreach) {
  int n = wreach.size() - 1;
  vector<vector<int>> clusters(n + 1);
  for (int i = 1; i <= n; i++) {
    for (auto u : wreach[i]) {
      clusters[u].PB(i);
    }
  }
  return clusters;
}

int ComputeWcolFromWReach(vector<vector<int>>& wreach) {
  int wcol = 1;
  for (auto& v : wreach) {
    wcol = max(wcol, (int)v.size());
  }
  return wcol;
}

  