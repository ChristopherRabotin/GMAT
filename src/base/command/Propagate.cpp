//$Header$
//------------------------------------------------------------------------------
//                                 Propagate
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// **Legal**
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway
// Created: 2003/09/23
//
/**
 * Implementation for the Propagate command class
 */
//------------------------------------------------------------------------------

#include "Propagate.hpp"
#include "Publisher.hpp"
#include "Moderator.hpp"
#include "Parameter.hpp"
#include "MessageInterface.hpp"

#define DEBUG_PROPAGATE_OBJ 0
#define DEBUG_PROPAGATE_EXE 0

//------------------------------------------------------------------------------
//  Propagate(void)
//------------------------------------------------------------------------------
/**
 * Constructs the Propagate Command (default constructor).
 */
//------------------------------------------------------------------------------
Propagate::Propagate(void) :
   GmatCommand                 ("Propagate"),
   propName                    (""),
   propCoupled                 (true),
   interruptCheckFrequency     (30),
   prop                        (NULL),
   // Set the parameter IDs
   propCoupledID               (parameterCount),
   interruptCheckFrequencyID   (parameterCount+1),
   satNameID                   (parameterCount+2),
   propNameID                  (parameterCount+3),
   secondsToProp               (8640.0),
   secondsToPropID             (parameterCount+4)
{
   // Increase the number of parms by the 2 new ones
   parameterCount += 5;
}


//------------------------------------------------------------------------------
//  ~Propagate(void)
//------------------------------------------------------------------------------
/**
 * Destroys the Propagate Command.
 */
//------------------------------------------------------------------------------
Propagate::~Propagate(void)
{
   for (unsigned int i=0; i<stopWhen.size(); i++)
      delete stopWhen[i];
}


//------------------------------------------------------------------------------
//  Propagate(void)
//------------------------------------------------------------------------------
/**
 * Constructs the Propagate Command based on another instance (copy constructor).
 *
 * @param <p> Original we are copying
 */
//------------------------------------------------------------------------------
Propagate::Propagate(const Propagate &p) :
   GmatCommand                 (p),
   propName                    (p.propName),
   propCoupled                 (true),
   interruptCheckFrequency     (30),
   baseEpoch                   (0.0),
   prop                        (NULL),
   // Set the parameter IDs
   propCoupledID               (p.propCoupledID),
   interruptCheckFrequencyID   (p.interruptCheckFrequencyID),
   satNameID                   (p.satNameID),
   propNameID                  (p.propNameID),
   secondsToProp               (p.secondsToProp),
   secondsToPropID             (p.secondsToPropID)
{
   // Increase the number of parms by the 2 new ones
   parameterCount = p.parameterCount;
   initialized = false;
}


//------------------------------------------------------------------------------
//  Propagate& operator=(const Propagate &p)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the Propagate Command.
 *
 * @return reference to this copy
 */
//------------------------------------------------------------------------------
Propagate& Propagate::operator=(const Propagate &p)
{
   if (&p == this)
      return *this;

   // Call the base assignment operator
   GmatCommand::operator=(p);
   initialized = false;
    
   return *this;
}


//------------------------------------------------------------------------------
// bool SetObject(const std::string &name, const Gmat::ObjectType type, ...
//------------------------------------------------------------------------------
bool Propagate::SetObject(const std::string &name, const Gmat::ObjectType type,
                          const std::string &associate,
                          const Gmat::ObjectType associateType)
{
   switch (type) {
   case Gmat::SPACECRAFT:
      satName.push_back(name);
      return true;

   case Gmat::PROP_SETUP:
      propName = name;
      return true;

   default:
      break;
   }

   // Not handled here -- invoke the next higher SetObject call
   return GmatCommand::SetObject(name, type, associate, associateType);
}


//------------------------------------------------------------------------------
// bool SetObject(GmatBase *obj, const Gmat::ObjectType type)
//------------------------------------------------------------------------------
bool Propagate::SetObject(GmatBase *obj, const Gmat::ObjectType type)
{
   switch (type)
   {
   case Gmat::STOP_CONDITION:
      stopWhen.push_back((StopCondition *)obj);
      return true;
            
   default:
      break;
   }

   // Not handled here -- invoke the next higher SetObject call
   return GmatCommand::SetObject(obj, type);
}

