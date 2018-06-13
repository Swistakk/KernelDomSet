#include <iostream>
#include "Graph.hpp"
#include "FlagParser.hpp"
using namespace std;

void Err() {
  cerr<<"Usage: --in=graph.txtg --sparsity=on/off --alber=none/one/pair/both"<<endl;
  cerr<<"--h for help\n";
  exit(1);
}

int main(int argc, char** argv) {
  if (argc == 2 && string(argv[1]) == "--h") {
    cerr<<"Usage: --in=graph.txtg --sparsity=on/off --alber=none/one/pair/both"<<endl;
    return 0;
  }
  bool sparsity_mode = false;
  bool alber_one = false;
  bool alber_pair = false;
  string graph_file;
  try {
    FlagParser flag_parser;
    flag_parser.ParseFlags(argc, argv);
    graph_file = flag_parser.GetFlag("in", true);
    string sparsity = flag_parser.GetFlag("sparsity", true);
    string alber = flag_parser.GetFlag("alber", true);
    if (sparsity == "on") {
      sparsity_mode = true;
    } else if (sparsity != "off") {
      Err();
    }
    if (alber == "both") {
      alber_one = true;
      alber_pair = true;
    } else if (alber == "one") {
      alber_one = true;
    } else if (alber == "pair") {
      alber_pair = true;
    } else if (alber != "none") {
      Err();
    }
  } catch (string err) {
    cerr<<"Error: "<<err<<endl;
    Err();
  }
  
  GraphReader reader;
  vector<vector<int>> graph_vecs = reader.ReadGraph(graph_file);
  int n = (int)graph_vecs.size() - 1;
  int m = 0;
  Graph graph(n);
  for (int v = 1; v <= n; v++) {
    for (auto nei : graph_vecs[v]) {
      if (v < nei) {
        m++;
        graph.AddEdge(v, nei);
      }
    }
  }
  
  graph.Reduce(sparsity_mode, alber_one, alber_pair);
  int aft_n = (int)graph.alive.size();
  int aft_m = 0;
  for (auto v : graph.alive) {
    aft_m += (int)graph.all_neis[v].size();
  }
  aft_m /= 2;
  cout<<n<<" "<<m<<" "<<aft_n<<" "<<aft_m<<endl;
  //debug(n, m, aft_n, aft_m);
  //graph.Debug();
  //debug(graph.domset);
  // print something to output file
  
}
  
  
  