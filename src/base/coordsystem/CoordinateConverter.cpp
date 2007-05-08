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
//#define DEBUG_TO_FROM

#ifdef DEBUG_TO_FROM
   #include "MessageInterface.hpp"
#endif

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
   //internalState.SetSize(6);
   //toMJ2000RotMatrix = Rmatrix33();
   //fromMJ2000Matrix  = Rmatrix33();
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
//                CoordinateSystem *outCoord, 
//                bool forceComputation, bool omitTranslation) 
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
                          CoordinateSystem *outCoord, 
                          bool forceComputation, bool omitTranslation)
{
   const Real *in = inState.GetDataVector();
   Real *out = new Real[outState.GetSize()];
   
   if (Convert(epoch, in, inCoord, out, outCoord, forceComputation, omitTranslation))
   {
      outState.Set(out);
      delete [] out;
      return true;
   }
   
   delete [] out;
   return false;

//    #ifdef DEBUG_TO_FROM
//       MessageInterface::ShowMessage("In Convert, inCoord is %s and outCoord is %s\n",
//          (inCoord->GetName()).c_str(), (outCoord->GetName()).c_str());
//    #endif
//    if (inCoord->GetName() == outCoord->GetName())
//    {
//       outState = inState;
//       lastRotMatrix.Set(1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0);
//       return true;
//    }
//    #ifdef DEBUG_FIRST_CALL
//       if ((firstCallFired == false) || (epoch.Get() == 21545.0))
//       {
//          MessageInterface::ShowMessage(
//             "Coordinate conversion check:\n      %s --> %s\n", 
//             inCoord->GetName().c_str(), outCoord->GetName().c_str());
//          MessageInterface::ShowMessage(
//             "      Epoch: %.12lf\n", epoch.Get());
//          MessageInterface::ShowMessage(
//             "      input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
//             inState[0], inState[1], inState[2], inState[3], inState[4], 
//             inState[5]);
//       }
//    #endif
//    static Rvector internalState;
//    if (inState.GetSize() != outState.GetSize())
//       throw CoordinateSystemException(
//              "input and output states have different sizes - no conversion done");
//    if ((!inCoord) || (!outCoord))
//       throw CoordinateSystemException(
//             "Undefined coordinate system - conversion not performed.");
   
//    // call coordinate system methods to convert - allow exceptions to
//    // percolate up (to be caught at a higher level)
//    internalState.SetSize(inState.GetSize());
//    internalState.MakeZeroVector();
//    bool coincident = (inCoord->GetOrigin() == outCoord->GetOrigin() ? 
//                       true : false);
//    bool translateFlag = coincident || omitTranslation;
//    Real intState[6];  //
//    Rvector tmpState(inState);
//    const Real *inputState = tmpState.GetDataVector();

//    Real finalState[6];
//    inCoord->ToMJ2000Eq(epoch, inputState, intState, translateFlag,
//                        forceComputation);
//    outCoord->FromMJ2000Eq(epoch, intState, finalState, translateFlag, 
//                           forceComputation);
//    outState.Set(6,finalState[0],finalState[1],finalState[2],
//                   finalState[3],finalState[4],finalState[5]);
   
//    Real lastToMat[9];
//    Real lastFromMat[9];
//    inCoord->GetLastRotationMatrix(lastToMat);
//    outCoord->GetLastRotationMatrix(lastFromMat);
//    Real  fromDataT[9] = {lastFromMat[0], lastFromMat[3], lastFromMat[6],
//                          lastFromMat[1], lastFromMat[4], lastFromMat[7],
//                          lastFromMat[2], lastFromMat[5], lastFromMat[8]};
//    Real lrm[3][3];
//    Integer p3;
      
//    for (Integer p = 0; p < 3; ++p)
//    {
//       p3 = 3*p;
//       for (Integer q = 0; q < 3; ++q)
//       {
//          lrm[p][q] = fromDataT[p3]   * lastToMat[q]   + 
//                      fromDataT[p3+1] * lastToMat[q+3] + 
//                      fromDataT[p3+2] * lastToMat[q+6];
//       }
//    }
   
//    lastRotMatrix.Set(lrm[0][0],lrm[0][1],lrm[0][2],
//                      lrm[1][0],lrm[1][1],lrm[1][2],
//                      lrm[2][0],lrm[2][1],lrm[2][2]);
   
//    #ifdef DEBUG_FIRST_CALL
//       if ((firstCallFired == false) || (epoch.Get() == 21545.0))
//       {
//          MessageInterface::ShowMessage(
//             "Coordinate conversion check:\n      %s --> %s\n", 
//             inCoord->GetName().c_str(), outCoord->GetName().c_str());
//          MessageInterface::ShowMessage(
//             "      Epoch: %.12lf\n", epoch.Get());
//          MessageInterface::ShowMessage(
//             "      input State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
//             inState[0], inState[1], inState[2], inState[3], inState[4], 
//             inState[5]);
//          MessageInterface::ShowMessage(
//             "      internal State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
//             internalState[0], internalState[1], internalState[2], internalState[3], 
//             internalState[4], internalState[5]);
//          MessageInterface::ShowMessage(
//             "      output State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
//             outState[0], outState[1], outState[2], outState[3], outState[4], 
//             outState[5]);
//          firstCallFired = true;
//       }
//    #endif
   
//    return true;
   
}


