-----------------------------------------------------------------
Prerequisites
-----------------------------------------------------------------
To generate the GMAT help documentation, the following tools must be installed
and present on the system path for the current user:

* Java (for everything)

* Python 2.x (script assumes 2.7.x) (for LaTeX output)
    Main page: http://python.org
    Windows: http://python.org/download/
    
* dblatex (for LaTeX output)
    Main page: http://dblatex.sourceforge.net/
    Windows: https://sourceforge.net/projects/dblatex/files/dblatex/
    
    Download, extract, and run "python setup.py install".

* LaTeX (any distribution, such as MiKTeX) (for LaTeX output)

* ImageMagick (for LaTeX output)
    Main page: http://imagemagick.org
    Windows: (get the first download) 
        http://imagemagick.org/script/binary-releases.php#windows
    
    After install, add it to the beginning of your system path.
    
* HTML Help Workshop (for Windows Help output)
    Download: http://www.microsoft.com/downloads/en/details.aspx?FamilyID=00535334-c8a6-452f-9aa0-d597d16580cc&displaylang=en

    
-----------------------------------------------------------------
Usage
-----------------------------------------------------------------
Windows:
    1. Open a command window (such as cmd.exe).
    2. Navigate to the the folder containing makedocs.cmd.
    3. Run "makedocs". All output files will be placed in the build
        directory, and the final files will be moved to the output directory.
        
Mac/Linux:
    1. Open a shell.
    2. Navigate to the folder containing makedocs.
    3. Run "./makedocs" or "sh makedocs".