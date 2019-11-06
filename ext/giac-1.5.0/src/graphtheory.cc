/*
 * graphtheory.cc
 *
 * (c) 2018 Luka Marohnić
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "giacPCH.h"
#include "giac.h"
#include "graphe.h"
#include "graphtheory.h"

using namespace std;

#ifndef NO_NAMESPACE_GIAC
namespace giac {
#endif // ndef NO_NAMESPACE_GIAC

/* error messages */
static const char *gt_error_messages[] = {
    "Unknown error",                                                    //  0
    "Argument is not a graph",                                          //  1
    "Weighted graph is required",                                       //  2
    "Unweighted graph is required",                                     //  3
    "Directed graph is required",                                       //  4
    "Undirected graph is required",                                     //  5
    "does not specify an edge",                                         //  6
    "Mixing edges and arcs not allowed",                                //  7
    "Weight/adjacency matrix must be symmetric for undirected graphs",  //  8
    "Failed to read graph from file",                                   //  9
    "Edge not found",                                                   // 10
    "Vertex not found",                                                 // 11
    "Graph is not a tree",                                              // 12
    "Exactly one root node must be specified per connected component",  // 13
    "Invalid root node specification",                                  // 14
    "Graph is not planar",                                              // 15
    "Connected graph required",                                         // 16
    "Invalid drawing method specification",                             // 17
    "does not specify a cycle in the given graph",                      // 18
    "No cycle found",                                                   // 19
    "Graph name is not recognized",                                     // 20
    "Argument is not a subgraph",                                       // 21
    "Graph is null",                                                    // 22
    "Expected \"tag\"=value pair",                                      // 23
    "Argument is not a valid graphic sequence",                         // 24
    "Graph is not acyclic",                                             // 25
    "Biconnected graph required",                                       // 26
    "Graph is not bipartite",                                           // 27
    "Wrong number of arguments",                                        // 28
    "Positive integer required",                                        // 29
    "Invalid vertex specification",                                     // 30
};

gen generr(const char* msg) {
    string m(msg);
    m.append("\n");
    return gensizeerr(m.c_str());
}

gen generrtype(const char* msg) {
    string m(msg);
    m.append("\n");
    return gentypeerr(m.c_str());
}

gen generrdim(const char* msg) {
    string m(msg);
    m.append("\n");
    return gendimerr(m.c_str());
}

gen gt_err(int code) {
    return generr((string(gt_error_messages[code])).c_str());
}

gen gt_err(const gen &g,int code) {
    stringstream ss;
    ss << g << ": " << gt_error_messages[code];
    return generr(ss.str().c_str());
}

void identifier_assign(const identificateur &var,const gen &value,GIAC_CONTEXT) {
    _eval(symbolic(at_sto,makesequence(value,var)),contextptr);
}

bool has_suffix(const string &str,const string &suffix)
{
    return str.size()>=suffix.size() &&
            str.compare(str.size()-suffix.size(),suffix.size(),suffix)==0;
}

string strip_string(const string &str) {
    string res(str);
    int i=0;
    for (;res[i]==' ';++i);
    res=res.substr(i);
#if 1
    i=int(res.size())-1;
    for (;i>=0;--i){
      if (res[i]!=' ')
   break;
    }
    res=res.substr(0,i+1);
#else
    while (res.back()==' ') {
        res.pop_back();
    }
#endif
    return res;
}

string make_absolute_file_path(const string &relative_path) {
    if (relative_path[0]=='/')
        return relative_path;
#ifdef HAVE_NO_CWD
    string path="/"+relative_path;
#else
    string path=string(getcwd(0,0))+"/"+relative_path;
#endif
    vector<string> tokens;
    size_t lastpos=0,pos;
    while ((pos=path.find_first_of('/',lastpos))!=string::npos) {
        tokens.push_back(path.substr(lastpos,pos-lastpos));
        lastpos=pos+1;
    }
    tokens.push_back(path.substr(lastpos));
    vector<string>::iterator it;
    for (it=tokens.begin();it!=tokens.end();++it) {
        *it=strip_string(*it);
    }
    int i;
    while ((it=find(tokens.begin(),tokens.end(),string("..")))!=tokens.end()) {
        if (it==tokens.begin())
            return path;
        i=it-tokens.begin()-1;
        tokens.erase(it);
        tokens.erase(tokens.begin()+i);
    }
    string res;
    for (it=tokens.begin();it!=tokens.end();++it) {
        res=res+*it;
        if (it+1!=tokens.end())
            res=res+"/";
    }
    return res;
}

/* returns true iff g is a graph and writes the basic info to 'display_str' */
bool is_graphe(const gen &g,string &disp_out,GIAC_CONTEXT) {
    if (g.type!=_VECT || g.subtype!=_GRAPH__VECT)
        return false;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return false;
    int nv=G.node_count(),ne=G.edge_count();
    stringstream ss;
    ss << nv;
    string nvert(ss.str());
    ss.str("");
    ss << ne;
    bool isdir=G.is_directed();
    string nedg(ss.str());
    string dir_spec=isdir?"directed":"undirected";
    string weight_spec=G.is_weighted()?"weighted":"unweighted";
    nvert=nvert+(nv==1?" vertex":" vertices");
    nedg=nedg+(isdir?(ne==1?" arc":" arcs"):(ne==1?" edge":" edges"));
    disp_out.clear();
    string name=G.name();
    if (!name.empty())
        disp_out=name+": ";
    disp_out=disp_out+(isdir?"a ":"an ")+dir_spec+" "+weight_spec+" graph with "+nvert+" and "+nedg;
    return true;
}

/* evaluates the given command with the given inputs and returns the result */
gen gt_command(gen (*gtfunc)(const gen &,const context *),const char *args,GIAC_CONTEXT) {
    return (*gtfunc)(graphe::str2gen(args),contextptr);
}

bool vertices_from_integer_or_vecteur(const gen &g,graphe &G) {
    vecteur V;
    int n;
    if (g.is_integer()) {
        n=g.val;
        if (n<1)
            return false;
        G.make_default_labels(V,n);
    } else if (g.type==_VECT) {
        V=*g._VECTptr;
        if (V.empty())
            return false;
        n=V.size();
    } else return false;
    G.add_nodes(V);
    return true;
}

void parse_trail(graphe &G,const gen &g) {
    assert(g.is_symb_of_sommet(at_trail));
    vecteur &trail=*g._SYMBptr->feuille._VECTptr;
    int n=trail.size();
    for (int i=0;i<n-1;++i) {
        G.add_edge(trail[i],trail[i+1]);
    }
}

bool parse_vertex_colors(graphe &G,const gen &g,const graphe::ivector &nodes=graphe::ivector(0)) {
    if (g.type==_VECT) {
        assert(int(g._VECTptr->size()==nodes.empty()?G.node_count():int(nodes.size())));
        int k=0;
        for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
            if (!it->is_integer())
                return false;
            G.set_node_attribute(nodes.empty()?k:nodes[k],_GT_ATTRIB_COLOR,it->val);
            ++k;
        }
    } else if (g.is_integer()) {
        if (g.val<0)
            return false;
        if (nodes.empty()) {
            for (int k=G.node_count();k-->0;)
                G.set_node_attribute(k,_GT_ATTRIB_COLOR,g.val);
        } else {
            for (graphe::ivector_iter it=nodes.begin();it!=nodes.end();++it) {
                G.set_node_attribute(*it,_GT_ATTRIB_COLOR,g.val);
            }
        }
    } else return false;
    return true;
}

bool parse_vertex_coordinates(graphe &G,const vecteur &v,bool &size_error) {
    vecteur c;
    int i=0,dim=-1;
    for (const_iterateur it=v.begin();it!=v.end();++it) {
        if (it->is_symb_of_sommet(at_point))
            c=*it->_SYMBptr->feuille._VECTptr;
        else if (it->type==_CPLX) {
            c.resize(2);
            c[0]=*it->_CPLXptr;
            c[1]=*(it->_CPLXptr+1);
        } else if (it->type==_VECT) {
            c=*it->_VECTptr;
        } else return false;
        if (dim<0)
            dim=c.size();
        else if (dim!=int(c.size())) {
            size_error=true;
            return false;
        }
        G.set_node_attribute(i++,_GT_ATTRIB_POSITION,c);
    }
    return true;
}

bool parse_matrix(graphe &G,const matrice &m,bool is_weight_matrix,int mode,bool &size_error) {
    int n=m.size();
    size_error=false;
    if (int(m.front()._VECTptr->size())!=n || (mode>0 && G.node_count()!=n)) {
        size_error=true;
        return false;
    }
    if (!has_num_coeff(_evalf(m,context0)))
        return false;
    bool isdir=G.is_directed() || m!=mtran(m),isweighted=is_weight_matrix;
    if (mode==0) {
        vecteur V;
        G.make_default_labels(V,n);
        G.add_nodes(V);
    }
    if (mode<2) {
        G.set_directed(isdir);
        for (int i=0;i<n;++i) {
            for (int j=isdir?0:i+1;j<n;++j) {
                if (i==j)
                    continue;
                const gen &w=m[i][j];
                if (!is_zero(w)) {
                    G.add_edge(i,j);
                    if (!is_one(w))
                        isweighted=true;
                }
            }
        }
    }
    if (isweighted)
        G.make_weighted(m);
    return true;
}

bool parse_edge_with_weight(graphe &G,const vecteur &E) {
    if (E.size()!=2)
        return false;
    const vecteur &e=*E.front()._VECTptr;
    const gen &w=E.back();
    if (e.size()!=2)
        return false;
    if (!G.is_weighted())
        G.set_weighted(true);
    if (e.front()==e.back())
        return false;
    G.add_edge(e.front(),e.back(),w);
    return true;
}

bool parse_edges(graphe &G,const vecteur &E,bool is_set) {
    if (is_set) {
        for (const_iterateur it=E.begin();it!=E.end();++it) {
            if (it->type!=_VECT || it->_VECTptr->size()!=2)
                return false;
            if (it->_VECTptr->front().type!=_VECT) {
                if (it->_VECTptr->front()==it->_VECTptr->back())
                    return false;
                G.add_edge(it->_VECTptr->front(),it->_VECTptr->back());
            } else {
                if (!parse_edge_with_weight(G,*it->_VECTptr))
                    return false;
            }
        }
    } else {
        int n=E.size();
        if (n<2)
            return false;
        if (E.front().type==_VECT) {
            if (!parse_edge_with_weight(G,E))
                return false;
        } else {
            for (int i=0;i<n-1;++i) {
                if (E[i]==E[i+1])
                    return false;
                G.add_edge(E[i],E[i+1]);
            }
        }
    }
    return true;
}

bool delete_edges(graphe &G,const vecteur &E) {
    if (ckmatrix(E)) {
        if (E.front()._VECTptr->size()!=2)
            return false;
        for (const_iterateur it=E.begin();it!=E.end();++it) {
            int i=G.node_index(it->_VECTptr->front()),j=G.node_index(it->_VECTptr->back());
            if (i>=0 && j>=0)
                G.remove_edge(i,j);
        }
    } else {
        int n=E.size();
        if (n<2)
            return false;
        for (int k=0;k<n-1;++k) {
            int i=G.node_index(E[k]),j=G.node_index(E[k+1]);
            G.remove_edge(i,j);
        }
    }
    return true;
}

gen flights(const gen &g,bool arrive,bool all,GIAC_CONTEXT) {
    if (!all && g._VECTptr->front().type!=_VECT)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(all?g:g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    int i=0;
    if (!all) {
        i=G.node_index(g._VECTptr->at(1));
        if (i==-1)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    }
    vecteur res;
    graphe::ivector adj;
    do {
        G.adjacent_nodes(i,adj);
        vecteur v;
        for (graphe::ivector::const_iterator it=adj.begin();it!=adj.end();++it) {
            if (G.has_edge(arrive?*it:i,arrive?i:*it))
                v.push_back(G.node_label(*it));
        }
        if (!all)
            return _sort(v,contextptr);
        res.push_back(_sort(v,contextptr));
    } while (++i<G.node_count());
    return change_subtype(res,_LIST__VECT);
}

gen randomgraph(const vecteur &gv,bool directed,GIAC_CONTEXT) {
    graphe G(contextptr);
    vecteur V;
    if (gv.front().type==_VECT)
        V=*gv.front()._VECTptr;
    else if (gv.front().is_integer())
        G.make_default_labels(V,gv.front().val);
    else
        return gentypeerr(contextptr);
    int n=V.size();
    G.reserve_nodes(n);
    G.add_nodes(V);
    if (gv.size()==2) {
        gen p=_evalf(gv.back(),contextptr);
        if (p.type==_DOUBLE_) {
            if (!is_strictly_positive(p,contextptr))
                return gentypeerr("Expected a positive constant");
            G.set_directed(directed);
            G.erdos_renyi(p.DOUBLE_val());
        } else {
            vecteur P;
            if (directed)
                return generr("This method cannot generate digraphs");
            if (gv.back().type==_VECT) {
                P=*gv.back()._VECTptr;
            } else if (gv.back().is_symb_of_sommet(at_program) || gv.back().type==_FUNC) {
                vecteur L(n);
                for (int i=0;i<n;++i) L[i]=i;
                P=*_apply(makesequence(gv.back(),L),contextptr)._VECTptr;
            }
            for (iterateur it=P.begin();it!=P.end();++it) {
                if (!is_real(*it,contextptr) || !is_positive(*it,contextptr))
                    return generr("Weights must be nonnegative real numbers");
                *it=_evalf(*it,contextptr);
            }
            G.molloy_reed(P);
        }
    } else if (gv.size()==3) { // preferential attachment
        if (directed)
            return generr("This method cannot generate digraphs");
        if (!gv[1].is_integer() || !is_strictly_positive(gv[1],contextptr))
            return generr("Expected a positive integer");
        if (!gv[2].is_integer() || !is_positive(gv[2],contextptr))
            return generr("Expected a nonnegative integer");
        int d=gv[1].val,o=gv[2].val;
        G.preferential_attachment(d,o);
    } else return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    return G.to_gen();
}

bool compute_product_of_graphs(const vecteur &gv,graphe &P,bool cartesian,GIAC_CONTEXT) {
    stack<graphe> Gs;
    for (const_iterateur it=gv.begin();it!=gv.end();++it) {
        graphe G(contextptr);
        if (!G.read_gen(*it))
            return false;
        Gs.push(G);
    }
    P=Gs.top();
    Gs.pop();
    while (!Gs.empty()) {
        graphe G(P);
        if (cartesian)
            Gs.top().cartesian_product(G,P); // compute Cartesian product
        else Gs.top().tensor_product(G,P); // compute tensor product
        Gs.pop();
    }
    return true;
}

void add_prefix_to_vertex_label(gen &label,int prefix, stringstream &ss) {
    ss.str("");
    ss << prefix << ":"
       << (label.type==_STRNG?graphe::genstring2str(label):graphe::gen2str(label));
    label=graphe::str2gen(ss.str(),true);
}

int graphunion(graphe &G,const vecteur &gv,bool disjoint) {
    bool have_properties=false;
    int k=0,i,j;
    graphe::ipairs E;
    stringstream ss;
    for (const_iterateur it=gv.begin();it!=gv.end();++it) {
        ++k;
        graphe Gk(G.giac_context());
        if (!Gk.read_gen(*it))
            return _GT_ERR_NOT_A_GRAPH;
        if (have_properties) {
            if (G.is_directed()!=Gk.is_directed())
                return G.is_directed()?_GT_ERR_DIRECTED_GRAPH_REQUIRED:_GT_ERR_UNDIRECTED_GRAPH_REQUIRED;
            if (G.is_weighted()!=Gk.is_weighted())
                return G.is_weighted()?_GT_ERR_WEIGHTED_GRAPH_REQUIRED:_GT_ERR_UNWEIGHTED_GRAPH_REQUIRED;
        } else {
            G.set_directed(Gk.is_directed());
            G.set_weighted(Gk.is_weighted());
            have_properties=true;
        }
        vecteur V=Gk.vertices();
        for (iterateur it=V.begin();it!=V.end();++it) {
            if (disjoint)
                add_prefix_to_vertex_label(*it,k,ss);
            G.add_node(*it,Gk.node_attributes(it-V.begin()));
        }
        Gk.get_edges_as_pairs(E);
        for (graphe::ipairs_iter it=E.begin();it!=E.end();++it) {
            const gen &v=V[it->first],&w=V[it->second];
            i=G.node_index(v); j=G.node_index(w);
            assert(i>=0 && j>=0);
            if (!disjoint && G.is_weighted() && G.has_edge(i,j))
                G.set_edge_attribute(i,j,_GT_ATTRIB_WEIGHT,G.weight(i,j)+Gk.weight(it->first,it->second));
            if (!G.has_edge(i,j))
                G.add_edge(v,w,Gk.edge_attributes(it->first,it->second));
        }
    }
    return -1;
}

gen count_spanning_trees(const graphe &G) {
    matrice L;
    G.laplacian_matrix(L);
    const context *ctx=G.giac_context();
    return _det(_delcols(makesequence(_delrows(makesequence(L,0),ctx),0),ctx),ctx);
}

// +--------------------------------------------------------------------------+
// |                             GIAC COMMANDS                                |
// +--------------------------------------------------------------------------+

/* USAGE:   foldl(op,id,r1,r2,...)
 *
 * Returns the composition of the binary operator or function op, with identity
 * or initial value id onto its arguments r1, r2, ..., associating from the
 * left. For example, given three arguments a, b and c and an initial value id,
 * foldl(op,id,a,b,c) is equivalent to op(op(op(id,a),b),c).
 */
gen _foldl(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT  || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<3)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    gen &op=gv.front();
    gen arg=gv[1];
    for (const_iterateur it=gv.begin()+2;it!=gv.end();++it) {
        arg=symbolic(at_of,makesequence(op,makesequence(arg,*it)));
    }
    return _eval(arg,contextptr);
}
static const char _foldl_s[]="foldl";
static define_unary_function_eval(__foldl,&_foldl,_foldl_s);
define_unary_function_ptr5(at_foldl,alias_at_foldl,&__foldl,0,true)

/* USAGE:   foldr(op,id,r1,r2,...)
 *
 * Returns the composition of the binary operator or function op, with identity
 * or initial value id onto its arguments r1, r2, ..., associating from the
 * right. For example, given three arguments a, b and c and an initial value id,
 * foldl(op,id,a,b,c) is equivalent to op(a,op(b,op(c,id))).
 */
gen _foldr(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT  || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<3)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    gen &op=gv.front();
    gen arg=gv[1];
    for (int i=gv.size();i-->2;) {
        arg=symbolic(at_of,makesequence(op,makesequence(gv[i],arg)));
    }
    return _eval(arg,contextptr);
}
static const char _foldr_s[]="foldr";
static define_unary_function_eval(__foldr,&_foldr,_foldr_s);
define_unary_function_ptr5(at_foldr,alias_at_foldr,&__foldr,0,true)

/* USAGE:   randvar(f,[params])
 *          random_variable(f,[params])
 *          randvar(f,[range=]a..b,[V])
 *          randvar(L,[V])
 *
 * Returns a random variable with the specified probability distribution.
 */
gen _randvar(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.is_symb_of_sommet(at_program) || g.type==_FUNC) {
        if (g==at_normal || g==at_normald || g==at_NORMALD || g==at_randNorm || g==at_randnormald)
            return symbolic(at_normald,makesequence(0.0,1.0));
        if (g==at_uniform || g==at_uniformd)
            return symbolic(at_uniformd,makesequence(0.0,1.0));
        if (g==at_exp || g==at_exponential || g==at_exponentiald || g==at_EXP)
            return symbolic(at_exponentiald,1.0);
        return g;
    }
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    vecteur items,weights,categories;
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.front().type==_FUNC) {
            const_iterateur it=gv.begin()+1;
            for (;it!=gv.end();++it) {
                if (!graphe::is_real_number(*it) && it->type!=_IDNT) break;
            }
            if (it==gv.end()) {
                gen args=change_subtype(vecteur(gv.begin()+1,gv.end()),_SEQ__VECT);
                if (gv.front()==at_exp) {
                    if (args._VECTptr->size()==1)
                        return symbolic(at_exponentiald,args._VECTptr->front());
                    return gensizeerr(contextptr);
                }
                if (gv.front()==at_normal) {
                    if (args._VECTptr->size()==2)
                        return symbolic(at_exponentiald,args._VECTptr->front());
                    return gensizeerr(contextptr);
                }
                gen dist=symbolic(gv.front()._SYMBptr->sommet,args);
                int nd=is_distribution(dist);
                if (nd==0) return gensizeerr(contextptr);
                return symbolic(distribution(nd)._SYMBptr->sommet,args);
            }
        }
        if (gv.size()<2)
            return gensizeerr(contextptr);
        gen mean(undef),sdev(undef),a(undef),b(undef),&f=gv.front();
        int cnt=1,samples=0;
        for (const_iterateur it=gv.begin()+1;it!=gv.end();++it,++cnt) {
            if (it->is_symb_of_sommet(at_equal)) {
                gen &lh=it->_SYMBptr->feuille._VECTptr->front();
                gen &rh=it->_SYMBptr->feuille._VECTptr->back();
                if (lh==at_mean)
                    mean=rh;
                else if (lh==at_stddev)
                    sdev=rh;
                else if (lh==at_variance) {
                    if (!is_positive(rh,contextptr)) return gensizeerr(contextptr);
                    sdev=sqrt(rh,contextptr);
                } else if (lh==at_range) {
                    if (rh.type==_VECT) {
                        if (rh._VECTptr->size()!=2)
                            return gensizeerr(contextptr);
                        a=rh._VECTptr->front();
                        b=rh._VECTptr->back();
                    } else if (rh.is_symb_of_sommet(at_interval)) {
                        a=rh._SYMBptr->feuille._VECTptr->front();
                        b=rh._SYMBptr->feuille._VECTptr->back();
                    } else return gensizeerr(contextptr);
                } else return gensizeerr(contextptr);
            } else if (it->is_symb_of_sommet(at_interval)) {
                a=it->_SYMBptr->feuille._VECTptr->front();
                b=it->_SYMBptr->feuille._VECTptr->back();
            } else if (it->type==_VECT) {
                if (f.type==_VECT) {
                    if (cnt!=1) return gensizeerr(contextptr);
                    items=*(it->_VECTptr);
                } else if (cnt==1)
                    categories=*(it->_VECTptr);
                else items=*(it->_VECTptr);
            } else if (it->is_integer() && it->val>1 && cnt==2) {
                samples=it->val;
            } else return gensizeerr(contextptr);
        }
        if (!is_undef(sdev) && !is_strictly_positive(sdev,contextptr))
            return gensizeerr(contextptr);
        if (f.type==_VECT) { // custom discrete distribution
            weights=*f._VECTptr;
        } else if (f==at_normald || f==at_NORMALD || f==at_normal) {
            if (is_undef(mean)) mean=0;
            if (is_undef(sdev)) sdev=1;
            mean=_evalf(mean,contextptr);
            sdev=_evalf(sdev,contextptr);
            return symbolic(at_normald,makesequence(mean,sdev));
        } else if (f==at_uniform || f==at_uniformd) {
            if (!is_undef(a) && !is_undef(b)) {
                if (_evalf(a,contextptr).type!=_DOUBLE_ || _evalf(b,contextptr).type!=_DOUBLE_ ||
                        !is_strictly_greater(b,a,contextptr))
                    return gensizeerr(contextptr);
                return makesequence(at_uniformd,a,b);
            } else if (!is_undef(mean) && !is_undef(sdev)) {
                gen sqrt3=sqrt(3,contextptr);
                return symbolic(at_uniformd,makesequence(mean-sqrt3*sdev,mean+sqrt3*sdev));
            } else return gensizeerr(contextptr);
        } else if (f==at_poisson || f==at_POISSON) {
            gen lambda;
            if (!is_undef(mean)) {
                if (!is_undef(sdev) && !is_zero(_ratnormal(sq(sdev)-mean,contextptr)))
                    return gensizeerr(contextptr);
                lambda=mean;
            } else if (!is_undef(sdev)) {
                lambda=sq(sdev);
            } else return gensizeerr(contextptr);
            if (!is_strictly_positive(lambda,contextptr))
                return gensizeerr(contextptr);
            return symbolic(at_poisson,lambda);
        } else if (f==at_exp || f==at_EXP || f==at_exponential || f==at_exponentiald) {
            gen lambda;
            if (!is_undef(mean)) {
                if (!is_undef(sdev) && !is_zero(_ratnormal(sdev-mean,contextptr)))
                    return gensizeerr(contextptr);
                if (!is_strictly_positive(mean,contextptr))
                    return gensizeerr(contextptr);
                lambda=_inv(mean,contextptr);
            } else if (!is_undef(sdev)) {
                lambda=_inv(sdev,contextptr);
            } else return gensizeerr(contextptr);
            if (!is_strictly_positive(lambda,contextptr))
                return gensizeerr(contextptr);
            return symbolic(at_exponentiald,lambda);
        } else if (f==at_weibull || f==at_weibulld) {
            if (is_undef(mean) || is_undef(sdev) || !is_strictly_positive(mean,contextptr))
                return gensizeerr(contextptr);
            gen var=sq(sdev);
            if (is_zero(var))
                return gensizeerr(contextptr);
            identificateur tmp(" k");
            gen e=_Gamma(1+gen(2)/tmp,contextptr)/_Gamma(1+gen(1)/tmp,contextptr)-var/sq(mean)-1;
            gen k=_solve(makesequence(e,symb_equal(tmp,max(1,_inv(var,contextptr),contextptr)),_NEWTON_SOLVER),contextptr);
            gen lambda=mean/_Gamma(1+_inv(k,contextptr),contextptr);
            return symbolic(at_weibulld,makesequence(k,lambda));
        } else if (f==at_Gamma || f==at_gammad) {
            if (is_undef(mean) || is_undef(sdev) || !is_strictly_positive(mean,contextptr))
                return gensizeerr(contextptr);
            gen var=sq(sdev);
            return symbolic(at_gammad,makesequence(sq(mean)/var,mean/var));
        } else if (f==at_Beta || f==at_betad) {
            if (is_undef(mean) || is_undef(sdev) || !is_strictly_positive(mean,contextptr) ||
                    !is_strictly_greater(1,mean,contextptr))
                return gensizeerr(contextptr);
            gen var=sq(sdev),fac=(var+sq(mean)-mean)/var;
            gen par1=-mean*fac,par2=(mean-1)*fac;
            if (!is_strictly_positive(par1,contextptr) || !is_strictly_positive(par2,contextptr))
                return gensizeerr(contextptr);
            return symbolic(at_betad,makesequence(par1,par2));
        } else if (f==at_geometric) {
            gen p;
            if (!is_undef(mean)) {
                if (!is_strictly_positive(mean,contextptr) ||
                        (!is_undef(sdev) && !is_zero(_ratnormal(sq(sdev)-sq(mean)-mean,contextptr))))
                    return gensizeerr(contextptr);
                p=_inv(mean,contextptr);
            } else if (!is_undef(sdev)) {
                gen var=sq(sdev);
                p=_ratnormal((sqrt(4*var+1,contextptr)-1)/(2*var),contextptr);
            } else return gensizeerr(contextptr);
            return symbolic(at_geometric,p);
        } else if (f==at_fisher || f==at_fisherd || f==at_snedecor || f==at_chisquare || f==at_chisquared ||
                   f==at_cauchy || f==at_cauchyd || f==at_student || f==at_studentd) {
            return generr("Specifying moments is not supported for this distribution");
        } else if (f==at_multinomial) {
            if (categories.empty())
                return gensizeerr(contextptr);
            if (items.empty())
                return makesequence(at_multinomial,categories);
            return makesequence(at_multinomial,categories,items);
        } else if (f==at_binomial || f==at_BINOMIAL) {
            if (is_undef(mean) || is_undef(sdev) || !is_strictly_positive(mean,contextptr) ||
                    is_zero(ratnormal(mean-sq(sdev),contextptr)))
                return gensizeerr(contextptr);
            gen tmp=mean-sq(sdev),N=_ratnormal(sq(mean)/tmp,contextptr),p=tmp/mean;
            if (!is_zero(N-_floor(N,contextptr))) {
                N=_round(N,contextptr);
                if (is_zero(N))
                    return gensizeerr(contextptr);
                p=mean/N;
            } else N=_round(N,contextptr);
            if (!is_greater(p,0,contextptr) || is_strictly_greater(p,1,contextptr))
                return gensizeerr(contextptr);
            return symbolic(at_binomial,makesequence(N,_ratnormal(p,contextptr)));
        } else if (f.type==_FUNC || f.is_symb_of_sommet(at_program)) { // custom discrete distribution
            if (!is_integer(a) || !is_integer(b) || !is_strictly_greater(b,a,contextptr))
                return gensizeerr(contextptr);
            if (samples>0) {
                identificateur k(" k");
                gen support=_seq(makesequence(k,symb_equal(k,symb_interval(a,b)),
                                              _evalf((b-a)/gen(samples),contextptr)),contextptr);
                gen values=_apply(makesequence(f,support),contextptr);
                return _randvar(makesequence(values,support),contextptr);
            }
            int lb=a.val,ub=b.val;
            vecteur rng(ub-lb+1);
            for (iterateur it=rng.begin();it!=rng.end();++it) {
                *it=lb+int(it-rng.begin());
            }
            weights=*_apply(makesequence(gv.front(),rng),contextptr)._VECTptr;
            if (items.empty()) items=rng;
        }
    } else if (ckmatrix(g)) {
        if (g._VECTptr->front()._VECTptr->size()!=2)
            return gendimerr(contextptr);
        int n=g._VECTptr->size();
        weights.reserve(n);
        items.reserve(n);
        for (int i=0;i<n;++i) {
            items.push_back(g._VECTptr->at(i)._VECTptr->front());
            weights.push_back(g._VECTptr->at(i)._VECTptr->back());
        }
    } else weights=*g._VECTptr;
    if (weights.empty())
        return gensizeerr(contextptr);
    if (!items.empty() && items.size()!=weights.size())
        return gendimerr(contextptr);
    for (const_iterateur it=weights.begin();it!=weights.end();++it) {
        if (!graphe::is_real_number(*it) || !is_positive(*it,contextptr))
            return gensizeerr(contextptr);
    }
    graphe::ransampl rs(weights,contextptr);
    gen ret=rs.data();
    if (!items.empty())
        ret=mergevecteur(*ret._VECTptr,items);
    return symbolic(at_discreted,change_subtype(ret,_SEQ__VECT));
}
static const char _randvar_s[]="randvar";
static define_unary_function_eval(__randvar,&_randvar,_randvar_s);
define_unary_function_ptr5(at_randvar,alias_at_randvar,&__randvar,0,true)

static const char _random_variable_s[]="random_variable";
static define_unary_function_eval(__random_variable,&_randvar,_random_variable_s);
define_unary_function_ptr5(at_random_variable,alias_at_random_variable,&__random_variable,0,true)

/* USAGE:   trail(V)
 *
 * Returns a trail of vertices from sequence V (this is a dummy command, it
 * returns itself).
 */
