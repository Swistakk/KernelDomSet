#include "Headers.hpp"

vector<int> ReviveRedundantForb(vector<vector<int>>& graph, int R, vector<int> forb, vector<int>& scat) {
  int n = (int)graph.size() - 1;
  vector<int> forb_mask(n + 1), scat_mask(n + 1);
  for (auto v : forb) {
    forb_mask[v] = 1;
  }
  for (auto v : scat) {
    scat_mask[v] = 1;
  }
  random_shuffle(forb.begin(), forb.end());

  vector<int> dis(n + 1);
  vector<int> last_vis(n + 1);
  int filtered_forb_sz = (int)forb.size();
  for (auto to_add : forb) {
    vector<int> que{to_add};
    last_vis[to_add] = to_add;
    dis[to_add] = 0;
    int found_scat = 0;
    int closest_guy = 0;
    bool fail = false;
    for (int ii = 0; ii < (int)que.size(); ii++) {
      int v = que[ii];
      if (found_scat == 0 && R < 2 * (dis[v] + 1)) { break; }
      if (found_scat == 1 && R < closest_guy + dis[v] + 1) { break; }
      for (auto nei : graph[v]) {
        if (forb_mask[nei]) { continue; }
        if (last_vis[nei] == to_add) { continue; }
        last_vis[nei] = to_add;
        dis[nei] = dis[v] + 1;
        que.PB(nei);
        if (scat_mask[nei]) {
          if (found_scat) {
            assert(closest_guy + dis[nei] <= R);
            fail = true;
            break;
          } else {
            closest_guy = dis[nei];
            found_scat = 1;
          }
        }
      }
      if (fail) { break; }
    }
    if (!fail) {
      forb_mask[to_add] = 0;
    }
  }

  vector<int> filtered_forb;
  for (auto v : forb) {
    if (forb_mask[v]) {
      filtered_forb.PB(v);
    }
  }
  //debug(forb.size(), filtered_forb.size());
  return filtered_forb;
}
  