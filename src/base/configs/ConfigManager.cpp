//$Id$
//------------------------------------------------------------------------------
//                                ConfigManager
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool.
//
// Author: Darrel J. Conway
// Created: 2003/10/27
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Configuration manager used to manage configured (i.e. named) GMAT objects.
 */
//------------------------------------------------------------------------------


#include "ConfigManager.hpp"
#include "ConfigManagerException.hpp"

//#define DEBUG_RENAME 1
//#define DEBUG_CONFIG 1
//#define DEBUG_CONFIG_SS 1
//#define DEBUG_CONFIG_ADD_CLONE 1
//#define DEBUG_CONFIG_MEMORY

//---------------------------------
// static members
//---------------------------------


ConfigManager* ConfigManager::theConfigManager = NULL;


//---------------------------------
// public methods
//---------------------------------

//------------------------------------------------------------------------------
// ConfigManager* Instance()
//------------------------------------------------------------------------------
/**
 * Accessor method used to obtain the singleton.
 *
 * @return the singleton instance of the configuration manager.
 */
//------------------------------------------------------------------------------
ConfigManager* ConfigManager::Instance()
{
   if (!theConfigManager)
      theConfigManager = new ConfigManager;
        
   return theConfigManager;
}


//------------------------------------------------------------------------------
// ConfigManager()
//------------------------------------------------------------------------------
/**
 * Constructor.
 */
//------------------------------------------------------------------------------
ConfigManager::ConfigManager() :
   objectChanged     (false)
{
}

// class destructor
//------------------------------------------------------------------------------
// ~ConfigManager()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
ConfigManager::~ConfigManager()
{
   RemoveAllItems();
}


//------------------------------------------------------------------------------
// std::string GetNewName(const std::string &name, Integer startCount)
//------------------------------------------------------------------------------
/*
 * It gives new name by adding counter to the input name.
 *
 * @param <name> Base name to used to generate new name
 * @param <startCount> Starting counter
 * @return new name
 */
//------------------------------------------------------------------------------
std::string ConfigManager::GetNewName(const std::string &name, Integer startCount)
{
   #if DEBUG_CONFIG_NEW_NAME
   MessageInterface::ShowMessage
      ("ConfigManager::GetNewName() name = %s\n", name.c_str());
   #endif
   
   if (name == "")
      return "";
   
   // get initial new name
   Integer counter = 0;
   std::string baseName = GmatStringUtil::RemoveLastNumber(name, counter);
   if (counter == 0)
      counter = startCount;
   
   std::string newName = baseName + GmatStringUtil::ToString(counter, 1);
   
   // construct new name while it exists
   while (GetItem(newName))
   {
      ++counter;
      newName = baseName + GmatStringUtil::ToString(counter,1);
   }
   
   #if DEBUG_CONFIG_NEW_NAME
   MessageInterface::ShowMessage
      ("ConfigManager::GetNewName() newName = %s\n", newName.c_str());
   #endif
   
   return newName;
}


//------------------------------------------------------------------------------
// std::string AddClone(const std::string &name)
//------------------------------------------------------------------------------
/*
 * Adds the clone of the named object to configuration.
 * It gives new name by adding counter to the name to be cloned.
 *
 * return new name if object was cloned and added to configuration,
 *        blank otherwise
 */
//------------------------------------------------------------------------------
std::string ConfigManager::AddClone(const std::string &name)
{
   if (name == "")
      return "";
   
   GmatBase *obj1 = GetItem(name);
   std::string newName = GetNewName(name, 2);
   
   GmatBase* obj2 = obj1->Clone();
   obj2->SetName(newName, newName);
   AddObject(obj2);
   
   #if DEBUG_CONFIG_ADD_CLONE
   MessageInterface::ShowMessage
      ("ConfigManager::AddClone() newName = %s, type = %s\n", obj2->GetName().c_str(),
       obj2->GetTypeName().c_str());
   #endif
   
   return newName;
}