gen _trail(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    return symbolic(at_trail,g);
}
static const char _trail_s[]="trail";
static define_unary_function_eval(__trail,&_trail,_trail_s);
define_unary_function_ptr5(at_trail,alias_at_trail,&__trail,0,true)

/* USAGE:   graph(V,[opts])
 *          graph(V,E,[opts])
 *          graph(V,E,A,[opts])
 *          graph(V,A,[opts])
 *          graph(A,[opts])
 *          graph("name")
 *
 * Create an (un)directed (un)weighted graph from list of vertices V, set of edges
 * E, and/or adjacency matrix A containing edge weights. All parameters are
 * optional.
 *
 * 'opts' is a sequence of options containing weighted=true/false,
 * directed=true/false, color=c or coordinates=p. Here c is
 * integer or list of integers (color(s) to be assigned to vertices (in order))
 * and p is list of coordinates to assign to vertices (used for drawing).
 *
 * A special may be created by specifying its name as a string. Supported names
 * are: clebsch - coxeter - desargues - dodecahedron - durer - dyck - grinberg
 * - grotzsch - harries - harries-wong - heawood - herschel - icosahedron -
 * levi - ljubljana - mcgee - mobius-kantor - nauru - octahedron - pappus -
 * petersen - robertson - soccerball - shrikhande - tehtrahedron
*/
gen _graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type==_STRNG) {
        // construct special graph
        string name=graphe::genstring2str(g);
        graphe G(name,contextptr);
        if (G.is_null())
            return gt_err(_GT_ERR_NAME_NOT_RECOGNIZED);
        return G.to_gen();
    }
    graphe G(contextptr);
    if (g.is_integer() && g.val>=0) {
        vecteur V;
        G.make_default_labels(V,g.val);
        G.add_nodes(V);
    } else if (is_squarematrix(g) && g._VECTptr->size()>2) {
        // adjacency matrix is given
        bool size_err;
        if (!parse_matrix(G,*g._VECTptr,false,0,size_err))
            return size_err?gendimerr(contextptr):gentypeerr(contextptr);
    } else if (g.type==_VECT && g.subtype!=_SEQ__VECT) {
        // list of vertices or set of edges is given
        if (g.subtype==_SET__VECT) {
            if (!parse_edges(G,*g._VECTptr,true))
                return generrtype("Failed to parse edges");
        } else G.add_nodes(*g._VECTptr);
    } else if (g.is_symb_of_sommet(at_trail)) {
        // a trail is given
        parse_trail(G,g);
    } else {
        if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
            return gentypeerr(contextptr);
        vecteur &args=*g._VECTptr;
        int nargs=args.size(),n=nargs-1;
        // parse options first
        bool weighted=false,size_err;
        while(args[n].is_symb_of_sommet(at_equal)) {
            vecteur &sides=*args[n]._SYMBptr->feuille._VECTptr;
            if (!sides.front().is_integer())
                return generr("Unrecognized option");
            switch(sides.front().val) {
            case _GT_DIRECTED:
                if (!sides.back().is_integer())
                    return generr("Option value not supported");
                G.set_directed((bool)sides.back().val);
                break;
            case _GT_WEIGHTED:
                if (!sides.back().is_integer())
                    return generr("Option value not supported");
                weighted=(bool)sides.back().val;
                break;
            }
            n--;
        }
        // parse other arguments
        for (int i=0;i<nargs;++i) {
            const gen &arg=args[i];
            if (i<=n && ckmatrix(arg) && arg.subtype!=_SET__VECT) {
                // adjacency or weight matrix
                matrice &m=*arg._VECTptr;
                if (!G.is_directed() && m!=mtran(m))
                    return gt_err(_GT_ERR_MATRIX_NOT_SYMMETRIC);
                if (!parse_matrix(G,m,i==2 || weighted,i,size_err))
                    return size_err?gendimerr(contextptr):generrtype("Failed to parse matrix");
            } else if (i==0 && arg.is_integer()) {
                int nv=arg.val;
                if (nv<0)
                    return generr("Number of vertices must be positive");
                vecteur V;
                G.make_default_labels(V,nv);
                G.add_nodes(V);
            } else if (i<2 && arg.type==_VECT) {
                int permu_size;
                const vecteur &argv=*arg._VECTptr;
                if (arg.subtype==_SET__VECT) {
                    // set of edges
                    if (!parse_edges(G,argv,true))
                        return generrtype("Failed to parse edges");
                } else if (i==1 && !is_zero(_is_permu(arg,contextptr)) &&
                           (permu_size=argv.size())>0) {
                    if (permu_size!=G.node_count())
                        return generr("Permutation size does not match the number of vertices");
                    // directed cycle
                    G.set_directed(true);
                    int offset=array_start(contextptr);
                    for (const_iterateur it=argv.begin();it!=argv.end()-1;++it) {
                        G.add_edge(it->val-offset,(it+1)->val-offset);
                    }
                    G.add_edge(argv.back().val-offset,argv.front().val-offset);
                } else if (i==0) // list of vertices
                    G.add_nodes(argv);
                else return gentypeerr(contextptr);
            } else if (arg.is_symb_of_sommet(at_trail)) {
                // trail
                parse_trail(G,arg);
            } else if (i>n && arg.is_symb_of_sommet(at_equal)) {
                // option
                gen &lh=arg._SYMBptr->feuille._VECTptr->front();
                gen &rh=arg._SYMBptr->feuille._VECTptr->back();
                if (lh.is_integer()) {
                    switch(lh.val) {
                    case _COLOR:
                        // vertex colors are given
                        if (rh.type==_VECT || int(rh._VECTptr->size())!=G.node_count())
                            return gensizeerr(contextptr);
                        if (!parse_vertex_colors(G,rh))
                            return generr("Failed to parse vertex colors");
                        break;
                    }
                } else if (lh==at_coordonnees) {
                    // vertex coordinates are given
                    if (rh.type!=_VECT)
                        return gentypeerr(contextptr);
                    if (int(rh._VECTptr->size())!=G.node_count())
                        return gensizeerr(contextptr);
                    bool size_error=false;
                    if (!parse_vertex_coordinates(G,*rh._VECTptr,size_error))
                        return size_error?generr("Wrong number of vertex positions;"):
                                          generrtype("Failed to parse vertex positions");
                }
            } else return gentypeerr(contextptr);
        }
    }
    return G.to_gen();
}
static const char _graph_s[]="graph";
static define_unary_function_eval(__graph,&_graph,_graph_s);
define_unary_function_ptr5(at_graph,alias_at_graph,&__graph,0,true)

/* USAGE:   digraph(V,[opts])
 *          digraph(V,E,[opts])
 *          digraph(A,[opts])
 *          digraph(V,E,A,[opts])
 *
 * Create a directed (un)weighted graph from list of vertices V, set of edges E
 * and/or adjacency matrix A containing edge weights. All parameters are
 * optional.
 *
 * 'opts' may be one of weighted=true/false, vertexcolor=c and
 * vertexpositions=p. Here c is integer or list of integers (color(s) to be
 * assigned to vertices (in order)) and p is list of coordinates to assign to
 * vertices (used for drawing).
 */
gen _digraph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    vecteur args;
    if (g.type==_VECT && g.subtype==_SEQ__VECT)
        args=*g._VECTptr;
    else args.push_back(g);
    args.push_back(symbolic(at_equal,makesequence(_GT_DIRECTED,graphe::VRAI)));
    return _graph(change_subtype(args,_SEQ__VECT),contextptr);
}
static const char _digraph_s[]="digraph";
static define_unary_function_eval(__digraph,&_digraph,_digraph_s);
define_unary_function_ptr5(at_digraph,alias_at_digraph,&__digraph,0,true)

/* USAGE:   export_graph(G,"path/to/graphname",[latex[=params]])
 *
 * Writes the graph G to the file 'graphname.dot' in directory 'path/to' using
 * dot format or store the drawing of G in latex format if third argument is
 * given, where params is an option or a list of options to be passed to
 * the draw_graph command. Returns 1 on success and 0 on failure.
 */
gen _export_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2 || gv.size()>3)
        return gensizeerr(contextptr);
    gen &gr=gv.front(),&name=gv[1];
    bool to_latex=false;
    gen args=undef;
    if (gv.size()==3) {
        if (gv.back()==at_latex)
            to_latex=true;
        else if (gv.back().is_symb_of_sommet(at_equal)) {
            if (gv.back()._SYMBptr->feuille._VECTptr->front()!=at_latex)
                return generrtype("Option not supported, expected \"latex\"");
            to_latex=true;
            args=gv.back()._SYMBptr->feuille._VECTptr->back();
        } else return gentypeerr(contextptr);
    }
    graphe G(contextptr);
    if (!G.read_gen(gr))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (name.type!=_STRNG)
        return gentypeerr(contextptr);
    string filename=graphe::genstring2str(name);
    filename=make_absolute_file_path(filename);
    if (to_latex) {
        if (!has_suffix(filename,".tex"))
            filename=filename+".tex";
        if (args.type==_VECT)
            args._VECTptr->insert(args._VECTptr->begin(),gr);
        else if (!is_undef(args))
            args=vecteur(1,args);
        gen drawing=_draw_graph(args.type==_VECT?change_subtype(args,_SEQ__VECT):gr,contextptr);
        return G.write_latex(filename,drawing);
    }
    if (!has_suffix(filename,".dot") && !has_suffix(filename,".gv"))
        filename=filename+".dot";
    return G.write_dot(filename)?1:0;
}
static const char _export_graph_s[]="export_graph";
static define_unary_function_eval(__export_graph,&_export_graph,_export_graph_s);
define_unary_function_ptr5(at_export_graph,alias_at_export_graph,&__export_graph,0,true)

/* USAGE:   import_graph("path/to/graphname[.dot or .gv]")
 *
 * Returns the graph constructed from instructions in the file
 * 'path/to/graphname.dot' (in dot format) or undef on failure.
 */
gen _import_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_STRNG)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    string filename=graphe::genstring2str(g);
    if (filename.empty())
        return undef;
    if (!has_suffix(filename,".dot") && !has_suffix(filename,".gv"))
        filename=filename+".dot";
    filename=make_absolute_file_path(filename);
    if (!G.read_dot(filename)) {
        gt_err(_GT_ERR_READING_FAILED);
        return undef;
    }
    gen_map m;
    gen l;
    for (int i=0;i<G.node_count();++i) {
        l=G.node_label(i);
        if (!is_exactly_zero(m[l])) {
            *logptr(contextptr) << "Warning: imported graph contains equally labeled vertices\n";
            break;
        }
        m[l]=1;
    }
    return G.to_gen();
}
static const char _import_graph_s[]="import_graph";
static define_unary_function_eval(__import_graph,&_import_graph,_import_graph_s);
define_unary_function_ptr5(at_import_graph,alias_at_import_graph,&__import_graph,0,true)

/* USAGE:   vertices(G)
 *
 * Return list of vertices of graph G.
 */
gen _graph_vertices(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return G.vertices();
}
static const char _graph_vertices_s[]="graph_vertices";
static define_unary_function_eval(__graph_vertices,&_graph_vertices,_graph_vertices_s);
define_unary_function_ptr5(at_graph_vertices,alias_at_graph_vertices,&__graph_vertices,0,true)

/* USAGE:   edges(G,[weights])
 *
 * Return list of edges of graph G. If second argument is the option 'weights',
 * edge weights are also returned.
 */
gen _edges(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    bool include_weights=false;
    graphe G(contextptr);
    if (g.subtype==_SEQ__VECT) {
        if (int(g._VECTptr->size())!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if (g._VECTptr->back().is_integer() && g._VECTptr->back().val==_GT_WEIGHTS)
            include_weights=true;
    }
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (include_weights && !G.is_weighted())
        return gt_err(_GT_ERR_WEIGHTED_GRAPH_REQUIRED);
    return change_subtype(G.edges(include_weights),_LIST__VECT);
}
static const char _edges_s[]="edges";
static define_unary_function_eval(__edges,&_edges,_edges_s);
define_unary_function_ptr5(at_edges,alias_at_edges,&__edges,0,true)

/* USAGE:   has_edge(G,e)
 *
 * Returns true iff the edge e={v,w} is contained in the undirected graph G.
 */
gen _has_edge(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv = *g._VECTptr;
    if (int(gv.size())!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (gv.back().type!=_VECT)
        return gentypeerr(contextptr);
    if (int(gv.back()._VECTptr->size())!=2)
        return gensizeerr(contextptr);
    vecteur &e=*gv.back()._VECTptr;
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    int i=G.node_index(e.front()),j=G.node_index(e.back());
    if (i<0 || j<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    return graphe::boole(G.has_edge(i,j));
}
static const char _has_edge_s[]="has_edge";
static define_unary_function_eval(__has_edge,&_has_edge,_has_edge_s);
define_unary_function_ptr5(at_has_edge,alias_at_has_edge,&__has_edge,0,true)

/* USAGE:   has_arc(G,e)
 *
 * Returns true iff the arc e=[v,w] is contained in directed graph G. If
 * e={v,w}, true is returned if directed graph G has both edges [v,w] and
 * [v,w].
 */
gen _has_arc(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv = *g._VECTptr;
    if (int(gv.size())!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (gv.back().type!=_VECT)
        return gentypeerr(contextptr);
    if (int(gv.back()._VECTptr->size())!=2)
        return gensizeerr(contextptr);
    vecteur &e=*gv.back()._VECTptr;
    bool undirected=gv.back().subtype==_SET__VECT;
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    int i=G.node_index(e.front()),j=G.node_index(e.back());
    if (i<0 || j<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    return graphe::boole(G.has_edge(i,j) && (!undirected || G.has_edge(j,i)));
}
static const char _has_arc_s[]="has_arc";
static define_unary_function_eval(__has_arc,&_has_arc,_has_arc_s);
define_unary_function_ptr5(at_has_arc,alias_at_has_arc,&__has_arc,0,true)

/* USAGE:   adjacency_matrix(G)
 *
 * Returns the adjacency matrix of a graph G whose rows and columns are indexed
 * by the vertices. The entry [i,j] of this matrix is 1 if there is an edge
 * from vertex i to vertex j and 0 otherwise.
 */
gen _adjacency_matrix(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return vecteur(0);
    matrice m;
    G.adjacency_matrix(m);
    return change_subtype(m,_MATRIX__VECT);
}
static const char _adjacency_matrix_s[]="adjacency_matrix";
static define_unary_function_eval(__adjacency_matrix,&_adjacency_matrix,_adjacency_matrix_s);
define_unary_function_ptr5(at_adjacency_matrix,alias_at_adjacency_matrix,&__adjacency_matrix,0,true)

/* USAGE:   incidence_matrix(G)
 *
 * Returns the incidence matrix of a graph G whose rows are indexed by the
 * vertices and columns by the edges (in order defined by the command 'edges').
 * The entry [i,j] of this matrix is 1 if the i-th vertex is incident with the
 * j-th edge (for directed graphs, -1 if vertex is the tail and 1 if it is the
 * head of arc).
 */
gen _incidence_matrix(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return vecteur(0);
    matrice M;
    G.incidence_matrix(M);
    return change_subtype(M,_MATRIX__VECT);
}
static const char _incidence_matrix_s[]="incidence_matrix";
static define_unary_function_eval(__incidence_matrix,&_incidence_matrix,_incidence_matrix_s);
define_unary_function_ptr5(at_incidence_matrix,alias_at_incidence_matrix,&__incidence_matrix,0,true)

/* USAGE:   weight_matrix(G)
 *
 * Returns the weight matrix of graph G.
 */
gen _weight_matrix(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g) || !G.is_weighted())
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return vecteur(0);
    matrice W;
    G.weight_matrix(W);
    return change_subtype(W,_MATRIX__VECT);
}
static const char _weight_matrix_s[]="weight_matrix";
static define_unary_function_eval(__weight_matrix,&_weight_matrix,_weight_matrix_s);
define_unary_function_ptr5(at_weight_matrix,alias_at_weight_matrix,&__weight_matrix,0,true)

/* USAGE:   graph_complement(G)
 *
 * Return the complement of graph G, i.e. the graph with the same vertex set
 * as G, but whose edge (arc) set consists of the edges (arcs) not present in
 * G.
 */
gen _graph_complement(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr),C(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    G.complement(C);
    return C.to_gen();
}
static const char _graph_complement_s[]="graph_complement";
static define_unary_function_eval(__graph_complement,&_graph_complement,_graph_complement_s);
define_unary_function_ptr5(at_graph_complement,alias_at_graph_complement,&__graph_complement,0,true)

/* USAGE:   subgraph(G,E)
 *
 * Returns the subgraph of G defined by the edges in list E.
 */
gen _subgraph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || int(g._VECTptr->size())!=2 ||
            g._VECTptr->back().type!=_VECT)
        return gentypeerr(contextptr);
    vecteur &E=*g._VECTptr->back()._VECTptr;
    graphe G(contextptr),S(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    graphe::ipairs edges;
    bool notfound=false;
    if (!G.edges2ipairs(E,edges,notfound))
        return notfound?gt_err(_GT_ERR_EDGE_NOT_FOUND):gensizeerr(contextptr);
    G.extract_subgraph(edges,S);
    return S.to_gen();
}
static const char _subgraph_s[]="subgraph";
static define_unary_function_eval(__subgraph,&_subgraph,_subgraph_s);
define_unary_function_ptr5(at_subgraph,alias_at_subgraph,&__subgraph,0,true)

/* USAGE:   vertex_degree(G,v)
 *
 * Returns the degree of the vertex v in graph G (number of edges incident to v).
 */
gen _vertex_degree(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || int(g._VECTptr->size())<2)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int i=G.node_index(g._VECTptr->at(1));
    if (i==-1)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    return G.degree(i);
}
static const char _vertex_degree_s[]="vertex_degree";
static define_unary_function_eval(__vertex_degree,&_vertex_degree,_vertex_degree_s);
define_unary_function_ptr5(at_vertex_degree,alias_at_vertex_degree,&__vertex_degree,0,true)

/* USAGE:   vertex_in_degree(G,v)
 *
 * Returns the number of arcs ending in the vertex v of graph G.
 */
gen _vertex_in_degree(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || int(g._VECTptr->size())<2)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    int i=G.node_index(g._VECTptr->at(1));
    if (i==-1)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    return G.in_degree(i);
}
static const char _vertex_in_degree_s[]="vertex_in_degree";
static define_unary_function_eval(__vertex_in_degree,&_vertex_in_degree,_vertex_in_degree_s);
define_unary_function_ptr5(at_vertex_in_degree,alias_at_vertex_in_degree,&__vertex_in_degree,0,true)

/* USAGE:   vertex_out_degree(G,v)
 *
 * Returns the number of arcs starting in the vertex v of graph G.
 */
gen _vertex_out_degree(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || int(g._VECTptr->size())<2)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    int i=G.node_index(g._VECTptr->at(1));
    if (i==-1)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    return G.out_degree(i);
}
static const char _vertex_out_degree_s[]="vertex_out_degree";
static define_unary_function_eval(__vertex_out_degree,&_vertex_out_degree,_vertex_out_degree_s);
define_unary_function_ptr5(at_vertex_out_degree,alias_at_vertex_out_degree,&__vertex_out_degree,0,true)

/* USAGE:   induced_subgraph(G,V)
 *
 * Returns the subgraph of G induced by the vertices in list V.
 */
gen _induced_subgraph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || int(g._VECTptr->size())!=2 ||
            g._VECTptr->back().type!=_VECT)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    vecteur &V=*g._VECTptr->back()._VECTptr;
    int i=0,index;
    vector<int> vi(V.size());
    for (const_iterateur it=V.begin();it!=V.end();++it) {
        if ((index=G.node_index(*it))<0)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        vi[i++]=index;
    }
    graphe S(G);
    G.induce_subgraph(vi,S);
    return S.to_gen();
}
static const char _induced_subgraph_s[]="induced_subgraph";
static define_unary_function_eval(__induced_subgraph,&_induced_subgraph,_induced_subgraph_s);
define_unary_function_ptr5(at_induced_subgraph,alias_at_induced_subgraph,&__induced_subgraph,0,true)

/* USAGE:   maximum_matching(G)
 *
 * Returns the list of edges representing maximum matching for graph G. Jack
 * Edmonds' blossom algorithm is used.
 */
gen _maximum_matching(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    graphe::ipairs matching;
    G.find_maximum_matching(matching);
    vecteur res;
    for (graphe::ipairs_iter it=matching.begin();it!=matching.end();++it) {
        res.push_back(makevecteur(G.node_label(it->first),G.node_label(it->second)));
    }
    return change_subtype(res,_LIST__VECT);
}
static const char _maximum_matching_s[]="maximum_matching";
static define_unary_function_eval(__maximum_matching,&_maximum_matching,_maximum_matching_s);
define_unary_function_ptr5(at_maximum_matching,alias_at_maximum_matching,&__maximum_matching,0,true)

/* USAGE:   bipartite_matching(G)
 *
 * Returns the list of edges of a maximum matching an undirected unweighted
 * bipartite graph G.
 */
gen _bipartite_matching(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G.is_weighted())
        return gt_err(_GT_ERR_UNWEIGHTED_GRAPH_REQUIRED);
    graphe::ivector p1,p2;
    if (!G.is_bipartite(p1,p2))
        return gt_err(_GT_ERR_NOT_BIPARTITE);
    graphe::ipairs matching;
    vecteur res(G.bipartite_matching(p1,p2,matching));
    for (graphe::ipairs_iter it=matching.begin();it!=matching.end();++it) {
        res[it-matching.begin()]=makevecteur(G.node_label(it->first),G.node_label(it->second));
    }
    return change_subtype(res,_LIST__VECT);
}
static const char _bipartite_matching_s[]="bipartite_matching";
static define_unary_function_eval(__bipartite_matching,&_bipartite_matching,_bipartite_matching_s);
define_unary_function_ptr5(at_bipartite_matching,alias_at_bipartite_matching,&__bipartite_matching,0,true)

/* USAGE:   make_directed(G,[A])
 *
 * Returns the copy of an undirected graph G in which every edge is converted
 * to a pair of arcs [and with weights specified by matrix A].
 */
gen _make_directed(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    bool hasweights=g.subtype==_SEQ__VECT;
    if (hasweights && gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(hasweights?gv.front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    G.make_directed();
    if (hasweights) {
        if (gv.back().type!=_VECT)
            return gentypeerr(contextptr);
        G.make_weighted(*gv.back()._VECTptr);
    }
    return G.to_gen();
}
static const char _make_directed_s[]="make_directed";
static define_unary_function_eval(__make_directed,&_make_directed,_make_directed_s);
define_unary_function_ptr5(at_make_directed,alias_at_make_directed,&__make_directed,0,true)

/* USAGE:   underlying_graph(G)
 *
 * Returns the underlying graph of G, i.e. the graph obtained by stripping
 * directions and weights from arcs (pairs of arcs connecting the same vertices
 * are merged to a single edge).
 */
gen _underlying_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    graphe U;
    G.underlying(U);
    return U.to_gen();
}
static const char _underlying_graph_s[]="underlying_graph";
static define_unary_function_eval(__underlying_graph,&_underlying_graph,_underlying_graph_s);
define_unary_function_ptr5(at_underlying_graph,alias_at_underlying_graph,&__underlying_graph,0,true)

/* USAGE:   cycle_graph(n or V)
 *
 * Returns the cyclic graph with n vertices (or with vertices from list V).
 */
gen _cycle_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!vertices_from_integer_or_vecteur(g,G))
        return gt_err(_GT_ERR_BAD_VERTICES);
    if (G.node_count()<3)
        return generr("At least 3 vertices are required");
    G.make_cycle_graph();
    /*
    stringstream ss;
    ss << "C" << G.node_count();
    G.set_name(ss.str());
    */
    return G.to_gen();
}
static const char _cycle_graph_s[]="cycle_graph";
static define_unary_function_eval(__cycle_graph,&_cycle_graph,_cycle_graph_s);
define_unary_function_ptr5(at_cycle_graph,alias_at_cycle_graph,&__cycle_graph,0,true)

/* USAGE:   lcf_graph(jumps,[exp])
 *
 * Returns the graph constructed from LCF notation jumps[^exp]. Arguments are
 * list of integers 'jumps' [and a positive integer 'exp', by default 1].
 */
gen _lcf_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    vecteur jumps;
    gen e;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        e=g._VECTptr->at(1);
        if (!e.is_integer() || e.val<=0 || g._VECTptr->front().type!=_VECT)
            return gentypeerr(contextptr);
        jumps=*g._VECTptr->front()._VECTptr;
    } else {
        jumps=*g._VECTptr;
        e=gen(1);
    }
    if (jumps.empty())
        return gensizeerr(contextptr);
    if (!is_integer_vecteur(jumps))
        return gentypeerr(contextptr);
    graphe G(contextptr);
    graphe::ivector ijumps;
    vecteur2vector_int(jumps,0,ijumps);
    G.make_lcf_graph(ijumps,e.val);
    return G.to_gen();
}
static const char _lcf_graph_s[]="lcf_graph";
static define_unary_function_eval(__lcf_graph,&_lcf_graph,_lcf_graph_s);
define_unary_function_ptr5(at_lcf_graph,alias_at_lcf_graph,&__lcf_graph,0,true)

/* USAGE:   hypercube_graph(n)
 *
 * Constructs and returns the hypercube graph in dimension n (with 2^n vertices).
 */
gen _hypercube_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (!g.is_integer() || g.val<=0)
        return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    int n=g.val,N=std::pow(2,n);
    graphe G(contextptr);
    for (int i=0;i<N;++i) {
        G.add_node(graphe::to_binary(i,n));
    }
    for (int i=0;i<N;++i) {
        for (int j=i+1;j<N;++j) {
            if (_hamdist(makesequence(i,j),contextptr).val==1)
                G.add_edge(i,j);
        }
    }
    return G.to_gen();
}
static const char _hypercube_graph_s[]="hypercube_graph";
static define_unary_function_eval(__hypercube_graph,&_hypercube_graph,_hypercube_graph_s);
define_unary_function_ptr5(at_hypercube_graph,alias_at_hypercube_graph,&__hypercube_graph,0,true)

/* USAGE:   seidel_switch(G,V)
 *
 * Returns a copy of graph G in which edges between vertices in list V and
 * vertices not in V are inverted, i.e. replaced with a set of edges from V to
 * other vertices which is not present in G.
 */
gen _seidel_switch(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || int(g._VECTptr->size())!=2 ||
            g._VECTptr->back().type!=_VECT)
        return gentypeerr(contextptr);
    graphe G(contextptr),H(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G.is_weighted())
        return gt_err(_GT_ERR_UNWEIGHTED_GRAPH_REQUIRED);
    int n=G.node_count();
    vecteur &V=*g._VECTptr->back()._VECTptr;
    vector<bool> vb(n,false);
    for (const_iterateur it=V.begin();it!=V.end();++it) {
        int index=G.node_index(*it);
        if (index==-1)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        vb[index]=true;
    }
    H.add_nodes(G.vertices());
    for (int i=0;i<n;++i) {
        for (int j=i+1;j<n;++j) {
            bool b=G.has_edge(i,j);
            if (vb[i]!=vb[j])
                b=!b;
            if (b)
                H.add_edge(i,j);
        }
    }
    return H.to_gen();
}
static const char _seidel_switch_s[]="seidel_switch";
static define_unary_function_eval(__seidel_switch,&_seidel_switch,_seidel_switch_s);
define_unary_function_ptr5(at_seidel_switch,alias_at_seidel_switch,&__seidel_switch,0,true)

/* USAGE:   draw_graph(G,[options])
 *
 * Returns the graphic representation of graph G obtained by using
 * various algorithms (which can optionally be fine tuned by appending a
 * sequence of options after the first argument).
 *
 * Supported options are:
 *
 *  - spring: use force-directed method to draw graph G (the default)
 *  - tree[=r or [r1,r2,...]]: draw tree or forest G [with optional
 *    specification of root nodes]
 *  - bipartite: draw the bipartite graph G keeping the partitions separated
 *  - plane or planar: draw planar graph G
 *  - circle[=<cycle>]: draw graph G as circular using the leading cycle,
 *    otherwise one must be specified or all vertices are placed on a circle
 *  - plot3d: draw 3D representation of graph G (possible only with the
 *    spring method and with G connected)
 *  - labels=true or false: draw (the default) or suppress node labels and
 *    weights
 *
 * An exception is raised if a method is specified but the corresponding
 * necessary conditions are not met.
 */
