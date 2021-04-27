//------------------------------------------------------------------------------ 
//                         TutorialPathObject 
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
// Based on BrachistochronePathObject.hpp
// 
/**
 */
 //------------------------------------------------------------------------------ 
#ifndef TutorialPathObject_hpp 
#define TutorialPathObject_hpp 

#include "UserPathFunction.hpp" 

/**
 * TutorialPathObject class
 */
class TutorialPathObject : public UserPathFunction
{
public:

	/// default constructor 
	TutorialPathObject();
	/// default destructor 
	virtual ~TutorialPathObject();


	/// EvaluateFunctions 
	void EvaluateFunctions();

	/// EvaluateJacobians 
	void EvaluateJacobians();


protected:
	Real gravity;
};

#endif // TutorialPathObject_hpp
