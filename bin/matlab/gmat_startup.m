%$Header: /GMAT/dev/cvs/supportfiles/matlab/gmat_startup.m,v 1.1 2007/08/22 19:15:30 shughes Exp $
%-------------------------------------------------------------------------------
% This file contains start-up scripts.
%
% Author: Linda Jun
% Created: 2004/09/18
%-------------------------------------------------------------------------------
format long g

GMAT_KEYWORD_DIR = './gmat_keyword'
GMAT_COMMAND_DIR = './gmat_command'
GMAT_OPT_DIR  = './gmat_fmincon'
addpath (GMAT_KEYWORD_DIR)
addpath (GMAT_COMMAND_DIR)
addpath (GMAT_OPT_DIR)
