//$Header: /cygdrive/p/dev/cvs/test/TestScriptInterpreter/driver.hpp,v 1.1 2005/11/17 22:51:53 dconway Exp $
//------------------------------------------------------------------------------
//                           TestScriptInterpreter driver
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
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
void TestParser(std::string optionParm);
void SaveScript(std::string filename = "Output.script");
void ShowCommandSummary(std::string filename = "");
void TestSyncModeAccess(std::string filename = "Output.script");
int main(int argc, char *argv[]);

#endif /* driver_hpp */
