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
// DJC 8/13/04: These are now part of the ForceModel
//   "Drag",               // Place holder until we decide how to do this
//   "SRP"
};


const Gmat::ParameterType
PropSetup::PARAMETER_TYPE[PropSetupParamCount - GmatBaseParamCount] =
{
   Gmat::STRING_TYPE,
   Gmat::STRING_TYPE
// DJC 8/13/04: These are now part of the ForceModel
//   Gmat::STRING_TYPE,
//   Gmat::STRING_TYPE
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
// DJC 8/13/04: These are now part of the ForceModel
//     usedrag      (false),
//     dragType     ("BodyDefault"),
//     //useSRP       ("Off") //loj: 5/11/04
//     useSRP       (false)
{
   // GmatBase data
   parameterCount = PropSetupParamCount;
   ownedObjectCount += 1;

   // PropSetup data
   /// @note: For build 1, the PropSetup internal objects are defaulted
   if (propagator != NULL)
       mPropagator = propagator;
   else 
       mPropagator = new RungeKutta89("InternalRKV89"); //loj: 3/12/04 added the name

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
   
   //Initialize(); //loj: 5/11/04
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
// DJC 8/13/04: These are now part of the ForceModel
//     usedrag(propSetup.usedrag),
//     useSRP(propSetup.useSRP)
{
   ownedObjectCount = propSetup.ownedObjectCount;

//   // PropSetup data
   mPropagator = propSetup.mPropagator;
   mForceModel = propSetup.mForceModel;
//   //Initialize(); //loj: 5/11/04

//   if (propSetup.mPropagator != NULL)
//      mPropagator = (Propagator *)propSetup.mPropagator->Clone();
//   if (propSetup.mForceModel != NULL)
//      mForceModel = (ForceModel *)propSetup.mForceModel->Clone();
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
      mPropagator = right.mPropagator;
      mForceModel = right.mForceModel;
// DJC 8/13/04: These are now part of the ForceModel
//      usedrag     = right.usedrag;
//      useSRP      = right.useSRP;
      //Initialize(); //loj: 5/11/04
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
      if (mPropagator->GetName() == "InternalRKV89")
         delete mPropagator;
   if (mForceModel)
      if (mForceModel->GetName() == "InternalForceModel")
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
   //MessageInterface::ShowMessage("PropSetup::GetPropagator() mPropagator=%d "
   //                              "name=%s\n", mPropagator, mPropagator->GetName().c_str());
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
   //MessageInterface::ShowMessage("PropSetup::SetPropagator() entered \n");
   if (propagator == NULL)
      throw PropSetupException("PropSetup::SetPropagator failed: propagator is NULL");
       
   //if (mPropagator->GetName() == "InternalRKV89")
   //   delete mPropagator; //loj: 5/18/04 problem deleting? cannot set from GUI
   
   mPropagator = propagator;
   //    Initialize();
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
   //MessageInterface::ShowMessage("PropSetup::SetForceModel() mForceModel=%s "
   //                              "forceModel=%s\n", mForceModel->GetName().c_str(),
   //                              forceModel->GetName().c_str());
   if (forceModel == NULL)
      throw PropSetupException("PropSetup::SetForceModel failed: ForceModel is NULL");
       
   //if (mForceModel->GetName() == "InternalForceModel")
   //   delete mForceModel; //loj: 5/18/04 problem deleting? cannot set from GUI
   
   mForceModel = forceModel;
   //    Initialize();
}

// DJC 8/13/04: These are now part of the ForceModel
//------------------------------------------------------------------------------
// void SetUseDrag(bool flag)
//------------------------------------------------------------------------------
//void PropSetup::SetUseDrag(bool flag)
//{
//   usedrag = flag;
//}

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


Integer PropSetup::GetParameterCount(void) const
{
   Integer count = parameterCount;

   // Increase the parameter count by the number of parameters in the members
   //    if (mForceModel) 
   //        count += mForceModel->GetParameterCount();
   //    if (mPropagator)
   //        count += mPropagator->GetParameterCount();

   return count;
}


//------------------------------------
// Inherited methods from GmatBase
//------------------------------------
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
// DJC 8/13/04: These are now part of the ForceModel
//   case USE_DRAG:
//   case USE_SRP:
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
// DJC 8/13/04: These are now part of the ForceModel
//   case USE_DRAG:
//   case USE_SRP:
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
// DJC 8/13/04: These are now part of the ForceModel
//   case USE_DRAG:
//   case USE_SRP:
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
// DJC 8/13/04: These are now part of the ForceModel
//   case USE_DRAG:
//      if (usedrag)
//         return dragType;
//      return "Off";
//   case USE_SRP:
//      if (useSRP)
//         return "On";
//      return "Off";
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
               SetForceModel(fm);
               return true;
            }
         }
         return false;
      }
