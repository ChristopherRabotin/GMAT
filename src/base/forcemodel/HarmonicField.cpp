//$Header$
//------------------------------------------------------------------------------
//                              HarmonicField
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: D. Conway
// Created: 2003/03/14
// Modified: 2004/04/15 W. Shoan GSFC Code 583
//           Updated for GMAT style, standards; changed quad-t to Real, int
//           to Integer, add use of GMAT util classes, etc.
///
/**
 * This is the HarmonicField class.
 *
 * @note original prolog information included at end of file prolog.
 */
//
// *** File Name : shfieldmodel.cpp  -> HarmonicField.cpp (wcs 2004.04.14)
// *** Created   : March 14, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 572)                  ***
// ***  Under Contract:  P.O.  GSFC S-67521-G                             ***
// ***                                                                    ***
// ***  Copyright U.S. Government 2002                                    ***
// ***  Copyright United States Government as represented by the          ***
// ***  Administrator of the National Aeronautics and Space               ***
// ***  Administration                                                    ***
// ***                                                                    ***
// ***  This software is subject to the Sofware Usage Agreement described ***
// ***  by NASA Case Number GSC-14735-1.  The Softare Usage Agreement     ***
// ***  must be included in any distribution.  Removal of this header is  ***
// ***  strictly prohibited.                                              ***
// ***                                                                    ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C/C++ Source
// Development Environment   : Borland C++ 5.02
// Modification History      : 3/14/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
// **************************************************************************

#include "HarmonicField.hpp"

#include "PhysicalModel.hpp"
#include "ForceModelException.hpp"
#include "RealUtilities.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "MessageInterface.hpp"

using namespace GmatMathUtil;


//---------------------------------
// static data
//---------------------------------
const std::string
HarmonicField::PARAMETER_TEXT[HarmonicFieldParamCount - PhysicalModelParamCount] =
{
   "MaxDegree",
   "MaxOrder",
   "Degree",
   "Order",
   "Filename",
};

const Gmat::ParameterType
HarmonicField::PARAMETER_TYPE[HarmonicFieldParamCount - PhysicalModelParamCount] =
{
Gmat::INTEGER_TYPE,
Gmat::INTEGER_TYPE,
Gmat::INTEGER_TYPE,
Gmat::INTEGER_TYPE,
Gmat::STRING_TYPE,
};


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  HarmonicField(const std::string &name, Integer deg, Integer ord)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the HarmonicField class
 * (default constructor).
 *
 * @param <name> parameter indicating the name of the object.
 * @param <deg>  maximum degree of he polynomials.
 * @param <ord>  maximum order of he polynomials.
 */
//------------------------------------------------------------------------------
HarmonicField::HarmonicField(const std::string &name, const std::string &typeName,
                             Integer maxDeg, Integer maxOrd) :
PhysicalModel           (Gmat::PHYSICAL_MODEL, typeName, name),
hMinitialized           (false),
maxDegree               (maxDeg),
maxOrder                (maxOrd),
degree                  (4),
order                   (4),
Abar                    (NULL),
re                      (NULL),
im                      (NULL),
filename                (""),
fileRead                (false)
//epoch                   (21545.0)
{
   parameterCount = HarmonicFieldParamCount;
   r = s = t = u = 0.0;
}


//------------------------------------------------------------------------------
//  ~HarmonicField()
//------------------------------------------------------------------------------
/**
 * This method destroys the HarmonicField object.
 * (destructor).
 *
 */
//------------------------------------------------------------------------------
HarmonicField::~HarmonicField(void)
{
   if (Abar)
   {
      for (int i = 0; i <= maxDegree+1; ++i)
         delete [] Abar[i];

      delete [] Abar;
   }

   if (re)
      delete [] re;

   if (im)
      delete [] im;
}


//------------------------------------------------------------------------------
//  HarmonicField(const HarmonicField& hf)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the HarmonicField class with the state equal
 * to the input object (copy constructor).
 *
 * @param <hf> the HarmonicField object to copy.
 */
