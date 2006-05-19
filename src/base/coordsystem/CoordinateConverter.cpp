//$Header$
//------------------------------------------------------------------------------
//                                  CoordinateConverter
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2004/12/27
//
/**
 * Implementation of the CoordinateConverter class.
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "CoordinateConverter.hpp"
#include "SpacePoint.hpp"
#include "A1Mjd.hpp"
#include "CoordinateSystem.hpp"
#include "CoordinateSystemException.hpp"
#include "Rvector.hpp"

//#define DEBUG_FIRST_CALL


#ifdef DEBUG_FIRST_CALL
   #include "MessageInterface.hpp"
   static bool firstCallFired = false;
#endif

//---------------------------------
// static data
//---------------------------------
// none at this time

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  CoordinateConverter();
//---------------------------------------------------------------------------
/**
 * Constructs CoordinateConverter objects.
 * (default constructor).
 */
//---------------------------------------------------------------------------
CoordinateConverter::CoordinateConverter() //:
//j2000Body         (NULL),
//j2000BodyName     ("Earth")
{
}

//---------------------------------------------------------------------------
//  CoordinateConverter(const CoordinateConverter &coordCvt);
//---------------------------------------------------------------------------
/**
 * Constructs CoordinateConverter classes, by copying 
 * the input instance (copy constructor).
 *
 * @param coordCvt  CoordinateConverter instance to copy to create "this" 
 *                  instance.
 */
//---------------------------------------------------------------------------
CoordinateConverter::CoordinateConverter(const CoordinateConverter &coordCvt) //:
//j2000Body     (coordCvt.j2000Body),
//j2000BodyName (coordCvt.j2000BodyName)
{
}

//---------------------------------------------------------------------------
//  CoordinateConverter& operator=(const CoordinateConverter &coordCvt)
//---------------------------------------------------------------------------
/**
 * Assignment operator for CoordinateConverter objects.
 *
 * @param coordCvt The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const CoordinateConverter& CoordinateConverter::operator=(
                           const CoordinateConverter &coordCvt)
{
   if (&coordCvt == this)
      return *this;
   //j2000Body     = coordCvt.j2000Body;
   //j2000BodyName = coordCvt.j2000BodyName;
   
   return *this;
}
//---------------------------------------------------------------------------
//  ~CoordinateConverter()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
CoordinateConverter::~CoordinateConverter()
{
   // nothing to do ... hm ... hm ... hm ... hm ... la-de-da-de-da
}

//------------------------------------------------------------------------------
//  void  Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the CoordinateConverter class.
 *
 */
//------------------------------------------------------------------------------
void CoordinateConverter::Initialize()
{
   //if (!j2000Body)
   //   throw CoordinateSystemException(
   //         "j2000Body has not been defined for CoordinateConverter object");
   //toData      = toMJ2000RotMatrix.GetDataVector();
   //fromData    = fromMJ2000Matrix.GetDataVector();
   #ifdef DEBUG_FIRST_CALL
      firstCallFired = false;
   #endif
}

//------------------------------------------------------------------------------
//  void  SetJ2000BodyName(const std::string &toName)
//------------------------------------------------------------------------------
/**
 * This method sets the j2000 body name for the CoordinateConverter object.
 *
 * @param toName name to which to set the j2000 body name.
 */
//------------------------------------------------------------------------------
//void CoordinateConverter::SetJ2000BodyName(const std::string &toName)
//{
//   j2000BodyName = toName;
//}

//------------------------------------------------------------------------------
//  std::string  GetJ2000BodyName() const
//------------------------------------------------------------------------------
/**
 * This method returns the j2000 body name for the CoordinateConverter object.
 *
 * @return j2000 body name.
 */
//------------------------------------------------------------------------------
//std::string CoordinateConverter::GetJ2000BodyName() const
//{
//   if (j2000Body) return j2000Body->GetName();
//   else           return j2000BodyName;
//}

//------------------------------------------------------------------------------
//  void  SetJ2000Body(SpacePoint *toBody)
//------------------------------------------------------------------------------
/**
 * This method sets the j2000 body for the CoordinateConverter object.
 *
 * @param toBody body pointer to which to set the j2000 body parameter.
 */
//------------------------------------------------------------------------------
//void CoordinateConverter::SetJ2000Body(SpacePoint *toBody)
//{
//   j2000Body = toBody;
//}

