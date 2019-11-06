// -*- compile-command: "g++ -g pgcd.cc -lgiac -lgmp" -*-
#include <giac/config.h>
#include <giac/giac.h>

using namespace std;
using namespace giac;

gen pgcd(gen a,gen b){
  gen q,r;
  for (;b!=0;){
    r=irem(a,b,q);
    a=b;
    b=r;
  }
  return a;
}

int main(){
  cout << "Enter 2 integers ";
  gen a,b;
  cin >> a >> b;
  cout << pgcd(a,b) << endl;
  return 0;
}
