function test_suite = testDela()                          %#ok<STOUT>
%TESTDELA Unit tests for Delaunay state representation
clear all; clc;

initTestSuite();

end

function ret = setup()                                      %#ok<*DEFNU>
addpath('..');
ret.tol = 1e-6;
ret.mu = 398600.4418;
end

%% Circular EqPro
function testCart2DelaCircularEqPro(inp)
cart2dela('CircularEqPro.txt', inp);
end

function testDela2CartCircularEqPro(inp)
dela2cart('CircularEqPro.txt', inp);
end

%% Circular IncPro
function testCart2DelaCircularIncPro(inp)
cart2dela('CircularIncPro.txt', inp);
end

function testDela2CartCircularIncPro(inp)
dela2cart('CircularIncPro.txt', inp);
end

%% Circular Polar
function testCart2DelaCircularPolar(inp)
cart2dela('CircularPolar.txt', inp);
end

function testDela2CartCircularPolar(inp)
dela2cart('CircularPolar.txt', inp);
end

%% Circular IncRetro
function testCart2DelaCircularIncRetro(inp)
cart2dela('CircularIncRetro.txt', inp);
end

function testDela2CartCircularIncRetro(inp)
dela2cart('CircularIncRetro.txt', inp);
end

%% Circular EqRetro
function testCart2DelaCircularEqRetro(inp)
cart2dela('CircularEqRetro.txt', inp);
end

function testDela2CartCircularEqRetro(inp)
dela2cart('CircularEqRetro.txt', inp);
end

%% Elliptic EqPro
function testCart2DelaEllipticEqPro(inp)
cart2dela('EllipticEqPro.txt', inp);
end

function testDela2CartEllipticEqPro(inp)
dela2cart('EllipticEqPro.txt', inp);
end

%% Elliptic IncPro
function testCart2DelaEllipticIncPro(inp)
cart2dela('EllipticIncPro.txt', inp);
end

function testDela2CartEllipticIncPro(inp)
dela2cart('EllipticIncPro.txt', inp);
end

%% Elliptic Polar
function testCart2DelaEllipticPolar(inp)
cart2dela('EllipticPolar.txt', inp);
end

function testDela2CartEllipticPolar(inp)
dela2cart('EllipticPolar.txt', inp);
end

%% Elliptic IncRetro
function testCart2DelaEllipticIncRetro(inp)
cart2dela('EllipticIncRetro.txt', inp);
end

function testDela2CartEllipticIncRetro(inp)
dela2cart('EllipticIncRetro.txt', inp);
end

%% Elliptic EqRetro
function testCart2DelaEllipticEqRetro(inp)
cart2dela('EllipticPolar.txt', inp);
end

function testDela2CartEllipticEqRetro(inp)
dela2cart('EllipticPolar.txt', inp);
end

%% Hyperbolic EqPro
function testCart2DelaHyperbolicEqPro(inp)
cart2dela('HyperbolicEqPro.txt', inp);
end

function testDela2CartHyperbolicEqPro(inp)
dela2cart('HyperbolicEqPro.txt', inp);
end

%% Hyperbolic IncPro
function testCart2DelaHyperbolicIncPro(inp)
cart2dela('HyperbolicIncPro.txt', inp);
end

function testDela2CartHyperbolicIncPro(inp)
dela2cart('HyperbolicIncPro.txt', inp);
end

%% Hyperbolic Polar
function testCart2DelaHyperbolicPolar(inp)
cart2dela('HyperbolicPolar.txt', inp);
end

function testDela2CartHyperbolicPolar(inp)
dela2cart('HyperbolicPolar.txt', inp);
end

%% Hyperbolic IncRetro
function testCart2DelaHyperbolicIncRetro(inp)
cart2dela('HyperbolicIncRetro.txt', inp);
end

function testDela2CartHyperbolicIncRetro(inp)
dela2cart('HyperbolicIncRetro.txt', inp);
end

%% Hyperbolic EqRetro
function testCart2DelaHyperbolicEqRetro(inp)
cart2dela('HyperbolicEqRetro.txt', inp);
end

function testDela2CartHyperbolicEqRetro(inp)
dela2cart('HyperbolicEqRetro.txt', inp);
end

%% Null Orbit
% % function testCart2DelaNull(inp)
% % cart2dela('Null.truth', inp);
% % end
% % 
% % function testDela2CartNull(inp)
% % dela2cart('Null.truth', inp);
% % end
% % 
% % function teardown(~)
% % rmpath('..');
% % end

%% Common Functions
function cart2dela(tfilename, params)
truthFile = fullfile('truth', 'Delaunay', 'STK8', ...
    tfilename);
fid = fopen(truthFile);
cartSTK = textscan(fid, '%f', 6, 'HeaderLines', 6);
delaSTK = textscan(fid, '%f', 6, 'HeaderLines', 5);
fclose(fid);
delaSTK{1}(1) = delaSTK{1}(1) * 1e-6;
delaSTK{1}(2) = delaSTK{1}(2) * 1e-6;
delaSTK{1}(3) = delaSTK{1}(3) * 1e-6;
delaSTK{1}(4) = delaSTK{1}(4) * pi()/180;
delaSTK{1}(5) = delaSTK{1}(5) * pi()/180;
delaSTK{1}(6) = delaSTK{1}(6) * pi()/180;
assertElementsAlmostEqual(stateconv(cartSTK{1}, 1, 9, params.mu), ...
    delaSTK{1}, 'relative', params.tol);
end

function dela2cart(tfilename, params)
truthFile = fullfile('truth', 'Delaunay', 'STK8', ...
    tfilename);
fid = fopen(truthFile);
cartSTK = textscan(fid, '%f', 6, 'HeaderLines', 6);
delaSTK = textscan(fid, '%f', 6, 'HeaderLines', 5);
fclose(fid);
delaSTK{1}(1) = delaSTK{1}(1) * 1e-6;
delaSTK{1}(2) = delaSTK{1}(2) * 1e-6;
delaSTK{1}(3) = delaSTK{1}(3) * 1e-6;
delaSTK{1}(4) = delaSTK{1}(4) * pi()/180;
delaSTK{1}(5) = delaSTK{1}(5) * pi()/180;
delaSTK{1}(6) = delaSTK{1}(6) * pi()/180;
assertElementsAlmostEqual(stateconv(delaSTK{1}, 9, 1, params.mu), ...
    cartSTK{1}, 'relative', params.tol);
end