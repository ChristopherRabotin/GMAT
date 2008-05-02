//$Header$
//------------------------------------------------------------------------------
//                                 Create
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Wendy C. Shoan
// Created: 2008.03.14
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CCA54C
//
/**
 * Class implementation for the Create command
 */
//------------------------------------------------------------------------------


#include "Create.hpp"
#include "MessageInterface.hpp"
#include "CommandException.hpp"
#include "StringUtil.hpp"
#include "Array.hpp"

//#define DEBUG_CREATE

//---------------------------------
// static data
//---------------------------------
const std::string
Create::PARAMETER_TEXT[CreateParamCount - ManageObjectParamCount] =
{
      "ObjectType",
};

const Gmat::ParameterType
Create::PARAMETER_TYPE[CreateParamCount - ManageObjectParamCount] =
{
      Gmat::STRING_TYPE,
};

//------------------------------------------------------------------------------
// Create()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
Create::Create() :
   ManageObject("Create")
{
}


//------------------------------------------------------------------------------
// ~Create()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Create::~Create()
{
   creations.clear();
   // assuming objects in this list will be deleted in the Sandbox as members 
   // of the LOS or GOS
}


//------------------------------------------------------------------------------
// Create(const Create &cr)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <cr> The instance that gets copied.
 */
//------------------------------------------------------------------------------
Create::Create(const Create &cr) :
   ManageObject(cr),
   objType  (cr.objType),
   creations(cr.creations)
{
}


//------------------------------------------------------------------------------
// Create& operator=(const Create &cr)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <cr> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Create& Create::operator=(const Create &cr)
{
   if (&cr != this)
   {
      ManageObject::operator=(cr);
      objType    = cr.objType;
      creations  = cr.creations;
   }
   
   return *this;
}

// Parameter access methods - overridden from GmatBase
std::string Create::GetParameterText(const Integer id) const
{
   if (id >= ManageObjectParamCount && id < CreateParamCount)
      return PARAMETER_TEXT[id - ManageObjectParamCount];
   return ManageObject::GetParameterText(id);
}

Integer Create::GetParameterID(const std::string &str) const
{
   for (Integer i = ManageObjectParamCount; i < CreateParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - ManageObjectParamCount])
         return i;
   }
   
   return ManageObject::GetParameterID(str);
}

Gmat::ParameterType Create::GetParameterType(const Integer id) const
{
   if (id >= ManageObjectParamCount && id < CreateParamCount)
      return PARAMETER_TYPE[id - ManageObjectParamCount];
      
   return ManageObject::GetParameterType(id);
}

std::string Create::GetParameterTypeString(const Integer id) const
{
   return ManageObject::PARAM_TYPE_STRING[GetParameterType(id)];
}


std::string Create::GetStringParameter(const Integer id) const
{
   if (id == OBJECT_TYPE)
   {
      return objType;
   }
   return ManageObject::GetStringParameter(id);
}

std::string Create::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

bool Create::SetStringParameter(const Integer id, 
                                const std::string &value)
{
   if (id == OBJECT_TYPE)
   {
      objType = value;
      return true;
   }
   return ManageObject::SetStringParameter(id, value);
}

bool Create::SetStringParameter(const std::string &label, 
                                const std::string &value)
{
   return SetStringParameter(GetParameterID(label),value);
}


GmatBase* Create::GetRefObject(const Gmat::ObjectType type,
                               const std::string &name)
{
   Integer sz = (Integer) creations.size();
   for (Integer ii=0; ii < sz; ii++)
      if (((creations.at(ii))->GetType() == type) &&
          ((creations.at(ii))->GetName() == name))
            return creations.at(ii);
   return NULL;
}

bool Create::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                          const std::string &name)
{
   if (creations.size() > 0)
      throw CommandException(
            "Reference object for Create command already set.\n"); 
   if ((!GmatStringUtil::IsBlank(objType)) &&
       !(obj->IsOfType(objType)))
      throw CommandException(
            "Reference object for Create command is not of expected type.\n");
   creations.push_back(obj);
   return true;
}

//------------------------------------------------------------------------------
// GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Override of the GmatBase clone method.
 * 
 * @return A new copy of this instance.
 */
//------------------------------------------------------------------------------
GmatBase* Create::Clone() const
{
   return new Create(*this);
}



//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Method that initializes the internal data structures.
 * 
 * @return true if initialization succeeds.
 */
