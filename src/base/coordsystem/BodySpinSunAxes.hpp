//$Id: BodySpinSunAxes.hpp 10276 2012-09-17 16:41:37Z tdnguyen $
//------------------------------------------------------------------------------
//                                  BodySpinSunAxes
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
// Author: Tuan Dang Nguyen/GSFC
// Created: 2012/09/17
//
/**
 * Definition of the BodySpinSunAxes class.
 *
 */
//------------------------------------------------------------------------------

#ifndef BodySpinSunAxes_hpp
#define BodySpinSunAxes_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "AxisSystem.hpp"
#include "DynamicAxes.hpp"
#include "ObjectReferencedAxes.hpp"

class GMAT_API BodySpinSunAxes : public ObjectReferencedAxes
{
public:

   // default constructor
   BodySpinSunAxes(const std::string &itsName = "");
   // copy constructor
   BodySpinSunAxes(const BodySpinSunAxes &bss);
   // operator = for assignment
   const BodySpinSunAxes& operator=(const BodySpinSunAxes &bss);
   // destructor
   virtual ~BodySpinSunAxes();

   virtual GmatCoordinate::ParameterUsage UsesXAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesYAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesZAxis() const;
   virtual GmatCoordinate::ParameterUsage UsesEopFile(const std::string &forBaseSystem = "FK5") const;
   virtual GmatCoordinate::ParameterUsage UsesItrfFile() const;
   virtual GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const;

   // method to initialize the data
   virtual bool Initialize();

   // all classes derived from GmatBase must supply this Clone method;
   virtual GmatBase*       Clone() const;

/*
   // 5 following functions are rewriten in order to apply for any celestial bodies without
   // precession, nutaion, polar motion, and rotation speed different than earth.
   virtual void ComputePrecessionMatrix(const Real tTDB, A1Mjd atEpoch);
   virtual void ComputeNutationMatrix(const Real tTDB, A1Mjd atEpoch, 
                                           Real &dPsi,
                                           Real &longAscNodeLunar,
                                           Real &cosEpsbar,
                                           bool forceComputation = false);
   virtual void ComputeSiderealTimeRotation(const Real jdTT,
                                                 const Real tUT1,
                                                 Real dPsi,
                                                 Real longAscNodeLunar,
                                                 Real cosEpsbar,
                                                 Real &cosAst,
                                                 Real &sinAst);
   virtual void ComputeSiderealTimeDotRotation(const Real mjdUTC, A1Mjd atEpoch,
                                                    Real cosAst, Real sinAst,
                                                    bool forceComputation = false);
   virtual void ComputePolarMotionRotation(const Real mjdUTC, A1Mjd atEpoch,
                                            bool forceComputation = false);
*/
    
protected:
      
   enum
   {
      BodySpinSunAxesParamCount = ObjectReferencedAxesParamCount
   };


   Rvector3         spinaxisBF;
   const Real       *spinBF;
   Rvector6         rvSunVec;
   const Real       *rvSun;
   
   virtual void CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation = false);
   
};
#endif // BodySpinSunAxes_hpp
