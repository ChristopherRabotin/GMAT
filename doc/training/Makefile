# Makefile for the GMAT Training Manual.
# Must be run in an MSYS environment on Windows.

# Common variables
contribdir = ../build/contrib
JAVA = java

# Xalan
XALAN = $(JAVA) $(XALANDEFS) -cp $(XALANCP) org.apache.xalan.xslt.Process
XALANCP = $(contribdir)/xalan-j/serializer.jar:$(contribdir)/xalan-j/xalan.jar:$(contribdir)/xalan-j/xercesImpl.jar:$(contribdir)/xml-apis.jar
XALANDEFS=-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=org.apache.xerces.parsers.XIncludeParserConfiguration

# FOP
FOP = java -Dfop.home=$(contribdir)/fop -jar $(contribdir)/fop/build/fop.jar -c $(FOPCONF)
FOPCONF = config/fop.xconf

# Rules
all: pdf

pdf: GmatTrainingManualA4.pdf

clean:
	rm -f *.fo *.pdf

GmatTrainingManualA4.pdf: GmatTrainingManualA4.fo $(FOPCONF)
	$(FOP) $< $@

GmatTrainingManualA4.fo: src/*.xml xform/fo.xsl
	$(XALAN) \
	    -PARAM paper.type A4 \
	    -IN src/GmatTrainingManual.xml \
	    -XSL xform/fo.xsl \
	    -OUT $@