//------------------------------------------------------------------------------
bool Create::Initialize()
{
   #ifdef DEBUG_CREATE
      MessageInterface::ShowMessage("Create::Initialize() entered, for object type %s\n",
            objType.c_str());
   #endif
      
   ManageObject::Initialize();
   // Clone the reference object to create as many of that requested
   // type of object as needed
   if (GmatStringUtil::IsBlank(objType))
      throw CommandException("Object type not set for Create command.\n");
   if (creations.size() <= 0)
   {
      std::string ex = "No reference object of type """ + objType;
      ex += """ set for Create command.\n";
      throw CommandException(ex);
   }
   if (creations.size() > 1)
   {
      std::string ex = "Too many reference objects of type """ + objType;
      ex += """ set for Create command.\n";
      throw CommandException(ex);
   }
   // remove the array indices from the names if necessary
   Integer numNames = (Integer) objectNames.size();
   if (objType == "Array")
   {
      SetArrayInfo();
      numNames = (Integer) arrayNames.size();
      creations.at(0)->SetName(arrayNames.at(0));
      ((Array*) (creations.at(0)))->SetSize(rows.at(0), columns.at(0));
      // clone the other needed objects from the reference one
      for (Integer jj = 1; jj < numNames; jj++)
      {
         creations.push_back((creations.at(0))->Clone());
         creations.at(jj)->SetName(arrayNames.at(jj));
         ((Array*) (creations.at(jj)))->SetSize(rows.at(jj), columns.at(jj));
      }
   }
   else
   {
      creations.at(0)->SetName(objectNames.at(0));
      // clone the other needed objects from the reference one
      for (Integer jj = 1; jj < numNames; jj++)
      {
         creations.push_back((creations.at(0))->Clone());
         creations.at(jj)->SetName(objectNames.at(jj));
      }
   }
   return true;
}


//---------------------------------------------------------------------------
//  bool GmatCommand::Execute()
//---------------------------------------------------------------------------
/**
 * The method that is fired to perform this Create command.
 *
 * @return true if the Create runs to completion, false if an error
 *         occurs.
 */
//---------------------------------------------------------------------------
bool Create::Execute()
{
   #ifdef DEBUG_CREATE
      MessageInterface::ShowMessage("Create::Execute() entered, for object type %s\n",
            objType.c_str());
   #endif
   bool isOK = true;
   StringArray useThese = objectNames;
   if (objType == "Array")  useThese = arrayNames;
   #ifdef DEBUG_CREATE
      MessageInterface::ShowMessage("Create::Execute() executing for objects:\n");
      for (unsigned int ii = 0; ii < useThese.size(); ii++)
         MessageInterface::ShowMessage(" ........ %s\n", (useThese.at(ii)).c_str());
   #endif
   
   // If the type is Array, use the names without the indices

   //put the objects onto the LOS if not already there
   GmatBase *mapObj = NULL;
   for (Integer ii = 0; ii < (Integer) creations.size(); ii++)
   {
      // if it is already in the LOS, make sure the types match
      if (objectMap->find(useThese.at(ii)) != objectMap->end())
      {
         mapObj = (*objectMap)[useThese.at(ii)];
         if (!mapObj->IsOfType(objType))
         {
            std::string ex = "Object of name """ + useThese.at(ii);
            ex += """, but of a different type, already exists in Local Object Store\n";
            throw CommandException(ex);
         }
         if (objType == "Array")
         {
            Integer r1, r2, c1, c2;
            ((Array*) mapObj)->GetSize(r1, c1);
            ((Array*) (creations.at(ii)))->GetSize(r2, c2);
            if ((r1 != r2) || (c1 != c2))
            {
               std::string ex = "Array of name """ + useThese.at(ii);
               ex += """, but with different dimensions already exists in Local Object Store\n";
               throw CommandException(ex);
            }
         }
      }
      // put it into the LOS
      objectMap->insert(std::make_pair(useThese.at(ii),creations.at(ii)));
      // if the type of object created by this Create command is an automatic
      // global, move it to the GOS (an automatic global would have been 
      // created with its isGlobal flag set to true)
      if ((creations.at(ii))->GetIsGlobal()) 
         isOK += MakeGlobal(useThese.at(ii));
   }
   return isOK;
}

void Create::SetArrayInfo()
{
   // Extract the array names, number of rows, and number of columns from the
   // objectNames array
   arrayNames.clear();
   rows.clear();
   columns.clear();
   std::string itsName;
   Integer r = -99;
   Integer c = -99;
   unsigned int sz = objectNames.size();
   for (unsigned int ii = 0; ii < sz; ii++)
   {
      GmatStringUtil::GetArrayIndex(objectNames.at(ii), r, c, itsName, "[]");
      arrayNames.push_back(itsName);
      rows.push_back(r);
      columns.push_back(c);
   }
}
