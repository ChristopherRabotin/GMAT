//$Header$
//------------------------------------------------------------------------------
//                              DerivativeList
//------------------------------------------------------------------------------
// *** File Name : DerivativeList.hpp
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
//                           : 10/02/2003 - W. Waktola, Missions Applications Branch
//				Changes:
//				  - Updated style using GMAT cpp style guide
//
// **************************************************************************
/**
 * A linked list of derivative functions
 * 
 * The DerivativeList class provides a container for the PhysicalModel members
 * used to accumulate multiple individual first derivative functions.  This
 * class is a helper class for physical models that need to accumulate
 * independent sources of derivative information.  It is used, for example, in
 * the ForceModel class for the superposition of forces required to
 * model spacecraft trajectories.
*/

#ifndef DerivativeList_hpp
#define DerivativeList_hpp

#include "PhysicalModel.hpp"

class GMAT_API DerivativeList
{
public:
    DerivativeList(void);
    virtual ~DerivativeList(void);

    DerivativeList(const DerivativeList&);
    DerivativeList& operator=(const DerivativeList&);

    void AddForce(PhysicalModel *);
    PhysicalModel* GetDerivative(void);
    DerivativeList* Next(void);

protected:
    PhysicalModel *derivative;
    DerivativeList *next;

    void SetDerivative(PhysicalModel *pPhysicalModel);
};

#endif
