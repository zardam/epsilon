// -*- mode:c++; compile-command:" c++ -I.. -I. -fPIC -DPIC -g -c giac_Mandelbrot.cpp -o giac_Mandelbrot.lo && cc -shared giac_Mandelbrot.lo -lgiac -lc -Wl,-soname -Wl,libgiac_Mandelbrotso.0 -o libgiac_Mandelbrot.so.0.0.0 && ln -sf libgiac_Mandelbrot.so.0.0.0 libgiac_Mandelbrot.so" -*-
#include <giac/config.h>
#include <giac/giac.h>
using namespace std;
namespace giac {
  giac::gen Mandelbrot (double xmin_d, double xmax_d, double ymin_d, double ymax_d, long N_i, giac::gen coul, const giac::context * contextptr = 0) {
    complex < double >z_c;
    double hx_d;
    double hy_d;
    long j_i;
    long k_i;
    long l_i;
    giac::vecteur res_v;
    complex < double >c_c;
    res_v = cpp_convert_7 (giac::_range (giac::_pow (giac::makesequence (N_i + 1, 2), contextptr), contextptr), contextptr);
    hx_d = (xmax_d + (-xmin_d)) / N_i;
    hy_d = (ymax_d + (-ymin_d)) / N_i;
    for (j_i = 0; j_i <= N_i; j_i = j_i + 1) {
      for (k_i = 0; k_i <= N_i; k_i = k_i + 1) {
	c_c = complex<double> (xmin_d + j_i * hx_d , (ymin_d + k_i * hy_d));
	z_c = 0;
	for (l_i = 0; l_i <= 50; l_i = l_i + 1) {
	  z_c = z_c * z_c + c_c;
	  if (std::abs (z_c) > 100)
	  {
	    res_v[(N_i + 1) * j_i + k_i] = symb_pnt(symbolic(giac::at_pixon ,giac::makesequence (j_i, k_i, l_i + coul)), 0,contextptr);
	    break;;
	  };;
	}
	;
	if (l_i == 51) {
	  res_v[(N_i + 1) * j_i + k_i] = symb_pnt(symbolic(giac::at_pixon ,giac::makesequence (j_i, k_i,0)), 0,contextptr);
	};;
      }
      ;
    }
    ;
    return merge_pixon(res_v);
  }
  giac::gen _Mandelbrot (const giac::gen & g, const giac::context * contextptr) {
    if (g.type != _VECT || g.subtype != _SEQ__VECT || g._VECTptr->size () != 6)
      return gendimerr (contextptr);
    vecteur v = *g._VECTptr;
    v[0] = evalf_double (v[0], 1, contextptr);
    if (v[0].type != _DOUBLE_)
      return gensizeerr (contextptr);
    v[1] = evalf_double (v[1], 1, contextptr);
    if (v[1].type != _DOUBLE_)
      return gensizeerr (contextptr);
    v[2] = evalf_double (v[2], 1, contextptr);
    if (v[2].type != _DOUBLE_)
      return gensizeerr (contextptr);
    v[3] = evalf_double (v[3], 1, contextptr);
    if (v[3].type != _DOUBLE_)
      return gensizeerr (contextptr);
    if (!is_integral (v[4]))
      return gensizeerr (contextptr);
    return Mandelbrot (v[0]._DOUBLE_val, v[1]._DOUBLE_val, v[2]._DOUBLE_val, v[3]._DOUBLE_val, v[4].val, v[5], contextptr);
  }

  const string _Mandelbrot_s ("Mandelbrot");
  unary_function_eval __Mandelbrot (0, &_Mandelbrot, _Mandelbrot_s);
  unary_function_ptr at_Mandelbrot (&__Mandelbrot, 0, true);
}
