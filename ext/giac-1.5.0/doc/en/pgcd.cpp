// -*- mode:C++ ; compile-command: "g++ -I.. -fPIC -DPIC -g -c pgcd.cpp -o pgcd.lo && ln -sf pgcd.lo pgcd.o && gcc -shared pgcd.lo -lc -lgiac -Wl,-soname -Wl,libpgcd.so.0 -o libpgcd.so.0.0.0 && ln -sf libpgcd.so.0.0.0 libpgcd.so.0 && ln -sf libpgcd.so.0.0.0 libpgcd.so" -*-
using namespace std;
#include <stdexcept>
#include <cmath>
#include <cstdlib>
#include <giac/config.h>
#include <giac/giac.h>
//#include "pgcd.h"

#ifndef NO_NAMESPACE_GIAC
namespace giac {
#endif // ndef NO_NAMESPACE_GIAC

  gen monpgcd(const gen & a0,const gen & b0){
    gen q,r,a=a0,b=b0;
    for (;b!=0;){
      r=irem(a,b,q);
      a=b;
      b=r;
    }
    return a;
  }
  gen _monpgcd(const gen & args,GIAC_CONTEXT){
    if ( (args.type!=_VECT) || (args._VECTptr->size()!=2))
      setsizeerr();
    vecteur &v=*args._VECTptr;
    return monpgcd(v[0],v[1]);
  }
  const string _monpgcd_s("monpgcd");
  unary_function_eval __monpgcd(0,&_monpgcd,_monpgcd_s);
  unary_function_ptr at_monpgcd (&__monpgcd,0,true);
  

#ifndef NO_NAMESPACE_GIAC
} // namespace giac
#endif // ndef NO_NAMESPACE_GIAC
