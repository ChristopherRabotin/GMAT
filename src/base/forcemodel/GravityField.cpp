//$Id$
//------------------------------------------------------------------------------
//                              GravityField
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: D. Conway
// Created: 2003/03/17
// Modified: 2004/04/15 W. Shoan GSFC Code 583
//           Updated for GMAT style, standards; changed Real to Real, int
//           to Integer, etc.
//
//           2004/11/02 D. Conway Thinking Systems, Inc.
//           Changed internal arrays to static arrays to fix memory error.
//
// @note For now, we assume
//       1) we are computing state derivatives along with any
//          STM or A-Matrix derivatives.
/**
 * This is the GravityField class.
 *
 * @note original prolog information included at end of file prolog.
 */
//
// *** File Name : shgravityfield.cpp
// *** Created   : March 17, 2003
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
// Modification History      : 3/17/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery.  Happy St. Patrick's Day!
// **************************************************************************

//#include <iostream>   // JPD removed these
//#include <fstream>
////#include <strstream>
//#include <sstream>
//#include <iomanip>
#include "GravityField.hpp"
#include "ODEModelException.hpp"
#include "CelestialBody.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"
#include "Rvector.hpp"
#include "TimeTypes.hpp"
#include "CoordinateConverter.hpp"
#include "StringUtil.hpp"
#include "GmatConstants.hpp"
#include "GmatDefaults.hpp"
#include "GravityFileUtil.hpp"
#include "FileManager.hpp"
#include "HarmonicGravityCof.hpp"
#include "HarmonicGravityGrv.hpp"
#include <sstream>                 // for <<


//#define DEBUG_GRAVITY_FIELD
//#define DEBUG_GRAVITY_FIELD_DETAILS
//#define DEBUG_FIRST_CALL
//#define DEBUG_GRAV_COORD_SYSTEM
//#define DEBUG_BODY_STATE
//#define DEBUG_GRAVITY_FILE_READ
//#define DEBUG_GRAVITY_EARTH_TIDE
//#define DEBUG_CALCULATE
//#define DEBUG_BODY_DATA
//#define DEBUG_DEGREE_ORDER
//#define DEBUG_DERIVATIVES
//#define DEBUG_CREATE_DELETE


using namespace GmatMathUtil;

//---------------------------------
// static data
//---------------------------------

#ifdef DEBUG_FIRST_CALL
   static bool firstCallFired = false;
#endif

std::vector<HarmonicGravity*> GravityField::cache;

const std::string
GravityField::PARAMETER_TEXT[GravityFieldParamCount - HarmonicFieldParamCount] =
{
   "Mu",
   "A",
   "EarthTideModel",
};

const Gmat::ParameterType
GravityField::PARAMETER_TYPE[GravityFieldParamCount - HarmonicFieldParamCount] =
{
   Gmat::REAL_TYPE,
   Gmat::REAL_TYPE,
   Gmat::STRING_TYPE,
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GravityField(const std::string &name, const std::string &bodyName,
//               Integer deg, Integer ord)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the GravityField class
 * (default constructor).
 *
 * @param <name> parameter indicating the name of the object.
 * @param <deg>  maximum degree of the polynomials.
 * @param <ord>  maximum order of the polynomials.
 */
//------------------------------------------------------------------------------
GravityField::GravityField(const std::string &name, const std::string &forBodyName,
                           Integer maxDeg, Integer maxOrd) :
   HarmonicField          (name, "GravityField", maxDeg, maxOrd),
   mu                     (GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH]),
   a                      (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]),
   earthTideModel         ("None"),
   defaultMu              (GmatSolarSystemDefaults::PLANET_MU[GmatSolarSystemDefaults::EARTH]),
   defaultA               (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[GmatSolarSystemDefaults::EARTH]),
   gfInitialized          (false),
   orderTruncateReported  (false),
   degreeTruncateReported (false),
   gravityModel           (NULL)
{
   objectTypeNames.push_back("GravityField");
   bodyName = forBodyName;
   parameterCount = GravityFieldParamCount;

   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   derivativeIds.push_back(Gmat::ORBIT_STATE_TRANSITION_MATRIX);
   derivativeIds.push_back(Gmat::ORBIT_A_MATRIX);
}


//------------------------------------------------------------------------------
//  ~GravityField()
//------------------------------------------------------------------------------
/**
 * This method destroys the GravityField object
 * (destructor).
 *
 */
//------------------------------------------------------------------------------
GravityField::~GravityField()
{
   gravityModel = NULL;
}


//------------------------------------------------------------------------------
//  GravityField(const GravityField & gf)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the GravityField class from the input
 * object.
 * (copy constructor).
 *
 * @param <gf> object to copy to create this new object.
 */
