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
//                              Changes:
//                                - Updated style using GMAT cpp style guide
//
//                           : 10/20/2003 - W. Waktola, Missions Applications Branch
//                              Changes:
//                                - All double types to Real types
//                                - All primitive int types to Integer types
//                                - GetParameter to GetRealParameter
//                                - SetParameter to SetRealParameter
//                              Removals:
//                                - GetParameterName()
//                              Additions:
//                                - PARAMTER_TEXT[]
//                                - PARAMETER_TYPE[]
//                                - GetParameterText()
//                                - GetParameterID()
//                                - GetParameterType()
//                                - GetParameterTypeString()
//                                - GetRealParameter()
//                                - SetRealParameter()
// **************************************************************************

#ifndef PointMassForce_hpp
#define PointMassForce_hpp

#include "gmatdefs.hpp"
#include "PhysicalModel.hpp"
#include "Planet.hpp"

class GMAT_API PointMassForce : public PhysicalModel
{
public:
    
    //loj: 3/12/04 added name
    PointMassForce(const std::string &name = "", Integer satcount = 1);
    virtual ~PointMassForce(void);
    PointMassForce(const PointMassForce& pmf);
    PointMassForce& operator= (const PointMassForce& pmf);

    bool GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1);
    bool GetComponentMap(Integer * map, Integer order) const;
    bool Initialize(void);
    virtual Real EstimateError(Real * diffs, Real * answer) const;

    //waw: added o3/16/04
    Planet* GetBody();
    void SetBody(Planet *body);
    bool SetBody(const std::string &name);

    virtual std::string GetParameterText(const Integer id) const;
    virtual Integer     GetParameterID(const std::string &str) const;
    virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
    virtual std::string GetParameterTypeString(const Integer id) const;
    virtual Real GetRealParameter(const Integer id) const;
    virtual Real SetRealParameter(const Integer id, const Real value);    
    
    virtual std::string GetStringParameter(const Integer id) const;
    virtual std::string GetStringParameter(const std::string &value) const;
    virtual bool        SetStringParameter(const Integer id,
                                           const std::string &value);
    virtual bool        SetStringParameter(const std::string &label,
                                           const std::string &value);

protected:
    // Parameter IDs
    enum
    {
        EPOCH = PhysicalModelParamCount,  /// ID for the epoch, used by PointMass and PointMassForce instances
        MU,
        RADIUS,  /// ID for the radius, used by PointMass and PointMassForce instances
        //flatteningParameter,  /// Flattening factor ID, used by PointMass and PointMassForce instances
        //poleRadiusParameter,  /// Polar radius ID (read only), used by PointMass and PointMassForce instances
        ESTIMATE_METHOD,  /// Error estimate type: 1.0 for component estimate, 2.0 for vector
        BODY,    /// ID for the body, used by PointMass and PointMassForce instances
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
    //waw: added 03/16/04
    Planet *thePlanet;
};

#endif  // PointMassForce_hpp

