//$Header$
//------------------------------------------------------------------------------
//                                  TrueOfDateAxes
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/05/03
//
/**
 * Definition of the TrueOfDateAxes class.  This is the base class for those
 * DynamicAxes classes that implement True Of Date axis systems.
 *
 */
//------------------------------------------------------------------------------

#ifndef TrueOfDateAxes_hpp
#define TrueOfDateAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "DynamicAxes.hpp"
#include "A1Mjd.hpp"

class GMAT_API TrueOfDateAxes : public DynamicAxes
{
public:

   // default constructor
   TrueOfDateAxes(const std::string &itsType,
                  const std::string &itsName = "");
   // copy constructor
   TrueOfDateAxes(const TrueOfDateAxes &tod);
   // operator = for assignment
   const TrueOfDateAxes& operator=(const TrueOfDateAxes &tod);
   // destructor
   virtual ~TrueOfDateAxes();
   
   // initializes the TrueOfDateAxes
   virtual bool Initialize(); 
protected:

   enum
   {
      TrueOfDateAxesParamCount = DynamicAxesParamCount
   };
    

};
#endif // TrueOfDateAxes_hpp
