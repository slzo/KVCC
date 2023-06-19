#include "KVCC.h"
/*------contant------*/
int k; //the k of K-VCC
int maxid=0; //max nodeid

void P(TUG G) { //print grap
    cout << " G " << endl;
    for(auto v: G.E ) {
        cout << v.first << " : " ;
        for(auto e: v.second )
            cout << e << " ";
        cout << endl;
    }
}

void S2T(string S, unsigned&s, unsigned&t){ //parser S to split the source(s) and destnation(t)
    s = 0; t = 0;
    int len = S.length(), i;
    if(S[len-1] == '\r')
        len--;
    for( i = 0; i < len; i++ ) {
        if( S[i] == '\t' || S[i]==' ' )
            break;
        s *= 10;
        s += S[i]-'0';
    }
    for( i++; i < len; i++) {
        t *= 10;
        t += S[i]-'0';
    }
    return;
}

int TXT2TUG(const char*path, TUG&G){ //trans txt to the undirect graph
    ifstream file;
    file.open(path, ios::in);
    if( !file.is_open() )
        return -1;
    string strline;
    unsigned s, t;
    while(getline(file, strline)) { //parser and insert the edge
        S2T(strline, s, t); //parser the source and destnation
        G.E[s].push_back(t);
        G.E[t].push_back(s);
//          cout << s <<" " <<  t << endl;
    }

    for( auto i = G.E.begin(); i != G.E.end(); i++ ){ //delete repetition and init the V
        if(i->first > maxid)
            maxid = i->first;
        sort(i->second.begin(), i->second.end());
        i->second.erase(unique(i->second.begin(), i->second.end()), i->second.end());
        G.V.insert(i->first);
    }
    maxid = maxid*2 + 1;
//     cout << "TUG finished" << endl;
    return 0;
}

void DelE(TUG&G, Edge E){ //delete the edge(E) from the graph(G)
    unsigned s = E.first, t = E.second;
    for(auto i = G.E[s].begin(); i != G.E[s].end(); i++)
        if( *i == t ) {
            G.E[s].erase(i);
            if( G.E[s].empty() ) { //if only one edge, need to delete the node
                G.E.erase(s);
                G.V.erase(s);
            }
            break;
        }
}

void DelNID(TUG&G, unsigned id){ //delete the vnode(id) from the graph(G)
    for( auto i = G.E[id].begin(); i != G.E[id].end(); i++ ) //delete the edge <id,~>
        DelE(G, {*i, id});
    G.E.erase(id); //delete the node
    G.V.erase(id);
}

unsigned Getmin(TUG G) { //get the min degree vnode of the graph(G)
    unsigned min = G.V.size(), id;
    for( auto i = G.E.begin(); i != G.E.end(); i++ ){
        if(min > i->second.size()) {
            min = i->second.size();
            id = i->first;
        }
    }
    return id;
}

TUG SG(TUG G, set<unsigned> SubV) { //use V_set(subV) make a subGraph of graph(G)
    TUG SubG;
    for( auto i = SubV.begin(); i != SubV.end(); i++ ) {
        vector<unsigned> Ei = G.E[*i];
        for( auto j = Ei.begin(); j != Ei.end(); j++ )
            if( SubV.count(*j) ){
                SubG.E[*i].push_back(*j);
                SubG.E[*j].push_back(*i);
            }
    }
    for( auto i = SubG.E.begin(); i != SubG.E.end(); i++ ){ //delete repetition and init the V
        sort(i->second.begin(), i->second.end());
        i->second.erase(unique(i->second.begin(), i->second.end()), i->second.end());
        SubG.V.insert(i->first);
    }
    return SubG;
}

set<unsigned> Get_V(TUG G, unsigned s){ //DFS to get V_set in the same connect graph
    set<unsigned> re;
    stack<unsigned> S;
    S.push(s);
    while( !S.empty() ) {
        unsigned id = S.top(); S.pop();
        if( !re.count(id) ) {
            re.insert(id);
            for( auto i = G.E[id].begin(); i != G.E[id].end(); i++ )
                S.push(*i);
        }
    }
    return re;
}

TUG U2D(TUG G) { //construct the direct_flow_graph of G
    TUG TG;
    for( auto i = G.E.begin(); i != G.E.end(); i++ ){
        unsigned s = i->first;
        TG.E[s*2].push_back(s*2+1);
        for( auto j = i->second.begin(); j != i->second.end(); j++ )
            if(*j >= s){
                TG.E[s*2+1].push_back(*j*2);
                TG.E[*j*2+1].push_back(s*2);
            }
    }
    return TG;
}

