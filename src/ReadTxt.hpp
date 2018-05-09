#pragma once

#include "Headers.hpp"

struct GraphReader {
  map<string, int> shrink_indices;
  map<int, string> inv_shrinking;
  vector<vector<int>> ReadGraph(string filename);
  string GetOriginalFromMapped(int v);
  int GetMappedFromOriginal(string v);
};

pair<vector<int>, vector<int>> GetOrderAndWhInOrder(string filename, GraphReader& reader);