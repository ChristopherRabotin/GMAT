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

   bool IsInitialized();
   Propagator* GetPropagator();
   ForceModel* GetForceModel();
   void SetPropagator(Propagator *propagator);
   void SetForceModel(ForceModel *forceModel);

   void AddForce(PhysicalModel *force);
   void DeleteForce(const std::string &name);
   PhysicalModel* GetForce(const std::string &name);
   PhysicalModel* GetForce(Integer index); //loj: 2/11/04 added
   Integer GetNumForces();

   virtual const std::string* GetParameterList() const;

   // The inherited methods from GmatBase
   virtual Integer GetParameterCount(void) const;

   virtual Gmat::ParameterType GetParameterType(const Integer id) const;
   virtual std::string GetParameterTypeString(const Integer id) const;
   virtual std::string GetParameterText(const Integer id) const;
   virtual Integer GetParameterID(const std::string &str) const;
   virtual std::string GetStringParameter(const Integer id) const;
   virtual bool SetStringParameter(const Integer id, const std::string &value);

private:

   void Initialize();

   bool mInitialized;
   Propagator *mPropagator;
   ForceModel *mForceModel;
   
   bool usedrag;

   enum
   {
      FORCE_MODEL_NAME = 0,
      PROPAGATOR_NAME,
      USE_DRAG,
      PropSetupParamCount
   };

   static const std::string PARAMETER_TEXT[PropSetupParamCount];
   static const Gmat::ParameterType PARAMETER_TYPE[PropSetupParamCount];

};

#endif // PropSetup_hpp
