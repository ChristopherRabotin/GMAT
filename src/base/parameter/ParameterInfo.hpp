//$Header$
//------------------------------------------------------------------------------
//                                ParameterInfo
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2005/01/24
//
/**
 * Declares parameter info class.
 */
//------------------------------------------------------------------------------
#ifndef ParameterInfo_hpp
#define ParameterInfo_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"
#include <map>

class ParameterInfo
{

public:

   static ParameterInfo* Instance();
   
   Integer GetNumParameters() const;
   const StringArray& GetTypesOfParameters();
   const StringArray& GetNamesOfParameters();
   Gmat::ObjectType GetOwnerType(const std::string &type);
   GmatParam::DepObject GetDepObjectType(const std::string &name);
   bool IsPlottable(const std::string &type);
   bool IsReportable(const std::string &type);
      
   void Add(const std::string type, Gmat::ObjectType owner,
            const std::string &name, GmatParam::DepObject depType,
            bool isPlottable, bool isReportable);
   void Remove(const std::string &name);
   
protected:
private:

   static ParameterInfo *theInstance;
   
   std::map<std::string, GmatParam::DepObject> mParamDepObjMap;
   std::map<std::string, Gmat::ObjectType> mParamOwnerMap;
   std::map<std::string, bool> mParamPlottableMap;
   std::map<std::string, bool> mParamReportableMap;
   StringArray mParamTypes;
   StringArray mParamNames;
   Integer mNumParams;
   
   ParameterInfo();
   ~ParameterInfo();
   
};
#endif // ParameterInfo_hpp

