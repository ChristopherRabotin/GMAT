//$Header$
//------------------------------------------------------------------------------
//                                  EopFile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/01/26
//
/**
 * Definition of the EopFile class.  This is the code that reads the polar
 * motion information, as well as the UT1-UTC offset, from EOP file.
 *
 */
//------------------------------------------------------------------------------

#ifndef EopFile_hpp
#define EopFile_hpp

#include "gmatdefs.hpp"
#include "Rmatrix.hpp"

namespace GmatEop
{
   enum EopFileType
   {
      EOP_C04,
      FINALS
   };
};

class GMAT_API EopFile
{
public:
   // default constructor
   EopFile(const std::string &fileName = "eopc04.62-now", 
           GmatEop::EopFileType eop = GmatEop::EOP_C04);
   // copy constructor
   EopFile(const EopFile &eopF);
   // operator = 
   const EopFile& operator=(const EopFile &eopF);
   // destructor
   virtual ~EopFile();
   
   
   // initializes the EopFile (reads it and stores the data)
   virtual void Initialize();
   
   // method to return the UT1-UTC offset for the given UTCMjd - use UtcMjd???
   virtual Real    GetUt1UtcOffset(const Real utcMjd);
   
   // method to return JD, X, Y, LOD data (for use by coordinate systems)
   virtual Rmatrix GetPolarMotionData();
   // interpolate x, y, and lod to input time
   virtual bool    GetPolarMotionAndLod(Real forUtcMjd, Real &xval, Real  &yval,
                                        Real &lodval);
  
protected:

   static const Integer MAX_TABLE_SIZE = 50405;  // up to year >= 2100

   GmatEop::EopFileType eopFType;
   std::string          eopFileName;
   Integer              tableSz;

   /// table of polar motion data : MJD, X, Y, LOD
   Rmatrix*             polarMotion;
   /// vector of UT1-UTC offsets : MJD, offset
   Rmatrix*             ut1UtcOffsets;
   
   bool isInitialized;
   
   bool IsBlank(const char* aLine);

};
#endif // EopFile_hpp
