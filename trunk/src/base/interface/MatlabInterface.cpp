//$Id$
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
//#define DEBUG_MATLAB_GET

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
   
   #ifdef DEBUG_MATLAB_OPEN_CLOSE
   MessageInterface::ShowMessage
      ("MatlabInterface::Open() enginePtrD=%p\n", enginePtrD);
   #endif
   
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
   // -desktop now causes MATLAB desktop to come up (as of 2008.01.11) but it 
   // hangs both MATLAB and GMAT
   //std::string runString = "matlab -display " + hNameStr + ":0.0 -desktop";
   //std::string runString = "matlab -display " + hNameStr + ":0.0";
   std::string runString = "matlab ";
   if ((enginePtrD = engOpen(runString.c_str())))
   {
      MessageInterface::ShowMessage(
               "Successfully opened MATLAB engine using startcmd \"%s\"\n",
               runString.c_str());
      accessCount++;
      #ifdef DEBUG_MATLAB_OPEN_CLOSE
         MessageInterface::ShowMessage(
         "Attempting to open MATLAB connection ... accessCount = %d\n", accessCount);
      #endif
      //engSetVisible(enginePtrD,1);  // rats!
      return 1;
   }
   else
   {
      MessageInterface::ShowMessage(
               "Failed to open MATLAB engine using startcmd \"%s\"\n",
               runString.c_str());
      return 0;
   }
   
#else

   if (enginePtrD != NULL)
   {
      #ifdef DEBUG_MATLAB_OPEN_CLOSE
      MessageInterface::ShowMessage("Connecting to current MATLAB session\n");
      #endif
      
      return 1;
   }
   
   // open new MATLAB engine
   if ((enginePtrD = engOpen(NULL)))
   {
      accessCount++;
      
      #ifdef DEBUG_MATLAB_OPEN_CLOSE
      MessageInterface::ShowMessage
         ("Attempting to open MATLAB connection ... accessCount = %d, "
          "enginePtrD=%p\n", accessCount, enginePtrD);
      #endif
      
      // set precision to long
      EvalString("format long");
      return 1;
   }
   else
   {
      MessageInterface::ShowMessage("Failed to open MATLAB engine ...\n");
      return 0;
   }
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
      #ifdef DEBUG_MATLAB_OPEN_CLOSE
      MessageInterface::ShowMessage
         ("MatlabInterface::Close() enginePtrD=%p\n", enginePtrD);
      #endif
      
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
      #ifdef DEBUG_MATLAB_OPEN_CLOSE
      MessageInterface::ShowMessage
         ("\nUnable to close MATLAB because it is not currently running\n");
      #endif
      
      return 0;
   }
   
#endif

   return 1;

} // end Close()


//------------------------------------------------------------------------------
// int PutRealArray(const std::string &matlabVarName, int numElements,
//                  double inArray[])
//------------------------------------------------------------------------------
/**
 * Put arrays to Matlab workspace.
 *
 * @param <matlabVarName> variable name in the MATLAB workspace
 * @param <numElements> number of elements to put to MATLAB workspace
 * @return 1 = no error, 0 = error
 */
//------------------------------------------------------------------------------
int MatlabInterface::PutRealArray(const std::string &matlabVarName,
                                 int numElements, double inArray[])
{
#if defined __USE_MATLAB__

   // create a matlab variable
   mxArrayInputPtrD = mxCreateDoubleMatrix(1, numElements, mxREAL);
   
   memcpy((char*)mxGetPr(mxArrayInputPtrD), (char*)inArray, numElements*sizeof(double));
   
   // place the variable mxArrayInputPtrD into the MATLAB workspace
   engPutVariable(enginePtrD, matlabVarName.c_str(), mxArrayInputPtrD);
   
#endif

   return 1;
   
} // end PutRealArray()


//------------------------------------------------------------------------------
// int GetRealArray(const std::string &matlabVarName, int numElements,
//                  double outArray[])
//------------------------------------------------------------------------------
/**
 * Get arrays from Matlab workspace.
 *
 * @param <matlabVarName> variable name in the MATLAB workspace
 * @param <numElements> number of elements to receive from MATLAB
 * @param <outArray> array to receive double array from MATLAB
 * @Return 1 = no error, 0 = error
 * @exception <InterfaceException> thrown if empty output was received
 */