//------------------------------------------------------------------------------
// void ClearObject(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
void Propagate::ClearObject(const Gmat::ObjectType type)
{
   switch (type)
   {
   case Gmat::SPACECRAFT: //loj: 6/9/04 added
      satName.clear();
      break;
   case Gmat::STOP_CONDITION:
      stopWhen.clear();
      break;
            
   default:
      break;
   }
}


//------------------------------------------------------------------------------
// GmatBase* GetObject(const Gmat::ObjectType type, ...
//------------------------------------------------------------------------------
GmatBase* Propagate::GetObject(const Gmat::ObjectType type, 
                               const std::string objName)
{
   if (type == Gmat::STOP_CONDITION)
      if (stopWhen.empty())
         return NULL;
      else
         return stopWhen[0];
            
   return GmatCommand::GetObject(type, objName);
}


//------------------------------------------------------------------------------
//  GmatBase* Clone(void) const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the Propagate.
 *
 * @return clone of the Propagate.
 *
 */
//------------------------------------------------------------------------------
GmatBase* Propagate::Clone(void) const
{
   return (new Propagate(*this));
}

//loj: 6/25/04 added
// Reference object accessor methods
//------------------------------------------------------------------------------
// GmatBase* GetRefObject(const Gmat::ObjectType type, const std::string &name,
//                        const Integer index
//------------------------------------------------------------------------------
GmatBase* Propagate::GetRefObject(const Gmat::ObjectType type,
                                  const std::string &name, const Integer index)
{
   switch (type)
   {
   case Gmat::STOP_CONDITION:
      if (index < (Integer)stopWhen.size())
      {
         return stopWhen[index];
      }
      else
      {
         throw CommandException("Propagate::GetRefObject() invalid index\n");
      }
   default:
      break;
   }

   // Not handled here -- invoke the next higher GetRefObject call
   return GmatCommand::GetRefObject(type, name, index);
}

//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const Gmat::ObjectType type, ...
//------------------------------------------------------------------------------
/**
 * Sets reference object pointer.
 *
 * @return true if object successfully set, false otherwise
 */
//------------------------------------------------------------------------------
bool Propagate::SetRefObject(GmatBase *obj, const Gmat::ObjectType type,
                             const std::string &name, const Integer index)
{
#if DEBUG_PROPAGATE_OBJ
   MessageInterface::ShowMessage
      ("Propagate::SetRefObject() type=%s name=%s, index=%d\n",
       obj->GetTypeName().c_str(), name.c_str(), index);
#endif
   
   switch (type)
   {
   case Gmat::STOP_CONDITION:
      {
         Integer size = stopWhen.size();
         if (stopWhen.empty() && index == 0)
         {
            stopWhen.push_back((StopCondition *)obj);
            return true;
         }
         else if (index == size)
         {
            stopWhen.push_back((StopCondition *)obj);
            return true;
         }
         else if (index < size)
         {
            stopWhen[index] = (StopCondition *)obj;
            return true;
         }
         else
         {
            MessageInterface::ShowMessage
               ("Propagate::SetRefObject() index=%d is not next available "
                "index=%d. Setting %s:%s failed\n", index, size,
                obj->GetTypeName().c_str(), obj->GetName().c_str());
            return false;
         }
      }
   default:
      break;
   }

   // Not handled here -- invoke the next higher SetRefObject call
   return GmatCommand::SetRefObject(obj, type, name, index);
}

//------------------------------------------------------------------------------
// virtual ObjectArray& GetRefObjectArray(const Gmat::ObjectType type)
//------------------------------------------------------------------------------
ObjectArray& Propagate::GetRefObjectArray(const Gmat::ObjectType type)
{
   objectArray.clear();
   
   switch (type)
   {
   case Gmat::STOP_CONDITION:
      
      for (unsigned int i=0; i<stopWhen.size(); i++)
         objectArray.push_back(stopWhen[i]);
      
      return objectArray;
   default:
      break;
   }

   // Not handled here -- invoke the next higher SetReferenceObject call
   return GmatCommand::GetRefObjectArray(type);
}


