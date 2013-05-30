function datevec = mj2g(mjd)
    %MJ2G Convert Modified Julian date to Gregorian.
    %   DATEVEC = MJ2G(MJD) returns a 6-element date vector DATEVEC that
    %   corresponds to the Gregorian form of the Modified Julian date MJD.
    %
    %   This conversion is valid only from years 1900 to 2100. It uses the
    %   GMAT definition of the Modified Julian date.
    %
    %   Reference:
    %       Vallado, D. A. "ALGORITHM 22: JDtoGregorianDate," "Fundamentals
    %       of Astrodynamics and Applications," 2nd ed., Microcosm Press,
    %       El Segundo, CA, 2001. p. 204.
    
    %   Contact: Joel J. K. Parker <joel.j.k.parker@nasa.gov>
    %   See SVN for revision history.
    
    p = inputParser();
    p.addRequired('mjd', @(x)isfloat(x) && length(x) == 1);
    p.parse(mjd);
    mjd = p.Results.mjd;
    
    lMonth = [31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31];
    jd = mjd + 2430000;
    
    t1900 = (jd - 2415019.5) / 365.25;
    year = 1900 + fix(t1900);
    leapYrs = fix((year - 1900 - 1)*0.25);
    days = (jd - 2415019.5) - ((year - 1900) * 365 + leapYrs);
    
    if days < 1
        year = year - 1;
        leapYrs = fix((year - 1900 - 1)*0.25);
        days = (jd - 2415019.5) - ((year - 1900) * 365 + leapYrs);
    end
    
    if mod(year, 4) == 0
        lMonth(2) = 29;
    end
    
    dayOfYr = fix(days);
    
    sumDays = 0;
    for i = 1:length(lMonth)
        sumDaysTemp = sumDays + lMonth(i);
        
        if sumDaysTemp >= dayOfYr
            break;
        else
            sumDays = sumDaysTemp;
        end
    end
    mon = i;
    
    day = dayOfYr - sumDays;
    tau = (days - dayOfYr) * 24;
    h = fix(tau);
    min = fix((tau - h) * 60);
    s = (tau - h - min/60) * 3600;
    
    datevec = [year mon day h min s];
    
end
