function SolarSystem = SolarSystem_Initialize();

global SolarSystem

%     1 -> Mercury          8 -> Neptune
%     2 -> Venus            9 -> Pluto
%     3 -> Earth           10 -> Moon
%     4 -> Mars            11 -> Sun
%     5 -> Jupiter         12 -> Solar system barycenter
%     6 -> Saturn          13 -> Earth-Moon barycenter
%     7 -> Uranus          14 -> Nutations (longitude and obliquity)
%               15 -> Librations, if on file


%  Set the mu values
SolarSystem.Mu = [22032.080486418
                  324858.59882646 
                  398600.4415  
                  42828.314258067 
                  126712767.85780 
                  37940626.061137
                  5794549.0070719 
                  6836534.0638793 
                  981.60088770700
                  4902.8005821478
                  132712440017.99];
   
   
%  Initialize the DE file 
%addpath('c:\Codes\EphemerisReader\KSC-12544\ephm')
%init_eph('DE405.dat');

