//$Id$
//------------------------------------------------------------------------------
//                                 PenUp
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
 * Class implementation for the PenUp command
 */
//------------------------------------------------------------------------------


#include "PenUp.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"

//#define DEBUG_PENUP


//------------------------------------------------------------------------------
// PenUp()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
PenUp::PenUp() :
   PlotCommand    ("PenUp")
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
   PlotCommand    (c)
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
GmatBase* PenUp::Clone() const
{
   return new PenUp(*this);
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
               "PenUp command for this object, but it is a " + 
               sub->GetTypeName());      
      }
      else 
      {
         MessageInterface::ShowMessage
            ("PenUp command cannot find Plot \"%s\"; command has no effect."
            "\n", (plotNameList.at(ii)).c_str());
         return false;
      }
   }

   return true;
}


//---------------------------------------------------------------------------
//  bool Execute()
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
bool PenUp::Execute()
{
   for (unsigned int ii = 0; ii < thePlotList.size(); ii++)
   {
      if (thePlotList.at(ii))
         if (!(thePlotList.at(ii)->TakeAction("PenUp"))) return false;
   }
   
   // Build command summary
   BuildCommandSummary(true);
   
   return true;
}
