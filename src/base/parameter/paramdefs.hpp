//$Header$
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/26 Linda Jun
//
/**
 * Defines parameter typedefs.
 */
//------------------------------------------------------------------------------
#ifndef paramdefs_hpp
#define paramdefs_hpp

#include <string>
#include <map>
#include "Parameter.hpp"

typedef std::vector<Parameter*> ParameterPtrArray;
typedef std::map<std::string, Parameter*> StringParamPtrMap;
typedef std::pair<std::string, Parameter*> StringParamPtrPair;

#endif
