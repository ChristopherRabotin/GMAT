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
#include "gmatdefs.hpp"
#include "paramdefs.hpp"
#include "Parameter.hpp"
#include "ParameterDatabase.hpp"
#include "ParameterDatabaseException.hpp"
//  #include "Rvector3.hpp"
//  #include "Rmatrix33.hpp"
//  #include "Cartesian.hpp"
//  #include "Keplerian.hpp"
//  #include "A1Mjd.hpp"
//  #include "ElapsedTimeParam.hpp"
//  //loj:#include "UtcDate.hpp"

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
// ParameterDatabase(std::vector<Parameter> params)
//------------------------------------------------------------------------------
//  ParameterDatabase::ParameterDatabase(ParameterPtrArray params)
//  {
//     for (int i=0; i<params.size(); ++i)
//        Add(params[i])

//     mNumParams = mStringParamPtrMap->size();
//  }

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
// StringArray GetNamesOfParameters() const
//------------------------------------------------------------------------------
/**
 * @return names of parameters
 */
//------------------------------------------------------------------------------
StringArray ParameterDatabase::GetNamesOfParameters() const
{
   StringArray paramNames;
   StringParamPtrMap::iterator pos;

   for (pos = mStringParamPtrMap->begin(); pos != mStringParamPtrMap->end(); ++pos)
   {
      paramNames.push_back(pos->first);
   }
   
   return paramNames;
}

//------------------------------------------------------------------------------
// StringArray GetDescsOfParameters() const
//------------------------------------------------------------------------------
/**
 * @return descriptions of parameters
 */
//------------------------------------------------------------------------------
StringArray ParameterDatabase::GetDescsOfParameters() const
{
   StringArray paramDescs;
   StringParamPtrMap::iterator pos;

   for (pos = mStringParamPtrMap->begin(); pos != mStringParamPtrMap->end(); ++pos)
   {
      paramDescs.push_back(pos->second->GetStringParameter("Description"));
   }
   
   return paramDescs;
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

//------------------------------------------------------------------------------
// std::string GetDesc(const std::string name) const
//------------------------------------------------------------------------------
/**
 * @return description of given parameter name
 */
//------------------------------------------------------------------------------
std::string ParameterDatabase::GetDesc(const std::string &name) const
{
   StringParamPtrMap::iterator pos;
   
   pos = mStringParamPtrMap->find(name);
   if (pos == mStringParamPtrMap->end())
      throw ParameterDatabaseException("Parameter name " + name +
                                       " not found in the database");
   else
      return pos->second->GetStringParameter("Description");
   
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
      throw ParameterDatabaseException("Parameter name " + name +
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
      throw ParameterDatabaseException("Parameter name " + name +
                                       " not found in the database");
   else
      return pos->second;
}


//*********************************
// for Add, Remove 
//*********************************

//------------------------------------------------------------------------------
// void Add(Parameter *param)
//------------------------------------------------------------------------------
void ParameterDatabase::Add(Parameter *param)
{
   StringParamPtrMap::iterator pos;

   pos = mStringParamPtrMap->find(param->GetName());
   if (pos != mStringParamPtrMap->end())
      throw ParameterDatabaseException("Parameter name " + param->GetName() +
                                       " already in the database");

   mStringParamPtrMap->insert(StringParamPtrPair(param->GetName(), param));
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
      throw ParameterDatabaseException("Parameter name " + name +
                                       " not found in the database");
   
   mStringParamPtrMap->erase(name);
   mNumParams = mStringParamPtrMap->size();  
}

