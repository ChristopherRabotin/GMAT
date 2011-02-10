@rem build Xalan classpath
set xalancp=contrib/xalan-j/xalan.jar:contrib/xalan-j/serializer.jar
set xalancp=%xalancp%:contrib/xalan-j/xml-apis.jar:contrib/xalan-j/xercesImpl.jar
set xalancp=%xalancp%:contrib/docbook-xsl-ns/extensions/xalan27.jar
set xercesParser=org.apache.xerces.parsers.XIncludeParserConfiguration

mkdir build

@rem validate the XML
java -jar contrib/jing/bin/jing.jar ^
	'http://www.docbook.org/xml/5.0/rng/docbookxi.rng' ^
	src/help.xml

@rem DocBook -> XSL-FO -> PDF (letter)
xsltproc -o build/help-letter.fo ^
	contrib/docbook-xsl-ns/fo/docbook.xsl ^
	src/help.xml
fop help-letter.fo help-letter-fo.pdf

@rem DocBook -> XSL-FO -> PDF (A4)
xsltproc -o build/help-a4.fo ^
	--stringparam paper.type A4 ^
	contrib/docbook-xsl-ns/fo/docbook.xsl ^
	src/help.xml
fop help-a4.fo help-a4-fo.pdf

@rem DocBook -> LaTeX -> PDF (letter)
python C:/Python27/Scripts/dblatex -o build/help-letter-latex.pdf ^
	-P latex.class.options=letterpaper ^
	src/help.xml

@rem DocBook -> LaTeX -> PDF (A4)
python C:/Python27/Scripts/dblatex -o build/help-a4-latex.pdf ^
	-P latex.class.options=a4paper ^
	src/help.xml

@rem DocBook -> CHM
xsltproc --stringparam base.dir build/chm/ ^
	contrib/docbook-xsl-ns/htmlhelp/htmlhelp.xsl ^
	src/help.xml

@rem DocBook -> HTML (single page)
java -cp %xalancp%	\
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xercesParser% \
	org.apache.xalan.xslt.Process \
	-IN src/help.xml \
	-XSL contrib/docbook-xsl-ns/xhtml-1_1/docbook.xsl \
	-OUT build/help.html

@rem DocBook -> HTML (multiple pages)
xsltproc --stringparam base.dir build/html/ ^
	contrib/docbook-xsl-ns/xhtml-1_1/chunk.xsl ^
	src/help.xml
