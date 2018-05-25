#pragma once

#include "Headers.hpp"
#include "ReadTxt.hpp"
#include "FilesOps.hpp"
#include "FlagParser.hpp"
#include "ComputeWReach.hpp"
#include "CommonGraph.hpp"

vector<int> Independent1LeastDegree(vector<vector<int>>& graph);

vector<int> IndependentRLeastDegreePow(vector<vector<int>>& orig_graph, vector<int> A, int R, unordered_set<int>& forb);