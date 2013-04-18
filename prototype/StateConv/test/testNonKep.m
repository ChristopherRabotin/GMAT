function test_suite = testNonKep()                  %#ok<STOUT>
%TESTNONKEP Unit tests for Nonsingular Keplerian state representation

initTestSuite();

end

function ret = setup()                                      %#ok<*DEFNU>
addpath('..');
ret.tol = 1e-13;
ret.mu = 398600.4418;
end

%% Circular EqPro
function testCart2NonKepCircularEqPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Circular_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(cartFF{1}, 1, 8, inp.mu), nkFF{1}, ...
    'relative', inp.tol);
end

function testNonKep2CartCircularEqPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Circular_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(nkFF{1}, 8, 1, inp.mu), cartFF{1}, ...
    'relative', inp.tol);
end

%% Circular IncPro
function testCart2NonKepCircularIncPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Circular_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(cartFF{1}, 1, 8, inp.mu), nkFF{1}, ...
    'relative', inp.tol);
end

function testNonKep2CartCircularIncPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Circular_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(nkFF{1}, 8, 1, inp.mu), cartFF{1}, ...
    'relative', inp.tol);
end

%% Circular Polar
function testCart2NonKepCircularPolar(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Circular_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(cartFF{1}, 1, 8, inp.mu), nkFF{1}, ...
    'relative', inp.tol);
end

function testNonKep2CartCircularPolar(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Circular_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(nkFF{1}, 8, 1, inp.mu), cartFF{1}, ...
    'relative', inp.tol);
end

%% Elliptic EqPro
function testCart2NonKepEllipticEqPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Elliptic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(cartFF{1}, 1, 8, inp.mu), nkFF{1}, ...
    'relative', inp.tol);
end

function testNonKep2CartEllipticEqPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Elliptic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(nkFF{1}, 8, 1, inp.mu), cartFF{1}, ...
    'relative', inp.tol);
end

%% Elliptic IncPro
function testCart2NonKepEllipticIncPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Elliptic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(cartFF{1}, 1, 8, inp.mu), nkFF{1}, ...
    'relative', inp.tol);
end

function testNonKep2CartEllipticIncPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Elliptic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(nkFF{1}, 8, 1, inp.mu), cartFF{1}, ...
    'relative', inp.tol);
end

%% Elliptic Polar
function testCart2NonKepEllipticPolar(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Elliptic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(cartFF{1}, 1, 8, inp.mu), nkFF{1}, ...
    'relative', inp.tol);
end

function testNonKep2CartEllipticPolar(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Elliptic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(nkFF{1}, 8, 1, inp.mu), cartFF{1}, ...
    'relative', inp.tol);
end

%% Hyperbolic EqPro
function testCart2NonKepHyperbolicEqPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Hyperbolic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(cartFF{1}, 1, 8, inp.mu), nkFF{1}, ...
    'relative', inp.tol);
end

function testNonKep2CartHyperbolicEqPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Hyperbolic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(nkFF{1}, 8, 1, inp.mu), cartFF{1}, ...
    'relative', inp.tol);
end

%% Hyperbolic IncPro
function testCart2NonKepHyperbolicIncPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Hyperbolic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(cartFF{1}, 1, 8, inp.mu), nkFF{1}, ...
    'relative', inp.tol);
end

function testNonKep2CartHyperbolicIncPro(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Hyperbolic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(nkFF{1}, 8, 1, inp.mu), cartFF{1}, ...
    'relative', inp.tol);
end

%% Hyperbolic Polar
function testCart2NonKepHyperbolicPolar(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Hyperbolic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(cartFF{1}, 1, 8, inp.mu), nkFF{1}, ...
    'relative', inp.tol);
end

function testNonKep2CartHyperbolicPolar(inp)
truthFile = fullfile('truth', 'NonsingularKeplerian', 'FF', ...
    'Hyperbolic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
nkFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(stateconv(nkFF{1}, 8, 1, inp.mu), cartFF{1}, ...
    'relative', inp.tol);
end

function teardown(~)
rmpath('..');
end