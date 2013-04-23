
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%   conversion Keplerian to Nonsinular Keplerian                 %    
%   by HYKim, CBNU                                               %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


function Nonsingular = KEP2NSK(kepler)

% d2r = pi/180;
% r2d = 180/pi;

% Initializations
%     SMA = kepler(1);
%     ECC= kepler(2);
%     INC = kepler(3);
%     AOP = kepler(4);
%     RAAN = kepler(5); 
%     TA = kepler(6);

    SMA = kepler(1);
    ECC= kepler(2);
    INC = kepler(3);
    RAAN = kepler(4);
    AOP = kepler(5); 
    TA = kepler(6);


    
% Sanity check input
if max(size(kepler))~= 6
    Nonsingular = [];
    disp('Error: Classical element have six elements')
    return
end 

MA = nu2M(TA,ECC);

    a = SMA;
    e1 = ECC*cos(RAAN+AOP);                      % Nonsigular element = e1
    e2 = ECC*sin(RAAN+AOP);                      % Nonsigular element = e2
    e3 = sin(INC/2)*sin(RAAN);                   % Nonsigular element = e3
    e4 = sin(INC/2)*cos(RAAN);                   % Nonsigular element = e4
    e5 = (RAAN + AOP + MA);                        % Nonsigular element = e5 (Mean Longitude)
    
%     Output
Nonsingular = real([a e1 e2 e3 e4 e5])';     

