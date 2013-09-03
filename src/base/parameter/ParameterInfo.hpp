//$Id$
//------------------------------------------------------------------------------
//                                ParameterInfo
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

class GMAT_API ParameterInfo
{

public:

   static ParameterInfo* Instance();
   
   Integer GetNumParameters() const;
   const StringArray& GetTypesOfParameters();
   const StringArray& GetNamesOfParameters();
   Gmat::ObjectType GetObjectType(const std::string &type);
   Gmat::ObjectType GetOwnedObjectType(const std::string &type);
   GmatParam::DepObject GetDepObjectType(const std::string &name);
   bool IsPlottable(const std::string &type);
   bool IsReportable(const std::string &type);
   bool IsSettable(const std::string &type);
   bool IsTimeParameter(const std::string &type);
   bool RequiresBodyFixedCS(const std::string &type);
   bool IsForOwnedObject(const std::string &type);
   bool IsForAttachedObject(const std::string &type);
   
   void Add(const std::string &type, Gmat::ObjectType objectType,
            Gmat::ObjectType ownedObjType, const std::string &name,
            GmatParam::DepObject depType, bool isPlottable,
            bool isReportable, bool isSettable, bool isTimeParam,
            const std::string &desc);
   void Remove(const std::string &name);
   
   void SetRequiresBodyFixedCS(const std::string &type, bool flag);
   
protected:
private:
   
   static ParameterInfo *theInstance;
   
   std::map<std::string, GmatParam::DepObject> mDepObjMap;
   std::map<std::string, Gmat::ObjectType> mOwnerTypeMap;
   std::map<std::string, Gmat::ObjectType> mOwnedObjTypeMap;
   std::map<std::string, bool> mIsPlottableMap;
   std::map<std::string, bool> mIsReportableMap;
   std::map<std::string, bool> mIsSettableMap;
   std::map<std::string, bool> mIsTimeParamMap;
   std::map<std::string, bool> mRequiresBodyFixedCSMap;
   std::map<std::string, bool> mIsForOwnedObjMap;
   std::map<std::string, bool> mIsForAttachedObjMap;
   StringArray mParamTypes;
   StringArray mParamNames;
   Integer mNumParams;
   
   ParameterInfo();
   ~ParameterInfo();
   
};
#endif // ParameterInfo_hpp

