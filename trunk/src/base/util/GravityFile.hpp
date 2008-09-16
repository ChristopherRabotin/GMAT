//$Header$
//------------------------------------------------------------------------------
//                                  GravityFile
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc.
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2007/08/15
//
/**
 * Definition of the GravityFile class.  This is the code that reads the different
 * types of gravity files, such as .cof, .dat, .grv files.
 *
 */
//------------------------------------------------------------------------------
#ifndef GravityFile_hpp
#define GravityFile_hpp

#include "gmatdefs.hpp"

namespace GmatFM
{
   enum GravityFileType
   {
      GFT_COF = 1,
      GFT_DAT,
      GFT_GRV,
      GFT_UNKNOWN = -1
   };
}


class GMAT_API GravityFile
{
public:

   /// dimension normalized harmonic coefficients
   static const Integer DEG_DIM = 361;
   /// dimension of coefficient drifts per year
   static const Integer DRF_DIM = 17;
   
   GravityFile();
   GravityFile(const GravityFile &copy);
   const GravityFile& operator=(const GravityFile &right);
   virtual ~GravityFile();
   
   GmatFM::GravityFileType GetFileType(const std::string &filename);
   bool GetFileInfo(const std::string &filename, Integer& degree, Integer& order,
                    Real &mu, Real &radius);
   bool ReadFile(const std::string &filename, Integer& degree, Integer& order,
                 Real &mu, Real &radius, bool readCoeff,
                 Real cbar[][DEG_DIM] = NULL, Real sbar[][DEG_DIM] = NULL,
                 Real dcbar[][DRF_DIM] = NULL, Real dsbar[][DRF_DIM] = NULL,
                 Integer maxDegree = 360, Integer maxOrder = 360,
                 Integer maxDriftDegree = 2);
   
protected:

   bool ReadCofFile(const std::string &filename, Integer& degree, Integer& order,
                    Real &mu, Real &radius, bool readCoeff, Real cbar[][DEG_DIM],
                    Real sbar[][DEG_DIM], Integer maxDegree, Integer maxOrder,
                    Integer maxDriftDegree = 0);
   bool ReadDatFile(const std::string &filename, Integer& degree, Integer& order,
                    Real &mu, Real &radius, bool readCoeff, Real cbar[][DEG_DIM],
                    Real sbar[][DEG_DIM], Real dcbar[][DRF_DIM], Real dsbar[][DRF_DIM],
                    Integer maxDegree, Integer maxOrder, Integer maxDriftDegree);
   bool ReadGrvFile(const std::string &filename, Integer& degree, Integer& order,
                    Real &mu, Real &radius, bool readCoeff, Real cbar[][DEG_DIM],
                    Real sbar[][DEG_DIM], Integer maxDegree, Integer maxOrder,
                    Integer maxDriftDegree = 0);

};

#endif
