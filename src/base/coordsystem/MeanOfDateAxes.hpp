//$Header$
//------------------------------------------------------------------------------
//                                  MeanOfDateAxes
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
 * Definition of the MeanOfDateAxes class.  This is the base class for those
 * DynamicAxes classes that implement Mean Of Date axis systems.
 *
 */
//------------------------------------------------------------------------------

#ifndef MeanOfDateAxes_hpp
#define MeanOfDateAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "DynamicAxes.hpp"
#include "A1Mjd.hpp"

class GMAT_API MeanOfDateAxes : public DynamicAxes
{
public:

   // default constructor
   MeanOfDateAxes(const std::string &itsType,
                  const std::string &itsName = "");
   // copy constructor
   MeanOfDateAxes(const MeanOfDateAxes &mod);
   // operator = for assignment
   const MeanOfDateAxes& operator=(const MeanOfDateAxes &mod);
   // destructor
   virtual ~MeanOfDateAxes();
   
   // initializes the MeanOfDateAxes
   virtual bool Initialize(); 

   virtual GmatCoordinate::ParameterUsage UsesEopFile() const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;

protected:

   enum
   {
      MeanOfDateAxesParamCount = DynamicAxesParamCount
   };
    

};
#endif // MeanOfDateAxes_hpp
