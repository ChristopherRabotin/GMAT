//$Id$
//------------------------------------------------------------------------------
//                                ParameterDatabase
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
// Author: Linda Jun
// Created: 2003/09/18
//
/**
 * Declares parameter database class.
 */
//------------------------------------------------------------------------------
#ifndef ParameterDatabase_hpp
#define ParameterDatabase_hpp

#include "gmatdefs.hpp"
#include "paramdefs.hpp"
#include "Parameter.hpp"

class GMAT_API ParameterDatabase
{

public:
   
   ParameterDatabase();
   ParameterDatabase(const ParameterDatabase &copy);
   ParameterDatabase& operator=(const ParameterDatabase &right);
   virtual ~ParameterDatabase();
   
   Integer GetNumParameters() const;
   const StringArray& GetNamesOfParameters();
   ParameterPtrArray GetParameters() const;
   
   bool HasParameter(const std::string &name) const;
   bool RenameParameter(const std::string &oldName, const std::string &newName);
   Integer GetParameterCount(const std::string &name) const;
   
   Parameter* GetParameter(const std::string &name) const;
   std::string GetFirstParameterName() const;
   bool SetParameter(const std::string &name, Parameter *param);
   
   void Add(const std::string &name, Parameter *param = NULL);
   void Add(Parameter *param);
   void Remove(const std::string &name);
   void Remove(const Parameter *param);
   
protected:
private:

   StringParamPtrMap *mStringParamPtrMap;
   StringArray mParamNames;
   Integer mNumParams;

};
#endif // ParameterDatabase_hpp

