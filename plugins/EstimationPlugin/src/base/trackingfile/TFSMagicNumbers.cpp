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

//#define DEBUG_MNLOOKUP


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
            // ++nodeCount;
            uniqueNodes.push_back(nodelist[i][j]);
         }
      }
   }
   nodeCount = uniqueNodes.size();

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
   //// Build the factor map
   //factorMap.clear();
   //for (UnsignedInt i = 0; i < lookupTable.size(); ++i)
   //   factorMap[lookupTable[i]->magicNumber] = lookupTable[i]->multFactor;

   if (retval == -1)
      retval = arbitraryMagicNumber;

   return retval;
}

//------------------------------------------------------------------------------
// Integer FillMagicNumber(ObsData* theObs)
//------------------------------------------------------------------------------
/**
 * Parses the configuration in an observation and fills in magic number and type
 *
 * TDM Data files do not directly provide enough information to fill in the GMAT
 * observation type numbers and types.  The TDMObType code calls this method to
 * complete the data needed in an ObsData record.
 *
 * @todo The implementation here hard codes the mapping for two data types:
 *       DSNRange and DSNDoppler.  We need to refactor this code so that new
 *       data types can be added -- for example, in a plugin -- without the need
 *       to edit this file.
 *
 * @param theObs The observation that needs magic number data
 *
 * @return The magic number for the observation
 */