gen _draw_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    bool has_opts=g.subtype==_SEQ__VECT;
    graphe G_orig(contextptr);
    if (!G_orig.read_gen(has_opts?gv.front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    bool labels=G_orig.node_count()<=60,isdir=G_orig.is_directed();
    vecteur root_nodes,outer_vertices;
    gen coords_dest=undef;
    int method=_GT_STYLE_DEFAULT,opt_counter=0;
    if (has_opts) {
        // parse options
        for (const_iterateur it=gv.begin()+1;it!=gv.end();++it) {
            opt_counter++;
            const gen &opt=*it;
            if (opt.type==_IDNT) {
                coords_dest=opt;
                opt_counter--;
            } else if (opt.is_symb_of_sommet(at_equal)) {
                gen &lh=opt._SYMBptr->feuille._VECTptr->front();
                gen &rh=opt._SYMBptr->feuille._VECTptr->back();
                if (lh.is_integer()) {
                    switch (lh.val) {
                    case _GT_TREE:
                        if (rh.type==_VECT)
                            root_nodes=*rh._VECTptr;
                        else
                            root_nodes.push_back(rh);
                        method=_GT_STYLE_TREE;
                        break;
                    case _LABELS:
                        if (!rh.is_integer())
                            return generrtype("Expected an integer");
                        labels=(bool)rh.val;
                        opt_counter--;
                        break;
                    }
                } else if (lh==at_cercle || lh==at_convexhull) {
                    if (rh.type!=_VECT)
                        return gentypeerr(contextptr);
                    outer_vertices=*rh._VECTptr;
                    method=_GT_STYLE_CIRCLE;
                }
            } else if (opt==at_cercle || opt==at_convexhull)
                method=_GT_STYLE_CIRCLE;
            else if (opt==at_plan)
                method=_GT_STYLE_PLANAR;
            else if (opt==at_plot3d)
                method=_GT_STYLE_3D;
            else if (opt.is_integer()) {
                switch (opt.val) {
                case _GT_TREE:
                    method=_GT_STYLE_TREE;
                    break;
                case _GT_BIPARTITE:
                    method=_GT_STYLE_BIPARTITE;
                    break;
                case _GT_SPRING:
                    method=_GT_STYLE_SPRING;
                    break;
                case _GT_PLANAR:
                    method=_GT_STYLE_PLANAR;
                }
            }
        }
        if (opt_counter>1)
            return gt_err(_GT_ERR_INVALID_DRAWING_METHOD);
    }
    graphe G(contextptr);
    G_orig.underlying(G);
    int i,comp_method=method;
    vector<graphe> Cv;
    vector<graphe::layout> layouts;
    graphe::layout main_layout;
    graphe::ivector partition1,partition2;
    if (opt_counter==0 && G_orig.has_stored_layout(main_layout)) {
        ; // the graph G already has a layout, display it
    } else {
        graphe::ivectors components;
        G.connected_components(components);
        int nc=components.size();
        graphe::ivector hull;
        vecteur roots;
        if (!root_nodes.empty()) {
            // get the root nodes for forest drawing
            if (int(root_nodes.size())!=nc)
                return gt_err(_GT_ERR_INVALID_NUMBER_OF_ROOTS);
            graphe::ivector indices(nc);
            roots.resize(nc);
            for (const_iterateur it=root_nodes.begin();it!=root_nodes.end();++it) {
                i=G.node_index(*it);
                if (i==-1)
                    return gt_err(*it,_GT_ERR_VERTEX_NOT_FOUND);
                indices[it-root_nodes.begin()]=i;
            }
            for (int i=0;i<nc;++i) {
                const graphe::ivector &comp=components[i];
                graphe::ivector::iterator it=indices.begin();
                for (;it!=indices.end();++it) {
                    if (find(comp.begin(),comp.end(),*it)!=comp.end())
                        break;
                }
                if (it==indices.end())
                    return gt_err(_GT_ERR_INVALID_ROOT);
                roots[i]=root_nodes[it-indices.begin()];
                indices.erase(it);
            }
        }
        layouts.resize(nc);
        vector<graphe::rectangle> bounding_rects(nc);
        bool check=method!=_GT_STYLE_DEFAULT;
        double sep=1.0;
        // draw the components separately
        Cv.reserve(nc);
        for (graphe::ivectors_iter it=components.begin();it!=components.end();++it) {
            i=it-components.begin();
            graphe C(contextptr);
            G.induce_subgraph(*it,C);
            if (!outer_vertices.empty()) {
                if (nc>1)
                    return gt_err(_GT_ERR_CONNECTED_GRAPH_REQUIRED);
                // get the outer face for circular drawing
                int m=outer_vertices.size(),index;
                hull.resize(m);
                for (const_iterateur jt=outer_vertices.begin();jt!=outer_vertices.end();++jt) {
                    index=C.node_index(*jt);
                    if (index==-1)
                        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
                    hull[jt-outer_vertices.begin()]=index;
                }
            }
            graphe::layout &x=layouts[i];
            if (it->size()<3)
                comp_method=_GT_STYLE_SPRING;
            else if (method==_GT_STYLE_DEFAULT) {
                if (C.is_tree())
                    comp_method=_GT_STYLE_TREE;
                else if (C.is_bipartite(partition1,partition2) && partition1.size()>1 && partition2.size()>1)
                    comp_method=_GT_STYLE_BIPARTITE;
                else
                    comp_method=_GT_STYLE_CIRCLE;
            }
            switch (comp_method) {
            case _GT_STYLE_SPRING:
                C.make_spring_layout(x,2);
                break;
            case _GT_STYLE_3D:
                C.make_spring_layout(x,3);
                break;
            case _GT_STYLE_TREE:
                if (check && !C.is_tree())
                    return gt_err(_GT_ERR_NOT_A_TREE);
                C.make_tree_layout(x,sep,roots.empty()?0:C.node_index(roots[i]));
                break;
            case _GT_STYLE_PLANAR:
                if (!C.make_planar_layout(x))
                    return gt_err(_GT_ERR_NOT_PLANAR);
                break;
            case _GT_STYLE_CIRCLE:
                if (hull.empty()) {
                    gen_map vmap;
                    for (int vi=C.node_count();vi-->0;) {
                        vmap[C.node_label(vi)]=vi;
                    }
                    for (gen_map::const_iterator it=vmap.begin();it!=vmap.end();++it) {
                        hull.push_back(it->second.val);
                    }
                }
                C.make_circular_layout(x,hull,1.25);
                hull.clear();
                break;
            case _GT_STYLE_BIPARTITE:
                if (check && !C.is_bipartite(partition1,partition2))
                    return gt_err(_GT_ERR_NOT_BIPARTITE);
                C.make_bipartite_layout(x,partition1,partition2);
                break;
            }
            if (C.node_count()>2 && (comp_method==_GT_STYLE_PLANAR || comp_method==_GT_STYLE_SPRING))
                C.layout_best_rotation(x);
            if (comp_method!=_GT_TREE)
                graphe::scale_layout(x,sep*std::sqrt((double)C.node_count()));
            Cv.push_back(C);
        }
        // combine component layouts
        graphe::point dx(method==_GT_STYLE_3D?3:2,0.0);
        for (int i=0;i<nc;++i) {
            graphe::rectangle &rect=bounding_rects[i];
            rect=graphe::layout_bounding_rect(layouts[i],sep/PLASTIC_NUMBER_3);
            dx[0]=-rect.x();
            dx[1]=-rect.y();
            graphe::translate_layout(layouts[i],dx);
        }
        graphe::rectangle::comparator comp;
        sort(bounding_rects.begin(),bounding_rects.end(),comp);
        graphe::pack_rectangles(bounding_rects);
        for (vector<graphe::rectangle>::const_iterator it=bounding_rects.begin();it!=bounding_rects.end();++it) {
            dx[0]=it->x();
            dx[1]=it->y();
            graphe::translate_layout(*(it->get_layout()),dx);
        }
        int i,j;
        main_layout.resize(G.node_count());
        for (vector<graphe>::const_iterator it=Cv.begin();it!=Cv.end();++it) {
            graphe::layout &x=layouts[it-Cv.begin()];
            for (graphe::layout_iter jt=x.begin();jt!=x.end();++jt) {
                const graphe::vertex &v=it->node(jt-x.begin());
                i=components[it-Cv.begin()][jt-x.begin()];
                main_layout[i]=*jt;
                if (isdir) {
                    for (graphe::ivector_iter nt=v.neighbors().begin();nt!=v.neighbors().end();++nt) {
                        j=components[it-Cv.begin()][*nt];
                        const graphe::attrib &attr=v.neighbor_attributes(*nt);
                        if (G_orig.has_edge(i,j))
                            G_orig.set_edge_attribute(i,j,_GT_ATTRIB_POSITION,attr.find(_GT_ATTRIB_POSITION)->second);
                    }
                }
            }
        }
    }
    vecteur drawing,coords;
    if (!is_undef(coords_dest)) {
        // store vertex coordinates to coords_dest
        coords.resize(main_layout.size());
        for (graphe::layout_iter it=main_layout.begin();it!=main_layout.end();++it) {
            coords[it-main_layout.begin()]=method==_GT_STYLE_3D?
                        makevecteur(it->at(0),it->at(1),it->at(2)) :
                        makecomplex(it->front(),it->back());
        }
        identifier_assign(*coords_dest._IDNTptr,coords,contextptr);
    }
    if (isdir || G_orig.is_weighted())
        G_orig.edge_labels_placement(main_layout);
    if (method!=_GT_STYLE_3D) {
        drawing.push_back(symb_equal(change_subtype(gen(_AXES),_INT_PLOT),0));
        drawing.push_back(symb_equal(change_subtype(gen(_GL_ORTHO),_INT_PLOT),1));
    }
    G_orig.draw_edges(drawing,main_layout);
    G_orig.draw_nodes(drawing,main_layout);
    if (labels)
        G_orig.draw_labels(drawing,main_layout);
    return drawing;
}
static const char _draw_graph_s[]="draw_graph";
static define_unary_function_eval(__draw_graph,&_draw_graph,_draw_graph_s);
define_unary_function_ptr5(at_draw_graph,alias_at_draw_graph,&__draw_graph,0,true)

/* USAGE:   sierpinski_graph(n,k,[triangle])
 *
 * Returns Sierpinski (triangle) graph S(n,k) (resp. ST(n,k)).
 */
gen _sierpinski_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    int n,k=3;
    bool trng=false;
    if (g.is_integer())
        n=g.val;
    else {
        if (g.type!=_VECT || g.subtype!=_SEQ__VECT ||
                !g._VECTptr->front().is_integer() || !g._VECTptr->at(1).is_integer())
            return gentypeerr();
        n=g._VECTptr->front().val;
        k=g._VECTptr->at(1).val;
        if (n<=0 || k<=0)
            return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
        if (g._VECTptr->size()>2 && g._VECTptr->at(2)==at_triangle)
            trng=true;
    }
    graphe G(contextptr);
    G.make_sierpinski_graph(n,k,trng);
    return G.to_gen();
}
static const char _sierpinski_graph_s[]="sierpinski_graph";
static define_unary_function_eval(__sierpinski_graph,&_sierpinski_graph,_sierpinski_graph_s);
define_unary_function_ptr5(at_sierpinski_graph,alias_at_sierpinski_graph,&__sierpinski_graph,0,true)

/* USAGE:   complete_graph(n or V)
 *          complete_graph(m,n)
 *          complete_graph(n1,n2,...,nk)
 *
 * Create and return a complete graph with vertices from list V or enumerated
 * with first n integers. When two positive integers m and n are given, a
 * complete bipartite graph is returned (or complete k-partite graph if k
 * integers n1,n2,...,nk are given).
 */
gen _complete_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (g.type==_VECT && g.subtype!=_SEQ__VECT) {
        G.add_nodes(*g._VECTptr);
        G.make_complete_graph();
    } else if (g.is_integer() && g.val>0){
        vecteur V;
        G.make_default_labels(V,g.val);
        G.add_nodes(V);
        G.make_complete_graph();
    } else if (g.type==_VECT && g.subtype==_SEQ__VECT) {
        // multipartite graph
        vecteur &partition_sizes_gen=*g._VECTptr;
        vector<int> partition_sizes;
        for (const_iterateur it=partition_sizes_gen.begin();it!=partition_sizes_gen.end();++it) {
            if (!it->is_integer() || it->val<=0)
                return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
            partition_sizes.push_back(it->val);
        }
        graphe::layout x;
        G.make_complete_multipartite_graph(partition_sizes,&x);
        if (!x.empty())
            G.store_layout(x);
    } else
        return gentypeerr(contextptr);
    return G.to_gen();
}
static const char _complete_graph_s[]="complete_graph";
static define_unary_function_eval(__complete_graph,&_complete_graph,_complete_graph_s);
define_unary_function_ptr5(at_complete_graph,alias_at_complete_graph,&__complete_graph,0,true)

/* USAGE:   petersen_graph(n,[k])
 *
 * Returns the generalized Petersen graph G(n,k), where n and k are positive
 * integers. Parameter k defaults to 2.
 */
gen _petersen_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    int n=5,k=2;
    if (g.is_integer()) {
        n=g.val;
        if (n<=0)
            return gentypeerr(contextptr);
    } else if (g.type==_VECT && g.subtype==_SEQ__VECT) {
        if ((g._VECTptr->size())!=2)
            return gensizeerr(contextptr);
        if (!g._VECTptr->front().is_integer() || !g._VECTptr->at(1).is_integer())
            return gentypeerr(contextptr);
        n=g._VECTptr->front().val;
        k=g._VECTptr->at(1).val;
        if (n<=0 || k<=0)
            return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    }
    graphe G(contextptr);
    graphe::layout x;
    G.make_petersen_graph(n,k,&x);
    G.store_layout(x);
    return G.to_gen();
}
static const char _petersen_graph_s[]="petersen_graph";
static define_unary_function_eval(__petersen_graph,&_petersen_graph,_petersen_graph_s);
define_unary_function_ptr5(at_petersen_graph,alias_at_petersen_graph,&__petersen_graph,0,true)

/* USAGE:   random_graph(n or V,p)
 *          random_graph(n or V,m)
 *
 * Returns a random undirected unweighted graph with n vertices where two
 * vertices are connected with probability p. Alternatively, m edges are
 * created at random. Instead of number n of vertices, a list V of vertex
 * labels may be specified.
 */
gen _random_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    return randomgraph(*g._VECTptr,false,contextptr);
}
static const char _random_graph_s[]="random_graph";
static define_unary_function_eval(__random_graph,&_random_graph,_random_graph_s);
define_unary_function_ptr5(at_random_graph,alias_at_random_graph,&__random_graph,0,true)

/* USAGE:   random_digraph(n or V,p)
 *          random_digraph(n or V,m)
 *
 * Returns a random directed unweighted graph with n vertices where two
 * vertices are connected with probability p. Alternatively, m edges are
 * created at random. Instead of number n of vertices, a list V of vertex
 * labels may be specified.
 */
gen _random_digraph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    return randomgraph(*g._VECTptr,true,contextptr);
}
static const char _random_digraph_s[]="random_digraph";
static define_unary_function_eval(__random_digraph,&_random_digraph,_random_digraph_s);
define_unary_function_ptr5(at_random_digraph,alias_at_random_digraph,&__random_digraph,0,true)

/* USAGE:   random_bipartite_graph(n or [a,b],p)
 *          random_bipartite_graph(n or [a,b],m)
 *
 * Returns a random undirected unweighted bipartite graph with n vertices where
 * each possible edge is present with probability p. Alternatively, m edges are
 * created at random. Also, when first argument is list [a,b] of integers, two
 * groups of vertices with sizes a and b are created.
 */
gen _random_bipartite_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (!is_strictly_positive(gv.back(),contextptr))
        return gentypeerr(contextptr);
    int n,a,b;
    vecteur V,W;
    graphe G(contextptr);
    if (gv.front().is_integer()) {
        n=gv.front().val;
        if (n<1)
            return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
        a=G.rand_integer(n-1)+1;
        b=n-a;
    } else if (gv.front().type==_VECT && gv.front()._VECTptr->size()==2) {
        vecteur &ab=*gv.front()._VECTptr;
        if (!ab.front().is_integer() || !ab.back().is_integer())
            return generrtype("Expected a pair of integers");
        a=ab.front().val;
        b=ab.back().val;
        if (a<=0 || b<=0)
            return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    } else return gentypeerr(contextptr);
    int m=0;
    double p;
    if (gv.back().is_integer()) {
        if ((m=gv.back().val)<1)
            return generr("Expected a positive integer");
        if (m>a*b)
            return generr("Number of edges too large");
        p=m;
    } else p=gv.back().DOUBLE_val();
    G.make_default_labels(V,a,0);
    G.make_default_labels(W,b,a);
    G.reserve_nodes(a+b);
    G.add_nodes(mergevecteur(V,W));
    G.make_random_bipartite(a,b,p);
    return G.to_gen();
}
static const char _random_bipartite_graph_s[]="random_bipartite_graph";
static define_unary_function_eval(__random_bipartite_graph,&_random_bipartite_graph,_random_bipartite_graph_s);
define_unary_function_ptr5(at_random_bipartite_graph,alias_at_random_bipartite_graph,&__random_bipartite_graph,0,true)

/* USAGE:   random_tournament(n or V)
 *
 * Returns a random tournament graph with n vertices, which may be specified as
 * list V of their labels.
 */
gen _random_tournament(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    G.set_directed(true);
    if (!vertices_from_integer_or_vecteur(g,G))
        return gt_err(_GT_ERR_BAD_VERTICES);
    int n=G.node_count();
    if (n<2)
        return gensizeerr(contextptr);
    for (int i=0;i<n;++i) {
        for (int j=i+1;j<n;++j) {
            if (giac_rand(contextptr)%2==0)
                G.add_edge(i,j);
            else
                G.add_edge(j,i);
        }
    }
    return G.to_gen();
}
static const char _random_tournament_s[]="random_tournament";
static define_unary_function_eval(__random_tournament,&_random_tournament,_random_tournament_s);
define_unary_function_ptr5(at_random_tournament,alias_at_random_tournament,&__random_tournament,0,true)

/* USAGE:   random_regular_graph(n or V,d,[connected])
 *
 * Returns a random d-regular graph with n vertices, which may be specified as
 * list V of their labels.
 */
gen _random_regular_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    vecteur V;
    vecteur &gv=*g._VECTptr;
    if (gv.front().is_integer())
        G.make_default_labels(V,gv.front().val);
    else if (gv.front().type==_VECT)
        V=*gv.front()._VECTptr;
    else
        return gentypeerr(contextptr);
    if (gv.size()<2)
        return gensizeerr(contextptr);
    if (!gv[1].is_integer() || !is_strictly_positive(gv[1],contextptr))
        return gentypeerr(contextptr);
    int d=gv[1].val;
    bool connected=false;
    if (gv.size()>2 && gv[2].is_integer() && gv[2].val==_GT_CONNECTED)
        connected=true;
    int n=V.size();
    if (n<=d+1 || (n*d)%2!=0) // check the necessary condition
        return generr("Graph does not exist");
    G.reserve_nodes(V.size());
    G.add_nodes(V);
    G.make_random_regular(d,connected);
    return G.to_gen();
}
static const char _random_regular_graph_s[]="random_regular_graph";
static define_unary_function_eval(__random_regular_graph,&_random_regular_graph,_random_regular_graph_s);
define_unary_function_ptr5(at_random_regular_graph,alias_at_random_regular_graph,&__random_regular_graph,0,true)

/* USAGE:   random_sequence_graph(List(D))
 *
 * Returns a random graph with n=|D| vertices where i-th vertex has degree D[i].
 */
gen _random_sequence_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    int n=g._VECTptr->size(),m=0;
    if (n==0)
        return generr("Expected a non-empty list");
    if (_is_graphic_sequence(g,contextptr)==graphe::FAUX)
        return gt_err(_GT_ERR_NOT_A_GRAPHIC_SEQUENCE);
    graphe::ivector deg(n);
    for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
        m+=(deg[it-g._VECTptr->begin()]=it->val);
    }
    if ((m%2)!=0)
        return generr("Sum of degrees must be even");
    graphe G(contextptr);
    vecteur labels;
    G.make_default_labels(labels,n);
    G.reserve_nodes(n);
    G.add_nodes(labels);
    G.make_random_sequential(deg);
    return G.to_gen();
}
static const char _random_sequence_graph_s[]="random_sequence_graph";
static define_unary_function_eval(__random_sequence_graph,&_random_sequence_graph,_random_sequence_graph_s);
define_unary_function_ptr5(at_random_sequence_graph,alias_at_random_sequence_graph,&__random_sequence_graph,0,true)

/* USAGE:   random_tree(n or V,[d])
 *
 * Returns a random tree graph with n vertices, which may be specified as list
 * V of their labels. Optional parameter d is a positive integer which
 * represents the upper bound for degree of graph.
 */
gen _random_tree(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    int maxd=-1,n;
    bool rooted=false;
    vecteur V;
    graphe G(contextptr);
    if (g.is_integer()) {
        n=g.val;
    } else if (g.type==_VECT) {
        vecteur &gv=*g._VECTptr;
        if (g.subtype==_SEQ__VECT) {
            if (gv.size()!=2)
                return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
            if (gv.front().is_integer()) {
                n=gv.front().val;
            } else if (gv.front().type==_VECT) {
                V=*gv.front()._VECTptr;
                n=V.size();
            } else
                return gt_err(_GT_ERR_BAD_VERTICES);
            if (gv.back().is_integer()) {
                if ((maxd=gv.back().val)<2)
                    return generr("Maximum degree must be at least two");
            } else if (is_inf(gv.back()))
                maxd=RAND_MAX;
            else if (gv.back()==at_maple_root)
                rooted=true;
            else if (gv.back().is_symb_of_sommet(at_equal) &&
                     gv.back()._SYMBptr->feuille._VECTptr->front()==at_maple_root &&
                     gv.front().type==_VECT) {
                gen root_label=gv.back()._SYMBptr->feuille._VECTptr->back();
                iterateur it;
                if ((it=find(V.begin(),V.end(),root_label))==V.end())
                    return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
                V.erase(it);
                V.insert(V.begin(),root_label);
                rooted=true;
            }
            else return generr("Invalid option");
        } else {
            V=gv;
            n=V.size();
        }
    } else return gentypeerr(contextptr);
    if (n<1)
        return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    if (V.empty())
        G.make_default_labels(V,n);
    G.reserve_nodes(V.size());
    G.add_nodes(V);
    if (maxd>0)
        G.make_random_tree(maxd);
    else if (rooted)
        G.make_random_rooted_tree();
    else
        G.make_random_free_tree();
    return G.to_gen();
}
static const char _random_tree_s[]="random_tree";
static define_unary_function_eval(__random_tree,&_random_tree,_random_tree_s);
define_unary_function_ptr5(at_random_tree,alias_at_random_tree,&__random_tree,0,true)

/* USAGE:   random_planar_graph(n or V,p,[c])
 *
 * Return random biconnected planar graph with n vertices, which can also be
 * specified as a list V of their labels.
 */
gen _random_planar_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    double p=0.5;
    int connectivity=1;
    gen spec;
    if (g.is_integer() || (g.type==_VECT && g.subtype!=_SEQ__VECT))
        spec=g;
    else if (g.type==_VECT && g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()>1 && gv.size()<=3) {
            spec=gv.front();
            if (_evalf(gv[1],contextptr).type!=_DOUBLE_)
                return gentypeerr(contextptr);
            p=gv[1].DOUBLE_val();
            if (p<0 || p>=1)
                return generrtype("Invalid probability specification");
            if (gv.size()==3) {
                if (!gv.back().is_integer() || gv.back().val<0 || gv.back().val>3)
                    return generrtype("Invalid connectivity specification");
                connectivity=gv.back().val;
            }
        } else return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    } else return gentypeerr(contextptr);
    graphe G(contextptr);
    if (!vertices_from_integer_or_vecteur(spec,G))
        return gt_err(_GT_ERR_BAD_VERTICES);
    G.make_random_planar(p,connectivity);
    return G.to_gen();
}
static const char _random_planar_graph_s[]="random_planar_graph";
static define_unary_function_eval(__random_planar_graph,&_random_planar_graph,_random_planar_graph_s);
define_unary_function_ptr5(at_random_planar_graph,alias_at_random_planar_graph,&__random_planar_graph,0,true)

/* USAGE:   assign_edge_weights(G,m,n)
 *          assign_edge_weights(G,a..b)
 *
 * Assigns random edge weights to the edges of graph G and returns a modified
 * copy of G. If integers n and m such that n>=m are specified, weights are
 * integers randomly chosen in [m,n]. If an interval a..b is specified, weights
 * are uniformly distributed in the interval [a,b).
 */
gen _assign_edge_weights(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    G.set_weighted(true);
    if (gv.size()==3) {
        if (!gv[1].is_integer() || !gv[2].is_integer())
            return gentypeerr(contextptr);
        int m=gv[1].val,n=gv[2].val;
        if (m>n)
            return generr("Lower bound too high");
        G.randomize_edge_weights(m,n,true);
    } else if (gv.size()==2) {
        if (!gv.back().is_symb_of_sommet(at_interval))
            return generrtype("Expected an interval");
        gen a=gv.back()._SYMBptr->feuille._VECTptr->front(),
                b=gv.back()._SYMBptr->feuille._VECTptr->back();
        if (!graphe::is_real_number(a) || !graphe::is_real_number(b))
            return generrtype("Expected an interval of reals");
        G.randomize_edge_weights(_evalf(a,contextptr).DOUBLE_val(),_evalf(b,contextptr).DOUBLE_val(),false);
    }
    return G.to_gen();
}
static const char _assign_edge_weights_s[]="assign_edge_weights";
static define_unary_function_eval(__assign_edge_weights,&_assign_edge_weights,_assign_edge_weights_s);
define_unary_function_ptr5(at_assign_edge_weights,alias_at_assign_edge_weights,&__assign_edge_weights,0,true)

/* USAGE:   articulation_points(G)
 *
 * Returns the list of articulation points (i.e. cut vertices) of graph G.
 */
gen _articulation_points(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    graphe::ivector v;
    if (G.is_directed()) {
        graphe U(contextptr,false);
        G.underlying(U);
        U.find_cut_vertices(v);
    } else G.find_cut_vertices(v);
    return G.get_node_labels(v);
}
static const char _articulation_points_s[]="articulation_points";
static define_unary_function_eval(__articulation_points,&_articulation_points,_articulation_points_s);
define_unary_function_ptr5(at_articulation_points,alias_at_articulation_points,&__articulation_points,0,true)

/* USAGE:   biconnected_components(G)
 *
 * Returns the list of biconnected components of graph G. Every component is
 * given as a list of vertices belonging to that component.
 */
gen _biconnected_components(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    graphe::ivectors comp;
    if (G.is_directed()) {
        graphe U(contextptr,false);
        G.underlying(U);
        U.biconnected_components(comp);
    } else G.biconnected_components(comp);
    vecteur res;
    G.ivectors2vecteur(comp,res,true);
    return change_subtype(res,_LIST__VECT);
}
static const char _biconnected_components_s[]="biconnected_components";
static define_unary_function_eval(__biconnected_components,&_biconnected_components,_biconnected_components_s);
define_unary_function_ptr5(at_biconnected_components,alias_at_biconnected_components,&__biconnected_components,0,true)

/* USAGE:   add_arc(G,e)
 *
 * Returns graph G (which must be directed) with added arc e (or trail or list
 * of arcs).
 */