//------------------------------------------------------------------------------
GravityField::GravityField(const GravityField &gf) :
    HarmonicField          (gf),
    mu                     (gf.mu),
    a                      (gf.a),
    earthTideModel         (gf.earthTideModel),
    defaultMu              (gf.defaultMu),
    defaultA               (gf.defaultA),
    gfInitialized          (false),
    orderTruncateReported  (gf.orderTruncateReported),
    degreeTruncateReported (gf.degreeTruncateReported),
    gravityModel           (NULL),
    frv                    (gf.frv),
    trv                    (gf.trv),
    now                    (gf.now)
{
   objectTypeNames.push_back("GravityField");
   bodyName = gf.bodyName;


   parameterCount = GravityFieldParamCount;

   derivativeIds.push_back(Gmat::CARTESIAN_STATE);
   derivativeIds.push_back(Gmat::ORBIT_STATE_TRANSITION_MATRIX);
   derivativeIds.push_back(Gmat::ORBIT_A_MATRIX);
}


//------------------------------------------------------------------------------
//  GravityField& operator=(const GravityField & gf)
//------------------------------------------------------------------------------
/**
 * This method sets the 'this' object to the value(s) of the input
 * object.
 *
 * @param <gf> object whose values to copy to this object.
 */
//------------------------------------------------------------------------------
GravityField& GravityField::operator=(const GravityField &gf)
{
   if (&gf == this)
      return *this;

   HarmonicField::operator=(gf);
   mu                     = gf.mu;
   a                      = gf.a;
   earthTideModel         = gf.earthTideModel;
   defaultMu              = gf.defaultMu;
   defaultA               = gf.defaultA;
   bodyName               = gf.bodyName;
   gfInitialized          = false;  // is that what I want to do?
   orderTruncateReported  = gf.orderTruncateReported;
   degreeTruncateReported = gf.degreeTruncateReported;
//   if ((gravityModel) && (gravityModel->GetFilename() == "")) delete gravityModel;  // delete only Body ones
   gravityModel           = gf.gravityModel;
//   gravityModel           = NULL;
   frv                    = gf.frv;
   trv                    = gf.trv;
   now                    = gf.now;

   return *this;
}


