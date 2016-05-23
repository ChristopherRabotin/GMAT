//$Id$
//------------------------------------------------------------------------------
//                                GetEphemStates
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2015 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P.
//
// Author: Linda Jun
// Created: 2016.05.02
//
/**
 * Implements GetEphemStates class.
 * [initialEpoch, initialState, finalEpoch, finalState] =
 *    GetEphemStates(ephemType, sat, epochFormat, coordSystem)
 *   Input:
 *      ephemType    : Ephemeris type ('STK', 'SPK', 'Code500')
 *      sat          : Spacecraft with an associated ephemeris file, Otherwise throw error
 *      epochFormat  : String containing a valid epoch format for the resulting epoch output
 *      coordSystem  : CoordinateSystem for the resulting state output.
 *   Output:
 *      initialEpoch : String of intial epoch in requested epochFormat
 *      initialState : 6-element Array in the requested coordinateSystem
 *      finalEpoch   : String of final epoch on the file in requested epochFormat
 *      finalState   : 6-element Array in the requested coordinateSystem
 */
//------------------------------------------------------------------------------

#include "GetEphemStates.hpp"
#include "ArrayWrapper.hpp"
#include "StringObjectWrapper.hpp"
#include "FileManager.hpp"
#include "SpiceOrbitKernelReader.hpp"
#include "TimeSystemConverter.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_FUNCTION_INIT
//#define DEBUG_FUNCTION_EXEC
//#define DEBUG_WRAPPERS
//#define DEBUG_SPICE_READ

//#ifndef DEBUG_MEMORY
//#define DEBUG_MEMORY
//#endif

#ifdef DEBUG_MEMORY
#include "MemoryTracker.hpp"
#endif

//---------------------------------
// static data
//---------------------------------


//------------------------------------------------------------------------------
//  GetEphemStates(std::string typeStr, std::string name)
//------------------------------------------------------------------------------
/**
 * Constructs the GetEphemStates object (default constructor).
 * 
 * @param <typeStr> String text identifying the object type
 * @param <name>   Name for the object
 */
//------------------------------------------------------------------------------
GetEphemStates::GetEphemStates(const std::string &typeStr, const std::string &name) :
   BuiltinGmatFunction(typeStr, name),
   inSat             (NULL),
   inEphemType       (""),
   outEpochFormat    (""),
   outCoordSys       (NULL),
   ephemInitialA1Mjd (0.0),
   ephemFinalA1Mjd   (0.0)
{
   objectTypeNames.push_back(typeStr);
   objectTypeNames.push_back("GetEphemStates");
   
   // Build input and output arrays. Function interface is:
   // [initialEpoch, initialState, finalEpoch, finalState] =
   //     GetEphemStates(ephemType, sat, epochFormat, coordinateSystem)
   
   // Add dummy input names
   inputNames.push_back("__builtin_input_1_ephemtype__");
   inputArgMap.insert(std::make_pair("__builtin_input_1_ephemtype__", (ElementWrapper*) NULL));
   inputNames.push_back("__builtin_input_2_spacecraft__");
   inputArgMap.insert(std::make_pair("__builtin_input_2_spacecraft__", (ElementWrapper*) NULL));
   inputNames.push_back("__builtin_input_3_epochformat__");
   inputArgMap.insert(std::make_pair("__builtin_input_3_epochformat__", (ElementWrapper*) NULL));
   inputNames.push_back("__builtin_input_4_coordsys__");
   inputArgMap.insert(std::make_pair("__builtin_input_4_coordsys__", (ElementWrapper*) NULL));
   
   // Add dummy output names
   outputNames.push_back("__builtin_output_1_initialepoch__");
   outputArgMap.insert(std::make_pair("__builtin_output_1_initialepoch__", (ElementWrapper*) NULL));
   outputNames.push_back("__builtin_output_2_initialstate__");
   outputArgMap.insert(std::make_pair("__builtin_output_2_initialstate__", (ElementWrapper*) NULL));
   outputNames.push_back("__builtin_output_3_finalepoch__");
   outputArgMap.insert(std::make_pair("__builtin_output_3_finalepoch__", (ElementWrapper*) NULL));
   outputNames.push_back("__builtin_output_4_finalstate__");
   outputArgMap.insert(std::make_pair("__builtin_output_4_finalstate__", (ElementWrapper*) NULL));
   
   // Add output types
   outputWrapperTypes.push_back(Gmat::STRING_OBJECT_WT); // initialEpoch
   outputWrapperTypes.push_back(Gmat::ARRAY_WT);         // initialState
   outputWrapperTypes.push_back(Gmat::STRING_OBJECT_WT); // finalEpoch
   outputWrapperTypes.push_back(Gmat::ARRAY_WT);         // finalState
   
   // Add output row and column counts
   // initial epoch is 1x1
   outputRowCounts.push_back(1);
   outputColCounts.push_back(1);   
   // initial state is 6x1 array
   outputRowCounts.push_back(6);
   outputColCounts.push_back(1);
   // final epoch is 1x1
   outputRowCounts.push_back(1);
   outputColCounts.push_back(1);
   // final state is 6x1 array
   outputRowCounts.push_back(6);
   outputColCounts.push_back(1);   
}


