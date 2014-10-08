//$Id$
//------------------------------------------------------------------------------
//                           TFSMagicNumbers
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS
// contract, Task Order 28
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Mar 11, 2014
/**
 * Magic number generator for signal based tracking data types
 */
//------------------------------------------------------------------------------

#include "TFSMagicNumbers.hpp"
#include "MessageInterface.hpp"
#include <algorithm>


#define MAGIC_NUMBER_BASE 9000

//------------------------------------------------------------------------------
// Static data
//------------------------------------------------------------------------------
TFSMagicNumbers* TFSMagicNumbers::instance = NULL;


//------------------------------------------------------------------------------
// Public Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TFSMagicNumbers* Instance()
//------------------------------------------------------------------------------
/**
 * Accessor for the singleton
 *
 * @return The singleton
 */
//------------------------------------------------------------------------------
TFSMagicNumbers* TFSMagicNumbers::Instance()
{
   if (instance == NULL)
      instance = new TFSMagicNumbers;
   return instance;
}


//------------------------------------------------------------------------------
// Integer GetMagicNumber(const std::vector<StringArray> &nodelist,
//       const std::string &type)
//------------------------------------------------------------------------------
/**
 * Retrieves the measurement data type magic number
 *
 * @param nodelist The list of nodes, signal path by signal path
 * @param type The type of measurement that is being modeled
 *
 * @return The magic number associated with the measuremetn description, or -1
 *         if the configuration is not recognized.
 */
//------------------------------------------------------------------------------
Integer TFSMagicNumbers::GetMagicNumber(
      const std::vector<StringArray> &nodelist, const std::string &type)
{
   #ifdef DEBUG_MNLOOKUP
      MessageInterface::ShowMessage("Entered TFSMagicNumbers::GetMagicNumber; "
            "type = %s\nNodeList:\n", type.c_str());
      for (UnsignedInt i = 0; i < nodelist.size(); ++i)
      {
         MessageInterface::ShowMessage("   {");
         for (UnsignedInt j = 0; j < nodelist[i].size(); ++j)
         {
            if (j > 0)
               MessageInterface::ShowMessage(", ");
            MessageInterface::ShowMessage("'%s'", nodelist[i][j].c_str());
         }
         MessageInterface::ShowMessage("}\n");
      }
   #endif

   Integer retval = -1;
   Integer arbitraryMagicNumber = -1;

   // Build the lookup data
   Integer nodeCount = 0, strandCount = (Integer)nodelist.size();
   StringArray uniqueNodes;

   for (UnsignedInt i = 0; i < nodelist.size(); ++i)
   {
      for (UnsignedInt j = 0; j < nodelist[i].size(); ++j)
      {
         if (find(uniqueNodes.begin(), uniqueNodes.end(), nodelist[i][j]) ==
               uniqueNodes.end())
         {
            // ++nodeCount;                     // made changes by TUAN NGUYEN
            uniqueNodes.push_back(nodelist[i][j]);
         }
      }
   }
   nodeCount = uniqueNodes.size();               // made changes by TUAN NGUYEN

   for (UnsignedInt i = 0; i < lookupTable.size(); ++i)
   {
      if (lookupTable[i]->type == type)
      {
         if ((lookupTable[i]->arbitraryCount == true) &&
             (lookupTable[i]->signalPathCount == strandCount))
            arbitraryMagicNumber = lookupTable[i]->magicNumber;
         else
         {
            // Must be an exact match
            if ((lookupTable[i]->nodeCount == nodeCount) &&
                (lookupTable[i]->signalPathCount == strandCount))
            {
               // Check node list ordering
               bool numberFound = false;
               for (UnsignedInt j = 0; j < nodelist.size(); ++j)
               {
                  if (nodelist[j] == lookupTable[i]->nodes[j])
                  {
                     retval = lookupTable[i]->magicNumber;
                     numberFound = true;
                     break;
                  }
               }
               if (numberFound)
                  break;
            }
         }
      }
   }

   // This code is moved to default constructor fucntion
   //// Build the factor map                                                   // made changes by TUAN NGUYEN
   //factorMap.clear();                                                        // made changes by TUAN NGUYEN
   //for (UnsignedInt i = 0; i < lookupTable.size(); ++i)                      // made changes by TUAN NGUYEN
   //   factorMap[lookupTable[i]->magicNumber] = lookupTable[i]->multFactor;   // made changes by TUAN NGUYEN

   if (retval == -1)
      retval = arbitraryMagicNumber;

   return retval;
}


//------------------------------------------------------------------------------
// StringArray GetKnownTypes()
//------------------------------------------------------------------------------
/**
 * Returns list of registered measurement type keywords
 *
 * @return The list
 */
