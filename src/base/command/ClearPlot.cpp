//$Header$
//------------------------------------------------------------------------------
//                               ClearPlot
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
 * Class implementation for the ClearPlot command
 */
//------------------------------------------------------------------------------


#include "ClearPlot.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// ClearPlot()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
ClearPlot::ClearPlot() :
   GmatCommand    ("ClearPlot"),
   plotName       (""),
   thePlot        (NULL)
{
}


//------------------------------------------------------------------------------
// ~ClearPlot()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
ClearPlot::~ClearPlot()
{
}


//------------------------------------------------------------------------------
// ClearPlot(const ClearPlot &c)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 * 
 * @param <c> The command that gets copied.
 */
//------------------------------------------------------------------------------
ClearPlot::ClearPlot(const ClearPlot &c) :
   GmatCommand    (c),
   plotName       (c.plotName),
   thePlot        (NULL)
{
}


//------------------------------------------------------------------------------
// ClearPlot& operator=(const ClearPlot &c)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <c> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
ClearPlot& ClearPlot::operator=(const ClearPlot &c)
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
GmatBase* ClearPlot::Clone() const
{
   return new ClearPlot(*this);
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
bool ClearPlot::InterpretAction()
{
   Integer loc = generatingString.find("ClearPlot", 0) + 9, end;
   const char *str = generatingString.c_str();
   while (str[loc] == ' ')
      ++loc;

   // this command, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(str, false))
   {
      std::string msg = 
         "The ClearPlot command is not allowed to contain brackets, braces, or "
         "parentheses";
      throw CommandException(msg);
   }

   // Find the Subscriber list
   end = generatingString.find(" ", loc);
   plotName = generatingString.substr(loc, end-loc);
   
   #ifdef DEBUG_CLEARPLOT
      MessageInterface::ShowMessage("Plot to be cleared: %s\n", plotName.c_str());
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
bool ClearPlot::Initialize()
{
   #ifdef DEBUG_CLEARPLOT
      MessageInterface::ShowMessage("ClearPlot::Initialize() entered\n");
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
            "ClearPlot command for this object, but it is a " + 
            xy->GetTypeName());      
   }
   else 
   {
      MessageInterface::ShowMessage
         ("ClearPlot command cannot find XY Plot \"%s\"; command has no effect."
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
bool ClearPlot::Execute()
{
   if (thePlot)
      thePlot->TakeAction("ClearData");
   return true;
}
