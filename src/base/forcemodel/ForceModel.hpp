//$Header$
//------------------------------------------------------------------------------
//                              ForceModel
//------------------------------------------------------------------------------
// *** File Name : ForceModel.hpp
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
//                           : 2/28/2003 - D. Conway, Thinking Systems, Inc.
//                             Filled in some code to model cases with 
//                             discontinuous forces (e.g. SRP)
//
//                           : 10/01/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - Updated style using GMAT cpp style guide
//				  - Changed FlightDynamicsForces class to ForceModel class
//
//                           : 10/20/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - All double types to Real types
//				  - All primitive int types to Integer types
// **************************************************************************
/**
 * Container for forces acting on spacecraft.
 * 
 * ForceModel is a class that accumulates the forces acting on one or
 * more spacecraft
*/

#ifndef ForceModel_hpp
#define ForceModel_hpp

#include "PhysicalModel.hpp"
#include "DerivativeList.hpp"

#include <fstream>

class GMAT_API ForceModel : public PhysicalModel
{
public:
//    ForceModel(void);
    ForceModel(const std::string &nomme = "");
    virtual ~ForceModel(void);

    ForceModel(const ForceModel& fdf);

    ForceModel&   operator=(const ForceModel& fdf);

    void AddForce(PhysicalModel *pPhyscialModel);
    virtual bool Initialize(void);
    virtual void IncrementTime(Real dt);
    virtual void SetTime(Real t);

    bool GetDerivatives(Real * state, Real dt = 0.0, Integer order = 1);
    Real EstimateError(Real *diffs, Real *answer) const;

    // new methods for future implementation
    bool HasForce(const std::string &name) // should check if list has the name
      { return false;};
    void DeleteForce(const std::string &name) // should delete the force
      { ;};
    PhysicalModel* GetForce(const std::string &name) // should return the correct force
      { return NULL;};
    Integer GetNumForces() // should return the correct number of force
      { return 1;};

protected:
    DerivativeList * derivatives;
    Real estimationMethod;

};

#endif  // ForceModel_hpp