//------------------------------------------------------------------------------
// void AddPhysicalModel(PhysicalModel *pm)
//------------------------------------------------------------------------------
/**
 * Adds a PhysicalModel to the configuration.
 *
 * @param pm Pointer to the PhysicalModel instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddPhysicalModel(PhysicalModel *pm)
{
   std::string name = pm->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!pm->IsOfType(Gmat::PHYSICAL_MODEL))
      throw ConfigManagerException(name + " is not a PhysicalModel");

   AddObject(pm);
}

//------------------------------------------------------------------------------
// void ConfigManager::AddPropagator(Propagator *prop)
//------------------------------------------------------------------------------
/**
 * Adds a Propagator to the configuration.
 *
 * @param prop Pointer to the Propagator instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddPropagator(Propagator *prop)
{
   std::string name = prop->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!prop->IsOfType(Gmat::PROPAGATOR))
      throw ConfigManagerException(name + " is not a Propagator");

   AddObject(prop);
}


//------------------------------------------------------------------------------
// void AddForceModel(ForceModel *fm)
//------------------------------------------------------------------------------
/**
 * Adds a ForceModel to the configuration.
 *
 * @param fm Pointer to the ForceModel instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddForceModel(ForceModel *fm)
{
   std::string name = fm->GetName();

   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!fm->IsOfType(Gmat::FORCE_MODEL))
      throw ConfigManagerException(name + " is not a ForceModel");

   AddObject(fm);
}


//------------------------------------------------------------------------------
// void AddSubscriber(Subscriber *subs)
//------------------------------------------------------------------------------
/**
 * Adds a Subscriber to the configuration.
 *
 * @param subs Pointer to the Subscriber.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddSubscriber(Subscriber *subs)
{
   std::string name = subs->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!subs->IsOfType(Gmat::SUBSCRIBER))
      throw ConfigManagerException(name + " is not a Subscriber");

   AddObject(subs);
}


//------------------------------------------------------------------------------
// void AddSolarSystem(SolarSystem *solarSys)
//------------------------------------------------------------------------------
/**
 * Adds a SolarSystem to the configuration.
 *
 * @param solarSys Pointer to the SolarSystem instance.
 *
 * @todo Add solar systems to the ConfigManager
 */
//------------------------------------------------------------------------------
void ConfigManager::AddSolarSystem(SolarSystem *solarSys)
{
   throw ConfigManagerException("SolarSystem objects are not yet managed");
}


