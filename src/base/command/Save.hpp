//$Id$
//------------------------------------------------------------------------------
//                                  Save
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2004/02/26
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Class implementation for the save command
 */
//------------------------------------------------------------------------------


#ifndef Save_hpp
#define Save_hpp

#include "GmatCommand.hpp"
#include <fstream>

/**
 * Command used to write objects to ASCII files.
 */
class GMAT_API Save : public GmatCommand
{
public:
   Save();
   virtual ~Save();
   Save(const Save& sv);
   Save&                operator=(const Save&);
   
   // inherited from GmatCommand
   virtual bool         Execute();
   virtual void         RunComplete();
   
   // inherited from GmatBase
   virtual bool         TakeAction(const std::string &action,  
                                   const std::string &actionData = "");
   virtual std::string  GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray&
                        GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool         SetRefObjectName(const Gmat::ObjectType type,
                                            const std::string &name);
   virtual bool         Initialize();
   
   virtual GmatBase*    Clone() const;
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName);
   
   
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
protected:
   /// Name of the save file -- for now, it is objectName.objectType
   StringArray          fileNameArray;
   /// Toggle to allow multiple writes
   bool                 appendData;
   /// Toggle to tell if file was written this run
   bool                 wasWritten;
   /// Name of the objects that are written
   StringArray          objNameArray;
   /// Pointer to the objects
   ObjectArray          objArray;
   /// Toggle to show or hide emply fields
   bool                 writeVerbose;
   /// File streams used for the output
   std::ofstream        *fileArray;
   
   void  UpdateOutputFileNames(Integer index, const std::string objName);
   void  WriteObject(UnsignedInt i, GmatBase *o);
};

#endif // Save_hpp
