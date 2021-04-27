// Created 2/7/17
#ifndef DynamicDataStruct_hpp
#define DynamicDataStruct_hpp

#include "gmatdefs.hpp"
#include "Parameter.hpp"

struct GMAT_API DDD
{
   std::string paramName;
   std::string refObjectName;
   ElementWrapper *paramWrapper;
   std::string paramValue;
   UnsignedInt paramTextColor;
   UnsignedInt paramBackgroundColor;
   Real warnLowerBound;
   Real warnUpperBound;
   Real critLowerBound;
   Real critUpperBound;
   Parameter *paramRef;
   bool isTextColorUserSet;
};
#endif
