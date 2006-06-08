//$Header$
//------------------------------------------------------------------------------
//                              VnbManeuverFrame 
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
 * Defines the class used for maneuvers defined in VNB coordinates. 
 */
//------------------------------------------------------------------------------


#ifndef VnbManeuverFrame_hpp
#define VnbManeuverFrame_hpp

#include "ManeuverFrame.hpp"


class GMAT_API VnbManeuverFrame : public ManeuverFrame
{
public:
   VnbManeuverFrame();
   virtual ~VnbManeuverFrame();
   VnbManeuverFrame(const VnbManeuverFrame& imf);
   VnbManeuverFrame&   operator=(const VnbManeuverFrame& imf);

   virtual std::string GetFrameLabel(Integer id);

protected:
   void                CalculateBasis();
};

#endif // VnbManeuverFrame_hpp
