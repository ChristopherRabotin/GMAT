//$Id: MatlabFunctions.hpp 9540 2011-05-18 00:02:57Z djcinsb $
//------------------------------------------------------------------------------
//                            GmatOdtbxFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
// ODTBX: Orbit Determination Toolbox
//
// **Legal**
//
// Developed jointly by NASA/GSFC, Emergent Space Technologies, Inc.
// and Thinking Systems, Inc. under the FDSS contract, Task 28
//
// Author: Darrel J. Conway (Thinking Systems)
// Created: 2011/05/17
//
/**
 * Functions called on the ODTBX side of the interface
 *
 * This file is parsed by MATLAB to define the functions that are accessed using 
 * MATLAB's loadlibrary/calllib functions. These are all pure C functions; the 
 * file is loaded by GmatOdtbxFunctions.hpp when building the interface library.
 */
//------------------------------------------------------------------------------

#ifndef MatlabFunctions_hpp
#define MatlabFunctions_hpp

#include "GmatCFunc_defs.hpp"

// Interfaces used by ODTBX
const char CINTERFACE_API *getLastMessage();
int CINTERFACE_API StartGmat();

int CINTERFACE_API LoadScript(const char* scriptName);
int CINTERFACE_API RunScript();
int CINTERFACE_API LoadAndRunScript(const char* scriptName);

int CINTERFACE_API FindOdeModel(const char* modelName);
int CINTERFACE_API GetStateSize();
const char CINTERFACE_API *GetStateDescription();
int CINTERFACE_API SetState(double epoch, double state[], int stateDim);
double CINTERFACE_API *GetState();

double CINTERFACE_API *GetDerivativesForState(double epoch, double state[], 
      int stateDim, double dt, int order, int *pdim);
double CINTERFACE_API *GetDerivatives(double dt, int order, int *pdim);

int CINTERFACE_API CountObjects();
const char CINTERFACE_API *GetObjectName(int which);
const char CINTERFACE_API *GetRunSummary();

#endif /*GMATPLUGINFUNCTIONS_H_*/