//------------------------------------------------------------------------------
//  bool Initialize()
//------------------------------------------------------------------------------
/**
 * This method initializes the object.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::Initialize()
{
   if (gfInitialized && hMinitialized) return true;
   if (!HarmonicField::Initialize())
   {
      throw ODEModelException(
            "GravityField: Legendre Polynomial initialization failed!");
   }

   degreeTruncateReported = false;
   orderTruncateReported  = false;

   #ifdef DEBUG_FIRST_CALL
      firstCallFired = false;
   #endif
   
   // JPD code added here
   if (!body)
   {
      body = solarSystem->GetBody(bodyName);
      if (!body) throw ODEModelException("Body \"" + bodyName + "\" undefined for Gravity Field.");
   }
   
//   // The required distance from the origin to avoid numerical instability, this
//   // value is compared to the L1 distance norm in GetDerivatives to make sure
//   // that the offset is acceptable.  This piece is currently disabled
//   minimumDistance = body->GetEquatorialRadius() * 0.5;

   if (body->GetName() != fixedCS->GetOriginName()) // ********** is this the right CS to use???
      throw ODEModelException("Full field gravity is only supported for "
                              "the force model origin in current GMAT builds.");
   if ((!fileRead) || (!gravityModel))
   {
      Integer fileDegree = 0;
      Integer fileOrder  = 0;
      mu = body->GetGravitationalConstant();
      a  = body->GetEquatorialRadius();
      if (IsBlank(filename.c_str()))
      {
         std::string errmsg = "There is no gravity file specified for GravityField ";
         errmsg += instanceName + "\n";
         throw ODEModelException(errmsg);
      }
      else
      {
         #ifdef DEBUG_GRAVITY_FILE_READ
            MessageInterface::ShowMessage("Now getting HarmonicGravity with filename = %s, a = %12.10f, mu = %12.10f\n",
                  filename.c_str(), a, mu);
         #endif
         gravityModel = GetGravityFile(filename,a,mu);
         if (!gravityModel)
         {
            std::string errmsg = "Gravity file ";
            errmsg += filename + " cannot be opened or read.\n";
            throw ODEModelException(errmsg);
         }
         else
         {
            #ifdef DEBUG_GRAVITY_FILE_READ
               MessageInterface::ShowMessage("--- HarmonicGravity object retrieved is <%p>\n", gravityModel);
            #endif
            fileDegree =   gravityModel->GetNN();
            fileOrder  =   gravityModel->GetMM();
            mu         = - gravityModel->GetFactor();
            a          =   gravityModel->GetRadius();
         }
      }
      // truncate the degree and/or order, if necessary
      #ifdef DEBUG_DEGREE_ORDER
         MessageInterface::ShowMessage("for body %s, fileDegree = %d, degree = %d\n",
               (body->GetName()).c_str(), fileDegree, degree);
         MessageInterface::ShowMessage("for body %s, fileOrder  = %d, order  = %d\n",
               (body->GetName()).c_str(), fileDegree, degree);
      #endif
      if (fileDegree < degree)   // for now, do not truncate ...
      {
         std::stringstream ss("");
         ss << "In GravityField " << instanceName << ", requested degree (";
         ss << degree << ") is greater than degree (" << fileDegree;
         ss << ") on file " << filename << std::endl;
         throw ODEModelException(ss.str());
      }
      if (fileOrder < order)
      {
         std::stringstream ss("");
         ss << "In GravityField " << instanceName << ", requested order (";
         ss << order << ") is greater than order (" << fileOrder;
         ss << ") on file " << filename << std::endl;
         throw ODEModelException(ss.str());
      }
      if (order > degree)
      {
		 std::stringstream ss("");
         ss << "*** WARNING *** In " + GetName();
		 ss << " the Order (" 
         << order 
         << ") is greater than Degree ("
         << degree 
         << ") so truncating Order to ";
         MessageInterface::ShowMessage("%s %d\n",ss.str().c_str(), degree);
		 order = degree;
      }
      if (degree < 0)
         throw ODEModelException("Invalid degree in GravityField: Degree < 0\n");
      if (order < 0)
         throw ODEModelException("Invalid order in GravityField: Degree < 0\n");
      fileRead = true;
   }
   #ifdef DEBUG_BODY_DATA
      MessageInterface::ShowMessage("for body %s, mu = %12.10f, a = %12.10f\n",
                                   (body->GetName()).c_str(), mu, a);
   #endif

   gfInitialized = true;
   return true;
}




//------------------------------------------------------------------------------
//  bool GetDerivatives(Real * state, Real dt, int dvorder)
//------------------------------------------------------------------------------
/**
* This method initializes the gravity parameters.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::GetDerivatives(Real * state, Real dt, Integer dvorder,
      const Integer id)
{
   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
         MessageInterface::ShowMessage(
            "GravityField(%s) inputs:\n"
            "   state = [%.10lf %.10lf %.10lf %.16lf %.16lf %.16lf]\n"
            "   dt = %.10lf\n   dvorder = %d\n",
            instanceName.c_str(), state[0], state[1], state[2], state[3],
            state[4], state[5], dt, dvorder);
      }
   #endif

   // We may want to do this down the road:
//   if (fabs(state[0]) + fabs(state[1]) + fabs(state[2]) < minimumDistance)
//      throw ODEModelException("A harmonic gravity field is being computed "
//            "inside of the " + bodyName + ", which is not allowed");

   if ((dvorder > 2) || (dvorder < 1))
      return false;

   #ifdef DEBUG_GRAVITY_FIELD
      MessageInterface::ShowMessage("%s %d %s %le %s  %le %le %le %le %le %le\n",
          "Entered GravityField::GetDerivatives with order", dvorder, "dt = ",
          dt, "and state\n",
          state[0], state[1], state[2], state[3], state[4], state[5]);
      MessageInterface::ShowMessage("cartesianCount = %d, stmCount = %d, aMatrixCount = %d\n",
            cartesianCount, stmCount, aMatrixCount);
      MessageInterface::ShowMessage("fillCartesian = %s, fillSTM = %s, fillAMatrix = %s\n",
            (fillCartesian? "true" : "false"), (fillSTM? "true" : "false"), (fillAMatrix? "true" : "false"));
      MessageInterface::ShowMessage("cartesianStart = %d, stmStart = %d, aMatrixStart = %d\n",
            cartesianStart, stmStart, aMatrixStart);
   #endif

/// @todo Optimize this code -- May be possible to make GravityField calculations more efficient


   if ((cartesianCount < 1)  && (stmCount < 1) && (aMatrixCount < 1))
      throw ODEModelException(
         "GravityField requires at least one spacecraft.");

   // todo: Move into header; this flag is used to decide if the velocity terms
   // are copied into the position derivatives for first order integrators, so
   // when the GravityField is set to work at non-central bodies, the detection
   // will need to happen in initialization.
   Real satState[6];
   Integer nOffset;

   now = epoch + dt/GmatTimeConstants::SECS_PER_DAY;

   #ifdef DEBUG_GRAV_COORD_SYSTEM
       MessageInterface::ShowMessage(
         "------ body = %s\n------ inputCS = %s\n------ targetCS = %s"
         "\n------ fixedCS = %s\n",
         body->GetName().c_str(),     (inputCS == NULL? "NULL" : inputCS->GetName().c_str()),
         (targetCS == NULL? "NULL" : targetCS->GetName().c_str()), (fixedCS == NULL? "NULL" : fixedCS->GetName().c_str()));
   #endif



   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
    	 CelestialBody *targetBody = (CelestialBody*) targetCS->GetOrigin();
    	 CelestialBody *fixedBody  = (CelestialBody*) fixedCS->GetOrigin();
         MessageInterface::ShowMessage(
            "   Epoch = %.12lf\n   targetBody = %s\n   fixedBody = %s\n",
            now.Get(), targetBody->GetName().c_str(),
            fixedBody->GetName().c_str());
         MessageInterface::ShowMessage(
            "------ body = %s\n------ inputCS = %s\n------ targetCS = %s\n"
            "------ fixedCS = %s\n",
            body->GetName().c_str(),     inputCS->GetName().c_str(),
            targetCS->GetName().c_str(), fixedCS->GetName().c_str());
      }
   #endif


   if (fillCartesian || fillAMatrix || fillSTM)
   {
      // See assumption 1, above
      if ((cartesianCount < stmCount) || (cartesianCount < aMatrixCount))
      {
         throw ODEModelException("GetDerivatives: cartesianCount < stmCount or aMatrixCount\n");
      }
      Real originacc[3] = { 0.0,0.0,0.0 };  // JPD code
      Rmatrix33 origingrad (0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
      Rmatrix33 emptyGradient(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
      Rmatrix33 gradnew (0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
      if (body != forceOrigin)
      {
         Real originstate[6] = { 0.0,0.0,0.0,0.0,0.0,0.0 };
         Calculate(dt,originstate,originacc,origingrad);
#ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("---------> origingrad = %s\n", origingrad.ToString().c_str());
#endif
      }

      for (Integer n = 0; n < cartesianCount; ++n)
      {
         nOffset = cartesianStart + n * stateSize;
         for (Integer i = 0; i < 6; ++i)
            satState[i] = state[i+nOffset];

         Real accnew[3];  // JPD code
         gradnew = emptyGradient;
         Calculate(dt,satState,accnew,gradnew);
         if (body != forceOrigin)
         {
            for (Integer i=0;  i<=2;  ++i)
               accnew[i] -= originacc[i];
            gradnew -= origingrad;
#ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("---------> body not equal to forceOrigin\n");
#endif
         }
#ifdef DEBUG_DERIVATIVES
      MessageInterface::ShowMessage("---------> gradnew (%d) = %s\n", n, gradnew.ToString().c_str());
#endif
         
         // Fill Derivatives
         switch (dvorder)
         {
            case 1:
               deriv[0+nOffset] = satState[3];
               deriv[1+nOffset] = satState[4];
               deriv[2+nOffset] = satState[5];
               deriv[3+nOffset] = accnew[0];
               deriv[4+nOffset] = accnew[1];
               deriv[5+nOffset] = accnew[2];
               break;

            case 2:
               deriv[0+nOffset] = accnew[0];
               deriv[1+nOffset] = accnew[1];
               deriv[2+nOffset] = accnew[2];
               deriv[3+nOffset] = 0.0;
               deriv[4+nOffset] = 0.0;
               deriv[5+nOffset] = 0.0;
               break;
         }

#ifdef DEBUG_DERIVATIVES
         for (Integer ii = 0 + nOffset; ii < 6+nOffset; ii++)
                     MessageInterface::ShowMessage("------ deriv[%d] = %12.10f\n", ii, deriv[ii]);
#endif
         if (fillSTM)
         {
            Real aTilde[36];
            Integer element;
            // @todo Add the use of the GetAssociateIndex() method here to get index into state array
            //       (See assumption 1, above)
            if (n <= stmCount)
            {
               Integer i6 = stmStart + n * 36;

               // Calculate A-tilde
               aTilde[ 0] = aTilde[ 1] = aTilde[ 2] =
               aTilde[ 3] = aTilde[ 4] = aTilde[ 5] =
               aTilde[ 6] = aTilde[ 7] = aTilde[ 8] =
               aTilde[ 9] = aTilde[10] = aTilde[11] =
               aTilde[12] = aTilde[13] = aTilde[14] =
               aTilde[15] = aTilde[16] = aTilde[17] =
               aTilde[21] = aTilde[22] = aTilde[23] =
               aTilde[27] = aTilde[28] = aTilde[29] =
               aTilde[33] = aTilde[34] = aTilde[35] = 0.0;

               // fill in the lower left quadrant with the calculated gradient values
               aTilde[18] = gradnew(0,0);
               aTilde[19] = gradnew(0,1);
               aTilde[20] = gradnew(0,2);
               aTilde[24] = gradnew(1,0);
               aTilde[25] = gradnew(1,1);
               aTilde[26] = gradnew(1,2);
               aTilde[30] = gradnew(2,0);
               aTilde[31] = gradnew(2,1);
               aTilde[32] = gradnew(2,2);

               for (Integer j = 0; j < 6; j++)
               {
                  for (Integer k = 0; k < 6; k++)
                  {
                     element = j * 6 + k;
#ifdef DEBUG_DERIVATIVES
                     MessageInterface::ShowMessage("------ deriv[%d] = %12.10f\n", (i6+element), aTilde[element]);
#endif
                     deriv[i6+element] = aTilde[element];
                  }
               }
            }
         }

         if (fillAMatrix)
         {
            Real aTilde[36];
            Integer element;
            // @todo Add the use of the GetAssociateIndex() method here to get index into state array
            //       (See assumption 1, above)
            if (n <= aMatrixCount)
            {
               Integer i6 = aMatrixStart + n * 36;

               // Calculate A-tilde
               aTilde[ 0] = aTilde[ 1] = aTilde[ 2] =
               aTilde[ 3] = aTilde[ 4] = aTilde[ 5] =
               aTilde[ 6] = aTilde[ 7] = aTilde[ 8] =
               aTilde[ 9] = aTilde[10] = aTilde[11] =
               aTilde[12] = aTilde[13] = aTilde[14] =
               aTilde[15] = aTilde[16] = aTilde[17] =
               aTilde[21] = aTilde[22] = aTilde[23] =
               aTilde[27] = aTilde[28] = aTilde[29] =
               aTilde[33] = aTilde[34] = aTilde[35] = 0.0;

               // fill in the lower left quadrant with the calculated gradient values
               aTilde[18] = gradnew(0,0);
               aTilde[19] = gradnew(0,1);
               aTilde[20] = gradnew(0,2);
               aTilde[24] = gradnew(1,0);
               aTilde[25] = gradnew(1,1);
               aTilde[26] = gradnew(1,2);
               aTilde[30] = gradnew(2,0);
               aTilde[31] = gradnew(2,1);
               aTilde[32] = gradnew(2,2);

               for (Integer j = 0; j < 6; j++)
               {
                  for (Integer k = 0; k < 6; k++)
                  {
                     element = j * 6 + k;
#ifdef DEBUG_DERIVATIVES
                     MessageInterface::ShowMessage("------ deriv[%d] = %12.10f\n", (i6+element), aTilde[element]);
#endif
                     deriv[i6+element] = aTilde[element];
                  }
               }
            }
         }

      }  // end for
   }

   #ifdef DEBUG_FIRST_CALL
      if (firstCallFired == false)
      {
         if (body->GetName() == "Mars")
         {
         MessageInterface::ShowMessage(
            "   GravityField[%s <> %s] --> mu = %lf, origin = %s, [%.10lf %.10lf "
            "%.10lf %.16lf %.16lf %.16lf]\n",
            instanceName.c_str(), body->GetName().c_str(), mu,
            targetCS->GetOriginName().c_str(),
            deriv[0], deriv[1], deriv[2], deriv[3], deriv[4], deriv[5]);
         firstCallFired = true;
         }
      }
   #endif

   return true;
}


//------------------------------------------------------------------------------
// Rvector6 GetDerivativesForSpacecraft(Spacecraft *sc)
//------------------------------------------------------------------------------
/**
 * Retrieves the Cartesian state vector of derivatives w.r.t. time
 *
 * @param sc The spacecraft that holds the state vector
 *
 * @return The derivative vector
 */