//------------------------------------------------------------------------------
//  ~GetEphemStates(void)
//------------------------------------------------------------------------------
/**
 * Destroys the GetEphemStates object (destructor).
 */
//------------------------------------------------------------------------------
GetEphemStates::~GetEphemStates()
{
}


//------------------------------------------------------------------------------
//  GetEphemStates(const GetEphemStates &f)
//------------------------------------------------------------------------------
/**
 * Constructs the GetEphemStates object (copy constructor).
 * 
 * @param <f> Object that is copied
 */
//------------------------------------------------------------------------------
GetEphemStates::GetEphemStates(const GetEphemStates &f) :
   BuiltinGmatFunction(f),
   inSat             (f.inSat),
   inEphemType       (f.inEphemType),
   outEpochFormat    (f.outEpochFormat),
   outCoordSys       (f.outCoordSys),
   ephemInitialA1Mjd (f.ephemInitialA1Mjd),
   ephemFinalA1Mjd   (f.ephemFinalA1Mjd)
{
}


//------------------------------------------------------------------------------
//  GetEphemStates& operator=(const GetEphemStates &f)
//------------------------------------------------------------------------------
/**
 * Sets one GetEphemStates object to match another (assignment operator).
 * 
 * @param <f> The object that is copied.
 * 
 * @return this object, with the parameters set as needed.
 */
//------------------------------------------------------------------------------
GetEphemStates& GetEphemStates::operator=(const GetEphemStates &f)
{
   if (this == &f)
      return *this;
   
   BuiltinGmatFunction::operator=(f);
   inSat             = f.inSat;
   inEphemType       = f.inEphemType;
   outEpochFormat    = f.outEpochFormat;
   outCoordSys       = f.outCoordSys;
   ephemInitialA1Mjd = f.ephemInitialA1Mjd;
   ephemFinalA1Mjd   = f.ephemFinalA1Mjd;
   
   return *this;
}


//------------------------------------------------------------------------------
// virtual WrapperTypeArray GetOutputTypes(IntegerArray &rowCounts,
//                                         IntegeArrayr &colCounts) const
//------------------------------------------------------------------------------
WrapperTypeArray GetEphemStates::GetOutputTypes(IntegerArray &rowCounts,
                                                IntegerArray &colCounts) const
{
   rowCounts = outputRowCounts;
   colCounts = outputColCounts;
   return outputWrapperTypes;
}

//------------------------------------------------------------------------------
// virtual void SetOutputTypes(WrapperTypeArray &outputTypes,
//                             IntegerArray &rowCounts, IntegerArray &colCounts)
//------------------------------------------------------------------------------
/*
 * Sets function output types. This method is called when parsing the function
 * file from the Interpreter.
 */
//------------------------------------------------------------------------------
void GetEphemStates::SetOutputTypes(WrapperTypeArray &outputTypes,
                              IntegerArray &rowCounts, IntegerArray &colCounts)
{
   #ifdef DEBUG_FUNCTION_IN_OUT
   MessageInterface::ShowMessage
      ("GetEphemStates::SetOutputTypes() setting %d outputTypes\n", outputTypes.size());
   #endif
   
   // Set output wrapper type for GetEphemStates
   outputWrapperTypes = outputTypes;
   outputRowCounts = rowCounts;
   outputColCounts = colCounts;   
}

