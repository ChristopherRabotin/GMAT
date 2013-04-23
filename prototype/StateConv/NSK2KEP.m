
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   conversion Nonsingular to Keplerian                          %    
%   by HYKim, CBNU                                               %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function kepler = NSK2KEP(Nonsingular)

% Initializations
a = Nonsingular(1);
e1 = Nonsingular(2);
e2 = Nonsingular(3);
e3 = Nonsingular(4);
e4 = Nonsingular(5);
e5 = Nonsingular(6);

% Sanity check input
    if max(size(Nonsingular))~= 6
        kepler = [];
        disp('Error: Nonsingualr Keplerian have six elements')
        return
    end

    SMA = a;
    
    % Find RAAN
    if(e3 == 0 && e4 == 0)
        INC = 0;
        RAAN = 0;    % RAAN is not defined.
    else
        RAAN = atan2(e3,e4);
        % Find eccentricity
        INC = 2*asin(sqrt(e3^2+e4^2));
    end
    RAAN = mod(RAAN,2*pi);
    % check if the AOP is less than 0
    if(RAAN < 0)
        RAAN = RAAN + 2*pi;
    end
    
    
    % Find AOP
    if (e1 == 0) && (e2 == 0)
        ECC = 0;
        AOP = 0;  % Argument of perigee is not defined.
    else
        AOP = - RAAN + atan2(e2,e1);
        % Find Eccentricity
        ECC = sqrt(e1^2 + e2^2);  
    end
    AOP = mod(AOP,2*pi);
    % check if the AOP is less than 0
    if(AOP < 0)
        AOP = AOP + 2*pi;
    end
    
    % Find Mean Anomaly
    MA = mod(e5-AOP-RAAN,2*pi);
    if(MA < 0)
        MA = MA + 2*pi;
    end
    
    % Find True Anomaly
    TA = M2nu(MA,ECC);

%     Output
%     kepler = real([SMA ECC INC AOP RAAN TA]);
    kepler = real([SMA ECC INC RAAN AOP TA])';
    
    
    
    