//------------------------------------------------------------------------------
// void AddPropSetup(PropSetup* propSetup)
//------------------------------------------------------------------------------
/**
 * Adds a PropSetup to the configuration.
 *
 * @param propSetup Pointer to the PropSetup instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddPropSetup(PropSetup* propSetup)
{
   std::string name = propSetup->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!propSetup->IsOfType(Gmat::PROP_SETUP))
      throw ConfigManagerException(name + " is not a PropSetup");

   AddObject(propSetup);
}


//------------------------------------------------------------------------------
// void AddSpacecraft(SpaceObject *sc)
//------------------------------------------------------------------------------
/**
 * Adds a Spacecraft and formations to the configuration.
 *
 * @param sc Pointer to the Spacecraft/Formation instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddSpacecraft(SpaceObject *sc)
{
   std::string name = sc->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!sc->IsOfType(Gmat::SPACEOBJECT))
      throw ConfigManagerException(name + " is not a SpaceObject");

   AddObject(sc);
}


//------------------------------------------------------------------------------
// void AddHardware(Hardware *hw)
//------------------------------------------------------------------------------
/**
 * Adds a Hardware object to the configuration.
 *
 * @param hw Pointer to the Hardware object.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddHardware(Hardware *hw)
{
   std::string name = hw->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!hw->IsOfType(Gmat::HARDWARE))
      throw ConfigManagerException(name + " is not Hardware");

   AddObject(hw);
}


//------------------------------------------------------------------------------
// void AddStopCondition(StopCondition* stopCond)
//------------------------------------------------------------------------------
/**
 * Adds a StopCondition to the configuration.
 *
 * @param stopCond Pointer to the StopCondition instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddStopCondition(StopCondition* stopCond)
{
   std::string name = stopCond->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!stopCond->IsOfType(Gmat::STOP_CONDITION))
      throw ConfigManagerException(name + " is not a StopCondition");

   AddObject(stopCond);
}


//------------------------------------------------------------------------------
// void AddParameter(Parameter* parameter)
//------------------------------------------------------------------------------
/**
 * Adds a Parameter to the configuration.
 *
 * @param parameter Pointer to the Parameter instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddParameter(Parameter* parameter)
{
   std::string name = parameter->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!parameter->IsOfType(Gmat::PARAMETER))
      throw ConfigManagerException(name + " is not a Parameter");

   AddObject(parameter);
}


//------------------------------------------------------------------------------
// void AddBurn(Burn* burn)
//------------------------------------------------------------------------------
/**
 * Adds a Burn to the configuration.
 *
 * @param burn Pointer to the Burn instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddBurn(Burn* burn)
{
   std::string name = burn->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!burn->IsOfType(Gmat::BURN))
      throw ConfigManagerException(name + " is not a Burn");

   AddObject(burn);
}

//------------------------------------------------------------------------------
// void AddSolver(Solver* solver)
//------------------------------------------------------------------------------
/**
 * Adds a Solver to the configuration.
 *
 * @param solver Pointer to the Solver instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddSolver(Solver* solver)
{
   std::string name = solver->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!solver->IsOfType(Gmat::SOLVER))
      throw ConfigManagerException(name + " is not a Solver");

   AddObject(solver);
}

//------------------------------------------------------------------------------
// void AddAtmosphereModel(AtmosphereModel* atmosModel)
//------------------------------------------------------------------------------
/**
 * Adds an AtmosphereModel to the configuration.
 *
 * @param pm atmosModel to the AtmosphereModel instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddAtmosphereModel(AtmosphereModel* atmosModel)
{
   std::string name = atmosModel->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!atmosModel->IsOfType(Gmat::ATMOSPHERE))
      throw ConfigManagerException(name + " is not an AtmosphereModel");

   AddObject(atmosModel);
}

//------------------------------------------------------------------------------
// void AddFunction(Function* function)
//------------------------------------------------------------------------------
/**
 * Adds a Function to the configuration.
 *
 * @param function Pointer to the Function instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddFunction(Function* function)
{
   std::string name = function->GetName();
   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");
   
   if (!function->IsOfType(Gmat::FUNCTION))
      throw ConfigManagerException(name + " is not a Function");

   AddObject(function);
}

//------------------------------------------------------------------------------
// void AddCoordinateSystem(CoordinateSystem *cs)
//------------------------------------------------------------------------------
/**
 * Adds a CoordinateSystem to the configuration.
 *
 * @param cs Pointer to the CoordinateSystem instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddCoordinateSystem(CoordinateSystem *cs)
{
   std::string name = cs->GetName();

   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!cs->IsOfType(Gmat::COORDINATE_SYSTEM))
      throw ConfigManagerException(name + " is not a CoordinateSystem");

   AddObject(cs);
}

//------------------------------------------------------------------------------
// void AddCalculatedPoint(CalculatedPoint *cs)
//------------------------------------------------------------------------------
/**
 * Adds a CalculatedPoint to the configuration.
 *
 * @param cs Pointer to the CalculatedPoint instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddCalculatedPoint(CalculatedPoint *cs)
{
   std::string name = cs->GetName();

   if (name == "")
      throw ConfigManagerException("Unnamed objects cannot be managed");

   if (!cs->IsOfType(Gmat::CALCULATED_POINT))
      throw ConfigManagerException(name + " is not a CalculatedPoint");

   AddObject(cs);
}

//------------------------------------------------------------------------------
// void AddObject(GmatBase *obj)
//------------------------------------------------------------------------------
/**
 * Adds a CoordinateSystem to the configuration.
 *
 * @param cs Pointer to the CoordinateSystem instance.
 */
//------------------------------------------------------------------------------
void ConfigManager::AddObject(GmatBase *obj)
{
   std::string name = obj->GetName();
   
   #ifdef DEBUG_CONFIG_MEMORY
      MessageInterface::ShowMessage
         ("ConfigManager::AddObject() Adding <%p><%s>'%s'\n",
          obj, obj->GetTypeName().c_str(), name.c_str());
   #endif
      
   if (mapping.find(name) != mapping.end())
   {
      name += " is already in the configuration table";
      throw ConfigManagerException(name);
   }
   else
   {
      objects.push_back(obj);
      mapping[name] = obj;
   }
   
   // Until we can add TextEphemFile to resource tree, we don't want to
   // write to script file on save script. (loj: 2007.04.07)
   
   if (obj->GetTypeName() != "TextEphemFile")
      objectChanged = true;
}


