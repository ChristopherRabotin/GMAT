%
%   Copyright 2002-2005, United States Government as represented by the
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.
clear all
clc

%-----  Set Options Structure for miNLP
Options.DescentMethod     = 'SelfScaledBFGS';
Options.StepSearchMethod  = 'NocWright';
Options.FiniteDiffVector  = ones(5,1)*1e-9;
Options.DerivativeMethod  = 'Analytic';
Options.MaxIter           = 1350;
Options.MaxFunEvals       =  1750;
Options.TolCon            = 1e-8;
Options.TolX              = 1e-8;
Options.TolF              = 1e-8;
Options.TolGrad           = 1e-8;
Options.TolStep           = 1e-8;
Options.MaxStepLength     = 1000;
Options.QPMethod          = 'minQP';

%-----  Define names of test problems
ProblemSet = {
    'TP239';
    'TP238';
    'TP395';
    'TP394';
    'TP246'; 
    'TP201';
    'TP328'; 
    'TP6'; 
    'PLR_T1_4'; 
    'QLR_T1_1';
    'SGR_P1_2';   
    'TP6';'TP1';  
    'TP218';
    'TP369';    
    'TP242'; 
    'TP225';
    'TP254'; 
    'PQR_T1_6'; 
    'LQR_T1_4';
    'LLR_T1_1'
    'LPR_T1_1'; 
    'LPR_T1_2'
    'LPR_T1_5';  
    'SGR_P1_2'; 
    'PLR_T1_2';
    'SLR_T1_1';
    'QLR_T1_2'};

%-----  Define names of test problems
% Starting this problem at x0 = [0 0] results in NaNs.
%ProblemSet = {'TP319';  };


%-----  Loop over problem set and solve each one
for i = 1:size(ProblemSet,1);
    
    %----- Set up problem data
    name    = ProblemSet{i};
    objname = ['OBJ_' name];
    conname = ['CON_' name];
    d       = feval(ProblemSet{i});
    RunData{i}.func = name;
    
    %----- Call miNLP and save data
    [x,f,exitFlag,OutPut]      = miNLP(objname,d.x0,d.A,d.b,d.Aeq,d.beq,d.lb,d.ub,conname,Options);
    RunData{i}.miNLP.x         = x;
    RunData{i}.miNLP.f         = f;
    RunData{i}.miNLP.exitFlag  = exitFlag;
    RunData{i}.miNLP.iter      = OutPut.iter;
    RunData{i}.miNLP.fevals    = OutPut.fevals;
    RunData{i}.xstar           = d.xstar;
    RunData{i}.fstar           = d.fstar;

end

% =========================================================================
% =======================  Output Summary Data  ===========================
% =========================================================================
header = sprintf('\n                  Test         exit ');
disp(header);
header = sprintf(' Solver          Problem        Flag    abs(f(x)-f(x*))     norm(x* - x)    Iter    Feval');
formatstr = '%s %16s      %5.0f    %15.6g  %15s   %6.0d  %6.0d';
disp(header)
disp('=====================================================================================================')

for i = 1:size(RunData,2)
   
    Solver = 'miNLP  ';
    Ferror = abs(RunData{i}.miNLP.f  - RunData{i}.fstar);
    if ~strcmp(RunData{i}.xstar,'N/A')
        Xerror = sprintf('%15.6g', norm(RunData{i}.miNLP.x  - RunData{i}.xstar));
    else
        Xerror = 'N/A';
    end
    iterdata = sprintf(formatstr, Solver, RunData{i}.func, RunData{i}.miNLP.exitFlag,Ferror,Xerror,...
        RunData{i}.miNLP.iter, RunData{i}.miNLP.fevals );
    disp(iterdata)
    
end




