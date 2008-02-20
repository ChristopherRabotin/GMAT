//$Header$
//------------------------------------------------------------------------------
//                              PropSetup
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/10/15
//
/**
 * Declares propagator setup operations.
 */
//------------------------------------------------------------------------------
#ifndef PropSetup_hpp
#define PropSetup_hpp

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Propagator.hpp"
#include "ForceModel.hpp"
#include "PhysicalModel.hpp"

class GMAT_API PropSetup : public GmatBase
{
public:

   PropSetup(const std::string &name, Propagator *propagator = NULL,
             ForceModel *forceModel = NULL);
   PropSetup(const PropSetup &propagatorSetup);
   PropSetup& operator= (const PropSetup &right); 
   virtual ~PropSetup();
   
   bool                 IsInitialized();
   Propagator*          GetPropagator();
   ForceModel*          GetForceModel();
   void                 SetPropagator(Propagator *propagator);
   void                 SetForceModel(ForceModel *forceModel);
   void                 SetUseDrag(bool flag);
   
   void                 AddForce(PhysicalModel *force);
   PhysicalModel*       GetForce(Integer index);
   Integer              GetNumForces();
   
   // inherited from GmatBase
   virtual bool         RenameRefObject(const Gmat::ObjectType type,
                                        const std::string &oldName,
                                        const std::string &newName);
   
   virtual GmatBase*    Clone(void) const;
   virtual void         Copy(const GmatBase* orig);
   
   virtual GmatBase*    GetOwnedObject(Integer whichOne);
   
   virtual const std::string*
                        GetParameterList() const;   
   virtual Integer      GetParameterCount() const;
   
   virtual Gmat::ParameterType
                        GetParameterType(const Integer id) const;
   virtual std::string  GetParameterTypeString(const Integer id) const;
   virtual std::string  GetParameterText(const Integer id) const;
   virtual Integer      GetParameterID(const std::string &str) const;
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   bool                 Initialize();
   
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");
   
   
private:

   bool mInitialized;
   Propagator *mPropagator;
   ForceModel *mForceModel;
   
   enum
   {
      FORCE_MODEL_NAME = GmatBaseParamCount,
      PROPAGATOR_NAME,
      PropSetupParamCount
   };

   static const std::string PARAMETER_TEXT[PropSetupParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[PropSetupParamCount - GmatBaseParamCount];

};

#endif // PropSetup_hpp
