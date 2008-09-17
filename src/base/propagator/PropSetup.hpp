//$Id$
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
   virtual bool         SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                                     const std::string &name = "");
   
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
   virtual bool         IsParameterReadOnly(const Integer id) const;
   virtual bool         IsParameterReadOnly(const std::string &label) const;
   
   virtual std::string  GetStringParameter(const Integer id) const;
   virtual std::string  GetStringParameter(const std::string &label) const;
   virtual bool         SetStringParameter(const Integer id,
                                           const std::string &value);
   virtual bool         SetStringParameter(const std::string &label,
                                           const std::string &value);
   
   virtual Real         GetRealParameter(const Integer id) const;
   virtual Real         GetRealParameter(const std::string &label) const;
   virtual Real         SetRealParameter(const Integer id,
                                         const Real value);
   virtual Real         SetRealParameter(const std::string &label,
                                         const Real value);
   
   virtual Integer      GetIntegerParameter(const Integer id) const;
   virtual Integer      GetIntegerParameter(const std::string &label) const;
   virtual Integer      SetIntegerParameter(const Integer id,
                                            const Integer value);
   virtual Integer      SetIntegerParameter(const std::string &label,
                                            const Integer value);
   
   bool                 Initialize();
   
   virtual const std::string&
                        GetGeneratingString(Gmat::WriteMode mode = Gmat::SCRIPTING,
                                            const std::string &prefix = "",
                                            const std::string &useName = "");
   
   
private:
   
   Integer GetOwnedObjectId(Integer id, Gmat::ObjectType objType) const;
   
   bool mInitialized;
   Propagator *mPropagator;
   ForceModel *mForceModel;
   
   enum
   {
      FORCE_MODEL_NAME = GmatBaseParamCount,
      PROPAGATOR_NAME,
      
      // owned object parameters
      INITIAL_STEP_SIZE,
      ACCURACY,
      ERROR_THRESHOLD,
      SMALLEST_INTERVAL,
      MIN_STEP,
      MAX_STEP,
      MAX_STEP_ATTEMPTS,
      LOWER_ERROR,
      TARGET_ERROR,
      PropSetupParamCount
   };
   
   static const std::string
      PARAMETER_TEXT[PropSetupParamCount - GmatBaseParamCount];
   static const Gmat::ParameterType
      PARAMETER_TYPE[PropSetupParamCount - GmatBaseParamCount];
   
};

#endif // PropSetup_hpp
