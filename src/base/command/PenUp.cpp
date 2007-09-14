//$Header$
//------------------------------------------------------------------------------
//                                 PenUp
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2004/02/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Class implementation for the PenUp command
 */
//------------------------------------------------------------------------------


#include "PenUp.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// PenUp()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
PenUp::PenUp() :
   GmatCommand    ("PenUp"),
   plotName       (""),
   thePlot        (NULL)
{
}


//------------------------------------------------------------------------------
// ~PenUp()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PenUp::~PenUp()
{
}


//------------------------------------------------------------------------------
// PenUp(const PenUp &c)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <c> The instance that gets copied.
 */
//------------------------------------------------------------------------------
PenUp::PenUp(const PenUp &c) :
   GmatCommand    (c),
   plotName       (c.plotName),
   thePlot        (NULL)
{
}


//------------------------------------------------------------------------------
// PenUp& operator=(const PenUp &c)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <c> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
PenUp& PenUp::operator=(const PenUp &c)
{
   if (&c != this)
   {
      plotName = c.plotName;
      thePlot = NULL;
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
GmatBase* PenUp::Clone() const
{
   return new PenUp(*this);
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
bool PenUp::InterpretAction()
{
   Integer loc = generatingString.find("PenUp", 0) + 5, end;
   const char *str = generatingString.c_str();
   while (str[loc] == ' ')
      ++loc;
    
   // Find the Subscriber list
   end = generatingString.find(" ", loc);
   plotName = generatingString.substr(loc, end-loc);
   
   #ifdef DEBUG_PENUP
      MessageInterface::ShowMessage("Plot to be PenUped: %s\n", plotName.c_str());
   #endif

   return true;
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
bool PenUp::Initialize()
{
   #ifdef DEBUG_PENUP
      MessageInterface::ShowMessage("PenUp::Initialize() entered\n");
   #endif
      
   GmatCommand::Initialize();
   
   GmatBase *xy;
   
   if ((*objectMap).find(plotName) != objectMap->end()) 
   {
      xy = (GmatBase *)(*objectMap)[plotName];
      if (xy->GetTypeName() == "XYPlot") 
         thePlot = (TsPlot*)xy;
      else
         throw CommandException(
            "Object named \"" + plotName + "\" should be an XYPlot to use the "
            "PenUp command for this object, but it is a " + 
            xy->GetTypeName());      
   }
   else 
   {
      MessageInterface::ShowMessage
         ("PenUp command cannot find XY Plot \"%s\"; command has no effect."
         "\n", plotName.c_str());
      return false;
   }

   return true;
}


//---------------------------------------------------------------------------
//  bool GmatCommand::Execute()
//---------------------------------------------------------------------------
/**
 * The method that is fired to perform the GmatCommand.
 *
 * Derived classes implement this method to perform their actions on
 * GMAT objects.
 *
 * @return true if the GmatCommand runs to completion, false if an error
 *         occurs.
 */
//---------------------------------------------------------------------------
bool PenUp::Execute()
{
   if (thePlot)
      thePlot->TakeAction("PenUp");
   return true;
}
