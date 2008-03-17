//$Header$
//------------------------------------------------------------------------------
//                            CommandFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy Shoan
// Created: 2003/10/09
//
/**
 *  Implementation code for the CommandFactory class, responsible for
 *  creating Command objects.
 */
//------------------------------------------------------------------------------
 
#include "gmatdefs.hpp"
#include "Factory.hpp"
#include "CommandFactory.hpp"
#include "Propagate.hpp"      // for Propagate command
#include "Toggle.hpp"         // for Toggle command
#include "Maneuver.hpp"       // for Maneuver command
#include "Target.hpp"         // for Target command
#include "Vary.hpp"           // for Vary command
#include "Achieve.hpp"        // for Achieve command
#include "EndTarget.hpp"      // for EndTarget command
#include "For.hpp"            // for FOR command
#include "EndFor.hpp"         // for EndFor command
#include "If.hpp"             // for IF command
#include "Else.hpp"           // for Else command
#include "EndIf.hpp"          // for EndIf command
#include "While.hpp"          // for FOR command
#include "EndWhile.hpp"       // for EndFor command
#include "Assignment.hpp"     // for Assignment (GMAT) command  
#include "Report.hpp"         // for Report command
#include "Save.hpp"           // for Save command  
#include "Stop.hpp"           // for Save command  
#include "CallFunction.hpp"   // for CallFunction command
#include "Assignment.hpp"     // for Assignment command
#include "BeginFiniteBurn.hpp"// for BeginFiniteBurn command
#include "EndFiniteBurn.hpp"  // for EndFiniteBurn command
#include "BeginScript.hpp"    // for BeginScript command
#include "EndScript.hpp"      // for EndScript command
#include "BeginFunction.hpp"  // for BeginFunction command
#include "EndFunction.hpp"    // for EndFunction command
#include "Optimize.hpp"       // for Optimize command
#include "EndOptimize.hpp"    // for EndOptimize command
#include "Minimize.hpp"       // for Minimize command
#include "NonlinearConstraint.hpp" // for NonlinearConstraint command

#include "ClearPlot.hpp"      // for ClearPlot command
#include "PenUp.hpp"          // for PenUp command
#include "PenDown.hpp"        // for PenDown command
#include "Global.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatCommand* CreateCommand(const std::string &ofType, 
//                             const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested command class 
 *
 * @param <ofType>   type of command object to create and return.
 * @param <withName> name of the command (currently not used).
 *
 * @return command object
 *
 * @note As of 2003/10/14, we are ignoring the withname parameter.  Use of this
 *       parameter may be added later.
 */
//------------------------------------------------------------------------------
GmatCommand* CommandFactory::CreateCommand(const std::string &ofType,
                                           const std::string &withName)
{
    if (ofType == "Propagate")
        return new Propagate;
    else if (ofType == "Toggle")
        return new Toggle;
    else if (ofType == "Maneuver")
        return new Maneuver;
    else if (ofType == "Target")
        return new Target;
    else if (ofType == "Vary")
        return new Vary;
    else if (ofType == "Achieve")
        return new Achieve;
    else if (ofType == "EndTarget")
        return new EndTarget;
    else if (ofType == "For")
        return new For;
    else if (ofType == "EndFor")
        return new EndFor;
    else if (ofType == "While")
        return new While;
    else if (ofType == "EndWhile")
        return new EndWhile;
    else if (ofType == "If")
        return new If;
    else if (ofType == "Else")
        return new Else;
    else if (ofType == "EndIf")
        return new EndIf;
    else if (ofType == "GMAT")
        return new Assignment;
    else if (ofType == "Report")
        return new Report;
    else if (ofType == "Save")
        return new Save;
    else if (ofType == "CallFunction")
        return new CallFunction;
    else if (ofType == "Assignment")
         return new Assignment;
    else if (ofType == "BeginFiniteBurn")
        return new BeginFiniteBurn;
    else if (ofType == "EndFiniteBurn")
         return new EndFiniteBurn;
    else if (ofType == "BeginScript")
        return new BeginScript;
    else if (ofType == "EndScript")
         return new EndScript;
    else if (ofType == "BeginFunction")
        return new BeginFunction;
    else if (ofType == "EndFunction")
         return new EndFunction;
    else if (ofType == "Stop")
        return new Stop;
    else if (ofType == "Optimize")
        return new Optimize;
    else if (ofType == "EndOptimize")
        return new EndOptimize;
    else if (ofType == "Minimize")
        return new Minimize;
    else if (ofType == "NonlinearConstraint")
        return new NonlinearConstraint;
    else if (ofType == "ClearPlot")
        return new ClearPlot;
    else if (ofType == "PenUp")
        return new PenUp;
    else if (ofType == "PenDown")
        return new PenDown;
    else if (ofType == "Global")
        return new Global;
   // add more here .......
   else 
   {
      return NULL;   // doesn't match any known type of command
   }
   
}


