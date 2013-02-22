//$Id: CallMatlabFunction.hpp 9460 2011-04-21 22:03:28Z lindajun $
//------------------------------------------------------------------------------
//                                 CallMatlabFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Allison Greene
// Created: 2004/09/22
//
/**
 * Definition for the CallMatlabFunction command class
 */
//------------------------------------------------------------------------------
#ifndef CallMatlabFunction_hpp
#define CallMatlabFunction_hpp

#include "matlabinterface_defs.hpp"
#include "CallFunction.hpp"
#include "MatlabInterface.hpp"

class MATLAB_API CallMatlabFunction : public CallFunction
{
public:
   CallMatlabFunction();
   virtual ~CallMatlabFunction();
   
   CallMatlabFunction(const CallMatlabFunction& cf);
   CallMatlabFunction&  operator=(const CallMatlabFunction& cf);
   
   std::string          FormEvalString();
   
   // override GmatCommand methods
   virtual bool         Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();
   
   // override GmatBase methods
   virtual GmatBase*    Clone() const;

protected:
   
   MatlabInterface *matlabIf;
   
   bool ExecuteMatlabFunction();
   void SendInParam(Parameter *param, const std::string &inStr);
   void GetOutParams();
   void EvalMatlabString(const std::string &evalString);
   void ClearInputParameters();
   void ClearOutputParameters();
   void UpdateObject(GmatBase *obj, char *buffer);
   void HandleError(double **outArray, std::string &errMsg, Integer numRows,
                    Integer numCols, Integer numRowsReceived,
                    Integer numColsReceived);
};

#endif // CallMatlabFunction_hpp
