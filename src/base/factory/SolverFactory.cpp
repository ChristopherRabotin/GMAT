//$Header$
//------------------------------------------------------------------------------
//                            SolverFactory
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel Conway
// Created: 2004/02/02  (Groundhog Day)
//
/**
 *  Implementation code for the SolverFactory class, responsible for creating
 *  targeters, optimizers, and other parameteric scanning objects.
 */
//------------------------------------------------------------------------------


#include "SolverFactory.hpp"

// Headers for the supported Solvers
#include "DifferentialCorrector.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  Solver* CreateSolver(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Solver class. 
 *
 * @param <ofType> type of Solver object to create and return.
 * @param <withName> the name for the newly-created Solver object.
 * 
 * @return A pointer to the created object.
 */
//------------------------------------------------------------------------------
Solver* SolverFactory::CreateSolver(const std::string &ofType,
                                    const std::string &withName)
{
   if (ofType == "DifferentialCorrector")
      return new DifferentialCorrector(withName);
   // Add other solvers here    
    
   return NULL;
}


//------------------------------------------------------------------------------
//  SolverFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SolverFactory. 
 * (default constructor)
 */
//------------------------------------------------------------------------------
SolverFactory::SolverFactory() :
    Factory     (Gmat::SOLVER)
{
   if (creatables.empty())
   {
      creatables.push_back("DifferentialCorrector");
      // Add new descriptors here
   }
}


//------------------------------------------------------------------------------
// ~SolverFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the SolverFactory base class.
 */
//------------------------------------------------------------------------------
SolverFactory::~SolverFactory()
{
}


//------------------------------------------------------------------------------
//  SolverFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SolverFactory.
 *
 * @param <createList> list of creatable solver objects
 *
 */
//------------------------------------------------------------------------------
SolverFactory::SolverFactory(StringArray createList)
{
}


//------------------------------------------------------------------------------
//  SolverFactory(const SolverFactory& fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class SolverFactory.  (copy constructor)
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
SolverFactory::SolverFactory(const SolverFactory& fact) :
    Factory     (fact)
{
   if (creatables.empty())
   {
      creatables.push_back("DifferentialCorrector");
      // Add new descriptors here
   }
}


//------------------------------------------------------------------------------
//  CommandFactory& operator= (const CommandFactory& fact)
//------------------------------------------------------------------------------
/**
 * SolverFactory operator for the SolverFactory base class.
 *
 * @param <fact> the SolverFactory object that is copied.
 *
 * @return "this" SolverFactory with data set to match the input factory (fact).
 */
//------------------------------------------------------------------------------
SolverFactory& SolverFactory::operator=(const SolverFactory& fact)
{
   Factory::operator=(fact);
   return *this;
}
    
  

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

