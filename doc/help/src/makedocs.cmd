@echo off

rem build Xalan classpath
set xalancp=..\contrib\xalan-j\xalan.jar
set xalancp=%xalancp%;..\contrib\xalan-j\serializer.jar
set xalancp=%xalancp%;..\contrib\xalan-j\xml-apis.jar
set xalancp=%xalancp%;..\contrib\xalan-j\xercesImpl.jar
set xalancp=%xalancp%;..\contrib\docbook-xsl-ns\extensions\xalan27.jar

rem set Java options
set xmlparser=org.apache.xerces.parsers.XIncludeParserConfiguration

rem validate the XML
echo Validating document...
java ^
    -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
    -jar ..\contrib\jing\bin\jing.jar ^
	"http://www.docbook.org/xml/5.0/rng/docbookxi.rng" ^
	help.xml
echo --------------------
echo.

rem DocBook -> XSL-FO -> PDF (letter)
echo PDF (letter)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	-IN help.xml ^
	-XSL ..\contrib\docbook-xsl-ns\fo\docbook.xsl ^
	-OUT help-letter.fo
start /b ..\contrib\fop\fop.cmd help-letter.fo help-letter-fo.pdf
echo --------------------
echo.

rem DocBook -> XSL-FO -> PDF (A4)
echo PDF (A4)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	-IN help.xml ^
	-XSL ..\contrib\docbook-xsl-ns\fo\docbook.xsl ^
	-OUT help-a4.fo
start /b ..\contrib\fop\fop.cmd help-a4.fo help-a4-fo.pdf
echo --------------------
echo.

rem DocBook -> LaTeX -> PDF (letter)
echo LaTeX PDF (letter)
python C:\Python27\Scripts\dblatex -o help-letter-latex.pdf ^
	-P latex.class.options=letterpaper ^
	help.xml
echo --------------------
echo.

rem DocBook -> LaTeX -> PDF (A4)
echo LaTeX PDF (A4)
python C:\Python27\Scripts\dblatex -o help-a4-latex.pdf ^
	-P latex.class.options=a4paper ^
	help.xml
echo --------------------
echo.

rem DocBook -> CHM
echo Windows Help
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	-PARAM base.dir ..\chm\ ^
	-IN help.xml ^
	-XSL ..\contrib\docbook-xsl-ns\htmlhelp\htmlhelp.xsl
hhc htmlhelp.hhp
echo --------------------
echo.

rem DocBook -> HTML (single page)
echo HTML (single page)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	-IN help.xml ^
	-XSL ..\contrib\docbook-xsl-ns\xhtml-1_1\docbook.xsl ^
	-OUT help.html
echo --------------------
echo.

rem DocBook -> HTML (multiple pages)
echo HTML (multiple pages)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	-PARAM base.dir ..\html\ ^
	-IN help.xml ^
	-XSL contrib\docbook-xsl-ns\xhtml-1_1\chunk.xsl
echo --------------------
echo.