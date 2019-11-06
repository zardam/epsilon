SUBDIRS = tutoriel casinter
DIST_SUBDIRS = tutoriel casinter
info_TEXINFOS = giac_el.texinfo
AM_MAKEINFOHTMLFLAGS = --no-split

TEX_LOCAL = cas_el.tex cascmd_el.tex
PDF_LOCAL = cas_el.pdf
PS_LOCAL = cas_el.ps
DVI_LOCAL = cas_el.dvi
HTML_DIRS_LOCAL = cascmd_el
HTML_STAMPS = cas_el.stamp cascmd_el.stamp

EXTRA_DIST = $(TEX_LOCAL) $(PDF_LOCAL) $(HTML_LOCAL) $(HTML_STAMPS) $(HTML_DIRS_LOCAL)/* cutting.hva cutting.sty


the_LANG = el
thelangdocdir = $(docdir)/$(the_LANG)
dist_thelangdoc_DATA = html_mall html_mtt html_vall xcasmenu xcasex keywords 

html-local: $(HTML_STAMPS)
pdf-local: $(PDF_LOCAL)
dvi-local: $(DVI_LOCAL)
ps-local: $(PS_LOCAL)

$(srcdir)/cascmd_el: cascmd_el.stamp

install-data-local: $(HTML_STAMPS) $(PDF_LOCAL) $(PS_LOCAL) $(DVI_LOCAL)
	$(install_sh) -d $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
#	$(INSTALL_DATA) $(PDF_LOCAL) $(DESTDIR)/$(the_LANG)
#	$(INSTALL_DATA) $(PS_LOCAL) $(DESTDIR)/$(the_LANG)
#	$(INSTALL_DATA) $(HTML_LOCAL) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
	for dd in $(HTML_DIRS_LOCAL); do \
		$(install_sh) -d $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)/$$dd ; \
	done
	for dd in $(HTML_DIRS_LOCAL); do \
		$(INSTALL_DATA) $(srcdir)/$$dd/* $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)/$$dd ; \
	done
#	$(INSTALL_DATA) $(DVI_LOCAL) $(DESTDIR)/$(the_LANG)
	$(INSTALL_DATA) $(dist_thelangdoc_DATA) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)

LATEX2HTML_HOMEPAGE = "http://www-fourier.ujf-grenoble.fr/~parisse/giac.html"
LATEX2HTML_ADDRESS = <A HREF=$(LATEX2HTML_HOMEPAGE)>giac</A> documentation written by
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

.tex.stamp:
#	$(RUN_LATEX2HTML) $< --address '$(GIAC_ADDRESS)'
	$(RUN_HEVEA) $< 
	tdir=`echo $< | sed -e 's/\.tex//'`; \
	imagen -png -pdf -mag 2000 $$tdir; \
	$(install_sh) -d $$tdir ; \
	hacha $$tdir.html -o $$tdir/index.html ; \
	touch $$tdir.png ; \
	/bin/cp -f $$tdir*.png $$tdir
#	rm -f $$tdir/*.old $$tdir/*.pl $$tdir/images.??? $$tdir/WARNINGS
	touch $@

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

