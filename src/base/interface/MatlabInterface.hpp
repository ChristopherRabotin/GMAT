//$Header$
//------------------------------------------------------------------------------
//                             MatlabInterface
//------------------------------------------------------------------------------
// Purpose:
//    declares MatlabInterface class.
//
// Modifications:
//    11/04/02  L. Jun - Created.
//------------------------------------------------------------------------------
#ifndef h_MatlabInterface_h
#define h_MatlabInterface_h

#include "engine.h"      // for Matlab Engine
#include <string>

class MatlabInterface
{

public:

static int Open();
static int IsOpen();
static int Close();
static int PutVariable(const std::string &matlabVarName, int numElements, double inArray[]);
static int GetVariable(const std::string &matlabVarName, int numElements, double outArray[]);
static int EvalString(const std::string &evalString);
static int OutputBuffer(char *buffer, int size);

private:

MatlabInterface();
~MatlabInterface();

static Engine *enginePtrD;
static mxArray *mxArrayInputPtrD;
static mxArray *mxArrayOutputPtrD;

};

#endif // h_MatlabInterface_h