//------------------------------------------------------------------------------
// bool Initialize(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool GetEphemStates::Initialize(ObjectInitializer *objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("GetEphemStates::Initialize() <%p>'%s' entered\n", this, GetName().c_str());
   #endif
   
   BuiltinGmatFunction::Initialize(objInit);
   
   #ifdef DEBUG_FUNCTION_INIT
   MessageInterface::ShowMessage
      ("GetEphemStates::Initialize() <%p>'%s' returning true\n", this, GetName().c_str());
   #endif
   return true;
}


//------------------------------------------------------------------------------
// bool Execute(ObjectInitializer *objInit, bool reinitialize)
//------------------------------------------------------------------------------
bool GetEphemStates::Execute(ObjectInitializer *objInit, bool reinitialize)
{
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("GetEphemStates::Execute() <%p>'%s' entered\n", this, GetName().c_str());
   ShowObjects("");
   MessageInterface::ShowMessage("   inputArgMap.size() = %d\n", inputArgMap.size());
   MessageInterface::ShowMessage
      ("   outputArgMap.size() = %d\n   outputWrapperTypes.size() = %d\n",
       outputArgMap.size(), outputWrapperTypes.size());
   MessageInterface::ShowMessage
      ("   outputRowCounts.size() = %d\n   outputColCounts() = %d\n",
       outputRowCounts.size(), outputColCounts.size());
   #endif
   
   //=================================================================
   // Do some validation here
   //=================================================================
   // Check for input info, there should be 4 inputs
   if (inputArgMap.size() != 4)
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("GetEphemStates::Execute() returning false, size of inputArgMap:(%d) "
          "is not 4\n", inputArgMap.size());
      #endif
      return false;
   }
   
   // Check for output info, there shoud be 4 outputs
   // This is internal coding error if not 4
   if ((outputArgMap.size() != outputWrapperTypes.size()) &&
       outputWrapperTypes.size() != 4)
   {
      if (outputArgMap.size() != 4)
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("GetEphemStates::Execute() returning false, size of outputArgMap: %d or "
             "outputWrapperTypes: %d are not 4\n",outputArgMap.size(),  outputWrapperTypes.size());
         #endif
      }
      return false;
   }
   
   // Check for output row and col counts
   if (outputRowCounts.empty() || outputColCounts.empty())
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("GetEphemStates::Execute() returning false, size of outputRowCounts: &d or "
          "outputColCounts: %d are zero\n",outputRowCounts.size(),  outputColCounts.size());
      #endif
      return false;
   }
   
   // Check if input names are in the objectStore
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Checking if inputs exist in the objectStore\n");
   #endif
   std::string msg;
   GmatBase *obj = NULL;
   GmatBase *input1_ephemType = NULL;
   GmatBase *input2_sat = NULL;
   GmatBase *input3_epochFormat = NULL;
   GmatBase *input4_coordSys = NULL;
   
   for (unsigned int i = 0; i < inputNames.size(); i++)
   {
      std::string objName = inputNames[i];
      ObjectMap::iterator objIter = objectStore->find(objName);
      if (objIter != objectStore->end())
      {
         obj = objIter->second;
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage
            ("   input[%d] = <%p><%s>'%s'\n", i, obj, obj ? obj->GetTypeName().c_str() : "NULL",
             obj ? obj->GetName().c_str() : "NULL");
         #endif
         if (obj == NULL)
         {
            msg = msg + "Cannot find the object '" + objName + "' in the objectStore\n";
         }
         else
         {
            if (i == 1)
            {
               // 2nd Input: check for Spacecraft type
               if (obj->IsOfType(Gmat::SPACECRAFT))
                  input2_sat = obj;
               else
                  msg = msg + "The object '" + objName + "' is not valid 1st input type; "
                     "It is expecting a Spacecraft\n";
            }
            else if (i == 0 || i == 2)
            {
               // 1st or 3rd Input: check for String type
               if (obj->IsOfType(Gmat::STRING))
               {
                  if (i == 0)
                     input1_ephemType = obj;
                  else
                     input3_epochFormat = obj;
               }
               else
                  msg = msg + "The object '" + objName + "' is not valid 2nd input type; "
                     "It is expecting a String\n";
            }
            else if (i == 3)
            {
               // 4th Input: check for CoordinateSystem type
               if (obj->IsOfType(Gmat::COORDINATE_SYSTEM))
                  input4_coordSys = obj;
               else
                  msg = msg + "The object '" + objName + "' is not valid 3rd input type; "
                     "It is expecting a CoordinateSystem\n";
            }
         }
      }
   }
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   msg = '%s'\n", msg.c_str());
   #endif
   if (msg != "")
   {
      #ifdef DEBUG_FUNCTION_EXEC
      MessageInterface::ShowMessage
         ("GetEphemStates::Execute() returning false, one or more inputs "
          "not found in the objectStore or wrong type to operate on\n");
      #endif
      return false;
   }
   
   // Set objects to passed in input values
   inSat = (Spacecraft*)(inputArgMap[input2_sat->GetName()]->GetRefObject());
   inEphemType = inputArgMap[input1_ephemType->GetName()]->EvaluateString();
   outEpochFormat = inputArgMap[input3_epochFormat->GetName()]->EvaluateString();
   outCoordSys = (CoordinateSystem*)(inputArgMap[input4_coordSys->GetName()]->GetRefObject());
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("   inSat=<%p>'%s'\n   inEphemType = '%s'\n   outEpochFormat = '%s'\n   "
       "outCoordSys = <%p>'%s'\n", inSat, inSat->GetName().c_str(), inEphemType.c_str(),
       outEpochFormat.c_str(), outCoordSys, outCoordSys->GetName().c_str());
   #endif
   
   // Now acces ephemeris file specified in the Spacecraft
   if (inEphemType == "SPK")
   {
      if (!ReadSpiceEphemerisFile())
      {
         #ifdef DEBUG_FUNCTION_EXEC
         MessageInterface::ShowMessage("GetEphemStates::Execute() returning false\n");
         #endif
         return false;
      }
   }
   else if (inEphemType == "Code500")
   {
      MessageInterface::ShowMessage("*** ERROR *** Reading Code500 file is not supported yet\n");
      return false;
   }
   else if (inEphemType == "SPK")
   {
      MessageInterface::ShowMessage("*** ERROR *** Reading SPK file is not supported yet\n");
      return false;
   }
   
   //=================================================================
   // Now start setting output wrappers
   // 4 outputS
   //=================================================================
   int index = 0;
   std::map<std::string, ElementWrapper *>::iterator ewi = outputArgMap.begin();
   int numRows = 0;
   int numCols = 0;
   
   //=======================================================
   // 1st output is initialEpoch, wrapper type is STRING_OBJECT_WT
   //=======================================================
   
   ElementWrapper *outWrapper1 = CreateOutputEpochWrapper(ephemInitialA1Mjd, ewi->first);
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outWrapper1 to outputWrapperMap\n");
   #endif
   ewi->second = outWrapper1;
   
   //=======================================================
   // 2nd output is initialState, wrapper type is ARRAY_WT
   //=======================================================
   ++index;
   ++ewi;
   
   ElementWrapper *outWrapper2 = CreateOutputStateWrapper(index, ewi->first);
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outWrapper2 to outputWrapperMap\n");
   #endif
   ewi->second = outWrapper2;
   
   //=======================================================
   // 3rd output is finalEpoch, wrapper type is STRING_OBJECT_WT
   //=======================================================
   ++index;
   ++ewi;
   
   ElementWrapper *outWrapper3 = CreateOutputEpochWrapper(ephemFinalA1Mjd, ewi->first);
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outWrapper3 to outputWrapperMap\n");
   #endif
   ewi->second = outWrapper3;
   
   //=======================================================
   // 4th output is finalState, wrapper type is ARRAY_WT
   //=======================================================
   ++index;
   ++ewi;
   
   ElementWrapper *outWrapper4 = CreateOutputStateWrapper(index, ewi->first);
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outWrapper4 to outputWrapperMap\n");
   #endif
   ewi->second = outWrapper4;
   
   //=================================================================
   // End of setting output wrappers
   //=================================================================
   
   #ifdef DEBUG_FUNCTION_EXEC
   for (ewi = outputArgMap.begin(); ewi != outputArgMap.end(); ++ewi)
      MessageInterface::ShowMessage
         ("   ewi->first  = '%s', ewi->second = <%p>\n", (ewi->first).c_str(),
          ewi->second);
   MessageInterface::ShowMessage
      ("GetEphemStates::Execute() <%p>'%s' returning true\n", this,
       GetName().c_str());
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// void GetEphemStates::Finalize(bool cleanUp)
//------------------------------------------------------------------------------
void GetEphemStates::Finalize(bool cleanUp)
{
   #ifdef DEBUG_FUNCTION_FINALIZE
   MessageInterface::ShowMessage
      ("GetEphemStates::Finalize() <%p>'%s' entered, nothing to do here?\n",
       this, GetName().c_str());
   #endif
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * Clone of the GetEphemStates.
 *
 * @return clone of the GetEphemStates.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GetEphemStates::Clone() const
{
   return (new GetEphemStates(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void GetEphemStates::Copy(const GmatBase* orig)
{
   operator=(*((GetEphemStates *)(orig)));
}


//------------------------------------------------------------------------------
// bool ReadSpiceEphemerisFile()
//------------------------------------------------------------------------------
bool GetEphemStates::ReadSpiceEphemerisFile()
{
   #ifdef DEBUG_SPICE_READ
   MessageInterface::ShowMessage
      ("\nGetEphemStates::ReadSpiceEphemerisFile() entered, inEphemType='%s'\n", inEphemType.c_str());
   MessageInterface::ShowMessage("%s", GmatBase::WriteObjectInfo("   ", inSat).c_str());
   #endif
   
   StringArray spiceFiles = inSat->GetStringArrayParameter("OrbitSpiceKernelName");

   #ifdef DEBUG_SPICE_READ
   MessageInterface::ShowMessage("   spiceFiles.size() = %d\n", spiceFiles.size());
   #endif
   
   // If no spice files, throw an exception
   if (spiceFiles.empty())
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** Failed to execute built-in function: GetEphemState(). "
          "There are no SPK files associated with Spacecraft named '%s'\n",
          inSat->GetName().c_str());
      return false;
   }
   
   // Create SpiceOrbitKernelReader
   SpiceOrbitKernelReader *sokr = new SpiceOrbitKernelReader;
   if (sokr == NULL)
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** Error creating SpiceOrbitKernelReader\n");
      return false;
   }
   
   // Check spice files and load spice kernel
   FileManager *fm = FileManager::Instance();
   std::string fullPath;
   StringArray spkFullPathNames;
   
   for (int i = 0; i < spiceFiles.size(); i++)
   {
      std::string spiceFile = spiceFiles[i];      
      fullPath = fm->FindPath(spiceFile, "VEHICLE_EPHEM_SPK_PATH", true, false, true);
      
      #ifdef DEBUG_SPICE_READ
      MessageInterface::ShowMessage
         ("   Checking for kernel spiceFile = '%s'\n   fullPath = '%s'\n",
          spiceFile.c_str(), fullPath.c_str());
      #endif
      
      if (fullPath == "")
      {
         MessageInterface::ShowMessage
            ("*** ERROR *** The Spice file '%s' does not exist\n", spiceFile.c_str());
         delete sokr;
         return false;
      }
      
      if (sokr->IsLoaded(fullPath) == false)
         sokr->LoadKernel(fullPath);
      
      if (find(spkFullPathNames.begin(), spkFullPathNames.end(), fullPath) ==
          spkFullPathNames.end())
         spkFullPathNames.push_back(fullPath);
   }
   
   
   // Load the initial data point
   try
   {
      std::string scName = inSat->GetName();
      std::string spkCentralBody = inSat->GetOriginName();
      if (spkCentralBody == "Luna")
         spkCentralBody = "Moon";
      Integer satNaifId = inSat->GetIntegerParameter("NAIFId");
      Integer spkCentralBodyNaifId = sokr->GetNaifID(spkCentralBody);

      #ifdef DEBUG_SPICE_READ
      MessageInterface::ShowMessage
         ("   scName = '%s', satNaifId = '%d', spkCentralBody = '%s, spkCentralBodyNaifId = '%d'\n",
          scName.c_str(), satNaifId, spkCentralBody.c_str(), spkCentralBodyNaifId);
      #endif
      
      Real fileStart;
      Real fileEnd;
      
      // Get initial and final epoch
      sokr->GetCoverageStartAndEnd(spkFullPathNames, satNaifId, fileStart, fileEnd);
      
      #ifdef DEBUG_SPICE_READ
      MessageInterface::ShowMessage
         ("   fileStart = %.12f, fileEnd = %.12f\n", fileStart, fileEnd);
      #endif
      
      // Get initial state
      // @note I had add 1.0e-6 sec to get initial state without spice kernel error
      // [Insufficient ephemeris data has been loaded to compute the state of ...]
      Real secsDiff = 1.0e-6;
      Real daysDiff = secsDiff / 86400;
      ephemInitialA1Mjd = fileStart + daysDiff;
      
      #ifdef DEBUG_SPICE_READ
      MessageInterface::ShowMessage("   secsDiff = %.12f, daysDiff = %.12f\n", secsDiff, daysDiff);
      MessageInterface::ShowMessage("   ephemInitialA1Mjd = %.12f\n", ephemInitialA1Mjd);
      std::string epochStr1;
      Real toMjd1;
      TimeConverterUtil::Convert("A1ModJulian", fileStart, "",
                                 outEpochFormat, toMjd1, epochStr1);
      MessageInterface::ShowMessage("   fileStart    = '%s'\n", epochStr1.c_str());
      TimeConverterUtil::Convert("A1ModJulian", ephemInitialA1Mjd, "",
                                 outEpochFormat, toMjd1, epochStr1);
      MessageInterface::ShowMessage("   ephemInitial = '%s'\n", epochStr1.c_str());
      #endif
      
      ephemInitialState =
          sokr->GetTargetState(scName, satNaifId, ephemInitialA1Mjd,
                               spkCentralBody, spkCentralBodyNaifId);
      
      #ifdef DEBUG_SPICE_READ
      MessageInterface::ShowMessage
         ("   ephemInitialState = %s\n", ephemInitialState.ToString().c_str());
      #endif
      
      // Get final state
      ephemFinalA1Mjd = fileEnd - daysDiff;
      ephemFinalState =
          sokr->GetTargetState(scName, satNaifId, ephemFinalA1Mjd,
                               spkCentralBody, spkCentralBodyNaifId);
      
      #ifdef DEBUG_SPICE_READ
      MessageInterface::ShowMessage("   ephemFinalA1Mjd = %.12f\n", ephemFinalA1Mjd);
      std::string epochStr2;
      Real toMjd2;
      TimeConverterUtil::Convert("A1ModJulian", fileEnd, "",
                                 outEpochFormat, toMjd2, epochStr2);
      MessageInterface::ShowMessage("   fileEnd      = '%s'\n", epochStr2.c_str());
      TimeConverterUtil::Convert("A1ModJulian", ephemFinalA1Mjd, "",
                                 outEpochFormat, toMjd2, epochStr2);
      MessageInterface::ShowMessage("   ephemFinal   = '%s'\n", epochStr2.c_str());
      MessageInterface::ShowMessage
         ("   ephemFinalState  = %s\n", ephemFinalState.ToString().c_str());
      #endif
   }
   catch (BaseException &be)
   {
      MessageInterface::ShowMessage
         ("*** ERROR *** Error occurred while accessing SpiceOrbitKernel:\n");
      for (UnsignedInt i = 0; i < spkFullPathNames.size(); i++)
         MessageInterface::ShowMessage("   '%s'\n", spkFullPathNames[i].c_str());
      MessageInterface::ShowMessage(be.GetFullMessage());
      
      // unload the SPK kernels so they will not be retained in the kernel pool
      sokr->UnloadKernels(spkFullPathNames);
      delete sokr;
      return false;
   }
   
   // unload the SPK kernels so they will not be retained in the kernel pool
   sokr->UnloadKernels(spkFullPathNames);
   delete sokr;

   #ifdef DEBUG_SPICE_READ
   MessageInterface::ShowMessage("GetEphemStates::ReadSpiceEphemerisFile() returning true\n");
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
// ElementWrapper* CreateOutputEpochWrapper(Real a1MjdEpoch, 
//                                          const std::string &outName)
//------------------------------------------------------------------------------
ElementWrapper* GetEphemStates::CreateOutputEpochWrapper(Real a1MjdEpoch, 
                                                         const std::string &outName)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("GetEphemStates::CreateOutputStateWrapper() entered, a1MjdEpoch=%.12f, outName='%s'\n",
       a1MjdEpoch, outName.c_str());
   #endif
   
   // Epoch string output
   std::string epochStr;
   Real toMjd;
   TimeConverterUtil::Convert("A1ModJulian", a1MjdEpoch, "",
                              outEpochFormat, toMjd, epochStr);
   
   // Create StringVar, this will be deleted when StringObjectWrapper is deleted
   StringVar *outString = new StringVar(outName);
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (outString, "outString", "GetEphemStates::CreateOutputEpochWrapper()",
       "outString = new StringVar()");
   #endif
   outString->SetString(epochStr);
   
   // Create ArrayWrapper
   ElementWrapper *outWrapper = new StringObjectWrapper();         
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (outWrapper, "outWrapper", "GetEphemStates::CreateOutputEpochWrapper()",
       "outWrapper = new StringObjectWrapper()");
   #endif
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outString to outWrapper\n");
   #endif
   outWrapper->SetDescription(outName);
   outWrapper->SetRefObject(outString);
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("GetEphemStates::CreateOutputEpochWrapper() returning wrpper <%p>\n",
       outWrapper);
   #endif
   
   return outWrapper;
}

//------------------------------------------------------------------------------
// ElementWrapper* CreateOutputStateWrapper(Integer outIndex, 
//                                          const std::string &outName)
//------------------------------------------------------------------------------
ElementWrapper* GetEphemStates::CreateOutputStateWrapper(Integer outIndex, 
                                                         const std::string &outName)
{
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("GetEphemStates::CreateOutputStateWrapper() entered, outIndex=%d, outName='%s'\n",
       outIndex, outName.c_str());
   #endif
   
   if (outIndex != 1 && outIndex != 3)
   {
      MessageInterface::ShowMessage("**** CODING ERROR **** outIndex must be 1 or 3\n");
      return NULL;
   }
   
   int numRows = outputRowCounts[outIndex];
   int numCols = outputColCounts[outIndex];
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage
      ("   outName  = '%s', numRows = %d, numCols = %d\n", outName.c_str(),
       numRows, numCols);
   #endif
   
   Rvector6 state;
   Rmatrix rmat;
   rmat.SetSize(numRows, numCols);
   
   if (outIndex == 1)
      state = ephemInitialState;
   else if (outIndex == 3)
      state = ephemFinalState;
   
   // Set state
   for (int jj = 0; jj < numRows; jj++)
      rmat.SetElement(jj, 0, state(jj));
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   rmat = %s", rmat.ToString().c_str());
   #endif
   
   // Create Array, this array will be deleted when ArrayWrapper is deleted
   Array *outArray = new Array(outName);
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (outArray, "outArray", "GetEphemStates::CreateOutputStateWrapper()",
       "outArray = new Array()");
   #endif
   
   outArray->SetSize(numRows, numCols);
   outArray->SetRmatrix(rmat);
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Creating ArrayWrapper for output\n");
   #endif
   
   // Create ArrayWrapper
   ElementWrapper *outWrapper = new ArrayWrapper();         
   #ifdef DEBUG_MEMORY
   MemoryTracker::Instance()->Add
      (outWrapper, "outWrapper", "GetEphemStates::CreateOutputStateWrapper()",
       "outWrapper = new ArrayWrapper()");
   #endif
   
   #ifdef DEBUG_FUNCTION_EXEC
   MessageInterface::ShowMessage("   Setting outArray to outWrapper\n");
   #endif
   outWrapper->SetDescription(outName);
   outWrapper->SetRefObject(outArray);
   
   #ifdef DEBUG_WRAPPERS
   MessageInterface::ShowMessage
      ("GetEphemStates::CreateOutputStateWrapper() returning wrpper <%p>\n",
       outWrapper);
   #endif
   
   return outWrapper;
}