//------------------------------------------------------------------------------
// void SetDefaultSolarSystem(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the default SolarSystem.
 *
 * @param ss The SolarSystem object pointer.
 *
 */
//------------------------------------------------------------------------------
void ConfigManager::SetDefaultSolarSystem(SolarSystem *ss)
{
   defaultSolarSystem = ss;
}


//------------------------------------------------------------------------------
// void SetSolarSystemInUse(SolarSystem *ss)
//------------------------------------------------------------------------------
/**
 * Sets the current SolarSystem.
 *
 * @param ss The SolarSystem object pointer.
 *
 */
//------------------------------------------------------------------------------
void ConfigManager::SetSolarSystemInUse(SolarSystem *ss)
{
   #if DEBUG_CONFIG_SS
   MessageInterface::ShowMessage
      ("ConfigManager::SetSolarSystemInUse() name=%s\n", ss->GetName().c_str());
   #endif
   
   solarSystemInUse = ss;
}


//------------------------------------------------------------------------------
// bool SetSolarSystemInUse(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the name for the current SolarSystem.
 *
 * @param name The SolarSystem name.
 *
 * @note This method is not yet used in GMAT.
 */
//------------------------------------------------------------------------------
bool ConfigManager::SetSolarSystemInUse(const std::string &name)
{
   throw ConfigManagerException
      ("ConfigManager::SetSolarSystemInUse(name) has not been implemented.\n");
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfAllItems()
//------------------------------------------------------------------------------
/**
 * Retrieves a list of all configured objects.
 *
 * @return The list of objects.
 */
//------------------------------------------------------------------------------
const StringArray& ConfigManager::GetListOfAllItems()
{
   listOfItems.erase(listOfItems.begin(), listOfItems.end());
    
   std::vector<GmatBase*>::iterator current =
      (std::vector<GmatBase*>::iterator)(objects.begin());
   
   while (current != (std::vector<GmatBase*>::iterator)(objects.end()))
   {
      listOfItems.push_back((*current)->GetName());
      ++current;
   }
   return listOfItems;
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfItemsHas(Gmat::ObjectType type, const std::string &name,
//                                      bool includeSysParam)
//------------------------------------------------------------------------------
/**
 * Checks a specific item used in anywhere.
 *
 * @param type The type of the object that is being checked.
 * @param name The name of the object.
 * @param includeSysParam True if system parameter to be included
 *
 * @return array of item names where the name is used.
 */
//------------------------------------------------------------------------------
const StringArray& ConfigManager::GetListOfItemsHas(Gmat::ObjectType type,
                                                    const std::string &name,
                                                    bool includeSysParam)
{
   StringArray items = GetListOfAllItems();
   std::string::size_type pos;
   GmatBase *obj;
   std::string objName;
   std::string objString;
   static StringArray itemList;
   itemList.clear();

   #if DEBUG_RENAME
   MessageInterface::ShowMessage
      ("ConfigManager::GetListOfItemsHas() name=%s, includeSysParam=%d\n",
       name.c_str(), includeSysParam);
   #endif
   
   try
   {
      for (UnsignedInt i=0; i<items.size(); i++)
      {
         obj = GetItem(items[i]);
         
         #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("===> obj[%d]=%s, %s\n", i, obj->GetTypeName().c_str(),
             obj->GetName().c_str());
         #endif
         
         // if same type, skip
         if (obj->IsOfType(type))
            continue;
         
         // if system parameter not to be included, skip
         if (!includeSysParam)
         {
            if (obj->IsOfType(Gmat::PARAMETER))
               if (((Parameter*)obj)->GetKey() == GmatParam::SYSTEM_PARAM)
                  continue;
         }
         
         objName = obj->GetName();
         objString = obj->GetGeneratingString();
         pos = objString.find(name);
         
         #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("===> objString=\n%s\n", objString.c_str());
         #endif
         
         if (pos != objString.npos)
         {
            itemList.push_back(objName);
         }
      }
   }
   catch (BaseException &e)
   {
      MessageInterface::ShowMessage("*** Error: %s\n", e.GetFullMessage().c_str());
   }
   
   return itemList;
}


//------------------------------------------------------------------------------
// const StringArray& GetListOfItems(Gmat::ObjectType itemType)
//------------------------------------------------------------------------------
/**
 * Retrieves a list of all configured objects of a given type.
 *
 * @param itemType The type of object requested.
 *
 * @return The list of objects.
 */
//------------------------------------------------------------------------------
const StringArray& ConfigManager::GetListOfItems(Gmat::ObjectType itemType)
{
   listOfItems.erase(listOfItems.begin(), listOfItems.end());
    
   std::vector<GmatBase*>::iterator current =
      (std::vector<GmatBase*>::iterator)(objects.begin());
   while (current != (std::vector<GmatBase*>::iterator)(objects.end()))
   {
      if ((*current)->IsOfType(itemType))
         listOfItems.push_back((*current)->GetName());
      ++current;
   }
   return listOfItems;
}


//------------------------------------------------------------------------------
// GmatBase* GetItem(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves an object by name.
 *
 * @param name The name of the object requested.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
GmatBase* ConfigManager::GetItem(const std::string &name)
{
   //MessageInterface::ShowMessage("===> ConfigManager::GetItem() name='%s'\n", name.c_str());
   
   GmatBase *obj = NULL;
   
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->GetName() == name)
      {
         obj = mapping[name];
      }
   }
   
   //MessageInterface::ShowMessage("===> ConfigManager::GetItem() returning <%p>\n", obj);
   return obj;
}


