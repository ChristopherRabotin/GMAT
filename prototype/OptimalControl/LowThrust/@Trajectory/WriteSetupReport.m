%  Report data about the problem
function WriteSetupReport(obj)

% number of phases
disp(' -----------------------------------------------------------');
disp(' ----------- GMAT Optimal Control SetUp Summary ------------');
disp(' -----------------------------------------------------------');

%  Write out function data
for phaseIdx = 1:obj.numPhases;
    str = sprintf('%s %3i',' PHASE NUMBER        : ',...
        phaseIdx);
    disp(str);
    %  Dynamics function information
    str = sprintf('%s %3i %s  %3i',' Dynamics Functions  : ',...
        obj.phaseList{phaseIdx}.PathFunction.numDynFunctions,...
        '    Dynamics Constraints  :',...
        obj.phaseList{phaseIdx}.numDefectConstraints);
    disp(str)
    %  Algebraic path function information
    str = sprintf('%s %3i %s  %3i',' Algebraic Functions : ',...
        obj.phaseList{phaseIdx}.PathFunction.numAlgFunctions,...
        '    Algebraic Constraints :',...
        obj.phaseList{phaseIdx}.numPathConstraints);
    disp(str)
    %  Integral Function information
    str = sprintf('%s %3i %s  %3i',' Integral Functions  : ',...
        obj.phaseList{phaseIdx}.PathFunction.numIntFunctions,...
        '    Integral Constraints  :',...
        obj.phaseList{phaseIdx}.numEventConstraints);
    disp(str)
    %  Point function information
    str = sprintf('%s %3i %s  %3i',' Point Functions     : ',...
        obj.phaseList{phaseIdx}.PointFunction.numEventFunctions,...
        '    Total Num Constraints :',...
        obj.phaseList{phaseIdx}.numConstraints);
    disp(str)
    
    %  Cost Function information (integral)
    if obj.phaseList{phaseIdx}.PathFunction.hasCostFunction
        flagStr = 'true';
    else
        flagStr = 'false';
    end
    str = sprintf('%s %s',' Has Integral Cost   : ',flagStr);
    disp(str)
    %  Cost Function information (point)
    if obj.phaseList{phaseIdx}.PointFunction.hasCostFunction
        flagStr = 'true';
    else
        flagStr = 'false';
    end
    str = sprintf('%s %s',' Has Point Cost      : ',flagStr);
    disp(str)
    disp('  ')
    
    %  Write out parameterization data
    str = sprintf('%s %3i %s  %3i',' Number of States    : ',...
        obj.phaseList{phaseIdx}.numStates,...
        '    State Parameters      :',...
        obj.phaseList{phaseIdx}.numStateParams);
    disp(str)
    str = sprintf('%s %3i %s  %3i',' Number of Controls  : ',...
        obj.phaseList{phaseIdx}.numControls,...
        '    Control Parameters    :',...
        obj.phaseList{phaseIdx}.numControlParams);
    disp(str)
    str = sprintf('%s %5i',' Decision Parameters : ',...
        obj.phaseList{phaseIdx}.numDecisionParams);
    disp(str)
    
    %  Write space between sections if not the last section
    if phaseIdx ~= obj.numPhases
        disp(' -----------------------------------------------------------')
    end
end
disp(' -----------------------------------------------------------');
disp(' -----------------------------------------------------------');
disp(' -----------------------------------------------------------');
end