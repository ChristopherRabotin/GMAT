// *** File Name : dormandelmikkawyprince68.h
// *** Created   : January 28, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-67521-G                             ***
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
// Modification History      : 1/28/03 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//				      Changes
//                           : 02/21/2004 - L. Ruley, Missions Applications Branch
//				              - Updated style using GMAT cpp style guide
//				              - All Real types to Real types
//                            - All int types to Integer types
// **************************************************************************


#ifndef DormandElMikkawyPrince68_hpp
#define DormandElMikkawyPrince68_hpp


#include "RungeKuttaNystrom.hpp"


/** \brief The 6(8) Nystrom integrator found by Dormand, El-Mikkawy and Prince

  This class implements the Runge-Kutta-Nystrom integrator from the 1987 paper
  by Dormand, El-Mikkawy and Prince, with corrections noted 1n 1991.  The 
  integrator is a 9-stage Nystrom integrator, with error control on both the 
  dependent variables and their derivatives.
 */
class GMAT_API DormandElMikkawyPrince68 :  public RungeKuttaNystrom
{
public:
//    DormandElMikkawyPrince68(void);
    DormandElMikkawyPrince68(const std::string &nomme = "");
    virtual ~DormandElMikkawyPrince68(void);


    DormandElMikkawyPrince68(const DormandElMikkawyPrince68& dpn);
    DormandElMikkawyPrince68&   operator=(const DormandElMikkawyPrince68& dpn);

    virtual char*               GetType(void) const;
    virtual Propagator*         Clone(void) const;

protected:
    virtual void                SetCoefficients(void);
   
};


#endif