//------------------------------------------------------------------------------
// bool RenameItem(Gmat::ObjectType type, const std::string &oldName,
//                 const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Changes the name for a configured object.
 *
 * @param type The type of object that is renamed.
 * @param oldName The current name for the object.
 * @param newName The new name for the object.
 *
 * @return true if the object was renamed, false if not.
 */
//------------------------------------------------------------------------------
bool ConfigManager::RenameItem(Gmat::ObjectType type,
                               const std::string &oldName,
                               const std::string &newName)
{
   #if DEBUG_RENAME
      MessageInterface::ShowMessage
         ("ConfigManager::RenameItem() type=%d, oldName=%s, newName=%s\n",
          type, oldName.c_str(), newName.c_str());
   #endif
   
   bool renamed = false;
   
   //--------------------------------------------------
   // change mapping name
   //--------------------------------------------------
   
   if (mapping.find(oldName) != mapping.end())
   {
      GmatBase *obj = mapping[oldName];
      if (obj->GetType() == type)
      {
         // if newName does not exist, change name
         if (mapping.find(newName) == mapping.end())
         {
            mapping.erase(oldName);
            mapping[newName] = obj;
            obj->SetName(newName);
            renamed = true;
            #if DEBUG_RENAME
            MessageInterface::ShowMessage("===> Rename mapping obj=%s\n", obj->GetName().c_str());
            #endif
         }
         else
         {
            MessageInterface::PopupMessage
               (Gmat::WARNING_, "%s already exist, Please enter different name.\n",
                newName.c_str());
         }
      }
      else
      {
         MessageInterface::ShowMessage
            ("ConfigManager::RenameItem() oldName has different type:%d\n",
             obj->GetType());
      }
   }
   
   if (!renamed)
   {
      #if DEBUG_RENAME
         MessageInterface::ShowMessage
            ("ConfigManager::RenameItem() Unable to rename: oldName not found.\n");
      #endif
      return false;
   }
   
   //----------------------------------------------------
   // Rename ref. objects
   //----------------------------------------------------
   GmatBase *obj;
   StringArray itemList = GetListOfItemsHas(type, oldName);
   for (UnsignedInt i=0; i<itemList.size(); i++)
   {
      obj = GetItem(itemList[i]);
      if (obj)
      {
         #if DEBUG_RENAME
         MessageInterface::ShowMessage("===> Rename obj=%s\n", obj->GetName().c_str());
         #endif
         
         renamed = obj->RenameRefObject(type, oldName, newName);
      }
   }
   
   //----------------------------------------------------
   // Rename tanks in the thrusters.
   // Tank is ReadOnly parameter so it does show in
   // GeneratingString()
   //----------------------------------------------------
   
   if (type == Gmat::HARDWARE)
   {
      itemList = GetListOfItems(Gmat::HARDWARE);
      
      for (unsigned int i=0; i<itemList.size(); i++)
      {
         obj = GetItem(itemList[i]);
         if (obj->IsOfType("Thruster"))
             obj->RenameRefObject(type, oldName, newName);
      }
   }
   
   //----------------------------------------------------
   // Rename system parameters and expression of variables
   //----------------------------------------------------
   
   else if (type == Gmat::SPACECRAFT || type == Gmat::COORDINATE_SYSTEM ||
            type == Gmat::CALCULATED_POINT || type == Gmat::IMPULSIVE_BURN)
   {
      StringArray params = GetListOfItems(Gmat::PARAMETER);
      std::string oldParamName, newParamName;
      Parameter *param;
      std::string::size_type pos;
      
      for (unsigned int i=0; i<params.size(); i++)
      {
         #if DEBUG_RENAME
         MessageInterface::ShowMessage("params[%d]=%s\n", i, params[i].c_str());
         #endif
         
         param = GetParameter(params[i]);
         
         // if system parameter, change it's own name
         if (param->GetKey() == GmatParam::SYSTEM_PARAM)
         {
            oldParamName = param->GetName();
            pos = oldParamName.find(oldName);
            
            // rename actual parameter name
            if (pos != oldParamName.npos)
            {
               newParamName = oldParamName;
               newParamName.replace(pos, oldName.size(), newName);
               
               #if DEBUG_RENAME
               MessageInterface::ShowMessage
                  ("===> oldParamName=%s, newParamName=%s\n",
                   oldParamName.c_str(), newParamName.c_str());
               #endif
               
               // change parameter mapping name
               if (mapping.find(oldParamName) != mapping.end())
               {
                  mapping.erase(oldParamName);
                  mapping[newParamName] = (GmatBase*)param;
                  param->SetName(newParamName);
                  renamed = true;
               }
            }
         }
         // if variable, need to change expression
         else if (param->GetTypeName() == "Variable")
         {
            param->RenameRefObject(Gmat::PARAMETER, oldName, newName);
         }
      }
   }
   
   #if DEBUG_RENAME
   StringArray& allItems = GetListOfAllItems();
   for (UnsignedInt i=0; i<allItems.size(); i++)
      MessageInterface::ShowMessage("===> item[%d] = %s\n", i, allItems[i].c_str());
   #endif
   
   objectChanged = true;
   
   return renamed;
} // RenameItem()