//------------------------------------------------------------------------------
StringArray TFSMagicNumbers::GetKnownTypes()
{
   return knownTypes;
}


//------------------------------------------------------------------------------
// Real GetMNMultiplier(Integer magicNumber)
//------------------------------------------------------------------------------
/**
 * Retrieves the global multiplicative factor used in the model
 *
 * @param magicNumber The model type ID for the model
 *
 * @return The corresponding multiplier
 */
//------------------------------------------------------------------------------
Real TFSMagicNumbers::GetMNMultiplier(Integer magicNumber)
{
   return factorMap[magicNumber];
}


//------------------------------------------------------------------------------
// Private Methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// TFSMagicNumbers()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
TFSMagicNumbers::TFSMagicNumbers() :
   lastNumber        (MAGIC_NUMBER_BASE)
{
   // Load in the defined magic number maps.  In the long run, we should move
   // these to a config file.
   LookupEntry *lue = new LookupEntry;

   // Generic range entry.  Other range entries take precedence
   lue = new LookupEntry;
   lue->arbitraryCount = true;
   lue->signalPathCount = 1;
   lue->nodeCount = -1;             // Arbitrary
   StringArray nodes;
   lue->nodes.push_back(nodes);     // Empty for this one
   lue->type = "Range";
   lue->multFactor = 1.0;
   lue->magicNumber = lastNumber;

   // Yeah, yeah, the list is empty here, but in case code is cut and pasted...
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   // One way range, 2 participants
   lue = new LookupEntry;
   lue->arbitraryCount = false;
   lue->signalPathCount = 1;
   lue->nodeCount = 2;
   nodes.clear();
   nodes.push_back("S1");
   nodes.push_back("T1");
   lue->nodes.push_back(nodes);
   lue->type = "Range";
   lue->multFactor = 1.0;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   // Two way range
   lue = new LookupEntry;
   lue->arbitraryCount = false;
   lue->signalPathCount = 1;
   lue->nodeCount = 2;
   nodes.clear();
   nodes.push_back("T1");
   nodes.push_back("S1");
   nodes.push_back("T1");
   lue->nodes.push_back(nodes);
   lue->type = "Range";
   lue->multFactor = 0.5;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   
   // Generic DSNRange entry.  Other DSN Range entries take precedence
   lue = new LookupEntry;
   lue->arbitraryCount = true;
   lue->signalPathCount = 1;
   lue->nodeCount = -1;             // Arbitrary
   lue->nodes.clear();
   lue->nodes.push_back(nodes);     // Empty for this one
   lue->type = "DSNRange";
   lue->multFactor = 1.0;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   // Added by TUAN NGUYEN
   // DSN two way range
   lue = new LookupEntry;
   lue->arbitraryCount = false;
   lue->signalPathCount = 1;
   lue->nodeCount = 2;
   nodes.clear();
   nodes.push_back("T1");
   nodes.push_back("S1");
   nodes.push_back("T1");
   lue->nodes.push_back(nodes);
   lue->type = "DSNRange";
   lue->multFactor = 1.0;   //-1.0;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   // Added by TUAN NGUYEN
   // Generic doppler entry.  Other doppler entries take precedence
   lue = new LookupEntry;
   lue->arbitraryCount = true;
   lue->signalPathCount = 1;
   lue->nodeCount = -1;             // Arbitrary
   nodes.clear();
   lue->nodes.push_back(nodes);     // Empty for this one
   lue->type = "Doppler";
   lue->multFactor = 1.0;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);
   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   // Added by TUAN NGUYEN
   // DSN two way doppler
   lue = new LookupEntry;
   lue->arbitraryCount = false;
   lue->signalPathCount = 1;
   lue->nodeCount = 2;
   nodes.clear();
   nodes.push_back("T1");
   nodes.push_back("S1");
   nodes.push_back("T1");
   lue->nodes.push_back(nodes);
   lue->type = "Doppler";
   lue->multFactor = 1.0;   // -1.0;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   
   // Build the factor map                                                      // made changes by TUAN NGUYEN
   factorMap.clear();                                                           // made changes by TUAN NGUYEN
   for (UnsignedInt i = 0; i < lookupTable.size(); ++i)                         // made changes by TUAN NGUYEN
      factorMap[lookupTable[i]->magicNumber] = lookupTable[i]->multFactor;      // made changes by TUAN NGUYEN

}


//------------------------------------------------------------------------------
// ~TFSMagicNumbers()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
TFSMagicNumbers::~TFSMagicNumbers()
{
   // Clean up memory
   for (UnsignedInt i = 0; i < lookupTable.size(); ++i)
      delete lookupTable[i];
}
