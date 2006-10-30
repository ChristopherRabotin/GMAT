//$Header$
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
   Save&                  operator=(const Save&);
   
   virtual bool TakeAction(const std::string &action,  
                           const std::string &actionData = "");
   virtual std::string     GetRefObjectName(const Gmat::ObjectType type) const;
   virtual const StringArray&
                           GetRefObjectNameArray(const Gmat::ObjectType type);
   virtual bool            SetRefObjectName(const Gmat::ObjectType type,
                                            const std::string &name);
   virtual bool            Initialize();
   bool                    Execute();
   virtual void            RunComplete();
   
   // inherited from GmatBase
   virtual GmatBase*      Clone() const;
   virtual const std::string&
                          GetGeneratingString(Gmat::WriteMode mode,
                                              const std::string &prefix,
                                              const std::string &useName);


   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
protected:
   /// Name of the save file -- for now, it is objectName.objectType
   std::string            filename;
   /// Toggle to allow multiple writes
   bool                   appendData;
   /// Toggle to tell if file was written this run
   bool                   wasWritten;
   /// Name of the objects that are written
   StringArray            objName;
   /// Pointer to the objects
   ObjectArray            obj;
   /// Toggle to show or hide emply fields
   bool                   writeVerbose;
   /// File stream used for the output
   std::ofstream          file;

   void                   WriteObject(GmatBase *o);
   void                   WriteParameterValue(GmatBase *o, std::ofstream &file, 
                                              Integer id);
};

#endif // Save_hpp