gen _add_arc(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (g._VECTptr->size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (gv.back().type!=_VECT)
        return gt_err(_GT_ERR_INVALID_EDGE);
    vecteur E=gv.back().is_symb_of_sommet(at_trail)?
                *gv.back()._SYMBptr->feuille._VECTptr:*gv.back()._VECTptr;
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    if (!parse_edges(G,E,ckmatrix(gv.back())))
        return gendimerr(contextptr);
    return G.to_gen();
}
static const char _add_arc_s[]="add_arc";
static define_unary_function_eval(__add_arc,&_add_arc,_add_arc_s);
define_unary_function_ptr5(at_add_arc,alias_at_add_arc,&__add_arc,0,true)

/* USAGE:   delete_arc(G,e)
 *
 * Returns graph G (which must be directed) with arc e (or trail or list of
 * arcs) removed.
 */
gen _delete_arc(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (gv.back().type!=_VECT)
        return gt_err(_GT_ERR_INVALID_EDGE);
    vecteur E=gv.back().is_symb_of_sommet(at_trail)?
                *gv.back()._SYMBptr->feuille._VECTptr:*gv.back()._VECTptr;
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    if (!delete_edges(G,E))
        return gendimerr(contextptr);
    return G.to_gen();
}
static const char _delete_arc_s[]="delete_arc";
static define_unary_function_eval(__delete_arc,&_delete_arc,_delete_arc_s);
define_unary_function_ptr5(at_delete_arc,alias_at_delete_arc,&__delete_arc,0,true)

/* USAGE:   add_edge(G,e)
 *
 * Returns graph G (which must be undirected) with added edge e (or trail or
 * list of edges).
 */
gen _add_edge(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (gv.back().type!=_VECT)
        return gt_err(_GT_ERR_INVALID_EDGE);
    vecteur E=gv.back().is_symb_of_sommet(at_trail)?
                *gv.back()._SYMBptr->feuille._VECTptr:*gv.back()._VECTptr;
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (!parse_edges(G,E,ckmatrix(gv.back())))
        return gendimerr(contextptr);
    return G.to_gen();
}
static const char _add_edge_s[]="add_edge";
static define_unary_function_eval(__add_edge,&_add_edge,_add_edge_s);
define_unary_function_ptr5(at_add_edge,alias_at_add_edge,&__add_edge,0,true)

/* USAGE:   delete_edge(G,e)
 *
 * Returns graph G (which must be undirected) with edge e (or trail or list of
 * edges) removed.
 */
gen _delete_edge(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (gv.back().type!=_VECT)
        return gt_err(_GT_ERR_INVALID_EDGE);
    vecteur E=gv.back().is_symb_of_sommet(at_trail)?
                *gv.back()._SYMBptr->feuille._VECTptr:*gv.back()._VECTptr;
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (!delete_edges(G,E))
        return gendimerr(contextptr);
    return G.to_gen();
}
static const char _delete_edge_s[]="delete_edge";
static define_unary_function_eval(__delete_edge,&_delete_edge,_delete_edge_s);
define_unary_function_ptr5(at_delete_edge,alias_at_delete_edge,&__delete_edge,0,true)

/* USAGE:   add_vertex(G,v)
 *
 * Returns graph G with added vertex v (or vertices from v if v is a list).
 */
gen _add_vertex(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    if (g._VECTptr->size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    gen &V=g._VECTptr->back();
    if (V.type==_VECT)
        G.add_nodes(*V._VECTptr);
    else
        G.add_node(V);
    return G.to_gen();
}
static const char _add_vertex_s[]="add_vertex";
static define_unary_function_eval(__add_vertex,&_add_vertex,_add_vertex_s);
define_unary_function_ptr5(at_add_vertex,alias_at_add_vertex,&__add_vertex,0,true)

/* USAGE:   delete_vertex(G,v)
 *
 * Returns graph G with vertex v (or vertices from v if v is a list) removed.
 */
gen _delete_vertex(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    if (g._VECTptr->size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr),H(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    gen &V=g._VECTptr->back();
    graphe::iset s;
    if (V.type==_VECT) {
        G.labels2iset(*V._VECTptr,s);
        G.isolate_nodes(s);
        G.remove_isolated_nodes(s,H);
    } else {
        s.insert(G.node_index(V));
        if (*s.begin()==-1)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        G.isolate_nodes(s);
        G.remove_isolated_nodes(s,H);
    }
    return H.to_gen();
}
static const char _delete_vertex_s[]="delete_vertex";
static define_unary_function_eval(__delete_vertex,&_delete_vertex,_delete_vertex_s);
define_unary_function_ptr5(at_delete_vertex,alias_at_delete_vertex,&__delete_vertex,0,true)

/* USAGE:   contract_edge(G,e)
 *
 * Returns graph G (undirected) with edge e contracted (collapsed).
 */
gen _contract_edge(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    if (g._VECTptr->size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (g._VECTptr->back().type!=_VECT)
        return gt_err(_GT_ERR_INVALID_EDGE);
    graphe G(contextptr),H(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    vecteur &E=*g._VECTptr->back()._VECTptr;
    if (E.size()!=2)
        return gensizeerr(contextptr);
    int i=G.node_index(E.front()),j=G.node_index(E.back());
    if (i<0 || j<0 || !G.has_edge(i,j))
        return gt_err(_GT_ERR_EDGE_NOT_FOUND);
    G.contract_edge(i,j);
    graphe::iset s;
    s.insert(j);
    G.remove_isolated_nodes(s,H);
    return H.to_gen();
}
static const char _contract_edge_s[]="contract_edge";
static define_unary_function_eval(__contract_edge,&_contract_edge,_contract_edge_s);
define_unary_function_ptr5(at_contract_edge,alias_at_contract_edge,&__contract_edge,0,true)

/* USAGE:   connected_components(G)
 *
 * Returns list of lists of vertices, each sublist representing a connected
 * component of graph G. Individual components can be made available as
 * subgraphs of G by applying the induced_subgraph command.
 */
gen _connected_components(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    graphe::ivectors comp;
    if (G.is_directed()) {
        graphe U(contextptr,false);
        G.underlying(U);
        U.connected_components(comp);
    } else  G.connected_components(comp);
    vecteur res;
    G.ivectors2vecteur(comp,res,true);
    return change_subtype(res,_LIST__VECT);
}
static const char _connected_components_s[]="connected_components";
static define_unary_function_eval(__connected_components,&_connected_components,_connected_components_s);
define_unary_function_ptr5(at_connected_components,alias_at_connected_components,&__connected_components,0,true)

/* USAGE:   departures(G,[v])
 *
 * Returns the list of vertices of directed graph G which are connected by v
 * with arcs such that tails are in v. If v is omitted, list of departures is
 * computed for every vertex and a list of lists is returned.
 */
gen _departures(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    return flights(g,false,g.subtype!=_SEQ__VECT,contextptr);
}
static const char _departures_s[]="departures";
static define_unary_function_eval(__departures,&_departures,_departures_s);
define_unary_function_ptr5(at_departures,alias_at_departures,&__departures,0,true)

/* USAGE:   arrivals(G,[v])
 *
 * Returns the list of vertices of directed graph G which are connected by v
 * with arcs such that heads are in v. If v is omitted, list of arrivals is
 * computed for every vertex and a list of lists is returned.
 */
gen _arrivals(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    return flights(g,true,g.subtype!=_SEQ__VECT,contextptr);
}
static const char _arrivals_s[]="arrivals";
static define_unary_function_eval(__arrivals,&_arrivals,_arrivals_s);
define_unary_function_ptr5(at_arrivals,alias_at_arrivals,&__arrivals,0,true)

/* USAGE:   incident_edges(G,v)
 *
 * Returns the list of all edges incident to the vertex v (or to the vertices in
 * the list v).
 */
gen _incident_edges(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    if (g._VECTptr->size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    vecteur V;
    if (g._VECTptr->back().type==_VECT)
        V=*g._VECTptr->back()._VECTptr;
    else
        V.push_back(g._VECTptr->back());
    graphe::ivector indices;
    int i;
    for (const_iterateur it=V.begin();it!=V.end();++it) {
        if ((i=G.node_index(*it))==-1)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        indices.push_back(i);
    }
    graphe::edgeset E;
    G.incident_edges(indices,E);
    vecteur res;
    for (graphe::edgeset_iter it=E.begin();it!=E.end();++it) {
        res.push_back(makevecteur(G.node_label(it->first),G.node_label(it->second)));
    }
    return change_subtype(res,_LIST__VECT);
}
static const char _incident_edges_s[]="incident_edges";
static define_unary_function_eval(__incident_edges,&_incident_edges,_incident_edges_s);
define_unary_function_ptr5(at_incident_edges,alias_at_incident_edges,&__incident_edges,0,true)

/* USAGE:   make_weighted(G,[M])
 *
 * Returns the copy of graph G with edge/arc weights set as specified by matrix
 * M. If M is omitted, a suqare matrix of ones is used. If G is undirected, M
 * is assumed to be symmetric.
 */
gen _make_weighted(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    bool has_matrix=g.subtype==_SEQ__VECT;
    if (has_matrix && g._VECTptr->size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(has_matrix?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_weighted()) {
        if (!has_matrix)
            return gt_err(_GT_ERR_UNWEIGHTED_GRAPH_REQUIRED);
        graphe::ipairs E;
        G.get_edges_as_pairs(E);
        G.set_weighted(false);
        for (graphe::ipairs_iter it=E.begin();it!=E.end();++it) {
            G.discard_edge_attribute(it->first,it->second,_GT_ATTRIB_WEIGHT);
        }
    }
    int n=G.node_count();
    matrice m=*_matrix(makesequence(n,n,1),contextptr)._VECTptr;
    if (has_matrix) {
        m=*g._VECTptr->back()._VECTptr;
        if (int(m.size())!=n || int(m.front()._VECTptr->size())!=n)
            return gendimerr(contextptr);
    }
    G.make_weighted(m);
    return G.to_gen();
}
static const char _make_weighted_s[]="make_weighted";
static define_unary_function_eval(__make_weighted,&_make_weighted,_make_weighted_s);
define_unary_function_ptr5(at_make_weighted,alias_at_make_weighted,&__make_weighted,0,true)

/* USAGE:   set_graph_attribute(G,attr1,attr2,...)
 *
 * Stores the attributes attr1, attr2, ..., each in form tag=value, where tag
 * is string, and returns the modified copy of G. Attributes may also be
 * specified in a list or as two lists [tag1,tag2,...] and [value1,value2,...].
 */
gen _set_graph_attribute(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr,attr;
    graphe G(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (gv.size()==2 && gv.back().type==_VECT)
        attr=*gv.back()._VECTptr;
    else if (gv.size()==3 && gv[1].type==_VECT && gv[2].type==_VECT)
        attr=*_zip(makesequence(at_equal,gv[1],gv[2]),contextptr)._VECTptr;
    else attr=vecteur(gv.begin()+1,gv.end());
    int key;
    for (const_iterateur it=attr.begin();it!=attr.end();++it) {
        if (!it->is_symb_of_sommet(at_equal) || it->_SYMBptr->feuille._VECTptr->front().type!=_STRNG)
            return gt_err(_GT_ERR_TAGVALUE_PAIR_EXPECTED);
        key=G.tag2index(graphe::genstring2str(it->_SYMBptr->feuille._VECTptr->front()));
        G.set_graph_attribute(key,it->_SYMBptr->feuille._VECTptr->back());
    }
    return G.to_gen();
}
static const char _set_graph_attribute_s[]="set_graph_attribute";
static define_unary_function_eval(__set_graph_attribute,&_set_graph_attribute,_set_graph_attribute_s);
define_unary_function_ptr5(at_set_graph_attribute,alias_at_set_graph_attribute,&__set_graph_attribute,0,true)

/* USAGE:   set_vertex_attribute(G,v,attr1,attr2,...)
 *
 * Stores the attributes attr1, attr2, ..., each in form tag=value, where tag
 * is string, to vertex v and returns the modified copy of G. Attributes may
 * also be specified in a list or as two lists [tag1,tag2,...] and
 * [value1,value2,...].
 */
gen _set_vertex_attribute(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr,attr;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int v,key;
    if ((v=G.node_index(gv[1]))<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    if (gv.size()==3 && gv.back().type==_VECT)
        attr=*gv.back()._VECTptr;
    else if (gv.size()==4 && gv[2].type==_VECT && gv[3].type==_VECT)
        attr=*_zip(makesequence(at_equal,gv[2],gv[3]),contextptr)._VECTptr;
    else attr=vecteur(gv.begin()+2,gv.end());
    for (const_iterateur it=attr.begin();it!=attr.end();++it) {
        if (!it->is_symb_of_sommet(at_equal) || it->_SYMBptr->feuille._VECTptr->front().type!=_STRNG)
            return gt_err(_GT_ERR_TAGVALUE_PAIR_EXPECTED);
        key=G.tag2index(graphe::genstring2str(it->_SYMBptr->feuille._VECTptr->front()));
        G.set_node_attribute(v,key,it->_SYMBptr->feuille._VECTptr->back());
    }
    return G.to_gen();
}
static const char _set_vertex_attribute_s[]="set_vertex_attribute";
static define_unary_function_eval(__set_vertex_attribute,&_set_vertex_attribute,_set_vertex_attribute_s);
define_unary_function_ptr5(at_set_vertex_attribute,alias_at_set_vertex_attribute,&__set_vertex_attribute,0,true)

/* USAGE:   set_edge_attribute(G,e,attr1,attr2,...)
 *
 * Stores the attributes attr1, attr2, ..., each in form tag=value, where tag
 * is string, to edge e and returns the modified copy of G. Attributes may also
 * be specified in a list or as two lists [tag1,tag2,...] and
 * [value1,value2,...].
 */
gen _set_edge_attribute(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr,attr;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int i,j,key;
    if (gv[1].type!=_VECT || gv[1]._VECTptr->size()!=2)
        return gt_err(_GT_ERR_INVALID_EDGE);
    if ((i=G.node_index(gv[1]._VECTptr->front()))<0 ||
            (j=G.node_index(gv[1]._VECTptr->back()))<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    if (!G.has_edge(i,j))
        return gt_err(_GT_ERR_EDGE_NOT_FOUND);
    if (gv.size()==3 && gv.back().type==_VECT)
        attr=*gv.back()._VECTptr;
    else if (gv.size()==4 && gv[2].type==_VECT && gv[3].type==_VECT)
        attr=*_zip(makesequence(at_equal,gv[2],gv[3]),contextptr)._VECTptr;
    else attr=vecteur(gv.begin()+2,gv.end());
    for (const_iterateur it=attr.begin();it!=attr.end();++it) {
        if (!it->is_symb_of_sommet(at_equal) || it->_SYMBptr->feuille._VECTptr->front().type!=_STRNG)
            return gt_err(_GT_ERR_TAGVALUE_PAIR_EXPECTED);
        key=G.tag2index(graphe::genstring2str(it->_SYMBptr->feuille._VECTptr->front()));
        G.set_edge_attribute(i,j,key,it->_SYMBptr->feuille._VECTptr->back());
    }
    return G.to_gen();
}
static const char _set_edge_attribute_s[]="set_edge_attribute";
static define_unary_function_eval(__set_edge_attribute,&_set_edge_attribute,_set_edge_attribute_s);
define_unary_function_ptr5(at_set_edge_attribute,alias_at_set_edge_attribute,&__set_edge_attribute,0,true)

/* USAGE:   get_graph_attribute(G,tag1,tag2,...)
 *
 * Get the graph attributes tag1, tag2, ..., i.e. return the sequence of values
 * corresponding to the given tags, which may also be specified in a list.
 */
gen _get_graph_attribute(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr,tags,values;
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    bool istagvec=gv.size()==2 && gv.back().type==_VECT;
    if (istagvec)
        tags=*gv.back()._VECTptr;
    else tags=vecteur(gv.begin()+1,gv.end());
    int key;
    gen value;
    for (const_iterateur it=tags.begin();it!=tags.end();++it) {
        if (it->type!=_STRNG)
            return gentypeerr(contextptr);
        key=G.tag2index(graphe::genstring2str(*it));
        G.get_graph_attribute(key,value);
        values.push_back(value);
    }
    return istagvec?values:change_subtype(values,_SEQ__VECT);
}
static const char _get_graph_attribute_s[]="get_graph_attribute";
static define_unary_function_eval(__get_graph_attribute,&_get_graph_attribute,_get_graph_attribute_s);
define_unary_function_ptr5(at_get_graph_attribute,alias_at_get_graph_attribute,&__get_graph_attribute,0,true)

/* USAGE:   get_vertex_attribute(G,v,tag1,tag2,...)
 *
 * Get the attributes tag1, tag2, ... assigned to vertex v in graph G, i.e.
 * return the sequence of values corresponding to the given tags, which may
 * also be specified in a list.
 */
gen _get_vertex_attribute(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr,tags,values;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int v,key;
    if ((v=G.node_index(gv[1]))<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    bool istagvec=gv.size()==3 && gv.back().type==_VECT;
    if (istagvec)
        tags=*gv.back()._VECTptr;
    else tags=vecteur(gv.begin()+2,gv.end());
    gen value;
    for (const_iterateur it=tags.begin();it!=tags.end();++it) {
        if (it->type!=_STRNG)
            return gentypeerr(contextptr);
        key=G.tag2index(graphe::genstring2str(*it));
        G.get_node_attribute(v,key,value);
        values.push_back(value);
    }
    return istagvec?values:change_subtype(values,_SEQ__VECT);
}
static const char _get_vertex_attribute_s[]="get_vertex_attribute";
static define_unary_function_eval(__get_vertex_attribute,&_get_vertex_attribute,_get_vertex_attribute_s);
define_unary_function_ptr5(at_get_vertex_attribute,alias_at_get_vertex_attribute,&__get_vertex_attribute,0,true)

/* USAGE:   get_edge_attribute(G,e,tag1,tag2,...)
 *
 * Get the attributes tag1, tag2, ... assigned to edge e in graph G, i.e.
 * return the sequence of values corresponding to the given tags, which may
 * also be specified in a list.
 */
gen _get_edge_attribute(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr,tags,values;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (gv[1].type!=_VECT || gv[1]._VECTptr->size()!=2)
        return gt_err(_GT_ERR_INVALID_EDGE);
    int i,j,key;
    if ((i=G.node_index(gv[1]._VECTptr->front()))<0 ||
            (j=G.node_index(gv[1]._VECTptr->back()))<0)
        return gt_err(_GT_ERR_EDGE_NOT_FOUND);
    if (!G.has_edge(i,j))
        return gt_err(_GT_ERR_EDGE_NOT_FOUND);
    bool istagvec=gv.size()==3 && gv.back().type==_VECT;
    if (istagvec)
        tags=*gv.back()._VECTptr;
    else tags=vecteur(gv.begin()+2,gv.end());
    if (tags.empty()) {
        const graphe::attrib &attr=G.edge_attributes(i,j);
        for (graphe::attrib_iter it=attr.begin();it!=attr.end();++it) {
            values.push_back(symbolic(at_equal,makesequence(graphe::str2gen(G.index2tag(it->first),true),it->second)));
        }
    } else {
        gen value;
        for (const_iterateur it=tags.begin();it!=tags.end();++it) {
            if (it->type!=_STRNG)
                return gentypeerr(contextptr);
            key=G.tag2index(graphe::genstring2str(*it));
            G.get_edge_attribute(i,j,key,value);
            values.push_back(value);
        }
    }
    return istagvec?values:change_subtype(values,_SEQ__VECT);
}
static const char _get_edge_attribute_s[]="get_edge_attribute";
static define_unary_function_eval(__get_edge_attribute,&_get_edge_attribute,_get_edge_attribute_s);
define_unary_function_ptr5(at_get_edge_attribute,alias_at_get_edge_attribute,&__get_edge_attribute,0,true)

/* USAGE:   discard_graph_attribute(G,tag1,tag2,...)
 *
 * Discards the graph attributes with tags tag1, tag2, ..., which may also be
 * specified in a list, and returns the modified copy of G.
 */
gen _discard_graph_attribute(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr,tags;
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (gv.size()==2 && gv.back().type==_VECT)
        tags=*gv.back()._VECTptr;
    else tags=vecteur(gv.begin()+1,gv.end());
    int key;
    for (const_iterateur it=tags.begin();it!=tags.end();++it) {
        if (it->type!=_STRNG)
            return gentypeerr(contextptr);
        key=G.tag2index(graphe::genstring2str(*it));
        G.discard_graph_attribute(key);
    }
    return G.to_gen();
}
static const char _discard_graph_attribute_s[]="discard_graph_attribute";
static define_unary_function_eval(__discard_graph_attribute,&_discard_graph_attribute,_discard_graph_attribute_s);
define_unary_function_ptr5(at_discard_graph_attribute,alias_at_discard_graph_attribute,&__discard_graph_attribute,0,true)

/* USAGE:   discard_vertex_attribute(G,v,tag1,tag2,...)
 *
 * Discards the attributes with tags tag1, tag2, ... assigned to vertex v in
 * graph G, which may also be specified in a list, and returns the modified
 * copy of G.
 */
gen _discard_vertex_attribute(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr,tags;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int v,key;
    if ((v=G.node_index(gv[1]))<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    if (gv.size()==3 && gv.back().type==_VECT)
        tags=*gv.back()._VECTptr;
    else tags=vecteur(gv.begin()+2,gv.end());
    for (const_iterateur it=tags.begin();it!=tags.end();++it) {
        if (it->type!=_STRNG)
            return gentypeerr(contextptr);
        key=G.tag2index(graphe::genstring2str(*it));
        G.discard_node_attribute(v,key);
    }
    return G.to_gen();
}
static const char _discard_vertex_attribute_s[]="discard_vertex_attribute";
static define_unary_function_eval(__discard_vertex_attribute,&_discard_vertex_attribute,_discard_vertex_attribute_s);
define_unary_function_ptr5(at_discard_vertex_attribute,alias_at_discard_vertex_attribute,&__discard_vertex_attribute,0,true)

/* USAGE:   discard_edge_attribute(G,e,tag1,tag2,...)
 *
 * Discards the attributes with tags tag1, tag2, ... assigned to edge e in
 * graph G, which may also be specified in a list, and returns the modified
 * copy of G.
 */
gen _discard_edge_attribute(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr,tags;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (gv[1].type!=_VECT || gv[1]._VECTptr->size()!=2)
        return gt_err(_GT_ERR_INVALID_EDGE);
    int i,j,key;
    if ((i=G.node_index(gv[1]._VECTptr->front()))<0 ||
            (j=G.node_index(gv[1]._VECTptr->back()))<0)
        return gt_err(_GT_ERR_EDGE_NOT_FOUND);
    if (!G.has_edge(i,j))
        return gt_err(_GT_ERR_EDGE_NOT_FOUND);
    if (gv.size()==3 && gv.back().type==_VECT)
        tags=*gv.back()._VECTptr;
    else tags=vecteur(gv.begin()+2,gv.end());
    for (const_iterateur it=tags.begin();it!=tags.end();++it) {
        if (it->type!=_STRNG)
            return gentypeerr(contextptr);
        key=G.tag2index(graphe::genstring2str(*it));
        G.discard_edge_attribute(i,j,key);
    }
    return G.to_gen();
}
static const char _discard_edge_attribute_s[]="discard_edge_attribute";
static define_unary_function_eval(__discard_edge_attribute,&_discard_edge_attribute,_discard_edge_attribute_s);
define_unary_function_ptr5(at_discard_edge_attribute,alias_at_discard_edge_attribute,&__discard_edge_attribute,0,true)

/* USAGE:   list_graph_attributes(G)
 *
 * Returns the list of graph attributes in form tag=value.
 */
gen _list_graph_attributes(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    vecteur tags,values;
    G.attrib2vecteurs(G.graph_attributes(),tags,values);
    return _zip(makesequence(at_equal,tags,values),contextptr);
}
static const char _list_graph_attributes_s[]="list_graph_attributes";
static define_unary_function_eval(__list_graph_attributes,&_list_graph_attributes,_list_graph_attributes_s);
define_unary_function_ptr5(at_list_graph_attributes,alias_at_list_graph_attributes,&__list_graph_attributes,0,true)

/* USAGE:   list_vertex_attributes(G,v)
 *
 * Returns the list of attributes assigned to vertex v in form tag=value.
 */
gen _list_vertex_attributes(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int i;
    if ((i=G.node_index(gv.back()))<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    vecteur tags,values;
    G.attrib2vecteurs(G.node_attributes(i),tags,values);
    return _zip(makesequence(at_equal,tags,values),contextptr);
}
static const char _list_vertex_attributes_s[]="list_vertex_attributes";
static define_unary_function_eval(__list_vertex_attributes,&_list_vertex_attributes,_list_vertex_attributes_s);
define_unary_function_ptr5(at_list_vertex_attributes,alias_at_list_vertex_attributes,&__list_vertex_attributes,0,true)

/* USAGE:   list_edge_attributes(G,e)
 *
 * Returns the list of attributes assigned to edge e in form tag=value.
 */
gen _list_edge_attributes(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (gv.back().type!=_VECT || gv.back()._VECTptr->size()!=2)
        return gt_err(_GT_ERR_INVALID_EDGE);
    int i,j;
    if ((i=G.node_index(gv.back()._VECTptr->front()))<0 ||
            (j=G.node_index(gv.back()._VECTptr->back()))<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    if (!G.has_edge(i,j))
        return gt_err(_GT_ERR_EDGE_NOT_FOUND);
    vecteur tags,values;
    G.attrib2vecteurs(G.edge_attributes(i,j),tags,values);
    return _zip(makesequence(at_equal,tags,values),contextptr);
}
static const char _list_edge_attributes_s[]="list_edge_attributes";
static define_unary_function_eval(__list_edge_attributes,&_list_edge_attributes,_list_edge_attributes_s);
define_unary_function_ptr5(at_list_edge_attributes,alias_at_list_edge_attributes,&__list_edge_attributes,0,true)

/* USAGE:   number_of_edges(G)
 *
 * Returns the number of edges/arcs of graph G.
 */
gen _number_of_edges(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return G.edge_count();
}
static const char _number_of_edges_s[]="number_of_edges";
static define_unary_function_eval(__number_of_edges,&_number_of_edges,_number_of_edges_s);
define_unary_function_ptr5(at_number_of_edges,alias_at_number_of_edges,&__number_of_edges,0,true)

/* USAGE:   number_of_vertices(G)
 *
 * Returns the number of vertices of graph G.
 */
gen _number_of_vertices(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return G.node_count();
}
static const char _number_of_vertices_s[]="number_of_vertices";
static define_unary_function_eval(__number_of_vertices,&_number_of_vertices,_number_of_vertices_s);
define_unary_function_ptr5(at_number_of_vertices,alias_at_number_of_vertices,&__number_of_vertices,0,true)

/* USAGE:   get_edge_weight(G,e)
 *
 * Returns the weight of the edge e in graph G (which must be weighted).
 */
gen _get_edge_weight(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || g._VECTptr->size()!=2)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_weighted())
        return gt_err(_GT_ERR_WEIGHTED_GRAPH_REQUIRED);
    gen &E=g._VECTptr->back();
    if (E.type!=_VECT || E._VECTptr->size()!=2)
        return gt_err(_GT_ERR_INVALID_EDGE);
    int i=G.node_index(E._VECTptr->front()),j=G.node_index(E._VECTptr->back());
    if (i==-1 || j==-1)
        return gt_err(_GT_ERR_EDGE_NOT_FOUND);
    return G.weight(i,j);
}
static const char _get_edge_weight_s[]="get_edge_weight";
static define_unary_function_eval(__get_edge_weight,&_get_edge_weight,_get_edge_weight_s);
define_unary_function_ptr5(at_get_edge_weight,alias_at_get_edge_weight,&__get_edge_weight,0,true)

/* USAGE:   set_edge_weight(G,e,w)
 *
 * Sets weight of the edge e in graph G (which must be weighted) to w and
 * returns the modified copy of G.
 */
gen _set_edge_weight(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT || g._VECTptr->size()!=3)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(g._VECTptr->front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_weighted())
        return gt_err(_GT_ERR_WEIGHTED_GRAPH_REQUIRED);
    gen &E=g._VECTptr->at(1);
    if (E.type!=_VECT || E._VECTptr->size()!=2)
        return gt_err(_GT_ERR_INVALID_EDGE);
    int i=G.node_index(E._VECTptr->front()),j=G.node_index(E._VECTptr->back());
    if (i==-1 || j==-1)
        return gt_err(_GT_ERR_EDGE_NOT_FOUND);
    G.set_edge_attribute(i,j,_GT_ATTRIB_WEIGHT,g._VECTptr->back());
    return G.to_gen();
}
static const char _set_edge_weight_s[]="set_edge_weight";
static define_unary_function_eval(__set_edge_weight,&_set_edge_weight,_set_edge_weight_s);
define_unary_function_ptr5(at_set_edge_weight,alias_at_set_edge_weight,&__set_edge_weight,0,true)

/* USAGE:   is_directed(G)
 *
 * Returns true iff G is a directed graph.
 */
gen _is_directed(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return graphe::boole(G.is_directed());
}
static const char _is_directed_s[]="is_directed";
static define_unary_function_eval(__is_directed,&_is_directed,_is_directed_s);
define_unary_function_ptr5(at_is_directed,alias_at_is_directed,&__is_directed,0,true)

/* USAGE:   neighbors(G,[v])
 *
 * Returns the list of vertices adjacent to v. If v is omitted, a list of
 * adjacency lists of all vertices in G is returned.
 */
gen _neighbors(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    if (g.subtype==_SEQ__VECT && g._VECTptr->size()!=2) {
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (g.subtype==_SEQ__VECT) {
        gen &v=g._VECTptr->back();
        int i=G.node_index(v);
        if (i==-1)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        graphe::ivector adj;
        G.adjacent_nodes(i,adj);
        return G.get_node_labels(adj);
    } else {
        vecteur res;
        int n=G.node_count();
        graphe::ivector adj;
        for (int i=0;i<n;++i) {
            G.adjacent_nodes(i,adj,false);
            res.push_back(_sort(G.get_node_labels(adj),contextptr));
        }
        return change_subtype(res,_LIST__VECT);
    }
}
static const char _neighbors_s[]="neighbors";
static define_unary_function_eval(__neighbors,&_neighbors,_neighbors_s);
define_unary_function_ptr5(at_neighbors,alias_at_neighbors,&__neighbors,0,true)

/* USAGE:   minimum_degree(G)
 *
 * Returns the smallest degree among vertices in an undirected graph G.
 */
gen _minimum_degree(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    return G.minimum_degree();
}
static const char _minimum_degree_s[]="minimum_degree";
static define_unary_function_eval(__minimum_degree,&_minimum_degree,_minimum_degree_s);
define_unary_function_ptr5(at_minimum_degree,alias_at_minimum_degree,&__minimum_degree,0,true)

/* USAGE:   maximum_degree(G)
 *
 * Returns the largest degree among vertices in an undirected graph G.
 */
gen _maximum_degree(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    return G.maximum_degree();
}
static const char _maximum_degree_s[]="maximum_degree";
static define_unary_function_eval(__maximum_degree,&_maximum_degree,_maximum_degree_s);
define_unary_function_ptr5(at_maximum_degree,alias_at_maximum_degree,&__maximum_degree,0,true)

/* USAGE:   is_regular(G,[d])
 *
 * Returns true iff max and min degrees of graph G are equal [and stores that
 * number in d].
 */
gen _is_regular(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    int d=-1,dd;
    gen arg(undef);
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        arg=g._VECTptr->at(1);
    }
    graphe G(contextptr,false);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (!is_undef(arg)) {
        if (arg.is_integer()) {
            if ((d=arg.val)<0)
                return generr("Expected a nonnegative integer");
        } else if (arg.type!=_IDNT)
            return gentypeerr(contextptr);
    }
    dd=G.is_regular(d);
    if (dd>=0 && !is_undef(arg) && arg.type==_IDNT)
        identifier_assign(*arg._IDNTptr,gen(dd),contextptr);
    return graphe::boole(dd>=0);
}
static const char _is_regular_s[]="is_regular";
static define_unary_function_eval(__is_regular,&_is_regular,_is_regular_s);
define_unary_function_ptr5(at_is_regular,alias_at_is_regular,&__is_regular,0,true)

/* USAGE:   is_strongly_regular(G,[srg])
 *
 * Returns true iff the graph G is strongly regular and optionally outputs
 * srg=[k,l,m] where k is the vertex degree and l resp. m is the number of
 * common neighbors for adjacent resp. non-adjacent vertices.
 */
gen _is_strongly_regular(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen srg=undef;
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if ((srg=g._VECTptr->back()).type!=_IDNT)
            return generr("Expected an identifier");
    }
    graphe G(contextptr,false);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    graphe::ipair sig;
    bool res=G.is_strongly_regular(sig);
    if (!res)
        return graphe::FAUX;
    if (!is_undef(srg))
        identifier_assign(*srg._IDNTptr,makevecteur(G.degree(0),sig.first,sig.second),contextptr);
    return graphe::VRAI;
}
static const char _is_strongly_regular_s[]="is_strongly_regular";
static define_unary_function_eval(__is_strongly_regular,&_is_strongly_regular,_is_strongly_regular_s);
define_unary_function_ptr5(at_is_strongly_regular,alias_at_is_strongly_regular,&__is_strongly_regular,0,true)

/* USAGE:   isomorphic_copy(G,[sigma])
 *
 * Returns a new graph H with neigbors data reordered according to a [random]
 * permutation sigma.
 */
gen _isomorphic_copy(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    vecteur sigma(0);
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()<2)
            return gensizeerr(contextptr);
        if (is_zero(_is_permu(gv[1],contextptr)))
            return generrtype("Expected a permutation");
        sigma=*gv[1]._VECTptr;
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (sigma.empty())
        sigma=*_randperm(G.node_count(),contextptr)._VECTptr;
    if (int(sigma.size())!=G.node_count())
        return generr("Invalid permutation size");
    graphe::ivector v(sigma.size());
    int ofs=array_start(contextptr);
    for (const_iterateur it=sigma.begin();it!=sigma.end();++it) {
        v[it-sigma.begin()]=it->val-ofs;
    }
    graphe H(contextptr);
    if (!G.isomorphic_copy(H,v,true))
        return generrtype("Failed to create isomorphic copy");
    H.relabel_nodes(G.vertices());
    return H.to_gen();
}
static const char _isomorphic_copy_s[]="isomorphic_copy";
static define_unary_function_eval(__isomorphic_copy,&_isomorphic_copy,_isomorphic_copy_s);
define_unary_function_ptr5(at_isomorphic_copy,alias_at_isomorphic_copy,&__isomorphic_copy,0,true)

/* USAGE:   permute_vertices(G,[V])
 *
 * Returns a copy of graph G with vertices reordered as given in V [or
 * randomly].
 */
gen _permute_vertices(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    vecteur sigma(0);
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()<2)
            return gensizeerr(contextptr);
        if (gv[1].type!=_VECT)
            return generrtype("Expected a list of vertices");
        sigma=*gv[1]._VECTptr;
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    vecteur V=G.vertices();
    if (sigma.empty())
        sigma=*_eval(symbolic(at_shuffle,V),contextptr)._VECTptr;
    if (sigma.size()!=V.size())
        return generr("Invalid permutation size");
    graphe::ivector v(sigma.size(),-1);
    const_iterateur jt;
    int i;
    for (const_iterateur it=sigma.begin();it!=sigma.end();++it) {
        if ((jt=find(V.begin(),V.end(),*it))==V.end())
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        i=jt-V.begin();
        if (find(v.begin(),v.end(),i)!=v.end())
            return generrtype("Expected a permutation");
        v[it-sigma.begin()]=i;
    }
    graphe H(contextptr);
    if (!G.isomorphic_copy(H,v))
        return gentypeerr(contextptr);
    return H.to_gen();
}
static const char _permute_vertices_s[]="permute_vertices";
static define_unary_function_eval(__permute_vertices,&_permute_vertices,_permute_vertices_s);
define_unary_function_ptr5(at_permute_vertices,alias_at_permute_vertices,&__permute_vertices,0,true)

/* USAGE:   relabel_vertices(G,V)
 *
 * Returns a new graph H with vertex labels changed to those in V.
 */
gen _relabel_vertices(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    if (gv[1].type!=_VECT)
        return generrtype("Expected a list of labels");
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    vecteur &labels=*gv[1]._VECTptr;
    if (int(labels.size())!=G.node_count())
        return generr("Invalid number of labels");
    if (!G.relabel_nodes(labels))
        return generrtype("Failed to relabel vertices");
    return G.to_gen();
}
static const char _relabel_vertices_s[]="relabel_vertices";
static define_unary_function_eval(__relabel_vertices,&_relabel_vertices,_relabel_vertices_s);
define_unary_function_ptr5(at_relabel_vertices,alias_at_relabel_vertices,&__relabel_vertices,0,true)

/* USAGE:   is_tree(G)
 *
 * Returns true iff the graph G is a tree, i.e. an undirected connected graph with
 * exactly n-1 edges, where n is the number of nodes of G.
 */
gen _is_tree(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return graphe::FAUX;
    return graphe::boole(G.is_tree());
}
static const char _is_tree_s[]="is_tree";
static define_unary_function_eval(__is_tree,&_is_tree,_is_tree_s);
define_unary_function_ptr5(at_is_tree,alias_at_is_tree,&__is_tree,0,true)

/* USAGE:   is_forest(G)
 *
 * Returns true iff the graph G is a forest, i.e. an undirected graph whose
 * connected components are all trees.
 */
gen _is_forest(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return graphe::FAUX;
    return graphe::boole(G.is_forest());
}
static const char _is_forest_s[]="is_forest";
static define_unary_function_eval(__is_forest,&_is_forest,_is_forest_s);
define_unary_function_ptr5(at_is_forest,alias_at_is_forest,&__is_forest,0,true)

/* USAGE:   is_tournament(G)
 *
 * Returns true iff the graph G is a tournament, i.e. a complete graph with a
 * direction for each edge.
 */
gen _is_tournament(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return graphe::boole(G.is_tournament());
}
static const char _is_tournament_s[]="is_tournament";
static define_unary_function_eval(__is_tournament,&_is_tournament,_is_tournament_s);
define_unary_function_ptr5(at_is_tournament,alias_at_is_tournament,&__is_tournament,0,true)

/* USAGE:   tree_height(T,r)
 *
 * Returns the height of the tree T with r as the root node.
 */
gen _tree_height(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.node_count()==1)
        return 0;
    if (!G.is_tree())
        return gt_err(_GT_ERR_NOT_A_TREE);
    int root;
    if ((root=G.node_index(gv.back()))==-1)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    return G.tree_height(root);
}
static const char _tree_height_s[]="tree_height";
static define_unary_function_eval(__tree_height,&_tree_height,_tree_height_s);
define_unary_function_ptr5(at_tree_height,alias_at_tree_height,&__tree_height,0,true)

/* USAGE:   number_of_triangles(G,[L])
 *
 * Returns the number of (directed) cycles of length 3 in a (di)graph G and
 * optionally lists the triangles in L.
 */
gen _number_of_triangles(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen dest(undef);
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        dest=g._VECTptr->back();
        if (dest.type!=_IDNT)
            return generr("Expected an unassigned identifier");
    }
    graphe G(contextptr,!is_undef(dest));
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!is_undef(dest) && G.is_directed())
        return generr("Undirected graph is required for triangle listing");
    graphe::ivectors lst;
    gen cnt=G.triangle_count(is_undef(dest)?NULL:&lst);
    if (!is_undef(dest)) {
        vecteur triangles;
        for (graphe::ivectors_iter it=lst.begin();it!=lst.end();++it) {
            triangles.push_back(_sort(G.get_node_labels(*it),contextptr));
            identifier_assign(*dest._IDNTptr,triangles,contextptr);
        }
    }
    return cnt;
}
static const char _number_of_triangles_s[]="number_of_triangles";
static define_unary_function_eval(__number_of_triangles,&_number_of_triangles,_number_of_triangles_s);
define_unary_function_ptr5(at_number_of_triangles,alias_at_number_of_triangles,&__number_of_triangles,0,true)

/* USAGE:   is_connected(G)
 *
 * Returns true iff graph G is connected.
 */
gen _is_connected(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return graphe::boole(G.is_connected());
}
static const char _is_connected_s[]="is_connected";
static define_unary_function_eval(__is_connected,&_is_connected,_is_connected_s);
define_unary_function_ptr5(at_is_connected,alias_at_is_connected,&__is_connected,0,true)

/* USAGE:   is_biconnected(G)
 *
 * Returns true iff graph G is biconnected. Running time is O(n+m).
 */
gen _is_biconnected(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return graphe::boole(G.is_biconnected());
}
static const char _is_biconnected_s[]="is_biconnected";
static define_unary_function_eval(__is_biconnected,&_is_biconnected,_is_biconnected_s);
define_unary_function_ptr5(at_is_biconnected,alias_at_is_biconnected,&__is_biconnected,0,true)

/* USAGE:   is_triconnected(G)
 *
 * Returns true iff graph G is triconnected. Running time is O(n*(n+m)).
 */
gen _is_triconnected(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return graphe::boole(G.is_triconnected());
}
static const char _is_triconnected_s[]="is_triconnected";
static define_unary_function_eval(__is_triconnected,&_is_triconnected,_is_triconnected_s);
define_unary_function_ptr5(at_is_triconnected,alias_at_is_triconnected,&__is_triconnected,0,true)

/* USAGE:   is_weighted(G)
 *
 * Returns true iff graph G is weighted.
 */
gen _is_weighted(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return graphe::boole(G.is_weighted());
}
static const char _is_weighted_s[]="is_weighted";
static define_unary_function_eval(__is_weighted,&_is_weighted,_is_weighted_s);
define_unary_function_ptr5(at_is_weighted,alias_at_is_weighted,&__is_weighted,0,true)

/* USAGE:   is_planar(G,[F])
 *
 * Returns true iff graph G is planar [and store the list of its faces to F if G
 * is biconnected].
 */
gen _is_planar(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen F=undef;
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if (g._VECTptr->back().type!=_IDNT)
            return generrtype("Expected an identifier");
        F=g._VECTptr->back();
    }
    graphe G(contextptr,!is_undef(F)),U(contextptr,!is_undef(F));
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    G.underlying(U);
    if (!is_undef(F)) {
        if (!U.is_biconnected())
            return gt_err(_GT_ERR_BICONNECTED_GRAPH_REQUIRED);
        graphe::ivectors faces;
        if (!U.demoucron(faces))
            return G.boole(false);
        vecteur res;
        for (graphe::ivectors_iter it=faces.begin();it!=faces.end();++it) {
            res.push_back(G.get_node_labels(*it));
        }
        identifier_assign(*F._IDNTptr,res,contextptr);
    }
    return G.boole(U.is_planar());
}
static const char _is_planar_s[]="is_planar";
static define_unary_function_eval(__is_planar,&_is_planar,_is_planar_s);
define_unary_function_ptr5(at_is_planar,alias_at_is_planar,&__is_planar,0,true)

/* USAGE:   complete_binary_tree(n)
 *
 * Returns the complete binary tree with depth equal to n.
 */
gen _complete_binary_tree(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (!g.is_integer() || g.val<0)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    G.make_complete_kary_tree(2,g.val);
    return G.to_gen();
}
static const char _complete_binary_tree_s[]="complete_binary_tree";
static define_unary_function_eval(__complete_binary_tree,&_complete_binary_tree,_complete_binary_tree_s);
define_unary_function_ptr5(at_complete_binary_tree,alias_at_complete_binary_tree,&__complete_binary_tree,0,true)

/* USAGE:   complete_kary_tree(k,n)
 *
 * Returns the complete k-ary tree with depth equal to n.
 */
gen _complete_kary_tree(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (!gv.front().is_integer() || !gv.back().is_integer())
        return generrtype("Expected an integer");
    int k=gv.front().val,n=gv.back().val;
    if (k<2 || n<1)
        return gensizeerr(contextptr);
    graphe G(contextptr);
    G.make_complete_kary_tree(k,n);
    return G.to_gen();
}
static const char _complete_kary_tree_s[]="complete_kary_tree";
static define_unary_function_eval(__complete_kary_tree,&_complete_kary_tree,_complete_kary_tree_s);
define_unary_function_ptr5(at_complete_kary_tree,alias_at_complete_kary_tree,&__complete_kary_tree,0,true)

/* USAGE:   prism_graph(n)
 *
 * Returns the prism graph of order n, i.e. the generalized Petersen graph
 * GP(n,1).
 */
gen _prism_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (!g.is_integer() || g.val<3)
        return generr("Expected an integer greater than two");
    return _petersen_graph(makesequence(g,1),contextptr);
}
static const char _prism_graph_s[]="prism_graph";
static define_unary_function_eval(__prism_graph,&_prism_graph,_prism_graph_s);
define_unary_function_ptr5(at_prism_graph,alias_at_prism_graph,&__prism_graph,0,true)

/* USAGE:   antiprism_graph(n)
 *
 * Returns the antiprism graph of order n.
 */
gen _antiprism_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (!g.is_integer() || g.val<3)
        return generr("Expected an integer greater than two");
    graphe G(contextptr);
    graphe::layout x;
    G.make_antiprism_graph(g.val,&x);
    G.store_layout(x);
    return G.to_gen();
}
static const char _antiprism_graph_s[]="antiprism_graph";
static define_unary_function_eval(__antiprism_graph,&_antiprism_graph,_antiprism_graph_s);
define_unary_function_ptr5(at_antiprism_graph,alias_at_antiprism_graph,&__antiprism_graph,0,true)

/* USAGE:   star_graph(n)
 *
 * Returns the star graph with n+1 vertices, i.e. the complete bipartite graph
 * complete_graph(1,n).
 */
gen _star_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (!g.is_integer() || g.val<1)
        return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    return _complete_graph(makesequence(1,g),contextptr);
}
static const char _star_graph_s[]="star_graph";
static define_unary_function_eval(__star_graph,&_star_graph,_star_graph_s);
define_unary_function_ptr5(at_star_graph,alias_at_star_graph,&__star_graph,0,true)

/* USAGE:   wheel_graph(n)
 *
 * Returns the wheel graph with n+1 vertices.
 */
gen _wheel_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (!g.is_integer() || g.val<3)
        return generr("Expected an integer greater than two");
    graphe G(contextptr);
    graphe::layout x;
    G.make_wheel_graph(g.val,&x);
    G.store_layout(x);
    return G.to_gen();
}
static const char _wheel_graph_s[]="wheel_graph";
static define_unary_function_eval(__wheel_graph,&_wheel_graph,_wheel_graph_s);
define_unary_function_ptr5(at_wheel_graph,alias_at_wheel_graph,&__wheel_graph,0,true)

