//$Id$
//------------------------------------------------------------------------------
//                                  Barycenter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Wendy C. Shoan
// Created: 2005.04.05
//
/**
 * Definition of the Barycenter class.
 *
 */
//------------------------------------------------------------------------------

#ifndef Barycenter_hpp
#define Barycenter_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CalculatedPoint.hpp"
#include "A1Mjd.hpp"
#include "Rmatrix.hpp"
#include "Rvector6.hpp"
#include "TimeTypes.hpp"

/**
 * Barycenter class, derived from CalculatedPoint class.
 *
 * @note Bodies are sent to an (sub)object of this class via the SetRefObject
 *       method.
 */
class GMAT_API Barycenter : public CalculatedPoint
{
public:
   // default constructor, specifying calculated point type and name
   Barycenter(const std::string &itsName = "");
   // copy constructor
   Barycenter(const Barycenter &bary);
   // operator=
   Barycenter& operator=(const Barycenter &bary);
   // destructor
   virtual ~Barycenter();
   
   virtual GmatBase*       Clone() const;
   virtual void            Copy(const GmatBase* orig);
   
   // methods inherited from SpacePoint, that must be implemented here
   virtual const Rvector6  GetMJ2000State(const A1Mjd &atTime);
   virtual const Rvector3  GetMJ2000Position(const A1Mjd &atTime);
   virtual const Rvector3  GetMJ2000Velocity(const A1Mjd &atTime);   
   
   virtual Real            GetMass();
   
   // Parameter access methods - overridden from GmatBase
   /*
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const; 
   virtual Gmat::ParameterType 
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   */
protected:

   enum
   {
      BarycenterParamCount = CalculatedPointParamCount
   };
   //static const std::string PARAMETER_TEXT[BarycenterParamCount - CalculatedPointParamCount];
   //static const Gmat::ParameterType PARAMETER_TYPE[BarycenterParamCount - CalculatedPointParamCount];

    
private:
      // check that all bodies in the list are CelestialBody objects
      virtual void CheckBodies();
};
#endif // Barycenter_hpp

