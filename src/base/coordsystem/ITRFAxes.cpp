//$Id: ITRFAxes.cpp 9513 2012-02-29 21:23:06Z tuandangnguyen $
//------------------------------------------------------------------------------
//                                  ITRFAxes.cpp
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Tuan Dang Nguyen (NASA/GSFC)
// Created: 2012/02/29
//
/**
 * Implementation of the ITRFAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "ITRFAxes.hpp"
#include "DynamicAxes.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "DeFile.hpp"
#include "Planet.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "GmatConstants.hpp"
#include "Rvector3.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"
#include "MessageInterface.hpp"
#include "Attitude.hpp"
#include "FileManager.hpp"

#define PI 3.1415926535897

using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeConstants;      // for JD offsets, etc.

//#define DEBUG_FIRST_CALL
//#define DEBUG_TIME_CALC
//#define DEBUG_MOON_MATRIX
//#define DEBUG_BF_MATRICES
//#define DEBUG_BF_CHECK_DETERMINANT
//#define DEBUG_BF_ROT_MATRIX
//#define DEBUG_BF_RECOMPUTE
//#define DEBUG_BF_EPOCHS

#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
#endif


//---------------------------------
// static data
//---------------------------------

//const std::string
//ITRFAxes::PARAMETER_TEXT[ITRFAxesParamCount - DynamicAxesParamCount] =
//{
//};
//
//const Gmat::ParameterType
//ITRFAxes::PARAMETER_TYPE[ITRFAxesParamCount - DynamicAxesParamCount] =
//{
//};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  ITRFAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base ITRFAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 */
//------------------------------------------------------------------------------
ITRFAxes::ITRFAxes(const std::string &itsName) :
DynamicAxes("ITRF",itsName),
//de                       (NULL),
iauFile					 (NULL),
prevEpoch                (0.0),
prevUpdateInterval       (-99.9),
prevOriginUpdateInterval (-99.9),
prevLunaSrc              (Gmat::RotationDataSrcCount)
{
   objectTypeNames.push_back("ITRFAxes");
   parameterCount = ITRFAxesParamCount;

   baseSystem = "ICRF";
}


//------------------------------------------------------------------------------
//  ITRFAxes(const ITRFAxes &itrfAxes);
//------------------------------------------------------------------------------
/**
 * Constructs base ITRFAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param itrfAxes  ITRFAxes instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
ITRFAxes::ITRFAxes(const ITRFAxes &itrfAxes) :
DynamicAxes(itrfAxes),
//de                       (NULL),
iauFile					 (NULL),
prevEpoch                (itrfAxes.prevEpoch),
prevUpdateInterval       (itrfAxes.prevUpdateInterval),
prevOriginUpdateInterval (itrfAxes.prevOriginUpdateInterval),
prevLunaSrc              (itrfAxes.prevLunaSrc)
{
   #ifdef DEBUG_BF_RECOMPUTE
      MessageInterface::ShowMessage("Constructing a new BFA (%p) from the old one (%p)\n",
            this, &bfAxes);
      MessageInterface::ShowMessage("   and prevEpoch(old) %12.10f copied to prevEpoch(new) %12.10f\n",
            bfAxes.prevEpoch, prevEpoch);
   #endif
}

//------------------------------------------------------------------------------
//  ITRFAxes& operator=(const ITRFAxes &itrfAxes)
//------------------------------------------------------------------------------
/**
 * Assignment operator for ITRFAxes structures.
 *
 * @param itrfAxes The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const ITRFAxes& ITRFAxes::operator=(const ITRFAxes &itrfAxes)
{
   if (&itrfAxes == this)
      return *this;
   DynamicAxes::operator=(itrfAxes); 
//   de                       = itrfAxes.de;
   iauFile					= itrfAxes.iauFile;
   prevEpoch                = itrfAxes.prevEpoch;
   prevUpdateInterval       = itrfAxes.prevUpdateInterval;
   prevOriginUpdateInterval = itrfAxes.prevOriginUpdateInterval;
   prevLunaSrc              = itrfAxes.prevLunaSrc;

   return *this;
}


//------------------------------------------------------------------------------
//  ~ITRFAxes(void)
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ITRFAxes::~ITRFAxes()
{
}

GmatCoordinate::ParameterUsage ITRFAxes::UsesEopFile(const std::string &forBaseSystem) const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage ITRFAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
}

GmatCoordinate::ParameterUsage ITRFAxes::UsesNutationUpdateInterval() const
{
   if (originName == SolarSystem::EARTH_NAME) 
      return GmatCoordinate::REQUIRED;
   return DynamicAxes::UsesNutationUpdateInterval();
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this ITRFAxes.
 *
 */