//------------------------------------------------------------------------------
// bool Convert(const A1Mjd &epoch, const Real *inState,
//              CoordinateSystem *inCoord, Real *outState,
//              CoordinateSystem *outCoord, bool forceComputation = false,
//              bool omitTranslation = false)
//------------------------------------------------------------------------------
bool CoordinateConverter::Convert(const A1Mjd &epoch, const Real *inState,
                          CoordinateSystem *inCoord, Real *outState,
                          CoordinateSystem *outCoord, 
                          bool forceComputation, bool omitTranslation)
{
   
   #ifdef DEBUG_TO_FROM
      MessageInterface::ShowMessage
         ("In Convert, inCoord is %s(%p) and outCoord is %s(%p)\n",
         (inCoord->GetName()).c_str(), inCoord, (outCoord->GetName()).c_str(),
          outCoord);
   #endif
   if (inCoord->GetName() == outCoord->GetName())
   {
      // asssuming state is size 6 here!!!
      for (Integer i=0;i<6;i++) outState[i] = inState[i];
      lastRotMatrix.Set(1.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,1.0);
      return true;
   }
   
   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == 21545.0))
      {
         MessageInterface::ShowMessage(
            "Coordinate conversion check:\n   %s --> %s\n", 
            inCoord->GetName().c_str(), outCoord->GetName().c_str());
         MessageInterface::ShowMessage(
            "   Epoch: %.12lf\n", epoch.Get());
         MessageInterface::ShowMessage(
            "   input State    = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            inState[0], inState[1], inState[2], inState[3], inState[4], 
            inState[5]);
      }
   #endif

   if ((!inCoord) || (!outCoord))
      throw CoordinateSystemException(
         "Undefined coordinate system - conversion not performed.");
   
   //MessageInterface::ShowMessage
   //   ("===> inCoord->GetOrigin=%s(%p), outCoord->GetOrigin=%s(%p)\n",
   //    inCoord->GetOrigin()->GetName().c_str(), inCoord->GetOrigin(),
   //    outCoord->GetOrigin()->GetName().c_str(), outCoord->GetOrigin());
   
   // call coordinate system methods to convert - allow exceptions to
   // percolate up (to be caught at a higher level)
   bool sameOrigin = (inCoord->GetOrigin() == outCoord->GetOrigin() ? 
                      true : false);
   
   bool coincident = sameOrigin || omitTranslation;
   Real intState[6];
   
   //MessageInterface::ShowMessage
   //   ("===> sameOrigin=%d, omitTranslation=%d, coincident=%d\n",
   //    sameOrigin, omitTranslation, coincident);
   
   inCoord->ToMJ2000Eq(epoch, inState, intState, coincident, forceComputation);
   outCoord->FromMJ2000Eq(epoch, intState, outState, coincident,
                          forceComputation);
   
   Real lastToMat[9];
   Real lastFromMat[9];
   inCoord->GetLastRotationMatrix(lastToMat);
   outCoord->GetLastRotationMatrix(lastFromMat);
   Real  fromDataT[9] = {lastFromMat[0], lastFromMat[3], lastFromMat[6],
                         lastFromMat[1], lastFromMat[4], lastFromMat[7],
                         lastFromMat[2], lastFromMat[5], lastFromMat[8]};
   Real lrm[3][3];
   Integer p3;
   
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         lrm[p][q] = fromDataT[p3]   * lastToMat[q]   + 
                     fromDataT[p3+1] * lastToMat[q+3] + 
                     fromDataT[p3+2] * lastToMat[q+6];
      }
   }
   lastRotMatrix.Set(lrm[0][0],lrm[0][1],lrm[0][2],
                     lrm[1][0],lrm[1][1],lrm[1][2],
                     lrm[2][0],lrm[2][1],lrm[2][2]);
   
   #ifdef DEBUG_FIRST_CALL
      if ((firstCallFired == false) || (epoch.Get() == 21545.0))
      {
         MessageInterface::ShowMessage(
            "   internal State = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            intState[0], intState[1], intState[2], intState[3], 
            intState[4], intState[5]);
         MessageInterface::ShowMessage(
            "   output State   = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n",
            outState[0], outState[1], outState[2], outState[3], outState[4], 
            outState[5]);
         firstCallFired = true;
      }
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// Rmatrix33 CoordinateConverter::GetLastRotationMatrix() const
//------------------------------------------------------------------------------
Rmatrix33 CoordinateConverter::GetLastRotationMatrix() const
{
   return lastRotMatrix;
}

