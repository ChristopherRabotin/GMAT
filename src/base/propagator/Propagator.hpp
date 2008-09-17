//$Header$
//------------------------------------------------------------------------------
//                              Propagator
//------------------------------------------------------------------------------
// *** File Name : Propagator.hpp
// *** Created   : October 1, 2002
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-66617-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 11/26/2002 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//
//                           : 12/18/2002 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 09/23/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/09/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - virtual char* GetParameterName(const int parm) const to
//                                  virtual std::string GetParameterName(const int parm) const
//                              Additions:
//                                - GetParameterType()
//                                - GetParameterText()
//                                - GetParameterTypeString()
//                                - GetRealParameter()
//                                - SetRealParameter()
//                              Removals:
//                                - GetParameter()
//                                - SetParameter()
//                                - ParameterCount()
//
//                           : 10/15/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                - STEP_SIZE_PARAMETER to stepSizeParameter
//                              Removals:
//                                - static Real parameterUndefined
//                                - SetUndefinedValue()
//                                - GetParameterName(), replaced by GetParameterText()
//                              Additions:
//                                - PARAMTER_TEXT[]
//                                - PARAMETER_TYPE[]
//
// **************************************************************************

#ifndef Propagator_hpp
#define Propagator_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "PhysicalModel.hpp"
#include "PropagatorException.hpp"      // Propagator Exceptions


class GMAT_API Propagator : public GmatBase
{
public:
   Propagator(const std::string &typeStr,
              const std::string &nomme = "");
   virtual ~Propagator(void);
   
   Propagator(const Propagator&);
   Propagator& operator=(const Propagator&);
   
   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   // Parameter accessor methods -- overridden from GmatBase   
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual Real GetRealParameter(const Integer id) const;
   virtual Real GetRealParameter(const std::string &label) const;
   virtual Real SetRealParameter(const Integer id, const Real value);
   virtual Real SetRealParameter(const std::string &label, const Real value);
   
   virtual bool Initialize();
   virtual void SetPhysicalModel(PhysicalModel *pPhysicalModel);
   virtual bool Step(Real dt);
   virtual void SetAsFinalStep(bool fs);
   
   virtual void Update(bool forwards = true);
   virtual void ResetInitialData();
   const Real* AccessOutState();
   
   virtual Integer GetPropagatorOrder(void) const;    
   
   //------------------------------------------------------------------------------
   // virtual bool Step(void)
   //------------------------------------------------------------------------------
   /**
    * Method to propagate by the default step
    * This pure virtual method provides the interface used to advance the state
    * by the amount specified in the stepSize parameter.  This is the core 
    * propagation routine.  
    * Adaptive step propagators will take the desired step if accuracy permits, 
    * and then adjust the stepSize parameter to the estimated optimal size for the 
    * next call.
    */
   //------------------------------------------------------------------------------
   virtual bool Step() = 0;
   
   //------------------------------------------------------------------------------
   // virtual bool RawStep(void)
   //------------------------------------------------------------------------------
   /**
    * Method to take a raw propagation step without error control
    */
   virtual bool RawStep() = 0;
   
   //------------------------------------------------------------------------------
   // virtual bool GetStepTaken()
   //------------------------------------------------------------------------------
   /**
    * Method to report the size of the most recent propagation step.
    */
   virtual Real                GetStepTaken() = 0;
   
   virtual bool RawStep(Real dt);
   
   enum
   {
      INITIAL_STEP_SIZE = GmatBaseParamCount, /// Stepsize for the propagation
      PropagatorParamCount                    /// Count of the parameters for this class
   };
   
protected:
    
   // Start with the parameter IDs and associated strings
   static const std::string
      PARAMETER_TEXT[PropagatorParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[PropagatorParamCount - GmatBaseParamCount];
   
   /// Size of the default time step
   Real stepSize;
   /// Buffer for the stepsize.
   Real stepSizeBuffer;
   /// Flag used to determine if the propagator has been initialized
   bool initialized;
   /// Flag used to determine if the propagator need to reset the initial step
   bool resetInitialData;
   /// Pointer to the input state vector
   Real *inState;
   /// Pointer to the location for the propagated state
   Real *outState;
   /// Number of parameters being propagated
   Integer dimension;
   /// Pointer to the information related to the physics of the system
   PhysicalModel *physicalModel;
   /// Flag used to detect if the code is taking the last Propagate step
   bool finalStep; 
};

#endif // Propagator_hpp

