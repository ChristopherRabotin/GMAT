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

#include "gmatdefs.hpp"
#include "SolverFactory.hpp"

// Headers for the supported Solvers
//#include "QuasiNewton.hpp"
//#include "SteepestDescent.hpp"

#ifdef __USE_MATLAB__
#include "FminconOptimizer.hpp"
#endif

#include "DifferentialCorrector.hpp"
//#include "Broyden.hpp"
//#include "ParametricScanner.hpp"
//#include "MonteCarlo.hpp"

#include "MessageInterface.hpp"  // temporary



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
   
   #if defined __USE_MATLAB__
   if (ofType == "FminconOptimizer")
      return new FminconOptimizer(withName);
   #endif
   
   //if (ofType == "QuasiNewton") 
   //   return new QuasiNewton(withName);
   //else if (ofType == "SteepestDescent")
   //   return new SteepestDescent(withName);
   // else if (ofType == "Broyden")
   //   return new Broyden(withName);
   //else if (ofType == "ParametricScanner")
   //   return new ParametricScanner(withName);
   //else if (ofType == "MonteCarlo")
   //   return new MonteCarlo(withName);
    
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
      //creatables.push_back("QuasiNewton");
      //creatables.push_back("SteepestDescent");
      
      #if defined __USE_MATLAB__
      creatables.push_back("FminconOptimizer");
      #endif
      
      creatables.push_back("DifferentialCorrector");
      //creatables.push_back("Broyden");
      //creatables.push_back("ParametricScanner");
      //creatables.push_back("MonteCarlo");
   }
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
SolverFactory::SolverFactory(StringArray createList) :
Factory(createList, Gmat::SOLVER)
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
      //creatables.push_back("QuasiNewton");
      //creatables.push_back("SteepestDescent");
      
      #if defined __USE_MATLAB__
      creatables.push_back("FminconOptimizer");
      #endif
      
      creatables.push_back("DifferentialCorrector");
      //creatables.push_back("Broyden");
      //creatables.push_back("ParametricScanner");
      //creatables.push_back("MonteCarlo");
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

//---------------------------------
//  protected methods
//---------------------------------

//---------------------------------
//  private methods
//---------------------------------

