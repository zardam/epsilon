info_TEXINFOS = giac_es.texinfo
AM_MAKEINFOHTMLFLAGS = --no-split

TEX_LOCAL = cas_es.tex
PDF_LOCAL = cas_es.pdf
PS_LOCAL = cas_es.ps
DVI_LOCAL = cas_es.dvi

EXTRA_DIST = cas_es.tex $(PDF_LOCAL) $(PS_LOCAL) $(DVI_LOCAL) # $(HTML_LOCAL) $(HTML_STAMPS) $(HTML_DIRS_LOCAL)

the_LANG = es
thelangdocdir = $(docdir)/$(the_LANG)
dist_thelangdoc_DATA = html_mall html_mtt html_vall xcasmenu xcasex keywords 

pdf-local: $(PDF_LOCAL)
dvi-local: $(DVI_LOCAL)
ps-local: $(PS_LOCAL)

install-data-local: $(HTML_STAMPS) $(PDF_LOCAL) $(PS_LOCAL) $(DVI_LOCAL)
	$(install_sh) -d $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
#	$(INSTALL_DATA) $(PDF_LOCAL) $(DESTDIR)/$(the_LANG)
#	$(INSTALL_DATA) $(PS_LOCAL) $(DESTDIR)/$(the_LANG)
#	$(INSTALL_DATA) $(DVI_LOCAL) $(DESTDIR)/$(the_LANG)
#	$(INSTALL_DATA) $(HTML_LOCAL) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
	$(INSTALL_DATA) $(dist_thelangdoc_DATA) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)

LATEX2HTML_HOMEPAGE = "http://www-fourier.ujf-grenoble.fr/~parisse/giac.html"
LATEX2HTML_ADDRESS = <A HREF=$(LATEX2HTML_HOMEPAGE)>giac</A> documentation written by
LATEX2HTML_OPTIONS = -local_icons -no_math -html_version 3.2,math

SUFFIXES = .tex .pdf .ps .dvi

if DOC_PROGS

RUN_PDFLATEX = TEXINPUTS=$(srcdir):$$TEXINPUTS $(PDFLATEX)
RUN_LATEX = TEXINPUTS=$(srcdir):$$TEXINPUTS $(LATEX)
RUN_MAKEINDEX = TEXINPUTS=$(srcdir):$$TEXINPUTS $(MAKEINDEX)
RUN_LATEX2HTML = TEXINPUTS=$(srcdir):$$TEXINPUTS $(LATEX2HTML) $(LATEX2HTML_OPTIONS)

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

dist-hook:
	echo '*** Error: no tools to rebuild documentation is installed' ; \
	exit 1

endif

