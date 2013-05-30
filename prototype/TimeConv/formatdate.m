function dateStr = formatdate(dateVec)
    %FORMATDATE Format a Gregorian date vector into a string.
    %   DATESTR = FORMATDATE(DATEVEC) returns a Gregorian date-formatted
    %   string representation of the 6-element date vector DATEVEC.

    %   Contact: Joel J. K. Parker <joel.j.k.parker@nasa.gov>
    %   See SVN for revision history.
    
    monthNames = {
        'Jan'
        'Feb'
        'Mar'
        'Apr'
        'May'
        'Jun'
        'Jul'
        'Aug'
        'Sep'
        'Oct'
        'Nov'
        'Dec'
        };
    
    dateStr = sprintf('%02u %s %4u %02u:%02u:%06.3f', dateVec(3), ...
        monthNames{dateVec(2)}, dateVec(1), dateVec(4), dateVec(5), ...
        dateVec(6));
    
end
