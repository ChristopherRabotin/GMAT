# Makefile for the GMAT User Guide and Training Manual.
# On Windows, requires an MSYS environment.
# This file generally follows the rules of the GNU Makefile Conventions

# portability boilerplate
SHELL = /bin/sh
.SUFFIXES:

# directories
srcdir = ./src
configdir = ./config
contribdir = ../build/contrib

# tools
FOP = $(JAVA) -Dfop.home=$(contribdir)/fop $(JAVAFLAGS) -jar $(contribdir)/fop/build/fop.jar -c $(configdir)/fop.xconf
JAVA = java
JING = $(JAVA) $(ALL_JAVAFLAGS) -jar $(contribdir)/jing/bin/jing.jar
XALAN = $(JAVA) -cp $(contribdir)/xalan-j/serializer.jar:$(contribdir)/xalan-j/xalan.jar:$(contribdir)/xalan-j/xercesImpl.jar:$(contribdir)/xml-apis.jar:$(contribdir)/docbook-xsl-ns/extensions/xalan27.jar $(ALL_JAVAFLAGS) org.apache.xalan.xslt.Process

# flags
ALL_JAVAFLAGS = -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=org.apache.xerces.parsers.XIncludeParserConfiguration $(JAVAFLAGS)
ALL_XALANFLAGS = -PARAM use.extensions 1 \
	  -PARAM fop1.extensions 1 \
	  -PARAM graphicsize.extension 1 \
	  $(XALANFLAGS)

# source files
helpsrc = $(srcdir)/help.xml
trainingsrc = $(srcdir)/training.xml
srcfiles = $(srcdir)/*.xml
fostyle = ./xform/fo.xsl

# Rules
all: help training

clean:
	rm -rf *.chm *.fo *.html *.pdf chm files html

distclean: clean
mostlyclean: clean
maintainer-clean: clean

validate:
	$(JING) $(contribdir)/docbook/docbookxi.rng $(helpsrc) $(trainingsrc)

dist: all

help: help-pdf \
    help-html-single \
    help-html-multiple \
    help-chm

training: training-pdf

help-pdf: help-pdf-letter \
    help-pdf-a4

training-pdf: training-pdf-letter \
    training-pdf-a4

help-pdf-letter: help-letter.pdf

help-pdf-a4: help-a4.pdf

training-pdf-letter: training-letter.pdf

training-pdf-a4: training-a4.pdf

help-html-single: help.html

training-html-single: training.html

help-html-multiple: html/index.html

help-chm: help.chm

help-letter.pdf: help-letter.fo $(fopconf)
	$(FOP) $< $@

training-letter.pdf: training-letter.fo $(fopconf)
	$(FOP) $< $@

help-a4.pdf: help-a4.fo $(fopconf)
	$(FOP) $< $@

training-a4.pdf: training-a4.fo $(fopconf)
	$(FOP) $< $@

help-letter.fo: validate $(fostyle) $(srcfiles)
	$(XALAN) \
	    -PARAM paper.type USletter \
	    -IN $(helpsrc) \
	    -XSL $(fostyle) \
	    -OUT $@ \
	    $(ALL_XALANFLAGS)

training-letter.fo: validate $(fostyle) $(srcfiles)
	$(XALAN) \
	    -PARAM paper.type USletter \
	    -IN $(trainingsrc) \
	    -XSL $(fostyle) \
	    -OUT $@ \
	    $(ALL_XALANFLAGS)

help-a4.fo: validate $(fostyle) $(srcfiles)
	$(XALAN) \
	    -PARAM paper.type A4 \
	    -IN $(helpsrc) \
	    -XSL $(fostyle) \
	    -OUT $@ \
	    $(ALL_XALANFLAGS)

training-a4.fo : validate $(fostyle) $(srcfiles)
	$(XALAN) \
	    -PARAM paper.type A4 \
	    -IN $(trainingsrc) \
	    -XSL $(fostyle) \
	    -OUT $@ \
	    $(ALL_XALANFLAGS)

help.html : validate files/style.css $(srcfiles)
	$(XALAN) \
	    -PARAM html.stylesheet files/style.css \
	    -PARAM use.id.as.filename 1 \
	    -PARAM variablelist.as.table 1 \
	    -IN $(helpsrc) \
	    -XSL $(contribdir)/docbook-xsl-ns/html/docbook.xsl \
	    -OUT $@ \
	    $(ALL_XALANFLAGS)

html/index.html: validate files/style.css $(srcfiles)
	$(XALAN) \
	    -PARAM base.dir ../html/ \
	    -PARAM img.src.path ../ \
	    -PARAM html.stylesheet ../files/style.css \
	    -PARAM chunk.quietly 1 \
	    -PARAM use.id.as.filename 1 \
	    -PARAM variablelist.as.table 1 \
	    -IN $(helpsrc) \
	    -XSL $(contribdir)/docbook-xsl-ns/html/chunkfast.xsl \
	    $(ALL_XALANFLAGS)

files/style.css: $(srcdir)/files/style.css
	svn export --force src/files files

help.chm: validate files/style.css $(srcfiles)
	$(XALAN) \
	    -PARAM base.dir ../chm/ \
	    -PARAM manifest.in.base.dir 1 \
	    -PARAM chunk.quietly 1 \
	    -PARAM chunk.section.depth 0 \
	    -PARAM generate.toc "" \
	    -PARAM html.stylesheet files/style.css \
	    -PARAM htmlhelp.remember.window.position 1 \
	    -PARAM htmlhelp.hhc.section.depth 1 \
	    -PARAM htmlhelp.hhc.show.root 0 \
	    -PARAM htmlhelp.show.favorites 1 \
	    -PARAM htmlhelp.show.advanced.search 1 \
	    -PARAM htmlhelp.use.hhk 1 \
	    -PARAM use.id.as.filename 1 \
	    -PARAM variablelist.as.table 1 \
	    -IN $(helpsrc) \
	    -XSL $(contribdir)/docbook-xsl-ns/htmlhelp/htmlhelp.xsl \
	    $(ALL_XALANFLAGS)
	cp -pR files chm/files
	-hhc chm/htmlhelp.hhp
	mv chm/htmlhelp.chm $@