//------------------------------------------------------------------------------
HarmonicField::HarmonicField(const HarmonicField& hf) :
PhysicalModel           (hf),
hMinitialized           (false),
maxDegree               (hf.maxDegree),
maxOrder                (hf.maxOrder),
degree                  (hf.degree),
order                   (hf.order),
Abar                    (NULL),
r                       (0.0),
s                       (0.0),
t                       (0.0),
u                       (0.0),
re                      (NULL),
im                      (NULL),
filename                (hf.filename),
fileRead                (false)
{
}


//------------------------------------------------------------------------------
//  operator=(const HarmonicField& hf)
//------------------------------------------------------------------------------
/**
 * This method sets the state of the input HarmonicField class to 'this' object.
 *
 * @param <hf> the HarmonicField object whose values to use.
 */
//------------------------------------------------------------------------------
HarmonicField& HarmonicField::operator=(const HarmonicField& hf)
{
   if (&hf == this)
      return *this;

   PhysicalModel::operator=(hf);
   hMinitialized  = false;  // or hf.hMinitialized?
   maxDegree      = hf.maxDegree;
   maxOrder       = hf.maxOrder;
   degree         = hf.degree;
   order          = hf.order;
   Abar           = NULL;    // or hf.Abar?
   r              = 0.0;     // or hf.r?
   s              = 0.0;     // or hf.s?
   t              = 0.0;     // or hf.t?
   u              = 0.0;     // or hf.u?
   re             = NULL;    // or hf.re?
   im             = NULL;    // or hf.im?
   filename       = hf.filename;
   fileRead       = false;

   return *this;
}

//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes this HarmonicField object.
 *
 * @return flag indicating success of initialization.
 */
//------------------------------------------------------------------------------
bool HarmonicField::Initialize(void)
{
    if (!PhysicalModel::Initialize())
        return false;
   // initialize the body
   if (solarSystem == NULL) throw ForceModelException(
                            "Solar System undefined for Harmonic Field.");
   //body = solarSystem->GetBody(bodyName);
   return legendreP_init();
}


//------------------------------------------------------------------------------
//  bool SetDegreeOrder(Integer deg, Integer ord)
//------------------------------------------------------------------------------
/**
 * This method sets the degree and order for this HarmonicField object.
 *
 * @param <deg> degree to use.
 * @param <ord> order to use.
 *
 * @return flag indicating success of the operation.
 */
//------------------------------------------------------------------------------
bool HarmonicField::SetDegreeOrder(Integer deg, Integer ord)
{
    bool retval = true;

    if (deg <= maxDegree)
        degree = deg;
    else
    {
        degree = maxDegree;
        retval = false;
        MessageInterface::ShowMessage(
          "In HarmonicField, Potential Degree exceeds maximum degree in model");
    }

    if ((ord <= deg) && (ord <= maxOrder))
        order = ord;
    else
    {
        order = (deg < maxOrder ? deg : maxOrder );
        retval = false;
        MessageInterface::ShowMessage(
          "In HarmonicField, Potential Order exceeds valid range in model");
    }

    return retval;
}


//------------------------------------------------------------------------------
//  bool SetFilename(const std::string &fn)
//------------------------------------------------------------------------------
/**
 * This method sets the filename for this HarmonicField object.
 *
 * @param <fn> file name to use to get coefficients, etc.
 *
 * @return flag indicating success of the operation.
 */
//------------------------------------------------------------------------------
bool HarmonicField::SetFilename(const std::string &fn)
{
   if (filename != fn)
   {
      fileRead = false;
      filename = fn;
   }
   return true;
}

/** \brief Calculate normalized Legendre Polynomials

  The normalized "Derived" associated Legendre Polynomials of the 1st kind are
  calculated using the algorithm in Lundberg and Schutz:

    Lundberg, J.B., and Schutz, B.E., "Recursion Formulas of Legendre
    Functions for Use with Nonsingular Geopotential Models", Journal
    of Guidance, Dynamics, and Control, Vol. 11, No.1, Jan.-Feb. 1988.

  Code was derived from the implementation coded by Steve Queen.  This version
  takes his C version, and encapsulates it in C++ classes.  Steve can be
  contacted at

    Steven Queen
    Goddard Space Flight Center
    Flight Dynamics Analysis Branch
    Steven.Z.Queen@nasa.gov
 */
