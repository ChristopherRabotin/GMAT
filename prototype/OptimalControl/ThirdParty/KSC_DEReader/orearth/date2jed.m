function jed = date2jed(date)

% DATE2JED Calculate Julian date
%     DATE2JED(DATE) calculates the Julian date from the date
%     vector DATE which has length 3 or 6.
%     
%     Input date as an vector:
%         date(1) <-> the year (all digits, e.g. 1963, NOT 63)
%         date(2) <-> month
%         date(3) <-> day of month
%         date(4) <-> UTC hour      ]
%         date(5) <-> UTC minute    ] -> optional
%         date(6) <-> UTC second    ]
%
%     If DATE is n-by-3 or n-by-6, DATE2JED will return a vector
%     of n corresponding Julian dates.

jed=datenum(date)+1721058.5;