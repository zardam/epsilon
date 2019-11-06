/*
 * lpsolve.cc
 *
 * Copyright 2017 Luka Marohnić
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

#include "lpsolve.h"
#include "optimization.h"
#include "giac.h"
#include <ctime>

#ifndef DBL_MAX
#define DBL_MAX 1.79769313486e+308
#endif

using namespace std;

#ifndef NO_NAMESPACE_GIAC
namespace giac {
#endif //ndef NO_NAMESPACE_GIAC

const gen inf=symbolic(at_plus,unsigned_inf);
const gen minf=symbolic(at_neg,unsigned_inf);

/*
 * Convert double to exact gen.
 */
gen double2fraction(double d,GIAC_CONTEXT) {
    return exact(double2gen(d),contextptr);
}

/*
 * Convert gen to double.
 */
double gen2double(const gen &g,GIAC_CONTEXT) {
    return _evalf(g,contextptr).DOUBLE_val();
}

/*
 * Return number of digits of an unsigned integer.
 */
int numdigits(unsigned i) {
    return 1+(i>0?(int)std::log10((double)i):0);
}

/*
 * Return true iff g is a (vector of) real number or +infinity or -infinity.
 */
bool is_realcons(const gen &g,GIAC_CONTEXT) {
    if (g.type==_VECT) {
        vecteur &v = *g._VECTptr;
        for (const_iterateur it=v.begin();it!=v.end();++it) {
            if (!is_realcons(*it,contextptr))
                return false;
        }
        return true;
    }
    return (is_inf(g) || _evalf(g,contextptr).type==_DOUBLE_);
}

/*
 * If g is an interval, store its bounds to pair p and return true. If g is not
 * an interval, return false.
 */
bool interval2pair(const gen &g,pair<gen,gen> &p,GIAC_CONTEXT) {
    if (g.type!=_SYMB || !g.is_symb_of_sommet(at_interval))
        return false;  //g is not an interval
    vecteur &v=*g._SYMBptr->feuille._VECTptr;
    p=make_pair(v[0],v[1]);
    return is_realcons(v,contextptr);
}

/*
 * Make singleton vector (with one at position j and zeros at other positions).
 */
vecteur singleton(int n,int j) {
    vecteur v(n,gen(0));
    v[j]=gen(1);
    return v;
}

/*
 * Insert column in matrix at position j.
 */
void insert_column(matrice &m,const vecteur &c,int j) {
    assert(m.size()==c.size());
    for (int i=0;i<int(m.size());++i) {
        m[i]._VECTptr->insert(j>=0?m[i]._VECTptr->begin()+j:m[i]._VECTptr->end()+j,c[i]);
    }
}

/*
 * Append column to matrix.
 */
void append_column(matrice &m,const vecteur &c) {
    assert(m.size()==c.size());
    matrice mt=mtran(m);
    mt.push_back(c);
    m=mtran(mt);
}

/*
 * Remove jth column from matrix. If j<0, count from last column towards the first.
 */
void remove_column(matrice &m,int j) {
    for (int i=0;i<int(m.size());++i) {
        m[i]._VECTptr->erase(j+(j>=0?m[i]._VECTptr->begin():m.end()));
    }
}

/*
 * Get jth column from matrix. If j<0, count from last column towards the first.
 */
vecteur jth_column(const matrice &m,int j) {
    int n=m.front()._VECTptr->size();
    vecteur col(m.size());
    for (int i=0;i<int(m.size());++i) {
        col[i]=m[i][j>=0?j:n+j];
    }
    return col;
}

/*
 * Multiply coefficients by LCM of denominators and then divide them by their
 * GCD.
 */
vecteur integralize(const vecteur &v_orig,GIAC_CONTEXT) {
    vecteur v(v_orig),vd;
    for (const_iterateur it=v.begin();it!=v.end();++it) {
        if (!is_zero(*it))
            vd.push_back(_denom(*it,contextptr));
    }
    if (vd.empty())
        return v;
    v=multvecteur(abs(_lcm(vd,contextptr),contextptr),v);
    return divvecteur(v,abs(_gcd(v,contextptr),contextptr));
}

/*
 * LP variable constructor. By default, it is a nonnegative variable
 * unrestricted from above.
 */
lp_variable::lp_variable() {
    is_integral=false;
    sign_type=_LP_VARSIGN_POS;
    range=lp_range();
    range.lbound=gen(0);
    fill_n(nbranch,2,0);
}

/*
 * Update lower (dir=0) or upper (dir=1) pseudocost.
 */
void lp_variable::update_pseudocost(double delta, double fr, int dir) {
    double sigma=pseudocost[dir]*nbranch[dir];
    sigma+=delta/(dir==0?fr:(1-fr));
    pseudocost[dir]=sigma/(++nbranch[dir]);
}

/*
 * Return score, a positive value based on pseudocost values. Variable with the
 * best (highest) score is selected for branching.
 */
double lp_variable::score(double fr) {
    if (nbranch[0]==0 || nbranch[1]==0)
        return 0;
    double qlo=fr*pseudocost[0],qhi=(1-fr)*pseudocost[1];
    return (1-LP_SCORE_FACTOR)*std::min(qlo,qhi)+LP_SCORE_FACTOR*std::max(qlo,qhi);
}

/*
 * Range constructor: by default, it contains no restriction.
 */
lp_range::lp_range () {
    lbound=minf;
    ubound=inf;
}

/*
 * Settings constructor loads some sensible defaults.
 */
lp_settings::lp_settings() {
    verbose=false;
    status_report_freq=0.2;
    solver=_LP_SIMPLEX;
    precision=_LP_PROB_DEPENDENT;
    presolve=false;
    maximize=false;
    relative_gap_tolerance=0.0;
    has_binary_vars=false;
    varselect=-1;
    nodeselect=_LP_BEST_PROJECTION;
    depth_limit=RAND_MAX;
    node_limit=RAND_MAX;
    iteration_limit=RAND_MAX;
    time_limit=RAND_MAX;
    max_cuts=5;
}

/*
 * Stats constructor initializes the status container for the problem being
 * solved. It is used to monitor the progress and to summarize when done.
 */
lp_stats::lp_stats() {
    subproblems_examined=0;
    cuts_applied=0;
    max_active_nodes=0;
    mip_gap=-1; //negative means undefined
}

/*
 * Pivot on element with coordinates I,J.
 */
void pivot_ij(matrice &m,int I,int J,bool negate=false) {
    int nr=m.size(),nc=m.front()._VECTptr->size();
    gen a(m[I][J]),b;
    m[I]=divvecteur(*m[I]._VECTptr,a);
    vecteur col(jth_column(m,J)),&pv=*m[I]._VECTptr;
    for (int i=0;i<nr;++i) {
        m[i]._VECTptr->at(J)=(i==I?gen(negate?-1:1)/a:gen(0));
    }
    for (int i=0;i<nr;++i) {
        b=col[i];
        if (i==I || is_zero(b))
            continue;
        vecteur &v=*m[i]._VECTptr;
        for (int j=0;j<nc;++j) {
            v[j]-=b*pv[j];
        }
    }
}

/*
 * Simplex algorithm that handles upper bounds of the variables. The solution x
 * satisfies 0<=x<=u. An initial basis must be provided.
 *
 * Basis is a vector of integers B and B[i]=j means that jth variable is basic
 * and appears in ith row (constraint). Basic columns are not kept in matrix,
 * which contains only the columns of nonbasic variables. A nonbasic variable
 * is assigned to the respective column with integer vector 'cols': cols[i]=j
 * means that ith column of the matrix is associated with the jth variable. The
 * algorithm uses upper-bounding technique when pivoting and uses an adaptation
 * of Bland's rule to prevent cycling. ith element of 'is_slack' is true iff
 * ith (nonbasic) variable is at its upper bound.
 *
 * If limit>0, simplex algorithm will terminate after that many iterations.
 */
