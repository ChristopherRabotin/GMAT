function [ttlsize,cnamsize,sssize,iptsize] = eph_header_size()

% EPH_HEADER_SIZE Set sizes for JPL Ephemeris header file parameters

% DE200 values
ttlsize  = [3 80];
cnamsize = 8;
sssize   = 3;
iptsize  = [3 13];