// Parameter accessor methods
//------------------------------------------------------------------------------
// std::string GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
std::string Propagate::GetParameterText(const Integer id) const
{
   if (id == propCoupledID)
      return "PropagateCoupled";

   if (id == interruptCheckFrequencyID)
      return "InterruptFrequency";

   if (id == satNameID)
      return "Spacecraft";
    
   if (id == propNameID)
      return "Propagator";
    
   if (id == stopWhenID)
      return "StoppingConditions";
        
   if (id == secondsToPropID)
      return "ElapsedSeconds";

   return GmatCommand::GetParameterText(id);
}


//------------------------------------------------------------------------------
// Integer GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
Integer Propagate::GetParameterID(const std::string &str) const
{
   if (str == "PropagateCoupled")
      return propCoupledID;

   if (str == "InterruptFrequency")
      return interruptCheckFrequencyID;

   if (str == "Spacecraft")
      return satNameID;
    
   if (str == "Propagator")
      return propNameID;
    
   if (str == "ElapsedSeconds")
      return secondsToPropID;
    
   return GmatCommand::GetParameterID(str);
}


//------------------------------------------------------------------------------
// Gmat::ParameterType GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
Gmat::ParameterType Propagate::GetParameterType(const Integer id) const
{
   if (id == propCoupledID)
      return Gmat::BOOLEAN_TYPE;

   if (id == interruptCheckFrequencyID)
      return Gmat::INTEGER_TYPE;

   if (id == satNameID)
      return Gmat::STRINGARRAY_TYPE;
    
   if (id == propNameID)
      return Gmat::STRING_TYPE;
    
   if (id == secondsToPropID)
      return Gmat::REAL_TYPE;
    
   return GmatCommand::GetParameterType(id);
}


//------------------------------------------------------------------------------
// std::string GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
std::string Propagate::GetParameterTypeString(const Integer id) const
{
   if (id == propCoupledID)
      return PARAM_TYPE_STRING[Gmat::BOOLEAN_TYPE];

   if (id == interruptCheckFrequencyID)
      return PARAM_TYPE_STRING[Gmat::INTEGER_TYPE];

   if (id == satNameID)
      return PARAM_TYPE_STRING[Gmat::STRINGARRAY_TYPE];

   if (id == propNameID)
      return PARAM_TYPE_STRING[Gmat::STRING_TYPE];

   if (id == secondsToPropID)
      return PARAM_TYPE_STRING[Gmat::REAL_TYPE];

   return GmatCommand::GetParameterTypeString(id);
}

// Temporary method -- remove once StopConditions are hooked up
//------------------------------------------------------------------------------
// Real GetRealParameter(const Integer id) const
//------------------------------------------------------------------------------
Real Propagate::GetRealParameter(const Integer id) const
{
   if (id == secondsToPropID)
      return secondsToProp;
        
   return GmatCommand::GetRealParameter(id);
}

// Temporary method -- remove once StopConditions are hooked up
//------------------------------------------------------------------------------
// Real SetRealParameter(const Integer id, const Real value)
//------------------------------------------------------------------------------
Real Propagate::SetRealParameter(const Integer id, const Real value)
{
   if (id == secondsToPropID) {
      secondsToProp = value;
      return secondsToProp;
   }
        
   return GmatCommand::SetRealParameter(id, value);
}

//------------------------------------------------------------------------------
// Integer GetIntegerParameter(const Integer id) const
//------------------------------------------------------------------------------
Integer Propagate::GetIntegerParameter(const Integer id) const
{
   if (id == interruptCheckFrequencyID)
      return interruptCheckFrequency;

   return GmatCommand::GetIntegerParameter(id);
}


//------------------------------------------------------------------------------
// Integer SetIntegerParameter(const Integer id, const Integer value)
//------------------------------------------------------------------------------
Integer Propagate::SetIntegerParameter(const Integer id, const Integer value)
{
   if (id == interruptCheckFrequencyID) {
      if (value >= 0)
         interruptCheckFrequency = value;
      return interruptCheckFrequency;
   }

   return GmatCommand::SetIntegerParameter(id, value);
}


