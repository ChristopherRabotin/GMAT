function date = jed2date(jed)

% DATE2JED Calculate date vector from Julian Date
%     JED2DATE(JED) calculates the 1-by-6 date 
%     vector from the Julian date JED.
%     
%     Output is date as a vector:
%         date(1) <-> the year 
%         date(2) <-> month
%         date(3) <-> day of month
%         date(4) <-> UTC hour
%         date(5) <-> UTC minute
%         date(6) <-> UTC second

date=datevec(jed-1721058.5);