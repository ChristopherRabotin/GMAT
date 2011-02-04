rem DocBook -> XSL-FO -> PDF
xsltproc -o help.fo contrib/docbook-xsl-ns-1.76.1/fo/docbook.xsl src/help.xml
fop help.fo help-fo.pdf

rem DocBook -> LaTeX -> PDF
python C:/Python27/Scripts/dblatex -o help-latex.pdf src/help.xml

rem DocBook -> CHM
xsltproc --stringparam base.dir chm contrib/docbook-xsl-ns-1.76.1/htmlhelp/htmlhelp.xsl src/help.xml

rem DocBook -> HTML (single page)
xsltproc -o help.html contrib/docbook-xsl-ns-1.76.1/xhtml-1_1/docbook.xsl src/help.xml

rem DocBook -> HTML (multiple pages)
xsltproc --stringparam base.dir html contrib/docbook-xsl-ns-1.76.1/xhtml-1_1/chunk.xsl src/help.xml
