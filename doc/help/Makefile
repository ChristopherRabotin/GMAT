# Java config
java = java $(javaDefs)
javaDefs = -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=$(xercesP)
xercesP = org.apache.xerces.parsers.XIncludeParserConfiguration

# Build Xalan classpath
xalancp = "contrib/xalan-j/xalan.jar:contrib/xalan-j/serializer.jar:\
	contrib/xalan-j/xml-apis.jar:contrib/xalan-j/xercesImpl.jar:\
	contrib/docbook-xsl-ns/extensions/xalan27.jar"

# Common XSLT options
xslopts = -PARAM use.extensions 1 \
	-PARAM graphicsize.extension 1 \
	-PARAM tablecolumns.extension 1 \
	-PARAM refentry.generate.name 0 \
	-PARAM refentry.generate.title 1 \

# Source files
top = src/help.xml
xmlfiles = src/*.xml
graphicfiles = src/files/images/*.*
srcfiles = $(xmlfiles) $(graphicfiles)

# Rules
all : help-letter-fo.pdf \
	help-a4-fo.pdf \
	help-letter-latex.pdf \
	help-letter-latex.pdf

help-letter-fo.pdf : help-letter.fo
	sh contrib/fop/fop $< $@

help-a4-fo.pdf : help-a4.fo
	sh contrib/fop/fop $< $@

help-letter-latex.pdf : $(srcfiles)
help-a4-latex.pdf : $(srcfiles)

validate :
	$(java) -jar contrib/jing/bin/jing.jar contrib/docbook/docbookxi.rng $(top)

clean :
	rm -f *.fo *.pdf

help-letter.fo : validate $(xmlfiles)
	$(java) -cp $(xalancp) org.apache.xalan.xslt.Process \
	$(xslopts) \
	-PARAM body.font.family sans-serif \
	-PARAM fop1.extensions 1 \
	-PARAM paper.type USletter \
	-PARAM shade.verbatim 1 \
	-IN $(top) \
	-XSL contrib/docbook-xsl-ns/fo/docbook.xsl \
	-OUT $@

help-a4.fo : validate $(xmlfiles)
	$(java) -cp $(xalancp) org.apache.xalan.xslt.Process \
	$(xslopts) \
	-PARAM body.font.family sans-serif \
	-PARAM fop1.extensions 1 \
	-PARAM paper.type A4 \
	-PARAM shade.verbatim 1 \
	-IN $(top) \
	-XSL contrib/docbook-xsl-ns/fo/docbook.xsl \
	-OUT $@