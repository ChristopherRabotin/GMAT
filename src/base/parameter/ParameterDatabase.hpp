//$Header$
//------------------------------------------------------------------------------
//                                ParameterDatabase
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
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

class ParameterDatabase
{

public:

   ParameterDatabase();
   ~ParameterDatabase();

   Integer GetNumParameters() const;
   const StringArray& GetNamesOfParameters();
   ParameterPtrArray GetParameters() const;

   bool HasParameter(const std::string &name) const;
   Integer GetParameterCount(const std::string &name) const;

   Parameter* GetParameter(const std::string &name) const;
   std::string GetFirstParameterName() const;
   bool SetParameter(const std::string &name, Parameter *param);
   
   void Add(const std::string &name, Parameter *param = NULL); //loj: 9/16/04 added
   void Add(Parameter *param);
   void Remove(const std::string &name);
   void Remove(const Parameter *param);

protected:
private:

   StringParamPtrMap *mStringParamPtrMap;
   StringArray mParamNames; //loj: 9/22/04 added
   Integer mNumParams;

};
#endif // ParameterDatabase_hpp