//------------------------------------------------------------------------------
//  SpacePoint*  GetJ2000Body() const
//------------------------------------------------------------------------------
/**
 * This method returns the j2000 body pointer for the CoordinateConverter 
 * object.
 *
 * @return j2000 body pointer.
 */
//------------------------------------------------------------------------------
//SpacePoint* CoordinateConverter::GetJ2000Body()
//{
//   return j2000Body;
//}


//------------------------------------------------------------------------------
//  bool  Convert(const A1Mjd &epoch, const Rvector &inState,
//                CoordinateSystem *inCoord, Rvector &outState,
//                CoordinateSystem *outCoord, bool omitTranslation) 
//------------------------------------------------------------------------------
/**
 * This method converts the inState vector from coordIn CoordinateSystem
 * to the coordOut CoordinateSystem, at the input epoch, and returns the
 * result in outState.
 *
 * @param epoch    time for which to do the conversion.
 * @param inState  input State (in inCoord system).
 * @param inCoord  pointer to the input CoordinateSystem.
 * @param outState resulting vector, in the outCoord system.
 * @param outCoord pointer to the output CoordinateSystem.
 * @param omitTranslation omit the translation whether coincident or not
 *                        (default is false)
 *
 * @return true if successful; false otherwise.
 */
//------------------------------------------------------------------------------
bool CoordinateConverter::Convert(const A1Mjd &epoch, const Rvector &inState,
                          CoordinateSystem *inCoord, Rvector &outState,
                          CoordinateSystem *outCoord, bool omitTranslation)
{
   static Rvector internalState;
   static Rmatrix33 toMJ2000RotMatrix;
   static Rmatrix33 fromMJ2000Matrix;
   static const Real *toData;
   static const Real *fromData;
   toData      = toMJ2000RotMatrix.GetDataVector();
   fromData    = fromMJ2000Matrix.GetDataVector();
   if (inState.GetSize() != outState.GetSize())
      throw CoordinateSystemException(
             "input and output states have different sizes - no conversion done");
   if ((!inCoord) || (!outCoord))
      throw CoordinateSystemException(
            "Undefined coordinate system - conversion not performed.");
   // call coordinate system methods to convert - allow exceptions to
   // percolate up (to be caught at a higher level)
   internalState.SetSize(inState.GetSize());
   bool coincident = (inCoord->GetOrigin() == outCoord->GetOrigin() ? 
                      true : false);
   bool translateFlag = coincident || omitTranslation;
   internalState = inCoord->ToMJ2000Eq(epoch, inState, translateFlag);
   outState      = outCoord->FromMJ2000Eq(epoch, internalState, translateFlag);
   toMJ2000RotMatrix   = inCoord->GetLastRotationMatrix();
   fromMJ2000Matrix    = outCoord->GetLastRotationMatrix();
   const Real  fromDataT[9] = {fromData[0], fromData[3], fromData[6],
                               fromData[1], fromData[4], fromData[7],
                               fromData[2], fromData[5], fromData[8]};
   Real lrm[3][3];
   Integer p3;
      
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         lrm[p][q] = fromDataT[p3]   * toData[q]   + 
                     fromDataT[p3+1] * toData[q+3] + 
                     fromDataT[p3+2] * toData[q+6];
      }
   }     
   lastRotMatrix.Set(lrm[0][0],lrm[0][1],lrm[0][2],
                     lrm[1][0],lrm[1][1],lrm[1][2],
                     lrm[2][0],lrm[2][1],lrm[2][2]);
   //lastRotMatrix = (fromMJ2000Matrix.Transpose()) * toMJ2000RotMatrix;
   
   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == 21545.0))
      {
         MessageInterface::ShowMessage(
            "Coordinate conversion check:\n      %s --> %s\n", 
            inCoord->GetName().c_str(), outCoord->GetName().c_str());
         MessageInterface::ShowMessage(
            "      Epoch: %.12lf\n", epoch.Get());
         MessageInterface::ShowMessage(
            "      input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
         MessageInterface::ShowMessage(
            "      outpt State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            outState[0], outState[1], outState[2], outState[3], outState[4], 
            outState[5]);
         firstCallFired = true;
      }
   #endif
   
   return true;
}

Rmatrix33 CoordinateConverter::GetLastRotationMatrix() const
{
   return lastRotMatrix;
}