matrice simplex_reduce_bounded(const matrice &m_orig,const vecteur &u,vector<bool> &is_slack,vecteur &bfs,gen &optimum,
                               ints &basis,ints &cols,int limit,GIAC_CONTEXT) {
    matrice m(m_orig);
    //ev, lv: indices of entering and leaving variables
    //ec, lr: 'entering' column and 'leaving' row in matrix m, respectively
    int nr=basis.size(),nc=cols.size(),nv=nr+nc,ec,ev,lr,lv,icount=0;
    gen a,b,ratio,mincoeff,opt;
    //iterate the simplex method
    bool choose_first=false;
    optimum=undef;
    while ((icount++)<limit) {
        opt=m.back()._VECTptr->back();
        //determine which variable enters the basis
        mincoeff=gen(0);
        ev=-1;
        m.back()=_eval(m.back(),contextptr);
        vecteur &last=*m.back()._VECTptr;
        for (int j=0;j<nc;++j) {
            int k=cols[j];
            if ((choose_first && is_strictly_positive(-last[j],contextptr) &&
                 (ev<0 || k+(is_slack[k]?nv:0)<ev+(is_slack[ev]?nv:0))) ||
                    (!choose_first && is_strictly_greater(mincoeff,last[j],contextptr))) {
                ec=j;
                ev=k;
                mincoeff=last[j];
            }
        }
        if (ev<0) //the current solution is optimal
            break;
        //determine which variable leaves the basis
        mincoeff=inf;
        lv=-1;
        bool hits_ub,ub_subs;
        for (int i=0;i<nr;++i) {
            a=m[i][ec];
            b=m[i]._VECTptr->back();
            int j=basis[i];
            if (is_strictly_positive(a,contextptr) && is_greater(mincoeff,ratio=b/a,contextptr))
                hits_ub=false;
            else if (is_strictly_positive(-a,contextptr) && !is_inf(u[j]) &&
                     is_greater(mincoeff,ratio=(b-u[j])/a,contextptr))
                hits_ub=true;
            else continue;
            if (is_strictly_greater(mincoeff,ratio,contextptr)) {
                lv=-1;
                mincoeff=ratio;
            }
            if (lv<0 || (choose_first && j+(is_slack[j]?nv:0)<lv+(is_slack[lv]?nv:0))) {
                lv=j;
                lr=i;
                ub_subs=hits_ub;
            }
        }
        if (lv<0 && is_inf(u[ev])) { //solution is unbounded
            optimum=inf;
            return m;
        }
        if (is_zero(mincoeff))
            choose_first=true; //switch on Bland's rule
        if (lv<0 || is_greater(mincoeff,u[ev],contextptr)) {
            for (iterateur it=m.begin();it!=m.end();++it) {
                it->_VECTptr->back()-=u[ev]*(a=it->_VECTptr->at(ec));
                it->_VECTptr->at(ec)=-a;
            }
            if (ev<nv)
                is_slack[ev]=!is_slack[ev];
            continue;
        }
        //swap variables: basic leaves, nonbasic enters
        if (ub_subs) {
            m[lr]._VECTptr->back()-=u[lv];
            if (lv<nv)
                is_slack[lv]=!is_slack[lv];
        }
        pivot_ij(m,lr,ec,ub_subs);
        basis[lr]=ev;
        cols[ec]=lv;
    }
    m=*_eval(m,contextptr)._VECTptr;
    optimum=m[nr][nc];
    bfs=vecteur(nv,gen(0));
    for (int i=0;i<nr;++i) {
        bfs[basis[i]]=m[i][nc];
    }
    for (int j=0;j<nv;++j) {
        if (is_slack[j])
            bfs[j]=u[j]-bfs[j];
    }
    return m;
}

/*
 * Solve the relaxed subproblem corresponding to this node.
 *
 * This function uses two-phase simplex method and applies suitable Gomory
 * mixed integer cuts generated after (each re)optimization. Weak GMI cuts are
 * discarded either because of small away or because not being parallel enough
 * to the objective. Cuts with too large coefficients (when integralized) are
 * discarded too because they slow down the computational process. Generated
 * cuts are kept in the problem structure to be used by child suboproblems
 * during the branch&bound algorithm.
 */
int lp_node::solve_relaxation() {
    int nrows=prob->constr.nrows(),ncols=prob->constr.ncols(),bs;
    matrice m;
    vecteur obj(prob->objective.first),l(ncols),u(ncols),br,row,b,lh,gmi_cut;
    gen rh,obj_ct(0),mgn,minmgn;
    ints cols(ncols),basis;
    vector<bool> is_slack(ncols,false);
    map<int,int> slack_cut;
    bool is_mip=prob->has_integral_variables();
    //determine upper and lower bounds
    for (int j=0;j<ncols;++j) {
        lp_variable &var=prob->variables[j];
        lp_range &range=ranges[j];
        l[j]=max(var.range.lbound,range.lbound,prob->ctx);
        u[j]=min(var.range.ubound,range.ubound,prob->ctx);
        if (is_strictly_greater(l[j],u[j],prob->ctx))
            return _LP_INFEASIBLE;
    }
    //populate matrix with consraint coefficients
    m=*_matrix(makesequence(nrows,ncols+1,0),prob->ctx)._VECTptr;
    for (int i=0;i<nrows;++i) for (int j=0;j<ncols;++j) {
        m[i]._VECTptr->at(j)=prob->constr.lhs[i][j];
    }
    b=prob->constr.rhs;
    //shift variables according to their lower bounds such that l<=x<=u becomes 0<=x'<=u'
    for (int j=0;j<ncols;++j) {
        b=subvecteur(b,multvecteur(l[j],jth_column(m,j)));
        u[j]-=l[j];
        obj_ct+=obj[j]*l[j];
        cols[j]=j;
    }
    for (int i=0;i<nrows;++i) {
        m[i]._VECTptr->at(ncols)=b[i];
    }
    //assure that the right-hand side column has nonnegative coefficients
    for (iterateur it=m.begin();it!=m.end();++it) {
        if (is_strictly_positive(-it->_VECTptr->back(),prob->ctx)) {
            *it=-*it;
        }
    }
    //append cuts inherited from parent node
    for (ints::const_iterator it=cut_indices.begin();it!=cut_indices.end();++it) {
        insert_column(m,vecteur(nrows,gen(0)),-1);
        slack_cut[ncols]=*it;
        prob->cuts.get_lr(*it,lh,rh);
        for (int i=0;i<prob->nv();++i) {
            rh-=lh[i]*l[i];
        }
        row=vecteur(cols.size(),gen(0));
        for (int i=0;i<int(cols.size());++i) {
            int j=cols.at(i);
            if (j<prob->nv())
                row[i]=lh[j];
        }
        row.push_back(gen(-1));
        row.push_back(rh);
        if (is_strictly_positive(-rh,prob->ctx))
            row=multvecteur(gen(-1),row);
        m.push_back(row);
        l.push_back(gen(0));
        u.push_back(inf);
        is_slack.push_back(false);
        obj.push_back(gen(0));
        cols.push_back(ncols++);
        ++nrows;
    }
    //optimize-add cut-reoptimize-add cut...
    //repeat until no more cuts are generated or max_cuts limit is reached
    while (true) {
        br=vecteur(int(cols.size())+1,gen(0));
        bs=basis.size();
        basis.resize(nrows);
        u.resize(ncols+nrows-bs);
        for (int i=bs;i<nrows;++i) {
            br=subvecteur(br,*m[i]._VECTptr);
            basis[i]=ncols+i;
            u[ncols+i-bs]=inf;
        }
        m.push_back(br);
        //phase 1: minimize the sum of artificial variables
        m=simplex_reduce_bounded(m,u,is_slack,solution,optimum,basis,cols,RAND_MAX,prob->ctx);
        if (!is_zero(_simplify(optimum,prob->ctx)))
            return _LP_INFEASIBLE; //at least one artificial variable is basic and positive
        m.pop_back(); //remove bottom row
        for (int i=0;i<nrows;++i) {
            int j=basis[i];
            if (j<ncols)
                continue;
            //ith basic variable is artificial, push it out of the basis
            int k=0;
            for (;k<ncols && (is_zero(m[i][k]) || cols[k]>=ncols);++k);
            if (k==ncols)
                return _LP_ERROR;
            pivot_ij(m,i,k);
            basis[i]=cols[k];
            cols[k]=j;
        }
        //phase 1 finished, remove artificial columns from m
        for (int j=int(cols.size())-1;j>=0;--j) {
            if (cols[j]>=ncols) {
                remove_column(m,j);
                cols.erase(cols.begin()+j);
            }
        }
        //append bottom row to maximize -obj
        br=vecteur(ncols-nrows+1);
        for (int j=0;j<ncols-nrows;++j) {
            int k=cols[j];
            br[j]=obj[k];
            if (is_slack[k]) {
                br.back()-=br[j]*u[k];
                br[j]=-br[j];
            }
        }
        for (int i=0;i<nrows;++i) {
            int j=basis[i];
            if (is_slack[j])
                br.back()-=obj[j]*u[j];
            br=subvecteur(br,multvecteur(is_slack[j]?-obj[j]:obj[j],*m[i]._VECTptr));
        }
        m.push_back(br);
        u.resize(ncols);
        //phase 2: optimize the objective
        m=simplex_reduce_bounded(m,u,is_slack,solution,optimum,basis,cols,
                                 prob->settings.iteration_limit,prob->ctx);
        if (is_inf(optimum))
            return _LP_UNBOUNDED; //solution is unbounded
        m.pop_back(); //remove bottom row
        if (!is_mip) break;
        if (int(cut_indices.size())>=prob->settings.max_cuts)
            break;
        //try to generate Gomory mixed integer cut
        gmi_cut.clear();
        for (int i=0;i<nrows;++i) {
            gen p(fracpart(solution[basis[i]]));
            vecteur eq(*m[i]._VECTptr);
            if (!is_zero(p) && !is_integer(eq.back())) {
                gen f0(fracpart(eq.back())),fj,sp(0),eqnorm(0);
                double away=_evalf(min(f0,gen(1)-f0,prob->ctx),prob->ctx).DOUBLE_val();
                if (away<LP_MIN_AWAY)
                    continue; //too small away, discard this cut
                eq.back()=gen(1);
                for (int k=0;k<int(cols.size());++k) {
                    int j=cols[k];
                    if (j<prob->nv() && prob->variables[j].is_integral) {
                        fj=fracpart(eq[k]);
                        eq[k]=(is_strictly_greater(fj,f0,prob->ctx)?(fj-1)/(f0-1):fj/f0);
                    }
                    else
                        eq[k]=eq[k]/(is_strictly_positive(eq[k],prob->ctx)?f0:(f0-1));
                    if (j<prob->nv()) {
                        sp+=eq[k]*obj[j];
                        eqnorm+=pow(eq[k],2);
                    }
                }
                double dsp=_evalf(sp,prob->ctx).DOUBLE_val(),deqnorm=_evalf(eqnorm,prob->ctx).DOUBLE_val();
                if (std::abs(dsp/(prob->objective_norm*std::sqrt(deqnorm)))<LP_MIN_PARALLELISM)
                    continue; //this cut is not parallel enough to the objective
                eq=integralize(eq,prob->ctx); //turn cut into an equivalent integral representation
                mgn=_max(_abs(eq,prob->ctx),prob->ctx);
                if (_evalf(mgn,prob->ctx).DOUBLE_val()>LP_MAX_MAGNITUDE)
                    continue; //this cut has too large coefficients
                if (gmi_cut.empty() || is_strictly_greater(minmgn,mgn,prob->ctx)) {
                    minmgn=mgn;
                    gmi_cut=eq;
                }
            }
        }
        if (gmi_cut.empty())
            break; //no acceptable cut was generated, so there's no need to reoptimize further
        //store GMI cut
        lh=vecteur(ncols,gen(0));
        rh=gmi_cut.back();
        for (int k=0;k<int(cols.size());++k) {
            int j=cols[k];
            if(!is_zero(lh[j]=gmi_cut[k])) {
                if (is_slack[j]) {
                    rh-=u[j]*lh[j];
                    lh[j]=-lh[j];
                }
                rh+=l[j]*lh[j];
            }
        }
        vecteur slacks(lh.begin()+prob->nv(),lh.end()),orig_lh;
        gen orig_rh;
        lh.resize(prob->nv());
        for (int k=0;k<int(slacks.size());++k) {
            int j=prob->nv()+k;
            prob->cuts.get_lr(slack_cut[j],orig_lh,orig_rh);
            rh+=slacks[k]*orig_rh;
            lh=addvecteur(lh,multvecteur(slacks[k],orig_lh));
        }
        cut_indices.push_back(prob->cuts.nrows());
        prob->cuts.append(lh,rh,_LP_GEQ);
        ++prob->stats.cuts_applied;
        //append GMI cut to simplex tableau and reoptimize
        gmi_cut.insert(gmi_cut.end()-1,gen(-1));
        insert_column(m,vecteur(nrows,gen(0)),-1);
        m.push_back(gmi_cut);
        slack_cut[ncols]=cut_indices.back();
        l.push_back(gen(0));
        u.push_back(inf);
        is_slack.push_back(false);
        obj.push_back(gen(0));
        cols.push_back(ncols++);
        ++nrows;
    }
    for (int i=0;i<prob->nv();++i) {
        solution[i]+=l[i];
    }
    solution.resize(prob->nv());
    optimum=obj_ct-optimum;
    //compute some data useful during branch&bound
    opt_approx=gen2double(optimum,prob->ctx);
    infeas=gen(0);
    most_fractional=-1;
    gen p,ifs,max_ifs(0);
    for (int i=0;i<prob->nv();++i) {
        if (!prob->variables[i].is_integral)
            continue;
        p=fracpart(solution[i]);
        ifs=min(p,gen(1)-p,prob->ctx);
        if (is_zero(ifs))
            continue;
        fractional_vars[i]=gen2double(p,prob->ctx);
        infeas+=ifs;
        if (is_strictly_greater(ifs,max_ifs,prob->ctx)) {
            most_fractional=i;
            max_ifs=ifs;
        }
    }
    return _LP_SOLVED;
}

