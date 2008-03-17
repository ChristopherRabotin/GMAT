//$Header$
//------------------------------------------------------------------------------
//                                 ManageObject
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy C. Shoan
// Created: 2008.03.12
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
/**
 * Class implementation for the ManageObject command
 */
//------------------------------------------------------------------------------


#include "ManageObject.hpp"
#include "MessageInterface.hpp"
#include "CommandException.hpp"

//#define DEBUG_MANAGE_OBJECT

//---------------------------------
// static data
//---------------------------------
const std::string
ManageObject::PARAMETER_TEXT[ManageObjectParamCount - GmatCommandParamCount] =
{
   "ObjectNames",
};

const Gmat::ParameterType
ManageObject::PARAMETER_TYPE[ManageObjectParamCount - GmatCommandParamCount] =
{
   Gmat::STRINGARRAY_TYPE,   //"ObjectNames",
};

//------------------------------------------------------------------------------
// ManageObject()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ManageObject::ManageObject(const std::string &typeStr) :
   GmatCommand(typeStr)
{
}


//------------------------------------------------------------------------------
// ~ManageObject()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ManageObject::~ManageObject()
{
}


//------------------------------------------------------------------------------
// ManageObject(const ManageObject &mo)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <mo> The instance that gets copied.
 */
//------------------------------------------------------------------------------
ManageObject::ManageObject(const ManageObject &mo) :
   GmatCommand(mo),
   objectNames   (mo.objectNames)      
{
}


//------------------------------------------------------------------------------
// ManageObject& operator=(const ManageObject &mo)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <mo> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ManageObject& ManageObject::operator=(const ManageObject &mo)
{
   if (&mo != this)
   {
      GmatCommand::operator=(mo);
      objectNames = mo.objectNames;
   }
   
   return *this;
}


//------------------------------------------------------------------------------
//  bool  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @exception <CommandException> thrown if value is already in the list.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool ManageObject::SetStringParameter(const Integer id, 
                                      const std::string &value)
{
   if (id == OBJECT_NAMES)
   {
      Integer sz = objectNames.size();
      for (Integer ii = 0; ii < sz; ii++)
         if (objectNames[ii] == value)
         {
            std::string ex = "Attempting to add """;
            ex += value + """ more than once to list of objects.\n";
            throw CommandException(ex);
         }
      objectNames.push_back(value);
    }
   return GmatCommand::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id, const Integer index)
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID and the index into the array.
 *
 * @param <id> ID for the requested parameter.
 * @param <index> index into the StringArray parameter.
 *
 * @exception <CommandException> thrown if value is out of range
 *
 * @return  string value at index 'index'.
 *
 */
//------------------------------------------------------------------------------
std::string ManageObject::GetStringParameter(const Integer id,
                           const Integer index) const
{
   if (id == OBJECT_NAMES)
   {
      if ((index < 0) || (index >= ((Integer) objectNames.size())))
         throw CommandException(
               "Index out of bounds when attempting to return object name\n");
      return objectNames.at(index);
    }
   return GmatCommand::GetStringParameter(id, index);
}

//------------------------------------------------------------------------------
//  const StringArray&  GetStringArrayParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * This method returns the string array value, given the input
 * parameter ID .
 *
 * @param <id> ID for the requested parameter.
 *
  * @return  string array.
 *
 */
//------------------------------------------------------------------------------
const StringArray& 
ManageObject::GetStringArrayParameter(const Integer id) const
{
   if (id == OBJECT_NAMES)
   {
      return objectNames;
   }
   return GmatCommand::GetStringArrayParameter(id);
}

//---------------------------------------------------------------------------
//  bool GmatCommand::MakeGlobal(const std::string &objName)
//---------------------------------------------------------------------------
/**
 * The method that makes the named object a Global (i.e. moves it
 * from theLOS to the GOS)
 *
 *
 * @return true if successful
 */
//---------------------------------------------------------------------------
bool ManageObject::MakeGlobal(const std::string &objName)
{
   // Check to see if the object is already in the LOS
   GmatBase *mapObj = NULL;
   bool isInLOS = false;
   bool isInGOS = false;
   if ((objectMap->find(objName) != objectMap->end()))
      isInLOS = true;
   if ((globalObjectMap->find(objName) != globalObjectMap->end()))
      isInGOS = true;
   if (isInLOS)
   {
      if (isInGOS)
      {
         std::string ex = "Cannot add more than one object with name """;
         ex += objName + """  to the Global Object Store\n";
         throw CommandException(ex);
      }
      mapObj = (*objectMap)[objName];
      objectMap->erase(objName);
      globalObjectMap->insert(std::make_pair(objName,mapObj));
   }
   else
   {
      if (((isInGOS) && 
          ((*globalObjectMap)[objName] == NULL)) ||
          (!isInGOS))
      {
         std::string ex = "Cannot add NULL object with name """;
         ex += objName + """  to the Global Object Store\n";
         throw CommandException(ex);
      }
   }
   return true;
}

