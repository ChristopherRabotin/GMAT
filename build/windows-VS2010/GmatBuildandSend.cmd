@echo off

rem recipient address
set toaddress=gmat-buildtest@lists.sourceforge.net

blat start.txt -to %toaddress% -f j.parker@nasa.gov -s "GMAT Build Started "%time% -server mailhost.gsfc.nasa.gov -timestamp -debug -log blat.log
call GmatBuild.cmd >results.txt
blat results.txt -to %toaddress% -f j.parker@nasa.gov -s "GMAT Build "%time% -server mailhost.gsfc.nasa.gov -timestamp -debug -log blat.log