//------------------------------------------------------------------------------
//                              UserFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2015.07.16
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef UserFunction_hpp
#define UserFunction_hpp

#include "csaltdefs.hpp"
#include "ScalingUtility.hpp"

class CSALT_API UserFunction
{
public:
   // save all the enumerations here. YK 2018-05-02
   enum FunctionType { DYNAMICS, COST, ALGEBRAIC, ALLFUNCTIONS };
   enum JacobianType { STATE, CONTROL, TIME, STATIC, ALLJACOBIANS };
   enum FunctionBound { UPPER, LOWER };


   UserFunction();
   UserFunction(const UserFunction &copy);
   UserFunction& operator=(const UserFunction &copy);
   virtual ~UserFunction();
   
   virtual void           SetIsInitializing(bool isInit);
   
   virtual bool           IsInitializing();
   
   // leaf class(es) must implement these abstract methods
   virtual void           EvaluateFunctions() = 0;
   virtual void           EvaluateJacobians() = 0;
   virtual void           SetPhaseScaleUtilList(std::vector<ScalingUtility*> scalingList);
   virtual void           SetStatePerturbation(Real statePertIn);
   virtual void           SetTimePerturbation(Real timePertIn);
   virtual void           SetControlPerturbation(Real controlPertIn);
   virtual Real           GetStatePerturbation();
   virtual Real           GetTimePerturbation();
   virtual Real           GetControlPerturbation();
   virtual Real           GetStaticPerturbation();
   
protected:
   
   /// number of alg functions
   Integer numAlgFunctions;
   /// indicates if user function has algebraic functions
   bool    hasAlgFunctions;
   /// indicates if there is a path function portion to cost
   bool    hasCostFunction;
   /// Is it initializing?
   bool    isInitializing;
   /// Perturbation for state finite differencing
   Real statePert;
   /// Perturbation for time finite differencing
   Real timePert;
   /// Perturbation for control finite differencing
   Real controlPert;
   /// Perturbation for static finite differencing
   Real staticPert;
   /// List of scaling utility objects from each phase
   std::vector<ScalingUtility*> phaseScaleUtilList;
};
#endif // UserFunction_hpp