// DJC 8/13/04: These are now part of the ForceModel
//   case USE_DRAG:
//      if (value == "Off") {
//         usedrag = false;
//      }
//      else {
//         usedrag = true;
//         dragType = value;
//      }
//      return true;
//   case USE_SRP:
//      if (value == "Off") {
//         useSRP = false;
//      }
//      else 
//         useSRP = true;
//      return true;
      
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
bool PropSetup::SetStringParameter(const std::string &label, const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
// void Initialize()
//------------------------------------------------------------------------------
/**
 * Sets mInitialized to true if pointers of Propagator and ForceModel are not
 * NULL and there is at least one Force in the ForceModel; false otherwise
 */
//------------------------------------------------------------------------------
void PropSetup::Initialize()
{
   //MessageInterface::ShowMessage("PropSetup::Initialize() entered \n");
   mInitialized = true;

   if (mPropagator == NULL)
   {
      MessageInterface::ShowMessage("PropSetup::Initialize() mPropagator is NULL\n");
      mInitialized = false;
   }
   
   if (mForceModel == NULL)
   {
      MessageInterface::ShowMessage("PropSetup::Initialize() mForceModel is NULL\n");
      mInitialized = false;
   }
   else if (mForceModel->GetNumForces() == 0)
   {
      MessageInterface::ShowMessage("PropSetup::Initialize() NumForces is 0\n");
      mInitialized = false;
   }
   
   //MessageInterface::ShowMessage("PropSetup::Initialize() initialized = %d\n",
   //                              mInitialized);
   
   if (mInitialized == true)
   {
// DJC 8/13/04: These are now part of the ForceModel
//      if (usedrag)
//      {
//         DragForce *dragForce = new DragForce;
//         mForceModel->AddForce(dragForce);
//         Integer id = dragForce->GetParameterID("AtmosphereModel");
//         dragForce->SetStringParameter(id, dragType);
//      }
//      
//      if (useSRP)
//      {
//         SolarRadiationPressure *srp = new SolarRadiationPressure("srp");
//         mForceModel->AddForce(srp);
//      }
      
      mPropagator->SetPhysicalModel(mForceModel);
      //MessageInterface::ShowMessage("PropSetup::Initialize() after SetPhysicalModel(%s) \n",
      //                              mForceModel->GetName().c_str());

      mPropagator->Initialize();
      //MessageInterface::ShowMessage("PropSetup::Initialize() after mPropagator->Initialize() \n");
   }
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

   if (mForceModel != NULL)
   {
      temp = mForceModel->GetName();
      if (temp == "") {
         fMName = instanceName + "_ForceModel";
         gen = mForceModel->GetGeneratingString(mode, prefix, fMName) + "\n";
      }
      else
         fMName = temp;
   }

   gen += GmatBase::GetGeneratingString(mode, prefix, useName);

   if (mPropagator != NULL)
   {
      temp = mPropagator->GetGeneratingString(mode, prefix, instanceName);
      unsigned loc = 0;
      if (temp.find("Create") != std::string::npos) // Skip the Create line
         loc = temp.find("\n");
      gen += temp.substr(loc+1);
   }
   
   generatingString = gen;
      
   return generatingString;
}
