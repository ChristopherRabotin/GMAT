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

//#define DEBUG_PROPAGATE_OBJ 1
//#define DEBUG_PROPAGATE_EXE 1


std::string Propagate::PropModeList[PROP_MODE_COUNT] =
{
   "", "Synchronized"
};

//------------------------------------------------------------------------------
//  Propagate(void)
//------------------------------------------------------------------------------
/**
 * Constructs the Propagate Command (default constructor).
 */
//------------------------------------------------------------------------------
Propagate::Propagate(void) :
   GmatCommand                 ("Propagate"),
   propCoupled                 (true),
   interruptCheckFrequency     (30),
   inProgress                  (false),
   // Set the parameter IDs
   propCoupledID               (parameterCount),
   interruptCheckFrequencyID   (parameterCount+1),
   satNameID                   (parameterCount+2),
   propNameID                  (parameterCount+3),
   secondsToProp               (8640.0),
   secondsToPropID             (parameterCount+4),
   singleStepMode              (false),
   currentMode                 (INDEPENDENT)
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
   /// todo: clean memory for satName.push_back(new StringArray);
 
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
   propCoupled                 (true),
   interruptCheckFrequency     (p.interruptCheckFrequency),
   inProgress                  (false),
   // Set the parameter IDs
   propCoupledID               (p.propCoupledID),
   interruptCheckFrequencyID   (p.interruptCheckFrequencyID),
   satNameID                   (p.satNameID),
   propNameID                  (p.propNameID),
   secondsToProp               (p.secondsToProp),
   secondsToPropID             (p.secondsToPropID),
   singleStepMode              (p.singleStepMode),
   currentMode                 (p.currentMode)
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
   Integer propNum = propName.size() - 1;
   
   switch (type) {
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION:
         (satName[propNum])->push_back(name);
         return true;
   
      case Gmat::PROP_SETUP:
         propName.push_back(name);
         satName.push_back(new StringArray);
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
   case Gmat::FORMATION:
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


std::string Propagate::GetRefObjectName(const Gmat::ObjectType type) const
{
   /// @todo Figure out how to send back the arrays of names
   switch (type) {
      // Propagator setups
      case Gmat::PROP_SETUP:
         return propName[0];
      
      // Objects that get propagated
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION:
         if (satName.size() > 0)
            return (*satName[0])[0];
      
      default:
         break;
   }
   
   return GmatCommand::GetRefObjectName(type);
}


bool Propagate::SetRefObjectName(const Gmat::ObjectType type,
                                 const std::string &name)
{
   switch (type) {
      // Propagator setups
      case Gmat::PROP_SETUP:
         propName.push_back(name);
         satName.push_back(new StringArray);
         return true;
      
      // Objects that get propagated
      case Gmat::SPACECRAFT:
      case Gmat::FORMATION: 
      {
         Integer propNum = propName.size()-1;
         satName[propNum]->push_back(name);
         return true;
      }
      
      default:
         break;
   }
   
   return GmatCommand::SetRefObjectName(type, name);
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
         }
         else if (index == size)
         {
            stopWhen.push_back((StopCondition *)obj);
         }
         else if (index < size)
         {
            stopWhen[index] = (StopCondition *)obj;
         }
         else
         {
            MessageInterface::ShowMessage
               ("Propagate::SetRefObject() index=%d is not next available "
                "index=%d. Setting %s:%s failed\n", index, size,
                obj->GetTypeName().c_str(), obj->GetName().c_str());
            return false;
         }
         
         std::string satName = obj->GetName();
         Integer strt = satName.find("StopOn") + 6;
         if (strt == (Integer)std::string::npos)
            strt = 0;
         Integer ndx = satName.find(".",0);
         if (ndx != (Integer)std::string::npos)
            satName = satName.substr(strt, ndx-strt);
         stopSatNames.push_back(satName);
         return true;
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
   /// @todo Remove this deprecated option
   if (id == propNameID)
      return propName[0];
   
   return GmatCommand::GetStringParameter(id);
}