/* USAGE:   grid_graph(m,n,[triangle])
 *
 * Returns a [triangular] grid graph on m*n vertices, where m,n>=2.
 */
gen _grid_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2 || gv.size()>3)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    bool trg=false;
    if (gv.size()==3) {
        if (gv[2]!=at_triangle)
            return generr("Unrecognized option");
        trg=true;
    }
    if (!gv[0].is_integer() || !gv[1].is_integer())
        return generrtype("Expected an integer");
    int m=gv[0].val,n=gv[1].val;
    if (m<2 || n<2)
        return generr("Expected an integer greater than one");
    graphe G(contextptr);
    if (trg) {
        /* create triangulated grid using strong product of digraphs */
        graphe X(contextptr),Y(contextptr);
        vecteur Xlab,Ylab;
        X.make_default_labels(Xlab,m);
        Y.make_default_labels(Ylab,n);
        X.add_nodes(Xlab);
        Y.add_nodes(Ylab);
        X.make_directed();
        Y.make_directed();
        for (int i=0;i<m-1;++i) X.add_edge(i,i+1);
        for (int i=0;i<n-1;++i) Y.add_edge(i,i+1);
        gen seq=makesequence(X.to_gen(),Y.to_gen());
        return _underlying_graph(_graph_union(makesequence(
                                                  _cartesian_product(seq,contextptr),
                                                  _tensor_product(seq,contextptr)),
                                              contextptr),
                                 contextptr);
    }
    G.make_grid_graph(m,n,false);
    return G.to_gen();
}
static const char _grid_graph_s[]="grid_graph";
static define_unary_function_eval(__grid_graph,&_grid_graph,_grid_graph_s);
define_unary_function_ptr5(at_grid_graph,alias_at_grid_graph,&__grid_graph,0,true)

/* USAGE:   torus_grid_graph(m,n)
 *
 * Returns the torus grid graph on m*n vertices, where m,n>=3.
 */
gen _torus_grid_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (!gv.front().is_integer() || !gv.back().is_integer())
        return generrtype("Expected an integer");
    int m=gv.front().val,n=gv.back().val;
    if (m<3 || n<3)
        return generr("Expected an integer greater than two");
    graphe G(contextptr);
    G.make_grid_graph(m,n,true);
    return G.to_gen();
}
static const char _torus_grid_graph_s[]="torus_grid_graph";
static define_unary_function_eval(__torus_grid_graph,&_torus_grid_graph,_torus_grid_graph_s);
define_unary_function_ptr5(at_torus_grid_graph,alias_at_torus_grid_graph,&__torus_grid_graph,0,true)

/* USAGE:   web_graph(a,b)
 *
 * Returns the web graph on a*b vertices, where a>=3 and b>=2.
 */
gen _web_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (!gv.front().is_integer() || !gv.back().is_integer())
        return generrtype("Expected an integer");
    int a=gv.front().val,b=gv.back().val;
    if (a<3 || b<2)
        return generr("Value too small");
    graphe G(contextptr);
    graphe::layout x;
    G.make_web_graph(a,b,&x);
    G.store_layout(x);
    return G.to_gen();
}
static const char _web_graph_s[]="web_graph";
static define_unary_function_eval(__web_graph,&_web_graph,_web_graph_s);
define_unary_function_ptr5(at_web_graph,alias_at_web_graph,&__web_graph,0,true)

/* USAGE:   cartesian_product(G1,G2,...)
 *
 * Returns Cartesian product of graphs G1, G2, ... Vertices in the resulting
 * graph are labelled as "u:v:..;" where u, v, ... are vertices from G1, G2,
 * ..., respectively.
 */
gen _cartesian_product(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    graphe P(contextptr);
    if (!compute_product_of_graphs(gv,P,true,contextptr))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return P.to_gen();
}
static const char _cartesian_product_s[]="cartesian_product";
static define_unary_function_eval(__cartesian_product,&_cartesian_product,_cartesian_product_s);
define_unary_function_ptr5(at_cartesian_product,alias_at_cartesian_product,&__cartesian_product,0,true)

/* USAGE:   tensor_product(G1,G2,...)
 *
 * Returns tensor product of graphs G1, G2, ... Vertices in the resulting graph
 * are labelled as "u:v:..." where u, v, ... are vertices from G1, G2, ...,
 * respectively.
 */
gen _tensor_product(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    graphe P(contextptr);
    if (!compute_product_of_graphs(gv,P,false,contextptr))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return P.to_gen();
}
static const char _tensor_product_s[]="tensor_product";
static define_unary_function_eval(__tensor_product,&_tensor_product,_tensor_product_s);
define_unary_function_ptr5(at_tensor_product,alias_at_tensor_product,&__tensor_product,0,true)

/* USAGE:   path_graph(n or V)
 *
 * Returns a path graph with n vertices, which can also be specified as list of
 * their labels.
 */
gen _path_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!vertices_from_integer_or_vecteur(g,G))
        return gt_err(_GT_ERR_BAD_VERTICES);
    int n=G.node_count();
    if (n<2)
        return generr("At least two vertices are required");
    for (int i=0;i<n-1;++i) {
        G.add_edge(i,i+1);
    }
    return G.to_gen();
}
static const char _path_graph_s[]="path_graph";
static define_unary_function_eval(__path_graph,&_path_graph,_path_graph_s);
define_unary_function_ptr5(at_path_graph,alias_at_path_graph,&__path_graph,0,true)

/* USAGE:   eulerian_trail(G,[V])
 *
 * Returns true iff graph G is eulerian, i.e. if it has eulerian trail. If
 * identifier V is specified as the second argument, that path is written to it.
 */
gen _is_eulerian(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    bool has_path_idnt=g.subtype==_SEQ__VECT;
    graphe G(contextptr);
    if (!G.read_gen(has_path_idnt?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int n=G.node_count();
    if (n==0) return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed()) {
        if (has_path_idnt) return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
        int start=-1,finish=-1,ic=0;
        graphe::ivector ind,outd;
        G.compute_in_out_degrees(ind,outd);
        for (int i=0;i<n;++i) {
            if (outd[i]==ind[i]+1) {
                if (start>=0) return graphe::FAUX;
                start=i;
            } else if (ind[i]==outd[i]+1) {
                if (finish>=0) return graphe::FAUX;
                finish=i;
            } else if (ind[i]!=outd[i]) return graphe::FAUX;
        }
        if ((start>=0 && finish<0) || (start<0 && finish>=0)) return graphe::FAUX;
        if (start>=0 && finish>=0) G.add_edge(finish,start);
        graphe::ivectors sc;
        G.strongly_connected_components(sc);
        for (graphe::ivectors_iter it=sc.begin();it!=sc.end();++it) {
            if (it->size()>1) ++ic;
        }
        if (ic!=1) return graphe::FAUX;
        if (start>=0 && finish>=0)
            *logptr(contextptr) << "Input digraph has an Eulerian trail starting at "
                                << G.node_label(start) << " and ending at " << G.node_label(finish) << "\n";
        else *logptr(contextptr) << "Input digraph has an Eulerian circuit\n";
        return graphe::VRAI;
    }
    graphe::ivector path;
    bool iscycle; // dummy
    if ((has_path_idnt && !G.find_eulerian_trail(path)) ||
            (!has_path_idnt && G.eulerian_trail_start(iscycle)<0))
        return graphe::FAUX;
    if (has_path_idnt) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        // output path as vecteur V
        gen V=g._VECTptr->back();
        if (V.type!=_IDNT)
            return generrtype("Expected an identifier");
        vecteur P(path.size());
        int i=0;
        for (graphe::ivector_iter it=path.begin();it!=path.end();++it) {
            P[i++]=G.node_label(*it);
        }
        identifier_assign(*V._IDNTptr,P,contextptr);
    }
    return graphe::VRAI;
}
static const char _is_eulerian_s[]="is_eulerian";
static define_unary_function_eval(__is_eulerian,&_is_eulerian,_is_eulerian_s);
define_unary_function_ptr5(at_is_eulerian,alias_at_is_eulerian,&__is_eulerian,0,true)

/* USAGE:   kneser_graph(n,k)
 *
 * Returns Kneser graph K(n,k) with comb(n,k) vertices. The largest acceptable
 * value of n is 20. Kneser graphs with more than 10000 vertices will not be
 * created.
 */
gen _kneser_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (!gv.front().is_integer() || !gv.back().is_integer())
        return generrtype("Expected an integer");
    int n=gv.front().val,k=gv.back().val;
    if (n<2 || n>20 || k<1 || k>=n)
        return generr("Failed to satisfy 2<n<=20 and 1<=k<n");
    graphe G(contextptr);
    if (!G.make_kneser_graph(n,k))
        return gensizeerr(contextptr);
    return G.to_gen();
}
static const char _kneser_graph_s[]="kneser_graph";
static define_unary_function_eval(__kneser_graph,&_kneser_graph,_kneser_graph_s);
define_unary_function_ptr5(at_kneser_graph,alias_at_kneser_graph,&__kneser_graph,0,true)

/* USAGE:   odd_graph(n)
 *
 * Returns odd graph of order n as Kneser graph K(2n-1,n-1). The largest
 * acceptable value of n is 8.
 */
gen _odd_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (!g.is_integer() || !is_strictly_positive(g,contextptr))
        return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    int n=g.val;
    if (n<2 || n>8)
        return generr("Failed to satisfy 2<n<=8");
    graphe G(contextptr);
    assert(G.make_kneser_graph(2*n-1,n-1));
    return G.to_gen();
}
static const char _odd_graph_s[]="odd_graph";
static define_unary_function_eval(__odd_graph,&_odd_graph,_odd_graph_s);
define_unary_function_ptr5(at_odd_graph,alias_at_odd_graph,&__odd_graph,0,true)

/* USAGE:   highlight_vertex(G,V,[C])
 *
 * Change color of vertex or list of vertices V in graph G to C (or green, if C
 * is not specified) and return the modified graph.
 */
gen _highlight_vertex(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    vecteur V;
    if (gv[1].type==_VECT)
        V=*gv[1]._VECTptr;
    else
        V.push_back(gv[1]);
    graphe::ivector indices;
    int index;
    for (const_iterateur it=V.begin();it!=V.end();++it) {
        index=G.node_index(*it);
        if (index<0)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        indices.push_back(index);
    }
    gen C=gv.size()==3?gv.back():graphe::default_highlighted_vertex_color;
    if (!parse_vertex_colors(G,C,indices))
        return generrtype("Failed to parse vertex colors");
    return G.to_gen();
}
static const char _highlight_vertex_s[]="highlight_vertex";
static define_unary_function_eval(__highlight_vertex,&_highlight_vertex,_highlight_vertex_s);
define_unary_function_ptr5(at_highlight_vertex,alias_at_highlight_vertex,&__highlight_vertex,0,true)

/* USAGE:   highlight_edges(G,E,[C])
 *
 * Change color of edge or list of edges E in graph V to C (or red, if C is not
 * specified) and return the modified graph.
 */
gen _highlight_edges(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (gv[1].type!=_VECT)
        return gt_err(_GT_ERR_INVALID_EDGE);
    vecteur &E=*gv[1]._VECTptr;
    graphe::ipairs edges;
    bool notfound=false;
    if (!G.edges2ipairs(E,edges,notfound))
        return notfound?gt_err(_GT_ERR_EDGE_NOT_FOUND):gensizeerr(contextptr);
    gen C=gv.size()==3?gv.back():graphe::default_highlighted_edge_color;
    for (graphe::ipairs_iter it=edges.begin();it!=edges.end();++it) {
        G.set_edge_attribute(it->first,it->second,_GT_ATTRIB_COLOR,C.type==_VECT?C._VECTptr->at(it-edges.begin()):C);
    }
    return G.to_gen();
}
static const char _highlight_edges_s[]="highlight_edges";
static define_unary_function_eval(__highlight_edges,&_highlight_edges,_highlight_edges_s);
define_unary_function_ptr5(at_highlight_edges,alias_at_highlight_edges,&__highlight_edges,0,true)

/* USAGE:   highlight_subgraph(G,S,[C1,C2])
 *
 * Change color of edges and vertices from S (which is a subgraph of G or a
 * list of subgraphs of G) to C1 and C2, respectively (by default, C1=red and
 * C2=green).
 */
gen _highlight_subgraph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    if (gv[1].type!=_VECT)
        return gentypeerr(contextptr);
    int C1=graphe::default_highlighted_edge_color,C2=graphe::default_highlighted_vertex_color;
    if (gv.size()>=4) {
        if (!gv[2].is_integer() || !gv[3].is_integer())
            return generrtype("Expected an integer");
        C1=gv[2].val;
        C2=gv[3].val;
    }
    bool overwrite_weights=false;
    if (gv.back().is_integer() && gv.back().val==_GT_WEIGHTS)
        overwrite_weights=true;
    gen modG;
    if (gv[1]._VECTptr->front().type==_VECT) {
        modG=gv.front();
        for (const_iterateur it=gv[1]._VECTptr->begin();it!=gv[1]._VECTptr->end();++it) {
            modG=_highlight_subgraph(makesequence(modG,*it,C1,C2),contextptr);
        }
        return modG;
    }
    graphe G(contextptr),S(contextptr);
    if (!G.read_gen(gv[0]) || !S.read_gen(gv[1]))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!S.is_subgraph(G))
        return gt_err(_GT_ERR_NOT_A_SUBGRAPH);
    vecteur V=S.vertices(),E=S.edges(false);
    if (overwrite_weights && G.is_weighted() && S.is_weighted()) {
        int i,j,k,l;if (g.type==_STRNG && g.subtype==-1) return g;
        for (const_iterateur it=E.begin();it!=E.end();++it) {
            i=G.node_index(it->_VECTptr->front());
            j=G.node_index(it->_VECTptr->back());
            k=S.node_index(it->_VECTptr->front());
            l=S.node_index(it->_VECTptr->back());
            G.set_edge_attribute(i,j,_GT_ATTRIB_WEIGHT,S.weight(k,l));
        }
    }
    modG=_highlight_edges(makesequence(G.to_gen(),E,C1),contextptr);
    return _highlight_vertex(makesequence(modG,V,C2),contextptr);
}
static const char _highlight_subgraph_s[]="highlight_subgraph";
static define_unary_function_eval(__highlight_subgraph,&_highlight_subgraph,_highlight_subgraph_s);
define_unary_function_ptr5(at_highlight_subgraph,alias_at_highlight_subgraph,&__highlight_subgraph,0,true)

/* USAGE:   highlight_trail(G,T,[C])
 *
 * Change color of edges in G which lie along the trail T to C (or red, if C is
 * not specified).
 */
gen _highlight_trail(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    if (gv.size()==3 && !gv.back().is_integer() && gv.back().type!=_VECT)
        return gentypeerr(contextptr);
    vecteur V;
    if (gv[1].type==_VECT)
        V=*gv[1]._VECTptr;
    else if (gv[1].is_symb_of_sommet(at_trail))
        V=*gv[1]._SYMBptr->feuille._VECTptr;
    if (V.empty())
        return gv.front();
    if (V.front().type!=_VECT)
        V=makevecteur(V);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    gen color=gv.size()==3?gv.back():gen(_RED);
    if (color.type==_VECT && !is_integer_vecteur(*color._VECTptr))
        return generrtype("Invalid specification of colors");
    if (color.type==_VECT && color._VECTptr->size()!=V.size())
        return generrdim("Numbers of colors and vertices do not match");
    int i,j;
    for (const_iterateur it=V.begin();it!=V.end();++it) {
        for (const_iterateur jt=it->_VECTptr->begin();jt!=it->_VECTptr->end()-1;++jt) {
            const gen &v=*jt,&w=*(jt+1);
            if (G.node_index(v)<0)
                return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
            if ((i=G.node_index(v))<0 || (j=G.node_index(w))<0)
                return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
            if (!G.has_edge(i,j))
                return gt_err(_GT_ERR_EDGE_NOT_FOUND);
            G.set_edge_attribute(i,j,_GT_ATTRIB_COLOR,(color.type==_VECT?color._VECTptr->at(it-V.begin()):color).val);
        }
    }
    return G.to_gen();
}
static const char _highlight_trail_s[]="highlight_trail";
static define_unary_function_eval(__highlight_trail,&_highlight_trail,_highlight_trail_s);
define_unary_function_ptr5(at_highlight_trail,alias_at_highlight_trail,&__highlight_trail,0,true)

/* USAGE:   disjoint_union(G1,G2,...)
 *
 * Returns the disjoint union of the input graphs. Vertices in the resulting
 * graph are labelled with "k:v", where k is index of the corresponding k-th
 * graph Gk and v is vertex in Gk.
 */
gen _disjoint_union(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    int err;
    if ((err=graphunion(G,*g._VECTptr,true))>=0)
        return gt_err(err);
    return G.to_gen();
}
static const char _disjoint_union_s[]="disjoint_union";
static define_unary_function_eval(__disjoint_union,&_disjoint_union,_disjoint_union_s);
define_unary_function_ptr5(at_disjoint_union,alias_at_disjoint_union,&__disjoint_union,0,true)

/* USAGE:   graph_union(G1,G2,...)
 *
 * Returns the union of the input graphs. Set of vertices of the resulting
 * graph is the union of the sets of vertices of the input graphs and the set
 * of edges of the resulting graph is the union of sets of edges of the input
 * graphs. If graphs G1 and G2 are both weighted, the weight of any common edge
 * is the sum of the weights of that edge in G1 and G2.
 */
gen _graph_union(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    graphe G(contextptr);
    int err;
    if ((err=graphunion(G,*g._VECTptr,false))>=0)
        return gt_err(err);
    return G.to_gen();
}
static const char _graph_union_s[]="graph_union";
static define_unary_function_eval(__graph_union,&_graph_union,_graph_union_s);
define_unary_function_ptr5(at_graph_union,alias_at_graph_union,&__graph_union,0,true)

/* USAGE:   graph_join(G,H)
 *
 * Returns the graph obtained by connecting every vertex from G with every
 * vertex from H. The vertex labels in the resulting graph are strings of form
 * "1:u" and "2:v" where u and v are vertices from G and H, respectively.
 */
gen _graph_join(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr),G1(contextptr),G2(contextptr);
    if (!G1.read_gen(gv.front()) || !G2.read_gen(gv.back()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G1.is_directed() || G2.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G1.is_weighted() || G2.is_weighted())
        return gt_err(_GT_ERR_UNWEIGHTED_GRAPH_REQUIRED);
    vecteur V=G1.vertices(),W=G2.vertices();
    stringstream ss;
    for (iterateur it=V.begin();it!=V.end();++it) {
        add_prefix_to_vertex_label(*it,1,ss);
    }
    for (iterateur it=W.begin();it!=W.end();++it) {
        add_prefix_to_vertex_label(*it,2,ss);
    }
    graphunion(G,gv,true);
    for (const_iterateur it=V.begin();it!=V.end();++it) {
        for (const_iterateur jt=W.begin();jt!=W.end();++jt) {
            G.add_edge(*it,*jt);
        }
    }
    return G.to_gen();
}
static const char _graph_join_s[]="graph_join";
static define_unary_function_eval(__graph_join,&_graph_join,_graph_join_s);
define_unary_function_ptr5(at_graph_join,alias_at_graph_join,&__graph_join,0,true)

/* USAGE:   graph_equal(G1,G2)
 *
 * Returns true iff G1 is equal to G2, that is when the sets of vertices and
 * edges of G1 and G2, as well as the orderings of vertices in both graphs,
 * mutually coincide. If the graphs are weighted (they must both be
 * (un)weighted and (un)directed), weights given to the same edge in two graphs
 * must be equal.
 */
gen _graph_equal(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G1(contextptr),G2(contextptr);
    if (!G1.read_gen(gv.front()) || !G2.read_gen(gv.back()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return graphe::boole(G1.is_equal(G2));
}
static const char _graph_equal_s[]="graph_equal";
static define_unary_function_eval(__graph_equal,&_graph_equal,_graph_equal_s);
define_unary_function_ptr5(at_graph_equal,alias_at_graph_equal,&__graph_equal,0,true)

/* USAGE:   reverse_graph(G)
 *
 * Returns reverse graph of G, i.e. the graph G with the directions of all edges
 * reversed.
 */
gen _reverse_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr),H(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return G.to_gen();
    G.reverse(H);
    return H.to_gen();
}
static const char _reverse_graph_s[]="reverse_graph";
static define_unary_function_eval(__reverse_graph,&_reverse_graph,_reverse_graph_s);
define_unary_function_ptr5(at_reverse_graph,alias_at_reverse_graph,&__reverse_graph,0,true)

/* USAGE:   interval_graph(a..b,c..d,...)
 *          interval_graph([a..b,c..d,...])
 *
 * Returns the interval graph with respect to intervals a..b, c..d, ... on the
 * real line. It has one vertex per interval and two vertices are connected iff
 * the corresponding intervals intersect.
 */
gen _interval_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    gen a,b;
    stringstream ss;
    int n=gv.size();
    vecteur V;
    V.reserve(n);
    for (const_iterateur it=gv.begin();it!=gv.end();++it) {
        if (!it->is_symb_of_sommet(at_interval))
            return generrtype("Expected an interval");
        a=it->_SYMBptr->feuille._VECTptr->front();
        b=it->_SYMBptr->feuille._VECTptr->back();
        if (!graphe::is_real_number(a) || !graphe::is_real_number(b))
            return generrtype("Expected an interval of reals");
        ss.str("");
        ss << a << " .. " << b;
        V.push_back(graphe::str2gen(ss.str(),true));
    }
    graphe G(contextptr);
    G.add_nodes(V);
    for (const_iterateur it=gv.begin();it!=gv.end();++it) {
        const gen &a1=it->_SYMBptr->feuille._VECTptr->front();
        const gen &b1=it->_SYMBptr->feuille._VECTptr->back();
        for (const_iterateur jt=it+1;jt!=gv.end();++jt) {
            const gen &a2=jt->_SYMBptr->feuille._VECTptr->front();
            const gen &b2=jt->_SYMBptr->feuille._VECTptr->back();
            if (is_greater(b2,a1,contextptr) && is_greater(b1,a2,contextptr))
                G.add_edge(it-gv.begin(),jt-gv.begin());
        }
    }
    return G.to_gen();
}
static const char _interval_graph_s[]="interval_graph";
static define_unary_function_eval(__interval_graph,&_interval_graph,_interval_graph_s);
define_unary_function_ptr5(at_interval_graph,alias_at_interval_graph,&__interval_graph,0,true)

