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
 * Defines propagator setup operations.
 */
//------------------------------------------------------------------------------

#include "PropSetup.hpp"
#include "PropSetupException.hpp"
#include "PhysicalModel.hpp"
#include "PointMassForce.hpp"
#include "DragForce.hpp"
#include "SolarRadiationPressure.hpp"
#include "RungeKutta89.hpp"
#include "Moderator.hpp"
#include "MessageInterface.hpp"

//#define DEBUG_PROPSETUP
//#define DEBUG_PROPSETUP_SET

//---------------------------------
// static data
//---------------------------------

/**
 * @note
 * Since we set some Propagator's property through PropSetup, such as
 * 'Propagator.InitialStepSize', properties owend by owning objects were
 * added here so that Validator can create corresponding element wrappers 
 * without going through owning object's property list to make Validator
 * job easy. The Validator will simply call GetParameterID() of PropSetup
 * to find out valid property or not. (LOJ: 2008.06.12)
 */

const std::string
PropSetup::PARAMETER_TEXT[PropSetupParamCount - GmatBaseParamCount] =

{
   "FM",
   "Type",
   "InitialStepSize",
   "Accuracy",
   "ErrorThreshold",
   "SmallestInterval",
   "MinStep",
   "MaxStep",
   "MaxStepAttempts",
   "LowerError",
   "TargetError",
};


const Gmat::ParameterType
PropSetup::PARAMETER_TYPE[PropSetupParamCount - GmatBaseParamCount] =
{
   Gmat::OBJECT_TYPE,  // "FM"
   Gmat::OBJECT_TYPE,  // "Type"
   Gmat::REAL_TYPE,    // "InitialStepSize",
   Gmat::REAL_TYPE,    // "Accuracy",
   Gmat::REAL_TYPE,    // "ErrorThreshold",
   Gmat::REAL_TYPE,    // "SmallestInterval",
   Gmat::REAL_TYPE,    // "MinStep",
   Gmat::REAL_TYPE,    // "MaxStep",
   Gmat::INTEGER_TYPE, // "MaxStepAttempts",
   Gmat::REAL_TYPE,    // "LowerError",
   Gmat::REAL_TYPE,    // "TargetError",
};

//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// PropSetup(const std::string &name,  Propagator *propagator,
//           ForceModel *forceModel)
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
PropSetup::PropSetup(const std::string &name, Propagator *propagator,
                     ForceModel *forceModel)
   : GmatBase     (Gmat::PROP_SETUP, "PropSetup", name)
{
   // GmatBase data
   objectTypes.push_back(Gmat::PROP_SETUP);
   objectTypeNames.push_back("PropSetup");
   
   parameterCount = PropSetupParamCount;
   ownedObjectCount += 1;
   
   // PropSetup data
   if (propagator != NULL)
       mPropagator = propagator;
   else 
       mPropagator = new RungeKutta89("");
   
   if (forceModel != NULL)
   {
       mForceModel = forceModel;
   }
   else
   {
       mForceModel = new ForceModel("InternalForceModel");
       PhysicalModel *pmf = new PointMassForce;
       mForceModel->AddForce(pmf);
   }
}

//------------------------------------------------------------------------------
// PropSetup(const PropSetup &propSetup)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
PropSetup::PropSetup(const PropSetup &propSetup)
   : GmatBase(propSetup)
{
   ownedObjectCount = propSetup.ownedObjectCount;
   
   // PropSetup data
   mInitialized = false;
   
   if (propSetup.mPropagator != NULL)
      mPropagator = (Propagator *)(propSetup.mPropagator->Clone());
   else
      mPropagator = NULL;
   
   if (propSetup.mForceModel != NULL)
      mForceModel = (ForceModel *)(propSetup.mForceModel->Clone());
   else
      mForceModel = NULL;
   
   #ifdef DEBUG_PROPSETUP
   MessageInterface::ShowMessage
      ("In PropSetup copy constructor, Cloned Propagator=<%p><%s>'%s'\n   "
       "Cloned ForceModel=<%p><%s>'%s'\n", mPropagator,
       mPropagator->GetTypeName().c_str(), mPropagator->GetName().c_str(),
       mForceModel,  mForceModel->GetTypeName().c_str(),
       mForceModel->GetName().c_str());
   #endif
}

