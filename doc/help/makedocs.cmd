@echo off

rem build Xalan classpath
set xalancp=contrib\xalan-j\xalan.jar
set xalancp=%xalancp%;contrib\xalan-j\serializer.jar
set xalancp=%xalancp%;contrib\xalan-j\xml-apis.jar
set xalancp=%xalancp%;contrib\xalan-j\xercesImpl.jar
set xalancp=%xalancp%;contrib\docbook-xsl-ns\extensions\xalan27.jar

rem set Java options
set xmlparser=org.apache.xerces.parsers.XIncludeParserConfiguration

rem set XSL options
set xslopts=-PARAM use.extensions 1
set xslopts=%xslopts% -PARAM graphicsize.extension 1
set xslopts=%xslopts% -PARAM tablecolumns.extension 1
set xslopts=%xslopts% -PARAM refentry.generate.name 0
set xslopts=%xslopts% -PARAM refentry.generate.title 1

rem set up environment
xcopy src\files build\files /i /s /e /q /v /y

rem validate the XML
echo Validating document...
java ^
    -Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
    -jar contrib\jing\bin\jing.jar ^
	contrib\docbook\docbookxi.rng ^
	src\help.xml
echo --------------------
echo.

rem DocBook -> XSL-FO -> PDF (letter)
echo PDF (letter)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	%xslopts% ^
    -PARAM body.font.family sans-serif ^
    -PARAM fop1.extensions 1 ^
    -PARAM shade.verbatim 1 ^
	-PARAM paper.type USletter ^
	-IN src\help.xml ^
	-XSL xform\fo.xsl ^
	-OUT build\help-letter.fo
call contrib\fop\fop.cmd build\help-letter.fo build\help-letter.pdf
echo --------------------
echo.

rem DocBook -> XSL-FO -> PDF (A4)
echo PDF (A4)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	%xslopts% ^
    -PARAM body.font.family sans-serif ^
    -PARAM fop1.extensions 1 ^
    -PARAM shade.verbatim 1 ^
	-PARAM paper.type A4 ^
	-IN src\help.xml ^
	-XSL xform\fo.xsl ^
	-OUT build\help-a4.fo
call contrib\fop\fop.cmd build\help-a4.fo build\help-a4.pdf
echo --------------------
echo.

rem DocBook -> LaTeX -> PDF (letter)
REM echo LaTeX PDF (letter)
REM python C:\Python27\Scripts\dblatex ^
	REM -o build\help-letter-latex.pdf ^
	REM -P latex.class.options=letterpaper ^
	REM -T simple ^
	REM src\help.xml
REM echo --------------------
REM echo.

rem DocBook -> LaTeX -> PDF (A4)
REM echo LaTeX PDF (A4)
REM python C:\Python27\Scripts\dblatex ^
	REM -o build\help-a4-latex.pdf ^
	REM -P latex.class.options=a4paper ^
	REM -T simple ^
	REM src\help.xml
REM echo --------------------
REM echo.

rem DocBook -> CHM
echo Windows Help
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
 	org.apache.xalan.xslt.Process ^
 	%xslopts% ^
	-PARAM base.dir ..\build\chm\ ^
	-PARAM manifest.in.base.dir 1 ^
    -PARAM chapter.autolabel 0 ^
	-PARAM chunk.quietly 1 ^
    -PARAM chunk.section.depth 1 ^
    -PARAM generate.toc "" ^
    -PARAM html.stylesheet files\style.css ^
    -PARAM htmlhelp.remember.window.position 1 ^
    -PARAM htmlhelp.hhc.section.depth 1 ^
    -PARAM htmlhelp.hhc.show.root 0 ^
    -PARAM htmlhelp.show.favorites 1 ^
    -PARAM htmlhelp.show.advanced.search 1 ^
    -PARAM htmlhelp.use.hhk 1 ^
    -PARAM htmlhelp.default.topic ch01.html ^
	-IN src\help.xml ^
	-XSL contrib\docbook-xsl-ns\htmlhelp\htmlhelp.xsl
xcopy src\files build\chm\files /i /s /e /q /v /y
hhc build\chm\htmlhelp.hhp
move build\chm\htmlhelp.chm build\help.chm
echo --------------------
echo.

rem DocBook -> HTML (single page)
echo HTML (single page)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	%xslopts% ^
    -PARAM html.stylesheet files\style.css ^
	-IN src\help.xml ^
	-XSL contrib\docbook-xsl-ns\html\docbook.xsl ^
	-OUT build\help.html
echo --------------------
echo.

rem DocBook -> HTML (multiple pages)
echo HTML (multiple pages)
java -cp %xalancp%	^
	-Dorg.apache.xerces.xni.parser.XMLParserConfiguration=%xmlparser% ^
	org.apache.xalan.xslt.Process ^
	%xslopts% ^
	-PARAM base.dir ..\build\html\ ^
	-PARAM img.src.path ..\ ^
    -PARAM html.stylesheet ..\files\style.css ^
    -PARAM chunk.quietly 1 ^
	-IN src\help.xml ^
	-XSL contrib\docbook-xsl-ns\html\chunkfast.xsl
echo --------------------
echo.

rem move output files
echo Collecting output files...
xcopy build\*.pdf output /i /s /q /v /y
xcopy build\help.chm output /i /s /q /v /y
xcopy build\help.html output /i /s /q /v /y
xcopy build\html output\html /i /s /e /q /v /y
xcopy build\files output\files /i /s /e /q /v /y