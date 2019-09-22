#include "script_template.h"

namespace Khicas {

#if 0
constexpr ScriptTemplate emptyScriptTemplate(".py", "\x01" R"(from math import *
)");
#else
constexpr ScriptTemplate emptyScriptTemplate(".py", "\x01" R"()");
#endif

#if 0
constexpr ScriptTemplate squaresScriptTemplate("squares.py", "\x01" R"(
def squares(angle=0.5):
  reset()
  L=330
  speed(10)
  penup()
  goto(-L/2,-L/2)
  pendown()
  for i in range(660):
    forward(L)
    left(90+angle)
    L=L-L*sin(angle*pi/180)
  hideturtle())");
#else
constexpr ScriptTemplate squaresScriptTemplate("squares.py", "\x01" R"(def s(n=333333):
  plotfield(sin(t*y),t,y,[[0,1]])
#  for j in range(n):
#    print(j)
)");
#endif
constexpr ScriptTemplate mandelbrotScriptTemplate("mandelbrot.py", "\x01" R"(# This script draws a Mandelbrot fractal set
# N_iteration: degree of precision
def mandelbrot(X=160,Y=111,Nmax=10):
  w=2.7/X
  h=-1.87/Y
  Y=Y-1
  for y in range(ceiling(Y/2)+1):
    c = -2.1+i*(h*y+0.935)
    for x in range(X):
      z = 0
      for j in range(Nmax):
        if abs(z:=z**2+c)>2:
          break
      draw_rectangle(2*x,2*y,2,2,126*j+1073743903)
      draw_rectangle(2*x,2*(Y-y),2,2,126*j+1073743903)
      c = c+w)");

constexpr ScriptTemplate polynomialScriptTemplate("polynomial.py", "\x01" R"(# rac(a,b,c) calcule les racines de a*x**2+b*x+c=0
def rac(a,b,c):
  delta = b*b-4*a*c
  if delta == 0:
    return -b/(2*a)
  elif delta > 0:
    x_1 = (-b-sqrt(delta))/(2*a)
    x_2 = (-b+sqrt(delta))/(2*a)
    return [x_1, x_2]
  else:
    return None)");

const ScriptTemplate * ScriptTemplate::Empty() {
  return &emptyScriptTemplate;
}

const ScriptTemplate * ScriptTemplate::Squares() {
  return &squaresScriptTemplate;
}

const ScriptTemplate * ScriptTemplate::Mandelbrot() {
  return &mandelbrotScriptTemplate;
}

const ScriptTemplate * ScriptTemplate::Polynomial() {
  return &polynomialScriptTemplate;
}

}