//------------------------------------------------------------------------------
// PropSetup& operator= (const PropSetup &right)
//------------------------------------------------------------------------------
/**
 * Assignment operator.
 */
//------------------------------------------------------------------------------
PropSetup& PropSetup::operator= (const PropSetup &right)
{
   if (this != &right)
   {
      GmatBase::operator=(right);
      
      // PropSetup data
      mInitialized = false;
      
      if (mPropagator != NULL)
      {
         #ifdef DEBUG_PROPSETUP
         MessageInterface::ShowMessage
            ("PropSetup::operator= Deleting Propagator <%p><%s>'%s'\n", mPropagator,
             mPropagator->GetTypeName().c_str(), mPropagator->GetName().c_str());
         #endif
         delete mPropagator;
      }
      
      if (mForceModel != NULL)
      {
         #ifdef DEBUG_PROPSETUP
         MessageInterface::ShowMessage
            ("PropSetup::operator= Deleting ForceModel <%p><%s>'%s'\n", mForceModel,
             mForceModel->GetTypeName().c_str(), mForceModel->GetName().c_str());
         #endif
         delete mForceModel;
      }
      
      if (right.mPropagator != NULL)
         mPropagator = (Propagator*)(right.mPropagator->Clone());
      else
         mPropagator = NULL;
      
      if (right.mForceModel != NULL)
         mForceModel = (ForceModel*)(right.mForceModel->Clone());
      else
         mForceModel = NULL;
   }
   
   return *this;
}

//------------------------------------------------------------------------------
// virtual ~PropSetup()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
PropSetup::~PropSetup()
{
   if (mPropagator)
   {
      #ifdef DEBUG_PROPSETUP
      MessageInterface::ShowMessage
         ("PropSetup::~PropSetup() Deleting Propagator <%p><%s>'%s'\n", mPropagator,
          mPropagator->GetTypeName().c_str(), mPropagator->GetName().c_str());
      #endif
      delete mPropagator;
   }
   
   if (mForceModel)
   {
      #ifdef DEBUG_PROPSETUP
      MessageInterface::ShowMessage
         ("PropSetup::~PropSetup() Deleting ForceModel <%p><%s>'%s'\n", mForceModel,
          mForceModel->GetTypeName().c_str(), mForceModel->GetName().c_str());
      #endif
      delete mForceModel;
   }
}

//------------------------------------------------------------------------------
// bool IsInitialized()
//------------------------------------------------------------------------------
/**
 * @return true if pointers of Propagator and ForceModel are not NULL and
 *    there is at least one Force in the ForceModel; false otherwise.
 */
//------------------------------------------------------------------------------
bool PropSetup::IsInitialized()
{
   return mInitialized;
}

//------------------------------------------------------------------------------
// Propagator* GetPropagator()
//------------------------------------------------------------------------------
/**
 *@return internal Propagator pointer
 */
//------------------------------------------------------------------------------
Propagator* PropSetup::GetPropagator()
{
   #if DEBUG_PROPSETUP_GET
   MessageInterface::ShowMessage
      ("PropSetup::GetPropagator() mPropagator=<%p>, name='%s'\n",
       mPropagator, mPropagator->GetName().c_str());
   #endif
   
   return mPropagator;
}

//------------------------------------------------------------------------------
// ForceModel* GetForceModel()
//------------------------------------------------------------------------------
/**
 *@return internal ForceModel pointer
 */
//------------------------------------------------------------------------------
ForceModel* PropSetup::GetForceModel()
{
   return mForceModel;
}

//------------------------------------------------------------------------------
// void SetPropagator(Propagator *propagator)
//------------------------------------------------------------------------------
/**
 * Sets internal propagator pointer to given propagator.
 *
 *@param <*propagator> propagator pointer to set internal propagator to
 */
//------------------------------------------------------------------------------
void PropSetup::SetPropagator(Propagator *propagator)
{
   #ifdef DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage("PropSetup::SetPropagator() entered \n");
   #endif
   
   if (propagator == NULL)
      throw PropSetupException("SetPropagator() failed: propagator is NULL");
   
   if (mPropagator != NULL)
      delete mPropagator;
   
   mPropagator = (Propagator*)(propagator->Clone());
}