void GR(TUG&G, vector<unsigned>path){ //reverse the edge
    for(auto i = 1; i < path.size()-2; i++) {
        unsigned s = path[i], t = path[i+1];
        for(auto i = G.E[s].begin(); i != G.E[s].end(); i++ )
            if( *i == t ) {
                G.E[s].erase(i);
                break;
            }
        G.E[t].push_back(s);
    }
    return;
}

vector <set<unsigned> > KVCC_ENUM(TUG&G){ //calculate the kvccs
    vector< set<unsigned> > kvccs;
    unsigned min = Getmin(G);
    while( G.E[min].size() < k ){ //delete the vertex(degree<k) to get k-core
        DelNID(G, min);
        min = Getmin(G);
    }
    auto Connects = Get_Connect(G);

    for( auto Gi = Connects.begin(); Gi != Connects.end(); Gi++ ){
        auto S = Global_Cut(*Gi);
        if( S.empty() ) kvccs.push_back(Gi->V);
        else{ //recursion
            auto GS = Over_Partition(*Gi, S);
            for( auto Gij = GS.begin(); Gij != GS.end(); Gij++ ) {
                auto Subkvccs = KVCC_ENUM(*Gij); //recursively calculate
                for( auto i = Subkvccs.begin(); i != Subkvccs.end(); i++ )
                    kvccs.push_back(*i);
            }
        }
    }
    return kvccs;
}

vector<TUG> Get_Connect(TUG G){ //get the connect subGraph
    vector<TUG> SubG;
    int visited[maxid+1];
    for(int i= 0; i < maxid; i++) visited[i] = 0;
    for( auto i:G.V ) { //put V to a subGraph
        if( visited[i] )
            continue;
        set<unsigned> SubV = Get_V(G,i);
        visited[i]=1;
        for(auto j:SubV) visited[j] = 1;
        SubG.push_back(SG(G,SubV));
    }
    return SubG;
}

vector<TUG> Over_Partition(TUG G, vector<unsigned>& S){ //do the work of overlap_partition
    vector<TUG> re;
    for(auto i = S.begin(); i != S.end(); i++)
        G.V.erase(*i);
    auto SubG = SG(G, G.V); //calculate G*=G-S
    auto Connects = Get_Connect(SubG);
    for( auto Gi = Connects.begin(); Gi != Connects.end(); Gi++ ) {
        auto Vi = Gi->V;
        for( auto vi = S.begin(); vi != S.end(); vi++ )
            Vi.insert(*vi);
        re.push_back(SG(G,Vi)); //calculate G[V(Gi)+S]
    }
    return re;

}

vector <unsigned> Global_Cut(TUG&G){ //calculate the global cut
    TUG TG = U2D(G); //construct direct flow grap of G
    unsigned u = Getmin(G); //select u with the minimum degree
    int *pru = new int [maxid+1];
    int *deposit = new int[maxid+1];
    for( int i = 0; i <= maxid; i++ )
        pru[i] = deposit[i] = 0;

    SWEEP(u, pru, deposit, G);
    //DFS get other node in the connect graph in a non_ascending order
    set<unsigned>  Vs = Get_V(G, u);
    //step1: find the min_cut of <u,others>
    for( auto v:Vs ){
        if( pru[v] )
            continue;
        auto S = Loc_Cut(u, v, TG, G);
        if( !S.empty() ) return S;
        SWEEP(v, pru, deposit, G);
    }
    //step2: find the min_cut of <u,neighbors>
    if( !Strong_Side_Vertex(G, u) )
        for( auto i = G.E[u].begin(); i != G.E[u].end(); i++ )
            for( auto j = G.E[u].begin(); j != G.E[u].end(); j++ ) {
                if( i == j ) continue;
                auto S = Loc_Cut(*i, *j, TG, G);
                if( !S.empty() )
                    return S;
            }
    return {};
}

vector<unsigned> Loc_Cut(unsigned u, unsigned v, TUG TG, TUG G) { //calculate the vertex_cut of <s,t>
    if( u == v ) return{};
    for( auto i:G.E[u] ) if( i == v ) return {};
    vector<unsigned> re;

    unsigned maxflow = 0;
    u*=2; v*= 2; v+=1;
    GetMaxFlow(TG, u, v, maxflow);
    if(maxflow > k) return {};
    vector<Edge> MinCut;
    GetMinCut(TG,  u, v, maxflow, MinCut);
    for(auto i:MinCut)
        re.push_back(i.first/2);
    return re;
}

