//$Header$
//------------------------------------------------------------------------------
//                                PenDown
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
 * Class implementation for the PenDown command
 */
//------------------------------------------------------------------------------


#include "PenDown.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// ~PenDown()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
PenDown::PenDown() :
   GmatCommand    ("PenDown"),
   plotName       (""),
   thePlot        (NULL)
{
}


//------------------------------------------------------------------------------
// ~PenDown()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
PenDown::~PenDown()
{
}


//------------------------------------------------------------------------------
// PenDown(const PenDown &c)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <c> The instance that gets copied.
 */
//------------------------------------------------------------------------------
PenDown::PenDown(const PenDown &c) :
   GmatCommand    (c),
   plotName       (c.plotName),
   thePlot        (NULL)
{
}

//------------------------------------------------------------------------------
// PenDown& operator=(const PenDown &c)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <c> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
PenDown& PenDown::operator=(const PenDown &c)
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
GmatBase* PenDown::Clone() const
{
   return new PenDown(*this);
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
bool PenDown::InterpretAction()
{
   Integer loc = generatingString.find("PenDown", 0) + 7, end;
   const char *str = generatingString.c_str();
   while (str[loc] == ' ')
      ++loc;
    
   // Find the Subscriber list
   end = generatingString.find(" ", loc);
   plotName = generatingString.substr(loc, end-loc);
   
   #ifdef DEBUG_PENDOWN
      MessageInterface::ShowMessage(
         "Plot to be PenDowned: %s\n", plotName.c_str());
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
bool PenDown::Initialize()
{
   #ifdef DEBUG_PENDOWN
      MessageInterface::ShowMessage("PenDown::Initialize() entered\n");
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
            "PenDown command for this object, but it is a " + 
            xy->GetTypeName());      
   }
   else 
   {
      MessageInterface::ShowMessage
         ("PenDown command cannot find XY Plot \"%s\"; command has no effect."
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
bool PenDown::Execute()
{
   if (thePlot)
      thePlot->TakeAction("PenDown");
   return true;
}