//------------------------------------------------------------------------------
//  bool legendreP_init()
//------------------------------------------------------------------------------
/**
 * This method initializes the Legendre polynomials for this HarmonicField
 * object.
 *
 * @return flag indicating success of the operation.
 */
//------------------------------------------------------------------------------
bool HarmonicField::legendreP_init(void)
{
   Integer  n;
   Integer  cc;

   Abar = new Real*[maxDegree+3];
   if ( !Abar ) {
      throw ForceModelException("legendreP_init: memory allocation failed for Abar!");
   }
   for (cc = 0; cc <= maxDegree+1; ++cc) {
      if ( maxOrder >= cc )
         Abar[cc] = new Real[cc+3];
      else
         Abar[cc] = new Real[maxOrder+3];

      if ( !Abar[cc] ) {
         throw ForceModelException("legendreP_init:  calloc failed!\a\n!");
      }
   }

   /* initialize the diagonal elements (not a function of the input) */
   Abar[0][0] = (Real)1.0;
   for (n = 1;n <= maxDegree+1; ++n){
      Abar[n][n] = Sqrt((Real)(2*n+1)/(Real)(2*n)) * Abar[n-1][n-1];
   }
   
   /* additional shared quantities (real & imaginary coordinates) */
   re = new Real[maxOrder+3];
   im = new Real[maxOrder+3];
   if ( !re || !im ) {
      throw ForceModelException("legendreP_init:  calloc failed!\a\n!");
   }

   /* initalize recursion, Ref.[1] */
   re[0] = (Real)1.0;  /* real part of (s + i*t)^m       */
   im[0] = (Real)0.0;  /* imaginary part of (s + i*t)^m  */

   hMinitialized = true;
   return true;
}


