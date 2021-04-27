//------------------------------------------------------------------------------
//                         NLPFuncUtil_Path
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Wendy Shoan
// Created: 2016.05.20
//
/**
 * NOTE: currently, this class is empty - keeping as placeholder and
 *       for possible future use
 */
//------------------------------------------------------------------------------
#ifndef NLPFuncUtil_Path_hpp
#define NLPFuncUtil_Path_hpp

#include "NLPFuncUtil.hpp"

/**
 * NLPFuncUtil_Path class
 */
class NLPFuncUtil_Path : public NLPFuncUtil
{
public:
   
   
   /// default constructor
   NLPFuncUtil_Path();
   
   /// copy constructor
   NLPFuncUtil_Path(const NLPFuncUtil_Path &copy);
   
   /// assignment operator
   NLPFuncUtil_Path&   operator=(const  NLPFuncUtil_Path &copy);
   
   /// virtual destructor
   virtual ~NLPFuncUtil_Path();
   
   
protected:
   
};
#endif // NLPFuncUtil_Path_hpp