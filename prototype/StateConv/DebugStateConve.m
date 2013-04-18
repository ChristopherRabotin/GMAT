
format long g
mu = 398600.1145;
orbitIdx = 1;
inputIdx = 4;
outputIdx = 6;
oe           = [7000 0 pi/2 0 0 pi/2];
stateIn      = stateconv(oe,2,inputIdx,mu);
stateOut     = stateconv(stateIn,inputIdx,outputIdx,mu);
stateCompare = stateconv(stateOut,outputIdx,inputIdx,mu);

stateconv(stateIn,inputIdx,1,mu)
stateconv(stateCompare,inputIdx,1,mu)