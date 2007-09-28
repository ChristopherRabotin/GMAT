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

#if defined __USE_MATLAB__
#include "engine.h"      // for Matlab Engine
#endif

#include <string>

class MatlabInterface
{

public:

static int  Open();
static int  Close();
static int  PutRealArray(const std::string &matlabVarName, int numElements, double inArray[]);
static int  GetRealArray(const std::string &matlabVarName, int numElements, double outArray[]);
static int  GetString(const std::string &matlabVarName, std::string &outStr);
static int  EvalString(const std::string &evalString);
static int  OutputBuffer(char *buffer, int size);
static bool IsOpen();
static void RunMatlabString(std::string evalString); 


private:

MatlabInterface();
~MatlabInterface();

#if defined __USE_MATLAB__
static Engine *enginePtrD;
static mxArray *mxArrayInputPtrD;
static mxArray *mxArrayOutputPtrD;

static int accessCount;
#endif

};

#endif // h_MatlabInterface_h
