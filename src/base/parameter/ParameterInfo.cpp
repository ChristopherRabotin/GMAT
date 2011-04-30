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
 * Implements parameter info class.
 */
//------------------------------------------------------------------------------

#include "ParameterInfo.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PARAM_INFO

//---------------------------------
// static data
//---------------------------------
ParameterInfo* ParameterInfo::theInstance = NULL;

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// ParameterInfo* Instance()
//------------------------------------------------------------------------------
ParameterInfo* ParameterInfo::Instance()
{
   if (theInstance == NULL)
      theInstance = new ParameterInfo;
    
   return theInstance;
}

//------------------------------------------------------------------------------
// ParameterInfo()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
ParameterInfo::ParameterInfo()
{
   mNumParams = 0;
}

//------------------------------------------------------------------------------
// ~ParameterInfo()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ParameterInfo::~ParameterInfo()
{
}

//---------------------------------
// Get methods
//---------------------------------

//------------------------------------------------------------------------------
// Integer GetNumParameters() const
//------------------------------------------------------------------------------
/**
 * Retrieves number of parameters in the database.
 *
 * @return number of parameters
 */
//------------------------------------------------------------------------------
Integer ParameterInfo::GetNumParameters() const
{
   return mNumParams;
}


//------------------------------------------------------------------------------
// const StringArray& GetTypesOfParameters()
//------------------------------------------------------------------------------
/**
 * @return names of parameters
 */
//------------------------------------------------------------------------------
const StringArray& ParameterInfo::GetTypesOfParameters()
{
   mParamTypes.clear();
   std::map<std::string, Gmat::ObjectType>::iterator pos;
   
   for (pos = mParamObjectTypeMap.begin(); pos != mParamObjectTypeMap.end(); ++pos)
   {
      mParamTypes.push_back(pos->first);
   }
   
   return mParamTypes;
}


//------------------------------------------------------------------------------
// const StringArray& GetNamesOfParameters()
//------------------------------------------------------------------------------
/**
 * @return names of parameters
 */
//------------------------------------------------------------------------------
const StringArray& ParameterInfo::GetNamesOfParameters()
{
   mParamNames.clear();
   std::map<std::string, GmatParam::DepObject>::iterator pos;

   for (pos = mParamDepObjMap.begin(); pos != mParamDepObjMap.end(); ++pos)
   {
      mParamNames.push_back(pos->first);
   }
   
   return mParamNames;
}


//------------------------------------------------------------------------------
// Gmat::ObjectType GetObjectType(const std::string &name)
//------------------------------------------------------------------------------
Gmat::ObjectType ParameterInfo::GetObjectType(const std::string &name)
{
   if (mParamObjectTypeMap.find(name) != mParamObjectTypeMap.end())
      return mParamObjectTypeMap[name];
   else
      return Gmat::UNKNOWN_OBJECT;
}


//------------------------------------------------------------------------------
// GmatParam::DepObject GetDepObjectType(const std::string &name)
//------------------------------------------------------------------------------
GmatParam::DepObject ParameterInfo::GetDepObjectType(const std::string &name)
{
   if (mParamDepObjMap.find(name) != mParamDepObjMap.end())
      return mParamDepObjMap[name];
   else
      return GmatParam::NO_DEP;
}


//------------------------------------------------------------------------------
// bool ParameterInfo::IsPlottable(const std::string &name)
//------------------------------------------------------------------------------
bool ParameterInfo::IsPlottable(const std::string &name)
{
   if (mParamPlottableMap.find(name) != mParamPlottableMap.end())
      return mParamPlottableMap[name];
   else
      return false;
}


//------------------------------------------------------------------------------
// bool ParameterInfo::IsReportable(const std::string &name)
//------------------------------------------------------------------------------
bool ParameterInfo::IsReportable(const std::string &name)
{
   if (mParamReportableMap.find(name) != mParamReportableMap.end())
      return mParamReportableMap[name];
   else
      return false;
}


//------------------------------------------------------------------------------
// bool ParameterInfo::IsSettable(const std::string &name)
//------------------------------------------------------------------------------
bool ParameterInfo::IsSettable(const std::string &name)
{
   if (mParamSettableMap.find(name) != mParamSettableMap.end())
      return mParamSettableMap[name];
   else
      return false;
}


//------------------------------------------------------------------------------
// void Add(const std::string &type, Gmat::ObjectType objectType,
//          const std::string &name, GmatParam::DepObject depType,
//          bool isPlottable, bool isReportable, bool isSettable)
//------------------------------------------------------------------------------
void ParameterInfo::Add(const std::string &type, Gmat::ObjectType objectType,
                        const std::string &name, GmatParam::DepObject depType,
                        bool isPlottable, bool isReportable, bool isSettable)
{
   #ifdef DEBUG_PARAM_INFO
   MessageInterface::ShowMessage
      ("ParameterInfo::Add() entered, type='%s', objectType=%d\n   name='%s', "
       "depType=%d, isPlottable=%d, isReportable=%d, isSettable=%d\n", type.c_str(),
       objectType, name.c_str(), depType, isPlottable, isReportable, isSettable);
   #endif
   
   // Check for dot first
   std::string::size_type pos = name.find_last_of(".");
   if (pos == name.npos)
   {
      #ifdef DEBUG_PARAM_INFO
      MessageInterface::ShowMessage
         ("ParameterInfo::Add() leaving, '%s' was not added, it's not a System Parameter\n",
          type.c_str());
      #endif
      return;
   }
   
   // Check for the same property
   if (mParamObjectTypeMap.find(type) != mParamObjectTypeMap.end())
   {
      #ifdef DEBUG_PARAM_INFO
      MessageInterface::ShowMessage
         ("ParameterInfo::Add() leaving, '%s' was not added, it's been already added\n",
          type.c_str());
      #endif
      return;
   }
   
   // add property objectType
   mParamObjectTypeMap[type] = objectType;
   
   // add property name
   std::string propertyName = name.substr(pos+1, name.npos-pos);
   
   mParamDepObjMap[propertyName] = depType;
   
   // add plottable
   mParamPlottableMap[type] = isPlottable;
   
   // add reportable
   mParamReportableMap[type] = isReportable;
   
   // add settable
   mParamSettableMap[type] = isSettable;
   
   mNumParams = mParamDepObjMap.size();
   
   #ifdef DEBUG_PARAM_INFO
   MessageInterface::ShowMessage
      ("ParameterInfo::Add() leaving, propertyName:'%s' was added. There are "
       "total %d Parameters\n", propertyName.c_str(), mNumParams);
   #endif
}


//------------------------------------------------------------------------------
// void Remove(const std::string &name)
//------------------------------------------------------------------------------
void ParameterInfo::Remove(const std::string &name)
{
   mParamDepObjMap.erase(name);
   mNumParams = mParamDepObjMap.size();
}

