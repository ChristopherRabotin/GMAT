//$Id$
//------------------------------------------------------------------------------
//                           TestScriptInterpreter driver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2003/08/28
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Function prototypes for TestScriptInterpreter.
 */
//------------------------------------------------------------------------------


#ifndef driver_hpp
#define driver_hpp

#include <iostream>
#include "gmatdefs.hpp"


enum {
        RUN_SCRIPT = 2001,
        RUN_BATCH,
        SAVE,
        SUMMARY,
        PARSE,
        HELP,
        VERBOSE
     };

void ShowHelp();
void RunScriptInterpreter(std::string script, int verbosity, 
                          bool batchmode = false);
Integer RunBatch(std::string& batchfilename);
void SaveScript(std::string filename = "");
void ShowVersionInfo();
void ShowCommandSummary(std::string filename = "");
void DumpDEData(double secsToStep, double spanInSecs = 86400.0);
int main(int argc, char *argv[]);

#endif /* driver_hpp */