//------------------------------------------------------------------------------
// void SetForceModel(ForceModel *forceModel)
//------------------------------------------------------------------------------
/**
 * Sets internal force model pointer to given force model.
 *
 *@param <*forceModel> ForceModel pointer to set internal force model to
 */
//------------------------------------------------------------------------------
void PropSetup::SetForceModel(ForceModel *forceModel)
{
   if (forceModel == NULL)
      throw PropSetupException("SetForceModel() failed: ForceModel is NULL");
   
   #ifdef DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage
      ("PropSetup::SetForceModel() current ForceModel=<%p>'%s'\n   new ForceModel=<%p>'%s'\n",
       mForceModel, mForceModel->GetName().c_str(), forceModel, forceModel->GetName().c_str());
   #endif
   
   if (mForceModel != NULL)
   {
      #ifdef DEBUG_PROPSETUP_SET
      MessageInterface::ShowMessage
         ("   Deleting ForceModel <%p><%s>'%s'\n", mForceModel,
          mForceModel->GetTypeName().c_str(), mForceModel->GetName().c_str());
      #endif
      mForceModel = NULL;
   }
   
   mForceModel = (ForceModel*)(forceModel->Clone());
   
   #ifdef DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage
      ("PropSetup::SetForceModel() after forceModel->Clone() mForceModel=<%p>\n",
       mForceModel);
   #endif
}


//------------------------------------------------------------------------------
// void AddForce(PhysicalModel *force)
//------------------------------------------------------------------------------
/**
 * Adds a force to force model.
 */
//------------------------------------------------------------------------------
void PropSetup::AddForce(PhysicalModel *force)
{
   mForceModel->AddForce(force);
}


// future implementation
//------------------------------------------------------------------------------
// void RemoveForce(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Removes a force from force model.
 */
//------------------------------------------------------------------------------
//  void PropSetup::RemoveForce(const std::string &name)
//  {  
//  }


//------------------------------------------------------------------------------
// PhysicalModel* GetForce(Integer index)
//------------------------------------------------------------------------------
/**
 * @return Force pointer give by index, NULL if invalid index
 */
//------------------------------------------------------------------------------
PhysicalModel* PropSetup::GetForce(Integer index)
{
   return mForceModel->GetForce(index);      
}


//------------------------------------------------------------------------------
// Integer GetNumForces()
//------------------------------------------------------------------------------
/**
 * @return number of forces in the force model
 */
//------------------------------------------------------------------------------
Integer PropSetup::GetNumForces()
{
   return mForceModel->GetNumForces();
}

//------------------------------------------------------------------------------
// const std::string* GetParameterList() const
//------------------------------------------------------------------------------
/**
 * @return pointer to parameter name list
 */
//------------------------------------------------------------------------------
const std::string* PropSetup::GetParameterList() const
{
   return PARAMETER_TEXT;
}


//------------------------------------------------------------------------------
// Integer GetParameterCount(void) const
//------------------------------------------------------------------------------
Integer PropSetup::GetParameterCount(void) const
{
   Integer count = parameterCount;
   return count;
}


//------------------------------------
// Inherited methods from GmatBase
//------------------------------------

//---------------------------------------------------------------------------
//  bool RenameRefObject(const Gmat::ObjectType type,
//                       const std::string &oldName, const std::string &newName)
//---------------------------------------------------------------------------
/**
 * Renames reference objects used in this class.
 *
 * @param <type> reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 * 
 * @return true if object name changed, false if not.
 */
//---------------------------------------------------------------------------
bool PropSetup::RenameRefObject(const Gmat::ObjectType type,
                                const std::string &oldName,
                                const std::string &newName)
{
   // There is nothing to check for now
   return true;
}


//---------------------------------------------------------------------------
// virtual bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
//                           const std::string &name = "");
//---------------------------------------------------------------------------
/*
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool PropSetup::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name)
{
   #ifdef DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage
      ("PropSetup::SetRefObject() entered, obj=<%p><%s><%s>, type=%d, name='%s'\n",
       obj, obj ? obj->GetTypeName().c_str() : "NULL",
       obj ? obj->GetName().c_str() : "NULL", type, name.c_str());
   #endif
   
   if (obj == NULL)
      return false;
   
   switch (type)
   {
   case Gmat::PROPAGATOR:
      SetPropagator((Propagator*)obj);
      return true;
   case Gmat::FORCE_MODEL:
      SetForceModel((ForceModel*)obj);
      return true;;
   default:
      return false;
   }
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the PropSetup.
 *
 * @return clone of the PropSetup.
 *
 */
