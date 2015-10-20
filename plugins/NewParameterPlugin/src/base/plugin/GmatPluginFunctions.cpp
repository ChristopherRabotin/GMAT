//$Id: GmatPluginFunctions.cpp 1397 2011-04-21 19:04:45Z ljun@NDC $
//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract task order 28
//
// Author: Darrel Conway
// Created: 2013/05/24
//
/**
 * Implementation for library code interfaces.
 * 
 * This is prototype code.
 */
//------------------------------------------------------------------------------

#include "GmatPluginFunctions.hpp"
#include "MessageInterface.hpp"

#include "NewParameterFactory.hpp"

extern "C"
{
   Integer GetFactoryCount()
   {
      return 1;
   }
   
   Factory* GetFactoryPointer(Integer index)
   {
      Factory* factory = NULL;

      switch (index)
      {
         case 0:
            factory = new NewParameterFactory;
            break;
            
         default:
            break;
      }

      return factory;
   }
   
   void SetMessageReceiver(MessageReceiver* mr)
   {
      MessageInterface::SetMessageReceiver(mr);
   }
};
