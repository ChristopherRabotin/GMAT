%INTLAB some utility routines
%
%Matrix routines
%  isspd        - symmetric (Hermitian) is spd
%  gensum       - generation of extremely ill-conditioned sums
%  gendot       - generation of extremely ill-conditioned dot products
%  compmat      - Ostrowski's comparison matrix
%  gregk...     - Some sparse test matrices from Gregory/Karney test set
%  circulant    - Circulant out of first row
%
%Random numbers
%  randint      - Random integers in specified range
%  random       - Random numbers uniformly distrubuted in [-1,1] or
%                   within specified range
%  randomc      - Complex random numbers with real and imaginary part as random
%  randmat      - random matrix with specified condition number
%  randorth     - random orthogonal matrix
%
%Other routines
%  format       - change interval output format
%  finish       - makes sure rounding mode is set to nearest after exiting Matlab
%  helpp        - intelligent help
%  binom        - Binomial coefficient (vectowise)
%  relerr       - Relative error
%  perms_       - Fast version of Matlab perms
%  sqr          - Square
%  odd          - logical integer odd
%  even         - logical integer even
%  factors      - List of factors of an integer
%  flip         - Flip array at first non-singleton (useful for vectors)
%  distbits     - Distance in bits
%  fletcher     - Sample routine for nonlinear systems
%

% written  11/30/98     S.M. Rump
% modified 12/15/01     S.M. Rump  Routine Fletcher added
% modified 11/23/05     S.M. Rump  band and bandwidth removed (already in /intval)
%
% Copyright (c) Siegfried M. Rump, head of the Institute for Reliable Computing, 
%               Hamburg University of Technology
