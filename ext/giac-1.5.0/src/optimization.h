#ifndef __OPTIMIZATION_H
#define __OPTIMIZATION_H
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "first.h"
#include "gen.h"
#include "unary.h"

#ifndef NO_NAMESPACE_GIAC
namespace giac {
#endif // ndef NO_NAMESPACE_GIAC

enum critical_point_classification {
    _CPCLASS_UNDECIDED=0,
    _CPCLASS_MIN=1,
    _CPCLASS_MAX=2,
    _CPCLASS_POSSIBLE_MIN=3,
    _CPCLASS_POSSIBLE_MAX=4,
    _CPCLASS_SADDLE=5
};

enum kernel_density_estimation_method {
    _KDE_METHOD_EXACT,
    _KDE_METHOD_PIECEWISE,
    _KDE_METHOD_LIST
};

enum bandwidth_selection_method {
    _KDE_BW_METHOD_DPI,
    _KDE_BW_METHOD_ROT
};

enum bvp_output_type {
    _BVP_LIST,
    _BVP_DIFF,
    _BVP_PIECEWISE,
    _BVP_SPLINE
};

class ipdiff {
    /* IPDIFF CLASS (Implicit Partial DIFFerentiation)
     * This class is used for implicit differentiation of f with respect to g=0.
     * Here, f:R^(n+m)->R, g:R^(n+m)->R^m with rank(g')=m and J(x)=f(x,h(x)) where x in R^n and h is
     * the implicit function guaranteed to exist (in a neighborhood of some point x0) by the
     * Implicit Function Theorem if rank(g')=m. Function J may now be differentiated wrt x1,x2,...,xn. */
public:
    typedef std::vector<int> ivector;
    typedef ivector::const_iterator ivector_iter;
    typedef std::vector<ivector> ivectors;
    typedef std::map<ivector,int> ivector_map;
    typedef std::pair<ivector,ivector_map> diffterm;
    typedef std::map<diffterm,int> diffterms;
    typedef std::map<ivector,gen> pd_map;
private:
    vecteur vars; // list of variables [x1,x2,..,xn,y1,y2,..,ym] where x are free and y are dependent
    gen f; // the expression f(x1,x2,..,xn,y1,y2,..,ym)
    vecteur g; // list [g1,g2,..,gm], it is assumed that gj(x1,x2,..,xn,y1,y2,..,ym)=0 for all j=1,2,..,m
    const context *ctx; // giac context
    pd_map pdv; // partial derivatives of J
    pd_map pdf; // partial derivatives of f
    pd_map pdg; // partial derivatives of g
    pd_map pdh; // partial derivatives of h
    std::map<ivector,diffterms> cterms;
    int ord; // the maximal order of the currently computed partial derivatives of h
    int nvars; // the number of free variables
    int nconstr; // the number of constraints = the number of dependent variables
    diffterms derive_diffterms(const diffterms &terms,ivector &sig);
    void find_nearest_terms(const ivector &sig,diffterms &match,ivector &excess);
    const gen &differentiate(const gen &e,pd_map &pds,const ivector &sig);
    void compute_h(const std::vector<diffterms> &grv,int order);
    void compute_pd(int order,const ivector &sig=ivector(0));
    void raise_order(int order); // compute the missing partial derivatives of h
    const gen &get_pd(const pd_map &pds,const ivector &sig) const;
public:
    /* construct the ipdiff instance for function f_orig(vars_orig) subject to
     * the constraints in g_orig, the Jacobian of g_orig must have maximal rank */
    ipdiff(const gen &f_orig,const vecteur &g_orig,const vecteur &vars_orig,GIAC_CONTEXT);
    /* return the partial derivative of J represented by sig */
    const gen &derivative(const ivector &sig);
    /* return the partial derivative of J with respect to the variables in dvars */
    const gen &derivative(const vecteur &dvars);
    /* store all partial derivatives of the specified order to pd_map */
    void partial_derivatives(int order,pd_map &pdmap);
    /* return 1/k!*(sum(i=1)^n (xi-ai)*d/dxi)^k f(x)|a */
    gen taylor_term(const vecteur &a,int k,bool shift=true);
    /* return the Taylor polynomial, with the specified order, of J in the vicinity of a */
    gen taylor(const vecteur &a,int order);
    /* store grad(J) as vector res */
    void gradient(vecteur &res);
    /* store Hessian(J) as matrix res */
    void hessian(matrice &res);
    /* generate all partitions of m into n terms and store them to vector c */
    static void ipartition(int m,int n,ivectors &c,const ivector &p=ivector(0));
    /* return the sum the elements of a vector v of integers, ignore the last element if drop_last=true */
    static int sum_ivector(const ivector &v,bool drop_last=false);
};

class tprob {
    /* TPROB CLASS (Transportation PROBlem)
     * The class implementing the MODI method for balanced TP with degeneracy handling */
public:
    typedef std::pair<int,int> ipair;
    typedef std::vector<ipair> ipairs;
private:
    const context *ctx;
    vecteur demand;
    vecteur supply;
    gen eps; // epsilon
    gen M; // symbol for marking forbidden routes in cost matrix
    void north_west_corner(matrice &feas);
    ipairs stepping_stone_path(ipairs &path_orig,const matrice &X);
    void modi(const matrice &P_orig,matrice &X);
public:
    /* construct the TP with supply s and demand d, m marks forbidden routes */
    tprob(const vecteur &s,const vecteur &d,const gen &m,GIAC_CONTEXT);
    /* solve the transportation problem with the given cost matrix, output in sol */
    void solve(const matrice &cost_matrix,matrice &sol);
};

gen _implicitdiff(const gen &g,GIAC_CONTEXT);
gen _minimize(const gen &g,GIAC_CONTEXT);
gen _maximize(const gen &g,GIAC_CONTEXT);
gen _extrema(const gen &g,GIAC_CONTEXT);
gen _minimax(const gen &g,GIAC_CONTEXT);
gen _tpsolve(const gen &g,GIAC_CONTEXT);
gen _nlpsolve(const gen &g,GIAC_CONTEXT);
gen _thiele(const gen &g,GIAC_CONTEXT);
gen _triginterp(const gen &g,GIAC_CONTEXT);
gen _kernel_density(const gen &g,GIAC_CONTEXT);
gen _fitdistr(const gen &g,GIAC_CONTEXT);
gen _bvpsolve(const gen &g,GIAC_CONTEXT);
gen _euler_lagrange(const gen &g,GIAC_CONTEXT);
gen _jacobi_equation(const gen &g,GIAC_CONTEXT);
gen _conjugate_equation(const gen &g,GIAC_CONTEXT);
gen _convex(const gen &g,GIAC_CONTEXT);

extern const unary_function_ptr * const at_implicitdiff;
extern const unary_function_ptr * const at_minimize;
extern const unary_function_ptr * const at_maximize;
extern const unary_function_ptr * const at_extrema;
extern const unary_function_ptr * const at_minimax;
extern const unary_function_ptr * const at_tpsolve;
extern const unary_function_ptr * const at_nlpsolve;
extern const unary_function_ptr * const at_thiele;
extern const unary_function_ptr * const at_triginterp;
extern const unary_function_ptr * const at_kernel_density;
extern const unary_function_ptr * const at_fitdistr;
extern const unary_function_ptr * const at_bvpsolve;
extern const unary_function_ptr * const at_euler_lagrange;
extern const unary_function_ptr * const at_convex;

#ifndef NO_NAMESPACE_GIAC
} // namespace giac
#endif // ndef NO_NAMESPACE_GIAC
#endif // __OPTIMIZATION_H
