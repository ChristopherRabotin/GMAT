//------------------------------------------------------------------------------ 
//                         BrachistichronePathObject 
//------------------------------------------------------------------------------ 
// GMAT: General Mission Analysis Tool. 
// 
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved. 
// 
// Author: Jeremy Knittel 
// Created: 2016.11.03 
// 
/** 
 * Developed based on BrachistichronePathObject.m 
 */ 
//------------------------------------------------------------------------------ 
 
#include "BrachistichronePathObject.hpp" 
#include "MessageInterface.hpp" 
 
//#define DEBUG_Brachistichrone_PATH 
 
//------------------------------------------------------------------------------ 
// static data 
//------------------------------------------------------------------------------ 
// none 
 
//------------------------------------------------------------------------------ 
// public methods 
//------------------------------------------------------------------------------ 
 
//------------------------------------------------------------------------------ 
// default constructor 
//------------------------------------------------------------------------------ 
 
BrachistichronePathObject::BrachistichronePathObject() : 
   UserPathFunction(), 
   gravity(-32.174) 
{ 
} 
 
//------------------------------------------------------------------------------ 
// copy constructor 
//------------------------------------------------------------------------------ 
BrachistichronePathObject::BrachistichronePathObject(const BrachistichronePathObject &copy) : 
   UserPathFunction(copy), 
   gravity(copy.gravity) 
{ 
} 
 
 
//------------------------------------------------------------------------------ 
// operator= 
//------------------------------------------------------------------------------ 
BrachistichronePathObject& BrachistichronePathObject::operator=(const BrachistichronePathObject &copy) 
{ 
   if (&copy == this) 
      return *this; 
    
   UserPathFunction::operator=(copy); 
   gravity = copy.gravity; 
    
   return *this; 
} 
 
//------------------------------------------------------------------------------ 
// destructor 
//------------------------------------------------------------------------------ 
BrachistichronePathObject::~BrachistichronePathObject() 
{ 
} 
 
//------------------------------------------------------------------------------ 
// void EvaluateFunctions() 
//----------------------------------------------------------------------------- 
void BrachistichronePathObject::EvaluateFunctions() 
{ 
   // Extract parameter data 
   Rvector yVec = GetStateVector(); 
   Rvector uVec = GetControlVector(); 
    
#ifdef DEBUG_Brachistichrone_PATH 
   MessageInterface::ShowMessage("yVec size = %d\n", yVec.GetSize()); 
   MessageInterface::ShowMessage("uVec size = %d\n", uVec.GetSize()); 
#endif 
 
   Real    y  = yVec(2) * sin(uVec(0)); 
   Real    y2 = yVec(2) * cos(uVec(0)); 
   Real    y3 = gravity * cos(uVec(0)); 
    
#ifdef DEBUG_Brachistichrone_PATH 
   MessageInterface::ShowMessage("y  = %12.10f\n", y); 
   MessageInterface::ShowMessage("y2 = %12.10f\n", y2); 
   MessageInterface::ShowMessage("y3 = %12.10f\n", y3); 
#endif 
    
   // Evaluate dynamics and compute Jacobians 
   Rvector dynFunctions(y, y2, y3); 
   SetDynFunctions(dynFunctions); 
#ifdef DEBUG_Brachistichrone_PATH 
   MessageInterface::ShowMessage("EXITING BrachistichronePathObject::EvaluateFunctions\n"); 
#endif 
} 
 
//------------------------------------------------------------------------------ 
// void EvaluateJacobians() 
//----------------------------------------------------------------------------- 
void BrachistichronePathObject::EvaluateJacobians() 
{ 
   // Computes the user Jacobians 
    
   Rvector yVec = GetStateVector(); 
   Rvector uVec = GetControlVector(); 
    
#ifdef DEBUG_Brachistichrone_PATH 
   MessageInterface::ShowMessage("yVec size = %d\n", yVec.GetSize()); 
   MessageInterface::ShowMessage("uVec size = %d\n", uVec.GetSize()); 
#endif 
    
   Real    dydy3  =  sin(uVec(0)); 
   Real    dy2dy3 =  cos(uVec(0)); 
       
   Real    dydu  =  yVec(2) * cos(uVec(0)); 
   Real    dy2du = -yVec(2) * sin(uVec(0)); 
   Real    dy3du = -gravity * sin(uVec(0)); 
    
#ifdef DEBUG_Brachistichrone_PATH 
   MessageInterface::ShowMessage("dydy3  = %12.10f\n", dydy3); 
   MessageInterface::ShowMessage("dy2dy3 = %12.10f\n", dy2dy3); 
   MessageInterface::ShowMessage("dydu  = %12.10f\n", dydu); 
   MessageInterface::ShowMessage("dy2du = %12.10f\n", dy2du); 
   MessageInterface::ShowMessage("dy3du = %12.10f\n", dy3du); 
#endif 
    
   Rmatrix dynState(0,0,dydy3, 
                    0,0,dy2dy3, 
          0,0,0); 
   Rmatrix dynControl(dydu, dy2du, dy3du); 
   Rmatrix dynTime(0, 0, 0); 
    
   // Dynamics Function Partials 
   SetDynStateJacobian(dynState); 
   SetDynControlJacobian(dynControl); 
   SetDynTimeJacobian(dynTime); 
    
#ifdef DEBUG_Brachistichrone_PATH 
   MessageInterface::ShowMessage("EXITING BrachistichronePathObject::EvaluateJacobians\n"); 
#endif 
    
} 
 
 
//------------------------------------------------------------------------------ 
// protected methods 
//------------------------------------------------------------------------------ 
// none 