/* USAGE:   subdivide_edges(G,E,[r])
 *
 * Inserts r (by default 1) new vertices to each edge/arc from G contained in
 * the list E (which may be a single edge/arc) and returns a modified copy of
 * G. New vertices are labeled with the smallest available integers.
 */
gen _subdivide_edges(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv[1].type!=_VECT)
        return gt_err(_GT_ERR_INVALID_EDGE);
    int r=1;
    if(gv.size()==3 && (!gv[2].is_integer() || (r=gv[2].val)<1))
        return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    vecteur &E=*gv[1]._VECTptr;
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int i,j,l=G.largest_integer_label();
    graphe::ipairs edges;
    if (ckmatrix(E)) {
        // a list of edges/arcs is given
        if (E.front()._VECTptr->size()!=2)
            return gensizeerr(contextptr);
        edges.resize(E.size());
        int k=0;
        for (const_iterateur it=E.begin();it!=E.end();++it) {
            const vecteur &e=*(it->_VECTptr);
            i=G.node_index(e.front());
            j=G.node_index(e.back());
            if (i<0 || j<0)
                return gt_err(i<0?e.front():e.back(),_GT_ERR_VERTEX_NOT_FOUND);
            if (!G.has_edge(i,j))
                return gt_err(e,_GT_ERR_EDGE_NOT_FOUND);
            edges[k++]=make_pair(i,j);
        }
    } else {
        // a single edge/arc is given
        if (E.size()!=2)
            return gt_err(_GT_ERR_INVALID_EDGE);
        edges.push_back(make_pair(G.node_index(E.front()),G.node_index(E.back())));
    }
    for (graphe::ipairs_iter it=edges.begin();it!=edges.end();++it) {
        G.subdivide_edge(*it,r,l);
    }
    return G.to_gen();
}
static const char _subdivide_edges_s[]="subdivide_edges";
static define_unary_function_eval(__subdivide_edges,&_subdivide_edges,_subdivide_edges_s);
define_unary_function_ptr5(at_subdivide_edges,alias_at_subdivide_edges,&__subdivide_edges,0,true)

/* USAGE:   graph_power(G,k)
 *
 * Returns the k-th power of graph G, where two vertices are connected iff there
 * exists a path of length at most k in the original graph.
 */
gen _graph_power(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    int k;
    if (!gv.back().is_integer() || (k=gv.back().val)<1)
        return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int n=G.node_count();
    matrice m,mpow;
    G.adjacency_matrix(m);
    mpow=m;
    for (int i=1;i<k;++i) {
        for (int j=0;j<n;++j) {
            mpow[j]._VECTptr->at(j)+=1;
        }
        mpow=mmult(mpow,m);
    }
    for (int i=0;i<n;++i) {
        for (int j=0;j<n;++j) {
            gen &mij=mpow[i]._VECTptr->at(j);
            if (!is_zero(mij))
                mij=1;
        }
    }
    gen opt=symbolic(at_equal,makesequence(_GT_DIRECTED,G.is_directed()));
    return _graph(makesequence(G.vertices(),mpow,opt),contextptr);
}
static const char _graph_power_s[]="graph_power";
static define_unary_function_eval(__graph_power,&_graph_power,_graph_power_s);
define_unary_function_ptr5(at_graph_power,alias_at_graph_power,&__graph_power,0,true)

/* USAGE:   vertex_distance(G,s,t)
 *          vertex_distance(G,s,T)
 *
 * Returns the number of edges in the shortest path from vertex s to vertex t
 * in graph G. If such path does not exist, returns +infinity. For vector T of
 * vertices from G returns the list of distances from s to each vertex t in T.
 */
gen _vertex_distance(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=3)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int i,j;
    if ((i=G.node_index(gv[1]))<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    vecteur T;
    bool single=false;
    if (gv[2].type==_VECT)
        T=*gv[2]._VECTptr;
    else {
        T.push_back(gv[2]);
        single=true;
    }
    graphe::ivector J(T.size()),dist;
    for (const_iterateur it=T.begin();it!=T.end();++it) {
        if ((j=G.node_index(*it))<0)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        J[it-T.begin()]=j;
    }
    G.distance(i,J,dist);
    vecteur res(T.size());
    for (graphe::ivector_iter it=dist.begin();it!=dist.end();++it) {
        res[it-dist.begin()]=*it>=0?gen(*it):graphe::plusinf();
    }
    return single?res.front():res;
}
static const char _vertex_distance_s[]="vertex_distance";
static define_unary_function_eval(__vertex_distance,&_vertex_distance,_vertex_distance_s);
define_unary_function_ptr5(at_vertex_distance,alias_at_vertex_distance,&__vertex_distance,0,true)

/* USAGE:   shortest_path(G,s,t)
 *          shortest_path(G,s,T)
 *
 * Returns the shortest path from vertex s to vertex t in graph G. If such path
 * does not exist, returns an empty list. If vector T of vertices from G is
 * given, the list of shortest paths from s to each t int T is returned.
 */
gen _shortest_path(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=3)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int i,j;
    if ((i=G.node_index(gv[1]))<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    vecteur T;
    bool single=false;
    if (gv[2].type==_VECT)
        T=*gv[2]._VECTptr;
    else {
        T.push_back(gv[2]);
        single=true;
    }
    graphe::ivector J(T.size()),dist;
    for (const_iterateur it=T.begin();it!=T.end();++it) {
        if ((j=G.node_index(*it))<0)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        J[it-T.begin()]=j;
    }
    graphe::ivectors shortest_paths;
    G.distance(i,J,dist,&shortest_paths);
    vecteur res(T.size());
    for (graphe::ivectors_iter it=shortest_paths.begin();it!=shortest_paths.end();++it) {
        i=it-shortest_paths.begin();
        res[it-shortest_paths.begin()]=dist[i]>=0?G.get_node_labels(*it):vecteur(0);
    }
    return single?res.front():change_subtype(res,_LIST__VECT);
}
static const char _shortest_path_s[]="shortest_path";
static define_unary_function_eval(__shortest_path,&_shortest_path,_shortest_path_s);
define_unary_function_ptr5(at_shortest_path,alias_at_shortest_path,&__shortest_path,0,true)

/* USAGE:   allpairs_distance(G)
 *
 * Returns a square matrix D of order n(=number of vertices in G) such that
 * D(i,j) is the distance between i-th and j-th vertex of (weighted) graph G,
 * computed by using Floyd-Warshall algorithm with complexity O(n^3). If For
 * some vertex pair no path exists, the corresponding entry in D is equal to
 * +infinity. Edges may have positive or negative weights but G shouldn't
 * contain negative cycles.
 */
gen _allpairs_distance(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    matrice dist;
    if (!G.is_null())
        G.allpairs_distance(dist);
    return dist;
}
static const char _allpairs_distance_s[]="allpairs_distance";
static define_unary_function_eval(__allpairs_distance,&_allpairs_distance,_allpairs_distance_s);
define_unary_function_ptr5(at_allpairs_distance,alias_at_allpairs_distance,&__allpairs_distance,0,true)

/* USAGE:   graph_diameter(G)
 *
 * Returns the diameter of graph G, i.e. the maximum distance between a pair of
 * vertices in G. If G is disconnected, its diameter is equal to +infinity.
 */
gen _graph_diameter(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (!G.is_connected())
        return graphe::plusinf();
    matrice D;
    G.allpairs_distance(D);
    int n=G.node_count();
    gen max_dist(symbolic(at_neg,_IDNT_infinity()));
    for (int i=0;i<n;++i) {
        for (int j=0;j<n;++j) {
            const gen &dist=D[i][j];
            if (is_inf(dist))
                continue;
            max_dist=_max(makesequence(max_dist,dist),contextptr);
        }
    }
    return max_dist;
}
static const char _graph_diameter_s[]="graph_diameter";
static define_unary_function_eval(__graph_diameter,&_graph_diameter,_graph_diameter_s);
define_unary_function_ptr5(at_graph_diameter,alias_at_graph_diameter,&__graph_diameter,0,true)

/* USAGE:   dijkstra(G,v,w)
 *          dijkstra(G,v,W)
 *          dijkstra(G,v)
 *
 * Returns the cheapest weighted path from vertex v to w in graph G. Output is
 * in form [[v1,v2,...,vk],d] where v1,v2,...,vk are vertices along the path
 * and d is the weight of the path. If no such path exists, returns
 * [[],+infinity]. Also, when list W of vertices is specified, a sequence of
 * cheapest paths to vertices from W is returned. If W is omitted, it is
 * assumed that W=vertices(G). The Dijkstra's algorithm is used (nonnegative
 * weights).
 */
gen _dijkstra(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_weighted())
        return gt_err(_GT_ERR_WEIGHTED_GRAPH_REQUIRED);
    int v,i;
    if ((v=G.node_index(gv[1]))<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    int n=G.node_count();
    vecteur V,path_weights,paths;
    graphe::ivector dest;
    if (gv.size()==2) {
        V=G.vertices();
        dest.resize(n);
        for (i=0;i<n;++i) {
            dest[i]=i;
        }
    } else {
        if (gv[2].type==_VECT)
            V=*gv[2]._VECTptr;
        else V.push_back(gv[2]);
        dest.resize(V.size());
        i=0;
        for (const_iterateur it=V.begin();it!=V.end();++it) {
            if ((dest[i++]=G.node_index(*it))<0)
                return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        }
    }
    graphe::ivectors cheapest_paths;
    G.dijkstra(v,dest,path_weights,&cheapest_paths);
    paths.resize(dest.size());
    i=0;
    for (graphe::ivectors_iter it=cheapest_paths.begin();it!=cheapest_paths.end();++it) {
        vecteur &path=*(paths[i++]=vecteur(it->size()))._VECTptr;
        for (graphe::ivector_iter jt=it->begin();jt!=it->end();++jt) {
            path[jt-it->begin()]=G.node_label(*jt);
        }
    }
    if (gv.size()>2 && gv[2].type!=_VECT) {
        gen &w=path_weights.front();
        return makevecteur(is_inf(w)?vecteur(0):paths.front(),w);
    }
    vecteur res(dest.size());
    i=0;
    for (const_iterateur it=paths.begin();it!=paths.end();++it) {
        vecteur &path=*(it->_VECTptr);
        gen &w=path_weights[i];
        res[i++]=makevecteur(is_inf(w)?vecteur(0):path,w);
    }
    return change_subtype(res,_SEQ__VECT);
}
static const char _dijkstra_s[]="dijkstra";
static define_unary_function_eval(__dijkstra,&_dijkstra,_dijkstra_s);
define_unary_function_ptr5(at_dijkstra,alias_at_dijkstra,&__dijkstra,0,true)

/* USAGE:   bellman_ford(G,v,w)
 *          bellman_ford(G,v,W)
 *          bellman_ford(G,v)
 *
 * Returns the cheapest weighted path from vertex v to w in graph G. Output is
 * in form [[v1,v2,...,vk],d] where v1,v2,...,vk are vertices along the path
 * and d is the weight of the path. If no such path exists, returns
 * [[],+infinity]. Also, when list W of vertices is specified, a sequence of
 * cheapest paths to vertices from W is returned. If W is omitted, it is
 * assumed that W=vertices(G). The Bellman-Ford algorithm is used, which
 * accepts negative weights but fails on graphs containing negative cycles.
 */
gen _bellman_ford(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2)
        return gensizeerr(contextptr);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_weighted())
        return gt_err(_GT_ERR_WEIGHTED_GRAPH_REQUIRED);
    int v,i;
    if ((v=G.node_index(gv[1]))<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    int n=G.node_count();
    vecteur V,path_weights,paths;
    graphe::ivector dest;
    if (gv.size()==2) {
        V=G.vertices();
        dest.resize(n);
        for (i=0;i<n;++i) {
            dest[i]=i;
        }
    } else {
        if (gv[2].type==_VECT)
            V=*gv[2]._VECTptr;
        else V.push_back(gv[2]);
        dest.resize(V.size());
        i=0;
        for (const_iterateur it=V.begin();it!=V.end();++it) {
            if ((dest[i++]=G.node_index(*it))<0)
                return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        }
    }
    graphe::ivectors cheapest_paths;
    if (!G.bellman_ford(v,dest,path_weights,&cheapest_paths)) {
        *logptr(contextptr) << "Error: graph contains a negative-weight cycle\n";
        return vecteur(0);
    }
    paths.resize(dest.size());
    i=0;
    for (graphe::ivectors_iter it=cheapest_paths.begin();it!=cheapest_paths.end();++it) {
        vecteur &path=*(paths[i++]=vecteur(it->size()))._VECTptr;
        for (graphe::ivector_iter jt=it->begin();jt!=it->end();++jt) {
            path[jt-it->begin()]=G.node_label(*jt);
        }
    }
    if (gv.size()>2 && gv[2].type!=_VECT) {
        gen &w=path_weights.front();
        return makevecteur(is_inf(w)?vecteur(0):paths.front(),w);
    }
    vecteur res(dest.size());
    i=0;
    for (const_iterateur it=paths.begin();it!=paths.end();++it) {
        vecteur &path=*(it->_VECTptr);
        gen &w=path_weights[i];
        res[i++]=makevecteur(is_inf(w)?vecteur(0):path,w);
    }
    return change_subtype(res,_SEQ__VECT);
}
static const char _bellman_ford_s[]="bellman_ford";
static define_unary_function_eval(__bellman_ford,&_bellman_ford,_bellman_ford_s);
define_unary_function_ptr5(at_bellman_ford,alias_at_bellman_ford,&__bellman_ford,0,true)

/* USAGE:   topologic_sort(G)
 *
 * Returns the list of vertices sorted according to the topological ordering in
 * a directed acyclic graph G.
 */
gen _topologic_sort(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    graphe::ivector ordering;
    if (!G.topologic_sort(ordering))
        return gt_err(_GT_ERR_NOT_ACYCLIC_GRAPH);
    vecteur res(ordering.size());
    for (graphe::ivector_iter it=ordering.begin();it!=ordering.end();++it) {
        res[it-ordering.begin()]=G.node_label(*it);
    }
    return res;
}
static const char _topologic_sort_s[]="topologic_sort";
static define_unary_function_eval(__topologic_sort,&_topologic_sort,_topologic_sort_s);
define_unary_function_ptr5(at_topologic_sort,alias_at_topologic_sort,&__topologic_sort,0,true)

static const char _topological_sort_s[]="topological_sort";
static define_unary_function_eval(__topological_sort,&_topologic_sort,_topological_sort_s);
define_unary_function_ptr5(at_topological_sort,alias_at_topological_sort,&__topological_sort,0,true)

/* USAGE:   is_acyclic(G)
 *
 * Returns true iff the directed graph G is acyclic, i.e. has no topological
 * ordering.
 */
gen _is_acyclic(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    graphe::ivector ordering;
    return graphe::boole(G.topologic_sort(ordering));
}
static const char _is_acyclic_s[]="is_acyclic";
static define_unary_function_eval(__is_acyclic,&_is_acyclic,_is_acyclic_s);
define_unary_function_ptr5(at_is_acyclic,alias_at_is_acyclic,&__is_acyclic,0,true)

/* USAGE:   is_clique(G)
 *
 * Returns true iff graph G is a clique (i.e. a complete graph).
 */
gen _is_clique(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    return graphe::boole(G.is_clique());
}
static const char _is_clique_s[]="is_clique";
static define_unary_function_eval(__is_clique,&_is_clique,_is_clique_s);
define_unary_function_ptr5(at_is_clique,alias_at_is_clique,&__is_clique,0,true)

/* USAGE:   maximum_clique(G)
 *
 * Returns maximum clique of undirected graph G as a list of vertices.
 */
gen _maximum_clique(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    graphe::ivector clique;
    G.maximum_clique(clique);
    vecteur res=G.get_node_labels(clique);
    return _sort(res,contextptr);
}
static const char _maximum_clique_s[]="maximum_clique";
static define_unary_function_eval(__maximum_clique,&_maximum_clique,_maximum_clique_s);
define_unary_function_ptr5(at_maximum_clique,alias_at_maximum_clique,&__maximum_clique,0,true)

/* USAGE:   clique_number(G)
 *
 * Returns the clique number of graph G, which is equal to the size of maximum
 * clique.
 */
gen _clique_number(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    graphe::ivector clique;
    return G.maximum_clique(clique);
}
static const char _clique_number_s[]="clique_number";
static define_unary_function_eval(__clique_number,&_clique_number,_clique_number_s);
define_unary_function_ptr5(at_clique_number,alias_at_clique_number,&__clique_number,0,true)

/* USAGE:   clique_cover(G,[k])
 *
 * Returns a clique vertex cover of graph G [containing at most k cliques].
 */
gen _clique_cover(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    int k=0;
    if (g.type==_VECT && g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if (!g._VECTptr->back().is_integer() || (k=g._VECTptr->back().val)<1)
            return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return vecteur(0);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    graphe::ivectors cover;
    if (!G.clique_cover(cover,k))
        return vecteur(0);
    vecteur res;
    G.ivectors2vecteur(cover,res,true);
    return change_subtype(res,_LIST__VECT);
}
static const char _clique_cover_s[]="clique_cover";
static define_unary_function_eval(__clique_cover,&_clique_cover,_clique_cover_s);
define_unary_function_ptr5(at_clique_cover,alias_at_clique_cover,&__clique_cover,0,true)

/* USAGE:   clique_cover_number(G)
 *
 * Returns the clique cover number of graph G (i.e. the chromatic number of the
 * graph complement of G).
 */
gen _clique_cover_number(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return 0;
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    graphe::ivectors cover;
    G.clique_cover(cover);
    int ncov=cover.size();
    if (ncov==0)
        return undef;
    return ncov;
}
static const char _clique_cover_number_s[]="clique_cover_number";
static define_unary_function_eval(__clique_cover_number,&_clique_cover_number,_clique_cover_number_s);
define_unary_function_ptr5(at_clique_cover_number,alias_at_clique_cover_number,&__clique_cover_number,0,true)

/* USAGE:   chromatic_number(G,[interval or approx || cols])
 *
 * Returns the chromatic number of the input graph G. If "interval" or "approx" parameter
 * is given, the bounds are returned as an interval. If an identifier cols is
 * given, the coloring will be stored to it.
 */
gen _chromatic_number(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen colors_dest=undef;
    bool only_provide_bounds=false;
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()!=2 || gv.size()>3)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        gen &opt=g._VECTptr->back();
        if (opt==at_interval || opt==at_approx)
            only_provide_bounds=true;
        else if (opt.type==_IDNT)
            colors_dest=opt;
        else return gentypeerr(contextptr);
    }
    graphe G(contextptr,false);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (only_provide_bounds) {
        graphe::ipair bounds=G.chromatic_number_bounds();
        return symbolic(at_interval,makesequence(bounds.first,bounds.second));
    }
    int ncolors=G.exact_vertex_coloring();
    if (ncolors==0)
        return undef;
    if (!is_undef(colors_dest)) { // store the coloring
        graphe::ivector colors;
        G.get_node_colors(colors);
        identifier_assign(*colors_dest._IDNTptr,vector_int_2_vecteur(colors),contextptr);
    }
    return ncolors;
}
static const char _chromatic_number_s[]="chromatic_number";
static define_unary_function_eval(__chromatic_number,&_chromatic_number,_chromatic_number_s);
define_unary_function_ptr5(at_chromatic_number,alias_at_chromatic_number,&__chromatic_number,0,true)

/* USAGE:   maximum_independent_set(G)
 *
 * Returns the maximum independent vertex set of graph G (i.e. maximum clique
 * of the graph complement of G).
 */
gen _maximum_independent_set(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr),C(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    G.complement(C);
    graphe::ivector clique;
    C.maximum_clique(clique);
    vecteur res=C.get_node_labels(clique);
    return _sort(res,contextptr);
}
static const char _maximum_independent_set_s[]="maximum_independent_set";
static define_unary_function_eval(__maximum_independent_set,&_maximum_independent_set,_maximum_independent_set_s);
define_unary_function_ptr5(at_maximum_independent_set,alias_at_maximum_independent_set,&__maximum_independent_set,0,true)

/* USAGE:   independence_number(G)
 *
 * Returns the independence number of graph G (i.e. the size of maximum
 * independent set).
 */
gen _independence_number(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false),C(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    G.complement(C);
    graphe::ivector clique;
    return C.maximum_clique(clique);
}
static const char _independence_number_s[]="independence_number";
static define_unary_function_eval(__independence_number,&_independence_number,_independence_number_s);
define_unary_function_ptr5(at_independence_number,alias_at_independence_number,&__independence_number,0,true)

/* USAGE:   strongly_connected_components(G)
 *
 * Returns the list of strongly connected components of directed graph G.
 */
gen _strongly_connected_components(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    graphe::ivectors comp;
    G.strongly_connected_components(comp);
    vecteur res(comp.size());
    G.ivectors2vecteur(comp,res,true);
    return change_subtype(res,_LIST__VECT);
}
static const char _strongly_connected_components_s[]="strongly_connected_components";
static define_unary_function_eval(__strongly_connected_components,&_strongly_connected_components,_strongly_connected_components_s);
define_unary_function_ptr5(at_strongly_connected_components,alias_at_strongly_connected_components,&__strongly_connected_components,0,true)

/* USAGE:   condensation(G)
 *
 * Returns the graph obtained by contracting strongly connected components of
 * the digraph G to single vertices.
 */
gen _condensation(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr),H(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    G.condensation(H);
    return H.to_gen();
}
static const char _condensation_s[]="condensation";
static define_unary_function_eval(__condensation,&_condensation,_condensation_s);
define_unary_function_ptr5(at_condensation,alias_at_condensation,&__condensation,0,true)

/* USAGE:   is_strongly_connected(G)
 *
 * Returns true iff the directed graph G is strongly connected.
 */
gen _is_strongly_connected(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    graphe::ivectors components;
    G.strongly_connected_components(components);
    return graphe::boole(components.size()==1);
}
static const char _is_strongly_connected_s[]="is_strongly_connected";
static define_unary_function_eval(__is_strongly_connected,&_is_strongly_connected,_is_strongly_connected_s);
define_unary_function_ptr5(at_is_strongly_connected,alias_at_is_strongly_connected,&__is_strongly_connected,0,true)

/* USAGE:   degree_sequence(G)
 *
 * Returns the list of degrees of vertices of graph G (arc directions are
 * ignored).
 */
gen _degree_sequence(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return G.degree_sequence();
}
static const char _degree_sequence_s[]="degree_sequence";
static define_unary_function_eval(__degree_sequence,&_degree_sequence,_degree_sequence_s);
define_unary_function_ptr5(at_degree_sequence,alias_at_degree_sequence,&__degree_sequence,0,true)

/* USAGE:   is_graphic_sequence(L)
 *
 * Returns true iff there exists a graph with degree sequence equal to the list
 * L. The algorithm is based on Erdos-Gallai theorem.
 */
gen _is_graphic_sequence(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    int n=g._VECTptr->size();
    for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
        if (!it->is_integer() || !is_positive(*it,contextptr) || !is_strictly_greater(n,*it,contextptr))
            return graphe::FAUX;
    }
    return graphe::boole(graphe::is_graphic_sequence(vecteur_2_vector_int(*g._VECTptr)));
}
static const char _is_graphic_sequence_s[]="is_graphic_sequence";
static define_unary_function_eval(__is_graphic_sequence,&_is_graphic_sequence,_is_graphic_sequence_s);
define_unary_function_ptr5(at_is_graphic_sequence,alias_at_is_graphic_sequence,&__is_graphic_sequence,0,true)

/* USAGE:   sequence_graph(L)
 *
 * Returns an undirected graph with the degree sequence equal to the list L, as
 * constructed by Havel-Hakimi algorithm.
 */
gen _sequence_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    int n=g._VECTptr->size();
    graphe::ivector deg(n);
    for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
        if (!it->is_integer() || !is_positive(*it,contextptr) || !is_strictly_greater(n,*it,contextptr))
            return gt_err(_GT_ERR_NOT_A_GRAPHIC_SEQUENCE);
        deg[it-g._VECTptr->begin()]=it->val;
    }
    graphe G(contextptr);
    vecteur V;
    G.make_default_labels(V,deg.size());
    G.reserve_nodes(deg.size());
    G.add_nodes(V);
    if (!G.hakimi(deg))
        return gt_err(_GT_ERR_NOT_A_GRAPHIC_SEQUENCE);
    return G.to_gen();
}
static const char _sequence_graph_s[]="sequence_graph";
static define_unary_function_eval(__sequence_graph,&_sequence_graph,_sequence_graph_s);
define_unary_function_ptr5(at_sequence_graph,alias_at_sequence_graph,&__sequence_graph,0,true)

/* USAGE:   girth(G)
 *
 * Returns the girth of undirected and unweighted graph G (i.e. the length of
 * the shortest cycle in G).
 */
gen _girth(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G.is_weighted())
        return gt_err(_GT_ERR_UNWEIGHTED_GRAPH_REQUIRED);
    int grth=G.girth();
    return grth<0?graphe::plusinf():gen(grth);
}
static const char _girth_s[]="girth";
static define_unary_function_eval(__girth,&_girth,_girth_s);
define_unary_function_ptr5(at_girth,alias_at_girth,&__girth,0,true)

/* USAGE:   odd_girth(G)
 *
 * Returns the length of the shortest odd cycle in undirected and unweighted
 * graph G.
 */
gen _odd_girth(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G.is_weighted())
        return gt_err(_GT_ERR_UNWEIGHTED_GRAPH_REQUIRED);
    int grth=G.girth(true);
    return grth<0?graphe::plusinf():gen(grth);
}
static const char _odd_girth_s[]="odd_girth";
static define_unary_function_eval(__odd_girth,&_odd_girth,_odd_girth_s);
define_unary_function_ptr5(at_odd_girth,alias_at_odd_girth,&__odd_girth,0,true)

/* USAGE:   is_arborescence(G)
 *
 * Returns true iff directed and unweighted graph G is an arborescence.
 */
gen _is_arborescence(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    return graphe::boole(G.is_arborescence());
}
static const char _is_arborescence_s[]="is_arborescence";
static define_unary_function_eval(__is_arborescence,&_is_arborescence,_is_arborescence_s);
define_unary_function_ptr5(at_is_arborescence,alias_at_is_arborescence,&__is_arborescence,0,true)

/* USAGE:   graph_spectrum(G)
 *
 * Returns the graph spectrum of G. The return value is a list of lists with two
 * elements, each containing an eigenvalue and its multiplicity.
 */
gen _graph_spectrum(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    matrice A,res;
    G.adjacency_matrix(A);
    vecteur ev=*_eigenvals(A,contextptr)._VECTptr;
    gen_map ev_map;
    for (const_iterateur it=ev.begin();it!=ev.end();++it) {
        ev_map[*it]+=1;
    }
    for (gen_map::const_iterator it=ev_map.begin();it!=ev_map.end();++it) {
        res.push_back(makevecteur(it->first,it->second));
    }
    return res;
}
static const char _graph_spectrum_s[]="graph_spectrum";
static define_unary_function_eval(__graph_spectrum,&_graph_spectrum,_graph_spectrum_s);
define_unary_function_ptr5(at_graph_spectrum,alias_at_graph_spectrum,&__graph_spectrum,0,true)

/* USAGE:   seidel_spectrum(G)
 *
 * Returns the Seidel spectrum of G. The return value is a list of lists with two
 * elements, each containing an eigenvalue and its multiplicity.
 */
gen _seidel_spectrum(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int n=G.node_count();
    matrice A,I,J,res;
    G.adjacency_matrix(A);
    I=*_idn(n,contextptr)._VECTptr;
    J=*_matrix(makesequence(n,n,1),contextptr)._VECTptr;
    vecteur ev=*_eigenvals(J-I-A-A,contextptr)._VECTptr;
    gen_map ev_map;
    for (const_iterateur it=ev.begin();it!=ev.end();++it) {
        ev_map[*it]+=1;
    }
    for (gen_map::const_iterator it=ev_map.begin();it!=ev_map.end();++it) {
        res.push_back(makevecteur(it->first,it->second));
    }
    return res;
}
static const char _seidel_spectrum_s[]="seidel_spectrum";
static define_unary_function_eval(__seidel_spectrum,&_seidel_spectrum,_seidel_spectrum_s);
define_unary_function_ptr5(at_seidel_spectrum,alias_at_seidel_spectrum,&__seidel_spectrum,0,true)

/* USAGE:   graph_charpoly(G,[x])
 *
 * Returns the value p(x) of the characteristic polynomial p of an undirected
 * graph G. If x is omitted, a list of coefficients of p is returned.
 */
gen _graph_charpoly(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    identificateur x(" x");
    gen val(undef);
    graphe G(contextptr,false);
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        val=g._VECTptr->back();
    }
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    matrice A;
    G.adjacency_matrix(A);
    if (is_undef(val))
        return _eval(symbolic(at_charpoly,A),contextptr);
    gen p=_eval(symbolic(at_charpoly,makesequence(A,x)),contextptr);
    return _subs(makesequence(p,x,val),contextptr);
}
static const char _graph_charpoly_s[]="graph_charpoly";
static define_unary_function_eval(__graph_charpoly,&_graph_charpoly,_graph_charpoly_s);
define_unary_function_ptr5(at_graph_charpoly,alias_at_graph_charpoly,&__graph_charpoly,0,true)

/* USAGE:   is_integer_graph(G)
 *
 * Returns true iff the spectrum of graph G consists only of integers.
 */
gen _is_integer_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    matrice A;
    G.adjacency_matrix(A);
    return graphe::boole(is_integer_vecteur(*_eigenvals(A,contextptr)._VECTptr,true));
}
static const char _is_integer_graph_s[]="is_integer_graph";
static define_unary_function_eval(__is_integer_graph,&_is_integer_graph,_is_integer_graph_s);
define_unary_function_ptr5(at_is_integer_graph,alias_at_is_integer_graph,&__is_integer_graph,0,true)

/* USAGE:   spanning_tree(G,[r])
 *
 * Returns a spanning tree of the undirected graph G [with the vertex r as the
 * root node].
 */
gen _spanning_tree(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen root(undef);
    graphe G(contextptr),T(contextptr);
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        root=g._VECTptr->back();
    }
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    int i=is_undef(root)?0:G.node_index(root);
    if (i<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    G.spanning_tree(i,T);
    return T.to_gen();
}
static const char _spanning_tree_s[]="spanning_tree";
static define_unary_function_eval(__spanning_tree,&_spanning_tree,_spanning_tree_s);
define_unary_function_ptr5(at_spanning_tree,alias_at_spanning_tree,&__spanning_tree,0,true)

/* USAGE:   number_of_spanning_trees(G)
 *
 * Returns the number of spanning trees in the undirected graph G if it is
 * connected, else return the number of spanning forests.
 */
