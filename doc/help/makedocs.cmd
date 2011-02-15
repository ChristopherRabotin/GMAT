@echo off

rem build Xalan classpath
set xalancp=contrib\xalan-j\xalan.jar
set xalancp=%xalancp%;contrib\xalan-j\serializer.jar
set xalancp=%xalancp%;contrib\xalan-j\xml-apis.jar
set xalancp=%xalancp%;contrib\xalan-j\xercesImpl.jar
set xalancp=%xalancp%;contrib\docbook-xsl-ns\extensions\xalan27.jar

rem set Java options
set xmlparser=org.apache.xerces.parsers.XIncludeParserConfiguration

rem validate the XML
echo Validating document...
java ^
    -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
    -jar contrib\jing\bin\jing.jar ^
	"http://www.docbook.org/xml/5.0/rng/docbookxi.rng" ^
	src\help.xml
echo --------------------
echo.

rem DocBook -> XSL-FO -> PDF (letter)
echo PDF (letter)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	-IN src\help.xml ^
	-XSL contrib\docbook-xsl-ns\fo\docbook.xsl ^
	-OUT build\help-letter.fo
start /b contrib\fop\fop.cmd help-letter.fo help-letter-fo.pdf
echo --------------------
echo.

rem DocBook -> XSL-FO -> PDF (A4)
echo PDF (A4)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	-IN src\help.xml ^
	-XSL contrib\docbook-xsl-ns\fo\docbook.xsl ^
	-OUT build\help-a4.fo
start /b contrib\fop\fop.cmd help-a4.fo help-a4-fo.pdf
echo --------------------
echo.

rem DocBook -> LaTeX -> PDF (letter)
echo LaTeX PDF (letter)
python C:\Python27\Scripts\dblatex -o help-letter-latex.pdf ^
	-P latex.class.options=letterpaper ^
	src\help.xml
echo --------------------
echo.

rem DocBook -> LaTeX -> PDF (A4)
echo LaTeX PDF (A4)
python C:\Python27\Scripts\dblatex -o help-a4-latex.pdf ^
	-P latex.class.options=a4paper ^
	src\help.xml
echo --------------------
echo.

rem DocBook -> CHM
rem echo Windows Help
rem java -cp %xalancp%	^
rem 	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
rem  	org.apache.xalan.xslt.Process ^
rem 	-PARAM base.dir ..\build\chm\ ^
rem 	-PARAM manifest.in.base.dir 0 ^
rem 	-PARAM chunk.first.sections 1 ^
rem 	-IN src\help.xml ^
rem 	-XSL contrib\docbook-xsl-ns\htmlhelp\htmlhelp.xsl
rem hhc htmlhelp.hhp
echo --------------------
echo.

rem DocBook -> HTML (single page)
rem echo HTML (single page)
rem java -cp %xalancp%	^
rem 	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
rem 	org.apache.xalan.xslt.Process ^
rem 	-PARAM img.src.path ..\build\files\ ^
rem 	-IN src\help.xml ^
rem 	-XSL contrib\docbook-xsl-ns\xhtml-1_1\docbook.xsl ^
rem 	-OUT build\help.html
rem echo --------------------
rem echo.

rem DocBook -> HTML (multiple pages)
rem echo HTML (multiple pages)
rem java -cp %xalancp%	^
rem 	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
rem 	org.apache.xalan.xslt.Process ^
rem 	-PARAM base.dir ..\build\html\ ^
rem 	-PARAM img.src.path ..\files\ ^
rem 	-IN src\help.xml ^
rem 	-XSL contrib\docbook-xsl-ns\xhtml-1_1\chunk.xsl
rem echo --------------------
rem echo.