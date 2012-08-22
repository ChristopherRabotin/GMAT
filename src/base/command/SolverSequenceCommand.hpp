/*
 * SolverSequenceCommand.hpp
 *
 *  Created on: Aug 22, 2012
 *      Author: djc
 */

#ifndef SOLVERSEQUENCECOMMAND_HPP_
#define SOLVERSEQUENCECOMMAND_HPP_

#include "GmatCommand.hpp"


/**
 * Base class for commands specific to Solver Control Sequences
 *
 * This code is a partial refactorization of the Vary, Achieve, Minimize,
 * and NonlinearConstraint commands to provide them with a common interface
 * for accessing the name of the Solver associated with each command.  Full
 * refactorization should be performed when schedule permits.
 */
class SolverSequenceCommand : public GmatCommand
{
public:
   SolverSequenceCommand(const std::string &cmdType);
   virtual ~SolverSequenceCommand();
   SolverSequenceCommand(const SolverSequenceCommand& ssc);
   SolverSequenceCommand& operator=(const SolverSequenceCommand& ssc);

   // for Ref Objects
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
//
//   virtual const ObjectTypeArray&
//                        GetRefObjectTypeArray();
//   virtual const StringArray&
//                        GetRefObjectNameArray(const Gmat::ObjectType type);

   // Parameter accessors
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;

   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const Integer id,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value,
                                           const Integer index);

   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label,
                                           const Integer index) const;
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value,
                                           const Integer index);

protected:
   /// Name of the Solver used by the command
   std::string solverName;

   // Parameter IDs
   enum
   {
      SOLVER_NAME = GmatCommandParamCount,
      SolverSequenceCommandParamCount
   };

   static const std::string    PARAMETER_TEXT[SolverSequenceCommandParamCount -
                                              GmatCommandParamCount];
   static const Gmat::ParameterType
                               PARAMETER_TYPE[SolverSequenceCommandParamCount -
                                              GmatCommandParamCount];
};

#endif /* SOLVERSEQUENCECOMMAND_HPP_ */