//------------------------------------------------------------------------------
//  CommandFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CommandFactory.
 * (default constructor)
 *
 */
//------------------------------------------------------------------------------
CommandFactory::CommandFactory() :
    Factory(Gmat::COMMAND)
{
   if (creatables.empty())
   {
      creatables.push_back("Toggle");  // default type for this factory
      creatables.push_back("Propagate");  // default type for this factory
      creatables.push_back("Maneuver");
      // Commands related to the targeter
      creatables.push_back("Target");
      creatables.push_back("Vary");
      creatables.push_back("Achieve");
      creatables.push_back("EndTarget");
      creatables.push_back("For");
      creatables.push_back("EndFor");
      creatables.push_back("If");
      creatables.push_back("Else");
      creatables.push_back("EndIf");
      creatables.push_back("While");
      creatables.push_back("EndWhile");
      creatables.push_back("GMAT");
      creatables.push_back("Report");
      creatables.push_back("Save");
      creatables.push_back("CallFunction");
      creatables.push_back("Assignment");
      creatables.push_back("BeginFiniteBurn");
      creatables.push_back("EndFiniteBurn");
      creatables.push_back("BeginScript");
      creatables.push_back("EndScript");
      creatables.push_back("BeginFunction");
      creatables.push_back("EndFunction");
      creatables.push_back("Stop");
      creatables.push_back("Optimize");
      creatables.push_back("EndOptimize");
      creatables.push_back("Minimize");
      creatables.push_back("NonlinearConstraint");
      creatables.push_back("ClearPlot");
      creatables.push_back("PenUp");
      creatables.push_back("PenDown");
      creatables.push_back("Global");
   }
}

//------------------------------------------------------------------------------
//  CommandFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class CommandFactory.
 *
 * @param <createList> list of creatable command objects
 *
 */
//------------------------------------------------------------------------------
CommandFactory::CommandFactory(StringArray createList) :
    Factory(createList,Gmat::COMMAND)
{
}

//------------------------------------------------------------------------------
//  CommandFactory(const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the (base) class CommandFactory (called by
 * copy constructors of derived classes).  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
CommandFactory::CommandFactory(const CommandFactory& fact) :
    Factory(fact)
{
   if (creatables.empty())
   {
      creatables.push_back("Toggle");  // default type for this factory
      creatables.push_back("Propagate");  // default type for this factory
      creatables.push_back("Maneuver");
      // Commands related to the targeter
      creatables.push_back("Target");
      creatables.push_back("Vary");
      creatables.push_back("Achieve");
      creatables.push_back("EndTarget");
      creatables.push_back("For");
      creatables.push_back("EndFor");
      creatables.push_back("If");
      creatables.push_back("Else");
      creatables.push_back("EndIf");
      creatables.push_back("While");
      creatables.push_back("EndWhile");
      creatables.push_back("GMAT");
      creatables.push_back("Report");
      creatables.push_back("Save");
      creatables.push_back("CallFunction");
      creatables.push_back("Assignment");
      creatables.push_back("BeginFiniteBurn");
      creatables.push_back("EndFiniteBurn");
      creatables.push_back("BeginScript");
      creatables.push_back("EndScript");
      creatables.push_back("BeginFunction");
      creatables.push_back("EndFunction");
      creatables.push_back("Stop");
      creatables.push_back("Optimize");
      creatables.push_back("EndOptimize");
      creatables.push_back("Minimize");
      creatables.push_back("NonlinearConstraint");
      creatables.push_back("ClearPlot");
      creatables.push_back("PenUp");
      creatables.push_back("PenDown");
      creatables.push_back("Global");
   }
}

//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the CommandFactory base class.
 *
 * @param <fact> the CommandFactory object whose data to assign to "this" factory.
 *
 * @return "this" CommandFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
CommandFactory& CommandFactory::operator= (const CommandFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}

//------------------------------------------------------------------------------
// ~CommandFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the CommandFactory base class.
 */
//------------------------------------------------------------------------------
CommandFactory::~CommandFactory()
{
   // deletes handled by Factory destructor
}

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