/*
 * Return fractional part of g, i.e. [g]=g-floor(g). It is always 0<=[g]<1.
 */
gen lp_node::fracpart(const gen &g) {
    return g-_floor(g,prob->ctx);
}

/*
 * Return true iff variable with specified index is fractional.
 */
bool lp_node::is_var_fractional(int index) {
    for (map<int,double>::const_iterator it=fractional_vars.begin();it!=fractional_vars.end();++it) {
        if (it->first==index)
            return true;
    }
    return false;
}

/*
 * Create a child node with copy of ranges and cut indices and depth increased
 * by one.
 */
lp_node lp_node::create_child() {
    lp_node node;
    node.depth=depth+1;
    node.ranges=vector<lp_range>(ranges);
    node.cut_indices=ints(cut_indices);
    node.prob=prob;
    return node;
}

/*
 * Return true iff there are integrality constraints, i.e. iff this is a
 * (mixed) integer problem.
 */
bool lp_problem::has_integral_variables() {
    for (vector<lp_variable>::const_iterator it=variables.begin();it!=variables.end();++it) {
        if (it->is_integral)
            return true;
    }
    return false;
}

/*
 * Return true iff problem has approximate (floating-point) coefficients.
 */
bool lp_problem::has_approx_coefficients() {
    if (is_approx(objective.first) ||
            objective.second.is_approx() ||
            is_approx(constr.lhs) ||
            is_approx(constr.rhs))
        return true;
    for (vector<lp_variable>::const_iterator it=variables.begin();it!=variables.end();++it) {
        if (it->range.lbound.is_approx() || it->range.ubound.is_approx())
            return true;
    }
    return false;
}

/*
 * Set objective function parameters.
 */
void lp_problem::set_objective(const vecteur &v, const gen &ft) {
    objective.first=v;
    objective.second=ft;
    for (const_iterateur it=v.begin();it!=v.end();++it) {
        obj_approx.push_back(gen2double(abs(*it,ctx),ctx));
    }
}

/*
 * Display a message.
 */
void lp_problem::message(const char *msg, bool err) {
    if (err || settings.verbose)
        *logptr(ctx) << msg << "\n";
}

/*
 * Return the jth column (coefficients of the jth variable) as vecteur.
 */
vecteur lp_constraints::column(int index) {
    return jth_column(lhs,index);
}

/*
 * Duplicate the jth column.
 */
void lp_constraints::duplicate_column(int index) {
    assert(index<ncols());
    vecteur col(column(index));
    insert_column(lhs,col,index);
}

/*
 * Multiply the jth column by -1.
 */
void lp_constraints::negate_column(int index) {
    for (int i=0;i<nrows();++i) {
        vecteur &lh=*lhs[i]._VECTptr;
        lh[index]=-lh[index];
    }
}

/*
 * Subtract v from rhs column of constraints.
 */
void lp_constraints::subtract_from_rhs_column(const vecteur &v) {
    assert(int(v.size())==nrows());
    for (int i=0;i<nrows();++i) {
        rhs[i]-=v[i];
    }
}

/*
 * Append constraint "lh rel rh".
 */
void lp_constraints::append(const vecteur &lh, const gen &rh, int relation_type) {
    assert(nrows()==0 || int(lh.size())==ncols());
    lhs.push_back(lh);
    rhs.push_back(rh);
    rv.push_back(relation_type);
}

/*
 * Set the constraint with specified index.
 */
void lp_constraints::set(int index, const vecteur &lh, const gen &rh, int relation_type) {
    assert(index<nrows());
    lhs[index]=lh;
    rhs[index]=rh;
    rv[index]=relation_type;
}

/*
 * Get left and right side of the constraint with specified index.
 */