//------------------------------------------------------------------------------
// bool RemoveAllItems()
//------------------------------------------------------------------------------
/**
 * Removes all configured objects from memory
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool ConfigManager::RemoveAllItems()
{
   // delete objects
   #ifdef DEBUG_CONFIG_MEMORY
   MessageInterface::ShowMessage
      ("ConfigManager::RemoveAllItems() Deleting %d objects\n", objects.size());
   #endif
   
   for (unsigned int i=0; i<objects.size(); i++)
   {
      std::string objName = objects[i]->GetName();
      
      #ifdef DEBUG_CONFIG_MEMORY
         MessageInterface::ShowMessage("  Deleting %s\n", objects[i]->GetName().c_str());
      #endif

      delete objects[i];
      objects[i] = NULL;
   }
   
   objects.clear();
   mapping.clear();
   
   return true;
}


//------------------------------------------------------------------------------
// bool RemoveItem(Gmat::ObjectType type, const std::string &name)
//------------------------------------------------------------------------------
/**
 * Removes a specific item from memory.
 *
 * @param type The type of the object that is being removed.
 * @param name The name of the object.
 *
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool ConfigManager::RemoveItem(Gmat::ObjectType type, const std::string &name)
{
   #ifdef DEBUG_CONFIG_MEMORY
   MessageInterface::ShowMessage
      ("ConfigManager::RemoveItem() entered, type=%d, name='%s'\n", type,
       name.c_str());
   #endif
   
   bool status = false;
   
   // remove from objects
   std::vector<GmatBase*>::iterator currentIter =
      (std::vector<GmatBase*>::iterator)(objects.begin());
   
   while (currentIter != (std::vector<GmatBase*>::iterator)(objects.end()))
   {
      if ((*currentIter)->GetType() == type)
      {
         if ((*currentIter)->GetName() == name)
         {
            objects.erase(currentIter);
            break;
         }
      }
      ++currentIter;
   }
   
   // remove from mapping
   if (mapping.find(name) != mapping.end())
   {
      GmatBase *obj = mapping[name];
      if (obj->GetType() == type)
      {
         mapping.erase(name);
         #ifdef DEBUG_CONFIG_MEMORY
         MessageInterface::ShowMessage
            ("  Deleting %s\n", obj->GetName().c_str());
         #endif
         delete obj;
         status = true;
      }
   }
    
   objectChanged = true;
   return status;
}


//------------------------------------------------------------------------------
// bool ReconfigureItem(GmatBase *newobj, const std::string &name)
//------------------------------------------------------------------------------
/*
 * Sets configured object pointer with new pointer.
 *
 * @param  newobj  New object pointer
 * @param  name  Name of the configured object to be reset
 *
 * @return  true if pointer was reset, false otherwise
 */