//------------------------------------------------------------------------------
// bool SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
bool Propagate::SetStringParameter(const Integer id, const std::string &value)
{
   if (id == satNameID) {
//       if (satName.empty())
//          satName.push_back(value);
//       else                            /// @todo: Generalize for multiple sats
//          satName[0] = value;
      Integer propNum = propName.size()-1;
      satName[propNum]->push_back(value);
      return true;
   }

   if (id == propNameID) {
      propName.push_back(value);
      return true;
   }
 
   return GmatCommand::SetStringParameter(id, value);
}


//------------------------------------------------------------------------------
// const StringArray& GetStringArrayParameter(const Integer id) const
//------------------------------------------------------------------------------
const StringArray& Propagate::GetStringArrayParameter(const Integer id) const
{
   if (id == satNameID)
      return *satName[0];
 
   if (id == propNameID) {
      return propName;
   }
 
   return GmatCommand::GetStringArrayParameter(id);
}


//------------------------------------------------------------------------------
// void InterpretAction(void)
//------------------------------------------------------------------------------
bool Propagate::InterpretAction(void)
{
   // DJC: Uncomment the following when working Interpreter
   //return false;
   
   /// @todo: Clean up this hack for the Propagate::InterpretAction method
   // Sample string:  "Propagate RK89(Sat1, {Duration = 86400.0});"
    
   Integer loc = generatingString.find("Propagate", 0) + 9;
   const char *str = generatingString.c_str();
   while (str[loc] == ' ')
      ++loc;

   // Check to see if there are optional parameters (e.g. "Synchronized")
   CheckForOptions(loc, generatingString);
   // Now fill in the list of propagators
   AssemblePropagators(loc, generatingString);
   
   return true;
}


//------------------------------------------------------------------------------
// void CheckForOptions(Integer &loc, std::string& generatingString)
//------------------------------------------------------------------------------
void Propagate::CheckForOptions(Integer &loc, std::string& generatingString)
{
   std::string modeStr;
   currentMode = INDEPENDENT;
   
   for (Integer modeId = INDEPENDENT+1; modeId != PROP_MODE_COUNT; ++modeId) {
      modeStr = PropModeList[modeId];
      modeStr += " ";
      
      #ifdef DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage("\nPropagate::CheckForOptions() looking"
                                " for \"%s\" starting at loc=%d\n in \n\"%s\"",
                                modeStr.c_str(), loc, generatingString.c_str());
      #endif
      Integer end = generatingString.find(modeStr, loc);
      if (end != (Integer)std::string::npos) {
         currentMode = (PropModes)modeId;
         #ifdef DEBUG_PROPAGATE_EXE
            MessageInterface::ShowMessage("\nLocated at %d\n", end);
            MessageInterface::ShowMessage("Mode is now %d\n", currentMode);
         #endif

         if (end >= loc)
            loc = end + modeStr.length();
      }
   }
}