void lp_constraints::get_lr(int index, vecteur &lh, gen &rh) {
    assert(index<nrows());
    lh=*lhs[index]._VECTptr;
    rh=rhs[index];
}

/*
 * Get the constraint with specified index.
 */
void lp_constraints::get(int index, vecteur &lh, gen &rh, int &relation_type) {
    get_lr(index,lh,rh);
    relation_type=rv[index];
}

/*
 * Divide the constraint by g.
 */
void lp_constraints::div(int index, const gen &g,GIAC_CONTEXT) {
    assert(index<nrows() && !is_zero(g));
    lhs[index]=divvecteur(*lhs[index]._VECTptr,g);
    rhs[index]=rhs[index]/g;
    if (is_strictly_positive(-g,contextptr))
        rv[index]*=-1;
}

/*
 * Subtract vector from the constraint.
 */
void lp_constraints::subtract(int index, const vecteur &v, const gen &g) {
    assert(index<nrows());
    lhs[index]=subvecteur(*lhs[index]._VECTptr,v);
    rhs[index]-=g;
}

/*
 * Remove row with specified index.
 */
void lp_constraints::remove(int index) {
    lhs.erase(lhs.begin()+index);
    rhs.erase(rhs.begin()+index);
    rv.erase(rv.begin()+index);
    if (int(score.size())>index)
        score.erase(score.begin()+index);
}

/*
 * Add identifiers from g to variable_identifiers.
 */
void lp_problem::add_identifiers_from(const gen &g) {
    vecteur vars(*_lname(g,ctx)._VECTptr);
    for (const_iterateur it=vars.begin();it!=vars.end();++it) {
        if (!contains(variable_identifiers,*it))
            variable_identifiers.push_back(*it);
    }
    variable_identifiers=*_sort(variable_identifiers,ctx)._VECTptr;
}

/*
 * Return variable index corresponding to the given identifier.
 */
int lp_problem::get_variable_index(const identificateur &idnt) {
    int n=variable_identifiers.size();
    for (int i=0;i<n;++i) {
        if (*variable_identifiers[i]._IDNTptr==idnt)
            return i;
    }
    return -1;
}

/*
 * Create the problem variables. Assume that they are continuous and
 * unrestricted by default.
 */
void lp_problem::create_variables(int n) {
    variables=vector<lp_variable>(n);
    nvars_initial=n;
    for (int i=0;i<n;++i) {
        lp_variable var;
        var.range.lbound=minf; // default: no restrictions whatsoever
        variables[i]=var;
    }
}

/*
 * Tighten both upper and lower bound of the variable.
 */
void lp_problem::tighten_variable_bounds(int i, const gen &l, const gen &u) {
    lp_variable &var=variables[i];
    var.range.tighten_lbound(l,ctx);
    var.range.tighten_ubound(u,ctx);
}

/*
 * Output solution in form [x1=v1,x2=v2,...,xn=vn] where xk are variable
 * identifiers and vk are solution values.
 */
vecteur lp_problem::output_solution() {
    if (variable_identifiers.empty())
        return solution;
    return *_zip(makesequence(at_equal,variable_identifiers,solution),ctx)._VECTptr;
}

/*
 * Determine coeffcients of linear combination g of variables x in
 * variable_identifiers. varcoeffs C and freecoeff c are filled such that
 * g=C*x+c.
 */
bool lp_problem::lincomb_coeff(const gen &g,vecteur &varcoeffs,gen &freecoeff) {
    gen e(g),a;
    varcoeffs.clear();
    for (const_iterateur it=variable_identifiers.begin();it!=variable_identifiers.end();++it) {
        a=gen(0);
        if (is_constant_wrt(e,*it,ctx) || (is_linear_wrt(e,*it,a,e,ctx) && is_realcons(a,ctx)))
            varcoeffs.push_back(a);
        else return false;
    }
    return is_realcons(freecoeff=e,ctx);
}

/*
 * Add slack variables to the problem if necessary (i.e. convert all
 * inequalities to equalities).
 */
void lp_problem::add_slack_variables() {
    ints posv;
    int nv0=constr.ncols();
    for (int i=0;i<nc();++i) {
        if (constr.rv[i]==_LP_EQ)
            continue;
        append_column(constr.lhs,multvecteur(gen(-constr.rv[i]),singleton(nc(),i)));
        constr.rv[i]=_LP_EQ;
        variables.push_back(lp_variable()); //add slack variable
        posv.push_back(i);
    }
    objective.first.resize(nv());
    //determine types of slack variables
    vecteur lh;
    gen rh;
    for (int k=0;k<int(posv.size());++k) {
        int i=posv[k],j=nv0+k;
        lp_variable &var=variables[j];
        constr.get_lr(i,lh,rh);
        if (is_exact(lh) && is_exact(rh)) {
            gen den(_denom(rh,ctx));
            for (int l=0;l<nv0;++l) {
                if (is_zero(lh[l]))
                    continue;
                if (!variables[l].is_integral) {
                    den=undef;
                    break;
                }
                if (!lh[l].is_integer())
                    den=_lcm(makesequence(den,_denom(lh[l],ctx)),ctx);
            }
            if (!is_undef(den)) {
                lh=multvecteur(den,lh);
                rh=den*rh;
                var.is_integral=true;
            }
        }
    }
}

/*
 * Make all decision variables bounded below by negating variables unrestricted
 * below and replacing variables unrestricted from both above and below with
 * the difference of two nonnegative variables. This process is reversed after
 * an optimal solution is found.
 */
void lp_problem::make_all_vars_bounded_below() {
    for (int i=nv()-1;i>=0;--i) {
        lp_variable &var=variables[i];
        if (var.range.is_unrestricted_below()) {
            if (var.range.is_unrestricted_above()) {
                var.range.lbound=gen(0);
                lp_variable negvar(var);
                var.sign_type=_LP_VARSIGN_POS_PART;
                negvar.sign_type=_LP_VARSIGN_NEG_PART;
                variables.insert(variables.begin()+i,negvar);
                objective.first.insert(objective.first.begin()+i,-objective.first[i]);
                constr.duplicate_column(i);
            }
            else {
                var.range.lbound=-var.range.ubound;
                var.range.ubound=inf;
                var.sign_type=_LP_VARSIGN_NEG;
                objective.first[i]=-objective.first[i];
            }
            constr.negate_column(i);
        }
    }
}

/*
 * Force all problem parameters to be exact.
 */
void lp_problem::make_problem_exact() {
    objective.first=*exact(objective.first,ctx)._VECTptr;
    objective.second=exact(objective.second,ctx);
    constr.lhs=*exact(constr.lhs,ctx)._VECTptr;
    constr.rhs=*exact(constr.rhs,ctx)._VECTptr;
    for (vector<lp_variable>::iterator it=variables.begin();it!=variables.end();++it) {
        it->range.lbound=exact(it->range.lbound,ctx);
        it->range.ubound=exact(it->range.ubound,ctx);
    }
}

/*
 * Report status.
 */
void lp_problem::report_status(const char *msg, int count) {
    char buf[16];
    sprintf(buf,"%d: ",count);
    int nd=numdigits((unsigned)count);
    string str(msg);
    str.insert(0,buf);
    while (nd<8) {
        str.insert(str.begin(),(char)32);
        ++nd;
    }
    message(str.c_str());
}

/*
 * Solve the problem using the specified settings.
 */