//---------------------------------
// inherited methods from GmatBase
//---------------------------------
//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string HarmonicField::GetParameterText(const Integer id) const
{
   if ((id >= PhysicalModelParamCount) && (id < HarmonicFieldParamCount))
      return PARAMETER_TEXT[id - PhysicalModelParamCount];
   return PhysicalModel::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Integer HarmonicField::GetParameterID(const std::string &str) const
{
   for (Integer i = PhysicalModelParamCount; i < HarmonicFieldParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - PhysicalModelParamCount])
         return i;
   }
   return PhysicalModel::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType HarmonicField::GetParameterType(const Integer id) const
{
   if ((id >= PhysicalModelParamCount) && (id < HarmonicFieldParamCount))
      return PARAMETER_TYPE[id - PhysicalModelParamCount];
   return PhysicalModel::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string HarmonicField::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];   
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param <id>    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
Integer HarmonicField::GetIntegerParameter(const Integer id) const
{
   if (id == MAX_DEGREE)  return maxDegree;
   if (id == MAX_ORDER)   return maxOrder;
   if (id == DEGREE)      return degree;
   if (id == ORDER)       return order;

   return PhysicalModel::GetIntegerParameter(id);
}

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    <id>    Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------
Integer HarmonicField::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == MAX_DEGREE)  return (maxDegree = value);
   if (id == MAX_ORDER)   return (maxOrder  = value);
   if (id == DEGREE)      return (degree    = value);
   if (id == ORDER)       return (order     = value);

   return PhysicalModel::SetIntegerParameter(id, value);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label>  parameter label
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
Integer HarmonicField::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label> Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------
Integer HarmonicField::SetIntegerParameter(const std::string &label,
                                           const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <id>  Integer ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string HarmonicField::GetStringParameter(const Integer id) const
{
   if (id == FILENAME) return filename;
   return PhysicalModel::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <id>    Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------
bool HarmonicField::SetStringParameter(const Integer id,
                                       const std::string &value)
{
//<<<<<<< HarmonicField.cpp
//   if (id == BODY)
//   {
//// DJC: Channges that allow setting without a solar system instance
////      if (!solarSystem) throw ForceModelException(
////          "In HarmonicField, cannot set body, as no solar system has been set");
//      if (value != bodyName)
//      {
////         body = solarSystem->GetBody(value);
////         if (body)
////         {
////            bodyName = body->GetName();
////            return true;
////         }
////         else      return false;
//         bodyName = value;
//         if (filename == "")
//         {
//            fileRead = false;
//            filename = value;
//            filename += ".grv";
//         }
//      }
////      else return true;
//      return true;
//   }
//=======
//>>>>>>> 1.6
   if (id == FILENAME)
   {
      if (filename != value)
      {
         fileRead = false;
         filename = value;
      }
      return true;
   }
   return PhysicalModel::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label>  Integer ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
std::string HarmonicField::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to get a parameter value
 *
 * @param    <label> Integer ID for the parameter
 * @param    <value> The new value for the parameter
 */
//------------------------------------------------------------------------------
bool HarmonicField::SetStringParameter(const std::string &label,
                                       const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//---------------------------------
// protected methods
//---------------------------------


/**
 * Normalized Derived Associated Lengendre Polynomials (of the 1st kind)
 * per the method of Ref.[1]. "Fully" normalized for compatibility with the
 * EGM96 coefficients per Ref[2]
 *
 * REFERENCES:
 * [1]   Lundberg, J.B., and Schutz, B.E., "Recursion Formulas of Lengendre
 *       Functions for Use with Nonsingular Geopotential Models", Journal
 *       of Guidance, Dynamics, and Control, Vol. 11, No.1, Jan.-Feb. 1988.
 *
 * [2]   Heiskanen, W.A., and Moritz, H., "Physical Geodesy", W.H. Freeman
 *       and Company, San Francisco, 1967.
 *
 * [3]   Pines, S., "Uniform Representation of the Gravitational Potential
 *       and its Derivatives", AIAA Journal, Vol. 11, No. 11, 1973.
 *
 * Steven Queen
 * Goddard Space Flight Center
 * Flight Dynamics Analysis Branch
 * Steven.Z.Queen@nasa.gov
 * February 28, 2003
 */
//------------------------------------------------------------------------------
//  bool legendreP_rtq(Rvector& R )
//------------------------------------------------------------------------------
/**
 * This method calculates the Legendre polynomials for this HarmonicField
 * object.
 *
 * @param <R> ????
 *
 * @return flag indicating success of the operation.
 */
//------------------------------------------------------------------------------
bool HarmonicField::legendreP_rtq(Real *R )
{

   if (!hMinitialized)
      if (!Initialize())
      {
         return false;
     }
 
   Integer  n, m;
   Real     sqrt3 = Sqrt(3.0);

   /* coordinate transformation, Ref.[3], Eqs.(7),(40) */
   r = Sqrt( R[0]*R[0] + R[1]*R[1] + R[2]*R[2] );
   if (r == 0.0) {
      throw ForceModelException (
                   "In HarmonicField::legendreP_rtq,  Radial distance is zero");
     // return false;
   }

   s = R[0]/r;
   t = R[1]/r;
   u = R[2]/r; /* sin(phi), phi = geocentric latitude */

   /* generate the off-diagonal elements */
   Abar[1][0] = u * sqrt3;
   for (n = 1; n <= degree; ++n) {
      Abar[n+1][n] = u*Sqrt( (Real)(2*n+3) ) * Abar[n][n];
   }
   
   /* apply column-fill recursion formula (Table 2, Row I, Ref.[1]) */
   for (m = 0 ; m <= order+1; m++) {
      for (n = m+2; n <= degree+1; n++) {
         Abar[n][m] = u*Sqrt((Real)((2*n+1)*(2*n-1))/(Real)((n-m)*(n+m)))*Abar[n-1][m]
         - Sqrt((Real)((2*n+1)*(n-m-1)*(n+m-1))/(Real)((2*n-3)*(n+m)*(n-m)))*Abar[n-2][m];
      }
      if ( m > 0 ) {
         /* Ref.[2], Eq.(24) */
         re[m] = s*re[m-1] - t*im[m-1]; /* real part of (s + i*t)^m */
         im[m] = s*im[m-1] + t*re[m-1]; /* imaginary part of (s + i*t)^m */
      }
   }

   return true;
}

