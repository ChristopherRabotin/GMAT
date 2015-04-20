@echo off

rem configuration
set server=mailhost.gsfc.nasa.gov
set fromaddress=linda.o.jun@nasa.gov
set toaddress=gmat-buildtest@lists.sourceforge.net

blat start.txt -to %toaddress% -f %fromaddress% -s "GMAT Build Started "%time% -server %server% -timestamp -debug -log blat.log
call GmatBuild.cmd %1 > results.txt
blat results.txt -to %toaddress% -f %fromaddress% -s "GMAT Build "%time% -server %server% -timestamp -debug -log blat.log