int lp_problem::solve() {
    stats=lp_stats();
    char buffer[1024];
    make_problem_exact();
    add_slack_variables();
    make_all_vars_bounded_below();
    objective_norm=_evalf(_l2norm(objective.first,ctx),ctx).DOUBLE_val();
    if (settings.maximize)
        objective.first=-objective.first;
    message("Optimizing...");
    int result;
    optimum=undef;
    double opt_approx;
    lp_node root;
    root.prob=this;
    root.ranges=vector<lp_range>(nv());
    root.depth=0;
    if ((result=root.solve_relaxation())!=_LP_SOLVED)
        return result; //optimal solution does not exist
    if (root.is_integer_feasible()) {
        solution=root.solution;
        optimum=root.optimum;
    }
    if (is_undef(optimum)) {
        message("Applying branch&bound method to find integer feasible solutions...");
        double root_optimum=_evalf(root.optimum,ctx).DOUBLE_val();
        double root_infeas=_evalf(root.infeas,ctx).DOUBLE_val();
        root.ranges.resize(nv());
        vector<lp_node> active_nodes;
        active_nodes.push_back(root);
        clock_t t=clock(),t0=t,now;
        int n,j,k,depth;
        double opt_lbound,fr,max_score;
        bool depth_exceeded=false,incumbent_updated,is_use_pseudocost=false;
        map<double,int> candidates;
        gen lb,ub;
        while (!active_nodes.empty()) {
            if (stats.subproblems_examined>=settings.node_limit) {
                message("Warning: node limit exceeded",true);
                break;
            }
            n=active_nodes.size();
            if (n>stats.max_active_nodes)
                stats.max_active_nodes=n;
            opt_lbound=DBL_MAX;
            k=-1;
            depth=-1;
            for (int i=0;i<n;++i) {
                lp_node &node=active_nodes[i];
                if (opt_lbound>node.opt_approx) {
                    opt_lbound=node.opt_approx;
                    k=i;
                }
                if (settings.nodeselect==_LP_BREADTHFIRST ||
                        (settings.nodeselect==_LP_HYBRID && !is_undef(optimum)) ||
                        node.depth<depth)
                    continue;
                if (node.depth>depth) {
                    candidates.clear();
                    depth=node.depth;
                }
                candidates[node.opt_approx]=i;
            }
            if (settings.nodeselect==_LP_DEPTHFIRST ||
                    (settings.nodeselect==_LP_HYBRID && is_undef(optimum)))
                k=candidates.begin()->second;
            if (settings.nodeselect==_LP_BEST_PROJECTION) {
                double bestproj=DBL_MAX,proj,iopt=is_undef(optimum)?0:_evalf(optimum,ctx).DOUBLE_val();
                for (int i=0;i<n;++i) {
                    lp_node &node=active_nodes[i];
                    proj=_evalf(node.optimum,ctx).DOUBLE_val()+
                            (iopt-root_optimum)*_evalf(node.infeas,ctx).DOUBLE_val()/root_infeas;
                    if (proj<bestproj) {
                        bestproj=proj;
                        k=i;
                    }
                }
            }
            if (k<0) {
                message("Error: node selection strategy failed",true);
                break;
            }
            j=-1;
            if (settings.varselect==_LP_PSEUDOCOST || settings.varselect<0) {
                max_score=0;
                for (int i=0;i<nv();++i) {
                    if (!active_nodes[k].is_var_fractional(i))
                        continue;
                    lp_variable &var=variables[i];
                    double score=var.score(active_nodes[k].fractional_vars[i]);
                    if (score==0) {
                        j=-1;
                        break;
                    }
                    if (score>max_score) {
                        j=i;
                        max_score=score;
                    }
                }
                if (j>=0 && !is_use_pseudocost) {
                    report_status("Switched to pseudocost based branching",stats.subproblems_examined);
                    is_use_pseudocost=true;
                }
            }
            if (j<0) {
                switch (settings.varselect) {
                case -1:
                case _LP_MOSTFRACTIONAL:
                case _LP_PSEUDOCOST:
                    j=active_nodes[k].most_fractional;
                    break;
                case _LP_FIRSTFRACTIONAL:
                    j=active_nodes[k].fractional_vars.begin()->first;
                    break;
                case _LP_LASTFRACTIONAL:
                    j=active_nodes[k].fractional_vars.rbegin()->first;
                    break;
                }
            }
            if (j<0) {
                message("Error: branching variable selection strategy failed",true);
                break;
            }
            if (!is_undef(optimum)) {
                stats.mip_gap=is_zero(optimum)?-opt_lbound:(opt_approx-opt_lbound)/std::abs(opt_approx);
                if (stats.mip_gap<=settings.relative_gap_tolerance) {
                    if (settings.relative_gap_tolerance>0)
                        message("Warning: integrality gap threshold reached",true);
                    break;
                }
            }
            fr=_evalf(active_nodes[k].solution[j],ctx).DOUBLE_val();
            lb=gen(int(std::ceil(fr)));
            ub=gen(int(std::floor(fr)));
            incumbent_updated=false;
            lp_node child_node;
            for (int i=0;i<2;++i) {
                child_node=active_nodes[k].create_child();
                if (child_node.depth>settings.depth_limit) {
                    if (!depth_exceeded) {
                        message ("Warning: depth limit exceeded",true);
                        depth_exceeded=true;
                    }
                    break;
                }
                lp_range &range=child_node.ranges[j];
                if (i==1) {
                    range.tighten_lbound(lb,ctx);
                    if (is_strictly_positive(child_node.ranges[j].lbound,ctx)) {
                        switch (variables[j].sign_type) {
                        case _LP_VARSIGN_POS_PART:
                            child_node.ranges[j-1].ubound=gen(0);
                            break;
                        case _LP_VARSIGN_NEG_PART:
                            child_node.ranges[j+1].ubound=gen(0);
                        }
                    }
                }
                else
                    range.tighten_ubound(ub,ctx);
                ++stats.subproblems_examined;
                if (child_node.solve_relaxation()==_LP_SOLVED) {
                    double p=child_node.fractional_vars[j];
                    variables[j].update_pseudocost(std::abs(child_node.opt_approx-active_nodes[k].opt_approx),
                                                   i==0?p:1-p,i);
                    if (!is_undef(optimum) && is_greater(child_node.optimum,optimum,ctx))
                        continue;
                    if (child_node.is_integer_feasible()) {
                        //new potential incumbent found
                        if (is_undef(optimum) || is_strictly_greater(optimum,child_node.optimum,ctx)) {
                            if (is_undef(optimum))
                                report_status("Incumbent solution found",stats.subproblems_examined);
                            else {
                                sprintf(buffer,"Incumbent solution updated, objective value improvement: %g%%",
                                        (opt_approx-child_node.opt_approx)/std::abs(opt_approx)*100.0);
                                report_status(buffer,stats.subproblems_examined);
                            }
                            solution=child_node.solution;
                            optimum=child_node.optimum;
                            opt_approx=child_node.opt_approx;
                            incumbent_updated=true;
                        }
                    }
                    else
                        active_nodes.push_back(child_node);
                }
            }
            //fathom
            active_nodes.erase(active_nodes.begin()+k);
            if (incumbent_updated) {
                for (int i=int(active_nodes.size())-1;i>=0;--i) {
                    if (is_greater(active_nodes[i].optimum,optimum,ctx))
                        active_nodes.erase(active_nodes.begin()+i);
                }
            }
            now=clock();
            if (1e3*double(now-t0)/CLOCKS_PER_SEC>settings.time_limit) {
                message("Warning: time limit exceeded",true);
                break;
            }
            if (CLOCKS_PER_SEC/double(now-t)<=settings.status_report_freq) { //report status
                sprintf(buffer,"%d nodes active, lower bound: %g",(int)active_nodes.size(),opt_lbound);
                string str(buffer);
                if (stats.mip_gap>=0) {
                    sprintf(buffer,", integrality gap: %g%%",stats.mip_gap*100);
                    str+=string(buffer);
                }
                report_status(str.c_str(),stats.subproblems_examined);
                t=clock();
            }
        }
        //show branch&bound summary
        sprintf(buffer,"Summary:\n * %d subproblem(s) examined\n * max. tree size: %d nodes\n * %d Gomory cut(s) applied",
                stats.subproblems_examined,stats.max_active_nodes,stats.cuts_applied);
        message(buffer);
    }
    if (is_undef(optimum))
        return _LP_INFEASIBLE;
    if (settings.maximize)
        optimum=-optimum;
    optimum+=objective.second;
    for (int i=nv()-1;i>=0;--i) {
        lp_variable &var=variables[i];
        switch (var.sign_type) {
        case _LP_VARSIGN_NEG:
            solution[i]=-solution[i];
            break;
        case _LP_VARSIGN_NEG_PART:
            solution[i+1]-=solution[i];
            solution.erase(solution.begin()+i);
            break;
        }
    }
    solution.resize(nvars_initial);
    return _LP_SOLVED;
}

#ifdef HAVE_LIBGLPK

/*
 * Create GLPK problem from constr.
 */
