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
//				Changes:
//				  - Updated style using GMAT cpp style guide
//
//                           : 10/09/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - virtual char* GetParameterName(const int parm) const to
//				    virtual std::string GetParameterName(const int parm) const
//				Additions:
//				  - GetParameterType()
//				  - GetParameterText()
//				  - GetParameterTypeString()
//				  - GetRealParameter()
//				  - SetRealParameter()
//				Removals:
//				  - GetParameter()
//				  - SetParameter()
//				  - ParameterCount()
//
//                           : 10/15/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - All double types to Real types
//				  - All primitive int types to Integer types
//                - STEP_SIZE_PARAMETER to stepSizeParameter
//				Removals:
//				  - static Real parameterUndefined
//				  - SetUndefinedValue()
//				  - GetParameterName(), replaced by GetParameterText()
//				Additions:
//				  - PARAMTER_TEXT[]
//				  - PARAMETER_TYPE[]
//
// **************************************************************************

#ifndef Propagator_hpp
#define Propagator_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "PhysicalModel.hpp"

class GMAT_API Propagator : public GmatBase
{
public:
    Propagator(const std::string &typeStr,
			   const std::string &nomme = "");
    virtual ~Propagator(void);

    Propagator(const Propagator&);
    Propagator& operator=(const Propagator&);

    // Parameter accessor methods -- overridden from GmatBase	
    virtual std::string GetParameterText(const Integer id);
    virtual Integer GetParameterID(const std::string str);
    virtual Gmat::ParameterType GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;
    virtual Real GetRealParameter(const Integer id);
    virtual Real SetRealParameter(const Integer id, const Real value);
	
    virtual void Initialize(void);
    virtual void SetPhysicalModel(PhysicalModel *pPhysicalModel);
    virtual bool Step(Real dt);

    virtual void Update(void);
    const Real* AccessOutState(void);

    virtual Integer GetPropagatorOrder(void) const;    

    //------------------------------------------------------------------------------
    // virtual Propagator* Clone(void) const
    //------------------------------------------------------------------------------
    /**
     * Method used to create a copy of the object
     * Each class that can be instantiated provides an implementation of this
     * method so that copies can be made from a base class pointer. 
     */
    //------------------------------------------------------------------------------
    virtual Propagator* Clone(void) const = 0;

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
    virtual bool Step(void) = 0;

    //------------------------------------------------------------------------------
    // virtual bool RawStep(void)
    //------------------------------------------------------------------------------
    /**
     * Method to take a raw propagation step without error control
     */
    virtual bool RawStep(void) = 0;

    virtual bool RawStep(Real dt);
	
protected:
    // Start with the parameter IDs and associated strings

    // Parameter IDs
    enum
    {
        stepSizeParameter = 0,  /// Stepsize for the propagation
        PropagatorParamCount  /// Count of the parameters for this class
    };

    static const std::string PARAMETER_TEXT[PropagatorParamCount];
    static const Gmat::ParameterType PARAMETER_TYPE[PropagatorParamCount];
	
    /// Size of the default time step
    Real stepSize;
    /// Flag used to determine if the propagator has been initialized
    bool initialized;
    /// Pointer to the input state vector
    Real *inState;
    /// Pointer to the location for the propagated state
    Real *outState;
    /// Number of parameters being propagated
    Integer dimension;
    /// Pointer to the information related to the physics of the system
    PhysicalModel *physicalModel;
};

#endif // Propagator_hpp

