//------------------------------------------------------------------------------
//                              UserPointFunction
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan / NASA
// Created: 2015.08.14
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#ifndef UserPointFunction_hpp
#define UserPointFunction_hpp

#include "csaltdefs.hpp"
#include "UserFunction.hpp"
#include "FunctionInputData.hpp"
#include "PointFunctionContainer.hpp"
#include "OptimalControlFunction.hpp"

class CSALT_API UserPointFunction : public UserFunction
{
public:
   /* 
   enum FunctionType { COST, ALGEBRAIC };
   enum JacobianType { STATE, TIME };
   enum FunctionBound { UPPER, LOWER };
   */

   UserPointFunction();
   UserPointFunction(const UserPointFunction &copy);
   UserPointFunction& operator=(const UserPointFunction &copy);
   virtual ~UserPointFunction();
   
   virtual void    Initialize(const std::vector<FunctionInputData*> &initData,
                              const std::vector<FunctionInputData*> &finalData);
   
   virtual void    AddFunctions(std::vector<OptimalControlFunction*> funcList);

   virtual bool    EvaluateUserFunction();
   virtual bool    EvaluateUserJacobian();
   
   virtual bool    ValidatePhaseIndex(Integer idx);
   
   virtual void    SetFunctionData(PointFunctionContainer *toData);
   
   virtual PointFunctionContainer*
                   GetFunctionData();
   virtual const Rvector& GetInitialStateVector(Integer phaseIdx);
   virtual const Rvector& GetFinalStateVector(Integer phaseIdx);
   virtual const Rvector& GetStaticVector(Integer phaseIdx); // yk mod static params
   virtual const Rvector& GetCostFunctionValue();
   virtual Real    GetInitialTime(Integer phaseIdx);
   virtual Real    GetFinalTime(Integer phaseIdx);
   virtual const StringArray&
                   GetAlgFunctionNames();
   virtual const std::vector<OptimalControlFunction*>& GetOptControlFunctionObjects();
   
   DEPRECATED(virtual void SetAlgFunctions(const Rvector &toValues));
   DEPRECATED(virtual void    SetAlgUpperBounds(const Rvector &toValues));
   DEPRECATED(virtual void    SetAlgLowerBounds(const Rvector &toValues));
   DEPRECATED(virtual void    SetAlgFunctionNames(const StringArray& toNames));
   DEPRECATED(virtual void    SetAlgStateJacobian(const Rmatrix &toValue));
   DEPRECATED(virtual void    SetAlgTimeJacobian(const Rmatrix &toValue));
   
   DEPRECATED(virtual void    SetCostFunction(const Rvector &toValue));
   DEPRECATED(virtual void    SetCostStateJacobian(const Rmatrix &toValue));
   DEPRECATED(virtual void    SetCostTimeJacobian(const Rmatrix &toValue));

   virtual void    SetFunctions(FunctionType type, const Rvector& toValues);
   virtual void    SetFunctionNames(FunctionType type, const StringArray& toNames);
   virtual void    SetFunctionBounds(FunctionType type, FunctionBound bound, const Rvector& toValues);
   virtual void    SetJacobian(FunctionType funcType, JacobianType jacType, const Rmatrix& toJacobian);
protected:
   
   /// number of phases
   Integer                          numPhases;
   /// container of point function(s)
   PointFunctionContainer           *pfContainer;  // funcData in MATLAB
   /// input data
   std::vector<FunctionInputData*>  initialInputData;
   std::vector<FunctionInputData*>  finalInputData;
   std::vector<OptimalControlFunction*>  optControlFuncVec;

};
#endif // UserPointFunction_hpp