glp_prob *lp_problem::glpk_initialize() {
    glp_prob *glp=glp_create_prob();
    glp_add_rows(glp,nc());
    glp_add_cols(glp,nv());
    glp_set_obj_dir(glp,settings.maximize?GLP_MAX:GLP_MIN);
    for (int i=0;i<=nv();++i) {
        glp_set_obj_coef(glp,i,gen2double(i==0?objective.second:objective.first[i-1],ctx));
        if (i>0) {
            lp_variable &var=variables[i-1];
            glp_set_col_kind(glp,i,var.is_integral?GLP_IV:GLP_CV);
            int bound_type=GLP_FR;
            if (!var.range.is_unrestricted_below() && var.range.is_unrestricted_above())
                bound_type=GLP_LO;
            else if (var.range.is_unrestricted_below() && !var.range.is_unrestricted_above())
                bound_type=GLP_UP;
            else if (!var.range.is_unrestricted_below() && !var.range.is_unrestricted_above())
                bound_type=GLP_DB;
            else if (is_zero(var.range.ubound-var.range.lbound))
                bound_type=GLP_FX;
            double lo=var.range.is_unrestricted_below()?0.0:gen2double(var.range.lbound,ctx);
            double hi=var.range.is_unrestricted_above()?0.0:gen2double(var.range.ubound,ctx);
            glp_set_col_bnds(glp,i,bound_type,lo,hi);
        }
    }
    int n=constr.nrows()*constr.ncols();
    int *ia=new int[n+1],*ja=new int[n+1]; int k=0;
    double *ar=new double[n+1];
    gen a;
    for (int i=0;i<constr.nrows();++i) {
        for (int j=0;j<constr.ncols();++j) {
            if (is_zero(a=constr.lhs[i][j]))
                continue;
            ++k;
            ia[k]=i+1;
            ja[k]=j+1;
            ar[k]=gen2double(a,ctx);
        }
        double rh=gen2double(constr.rhs[i],ctx);
        switch (constr.rv[i]) {
        case _LP_EQ:
            glp_set_row_bnds(glp,i+1,GLP_FX,rh,0.0);
            break;
        case _LP_LEQ:
            glp_set_row_bnds(glp,i+1,GLP_UP,0.0,rh);
            break;
        case _LP_GEQ:
            glp_set_row_bnds(glp,i+1,GLP_LO,rh,0.0);
            break;
        }
    }
    glp_load_matrix(glp,k,ia,ja,ar);
    delete [] ia;
    delete [] ja;
    delete [] ar;
    return glp;
}

/*
 * Solve LP problem using GLPK implementation of simplex method.
 */
int lp_problem::glpk_simplex(glp_prob *prob) {
    glp_smcp parm;
    glp_init_smcp(&parm);
    parm.msg_lev=settings.verbose?GLP_MSG_ALL:GLP_MSG_ERR;
    parm.it_lim=settings.iteration_limit;
    parm.tm_lim=settings.time_limit;
    parm.presolve=settings.presolve?GLP_ON:GLP_OFF;
    return glp_simplex(prob,&parm);
}

/*
 * Solve LP problem using GLPK implementation of interior point method.
 */
int lp_problem::glpk_interior_point(glp_prob *prob) {
    glp_iptcp parm;
    glp_init_iptcp(&parm);
    parm.msg_lev=settings.verbose?GLP_MSG_ALL:GLP_MSG_ERR;
    return glp_interior(prob,&parm);
}

/*
 * Solve MIP problem using GLPK implementation of branch and cut method.
 */
int lp_problem::glpk_branchcut(glp_prob *prob) {
    glp_iocp parm;
    glp_init_iocp(&parm);
    parm.msg_lev=settings.verbose?GLP_MSG_ALL:GLP_MSG_ERR;
    parm.tm_lim=settings.time_limit;
    parm.out_frq=(int)(1000.0/settings.status_report_freq);
    parm.mip_gap=settings.relative_gap_tolerance;
    parm.gmi_cuts=settings.max_cuts>0?GLP_ON:GLP_OFF;
    parm.mir_cuts=settings.max_cuts>0?GLP_ON:GLP_OFF;
    parm.clq_cuts=settings.has_binary_vars?GLP_ON:GLP_OFF;
    parm.cov_cuts=settings.has_binary_vars?GLP_ON:GLP_OFF;
    parm.presolve=GLP_ON;
    switch (settings.varselect) {
    case _LP_FIRSTFRACTIONAL:
        parm.br_tech=GLP_BR_FFV;
        break;
    case _LP_LASTFRACTIONAL:
        parm.br_tech=GLP_BR_LFV;
        break;
    case _LP_MOSTFRACTIONAL:
        parm.br_tech=GLP_BR_MFV;
        break;
    case _LP_PSEUDOCOST:
        parm.br_tech=GLP_BR_PCH;
        break;
    default:
        parm.br_tech=GLP_BR_DTH;
    }
    switch (settings.nodeselect) {
    case _LP_DEPTHFIRST:
        parm.bt_tech=GLP_BT_DFS;
        break;
    case _LP_BREADTHFIRST:
        parm.bt_tech=GLP_BT_BFS;
        break;
    case _LP_BEST_PROJECTION:
        parm.bt_tech=GLP_BT_BPH;
        break;
    default:
        parm.bt_tech=GLP_BT_BLB;
    }
    return glp_intopt(prob,&parm);
}

#endif

/*
 * Solve the problem using the GLPK library.
 */
int lp_problem::glpk_solve() {
#ifndef HAVE_LIBGLPK
    message("Warning: solving in floating-point arithmetic requires GLPK library",true);
    return solve();
#endif
#ifdef HAVE_LIBGLPK
    glp_prob *prob=glpk_initialize();
    int result,solution_status;
    bool is_mip=has_integral_variables();
    switch (settings.solver) {
    case _LP_SIMPLEX:
        if (is_mip) {
            result=glpk_branchcut(prob);
            if (result==GLP_EMIPGAP)
                result=0;
            if (result==0)
                solution_status=glp_mip_status(prob);
        }
        else if (!is_mip && (result=glpk_simplex(prob))==0)
            solution_status=glp_get_status(prob);
        break;
    case _LP_INTERIOR_POINT:
        if ((result=glpk_interior_point(prob))==0)
            solution_status=glp_ipt_status(prob);
        break;
    }
    if (result==0) { //solving process finished successfully
        switch (solution_status) {
        case GLP_OPT:
            solution_status=_LP_SOLVED;
            break;
        case GLP_FEAS:
            message("Warning: solution is possibly not optimal",true);
            solution_status=_LP_SOLVED;
            break;
        case GLP_INFEAS:
            message("Warning: solution is infeasible",true);
            solution_status=_LP_SOLVED;
            break;
        case GLP_NOFEAS:
            solution_status=_LP_INFEASIBLE;
            break;
        case GLP_UNBND:
            solution_status=_LP_UNBOUNDED;
            break;
        case GLP_UNDEF:
            solution_status=_LP_ERROR;
            break;
        }
        if (solution_status==_LP_SOLVED) { //get solution and optimum
            solution=vecteur(nv());
            switch (settings.solver) {
            case _LP_SIMPLEX:
                if (is_mip) {
                    optimum=gen(glp_mip_obj_val(prob));
                    for (int i=0;i<nv();++i) {
                        solution[i]=gen(glp_mip_col_val(prob,i+1));
                    }
                }
                else {
                    optimum=gen(glp_get_obj_val(prob));
                    for (int i=0;i<nv();++i) {
                        solution[i]=gen(glp_get_col_prim(prob,i+1));
                    }
                }
                break;
            case _LP_INTERIOR_POINT:
                optimum=gen(glp_ipt_obj_val(prob));
                for (int i=0;i<nv();++i) {
                    solution[i]=gen(glp_ipt_col_prim(prob,i+1));
                }
                break;
            }
        }
    }
    glp_delete_prob(prob);
    if (result==0)
        return solution_status;
    return _LP_ERROR;
#endif
}

/*
 * Load problem from file in LP or (possibly gzipped) MPS format. File types
 * are distinguished by examining the filename extension. If it is .gz or .mps,
 * it is assumed that the file is in (gzipped) MPS format. Else, it is assumed
 * that file is in LP format.
 */
