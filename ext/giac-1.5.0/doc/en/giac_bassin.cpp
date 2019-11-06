// -*- mode:c++; compile-command:"c++ -I.. -I. -fPIC -DPIC -g -c giac_bassin.cpp -o giac_bassin.lo && cc -shared giac_bassin.lo -lgiac -lc -Wl,-soname -Wl,libgiac_bassinso.0 -o libgiac_bassin.so.0.0.0 && ln -sf libgiac_bassin.so.0.0.0 libgiac_bassin.so.0 && ln -sf libgiac_bassin.so.0.0.0 libgiac_bassin.so" -*-
#include <giac/config.h>
#include <giac/giac.h>
using namespace std;
namespace giac {
  std::complex<double> horner_newton(const vecteur & p,const std::complex<double> &x,GIAC_CONTEXT); // x-p(x)/p'(x)
  giac::gen bassin (giac::gen P, giac::gen xmin, giac::gen xmax, giac::gen ymin, giac::gen ymax, giac::gen Ng, giac::gen maxiterg, const giac::context * contextptr = 0) {
    long N_i;
    long maxiter_i;
    complex < double >z_c,z;
    double hx_d;
    double hy_d;
    long j_i;
    long k_i;
    long l_i,n;
    giac::vecteur r_v;
    giac::vecteur res_v;
    giac::gen P1;
    N_i = cpp_convert_2 (Ng, contextptr);
    maxiter_i = cpp_convert_2 (maxiterg, contextptr);
    P1 = giac::_diff (P, contextptr);
    P = giac::_symb2poly (P, contextptr);
    P1 = giac::_symb2poly (P1, contextptr);
    res_v = cpp_convert_7 (giac::_makelist ((N_i + 1) * (N_i + 1), contextptr), contextptr);
    r_v = cpp_convert_7 (giac::_proot (P, contextptr), contextptr);
    vector< complex<double> > r(r_v.size());
    for (int i=0;i<r_v.size();++i){
      r[i]=cpp_convert_4(r_v[i],contextptr);
    }
    hx_d = cpp_convert_1 (giac::_division (giac::makesequence (xmax + (-xmin), N_i), contextptr), contextptr);
    hy_d = cpp_convert_1 (giac::_division (giac::makesequence (ymax + (-ymin), N_i), contextptr), contextptr);
    for (j_i = 0; j_i <= N_i; j_i = j_i + 1) {
      for (k_i = 0; k_i <= N_i; k_i = k_i + 1) {
	z_c = cpp_convert_4 (giac::_evalf (xmin + j_i * hx_d + gen (0, 1) * (ymin + k_i * hy_d), contextptr), contextptr);
	for (l_i = 0; l_i <= maxiter_i; l_i = l_i + 1) {
	  z=z_c;
	  z_c=horner_newton(*P._VECTptr,z_c,contextptr);
	  if (std::abs(z_c)>100 || std::abs(z_c-z)<1e-6)
	    break;
	}
	;
	if (z_c == undef) {
	  continue;
	};
	for (n = 0; n < r.size(); n++) {
	  if (std::abs(z_c-r[n])<1e-4)
	    break;
	}
	res_v[(N_i + 1) * j_i + k_i] = symbolic(at_pixon,makesequence (j_i, k_i, 256+25*n+l_i));
      }
      ;
    }

    return res_v;
  }
  giac::gen _bassin (const giac::gen & g, const giac::context * contextptr) {
    if (g.type != _VECT || g.subtype != _SEQ__VECT || g._VECTptr->size () != 7)
      return gendimerr (contextptr);
    vecteur v = *g._VECTptr;
    return bassin (v[0], v[1], v[2], v[3], v[4], v[5], v[6], contextptr);
  }

  const string _bassin_s ("bassin");
  unary_function_eval __bassin (0, &_bassin, _bassin_s);
  unary_function_ptr at_bassin (&__bassin, 0, true);
}