//------------------------------------------------------------------------------
bool ConfigManager::ReconfigureItem(GmatBase *newobj, const std::string &name)
{
   GmatBase *obj = NULL;
   
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->GetName() == name)
      {
         obj = mapping[name];
         
         #if DEBUG_CONFIG_RECONFIGURE
         MessageInterface::ShowMessage
            ("ConfigManager::ReconfigureItem() obj=%p newobj=%p\n", obj,
             newobj);
         #endif
         
         if (obj->GetTypeName() == newobj->GetTypeName())
         {
            mapping[name] = newobj;
            return true;
         }
      }
   }
   
   return false;
}


//------------------------------------------------------------------------------
// PhysicalModel* GetPhysicalModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a PhysicalModel object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
PhysicalModel* ConfigManager::GetPhysicalModel(const std::string &name)
{
   PhysicalModel *physicalModel = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::PHYSICAL_MODEL))
      {
         physicalModel = (PhysicalModel *)mapping[name];
      }
   }
   return physicalModel;
}

//------------------------------------------------------------------------------
// Propagator* GetPropagator(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Propagator object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Propagator* ConfigManager::GetPropagator(const std::string &name)
{
   Propagator *prop = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::PROPAGATOR))
      {
         prop = (Propagator *)mapping[name];
      }
   }
   return prop;
}


//------------------------------------------------------------------------------
// ForceModel* GetForceModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a ForceModel object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
ForceModel* ConfigManager::GetForceModel(const std::string &name)
{
   ForceModel *fm = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name] == NULL)
         throw ConfigManagerException
            ("ConfigManager::GetForceModel(name) is finding a NULL object in the mapping.\n");
     
      if (mapping[name]->IsOfType(Gmat::FORCE_MODEL))
      {
         fm = (ForceModel *)mapping[name];
      }
   }
   return fm;
}


//------------------------------------------------------------------------------
// SpaceObject* GetSpacecraft(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Spacecraft or Formation object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
SpaceObject* ConfigManager::GetSpacecraft(const std::string &name)
{
   SpaceObject *sc = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if ((mapping[name]->IsOfType(Gmat::SPACECRAFT)) ||
          (mapping[name]->IsOfType(Gmat::FORMATION)))
      {
         sc = (SpaceObject *)mapping[name];
      }
   }
   return sc;
}


//------------------------------------------------------------------------------
// Hardware* GetHardware(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Hardware object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Hardware* ConfigManager::GetHardware(const std::string &name)
{
   Hardware *hw = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::HARDWARE))
      {
         hw = (Hardware *)mapping[name];
      }
   }
   return hw;
}


//------------------------------------------------------------------------------
// PropSetup* GetPropSetup(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a PropSetup object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
PropSetup* ConfigManager::GetPropSetup(const std::string &name)
{
   PropSetup *ps = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::PROP_SETUP))
      {
         ps = (PropSetup *)mapping[name];
      }
   }
   return ps;
}


//------------------------------------------------------------------------------
// Subscriber* GetSubscriber(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Subscriber object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Subscriber* ConfigManager::GetSubscriber(const std::string &name)
{
   Subscriber *sub = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::SUBSCRIBER))
      {
         sub = (Subscriber *)mapping[name];
      }
   }
   return sub;
}


//------------------------------------------------------------------------------
// SolarSystem* GetDefaultSolarSystem()
//------------------------------------------------------------------------------
/**
 * Retrieves the default SolarSystem object.
 *
 * @return A pointer to the object.
 *
 * @note This method is not yet used in GMAT.
 */
