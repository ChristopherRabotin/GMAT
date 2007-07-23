//$Header$
//------------------------------------------------------------------------------
//                                  Subscriber
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// *** File Name : Subscriber.hpp
// *** Created   : May 20, 2003
// **************************************************************************
// ***  Developed By  :  Thinking Systems, Inc. (www.thinksysinc.com)     ***
// ***  For:  Flight Dynamics Analysis Branch (Code 595)                  ***
// ***  Under Contract:  P.O.  GSFC S-67573-G                             ***
// ***  Copyright Thinking Systems 2003                                   ***
// ***                                                                    ***
// ***  Header Version: July 12, 2002                                     ***
// **************************************************************************
// Module Type               : ANSI C++ Source
// Development Environment   : Visual C++ 7.0
// Modification History      : 5/20/2003 - D. Conway, Thinking Systems, Inc.
//                             Original delivery
//                             Source code not yet transferred to GSFC
//                           : 10/01/2003 - L. Ruley, Missions Applications Branch
//                             Updated style using GMAT cpp style guide
// **************************************************************************
//
/**
 * Definition for the Subscriber base class.
 */
//------------------------------------------------------------------------------
#ifndef Subscriber_hpp
#define Subscriber_hpp

#include "GmatBase.hpp"
#include "CoordinateSystem.hpp"
#include "ElementWrapper.hpp"


class GMAT_API Subscriber : public GmatBase
{
public:
   Subscriber(std::string typeStr, std::string nomme);
   Subscriber(const Subscriber &);
   Subscriber& operator=(const Subscriber &);
   virtual ~Subscriber(void);
   
   virtual bool Initialize();
   virtual bool ReceiveData(const char * datastream);
   virtual bool ReceiveData(const char * datastream, const Integer len);
   virtual bool ReceiveData(const Real * datastream, const Integer len = 0);
   virtual bool FlushData();
   virtual bool SetEndOfRun();
   
   Subscriber*  Next(void);
   bool Add(Subscriber *s);
   bool Remove(Subscriber *s, const bool del);
   
   void Activate(bool state = true);
   bool IsActive();
   
   virtual void SetProviderId(Integer id);
   virtual Integer GetProviderId();
   virtual void SetInternalCoordSystem(CoordinateSystem *cs);
   
   // methods for setting up the items to subscribe
   virtual const StringArray& GetWrapperObjectNameArray();
   virtual bool SetElementWrapper(ElementWrapper* toWrapper,
                                  const std::string &name);
   virtual void ClearWrappers();
   
   // methods for parameters
   virtual bool IsParameterReadOnly(const Integer id) const;
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer     GetParameterID(const std::string &str) const;
   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   
   virtual std::string GetStringParameter(const Integer id) const;
   virtual std::string GetStringParameter(const std::string &label) const;
   virtual bool SetStringParameter(const Integer id,
                                   const std::string &value);
   virtual bool Subscriber::SetStringParameter(const std::string &label,
                                               const std::string &value);
   virtual bool SetStringParameter(const Integer id, const std::string &value,
                                   const Integer index);
   virtual bool SetStringParameter(const std::string &label,
                                   const std::string &value,
                                   const Integer index);
   
   virtual std::string GetOnOffParameter(const Integer id) const;
   virtual bool        SetOnOffParameter(const Integer id, 
                                         const std::string &value);
   virtual std::string GetOnOffParameter(const std::string &label) const;
   virtual bool        SetOnOffParameter(const std::string &label, 
                                         const std::string &value);
   
   enum SolverIterOption
   {
      SI_ALL,
      SI_LAST,
      SI_NONE,
      SolverIterOptionCount
   };
   
   static Integer GetSolverIterOptionCount() { return SolverIterOptionCount; }
   static const std::string* GetSolverIterOptionList();
   
protected:
   
   std::string mSolverIterations;
   
   const char *data;
   Subscriber *next;
   CoordinateSystem *internalCoordSystem;
   
   bool        active;
   bool        isEndOfReceive;
   bool        isEndOfRun;
   Integer     currentProvider;
   
   /// The list of names of Wrapper objects
   StringArray wrapperObjectNames;
   /// vector of pointers to ElementWrappers for the item
   std::vector<ElementWrapper*> depParamWrappers;
   std::vector<ElementWrapper*> paramWrappers;
   
   bool SetWrapperReference(GmatBase *obj, const std::string &name);
   virtual bool Distribute(Integer len) = 0;
   virtual bool Distribute(const Real *dat, Integer len);
   
   enum
   {
      SOLVER_ITERATIONS = GmatBaseParamCount,
      TARGET_STATUS,
      SubscriberParamCount,
   };
   
   static const Gmat::ParameterType
      PARAMETER_TYPE[SubscriberParamCount - GmatBaseParamCount];
   static const std::string
      PARAMETER_TEXT[SubscriberParamCount - GmatBaseParamCount];
   
private:
   
   static const std::string SOLVER_ITER_OPTION_TEXT[SolverIterOptionCount];
   
};
#endif // Subscribe_hpp

