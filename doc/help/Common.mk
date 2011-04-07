# Java config
java = java $(javaDefs)
javaDefs = -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=$(xercesP)
xercesP = org.apache.xerces.parsers.XIncludeParserConfiguration

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
all : pdf help.html html/index.html

pdf : help-letter.pdf \
	help-a4.pdf \

help-letter.pdf : files/style.css help-letter.fo
	$(fop) help-letter.fo $@

help-a4.pdf : files/style.css help-a4.fo
	$(fop) help-a4.fo $@

help.html : files/style.css validate $(xmlfiles)
	$(java) -cp $(xalancp) org.apache.xalan.xslt.Process \
	$(xslopts) \
    -PARAM html.stylesheet files/style.css \
	-PARAM use.id.as.filename 1 \
    -PARAM variablelist.as.table 1 \
	-IN src/help.xml \
	-XSL contrib/docbook-xsl-ns/html/docbook.xsl \
	-OUT help.html

html/index.html : files/style.css validate $(xmlfiles)
	$(java) -cp $(xalancp) org.apache.xalan.xslt.Process \
	$(xslopts) \
	-PARAM base.dir ../html/ \
	-PARAM img.src.path ../ \
    -PARAM html.stylesheet ../files/style.css \
    -PARAM chunk.quietly 1 \
	-PARAM use.id.as.filename 1 \
    -PARAM variablelist.as.table 1 \
	-IN src/help.xml \
	-XSL contrib/docbook-xsl-ns/html/chunkfast.xsl

# stand-in for entire files directory
files/style.css : src/files/style.css
	svn export --force src/files files
    
validate :
	$(java) -jar contrib/jing/bin/jing.jar contrib/docbook/docbookxi.rng $(top)

help-letter.fo : validate $(xmlfiles)
	$(java) -cp $(xalancp) org.apache.xalan.xslt.Process \
	$(xslopts) \
	-PARAM body.font.family sans-serif \
	-PARAM fop1.extensions 1 \
	-PARAM paper.type USletter \
	-PARAM shade.verbatim 1 \
	-IN $(top) \
	-XSL xform/fo.xsl \
	-OUT $@

help-a4.fo : validate $(xmlfiles)
	$(java) -cp $(xalancp) org.apache.xalan.xslt.Process \
	$(xslopts) \
	-PARAM body.font.family sans-serif \
	-PARAM fop1.extensions 1 \
	-PARAM paper.type A4 \
	-PARAM shade.verbatim 1 \
	-IN $(top) \
	-XSL xform/fo.xsl \
	-OUT $@