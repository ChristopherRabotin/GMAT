function mjd = g2mj(datevector)
    %G2MJ Convert Gregorian date to Modified Julian.
    %   MJD = G2MJ(DATEVECTOR) converts a 6-element date vector DATEVECTOR
    %   from Gregorian to Modified Julian format.
    %
    %   This conversion is valid only from years 1900 to 2100. It uses the
    %   GMAT definition of the Modified Julian date.
    %
    %   Reference:
    %       Vallado, D. A. "ALGORITHM 14: Julian Date," "Fundamentals of
    %       Astrodynamics and Applications," 2nd ed., Microcosm Press, El
    %       Segundo, CA, 2001. p. 186.
    
    %   Contact: Joel J. K. Parker <joel.j.k.parker@nasa.gov>
    %   See SVN for revision history.
    
    p = inputParser();
    p.addRequired('datevector', @(x)isfloat(x) && length(x) == 6);
    p.parse(datevector);
    
    % normalize date vector and extract components
    [yr, mo, d, h, min, s] = datevec(datenum(p.Results.datevector));
    
    % calculate MJD
    jDay = 367 * yr - fix(7 * (yr + fix((mo + 9) / 12)) / 4) + ...
        fix(275 * mo / 9) + d + 1721013.5;
    partOfDay = ((s/60 + min) / 60 + h) / 24;
    mjd = (jDay - 2430000) + partOfDay;
    
end
