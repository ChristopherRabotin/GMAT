#ifndef SAVE_H
#define SAVE_H

#include "Command.hpp"
#include <fstream>

class Save : public GmatCommand
{
public:
	Save();
	virtual ~Save();
    Save(const Save& sv);
    Save&                  operator=(const Save&);
     
//    virtual bool           InterpretAction(void);
   virtual std::string     GetRefObjectName(const Gmat::ObjectType type) const;
   virtual bool            SetRefObjectName(const Gmat::ObjectType type,
                                            const std::string &name);
   virtual bool            Initialize(void);
   bool                    Execute(void);

    // inherited from GmatBase
    virtual GmatBase*      Clone(void) const;

protected:
    /// Name of the save file -- for now, it is objectName.objectType
    std::string            filename;
    /// Toggle to allow multiple writes
    bool                   append;
    /// Toggle to tell is file was written this run
    bool                   written;
    /// Name of the object that is written
    std::string            objName;
    /// Pointer to the object
    GmatBase               *obj;

    void                   WriteObject(void);
    void                   WriteParameterValue(std::ofstream &file, Integer id);
};

#endif // SAVE_H