//------------------------------------------------------------------------------
// void AssemblePropagators(Integer &loc, std::string& generatingString)
//------------------------------------------------------------------------------
void Propagate::AssemblePropagators(Integer &loc, std::string& generatingString)
{
   // At this point we are set with loc pointing to the start of the list of
   // PropSetups.  First we break that list into the individual pieces
   StringArray pieces;
   const char *str = generatingString.c_str();
   
   #ifdef DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage("Propagate::AssemblePropagators: "
                                    "Breaking \"%s\" into pieces\n", 
                                    generatingString.c_str()+loc);
   #endif

   bool scanning = true;
   Integer currentLoc = loc, parmstart, end;
   
   parmstart = generatingString.find("(", currentLoc);   
   while (scanning) {
      end = generatingString.find(")", parmstart)+1;

      if (end == (Integer)std::string::npos)
         throw CommandException("Propagate::AssemblePropagators: Propagate"
                                " string does not identify propagator\n");
      
      pieces.push_back(generatingString.substr(currentLoc, end-currentLoc));
      currentLoc = end+1;
      // Skip trailing comma or white space
      while ((str[currentLoc] == ',') || (str[currentLoc] == ' '))
         ++currentLoc;
      parmstart = generatingString.find("(", currentLoc);
      if (parmstart == (Integer)std::string::npos)
         scanning = false;
   }
      
   #ifdef DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage("Propagate::AssemblePropagators():"
                                    " Propagators Identified:\n");
      for (StringArray::iterator i = pieces.begin(); i != pieces.end(); ++i) 
         MessageInterface::ShowMessage("   \"%s\"\n", i->c_str());
   #endif
   
   Integer satEnd;

   for (StringArray::iterator i = pieces.begin(); i != pieces.end(); ++i) {
      loc = 0;
      end = i->find("(", loc);
      if (end == (Integer)std::string::npos)
         throw CommandException("Propagate string does not identify propagator\n");
   
      std::string component = i->substr(loc, end-loc);
      SetObject(component, Gmat::PROP_SETUP);
   
      loc = end + 1;
      satEnd = loc;
      end = i->find(",", loc);
      
      // Find the location of the end of the list of SpaceObjects
      satEnd = i->find("{", loc);
      if (satEnd == (Integer)std::string::npos)
         satEnd = i->find(")", loc);
      if (satEnd == (Integer)std::string::npos)
         throw CommandException("Propagate string terminating paren \")\" missing\n");

      if (end == (Integer)std::string::npos)
         end = satEnd;
   
      #ifdef DEBUG_PROPAGATE_EXE
         MessageInterface::ShowMessage("Building list of SpaceObjects:\n");
      #endif
      do {
         while ((*i)[loc] == ' ')
            ++loc;
         if (end == (Integer)std::string::npos)
            throw CommandException("Propagate string \"" + (*i) +
                                   "\" does not identify spacecraft\n");
         #ifdef DEBUG_PROPAGATE_EXE
            MessageInterface::ShowMessage("i = \"%s\", loc = %d and end = %d\n",
                                 i->c_str(), loc, end);
         #endif
          
         component = i->substr(loc, end-loc);
         SetObject(component, Gmat::SPACECRAFT);
         #ifdef DEBUG_PROPAGATE_EXE
            MessageInterface::ShowMessage("   \"%s\"\n", 
                                          component.c_str());
         #endif
      
         loc = end + 1;
         end = i->find(",", loc);
         if (end == (Integer)std::string::npos)
            end = satEnd;
      } while (end < satEnd);

      end = i->find("{", loc);
//      if (end == (Integer)std::string::npos)
//         throw CommandException("Propagate does not identify stopping condition: looking for {\n");
   
      if (end != (Integer)std::string::npos) {
         loc = end + 1;
         while ((*i)[loc] == ' ')
            ++loc;
            
         end = i->find(".", loc);
      
         if (end == (Integer)std::string::npos)
            throw CommandException("Propagate does not identify stopping condition: looking for .\n");
          
         std::string paramObj = i->substr(loc, end-loc);
          
         loc = end + 1;
         end = i->find("=", loc);
         if (end == (Integer)std::string::npos)
         {
            end = i->find(",", loc);
            if (end != (Integer)std::string::npos)
               throw CommandException("Propagate does not yet support multiple stopping condition\n");
              
            end = i->find("}", loc);
            if (end == (Integer)std::string::npos)
               throw CommandException("Propagate does not identify stopping condition: looking for }\n");
         }
          
         std::string paramType = i->substr(loc, end-loc);
      
         unsigned int start = 0;
         for (unsigned int idx=start; idx<paramType.size(); ++idx)
         {
            if (paramType[idx] == ' ')
            {
               paramType.erase(idx, 1);
               start = idx;
            }
         }
          
         Moderator *theModerator = Moderator::Instance();
          
          // create parameter
         std::string paramName = paramObj + "." + paramType;
         //loj: if we want to see parameter via the GUI, use named parameter
         // This will be deleted when system shuts down
         Parameter *stopParam = theModerator->CreateParameter(paramType, paramName);
         //stopParam->SetStringParameter("Object", paramObj);
         stopParam->SetRefObjectName(Gmat::SPACECRAFT, paramObj); //loj: 9/13/04 added
         
         StopCondition *stopCond =
            theModerator->CreateStopCondition("StopCondition", "StopOn" + paramName);
         
         stopCond->SetStringParameter("StopVar", paramName);
         
         SetObject(stopCond, Gmat::STOP_CONDITION);
         // Store the spacecraft name for use when setting the epoch data
         stopSatNames.push_back(paramObj);
   //      stopSatNames[index] = paramObj;
      
         if (paramType != "Apoapsis" && paramType != "Periapsis")
         {
            loc = end + 1;
            Real propStopVal = atof(&(i->c_str())[loc]);
            stopCond->SetRealParameter("Goal", propStopVal);
            
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage("Propagate::AssemblePropagators()"
                                             " propStopVal = %f\n", propStopVal);
            #endif
      
            loc = end + 1;
            end = i->find(",", loc);
            if (end != (Integer)std::string::npos)
               throw CommandException("Propagate does not yet support multiple stopping condition\n");
          
            loc = end + 1;
            end = i->find("}", loc);
              
            if (end == (Integer)std::string::npos)
               throw CommandException("Propagate does not identify stopping condition: looking for }\n");
         }
         loc = end + 1;
      }
      else
         loc = satEnd - 1;
      end = i->find(")", loc);
          
      if (end == (Integer)std::string::npos)
         throw CommandException("Propagate does not end correctly: looking for )\n");
   }
   // NOW test to be sure there is at least one stop cond.
   if (stopWhen.empty())
      singleStepMode = true;
      //throw CommandException("Propagate does not identify any stopping conditions\n");
}


