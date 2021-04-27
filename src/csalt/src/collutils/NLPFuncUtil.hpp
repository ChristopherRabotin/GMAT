//------------------------------------------------------------------------------
//                         NLPFuncUtil
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Youngkwang Kim
// Created: 2015/07/10 (mods 2016.05.20 WCS)
//
/**
 * header only NLPFuncUtil class. 
 * NOTE: currently, this class is empty - keeping as placeholder and
 *       for possible future use
 */
//------------------------------------------------------------------------------
#ifndef NLPFuncUtil_hpp
#define NLPFuncUtil_hpp

/**
 * NLPFuncUtil class
 */
class NLPFuncUtil
{
public:

   /// default constructor
   NLPFuncUtil();
   
   /// copy constructor
   NLPFuncUtil(const NLPFuncUtil &copy);
   
   /// assignment operator
   NLPFuncUtil&   operator=(const  NLPFuncUtil &copy);
   
   /// virtual destructor
   virtual ~NLPFuncUtil();
   

protected:

};
#endif // NLPFuncUtil_hpp