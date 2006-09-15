//$Header$
//------------------------------------------------------------------------------
//                               MatlabInterface
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2002/11/04
//
/**
 * Implements MatlabInterface functions.
 */
//------------------------------------------------------------------------------
#include <stdlib.h>         // for NULL
#include <string.h>         // for memcpy()
#include <sstream>          // for std::stringstream
#if defined __USE_MATLAB__
#include "engine.h"         // for Engine
#endif

#ifdef __WXMAC__
#include <stdlib.h>         // for system() to launch X11 application
#include <unistd.h>         // for gethostname()
#endif

#include "MatlabInterface.hpp"
#include "MessageInterface.hpp"
#include "InterfaceException.hpp"

//#define DEBUG_MATLAB_IF 1
//#define DEBUG_MATLAB_OPEN_CLOSE

//--------------------------------------
//  initialize static variables
//--------------------------------------
#if defined __USE_MATLAB__
Engine* MatlabInterface::enginePtrD = 0;
mxArray* MatlabInterface::mxArrayInputPtrD = 0;
mxArray* MatlabInterface::mxArrayOutputPtrD = 0;
int MatlabInterface::accessCount = 0;
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

   if (enginePtrD == NULL)
      MessageInterface::ShowMessage("Please wait while MATLAB opens...\n");

#ifdef __WXMAC__
   /// Check if MATLAB is still running then doesn't need to re-launch
   if (enginePtrD != NULL)
   {
      accessCount++;
      #ifdef DEBUG_MATLAB_OPEN_CLOSE
         MessageInterface::ShowMessage(
         "Attempting to reopen MATLAB connection ... accessCount = %d\n", accessCount);
      #endif
      return 1;
   }

   // open the X11 application before launching the matlab
   system("open -a X11");
   // need to get IP address or hostname here
   char hName[128];
   int OK = gethostname(hName, 128);
   if (OK != 0) MessageInterface::ShowMessage("Error getting host name\n");
   std::string hNameStr(hName);
   //std::string runString = "matlab -display " + hNameStr + ":0.0 -desktop";
   std::string runString = "matlab -display " + hNameStr + ":0.0";
   //MessageInterface::ShowMessage("MATLAB being opened with the command: " 
   //                  + runString + "\n");
   //if ((enginePtrD = engOpen("\0")))
   if ((enginePtrD = engOpen(NULL))) // ***** temporary or dos it matter? ******
   //if ((enginePtrD = engOpen(runString.c_str())))
   {
      MessageInterface::ShowMessage("Successfully opened MATLAB engine ...\n");
      accessCount++;
      #ifdef DEBUG_MATLAB_OPEN_CLOSE
         MessageInterface::ShowMessage(
         "Attempting to open MATLAB connection ... accessCount = %d\n", accessCount);
      #endif
      return 1;
   }
   else
   {
      MessageInterface::ShowMessage("Failed to open MATLAB engine ...\n");
      return 0;
   }
#else
   if ((enginePtrD = engOpen(NULL)))
   {
      accessCount++;
      #ifdef DEBUG_MATLAB_OPEN_CLOSE
         MessageInterface::ShowMessage(
         "Attempting to open MATLAB connection ... accessCount = %d\n", accessCount);
      #endif
      return 1;
   }
   else
      return 0;
#endif  // End-ifdef __WXMAC__

      return 0;
#endif // End-ifdef __USE_MATLAB__

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
   // Check if MATLAB is still running then close it.
   if (enginePtrD != NULL)
   {
      //MessageInterface::ShowMessage("Please wait while MATLAB closes ...\n");
      accessCount--;
      #ifdef DEBUG_MATLAB_OPEN_CLOSE
         MessageInterface::ShowMessage(
         "Attempting to close MATLAB connection ... accessCount = %d\n", accessCount);
      #endif
      //if (accessCount <= 0)
      //{
         #ifdef __WXMAC__
            // need to close X11 here ------------ **** TBD ****
            MessageInterface::ShowMessage(
            "Closing connection to MATLAB ... please close X11 ...\n");
         #endif
         if (engClose(enginePtrD) != 0)
               MessageInterface::ShowMessage("\nError closing MATLAB\n");    
         //engClose(enginePtrD);
         enginePtrD = NULL;      // set to NULL, so it can be reopened
         MessageInterface::ShowMessage("MATLAB has been closed ...\n");
      //}
      //else
      //{
      //   MessageInterface::ShowMessage(
      //   "\nGMAT still accessing MATLAB ... not closing connection at this time\n");
      //}
   }
   else
   {
      MessageInterface::ShowMessage(
             "\nUnable to close MATLAB because it is not currently running\n");
      return 0;
   }

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
//      MessageInterface::ShowMessage("MatlabInterface::GetArray(): mxArrayOutputPtrD is NULL\n");
      // printf("MatlabInterface::GetArray(): mxArrayOutputPtrD is NULL\n");
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
   
   #if DEBUG_MATLAB_IF
   MessageInterface::ShowMessage
      ("MatlabInterface::EvalString() evalString = \n%s\n\n", evalString.c_str());
   #endif
   
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
      // printf("MatlabInterface::OutputBuffer(): buffer is NULL\n");
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

//------------------------------------------------------------------------------
// void RunMatlabString(std::string evalString)
//------------------------------------------------------------------------------
void MatlabInterface::RunMatlabString(std::string evalString)
{
#if defined __USE_MATLAB__
   if (!MatlabInterface::IsOpen())
      throw InterfaceException(
         "ERROR - Matlab Interface not yet open");

   // try to call the function
   evalString = "try,\n  " + evalString + "\ncatch\n  errormsg = lasterr;\nend";
   MatlabInterface::EvalString(evalString);

   double errormsg[128];
   // if there was an error throw an exception
   if (MatlabInterface::GetVariable("errormsg", 1, errormsg))
   {
      double buffer[128];

      MatlabInterface::OutputBuffer((char *)buffer, 128);
      MatlabInterface::EvalString("errormsg");

      // get rid of "errormsg ="
      char *ptr = strtok((char *)buffer, "=");
      ptr = strtok(NULL, "\n");

      std::stringstream errorStr;
      errorStr << "Error from Matlab\n"<< ptr;

      throw InterfaceException(errorStr.str());
   }
#endif
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


