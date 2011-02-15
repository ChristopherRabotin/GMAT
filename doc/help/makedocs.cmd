@echo off

rem build Xalan classpath
set xalancp=contrib\xalan-j\xalan.jar
set xalancp=%xalancp%;contrib\xalan-j\serializer.jar
set xalancp=%xalancp%;contrib\xalan-j\xml-apis.jar
set xalancp=%xalancp%;contrib\xalan-j\xercesImpl.jar
set xalancp=%xalancp%;contrib\docbook-xsl-ns\extensions\xalan27.jar

rem set Java options
set xmlparser=org.apache.xerces.parsers.XIncludeParserConfiguration

rem set up environment
xcopy src\files build\files /i /s /e /q

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
call contrib\fop\fop.cmd build\help-letter.fo build\help-letter-fo.pdf
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
call contrib\fop\fop.cmd build\help-a4.fo build\help-a4-fo.pdf
echo --------------------
echo.

rem DocBook -> LaTeX -> PDF (letter)
echo LaTeX PDF (letter)
python C:\Python27\Scripts\dblatex -o build\help-letter-latex.pdf ^
	-P latex.class.options=letterpaper ^
	src\help.xml
echo --------------------
echo.

rem DocBook -> LaTeX -> PDF (A4)
echo LaTeX PDF (A4)
python C:\Python27\Scripts\dblatex -o build\help-a4-latex.pdf ^
	-P latex.class.options=a4paper ^
	src\help.xml
echo --------------------
echo.

rem DocBook -> CHM
echo Windows Help

java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
 	org.apache.xalan.xslt.Process ^
	-PARAM base.dir ..\build\chm\ ^
	-PARAM manifest.in.base.dir 1 ^
	-PARAM chunk.first.sections 1 ^
	-IN src\help.xml ^
	-XSL contrib\docbook-xsl-ns\htmlhelp\htmlhelp.xsl
xcopy src\files build\chm\files /i /s /e /q
hhc build\chm\htmlhelp.hhp
move build\chm\htmlhelp.chm build\help.chm
echo --------------------
echo.

rem DocBook -> HTML (single page)
echo HTML (single page)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	-IN src\help.xml ^
	-XSL contrib\docbook-xsl-ns\xhtml-1_1\docbook.xsl ^
	-OUT build\help.html
echo --------------------
echo.

rem DocBook -> HTML (multiple pages)
echo HTML (multiple pages)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	-PARAM base.dir ..\build\html\ ^
	-PARAM img.src.path ..\ ^
	-IN src\help.xml ^
	-XSL contrib\docbook-xsl-ns\xhtml-1_1\chunk.xsl
echo --------------------
echo.

rem move output files
echo Collecting output files...
xcopy build\*.pdf output /i /s /e /q
xcopy build\*.chm output /i /s /e /q
xcopy build\help.html output /i /s /e /q
xcopy build\html output\html /i /s /e /q
xcopy build\files output\files /i /s /e /q