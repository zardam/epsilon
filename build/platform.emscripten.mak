TOOLCHAIN = emscripten
USE_LIBA = 0
ION_KEYBOARD_LAYOUT = layout_B2
EXE = js
EPSILON_GETOPT = 1

# giac
SFLAGS += -DGIAC_NUMWORKS
LDFLAGS += -O3 -v -s TOTAL_MEMORY=1GB -s LEGACY_GL_EMULATION=1 -s GL_UNSAFE_OPTS=0 --memory-init-file 0 -s ASSERTIONS=1 -s ERROR_ON_UNDEFINED_SYMBOLS=0 -L/home/parisse/emgiac/giac  -lgiac -lpari -lmpfi -lmpfr -lgmp -lglpk --js-library /home/parisse/emgiac/giac/time.js
