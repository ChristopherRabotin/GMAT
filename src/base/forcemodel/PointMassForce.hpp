//$Header$
//------------------------------------------------------------------------------
//                             PointMassForce
//------------------------------------------------------------------------------
// *** File Name : PointMassForce.hpp
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
//                           : 1/8/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces based on GSFC feedback
//
//                           : 1/21/2003 - D. Conway, Thinking Systems, Inc.
//                             Updated interfaces to support Nystrom code
//
//                           : 10/01/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - Updated style using GMAT cpp style guide
//
//                           : 10/20/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - All double types to Real types
//				  - All primitive int types to Integer types
//				  - EPOCH_PARAMETER to epochParameter
//				  - MU_PARAMETER to muParameter
//				  - RADIUS_PARAMETER to radiusParameter
//				  - FLATTENING_PARAMETER to flatteningParameter
//				  - POLERADIUS_PARAMETER to poleRadiusParameter
//				  - ESTIMATEMETHOD_PARAMETER to estimateMethodParameter
//				  - PARAMETER_COUNT to parameterCount
//				  - GetParameter to GetRealParameter
//				  - SetParameter to SetRealParameter
//				Removals:
//				  - GetParameterName()
//				Additions:
//				  - PARAMTER_TEXT[]
//				  - PARAMETER_TYPE[]
//				  - GetParameterText()
//				  - GetParameterID()
//				  - GetParameterType()
//				  - GetParameterTypeString()
//				  - GetRealParameter()
//				  - SetRealParameter()
// **************************************************************************

#ifndef PointMassForce_hpp
#define PointMassForce_hpp

#include "gmatdefs.hpp"
#include "PhysicalModel.hpp"

class GMAT_API PointMassForce : public PhysicalModel
{
public:
    PointMassForce(Integer satcount = 1);
    virtual ~PointMassForce(void);
    PointMassForce(const PointMassForce& pmf);
    PointMassForce& operator= (const PointMassForce& pmf);

    bool GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1);
    bool GetComponentMap(Integer * map, Integer order) const;
    bool Initialize(void);
    virtual Real EstimateError(Real * diffs, Real * answer) const;

    // Parameter accessor methods -- overridden from GmatBase
    std::string PointMassForce::GetParameterText(const Integer id);
    Integer PointMassForce::GetParameterID(const std::string str);
    Gmat::ParameterType PointMassForce::GetParameterType(const Integer id) const;
    std::string PointMassForce::GetParameterTypeString(const Integer id) const;
    virtual Real GetRealParameter(const Integer id) const;
    virtual Real SetRealParameter(const Integer id, const Real value);    

protected:
    // Start with the parameter IDs and associates strings

    // Parameter IDs
    enum
    {
        epochParameter = PhysicalModelParamCount,  /// ID for the epoch, used by PointMass and PointMassForce instances
        muParameter,  /// ID for the gravity constant, used by PointMass and PointMassForce instances
        radiusParameter,  /// ID for the radius, used by PointMass and PointMassForce instances
        flatteningParameter,  /// Flattening factor ID, used by PointMass and PointMassForce instances
        poleRadiusParameter,  /// Polar radius ID (read only), used by PointMass and PointMassForce instances
        estimateMethodParameter,  /// Error estimate type: 1.0 for component estimate, 2.0 for vector
        PointMassParamCount  /// Count of the parameters for this class 
    };

    static const std::string PARAMETER_TEXT[PointMassParamCount - PhysicalModelParamCount];
    static const Gmat::ParameterType PARAMETER_TYPE[PointMassParamCount - PhysicalModelParamCount];

    /// Gravitational constant for the central body (\f$G*M\f$)
    Real mu;
    /// Epoch of the PhysicalModel
    Real epoch;
    /// Type of error estimate to perform
    Real estimationMethod;
};

#endif  // PointMassForce_hpp
