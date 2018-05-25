#pragma once

#include "Headers.hpp"

vector<vector<int>> ComputeAllWReach(vector<vector<int>>& graph,
                                     vector<int>& where_in_order,
                                     int radius,
                                     vector<int> is_forb);

vector<int> ComputeSingleCluster(vector<vector<int>>& graph,
                                 vector<int>& where_in_order,
                                 int R,
                                 vector<int>& is_forb,
                                 vector<int>& last_vis,
                                 vector<int>& dis,
                                 int root,
                                 int phase_id);

vector<vector<int>> ComputeClustersFromWReach(vector<vector<int>>& wreach);

int ComputeWcolFromWReach(vector<vector<int>>& wreach);

int ComputeWcol(vector<vector<int>>& graph, vector<int>& where_in_order, int radius);