gen _number_of_spanning_trees(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (!G.is_connected()) {
        gen res(1);
        graphe C(contextptr,false);
        graphe::ivectors comp;
        G.connected_components(comp);
        for (graphe::ivectors_iter it=comp.begin();it!=comp.end();++it) {
            G.induce_subgraph(*it,C);
            res=res*count_spanning_trees(C);
        }
        return res;
    }
    return count_spanning_trees(G);
}
static const char _number_of_spanning_trees_s[]="number_of_spanning_trees";
static define_unary_function_eval(__number_of_spanning_trees,&_number_of_spanning_trees,_number_of_spanning_trees_s);
define_unary_function_ptr5(at_number_of_spanning_trees,alias_at_number_of_spanning_trees,&__number_of_spanning_trees,0,true)

/* USAGE:   minimal_spanning_tree(G)
 *
 * Returns the minimal spanning tree of the undirected graph G.
 */
gen _minimal_spanning_tree(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr),T(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (!G.is_weighted())
        G.spanning_tree(0,T);
    else
        G.minimal_spanning_tree(T);
    return T.to_gen();
}
static const char _minimal_spanning_tree_s[]="minimal_spanning_tree";
static define_unary_function_eval(__minimal_spanning_tree,&_minimal_spanning_tree,_minimal_spanning_tree_s);
define_unary_function_ptr5(at_minimal_spanning_tree,alias_at_minimal_spanning_tree,&__minimal_spanning_tree,0,true)

/* USAGE:   graph_rank(G,[E])
 *
 * Returns the graph rank of G. If optional set E of edges is given, rank of the
 * spanning subgraph of G with edge set E is returned.
 */
gen _graph_rank(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    vecteur E;
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if (g._VECTptr->back().type!=_VECT)
            return gentypeerr(contextptr);
        E=*g._VECTptr->back()._VECTptr;
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (E.empty())
        return G.node_count()-G.connected_component_count();
    else {
        graphe::ipairs ev;
        bool notfound=false;
        if (!G.edges2ipairs(E,ev,notfound))
            return notfound?gt_err(_GT_ERR_EDGE_NOT_FOUND):gentypeerr(contextptr);
        G.set_subgraph(ev,1);
        return G.subgraph_size(1)-G.connected_component_count(1);
    }
}
static const char _graph_rank_s[]="graph_rank";
static define_unary_function_eval(__graph_rank,&_graph_rank,_graph_rank_s);
define_unary_function_ptr5(at_graph_rank,alias_at_graph_rank,&__graph_rank,0,true)

/* USAGE:   lowest_common_ancestor(T,r,u,v)
 *          lowest_common_ancestor(T,r,L)
 *
 * Returns the lowest common ancestor of the nodes u,v in the tree T with
 * respect to the root node r. If a list L of node pairs is given, common
 * ancestors of all pairs are returned in a list. Tarjan's offline algorithm is
 * used.
 */
gen _lowest_common_ancestor(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<3)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_tree())
        return gt_err(_GT_ERR_NOT_A_TREE);
    int r=G.node_index(gv[1]);
    if (r<0)
        return gt_err(gv[1],_GT_ERR_VERTEX_NOT_FOUND);
    if (gv.size()==4) {
        int i=G.node_index(gv[2]),j=G.node_index(gv[3]);
        if (i<0 || j<0)
            return gt_err(i<0?gv[1]:gv[2],_GT_ERR_VERTEX_NOT_FOUND);
        return G.node_label(G.lowest_common_ancestor(i,j,r));
    } else if (gv.size()==3) {
        if (!ckmatrix(gv.back()))
            return gentypeerr(contextptr);
        if (gv.back()._VECTptr->front()._VECTptr->size()!=2)
            return gendimerr(contextptr);
        graphe::ipairs p;
        matrice &m=*gv.back()._VECTptr;
        int n=m.size(),i,j;
        for (int k=0;k<n;++k) {
            vecteur &row=*m[k]._VECTptr;
            i=G.node_index(row.front());
            j=G.node_index(row.back());
            if (i<0 || j<0)
                return gt_err(i<0?row.front():row.back(),_GT_ERR_VERTEX_NOT_FOUND);
            p.push_back(make_pair(i,j));
        }
        graphe::ivector lca;
        G.lowest_common_ancestors(r,p,lca);
        return G.get_node_labels(lca);
    } else return gensizeerr(contextptr);
}
static const char _lowest_common_ancestor_s[]="lowest_common_ancestor";
static define_unary_function_eval(__lowest_common_ancestor,&_lowest_common_ancestor,_lowest_common_ancestor_s);
define_unary_function_ptr5(at_lowest_common_ancestor,alias_at_lowest_common_ancestor,&__lowest_common_ancestor,0,true)

/* USAGE:   st_ordering(G,s,t,[D],[p])
 *
 * Returns ST numbering for the biconnected graph G with source s and sink
 * (target) t. If an identifier D is given, the acyclic directed graph defined
 * by the ordering will ber constructed and stored to it. If a value of the
 * parameter p in [0,1] is given, the longest path in the resulting DAG will be
 * approcimately equal to L*p, where L is the length of the longest possible
 * (Hamiltonian) path.
 */
gen _st_ordering(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<3 && gv.size()>5)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_biconnected())
        return gt_err(_GT_ERR_BICONNECTED_GRAPH_REQUIRED);
    int s=G.node_index(gv[1]),t=G.node_index(gv[2]);
    if (s<0 || t<0)
        return gt_err(s<0?gv[1]:gv[2],_GT_ERR_VERTEX_NOT_FOUND);
    if (!G.has_edge(s,t))
        return gt_err(makevecteur(gv[1],gv[2]),_GT_ERR_EDGE_NOT_FOUND);
    double p=-1;
    gen gp;
    if ((gp=_evalf(gv.back(),contextptr)).type==_DOUBLE_) {
        p=gp.DOUBLE_val();
        if (p<0 || p>1)
            return generr("0<=p<=1 is required");
    }
    if (p<0)
        G.compute_st_numbering(s,t);
    else G.parametrized_st_orientation(s,t,p);
    vecteur st=G.get_st_numbering();
    if ((gv.size()==4 && p<0) || (gv.size()==5 && p>=0)) {
        if (gv[3].type!=_IDNT)
            return generrtype("Expected an identifier");
        G.assign_edge_directions_from_st();
        identifier_assign(*gv[3]._IDNTptr,G.to_gen(),contextptr);
    }
    return st;
}
static const char _st_ordering_s[]="st_ordering";
static define_unary_function_eval(__st_ordering,&_st_ordering,_st_ordering_s);
define_unary_function_ptr5(at_st_ordering,alias_at_st_ordering,&__st_ordering,0,true)

/* USAGE:   greedy_color(G,[p])
 *
 * Returns the list of vertex colors (positive integers) obtained by coloring
 * vertices one at a time [in the order given by permutation p], assigning to
 * it the smallest available color.
 */
gen _greedy_color(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    graphe::ivector p,colors;
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if (is_zero(_is_permu(g._VECTptr->back(),contextptr)))
            return generrtype("Expected a permutation");
        p=vecteur_2_vector_int(*g._VECTptr->back()._VECTptr);
        int offset=array_start(contextptr);
        for (graphe::ivector::iterator it=p.begin();it!=p.end();++it) {
            *it-=offset;
        }
    }
    graphe G(contextptr,false);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (p.empty()) { // construct the identity permutation
        p.resize(G.node_count());
        for (graphe::ivector::iterator it=p.begin();it!=p.end();++it) {
            *it=it-p.begin();
        }
    } else if (G.node_count()!=int(p.size()))
        return generr("Permutation size must match the number of vertices");
    G.greedy_vertex_coloring(p);
    G.get_node_colors(colors);
    return vector_int_2_vecteur(colors);
}
static const char _greedy_color_s[]="greedy_color";
static define_unary_function_eval(__greedy_color,&_greedy_color,_greedy_color_s);
define_unary_function_ptr5(at_greedy_color,alias_at_greedy_color,&__greedy_color,0,true)

/* USAGE:   is_bipartite(G,[P])
 *
 * Returns true iff the graph G is bipartite [and assign to P the list of partitions
 * represented as lists of vertices].
 */
gen _is_bipartite(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen P=undef;
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if ((P=g._VECTptr->back()).type!=_IDNT)
            return generrtype("Expected an identifier");
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    graphe::ivector V1,V2;
    if (!G.is_bipartite(V1,V2))
        return G.boole(false);
    if (!is_undef(P)) {
        identifier_assign(*P._IDNTptr,
                          makevecteur(_sort(G.get_node_labels(V1),contextptr),
                                      _sort(G.get_node_labels(V2),contextptr)),
                          contextptr);
    }
    return G.boole(true);
}
static const char _is_bipartite_s[]="is_bipartite";
static define_unary_function_eval(__is_bipartite,&_is_bipartite,_is_bipartite_s);
define_unary_function_ptr5(at_is_bipartite,alias_at_is_bipartite,&__is_bipartite,0,true)

/* USAGE:   plane_dual(G)
 *          plane_dual(F)
 *
 * Returns the plane dual of a biconnected planar graph G or constructed from
 * the list of faces F.
 */
gen _plane_dual(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype==_SEQ__VECT)
        return gentypeerr(contextptr);
    graphe::ivectors faces;
    graphe D(contextptr);
    if (g.subtype==_GRAPH__VECT) {
        graphe G(contextptr);
        if (!G.read_gen(g))
            return gt_err(_GT_ERR_NOT_A_GRAPH);
        if (!G.is_biconnected())
            return gt_err(_GT_ERR_BICONNECTED_GRAPH_REQUIRED);
        if (!G.demoucron(faces))
            return gt_err(_GT_ERR_NOT_PLANAR);
    } else {
        gen_map m;
        int k=0;
        vecteur &gv=*g._VECTptr;
        for (const_iterateur it=gv.begin();it!=gv.end();++it) {
            if (it->type!=_VECT)
                return gentypeerr(contextptr);
            graphe::ivector face;
            for (const_iterateur jt=it->_VECTptr->begin();jt!=it->_VECTptr->end();++jt) {
                if (is_zero(m[*jt]))
                    m[*jt]=++k;
                face.push_back(m[*jt].val-1);
            }
            faces.push_back(face);
        }
    }
    D.make_plane_dual(faces);
    return D.to_gen();
}
static const char _plane_dual_s[]="plane_dual";
static define_unary_function_eval(__plane_dual,&_plane_dual,_plane_dual_s);
define_unary_function_ptr5(at_plane_dual,alias_at_plane_dual,&__plane_dual,0,true)

/* USAGE:   is_vertex_colorable(G,k,[col])
 *
 * Returns true iff the vertices of graph G can be colored by using at most k
 * colors. If true is returned and an identifier col is given, the colors of
 * the vertices are stored in it.
 */
gen _is_vertex_colorable(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2 || gv.size()>3)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    int k;
    if (!gv[1].is_integer() || (k=gv[1].val)<1)
        return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    gen colors_dest=undef;
    if (gv.size()>2) {
        if (gv.back().type!=_IDNT)
            return generrtype("Expected an identifier");
        colors_dest=gv.back();
    }
    graphe G(contextptr,false);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_vertex_colorable(k))
        return graphe::boole(false);
    if (!is_undef(colors_dest)) {
        // store vertex colors to colors_dest
        graphe::ivector colors;
        G.get_node_colors(colors);
        vecteur cols=vector_int_2_vecteur(colors);
        identifier_assign(*colors_dest._IDNTptr,cols,contextptr);
    }
    return graphe::boole(true);
}
static const char _is_vertex_colorable_s[]="is_vertex_colorable";
static define_unary_function_eval(__is_vertex_colorable,&_is_vertex_colorable,_is_vertex_colorable_s);
define_unary_function_ptr5(at_is_vertex_colorable,alias_at_is_vertex_colorable,&__is_vertex_colorable,0,true)

/* USAGE:   set_vertex_positions(G,vp)
 *
 * Sets the coordinates, given in the list vp, to the vertices of graph G and
 * return the modified copy of G.
 */
gen _set_vertex_positions(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    if (gv.back().type!=_VECT)
        return generrtype("Expected a list of coordinates");
    vecteur vp=*_evalf(gv.back(),contextptr)._VECTptr;
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    int n,d=0;
    if ((n=vp.size())!=G.node_count())
        return generr("Number of positions must match the number of vertices");
    graphe::layout x(n);
    for (int i=0;i<n;++i) {
        graphe::gen2point(vp[i],x[i]);
        if (d==0)
            d=x[i].size();
        else if (int(x[i].size())!=d)
            return gendimerr(contextptr);
    }
    G.store_layout(x);
    return G.to_gen();
}
static const char _set_vertex_positions_s[]="set_vertex_positions";
static define_unary_function_eval(__set_vertex_positions,&_set_vertex_positions,_set_vertex_positions_s);
define_unary_function_ptr5(at_set_vertex_positions,alias_at_set_vertex_positions,&__set_vertex_positions,0,true)

/* USAGE:   clique_stats(G,[k or m..n])
 *
 * Returns the list of numbers of maximal cliques of size s in the graph G for
 * each s. If parameter k is given, the number of k-cliques is returned. If an
 * interval m..n is given, only cliques with size between m and n (inclusive)
 * are counted (m also may be +infinity).
 */
gen _clique_stats(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    int lb=0,ub=RAND_MAX;
    gen dest(undef);
    if (g.subtype==_SEQ__VECT) {
        int len=g._VECTptr->size();
        if (g._VECTptr->back().type==_IDNT) {
            dest=g._VECTptr->back();
            --len;
        }
        if (len==2) {
            gen &opt=g._VECTptr->at(1);
            if (opt.is_integer() && (lb=opt.val)>0)
                ub=opt.val;
            else if (opt.is_symb_of_sommet(at_interval)) {
                vecteur &bnds=*opt._SYMBptr->feuille._VECTptr;
                if (!bnds.front().is_integer() ||
                        !(bnds.back().is_integer() ||
                          (is_inf(bnds.back()) && is_positive(bnds.back(),contextptr))))
                    return gentypeerr(contextptr);
                lb=bnds.front().val;
                ub=is_inf(bnds.back())?RAND_MAX:bnds.back().val;
                if (lb<0 || ub<0 || lb>ub)
                    return gensizeerr(contextptr);
            } else return gentypeerr(contextptr);
        } else if (len>2)
            return gensizeerr(contextptr);
    }
    int mode=is_undef(dest)?0:3;
    graphe G(contextptr,mode==3);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return vecteur(0);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    map<int,int> stats,tmp;
    G.clique_stats(stats,mode);
    if (mode==3 && !stats.empty()) {
        const graphe::ivectors &mc=G.maximal_cliques();
        vecteur cg;
        int sz;
        for (graphe::ivectors_iter it=mc.begin();it!=mc.end();++it) {
            sz=it->size();
            if (sz>=lb && sz<=ub) {
                cg.push_back(G.get_node_labels(*it));
                ++tmp[sz];
            }
        }
        gen_map gm;
        for (map<int,int>::const_iterator it=tmp.begin();it!=tmp.end();++it) {
            (gm[it->first]=vecteur(0))._VECTptr->reserve(it->second);
        }
        for (const_iterateur it=cg.begin();it!=cg.end();++it) {
            gm[it->_VECTptr->size()]._VECTptr->push_back(*it);
        }
        cg.clear();
        for (gen_map::const_iterator it=gm.begin();it!=gm.end();++it) {
            cg.push_back(it->second);
        }
        assert(!cg.empty());
        identifier_assign(*dest._IDNTptr,cg.size()>1?cg:cg.front(),contextptr);
    }
    if (lb==ub)
        return stats[lb];
    vecteur res;
    for (map<int,int>::const_iterator it=stats.begin();it!=stats.end();++it) {
        if (it->first<=ub && it->first>=lb)
            res.push_back(makevecteur(it->first,it->second));
    }
    return res;
}
static const char _clique_stats_s[]="clique_stats";
static define_unary_function_eval(__clique_stats,&_clique_stats,_clique_stats_s);
define_unary_function_ptr5(at_clique_stats,alias_at_clique_stats,&__clique_stats,0,true)

/* USAGE:   minimal_vertex_coloring(G,[sto])
 *
 * Computes minimal vertex coloring for graph G and returns the colors in order
 * of vertices. If optional parameter "sto" is given, the colors are assigned
 * to vertices and the modified copy of G is returned.
 */
gen _minimal_vertex_coloring(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    bool store=false;
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if (g._VECTptr->back()!=at_sto)
            return gentypeerr(contextptr);
        store=true;
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    G.exact_vertex_coloring();
    graphe::ivector colors;
    G.get_node_colors(colors);
    vecteur cols=vector_int_2_vecteur(colors);
    if (store)
        return _highlight_vertex(makesequence(g._VECTptr->front(),G.vertices(),cols),contextptr);
    return cols;
}
static const char _minimal_vertex_coloring_s[]="minimal_vertex_coloring";
static define_unary_function_eval(__minimal_vertex_coloring,&_minimal_vertex_coloring,_minimal_vertex_coloring_s);
define_unary_function_ptr5(at_minimal_vertex_coloring,alias_at_minimal_vertex_coloring,&__minimal_vertex_coloring,0,true)

/* USAGE:   line_graph(G)
 *
 * Returns the line graph of the undirected input graph G.
 */
gen _line_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr),L(contextptr);
    if (!G.read_gen(g))
        gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    graphe::ipairs E;
    G.line_graph(L,E);
    return L.to_gen();
}
static const char _line_graph_s[]="line_graph";
static define_unary_function_eval(__line_graph,&_line_graph,_line_graph_s);
define_unary_function_ptr5(at_line_graph,alias_at_line_graph,&__line_graph,0,true)

/* USAGE:   transitive_closure(G,[weighted[=true or false]])
 *
 * Returns the [weighted, by default false] transitive closure of the input
 * graph G.
 */
gen _transitive_closure(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    bool weighted=false;
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        gen &opt=g._VECTptr->back();
        if (opt.is_integer() && opt.val==_GT_WEIGHTED)
            weighted=true;
        else if (opt.is_symb_of_sommet(at_equal)) {
            vecteur &args=*opt._SYMBptr->feuille._VECTptr;
            if (!args.front().is_integer() ||
                    args.front().val!=_GT_WEIGHTED ||
                    !args.back().is_integer())
                return gentypeerr(contextptr);
            weighted=(bool)args.back().val;
        }
    }
    graphe G(contextptr),C(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    G.transitive_closure(C,weighted);
    return C.to_gen();
}
static const char _transitive_closure_s[]="transitive_closure";
static define_unary_function_eval(__transitive_closure,&_transitive_closure,_transitive_closure_s);
define_unary_function_ptr5(at_transitive_closure,alias_at_transitive_closure,&__transitive_closure,0,true)

/* USAGE:   is_isomorphic(G1,G2,[isom])
 *
 * Returns true if the input graphs G1 and G2 are isomorphic, else returns
 * false. If an identifier 'isom' is given, the list with pairwise vertex
 * matching in G1 and G2 is stored to it.
 */
gen _is_isomorphic(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    gen isom=undef;
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2 || gv.size()>3)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G1(contextptr),G2(contextptr);
    if (!G1.read_gen(gv[0]) || !G2.read_gen(gv[1]))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G1.is_directed()!=G2.is_directed())
        return graphe::FAUX;
    if (gv.size()>2) {
        if ((isom=gv.back()).type!=_IDNT)
            return generrtype("Expected an identifier");
    }
    map<int,int> clab;
    int res=G1.is_isomorphic(G2,clab);
    if (res==0)
        return graphe::FAUX;
    if (res<0) // nauty not found
        return generr("nauty library is required for finding graph isomorphism");
    if (!is_undef(isom)) {
        vecteur mapping;
        int n=G1.node_count();
        for (int i=0;i<n;++i) {
            mapping.push_back(symbolic(at_equal,G1.node_label(i),G2.node_label(clab[i])));
        }
        identifier_assign(*isom._IDNTptr,mapping,contextptr);
    }
    return graphe::VRAI;
}
static const char _is_isomorphic_s[]="is_isomorphic";
static define_unary_function_eval(__is_isomorphic,&_is_isomorphic,_is_isomorphic_s);
define_unary_function_ptr5(at_is_isomorphic,alias_at_is_isomorphic,&__is_isomorphic,0,true)

/* USAGE:   graph_automorphisms(G)
 *
 * Returns the sequence of generators of Aut(G), the automorphism group of G.
 * Each element is a permutation in form of list of disjoint cycles.
 */
gen _graph_automorphisms(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    return G.aut_generators();
}
static const char _graph_automorphisms_s[]="graph_automorphisms";
static define_unary_function_eval(__graph_automorphisms,&_graph_automorphisms,_graph_automorphisms_s);
define_unary_function_ptr5(at_graph_automorphisms,alias_at_graph_automorphisms,&__graph_automorphisms,0,true)

/* USAGE:   canonical_labeling(G)
 *
 * Returns the permutation representing the canonical labeling of the input
 * graph G.
 */
gen _canonical_labeling(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    graphe::ivector sigma;
    if (!G.canonical_labeling(sigma)) // nauty not found
        return generr("nauty library is required for canonical labeling");
    vecteur res(G.node_count());
    int ofs=array_start(contextptr);
    for (iterateur it=res.begin();it!=res.end();++it) {
        *it=sigma[it-res.begin()]+ofs;
    }
    return res;
}
static const char _canonical_labeling_s[]="canonical_labeling";
static define_unary_function_eval(__canonical_labeling,&_canonical_labeling,_canonical_labeling_s);
define_unary_function_ptr5(at_canonical_labeling,alias_at_canonical_labeling,&__canonical_labeling,0,true)

/* USAGE:   minimal_edge_coloring(G,[sto])
 *
 * Finds the minimal edge coloring of the input graph G and returns the
 * sequence n,L where n is the class of G (1 for D colors and 2 for D+1 colors)
 * and L is the list of colors of edges of G as returned by the edges command,
 * or a copy of G with colored edges if the option 'sto' is specified.
 */
gen _minimal_edge_coloring(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    bool store=false;
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if (g._VECTptr->back()!=at_sto)
            return generr("Expected 'sto' as the second argument");
        store=true;
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    graphe::ivector colors;
    int typ=G.exact_edge_coloring(colors);
    if (typ==0)
        return undef;
    if (store)
        return G.to_gen();
    return makesequence(typ,vector_int_2_vecteur(colors));
}
static const char _minimal_edge_coloring_s[]="minimal_edge_coloring";
static define_unary_function_eval(__minimal_edge_coloring,&_minimal_edge_coloring,_minimal_edge_coloring_s);
define_unary_function_ptr5(at_minimal_edge_coloring,alias_at_minimal_edge_coloring,&__minimal_edge_coloring,0,true)

/* USAGE:   chromatic_index(G,[cols])
 *
 * Returns the chromatic index of the input graph G. If an identifier cols is
 * given, the coloring is stored to it.
 */
gen _chromatic_index(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen colors_dest=undef;
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if ((colors_dest=g._VECTptr->back()).type!=_IDNT)
            return generrtype("Expected an identifier");
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    graphe::ivector colors;
    int ncolors;
    G.exact_edge_coloring(colors,&ncolors);
    if (ncolors==0)
        return undef;
    if (!is_undef(colors_dest)) { // store the coloring
        identifier_assign(*colors_dest._IDNTptr,vector_int_2_vecteur(colors),contextptr);
    }
    return ncolors;
}
static const char _chromatic_index_s[]="chromatic_index";
static define_unary_function_eval(__chromatic_index,&_chromatic_index,_chromatic_index_s);
define_unary_function_ptr5(at_chromatic_index,alias_at_chromatic_index,&__chromatic_index,0,true)

/* USAGE:   is_hamiltonian(G,[C])
 *
 * Returns true if the input graph G is Hamiltonian, else returns false. If an
 * identifier C is given, the Hamiltonian circuit is stored to it.
 */
gen _is_hamiltonian(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen dest(undef);
    graphe G(contextptr);
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        dest=g._VECTptr->back();
        if (dest.type!=_IDNT)
            return generrtype("Expected an identifier");
    }
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    graphe::ivector hc;
    bool res=G.is_hamiltonian(hc);
    if (res && !is_undef(dest)) {
        if (hc.empty()) assert(G.hamcycle(hc));
        identifier_assign(*dest._IDNTptr,G.get_node_labels(hc),contextptr);
    }
    return graphe::boole(res);
}
static const char _is_hamiltonian_s[]="is_hamiltonian";
static define_unary_function_eval(__is_hamiltonian,&_is_hamiltonian,_is_hamiltonian_s);
define_unary_function_ptr5(at_is_hamiltonian,alias_at_is_hamiltonian,&__is_hamiltonian,0,true)

/* USAGE: traveling_salesman(G,[M])
 *
 * Returns a sequence of two objects, optimal cost for traveling salesman
 * problem and the corresponding Hamiltonian cycle in the undirected input
 * graph G. If G is not weighted, its adjacency matrix is used instead.
 * Alternatively, weight matrix may be passed as the optional parameter M. If G
 * is not Hamiltonian, an error is returned. A number of options may be passed
 * at the end of sequence of arguments: 'approx' for approximate solution, a
 * nonnegative integer representing the time limit (in milliseconds) or
 * 'vertex_distance' to automatically determine distances between the vertices
 * using their positions.
 */
gen _traveling_salesman(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    matrice M;
    vecteur options;
    if (g.subtype==_SEQ__VECT) {
        int pos=1;
        vecteur &gv=*g._VECTptr;
        if (gv.size()<2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if (gv.at(1).type==_VECT) {
            M=*gv.at(1)._VECTptr;
            ++pos;
        }
        options=vecteur(gv.begin()+pos,gv.end());
    }
    graphe G(contextptr),U(contextptr);
    graphe::ivector h;
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!M.empty() && !G.is_weighted()) {
        if (!is_squarematrix(M) || int(M.size())!=G.node_count())
            return generrdim("The given weight matrix has invalid dimensions");
        G.make_weighted(M);
    }
    if (G.is_directed()) {
        /* solve ATSP */
        graphe::ipairs incl;
        int i,j,k=1;
        for (const_iterateur it=options.begin();it!=options.end();++it) {
            if (it==options.begin() && it->is_symb_of_sommet(at_equal) &&
                it->_SYMBptr->feuille._VECTptr->front()==at_is_included &&
                it->_SYMBptr->feuille._VECTptr->back().type==_VECT) {
                const vecteur &v=*it->_SYMBptr->feuille._VECTptr->back()._VECTptr;
                if (ckmatrix(v)) {
                    incl.reserve(v.size());
                    for (const_iterateur jt=v.begin();jt!=v.end();++jt) {
                        if (jt->_VECTptr->size()!=2)
                            return generr("Expected an edge");
                        i=G.node_index(jt->_VECTptr->front());
                        j=G.node_index(jt->_VECTptr->back());
                        if (i<0 || j<0) return gt_err(_GT_ERR_EDGE_NOT_FOUND);
                        incl.push_back(make_pair(i,j));
                    }
                } else if (v.size()==2) {
                    i=G.node_index(v.front());
                    j=G.node_index(v.back());
                    if (i<0 || j<0) return gt_err(_GT_ERR_EDGE_NOT_FOUND);
                    incl.push_back(make_pair(i,j));
                } else return generr("Expected an edge or list of edges");
            } else if (it->is_integer() && it->val>0) {
                k=it->val;
            } else return generr("Option not supported");
        }
        graphe::ivectors hcv;
        graphe::dvector costs;
        if (!G.find_directed_tours(k,hcv,costs,incl))
            return undef;
        if (hcv.empty())
            return generr("Unable to find Hamiltonian cycle");
        vecteur res;
        G.ivectors2vecteur(hcv,res,false);
        vecteur cv(costs.size());
        for (iterateur it=cv.begin();it!=cv.end();++it) *it=gen(costs[it-cv.begin()]);
        if (G.is_weighted())
            return makesequence(k==1?cv.front():cv,k==1?res.front():res);
        return k==1?res.front():res;
    }
    if (G.hamcond()==0)
        return generr("The input graph is not Hamiltonian");
    /* parse options */
    bool approximate=false,make_distances=false;
    int time_limit=RAND_MAX;
    for (const_iterateur it=options.begin();it!=options.end();++it) {
        if (*it==at_approx)
            approximate=true;
        else if (approximate && it->is_integer())
            time_limit=it->val;
        else if (approximate && it->is_symb_of_sommet(at_equal) &&
                 it->_SYMBptr->feuille._VECTptr->front()==at_limit &&
                 it->_SYMBptr->feuille._VECTptr->back().is_integer())
            time_limit=it->_SYMBptr->feuille._VECTptr->back().val;
        else if (*it==at_vertex_distance && M.empty())
            make_distances=true;
        else return generr("Option not supported");
    }
    if (time_limit<0)
        return generr("Expected a nonnegative integer");
    if (make_distances) {
        if (G.is_weighted())
            return gt_err(_GT_ERR_UNWEIGHTED_GRAPH_REQUIRED);
        if (!G.make_euclidean_distances())
            return generr("Some vertex positions are invalid");
    }
    G.underlying(U);
    int res;
    double cost;
    if (approximate) {
        if (!G.is_weighted())
            gt_err(_GT_ERR_WEIGHTED_GRAPH_REQUIRED);
        if (!G.is_clique())
            return generr("The input graph must be complete");
        G.traveling_salesman(h,cost,true);
    } else {
        res=U.is_biconnected()?G.traveling_salesman(h,cost):0;
        if (res==0)
            return generr("The input graph is not Hamiltonian");
        if (res==-1)
            return undef;
    }
    /* success! */
    return G.is_weighted()?makesequence(!G.is_weighted()?gen(int(std::floor(cost+.5))):gen(cost),G.get_node_labels(h)):
                           G.get_node_labels(h);
}
static const char _traveling_salesman_s[]="traveling_salesman";
static define_unary_function_eval(__traveling_salesman,&_traveling_salesman,_traveling_salesman_s);
define_unary_function_ptr5(at_traveling_salesman,alias_at_traveling_salesman,&__traveling_salesman,0,true)

/* USAGE:   trail2edges(T)
 *
 * Returns the list of edges on the trail T.
 */
gen _trail2edges(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    vecteur t;
    if (g.type==_VECT)
        t=*g._VECTptr;
    else if (g.is_symb_of_sommet(at_trail))
        t=*g._SYMBptr->feuille._VECTptr;
    else return gentypeerr(contextptr);
    vecteur res;
    int n=t.size();
    for (int i=0;i<n-1;++i) {
        res.push_back(makevecteur(t[i],t[i+1]));
    }
    return res;
}
static const char _trail2edges_s[]="trail2edges";
static define_unary_function_eval(__trail2edges,&_trail2edges,_trail2edges_s);
define_unary_function_ptr5(at_trail2edges,alias_at_trail2edges,&__trail2edges,0,true)

/* USAGE:   maxflow(G,s,t,[F])
 *
 * Returns the optimal value for the max flow problem for network G with the
 * source s and sink t [along with an optimal flow F (as a matrix)].
 */
