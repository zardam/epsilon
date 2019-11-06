TEX_LOCAL = cascmd_en.tex casinter.tex cas.tex slipres.tex giacxcas.tex tutoriel.tex

PDF_LOCAL = cascmd_en.pdf casinter.pdf cas.pdf troussesurvie_en.pdf

PS_LOCAL = cascmd_en.ps casinter.ps cas.ps

DVI_LOCAL = cascmd_en.dvi casinter.dvi cas.dvi

HTML_DIRS_LOCAL = casinter cascmd_en tutoriel

HTML_STAMPS = casinter.stamp cascmd_en.stamp tutoriel

info_TEXINFOS = giac_us.texinfo
AM_MAKEINFOHTMLFLAGS = --no-split

EXTRA_DIST = $(TEX_LOCAL) $(PDF_LOCAL) $(DVI_LOCAL) $(PS_LOCAL) \
	     $(HTML_DIRS_LOCAL) $(HTML_LOCAL) $(HTML_STAMPS) \
	     pgcd.cc pgcd.cpp giac_bassin.cpp .gdbinit cutting.* \
	     x*.png sound*.png kde*.png rand*.png fit*.png catenoid*.png signal*.png hevea.sty 

GIAC_HOMEPAGE = "http://www-fourier.ujf-grenoble.fr/~parisse/giac.html"
GIAC_ADDRESS = <A HREF=$(LATEX2HTML_HOMEPAGE)>giac</A> documentation written

the_LANG = en
thelangdocdir = $(docdir)/$(the_LANG)
dist_thelangdoc_DATA = html_mall html_mtt html_vall xcasmenu xcasex keywords 

html-local: $(HTML_STAMPS) 
pdf-local: $(PDF_LOCAL)
dvi-local: $(DVI_LOCAL)
ps-local: $(PS_LOCAL)

$(srcdir)/cascmd_en: cascmd_en.stamp
$(srcdir)/casinter: casinter.stamp
$(srcdir)/tutoriel: tutoriel.stamp

install-data-local: $(HTML_STAMPS) $(PS_LOCAL) $(DVI_LOCAL) troussesurvie_en.pdf # $(PDF_LOCAL) 
	$(install_sh) -d $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
#	$(INSTALL_DATA) $(PDF_LOCAL) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
	$(INSTALL_DATA) troussesurvie_en.pdf $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
	$(INSTALL_DATA) $(PS_LOCAL) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
#	$(INSTALL_DATA) $(DVI_LOCAL) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
#	$(INSTALL_DATA) $(HTML_LOCAL) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
	for dd in $(HTML_DIRS_LOCAL); do \
		$(install_sh) -d $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)/$$dd ; \
	done
	for dd in $(HTML_DIRS_LOCAL); do \
		$(INSTALL_DATA) $(srcdir)/$$dd/* $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)/$$dd ; \
	done
#	export LD_LIBRARY_PATH=$(DESTDIR)/lib ; $(DESTDIR)/bin/icas --rebuild-help-cache
	$(INSTALL_DATA) $(dist_thelangdoc_DATA) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)


LATEX2HTML_OPTIONS = -local_icons -no_math -html_version 3.2,math
HEVEA_OPTIONS = -fix

SUFFIXES = .tex .pdf .ps .dvi

if DOC_PROGS

RUN_PDFLATEX = TEXINPUTS=$(srcdir):$$TEXINPUTS $(PDFLATEX)
RUN_LATEX = TEXINPUTS=$(srcdir):$$TEXINPUTS $(LATEX)
RUN_MAKEINDEX = TEXINPUTS=$(srcdir):$$TEXINPUTS $(MAKEINDEX)
RUN_LATEX2HTML = TEXINPUTS=$(srcdir):$$TEXINPUTS $(LATEX2HTML) $(LATEX2HTML_OPTIONS)
RUN_HEVEA = TEXINPUTS=$(srcdir):$$TEXINPUTS hevea $(HEVEA_OPTIONS)

.tex.pdf:
	$(RUN_PDFLATEX) $<
	$(RUN_PDFLATEX) $<
	if $(EGREP) -q -e '\\makeindex' $<; then \
		$(RUN_MAKEINDEX) `echo $@ | sed -e s/\.pdf/\.idx/` ; \
	fi
	$(RUN_PDFLATEX) $<

.tex.dvi:
	$(RUN_LATEX) $<
	$(RUN_LATEX) $<
	if $(EGREP) -q -e '\\makeindex' $<; then \
		$(RUN_MAKEINDEX) `echo $@ | sed -e s/\.dvi/\.idx/` ; \
	fi
	$(RUN_LATEX) $<

.dvi.ps:
	$(DVIPS) -o $@ $<

casinter.stamp: casinter.tex
	$(RUN_HEVEA) $< 
	tdir=`echo $< | sed -e 's/\.tex//'`; \
	$(install_sh) -d $$tdir ; \
	hacha $$tdir.html -o $$tdir/index.html ; \
	touch $$tdir.png ; \
	/bin/cp -f $$tdir*.png $$tdir
#	rm -f $$tdir/*.old $$tdir/*.pl $$tdir/images.??? $$tdir/WARNINGS
	touch $@	

cascmd_en.stamp: cascmd_en.tex
	$(RUN_HEVEA) $< 
	tdir=`echo $< | sed -e 's/\.tex//'`; \
	$(install_sh) -d $$tdir ; \
	hacha $$tdir.html -o $$tdir/index.html ; \
	touch $$tdir.png ; \
	/bin/cp -f $$tdir*.png $$tdir
#	rm -f $$tdir/*.old $$tdir/*.pl $$tdir/images.??? $$tdir/WARNINGS
	touch $@

tutoriel.stamp: tutoriel.tex
	$(RUN_HEVEA) $< 
	tdir=`echo $< | sed -e 's/\.tex//'`; \
	$(install_sh) -d $$tdir ; \
	hacha $$tdir.html -o $$tdir/index.html ; \
	touch $$tdir.png ; \
	/bin/cp -f $$tdir*.png $$tdir
#	rm -f $$tdir/*.old $$tdir/*.pl $$tdir/images.??? $$tdir/WARNINGS
	touch $@

dist-hook:
	for ddir in $(HTML_DIRS_LOCAL); do $(MAKE) $$ddir.stamp; done
else

.tex.pdf:
	if [ ! -f "$@" ]; then \
		echo '*** Error: TeX is not installed, and $@ is missing' ; \
		exit 1 ; \
	fi

.tex.dvi:
	if [ ! -f "$@" ]; then \
		echo '*** Error: TeX is not installed, and $@ is missing' ; \
		exit 1 ; \
	fi

.tex.ps:
	if [ ! -f "$@" ]; then \
		echo '*** Error: TeX is not installed, and $@ is missing' ; \
		exit 1 ; \
	fi

.tex.stamp:
	docdir=`echo $< | sed -e 's/\.tex//'` ; \
	if [ ! -f "$$docdir/index.html" ]; then \
		echo '*** Error: hevea is not installed, and $$docdir is missing' ; \
		exit 1 ; \
	fi

dist-hook:
	echo '*** Error: no tools to rebuild documentation is installed' ; \
	exit 1

endif

