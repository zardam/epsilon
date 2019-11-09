#include "script_template.h"

namespace Code {

#if 1
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
#if 1
constexpr ScriptTemplate squaresScriptTemplate("squares.py", "\x01" R"(from turtle import *
def polyg1(l=20,n=3):
  for j in range(n):
    forward(l)
    left(360.0/n)

def polyg(n=7):
  reset()
  for j in range(3,n):
    polyg1(30,j)
)");
#else
constexpr ScriptTemplate squaresScriptTemplate("squares.py", "\x01" R"()");
#endif
#endif

#if 0
constexpr ScriptTemplate mandelbrotScriptTemplate("mandelbrot.py", "\x01" R"(from math import *
from kandinsky import *
# This script draws a Mandelbrot fractal set
# N_iteration: degree of precision
def mandelbrot(X=160,Y=111,Nmax=10):
  w=2.7/X
  h=-1.87/Y
  Y=Y-1
  for y in range(ceil(Y/2)+1):
    c = complex(-2.1,h*y+0.935)
    for x in range(X):
      z = 0
      for j in range(Nmax):
        z=z**2+c
        if abs(z)>2:
          break
      set_pixel(x,y,126*j+2079) 
      set_pixel(x,Y-y,126*j+2079) 
      c = c+w)");
#else
constexpr ScriptTemplate mandelbrotScriptTemplate("mandelbrot.py", "\x01" R"(from math import *
from kandinsky import *
# Mandelbrot fractal
# Nmax: precision, s: scale
def mandelbrot(Nmax=10,s=2,X=160,Y=111):
  w=2.7/X
  h=-1.87/Y
  Y=Y-1
  for y in range(ceil(Y/2)+1):
    c = complex(-2.1,h*y+0.935)
    for x in range(X):
      z = 0
      for j in range(Nmax):
        z=z**2+c
        if abs(z)>2:
          break
      fill_rect(s*x,s*y,s,s,126*j+2079)
      fill_rect(s*x,s*(Y-y),s,s,126*j+2079)
      c = c+w)");
#endif
  
constexpr ScriptTemplate polynomialScriptTemplate("polynomial.py", "\x01" R"(from math import *
# rac(a,b,c) calcule les racines de a*x**2+b*x+c=0
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
