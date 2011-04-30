//$Id$
//------------------------------------------------------------------------------
//                                 MemoryTracker
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2008/11/21
//
/**
 * Declares MemoryTracker class which tracks memory usage. It is a singleton class -
 * only one instance of this class can be created.
 */
//------------------------------------------------------------------------------
#ifndef MemoryTracker_hpp
#define MemoryTracker_hpp

#include "gmatdefs.hpp"

class MemoryTracker
{
public:
   
   static MemoryTracker* Instance();
   
   void           SetScript(const std::string &script);
   void           SetShowTrace(bool show);
   void           Add(void *addr, const std::string &objName,
                      const std::string &funName, const std::string &note = "",
                      void *from = NULL);
   void           Remove(void *addr, const std::string &objName,
                         const std::string &funName, const std::string &note = "",
                         void *from = NULL);
   UnsignedInt    GetNumberOfTracks();
   StringArray&   GetTracks(bool clearTracks = false, bool writeScriptName = false);
   
private:
   
   static MemoryTracker *instance;
   
   struct TrackType
   {
      std::string preface;
      void *address;
      std::string objectName;
      std::string functionName;
      std::string remark;
      std::string scriptName;
      TrackType(const std::string &pref, void* addr, const std::string &objName,
                const std::string &funName, const std::string &note,
                const std::string &script)
         {
            preface = pref;
            address = addr;
            objectName = objName;
            functionName = funName;
            remark = note;
            scriptName = script;
         };
   };
   
   std::string scriptFile;
   std::vector<TrackType> memoryTracks;
   StringArray allTracks;
   bool showTrace;
   
   MemoryTracker();
   ~MemoryTracker();
   
};

#endif // MemoryTracker_hpp

