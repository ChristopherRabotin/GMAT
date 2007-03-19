//$Header$
//------------------------------------------------------------------------------
//                                  ForFor
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number NNG04CC06P
//
// Author:  Joey Gurgnaus
// Created: 2004/01/22
// Modified: W. Shoan 2004.09.13 - updated for use in Build 3
//
/**
 * Definition for the For command class 
 */
//------------------------------------------------------------------------------


#ifndef For_hpp
#define For_hpp
 

#include "BranchCommand.hpp"
#include "Parameter.hpp"
#include "ElementWrapper.hpp"


/**
 * Command that manages processing for entry to the For loop
 *
 * The For command manages the for loop.  
 *
 */
class For : public BranchCommand
{
public:
  // default constructor
   For();
   // copy constructor
   For(const For& f);
   // destructor
   virtual ~For();

   // operator = 
   For&                 operator=(const For& f);

   // Inherited methods that need some enhancement from the base class
   virtual bool         Append(GmatCommand *cmd);

   // Methods used to run the command
   virtual bool         Initialize();
   virtual bool         Execute();
   virtual void         RunComplete();

   // inherited from GmatBase
   virtual GmatBase*    Clone() const;
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode,
                                            const std::string &prefix,
                                            const std::string &useName);

   virtual GmatBase*    GetRefObject(const Gmat::ObjectType type,
                                     const std::string &name);
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name);
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   virtual const ObjectTypeArray&
                       GetRefObjectTypeArray();
   virtual const StringArray&
                       GetRefObjectNameArray(const Gmat::ObjectType type);
   
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual bool         SetStringParameter(const Integer id, 
                                           const std::string &value);
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const std::string &label, 
                                           const std::string &value);
   //virtual bool         GetBooleanParameter(const Integer id) const; 
   //virtual bool         GetBooleanParameter(const std::string &label) const; 
   
   virtual const StringArray& 
                       GetWrapperObjectNameArray();
   virtual bool        SetElementWrapper(ElementWrapper* toWrapper,
                                         const std::string &withName);
   virtual void        ClearWrappers();
    
protected:
   enum
   {
      START_VALUE = BranchCommandParamCount,
      END_VALUE,
      STEP,
      INDEX_NAME,
      START_NAME,
      END_NAME,
      INCREMENT_NAME,
      //INDEX_IS_PARAM,
      //START_IS_PARAM,
      //END_IS_PARAM,
      //INCR_IS_PARAM,
      ForParamCount
   };

   static const std::string PARAMETER_TEXT[
      ForParamCount - BranchCommandParamCount];

   static const Gmat::ParameterType PARAMETER_TYPE[
      ForParamCount - BranchCommandParamCount];
   
   static const Real UNINITIALIZED_VALUE;
   static const Real DEFAULT_START;
   static const Real DEFAULT_END;
   static const Real DEFAULT_INCREMENT;

   //std::string  forName;
   /// Start value for the For loop
   Real         startValue;
   /// End value for the For loop
   Real         endValue;
   /// Step value for the For loop
   Real         stepSize;
   /// Current value for the For loop counter
   Real         currentValue;

   //Parameter* indexParam;
   //Parameter* startParam;
   //Parameter* endParam;
   //Parameter* incrParam;
   ElementWrapper *index;
   ElementWrapper *start;
   ElementWrapper *end;
   ElementWrapper *increment;
   
   std::string indexName;
   std::string startName;
   std::string endName;
   std::string incrName;
   
   //std::string startParamName;
   //std::string endParamName;
   //std::string incrParamName;
   
   //bool indexIsParam;
   //bool startIsParam;
   //bool endIsParam;
   //bool incrIsParam;
   
   // method to evaluate the counter to see if we are still looping
   bool StillLooping();
   //Parameter* GetArrayIndex(const std::string &arrayStr,
   //                         Integer &row, Integer &col);
};
#endif  // For_hpp
