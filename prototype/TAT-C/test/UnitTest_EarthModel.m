% Unit test script for Earth class

cb = Earth();

GMT = cb.ComputeGMT(2457260.12345679)

%  Need to verify the sign.
if abs(GMT*180/pi + 198.002628503035)
   error('error calcualing GMT')
end