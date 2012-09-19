//$Id$
//------------------------------------------------------------------------------
//                                  GmatFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Allison Greene
// Created: 2004/12/16
//
/**
 * Definition for the GmatFunction class.
 */
//------------------------------------------------------------------------------


#ifndef GmatFunction_hpp
#define GmatFunction_hpp

#include "GmatFunction_defs.hpp"
#include "Function.hpp"
#include "ObjectInitializer.hpp"

class GMATFUNCTION_API GmatFunction : public Function
{
public:
   GmatFunction(const std::string &nomme);
   
   virtual ~GmatFunction(void);
   
   GmatFunction(const GmatFunction &copy);
   GmatFunction& operator=(const GmatFunction &right);
   
   virtual bool IsNewFunction();
   virtual void SetNewFunction(bool flag);
   
   // inherited from Function
   virtual bool         Initialize();
   virtual bool         Execute(ObjectInitializer *objInit, bool reinitialize);
   virtual void         Finalize();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
protected:
   
   bool         mIsNewFunction;
   StringArray  *unusedGlobalObjectList;
   
   bool InitializeLocalObjects(ObjectInitializer *objInit,
                               GmatCommand *current,
                               bool ignoreException);
   void BuildUnusedGlobalObjectList();
   
   // for debug
   void ShowTrace(Integer count, Integer t1, const std::string &label = "",
                  bool showMemoryTracks = false, bool addEol = false);
   
   enum
   {
      GmatFunctionParamCount = FunctionParamCount  /// Count of the parameters for this class
   };
   
//   static const std::string
//      PARAMETER_TEXT[GmatFunctionParamCount - FunctionParamCount];
//   static const Gmat::ParameterType
//      PARAMETER_TYPE[GmatFunctionParamCount - FunctionParamCount];

};

#endif