gen _maxflow(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<3)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    gen &S=gv[1],&T=gv[2];
    gen M(undef);
    if (gv.size()==4) {
        M=gv[3];
        if (M.type!=_IDNT)
            return generr("Expected an identifier");
    }
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    int s=G.node_index(S),t=G.node_index(T);
    if (s<0 || t<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    vector<map<int,gen> > flow;
    gen mf=G.maxflow_edmonds_karp(s,t,flow);
    int n=G.node_count();
    if (!is_undef(M)) {
        matrice m=*_matrix(makesequence(n,n,0),contextptr)._VECTptr;
        for (int i=0;i<n;++i) {
            map<int,gen> &f=flow[i];
            for (map<int,gen>::const_iterator it=f.begin();it!=f.end();++it) {
                m[i]._VECTptr->at(it->first)=max(it->second,0,contextptr);
            }
        }
        identifier_assign(*M._IDNTptr,m,contextptr);
    }
    return mf;
}
static const char _maxflow_s[]="maxflow";
static define_unary_function_eval(__maxflow,&_maxflow,_maxflow_s);
define_unary_function_ptr5(at_maxflow,alias_at_maxflow,&__maxflow,0,true)

/* USAGE:   minimum_cut(G,s,t)
 *
 * Returns the list of edges forming a minimum cut in a directed graph G with
 * respect to the source s and sink t.
 */
gen _minimum_cut(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=3)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    gen &S=gv[1],&T=gv[2];
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    int s=G.node_index(S),t=G.node_index(T);
    if (s<0 || t<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    vector<map<int,gen> > flow;
    G.maxflow_edmonds_karp(s,t,flow);
    graphe::ipairs cut;
    G.minimum_cut(s,flow,cut);
    vecteur res=G.ipairs2edges(cut);
    return change_subtype(res,_LIST__VECT);
}
static const char _minimum_cut_s[]="minimum_cut";
static define_unary_function_eval(__minimum_cut,&_minimum_cut,_minimum_cut_s);
define_unary_function_ptr5(at_minimum_cut,alias_at_minimum_cut,&__minimum_cut,0,true)

/* USAGE:   is_network(G,[s,t])
 *
 * Returns true if the graph G is a network with the source s and sink t, else
 * returns false. If s,t are not given, the output is a sequence of two sets of
 * vertices, sources and sinks (if both lists are empty then G is not a
 * network).
 */
gen _is_network(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen S=undef,T=undef;
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()!=3)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        S=gv[1];
        T=gv[2];
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    bool con=G.is_connected();
    vecteur sources,sinks;
    if (!is_undef(S) && !is_undef(T)) {
        int s=G.node_index(S),t=G.node_index(T);
        if (s<0 || t<0)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        if (!con || G.in_degree(s)>0 || G.out_degree(t)>0)
            return graphe::FAUX;
        return graphe::VRAI;
    } else if (con) {
        int n=G.node_count();
        for (int i=0;i<n;++i) {
            if (G.in_degree(i)==0)
                sources.push_back(G.node_label(i));
            else if (G.out_degree(i)==0) // can't be indeg=outdeg=0 because con=true
                sinks.push_back(G.node_label(i));
        }
    }
    return makesequence(_sort(sources,contextptr),_sort(sinks,contextptr));
}
static const char _is_network_s[]="is_network";
static define_unary_function_eval(__is_network,&_is_network,_is_network_s);
define_unary_function_ptr5(at_is_network,alias_at_is_network,&__is_network,0,true)

/* USAGE:   is_cut_set(G,E)
 *
 * Returns true if removing the edges in E from the graph G increases the
 * number of connected components of G, else returns false.
 */
gen _is_cut_set(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (gv.back().type!=_VECT)
        return gentypeerr("Expected a list of edges");
    vecteur &E=*gv.back()._VECTptr;
    graphe::ipairs edg;
    int i,j;
    for (const_iterateur it=E.begin();it!=E.end();++it) {
        if (it->type!=_VECT || it->_VECTptr->size()!=2)
            return gentypeerr("Expected an edge");
        const gen &u=it->_VECTptr->front(),&v=it->_VECTptr->back();
        i=G.node_index(u);
        j=G.node_index(v);
        if (i<0 || j<0)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        if (!G.has_edge(i,j))
            return gt_err(_GT_ERR_EDGE_NOT_FOUND);
        edg.push_back(make_pair(i,j));
    }
    graphe::ivectors comp;
    G.connected_components(comp);
    int nc=comp.size();
    for (graphe::ipairs_iter it=edg.begin();it!=edg.end();++it) {
        G.remove_edge(*it);
    }
    G.connected_components(comp);
    return nc!=int(comp.size())?graphe::VRAI:graphe::FAUX;
}
static const char _is_cut_set_s[]="is_cut_set";
static define_unary_function_eval(__is_cut_set,&_is_cut_set,_is_cut_set_s);
define_unary_function_ptr5(at_is_cut_set,alias_at_is_cut_set,&__is_cut_set,0,true)

/* USAGE:   random_network(a,b,[p],[opts])
 *
 * Returns a random network graph with b circular frames on a vertices in which
 * every edge appears with the probability p (by default 0.5). The source
 * vertex is in the bottom left corner of the first frame and the sink vertex
 * is in the top right corner of the b-th frame.
 */
gen _random_network(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()<2)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    int a,b;
    if (!gv[0].is_integer() || !gv[1].is_integer() || (a=gv[0].val)<1 || (b=gv[1].val)<1)
        return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    if (a+b<3)
        return generr("a+b>2 is required");
    double p=0.5;
    /* parse options */
    bool acyclic=false,weighted=false;
    gen lw,hw;
    for (const_iterateur it=gv.begin()+2;it!=gv.end();++it) {
        gen P=_evalf(*it,contextptr);
        if (P.type==_DOUBLE_ && is_strictly_positive(P,contextptr) && !is_strictly_greater(P,1,contextptr))
            p=P.DOUBLE_val();
        else if (it->is_integer()) {
            switch (it->val) {
            case _GT_ACYCLIC:
                acyclic=true;
                break;
            }
        } else if (it->is_symb_of_sommet(at_equal)) {
            gen &lh=it->_SYMBptr->feuille._VECTptr->front();
            gen &rh=it->_SYMBptr->feuille._VECTptr->back();
            if (lh.is_integer()) {
                switch (lh.val) {
                case _GT_ACYCLIC:
                    if (rh.is_integer() && rh.subtype==_INT_BOOLEAN)
                        acyclic=(bool)rh.val;
                    else return generr("Expected a boolean value");
                    break;
                case _GT_WEIGHTS:
                    if (rh.is_symb_of_sommet(at_interval)) {
                        lw=rh._SYMBptr->feuille._VECTptr->front();
                        hw=rh._SYMBptr->feuille._VECTptr->back();
                        if (_evalf(lw,contextptr).type!=_DOUBLE_ || _evalf(hw,contextptr).type!=_DOUBLE_)
                            return generr("Expected a real number");
                        if (is_greater(lw,hw,contextptr))
                            return generr("Invalid range");
                    } else return generr("Expected an interval");
                    weighted=true;
                    break;
                default:
                    return generr("Unrecognized option");
                }
            }
        }
    }
    /* construct the network */
    vecteur frames;
    gen lab1,lab2;
    int s,t,ofs=array_start(contextptr);
    for (int k=0;k<b;++k) {
        graphe F(contextptr);
        F.make_grid_graph(a,a);
        if (acyclic) {
            lab1=graphe::colon_label(ofs,ofs);
            lab2=graphe::colon_label(a-1+ofs,a-1+ofs);
            s=F.node_index(lab1); t=F.node_index(lab2);
            F.add_edge(s,t);
            F.compute_st_numbering(s,t);
            F.remove_edge(s,t);
            F.assign_edge_directions_from_st();
        } else F.make_directed();
        frames.push_back(F.to_gen());
    }
    graphe G(contextptr);
    G.read_gen(_disjoint_union(frames,contextptr));
    graphe::ivector x,y;
    for (int k=1;k<b;++k) {
        x=vecteur_2_vector_int(*_randperm(a,contextptr)._VECTptr);
        y=vecteur_2_vector_int(*_randperm(a,contextptr)._VECTptr);
        for (int i=0;i<a;++i) {
            for (int j=0;j<a;++j) {
                lab1=graphe::colon_label(k,i+ofs,j+ofs);
                lab2=graphe::colon_label(k+1,x[i],y[j]);
                G.add_edge(lab1,lab2);
            }
        }
    }
    graphe::ivector st(2);
    st.front()=G.node_index(graphe::colon_label(1,ofs,ofs));
    st.back()=G.node_index(graphe::colon_label(b,a-1+ofs,a-1+ofs));
    graphe::edgeset Eset;
    G.incident_edges(st,Eset);
    for (graphe::edgeset_iter it=Eset.begin();it!=Eset.end();++it) {
        const graphe::ipair &e=*it;
        if (e.first==st.back() || e.second==st.front())
            G.remove_edge(e);
    }
    graphe::ipairs E;
    G.get_edges_as_pairs(E);
    for (graphe::ipairs_iter it=E.begin();it!=E.end();++it) {
        const graphe::ipair &e=*it;
        if (G.rand_uniform()>p) {
            G.remove_edge(e);
            if (!G.is_connected())
                G.add_edge(e);
        }
    }
    int n=G.node_count();
    vecteur labels;
    G.make_default_labels(labels,n);
    G.relabel_nodes(labels);
    if (weighted)
        G.randomize_edge_weights(_evalf(lw,contextptr).DOUBLE_val(),_evalf(hw,contextptr).DOUBLE_val(),
                                 lw.is_integer() && hw.is_integer());
    return G.to_gen();
}
static const char _random_network_s[]="random_network";
static define_unary_function_eval(__random_network,&_random_network,_random_network_s);
define_unary_function_ptr5(at_random_network,alias_at_random_network,&__random_network,0,true)

/* USAGE:   tutte_polynomial(G,[x,y])
 *
 * Returns the Tutte polynomial (with x and y as its variables or
 * their values) of an undirected graph G. If G is weighted, all weights must
 * be positive integers and are interpreted as edge multiplicities.
 */
gen _tutte_polynomial(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen x=identificateur("x"),y=identificateur("y");
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()!=3)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        x=gv[1];
        y=gv[2];
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G.is_weighted()) {
        if (!G.weights2multiedges())
            return generr("Some edge weights are not positive integers");
        G.set_weighted(false);
    }
    gen p=G.tutte_polynomial(x,y);
    if (p.is_symb_of_sommet(at_plus) && p._SYMBptr->feuille._VECTptr->size()>7)
        return _factor(p,contextptr);
    return _ratnormal(p,contextptr);
}
static const char _tutte_polynomial_s[]="tutte_polynomial";
static define_unary_function_eval(__tutte_polynomial,&_tutte_polynomial,_tutte_polynomial_s);
define_unary_function_ptr5(at_tutte_polynomial,alias_at_tutte_polynomial,&__tutte_polynomial,0,true)

/* USAGE:   flow_polynomial(G,[x])
 *
 * Returns the flow polynomial (with x as its variable or value) of an
 * undirected unweighted graph G.
 */
gen _flow_polynomial(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen y=identificateur("x");
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        y=gv[1];
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G.is_weighted())
        return gt_err(_GT_ERR_UNWEIGHTED_GRAPH_REQUIRED);
    int n=G.node_count(),m=G.edge_count(),c=G.connected_component_count();
    assert(n>0 && c>0);
    gen p=_ratnormal(pow(gen(-1),m-n+c)*G.tutte_polynomial(0,1-y),contextptr);
    if (p.is_symb_of_sommet(at_plus) && p._SYMBptr->feuille._VECTptr->size()>7)
        return _factor(p,contextptr);
    return p;
}
static const char _flow_polynomial_s[]="flow_polynomial";
static define_unary_function_eval(__flow_polynomial,&_flow_polynomial,_flow_polynomial_s);
define_unary_function_ptr5(at_flow_polynomial,alias_at_flow_polynomial,&__flow_polynomial,0,true)

/* USAGE:   chromatic_polynomial(G,[t])
 *
 * Returns the chromatic polynomial (with t as its variable or value) of an
 * undirected unweighted graph G.
 */
gen _chromatic_polynomial(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen x=identificateur("x");
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        x=gv[1];
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G.is_weighted())
        return gt_err(_GT_ERR_UNWEIGHTED_GRAPH_REQUIRED);
    int n=G.node_count(),c=G.connected_component_count();
    assert(n>0 && c>0);
    gen p=_ratnormal(pow(gen(-1),n-c)*pow(x,c)*G.tutte_polynomial(1-x,0),contextptr);
    if (p.is_symb_of_sommet(at_plus) && p._SYMBptr->feuille._VECTptr->size()>7)
        return _factor(p,contextptr);
    return p;
}
static const char _chromatic_polynomial_s[]="chromatic_polynomial";
static define_unary_function_eval(__chromatic_polynomial,&_chromatic_polynomial,_chromatic_polynomial_s);
define_unary_function_ptr5(at_chromatic_polynomial,alias_at_chromatic_polynomial,&__chromatic_polynomial,0,true)

/* USAGE:   reliability_polynomial(G,[t])
 *
 * Returns the reliability polynomial (with t as its variable or value) of an
 * undirected graph G. If G is weighted, all weights must be positive integers
 * and are interpreted as edge multiplicities.
 */
gen _reliability_polynomial(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    gen x=identificateur("x"),tmp=identificateur(" x");
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        x=gv[1];
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G.is_weighted()) {
        if (!G.weights2multiedges())
            return generr("Some edge weights are not positive integers");
    }
    int n=G.node_count(),m=G.edge_count(),c=G.connected_component_count();
    assert(n>0 && c>0);
    gen p=_ratnormal(_subs(makesequence(_ratnormal(pow(gen(1-tmp),n-c)*pow(tmp,m-n+c)
                                                   *G.tutte_polynomial(1,pow(tmp,-1)),contextptr),
                                        tmp,x),contextptr),contextptr);
    if (p.is_symb_of_sommet(at_plus) && p._SYMBptr->feuille._VECTptr->size()>7)
        return _factor(p,contextptr);
    return p;
}
static const char _reliability_polynomial_s[]="reliability_polynomial";
static define_unary_function_eval(__reliability_polynomial,&_reliability_polynomial,_reliability_polynomial_s);
define_unary_function_ptr5(at_reliability_polynomial,alias_at_reliability_polynomial,&__reliability_polynomial,0,true)

/* USAGE:   laplacian_matrix(G,[normal])
 *
 * Returns the Laplacian matrix L=D-A of an undirected graph G where D resp. A
 * is the degree matrix resp. the adjacency matrix of G.
 */
gen _laplacian_matrix(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    bool normalize=false;
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if (gv[1]==at_normal)
            normalize=true;
        else return generr("Unrecognized option");
    }
    graphe G(contextptr,false);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    matrice L;
    G.laplacian_matrix(L,normalize);
    return change_subtype(_ratnormal(L,contextptr),_MATRIX__VECT);
}
static const char _laplacian_matrix_s[]="laplacian_matrix";
static define_unary_function_eval(__laplacian_matrix,&_laplacian_matrix,_laplacian_matrix_s);
define_unary_function_ptr5(at_laplacian_matrix,alias_at_laplacian_matrix,&__laplacian_matrix,0,true)

/* USAGE:   fundamental_cycle(G)
 *
 * Returns the unique cycle in an unicyclic graph G as a graph.
 */
gen _fundamental_cycle(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (!G.is_connected())
        return gt_err(_GT_ERR_CONNECTED_GRAPH_REQUIRED);
    graphe::ivectors cycles;
    G.fundamental_cycles(cycles,-1,false);
    if (cycles.size()!=1)
        return generr("The graph is not unicyclic");
    graphe::ivector &fc=cycles.front();
    return _cycle_graph(G.get_node_labels(fc),contextptr);
}
static const char _fundamental_cycle_s[]="fundamental_cycle";
static define_unary_function_eval(__fundamental_cycle,&_fundamental_cycle,_fundamental_cycle_s);
define_unary_function_ptr5(at_fundamental_cycle,alias_at_fundamental_cycle,&__fundamental_cycle,0,true)

/* USAGE:   cycle_basis(G)
 *
 * Returns the list of all fundamental cycles in undirected graph G.
 */
gen _cycle_basis(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    graphe::ivectors cycles;
    G.fundamental_cycles(cycles);
    vecteur res(cycles.size());
    for (graphe::ivectors_iter it=cycles.begin();it!=cycles.end();++it) {
        res[it-cycles.begin()]=G.get_node_labels(*it);
    }
    return change_subtype(res,_LIST__VECT);
}
static const char _cycle_basis_s[]="cycle_basis";
static define_unary_function_eval(__cycle_basis,&_cycle_basis,_cycle_basis_s);
define_unary_function_ptr5(at_cycle_basis,alias_at_cycle_basis,&__cycle_basis,0,true)

/* USAGE:   mycielski(G)
 *
 * Returns the Mycielski graph of an undirected graph G.
 */
gen _mycielski(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    graphe M(contextptr);
    G.mycielskian(M);
    return M.to_gen();
}
static const char _mycielski_s[]="mycielski";
static define_unary_function_eval(__mycielski,&_mycielski,_mycielski_s);
define_unary_function_ptr5(at_mycielski,alias_at_mycielski,&__mycielski,0,true)

/* USAGE:   clustering_coefficient(G,[v])
 *
 * Returns the [local] clustering coefficient [of the vertex v or vertices from
 * a sequence or list v] of an undirected graph G.
 */
gen _clustering_coefficient(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    vecteur V;
    bool apprx=false,exct=false;
    if (g.subtype==_SEQ__VECT) {
        vecteur &gv=*g._VECTptr;
        if (gv.size()<2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        for (const_iterateur it=gv.begin()+1;it!=gv.end();++it) {
            if (it->type==_VECT) {
                V=*it->_VECTptr;
            } else if (*it==at_approx) {
                apprx=true;
            } else if (*it==at_exact) {
                exct=true;
            } else {
                V.push_back(*it);
                break;
            }
            if (it-gv.begin()>1)
                return gentypeerr(contextptr);
        }
    }
    graphe G(contextptr,!V.empty());
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (V.empty())
        return G.clustering_coeff(apprx,exct);
    int i;
    vecteur res;
    for (const_iterateur it=V.begin();it!=V.end();++it) {
        i=G.node_index(*it);
        if (i<0)
            return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
        res.push_back(G.degree(i)<2?0:G.local_clustering_coeff(i));
    }
    if (res.size()==1)
        return res.front();
    return res;
}
static const char _clustering_coefficient_s[]="clustering_coefficient";
static define_unary_function_eval(__clustering_coefficient,&_clustering_coefficient,_clustering_coefficient_s);
define_unary_function_ptr5(at_clustering_coefficient,alias_at_clustering_coefficient,&__clustering_coefficient,0,true)

/* USAGE:   network_transitivity(G)
 *
 * Returns the transitivity (triangle density) of a graph G.
 */
gen _network_transitivity(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    return G.transitivity();
}
static const char _network_transitivity_s[]="network_transitivity";
static define_unary_function_eval(__network_transitivity,&_network_transitivity,_network_transitivity_s);
define_unary_function_ptr5(at_network_transitivity,alias_at_network_transitivity,&__network_transitivity,0,true)

/* USAGE:   two_edge_connected_components(G)
 *
 * Returns the list of two-edge connected components of an undirected graph G.
 */
gen _two_edge_connected_components(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return vecteur(0);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    graphe::ipairs bridges;
    G.find_bridges(bridges);
    for (graphe::ipairs_iter it=bridges.begin();it!=bridges.end();++it) {
        G.remove_edge(*it);
    }
    graphe::ivectors comp;
    G.connected_components(comp);
    vecteur res(comp.size());
    G.ivectors2vecteur(comp,res,true);
    return change_subtype(res,_LIST__VECT);
}
static const char _two_edge_connected_components_s[]="two_edge_connected_components";
static define_unary_function_eval(__two_edge_connected_components,&_two_edge_connected_components,_two_edge_connected_components_s);
define_unary_function_ptr5(at_two_edge_connected_components,alias_at_two_edge_connected_components,&__two_edge_connected_components,0,true)

/* USAGE:   is_two_edge_connected(G)
 *
 * Returns true iff the undirected graph G is two-edge connected.
 */
gen _is_two_edge_connected(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_null())
        return gt_err(_GT_ERR_GRAPH_IS_NULL);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    graphe::ipairs bridges;
    G.find_bridges(bridges);
    return graphe::boole(bridges.empty());
}
static const char _is_two_edge_connected_s[]="is_two_edge_connected";
static define_unary_function_eval(__is_two_edge_connected,&_is_two_edge_connected,_is_two_edge_connected_s);
define_unary_function_ptr5(at_is_two_edge_connected,alias_at_is_two_edge_connected,&__is_two_edge_connected,0,true)

/* USAGE:   edge_connectivity(G)
 *
 * Returns the edge connectivity of an undirected graph G.
 */
gen _edge_connectivity(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G.node_count()<2)
        return generr("The graph must have at least two vertices");
    if (!G.is_connected())
        return gt_err(_GT_ERR_CONNECTED_GRAPH_REQUIRED);
    return G.edge_connectivity();
}
static const char _edge_connectivity_s[]="edge_connectivity";
static define_unary_function_eval(__edge_connectivity,&_edge_connectivity,_edge_connectivity_s);
define_unary_function_ptr5(at_edge_connectivity,alias_at_edge_connectivity,&__edge_connectivity,0,true)

/* USAGE:   vertex_connectivity(G)
 *
 * Returns the vertex connectivity of an undirected graph G.
 */
gen _vertex_connectivity(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G.node_count()<2)
        return generr("The graph must have at least two vertices");
    if (!G.is_connected())
        return gt_err(_GT_ERR_CONNECTED_GRAPH_REQUIRED);
    return G.vertex_connectivity();
}
static const char _vertex_connectivity_s[]="vertex_connectivity";
static define_unary_function_eval(__vertex_connectivity,&_vertex_connectivity,_vertex_connectivity_s);
define_unary_function_ptr5(at_vertex_connectivity,alias_at_vertex_connectivity,&__vertex_connectivity,0,true)

/* USAGE:   tonnetz(a,b,c,[d])
 *
 * Returns the neo.Riemannian style tone network corresponding to the pitch
 * class [a,b,c] resp. [a,b,c,d], where n=a+b+c[+d] is the octave range.
 * Vertices of the resulting graph are elements of the cyclic group Zn, i.e.
 * 0,1,...,n-1. The neighbors of vertex v are: v+a, v-a, v+b, v-b, v+c, v-c,
 * [v+a+b, v-a-b, v+b+c,v-b-c].
 */
gen _tonnetz(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur &gv=*g._VECTptr;
    if (gv.size()!=3 && gv.size()!=4)
        return gensizeerr(contextptr);
    for (const_iterateur it=gv.begin();it!=gv.end();++it) {
        if (!it->is_integer() || !is_strictly_positive(*it,contextptr))
            return gensizeerr(contextptr);
    }
    graphe G(contextptr);
    vecteur labels;
    int n=_sum(gv,contextptr).val;
    G.make_default_labels(labels,n,0,0);
    G.add_nodes(labels);
    int a=gv[0].val,b=gv[1].val,c=gv[2].val;
    for (int i=0;i<n;++i) {
        set<int> adj;
        adj.insert((n+i+a)%n);
        adj.insert((n+i-a)%n);
        adj.insert((n+i+b)%n);
        adj.insert((n+i-a)%n);
        adj.insert((n+i+c)%n);
        adj.insert((n+i-c)%n);
        for (set<int>::const_iterator it=adj.begin();it!=adj.end();++it) {
            G.add_edge(i,*it);
        }
    }
    if (gv.size()==4) {
        int d=gv[3].val;
        for (int i=0;i<n;++i) {
            set<int> adj;
            adj.insert((n+i+d)%n);
            adj.insert((n+i-d)%n);
            adj.insert((n+i+a+b)%n);
            adj.insert((n+i-a-b)%n);
            adj.insert((n+i+b+c)%n);
            adj.insert((n+i-b-c)%n);
            for (set<int>::const_iterator it=adj.begin();it!=adj.end();++it) {
                G.add_edge(i,*it);
            }
        }
    }
    return G.to_gen();
}
static const char _tonnetz_s[]="tonnetz";
static define_unary_function_eval(__tonnetz,&_tonnetz,_tonnetz_s);
define_unary_function_ptr5(at_tonnetz,alias_at_tonnetz,&__tonnetz,0,true)

/* USAGE:   truncate_graph(G)
 *
 * Returns the graph obtained by truncating the biconnected planar graph G.
 */
gen _truncate_graph(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    graphe G(contextptr,false),H(contextptr);
    if (!G.read_gen(g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_directed())
        return gt_err(_GT_ERR_UNDIRECTED_GRAPH_REQUIRED);
    if (G.is_empty())
        return generr("graph is empty");
    if (!G.is_biconnected())
        return gt_err(_GT_ERR_BICONNECTED_GRAPH_REQUIRED);
    graphe::ivectors faces;
    if (!G.demoucron(faces))
        return gt_err(_GT_ERR_NOT_PLANAR);
    G.truncate(H,faces);
    return H.to_gen();
}
static const char _truncate_graph_s[]="truncate_graph";
static define_unary_function_eval(__truncate_graph,&_truncate_graph,_truncate_graph_s);
define_unary_function_ptr5(at_truncate_graph,alias_at_truncate_graph,&__truncate_graph,0,true)

/* USAGE:   find_cycles(G,[length=k||lb..ub])
 *
 * Returns the list of elementary cycles of the digraph G. If length option is
 * specified, only cycles of length k resp. of length between lb and ub are
 * returned.
 */
gen _find_cycles(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT)
        return gentypeerr(contextptr);
    int lb=-1,ub=-1;
    if (g.subtype==_SEQ__VECT) {
        if (g._VECTptr->size()!=2)
            return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
        if (!g._VECTptr->at(1).is_symb_of_sommet(at_equal))
            return gensizeerr(contextptr);
        gen &opt=g._VECTptr->at(1);
        if (opt._SYMBptr->feuille._VECTptr->front()!=at_length)
            return gensizeerr(contextptr);
        gen &val=opt._SYMBptr->feuille._VECTptr->back();
        if (val.is_integer() && val.val>0)
            lb=ub=val.val;
        else if (val.is_symb_of_sommet(at_interval)) {
            gen &lo=val._SYMBptr->feuille._VECTptr->front();
            gen &hi=val._SYMBptr->feuille._VECTptr->back();
            if (!lo.is_integer() || !hi.is_integer())
                return gensizeerr(contextptr);
            lb=lo.val;
            ub=hi.val;
            if (lb<0 || ub<0 || lb>ub)
                return gensizeerr(contextptr);
        }
    }
    graphe G(contextptr);
    if (!G.read_gen(g.subtype==_SEQ__VECT?g._VECTptr->front():g))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_empty())
        return generr("graph is empty");
    if (!G.is_directed())
        return gt_err(_GT_ERR_DIRECTED_GRAPH_REQUIRED);
    graphe::ivectors cyc;
    G.elementary_cycles(cyc,lb,ub);
    vecteur res;
    G.ivectors2vecteur(cyc,res,false);
    return change_subtype(res,_LIST__VECT);
}
static const char _find_cycles_s[]="find_cycles";
static define_unary_function_eval(__find_cycles,&_find_cycles,_find_cycles_s);
define_unary_function_ptr5(at_find_cycles,alias_at_find_cycles,&__find_cycles,0,true)

/* USAGE:   kspaths(G,u,v,k)
 *
 * Returns the list of k shortest paths between vertices u and v in the
 * (weighted) digraph G.
 */
gen _kspaths(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    const vecteur &gv=*g._VECTptr;
    if (gv.size()!=4)
        return gt_err(_GT_ERR_WRONG_NUMBER_OF_ARGS);
    graphe G(contextptr);
    if (!G.read_gen(gv.front()))
        return gt_err(_GT_ERR_NOT_A_GRAPH);
    if (G.is_empty())
        return generr("graph is empty");
    graphe::ivectors paths;
    int k,src,dest;
    src=G.node_index(gv[1]);
    dest=G.node_index(gv[2]);
    if (src<0 || dest<0)
        return gt_err(_GT_ERR_VERTEX_NOT_FOUND);
    if (src==dest)
        return generr("source and destination vertices must be different");
    if (!gv.back().is_integer() || (k=gv.back().val)<=0)
        return gt_err(_GT_ERR_POSITIVE_INTEGER_REQUIRED);
    G.yen_ksp(k,src,dest,paths);
    vecteur res;
    G.ivectors2vecteur(paths,res);
    return change_subtype(res,_LIST__VECT);
}
static const char _kspaths_s[]="kspaths";
static define_unary_function_eval(__kspaths,&_kspaths,_kspaths_s);
define_unary_function_ptr5(at_kspaths,alias_at_kspaths,&__kspaths,0,true)

void nexcom(int n,int k,int &h,int &t,vector<int> &r,bool &mtc) {
    int i;
    if (mtc) goto label20;
    r[0]=n;
    t=n;
    h=0;
    if (k==1) goto label15;
    for (i=1;i<k;++i) {
        r[i]=0;
    }
label15:
    mtc=r.back()!=n;
    return;
label20:
    if (t>1) h=0;
    ++h;
    t=r[h-1];
    r[h-1]=0;
    r[0]=t-1;
    ++r[h];
    goto label15;
}

/* USAGE:   icomp(n,k,[zeros=true or false])
 *
 * Returns the list of all decompositions of n into k parts (the order of parts
 * matters). If zeros is set to false, the compositions containing zero are
 * omitted (by default, zeros=true).
 */
gen _icomp(const gen &g,GIAC_CONTEXT) {
    if (g.type==_STRNG && g.subtype==-1) return g;
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gentypeerr(contextptr);
    vecteur gv=*g._VECTptr;
    if (gv.size()<1)
        return gensizeerr(contextptr);
    bool zer=true;
    if (gv.back().is_symb_of_sommet(at_equal)) {
        gen &lh=gv.back()._SYMBptr->feuille._VECTptr->front();
        gen &rh=gv.back()._SYMBptr->feuille._VECTptr->back();
        if (lh!=at_zeros || !rh.is_integer())
            return gensizeerr(contextptr);
        zer=(bool)rh.val;
        gv.pop_back();
    }
    if (gv.size()!=2)
        return gensizeerr(contextptr);
    gen &N=gv.front(),&K=gv.back();
    if (!is_integer(N) || !is_integer(K) || N.val<=0 || K.val<=0 || K.val>N.val)
        return gensizeerr(contextptr);
    int n=N.val,k=K.val,h,t;
    vector<int> r(k);
    bool mtc=false;
    vecteur res;
    do {
        nexcom(n,k,h,t,r,mtc);
        res.push_back(vector_int_2_vecteur(r,contextptr));
    } while (mtc);
    if (!zer) {
        for (int i=res.size();i-->0;) {
            if (!is_zero(_count_eq(makesequence(0,res[i]),contextptr)))
                res.erase(res.begin()+i);
        }
    }
    return res;
}
static const char _icomp_s[]="icomp";
static define_unary_function_eval(__icomp,&_icomp,_icomp_s);
define_unary_function_ptr5(at_icomp,alias_at_icomp,&__icomp,0,true)

#ifndef NO_NAMESPACE_GIAC
}
#endif // ndef NO_NAMESPACE_GIAC
