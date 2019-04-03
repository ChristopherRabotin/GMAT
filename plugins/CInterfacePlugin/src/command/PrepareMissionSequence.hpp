//$Id: PrepareMissionSequence.hpp 8788 2010-11-26 21:36:26Z djcinsb $
//------------------------------------------------------------------------------
//                           PrepareMissionSequence
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2011/03/30
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under FDSS task 28
//
/**
 * Null operator used to begin mission sequence -- used (optionally) to toggle
 * command mode
 */
//------------------------------------------------------------------------------



#ifndef PrepareMissionSequence_hpp
#define PrepareMissionSequence_hpp

#include "GmatCFunc_defs.hpp"
#include "GmatCommand.hpp"

/**
 * Command used to toggle GMAT into command mode
 *
 * When run, scripts that start the Mission Control Sequence using the
 * PrepareMissionSequence command will populate the GMAT Sandbox with the
 * objects identified in the script, and then initialize the objects and
 * commands in the Sandbox.  The script is not actually executed.  Replacing
 * BeginMissionSequence with PrepareMissionSequence may save time when
 * preparing objects for use through the C interface.
 */
class CINTERFACE_API PrepareMissionSequence : public GmatCommand
{
public:
   PrepareMissionSequence();
   virtual ~PrepareMissionSequence();
   PrepareMissionSequence(const PrepareMissionSequence& PrepareMissionSequence);
   PrepareMissionSequence& operator=(const PrepareMissionSequence &bms);
   
   bool                 Execute();
   
   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual bool         RenameRefObject(const UnsignedInt type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual const std::string&  
                        GetGeneratingString(
                           Gmat::WriteMode mode = Gmat::SCRIPTING,
                           const std::string &prefix = "",
                           const std::string &useName = "");

   DEFAULT_TO_NO_CLONES
};


#endif // PrepareMissionSequence_hpp
