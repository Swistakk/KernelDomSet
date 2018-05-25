// File for common graph operations

#pragma once

#include "Headers.hpp"
#include "ReadTxt.hpp"
#include "FilesOps.hpp"
#include "FlagParser.hpp"
#include "ComputeWReach.hpp"
#include "UQWReviver.hpp"

vector<int> RNei(vector<vector<int>>& graph, int v, int R, unordered_set<int>& forb);

// Returns graph where u, v are connected iff dis(u, v) <= R
vector<vector<int>> PowerGraph(vector<vector<int>>& graph, int R, unordered_set<int>& forb);

vector<vector<int>> RDistanceProfile(vector<vector<int>>& graph, int R, vector<int>& forb, vector<int>& scat);

int UQWScore(vector<vector<int>>& graph, int R, vector<int>& forb, vector<int>& scat);

struct Solution {
  vector<int> forb, scat;
  int score;
  Solution(vector<vector<int>>& graph, int R, vector<int>& forb_, vector<int>& scat_);
  Solution();
  bool operator<(const Solution& oth) const;
  bool operator==(const Solution& oth) const;
};