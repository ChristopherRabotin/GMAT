//$Header$
//------------------------------------------------------------------------------
//                                   Vary
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway
// Created: 2004/01/22
//
/**
 * Definition for the Vary command class
 */
//------------------------------------------------------------------------------


#ifndef Vary_hpp
#define Vary_hpp
 

#include "BranchCommand.hpp"
#include "Solver.hpp"
#include "ElementWrapper.hpp"


/**
 * Command that manages processing for entry to the Varyer loop
 *
 * The Vary command manages the Varyer loop.  All Varyers implement a state
 * machine that evaluates the current state of the Varying process, and provides 
 * data to the command sequence about the next step to be taken in the Varying
 * process.  As far as the Vary command is concerned, there are 3 possible steps 
 * to take: 
 *
 * 1.  Fire the Varyer to perform a calculation.
 *
 * 2.  Run through the Commands in the Varyer loop.
 *
 * 3.  On convergence, continue with the command sequence following the Varyer loop. 
 *
 * 
 *
 */
class Vary : public GmatCommand
{
public:
   Vary();
   virtual ~Vary();
    
   Vary(const Vary& t);
   Vary&              operator=(const Vary& t);

   // inherited from GmatBase
   virtual GmatBase*  Clone() const;
   virtual const std::string&
                      GetGeneratingString(Gmat::WriteMode mode,
                                          const std::string &prefix,
                                          const std::string &useName);
   // for Ref Objects
   virtual bool        RenameRefObject(const Gmat::ObjectType type,
                                       const std::string &oldName,
                                       const std::string &newName);
   
   virtual const ObjectTypeArray&
                       GetRefObjectTypeArray();
   virtual const StringArray&
                       GetRefObjectNameArray(const Gmat::ObjectType type);
   
   // Parameter accessors
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;

   virtual Real        GetRealParameter(const Integer id) const;
   virtual Real        SetRealParameter(const Integer id,
                                        const Real value);
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);

// Multiple variables specified on the same line are not allowed in build 2
//    virtual const StringArray& 
//                        GetStringArrayParameter(const Integer id) const; 

   virtual bool        SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                    const std::string &name = "");

   // Inherited methods overridden from the base class
   virtual bool        InterpretAction();
   virtual const StringArray& 
                       GetWrapperObjectNameArray();
   virtual bool        SetElementWrapper(ElementWrapper* toWrapper,
                                         const std::string &withName);
   virtual void        ClearWrappers();

   virtual bool        Initialize();
   virtual bool        Execute();
   virtual void        RunComplete();
   
   // Used to apply corrections to the command
   virtual void        SetInitialValue(Solver *theSolver);
    
protected:
   // Parameter IDs
   enum  
   {
      SOLVER_NAME = GmatCommandParamCount,
      VARIABLE_NAME,
      INITIAL_VALUE,
      PERTURBATION,
      VARIABLE_MINIMUM,
      VARIABLE_MAXIMUM,
      VARIABLE_MAXIMUM_STEP,
      ADDITIVE_SCALE_FACTOR,
      MULTIPLICATIVE_SCALE_FACTOR,
      VaryParamCount
   };

   static const std::string    PARAMETER_TEXT[VaryParamCount -
                                              GmatCommandParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[VaryParamCount -
                                              GmatCommandParamCount];

   /// The name of the solver that does the solving
   std::string         solverName;
   /// The solver instance used to manage the state machine
   Solver              *solver;

   /// Name(s) of the variable(s)
   std::string         variableName;
   /// A wrapper used for the variable's owning object
   ElementWrapper      *variable;

   /// Initial variable value(s)
   std::string         initialValueName;
   /// A wrapper used for the variable's initial value
   ElementWrapper      *initialValue;

   /// Current (nominal) variable value(s)
   Real                currentValue;

   /// Variable perturbation(s)
   std::string         perturbationName;
   /// A wrapper used for the variable perturbation
   ElementWrapper      *perturbation;
   /// Absolute minimum value
   std::string         variableMinimumName;
   /// A wrapper used for the variable's minimum allowed value
   ElementWrapper      *variableMinimum;
   /// Absolute maximum value
   std::string         variableMaximumName;
   /// A wrapper used for the variable's maximum allowed value
   ElementWrapper      *variableMaximum;
   /// Maximum step allowed
   std::string         variableMaximumStepName;
   /// A wrapper used for to set the largest allowed change in the variable
   ElementWrapper      *variableMaximumStep;
   /// additive scale factor for optimizers
   std::string         additiveScaleFactorName;
   /// A wrapper used for the additive scale factor
   ElementWrapper      *additiveScaleFactor;
   /// multiplicative scale factor for optimizers
   std::string         multiplicativeScaleFactorName;
   /// A wrapper used for the multiplicative scale factor
   ElementWrapper      *multiplicativeScaleFactor;

   /// The integer ID assigned by the solver to the variable
   Integer             variableID;

   /// Flag used to finalize the solver data during execution
   bool                solverDataFinalized;
   
   /// ???
   std::string         variableValueString;

};


#endif  // Vary_hpp
