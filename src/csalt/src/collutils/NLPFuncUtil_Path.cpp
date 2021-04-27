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
#include "NLPFuncUtil_Path.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
NLPFuncUtil_Path::NLPFuncUtil_Path() :
   NLPFuncUtil()
{
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
NLPFuncUtil_Path::NLPFuncUtil_Path(const NLPFuncUtil_Path &copy) :
   NLPFuncUtil(copy)
{
}

//------------------------------------------------------------------------------
// assignment operator
//------------------------------------------------------------------------------
NLPFuncUtil_Path& NLPFuncUtil_Path::operator=(const  NLPFuncUtil_Path &copy)
{
   if (&copy == this)
      return *this;
   
   NLPFuncUtil::operator=(copy);
   
   return *this;
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
NLPFuncUtil_Path::~NLPFuncUtil_Path()
{
}
//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
