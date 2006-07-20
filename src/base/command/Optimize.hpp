#ifndef OPTIMIZE_HPP_
#define OPTIMIZE_HPP_

#include "BranchCommand.hpp"
#include "Solver.hpp"
#include "Spacecraft.hpp"

class GMAT_API Optimize : public BranchCommand
{
public:
   Optimize();
   virtual ~Optimize();
    
   Optimize(const Optimize& o);
   Optimize& operator=(const Optimize& o);
    
   // Inherited methods that need some enhancement from the base class
   virtual bool Append(GmatCommand *cmd);


   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   const std::string&   GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName);

   virtual bool RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName);
   
   // Parameter access methods
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                       GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool        SetStringParameter(const Integer id, 
                                          const std::string &value);
   virtual bool        GetBooleanParameter(const Integer id) const;
   virtual std::string GetRefObjectName(const Gmat::ObjectType type) const;
   virtual bool        SetRefObjectName(const Gmat::ObjectType type,
                                        const std::string &name);
    
   // Methods used to run the command
   virtual bool        Initialize(void);
   virtual bool        Execute(void);

protected:
   /// The name of the spacecraft that gets maneuvered
   std::string         optimizerName;
   /// The optimizer instance used to manage the optimizer state machine
   Solver              *optimizer;
   /// Local store of the objects that we'll need to reset
   ObjectArray         localStore;
   /// Flag indicating is the optimizer has converged
   bool                optimizerConverged;
    
   // Parameter IDs 
   Integer             optimizerNameID;
   Integer             OptimizerConvergedID;
   bool                optimizerInDebugMode;
    
   // Methods used to save the starting point for the loops
   virtual void        StoreLoopData();
   virtual void        ResetLoopData();
   virtual void        FreeLoopData();
};

#endif /*OPTIMIZE_HPP_*/
