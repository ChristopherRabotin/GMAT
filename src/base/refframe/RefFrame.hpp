//$Header$
//------------------------------------------------------------------------------
//                              RefFrame
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/08/25
//
/**
 * Declares base class of Reference Frame.
 */
//------------------------------------------------------------------------------
#ifndef RefFrame_hpp
#define RefFrame_hpp

#include "gmatdefs.hpp"
#include "CelestialBody.hpp"
#include "A1Mjd.hpp"

class GMAT_API RefFrame
{
public:

   CelestialBody* GetCentralBody() const;
   std::string GetCentralBodyName() const;
   A1Mjd GetRefDate() const;
   std::string GetFrameName() const;

   bool operator== (const RefFrame &right) const;
   bool operator!= (const RefFrame &right) const;
 
protected:
   RefFrame(const CelestialBody &centralBody, const A1Mjd &refDate,
            const std::string &frameName);
   RefFrame();
   ~RefFrame();

private:

   CelestialBody* mCentralBody;
   A1Mjd mRefDate;
   std::string mFrameName;
};

#endif // RefFrame_hpp
