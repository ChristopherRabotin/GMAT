echo off
REM Clears the screen
CLS

echo Starting build process at %date% - %time%
echo    * Cleaning up old build (%time%)...
c:\msys\bin\sh.exe -c "make clean > GmatClean.log 2> GmatClean.err"
IF ERRORLEVEL 2 echo      There were errors cleaning the old build
IF ERRORLEVEL 0 echo      Cleanup was successful

rem echo    * Checking out the code (%time%)...
rem c:\msys\bin\sh.exe -c "make checkout > GmatCheckout.log 2> GmatCheckout.err"
rem IF ERRORLEVEL 2 GOTO CheckoutError
rem IF ERRORLEVEL 0 echo      Checkout from svn was successful

echo    * Compiling GMAT (%time%)...
c:\msys\bin\sh.exe -c "make all > GmatBuild.log 2> GmatBuild.err"
IF ERRORLEVEL 2 GOTO BuildError
IF ERRORLEVEL 0 echo      Compilation was successful

echo    * Creating user docs (%time%)...
c:\msys\bin\sh.exe -c "make userdocs > GmatUserDocs.log 2> GmatUserDocs.err"
IF ERRORLEVEL 2 echo      There were errors making the user documentation
IF ERRORLEVEL 0 echo      User documentation build was successful

echo    * Creating dev docs (%time%)...
c:\msys\bin\sh.exe -c "make devdocs > GmatDevDocs.log 2> GmatDevDocs.err"
IF ERRORLEVEL 2 echo      There were errors making the developer documentation
IF ERRORLEVEL 0 echo      Developer documentation build was successful

echo    * Assembling Daily Distribution (%time%)...
c:\msys\bin\sh.exe -c "make dist > GmatDist.log 2> GmatDist.err"
IF ERRORLEVEL 2 echo      There were errors assembling today's GMAT executable distribution
IF ERRORLEVEL 0 echo      GMAT executable distribution was successfully assembled

echo    * Assembling Latest Developer Docs Distribution (%time%)...
c:\msys\bin\sh.exe -c "make dist-devdocs > GmatDistDevDocs.log 2> GmatDistDevDocs.err"
IF ERRORLEVEL 2 echo      There were errors assembling the latest developer documentation distributable
IF ERRORLEVEL 0 echo      Latest developer documentation distributable was successfully assembled

GOTO End

:CheckoutError
echo There was an error checking out the code from svn
GOTO End

:BuildError
echo There was an error building (compiling) the code
GOTO End


:End
echo Stopping build process at %date% - %time%
echo on