//------------------------------------------------------------------------------
// bool TakeAStep(Real propStep)
//------------------------------------------------------------------------------
bool Propagate::TakeAStep(Real propStep)
{
   bool retval = false;
   Real stepToTake;
 
   std::vector<Propagator*>::iterator current = p.begin();
   if (propStep == 0.0) {
      switch (currentMode) {
         case INDEPENDENT:
            // Advance each propagator individually, without regard for the
            // epochs of the others
                        
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::TakeAStep() running in INDEPENDENT mode\n");
            #endif
            while (current != p.end()) { 
               if (!(*current)->Step())
                  throw CommandException("Propagator failed to take a good step\n");
               ++current;
            }
            retval = true;
            break;
            
         case SYNCHRONIZED:
            // This mode advances the first propagator, and then brings the 
            // others up to the epoch of that first one.
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::TakeAStep() running in SYNCHRONIZED mode\n");
            #endif
            if (!(*current)->Step())
               throw CommandException("Initial synchronized Propagator failed "
                                      "to take a good step\n");
            stepToTake = (*current)->GetStepTaken();
            ++current;
            while (current != p.end()) {
               if (!(*current)->Step(stepToTake))
                  throw CommandException("Propagator failed to take a good "
                                         "synchronized step\n");
               ++current;
            }
            retval = true;
            break;
            
         default:
            #ifdef DEBUG_PROPAGATE_EXE
               MessageInterface::ShowMessage
                  ("Propagate::TakeAStep() runnning in undefined mode (mode = %d)\n",
                   currentMode);
            #endif
            retval = false;
      }
   }
   else {
      // Step all of the propagators by the input amount
      while (current != p.end()) {
         if (!(*current)->Step(propStep))
            throw CommandException("Propagator failed to take a good final "
                                   "step\n");
         ++current;
      }
      retval = true;
   }
   
   return retval;
}


//------------------------------------------------------------------------------
// bool Initialize(void)
//------------------------------------------------------------------------------
bool Propagate::Initialize(void)
{
#if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage("Propagate::Initialize() entered.\n%s\n",
                                    generatingString.c_str());
