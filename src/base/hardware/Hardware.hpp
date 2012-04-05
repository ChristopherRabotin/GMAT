//$Id$
//------------------------------------------------------------------------------
//                               Hardware
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2004/11/08
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under MOMS Task
// Order 124.
//
/**
 * Class definition for the Hardware base class.
 */
//------------------------------------------------------------------------------


#ifndef HARDWARE_HPP
#define HARDWARE_HPP

#include "GmatBase.hpp"

/**
 * Base class used for spacecraft hardware.
 * 
 * This class is the base class for spacecraft fuel tanks, thrusters, and other
 * hardware elements that can be added to a spacecraft in GMAT.  It contains
 * data structures that locate the center of the element in the spacecraft's 
 * body coordinate system (BCS) and that orient the elements in the same system.
 * 
 * @note The current builds of GMAT do not model torques or moments of inertia, 
 * so the parameter access for those pieces is commented out.
 */
class GMAT_API Hardware : public GmatBase 
{
public:
   Hardware(Gmat::ObjectType typeId, const std::string &typeStr, 
            const std::string &nomme = "");
   virtual ~Hardware();
   Hardware(const Hardware& hw);
   Hardware&               operator=(const Hardware& hw);
   
   // Parameter access methods - overridden from GmatBase
   virtual std::string        GetParameterText(const Integer id) const;
   virtual Integer            GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                              GetParameterType(const Integer id) const;
   virtual std::string        GetParameterTypeString(const Integer id) const;
   
   
   virtual Real               GetRealParameter(const Integer id) const;
   virtual Real               SetRealParameter(const Integer id,
                                               const Real value);
   virtual Real               GetRealParameter(const std::string &label) const;
   virtual Real               SetRealParameter(const std::string &label,
                                         const Real value);

//   virtual bool	 VerifyRefObject(std::string subTypeName, GmatBase* obj); // made changes by Tuan Nguyen

   DEFAULT_TO_NO_CLONES

protected:
   /// Location of center of the hardware element on the spacecraft, in meters.
   Real                    location[3];
   /// Principle direction for hardware element on the spacecraft.
   Real                    direction[3];
   /// Secondary direction, to complete the orientation.
   Real                    secondDirection[3];

   /// Enumeration defining user accessible parameters for Hardware elements.
   enum
   {
      DIRECTION_X = GmatBaseParamCount,
      DIRECTION_Y,
      DIRECTION_Z,
// These will be needed when detailed attitude is added to GMAT:
//      SECOND_DIRECTION_X,
//      SECOND_DIRECTION_Y,
//      SECOND_DIRECTION_Z,
//      BCS_X,
//      BCS_Y,
//      BCS_Z,
      HardwareParamCount
   };
   
   /// Hardware Parameter labels
   static const std::string 
                        PARAMETER_TEXT[HardwareParamCount - GmatBaseParamCount];
   /// Hardware Parameter types
   static const Gmat::ParameterType 
                        PARAMETER_TYPE[HardwareParamCount - GmatBaseParamCount];
};

#endif // HARDWARE_HPP
