% Declare global variables to use with JPL planetary
% ephemeris functions
%
% Variable      Definition
% FID           Ephemeris coefficient file identifier
% TTL           Title string from ephemeris coefficient file
% SST           3-by-1 vector defining the start time,
%               end time, and interval span for the file
% NCON          The number of constants defined
% CNAM          NCON-by-8 character array of constant names
% CVAL          NCON-by-1 vector of constants
% AU            Definition of astronomical unit in km
% EMRAT         Ratio of Earth mass to Moon mass
% IPT           3-by-13 array of coefficient pointers
% NUMDE         DE number for the ephemeris
% NCOEF         Number of coefficients per interval
% HEADSIZE      Size, in bytes, of header information

global FID TTL SST NCON CNAM CVAL AU EMRAT IPT ...
    NUMDE NCOEF HEADSIZE