#endif

   GmatCommand::Initialize();
   
   inProgress = false;
   unsigned int index = 0;
   prop.clear();
   SpaceObject *so;
   
   for (StringArray::iterator i = propName.begin(); i != propName.end(); ++i) {
      if (satName.size() <= index)
         throw CommandException("Size mismatch for SpaceObject names\n");
    
      if (objectMap->find(*i) == objectMap->end())
         throw CommandException("Propagate command cannot find Propagator Setup\n");
   
      if (satName[index]->empty())
         throw CommandException("Propagate command does not have a SpaceObject for "
                                + (*i) + " in \n\"" + generatingString + "\"\n");
   
      if (stopWhen.empty())
         //throw CommandException("Propagate command does not have any stopping condition\n");
         singleStepMode = true;
      else
         singleStepMode = false;

      prop.push_back((PropSetup *)((*objectMap)[*i]));
      if (!prop[index])
         return false;
      Propagator *p = prop[index]->GetPropagator();
      if (!p)
         throw CommandException("Propagator not set in PropSetup\n");
   
      // Toss the spacecraft into the force model
      ForceModel *fm = prop[index]->GetForceModel();
      if (!fm)
         throw CommandException("ForceModel not set in PropSetup\n");
      fm->ClearSpacecraft();
      StringArray::iterator scName;
      StringArray owners, elements;
   
      /// @todo Check to see if All and All.Epoch belong in place for all modes.
      owners.push_back("All");
      elements.push_back("All.epoch");
      
      for (scName = satName[index]->begin(); scName != satName[index]->end(); 
           ++scName) {
         if (objectMap->find(*scName) == objectMap->end()) {
            std::string errmsg = "Unknown SpaceObject \"";
            errmsg += *scName;
            errmsg += "\"";
            throw CommandException(errmsg);
         }
         so = (SpaceObject*)(*objectMap)[*scName];
         sats.push_back(so);
         fm->AddSpaceObject(so);
         if (so->GetType() == Gmat::FORMATION)
            FillFormation(so, owners, elements);
         else {
            SetNames(so->GetName(), owners, elements);
         }
      }
   
      streamID = publisher->RegisterPublishedData(owners, elements);
      p->SetPhysicalModel(fm); 
      p->Initialize();
      ++index;
   }
   initialized = true;
   
   // Setup spacecraft array used for stopping conditions
   for (StringArray::iterator sc = stopSatNames.begin(); 
        sc != stopSatNames.end(); ++sc) {
      if (objectMap->find(*sc) == objectMap->end()) {
         std::string errmsg = "Unknown SpaceObject \"";
         errmsg += *sc;
         errmsg += "\" used in stopping conditions";
         throw CommandException(errmsg);
      }
      so = (SpaceObject*)(*objectMap)[*sc];
      stopSats.push_back(so);
   }

   if ((stopWhen.size() == 0) && !singleStepMode)
      throw CommandException("No stopping conditions specified!");
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

#if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage("Propagate::Initialize() complete.\n");
#endif

   return initialized;
}

//------------------------------------------------------------------------------
// void FillFormation(SpaceObject *so)
//------------------------------------------------------------------------------
/**
 * Fill in the components of a formation (recursively).
 *
 * @param so The SpaceObject that needs to be filled.
 */
//------------------------------------------------------------------------------
void Propagate::FillFormation(SpaceObject *so, StringArray& owners, 
                              StringArray& elements)
{
   if ((so == NULL) || (so->GetType() != Gmat::FORMATION))
      throw CommandException("Invalid SpaceObject passed to FillFormation");
      
   StringArray comps = so->GetStringArrayParameter(so->GetParameterID("Add"));
   SpaceObject *el;
   for (StringArray::iterator i = comps.begin(); i != comps.end(); ++i) {
      el = (SpaceObject*)(*objectMap)[*i];
      so->SetRefObject(el, el->GetType(), el->GetName()); 
      if (el->GetType() == Gmat::FORMATION)
         FillFormation(el, owners, elements);
      else     // Setup spacecraft data descriptions
         SetNames(el->GetName(), owners, elements);
   }
   
   ((Formation*)(so))->BuildState();
}


//------------------------------------------------------------------------------
// GmatCommand* GetNext()
//------------------------------------------------------------------------------
/**
 * Returns pointer to next command to be executed.
 * 
 * Propagate::GetNext overrides the base class's GetNext method so that it can
 * poll the moderator for user interrupts periodically.  If the stopping 
 * conditions have not yet been met, GetNext returns this object; otherwise, it 
 * returns the next one in the command list.
 *
 * @return The next pointer, as described above.
 */