//------------------------------------------------------------------------------
int MatlabInterface::GetRealArray(const std::string &matlabVarName,
                                  int numElements, double outArray[])
{
#if defined __USE_MATLAB__
   // create a matlab variable into which to put the data
   mxArrayInputPtrD = mxCreateDoubleMatrix(1, numElements, mxREAL);
   
   #ifdef DEBUG_MATLAB_GET
   MessageInterface::ShowMessage
      ("Entering MatlabInterface::GetRealArray() matlabVarName=%s, numElements=%d, "
       "mxArrayInputPtrD=%p\n", matlabVarName.c_str(), numElements, mxArrayInputPtrD);
   #endif
   
   // set precision to long
   EvalString("format long");
   
   // get the variable from the MATLAB workspace
   mxArrayOutputPtrD = engGetVariable(enginePtrD, matlabVarName.c_str());
   
   if (mxArrayOutputPtrD == NULL)
   {
      #ifdef DEBUG_MATLAB_GET
      MessageInterface::ShowMessage
         ("MatlabInterface::GetRealArray(): mxArrayOutputPtrD is NULL\n");
      #endif
      return 0;
   }
   else if (mxIsDouble(mxArrayOutputPtrD))
   {
      // If real numeric pointer is NULL, throw an exception
      double *realPtr = mxGetPr(mxArrayOutputPtrD);
      if (realPtr == NULL)
         throw InterfaceException("Received empty output from MATLAB");
      
      memcpy((char*)outArray, (char*)mxGetPr(mxArrayOutputPtrD),
             numElements*sizeof(double));
      
      #ifdef DEBUG_MATLAB_GET
      MessageInterface::ShowMessage("      outArray  = \n");
      for (Integer ii=0; ii < numElements; ii++)
         MessageInterface::ShowMessage("         %.12f\n", outArray[ii]);
      #endif
      
      mxDestroyArray(mxArrayOutputPtrD);
      return 1;
   }
   else
   {
      #ifdef DEBUG_MATLAB_GET
      MessageInterface::ShowMessage
         ("MatlabInterface::GetRealArray() Matlab variable is not a double array\n");
      #endif
      
      return 0;
   }
#endif

   return 0;
} // end GetRealArray()


//------------------------------------------------------------------------------
// int GetString(const std::string &matlabVarName, std::string &outStr)
//------------------------------------------------------------------------------
/**
 * Get char array from Matlab workspace.
 *
 * @param <matlabVarName> variable name in the MATLAB workspace
 * @param <outStr> string to receive char array from MATLAB
 * @Return 1 = no error, 0 = error
 * @exception <InterfaceException> thrown if empty output was received
 */
//------------------------------------------------------------------------------
int MatlabInterface::GetString(const std::string &matlabVarName,
                               std::string &outStr)
{
#if defined __USE_MATLAB__
   
   // get the variable from the MATLAB workspace
   mxArrayOutputPtrD = engGetVariable(enginePtrD, matlabVarName.c_str());
   
   if (mxArrayOutputPtrD == NULL)
   {
      #ifdef DEBUG_MATLAB_GET
      MessageInterface::ShowMessage
         ("MatlabInterface::GetString(): mxArrayOutputPtrD is NULL\n");
      #endif
      return 0;
   }
   else if (mxIsChar(mxArrayOutputPtrD))
   {
      char outArray[512];
      mxGetString(mxArrayOutputPtrD, outArray, 512);
      outStr.assign(outArray);
      
      #ifdef DEBUG_MATLAB_GET
      MessageInterface::ShowMessage
         ("MatlabInterface::GetString() outStr =\n%s\n", outStr.c_str());
      #endif
      
      mxDestroyArray(mxArrayOutputPtrD);
      return 1;
   }
   else
   {
      #ifdef DEBUG_MATLAB_GET
      MessageInterface::ShowMessage
         ("MatlabInterface::GetString() Matlab variable is not a char array\n");
      #endif
      
      return 0;
   }
#endif

   return 0;
} // end GetString()


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
// bool IsOpen()
//------------------------------------------------------------------------------
bool MatlabInterface::IsOpen() 
{  
   #ifdef __USE_MATLAB__
      return (MatlabInterface::enginePtrD != NULL);
   #endif
   return false;
}


//------------------------------------------------------------------------------
// void RunMatlabString(std::string evalString)
//------------------------------------------------------------------------------
void MatlabInterface::RunMatlabString(std::string evalString)
{
#if defined __USE_MATLAB__
   if (!MatlabInterface::IsOpen())
   {
      // Let's try to open it first (loj: 2008.03.06)
      if (MatlabInterface::Open() == 0)
         throw InterfaceException("**** ERROR **** Failed to open MATLAB engine\n");
      //throw InterfaceException("ERROR - Matlab Interface not yet open");
   }
   
   // try to call the function   
   evalString = "try,\n  " + evalString + "\ncatch\n  errormsg = lasterr;\nend";
   
   #if DEBUG_MATLAB_EVAL
   MessageInterface::ShowMessage("===> evalString=\n%s\n", evalString.c_str());
   MessageInterface::ShowMessage("===> calling EvalString()\n");
   #endif
   
   MatlabInterface::EvalString(evalString);
   
   // if there was an error throw an exception
   std::string errorStr;
   if (MatlabInterface::GetString("errormsg", errorStr))
   {
      //MessageInterface::ShowMessage("===> %s\n", errorStr.c_str());
      throw InterfaceException(errorStr.c_str());
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