bool lp_problem::glpk_load_from_file(const char *fname) {
#ifndef HAVE_LIBGLPK
    message("Error: loading CPLEX and MPS files requires GLPK library",true);
    return false;
#endif
#ifdef HAVE_LIBGLPK
    glp_prob *prob=glp_create_prob();
    int result;
    string str(fname);
    string ext(str.substr(str.find_last_of(".") + 1));
    if (ext=="mps" || ext=="gz") //MPS format (modern)
        result=glp_read_mps(prob,GLP_MPS_FILE,NULL,fname);
    else //LP format
        result=glp_read_lp(prob,NULL,fname);
    if (result==0) { //successfully loaded
        char buffer[1024];
        int obj_dir=glp_get_obj_dir(prob),len,t,k;
        int nr=glp_get_num_rows(prob),n=glp_get_num_cols(prob);
        if (nr*n>LP_CONSTR_MAXSIZE) {
            message("Error: constraint matrix is too big to be loaded. Aborting...",true);
            return false;
        }
        int *ind=new int[1+n];
        double *val=new double[1+n];
        create_variables(n);
        constr.lhs=*_matrix(makesequence(gen(nr),gen(nv())),ctx)._VECTptr;
        constr.rhs=vecteur(nr);
        constr.rv=ints(nr);
        variable_identifiers=vecteur(nv());
        settings.maximize=(obj_dir==GLP_MAX);
        objective.second=double2fraction(glp_get_obj_coef(prob,0),ctx);
        objective.first=vecteur(nv());
        obj_approx=vector<double>(nv());
        for (int j=0;j<nv();++j) {
            objective.first[j]=double2fraction(glp_get_obj_coef(prob,j+1),ctx);
            obj_approx[j]=std::abs(glp_get_obj_coef(prob,j+1));
            lp_variable &var=variables[j];
            t=glp_get_col_type(prob,j+1);
            k=glp_get_col_kind(prob,j+1);
            var.name=string(glp_get_col_name(prob,j+1));
            variable_identifiers[j]=identificateur(var.name);
            if (t!=GLP_UP && t!=GLP_FR)
                var.range.lbound=double2fraction(glp_get_col_lb(prob,j+1),ctx);
            if (t!=GLP_LO && t!=GLP_FR)
                var.range.ubound=double2fraction(glp_get_col_ub(prob,j+1),ctx);
            if (k!=GLP_CV)
                var.is_integral=true;
            if (k==GLP_BV) {
                var.range.tighten_lbound(gen(0),ctx);
                var.range.tighten_ubound(gen(1),ctx);
            }
        }
        for (int i=nr;i>0;--i) {
            if ((len=glp_get_mat_row(prob,i,ind,val))>0) {
                vecteur &row=*constr.lhs[i-1]._VECTptr;
                for (int j=1;j<=len;++j) {
                    row[ind[j]-1]=double2fraction(val[j],ctx);
                }
            }
            t=glp_get_row_type(prob,i);
            if (t==GLP_FR) {
                sprintf(buffer,"Warning: auxiliary variable bounds not set, discarding constraint %d",i);
                message(buffer,true);
                constr.remove(i-1);
                continue;
            }
            if (t==GLP_FX) {
                constr.rhs[i-1]=double2fraction(glp_get_row_lb(prob,i),ctx);
                constr.rv[i-1]=_LP_EQ;
            }
            else {
                if (t==GLP_LO || t==GLP_DB) {
                    constr.rhs[i-1]=double2fraction(glp_get_row_lb(prob,i),ctx);
                    constr.rv[i-1]=_LP_GEQ;
                }
                if (t==GLP_DB)
                    constr.append(*constr.lhs[i-1]._VECTptr,double2fraction(glp_get_row_ub(prob,i),ctx),_LP_LEQ);
                if (t==GLP_UP || t==GLP_DB) {
                    constr.rhs[i-1]=double2fraction(glp_get_row_ub(prob,i),ctx);
                    constr.rv[i-1]=_LP_LEQ;
                }
            }
        }
        delete [] ind;
        delete [] val;
        stringstream ss;
        const char* name=glp_get_prob_name(prob);
        if (name==NULL)
            ss << "a problem";
        else
            ss << "problem \"" << name << "\"";
        sprintf(buffer,"Successfully loaded %s with %d variables and %d constraints",
                ss.str().c_str(),nv(),nc());
        message(buffer);
    }
    glp_delete_prob(prob);
    return !(bool)result;
#endif
}

void lp_variable::set_type(int t,GIAC_CONTEXT) {
    switch (t) {
    case _LP_BINARYVARIABLES:
        range.tighten_lbound(gen(0),contextptr);
        range.tighten_ubound(gen(1),contextptr);
    case _LP_INTEGERVARIABLES:
        is_integral=true;
        break;
    }
}

bool assign_variable_types(const gen &g,int t,lp_problem &prob) {
    pair<gen,gen> range;
    int i,i0=array_start(prob.ctx);
    if (g.type==_VECT) {
        for (const_iterateur it=g._VECTptr->begin();it!=g._VECTptr->end();++it) {
            if (!assign_variable_types(*it,t,prob))
                return false;
        }
    }
    else if ((g.type==_IDNT && (i=prob.get_variable_index(*g._IDNTptr))>=0) ||
             (g.is_integer() && (i=g.val-i0)>=0))
        prob.variables[i].set_type(t,prob.ctx);
    else if (interval2pair(g,range,prob.ctx) &&
             range.first.is_integer() && range.second.is_integer()) {
        for (i=range.first.val;i<=range.second.val;++i) {
            prob.variables[i-i0].set_type(t,prob.ctx);
        }
    }
    else
        return false;
    return true;
}

void parse_limit(const gen &g,int &lim,GIAC_CONTEXT) {
    if (is_positive(g,contextptr)) {
        if (g.is_integer())
            lim=g.val;
        else if (is_inf(g))
            lim=RAND_MAX;
    }
}

bool parse_options_and_bounds(const_iterateur &it,const_iterateur &itend,lp_problem &prob) {
    for (;it!=itend;++it) {
        if (*it==at_maximize)
            prob.settings.maximize=true;
        else if (it->is_integer()) {
            switch(it->val) {
            case _LP_MAXIMIZE:
                prob.settings.maximize=true;
                break;
            case _LP_VERBOSE:
                prob.settings.verbose=true;
                break;
            }
        }
        else if (it->is_symb_of_sommet(at_equal)) {
            //parse the argument in form "option=value"
            pair<gen,gen> bounds;
            gen a,lh(_lhs(*it,prob.ctx)),rh(_rhs(*it,prob.ctx));
            if (lh.type==_IDNT && interval2pair(rh,bounds,prob.ctx)) {
                int vi=prob.get_variable_index(*lh._IDNTptr);
                if (vi>=0)
                    prob.tighten_variable_bounds(vi,bounds.first,bounds.second);
            }
            else if (lh==at_maximize && rh.is_integer())
                prob.settings.maximize=(bool)rh.to_int();
            else if (lh==at_assume && rh.is_integer())
                prob.settings.assumption=rh.val;
            else if (lh.is_integer()) {
                switch(lh.val) {
                case _LP_INTEGERVARIABLES:
                case _LP_BINARYVARIABLES:
                    if (!assign_variable_types(rh,lh.val,prob))
                        return false;
                    break;
                case _LP_DEPTHLIMIT:
                    parse_limit(rh,prob.settings.depth_limit,prob.ctx);
                    break;
                case _LP_NODE_LIMIT:
                    parse_limit(rh,prob.settings.node_limit,prob.ctx);
                    break;
                case _LP_ITERATION_LIMIT:
                    parse_limit(rh,prob.settings.iteration_limit,prob.ctx);
                    break;
                case _LP_MAX_CUTS:
                    parse_limit(rh,prob.settings.max_cuts,prob.ctx);
                    break;
                case _LP_TIME_LIMIT:
                    if (is_strictly_positive(rh,prob.ctx)) {
                        if (is_inf(rh))
                            prob.settings.time_limit=RAND_MAX;
                        a=_evalf(rh,prob.ctx);
                        if (a.type==_DOUBLE_)
                            prob.settings.time_limit=int(a.DOUBLE_val()*1000.0);
                    }
                    break;
                case _LP_NODESELECT:
                    if (rh.is_integer())
                        prob.settings.nodeselect=rh.val;
                    break;
                case _LP_VARSELECT:
                    if (rh.is_integer())
                        prob.settings.varselect=rh.val;
                    break;
                case _LP_METHOD:
                    if (rh==at_exact)
                        prob.settings.precision=_LP_EXACT;
                    else if (rh==at_float)
                        prob.settings.precision=_LP_INEXACT;
                    else if (rh.is_integer() && rh.val==_LP_INTERIOR_POINT) {
                        prob.settings.precision=_LP_INEXACT;
                        prob.settings.solver=_LP_INTERIOR_POINT;
                    }
                    break;
                case _LP_GAP_TOLERANCE:
                    a=_evalf(rh,prob.ctx);
                    if (a.type==_DOUBLE_ && is_strictly_positive(a,prob.ctx))
                        prob.settings.relative_gap_tolerance=a.DOUBLE_val();
                    break;
                case _LP_MAXIMIZE:
                    if (rh.is_integer())
                        prob.settings.maximize=(bool)rh.to_int();
                    break;
                case _LP_VERBOSE:
                    if (rh.is_integer())
                        prob.settings.verbose=(bool)rh.to_int();
                    break;
                }
            }
        }
    }
    return true;
}

