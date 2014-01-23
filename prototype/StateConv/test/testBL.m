%% test prototypes. Case A.1. general elliptic orbit

Cart=[ 4340.789050      -0.000000     -0.000000      -0.000000       9.784755       5.312688];
STK_OSC=[6.678137000000000e+003    3.499999999999997e-001    2.850000000000000e+001    0.000000000000000e+000    9.704955640900000e-015    0.000000000000000e+000];
STK_long=[ 6.659637260077382e+003    3.473326030715827e-001    2.846453224361113e+001    3.599450721824177e+002       3.599700015119411e+002     6.963626331704763e-002];
STK_short=[  6.659640789813191e+003    3.472942848235042e-001    2.846617054622099e+001    0.000000000000000e+000       0.000000000000000e+000     0.000000000000000e+000];

BLmean=Cart2BLlong(Cart);
Kep2=BROLYD(BLmean);
Cart2=Kep2Cart2(Kep2);

Cart-Cart2


STK_long-BLmean
%%

BLmean_s=Cart2BLshort(Cart);
Kep2=BROLYD_short(BLmean_s);
Cart2=Kep2Cart2(Kep2);

Cart-Cart2


STK_short-BLmean_s


%% test prototypes. Case A.2. general elliptic orbit

Cart=[ 461.001990     -709.880813    2095.002178       9.272005     -14.277636       1.818890];
STK_OSC=[6.678136999999987e+003    7.499999999999997e-001    9.000000000000003e+001    1.230000000000000e+002    4.499999999999999e+001    6.700000000000001e+001];
STK_long=[ 6.951362912368119e+003    7.569572648462358e-001    9.000000000000003e+001    1.230000000000000e+002       4.580219526453274e+001     7.408547260619747e+000];
STK_short=[  6.956038596126880e+003    7.583456514106794e-001    9.000000000000003e+001    1.230000000000000e+002       4.645105826482318e+001     7.292507905273626e+000];

BLmean=Cart2BLlong(Cart);
Kep2=BROLYD(BLmean);
Cart2=Kep2Cart2(Kep2);

Cart-Cart2


STK_long-BLmean


BLmean_s=Cart2BLshort(Cart);
Kep2=BROLYD_short(BLmean_s);
Cart2=Kep2Cart2(Kep2);

Cart-Cart2


STK_short-BLmean_s


%% Case B. Near equatorial elliptic orbit

Cart=[  -58220.489189    -83147.474321    -14.511972       1.155781      -2.278017      -0.000398];
STK_OSC=[3.000000000000009e+005    7.500000000000002e-001    1.000000000000003e-002    3.635764603856000e-013    1.679999999999997e+002    6.700000000000001e+001];
STK_long=[2.999967172967700e+005    7.499968064345355e-001    1.077526581161255e-002    1.280093731536645e+001       1.551983792640930e+002     7.856823222856527e+000];
STK_short=[    2.999967172969353e+005    7.499968083950251e-001    1.000000082692551e-002    2.176366005101659e-004       1.679990979951487e+002     7.856823437121984e+000];

BLmean=Cart2BLlong(Cart);
Kep2=BROLYD(BLmean);
Cart2=Kep2Cart2(Kep2);

Cart-Cart2


STK_long-BLmean


BLmean_s=Cart2BLshort(Cart);
Kep2=BROLYD_short(BLmean_s);
Cart2=Kep2Cart2(Kep2);

Cart-Cart2


STK_short-BLmean_s


%% Case C. Retrograde near circular orbit

Cart=[ -58220.489189    83129.239972    -1741.309346       1.155781       2.277518      -0.047707];
STK_OSC=[3.000000000000015e+005    7.500000000000006e-001    1.788000000000000e+002    2.851788551480000e-014    1.680000000000001e+002    6.699999999999996e+001];
STK_long=[ 2.999967203925151e+005    7.499967003959404e-001    1.787786995074731e+002    3.598878310358884e+002       1.678872826987391e+002     7.856797106384351e+000];
STK_short=[ 2.999967203089754e+005    7.499968113024010e-001    1.787999998218830e+002    3.599997824111455e+002       1.679990984851218e+002     7.856823270873667e+000];

BLmean=Cart2BLlong(Cart);
Kep2=BROLYD(BLmean);
Cart2=Kep2Cart2(Kep2);

Cart-Cart2


STK_long-BLmean


BLmean_s=Cart2BLshort(Cart);
Kep2=BROLYD_short(BLmean_s);
Cart2=Kep2Cart2(Kep2);

Cart-Cart2


STK_short-BLmean_s

%% highly eccentric and retrograde orbits
% rp=6378.5;
% ra=99999;
% sma=(rp+ra)/2
% ecc=(ra-rp)/2/sma
clear
clc
rp=1000;
ecc=0.999998;
sma=rp/(1-ecc);

% ra=sma*(1+ecc)
%BLmean_s=[   sma    ecc    6.343333333333334e1    rand(1,3)*360];
BLmean_s=[  7200 0 0 0 0 0];
Kep=BROLYD_short(BLmean_s);

Cart=Kep2Cart2(Kep);

[BLmean_s2 emag j]=Cart2BLshort(Cart);

BLmean_s-BLmean_s2

Kep2=BROLYD_short(BLmean_s2);

Cart2=Kep2Cart2(Kep2);

norm(Cart-Cart2)./norm(Cart)
%%
disp('long term averaged')
% BLmean_l=[  sma    ecc    6.052e1    rand(1,3)*360];

BLmean_l=[  7200 0 0 0 0 0];
Kep3=BROLYD(BLmean_l);

Cart3=Kep2Cart2(Kep3);

BLmean_l2=Cart2BLlong(Cart3);

BLmean_l-BLmean_l2

Kep4=BROLYD(BLmean_l2);

Cart4=Kep2Cart2(Kep4);

norm(Cart3-Cart4)/norm(Cart3)
%% test prototypes. Case A.1. general elliptic orbit

Cart=[ 7074.332549579282      -3.992328033513578e-013     1295.300325979304      -0.1778556913847675       7.378891568843633       0.9713656991014339];
Cart=[ 7074.332549579282      1     1295.300325979304      -0.1778556913847675       7.378891568843633       0.9713656991014339];

STK_OSC=[6.678137000000000e+003    3.499999999999997e-001    2.850000000000000e+001    0.000000000000000e+000    9.704955640900000e-015    0.000000000000000e+000];

STK_long=[ 6.659637260077382e+003    3.473326030715827e-001    2.846453224361113e+001    3.599450721824177e+002       3.599700015119411e+002     6.963626331704763e-002];
STK_short=[  6.659640789813191e+003    3.472942848235042e-001    2.846617054622099e+001    0.000000000000000e+000       0.000000000000000e+000     0.000000000000000e+000];

BLmean=Cart2BLshort(Cart);
Kep2=BROLYD_short(BLmean);
Cart2=Kep2Cart2(Kep2);

Cart-Cart2
