//$Header$
//------------------------------------------------------------------------------
//                                  CoordinateConverter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G and MOMS Task order 124
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2004/12/27
//
/**
 * Definition of the CoordinateConverter class.
 *
 */
//------------------------------------------------------------------------------

#ifndef CoordinateConverter_hpp
#define CoordinateConverter_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "SpacePoint.hpp"
#include "A1Mjd.hpp"
#include "CoordinateSystem.hpp"
#include "Rvector.hpp"

class GMAT_API CoordinateConverter
{
public:

   // default constructor
   CoordinateConverter();
   // copy constructor
   CoordinateConverter(const CoordinateConverter &coordCvt);
   // operator = 
   const CoordinateConverter& operator=(const CoordinateConverter &coordCvt);
   // destructor
   virtual ~CoordinateConverter();
   
   // initilaizes the CoordinateConverter
   virtual void Initialize(); 
   
   void        SetJ2000BodyName(const std::string &toName);
   std::string GetJ2000BodyName() const;
   void        SetJ2000Body(SpacePoint *toBody);
   SpacePoint* GetJ2000Body();
   
   bool AddCoordinateSystem(CoordinateSystem *coordSys);
   bool Convert(const A1Mjd &epoch, const Rvector &inState,
                CoordinateSystem *inCoord, Rvector &outState,
                CoordinateSystem *outCoord);
   
protected:
   
   /// Origin for the return coordinate system (aligned with the MJ2000 Earth
   /// Equatorial coordinate system)
   SpacePoint                     *j2000Body;  
   /// Name for the J2000 body
   std::string                    j2000BodyName;
   /// store pointers to all defined coordinate systems - is this necessary???
   std::vector<CoordinateSystem*> coordSystems;
   
   /// internal (intermediate) state
   Rvector                        internalState;
};
#endif // CoordinateConverter_hpp