//------------------------------------------------------------------------------
SolarSystem* ConfigManager::GetDefaultSolarSystem()
{
   return defaultSolarSystem;
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse()
//------------------------------------------------------------------------------
/**
 * Retrieves the current SolarSystem object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
SolarSystem* ConfigManager::GetSolarSystemInUse()
{
   return solarSystemInUse;
}


//------------------------------------------------------------------------------
// SolarSystem* GetSolarSystemInUse(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves the current SolarSystem object by name
 *
 * @return A pointer to the object.
 *
 * @note This method is not yet used in GMAT.
 */
//------------------------------------------------------------------------------
SolarSystem* ConfigManager::GetSolarSystemInUse(const std::string &name)
{
   #if DEBUG_CONFIG_SS
   MessageInterface::ShowMessage
      ("ConfigManager::GetSolarSystemInUse() name=%s\n", name.c_str());
   #endif
   
   throw ConfigManagerException
      ("ConfigManager::GetSolarSystemInUse(name) has not been implemented.\n");
}


//------------------------------------------------------------------------------
// StopCondition* GetStopCondition(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a StopCondition object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
StopCondition* ConfigManager::GetStopCondition(const std::string &name)
{
   StopCondition *sc = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::STOP_CONDITION))
      {
         sc = (StopCondition *)mapping[name];
      }
   }
   return sc;
}


//------------------------------------------------------------------------------
// Parameter* GetParameter(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Parameter object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Parameter* ConfigManager::GetParameter(const std::string &name)
{
   Parameter *param = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::PARAMETER))
      {
         param = (Parameter *)mapping[name];
      }
   }
   return param;
}

//------------------------------------------------------------------------------
// Burn* GetBurn(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Burn object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Burn* ConfigManager::GetBurn(const std::string &name)
{
   Burn *burn = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::BURN))
      {
         burn = (Burn *)mapping[name];
      }
   }
   return burn;
}

//------------------------------------------------------------------------------
// Solver* GetSolver(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Solver object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Solver* ConfigManager::GetSolver(const std::string &name)
{
   Solver *solver = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::SOLVER))
      {
         solver = (Solver *)mapping[name];
      }
   }
   return solver;
}

//------------------------------------------------------------------------------
// AtmosphereModel* GetAtmosphereModel(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves an Atmosphere object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
AtmosphereModel* ConfigManager::GetAtmosphereModel(const std::string &name)
{
   AtmosphereModel *atmosModel = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::ATMOSPHERE))
      {
         atmosModel = (AtmosphereModel *)mapping[name];
      }
   }
   return atmosModel;
}

//------------------------------------------------------------------------------
// Function* GetFunction(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a Function object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
Function* ConfigManager::GetFunction(const std::string &name)
{
   Function *function = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::FUNCTION))
      {
         function = (Function *)mapping[name];
      }
   }
   return function;
}

//------------------------------------------------------------------------------
// CoordinateSystem* GetCoordinateSystem(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a CoordinateSystem object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
CoordinateSystem* ConfigManager::GetCoordinateSystem(const std::string &name)
{
   CoordinateSystem *cs = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::COORDINATE_SYSTEM))
      {
         cs = (CoordinateSystem *)mapping[name];
      }
   }
   return cs;
}


//------------------------------------------------------------------------------
// CalculatedPoint* GetCalculatedPoint(const std::string &name)
//------------------------------------------------------------------------------
/**
 * Retrieves a CalculatedPoint object.
 *
 * @param name The name of the object.
 *
 * @return A pointer to the object.
 */
//------------------------------------------------------------------------------
CalculatedPoint* ConfigManager::GetCalculatedPoint(const std::string &name)
{
   CalculatedPoint *cs = NULL;
   if (mapping.find(name) != mapping.end())
   {
      if (mapping[name]->IsOfType(Gmat::CALCULATED_POINT))
      {
         cs = (CalculatedPoint *)mapping[name];
      }
   }
   return cs;
}


//------------------------------------------------------------------------------
// bool HasConfigurationChanged()
//------------------------------------------------------------------------------
bool ConfigManager::HasConfigurationChanged()
{
   return objectChanged;
}


//------------------------------------------------------------------------------
// void ConfigurationChanged(bool tf)
//------------------------------------------------------------------------------
void ConfigManager::ConfigurationChanged(bool tf)
{
   objectChanged = tf;
}


//------------------------------------------------------------------------------
// ObjectMap* GetObjectMap()
//------------------------------------------------------------------------------
ObjectMap* ConfigManager::GetObjectMap()
{
   return &mapping;
}

