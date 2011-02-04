-----------------------------------------------------------------
Prerequisites
-----------------------------------------------------------------
To generate the GMAT help documentation, the following tools must be installed
and present on the system path for the current user:

* xsltproc
    Main page: http://xmlsoft.org/XSLT/index.html
    Windows: http://www.zlatkovic.com/libxml.en.html
    
    Download zlib, iconv, libxml2, and libxslt and unzip them all to the same
    directory.
    
* fop
    Main page: http://xmlgraphics.apache.org/fop/
    Windows: http://apache.osuosl.org/xmlgraphics/fop/binaries/
    
* Python 2.x (assumes 2.7.x)
    Main page: http://python.org
    Windows: http://python.org/download/
    
* dblatex
    Main page: http://dblatex.sourceforge.net/
    Windows: https://sourceforge.net/projects/dblatex/files/dblatex/
    
    Download, extract, and run "python setup.py install".

* LaTeX (any distribution, such as MiKTeX)

* HTML Help Workshop (to generate .chm)
    Download: http://www.microsoft.com/downloads/en/details.aspx?FamilyID=00535334-c8a6-452f-9aa0-d597d16580cc&displaylang=en
    
-----------------------------------------------------------------
Usage
-----------------------------------------------------------------
Windows:
    1. Open a command window (such as cmd.exe).
    2. Navigate to the the folder containing makedocs.cmd.
    3. Run "makedocs". All output files will be placed in the current
        directory.
    4. Run the HTML Help Workshop to generate .chm from files in the chm
        directory.