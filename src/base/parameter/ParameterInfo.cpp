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
 * Implements parameter info class.
 */
//------------------------------------------------------------------------------

#include "ParameterInfo.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PARAM_INFO 1

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
   
   for (pos = mParamOwnerMap.begin(); pos != mParamOwnerMap.end(); ++pos)
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

//loj: 6/7/05 Added
//------------------------------------------------------------------------------
// Gmat::ObjectType GetOwnerType(const std::string &type)
//------------------------------------------------------------------------------
Gmat::ObjectType ParameterInfo::GetOwnerType(const std::string &type)
{
   return mParamOwnerMap[type];
}


//------------------------------------------------------------------------------
// GmatParam::DepObject GetDepObjectType(const std::string &name)
//------------------------------------------------------------------------------
GmatParam::DepObject ParameterInfo::GetDepObjectType(const std::string &name)
{
   return mParamDepObjMap[name];
}


//------------------------------------------------------------------------------
// void Add(const std::string type, Gmat::ObjectType owner,
//          const std::string &name, GmatParam::DepObject depType)
//------------------------------------------------------------------------------
void ParameterInfo::Add(const std::string type, Gmat::ObjectType owner,
                        const std::string &name, GmatParam::DepObject depType)
{
   // add property owner
   mParamOwnerMap[type] = owner;
   
   // add property name
   std::string::size_type pos = name.find_last_of(".");
   std::string propertyName = name.substr(pos+1, name.npos-pos);
   
   mParamDepObjMap[propertyName] = depType;
   
   #if DEBUG_PARAM_INFO
   MessageInterface::ShowMessage
      ("ParameterInfo::Add() propertyName:%s with owner=%d, depType:%d added\n",
       propertyName.c_str(), owner, depType);
   #endif
   
   mNumParams = mParamDepObjMap.size();
}


//------------------------------------------------------------------------------
// void Remove(const std::string &name)
//------------------------------------------------------------------------------
void ParameterInfo::Remove(const std::string &name)
{
   mParamDepObjMap.erase(name);
   mNumParams = mParamDepObjMap.size();
}

