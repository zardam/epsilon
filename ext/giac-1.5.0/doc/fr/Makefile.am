TEX_LOCAL = castor.tex casrouge.tex cascmd_fr.tex casgeo.tex casexo.tex \
	    cascas.tex cassim.tex permutation.tex tutoriel.tex \
	    casinter.tex algo.tex fltkcol.tex

PDF_LOCAL = algo.pdf troussesurvie_fr.pdf # casrouge.pdf castor.pdf cascmd_fr.pdf casgeo.pdf tutoriel.pdf casinter.pdf cascas.pdf casexo.pdf cassim.pdf 

PS_LOCAL = casrouge.ps castor.ps cascmd_fr.ps casgeo.ps cassim.ps \
	   tutoriel.ps casinter.ps casexo.ps cascas.ps

DVI_LOCAL = casrouge.dvi castor.dvi cascmd_fr.dvi casgeo.dvi cassim.dvi \
	    tutoriel.dvi casinter.dvi casexo.dvi cascas.dvi

HTML_DIRS_LOCAL = casrouge castor cascmd_fr casgeo cassim tutoriel \
		  casinter casexo cascas

HTML_STAMPS = casrouge.stamp castor.stamp cascmd_fr.stamp casgeo.stamp \
	      cassim.stamp tutoriel.stamp casinter.stamp casexo.stamp \
	      cascas.stamp

HTML_LOCAL = algo.html algo*png

FIGURES = figure1 figure2 figure3 figure4 figure5 th1968* windowinter* undemix* cesaro* baton* camembert* plotlist*  nuage* nuee* quadrill* etoile*  inscrit* sphere* nico* suite* trialea* factalea* codrot* codage* pasde2.* carr.* exoparam.* dessincomb.*  fougere* tirage* xex02* xexyey* triequi* Newtonimpl* chouchou* chouxfleur* demarr1* diffsuite* graph* triangl*.* carre*.* spiral*.* morley*.* pantalon.* airemax*.* trajetmin*.* napo*.* pytha*.* conj*.* moustache.* pain*.* parsim.* expole*.* mini*.* hexag*.* logoxas.* dragon.* sphinx*.* bouquet.* arbre*.* fleur.* sapin*.* gosper*.* hilbert*.* poly*.* floc*.* carrt.* peano*.* tor*eps tor*png ecran2* mobius* parabole* fibon* uniforme* cartexcas.* cube.* trio* goutte* biscuit* cercle* baceinst* exo*d* exomean* sierpinsk* newsq* newlog* bac?or* trace* casjohn* cassym* casetoi* deca* vasa* pa2b2* alga2b2* ellipse* conicfd* tangent* writergb* siglece* ciss* trisect* *folium* approx* visu* lohseg* tort* poletoil* chat* point* bezout* rect* fichgraph* tablefonc* pyramide* buffon* simul* hexagone* pont3* peauce* desolve* picarre* sinusoide* lunules* penta1* ujf* pli* doderhomb* patron* triacon* cyclohex* gluc* illusion* damier* trap* rota* simi* syracuse* trisbatab* ecartype* ecartbi* tetra* pavage* airearc* bassin* equagraph* systgraph* mondrian* delaunay* trois* castrois* factorise* algo*png stat* pappus* pussle* mab* bissect* quasi* csarre* puzzle* minamb* courbure* image* lieu* equi* barycentre* partager*

EXTRA_DIST = $(TEX_LOCAL) $(HTML_LOCAL) $(HTML_STAMPS) \
	     $(HTML_DIRS_LOCAL)/* cutting.hva cutting.sty $(FIGURES) \
		$(PDF_LOCAL) 

GIAC_HOMEPAGE = "http://www-fourier.ujf-grenoble.fr/~parisse/giac_fr.html"
GIAC_ADDRESS = Documentation de <A HREF=$(GIAC_HOMEPAGE)>giac</A> &eacute;crite par Ren&eacute;e De Graeve

the_LANG = fr
thelangdocdir = $(docdir)/$(the_LANG)
dist_thelangdoc_DATA = html_mall html_mtt html_vall xcasmenu xcasex keywords

giacfr.tex: ../giacfr.tex
	cp "$<" "$@"

# install xvfb-run and set this to "xvfb-run" to run "headless" (without a real display)
#XVFB_RUN =
#algo.pdf: algo.tex giacfr.tex ../../src/icas
#   $(XVFB_RUN) ../../src/icas "$<" --pdf

html-local: $(HTML_STAMPS)
pdf-local: $(PDF_LOCAL)
dvi-local: $(DVI_LOCAL)
ps-local: $(PS_LOCAL)

$(srcdir)/casrouge: casrouge.stamp
$(srcdir)/castor: castor.stamp
$(srcdir)/cascmd_fr: cascmd_fr.stamp
$(srcdir)/casgeo: casgeo.stamp
$(srcdir)/cassim: cassim.stamp
$(srcdir)/tutoriel: tutoriel.stamp
$(srcdir)/casinter: casinter.stamp
$(srcdir)/casexo: casexo.stamp
$(srcdir)/cascas: cascas.stamp

all-data-local: $(HTML_STAMPS) $(PDF_LOCAL) 

install-data-local: 
	$(install_sh) -d $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
	$(INSTALL_DATA) $(PDF_LOCAL) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
#	$(INSTALL_DATA) $(PS_LOCAL) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
#	$(INSTALL_DATA) $(DVI_LOCAL) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
	$(INSTALL_DATA) $(HTML_LOCAL) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)
	for dd in $(HTML_DIRS_LOCAL); do \
		$(install_sh) -d $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)/$$dd ; \
	done
	for dd in $(HTML_DIRS_LOCAL); do \
		$(INSTALL_DATA) $(srcdir)/$$dd/* $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)/$$dd ; \
	done
#	export LD_LIBRARY_PATH=$(DESTDIR)/lib ; $(DESTDIR)/bin/icas --rebuild-help-cache
	$(INSTALL_DATA) $(dist_thelangdoc_DATA) $(DESTDIR)$(prefix)/share/giac/doc/$(the_LANG)

HEVEA_OPTIONS = -fix 
# faire hevea -fix cascmd_fr ; cd cascmd_fr ; hacha ../cascmd_fr.html 
# verifier la presence de indexenv dans index.html
LATEX2HTML_OPTIONS = -local_icons -no_math -html_version 3.2,math 

SUFFIXES = .stamp .tex .pdf .ps .dvi

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
		$(RUN_MAKEINDEX) `echo $@ | sed -e s/\.dvi/\.idx/` ; \
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
		echo '*** Error: latex2html is not installed, and $$docdir is missing' ; \
		exit 1 ; \
	fi

dist-hook:
	echo '*** Error: no tools to rebuild documentation is installed' ; \
	exit 1

endif

install-exec-hook: 
	mkdir -p $(DESTDIR)$(prefix)/share/giac/doc/fr/
	ln -sf ../giac.js $(DESTDIR)$(prefix)/share/giac/doc/fr/giac.js
