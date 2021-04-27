//------------------------------------------------------------------------------ 
//                         TutorialPointObject 
//------------------------------------------------------------------------------ 
// GMAT: General Mission Analysis Tool. 
// 
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved. 
// 
// Author: Noble Hatten
// Created: 2019/07/03
// 
/**
 * Developed based on BrachistichronePointObject.hpp
 */
 //------------------------------------------------------------------------------ 
#ifndef TutorialPointObject_hpp 
#define TutorialPointObject_hpp 

#include "UserPointFunction.hpp" 

/**
 * TutorialPointObject class
 */
class TutorialPointObject : public UserPointFunction
{
public:

	/// default constructor 
	TutorialPointObject();
	/// default destructor 
	virtual ~TutorialPointObject();


	/// EvaluateFunctions 
	void EvaluateFunctions();

	/// EvaluateJacobians 
	void EvaluateJacobians();


protected:
};

#endif // BrachistichronePointObject_hpp