//------------------------------------------------------------------------------
GmatBase* PropSetup::Clone(void) const
{
   return (new PropSetup(*this));
}


//---------------------------------------------------------------------------
//  void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void PropSetup::Copy(const GmatBase* orig)
{
   // We don't want to copy instanceName
   std::string name = instanceName;
   operator=(*((PropSetup *)(orig)));
   instanceName = name;
}


//------------------------------------------------------------------------------
//  GmatBase* GetOwnedObject(Integer whichOne)
//------------------------------------------------------------------------------
/**
 * This method returns the unnamed objects owned by the PropSetup.
 *
 * The current implementation only contains one PropSetup owned object: the 
 * Propagator.
 * 
 * @return Pointer to the owned object.
 */
//------------------------------------------------------------------------------
GmatBase* PropSetup::GetOwnedObject(Integer whichOne)
{
   if (whichOne == ownedObjectCount - 1)   // If this works, make it more usable
      return mPropagator;
   return GmatBase::GetOwnedObject(whichOne);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Gmat::ParameterType PropSetup::GetParameterType(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < PropSetupParamCount)
      return PARAMETER_TYPE[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterType(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PropSetup::GetParameterTypeString(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < PropSetupParamCount)
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   else
      return GmatBase::GetParameterTypeString(id);
}

//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PropSetup::GetParameterText(const Integer id) const
{
   if (id >= GmatBaseParamCount && id < PropSetupParamCount)
      return PARAMETER_TEXT[id - GmatBaseParamCount];
   else
      return GmatBase::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string str)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
Integer PropSetup::GetParameterID(const std::string &str) const
{
   for (int i=GmatBaseParamCount; i<PropSetupParamCount; i++)
   {
      if (str == PropSetup::PARAMETER_TEXT[i - GmatBaseParamCount])
         return i;
   }
   
   return GmatBase::GetParameterID(str);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool PropSetup::IsParameterReadOnly(const Integer id) const
{
   if (id == FORCE_MODEL_NAME || id == PROPAGATOR_NAME)
      return false;
   else if (id >= INITIAL_STEP_SIZE && id <= TARGET_ERROR)
      return true;
   else
      return GmatBase::IsParameterReadOnly(id);
}


//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const std::string &label) const
//---------------------------------------------------------------------------
/**
 * Checks to see if the requested parameter is read only.
 *
 * @param <label> Description for the parameter.
 *
 * @return true if the parameter is read only, false (the default) if not.
 */
//---------------------------------------------------------------------------
bool PropSetup::IsParameterReadOnly(const std::string &label) const
{
   return IsParameterReadOnly(GetParameterID(label));
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PropSetup::GetStringParameter(const Integer id) const
{
   switch (id)
   {
   case PROPAGATOR_NAME:
      if (mPropagator)
         return mPropagator->GetTypeName();
      return "UndefinedPropagator";
   case FORCE_MODEL_NAME:
      if (mForceModel) {
         std::string nomme = mForceModel->GetName();
         if (nomme == "")
            nomme = instanceName + "_ForceModel";
         return nomme;
      }
      return "InternalForceModel";
   default:
      return GmatBase::GetStringParameter(id);
   }
}

//------------------------------------------------------------------------------
// std::string GetStringParameter(const std::string &label)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
std::string PropSetup::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool PropSetup::SetStringParameter(const Integer id, const std::string &value)
{
   Moderator *theModerator = Moderator::Instance();
   
   switch (id)
   {
      /** @todo Check behavior of PropSetup::SetStringParm -- should be used to 
          change members, not just their names */
   case PROPAGATOR_NAME:
      {
         Propagator *prop = theModerator->CreatePropagator(value, "");
         if (prop)
         {
            SetPropagator(prop);
            return true;
         }
         else
         {
            return false;
         }
      }
   case FORCE_MODEL_NAME:
      {
         if (value == "InternalForceModel")
         {
            mForceModel = new ForceModel("InternalForceModel");
            PhysicalModel *pmf = new PointMassForce;
            mForceModel->AddForce(pmf);
            return true;
         }
         else
         {
            ForceModel *fm = theModerator->GetForceModel(value);
            if (fm)
            {
               #ifdef DEBUG_PROPSETUP_SET
               MessageInterface::ShowMessage
                  ("PropSetup::SetStringParameter() before SetForceModel() "
                   "mForceModel=<%p>\n", mForceModel);
               #endif
               
               SetForceModel(fm);
               
               #ifdef DEBUG_PROPSETUP_SET
               MessageInterface::ShowMessage
                  ("PropSetup::SetStringParameter() after  SetForceModel() "
                   "mForceModel=%p\n", mForceModel);
               #endif
               
               // Since the ForceModel is cloned in SetForce(), we need to reset
               // the configured ForceModel pointer to cloned pointer in order to
               // get correct generating string when saving mission. (loj: 1/30/07)
               if (theModerator->ReconfigureItem(mForceModel, value))
                  return true;
               else
                  throw PropSetupException
                     ("SetStringParameter() unable to reconfigure ForceModel \"" + value + "\".");
               
            }
            else 
               throw PropSetupException
                  ("SetStringParameter() unable to set ForceModel \"" + value +
                   "\". It does not exist.");
         }
         return false;
      }
      
   default:
      return GmatBase::SetStringParameter(id, value);
   }
}

//------------------------------------------------------------------------------
// bool SetStringParameter(const std::string &label, const std::string &value)
//------------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//------------------------------------------------------------------------------
bool PropSetup::SetStringParameter(const std::string &label, 
                                   const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
/*
 * This method provides call-through to propagator
 */
//------------------------------------------------------------------------------
Real PropSetup::GetRealParameter(const Integer id) const
{
   switch (id)
   {
      case ACCURACY:
      case INITIAL_STEP_SIZE:
      case ERROR_THRESHOLD:
      case SMALLEST_INTERVAL:
      case MIN_STEP:
      case MAX_STEP:
      case MAX_STEP_ATTEMPTS:
      case LOWER_ERROR:
      case TARGET_ERROR:
         {
            // Get actual id
            Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
            return mPropagator->GetRealParameter(actualId);
         }
   default:
      return GmatBase::GetRealParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Real GetRealParameter(const std::string &label) const
//------------------------------------------------------------------------------
Real PropSetup::GetRealParameter(const std::string &label) const
{
   return GetRealParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
/*
 * This method provides call-through to propagator
 */
//------------------------------------------------------------------------------
Real PropSetup::SetRealParameter(const Integer id, const Real value)
{
   switch (id)
   {
      case ACCURACY:
      case INITIAL_STEP_SIZE:
      case ERROR_THRESHOLD:
      case SMALLEST_INTERVAL:
      case MIN_STEP:
      case MAX_STEP:
      case MAX_STEP_ATTEMPTS:
      case LOWER_ERROR:
      case TARGET_ERROR:
         {
            // Get actual id
            Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
            return mPropagator->SetRealParameter(actualId, value);
         }
   default:
      return GmatBase::SetRealParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Real SetRealParameter(const std::string &label, const Real value)
//------------------------------------------------------------------------------
Real PropSetup::SetRealParameter(const std::string &label, const Real value)
{
   return SetRealParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer PropSetup::GetIntegerParameter(const Integer id) const
{
   switch (id)
   {
      case MAX_STEP_ATTEMPTS:
         {
            // Get actual id
            Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
            return mPropagator->GetIntegerParameter(actualId);
         }
   default:
      return GmatBase::GetIntegerParameter(id);
   }
}


//------------------------------------------------------------------------------
// virtual Integer GetIntegerParameter(const std::string &label) const
//------------------------------------------------------------------------------
Integer PropSetup::GetIntegerParameter(const std::string &label) const
{
   return GetIntegerParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer PropSetup::SetIntegerParameter(const Integer id, const Integer value)
{
   switch (id)
   {
      case MAX_STEP_ATTEMPTS:
         {
            // Get actual id
            Integer actualId = GetOwnedObjectId(id, Gmat::PROPAGATOR);
            return mPropagator->SetIntegerParameter(actualId, value);
         }
   default:
      return GmatBase::SetIntegerParameter(id, value);
   }
}


//------------------------------------------------------------------------------
// virtual Integer SetIntegerParameter(const std::string &label, const Integer value)
//------------------------------------------------------------------------------
Integer PropSetup::SetIntegerParameter(const std::string &label, const Integer value)
{
   return SetIntegerParameter(GetParameterID(label), value);
}


//------------------------------------------------------------------------------
// bool Initialize()
//------------------------------------------------------------------------------
/**
 * Sets mInitialized to true if pointers of Propagator and ForceModel are not
 * NULL and there is at least one Force in the ForceModel; false otherwise
 */
//------------------------------------------------------------------------------
bool PropSetup::Initialize()
{
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage("PropSetup::Initialize() entered \n");
   #endif
   mInitialized = true;

   if (mPropagator == NULL)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
            "PropSetup::Initialize() mPropagator is NULL\n");
      #endif
      mInitialized = false;
   }
   
   if (mForceModel == NULL)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
            "PropSetup::Initialize() mForceModel is NULL\n");
      #endif
      mInitialized = false;
   }
   else if (mForceModel->GetNumForces() == 0)
   {
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
            "PropSetup::Initialize() NumForces is 0\n");
      #endif
      mInitialized = false;
   }
   
   #ifdef DEBUG_INITIALIZATION
      MessageInterface::ShowMessage(
         "PropSetup::Initialize() initialized = %d\n", mInitialized);
   #endif
   
   if (mInitialized == true)
   {
      mPropagator->SetPhysicalModel(mForceModel);
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
            "PropSetup::Initialize() after SetPhysicalModel(%s) \n",
            mForceModel->GetName().c_str());
      #endif

      mPropagator->Initialize();
      
      #ifdef DEBUG_INITIALIZATION
         MessageInterface::ShowMessage(
            "PropSetup::Initialize() after mPropagator->Initialize() \n");
      #endif
   }
   
   return true;
}


//------------------------------------------------------------------------------
// const std::string& GetGeneratingString(Gmat::WriteMode mode,
//            const std::string &prefix, const std::string &useName)
//------------------------------------------------------------------------------
/**
 * Provides special handling for the scripting for PropSetups.
 *
 * @param mode Specifies the type of serialization requested.
 * @param prefix Optional prefix appended to the object's name
 * @param useName Name that replaces the object's name.
 *
 * @return A string containing the scrit used to construct the PropSetup.
 */
//------------------------------------------------------------------------------
const std::string& PropSetup::GetGeneratingString(Gmat::WriteMode mode,
            const std::string &prefix, const std::string &useName)
{
   std::string gen, fMName = "", temp;
   bool showForceModel = false;
   if (mForceModel != NULL)
   {
      temp = mForceModel->GetName();
      if (temp == "")
      {
         fMName = instanceName + "_ForceModel";
         showForceModel = true;
      }
      else
         fMName = temp;
      
      if (mode == Gmat::SHOW_SCRIPT)
         showForceModel = true;
      
      #if DEBUG_PROPSETUP_GEN_STRING
      MessageInterface::ShowMessage
         ("PropSetup::GetGeneratingString() fMName='%s', mForceModel=<%p>, "
          "showForceModel=%d\n", fMName.c_str(), mForceModel, showForceModel);
      #endif
      
      if (showForceModel)
         gen = mForceModel->GetGeneratingString(mode, prefix, fMName) + "\n";
   }
   
   gen += GmatBase::GetGeneratingString(mode, prefix, useName);
   generatingString = gen;
      
   return generatingString;
}


//---------------------------------
// private methods
//---------------------------------

//------------------------------------------------------------------------------
// Integer GetOwnedObjectId(Integer id, Gmat::ObjectType objType) const
//------------------------------------------------------------------------------
Integer PropSetup::GetOwnedObjectId(Integer id, Gmat::ObjectType objType) const
{
   Integer actualId = -1;
   
   if (mPropagator == NULL || mForceModel == NULL)
      throw PropSetupException
         ("PropSetup::GetOwnedObjectId() failed: Propagator or ForceModel is NULL");
   
   try
   {
      if (objType == Gmat::PROPAGATOR)
         actualId = mPropagator->GetParameterID(GetParameterText(id));
      else if (objType == Gmat::FORCE_MODEL)
         actualId = mForceModel->GetParameterID(GetParameterText(id));
   }
   catch (BaseException &e)
   {
      throw;
   }
   
   return actualId;
}

