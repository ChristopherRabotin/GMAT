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

//---------------------------------
// static data
//---------------------------------

const std::string
PropSetup::PARAMETER_TEXT[PropSetupParamCount - GmatBaseParamCount] =

{
   "FM",
   "Type"              // To match the script spec
};


const Gmat::ParameterType
PropSetup::PARAMETER_TYPE[PropSetupParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE
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
      mPropagator = (Propagator *)propSetup.mPropagator->Clone();
   else
      mPropagator = NULL;
      
   if (propSetup.mForceModel != NULL)
      mForceModel = (ForceModel *)propSetup.mForceModel->Clone();
   else
      mForceModel = NULL;
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
         delete mPropagator;
      if (mForceModel != NULL)
         delete mForceModel;
         
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
      delete mPropagator;
   if (mForceModel)
      delete mForceModel;
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
      ("PropSetup::GetPropagator() mPropagator=%d name=%s\n",
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
   #if DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage("PropSetup::SetPropagator() entered \n");
   #endif
   
   if (propagator == NULL)
      throw PropSetupException(
         "PropSetup::SetPropagator failed: propagator is NULL");
   
   if (mPropagator != NULL)
      delete mPropagator;

   mPropagator = (Propagator*)propagator->Clone();
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
   #if DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage
      ("PropSetup::SetForceModel() mForceModel=%s forceModel=%s\n",
       mForceModel->GetName().c_str(), forceModel->GetName().c_str());
   #endif
   
   if (forceModel == NULL)
      throw PropSetupException(
         "PropSetup::SetForceModel failed: ForceModel is NULL");
   
   if (mForceModel != NULL)
      delete mForceModel;
   
   mForceModel = (ForceModel*)forceModel->Clone();
   
   #if DEBUG_PROPSETUP_SET
   MessageInterface::ShowMessage
      ("PropSetup::SetForceModel() after forceModel->Clone() mForceModel=%p\n",
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
   switch (id)
   {
   case PROPAGATOR_NAME:
   case FORCE_MODEL_NAME:
      return PropSetup::PARAMETER_TYPE[id - GmatBaseParamCount];
   default:
      return GmatBase::GetParameterType(id);
   }
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
   switch (id)
   {
   case PROPAGATOR_NAME:
   case FORCE_MODEL_NAME:
      return GmatBase::PARAM_TYPE_STRING[GetParameterType(id)];
   default:
      return GmatBase::GetParameterTypeString(id);
   }
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
   switch (id)
   {
   case PROPAGATOR_NAME:
   case FORCE_MODEL_NAME:
      return PropSetup::PARAMETER_TEXT[id - GmatBaseParamCount];
   default:
      return GmatBase::GetParameterText(id);
   }
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
               #if DEBUG_PROPSETUP_SET
               MessageInterface::ShowMessage
                  ("PropSetup::SetStringParameter() before SetForceModel() "
                   "mForceModel=%p\n", mForceModel);
               #endif
               
               SetForceModel(fm);
               
               #if DEBUG_PROPSETUP_SET
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
                     ("Unable to reconfigure Force model \"" + value + "\".");
               
            }
            else 
               throw PropSetupException("Force model '" + value + 
                  "' does not exist.");
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
         ("PropSetup::GetGeneratingString() fMName=%s, mForceModel=%p, "
          "showForceModel=%d\n", fMName.c_str(), mForceModel, showForceModel);
      #endif
      
      if (showForceModel)
         gen = mForceModel->GetGeneratingString(mode, prefix, fMName) + "\n";
   }
   
   gen += GmatBase::GetGeneratingString(mode, prefix, useName);
   generatingString = gen;
      
   return generatingString;
}
