//$Id$
//------------------------------------------------------------------------------
//                                  IChangeListener
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author: Thomas G. Grubb/GSFC
// Created: 2015.06.19
//
/**
 * Definition of the IChangeListener class.  This is the virtual class/interface
 * for a class that listens for changed values
 *
 */
//------------------------------------------------------------------------------

#ifndef IChangeListener_hpp
#define IChangeListener_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"

// forward references

class GMAT_API IChangeListener
{
public:
        virtual ~IChangeListener() {}
        virtual void VariabledChanged(std::string name, Real value) = 0;
        virtual void VariabledChanged(std::string name, std::string &value) = 0;
        virtual void ConstraintChanged(std::string name, Real desiredValue, 
           Real value, Integer condition = 0) = 0;
};

#endif // IChangeListener_hpp