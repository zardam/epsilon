#include <os.h>
#include <lauxlib.h>
#include "giac.h"
#include "luabridge.h"

using namespace giac;
//const char std::endl='\n';
//nio::console * std::console_cin_ptr=0;
using namespace std;

const char * giac_caseval(const char * s){
  vx_var=identificateur("x");
  //static nio::console console_cin;
  //console_cin_ptr=&console_cin;
  giac::debug_infolevel=2;
  const char * res=giac::caseval(s);
  return res;
}