//------------------------------------------------------------------------------
Rvector6 GravityField::GetDerivativesForSpacecraft(Spacecraft *sc)
{
   Rvector6 dv;
   Real satState[6];

   Real theEpoch = epoch;
   Real *j2kState = sc->GetState().GetState();
   epoch = sc->GetEpoch();
   BuildModelState(epoch, satState, j2kState);

   Real originacc[3] = {0.0, 0.0, 0.0};  // JPD code
   Rmatrix33 origingrad (0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
   Rmatrix33 emptyGradient(0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);
   Rmatrix33 gradnew (0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0);

   if (body != forceOrigin)
   {
      Real originstate[6] = { 0.0,0.0,0.0,0.0,0.0,0.0 };
      Calculate(0.0, originstate, originacc, origingrad);
   }

   Real accnew[3];  // JPD code
   gradnew = emptyGradient;
   Calculate(0.0, satState, accnew, gradnew);

   for (Integer i=0;  i<=2;  ++i)
      accnew[i] -= originacc[i];
   gradnew -= origingrad;

   // Fill Derivatives
   dv[0] = satState[3];
   dv[1] = satState[4];
   dv[2] = satState[5];
   dv[3] = accnew[0];
   dv[4] = accnew[1];
   dv[5] = accnew[2];

   #ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("scscsc deriv = [");
      for (Integer ii = 0; ii < 6; ++ii)
      {
         if (ii > 0)
            MessageInterface::ShowMessage(", ");
         MessageInterface::ShowMessage("%12.10lf", dv[ii]);
      }
      MessageInterface::ShowMessage("]\n", ii, dv[ii]);
   #endif

   #ifdef DEBUG_FIRST_CALL
      if (!firstCallFired)
         MessageInterface::ShowMessage("Drag Accel: [%le %le %le]\n", dv[3],
               dv[4], dv[5]);
   #endif

   epoch = theEpoch;
   return dv;
}


//------------------------------------------------------------------------------
//  bool GetBodyAndMu(std::string &itsName, Real &itsMu)
//------------------------------------------------------------------------------
/**
* This method returns the body name and mu value (from the gravity file).
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::GetBodyAndMu(std::string &itsName, Real &itsMu)
{
   itsName = bodyName;
   itsMu   = mu;
   return true;
}

//---------------------------------
// inherited methods from GmatBase
//---------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the GravityField.
 *
 * @return clone of the GravityField.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GravityField::Clone(void) const
{
   return (new GravityField(*this));
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string GravityField::GetParameterText(const Integer id) const
{
   if ((id >= HarmonicFieldParamCount) && (id < GravityFieldParamCount))
      return PARAMETER_TEXT[id - HarmonicFieldParamCount];
   return HarmonicField::GetParameterText(id);
}

//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Integer     GravityField::GetParameterID(const std::string &str) const
{
   for (Integer i = HarmonicFieldParamCount; i < GravityFieldParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - HarmonicFieldParamCount])
         return i;
   }
   return HarmonicField::GetParameterID(str);
}

//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType
GravityField::GetParameterType(const Integer id) const
{
   if ((id >= HarmonicFieldParamCount) && (id < GravityFieldParamCount))
      return PARAMETER_TYPE[id - HarmonicFieldParamCount];
   return HarmonicField::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
* @see GmatBase
 */
//------------------------------------------------------------------------------
std::string GravityField::GetParameterTypeString(const Integer id) const
{
   return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
}



// All read only for now except degree and order
bool GravityField::IsParameterReadOnly(const Integer id) const
{
   if (id < HarmonicFieldParamCount)
      return HarmonicField::IsParameterReadOnly(id);

   if ((id == EARTH_TIDE_MODEL) && (bodyName == GmatSolarSystemDefaults::EARTH_NAME)) return false;

   return true;
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param id    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real        GravityField::GetRealParameter(const Integer id) const
{
   if (id == MU)       return mu;
   if (id == A)        return a;

   return HarmonicField::GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 */
//------------------------------------------------------------------------------
Real        GravityField::SetRealParameter(const Integer id,
                                           const Real value)
{
   if (id == MU)       return (mu     = value);
   if (id == A)        return (a      = value);

   return HarmonicField::SetRealParameter(id, value);
}


//------------------------------------------------------------------------------
// Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
* Accessor method used to obtain a parameter value
 *
 * @param label    string ID for the requested parameter
 */
//------------------------------------------------------------------------------
Real        GravityField::GetRealParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetRealParameter(id);
}

//------------------------------------------------------------------------------
// Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    label    string ID for the requested parameter
 * @param    value    The new value for the parameter
 */
//------------------------------------------------------------------------------
Real GravityField::SetRealParameter(const std::string &label,
                                    const Real value)
{
   Integer id = GetParameterID(label);
   return SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param id    Integer ID for the requested parameter
 */
//------------------------------------------------------------------------------
std::string GravityField::GetStringParameter(const Integer id) const
{
   if (id == EARTH_TIDE_MODEL) return earthTideModel;
   return HarmonicField::GetStringParameter(id);
}

//------------------------------------------------------------------------------
// Real SetStringParameter(const Integer id, const std::string value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    id  Integer ID for the parameter
 * @param    val The new value for the parameter
 */
//------------------------------------------------------------------------------
bool GravityField::SetStringParameter(const Integer id,
                                      const std::string &value)
{
   if (id == EARTH_TIDE_MODEL)
   {
      if (bodyName == GmatSolarSystemDefaults::EARTH_NAME)
      {
         if ((GmatStringUtil::ToUpper(value) != "NONE") &&      // Currently, these are the only valid strings ...
             (GmatStringUtil::ToUpper(value) != "SOLIDANDPOLE"))
         {
            ODEModelException ome;
            ome.SetDetails(errorMessageFormat.c_str(),
                           value.c_str(), "EarthTideModel", "\'None\' or \'SolidAndPole\'");
            throw ome;
         }
         earthTideModel = value;
      }
      else
      {
         MessageInterface::ShowMessage
            ("*** WARNING *** \"EarthTideModel\" is deprecated for non-Earth primary bodies and will be "
             "removed from a future build.\n");
      }
      return true;
   }
   return HarmonicField::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain a parameter value
 *
 * @param label    string ID for the requested parameter
 */
//------------------------------------------------------------------------------
std::string GravityField::GetStringParameter(const std::string &label) const
{
   Integer id = GetParameterID(label);
   return GetStringParameter(id);
}

//------------------------------------------------------------------------------
// std::string SetStringParameter(const std::string &label, const std::string value)
//------------------------------------------------------------------------------
/**
 * Accessor method used to set a parameter value
 *
 * @param    label    string ID for the requested parameter
 * @param    value    The new value for the parameter
 */
//------------------------------------------------------------------------------
bool GravityField::SetStringParameter(const std::string &label,
                                      const std::string &value)
{
   Integer id = GetParameterID(label);
   return SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// bool SupportsDerivative(Gmat::StateElementId id)
//------------------------------------------------------------------------------
/**
 * Function used to determine if the physical model supports derivative
 * information for a specified type.
 *
 * @param id State Element ID for the derivative type
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool GravityField::SupportsDerivative(Gmat::StateElementId id)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage(
            "GravityField checking for support for id %d\n", id);
   #endif

   if (id == Gmat::CARTESIAN_STATE)
      return true;

   if (id == Gmat::ORBIT_STATE_TRANSITION_MATRIX)
      return true;

   if (id == Gmat::ORBIT_A_MATRIX)
      return true;

   return PhysicalModel::SupportsDerivative(id);
}


//------------------------------------------------------------------------------
// bool SetStart(Gmat::StateElementId id, Integer index, Integer quantity)
//------------------------------------------------------------------------------
/**
 * Function used to set the start point and size information for the state
 * vector, so that the derivative information can be placed in the correct place
 * in the derivative vector.
 *
 * @param id State Element ID for the derivative type
 * @param index Starting index in the state vector for this type of derivative
 * @param quantity Number of objects that supply this type of data
 *
 * @return true if the type is supported, false otherwise.
 */
//------------------------------------------------------------------------------
bool GravityField::SetStart(Gmat::StateElementId id, Integer index,
                      Integer quantity)
{
   #ifdef DEBUG_REGISTRATION
      MessageInterface::ShowMessage("GravityFiels setting start data for id = "
            "%d to index %d; %d objects identified\n", id, index, quantity);
   #endif

   bool retval = false;

   switch (id)
   {
      case Gmat::CARTESIAN_STATE:
//         satCount       = quantity;
         cartesianCount = quantity;
         cartesianStart = index;
         fillCartesian  = true;
         retval         = true;
         break;

      case Gmat::ORBIT_STATE_TRANSITION_MATRIX:
         stmCount       = quantity;
         stmStart       = index;
         fillSTM        = true;
         retval         = true;
         break;

      case Gmat::ORBIT_A_MATRIX:
         aMatrixCount   = quantity;
         aMatrixStart   = index;
         fillAMatrix    = true;
         retval         = true;
         break;

      default:
         break;
   }

   return retval;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  bool IsBlank(char* aLine)
//------------------------------------------------------------------------------
/**
 * This method returns true if the string is empty or is all white space.
 *
 * @return success flag.
 */
//------------------------------------------------------------------------------
bool GravityField::IsBlank(char* aLine)
{
   Integer i;
   for (i=0;i<(int)strlen(aLine);i++)
   {
      //loj: 5/18/04 if (!isblank(aLine[i])) return false;
      if (!isspace(aLine[i])) return false;
   }
   return true;
}

bool GravityField::IsBlank(const std::string &aLine)
{
   Integer i;
   for (i=0;i<(int)aLine.length();i++)
   {
      if (!isspace(aLine.at(i))) return false;
   }
   return true;
}



// ********** JPD added these ***********
//--------------------------------------------------------------------
void GravityField::InverseRotate (Rmatrix33& rot, const Real in[3], Real out[3])
{
   const Real *rm = rot.GetDataVector();
   const Real  rmt[9] = {rm[0], rm[3], rm[6],
      rm[1], rm[4], rm[7],
      rm[2], rm[5], rm[8]};
   for (Integer p = 0; p < 3; ++p)
   {
      Integer p3 = 3*p;
      out[p] = rmt[p3]   * in[0] +
      rmt[p3+1] * in[1] +
      rmt[p3+2] * in[2];
   }
   
}
//------------------------------------------------------------------------------
void GravityField::Calculate (Real dt, Real state[6], 
                              Real acc[3], Rmatrix33& grad)
{
   #ifdef DEBUG_CALCULATE
      MessageInterface::ShowMessage(
            "Entering Calculate with dt = %12.10f, state = %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            dt, state[0], state[1], state[2], state[3], state[4], state[5]);
      MessageInterface::ShowMessage("   acc = %12.10f  %12.10f  %12.10f\n", acc[0], acc[1], acc[2]);
   #endif
   Real jday = epoch + GmatTimeConstants::JD_JAN_5_1941 +
               dt/GmatTimeConstants::SECS_PER_DAY;
   // convert to body fixed coordinate system
   Real now = epoch + dt/GmatTimeConstants::SECS_PER_DAY;
   Real tmpState[6];
//   CoordinateConverter cc; - move back to class, for performance
   cc.Convert(now, state, inputCS, tmpState, fixedCS);  // which CSs to use here???
   #ifdef DEBUG_CALCULATE
      MessageInterface::ShowMessage(
            "After Convert, jday = %12.10f, now = %12.10f, and tmpState = %12.10f  %12.10f  %12.10f  %12.10f  %12.10f  %12.10f\n",
            jday, now, tmpState[0], tmpState[1], tmpState[2], tmpState[3], tmpState[4], tmpState[5]);
   #endif
   Rmatrix33 rotMatrix = cc.GetLastRotationMatrix();
#ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("---->>>> rotMatrix = %s\n", rotMatrix.ToString().c_str());
#endif
   // calculate sun and moon pos
   Real sunpos[3]   = {0.0,0.0,0.0};
   Real moonpos[3]  = {0.0,0.0,0.0};
   Real sunMass     = 0.0;
   Real moonMass    = 0.0;
   // Acceleration
   Real      rotacc[3];
   Rmatrix33 rotgrad;
   bool      useTides;
   // for now, "None" and "SolidAndPole" are the only valid EarthTideModel values
   if ((bodyName == GmatSolarSystemDefaults::EARTH_NAME) && (GmatStringUtil::ToUpper(earthTideModel) == "SOLIDANDPOLE"))
   {
      Real ep = epoch + dt / GmatTimeConstants::SECS_PER_DAY;  // isn't this the same as now?
      CelestialBody* theSun  = solarSystem->GetBody(SolarSystem::SUN_NAME);
      CelestialBody* theMoon = solarSystem->GetBody(SolarSystem::MOON_NAME);
      if (!theSun || !theMoon)
         throw ODEModelException("Solar system does not contain the Sun or Moon for Tide model.");
      Rvector6 sunstateinertial  = theSun->GetState(ep);
      Rvector6 moonstateinertial = theMoon->GetState(ep);
      
      Rvector6 sunstate;
      Rvector6 moonstate;
      cc.Convert(now, sunstateinertial, inputCS, sunstate, fixedCS);
      cc.Convert(now, moonstateinertial, inputCS, moonstate, fixedCS);
      sunstate.GetR(sunpos);
      moonstate.GetR(moonpos);

      sunMass     = theSun->GetMass();
      moonMass    = theMoon->GetMass();
      useTides = true;
   }
   else
      useTides = false;
   #ifdef DEBUG_GRAVITY_EARTH_TIDE
      MessageInterface::ShowMessage("Calling gravityModel->CalculateFullField with useTides = %s\n",
            (useTides? "true" : "false"));
   #endif
   // Get xp and yp from the EOP file
   Real xp, yp, lod;
   Real utcmjd  = TimeConverterUtil::Convert(now, TimeConverterUtil::A1MJD, TimeConverterUtil::UTCMJD,
                 GmatTimeConstants::JD_JAN_5_1941);
   eop->GetPolarMotionAndLod(utcmjd, xp, yp, lod);
   bool computeMatrix = fillAMatrix || fillSTM;

   gravityModel->CalculateFullField(jday, tmpState, degree, order, useTides, sunpos, moonpos, sunMass, moonMass,
                                    xp, yp, computeMatrix, rotacc, rotgrad);
#ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("after CalculateFullField, rotgrad = %s\n", rotgrad.ToString().c_str());
#endif
   /*
    MessageInterface::ShowMessage
    ("HarmonicField::Calculate pos= %20.14f %20.14f %20.14f\n",
    tmpState[0],tmpState[1],tmpState[2]);
    MessageInterface::ShowMessage
    ("HarmonicField::Calculate grad= %20.14e %20.14e %20.14e\n",
    rotgrad(0,0),rotgrad(0,1),rotgrad(0,2));
    MessageInterface::ShowMessage
    ("HarmonicField::Calculate grad= %20.14e %20.14e %20.14e\n",
    rotgrad(1,0),rotgrad(1,1),rotgrad(1,2));
    MessageInterface::ShowMessage
    ("HarmonicField::Calculate grad= %20.14e %20.14e %20.14e\n",
    rotgrad(2,0),rotgrad(2,1),rotgrad(2,2));
    */
   
   // Convert back to target CS
   InverseRotate (rotMatrix,rotacc,acc);
   grad = rotMatrix.Transpose() * rotgrad * rotMatrix;
#ifdef DEBUG_DERIVATIVES
   MessageInterface::ShowMessage("at end of Calculate, after rotation, grad = %s\n", grad.ToString().c_str());
#endif
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
HarmonicGravity* GravityField::GetGravityFile(const std::string &filename,
                                              const Real &radius, const Real &mukm)
{
   for (std::vector<HarmonicGravity*>::iterator objptPos = cache.begin();
        objptPos != cache.end(); ++objptPos)
   {
      if ((*objptPos)->GetFilename() == filename)
      {
         return (*objptPos);
      }
   }
   HarmonicGravity         *newOne  = NULL;
   GmatGrav::GravityFileType fileType = GravityFileUtil::GetFileType(filename);
   switch (fileType)
   {
      case GmatGrav::GFT_COF:
         newOne = new HarmonicGravityCof(filename,radius,mukm);
         break;
      case GmatGrav::GFT_GRV:
         newOne = new HarmonicGravityGrv(filename,radius,mukm);
         break;
      case GmatGrav::GFT_UNKNOWN:
         throw ODEModelException
            ("GravityField::Create file not found or incorrect type\n");
      default:
         return NULL;
   }

   if (newOne)
   {
      cache.push_back(newOne);
   }
   #ifdef DEBUG_CREATE_DELETE
      MessageInterface::ShowMessage(">>>> Just created a HarmonicGravity file object <%p> for filename %s\n",
            newOne, filename.c_str());
      MessageInterface::ShowMessage("cache pointers and filenames are: \n");
      for (unsigned int ii = 0; ii < cache.size(); ii++)
      {
         MessageInterface::ShowMessage("    <%p>     %s\n", cache.at(ii), ((cache.at(ii))->GetFilename()).c_str());
      }
   #endif
   return newOne;
}


