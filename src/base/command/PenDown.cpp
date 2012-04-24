//$Id$
//------------------------------------------------------------------------------
//                                PenDown
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
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
//#include "StringUtil.hpp"

//#define DEBUG_PENDOWN


//------------------------------------------------------------------------------
// ~PenDown()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
PenDown::PenDown() :
   PlotCommand    ("PenDown")
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
   PlotCommand    (c)
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
      PlotCommand::operator=(c);
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
      
   PlotCommand::Initialize();
   
   GmatBase *sub;
   thePlotList.clear();
   
   for (unsigned int ii = 0; ii < plotNameList.size(); ii++)
   {
      if ((sub = FindObject(plotNameList.at(ii))) != NULL) 
      {
         if (sub->GetTypeName() == "XYPlot" ||
             sub->GetTypeName() == "OrbitView" ||
             sub->GetTypeName() == "GroundTrackPlot") 
            thePlotList.push_back((Subscriber*) sub);
         else
            throw CommandException(
               "Object named \"" + plotNameList.at(ii) +
               "\" should be an XYPlot, OrbitView or GroundTrackPlot to use the "
               "PenDown command for this object, but it is a " + 
               sub->GetTypeName());      
      }
      else 
      {
         MessageInterface::ShowMessage
            ("PenDown command cannot find Plot \"%s\"; command has no effect."
            "\n", (plotNameList.at(ii)).c_str());
         return false;
      }
   }
   
   #ifdef DEBUG_PENDOWN
      MessageInterface::ShowMessage
         ("   thePlotList.size()=%d\n", thePlotList.size());
      MessageInterface::ShowMessage("PenDown::Initialize() returning true\n");
   #endif
   return true;
}


//---------------------------------------------------------------------------
//  bool PlotCommand::Execute()
//---------------------------------------------------------------------------
/**
 * The method that is fired to perform the PlotCommand.
 *
 * Derived classes implement this method to perform their actions on
 * GMAT objects.
 *
 * @return true if the PlotCommand runs to completion, false if an error
 *         occurs.
 */
//---------------------------------------------------------------------------
bool PenDown::Execute()
{
   #ifdef DEBUG_PENDOWN
      MessageInterface::ShowMessage
         ("PenDown::Execute() thePlotList.size()=%d\n", thePlotList.size());
   #endif
      
   for (unsigned int ii = 0; ii < thePlotList.size(); ii++)
   {
      if (thePlotList.at(ii))
         if (!(thePlotList.at(ii)->TakeAction("PenDown"))) return false;
   }
   
   // Build command summary
   BuildCommandSummary(true);
   
   #ifdef DEBUG_PENDOWN
      MessageInterface::ShowMessage("PenDown::Execute() returning true\n");
   #endif
   return true;
}
