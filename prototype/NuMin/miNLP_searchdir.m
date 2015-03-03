function  [px, q, plam, flag, W] = miNLP_searchdir(x,f,gradF,ce,ci,Je,Ji,A,b,Aeq,beq,eqInd,ineqInd,Options,mLE,mLI,m,W,lagMult)

%   Copyright 2002-2005, United States Government as represented by the
%   Administrator of The National Aeronautics and Space Administration.
%   All Rights Reserved.
%   This software is licensed under the NASA Open Source Agreement.
%
%   Modification History
%   -----------------------------------------------------------------------
%   Sept-08-08  S. Hughes:  Created the initial version.

if strcmp(Options.QPMethod,'minQP')
    
    %----- Setup the problem in terms of what minQP needs
    
    %  remove incompatible non-linear equality constraints
    %     jTemp = [];
    %     cTemp = [];
    %     for colIdx = 1:size(Je,2)
    %         if sum(abs(Je(:,colIdx))) > 1e-12
    %             jTemp = [jTemp,Je(colIdx,1)];
    %             cTemp = [cTemp,ce(colIdx,1)];
    %         end
    %     end
    %     Je = jTemp;
    %     ce = cTemp;
    %
    %     %  remove incompatible non-linear inequality constraints
    %     jTemp = [];
    %     cTemp = [];
    %     for colIdx = 1:size(Je,2)
    %         if sum(abs(Ji(:,colIdx))) > 1e-12
    %             jTemp = [jTemp,Ji(colIdx,1)];
    %             cTemp = [cTemp,ci(colIdx,1)];
    %         end
    %     end
    %     Ji = jTemp;
    %     ci = cTemp;
    
    %  Removes invalid constraint linearization for TP391 when xo = [0 0]'.
    %  Needs to be handled generally.
%     if ~isempty(Je) && sum(abs(Je)) <=1e-12
%         Je = [];
%         ce = [];
%         eqInd = eqInd(1);
%     end
    
    Aqp = [Aeq;Je';A;Ji'];
    bqp = [];
    if mLE > 0
        bqp = [-(Aeq*x - beq)];
    end
    bqp = [bqp;-ce];
    if mLI > 0
        bqp = [bqp;-(A*x-b)];
    end
    bqp = [bqp;-ci];
    QPoptions.Display = 0;
    [px, q, lambdaQP, flag, ActInd] = MinQP(x, W, gradF, Aqp, bqp, eqInd,...
        ineqInd, [], QPoptions, 2);
    
    %----- Extract data from solution to QP
    plam              = lambdaQP - lagMult;
    
else
    
    %-----  Use MATLAB's quadprog function to solve QP
    if mLI > 0
        bquadprog = (A*x-b);
    else
        bquadprog = [];
    end
    if mLE > 0
        beqquadprog = -(Aeq*x-beq);
    else
        beqquadprog = [];
    end
    [px,q,flag, OUTPUT,lambdadata] = quadprog(W,gradF,[-A;-Ji'],[bquadprog;ci],...
        [Aeq;Je'],[beqquadprog;-ce],[],[],x,[]);
    lambdanew = [lambdadata.eqlin ;lambdadata.ineqlin];
    plam = lambdanew - lagMult;
    
end
