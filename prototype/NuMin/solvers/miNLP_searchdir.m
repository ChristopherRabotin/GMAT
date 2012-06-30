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
    [px, q, lambdaQP, flag, ActInd] = minQP(x, W, gradF, Aqp, bqp, eqInd,...
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
