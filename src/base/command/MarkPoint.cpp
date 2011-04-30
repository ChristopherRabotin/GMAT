//$Id$
//------------------------------------------------------------------------------
//                                MarkPoint
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2009/10/06
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG06CA54C
//
/**
 * Class implementation for the MarkPoint command
 */
//------------------------------------------------------------------------------


#include "MarkPoint.hpp"
#include "MessageInterface.hpp"
#include "StringUtil.hpp"

//#define DEBUG_MarkPoint


//------------------------------------------------------------------------------
// ~MarkPoint()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
MarkPoint::MarkPoint() :
   GmatCommand    ("MarkPoint")
{
   plotNameList.clear();
   thePlotList.clear();
}


//------------------------------------------------------------------------------
// ~MarkPoint()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
MarkPoint::~MarkPoint()
{
   plotNameList.clear();
   thePlotList.clear();
}


//------------------------------------------------------------------------------
// MarkPoint(const MarkPoint &c)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param <c> The instance that gets copied.
 */
//------------------------------------------------------------------------------
MarkPoint::MarkPoint(const MarkPoint &c) :
   GmatCommand    (c),
   plotNameList   (c.plotNameList),
   thePlotList    (c.thePlotList)
{
}

//------------------------------------------------------------------------------
// MarkPoint& operator=(const MarkPoint &c)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 * 
 * @param <c> The command that gets copied.
 * 
 * @return A reference to this instance.
 */
//------------------------------------------------------------------------------
MarkPoint& MarkPoint::operator=(const MarkPoint &c)
{
   if (&c != this)
   {
      plotNameList = c.plotNameList;
      thePlotList.clear();
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
GmatBase* MarkPoint::Clone() const
{
   return new MarkPoint(*this);
}

//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by the Achieve.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& MarkPoint::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SUBSCRIBER);
   return refObjectTypes;
}



//------------------------------------------------------------------------------
// const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Accesses arrays of names for referenced objects.
 * 
 * @param type Type of object requested.
 * 
 * @return the StringArray containing the referenced object names.
 */
//------------------------------------------------------------------------------
const StringArray& MarkPoint::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   // There are only subscribers, so ignore object type
   return plotNameList;
}


//------------------------------------------------------------------------------
// bool InterpretAction()
//------------------------------------------------------------------------------
bool MarkPoint::InterpretAction()
{
   Integer loc = generatingString.find("MarkPoint", 0) + 9;
   const char *str = generatingString.c_str();
   while (str[loc] == ' ')
      ++loc;

   // this command, for compatability with MATLAB, should not have
   // parentheses (except to indicate array elements), brackets, or braces
   if (!GmatStringUtil::HasNoBrackets(str, false))
   {
      std::string msg = 
         "The MarkPoint command is not allowed to contain brackets, braces, or "
         "parentheses";
      throw CommandException(msg);
   }

   // Find the Subscriber list
   std::string sub = generatingString.substr(loc, generatingString.size()-loc);
   StringArray parts = GmatStringUtil::SeparateBy(sub," ", false);
   Integer partsSz = (Integer) parts.size();
   #ifdef DEBUG_MarkPoint
      MessageInterface::ShowMessage("In MarkPoint::InterpretAction, parts = \n");
      for (Integer jj = 0; jj < partsSz; jj++)
         MessageInterface::ShowMessage("   %s\n", parts.at(jj).c_str());
   #endif
   if (partsSz < 1) // 'MarkPoint' already found
      throw CommandException("Missing field in MarkPoint command");
   for (Integer ii = 0; ii < partsSz; ii++)
      plotNameList.push_back(parts.at(ii));
   
   #ifdef DEBUG_MarkPoint
      MessageInterface::ShowMessage("Plots to be MarkPointed:\n");
      for (unsigned int ii = 0; ii < plotNameList.size(); ii++)
         MessageInterface::ShowMessage("   %s\n", (plotNameList.at(ii)).c_str());
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
bool MarkPoint::Initialize()
{
   #ifdef DEBUG_MarkPoint
      MessageInterface::ShowMessage("MarkPoint::Initialize() entered\n");
   #endif
      
   GmatCommand::Initialize();
   
   GmatBase *xy;
   thePlotList.clear();
   
   for (unsigned int ii = 0; ii < plotNameList.size(); ii++)
   {
      if ((xy = FindObject(plotNameList.at(ii))) != NULL) 
      {
         if (xy->GetTypeName() == "XYPlot") 
            thePlotList.push_back((XyPlot*) xy);
            //thePlot = (XyPlot*)xy;
         else
            throw CommandException(
               "Object named \"" + plotNameList.at(ii) + "\" should be an XYPlot to use the "
               "MarkPoint command for this object, but it is a " +
               xy->GetTypeName());      
      }
      else 
      {
         MessageInterface::ShowMessage
            ("MarkPoint command cannot find XY Plot \"%s\"; command has no effect."
            "\n", (plotNameList.at(ii)).c_str());
         return false;
      }
   }
   
   #ifdef DEBUG_MarkPoint
      MessageInterface::ShowMessage
         ("   thePlotList.size()=%d\n", thePlotList.size());
      MessageInterface::ShowMessage("MarkPoint::Initialize() returning true\n");
   #endif
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
bool MarkPoint::Execute()
{
   #ifdef DEBUG_MarkPoint
      MessageInterface::ShowMessage
         ("MarkPoint::Execute() thePlotList.size()=%d\n", thePlotList.size());
   #endif
      
   for (unsigned int ii = 0; ii < thePlotList.size(); ii++)
   {
      if (thePlotList.at(ii))
         if (!(thePlotList.at(ii)->TakeAction("MarkPoint"))) return false;
   }
   
   #ifdef DEBUG_MarkPoint
      MessageInterface::ShowMessage("MarkPoint::Execute() returning true\n");
   #endif
   return true;
}
