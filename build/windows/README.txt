Microsoft Visual C++ static link build needs three environment variables:
1) WXWIN (for wxWidgets GUI)
2) MATLAB32 (for MATLAB interface)
3) CSPICE_VC32 (for CSPICE interface)

To add WXWIN variable:
From My Computer -> Properties -> Advanced Tab -> Environment Variables.
Add variable WXWIN and set value to directory where wxWidgets is installed, such as C:\wxWidgits-2.8.10.
Current GMAT development build uses wxWidgets-2.8.10 as of 2009.10.17.

To add MATLAB32 variable:
From My Computer -> Properties -> Advanced Tab -> Environment Variables.
Add variable MATLAB32 and set vaue to directory where MATLAB is installed, such as C:\Program Files (x86)\MATLAB\R2011a

To add CSPICE_VC32 variable:
From My Computer -> Properties -> Advanced Tab -> Environment Variables.
Add variable CSPICE_VC32 and set vaue to directory where CSPICE is installed, such as C:\Projects\GMAT\Gmat3rdParty\cspice

