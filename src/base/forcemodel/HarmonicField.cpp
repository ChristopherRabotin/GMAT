//$Id$
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
#include "StringUtil.hpp"     // for ToString()
#include "RealUtilities.hpp"
#include "Rvector.hpp"
#include "Rmatrix.hpp"
#include "FileManager.hpp"
#include "MessageInterface.hpp"

/*
#include <iostream>
 using namespace std; // ***************************** for debug only
 
#define DEBUG_REFERENCE_SETTING
*/

using namespace GmatMathUtil;

//#define DEBUG_HARMONIC_FIELD
//#define DEBUG_HARMONIC_FIELD_FILENAME

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
   "PotentialFile",
   "InputCoordinateSystem",
   "FixedCoordinateSystem",
   "TargetCoordinateSystem",
};

const Gmat::ParameterType
HarmonicField::PARAMETER_TYPE[HarmonicFieldParamCount - PhysicalModelParamCount] =
{
   Gmat::INTEGER_TYPE,   // "MaxDegree",
   Gmat::INTEGER_TYPE,   // "MaxOrder",
   Gmat::INTEGER_TYPE,   // "Degree",
   Gmat::INTEGER_TYPE,   // "Order",
   Gmat::STRING_TYPE,    // "PotentialFile",
   Gmat::STRING_TYPE,    // "InputCoordinateSystem",
   Gmat::STRING_TYPE,    // "FixedCoordinateSystem",
   Gmat::STRING_TYPE,    // "TargetCoordinateSystem",
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
fileRead                (false),
inputCSName             ("EarthMJ2000Eq"),
fixedCSName             ("EarthFixed"),
targetCSName            ("EarthMJ2000Eq"),
potPath                 (""),
offDiags                (NULL),
abarCoeff1              (NULL),
abarCoeff2              (NULL)
//epoch                   (21545.0)
{
   objectTypeNames.push_back("HarmonicField");
   parameterCount = HarmonicFieldParamCount;
   r = s = t = u = 0.0;
   
   FileManager *fm = FileManager::Instance();
   potPath = fm->GetAbsPathname(bodyName + "_POT_PATH");
   
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

   if (offDiags)
      delete [] offDiags;

   if (abarCoeff1)
   {
      for (Integer i = 0; i <= degree+1; ++i)
         delete [] abarCoeff1[i];
      
      delete [] abarCoeff1;
   }

   if (abarCoeff2)
   {
      for (Integer i = 0; i <= degree+1; ++i)
         delete [] abarCoeff2[i];
      
      delete [] abarCoeff2;
   }
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
fileRead                (false),
inputCSName             (hf.inputCSName),
fixedCSName             (hf.fixedCSName),
targetCSName            (hf.targetCSName),
potPath                 (hf.potPath),
offDiags                (NULL),
abarCoeff1              (NULL),
abarCoeff2              (NULL)
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
   inputCSName    = hf.inputCSName;
   fixedCSName    = hf.fixedCSName;
   targetCSName   = hf.targetCSName;
   potPath        = hf.potPath;
   offDiags       = NULL;
   abarCoeff1     = NULL;
   abarCoeff2     = NULL;

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
bool HarmonicField::Initialize()
{
   if (!PhysicalModel::Initialize())
      return false;

   // Set body to use the same potential file (loj: 3/24/06)
   // if we want to use mu and radius from this file later
   //body->SetPotentialFilename(filename);
   
   if (solarSystem == NULL) throw ForceModelException(
                            "Solar System undefined for Harmonic Field " 
                             + instanceName);
   if (!inputCS) throw ForceModelException(
                 "Input coordinate system undefined for Harmonic Field "
                  + instanceName);
   if (!fixedCS) throw ForceModelException(
                 "Body fixed coordinate system undefined for Harmonic Field "
                  + instanceName);
   //if (!targetCS) throw ForceModelException(
   //               "Target coordinate system undefined for Harmonic Field "
   //               + instanceName);
   if (!targetCS) targetCS = inputCS;

   // initialize the body
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
   #ifdef DEBUG_HARMONIC_FIELD_FILENAME
   MessageInterface::ShowMessage
      ("HarmonicField::SetFilename() for %s\n   filename = %s\n   newname  = %s\n",
       bodyName.c_str(), filename.c_str(), fn.c_str());
   MessageInterface::ShowMessage("   potPath  = %s\n", potPath.c_str());
   #endif
   
   if (filename != fn)
   {
      // Add default pathname if none specified
      if (fn.find("/") == fn.npos && fn.find("\\") == fn.npos)
      {
         try
         {
            filename = potPath + fn;
         }
         catch (GmatBaseException &e)
         {
            filename = fn;
            MessageInterface::ShowMessage(e.GetFullMessage());
         }
      }
      else
      {
         filename = fn;
      }
      
      std::ifstream potfile(filename.c_str());
      if (!potfile) 
      {
         throw ForceModelException
            ("The file name \"" + filename + "\" does not exist.");
      }
      
      if (body != NULL)
         body->SetPotentialFilename(filename);
   }
   
   #ifdef DEBUG_HARMONIC_FIELD_FILENAME
   MessageInterface::ShowMessage("   filename = %s\n", filename.c_str());
   #endif
   
   fileRead = false;
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
bool HarmonicField::legendreP_init()
{
   Integer  m, n;
   Integer  cc;
   Integer  i;

   // if re-initializing, delete the old arrays first
   if (Abar)
   {
      for (i = 0; i <= maxDegree+1; ++i)
         delete [] Abar[i];
      
      delete [] Abar;
   }
   
   if (re)
      delete [] re;
   
   if (im)
      delete [] im;

   if (offDiags)
      delete [] offDiags;
   if (abarCoeff1)
   {
      for (i = 0; i <= degree+1; ++i)
         delete [] abarCoeff1[i];
      
      delete [] abarCoeff1;
   }
   if (abarCoeff2)
   {
      for (i = 0; i <= degree+1; ++i)
         delete [] abarCoeff2[i];
      
      delete [] abarCoeff2;
   }
   
   Abar = new Real*[maxDegree+3];
   if ( !Abar ) {
      throw ForceModelException("legendreP_init: memory allocation failed for Abar!");
   }

   Integer allocsize = 0;
   
   for (cc = 0; cc <= maxDegree+1; ++cc)
   {
      if ( maxOrder >= cc )
         allocsize = cc + 3;
      else
         allocsize = maxOrder + 3;

      Abar[cc] = new Real[allocsize];
      
      if ( !Abar[cc] )
      {
         throw ForceModelException("legendreP_init:  calloc failed!\a\n!");
      }
      else
      {
         for (int jj=0; jj<allocsize; jj++)
            Abar[cc][jj] = 0.0;
      }
   }

   /* initialize the diagonal elements (not a function of the input) */
   Abar[0][0] = (Real)1.0;
   for (n = 1;n <= maxDegree+1; ++n)
   {
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
   
   offDiags = new Real[degree+2];
   abarCoeff1 = new Real*[degree+2];
   abarCoeff2 = new Real*[degree+2];
   
   for (n = 1; n <= degree+1; ++n) 
   {
      offDiags[n] = sqrt((Real)(2*n+3));
      abarCoeff1[n] = new Real[degree+2];
      abarCoeff2[n] = new Real[degree+2];
   }
   offDiags[0] = 0.0; 
   abarCoeff1[0] = new Real[degree+2];
   abarCoeff2[0] = new Real[degree+2];
   
   Real nmnm, n2;
   /* apply column-fill recursion formula (Table 2, Row I, Ref.[1]) */
   for (m = 0 ; m <= order+1; m++) 
   {
      for (n = m+2; n <= degree+1; n++) 
      {
         nmnm = (n-m)*(n+m);
         n2 = 2*n;
         abarCoeff1[n][m] = Sqrt((Real)((n2+1)*(n2-1))/(Real)(nmnm));
         abarCoeff2[n][m] = Sqrt((Real)((n2+1)*(n-m-1)*(n+m-1))/
                                 (Real)((n2-3)*nmnm));
      }
   }

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
   std::string useStr = str;
   if (useStr == "Model")
      useStr = "PotentialFile";
 
   for (Integer i = PhysicalModelParamCount; i < HarmonicFieldParamCount; i++)
   {
      if (useStr == PARAMETER_TEXT[i - PhysicalModelParamCount])
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
   if (id == DEGREE)
   {
      if (value >= 0)
         degree = value;
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ForceModelException(
            "The value of \"" + buffer.str() + "\" for field \"" 
            + GetParameterText(id).c_str() + "\" on object \"" 
            + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Integer >= 0 "
            "and < the maximum specified by the model, Order <= Degree].");
      }
      return degree;
   }
   if (id == ORDER)
   {
      if (value >= 0)
         order = value;
      else
      {
         std::stringstream buffer;
         buffer << value;
         throw ForceModelException(
            "The value of \"" + buffer.str() + "\" for field \"" 
            + GetParameterText(id).c_str() + "\" on object \"" 
            + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [Integer >= 0 "
            "and < the maximum specified by the model, Order <= Degree].");
      }
      return order;
   }

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
   if (id == FILENAME)
   {
      //return filename;
      std::string::size_type index = filename.find_last_of("/\\");
      
      // if path not found, just write filename
      if (index == filename.npos)
         return filename;
      else
      {
         // if actual pathname is the same as the default path, write only name part
         std::string actualPath = filename.substr(0, index+1);
         std::string fname = filename;
         if (potPath == actualPath)
            fname = filename.substr(index+1);
         
         return fname;
      }
   }
   
   if (id == INPUT_COORD_SYSTEM)  return inputCSName;
   if (id == FIXED_COORD_SYSTEM)  return fixedCSName;
   if (id == TARGET_COORD_SYSTEM) return targetCSName;
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
   if (id == FILENAME)
   {
      return SetFilename(value);  
   }
   if (id == INPUT_COORD_SYSTEM)
   {
      #ifdef DEBUG_HARMONIC_FIELD
         char str[1024];
         strcpy(str, value.c_str());
         
         MessageInterface::ShowMessage(
                "Setting input coordinate system name to \"%s\"\n", str);
      #endif
      inputCSName = value;
      
      return true;
   }
   if (id == FIXED_COORD_SYSTEM)
   {
      #ifdef DEBUG_HARMONIC_FIELD
         char str[1024];
         strcpy(str, value.c_str());
         
         MessageInterface::ShowMessage(
                "Setting fixed coordinate system name to \"%s\"\n", str);
      #endif
      fixedCSName = value;
      
      return true;
   }
   if (id == TARGET_COORD_SYSTEM)
   {
      #ifdef DEBUG_HARMONIC_FIELD
         char str[1024];
         strcpy(str, value.c_str());
         
         MessageInterface::ShowMessage(
                "Setting target coordinate system name to \"%s\"\n", str);
      #endif
      targetCSName = value;
      
      return true;
   }
   if (id == BODY_NAME)
   {      
      if (PhysicalModel::SetStringParameter(id, value))
      {
         // set default potential file path for the body
         FileManager *fm = FileManager::Instance();
         potPath = fm->GetAbsPathname(bodyName + "_POT_PATH");
         
         #ifdef DEBUG_HARMONIC_FIELD
         MessageInterface::ShowMessage
            ("Setting potential file path to \"%s\"\n", potPath.c_str());
         #endif
         
         fixedCSName = value + "Fixed";
         return true;
      }
      return false;
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

//------------------------------------------------------------------------------
//  GmatBase* GetRefObject(const Gmat::ObjectType type,
//                         const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method returns a reference object from the HarmonicField class.
 *
 * @param type  type of the reference object requested
 * @param name  name of the reference object requested
 *
 * @return pointer to the reference object requested.
 *
 */
//------------------------------------------------------------------------------
GmatBase* HarmonicField::GetRefObject(const Gmat::ObjectType type,
                                      const std::string &name)
{
   switch (type)
   {
      case Gmat::COORDINATE_SYSTEM:
         if ((inputCS) && (name == inputCSName))       return inputCS;
         if ((fixedCS) && (name == fixedCSName))       return fixedCS;
         if ((targetCS) && (name == targetCSName))     return targetCS;
         break;
      default:
            break;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return PhysicalModel::GetRefObject(type, name);
}


//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object. (Derived classes should implement
 * this as needed.)
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& HarmonicField::GetRefObjectNameArray(const Gmat::ObjectType type)
{
   static StringArray refs;
   
   // Start from an empty list
   refs.clear();
   
   if (type == Gmat::UNKNOWN_OBJECT)
   {
      refs = PhysicalModel::GetRefObjectNameArray(type);
      //refs.clear(); // why would I want to clear this????
      
      refs.push_back(inputCSName);
      refs.push_back(fixedCSName);
      refs.push_back(targetCSName);
      
      #ifdef DEBUG_REFERENCE_SETTING
         MessageInterface::ShowMessage("+++ReferenceObjects:\n");
         for (StringArray::iterator i = refs.begin(); i != refs.end(); ++i)
            MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif
      
      return refs;
   }
   
   if (type == Gmat::COORDINATE_SYSTEM)
   {
      refs = PhysicalModel::GetRefObjectNameArray(type);
      
      refs.push_back(inputCSName);
      refs.push_back(fixedCSName);
      refs.push_back(targetCSName);
      
      #ifdef DEBUG_REFERENCE_SETTING
         MessageInterface::ShowMessage("+++ReferenceObjects:\n");
         for (StringArray::iterator i = refs.begin(); i != refs.end(); ++i)
            MessageInterface::ShowMessage("   %s\n", i->c_str());
      #endif
      
      return refs;
   }
   
   // Not handled here -- invoke the next higher GetRefObject call
   return PhysicalModel::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the HarmonicField class.
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool HarmonicField::SetRefObject(GmatBase *obj,
                                 const Gmat::ObjectType type,
                                 const std::string &name)
{
   if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
   {
      if (name == inputCSName)
      {
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage("Setting %s as input CS for %s\n",
                                          name.c_str(), instanceName.c_str());
         #endif
         inputCS = (CoordinateSystem*) obj;
      }
      if (name == fixedCSName)
      {
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage("Setting %s as body fixed CS for %s\n",
                                          name.c_str(), instanceName.c_str());
         #endif
         fixedCS = (CoordinateSystem*) obj;
      }
      if (name == targetCSName)
      {
         #ifdef DEBUG_REFERENCE_SETTING
            MessageInterface::ShowMessage("Setting %s as target CS for %s\n",
                                          name.c_str(), instanceName.c_str());
         #endif
         targetCS = (CoordinateSystem*) obj;
      }
      return true;
   }
   
   // Not handled here -- invoke the next higher SetRefObject call
   return PhysicalModel::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets the force model origin, and derived the base coordinate 
 * system names from that SpacePoint.
 *
 * @param toBody  pointer to the force model origin.
 */
//------------------------------------------------------------------------------
void HarmonicField::SetForceOrigin(CelestialBody* toBody)
{
   PhysicalModel::SetForceOrigin(toBody);
   
   std::string originName = toBody->GetName();
   inputCSName = originName + "MJ2000Eq";
   targetCSName = inputCSName;
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
   static const Real sqrt3 = Sqrt(3.0);

   /* coordinate transformation, Ref.[3], Eqs.(7),(40) */
   r = sqrt( R[0]*R[0] + R[1]*R[1] + R[2]*R[2] );
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
   for (n = 1; n <= degree; ++n) 
   {
//      if (offDiags[n] != sqrt( (Real)(2*n+3) ))
//         MessageInterface::ShowMessage("%d %le ", n,
//                                       offDiags[n] - sqrt( (Real)(2*n+3) ));
      Abar[n+1][n] = u * offDiags[n] * Abar[n][n];
//      Abar[n+1][n] = u*sqrt( (Real)(2*n+3) ) * Abar[n][n];
   }
   
//   Real nmnm, n2;
   /* apply column-fill recursion formula (Table 2, Row I, Ref.[1]) */
   for (m = 0 ; m <= order+1; m++) 
   {
      for (n = m+2; n <= degree+1; n++) 
      {
         Abar[n][m] = u * abarCoeff1[n][m] * Abar[n-1][m] -
                      abarCoeff2[n][m] * Abar[n-2][m];
//         n2 = 2*n;
//         nmnm = (n-m)*(n+m);
//         Abar[n][m] = u * sqrt((Real)((n2+1)*(n2-1))/(Real)(nmnm)) * Abar[n-1][m] -
//            sqrt((Real)((n2+1)*(n-m-1)*(n+m-1))/(Real)((n2-3)*nmnm)) * Abar[n-2][m];
      }
      if ( m > 0 ) {
         /* Ref.[2], Eq.(24) */
         re[m] = s*re[m-1] - t*im[m-1]; /* real part of (s + i*t)^m */
         im[m] = s*im[m-1] + t*re[m-1]; /* imaginary part of (s + i*t)^m */
      }
   }

   return true;
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <id> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not,
 *         throws if the parameter is out of the valid range of values.
 */
//---------------------------------------------------------------------------
bool HarmonicField::IsParameterReadOnly(const Integer id) const
{
   if (id < PhysicalModelParamCount)
      return PhysicalModel::IsParameterReadOnly(id);
   
   if (id >= HarmonicFieldParamCount)
      throw ForceModelException(
         "Attempting to determine accessibility of a parameter outside of the "
         "scope of a HarmonicField object.");
   
   if ((id == DEGREE) || (id == ORDER) || (id == FILENAME))
      return false;
   
   return true;
}