//------------------------------------------------------------------------------
// bool GetBooleanParameter(const Integer id) const
//------------------------------------------------------------------------------
bool Propagate::GetBooleanParameter(const Integer id) const
{
   if (id == propCoupledID)
      return propCoupled;

   return GmatCommand::GetBooleanParameter(id);
}


//------------------------------------------------------------------------------
// bool SetBooleanParameter(const Integer id, const bool value)
//------------------------------------------------------------------------------
bool Propagate::SetBooleanParameter(const Integer id, const bool value)
{
   if (id == propCoupledID) {
      propCoupled = value;
      return propCoupled;
   }

   return GmatCommand::SetBooleanParameter(id, value);
}


//------------------------------------------------------------------------------
// std::string GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
std::string Propagate::GetStringParameter(const Integer id) const
{
   if (id == propNameID)
      return propName;
   
   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool Propagate::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == satNameID)
       if (satName.empty())
          satName.push_back(value);
       else                            /// @todo: Generalize for multiple sats
          satName[0] = value;
//        if (value == "")
//            satName.clear();

   if (id == propNameID)
      propName = value;
 
   return GmatCommand::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& Propagate::GetStringArrayParameter(const Integer id) const
{
   if (id == satNameID)
      return satName;
 
   return GmatCommand::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// void InterpretAction(void)
//------------------------------------------------------------------------------
bool Propagate::InterpretAction(void)
{
   /// @todo: Clean up this hack for the Propagate::InterpretAction method
   // Sample string:  "Propagate RK89(Sat1, {Duration = 86400.0});"
    
   Integer loc = generatingString.find("Propagate", 0) + 9, end;
   const char *str = generatingString.c_str();
   while (str[loc] == ' ')
      ++loc;
        
   end = generatingString.find("(", loc);
   if (end == (Integer)std::string::npos)
      throw CommandException("Propagate string does not identify propagator\n");

   std::string component = generatingString.substr(loc, end-loc);
   SetObject(component, Gmat::PROP_SETUP);

   loc = end + 1;
   end = generatingString.find(",", loc);
   if (end == (Integer)std::string::npos)
      throw CommandException("Propagate string does not identify spacecraft\n");
    
   component = generatingString.substr(loc, end-loc);
   SetObject(component, Gmat::SPACECRAFT);

   loc = end + 1;
   end = generatingString.find(",", loc);
   if (end != (Integer)std::string::npos)
      throw CommandException("Propagate does not yet support multiple spacecraft\n");

    //loj: 3/24/04 for b3 "," also valid for multiple spacecraft
   end = generatingString.find("{", loc);
   if (end == (Integer)std::string::npos)
      throw CommandException("Propagate does not identify stopping condition: looking for {\n");

//      //--------------------------------------
//      //loj: 3/22/04 old code
//      //--------------------------------------
//      end = generatingString.find("Duration", loc);
//      if (end == (Integer)std::string::npos)
//          throw CommandException("'Duration' is the only supported stopping condition");
    
//      loc = end + 8;
//      end = generatingString.find("=", loc);
//      if (end == (Integer)std::string::npos)
//          throw CommandException("Format: 'Duration = xxx'");

//      loc = end + 1;
    
//      secondsToProp = atof(&str[loc]);
//      //--------------------------------------

    
   //--------------------------------------
   //loj: 3/22/04 new code
   //--------------------------------------
   loc = end + 1;
   end = generatingString.find(".", loc);

   if (end == (Integer)std::string::npos)
      throw CommandException("Propagate does not identify stopping condition: looking for .\n");
    
   std::string paramObj = generatingString.substr(loc, end-loc);
   //MessageInterface::ShowMessage("Propagate::InterpretAction() component=%s, loc=%d, end=%d\n",
   //                              paramObj.c_str(), loc, end);
    
   loc = end + 1;
   end = generatingString.find("=", loc);
   if (end == (Integer)std::string::npos)
   {
      //MessageInterface::ShowMessage("Propagate::InterpretAction() ParamType is Apoapsis or Periapsis\n");
        
      end = generatingString.find(",", loc);
      if (end != (Integer)std::string::npos)
         throw CommandException("Propagate does not yet support multiple stopping condition\n");
        
      end = generatingString.find("}", loc);
      if (end == (Integer)std::string::npos)
         throw CommandException("Propagate does not identify stopping condition: looking for }\n");
   }
    
   std::string paramType = generatingString.substr(loc, end-loc);
   //MessageInterface::ShowMessage("Propagate::InterpretAction() paramType=%s, loc=%d, end=%d\n",
   //                              paramType.c_str(), loc, end);
    
    // remove blank spaces
   unsigned int start = 0;
   for (unsigned int i=start; i<paramType.size(); i++)
   {
      if (paramType[i] == ' ')
      {
         paramType.erase(i, 1);
         start = i;
      }
   }
    
   //MessageInterface::ShowMessage("Propagate::InterpretAction() after remove blanks paramType=%s\n",
   //                              paramType.c_str());
    
   Moderator *theModerator = Moderator::Instance();
    
    // create parameter
   std::string paramName = paramObj + "." + paramType;
   //loj: if we want to see parameter via the GUI, use named parameter
   // This will be deleted when system shuts down
   Parameter *stopParam = theModerator->CreateParameter(paramType, paramName);
   stopParam->SetStringParameter("Object", paramObj);
    
   StopCondition *stopCond =
      theModerator->CreateStopCondition("StopCondition", "StopOn" + paramName);
    
   stopCond->SetStringParameter("StopVar", paramName);
   SetObject(stopCond, Gmat::STOP_CONDITION);

   if (paramType != "Apoapsis" && paramType != "Periapsis")
   {
      loc = end + 1;
      Real propStopVal = atof(&str[loc]);
      stopCond->SetRealParameter("Goal", propStopVal);
      //MessageInterface::ShowMessage("Propagate::InterpretAction() propStopVal = %f\n",
      //                              propStopVal);

      loc = end + 1;
      end = generatingString.find(",", loc);
      if (end != (Integer)std::string::npos)
         throw CommandException("Propagate does not yet support multiple stopping condition\n");
    
      loc = end + 1;
      end = generatingString.find("}", loc);
        
      if (end == (Integer)std::string::npos)
         throw CommandException("Propagate does not identify stopping condition: looking for }\n");
   }
    
   loc = end + 1;
   end = generatingString.find(")", loc);
   //MessageInterface::ShowMessage("Propagate::InterpretAction() looking for ) loc=%d, end=%d\n",
   //                              loc, end);
    
   if (end == (Integer)std::string::npos)
      throw CommandException("Propagate does not identify stopping condition: looking for )\n");
    
   return true;
}

//------------------------------------------------------------------------------
// bool Initialize(void)
//------------------------------------------------------------------------------
bool Propagate::Initialize(void)
{
   GmatCommand::Initialize();
   
   if (objectMap->find(propName) == objectMap->end())
      throw CommandException("Propagate command cannot find Propagator Setup\n");

   if (stopWhen.empty())
      throw CommandException("Propagate command does not have any stopping condition\n");

   prop = (PropSetup *)((*objectMap)[propName]);
   if (!prop)
      return false;
   Propagator *p = prop->GetPropagator();
   if (!p)
      throw CommandException("Propagator not set in PropSetup\n");

   // Toss the spacecraft into the force model
   ForceModel *fm = prop->GetForceModel();
   fm->ClearSpacecraft();
   StringArray::iterator scName;
   for (scName = satName.begin(); scName != satName.end(); ++scName) {
      sats.push_back((Spacecraft*)(*objectMap)[*scName]);
      fm->AddSpacecraft((Spacecraft*)(*objectMap)[*scName]);
   }
    
   p->Initialize();
   initialized = true;
    
   //loj: 6/16/04
   // Set SolarSystem in StopCondition
   if (solarSys != NULL)
   {
      for (unsigned int i=0; i<stopWhen.size(); i++)
      {
         stopWhen[i]->SetSolarSystem(solarSys);
         stopWhen[i]->Initialize();
         stopWhen[i]->SetSpacecraft(sats[0]); 
          
         if (!stopWhen[i]->IsInitialized())
         {
            initialized = false;
            MessageInterface::ShowMessage
               ("Propagate::Initialize() StopCondition %s is not initialized.\n",
                stopWhen[i]->GetName().c_str());
            break;
         }
      }
   }
   else
   {
      initialized = false;
      MessageInterface::ShowMessage
         ("Propagate::Initialize() SolarSystem not set in StopCondition");
   }

   return initialized; //loj: 6/16/04
   //return true;
}

//------------------------------------------------------------------------------
// bool Execute(void)
//------------------------------------------------------------------------------
/**
 * Propagate the assigned members to the desired stopping condition
 *
 * @return true if the Command runs to completion, false if an error
 *         occurs.
 */
//------------------------------------------------------------------------------
bool Propagate::Execute(void)
{
   Real elapsedTime = 0.0;
   Real currEpoch = 0.0;
   
   if (initialized == false)
      throw CommandException("Propagate Command was not Initialized\n");

   Propagator *p = prop->GetPropagator();
   ForceModel *fm = prop->GetForceModel();
   fm->SetTime(0.0);
    
   p->Initialize();
   Real *state = fm->GetState();
   Integer dim = fm->GetDimension();
   Real *pubdata = new Real[dim+1];
   GmatBase* sat1 = (*objectMap)[*(satName.begin())];
   Integer epochID = sat1->GetParameterID("Epoch");
   baseEpoch = sat1->GetRealParameter(epochID);

#if DEBUG_PROPAGATE_EXE
   MessageInterface::ShowMessage
      ("Propagate::Execute() Propagate start; epoch = %f\n",
       (baseEpoch + fm->GetTime() / 86400.0));
   int stopCondCount = stopWhen.size();
   MessageInterface::ShowMessage
      ("Propagate::Execute() stopCondCount = %d\n", stopCondCount);
   for (int i=0; i<stopCondCount; i++)
   {
      MessageInterface::ShowMessage
         ("Propagate::Execute() stopCondName[%d]=%s\n", i,
          stopWhen[i]->GetName().c_str());
   }
#endif
    
   //---------------------------------------
   //loj: 3/22/04 new code
   //---------------------------------------
   bool stopCondMet = false;
   Real stopEpoch = 0.0;
   std::string stopVar;

   //loj: 6/16/04 added setting new initial epoch for "Elapsed*" parameters
   // for consecutive Propate commands
   elapsedTime = fm->GetTime();
   currEpoch = baseEpoch + elapsedTime / 86400.0;
   
   for (unsigned int i=0; i<stopWhen.size(); i++)
   {
      // StopCondition need new base epoch -- loj: 6/30/04
      stopWhen[i]->SetRealParameter("BaseEpoch", currEpoch);
      
      // ElapsedTime parameters need new initial epoch
      stopVar = stopWhen[i]->GetStringParameter("StopVar");
      if (stopVar.find("Elapsed") != stopVar.npos)
      {
         stopWhen[i]->GetStopParameter()->
            SetRealParameter("InitialEpoch", currEpoch);
      }
   }

   while (!stopCondMet)
   {
      if (!p->Step())
         throw CommandException("Propagate::Execute() Propagator Failed to Step\n");

      // orbit related parameters use spacecraft for data
      elapsedTime = fm->GetTime();
      currEpoch = baseEpoch + elapsedTime / 86400.0;
      
      //loj: 6/15/04 update spacecraft epoch, without argument the spacecraft epoch
      // won't get updated for consecutive Propagate command
      fm->UpdateSpacecraft(currEpoch);

      //------------------------------------------
      // loop through StopCondition list
      //------------------------------------------
      for (unsigned int i=0; i<stopWhen.size(); i++)
      {
         // StopCondition need epoch for the Interpolator
         stopWhen[i]->SetRealParameter("Epoch", currEpoch);
         
         if (stopWhen[i]->Evaluate())
         {
            stopCondMet = true;
            stopEpoch = (stopWhen[i]->GetStopEpoch());
#if DEBUG_PROPAGATE_EXE
            MessageInterface::ShowMessage
               ("Propagate::Execute() %s met\n", stopWhen[i]->GetName().c_str());
#endif
            break; // exit if any stop condition met
         }
      }
      
      if (!stopCondMet)
      {
         // Publish the data here
         pubdata[0] = currEpoch;
         memcpy(&pubdata[1], state, dim*sizeof(Real));
         publisher->Publish(pubdata, dim+1);
      }
      else
      {
         fm->RevertSpacecraft();
         elapsedTime = fm->GetTime();
         currEpoch = baseEpoch + elapsedTime / 86400.0;
         //fm->UpdateFromSpacecraft();
         //fm->SetTime(elapsedTime);
      }
#if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage
         ("Propagate::Execute() intermediate; epoch = %f\n", currEpoch);
      //(baseEpoch + fm->GetTime() / 86400.0));
#endif
   }

#if DEBUG_PROPAGATE_EXE
   MessageInterface::ShowMessage
      ("Propagate::Execute() currEpoch = %f, stopEpoch = %f, elapsedTime = %f\n",
       currEpoch, stopEpoch, elapsedTime);
#endif

   Real secsToStep = (stopEpoch - currEpoch) * 86400;
   
   if (secsToStep > 0.0)
   {
#if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage
         ("Propagate::Execute() before Step(%f) epoch = %f\n",
          secsToStep, (baseEpoch + fm->GetTime() / 86400.0));
#endif
      if (!p->Step(secsToStep))
         throw CommandException("Propagator Failed to Step fixed interval\n");
      
      fm->UpdateSpacecraft(baseEpoch + fm->GetTime() / 86400.0);

      // Publish the final data point here
      pubdata[0] = baseEpoch + fm->GetTime() / 86400.0;
      memcpy(&pubdata[1], state, dim*sizeof(Real));
      publisher->Publish(pubdata, dim+1);
      
#if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage
         ("Propagate::Execute() after Step(%f) epoch = %f\n",
          secsToStep, (baseEpoch + fm->GetTime() / 86400.0));
#endif
   }

   publisher->FlushBuffers(); //loj: 6/22/04 added
    
//      //---------------------------------------
//      //loj: 3/22/04 old code
//      //---------------------------------------
//      while (elapsedTime < secondsToProp) {
//          if (!p->Step())
//              throw CommandException("Propagate::Execute() Propagator Failed to Step");
//          // Not at stop condition yet
//          if (fm->GetTime() < secondsToProp) {
//              elapsedTime = fm->GetTime();
//              fm->UpdateSpacecraft();
//              /// @todo Update epoch on spacecraft <- loj: it's done in ForceModel::UpdateSpacecraft()
//          }
//          else // Passed stop epoch
//          {
//              fm->UpdateFromSpacecraft();
//              fm->SetTime(elapsedTime);
//              break;
//          }
//          // Publish the data here
//          pubdata[0] = baseEpoch + fm->GetTime() / 86400.0;
//          memcpy(&pubdata[1], state, dim*sizeof(Real));
//          publisher->Publish(pubdata, dim+1);
//      }    
//    
//      if (secondsToProp - elapsedTime > 0.0) {
//          if (!p->Step(secondsToProp - elapsedTime))
//              throw CommandException("Propagator Failed to Step fixed interval");
//          // Publish the final data point here
//          pubdata[0] = baseEpoch + fm->GetTime() / 86400.0;
        
//          memcpy(&pubdata[1], state, dim*sizeof(Real));
//          publisher->Publish(pubdata, dim+1);
//          fm->UpdateSpacecraft(baseEpoch + fm->GetTime() / 86400.0);
//      }
//      //---------------------------------------
    
   delete [] pubdata;
    
   //---------------------------------------
   //loj: 3/22/04 new code
   //---------------------------------------
   for (unsigned int i=0; i<stopWhen.size(); i++)
   {
      if (stopWhen[i]->GetName() == "")
         delete stopWhen[i];
   }
   //---------------------------------------

#if DEBUG_PROPAGATE_EXE
   MessageInterface::ShowMessage
      ("Propagate::Execute() complete; epoch = %f\n",
       (baseEpoch + fm->GetTime() / 86400.0));
#endif
   
   return true;
}

