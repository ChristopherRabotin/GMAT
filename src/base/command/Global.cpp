//$Header$
//------------------------------------------------------------------------------
//                                 Global
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
 * Class implementation for the Global command
 */
//------------------------------------------------------------------------------


#include "Global.hpp"
#include "MessageInterface.hpp"
#include "CommandException.hpp"

//#define DEBUG_GLOBAL

//---------------------------------
// static data
//---------------------------------
//const std::string
//Global::PARAMETER_TEXT[GlobalParamCount - ManageObjectParamCount] =
//{
//};

//const Gmat::ParameterType
//Global::PARAMETER_TYPE[GlobalParamCount - ManageObjectParamCount] =
//{
//};

//------------------------------------------------------------------------------
// Global()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
Global::Global() :
   ManageObject("Global")
{
}


//------------------------------------------------------------------------------
// ~Global()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Global::~Global()
{
}


//------------------------------------------------------------------------------
// Global(const Global &gl)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <gl> The instance that gets copied.
 */
//------------------------------------------------------------------------------
Global::Global(const Global &gl) :
   ManageObject(gl)
{
}


//------------------------------------------------------------------------------
// Global& operator=(const Global &gl)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <gl> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
Global& Global::operator=(const Global &gl)
{
   if (&gl != this)
   {
      ManageObject::operator=(gl);
   }
   
   return *this;
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
GmatBase* Global::Clone() const
{
   return new Global(*this);
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
bool Global::Initialize()
{
   #ifdef DEBUG_GLOBAL
      MessageInterface::ShowMessage("Global::Initialize() entered\n");
   #endif
      
   ManageObject::Initialize();
   
   return true;
}


//---------------------------------------------------------------------------
//  bool GmatCommand::Execute()
//---------------------------------------------------------------------------
/**
 * The method that is fired to perform this Global command.
 *
 * @return true if the Global runs to completion, false if an error
 *         occurs.
 */
//---------------------------------------------------------------------------
bool Global::Execute()
{
   #ifdef DEBUG_GLOBAL
      MessageInterface::ShowMessage("Global::Execute() entered\n");
   #endif
   bool isOK = true;
   Integer sz = objectNames.size();
   for (Integer ii=0; ii<sz; ii++)
      isOK += MakeGlobal(objectNames.at(ii));
   return isOK;
}