int parse_constraints(bool is_matrix_form,const_iterateur &it,const_iterateur &itend,lp_problem &prob) {
    vecteur cv;
    if (is_matrix_form) {
        if (it->type!=_VECT)
            return _LP_ERR_TYPE;
        cv=*it->_VECTptr;
        int n=cv.size();
        if ((n % 2)!=0)
            return _LP_ERR_SIZE;
        if (n>0) {
            //constraints are given in form [A,b,Aeq,beq] such that
            //A.x<=b and/or Aeq.x=beq (Aeq and beq are optional)
            if (cv[0].type!=_VECT || cv[1].type!=_VECT)
                return _LP_ERR_TYPE;
            vecteur &A=*cv[0]._VECTptr,&b=*cv[1]._VECTptr;
            int len;
            if ((len=A.size())!=int(b.size()))
                return _LP_ERR_DIM;
            for (int i=0;i<len;++i) {
                prob.constr.append(*A[i]._VECTptr,b[i],_LP_LEQ);
            }
            if (n>2) { //there are equality constraints
                if (cv[2].type!=_VECT || cv[3].type!=_VECT)
                    return _LP_ERR_TYPE;
                vecteur &Aeq=*cv[2]._VECTptr,&beq=*cv[3]._VECTptr;
                if ((len=Aeq.size())!=int(beq.size()))
                    return _LP_ERR_DIM;
                for (int i=0;i<len;++i) {
                    prob.constr.append(*Aeq[i]._VECTptr,beq[i],_LP_EQ);
                }
            }
            if (!ckmatrix(prob.constr.lhs))
                return _LP_ERR_DIM;
            if (prob.nv()==0)
                prob.create_variables(prob.constr.ncols());
            else if (prob.nv()!=prob.constr.ncols())
                return _LP_ERR_DIM;
        }
        //if the third arg is [bl,bu] then assume bl<=x<=bu
        ++it;
        if (it!=itend && it->type==_VECT) {
            matrice &bounds=*it->_VECTptr;
            if (!ckmatrix(bounds) || int(bounds.size())!=2 ||
                    int(bounds.front()._VECTptr->size())<prob.nv())
                return _LP_ERR_DIM;
            if (prob.nv()==0)
                prob.create_variables(bounds.front()._VECTptr->size());
            for (int i=0;i<prob.nv();++i) {
                prob.tighten_variable_bounds(i,bounds[0][i],bounds[1][i]);
            }
            ++it;
        }
        if (prob.nv()==0)
            return _LP_ERR_SIZE;
        if (prob.objective.first.empty())
            prob.set_objective(vecteur(prob.nv(),gen(0)),gen(0));
    }
    else { //the problem is given in symbolic form
        if (it->type==_VECT) {
            cv=*it->_VECTptr;
            prob.add_identifiers_from(cv);
            ++it;
        }
        prob.create_variables(prob.variable_identifiers.size());
        vecteur sides,c;
        pair<gen,gen> bounds;
        gen r;
        int rel;
        for (const_iterateur jt=cv.begin();jt!=cv.end();++jt) {
            if (jt->is_symb_of_sommet(at_equal))
                rel=_LP_EQ;
            else if (jt->is_symb_of_sommet(at_inferieur_egal))
                rel=_LP_LEQ;
            else if (jt->is_symb_of_sommet(at_superieur_egal))
                rel=_LP_GEQ;
            else
                return _LP_ERR_TYPE;
            sides=*jt->_SYMBptr->feuille._VECTptr;
            if (jt->is_symb_of_sommet(at_equal) && interval2pair(sides.back(),bounds,prob.ctx)) {
                if (!prob.lincomb_coeff(sides.front(),c,r))
                    return _LP_ERR_TYPE;
                prob.constr.append(c,bounds.first-r,_LP_GEQ);
                prob.constr.append(c,bounds.second-r,_LP_LEQ);
                continue;
            }
            if (!prob.lincomb_coeff(sides.front()-sides.back(),c,r))
                return _LP_ERR_TYPE;
            prob.constr.append(c,-r,rel);
        }
    }
    if (!parse_options_and_bounds(it,itend,prob))
        return _LP_ERR_TYPE;
    return 0;
}

gen _lpsolve(const gen &args,GIAC_CONTEXT) {
    gen g(args);
    if (g.type==_STRNG)
        g=makesequence(g);
    if (g.type!=_VECT || g.subtype!=_SEQ__VECT)
        return gensizeerr(contextptr);
    vecteur &gv=*g._VECTptr,obj;
    const_iterateur it=gv.begin(),itend=gv.end();
    lp_problem prob(contextptr); //create LP problem with default settings
    bool is_matrix_form=false;
    if (it->type==_STRNG) { //problem is given in file
        int len=_size(*it,contextptr).val;
        string fname(it->_STRNGptr->begin(),it->_STRNGptr->begin()+len);
        if (!prob.glpk_load_from_file(fname.c_str()))
            return undef;
        ++it;
        if (it->type==_VECT)
            return gentypeerr(contextptr);
    }
    else { //problem is entered manually
        is_matrix_form=it->type==_VECT;
        if (is_matrix_form) {
            obj=*it->_VECTptr;
            if (!obj.empty()) {
                prob.set_objective(obj,gen(0));
                prob.create_variables(obj.size());
            }
        }
        else {
            prob.add_identifiers_from(*it);
            gen obj_ct;
            if (!prob.lincomb_coeff(*it,obj,obj_ct))
                return _LP_ERR_TYPE;
            prob.set_objective(obj,obj_ct);
        }
        ++it;
    }
    switch (parse_constraints(is_matrix_form,it,itend,prob)) {
    case _LP_ERR_SIZE:
        return gensizeerr(contextptr);
    case _LP_ERR_TYPE:
        return gentypeerr(contextptr);
    case _LP_ERR_DIM:
        return gendimerr(contextptr);
    default:
        if (prob.nc()==0) {
            for (int i=0;i<prob.nv();++i) {
                lp_variable &var=prob.variables[i];
                vecteur row(prob.nv(),gen(0));
                if (!is_inf(var.range.lbound)) {
                    row[i]=gen(1);
                    prob.constr.append(row,var.range.lbound,_LP_GEQ);
                    break;
                }
                else if (!is_inf(var.range.ubound)) {
                    row[i]=gen(1);
                    prob.constr.append(row,var.range.ubound,_LP_LEQ);
                    break;
                }
            }
            if (prob.nc()==0) {
                prob.message("Error: no constraints detected",true);
                return gensizeerr(contextptr);
            }
        }
    }
    for (int i=0;i<prob.nv();++i) {
        lp_variable &var=prob.variables[i];
        switch (prob.settings.assumption) {
        case _ZINT:
        case _LP_INTEGER:
            var.set_type(_LP_INTEGERVARIABLES,contextptr);
            break;
        case _LP_BINARY:
            var.set_type(_LP_BINARYVARIABLES,contextptr);
            break;
        case _NONNEGINT:
        case _LP_NONNEGINT:
            var.set_type(_LP_INTEGERVARIABLES,contextptr);
        case _LP_NONNEGATIVE:
            var.range.tighten_lbound(gen(0),contextptr);
            break;
        }
    }
    bool is_solver_exact;
    switch (prob.settings.precision) {
    case _LP_EXACT:
        is_solver_exact=true;
        break;
    case _LP_INEXACT:
        is_solver_exact=false;
        break;
    case _LP_PROB_DEPENDENT:
        is_solver_exact=!prob.has_approx_coefficients();
        break;
    }
    vector<lp_variable>::const_iterator vt=prob.variables.begin();
    for (;vt!=prob.variables.end();++vt) {
        if (vt->is_integral && is_zero(vt->range.lbound) && is_one(vt->range.ubound)) {
            prob.settings.has_binary_vars=true;
            break;
        }
    }
    //solve the problem
    switch (is_solver_exact?prob.solve():prob.glpk_solve()) {
    case _LP_INFEASIBLE:
        prob.message("Error: problem has no feasible solutions",true);
        return vecteur(0);
    case _LP_UNBOUNDED:
        prob.message("Error: problem has unbounded solution",true);
        return makevecteur(prob.settings.maximize?inf:minf,vecteur(0));
    case _LP_ERROR:
        return undef;
    default: //_LP_SOLVED
        return gen(makevecteur(_eval(prob.optimum,contextptr),_eval(prob.output_solution(),contextptr)),_LIST__VECT);
    }
}
static const char _lpsolve_s[]="lpsolve";
static define_unary_function_eval (__lpsolve,&_lpsolve,_lpsolve_s);
define_unary_function_ptr5(at_lpsolve,alias_at_lpsolve,&__lpsolve,0,true)

#ifndef NO_NAMESPACE_GIAC
}
#endif //ndef NO_NAMESPACE_GIAC
