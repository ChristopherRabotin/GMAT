//$Header$
//------------------------------------------------------------------------------
//                             MatlabInterface
//------------------------------------------------------------------------------
//  Purpose:
//     Implements MatlabInterface.hpp class.
//
//  Modifications:
//     11/04/02  L. Jun - Created.
//------------------------------------------------------------------------------
#include <stdlib.h>         // for NULL
#include <string.h>         // for memcpy()
#if defined __USE_MATLAB__
#include "engine.h"         // for Engine
#endif

#include "MatlabInterface.hpp" // for MatlabInterface methods

//--------------------------------------
//  initialize static variables
//--------------------------------------
#if defined __USE_MATLAB__
Engine* MatlabInterface::enginePtrD = 0;
mxArray* MatlabInterface::mxArrayInputPtrD = 0;
mxArray* MatlabInterface::mxArrayOutputPtrD = 0;
#endif

//--------------------------------------
//  public functions
//--------------------------------------

//------------------------------------------------------------------------------
//  int Open()
//------------------------------------------------------------------------------
//  Purpose:
//     Opens Matlab engine.
//
//  Returns:
//     1 = no error, 0 = error
//------------------------------------------------------------------------------
int MatlabInterface::Open()
{
#if defined __USE_MATLAB__

   if ((enginePtrD = engOpen(NULL)))
      return 1;
   else
      return 0;
#endif

   return 0;
} // end Open()

//------------------------------------------------------------------------------
//  int Close()
//------------------------------------------------------------------------------
//  Purpose:
//     Closes Matlab engine.
//
//  Returns:
//     1 = no error, 0 = error
//------------------------------------------------------------------------------
int MatlabInterface::Close()
{
#if defined __USE_MATLAB__
   engClose(enginePtrD);
   enginePtrD = NULL;      // set to NULL, so it can be reopened
#endif
   return 1;

} // end Close()


//------------------------------------------------------------------------------
//  int PutVariable(const std::string &matlabVarName, int numElements, double inArray[])
//------------------------------------------------------------------------------
//  Purpose:
//     Put arrays to Matlab workspace.
//
//  Returns:
//     1 = no error, 0 = error
//------------------------------------------------------------------------------
int MatlabInterface::PutVariable(const std::string &matlabVarName, int numElements,
                        double inArray[])
{
#if defined __USE_MATLAB__

   // create a matlab variable
   mxArrayInputPtrD = mxCreateDoubleMatrix(1, numElements, mxREAL);

   memcpy((char*)mxGetPr(mxArrayInputPtrD), (char*)inArray, numElements*sizeof(double));

   // place the variable mxArrayInputPtrD into the MATLAB workspace
   engPutVariable(enginePtrD, matlabVarName.c_str(), mxArrayInputPtrD);
//   engPutArray(enginePtrD, mxArrayInputPtrD);
#endif

   return 1;
   
} // end PutVariable()


//------------------------------------------------------------------------------
//  int GetVariable(const std::string &matlabVarName, double outArray[])
//------------------------------------------------------------------------------
//  Purpose:
//     Get arrays from Matlab workspace.
//
//  Returns:
//     1 = no error, 0 = error
//------------------------------------------------------------------------------
int MatlabInterface::GetVariable(const std::string &matlabVarName, int numElements,
                           double outArray[])
{
#if defined __USE_MATLAB__
//   int i;
   mxArrayOutputPtrD = NULL;
   
   // get the variable from the MATLAB workspace
   mxArrayOutputPtrD = engGetVariable(enginePtrD, matlabVarName.c_str());
//   mxArrayOutputPtrD = engGetArray(enginePtrD, matlabVarName.c_str());

   if (mxArrayOutputPtrD == NULL)
   {
      printf("MatlabInterface::GetArray(): mxArrayOutputPtrD is NULL\n");
      return 0;
   }
   else
   {
      memcpy((char*)outArray, (char*)mxGetPr(mxArrayOutputPtrD), numElements*sizeof(double));
      //printf("MatlabInterface::GetArray():output value[0:2] = %g, %g, %g\n",
      //       outArray[0], outArray[1], outArray[2]);
      mxDestroyArray(mxArrayOutputPtrD);
      return 1;
   }
#endif

   return 0;
} // end GetVariable()


//------------------------------------------------------------------------------
//  int EvalString(const std::string &evalString)
//------------------------------------------------------------------------------
//  Purpose:
//     Evaluates matlab string.
//
//  Returns:
//     1 = no error, 0 = error
//------------------------------------------------------------------------------
int MatlabInterface::EvalString(const std::string &evalString)
{
#if defined __USE_MATLAB__
//   printf("MatlabInterface::EvalString() evalString = %s\n", evalString.c_str());
   engEvalString(enginePtrD, evalString.c_str());
   return 1;
#endif
   return 0;
}


//------------------------------------------------------------------------------
//  int OutputBuffer(const char *buffer, int size)
//------------------------------------------------------------------------------
//  Purpose:
//     outputs matlab results to buffer
//
//  Returns:
//     1 = no error, 0 = error
//------------------------------------------------------------------------------
int MatlabInterface::OutputBuffer(char *buffer, int size)
{
#if defined __USE_MATLAB__
   if (buffer == NULL)
   {
      printf("MatlabInterface::OutputBuffer(): buffer is NULL\n");
      return 0;
   }
   else
   {
      engOutputBuffer(enginePtrD, buffer, size);
      return 1;
   }
#endif
   return 0;
}


//--------------------------------------
//  private functions
//--------------------------------------


//------------------------------------------------------------------------------
//  <constructor>
//  MatlabInterface()
//------------------------------------------------------------------------------
MatlabInterface::MatlabInterface()
{
}


//------------------------------------------------------------------------------
//  <destructor>
//  MatlabInterface()
//------------------------------------------------------------------------------
MatlabInterface::~MatlabInterface()
{
}


