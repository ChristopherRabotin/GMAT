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
#include "RungeKutta89.hpp"


//---------------------------------
// static data
//---------------------------------

const std::string
PropSetup::PARAMETER_TEXT[PropSetupParamCount] =

{
   "ForceModelName",
   "Type",              // To match the script spec
   "Drag"               // Place holder until we decide how to do this
};


const Gmat::ParameterType
PropSetup::PARAMETER_TYPE[PropSetupParamCount] =
{
   Gmat::STRING_TYPE,
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
   : GmatBase(Gmat::PROP_SETUP, "PropSetup", name),
     usedrag(true)
{
   // GmatBase data
   parameterCount = PropSetupParamCount;

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
   
   Initialize();
}

//------------------------------------------------------------------------------
// PropSetup(const PropSetup &propSetup)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 */
//------------------------------------------------------------------------------
PropSetup::PropSetup(const PropSetup &propSetup)
   : GmatBase(propSetup),
     usedrag(propSetup.usedrag)
{
   // PropSetup data
   mPropagator = propSetup.mPropagator;
   mForceModel = propSetup.mForceModel;
   Initialize();
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
      usedrag     = right.usedrag;
      Initialize();
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
    if (propagator == NULL)
       throw PropSetupException("PropSetup::SetPropagator failed: propagator is NULL");
       
    if (mPropagator->GetName() == "InternalRKV89") //loj: 3/12/04 added
        delete mPropagator;
    
    mPropagator = propagator;
    Initialize();
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
       throw PropSetupException("PropSetup::SetForceModel failed: ForceModel is NULL");
       
   if (mForceModel->GetName() == "InternalForceModel")
       delete mForceModel;
   
   mForceModel = forceModel;
   Initialize();
}

//------------------------------------------------------------------------------
// void SetUseDrag(bool flag)
//------------------------------------------------------------------------------
void PropSetup::SetUseDrag(bool flag)
{
    usedrag = flag;
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
   case USE_DRAG:
      return PropSetup::PARAMETER_TYPE[id];
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
   case USE_DRAG:
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
   case USE_DRAG:
      return PropSetup::PARAMETER_TEXT[id];
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
   for (int i=0; i<PropSetupParamCount; i++)
   {
      if (str == PropSetup::PARAMETER_TEXT[i])
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
      return "Undefined";
   case FORCE_MODEL_NAME:
      if (mForceModel)
          return mForceModel->GetName();
      return "InternalForceModel";
   case USE_DRAG:
      if (usedrag)
          return "BodyDefault";
      return "Off";
   default:
      return GmatBase::GetStringParameter(id);
   }
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
   switch (id)
   {
   /** @todo Check behavior of PropSetup::SetStringParm -- should be used to 
       change members, not just their names */
   case PROPAGATOR_NAME:
      return mPropagator->SetName(value);
   case FORCE_MODEL_NAME:
      return mForceModel->SetName(value);
   case USE_DRAG:
      if (value == "Off") {
         usedrag = false;
      }
      else {
         usedrag = true;
      }
      Initialize();
      return true;
      
   default:
      return GmatBase::SetStringParameter(id, value);
   }
}

//---------------------------------
// private methods
//---------------------------------

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
   mInitialized = true;

   if (mPropagator == NULL)
      mInitialized = false;

   if (mForceModel == NULL)
      mInitialized = false;
   else if (mForceModel->GetNumForces() == 0)
      mInitialized = false;
      
   if (mInitialized == true) {
      if (usedrag) {
         DragForce *dragForce = new DragForce;
         mForceModel->AddForce(dragForce);
      }
      mPropagator->SetPhysicalModel(mForceModel);
      mPropagator->Initialize();
   }
}

