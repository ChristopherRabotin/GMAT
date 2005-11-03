//$Header$
//------------------------------------------------------------------------------
//                              GmatInterface
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2004/8/30
//
/**
 * Declares class to provide sending scripts from MATLAB to GMAT or
 * requesting GMAT data from MATLAB.
 */
//------------------------------------------------------------------------------
#ifndef GmatInterface_hpp
#define GmatInterface_hpp

#include "gmatdefs.hpp"
#include <sstream>       // for stringstream, istringstream

class GMAT_API GmatInterface
{
public:
   static GmatInterface* Instance();

   void OpenScript();
   void ClearScript();
   void PutScript(char *str);
   void BuildObject();
   void UpdateObject();
   void RunScript();
   
   char* GetRunState();
   char* GetObject(const std::string &name);
   char* GetParameter(const std::string &name);
   
private:
   static const int MAX_PARAM_VAL_STRING = 512;
   static const int MAX_OBJECT_VAL_STRING = 4096;
   
   void RedirectBuffer(std::ios *stream, std::streambuf* newBuff) 
      { stream->rdbuf(newBuff); }
   
   GmatInterface();
   ~GmatInterface();
   
   std::stringstream mStringStream;
   std::istringstream *mInStringStream;
   static GmatInterface *instance;
   static bool mPassedInterpreter;
};
#endif
