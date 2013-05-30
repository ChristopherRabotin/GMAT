function out = timeconv(t, fromscale, toscale, dat)
    %TIMECONV Convert a Modified Julian date between time scales.
    %   OUT = TIMECONV(T, FROMSCALE, TOSCALE) converts a Modified Julian
    %   date T from the time scale FROMSCALE to the time scale TOSCALE. The
    %   valid values for FROMSCALE and TOSCALE are:
    %       'a1'
    %       'tai'
    %       'tdb'
    %       'tt'
    %       'utc'
    %   OUT = TIMECONV(T, FROMSCALE, TOSCALE, DAT) uses the conversion data
    %   DAT as required for some conversions.
    %   For conversions involving 'utc', DAT is a three-element
    %   vector of conversion factors from the GMAT data file tai-utc.dat.
    %
    %   The Modified Julian date for this function uses the GMAT definition
    %   (offset = 2430000).
    
    %% Process inputs
    p = inputParser();
    p.addRequired('t', @isfloat);
    p.addRequired('fromscale', @isvalidscale);
    p.addRequired('toscale', @isvalidscale);
    p.parse(t, fromscale, toscale);
    
    if any(strcmpi('utc', {fromscale, toscale}))
        p.addRequired('dat', @(x)isfloat(x) && length(x)==3);
        p.parse(t, fromscale, toscale, dat);
    end
    
    %% Convert all input scales to A1
    switch fromscale
        case 'a1'
            a1 = p.Results.t;
        case 'tai'
            a1 = tai2a1(p.Results.t);
        case 'tdb'
            tt = tdb2tt(p.Results.t);
            tai = tt2tai(tt);
            a1 = tai2a1(tai);
        case 'tt'
            tai = tt2tai(p.Results.t);
            a1 = tai2a1(tai);
        case 'utc'
            tai = utc2tai(p.Results.t, p.Results.dat);
            a1 = tai2a1(tai);
    end
    
    %% Convert from A1 to the final scale
    switch toscale
        case 'a1'
            out = a1;
        case 'tai'
            out = a12tai(a1);
        case 'tdb'
            tai = a12tai(a1);
            tt = tai2tt(tai);
            out = tt2tdb(tt);
        case 'tt'
            tai = a12tai(a1);
            out = tai2tt(tai);
        case 'utc'
            tai = a12tai(a1);
            out = tai2utc(tai, p.Results.dat);
    end
    
end

function tf = isvalidscale(s)
    validScales = {'a1', 'tai', 'tdb', 'tt', 'utc'};
    tf = any(strcmpi(s, validScales));
end

function tai = utc2tai(utc, conv)
    tai = utc + (conv(1) + (utc + 2430000 - 2400000.5 - conv(2)) * ...
        conv(3)) / (60*60*24);
end

function utc = tai2utc(tai, conv)
    utc = tai - (conv(1) + (tai + 2430000 - 2400000.5 - conv(2)) * ...
        conv(3)) / (60*60*24);
end

function a1 = tai2a1(tai)
    a1 = tai + 0.0343817/(60*60*24);
end

function tai = a12tai(a1)
    tai = a1 - 0.0343817/(60*60*24);
end

function tt = tai2tt(tai)
    tt = tai + 32.184/(60*60*24);
end

function tai = tt2tai(tt)
    tai = tt - 32.184/(60*60*24);
end

function tdb = tt2tdb(tt)
    ttj = tt + 2430000;
    ttt = (ttj - 2451545) / 36525;
    mE = mod(357.5277233 + 35999.05034 * ttt, 360);
    tdb = tt + (0.001658 * sind(mE) + 0.00001385 * sind(2*mE)) / ...
        (60*60*24);
end

function tt = tdb2tt(tdb)
    tdbfun = @(t)(tt2tdb(t) - tdb);
    tt = fzero(tdbfun, tdb);
end