//------------------------------------------------------------------------------
//Integer TFSMagicNumbers::FillMagicNumber(ObsData* theObs)
Integer TFSMagicNumbers::FillMagicNumber(ObservationData* theObs)
{
   Integer retval = -1;
   bool remapData = false;

   #ifdef DEBUG_MNLOOKUP
      MessageInterface::ShowMessage("Generating magic number:\n"
            "   typeName:  %s\n   units:     %s\n   %d strands\n"
            "   %d participants in strand 0\n", theObs->typeName.c_str(),
            theObs->unit.c_str(), theObs->strands.size(),
            (theObs->strands.size() > 0 ? theObs->strands[0].size() : 0));
   #endif

   // For now, DSN Range and Doppler remap their data into the old style models
   if ((theObs->typeName == "RANGE") && (theObs->unit == "RU") &&
       (theObs->strands.size() == 1) && (theObs->strands[0].size() == 3))
   {
      theObs->typeName = "DSNTwoWayRange";
      remapData = true;
   }

   if ((theObs->typeName == "RECEIVE_FREQ") && // (theObs->units == "RU") &&
       (theObs->strands.size() == 1) && (theObs->strands[0].size() == 3))
   {
      theObs->typeName = "DSNTwoWayDoppler";
      remapData = true;
   }

   for (UnsignedInt i = 0; i < lookupTable.size(); ++i)
      if (lookupTable[i]->type == theObs->typeName)
         theObs->type = lookupTable[i]->magicNumber;

   // Helper function used to rearrange the data in an observation when needed
   if (remapData)
      SetType(theObs);

   #ifdef DEBUG_MNLOOKUP
      MessageInterface::ShowMessage("   Updated type name: %s\n",
            theObs->typeName.c_str());
      MessageInterface::ShowMessage("   Magic number: %d\n", theObs->type);
   #endif
   
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
// void TFSMagicNumbers::SetType(ObsData* forData)
//------------------------------------------------------------------------------
/**
 * Looks up the magic number based on data contained in an observation record
 *
 * This method sets the observation type in an observation record, remapping the
 * data to old style measurement models.  It is a temporary method used until
 * the adapter based DSN modeling is in place.
 *
 * @param forData The observation record
 */
//------------------------------------------------------------------------------
//void TFSMagicNumbers::SetType(ObsData* forData)
void TFSMagicNumbers::SetType(ObservationData* forData)
{
   #ifndef DSN_ADAPTERS_READY
      if (forData->typeName == "DSNTwoWayRange")
      {
         forData->type = Gmat::DSN_TWOWAYRANGE;
         forData->strands.clear();

         RealArray remappedData;
         for (UnsignedInt i = 0; i < forData->dataMap.size(); ++i)
         {
            if (forData->dataMap[i] == "RANGE")
            {
               remappedData.push_back(forData->value[i]);
               forData->value_orig.push_back(forData->value[i]);
            }
            if (forData->dataMap[i] == "TRANSMIT_BAND")
               forData->uplinkBand = (Integer)forData->value[i];
            if (forData->dataMap[i] == "RANGE_MODULUS")
               forData->rangeModulo = (Integer)forData->value[i];
            if ((forData->dataMap[i] == "TRANSMIT_FREQ") ||
                (forData->dataMap[i] == "TRANSMIT_FREQ_1"))
            {
               /// Current TDM is in MHz, not the spec'd Hz; try to fix that here:
               if (forData->value[i] > 1.0e6)
                  forData->uplinkFreq = (Integer)forData->value[i];
               else
                  forData->uplinkFreq = (Integer)forData->value[i] * 1.0e6;
            }
         }
         forData->dataMap.clear();
         forData->value = remappedData;
      }

      if (forData->typeName == "DSNTwoWayDoppler")
      {
         forData->type = Gmat::DSN_TWOWAYDOPPLER;
         forData->strands.clear();

         RealArray remappedData;
         for (UnsignedInt i = 0; i < forData->dataMap.size(); ++i)
         {
            // Hard code to X band for now
            forData->uplinkBand = 2;
            if (forData->dataMap[i] == "RECEIVE_FREQ")
            {
               remappedData.push_back(forData->value[i]);
               forData->value_orig.push_back(forData->value[i]);
            }

            if (forData->dataMap[i] == "INTEGRATION_INTERVAL")
               forData->dopplerCountInterval = (Integer)forData->value[i];

//            // CCSDS recommends using this for the transmit rather tahn a ramp
//            // file, but GMAT doesn't currently work that way
//            if ((forData->dataMap[i] == "TRANSMIT_FREQ") ||
//                (forData->dataMap[i] == "TRANSMIT_FREQ_1"))
//            {
//               /// Current TDM is in MHz, not the spec'd Hz; try to fix that here:
//               if (forData->value[i] > 1.0e6)
//                  forData->uplinkFreq = (Integer)forData->value[i];
//               else
//                  forData->uplinkFreq = (Integer)forData->value[i] * 1.0e6;
//            }
         }
         forData->dataMap.clear();
         forData->value = remappedData;
      }

      #ifdef DUMP_OBSDATA
         // Write out the observation
         MessageInterface::ShowMessage("Observation record at %p:\n", forData);
         MessageInterface::ShowMessage("   typeName:  %s\n", forData->typeName.c_str());
         MessageInterface::ShowMessage("   type:  %d\n", forData->type);
         MessageInterface::ShowMessage("   inUsed:  %s\n", (forData->inUsed ? "true" : "false"));
         MessageInterface::ShowMessage("   removedReason:  %s\n", forData->removedReason.c_str());
         MessageInterface::ShowMessage("   uniqueID:  %d\n", forData->uniqueID);
         MessageInterface::ShowMessage("   epochSystem:  %d\n", forData->epochSystem);
         MessageInterface::ShowMessage("   epoch:  %.12le\n", forData->epoch);
         MessageInterface::ShowMessage("   epochAtEnd:  %s\n", (forData->epochAtEnd ? "true" : "false"));
         MessageInterface::ShowMessage("   epochAtIntegrationEnd:  %s\n", (forData->epochAtIntegrationEnd ? "true" : "false"));
         MessageInterface::ShowMessage("   participantIDs:  %d members\n", forData->participantIDs.size());
         for (UnsignedInt i = 0; i < forData->participantIDs.size(); ++i)
            MessageInterface::ShowMessage("      %d: %s\n", i, forData->participantIDs[i].c_str());
         MessageInterface::ShowMessage("   strands: %d strands in the record\n", forData->strands.size());
         MessageInterface::ShowMessage("   value:  %d members\n", forData->value.size());
         MessageInterface::ShowMessage("   dataMap:  %d members\n", forData->dataMap.size());
         for (UnsignedInt i = 0; i < forData->value.size(); ++i)
         {
            if (forData->dataMap.size() > i)
               MessageInterface::ShowMessage("      %s --> ", forData->dataMap[i].c_str());
            else
               MessageInterface::ShowMessage("      ");
            MessageInterface::ShowMessage("%.12lf\n", forData->value[i]);
         }
         MessageInterface::ShowMessage("   value_orig:  %d members\n", forData->value_orig.size());
         MessageInterface::ShowMessage("   unit:  %s\n", forData->unit.c_str());
         MessageInterface::ShowMessage("   noiseCovariance:  <%p>\n", forData->noiseCovariance);
         MessageInterface::ShowMessage("   extraDataDescriptions:  %d members\n", forData->extraDataDescriptions.size());
         MessageInterface::ShowMessage("   extraTypes:  %d members\n", forData->extraTypes.size());
         MessageInterface::ShowMessage("   extraData:  %d members\n", forData->extraData.size());
         MessageInterface::ShowMessage("   uplinkBand:  %d\n", forData->uplinkBand);
         MessageInterface::ShowMessage("   uplinkFreq:  %.12le\n", forData->uplinkFreq);
         MessageInterface::ShowMessage("   rangeModulo: %.12le\n", forData->rangeModulo);
         MessageInterface::ShowMessage("   dopplerCountInterval:  %.12le\n", forData->dopplerCountInterval);
      #endif
   #endif
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
   lue->type = "Range_KM";               // "Range";
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
   lue->type = "Range_KM";              // "Range";
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
   lue->type = "Range_KM";              // "Range";
   lue->multFactor = 1.0;          // 0.5;
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

   // DSN two way range as used in the TDM file
   lue = new LookupEntry;
   lue->arbitraryCount = false;
   lue->signalPathCount = 1;
   lue->nodeCount = 2;
   nodes.clear();
   nodes.push_back("T1");
   nodes.push_back("S1");
   nodes.push_back("T1");
   lue->nodes.push_back(nodes);
   lue->type = "DSNTwoWayRange";
   // Note that: when multFactor is a non positive number, multiplier factor is a function. 
   lue->multFactor = -1.0;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   // DSN two way doppler as used in the TDM file
   lue = new LookupEntry;
   lue->arbitraryCount = false;
   lue->signalPathCount = 1;
   lue->nodeCount = 2;
   nodes.clear();
   nodes.push_back("T1");
   nodes.push_back("S1");
   nodes.push_back("T1");
   lue->nodes.push_back(nodes);
   lue->type = "DSNTwoWayDoppler";
   // Note that: when multFactor is a non positive number, multiplier factor is a function.
   lue->multFactor = -1.0;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);



   //Two way range-rate
   lue = new LookupEntry;
   lue->arbitraryCount = false;
   lue->signalPathCount = 1;
   lue->nodeCount = 2;
   nodes.clear();
   nodes.push_back("T1");
   nodes.push_back("S1");
   nodes.push_back("T1");
   lue->nodes.push_back(nodes);
   lue->type = "RangeRate";
   // Note that: when multFactor is a non positive number, multiplier factor is a function.
   lue->multFactor = -1.0;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;
   
   // Single point Two way range-rate
   lue = new LookupEntry;
   lue->arbitraryCount = false;
   lue->signalPathCount = 1;
   lue->nodeCount = 2;
   nodes.clear();
   nodes.push_back("T1");
   nodes.push_back("S1");
   nodes.push_back("T1");
   lue->nodes.push_back(nodes);
   lue->type = "PointRangeRate";
   // Note that: when multFactor is a non positive number, multiplier factor is a function.
   lue->multFactor = -1.0;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   // Generic range entry.  Other range entries take precedence
   lue = new LookupEntry;
   lue->arbitraryCount = true;
   lue->signalPathCount = 1;
   lue->nodeCount = -1;             // Arbitrary
   nodes.clear();
   lue->nodes.push_back(nodes);     // Empty for this one
   lue->type = "Doppler_RangeRate";
   lue->multFactor = 1.0;
   lue->magicNumber = lastNumber;

   // Yeah, yeah, the list is empty here, but in case code is cut and pasted...
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   // One way doppler, 2 participants
   lue = new LookupEntry;
   lue->arbitraryCount = false;
   lue->signalPathCount = 1;
   lue->nodeCount = 2;
   nodes.clear();
   nodes.push_back("S1");
   nodes.push_back("T1");
   lue->nodes.push_back(nodes);
   lue->type = "Doppler_RangeRate";
   lue->multFactor = 1.0;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   // Two way doppler
   lue = new LookupEntry;
   lue->arbitraryCount = false;
   lue->signalPathCount = 1;
   lue->nodeCount = 2;
   nodes.clear();
   nodes.push_back("T1");
   nodes.push_back("S1");
   nodes.push_back("T1");
   lue->nodes.push_back(nodes);
   lue->type = "Doppler_RangeRate";
   lue->multFactor = 0.5;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   // TDRS Two way doppler
   lue = new LookupEntry;
   lue->arbitraryCount = false;
   lue->signalPathCount = 1;
   lue->nodeCount = 3;
   nodes.clear();
   nodes.push_back("T1");
   nodes.push_back("S1");
   nodes.push_back("S2");
   nodes.push_back("S1");
   nodes.push_back("T1");
   lue->nodes.push_back(nodes);
   lue->type = "TDRSDoppler_HZ";
   lue->multFactor = -1.0;
   lue->magicNumber = lastNumber;
   if (find(knownTypes.begin(), knownTypes.end(), lue->type) == knownTypes.end())
      knownTypes.push_back(lue->type);

   lookupTable.push_back(lue);
   magicNumbers.push_back(lastNumber);
   ++lastNumber;

   
   // Build the factor map
   factorMap.clear();
   for (UnsignedInt i = 0; i < lookupTable.size(); ++i)
      factorMap[lookupTable[i]->magicNumber] = lookupTable[i]->multFactor;

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
