//$Header$
//------------------------------------------------------------------------------
//                              ManeuverFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/17
//
/**
 * Defines the base class for maneuver reference frames. 
 */
//------------------------------------------------------------------------------


#ifndef ManeuverFrame_hpp
#define ManeuverFrame_hpp


#include <math.h>
#include "gmatdefs.hpp"
#include "BurnException.hpp"


class GMAT_API ManeuverFrame
{
public:
	ManeuverFrame();
	virtual ~ManeuverFrame();
   ManeuverFrame(const ManeuverFrame& mf);
   ManeuverFrame&      operator=(const ManeuverFrame& mf);

   void                SetState(Real *pos, Real *vel = NULL);
   void                CalculateBasis(Real basis[3][3]);
   virtual std::string GetFrameLabel(Integer id);

protected:
   /// Matrix of the basis vectors -- Internal buffer used for efficiency
   Real                basisMatrix[3][3];

   /// Central body for the frame
   std::string         centralBody;
   /// Reference body for the frame
   std::string         referenceBody;
   /// Position vector used to calculate the basis
   Real                *position;
   /// Velocity vector used to calculate the basis
   Real                *velocity;
    
   /**
    * Calculates the principal directions for the maneuver frame.
    *
    * This method calculates the principal directions and stores them in the
    * basisMatrix member.  Derived classes need to implement this method, along
    * with the default methods.
    */
   virtual void        CalculateBasis() = 0;
};


#endif // ManeuverFrame_hpp
