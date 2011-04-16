TARGET=libsyndicate

dist_noinst_DATA=$(srcdir)/$(TARGET).tex 

SUBDIRS=picts

PICTS=$(shell grep '{picts/' $(srcdir)/$(TARGET).tex | cut -d '}' -f1 | cut -d '{' -f2 | sed -e 's/picts\///')
PICTS_EPS=$(patsubst %, %.eps, $(PICTS))
PICTS_PDF=$(patsubst %, %.pdf, $(PICTS))

if BUILD_DOC_LATEX_HTML
  HTML=$(TARGET).html
endif

if BUILD_DOC_LATEX_PDF
  PDF=$(TARGET).pdf
endif

all-local : $(HTML) $(PDF)

html-local : $(HTML)

pdf-local : $(PDF)

dvi-local : $(TARGET).dvi

$(PICTS_EPS) $(PICTS_PDF) :
	make -C picts $@

%.html : $(srcdir)/%.tex $(PICTS_EPS)
	$(HEVEA) -fix png.hva -O $<

%.pdf : $(srcdir)/%.tex $(PICTS_PDF)
	$(PDFLATEX) '\nonstopmode\input{$<}' 1> /dev/null 
	$(PDFLATEX) '\nonstopmode\input{$<}' 1> /dev/null
	$(PDFLATEX) '\nonstopmode\input{$<}' 1> /dev/null

%.dvi : $(srcdir)/%.tex $(PICTS_EPS)
	$(LATEX) '\nonstopmode\input{$<}' 1> /dev/null
	$(LATEX) '\nonstopmode\input{$<}' 1> /dev/null
	$(LATEX) '\nonstopmode\input{$<}' 1> /dev/null

clean-local :
	-rm -f *.aux *.toc *.log *.out *.pdf *.dvi *.bbl *.blg
	-rm -f *.haux *.htoc *.image.tex *.png *.html 

maintainer-clean-local :
	-rm -f *~


