#include "ReadTxt.hpp"
#include "FilesOps.hpp"

vector<vector<int>> GraphReader::ReadGraph(string filename) {
  ifstream in;
  InitIfstream(in, filename);
  string line;
  vector<pair<string, string>> edges;
  while (getline(in, line)) {
    if (line[0] == '#') { continue; }
    stringstream stream(line);
    string a, b;
    stream >> a >> b;
    shrink_indices[a] = shrink_indices[b] = 1;
    edges.push_back({a, b});
  }
  in.close();
  int n = 0;
  for (auto& p : shrink_indices) {
    n++;
    p.nd = n;
    inv_shrinking[p.nd] = p.st;
  }
  vector<vector<int>> graph(n + 1);
  for (auto e : edges) {
    graph[shrink_indices[e.nd]].PB(shrink_indices[e.st]);
    graph[shrink_indices[e.st]].PB(shrink_indices[e.nd]);
  }
  return graph;
}

string GraphReader::GetOriginalFromMapped(int ind) {
  if (inv_shrinking.count(ind) == 0) { assert(false); }
  return inv_shrinking[ind];
}

int GraphReader::GetMappedFromOriginal(string ind) {
  if (shrink_indices.count(ind) == 0) { debug(ind); assert(false); }
  return shrink_indices[ind];
}

pair<vector<int>, vector<int>> GetOrderAndWhInOrder(string filename, GraphReader& reader) {
  int n = reader.shrink_indices.size();
  vector<int> order;
  ifstream oin;
  InitIfstream(oin, filename);
  vector<int> where_in_order(n + 1);
  string v;
  int i = 0;
  while (oin >> v) {
    //debug(i, v);
    int mapped = reader.GetMappedFromOriginal(v);
    assert(mapped != -1 && where_in_order[mapped] == 0);
    order.PB(mapped);
    where_in_order[mapped] = i;
    i++;
  }
  oin.close();
  return {order, where_in_order};
}