int GetMinCut(TUG G, unsigned s, unsigned t, unsigned maxflow, vector<Edge>&cut){
//calculate the maximum flow and min_cut_set(store int (set)cut) from s to t
    unsigned flow;
    auto Es = GetMaxFlow(G,s,t,flow); //calculate the maxflow of <s,t> in the graph
    sort(Es.begin(), Es.end());
    if(flow==0) return 1;//get the cut,stop working
    vector<Edge> CutCond;
    for( auto i: Es ) {
        if( i.first==s || i.second==t || i.first%2 )
            continue;
        CutCond.push_back(i);
    }
    while( !CutCond.empty() ) { //delete condidate edge and recursively GetMinCut
        auto e = CutCond.back();  CutCond.pop_back();
        cut.push_back(e);
        if( cut.size() > maxflow ) {
            cut.pop_back();
            break;
        }
        TUG Gc = G; DelE(Gc, e);
        if(GetMinCut(Gc, s, t, maxflow, cut)) //recursively GetMinCut
            return 1;
    }
    cut.pop_back();
    return 0;
}

vector<Pointer_Pair> GetMaxFlow(TUG G,unsigned s, unsigned t, unsigned &maxflow) { //calculate the flow( = edges number of cut)
    vector<Pointer_Pair> flow;
    maxflow = 0;
    auto path = Get_Path(G,s,t);
    if(!path.empty()) maxflow++;
    while( !path.empty() ) { //repeat BFS get path between s and t and delete path until no path
        maxflow++;
        for( auto i = 1; i < path.size()-2; i++ ) {
            auto a=path[i], b = path[i+1];
            flow.push_back({a,b});
        }
        GR(G,path);
        path = Get_Path(G,s,t);
    }
    return flow;
}

vector<unsigned> Get_Path(TUG G, unsigned s, unsigned t) { // get the path from s to t
    if(s == t) return {s};
    int *visited = new int[maxid+1];
    unsigned *prev = new unsigned[maxid+1];
    for( int i = 0; i <= maxid; i++ ) {
        visited[i] = 0;
        prev[i] = -1;
    }
    queue<unsigned> Q;
    Q.push(s);
    while(!Q.empty()) { //BFS from s until reach t
        unsigned v = Q.front(); Q.pop();
        for( auto neg: G.E[v] ) {
            if(!visited[neg]) {
                visited[neg] = 1;
                prev[neg] = v;
                Q.push(neg);
            }
            if( neg == t )
                break;
        }
    }
    if(prev[t] == -1)
        return {};
    vector<unsigned> path;
    stack<unsigned> trans_path;
    unsigned nodeid = t;
    while( nodeid != s ) { //use stack to store the path transpose
        trans_path.push(nodeid);
        nodeid = prev[nodeid];
    }
    path.push_back(s);
    while( !trans_path.empty() ) { //check the stack to get the path
        path.push_back(trans_path.top());
        trans_path.pop();
    }
    return path;
}

void SWEEP(unsigned v, int* pru, int* deposit, TUG &G){ //do the sweep work
    pru[v] = 1;
    for(auto i = G.E[v].begin(); i != G.E[v].end(); i++ ) {
        if(pru[*i])
            continue;
        deposit[*i]++;
        if(Strong_Side_Vertex(G, v) || deposit[*i] >= k) //two optimization
            SWEEP(*i, pru, deposit, G);
    }
}

int Strong_Side_Vertex(TUG G, unsigned u){ //judge if strong side_vertex retur 1=true 0=false
    for(auto i = G.E[u].begin(); i != G.E[u].end(); i++) {
        for(auto j = i+1; j != G.E[u].end(); j++) { //double forloop to check every two neighbor
            auto Ni = G.E[*i]; auto Nj = G.E[*j];
            set<int> intersection;
            //calculate intersection
            set_intersection(Ni.begin(), Ni.end(), Nj.begin(), Nj.end(), inserter(intersection, intersection.begin()));
            if(intersection.size() < k)
                return 0;
        }
    }
    return 1;
}

int main() {
    const char path[] = "./Input/MiniDataSet.txt";
    TUG G;
    TXT2TUG(path,G);
    cin >> k;
    auto kvccs  = KVCC_ENUM(G);
    cout << "VCC_Num: " << kvccs.size() << endl;
    auto id  = 1;
    for(auto kvcc: kvccs) {
        cout << "K-VCC(No." << id << "): node_nums = " << kvcc.size() << endl;
        for(auto v: kvcc)
            cout << v << endl;
        id++;
    }
    return 0;
}







