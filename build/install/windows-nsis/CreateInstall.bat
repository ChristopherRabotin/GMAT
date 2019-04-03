#	Using a mingw shell window, Run from the windows-nsis directory as shown below
#	cd "C:\Repos\gmat\build\install\windows-nsis"
#   rm outfile     #delete old log file
#	CreateInstall.bat 2>&1 | tee outfile

#	Clean up directory as needed
make clean


make assemble VERSION="R2018a"
echo   
echo Assemble complete
echo  


#	Use sejda-console to amend the PDF files (e.g., add a cover sheet)

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-A4-Trimmed.pdf gmat-internal/GMAT/docs/help/help-a4.pdf -o gmat-internal/GMAT/docs/help/help-a4-new.pdf -s all:all:
mv gmat-internal/GMAT/docs/help/help-a4-new.pdf gmat-internal/GMAT/docs/help/help-a4.pdf

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-Letter-Trimmed.pdf gmat-internal/GMAT/docs/help/help-letter.pdf -o gmat-internal/GMAT/docs/help/help-letter-new.pdf -s all:all:
mv gmat-internal/GMAT/docs/help/help-letter-new.pdf gmat-internal/GMAT/docs/help/help-letter.pdf

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-A4-Trimmed.pdf gmat-public/GMAT/docs/help/help-a4.pdf -o gmat-public/GMAT/docs/help/help-a4-new.pdf -s all:all:
mv gmat-public/GMAT/docs/help/help-a4-new.pdf gmat-public/GMAT/docs/help/help-a4.pdf

sejda-console merge -f ../../../doc/help/src/files/images/Cover-UserGuide-Letter-Trimmed.pdf gmat-public/GMAT/docs/help/help-letter.pdf -o gmat-public/GMAT/docs/help/help-letter-new.pdf -s all:all:
mv gmat-public/GMAT/docs/help/help-letter-new.pdf gmat-public/GMAT/docs/help/help-letter.pdf
 
 
echo   
echo sejda-console PDF merging complete
echo 
 
 
#	Run 'make VERSION="R2015a-rc#"', where "#" is the number of the RC you're creating. 
#	This will create four packages in the current directory: A .zip and a .exe file for both 
#	the internal and public versions. 
#	Note: To create only an internal version, run 'make internal VERSION="R2015a-rc#"'.

make VERSION="R2018a"

echo   
echo Creation of Install packages complete.  Check for errors in log file.  
echo

#	Copy the four package files to the network: 
#           \\mesa-file\595\GMAT\Builds\windows\VS2013_build_32\R2015a    (old example)
#			\\mesa-file\595\GMAT\Builds\windows\VS2013_build_64\R2018a	  (current example)


#	To clean everything up afterwards, run "make clean".

#	Note: To make the final release bundles, you can't just rename the files to take off the "-rc#" portion. 
#		  You need to recreate the bundles using this command: make VERSION="R2015a" 
