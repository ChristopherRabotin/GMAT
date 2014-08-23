% JPL Ephemeris Utility Functions
% JPL Ephemeris Toolbox   Version 1.0 (R12)  14-Mar-2001
%
% Planetary state extraction
%   pleph           - Read JPL planetary ephemeris and give body states
%   state_eph       - Read and interpolate JPL Planetary Ephemeris File
%
% Binary ephemeris file generation
%   asc2bin_eph     - Write binary JPL planetary ephemeris file
%   binmerge        - Merge two binary JPL ephemeris files
%   mergemall       - Merge binary ephemeris files into one file
%
% Utilities
%   chebyder        - Evaluate Chebyshev polynomial derivatives
%   chebyval        - Evaluate Chebyshev polynomial
%   eph_global      - Declare global variables
%   eph_header_size - Set sizes for ephemeris header file parameters
%   init_eph        - Open ephemeris file and initialize constants
%
% Test functions
%   read_testpo     - Read contents of JPL ephemeris test data file
%   testeph         - Test ephemeris file reading and interpolation