#include "script_template.h"

namespace Khicas {

constexpr ScriptTemplate emptyScriptTemplate(".py", "\x01" R"(from math import *
)");

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
constexpr ScriptTemplate squaresScriptTemplate("squares.py", "\x01" R"(def squares(n):
  for j in range(n):
    print(j)
)");
#endif
constexpr ScriptTemplate mandelbrotScriptTemplate("mandelbrot.py", "\x01" R"(# This script draws a Mandelbrot fractal set
# N_iteration: degree of precision
import kandinsky
def mandelbrot(N_iteration):
  for x in range(320):
    for y in range(222):
# Compute the mandelbrot sequence for the point c = (c_r, c_i) with start value z = (z_r, z_i)
      z = complex(0,0)
# Rescale to fit the drawing screen 320x222
      c = complex(3.5*x/319-2.5, -2.5*y/221+1.25)
      i = 0
      while (i < N_iteration) and abs(z) < 2:
        i = i + 1
        z = z*z+c
# Choose the color of the dot from the Mandelbrot sequence
      rgb = int(255*i/N_iteration)
      col = kandinsky.color(int(rgb),int(rgb*0.75),int(rgb*0.25))
# Draw a pixel colored in 'col' at position (x,y)
      kandinsky.set_pixel(x,y,col))");

constexpr ScriptTemplate polynomialScriptTemplate("polynomial.py", "\x01" R"(from math import *
# rac(a,b,c) calcule les racines de a*x**2+b*x+c=0
def rac(a,b,c):
  delta = b*b-4*a*c
  if delta == 0:
    return -b/(2*a)
  elif delta > 0:
    x_1 = (-b-sqrt(delta))/(2*a)
    x_2 = (-b+sqrt(delta))/(2*a)
    return x_1, x_2
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
