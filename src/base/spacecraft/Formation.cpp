#include "Formation.hpp"
#include <algorithm>          // for find()

//---------------------------------
// static data
//---------------------------------
const std::string
Formation::PARAMETER_TEXT[FormationParamCount - SpaceObjectParamCount] =
{
   "Add",
   "Remove"
};

const Gmat::ParameterType
Formation::PARAMETER_TYPE[FormationParamCount - SpaceObjectParamCount] =
{
   Gmat::STRINGARRAY_TYPE,
   Gmat::UNKNOWN_PARAMETER_TYPE     // Don't output the "remove" parms
};



Formation::Formation(Gmat::ObjectType typeId, const std::string &typeStr, 
               const std::string &nomme) :
   SpaceObject    (typeId, typeStr, nomme)
{
}


Formation::~Formation()
{
}


Formation::Formation(const Formation& orig) :
   SpaceObject    (orig)
{
}


Formation& Formation::operator=(const Formation& orig)
{
   if (&orig == this)
      return *this;
      
   return *this;
}


GmatBase* Formation::Clone() const
{
   return new Formation(*this);
}


Integer Formation::GetParameterCount(void) const
{
   return FormationParamCount;
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Formation::GetParameterText(const Integer id) const
{
   if (id >= SpaceObjectParamCount && id < FormationParamCount)
      return PARAMETER_TEXT[id - SpaceObjectParamCount];
   return SpaceObject::GetParameterText(id);
}


//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 */
//------------------------------------------------------------------------------
Integer Formation::GetParameterID(const std::string &str) const
{
   for (Integer i = SpaceObjectParamCount; i < FormationParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - SpaceObjectParamCount])
         return i;
   }
   
   return SpaceObject::GetParameterID(str);
}


//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 */
//------------------------------------------------------------------------------
Gmat::ParameterType Formation::GetParameterType(const Integer id) const
{
   if (id >= SpaceObjectParamCount && id < FormationParamCount)
      return PARAMETER_TYPE[id - SpaceObjectParamCount];
      
   return GmatBase::GetParameterType(id);
}


//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 */
//------------------------------------------------------------------------------
std::string Formation::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}

#include <iostream>
bool Formation::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == ADDED_SPACECRAFT) {
      if (find(componentNames.begin(), componentNames.end(), value) != 
              componentNames.end())
         return false;
      componentNames.push_back(value);
      return true;
   }
   return SpaceObject::SetStringParameter(id, value);
}


bool Formation::SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index)
{
   if (id == ADDED_SPACECRAFT) {
      return false;
   }
   return SpaceObject::SetStringParameter(id, value, index);
}


const StringArray& Formation::GetStringArrayParameter(const Integer id) const
{
   if (id == ADDED_SPACECRAFT)
      return componentNames;
   return SpaceObject::GetStringArrayParameter(id);
}


//bool Formation::SetStringParameter(const std::string &label, 
//                                   const std::string &value)
//{
//}
//
//
//bool Formation::SetStringParameter(const std::string &label, 
//                                   const std::string &value,
//                                   const Integer index)
//{
//}
//
//
//const StringArray& Formation::GetStringArrayParameter(const std::string &label)const
//{
//}