//------------------------------------------------------------------------------
bool ITRFAxes::Initialize()
{
   DynamicAxes::Initialize();
   if (originName == SolarSystem::EARTH_NAME) InitializeFK5();
   #ifdef DEBUG_FIRST_CALL
      firstCallFired = false;
   #endif
   
   // create and initialize IAU2000/2006 object:
//   if (iauFile != NULL)
//      delete iauFile;
   if (iauFile == NULL)
	   iauFile = IAUFile::Instance();
   iauFile->Initialize();

   // create and initialize EopFile object:
   if (eop == NULL)
   {
	   FileManager* fm = FileManager::Instance();
	   std::string name = fm->GetFilename("EOP_FILE");
	   MessageInterface::ShowMessage("EOP file name:'%s'\n", name.c_str());
	   EopFile* eopFile = new EopFile(name);
	   eopFile->Initialize();
	   SetEopFile(eopFile);
   }

   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Planet.
 *
 * @return clone of the Planet.
 *
 */
//------------------------------------------------------------------------------
GmatBase* ITRFAxes::Clone() const
{
   return (new ITRFAxes(*this));
}

//------------------------------------------------------------------------------
//  bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                    const std::string &name)
//------------------------------------------------------------------------------
/**
 * This method sets a reference object for the ITRFAxes class.  This is
 * overridden from the CoordinateBase version, in order to make sure the origin
 * is a CelestialBody
 *
 * @param obj   pointer to the reference object
 * @param type  type of the reference object
 * @param name  name of the reference object
 *
 * @return true if successful; otherwise, false.
 *
 */
//------------------------------------------------------------------------------
bool ITRFAxes::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                 const std::string &name)
{
   if (obj == NULL)
      return false;

   #ifdef DEBUG_SET_REF
   MessageInterface::ShowMessage
      ("ITRFAxes::SetRefObject() <%s>, obj=%p, name=%s\n", GetName().c_str(),
       obj, name.c_str());
   #endif
   if (name == originName)
   {
      if (!obj->IsOfType("CelestialBody"))
      {
         CoordinateSystemException cse("");
         cse.SetDetails(errorMessageFormatUnnamed.c_str(),
                        (obj->GetName()).c_str(),
                       "Origin", "Celestial Body");
         throw cse;
      }
   }
   return DynamicAxes::SetRefObject(obj, type, name);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param id Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
//std::string ITRFAxes::GetParameterText(const Integer id) const
//{
//   if (id >= DynamicAxesParamCount && id < ITRFAxesParamCount)
//      return PARAMETER_TEXT[id - DynamicAxesParamCount];
//   return DynamicAxes::GetParameterText(id);
//}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param str string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
//Integer ITRFAxes::GetParameterID(const std::string &str) const
//{
//   for (Integer i = DynamicAxesParamCount; i < ITRFAxesParamCount; i++)
//   {
//      if (str == PARAMETER_TEXT[i - DynamicAxesParamCount])
//         return i;
//   }
//   
//   return DynamicAxes::GetParameterID(str);
//}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
//Gmat::ParameterType ITRFAxes::GetParameterType(const Integer id) const
//{
//   if (id >= DynamicAxesParamCount && id < ITRFAxesParamCount)
//      return PARAMETER_TYPE[id - DynamicAxesParamCount];
//   
//   return DynamicAxes::GetParameterType(id);
//}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param id ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
//std::string ITRFAxes::GetParameterTypeString(const Integer id) const
//{
//   return DynamicAxes::PARAM_TYPE_STRING[GetParameterType(id)];
//}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
* This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param id  ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
//std::string ITRFAxes::GetStringParameter(const Integer id) const
//{
//   return DynamicAxes::GetStringParameter(id);
//}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
* This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param id     ID for the requested parameter.
 * @param value  string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
//bool ITRFAxes::SetStringParameter(const Integer id, 
//                                       const std::string &value)
//{
//   return DynamicAxes::SetStringParameter(id, value);
//}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Accessor method used to get a parameter value
 *
 * @param    label  label ID for the parameter
 *
 * @return the value of the parameter
 */
//------------------------------------------------------------------------------
//std::string ITRFAxes::GetStringParameter(const std::string &label) const
//{
//   return GetStringParameter(GetParameterID(label));
//}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
* Accessor method used to get a parameter value
 *
 * @param    label  Integer ID for the parameter
 * @param    value  The new value for the parameter
 */
//------------------------------------------------------------------------------
//bool ITRFAxes::SetStringParameter(const std::string &label,
//                                        const std::string &value)
//{
//   return SetStringParameter(GetParameterID(label), value);
//}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

Rmatrix33 ITRFAxes::R1(Real angle)
{
	Rmatrix33 r;
	Real c = cos(angle);
	Real s = sin(angle);
	r.SetElement(0,0,1.0);	r.SetElement(0,1,0.0);	r.SetElement(0,2,0.0);
	r.SetElement(1,0,0.0);	r.SetElement(1,1,c);	r.SetElement(1,2,s);
	r.SetElement(2,0,0.0);	r.SetElement(2,1,-s);	r.SetElement(2,2,c);

	return r;
}


Rmatrix33 ITRFAxes::R2(Real angle)
{
	Rmatrix33 r;
	Real c = cos(angle);
	Real s = sin(angle);
	r.SetElement(0,0,c);	r.SetElement(0,1,0.0);	r.SetElement(0,2,-s);
	r.SetElement(1,0,0.0);	r.SetElement(1,1,1.0);	r.SetElement(1,2,0.0);
	r.SetElement(2,0,s);	r.SetElement(2,1,0.0);	r.SetElement(2,2,c);

	return r;
}

Rmatrix33 ITRFAxes::R3(Real angle)
{
	Rmatrix33 r;
	Real c = cos(angle);
	Real s = sin(angle);
	r.SetElement(0,0,c);	r.SetElement(0,1,s);	r.SetElement(0,2,0.0);
	r.SetElement(1,0,-s);	r.SetElement(1,1,c);	r.SetElement(1,2,0.0);
	r.SetElement(2,0,0.0);	r.SetElement(2,1,0.0);	r.SetElement(2,2,1.0);

	return r;
}


Rmatrix33 ITRFAxes::Skew(Rvector3 vec)
{
	Rmatrix33 r;
	r.SetElement(0,0,0.0);					r.SetElement(0,1,-vec.GetElement(2));	r.SetElement(0,2,vec.GetElement(1));
	r.SetElement(1,0,vec.GetElement(2));	r.SetElement(1,1,0.0);					r.SetElement(1,2,-vec.GetElement(0));
	r.SetElement(2,0,-vec.GetElement(1));	r.SetElement(2,1,vec.GetElement(0));	r.SetElement(2,2,0.0);

	return r;
}


//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch, 
//                               bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000Eq system
 *
 * @param atEpoch  epoch at which to compute the rotation matrix
 *
 * @note Code (for Earth) adapted from C code written by Steve Queen/ GSFC, 
 *       based on Vallado, pgs. 211- 227.  Equation references in parentheses
 *       will refer to those of the Vallado book.
 */
//------------------------------------------------------------------------------
void ITRFAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                            bool forceComputation) 
{
   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage(
            "Calling CalculateRotationMatrix at epoch %lf; ", atEpoch.Get());
   #endif
   Real theEpoch = atEpoch.Get();
   #ifdef DEBUG_BF_EPOCHS
      MessageInterface::ShowMessage("CalculateRotationMatrix(%s)   epoch = %12.10f, prevEpoch = %12.10f ...... ",
            (coordName.c_str()), theEpoch, prevEpoch);
   #endif
   #ifdef DEBUG_BF_RECOMPUTE
      MessageInterface::ShowMessage("Entering CalculateRotationMatrix on object %s (%p) of type %s, origin = %s\n",
            (coordName.c_str()), this, (GetTypeName()).c_str(), originName.c_str());
      MessageInterface::ShowMessage("     epoch = %12.10f, prevEpoch = %12.10f\n", theEpoch, prevEpoch);
   #endif

   // Code must check to see if we need to recompute.  Recomputation is only necessary
   // if one or more of the following conditions are true:
   // 1. the epoch is different (within machine precision) from the epoch at the last computation
   // 2. if the origin is the Earth, the nutation update interval has changed
   // 3. if the origin is Luna, the rotation data source has changed

   if ((!forceComputation)                    &&
       (originName == SolarSystem::MOON_NAME) &&
       (IsEqual(theEpoch,      prevEpoch))    &&
       (prevLunaSrc == ((CelestialBody*)origin)->GetRotationDataSource()))
      {
         #ifdef DEBUG_BF_RECOMPUTE
            MessageInterface::ShowMessage("Don't need to recompute for Luna at this time!!\n");
         #endif
         return;
      }
   

   // compute rotMatrix and rotDotMatrix
//   if (originName == SolarSystem::EARTH_NAME)
//   {
      Real intervalFromOrigin = ((Planet*) origin)->GetNutationUpdateInterval();
      if ((!forceComputation)                                     &&
          (IsEqual(theEpoch,           prevEpoch)                 &&
          (IsEqual(intervalFromOrigin, prevOriginUpdateInterval)) &&
          (IsEqual(updateInterval,     prevUpdateInterval))))
      {
         #ifdef DEBUG_BF_EPOCHS
            MessageInterface::ShowMessage("NOT recomputing\n");
         #endif
         #ifdef DEBUG_BF_RECOMPUTE
            MessageInterface::ShowMessage("Don't need to recompute for Earth at this time!!\n");
         #endif
         return;
      }

      #ifdef DEBUG_BF_EPOCHS
         MessageInterface::ShowMessage("RECOMPUTING!!!\n");
      #endif
      #ifdef DEBUG_BF_RECOMPUTE
         MessageInterface::ShowMessage("   RECOMPUTING!!! - body name is EARTH\n");
      #endif
      #ifdef DEBUG_FIRST_CALL
         if (!firstCallFired)
            MessageInterface::ShowMessage("In BFA, Body is the Earth\n");
      #endif


      //  Perform time computations and read EOP file
	  Real sec2rad = PI/180/3600;
	  Real a1MJD = theEpoch;

      Real utcMJD = TimeConverterUtil::Convert(a1MJD,
                    TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD,
                    JD_JAN_5_1941);
      Real offset = JD_JAN_5_1941 - JD_NOV_17_1858;

	  // [dUT1,LOD,xp,yp] = GetEOP(utcMJD +  29999.5);
	  Real xp,yp,LOD,dUT1;
	  dUT1 = eop->GetUt1UtcOffset(utcMJD +  offset);
	  eop->GetPolarMotionAndLod(utcMJD +  offset, xp, yp, LOD);

      xp = xp*sec2rad;
      yp = yp*sec2rad;

	  Real ut1MJD = TimeConverterUtil::Convert(a1MJD,
                    TimeConverterUtil::A1MJD, TimeConverterUtil::UT1,
                    JD_JAN_5_1941);

      // Compute elapsed Julian centuries (UT1)
      //Real tUT1     = (jdUT1 - JD_OF_J2000) / DAYS_PER_JULIAN_CENTURY;
      Real tDiff = JD_JAN_5_1941 - JD_OF_J2000;
//      MessageInterface::ShowMessage("tDiff = %lf\n",tDiff);
      Real jdUT1 = ut1MJD + JD_JAN_5_1941;

      // convert input A1 MJD to TT MJD (for most calculations)
      Real ttMJD = TimeConverterUtil::Convert(a1MJD,
                   TimeConverterUtil::A1MJD, TimeConverterUtil::TTMJD,
                   JD_JAN_5_1941);

      Real jdTT    = ttMJD + JD_JAN_5_1941; // right?
      // Compute Julian centuries of TDB from the base epoch (J2000)
      // NOTE - this is really TT, an approximation of TDB *********
      //Real tTDB    = (jdTT - JD_OF_J2000) / DAYS_PER_JULIAN_CENTURY;
      Real T_TT    = (ttMJD + tDiff) / DAYS_PER_JULIAN_CENTURY;

      //  Compute the Polar Motion Matrix, W, and Earth Rotation Angle, theta
      Real sPrime = -0.000047*sec2rad*T_TT;
      Rmatrix33 W = R3(-sPrime)*R2(xp)*R1(yp);
      Real theta  = fmod(2*PI*(0.7790572732640 + 1.00273781191135448*(jdUT1 - 2451545.0)),2*PI);

	  //  Compute the precession-nutation matrix
	  //  . interpolate the XYs data file
	  Real data[3];
	  if (iauFile == NULL)
	  {
		  MessageInterface::PopupMessage(Gmat::ERROR_, "Error: IAUFile object is NULL. GMAT cann't get IAU data...\n");
	  }
	  iauFile->GetIAUData(jdTT,data,3,9);
	  Real X = data[0]*sec2rad;
	  Real Y = data[1]*sec2rad;
	  Real s = data[2]*sec2rad;

	  // . construct the Precession Nutation matrix
	  Real b = 1/(1 + sqrt(1- X*X - Y*Y));
	  Rmatrix33 CT;
	  CT.SetElement(0,0, 1-b*X*X);	CT.SetElement(0,1, -b*X*Y ); CT.SetElement(0,2,X);
	  CT.SetElement(1,0, -b*X*Y);   CT.SetElement(1,1, 1-b*Y*Y); CT.SetElement(1,2,Y);
	  CT.SetElement(2,0, -X);       CT.SetElement(2,1, -Y);      CT.SetElement(2,2,(1 - b*(X*X + Y*Y)));
      CT = CT*R3(s);
	  
	  //  Form the complete rotation matrix from ITRF to GCRF
	  Rmatrix33 R    = CT*R3(-theta)*W;
	  Real omegaEarth = 7.292115146706979e-5*(1 - LOD/86400);
	  Rvector3 vec(0.0, 0.0, omegaEarth);
	  Rmatrix33 Rdot = CT*R3(-theta)*Skew(vec)*W;
	  rotMatrix = R;
	  rotDotMatrix = Rdot;

	  #ifdef DEBUG_BF_ROT_MATRIX
	  MessageInterface::ShowMessage("a1MJD  = %18.10lf\n",a1MJD);
	  MessageInterface::ShowMessage("utcMJD = %18.10lf\n",utcMJD);
	  MessageInterface::ShowMessage("dUT1=%18.10e, xp=%18.10e, yp=%18.10e, LOD=%18.10e\n",dUT1,xp,yp,LOD);
      MessageInterface::ShowMessage("ut1MJD = %18.10lf\n",ut1MJD);
      MessageInterface::ShowMessage("ttMJD  = %18.10lf\n",ttMJD);
      MessageInterface::ShowMessage("jdTT   = %18.10lf\n",jdTT);
      MessageInterface::ShowMessage("jdUT1  = %18.10lf\n",jdUT1);
      MessageInterface::ShowMessage("T_TT   = %18.10lf\n\n",T_TT);

	  MessageInterface::ShowMessage("sPrime = %18.10lf,  theta = %18.10lf\n",sPrime, theta);
	  MessageInterface::ShowMessage("W(0,0)=%18.10lf,  W(0,1)=%18.10lf,  W(0,2)=%18.10lf\n",W.GetElement(0,0),W.GetElement(0,1),W.GetElement(0,2));
	  MessageInterface::ShowMessage("W(1,0)=%18.10lf,  W(1,1)=%18.10lf,  W(1,2)=%18.10lf\n",W.GetElement(1,0),W.GetElement(1,1),W.GetElement(1,2));
	  MessageInterface::ShowMessage("W(2,0)=%18.10lf,  W(2,1)=%18.10lf,  W(2,2)=%18.10lf\n",W.GetElement(2,0),W.GetElement(2,1),W.GetElement(2,2));

	  MessageInterface::ShowMessage("X = %18.10lf,   Y = %18.10lf,   s = %18.10lf\n", X, Y, s);
	  MessageInterface::ShowMessage("CT(0,0)=%18.10lf,  CT(0,1)=%18.10lf,  CT(0,2)=%18.10lf\n",CT.GetElement(0,0),CT.GetElement(0,1),CT.GetElement(0,2));
	  MessageInterface::ShowMessage("CT(1,0)=%18.10lf,  CT(1,1)=%18.10lf,  CT(1,2)=%18.10lf\n",CT.GetElement(1,0),CT.GetElement(1,1),CT.GetElement(1,2));
	  MessageInterface::ShowMessage("CT(2,0)=%18.10lf,  CT(2,1)=%18.10lf,  CT(2,2)=%18.10lf\n",CT.GetElement(2,0),CT.GetElement(2,1),CT.GetElement(2,2));

	  MessageInterface::ShowMessage("R(0,0)=%18.10lf,  R(0,1)=%18.10lf,  R(0,2)=%18.10lf\n",R.GetElement(0,0),R.GetElement(0,1),R.GetElement(0,2));
	  MessageInterface::ShowMessage("R(1,0)=%18.10lf,  R(1,1)=%18.10lf,  R(1,2)=%18.10lf\n",R.GetElement(1,0),R.GetElement(1,1),R.GetElement(1,2));
	  MessageInterface::ShowMessage("R(2,0)=%18.10lf,  R(2,1)=%18.10lf,  R(2,2)=%18.10lf\n",R.GetElement(2,0),R.GetElement(2,1),R.GetElement(2,2));

	  MessageInterface::ShowMessage("Rdot(0,0)=%18.10lf,  Rdot(0,1)=%18.10lf,  Rdot(0,2)=%18.10lf\n",Rdot.GetElement(0,0),Rdot.GetElement(0,1),Rdot.GetElement(0,2));
	  MessageInterface::ShowMessage("Rdot(1,0)=%18.10lf,  Rdot(1,1)=%18.10lf,  Rdot(1,2)=%18.10lf\n",Rdot.GetElement(1,0),Rdot.GetElement(1,1),Rdot.GetElement(1,2));
	  MessageInterface::ShowMessage("Rdot(2,0)=%18.10lf,  Rdot(2,1)=%18.10lf,  Rdot(2,2)=%18.10lf\n\n\n",Rdot.GetElement(2,0),Rdot.GetElement(2,1),Rdot.GetElement(2,2));
      #endif

      if (overrideOriginInterval) 
      {
         updateIntervalToUse = updateInterval;
         //MessageInterface::ShowMessage("Overriding origin interval .....\n");
         #ifdef DEBUG_FIRST_CALL
            if (!firstCallFired)
               MessageInterface::ShowMessage(
                  "   Overrode origin interval; set to %.12lf\n", 
                  updateIntervalToUse);
         #endif
      }
      else 
      {
         updateIntervalToUse = intervalFromOrigin;
         //MessageInterface::ShowMessage("Using origin interval .....\n");
         #ifdef DEBUG_FIRST_CALL
            if (!firstCallFired)
               MessageInterface::ShowMessage(
                  "   Using body's origin interval, %.12lf\n", 
                  updateIntervalToUse);
         #endif
      }
      

      // save the data to compare against next time
      prevUpdateInterval       = updateInterval;
      prevOriginUpdateInterval = intervalFromOrigin;
//   }


   // Save the epoch for comparison the next time through
   prevEpoch = theEpoch;
   #ifdef DEBUG_BF_RECOMPUTE
      MessageInterface::ShowMessage("at the end, just set prevEpoch to %12.10f\n", prevEpoch);
   #endif
   if (originName == SolarSystem::MOON_NAME)
      prevLunaSrc = ((CelestialBody*)origin)->GetRotationDataSource();

   #ifdef DEBUG_FIRST_CALL
      firstCallFired = true;
      MessageInterface::ShowMessage("NOW exiting BFA::CalculateRotationMatrix ...\n");
   #endif

}

