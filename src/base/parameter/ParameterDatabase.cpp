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
 * Implements parameter database class.
 */
//------------------------------------------------------------------------------

#include "ParameterDatabase.hpp"
#include "ParameterDatabaseException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_RENAME 1

//---------------------------------
// public
//---------------------------------

//------------------------------------------------------------------------------
// ParameterDatabase()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 */
//------------------------------------------------------------------------------
ParameterDatabase::ParameterDatabase()
{
   mNumParams = 0;
   mStringParamPtrMap = new StringParamPtrMap;
}

//------------------------------------------------------------------------------
// ~ParameterDatabase()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ParameterDatabase::~ParameterDatabase()
{
   delete mStringParamPtrMap;
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
Integer ParameterDatabase::GetNumParameters() const
{
   return mNumParams;
}

//------------------------------------------------------------------------------
// const StringArray& GetNamesOfParameters()
//------------------------------------------------------------------------------
/**
 * @return names of parameters
 */
//------------------------------------------------------------------------------
const StringArray& ParameterDatabase::GetNamesOfParameters()
{
   mParamNames.clear();
   StringParamPtrMap::iterator pos;

   for (pos = mStringParamPtrMap->begin(); pos != mStringParamPtrMap->end(); ++pos)
   {
      mParamNames.push_back(pos->first);
   }
   
   return mParamNames;
}

//------------------------------------------------------------------------------
// ParameterPtrArray GetParameters() const
//------------------------------------------------------------------------------
/**
 * @return array of parameters
 */
//------------------------------------------------------------------------------
ParameterPtrArray ParameterDatabase::GetParameters() const
{
   ParameterPtrArray parameters;
   StringParamPtrMap::iterator pos;
   
   for (pos = mStringParamPtrMap->begin(); pos != mStringParamPtrMap->end(); ++pos)
   {
      parameters.push_back(pos->second);
   }
   
   return parameters;
}

//------------------------------------------------------------------------------
// bool HasParameter(const std::string &name) const
//------------------------------------------------------------------------------
/**
 * @return true if database has the parameter name, false otherwise
 */
//------------------------------------------------------------------------------
bool ParameterDatabase::HasParameter(const std::string &name) const
{
   bool found = false;
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   
   if (pos != mStringParamPtrMap->end())
      found = true;
   
   return found;
}

//loj: 11/17/04 added
//------------------------------------------------------------------------------
// bool RenameParameter(const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * @param <oldName> parameter name to be renamed
 * @param <newName> new prameter name
 *
 * @return true if parameter is renamed, false otherwise
 */
//------------------------------------------------------------------------------
bool ParameterDatabase::RenameParameter(const std::string &oldName,
                                        const std::string &newName)
{
#if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ParameterDatabase::RenameParameter() oldName=%s, newName=%s\n",
       oldName.c_str(), newName.c_str());
#endif
   
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(oldName);
   
   if (pos != mStringParamPtrMap->end())
   {
      // add new parameter name key and delete old
      Add(newName, pos->second);
      mStringParamPtrMap->erase(pos);

//        for (int i=0; i<mNumParams; i++)
//        {
//           if (mParamNames[i] == oldName)
//           {
//              mParamNames[i] = newName;
//              return true;
//           }
//        }
   }
   
   return false;
}

//------------------------------------------------------------------------------
// Integer GetParameterCount(const std::string &name) const
//------------------------------------------------------------------------------
/**
 * @return number of data elements of given parameter name
 */
//------------------------------------------------------------------------------
Integer ParameterDatabase::GetParameterCount(const std::string &name) const
{
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   if (pos == mStringParamPtrMap->end())
      throw ParameterDatabaseException
         ("ParameterDatabase::GetParameterCount() Parameter name " + name +
          " not found in the database");
   else
      return pos->second->GetParameterCount();
}

//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string &name) const
//------------------------------------------------------------------------------
/**
 * @return parameter object of given parameter name
 */
//------------------------------------------------------------------------------
Parameter* ParameterDatabase::GetParameter(const std::string &name) const
{
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   if (pos == mStringParamPtrMap->end())
      throw ParameterDatabaseException
         ("ParameterDatabase::GetParameter() Parameter name " + name +
          " not found in the database");
   else
      return pos->second;
}

//------------------------------------------------------------------------------
// std::string GetFirstParameterName() const
//------------------------------------------------------------------------------
std::string ParameterDatabase::GetFirstParameterName() const
{
   StringParamPtrMap::iterator pos;
   pos = mStringParamPtrMap->begin();
   return pos->first;
}

//------------------------------------------------------------------------------
// bool SetParameter(const std::string &name, Parameter *param)
//------------------------------------------------------------------------------
bool ParameterDatabase::SetParameter(const std::string &name, Parameter *param)
{
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   if (pos == mStringParamPtrMap->end())
      throw ParameterDatabaseException
         ("ParameterDatabase::SetParameter() Parameter name " + name +
          " not found in the database\n");

   pos->second = param;
   return true;
}


//*********************************
// for Add, Remove 
//*********************************

//------------------------------------------------------------------------------
// void Add(Parameter *param)
//------------------------------------------------------------------------------
void ParameterDatabase::Add(Parameter *param)
{
   if (param != NULL)
   {
      std::string name = param->GetName();
      Add(name, param);
   }
   else
   {
      throw ParameterDatabaseException
         ("ParameterDatabase::Add() Cannot add NULL Parameter\n");
   }
}

//------------------------------------------------------------------------------
// void Add(const std::string &name, Parameter *param)
//------------------------------------------------------------------------------
void ParameterDatabase::Add(const std::string &name, Parameter *param)
{
   StringParamPtrMap::iterator pos;

   pos = mStringParamPtrMap->find(name);
   if (pos != mStringParamPtrMap->end())
      throw ParameterDatabaseException
         ("ParameterDatabase::Add() Parameter name: " + name +
          " already in the database\n");

   mStringParamPtrMap->insert(StringParamPtrPair(name, param));
   mNumParams = mStringParamPtrMap->size();
}

//------------------------------------------------------------------------------
// void Remove(const Parameter *param)
//------------------------------------------------------------------------------
void ParameterDatabase::Remove(const Parameter *param)
{
   Remove(param->GetName());
}

//------------------------------------------------------------------------------
// void Remove(const std::string &name)
//------------------------------------------------------------------------------
void ParameterDatabase::Remove(const std::string &name)
{
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   if (pos == mStringParamPtrMap->end())
      throw ParameterDatabaseException
         ("ParameterDatabase::Remove() Parameter name: " + name +
          " not found in the database\n");
   
   mStringParamPtrMap->erase(name);
   mNumParams = mStringParamPtrMap->size();  
}

