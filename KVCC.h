#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <cstring>
#include <algorithm>
#include <map>
#include <set>
#include <stack>
#include <queue>
using namespace std;

/*------struct------*/
struct TUG{
    map< unsigned, vector<unsigned> > E; //store edge
    set<unsigned> V; //store vnode

};
typedef pair<unsigned, unsigned> Edge; //egde
typedef pair<unsigned, unsigned> Pointer_Pair; //point pair

/*------consruct graph------*/
//parser line to split the source(s) and destnation(t)
void S2T(string line, unsigned&s, unsigned&t);

//trans txt to the undirect graph
int TXT2TUG(const char*path, TUG&G);

/*------some basic graph function------*/
//delete the edge(E) from the graph(G)
void DelE(TUG&G, Edge E);

//delete the vnode(id) from the graph(G)
void DelNID(TUG&G, unsigned id);

//get the min degree vnode of the graph(G)
unsigned Getmin(TUG G);

//use V_set(subV) make a subGraph of graph(G)
TUG SG(TUG G, set<unsigned> SubV);

//DFS to get V_set in the same connect graph
set<unsigned> Get_V(TUG G, unsigned s);

//reverse the edge
void GR(TUG&G, vector<unsigned>path);

//construct the direct_flow_graph of G
TUG U2D(TUG G);

/*------K-VCC functions------*/
//calculate the kvccs
vector <set<unsigned> > KVCC_ENUM(TUG&G);

//get the connect subGraph
vector<TUG> Get_Connect(TUG G);

//do the work of overlap_partition
vector<TUG> Over_Partition(TUG G, vector<unsigned>& S);

//calculate the global cut
vector <unsigned> Global_Cut(TUG&G);

//calculate the vertex_cut of <s,t>
vector<unsigned> Loc_Cut(unsigned u, unsigned v,TUG, TUG);

//calculate the mincut of <s,t>
int GetMinCut(TUG G, unsigned s, unsigned t, unsigned maxflow, vector<Edge>&);

//calculate the flow( = edges number of cut)
vector<Pointer_Pair> GetMaxFlow(TUG G,unsigned s, unsigned t, unsigned &maxflow);

// get the path from s to t by BFS
vector<unsigned> Get_Path(TUG G, unsigned s, unsigned t);

/*------optimization functions------*/
//do the sweep work
void SWEEP(unsigned v, int* pru, int* deposit, TUG &G);

//judge if strong side_vertex retur 1=true 0=false
int Strong_Side_Vertex(TUG G, unsigned v);
