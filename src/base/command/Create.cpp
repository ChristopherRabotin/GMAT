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

//#define DEBUG_CREATE

//---------------------------------
// static data
//---------------------------------
const std::string
Create::PARAMETER_TEXT[CreateParamCount - ManageObjectParamCount] =
{
      "ObjectType",
      "Creations",
};

const Gmat::ParameterType
Create::PARAMETER_TYPE[CreateParamCount - ManageObjectParamCount] =
{
      Gmat::STRING_TYPE,
      Gmat::OBJECTARRAY_TYPE,
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
   objectType  (cr.objectType),
   creations   (cr.creations)
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
      objectType = cr.objectType;
      creations  = cr.creations;
   }
   
   return *this;
}

GmatBase* Create::GetRefObject(const Gmat::ObjectType type,
                               const std::string &name)
{
   // Need to check type here in case requested type is not
   // of the correct type
   Integer sz = (Integer) creations.size();
   for (Integer ii=0; ii < sz; ii++)
      if ((creations.at(ii))->GetName() == name)
         return creations.at(ii);
   return NULL;
}

bool Create::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                          const std::string &name)
{
   if (creations.size() > 0)
      throw CommandException(
            "Reference object for Create command already set,\n"); 
//   if ((!GmatStringUtil::IsBlank(objectType)) &&
//       ... need to check type match here ...
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
      MessageInterface::ShowMessage("Create::Initialize() entered\n");
   #endif
      
   ManageObject::Initialize();
   // Clone the reference object to create as many of that requested
   // type of object as needed
   if (creations.size() <= 0)
   {
      std::string ex = "No reference object of type """ + objectType;
      ex += """ set for Create command.\n ";
      throw CommandException(ex);
   }
   if (creations.size() > 1)
   {
      std::string ex = "Too many reference objects of type """ + objectType;
      ex += """ set for Create command.\n ";
      throw CommandException(ex);
   }
   // set the name for the first, reference, object
   creations.at(0)->SetName(objectNames.at(0));
   // clone the other needed objects from the reference one
   Integer numNames = (Integer) objectNames.size();
   for (Integer jj = 1; jj < numNames; jj++)
   {
      creations.push_back((creations.at(0))->Clone());
      creations.at(jj)->SetName(objectNames.at(jj));
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
   bool isOK = true;
   // ............ todo
   return isOK;
}
