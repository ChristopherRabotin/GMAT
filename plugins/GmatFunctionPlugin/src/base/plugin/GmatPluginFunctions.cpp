//$Id$
//------------------------------------------------------------------------------
//                            GmatPluginFunctions
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, task order 28.
//
// Author: Darrel Conway (Thinking Systems)
// Created: 2012/09/18
//
/**
 * Implementation for library code interfaces.
 */
//------------------------------------------------------------------------------

#include "GmatPluginFunctions.hpp"
#include "MessageInterface.hpp"

#include "GmatFunctionFactory.hpp"
#include "GmatFunctionCommandFactory.hpp"

extern "C"
{
   //---------------------------------------------------------------------------
   // Integer GetFactoryCount()
   //---------------------------------------------------------------------------
   /**
    * Method used to determine how many factories the plug-in library supports.
    *
    * @return The number of factories in the plug-in library.
    */
   //---------------------------------------------------------------------------
   Integer GetFactoryCount()
   {
      return 2;
   }

   //---------------------------------------------------------------------------
   // Factory* GetFactoryPointer(Integer index)
   //---------------------------------------------------------------------------
   /**
    * Retrieves an instance of the factory that corresponds to the input integer
    *
    * This function is used to build all of the Factory instances needed by the
    * plug-in library.
    *
    * @param index Zero-based index identifying which factory is requested
    *
    * @return A pointer to the factory, or NULL if there is no factory with the
    *         specified index
    */
   //---------------------------------------------------------------------------
   Factory* GetFactoryPointer(Integer index)
   {
      Factory* factory = NULL;

      switch (index)
      {
      case 0:
         factory = new GmatFunctionFactory;
         break;

      case 1:
         factory = new GmatFunctionCommandFactory;
         break;

      default:
         break;
      }

      return factory;
   }

   //---------------------------------------------------------------------------
   // void SetMessageReceiver(MessageReceiver* mr)
   //---------------------------------------------------------------------------
   /**
    * Method used to pass the active MessageReceiver to the plug-in.
    *
    * If the code in your plug-in writes to GMAT's message interfaces, implement
    * this method so that GMAT can set the output streams so that your messages
    * display correctly.
    *
    * @param mr The MessageReceiver.
    *
    * @note This method is marked to be deprecated, and may be removed in future
    * builds.
    */
   //---------------------------------------------------------------------------
   void SetMessageReceiver(MessageReceiver* mr)
   {
      MessageInterface::SetMessageReceiver(mr);
   }
};
