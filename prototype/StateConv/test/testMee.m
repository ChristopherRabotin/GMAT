function test_suite = testMee()                             %#ok<STOUT>
%TESTMEE Unit tests for Modified Equinoctial state representation

initTestSuite();

end

function ret = setup()                                      %#ok<*DEFNU>
addpath('..');
tol = 1e-13;
mu = 398600.4418;
ret = {tol, mu};
end

%% Circular EqPro
function testCart2MeeCircularEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartCircularEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Circular IncPro
function testCart2MeeCircularIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartCircularIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Circular Polar
function testCart2MeeCircularPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartCircularPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Circular_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Elliptic EqPro
function testCart2MeeEllipticEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartEllipticEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Elliptic IncPro
function testCart2MeeEllipticIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartEllipticIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Elliptic Polar
function testCart2MeeEllipticPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartEllipticPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Elliptic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Hyperbolic EqPro
function testCart2MeeHyperbolicEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartHyperbolicEqPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_EqPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Hyperbolic IncPro
function testCart2MeeHyperbolicIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartHyperbolicIncPro(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_IncPro.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

%% Hyperbolic Polar
function testCart2MeeHyperbolicPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Cart2Mee(cartFF{1}, inp{2}), meeFF{1}, ...
    'relative', inp{1});
end

function testMee2CartHyperbolicPolar(inp)
truthFile = fullfile('truth', 'ModifiedEquinoctial', 'FF', ...
    'Hyperbolic_Polar.truth');
fid = fopen(truthFile);
cartFF = textscan(fid, '%f', 6, 'HeaderLines', 3);
meeFF = textscan(fid, '%f', 6, 'HeaderLines', 4);
fclose(fid);
assertElementsAlmostEqual(Mee2Cart(meeFF{1}, inp{2}), cartFF{1}, ...
    'relative', inp{1});
end

function teardown(~)
rmpath('..');
end