//------------------------------------------------------------------------------
GmatCommand* Propagate::GetNext()
{
 
   if (!inProgress)
      return next;
   return this;
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
#if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage("Propagate::Execute() entered.\n");
#endif

   if (initialized == false)
      throw CommandException("Propagate Command was not Initialized\n");

   // Parm used to check for interrupt in the propagation   
   Integer checkCount = 0, trigger = 0;

   try {
      if (!inProgress) { 
         // Reset the initialization data
         Initialize();
         dim = 0;
         p.clear();
         fm.clear();

         for (Integer n = 0; n < (Integer)prop.size(); ++n) {
            elapsedTime.push_back(0.0);
            currEpoch.push_back(0.0);
            p.push_back(prop[n]->GetPropagator());
            fm.push_back(prop[n]->GetForceModel());
            fm[n]->SetTime(0.0);
            fm[n]->UpdateInitialData();
         
            p[n]->Initialize();
            state = fm[n]->GetState();
            dim += fm[n]->GetDimension();
         }   

         pubdata = new Real[dim+1];
         baseEpoch.clear();

         for (Integer n = 0; n < (Integer)prop.size(); ++n) {
            if (satName[n]->empty())
               throw CommandException("Propagator has no associated space objects.");
            GmatBase* sat1 = (*objectMap)[*(satName[n]->begin())];
            Integer epochID = sat1->GetParameterID("Epoch");
            baseEpoch.push_back(sat1->GetRealParameter(epochID));
            elapsedTime[n] = fm[n]->GetTime();
            currEpoch[n] = baseEpoch[n] + elapsedTime[n] / 86400.0;
         }
         // Now setup the stopping condition elements
         #if DEBUG_PROPAGATE_EXE
            MessageInterface::ShowMessage
               ("Propagate::Execute() Propagate start; epoch = %f\n",
             (baseEpoch[0] + fm[0]->GetTime() / 86400.0));
            MessageInterface::ShowMessage
               ("Propagate::Execute() Propagate start; fm epoch = %f\n",
               (fm[0]->GetRealParameter(fm[0]->GetParameterID("Epoch"))));
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
       
         stopCondMet = false;
         stopEpoch = 0.0;
         std::string stopVar;
         Integer epochID;
         Real stopEpochBase;
         
         for (unsigned int i = 0; i<stopWhen.size(); i++)
         {
            epochID = stopSats[i]->GetParameterID("Epoch");
            stopEpochBase = stopSats[i]->GetRealParameter(epochID);
            // StopCondition need new base epoch
            stopWhen[i]->SetRealParameter("BaseEpoch", stopEpochBase);

            // ElapsedTime parameters need new initial epoch
            stopVar = stopWhen[i]->GetStringParameter("StopVar");
            if (stopVar.find("Elapsed") != stopVar.npos)
            {
               stopWhen[i]->GetStopParameter()->
                  SetRealParameter("InitialEpoch", stopEpochBase);
            }
         }

         // Publish the initial data
         pubdata[0] = currEpoch[0];
         memcpy(&pubdata[1], state, dim*sizeof(Real));
         publisher->Publish(streamID, pubdata, dim+1);
         inProgress = true;
      }

      /// @todo Find a more elegant way to perform epoch refresh during propagation
      for (unsigned int i = 0; i < fm.size(); ++i)
         fm[i]->UpdateInitialData();

      Integer epochID = sats[0]->GetParameterID("Epoch");;
         
      while (!stopCondMet)
      {
         if (!TakeAStep())
            throw CommandException("Propagate::Execute() Propagator Failed to Step\n");
         for (unsigned int i = 0; i < fm.size(); ++i) {
            // orbit related parameters use spacecraft for data
            elapsedTime[i] = fm[i]->GetTime();
            currEpoch[i] = baseEpoch[i] + elapsedTime[i] / 86400.0;
            
            // Update spacecraft epoch, without argument the spacecraft epoch
            // won't get updated for consecutive Propagate command
            fm[i]->UpdateSpaceObject(currEpoch[i]);
         }
   
         if (singleStepMode)
            break;

         //------------------------------------------
         // loop through StopCondition list
         //------------------------------------------
         for (unsigned int i=0; i<stopWhen.size(); i++)
         {
            // StopCondition need epoch for the Interpolator
            stopWhen[i]->SetRealParameter("Epoch", 
                                        stopSats[i]->GetRealParameter(epochID));
            
            if (stopWhen[i]->Evaluate())
            {
               stopCondMet = true;
               stopEpoch = (stopWhen[i]->GetStopEpoch());
               #if DEBUG_PROPAGATE_EXE
                  MessageInterface::ShowMessage
                     ("Propagate::Execute() %s met\n", 
                     stopWhen[i]->GetName().c_str());
               #endif
               break; // exit if any stop condition met
            }
         }
         
         if (!stopCondMet)
         {
            // Publish the data here
            pubdata[0] = currEpoch[0];
            memcpy(&pubdata[1], state, dim*sizeof(Real));
            publisher->Publish(streamID, pubdata, dim+1);
         }
         else
         {  
            for (unsigned int i = 0; i < fm.size(); ++i) {
               fm[i]->RevertSpaceObject();
               elapsedTime[i] = fm[i]->GetTime();
               currEpoch[i] = baseEpoch[i] + elapsedTime[i] / 86400.0;
            }
         }
         
         #if DEBUG_PROPAGATE_EXE
            MessageInterface::ShowMessage
               ("Propagate::Execute() intermediate; epoch = %f\n", currEpoch[0]);
         #endif
   
         // Periodically see if the user has stopped the run 
         ++checkCount;
         if ((checkCount == interruptCheckFrequency) && !stopCondMet){
            inProgress = true;
            return true;
         }
      }
   }
   catch (BaseException &ex) {
      MessageInterface::ShowMessage
         ("Propagate::Execute() setting inProgress to false");
      inProgress = false;
      throw;
   }

   inProgress = false;
   
   #if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage
         ("Propagate::Execute() currEpoch = %f, stopEpoch = %f, elapsedTime = %f\n",
          currEpoch[0], stopEpoch, elapsedTime[0]);
   #endif
   
   
   Real secsToStep = (stopEpoch - currEpoch[trigger]) * 86400.0;
   
   if (secsToStep > 0.0)
   {
#if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage
         ("Propagate::Execute() before Step(%f) epoch = %f\n",
          secsToStep, (baseEpoch[0] + fm[0]->GetTime() / 86400.0));
#endif
//      if (!p->Step(secsToStep))
      if (!TakeAStep(secsToStep))
         throw CommandException("Propagator Failed to Step fixed interval\n");

      for (unsigned int i = 0; i < fm.size(); ++i) {
         fm[i]->UpdateSpaceObject(baseEpoch[i] + fm[i]->GetTime() / 86400.0);
      }
      // Publish the final data point here
      pubdata[0] = baseEpoch[0] + fm[0]->GetTime() / 86400.0;
      memcpy(&pubdata[1], state, dim*sizeof(Real));
      publisher->Publish(streamID, pubdata, dim+1);
         
#if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage
         ("Propagate::Execute() after Step(%f) epoch = %f\n",
          secsToStep, (baseEpoch[0] + fm[0]->GetTime() / 86400.0));
#endif
   
      publisher->FlushBuffers(); //loj: 6/22/04 added
      delete [] pubdata;
    
      for (unsigned int i=0; i<stopWhen.size(); i++)
      {
         if (stopWhen[i]->GetName() == "")
            delete stopWhen[i];
      }
      //---------------------------------------
   
#if DEBUG_PROPAGATE_EXE
      MessageInterface::ShowMessage
         ("Propagate::Execute() complete; epoch = %f\n",
          (baseEpoch[0] + fm[0]->GetTime() / 86400.0));
#endif
   }
   
   return true;
}


void Propagate::SetNames(const std::string& name, StringArray& owners, 
                         StringArray& elements)
{
   // Add satellite labels
   for (Integer i = 0; i < 6; ++i)
      owners.push_back(name);       // X, Y, Z, Vx, Vy, Vz
      
   elements.push_back(name+".X");
   elements.push_back(name+".Y");
   elements.push_back(name+".Z");
   elements.push_back(name+".Vx");
   elements.push_back(name+".Vy");
   elements.push_back(name+".Vz");
}

