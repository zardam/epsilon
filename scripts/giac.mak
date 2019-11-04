giac-1.5.0/src/libgiac.a:# libtommath-0.39/libtommath.a
	make -C giac-1.5.0/src/ -f Makefile.numworks

libtommath-0.39/libtommath.a:
	make -C libtommath-0.39 -f Makefile.n110
