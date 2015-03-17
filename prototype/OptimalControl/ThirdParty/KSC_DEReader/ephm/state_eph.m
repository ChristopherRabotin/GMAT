function [pv,vv] = state_eph(jd,list,baryflag,kmflag)

% STATE_EPH Read and interpolate JPL Planetary Ephemeris File
%     STATE_EPH(JD,LIST,BARYFLAG,KMFLAG,EPHFILE) reads and
%     interpolates a JPL planetary ephemeris file
%
%     INPUTS: JD      Julian Date at desired interpolation epoch
%
%             LIST    1-by-12 array indicating which bodies and what
%                     type of interpolation is desired:
%                     LIST(i) = 0 -> No interpolation for body i
%                             = 1 -> Position only
%                             = 2 -> Position and velocity
%
%                     Designation of astronomical bodies
%                      i = 1 -> Mercury
%                        = 2 -> Venus
%                        = 3 -> Earth-Moon barycenter
%                        = 4 -> Mars
%                        = 5 -> Jupiter
%                        = 6 -> Saturn
%                        = 7 -> Uranus
%                        = 8 -> Neptune
%                        = 9 -> Pluto
%                        = 10-> Geocentric Moon
%                        = 11-> Nutations in Earth longitude and 
%                               obliquity
%                        = 12-> Lunar librations (if on file)
%
%            BARYFLAG  = 0 -> Planetary states are heliocentric
%                     ~= 0 -> Planetary states are solar-system 
%                             barycentric
%
%            KMFLAG    = 0 -> Units are AU and AU/day
%                     ~= 0 -> Units are km and km/s
%
%            EPHFILE  Name of ephemeris file
%
%     OUTPUTS: PV       3-by-13 vector of positions
%              
%              VV       3-by-13 vector of velocities
%
%     States are relative to Earth mean equator and equinox of J2000
%     if the DE number is >= 200; of B1950 if the DE number is < 200.

% Declare global variables

eph_global

% Check to see if JD is in the ephemeris file interval

if jd < SST(1) | jd > SST(2), error('Specified date not in ephemeris interval.'), end

% Determine which interval jd is within and read in the coefficients for that
% interval

iint=max([1 ceil((jd-SST(1))/SST(3))]);
fseek(FID,HEADSIZE+(iint-1)*NCOEF*8,-1);
db=fread(FID,NCOEF,'*float64'); % buf is ncoef-by-1 coefficient vector


% Normalize time to be on [0 1]

ts=SST(3);
t=(jd-db(1))/ts;
% NCOEF
% iint
% db(1)
% db(2)
% size(db)

% Pre-allocate pv and vv matrices

pv=zeros(3,12);
vv=zeros(3,12);
au2km=AU; % define au2km conversion factor


% Get barycentric state of Sun

buf=getcoef(t,db,3,11);

[pvsun,vvsun]=int_eph(buf,t,ts,IPT(2,11),IPT(3,11));

% Get barycentric state of selected planets

for i=1:10
    if list(i) ~= 0
       buf=getcoef(t,db,3,i);
       [pv(:,i),vv(:,i)]=int_eph(buf,t,ts,IPT(2,i),IPT(3,i));
   end
end

% Put state in desired units

if ~kmflag
    pv=pv/au2km;
    vv=vv/au2km;
    pvsun=pvsun/au2km;
    vvsun=vvsun/au2km;
else
    vv=vv/86400;
    vvsun=vvsun/86400;
end

% If heliocentric state is desired (baryflag is false), subtract solar state from
% planetary state

if ~baryflag
    pv=pv-pvsun*[(list(1:10)>0) 0 0];
    vv=vv-vvsun*[(list(1:10)>0) 0 0];
end

% Get nutations and librations if requested and available

for i=11:12
    if (list(i) > 0 & IPT(2,i+1) > 0)
        buf=getcoef(t,db,i-9,i+1);
        [pnut,vnut]=int_eph(buf,t,ts,IPT(2,i+1),IPT(3,i+1));
        pv(1:size(pnut,1),i)=pnut;
        vv(1:size(vnut,1),i)=vnut;
    end
end

% Append barycentric solar state to pv and vv

pv=[pv(:,1:10) pvsun pv(:,11:12)];
vv=[vv(:,1:10) vvsun vv(:,11:12)];


% Subfunction INT_EPH

function [pv,vv]=int_eph(buf,t,ts,ncf,na)

% INT_EPH Interpolate a set of chebyshev coefficients to give position and velocity
%

% Transform T from [0 1] to [-1 1]

tc=2*(mod(na*t,1)+fix(t))-1;

%tc=2*t-1;

% Evaluate Chebyshev polynomials at TC

pc=chebyval(tc,ncf-1);

% Multiply and add to get position vector

pv=fliplr(buf')*flipud(pc); % Flip matrices to sum from smalles to largest

% Differentiate to get velocity

pcp=chebyder(tc,pc);
vv=fliplr(buf')*flipud(pcp)*2/ts*na;

% Subfunction getcoef

function buf=getcoef(t,db,ndim,ibod)

eph_global;

l=fix(IPT(3,ibod)*t-fix(t));
ist=IPT(1,ibod)+l*ndim*IPT(2,ibod);
ien=IPT(1,ibod)+(l+1)*ndim*IPT(2,ibod)-1;

buf=reshape(db(ist:ien),[IPT(2,ibod),ndim]);
