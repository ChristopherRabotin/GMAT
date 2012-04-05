//$Id$
//------------------------------------------------------------------------------
//                           BeginMissionSequence
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2011 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Author: Darrel J. Conway
// Created: 2010/05/21
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS task 28
//
/**
 * Null operator used to begin mission sequence -- used (optionally) to toggle
 * command mode
 */
//------------------------------------------------------------------------------



#ifndef BeginMissionSequence_hpp
#define BeginMissionSequence_hpp

#include "GmatCommand.hpp"

/**
 * Command used to toggle GMAT into command mode
 */
class GMAT_API BeginMissionSequence : public GmatCommand
{
public:
   BeginMissionSequence();
   virtual ~BeginMissionSequence();
   BeginMissionSequence(const BeginMissionSequence& BeginMissionSequence);
   BeginMissionSequence& operator=(const BeginMissionSequence &bms);
   
   bool                 Execute();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual const std::string&  
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");

   DEFAULT_TO_NO_CLONES
};


#endif // BeginMissionSequence_hpp
